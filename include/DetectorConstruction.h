
#ifndef _PFRICH_DETECTOR_CONSTRUCTION_
#define _PFRICH_DETECTOR_CONSTRUCTION_

#include "Materials.h"

class G4OpticalSurface;
class G4Colour;
class G4UnionSolid;
class G4PVPlacement;
class G4Tubs;
class G4VSolid;//Polyhedra;

class CherenkovDetector;
class CherenkovDetectorCollection;
class OpticalBoundary;

class DetectorConstruction : public Materials, public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~DetectorConstruction() {};

  void ConstructSDandField();

  G4VPhysicalVolume* Construct( void );

  void ExportGdmlFile( void );

private:
  CherenkovDetectorCollection *m_Geometry;

  double m_gas_volume_length, m_gas_volume_radius;
  G4LogicalVolume *m_expHall_log, *m_fiducial_volume_log, *m_gas_volume_log;
  double m_gzOffset, m_r0min, m_r0max, m_gas_volume_offset;
  OpticalBoundary *m_mboundaries[2];// = {0, 0};
  //G4Tubs *m_gas_tube;
  G4VSolid *m_gas_tube;
  G4PVPlacement *m_gas_phys, *m_fiducial_volume_phys;

  G4VSolid *G4TubsDodecagonWrapper(const char *name, double rmin, double rmax, double length);

  G4OpticalSurface *CreateLambertianMirrorSurface(const char *name, 
						  double reflectivity = 1.0, double roughness = 0.0);
  // 'vmax': limit imported volume count by hand in case of the beampipe import, otherwise
  // would have to go through a clipping exercise of the whole geometry tree;
  void ImportGdmlFile(const char *fname, unsigned vmax = 0);

  void SetColor(G4LogicalVolume *lvol, const G4Colour &color, bool visible = true, bool solid = true);

  void BuildVesselWalls( void );
  G4UnionSolid *FlangeCut(double length, double clearance);
  void DefineAerogel        (CherenkovDetector *cdet, G4UnionSolid *flange);
  void DefineAcrylic        (CherenkovDetector *cdet, G4UnionSolid *flange);
  void DefineMirrors        (CherenkovDetector *cdet, G4UnionSolid *flange);
  void DefinePhotonDetectors(CherenkovDetector *cdet);
};

#endif
