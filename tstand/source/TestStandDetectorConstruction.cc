
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

//#include <share.h>
#include <tstand.h>

#include "TestStandDetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <CherenkovRadiator.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>
#include <CherenkovPhotonDetector.h>
#include <AsphericLens.h>

// -------------------------------------------------------------------------------------

TestStandDetectorConstruction::TestStandDetectorConstruction(CherenkovDetectorCollection *geometry): 
  DetectorConstruction(geometry)
{
} // TestStandDetectorConstruction::TestStandDetectorConstruction()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *TestStandDetectorConstruction::Construct( void )
{
  // Chemical elements and materials;
  DefineElements();
  DefineMaterials();

  // The experimental hall; FIXME: hardcoded;
  auto expHall_box = new G4Box("PFRICH_World",  400*mm/2, 400*mm/2, (_FIDUCIAL_VOLUME_LENGTH_+1*mm)/2);
  auto expHall_log = new G4LogicalVolume(expHall_box, m_Air, "PFRICH_World", 0, 0, 0);
  auto expHall_phys = new G4PVPlacement(0, G4ThreeVector(), expHall_log, "PFRICH_World", 0, false, 0);
    
  {
    auto *cdet = m_Geometry->GetDetector("pfRICH-1x1");
    //det->SetReadoutCellMask(~0x0);
    cdet->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);
    
    // Do not use any conical mirrors for now; and do not use pyramids / cones on a test stand;
    auto dbox = new DarkBox(false, false, false);

    // Fiducial volume (air); has to be called "PFRICH";
    auto *fiducial_volume_box = new G4Box("PFRICH", 399.0*mm/2, 399.0*mm/2, _FIDUCIAL_VOLUME_LENGTH_/2);
    auto fiducial_volume_log = new G4LogicalVolume(fiducial_volume_box, m_Air,  "PFRICH", 0, 0, 0);
    // All volumes are defined assuming EIC h-going endcap orientation (dRICH case was developed this way 
    // for ATHENA); therefore need to rotate by 180 degrees around Y axis;
    G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(flip ? 180*degree : 0));
    dbox->m_fiducial_volume_phys =  
      new G4PVPlacement(rY, G4ThreeVector(0.0, 0.0, sign*_FIDUCIAL_VOLUME_OFFSET_), fiducial_volume_log, 
			"PFRICH", expHall_phys->GetLogicalVolume(), false, 0);
    
    // Gas container volume;
    dbox->m_gas_volume_length = _FIDUCIAL_VOLUME_LENGTH_ /*- _VESSEL_FRONT_SIDE_THICKNESS_*/ - _SENSOR_AREA_LENGTH_;
    double gas_volume_offset = -(_SENSOR_AREA_LENGTH_ /*- _VESSEL_FRONT_SIDE_THICKNESS_*/)/2;
    auto gas_box = new G4Box("GasVolume", 398.0*mm/2, 398.0*mm/2, dbox->m_gas_volume_length/2);
    auto gas_volume_log = new G4LogicalVolume(gas_box, _GAS_RADIATOR_,  "GasVolume", 0, 0, 0);
    dbox->m_gas_volume_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gas_volume_offset), 
						gas_volume_log, "GasVolume", fiducial_volume_log, false, 0);
    
    {
      // FIXME: Z-location does not really matter here, right?;
      auto boundary = new FlatSurface(TVector3(0,0,0), sign*TVector3(1,0,0), TVector3(0,-1,0));
      
      m_Geometry->SetContainerVolume(cdet, "GasVolume", 0, gas_volume_log, _GAS_RADIATOR_, boundary)
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
	->DisableOpticalPhotonGeneration()
#endif
	;
    }
    
    {
      std::vector<MisalignedLocation2D> xycoord;

      // 'false' here does not really play a role since pyramids were suppressed when creating
      // a DarkBox instance (see above);
      xycoord.push_back(MisalignedLocation2D(0.0, 0.0, 0.0, 0.0, 0.0, false));

      BuildPhotonDetectorMatrix(cdet, dbox, _FIDUCIAL_VOLUME_OFFSET_,
				// FIXME: is this offset correct?; front plate thickness?;
				_FIDUCIAL_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_, xycoord);
    }

#ifdef _ZCOORD_ASPHERIC_LENS_
    {
      // #67-265; D 25.0mm, EFL 20.0mm; CT 14.00mm;
      auto lens = new AsphericLens(m_FusedSilica, 25.000*mm,  9.170*mm, -1.102626E+00, 0.0, 
				   8.791869E-05, 3.051652E-07, -7.950597E-10, 8.042043E-12, 0.0, 0.0, 2.27*mm);
      
      // Z location is given with respect to the upstream end of the gas volume;
      lens->Install(m_Geometry, this, cdet, dbox, _ZCOORD_ASPHERIC_LENS_, false);
    }
#endif

    m_DarkBoxes.push_back(dbox);
  } 

  return expHall_phys;
} // TestStandDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------
