
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4GDMLParser.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <tuning.h>

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovPhotonDetector.h>

#include <DetectorConstruction.h>

// -------------------------------------------------------------------------------------

void DetectorConstruction::DefinePhotonDetectors(CherenkovDetector *cdet)
{
  double azOffset = _FIDUCIAL_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_;
  double xysize = _HRPPD_TILE_SIZE_, wndthick = _HRPPD_WINDOW_THICKNESS_, zwnd = azOffset + wndthick/2;
  
  // HRPPD assembly container volume; 
  double hrppd_container_volume_thickness = _HRPPD_CONTAINER_VOLUME_HEIGHT_;
  double zcont = azOffset + hrppd_container_volume_thickness/2;
  double zcables = azOffset + hrppd_container_volume_thickness + 1.0;///2;
  G4Box *hrppd_box  = new G4Box("HRPPD", xysize/2, xysize/2, hrppd_container_volume_thickness/2);
  // Well, the inside volume will then be air as well; fine;
  G4LogicalVolume* hrppd_log = new G4LogicalVolume(hrppd_box, m_Air,  "HRPPD", 0, 0, 0);
  
  //+G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(90*degree));

  // Full size quartz window;
  G4Box *wnd_box  = new G4Box("QuartzWindow", xysize/2, xysize/2, wndthick/2);
  G4LogicalVolume* wnd_log = new G4LogicalVolume(wnd_box, m_FusedSilica,  "QuartzWindow", 0, 0, 0);
  {	
    TVector3 nx(1*sign,0,0), ny(0,-1,0);
    
    // A single entry; this assumes of course that all the windows are at the same Z, and parallel to each other;
    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0,0,_FIDUCIAL_VOLUME_OFFSET_ + zwnd), nx, ny);
#ifdef _DISABLE_HRPPD_WINDOW_PHOTONS_
    auto radiator = m_Geometry->AddFlatRadiator(cdet, "QuartzWindow", CherenkovDetector::Downstream, 
						0, wnd_log, m_FusedSilica, surface, wndthick/mm);
    radiator->DisableOpticalPhotonGeneration();
#else
    m_Geometry->AddFlatRadiator(cdet, "QuartzWindow", CherenkovDetector::Downstream, 
				0, wnd_log, m_FusedSilica, surface, wndthick/mm);
#endif
  }	
  
  double pitch = xysize + _HRPPD_INSTALLATION_GAP_, xyactive = _HRPPD_ACTIVE_AREA_SIZE_;
  double xyopen = _HRPPD_OPEN_AREA_SIZE_;
  double certhick = _HRPPD_CERAMIC_BODY_THICKNESS_;//, zcer = azOffset + wndthick + certhick/2;
  
  // HRPPD body imitation; ignore MCPs (small fraction compared to the ceramic body);
  G4Box *cer_box  = new G4Box("CeramicBox", xysize/2, xysize/2, certhick/2);
  G4Box *cut_box  = new G4Box("CeramicCut", xyopen/2, xyopen/2, certhick/2);
  auto ceramic = new G4SubtractionSolid("CeramicBody", cer_box, cut_box, 0, 
					G4ThreeVector(0,0, -_HRPPD_BASEPLATE_THICKNESS_));
  G4LogicalVolume* cer_log = new G4LogicalVolume(ceramic, m_Ceramic,  "CeramicBody", 0, 0, 0);
  SetColor(cer_log, G4Colour(1, 1, 1, 0.5));
  
  G4Box *plating  = new G4Box("Plating", xyopen/2, xyopen/2, _HRPPD_PLATING_LAYER_THICKNESS_/2);
  G4LogicalVolume* plating_log = new G4LogicalVolume(plating, m_Silver,  "Plating", 0, 0, 0);
  
  G4Box *mcp  = new G4Box("MCP", xyopen/2, xyopen/2, _EFFECTIVE_MCP_THICKNESS_/2);
  G4LogicalVolume* mcp_log = new G4LogicalVolume(mcp, _EFFECTIVE_MCP_MATERIAL_,  "MCP", 0, 0, 0);
  
  double pdthick = 0.01*mm, zpdc = azOffset + wndthick + pdthick/2;
  G4Box *pd_box  = new G4Box("PhotoDetector", xyactive/2, xyactive/2, pdthick/2);
  auto pd = new CherenkovPhotonDetector(pd_box, m_Bialkali);
  pd->SetCopyIdentifierLevel(1);
  pd->DefineLogicalVolume();
  pd->SetColor(G4Colour(1, 0, 0, 1.0));
  // Cannot access GEANT shapes in the reconstruction code -> store this value;
  pd->SetActiveAreaSize(xyactive);
  
  G4Box *qd_box  = new G4Box("FakePhotoDetector", xyactive/2, xyactive/2, pdthick/2);
  G4LogicalVolume* qd_log = new G4LogicalVolume(qd_box, m_Bialkali, "FakePhotoDetector", 0, 0, 0);
  
  {
    double accu = -hrppd_container_volume_thickness/2;
    
    // Window layer;
    //+auto wnd_phys = 
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + wndthick/2), wnd_log, 
		      "QuartzWindow", hrppd_log, false, 0);
    accu += wndthick;
    
    // Ceramic pictureframe body behind it;
    {
      //+auto cer_phys = 
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + certhick/2), cer_log,
			"CeramicBody", hrppd_log, false, 0);
      // Place plating layer in the middle;
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + certhick/2), plating_log, 
			"Plating",     hrppd_log, false, 0);
      
      // Place effective MCP material nearby;
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + certhick/2 + 
					 _HRPPD_PLATING_LAYER_THICKNESS_/2 + _EFFECTIVE_MCP_THICKNESS_/2), 
			mcp_log, "MCP",     hrppd_log, false, 0);
      
      // Rough reflective optical border between them;
      //+G4OpticalSurface* opWindowMetallization = 
      //+CreateLambertianMirrorSurface("WindowMetallization", _HRPPD_METALLIZATION_REFLECTIVITY_, _HRPPD_METALLIZATION_ROUGHNESS_);
      //+new G4LogicalBorderSurface("WindowMetallization", wnd_phys, cer_phys, opWindowMetallization);
      
      // Fake photodector layer (photocathode);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + pdthick/2), qd_log, "FakePhotoDetector", 
			hrppd_log, false, 0);
      //accu += pdthick;
      // Photodector layer (photocathode);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + pdthick + pdthick/2), pd->GetLogicalVolume(), "PhotoDetector", 
			hrppd_log, false, 0);
      
      accu += certhick + 1*mm;
    }
    
#if 1//_MBUDGET_
    // Readout PCB; FIXME: ignore copper to the moment (will be few dozens of microns overall; take 
    // PCB thickness on a high end to kind of compensate for this;
    {
      G4Box *pcb  = new G4Box("PCB", _READOUT_PCB_SIZE_/2, _READOUT_PCB_SIZE_/2, _READOUT_PCB_THICKNESS_/2);
      G4LogicalVolume* pcb_log = new G4LogicalVolume(pcb, m_FR4,  "PCB", 0, 0, 0);
      
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _READOUT_PCB_THICKNESS_/2), pcb_log, "PCB", 
			hrppd_log, false, 0);

      accu += _READOUT_PCB_THICKNESS_ + 0.01*mm;
    }
#endif
    
#if 1//_MBUDGET_
    // ASIC chips;
    {
      double pitch = _READOUT_PCB_SIZE_/2;
        
      G4Box *asic  = new G4Box("ASIC", _ASIC_SIZE_XY_/2, _ASIC_SIZE_XY_/2, _ASIC_THICKNESS_/2);
      G4LogicalVolume* asic_log = new G4LogicalVolume(asic, _ASIC_MATERIAL_,  "ASIC", 0, 0, 0);

      for(unsigned ix=0; ix<2; ix++) {
	double xOffset = pitch*(ix - (2-1)/2.);
	
	for(unsigned iy=0; iy<2; iy++) {
	  double yOffset = pitch*(iy - (2-1)/2.);
	  
	  new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, accu + _ASIC_THICKNESS_/2), asic_log, "ASIC", 
			    hrppd_log, false, ix*2 + iy);
	} //for iy
      } //for ix
      
      accu += _ASIC_THICKNESS_ + 0.01*mm;
    }
#endif

    // Cold plates and pipes;
#if 1//_LATER_
    {
      double pitch = _READOUT_PCB_SIZE_/2;

      G4Box *cplate  = new G4Box("ColdPlate", _ASIC_SIZE_XY_/2, _ASIC_SIZE_XY_/2, _COLD_PLATE_THICKNESS_/2);
      G4LogicalVolume* cplate_log = new G4LogicalVolume(cplate, _COLD_PLATE_MATERIAL_,  "ColdPlate", 0, 0, 0);

      // FIXME: yes, have to make the pipes 1.5mm shorter to fit into the HRPPD container volume;
      // must be a minor simplification I guess;
      double length = _HRPPD_TILE_SIZE_ /*+ _HRPPD_INSTALLATION_GAP_*/, iradius = _COOLING_PIPE_INNER_DIAMETER_/2; 
      double oradius = iradius + _COOLING_PIPE_WALL_THICKNESS_;
      auto pipe = new G4Tubs("CoolingPipe", 0.0, oradius, length/2, 0*degree, 360*degree);
      G4LogicalVolume* pipe_log = new G4LogicalVolume(pipe, _COOLING_PIPE_MATERIAL_, "CoolingPipe", 0, 0, 0);

      auto water = new G4Tubs("Water", 0.0, iradius, length/2, 0*degree, 360*degree);
      G4LogicalVolume* water_log = new G4LogicalVolume(water, m_Water, "Water", 0, 0, 0);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), water_log, "Water", pipe_log, false, 0);

      G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(90*degree));

      for(unsigned bt=0; bt<2; bt++) {
	double yOffset = pitch*(bt - (2-1)/2.);
	
	new G4PVPlacement(rY, G4ThreeVector(0.0, yOffset, accu + _COLD_PLATE_THICKNESS_ + oradius), 
			  pipe_log, "CoolingPipe", hrppd_log, false, 0);
	
	for(unsigned lr=0; lr<2; lr++) {
	  double xOffset = pitch*(lr - (2-1)/2.);
	  
	  new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, accu + _COLD_PLATE_THICKNESS_/2), cplate_log, 
			    "ColdPlate", hrppd_log, false, 0);
	} //for lr
      } //for bt

      accu += _COLD_PLATE_THICKNESS_ + 2*oradius + 0.01*mm;
    }
#endif
  }

  // 'pitch': yes, want them installed without gaps;
#ifdef _USE_PYRAMIDS_
  G4Box *pyra_box = new G4Box("Dummy", pitch/2, pitch/2, _PYRAMID_MIRROR_HEIGHT_/2);
  G4Trd *pyra_cut = new G4Trd("Dummy", pitch/2, xyactive/2, pitch/2, xyactive/2, _PYRAMID_MIRROR_HEIGHT_/2 + 0.01*mm);
  auto *pyra_shape = new G4SubtractionSolid("MirrorPyramid", pyra_box, pyra_cut);
  
  // NB: geometry will be saved in [mm] throughout the code;
  auto pyramid = new CherenkovMirror(pyra_shape, m_Absorber);
  
  pyramid->SetColor(G4Colour(0, 1, 1, 0.5));
  pyramid->SetReflectivity(_MIRROR_REFLECTIVITY_, this);
  
  // Mimic the essential part of the mirror->PlaceWedgeCopies() call;
  pyramid->DefineLogicalVolume();
  m_Geometry->AddMirrorLookupEntry(pyramid->GetLogicalVolume(), pyramid);
#else
  double alu_thickness = _INCH_/2 - _HRPPD_SUPPORT_GRID_BAR_HEIGHT_;
  auto *alu_tube = new G4Tubs("AluFrame", 0.0, _VESSEL_OUTER_RADIUS_, alu_thickness/2, 0*degree, 360*degree);
  auto *alu_shape = new G4SubtractionSolid("AluFrame", alu_tube, 
					   FlangeCut(alu_thickness + 1*mm, _FLANGE_CLEARANCE_), 0, G4ThreeVector(0.0, 0.0, 0.0));
  double alu_cut_size = xysize + 0.5*mm;
  auto *alu_cut = new G4Box("AluWndCut", alu_cut_size/2, alu_cut_size/2, alu_thickness/2 + 1*mm);
  // Cut the central area by hand;
  alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(                        0.0, 0.0, 0.0));
  alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(-_HRPPD_CENTRAL_ROW_OFFSET_, 0.0, 0.0));
  
  G4Box *grid_box = new G4Box("Dummy", pitch/2, pitch/2, _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2);
  double value = pitch - _HRPPD_SUPPORT_GRID_BAR_WIDTH_;
  G4Box *grid_cut = new G4Box("Dummy", value/2, value/2, _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2 + 0.01*mm);
  auto *grid_shape = new G4SubtractionSolid("SupportGridBar", grid_box, grid_cut);
  auto *grid_log = new G4LogicalVolume(grid_shape, m_CarbonFiber, "SupportGridBar", 0, 0, 0);
  SetColor(grid_log, G4Colour(0, 1, 1, 0.5));
#endif
  
  {
#ifdef _USE_SiPM_QE_
    const G4int qeEntries = 15;
    
    // Create S13660-3050AE-08 SiPM QE table; FIXME: or was it S13661?;
    double WL[qeEntries] = { 325,  340,  350,  370,  400,  450,  500,  550,  600,  650,  700,  750,  800,  850,  900};
    double QE[qeEntries] = {0.04, 0.10, 0.20, 0.30, 0.35, 0.40, 0.38, 0.35, 0.27, 0.20, 0.15, 0.12, 0.08, 0.06, 0.04};
#else
    const G4int qeEntries = 26;
    
    // Create LAPPD QE table; use LAPPD126 from Alexey's March 2022 LAPPD Workshop presentation;
    double WL[qeEntries] = { 160,  180,  200,  220,  240,  260,  280,  300,  320,  340,  360,  380,  400,  
			     420,  440,  460,  480,  500,  520,  540,  560,  580,  600,  620,  640,  660};
    double QE[qeEntries] = {0.25, 0.26, 0.27, 0.30, 0.32, 0.35, 0.36, 0.36, 0.36, 0.36, 0.37, 0.35, 0.30, 
			    0.27, 0.24, 0.20, 0.18, 0.15, 0.13, 0.11, 0.10, 0.09, 0.08, 0.07, 0.05, 0.05};
#endif     
    
    double qemax = 0.0;
    G4double qePhotonEnergy[qeEntries], qeData[qeEntries];
    for(int iq=0; iq<qeEntries; iq++) {
      qePhotonEnergy[iq] = eV * _MAGIC_CFF_ / (WL[qeEntries - iq - 1] + 0.0);
      qeData        [iq] =                     QE[qeEntries - iq - 1] * _QE_DOWNSCALING_FACTOR_;
      
      if (qeData[iq] > qemax) qemax = qeData[iq];
    } //for iq
    
    pd->SetQE(eV * _MAGIC_CFF_ / WL[qeEntries-1], eV * _MAGIC_CFF_ / WL[0], 
	      // NB: last argument: want a built-in selection of unused photons, which follow the QE(lambda);
	      // see CherenkovSteppingAction::UserSteppingAction() for a usage case;
	      new G4DataInterpolation(qePhotonEnergy, qeData, qeEntries, 0.0, 0.0), qemax ? 1.0/qemax : 1.0);
    pd->SetGeometricEfficiency(_SENSOR_PLANE_GEOMETRIC_EFFICIENCY_ * _SAFETY_FACTOR_);
  }
  
  m_Geometry->AddPhotonDetector(cdet, pd->GetLogicalVolume(), pd);
  
  {
    const unsigned ydim = 5, qpop[ydim] = {9, 9, 9, 7, 5};
    
    //std::vector<TVector2> coord;
    std::vector<TVector2> coord;
    
    assert(qpop[0]%2);
    {
      double yOffset = 0.0;
      
      unsigned xdim = qpop[0];
      for(unsigned ix=0; ix<xdim; ix++) {
	double xOffset = pitch*(ix - (xdim-1)/2.) + (ix >= 5 ? 0.0 : -_HRPPD_CENTRAL_ROW_OFFSET_);
	
	if (ix == 4) continue;
	
	coord.push_back(TVector2(xOffset, yOffset));
      }
    } 
    for(unsigned iy=1; iy<ydim; iy++) {
      for(unsigned bt=0; bt<2; bt++) {
	double qsign = bt ? -1.0 : 1.0, yOffset = qsign*pitch*iy;
	
	unsigned xdim = qpop[iy];
	for(unsigned ix=0; ix<xdim; ix++) {
	  double xOffset = pitch*(ix - (xdim-1)/2.);
	  
	  coord.push_back(TVector2(xOffset, yOffset));
	}
      } //for bt
    } //for iy
   
    {
      int counter = 0, ccounter = 0;

      // FIXME: all hardcoded; round AWG numbers, don't be silly;
      double d18AWG = 1.0*mm, d24AWG = 0.5*mm;
      // Calculate effective copper layer thickness per HRPPD, assuming 6x 24AWG HV
      // lines and 2x 18AWG LV lines (ignore sense wires);
      double volume  = _HRPPD_TILE_SIZE_*(6*M_PI*d24AWG*d24AWG/4 + 2*M_PI*d18AWG*d18AWG/4);
      double surface = pow(_HRPPD_TILE_SIZE_, 2);
      double thickness = volume / surface;
      //printf("@@@ -> %f\n", thickness/mm);

      // FIXME: yes, this is not exactly fair: assume uniform layer;
      G4Box *cables  = new G4Box("Cables", pitch/2, pitch/2, thickness/2);
      G4LogicalVolume* cables_log = new G4LogicalVolume(cables, m_Copper, "Cables", 0, 0, 0);

      for(auto xy: coord) {
#if 1//_MBUDGET_
	// HRPPD assembly;
	//auto hrppd_phys = 
	new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zcont), hrppd_log, "HRPPD", m_fiducial_volume_log, 
			  false, counter);

	// Cabling imitation; pretend that these layers will simply accumulate towards the outer 
	// circumference; assume all cables are laid out horizontally along the cooling pipes;
	int column = (int)rint((xy.X()) / pitch);
	//printf("%d\n", column);
	for(int iq=0; iq<abs(column)+1; iq++)
	  new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zcables + iq*2*thickness), cables_log, 
			    "Cables", m_fiducial_volume_log, false, ccounter++);
#endif
	
	// Cut in the aluminum plate;
	alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(xy.X(), xy.Y(), 0.0));
	
	// Photocathode surface;
	auto surface = new FlatSurface((1/mm)*TVector3(sign*xy.X(), xy.Y(),sign*(_FIDUCIAL_VOLUME_OFFSET_ + zpdc)), 
				       TVector3(1*sign,0,0), TVector3(0,-1,0));
	
#ifdef _USE_PYRAMIDS_
	{
	  // FIXME: need to take care about overlap with the inner wall;
	  assert(0);
	  auto pyra_phys = 
	    new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zOffset - _PYRAMID_MIRROR_HEIGHT_/2), 
			      pyramid->GetLogicalVolume(), pyramid->GetSolid()->GetName(), m_fiducial_volume_log, 
			      false, counter);
	  
	  new G4LogicalBorderSurface(pyramid->GetSolid()->GetName(), m_gas_phys, pyra_phys, pyramid->GetMirrorSurface());
	}
#else
	// Yes, they are part of the gas volume;
#if 1//_MBUDGET_ 
	new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), m_gas_volume_length/2 - _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2), 
			  grid_log, "SupportGridBar", m_gas_volume_log, false, counter);
#endif
	
#endif
	
	{
#if defined(_USE_PYRAMIDS_) && defined(_USE_PYRAMIDS_OPTICS_)
	  // Calculate and store the pyramid mirror flat surfaces; 
	  OpticalBoundary *pboundaries[4];
	  for(unsigned iq=0; iq<4; iq++)
	    switch (iq) {
	    case 0:;
	    case 1:;
	    case 2:;
	    case 3:;
	      // Reflection off one of the four pyramid mirrors;
	      {		  
		double vx = (pitch - xyactive)/2, vy = _PYRAMID_MIRROR_HEIGHT_, norm = sqrt(vx*vx+vy*vy);
		double z0 = _FIDUCIAL_VOLUME_OFFSET_ + gas_volume_offset + zOffset - _PYRAMID_MIRROR_HEIGHT_/2;
		
		TVector3 nx( vx/norm, 0, -vy/norm), ny(0,-1,0), nz(0,0,1), nv(0,1,0);
		nx.Rotate(iq*M_PI/2, nz); ny.Rotate(iq*M_PI/2, nz);
		
		TVector3 center((pitch + xyactive)/4, 0.0, z0); 
		center.Rotate(iq*M_PI/2, nz); center += TVector3(xy.X(), xy.Y(), 0.0);
		if (flip) {
		  center.Rotate(M_PI, nv);
		  nx.Rotate(M_PI, nv);
		  ny.Rotate(M_PI, nv);
		} //if
		auto qsurface = new FlatSurface((1/mm)*center, nx, ny);
		
		auto boundary = new OpticalBoundary(m_Geometry->FindRadiator(m_gas_volume_log), qsurface, false);
		pboundaries[iq] = boundary;
		cdet->StoreOpticalBoundary(boundary);
	      }
	      break;
	    } //switch .. for iq 
#endif
	  
	  // Four "global" optical configurations considered, see the first switch below;
	  for(unsigned iq=0; iq<4; iq++) {
#ifndef _USE_CONICAL_MIRROR_OPTICS_
	    if (iq) continue;
#endif
	    
	    // And five more cases at the sensor end, see the second switch below;
	    for(unsigned ip=0; ip<5; ip++) {
#if !defined(_USE_PYRAMIDS_) || !defined(_USE_PYRAMIDS_OPTICS_)
	      if (ip != 4) continue;
#endif
	      // Mimic det->CreatePhotonDetectorInstance();
	      unsigned sector = 0, icopy = counter;
	      auto irt = pd->AllocateIRT(sector, icopy);
	      
	      // Aerogel and acrylic;
	      if (cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].find(sector) != 
		  cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].end())
		for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Upstream][sector])
		  irt->AddOpticalBoundary(boundary);
	      
	      // FIXME: will the gas-quartz boundary be described correctly in this sequence?;
	      switch (iq) {
	      case 0: 
		// Direct hit;
		break;
	      case 1:
	      case 2:         
		// Reflection on either inner or outer mirrors;
		irt->AddOpticalBoundary(m_mboundaries[iq-1]);
		break;
	      case 3:
		// Reflection on outer, then on inner mirror; happens at large angles; if the pyramids are
		// too high, these photons will undergo more reflections, and cannot be saved;
		irt->AddOpticalBoundary(m_mboundaries[1]);
		irt->AddOpticalBoundary(m_mboundaries[0]);
		break;
	      } //switch
	      
#if defined(_USE_PYRAMIDS_) && defined(_USE_PYRAMIDS_OPTICS_)
	      switch (ip) {
	      case 0:;
	      case 1:;
	      case 2:;
	      case 3:;
		irt->AddOpticalBoundary(pboundaries[ip]);
		break;
	      case 4: 
		// Direct hit;
		break;
	      } //switch
#endif
	      
	      // Quartz windows;
	      if (cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].find(sector) != 
		  cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].end())
		for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Downstream][sector])
		  irt->AddOpticalBoundary(boundary);
	      
	      // Terminate the optical path;
	      pd->AddItselfToOpticalBoundaries(irt, surface);
	    }
	  } //for ip
	} //for iq
	
	counter++;
      } //for xy
    } 
  }
  
#if 1//_MBUDGET_
  {
    auto *alu_log = new G4LogicalVolume(alu_shape, m_Aluminum,  "AluFrame", 0, 0, 0);
    // 'false': otherwise visualization hangs on this shape;
    SetColor(alu_log, G4Colour(0, 1, 1, 0.5), false);
    
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, _FIDUCIAL_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_ + alu_thickness/2),
		      alu_log, "AluFrame", m_fiducial_volume_log, false, 0);
  }
#endif

#if 1//_MBUDGET_
  for(unsigned im=0; im<2; im++)
    // FIXME: they are not really upstream (just need to store them);
    cdet->AddOpticalBoundary(CherenkovDetector::Upstream, 0, m_mboundaries[im]);
#endif
  
  for(auto radiator: cdet->Radiators())
    radiator.second->SetReferenceRefractiveIndex(radiator.second->GetMaterial()->RefractiveIndex(eV*_MAGIC_CFF_/_LAMBDA_NOMINAL_));
} // DetectorConstruction::DefinePhotonDetectors()

// -------------------------------------------------------------------------------------
