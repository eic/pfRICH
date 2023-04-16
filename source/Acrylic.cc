#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <tuning.h>

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>

#include <DetectorConstruction.h>

// -------------------------------------------------------------------------------------

void DetectorConstruction::DefineAcrylic(CherenkovDetector *cdet, G4UnionSolid *flange)
{
#ifdef _ACRYLIC_THICKNESS_
  double acthick = _ACRYLIC_THICKNESS_;
  m_gzOffset += acthick/2;
  
  G4Tubs *ac_tube  = new G4Tubs("Tmp", m_r0min, m_r0max, acthick/2, 0*degree, 360*degree);
  auto ac_shape = new G4SubtractionSolid("Acrylic", ac_tube, flange, 0, G4ThreeVector(0.0, 0.0, 0.0));
  G4LogicalVolume* ac_log = new G4LogicalVolume(ac_shape, m_Acrylic,  "Acrylic", 0, 0, 0);
  {
    TVector3 nx(1*sign,0,0), ny(0,-1,0);
    
    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0,0,_FIDUCIAL_VOLUME_OFFSET_ + 
							m_gas_volume_offset + m_gzOffset), nx, ny);
    m_Geometry->AddFlatRadiator(cdet, "Acrylic", CherenkovDetector::Upstream, 
				0, ac_log, m_Acrylic, surface, acthick/mm)
#ifdef _DISABLE_ACRYLIC_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }
  
  new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, m_gzOffset), ac_log, "Acrylic", m_gas_volume_log, false, 0);
  
  m_gzOffset += acthick/2 + _BUILDING_BLOCK_CLEARANCE_;
#endif
} // DetectorConstruction::DefineAcrylic()

// -------------------------------------------------------------------------------------
