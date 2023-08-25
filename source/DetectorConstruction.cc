
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4GDMLParser.hh>

#include <G4VSensitiveDetector.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>

#ifdef _POLYHEDRA_VESSEL_SEGMENTATION_
#include <G4Polyhedra.hh>
#endif

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <tuning.h>

#include "DetectorConstruction.h"

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

DetectorConstruction::DetectorConstruction(CherenkovDetectorCollection *geometry): 
G4VUserDetectorConstruction(), 
m_Geometry(geometry), m_gas_volume_length(0.0), m_gas_volume_radius(0.0), 
  m_expHall_log(0), m_fiducial_volume_log(0), m_gas_volume_log(0), m_gzOffset(0.0), 
  m_r0min(0.0), m_r0max(0.0), m_gas_volume_offset(.0), m_gas_tube(0), m_gas_phys(0), m_fiducial_volume_phys(0)
{
  // Optical boundaries of the inner and outer conical mirrors; do not want to spoil the 
  // central area (mirro-less) optical path (would happen if define them in situ);
  m_mboundaries[0] = m_mboundaries[1];
} // DetectorConstruction::DetectorConstruction()

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

void DetectorConstruction::ConstructSDandField()
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
} // DetectorConstruction::ConstructSDandField()

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

G4UnionSolid *DetectorConstruction::FlangeCut(double length, double clearance)
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
} // DetectorConstruction::FlangeCut()

// -------------------------------------------------------------------------------------

G4VSolid *DetectorConstruction::G4TubsDodecagonWrapper(const char *name, double rmin, double rmax, double length)
{
#ifdef _POLYHEDRA_VESSEL_SEGMENTATION_
  {
    double alfa = 2*M_PI / _POLYHEDRA_VESSEL_SEGMENTATION_, Rmin = rmin / cos(alfa/2), Rmax = rmax / cos(alfa/2);
    double ri[2] = {Rmin, Rmin}, ro[2] = {Rmax, Rmax}, z[2] = {-length/2, length/2};

    return new G4Polyhedra(name, 180.*deg/_POLYHEDRA_VESSEL_SEGMENTATION_, 360.*deg, 
			   _POLYHEDRA_VESSEL_SEGMENTATION_, 2, z, ri, ro);
  }
#else
  return new G4Tubs(name, rmin, rmax, length/2, 0*degree, 360*degree);
#endif
} // DetectorConstruction::G4TubsDodecagonWrapper()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *DetectorConstruction::Construct( void )
{
  // Chemical elements and materials;
  DefineElements();
  DefineMaterials();

  // The experimental hall; FIXME: hardcoded;
  G4Tubs *expHall_box = new G4Tubs("PFRICH_World",  0*cm, 400*cm/2, 1200*cm/2, 0*degree, 360*degree);
  m_expHall_log = new G4LogicalVolume(expHall_box, m_Air, "PFRICH_World", 0, 0, 0);
  G4VPhysicalVolume* expHall_phys = new G4PVPlacement(0, G4ThreeVector(), m_expHall_log, "PFRICH_World", 0, false, 0);
    
#ifndef _DISABLE_PFRICH_GEOMETRY_
  auto *det = m_Geometry->GetDetector("pfRICH");
  //det->SetReadoutCellMask(~0x0);
  det->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);

  // Fiducial volume (air); has to be called "PFRICH";
  auto *fiducial_volume_tube = 
    G4TubsDodecagonWrapper("PFRICH", 0.0, _VESSEL_OUTER_RADIUS_, _FIDUCIAL_VOLUME_LENGTH_);
  auto *fiducial_volume_shape = new G4SubtractionSolid("PFRICH", fiducial_volume_tube, 
  						       FlangeCut(_FIDUCIAL_VOLUME_LENGTH_ + 1*mm, _FLANGE_CLEARANCE_), 
  						       0, G4ThreeVector(0.0, 0.0, 0.0));
  m_fiducial_volume_log = new G4LogicalVolume(fiducial_volume_shape, m_Air,  "PFRICH", 0, 0, 0);
  // All volumes are defined assuming EIC h-going endcap orientation (dRICH case was developed this way 
  // for ATHENA); therefore need to rotate by 180 degrees around Y axis;
  G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(flip ? 180*degree : 0));
  //#ifdef _GENERATE_GDML_OUTPUT_
  m_fiducial_volume_phys = 
    //#endif
    new G4PVPlacement(rY, G4ThreeVector(0.0, 0.0, sign*_FIDUCIAL_VOLUME_OFFSET_), m_fiducial_volume_log, 
		      "PFRICH", expHall_phys->GetLogicalVolume(), false, 0);
    
  // Gas container volume;
  m_gas_volume_length = _FIDUCIAL_VOLUME_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_ - _SENSOR_AREA_LENGTH_;
  m_gas_volume_offset = -(_SENSOR_AREA_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_)/2;
  m_gas_volume_radius = _VESSEL_OUTER_RADIUS_ - _VESSEL_OUTER_WALL_THICKNESS_;
  m_gas_tube = G4TubsDodecagonWrapper("GasVolume", 0.0, m_gas_volume_radius, m_gas_volume_length);
  auto *gas_shape = new G4SubtractionSolid("GasVolume", m_gas_tube, 
					   // Yes, account for vessel inner wall thickness;
					   FlangeCut(m_gas_volume_length + 1*mm, _FLANGE_CLEARANCE_),
					   0, G4ThreeVector(0.0, 0.0, 0.0));
  m_gas_volume_log = new G4LogicalVolume(gas_shape, _GAS_RADIATOR_,  "GasVolume", 0, 0, 0);
  m_gas_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, m_gas_volume_offset), 
				 m_gas_volume_log, "GasVolume", m_fiducial_volume_log, false, 0);

#if 1//_MBUDGET_
  BuildVesselWalls();
#endif

  {
    // FIXME: Z-location does not really matter here, right?;
    auto boundary = new FlatSurface(TVector3(0,0,0), sign*TVector3(1,0,0), TVector3(0,-1,0));

    m_Geometry->SetContainerVolume(det, "GasVolume", 0, m_gas_volume_log, _GAS_RADIATOR_, boundary)
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }

  // To be used in boolean operations in several places;
  auto flange = FlangeCut(m_gas_volume_length + 1*mm, _FLANGE_CLEARANCE_ + _VESSEL_INNER_WALL_THICKNESS_ + 
			  _BUILDING_BLOCK_CLEARANCE_);

  // A running variable;
  m_gzOffset = -m_gas_volume_length/2 + _BUILDING_BLOCK_CLEARANCE_;
  
  // min/max radii available for aerogel, mirrors and such; r0min can be an estimate, since a flange
  // cut will be applied at the end to all these shapes anyway;
  m_r0min = _FLANGE_EPIPE_DIAMETER_/2 + _FLANGE_CLEARANCE_ + 
    _VESSEL_INNER_WALL_THICKNESS_ + _BUILDING_BLOCK_CLEARANCE_;
  m_r0max = m_gas_volume_radius - _BUILDING_BLOCK_CLEARANCE_;
  //-m_r0max = m_gas_volume_radius - 100.0;//_BUILDING_BLOCK_CLEARANCE_;
  
  // Aerogel;
  DefineAerogel(det, flange);
  
  // Acrylic;
#if 1//_MBUDGET_
#ifdef _ACRYLIC_THICKNESS_
  DefineAcrylic(det, flange);
#endif
#endif
  
#if 1//_MBUDGET_
  DefineMirrors(det, flange);
#endif
  
  // Photon detectors; 
  DefinePhotonDetectors(det);
#endif

#ifdef _IMPORT_TRACKER_GDML_FILE_ 
  ImportGdmlFile(_IMPORT_TRACKER_GDML_FILE_);
#endif
#ifdef _IMPORT_BEAMPIPE_GDML_FILE_ 
  // FIXME: '12' hardcoded;
  ImportGdmlFile(_IMPORT_BEAMPIPE_GDML_FILE_, 12);
#endif

  return expHall_phys;
} // DetectorConstruction::Construct()

// -------------------------------------------------------------------------------------

void DetectorConstruction::ExportGdmlFile( void )
{
#ifdef _GENERATE_GDML_OUTPUT_
  G4GDMLParser parser;
  
  unlink(_GENERATE_GDML_OUTPUT_);
  //parser.Write(_GENERATE_GDML_OUTPUT_, expHall_phys);
  //printf("@@@ parser.Write()\n");
  parser.Write(_GENERATE_GDML_OUTPUT_, m_fiducial_volume_phys);
#endif
} // DetectorConstruction::ExportGdmlFile()

// -------------------------------------------------------------------------------------

void DetectorConstruction::ImportGdmlFile(const char *fname, unsigned vmax)
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
} // DetectorConstruction::ImportGdmlFile()

// -------------------------------------------------------------------------------------

void DetectorConstruction::SetColor(G4LogicalVolume *lvol, const G4Colour &color, 
				    bool visible, bool solid)
{
  G4VisAttributes* visAtt = new G4VisAttributes(color);

  visAtt->SetVisibility(visible);
  visAtt->SetForceSolid(solid);
	
  lvol->SetVisAttributes(visAtt);
} // DetectorConstruction::SetColor()

// -------------------------------------------------------------------------------------
