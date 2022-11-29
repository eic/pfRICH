
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
//#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SubtractionSolid.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <tuning.h>

#include "DetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <CherenkovRadiator.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>
#include <CherenkovPhotonDetector.h>

// -------------------------------------------------------------------------------------

DetectorConstruction::DetectorConstruction(CherenkovDetectorCollection *geometry): 
  G4VUserDetectorConstruction(), m_Geometry(geometry)
{
} // DetectorConstruction::DetectorConstruction()

// -------------------------------------------------------------------------------------

G4OpticalSurface *DetectorConstruction::CreateLambertianMirrorSurface(const char *name, 
								      double reflectivity, double roughness)
{
  G4OpticalSurface* opSurface = new G4OpticalSurface(name);
  opSurface->SetType(dielectric_metal);
	
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();

  if (roughness) {
    // 'ground': yes, I need specular lobe reflection in this case;
    opSurface->SetFinish(ground);
    opSurface->SetModel(unified);
    // NB: Fresnel refraction (or specular lobe reflection) will be sampled with respect 
    // to the surface with a bit "smeared" normal direction vector; 
    opSurface->SetSigmaAlpha(roughness);
    
    G4double specularLobe[_WLDIM_];
    
    for(int iq=0; iq<_WLDIM_; iq++) 
      // NB: yes, it is only a specular lobe reflection here;
      specularLobe[iq] = 1.00;
    
    mpt->AddProperty("SPECULARLOBECONSTANT", GetPhotonEnergies(), specularLobe, _WLDIM_);
  }
  else
    // 'polished': pure Fresnel reflection;
    opSurface->SetFinish(polished);

  G4double reflectivity_array[_WLDIM_];
  for(int iq=0; iq<_WLDIM_; iq++) 
    reflectivity_array[iq] = reflectivity;
  mpt->AddProperty("REFLECTIVITY", GetPhotonEnergies(), reflectivity_array, _WLDIM_);
  opSurface->SetMaterialPropertiesTable(mpt);

  return opSurface;
} // DetectorConstruction::CreateLambertianMirrorSurface()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *DetectorConstruction::Construct( void )
{
  // Chemical elements and materials;
  DefineElements();
  DefineMaterials();

  // The experimental hall; FIXME: hardcoded;
  G4Tubs *expHall_box = new G4Tubs("World",  0*cm, 95*cm, 250*cm, 0*degree, 360*degree);
  auto expHall_log = new G4LogicalVolume(expHall_box, m_Air, "World", 0, 0, 0);
  G4VPhysicalVolume* expHall_phys = new G4PVPlacement(0, G4ThreeVector(), expHall_log, "World", 0, false, 0);
    
  auto *det = m_Geometry->GetDetector("pfRICH");
  //det->SetReadoutCellMask(~0x0);
  det->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);

  // Gas container volume;
  G4Tubs *gas_tube = new G4Tubs("GasVolume", _GAS_VOLUME_INNER_RADIUS_, _GAS_VOLUME_OUTER_RADIUS_, 
				_GAS_VOLUME_LENGTH_/2, 0*degree, 360*degree);
  G4LogicalVolume* gas_log = new G4LogicalVolume(gas_tube, m_Nitrogen,  "GasVolume", 0, 0, 0);
  auto gas_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, _GAS_VOLUME_OFFSET_), gas_log, 
				     "GasVolume", expHall_phys->GetLogicalVolume(), false, 0);
  {
    // FIXME: Z-location does not really matter here, right?;
    auto boundary = new FlatSurface(TVector3(0,0,0), TVector3(1,0,0), TVector3(0,-1,0));
    auto radiator = m_Geometry->SetContainerVolume(det, "GasVolume", 0, gas_log, m_Nitrogen, boundary);
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
    radiator->DisableOpticalPhotonGeneration();
#endif
  }

  {
    double  agthick[2] = {_AEROGEL_THICKNESS_1_, _AEROGEL_THICKNESS_2_};
    //const char *name[2] = {           "Aerogel1",            "Aerogel2"};
    // A running variable;
    double zOffset = -_GAS_VOLUME_LENGTH_/2 + _FIDUCIAL_VOLUME_OFFSET_;

    // min/max radii available for aerogel, mirrors and such;
    double r0min = _GAS_VOLUME_INNER_RADIUS_ + _FIDUCIAL_VOLUME_OFFSET_;
    double r0max = _GAS_VOLUME_OUTER_RADIUS_ - _FIDUCIAL_VOLUME_OFFSET_;
    // Aerogel;
    {
      // FIXME: yes, for now hardcoded;
      const unsigned adim[_AEROGEL_BAND_COUNT_] = {8, 14, 20};
      double rheight = (r0max - r0min - (_AEROGEL_BAND_COUNT_+1)*_AEROGEL_FRAME_WALL_THICKNESS_) / _AEROGEL_BAND_COUNT_;

      // Up to two aerogel layers;
      //for(auto aerogel: _m_Aerogel) {
      for(unsigned il=0; il<2; il++) {
	G4RadiatorMaterial *aerogel = 0;
#ifdef _AEROGEL_1_ 
	if (!il) aerogel = _m_Aerogel[_AEROGEL_1_];
#else
	if (!il) continue;
#endif
#ifdef _AEROGEL_2_ 
	if ( il) aerogel = _m_Aerogel[_AEROGEL_2_];
#else
	if ( il) continue;
#endif
	//if (!m_Aerogel[il]) continue;
	
	{
	  zOffset += agthick[il]/2;
	  
	  // First aerogel sectors and azimuthal spacers;
	  CherenkovRadiator *radiator = 0;
	  for(unsigned ir=0; ir<_AEROGEL_BAND_COUNT_; ir++) {
	    double apitch = 360*degree / adim[ir];
	    double r0 = r0min + (ir+1)*_AEROGEL_FRAME_WALL_THICKNESS_ + ir*rheight, r1 = r0 + rheight, rm = (r0+r1)/2;

	    // Calculate angular space occupied by the spacers and by the tiles; no gas gaps for now;
	    // assume that a wegde shape is good enough (GEANT visualization does not like boolean objects), 
	    // rather than creating constant thicjkess azimuthal spacers; just assume that spacer thickness is 
	    // _AEROGEL_FRAME_WALL_THICKNESS_ at r=rm;
	    double l0 = 2*M_PI*rm/adim[ir], l1 = _AEROGEL_FRAME_WALL_THICKNESS_, lsum = l0 + l1;

	    // FIXME: names overlap!;
	    double wd0 = (l0/lsum)*(360*degree / adim[ir]), wd1 = (l1/lsum)*(360*degree / adim[ir]);
	    //G4Tubs *ag_tube  = new G4Tubs(  name[il], r0, r1, agthick[il]/2, 0*degree, wd0);
	    G4Tubs *ag_tube  = new G4Tubs(aerogel->GetName(), r0, r1, agthick[il]/2, 0*degree, wd0);
	    G4Tubs *sp_tube  = new G4Tubs(        "A-Spacer", r0, r1, agthick[il]/2,      wd0, wd1);

	    //G4LogicalVolume* ag_log = new G4LogicalVolume(ag_tube, m_Aerogel[il], name[il],   0, 0, 0);
	    G4LogicalVolume* ag_log = new G4LogicalVolume(ag_tube, aerogel, aerogel->GetName(),   0, 0, 0);
	    G4LogicalVolume* sp_log = new G4LogicalVolume(sp_tube, _AEROGEL_SPACER_MATERIAL_, "A-Spacer", 0, 0, 0);
	    if (ir) 
	      // THis of course assumes that optical surfaces are the same (no relative tilts between bands, etc);
	      m_Geometry->AddRadiatorLogicalVolume(radiator, ag_log);
	    else {
	      TVector3 nx(1,0,0), ny(0,-1,0);
	      
	      auto surface = new FlatSurface((1/mm)*TVector3(0,0,_GAS_VOLUME_OFFSET_ + zOffset), nx, ny);
	      radiator = m_Geometry->AddFlatRadiator(det, aerogel->GetName(), CherenkovDetector::Upstream, 
						     //0, ag_log, m_Aerogel[il], surface, agthick[il]/mm);
						     0, ag_log, aerogel, surface, agthick[il]/mm);
	    } //if
	    
	    for(unsigned ia=0; ia<adim[ir]; ia++) {
	      G4RotationMatrix *rZ = new G4RotationMatrix(CLHEP::HepRotationZ(ia*apitch));
	      new G4PVPlacement(rZ, G4ThreeVector(0.0, 0.0, zOffset), ag_log, aerogel->GetName(), gas_log, false, 0);
	      new G4PVPlacement(rZ, G4ThreeVector(0.0, 0.0, zOffset), sp_log,         "A-Spacer", gas_log, false, 0);
	    } //for ia
	  } //for ir

	  // Then radial spacers;
	  for(unsigned ir=0; ir<_AEROGEL_BAND_COUNT_+1; ir++) {
	    double r0 = r0min + ir*(rheight + _AEROGEL_FRAME_WALL_THICKNESS_), r1 = r0 + _AEROGEL_FRAME_WALL_THICKNESS_;

	    G4Tubs *sp_tube  = new G4Tubs("R-Spacer", r0, r1, agthick[il]/2, 0*degree, 360*degree);
	    G4LogicalVolume* sp_log = new G4LogicalVolume(sp_tube, _AEROGEL_SPACER_MATERIAL_, "R-Spacer", 0, 0, 0);
	    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, zOffset), sp_log, "R-Spacer", gas_log, false, 0);
	  } //for ir
	  
	  zOffset += agthick[il]/2 + _LAYER_TO_LAYER_GAP_;
	}
      } //for il
    }

    // Acrylic;
#ifdef _ACRYLIC_THICKNESS_
    {
      double acthick = _ACRYLIC_THICKNESS_;
      zOffset += acthick/2;

      G4Tubs *ac_tube  = new G4Tubs("Acrylic", r0min, r0max, acthick/2, 0*degree, 360*degree);
      G4LogicalVolume* ac_log = new G4LogicalVolume(ac_tube, m_Acrylic,  "Acrylic", 0, 0, 0);
      {
	TVector3 nx(1,0,0), ny(0,-1,0);
	
	auto surface = new FlatSurface((1/mm)*TVector3(0,0,_GAS_VOLUME_OFFSET_ + zOffset), nx, ny);
	auto radiator = m_Geometry->AddFlatRadiator(det, "Acrylic", CherenkovDetector::Upstream, 
						    0, ac_log, m_Acrylic, surface, acthick/mm);
#ifdef _DISABLE_ACRYLIC_PHOTONS_
	radiator->DisableOpticalPhotonGeneration();
#endif
      }
      
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, zOffset), ac_log, "Acrylic", gas_log, false, 0);

      zOffset += acthick/2 + _LAYER_TO_LAYER_GAP_;
    }
#endif

    // Book optical boundaries of the inner and outer conical mirrors; do not want to spoil the 
    // central area (mirro-less) optical path (would happen if define them in situ);
    OpticalBoundary *mboundaries[2] = {0, 0};

    {
      const char *names[2] = {"InnerMirror", "OuterMirror"};
      double mlen = _GAS_VOLUME_LENGTH_/2 - zOffset - _SENSOR_AREA_LENGTH_ - _LAYER_TO_LAYER_GAP_ - _PYRAMID_MIRROR_HEIGHT_;
      double mpos = zOffset + mlen/2, thickness = 0.1*mm;
      double r0[2] = {r0min, r0max}, r1[2] = {_CONICAL_MIRROR_INNER_RADIUS_, _CONICAL_MIRROR_OUTER_RADIUS_};

      {
	//G4Tubs *block_tube = new G4Tubs("Block", 0.0, rmin, 0.1*mm, 0*degree, 360*degree);
	//G4LogicalVolume* block_log = new G4LogicalVolume(block_tube, mAbsorber,  "Block", 0, 0, 0);
	//new G4PVPlacement(0, G4ThreeVector(0,0,agzc + 3.5*mm), block_log, "Block", gas_log, false, 0);
      }

      for(unsigned im=0; im<2; im++) {
	//auto mgroup = new CherenkovMirrorGroup();
	
	{
	  auto mshape = im ? new G4Cons(names[im], r0[im] - thickness, r0[im], r1[im] - thickness, r1[im], mlen/2, 0*degree, 360*degree) :
	    new G4Cons(names[im], r0[im], r0[im] + thickness, r1[im], r1[im] + thickness, mlen/2, 0*degree, 360*degree);
	  
	  // NB: geometry will be saved in [mm] throughout the code;
	  auto mirror = new ConicalMirror(mshape, m_Absorber, (1/mm)*TVector3(0.0, 0.0, _GAS_VOLUME_OFFSET_ + mpos),
					  TVector3(0,0,1), r0[im]/mm, r1[im]/mm, mlen/mm);
	  
	  mirror->SetColor(G4Colour(0, 0, 1, 0.5));
	  mirror->SetReflectivity(_MIRROR_REFLECTIVITY_, this);
	  
	  // Mimic mirror->PlaceWedgeCopies() call; FIXME: can be vastly simplified for this simple case;
	  mirror->DefineLogicalVolume();
	  G4VPhysicalVolume *phys = new G4PVPlacement(/*rZ*/0, G4ThreeVector(0,0,mpos), mirror->GetLogicalVolume(), 
						      mirror->GetSolid()->GetName(), 
						      gas_phys->GetLogicalVolume(), false, 0);//m_Copies.size());
	  mirror->AddCopy(mirror->CreateCopy(phys));
	  //#if 0
	  {
	    auto msurface = mirror->GetMirrorSurface();
	    
	    if (msurface)
	      // Do I really need them separately?;
	      //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
	      new G4LogicalBorderSurface(mirror->GetSolid()->GetName(), gas_phys, phys, msurface);
	  }
	  //#endif	  

	  auto mcopy = dynamic_cast<SurfaceCopy*>(mirror->GetCopy(0));//m_Copies[iq]);
	  mcopy->m_Surface = dynamic_cast<ParametricSurface*>(mirror)->_Clone(0.0, TVector3(0,0,1));
	  if (!im) dynamic_cast<ConicalSurface*>(mcopy->m_Surface)->SetConvex();
	  
	  {
	    //mgroup->AddMirror(mirror);
	    m_Geometry->AddMirrorLookupEntry(mirror->GetLogicalVolume(), mirror);
	    
	    auto surface = dynamic_cast<SurfaceCopy*>(mirror->GetCopy(0))->m_Surface;
	    mboundaries[im] = new OpticalBoundary(m_Geometry->FindRadiator(gas_log), surface, false);
	    
	    // Complete the radiator volume description; this is the rear side of the container gas volume;
	    //+? det->GetRadiator("GasVolume")->m_Borders[0].second = surface;
	  }
	}
      } //for im
    }

    // Photon detector; 
    {
      zOffset = _GAS_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_;
      double xysize = _HRPPD_TILE_SIZE_, wndthick = _HRPPD_WINDOW_THICKNESS_, zwnd = zOffset + wndthick/2;

      // Full size quartz window;
      G4Box *wnd_box  = new G4Box("QuartzWindow", xysize/2, xysize/2, wndthick/2);
      G4LogicalVolume* wnd_log = new G4LogicalVolume(wnd_box, m_FusedSilica,  "QuartzWindow", 0, 0, 0);
      {	
	TVector3 nx(1,0,0), ny(0,-1,0);
	
	// A single entry; this assumes of course that all the windows are at the same Z, and parallel to each other;
	auto surface = new FlatSurface((1/mm)*TVector3(0,0,_GAS_VOLUME_OFFSET_ + zwnd), nx, ny);
	auto radiator = m_Geometry->AddFlatRadiator(det, "QuartzWindow", CherenkovDetector::Downstream, 
						    0, wnd_log, m_FusedSilica, surface, wndthick/mm);
#ifdef _DISABLE_HRPPD_WINDOW_PHOTONS_
	radiator->DisableOpticalPhotonGeneration();
#endif
      }	

      double pitch = xysize + _HRPPD_INSTALLATION_GAP_, xyactive = _HRPPD_ACTIVE_AREA_SIZE_;
      double certhick = _HRPPD_CERAMIC_BODY_THICKNESS_, zcer = zOffset + wndthick + certhick/2;

      // HRPPD body imitation; just a thick pictureframe;
      G4Box *cer_box  = new G4Box("CeramicBox",   xysize/2,   xysize/2, certhick/2);
      G4Box *cut_box  = new G4Box("CeramicCut", xyactive/2, xyactive/2, certhick/2 + 1*mm);
      const G4ThreeVector translation(0,0,0);
      auto ceramic = new G4SubtractionSolid("CeramicBody", cer_box, cut_box, 0, translation);
      G4LogicalVolume* cer_log = new G4LogicalVolume(ceramic, m_Absorber,  "CeramicBody", 0, 0, 0);
      
      double pdthick = 0.01*mm, zpdc = zOffset + wndthick + pdthick/2;
      G4Box *pd_box  = new G4Box("PhotoDetector", xyactive/2, xyactive/2, pdthick/2);
      auto pd = new CherenkovPhotonDetector(pd_box, m_Bialkali);
      pd->DefineLogicalVolume();
      pd->SetColor(G4Colour(1, 0, 0, 1.0));

      // 'pitch': yes, want them installed without gaps;
#ifdef _USE_PYRAMIDS_
      G4Box *pyra_box = new G4Box("Dummy", pitch/2, pitch/2, _PYRAMID_MIRROR_HEIGHT_/2);
      G4Trd *pyra_cut = new G4Trd("Dummy", pitch/2, xyactive/2, pitch/2, xyactive/2, _PYRAMID_MIRROR_HEIGHT_/2 + 0.01*mm);
      auto pyra_shape = new G4SubtractionSolid("MirrorPyramid", pyra_box, pyra_cut, 0, translation);
      
      // NB: geometry will be saved in [mm] throughout the code;
      auto pyramid = new CherenkovMirror(pyra_shape, m_Absorber);
      
      pyramid->SetColor(G4Colour(0, 1, 1, 0.5));
      pyramid->SetReflectivity(_MIRROR_REFLECTIVITY_, this);
	
      // Mimic the essential part of the mirror->PlaceWedgeCopies() call;
      pyramid->DefineLogicalVolume();
      m_Geometry->AddMirrorLookupEntry(pyramid->GetLogicalVolume(), pyramid);
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
	double WL[qeEntries] = { 160,  180,  200,  220,  240,  260,  280,  300,  320,  340,  360,  380,  400,  420,  440,  460,  480,  500,  520,  540,  560,  580,  600,  620,  640,  660};
	double QE[qeEntries] = {0.25, 0.26, 0.27, 0.30, 0.32, 0.35, 0.36, 0.36, 0.36, 0.36, 0.37, 0.35, 0.30, 0.27, 0.24, 0.20, 0.18, 0.15, 0.13, 0.11, 0.10, 0.09, 0.08, 0.07, 0.05, 0.05};
#endif     
	
	G4double qePhotonEnergy[qeEntries], qeData[qeEntries];
	for(int iq=0; iq<qeEntries; iq++) {
	  qePhotonEnergy[iq] = eV * _MAGIC_CFF_ / (WL[qeEntries - iq - 1] + 0.0);
	  qeData        [iq] =                     QE[qeEntries - iq - 1];
	} //for iq
	
	pd->SetQE(eV * _MAGIC_CFF_ / WL[qeEntries-1], eV * _MAGIC_CFF_ / WL[0], 
		  new G4DataInterpolation(qePhotonEnergy, qeData, qeEntries, 0.0, 0.0));
	pd->SetGeometricEfficiency(_SENSOR_PLANE_GEOMETRIC_EFFICIENCY_ * _SAFETY_FACTOR_);
      }

      m_Geometry->AddPhotonDetector(det, pd->GetLogicalVolume(), pd);

      {
	const unsigned ydim = 5, qpop[ydim] = {9, 9, 9, 7, 5};

	std::vector<TVector2> coord;

	assert(qpop[0]%2);
	{
	  double yOffset = 0.0;
	  
	  unsigned xdim = qpop[0];
	  for(unsigned ix=0; ix<xdim; ix++) {
	    double xOffset = pitch*(ix - (xdim-1)/2.) + (ix <= 3 ? -pitch/6 : pitch/6);
	    	    
	    if (ix == 4) continue;

	    coord.push_back(TVector2(xOffset, yOffset));
	  }
	} 
	for(unsigned iy=1; iy<ydim; iy++) {
	  for(unsigned bt=0; bt<2; bt++) {
	    double sign = bt ? -1.0 : 1.0, yOffset = sign*pitch*iy;
	    
	    unsigned xdim = qpop[iy];
	    for(unsigned ix=0; ix<xdim; ix++) {
	      double xOffset = pitch*(ix - (xdim-1)/2.), dy = xOffset ? 0.0 : sign*pitch/6;
	      
	      coord.push_back(TVector2(xOffset, yOffset + dy));
	    }
	  } //for bt
	} //for iy

	{
	  int counter = 0;

	  for(auto xy: coord) {
	    {
	      // Window layer;
	      auto wnd_phys = new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zwnd), wnd_log, "QuartzWindow", gas_log, false, counter);
	      // Ceramic pictureframe body behind it;
	      auto cer_phys = new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zcer), cer_log, "CeramicBody",  gas_log, false, counter);

	      // Rough reflective optical border between them;
	      G4OpticalSurface* opWindowMetallization = 
		CreateLambertianMirrorSurface("WindowMetallization", _HRPPD_METALLIZATION_REFLECTIVITY_, _HRPPD_METALLIZATION_ROUGHNESS_);
	      new G4LogicalBorderSurface("WindowMetallization", wnd_phys, cer_phys, opWindowMetallization);
	    }

	    new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zpdc), pd->GetLogicalVolume(), "PhotoDetector", gas_log, false, counter);
	    auto surface = new FlatSurface((1/mm)*TVector3(xy.X(), xy.Y(), _GAS_VOLUME_OFFSET_ + zpdc), TVector3(1,0,0), TVector3(0,-1,0));
	    
#ifdef _USE_PYRAMIDS_
	    {
	      auto pyra_phys = 
		new G4PVPlacement(0, G4ThreeVector(xy.X(), xy.Y(), zOffset - _PYRAMID_MIRROR_HEIGHT_/2), 
				  pyramid->GetLogicalVolume(), pyramid->GetSolid()->GetName(), gas_log, false, counter);

	      new G4LogicalBorderSurface(pyramid->GetSolid()->GetName(), gas_phys, pyra_phys, pyramid->GetMirrorSurface());
	    }
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
		    double z0 = _GAS_VOLUME_OFFSET_ + zOffset - _PYRAMID_MIRROR_HEIGHT_/2;
		    
		    TVector3 nx( vx/norm, 0, -vy/norm), ny(0,-1,0), nz(0,0,1);
		    nx.Rotate(iq*M_PI/2, nz); ny.Rotate(iq*M_PI/2, nz);
		    
		    TVector3 center((pitch + xyactive)/4, 0.0, z0); 
		    center.Rotate(iq*M_PI/2, nz); center += TVector3(xy.X(), xy.Y(), 0.0);
		    auto qsurface = new FlatSurface((1/mm)*center, nx, ny);
		    
		    auto boundary = new OpticalBoundary(m_Geometry->FindRadiator(gas_log), qsurface, false);
		    pboundaries[iq] = boundary;
		    det->StoreOpticalBoundary(boundary);
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
		  if (det->m_OpticalBoundaries[CherenkovDetector::Upstream].find(sector) != 
		      det->m_OpticalBoundaries[CherenkovDetector::Upstream].end())
		    for(auto boundary: det->m_OpticalBoundaries[CherenkovDetector::Upstream][sector])
		      irt->AddOpticalBoundary(boundary);
		  
		  // FIXME: will the gas-quartz boundary be described correctly in this sequence?;
		  switch (iq) {
		  case 0: 
		    // Direct hit;
		    break;
		  case 1:
		  case 2:         
		    // Reflection on either inner or outer mirrors;
		    irt->AddOpticalBoundary(mboundaries[iq-1]);
		    break;
		  case 3:
		    // Reflection on outer, then on inner mirror; happens at large angles; if the pyramids are
		    // too high, these photons will undergo more reflections, and cannot be saved;
		    irt->AddOpticalBoundary(mboundaries[1]);
		    irt->AddOpticalBoundary(mboundaries[0]);
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
		  if (det->m_OpticalBoundaries[CherenkovDetector::Downstream].find(sector) != 
		      det->m_OpticalBoundaries[CherenkovDetector::Downstream].end())
		    for(auto boundary: det->m_OpticalBoundaries[CherenkovDetector::Downstream][sector])
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
    }
    
    for(unsigned im=0; im<2; im++)
      // FIXME: they are not really upstream (just need to store them);
      det->AddOpticalBoundary(CherenkovDetector::Upstream, 0, mboundaries[im]);
  }

  for(auto radiator: det->Radiators())
    radiator.second->SetReferenceRefractiveIndex(radiator.second->GetMaterial()->RefractiveIndex(eV*_MAGIC_CFF_/_LAMBDA_NOMINAL_));

  return expHall_phys;
} // DetectorConstruction::Construct()

// -------------------------------------------------------------------------------------
