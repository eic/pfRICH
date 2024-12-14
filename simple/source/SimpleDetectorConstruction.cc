
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4DataInterpolation.hh"

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <simple.h>

#include "SimpleDetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <CherenkovRadiator.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>
#include <CherenkovPhotonDetector.h>

// -------------------------------------------------------------------------------------

SimpleDetectorConstruction::SimpleDetectorConstruction(CherenkovDetectorCollection *geometry): 
  DetectorConstruction(geometry)
{
} // SimpleDetectorConstruction::SimpleDetectorConstruction()

// -------------------------------------------------------------------------------------

#define _FAKE_HRPPD_WINDOW_THICKNESS_            (5.0*mm)
#define _FAKE_HRPPD_ACTIVE_AREA_SIZE_         (1290.0*mm)
#define _FAKE_HRPPD_TILE_SIZE_                (1298.0*mm)
#define _FAKE_HRPPD_CONTAINER_VOLUME_HEIGHT_    (32.0*mm)

#define _FAKE_QE_DOWNSCALING_FACTOR_          (30.0/37.0)
#define _FAKE_SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
#define _FAKE_SAFETY_FACTOR_                       (0.70)

G4LogicalVolume *SimpleDetectorConstruction::BuildFakeHRPPD(G4LogicalVolume *wnd_log, G4Box *pd_box,
							    CherenkovPhotonDetector *pd)
{
  double pdthick = 0.01*mm;

  auto hrppd_box =
    new G4Box("HRPPD", _FAKE_HRPPD_TILE_SIZE_/2, _FAKE_HRPPD_TILE_SIZE_/2, _FAKE_HRPPD_CONTAINER_VOLUME_HEIGHT_/2);
  // Well, the inside volume will then be air as well; fine;
  auto hrppd_log = new G4LogicalVolume(hrppd_box, m_Air,  "HRPPD", 0, 0, 0);
  
  pd->SetCopyIdentifierLevel(1);
  pd->DefineLogicalVolume();
  pd->SetColor(G4Colour(1, 0, 0, 1.0));
  // Cannot access GEANT shapes in the reconstruction code -> store this value;
  pd->SetActiveAreaSize(_FAKE_HRPPD_ACTIVE_AREA_SIZE_);
  
  auto qd_box  =
    new G4Box("FakePhotoDetector", _FAKE_HRPPD_ACTIVE_AREA_SIZE_/2, _FAKE_HRPPD_ACTIVE_AREA_SIZE_/2, pdthick/2);
  auto qd_log = new G4LogicalVolume(qd_box, m_Bialkali, "FakePhotoDetector", 0, 0, 0);

  {
    double accu = -_FAKE_HRPPD_CONTAINER_VOLUME_HEIGHT_/2;
    
    // Window layer;
    //+auto wnd_phys = 
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _FAKE_HRPPD_WINDOW_THICKNESS_/2), wnd_log, 
		      "QuartzWindow", hrppd_log, false, 0);
    accu += _FAKE_HRPPD_WINDOW_THICKNESS_;
    
    // Fake photodector layer (photocathode);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + pdthick/2), qd_log, "FakePhotoDetector", 
		      hrppd_log, false, 0);
    accu += pdthick;
    // Photodector layer (photocathode);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + pdthick/2), pd->GetLogicalVolume(), "PhotoDetector", 
		      hrppd_log, false, 0);
    accu += pdthick;
  }

  {                      
    const G4int qeEntries = 26;
    
    // Create HRPPD QE table; use LAPPD #126 from Alexey's March 2022 LAPPD Workshop presentation;
    double WL[qeEntries] = { 160,  180,  200,  220,  240,  260,  280,  300,  320,  340,  360,  380,  400,  
			     420,  440,  460,  480,  500,  520,  540,  560,  580,  600,  620,  640,  660};
    double QE[qeEntries] = {0.25, 0.26, 0.27, 0.30, 0.32, 0.35, 0.36, 0.36, 0.36, 0.36, 0.37, 0.35, 0.30, 
			    0.27, 0.24, 0.20, 0.18, 0.15, 0.13, 0.11, 0.10, 0.09, 0.08, 0.07, 0.05, 0.05};  
    
    double qemax = 0.0;
    G4double qePhotonEnergy[qeEntries], qeData[qeEntries];
    for(int iq=0; iq<qeEntries; iq++) {
      qePhotonEnergy[iq] = eV * _MAGIC_CFF_ / (WL[qeEntries - iq - 1] + 0.0);
      qeData        [iq] =                     QE[qeEntries - iq - 1] * _FAKE_QE_DOWNSCALING_FACTOR_;
      
      if (qeData[iq] > qemax) qemax = qeData[iq];
    } //for iq
    
    pd->SetQE(eV * _MAGIC_CFF_ / WL[qeEntries-1], eV * _MAGIC_CFF_ / WL[0], 
	      // NB: last argument: want a built-in selection of unused photons, which follow the QE(lambda);
	      // see CherenkovSteppingAction::UserSteppingAction() for a usage case;
	      new G4DataInterpolation(qePhotonEnergy, qeData, qeEntries, 0.0, 0.0), qemax ? 1.0/qemax : 1.0);
    pd->SetGeometricEfficiency(_FAKE_SENSOR_PLANE_GEOMETRIC_EFFICIENCY_ * _FAKE_SAFETY_FACTOR_);
  }

  return hrppd_log;
} // SimpleDetectorConstruction::BuildFakeHRPPD()

// -------------------------------------------------------------------------------------

void SimpleDetectorConstruction::BuildFakePhotonDetectorMatrix(CherenkovDetector *cdet, DarkBox *dbox, 
							       double fvzOffset, double wzOffset)
{
  double pdthick = 0.01*mm, zpdc = wzOffset + _FAKE_HRPPD_WINDOW_THICKNESS_ + pdthick/2;
  G4Box *pd_box  = new G4Box("PhotoDetector", _FAKE_HRPPD_ACTIVE_AREA_SIZE_/2,
			     _FAKE_HRPPD_ACTIVE_AREA_SIZE_/2, pdthick/2);
  auto pd = new CherenkovPhotonDetector(pd_box, m_Bialkali);

  // Full size quartz window; FIXME: Sapphire, here and in all other places;
  auto wnd_box = new G4Box("QuartzWindow", _FAKE_HRPPD_TILE_SIZE_/2, _FAKE_HRPPD_TILE_SIZE_/2,
			   _FAKE_HRPPD_WINDOW_THICKNESS_/2);
  auto wnd_log =
    new G4LogicalVolume(wnd_box, /*_HRPPD_WINDOW_MATERIAL_*/m_FusedSilica,  "QuartzWindow", 0, 0, 0);
  auto hrppd_log = BuildFakeHRPPD(wnd_log, pd_box, pd);

  {	
    TVector3 nx(1/**sign*/,0,0), ny(0,-1,0);
    
    auto surface = 
      new FlatSurface(/*sign**/(1/mm)*TVector3(0,0,fvzOffset + wzOffset + _FAKE_HRPPD_WINDOW_THICKNESS_/2), nx, ny);

    m_Geometry->AddFlatRadiator(cdet, "QuartzWindow", CherenkovDetector::Downstream, 
				0, wnd_log, m_FusedSilica, surface, _FAKE_HRPPD_WINDOW_THICKNESS_/mm)
#ifdef _DISABLE_HRPPD_WINDOW_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }	
  
  m_Geometry->AddPhotonDetector(cdet, pd->GetLogicalVolume(), pd);
   
  {
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, wzOffset + _FAKE_HRPPD_CONTAINER_VOLUME_HEIGHT_/2), 
		      hrppd_log, "HRPPD", dbox->m_fiducial_volume_phys->GetLogicalVolume(), false, 0);
      
    // Photocathode surface;
    auto surface = new FlatSurface((1/mm)*TVector3(/*sign*xyptr.m_X, xyptr.m_Y, */0.0, 0.0, /*sign**/(fvzOffset + zpdc)), 
				   TVector3(1/**sign*/,0,0), TVector3(0,-1,0));
      
    {
      // Mimic det->CreatePhotonDetectorInstance();
      unsigned sector = 0, icopy = 0;
      auto irt = pd->AllocateIRT(sector, icopy);
	    
      // Aerogel and acrylic;
      if (cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].find(sector) != 
	  cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].end())
	for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Upstream][sector])
	  irt->AddOpticalBoundary(boundary);
      
      // Fused silica windows;
      if (cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].find(sector) != 
	  cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].end())
	for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Downstream][sector])
	  irt->AddOpticalBoundary(boundary);
      
      // Terminate the optical path;
      pd->AddItselfToOpticalBoundaries(irt, surface);
    }
  } 

  for(auto radiator: cdet->Radiators())
    radiator.second->SetReferenceRefractiveIndex(radiator.second->GetMaterial()->RefractiveIndex(eV*_MAGIC_CFF_/_LAMBDA_NOMINAL_));
} // SimpleDetectorConstruction::BuildFakePhotonDetectorMatrix()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *SimpleDetectorConstruction::Construct( void )
{
  // Chemical elements and materials;
  DefineElements();
  DefineMaterials();

  // The experimental hall; FIXME: hardcoded;
  auto expHall_box = new G4Box("PFRICH_World",  200*cm/2, 200*cm/2, 400*cm/2);
  auto expHall_log = new G4LogicalVolume(expHall_box, m_Air, "PFRICH_World", 0, 0, 0);
  auto expHall_phys = new G4PVPlacement(0, G4ThreeVector(), expHall_log, "PFRICH_World", 0, false, 0);

  auto *cdet = m_Geometry->GetDetector("pfRICH-Simple");
  //det->SetReadoutCellMask(~0x0);
  cdet->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);

  double fvOffset = _FIDUCIAL_VOLUME_OFFSET_SIMPLE_;
  double fvLength = _FIDUCIAL_VOLUME_LENGTH_SIMPLE_;
    
  // No conical mirrors and no pyramids;
  auto dbox = new DarkBox(false, false, false);

  // Fiducial volume (air); has to be called "PFRICH";
  auto *fiducial_volume_box = new G4Box("PFRICH", 1300.0*mm/2, 1300.0*mm/2, fvLength/2);
  auto fiducial_volume_log = new G4LogicalVolume(fiducial_volume_box, m_Air,  "PFRICH", 0, 0, 0);
  // All volumes are defined assuming EIC h-going endcap orientation;
  dbox->m_fiducial_volume_phys = 
    // Yes, do not want to rotate the 1x1 box;
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, /*sign**/fvOffset), fiducial_volume_log, 
		      "PFRICH", expHall_phys->GetLogicalVolume(), false, 0);
    
  // Gas container volume;
  dbox->m_gas_volume_length = fvLength - _VESSEL_FRONT_SIDE_THICKNESS_ - _SENSOR_AREA_LENGTH_;
  double gas_volume_offset = -(_SENSOR_AREA_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_)/2;
  auto gas_box = new G4Box("GasVolume", 1299.0*mm/2, 1299.0*mm/2, dbox->m_gas_volume_length/2);
  auto gas_volume_log = new G4LogicalVolume(gas_box, _GAS_RADIATOR_,  "GasVolume", 0, 0, 0);
  dbox->m_gas_volume_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gas_volume_offset), 
					      gas_volume_log, "GasVolume", fiducial_volume_log, false, 0);
    
  {
    // FIXME: Z-location does not really matter here, right?;
    auto boundary = new FlatSurface(TVector3(0,0,0), /*sign**/TVector3(1,0,0), TVector3(0,-1,0));
    
    m_Geometry->SetContainerVolume(cdet, "GasVolume", 0, gas_volume_log, _GAS_RADIATOR_, boundary)
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }
    
  // A running variable;
  double gzOffset = -dbox->m_gas_volume_length/2 + _BUILDING_BLOCK_CLEARANCE_;
    
  // Aerogel, a single layer;
  {
    double agthick = _AEROGEL_THICKNESS_;
    G4RadiatorMaterial *aerogel = m_Aerogel[_AEROGEL_];
	
    {
      gzOffset += agthick/2;
	  
      auto ag_tube = new G4Box(aerogel->GetName(), 1298.0*mm/2, 1298.0*mm/2, agthick/2);
      auto ag_log = new G4LogicalVolume(ag_tube, aerogel, aerogel->GetName(), 0, 0, 0);
      {
	TVector3 nx(1/**sign*/,0,0), ny(0,-1,0);
	
	auto surface = new FlatSurface(/*sign**/(1/mm)*TVector3(0, 0, fvOffset + 
							    gas_volume_offset + gzOffset), nx, ny);
	auto radiator = m_Geometry->AddFlatRadiator(cdet, aerogel->GetName(), CherenkovDetector::Upstream, 
						    0, ag_log, aerogel, surface, agthick/mm);
#ifdef _DISABLE_AEROGEL_PHOTONS_
	radiator->DisableOpticalPhotonGeneration();
#endif
	m_Geometry->AddRadiatorLogicalVolume(radiator, ag_log);
	
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), ag_log, aerogel->GetName(),
			  gas_volume_log, false, 0);
      }
      
      // FIXME: not really needed that big between the two layers?;
      gzOffset += agthick/2 + _BUILDING_BLOCK_CLEARANCE_;
    }
    //} //for il
      
    // Acrylic filter;
#ifdef _ACRYLIC_THICKNESS_
    {
      double acthick = _ACRYLIC_THICKNESS_;
      gzOffset += acthick/2;
      
      auto ac_box  = new G4Box("Acrylic", 1298.0/2, 1298.0*mm/2, acthick/2);
      auto ac_log = new G4LogicalVolume(ac_box, m_Acrylic,  "Acrylic", 0, 0, 0);
      {
	TVector3 nx(1/**sign*/,0,0), ny(0,-1,0);
	
	auto surface = new FlatSurface(/*sign**/(1/mm)*TVector3(0, 0, fvOffset + 
							    gas_volume_offset + gzOffset), nx, ny);
	m_Geometry->AddFlatRadiator(cdet, "Acrylic", CherenkovDetector::Upstream, 
				    0, ac_log, m_Acrylic, surface, acthick/mm)
#ifdef _DISABLE_ACRYLIC_PHOTONS_
	  ->DisableOpticalPhotonGeneration()
#endif
	  ;
      }
      
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), ac_log, "Acrylic", gas_volume_log, false, 0);
      
      gzOffset += acthick/2 + _BUILDING_BLOCK_CLEARANCE_;
    }
#endif
  } 

  BuildFakePhotonDetectorMatrix(cdet, dbox, fvOffset, 
				// FIXME: is this offset correct?; front plate thickness?;
				fvLength/2 - _SENSOR_AREA_LENGTH_);
    
  m_DarkBoxes.push_back(dbox);
  
  return expHall_phys;
} // SimpleDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------
