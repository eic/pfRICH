
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4GDMLParser.hh>

#include <G4VSensitiveDetector.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>

#ifdef _POLYHEDRA_VESSEL_SEGMENTATION_
#include <G4Polyhedra.hh>
#endif

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <hrppd.h>
#include <epic.h>

#include "EpicDetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <CherenkovRadiator.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>
#include <CherenkovPhotonDetector.h>

#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
#include <G4TransportationManager.hh>
#include <G4FieldManager.hh>

#include <MagneticField.h>
#endif

// -------------------------------------------------------------------------------------

EpicDetectorConstruction::EpicDetectorConstruction(CherenkovDetectorCollection *geometry): 
  DetectorConstruction(geometry),
  m_expHall_log(0), 
  m_gas_volume_radius(0.0), 
  m_gzOffset(0.0), 
  m_r0min(0.0), m_r0max(0.0), m_gas_volume_offset(.0)
{
} // EpicDetectorConstruction::EpicDetectorConstruction()

// -------------------------------------------------------------------------------------

#ifdef _CREATE_FAKE_SENSITIVE_VOLUMES_
class FakeSD : public G4VSensitiveDetector {
public:
  FakeSD(const G4String& name, const G4String& hitsCollectionName): G4VSensitiveDetector(name) {};
  virtual ~FakeSD() {};
  
  // methods from base class
  virtual void   Initialize(G4HCofThisEvent* hitCollection) {};
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) { return true; };
  virtual void   EndOfEvent(G4HCofThisEvent* hitCollection) {};
};
#endif

void EpicDetectorConstruction::ConstructSDandField()
{
  // Fake sensitive detector (photocathode);
#ifdef _CREATE_FAKE_SENSITIVE_VOLUMES_
  FakeSD *sd = new FakeSD("FakeSD", "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  SetSensitiveDetector("PhotoDetector", sd, true);
#endif

#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
  G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  auto *mfield = new MagneticField(_USE_MAGNETIC_FIELD_);

  fieldMgr->SetDetectorField(mfield);
  fieldMgr->CreateChordFinder(mfield);
#endif
} // EpicDetectorConstruction::ConstructSDandField()

// -------------------------------------------------------------------------------------

G4UnionSolid *EpicDetectorConstruction::FlangeCut(double length, double clearance)
{
  // FIXME: do I really care about re-using the same names for these shapes?;
  auto *eflange = new G4Tubs("FlangeEpipe", 0.0, _FLANGE_EPIPE_DIAMETER_/2 + clearance, 
			     length/2, 0*degree, 360*degree);
  auto *hflange = new G4Tubs("FlangeHpipe", 0.0, _FLANGE_HPIPE_DIAMETER_/2 + clearance, 
			     length/2, 0*degree, 360*degree);
  // A wedge bridging two cylinders;
  double r0 = _FLANGE_EPIPE_DIAMETER_/2 + clearance, r1 = _FLANGE_HPIPE_DIAMETER_/2 + clearance;
  double L = _FLANGE_HPIPE_OFFSET_, a = r0*L/(r0-r1), b = r0*r0/a, c = r1*(a-b)/r0;

  // GEANT variables to define G4Trap;
  double pDz = length/2, pTheta = 0.0, pPhi = 0.0, pDy1 = (a - b - c)/2, pDy2 = pDy1; 
  double pDx1 = sqrt(r0*r0 - b*b), pDx2 = pDx1*r1/r0, pDx3 = pDx1, pDx4 = pDx2, pAlp1 = 0.0, pAlp2 = 0.0;
  auto *wedge = new G4Trap("FlangeWedge", pDz, pTheta, pPhi, pDy1, pDx1, pDx2, pAlp1, pDy2, pDx3, pDx4, pAlp2);
  G4RotationMatrix *rZ = new G4RotationMatrix(CLHEP::HepRotationZ(-90*degree));
  auto *flange_shape = new G4UnionSolid("Tmp", eflange, hflange, 0, G4ThreeVector(-_FLANGE_HPIPE_OFFSET_, 0.0, 0.0));

  return new G4UnionSolid("Tmp", flange_shape, wedge, rZ, G4ThreeVector(-b - pDy1, 0.0, 0.0));
} // EpicDetectorConstruction::FlangeCut()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *EpicDetectorConstruction::Construct( void )
{
  // Chemical elements and materials;
  DefineElements();
  DefineMaterials();

  // The experimental hall; FIXME: hardcoded;
  auto *expHall_tube = new G4Tubs("PFRICH_World",  0*cm, 400*cm/2, 1200*cm/2, 0*degree, 360*degree);
  m_expHall_log = new G4LogicalVolume(expHall_tube, m_Air, "PFRICH_World", 0, 0, 0);
  auto expHall_phys = new G4PVPlacement(0, G4ThreeVector(), m_expHall_log, "PFRICH_World", 0, false, 0);
    
  auto *det = m_Geometry->GetDetector("pfRICH");
  //det->SetReadoutCellMask(~0x0);
  det->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);

  auto dbox = new DarkBox(_USE_PYRAMIDS_, _USE_PYRAMID_OPTICS_, _USE_CONICAL_MIRROR_OPTICS_);

  // Fiducial volume (air); has to be called "PFRICH";
  auto *fiducial_volume_tube = 
    new G4Tubs("PFRICH", 0.0, _VESSEL_OUTER_RADIUS_, _FIDUCIAL_VOLUME_LENGTH_/2, 0*degree, 360*degree);
  auto *fiducial_volume_shape = new G4SubtractionSolid("PFRICH", fiducial_volume_tube, 
  						       FlangeCut(_FIDUCIAL_VOLUME_LENGTH_ + 1*mm, _FLANGE_CLEARANCE_), 
  						       0, G4ThreeVector(0.0, 0.0, 0.0));
  auto fiducial_volume_log = new G4LogicalVolume(fiducial_volume_shape, m_Air,  "PFRICH", 0, 0, 0);
  // All volumes are defined assuming EIC h-going endcap orientation (dRICH case was developed this way 
  // for ATHENA); therefore need to rotate by 180 degrees around Y axis;
  G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(flip ? 180*degree : 0));
  dbox->m_fiducial_volume_phys = 
    new G4PVPlacement(rY, G4ThreeVector(0.0, 0.0, sign*_FIDUCIAL_VOLUME_OFFSET_), fiducial_volume_log, 
		      "PFRICH", expHall_phys->GetLogicalVolume(), false, 0);
    
  // Gas container volume;
  dbox->m_gas_volume_length = _FIDUCIAL_VOLUME_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_ - _SENSOR_AREA_LENGTH_;
  m_gas_volume_offset = -(_SENSOR_AREA_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_)/2;
  m_gas_volume_radius = _VESSEL_OUTER_RADIUS_ - _VESSEL_OUTER_WALL_THICKNESS_;
  auto gas_tube = new G4Tubs("GasVolume", 0.0, m_gas_volume_radius, dbox->m_gas_volume_length/2, 0*degree, 360*degree);
  auto *gas_shape = new G4SubtractionSolid("GasVolume", gas_tube, 
					   // Yes, account for vessel inner wall thickness;
					   FlangeCut(dbox->m_gas_volume_length + 1*mm, _FLANGE_CLEARANCE_),
					   0, G4ThreeVector(0.0, 0.0, 0.0));
  auto gas_volume_log = new G4LogicalVolume(gas_shape, _GAS_RADIATOR_,  "GasVolume", 0, 0, 0);
  dbox->m_gas_volume_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, m_gas_volume_offset), 
					      gas_volume_log, "GasVolume", fiducial_volume_log, false, 0);

  BuildVesselWalls(dbox);//x, fiducial_volume_log);

  {
    // FIXME: Z-location does not really matter here, right?;
    auto boundary = new FlatSurface(TVector3(0,0,0), sign*TVector3(1,0,0), TVector3(0,-1,0));

    m_Geometry->SetContainerVolume(det, "GasVolume", 0, gas_volume_log, _GAS_RADIATOR_, boundary)
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }

  // To be used in boolean operations in several places;
  auto flange = FlangeCut(dbox->m_gas_volume_length + 1*mm, _FLANGE_CLEARANCE_ + _VESSEL_INNER_WALL_THICKNESS_ + 
			  _BUILDING_BLOCK_CLEARANCE_);

  // A running variable;
  m_gzOffset = -dbox->m_gas_volume_length/2 + _BUILDING_BLOCK_CLEARANCE_;
  
  // min/max radii available for aerogel, mirrors and such; r0min can be an estimate, since a flange
  // cut will be applied at the end to all these shapes anyway;
  //m_r0min = _FLANGE_EPIPE_DIAMETER_/2 + _FLANGE_CLEARANCE_ + 
  //_VESSEL_INNER_WALL_THICKNESS_ + _BUILDING_BLOCK_CLEARANCE_;
  //m_r0max = m_gas_volume_radius - _BUILDING_BLOCK_CLEARANCE_;
  m_r0min = _CONICAL_MIRROR_US_INNER_RADIUS_;
  m_r0max = _CONICAL_MIRROR_US_OUTER_RADIUS_;
  
  // Aerogel;
  DefineAerogel(det, dbox, flange);
  
  // Acrylic;
#ifdef _ACRYLIC_THICKNESS_
  DefineAcrylic(det, dbox, flange);
#endif
  
  DefineMirrors(det, dbox, gas_tube, flange);
  
  // Photon detectors; 
  {
    unsigned const hdim = 9;
    double pitch = _HRPPD_TILE_SIZE_ + _HRPPD_INSTALLATION_GAP_;
    std::vector<MisalignedLocation2D> xycoord;

    const unsigned flags[hdim][hdim] = {
      // NB: WYSIWIG fashion; well, it is top/ bottom and left/right symmetric;
      {0, 0, 1, 1, 1, 1, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 2, 1, 1, 1, 1},
      {3, 3, 3, 4, 0, 2, 1, 1, 1},
      {1, 1, 1, 1, 2, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 1, 1, 1, 1, 0, 0}
    };

    for(unsigned ix=0; ix<hdim; ix++) {
      double xOffset = pitch*(ix - (hdim-1)/2.); 
      
      for(unsigned iy=0; iy<hdim; iy++) {
	double yOffset = pitch*(iy - (hdim-1)/2.); 
	unsigned flag = flags[hdim-iy-1][ix];

	if (!flag) continue;

	double qxOffset = xOffset + (flag >= 3 ? -_HRPPD_CENTRAL_ROW_OFFSET_ : 0.0);
	xycoord.push_back(MisalignedLocation2D(qxOffset, yOffset, 0.0, 0.0, 0.0, flag%2));
      } //for iy
    } //for ix

    dbox->DefinePyramidMirrorGeometry(_HRPPD_TILE_SIZE_ + _HRPPD_INSTALLATION_GAP_, _PYRAMID_MIRROR_HEIGHT_);
    BuildPhotonDetectorMatrix(det, dbox, _FIDUCIAL_VOLUME_OFFSET_, 
			      // FIXME: is this offset correct?; front plate thickness?;
			      _FIDUCIAL_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_, xycoord);

    // Aluminum plate with HRPPD slots;
    {
      double alu_thickness = _INCH_/2 - _HRPPD_SUPPORT_GRID_BAR_HEIGHT_;
      auto *alu_tube = new G4Tubs("AluFrame", 0.0, _VESSEL_OUTER_RADIUS_, alu_thickness/2, 0*degree, 360*degree);
      auto *alu_shape = new G4SubtractionSolid("AluFrame", alu_tube, 
					       FlangeCut(alu_thickness + 1*mm, _FLANGE_CLEARANCE_), 0, G4ThreeVector(0.0, 0.0, 0.0));
      double alu_cut_size = _HRPPD_TILE_SIZE_ + 0.5*mm;
      auto *alu_cut = new G4Box("AluWndCut", alu_cut_size/2, alu_cut_size/2, alu_thickness/2 + 1*mm);
      // Cut the central area by hand;
      alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(                        0.0, 0.0, 0.0));
      alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(-_HRPPD_CENTRAL_ROW_OFFSET_, 0.0, 0.0));
      
#ifndef _USE_PYRAMIDS_
      G4Box *grid_box = new G4Box("Dummy", dbox->m_PyramidMirrorWidth/2, dbox->m_PyramidMirrorWidth/2, _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2);
      double value = dbox->m_PyramidMirrorWidth - _HRPPD_SUPPORT_GRID_BAR_WIDTH_;
      G4Box *grid_cut = new G4Box("Dummy", value/2, value/2, _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2 + 0.01*mm);
      auto *grid_shape = new G4SubtractionSolid("SupportGridBar", grid_box, grid_cut);
      auto *grid_log = new G4LogicalVolume(grid_shape, m_CarbonFiber, "SupportGridBar", 0, 0, 0);
      SetColor(grid_log, G4Colour(0, 1, 1, 0.5));
#endif

      // Cuts in the aluminum plate;
      {
#ifndef _USE_PYRAMIDS_
	unsigned counter = 0;
#endif

	for(auto xyptr: xycoord) {
	  alu_shape = new G4SubtractionSolid("AluFrame", alu_shape, alu_cut, 0, G4ThreeVector(xyptr.m_X, xyptr.m_Y, 0.0));
	  
#ifndef _USE_PYRAMIDS_
	  // Yes, they are part of the gas volume;
	  new G4PVPlacement(0, G4ThreeVector(xyptr.m_X, xyptr.m_Y, dbox->m_gas_volume_length/2 - _HRPPD_SUPPORT_GRID_BAR_HEIGHT_/2), 
			    grid_log, "SupportGridBar", gas_volume_log, false, counter++);
#endif
	} //for xyptr
      }
      {
	auto *alu_log = new G4LogicalVolume(alu_shape, m_Aluminum,  "AluFrame", 0, 0, 0);
	// 'false': otherwise visualization hangs on this shape;
	SetColor(alu_log, G4Colour(0, 1, 1, 0.5), false);
	
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, _FIDUCIAL_VOLUME_LENGTH_/2 - _SENSOR_AREA_LENGTH_ + alu_thickness/2),
			  alu_log, "AluFrame", fiducial_volume_log, false, 0);
      }
    } 

    m_DarkBoxes.push_back(dbox);
  }

#ifdef _IMPORT_TRACKER_GDML_FILE_ 
  ImportGdmlFile(_IMPORT_TRACKER_GDML_FILE_);
#endif
#ifdef _IMPORT_BEAMPIPE_GDML_FILE_ 
  // FIXME: '12' hardcoded;
  ImportGdmlFile(_IMPORT_BEAMPIPE_GDML_FILE_, 12);
#endif

#ifdef _GENERATE_GDML_OUTPUT_
  {
    G4GDMLParser parser;
  
    unlink(_GENERATE_GDML_OUTPUT_);
    parser.Write(_GENERATE_GDML_OUTPUT_, expHall_phys);
    //parser.Write(_GENERATE_GDML_OUTPUT_, m_fiducial_volume_phys);
  }
#endif
  
  return expHall_phys;
} // EpicDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------

void EpicDetectorConstruction::ExportGdmlFile( void )
{
#ifdef _GENERATE_GDML_OUTPUT_
  G4GDMLParser parser;
  
  unlink(_GENERATE_GDML_OUTPUT_);
  //parser.Write(_GENERATE_GDML_OUTPUT_, expHall_phys);
  //+parser.Write(_GENERATE_GDML_OUTPUT_, m_fiducial_volume_phys);
#endif
} // EpicDetectorConstruction::ExportGdmlFile()

// -------------------------------------------------------------------------------------

void EpicDetectorConstruction::ImportGdmlFile(const char *fname, unsigned vmax)
{
  {
    G4GDMLParser parser;
    parser.Read(fname);

    auto phys = parser.GetWorldVolume();
    auto log  = phys->GetLogicalVolume();

    for(unsigned iq=0; iq<(vmax ? vmax : log->GetNoDaughters()); iq++) {
      auto dphys = log->GetDaughter(iq);

      auto trans = dphys->GetTranslation();
      auto rot   = dphys->GetRotation();

      new G4PVPlacement(rot, trans, dphys->GetLogicalVolume(), dphys->GetName(), m_expHall_log, false, 0);
    } //for iq
  }
} // EpicDetectorConstruction::ImportGdmlFile()

// -------------------------------------------------------------------------------------
