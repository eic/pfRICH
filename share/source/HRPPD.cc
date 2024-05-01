
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4SubtractionSolid.hh>
#include <G4DataInterpolation.hh>

#define _GEANT_SOURCE_CODE_
//#include <G4Object.h>

#include <hrppd.h>
//#include <share.h>

#include "DetectorConstruction.h"

#include <G4RadiatorMaterial.h>
#include <CherenkovPhotonDetector.h>

// -------------------------------------------------------------------------------------

G4LogicalVolume *DetectorConstruction::BuildHRPPD(G4LogicalVolume *wnd_log, G4Box *pd_box, CherenkovPhotonDetector *pd)
{
  double pdthick = 0.01*mm;

  auto hrppd_box = new G4Box("HRPPD", _HRPPD_TILE_SIZE_/2, _HRPPD_TILE_SIZE_/2, _HRPPD_CONTAINER_VOLUME_HEIGHT_/2);
  // Well, the inside volume will then be air as well; fine;
  auto hrppd_log = new G4LogicalVolume(hrppd_box, m_Air,  "HRPPD", 0, 0, 0);

  // HRPPD body imitation; ignore MCPs (small fraction compared to the ceramic body);
  auto cer_box = new G4Box("CeramicBox", _HRPPD_TILE_SIZE_/2, _HRPPD_TILE_SIZE_/2, _HRPPD_CERAMIC_BODY_THICKNESS_/2);
  auto cut_box = new G4Box("CeramicCut", _HRPPD_OPEN_AREA_SIZE_/2, _HRPPD_OPEN_AREA_SIZE_/2, _HRPPD_CERAMIC_BODY_THICKNESS_/2);
  auto ceramic = new G4SubtractionSolid("CeramicBody", cer_box, cut_box, 0, 
					G4ThreeVector(0,0, -_HRPPD_BASEPLATE_THICKNESS_));
  auto cer_log = new G4LogicalVolume(ceramic, m_Ceramic,  "CeramicBody", 0, 0, 0);
  SetColor(cer_log, G4Colour(1, 1, 1, 0.5));
  
  auto plating  = new G4Box("Plating", _HRPPD_OPEN_AREA_SIZE_/2, _HRPPD_OPEN_AREA_SIZE_/2, _HRPPD_PLATING_LAYER_THICKNESS_/2);
  auto plating_log = new G4LogicalVolume(plating, m_Silver,  "Plating", 0, 0, 0);
  
  auto mcp_box = new G4Box("MCP", _HRPPD_OPEN_AREA_SIZE_/2, _HRPPD_OPEN_AREA_SIZE_/2, _EFFECTIVE_MCP_THICKNESS_/2);
  auto mcp_log = new G4LogicalVolume(mcp_box, _EFFECTIVE_MCP_MATERIAL_,  "MCP", 0, 0, 0);
  
  pd->SetCopyIdentifierLevel(1);
  pd->DefineLogicalVolume();
  pd->SetColor(G4Colour(1, 0, 0, 1.0));
  // Cannot access GEANT shapes in the reconstruction code -> store this value;
  pd->SetActiveAreaSize(_HRPPD_ACTIVE_AREA_SIZE_);
  
  auto qd_box  = new G4Box("FakePhotoDetector", _HRPPD_ACTIVE_AREA_SIZE_/2, _HRPPD_ACTIVE_AREA_SIZE_/2, pdthick/2);
  auto qd_log = new G4LogicalVolume(qd_box, m_Bialkali, "FakePhotoDetector", 0, 0, 0);
  
  {
    double accu = -_HRPPD_CONTAINER_VOLUME_HEIGHT_/2;
    
    // Window layer;
    //+auto wnd_phys = 
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _HRPPD_WINDOW_THICKNESS_/2), wnd_log, 
		      "QuartzWindow", hrppd_log, false, 0);
    accu += _HRPPD_WINDOW_THICKNESS_;
    
    // Ceramic pictureframe body behind it;
    {
      //+auto cer_phys = 
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _HRPPD_CERAMIC_BODY_THICKNESS_/2), cer_log,
			"CeramicBody", hrppd_log, false, 0);
      // Place plating layer in the middle;
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _HRPPD_CERAMIC_BODY_THICKNESS_/2), plating_log, 
			"Plating",     hrppd_log, false, 0);
      
      // Place effective MCP material nearby;
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _HRPPD_CERAMIC_BODY_THICKNESS_/2 + 
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
      
      accu += _HRPPD_CERAMIC_BODY_THICKNESS_ + 1*mm;
    }
    
    // Readout PCB; FIXME: ignore copper to the moment (will be few dozens of microns overall; take 
    // PCB thickness on a high end to kind of compensate for this;
    {
      auto pcb_box = new G4Box("PCB", _READOUT_PCB_SIZE_/2, _READOUT_PCB_SIZE_/2, _READOUT_PCB_THICKNESS_/2);
      auto pcb_log = new G4LogicalVolume(pcb_box, m_FR4,  "PCB", 0, 0, 0);
      
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, accu + _READOUT_PCB_THICKNESS_/2), pcb_log, "PCB", 
			hrppd_log, false, 0);

      accu += _READOUT_PCB_THICKNESS_ + 0.01*mm;
    }
    
    // ASIC chips;
    {
      double ipitch = _READOUT_PCB_SIZE_/2;
        
      auto asic_box  = new G4Box("ASIC", _ASIC_SIZE_XY_/2, _ASIC_SIZE_XY_/2, _ASIC_THICKNESS_/2);
      auto asic_log = new G4LogicalVolume(asic_box, _ASIC_MATERIAL_,  "ASIC", 0, 0, 0);

      for(unsigned ix=0; ix<2; ix++) {
	double xOffset = ipitch*(ix - (2-1)/2.);
	
	for(unsigned iy=0; iy<2; iy++) {
	  double yOffset = ipitch*(iy - (2-1)/2.);
	  
	  new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, accu + _ASIC_THICKNESS_/2), asic_log, "ASIC", 
			    hrppd_log, false, ix*2 + iy);
	} //for iy
      } //for ix
      
      accu += _ASIC_THICKNESS_ + 0.01*mm;
    }

    // Cold plates and pipes;
    {
      double ipitch = _READOUT_PCB_SIZE_/2;

      auto cplate_box = new G4Box("ColdPlate", _ASIC_SIZE_XY_/2, _ASIC_SIZE_XY_/2, _COLD_PLATE_THICKNESS_/2);
      auto cplate_log = new G4LogicalVolume(cplate_box, _COLD_PLATE_MATERIAL_,  "ColdPlate", 0, 0, 0);

      // FIXME: yes, have to make the pipes 1.5mm shorter to fit into the HRPPD container volume;
      // must be a minor simplification I guess;
      double length = _HRPPD_TILE_SIZE_ /*+ _HRPPD_INSTALLATION_GAP_*/, iradius = _COOLING_PIPE_INNER_DIAMETER_/2; 
      double oradius = iradius + _COOLING_PIPE_WALL_THICKNESS_;
      auto pipe_tubs = new G4Tubs("CoolingPipe", 0.0, oradius, length/2, 0*degree, 360*degree);
      auto pipe_log  = new G4LogicalVolume(pipe_tubs, _COOLING_PIPE_MATERIAL_, "CoolingPipe", 0, 0, 0);

      auto water_tubs = new G4Tubs("Water", 0.0, iradius, length/2, 0*degree, 360*degree);
      auto water_log  = new G4LogicalVolume(water_tubs, m_Water, "Water", 0, 0, 0);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), water_log, "Water", pipe_log, false, 0);

      G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(90*degree));

      for(unsigned bt=0; bt<2; bt++) {
	double yOffset = ipitch*(bt - (2-1)/2.);
	
	new G4PVPlacement(rY, G4ThreeVector(0.0, yOffset, accu + _COLD_PLATE_THICKNESS_ + oradius), 
			  pipe_log, "CoolingPipe", hrppd_log, false, 0);
	
	for(unsigned lr=0; lr<2; lr++) {
	  double xOffset = ipitch*(lr - (2-1)/2.);
	  
	  new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, accu + _COLD_PLATE_THICKNESS_/2), cplate_log, 
			    "ColdPlate", hrppd_log, false, 0);
	} //for lr
      } //for bt

      accu += _COLD_PLATE_THICKNESS_ + 2*oradius + 0.01*mm;
    }
  }

  {
#ifdef _USE_HRPPD_24_DATA_
    const G4int qeEntries = 44;
    
    // Create HRPPD QE table; use HRPPD #24 measurements by Alexey (e-mail from 2024/04/29);
    double WL[qeEntries] = {  200,   210,   220,   230,   240,   250,   260,   270,   280,   290,
			      300,   310,   320,   330,   340,   350,   360,   370,   380,   390, 
			      400,   410,   420,   430,   440,   450,   460,   470,   480,   490, 
			      500,   510,   520,   530,   540,   550,   560,   570,   580,   590, 
			      600,   610,   620,   630};
    double QE[qeEntries] = {0.227, 0.277, 0.301, 0.313, 0.322, 0.316, 0.317, 0.304, 0.289, 0.284,
                            0.286, 0.286, 0.291, 0.305, 0.316, 0.325, 0.327, 0.336, 0.343, 0.334,
			    0.314, 0.295, 0.269, 0.246, 0.226, 0.206, 0.188, 0.170, 0.156, 0.142,
                            0.130, 0.119, 0.109, 0.099, 0.091, 0.083, 0.076, 0.070, 0.064, 0.058,
                            0.053, 0.048, 0.044, 0.040};
#else                               
    const G4int qeEntries = 26;
    
    // Create HRPPD QE table; use LAPPD #126 from Alexey's March 2022 LAPPD Workshop presentation;
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

  return hrppd_log;
} // DetectorConstruction::BuildHRPPD()

// -------------------------------------------------------------------------------------
