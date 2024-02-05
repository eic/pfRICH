#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>
#include <G4LogicalBorderSurface.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <epic.h>

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>

#include <EpicDetectorConstruction.h>

// -------------------------------------------------------------------------------------

//
// FIXME: a lot of duplicate code and a lot of hardcoded numbers;
//

void EpicDetectorConstruction::BuildVesselWalls(DarkBox *dbox)
{
  auto fvmother = dbox->m_fiducial_volume_phys->GetLogicalVolume();

  // Inner vessel wall; it is part of the gas volume;
  {
    // FIXME: pyramid case, please;
    double wlength = dbox->m_gas_volume_length - _HRPPD_SUPPORT_GRID_BAR_HEIGHT_;
    auto outer = FlangeCut(wlength, _FLANGE_CLEARANCE_ + _VESSEL_INNER_WALL_THICKNESS_);
    auto *wall_shape = new G4SubtractionSolid("InnerWall", outer, 
					      FlangeCut(wlength + 1*mm, _FLANGE_CLEARANCE_),
					      0, G4ThreeVector(0.0, 0.0, 0.0));
    auto *wall_log = new G4LogicalVolume(wall_shape, m_QuarterInch_CF_HoneyComb,  "InnerWall", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, -_HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2), wall_log, "InnerWall", 
		      dbox->m_gas_volume_phys->GetLogicalVolume(), false, 0);

    // Add a pair of PEEK reinforcement rings; full radial thickness (a bit more material);
    {
      double rlength = _INCH_/4;

      auto qouter = FlangeCut(rlength, _FLANGE_CLEARANCE_ + _VESSEL_INNER_WALL_THICKNESS_);
      auto *ring_shape = new G4SubtractionSolid("InnerWallAluRing", qouter, 
						FlangeCut(rlength + 1*mm, _FLANGE_CLEARANCE_),
						0, G4ThreeVector(0.0, 0.0, 0.0));
      auto *ring_log = new G4LogicalVolume(ring_shape, m_PEEK,  "InnerWallAluRing", 0, 0, 0);
      for(unsigned iq=0; iq<2; iq++) {
	double zOffset = (iq ? 1.0 : -1.0)*(wlength/2 - rlength/2);

	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, zOffset), ring_log, "InnerWallAluRing", wall_log, false, iq);
      } //for iq
    }
  }

  // Front vessel wall; it is part of the fiducial volume;
  {
    auto *wall_tube = 
      new G4Tubs("FrontWall", 0.0, _VESSEL_OUTER_RADIUS_, _VESSEL_FRONT_SIDE_THICKNESS_/2, 0*degree, 360*degree);
    auto *wall_shape = new G4SubtractionSolid("FrontWall", wall_tube, 
					      FlangeCut(_VESSEL_FRONT_SIDE_THICKNESS_ + 1*mm, _FLANGE_CLEARANCE_),
					      0, G4ThreeVector(0.0, 0.0, 0.0));
    auto *wall_log = new G4LogicalVolume(wall_shape, m_QuarterInch_CF_HoneyComb,  "FrontWall", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, -_FIDUCIAL_VOLUME_LENGTH_/2 + _VESSEL_FRONT_SIDE_THICKNESS_/2), 
		      wall_log, "FrontWall", fvmother, false, 0);

    // Add inner reinforcement ring (PEEK);
    {
      double rlength = _VESSEL_FRONT_SIDE_THICKNESS_;

      auto qouter = FlangeCut(rlength, _FLANGE_CLEARANCE_ + _INCH_/4);
      auto *ring_shape = new G4SubtractionSolid("FrontWallAluRing1", qouter, 
						FlangeCut(rlength + 1*mm, _FLANGE_CLEARANCE_),
						0, G4ThreeVector(0.0, 0.0, 0.0));
      auto *ring_log = new G4LogicalVolume(ring_shape, m_PEEK,  "FrontWallAluRing1", 0, 0, 0);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), ring_log, "FrontWallAluRing1", wall_log, false, 0);
    }
    // Add outer reinforcement ring (aluminum);
    {
      double rlength = _VESSEL_FRONT_SIDE_THICKNESS_;

      auto *ring_tube = new G4Tubs("FrontWallAluRing2", _VESSEL_OUTER_RADIUS_ - _INCH_/2, _VESSEL_OUTER_RADIUS_, 
				   rlength/2, 0*degree, 360*degree);

      auto *ring_log = new G4LogicalVolume(ring_tube, m_Aluminum,  "FrontWallAluRing2", 0, 0, 0);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), ring_log, "FrontWallAluRing2", wall_log, false, 0);
    }
  }

  // Outer vessel wall; it is part of the fiducial volume;
  {
    double wlength = dbox->m_gas_volume_length;
    auto *wall_tube =
      new G4Tubs("OuterWall", m_gas_volume_radius, _VESSEL_OUTER_RADIUS_, wlength/2, 0*degree, 360*degree);
    auto *wall_log = new G4LogicalVolume(wall_tube, m_HalfInch_CF_HoneyComb,  "OuterWall", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, -_FIDUCIAL_VOLUME_LENGTH_/2 + _VESSEL_FRONT_SIDE_THICKNESS_ + 
				       wlength/2), 
		      wall_log, "OuterWall", fvmother, false, 0);

    // Add a pair of aluminum reinforcement rings; full radial thickness (a bit more material);
    {
      double rlength = _INCH_/2;

      auto *ring_tube = 
	new G4Tubs("OuterWallAluRing", m_gas_volume_radius, _VESSEL_OUTER_RADIUS_, rlength/2, 0*degree, 360*degree);

      auto *ring_log = new G4LogicalVolume(ring_tube, m_Aluminum,  "OuterWallAluRing", 0, 0, 0);
      for(unsigned iq=0; iq<2; iq++) {
	double zOffset = (iq ? 1.0 : -1.0)*(wlength/2 - rlength/2);

	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, zOffset), ring_log, "OuterWallAluRing", wall_log, false, iq);
      } //for iq
    }
  }
} // EpicDetectorConstruction::BuildVesselWalls()

// -------------------------------------------------------------------------------------
