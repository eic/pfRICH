
#include <vector>

#ifndef _PFRICH_EPIC_DETECTOR_CONSTRUCTION_
#define _PFRICH_EPIC_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

class G4UnionSolid;
class G4PVPlacement;

class CherenkovDetector;
class CherenkovDetectorCollection;
class OpticalBoundary;

class EpicDetectorConstruction : public DetectorConstruction
{
public:
  EpicDetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~EpicDetectorConstruction() {};

  void ConstructSDandField();

  G4VPhysicalVolume* Construct( void );

  void ExportGdmlFile( void );

private:
  G4LogicalVolume *m_expHall_log;
  double m_gas_volume_radius, m_gzOffset, m_r0min, m_r0max, m_gas_volume_offset;

  // 'vmax': limit imported volume count by hand in case of the beampipe import, otherwise
  // would have to go through a clipping exercise of the whole geometry tree;
  void ImportGdmlFile(const char *fname, unsigned vmax = 0);

  void BuildVesselWalls(DarkBox *dbox);
  G4UnionSolid *FlangeCut(double length, double clearance);
  void DefineAerogel        (CherenkovDetector *cdet, DarkBox *dbox, G4UnionSolid *flange);
  void DefineAcrylic        (CherenkovDetector *cdet, DarkBox *dbox, G4UnionSolid *flange);
  void DefineMirrors        (CherenkovDetector *cdet, DarkBox *dbox, G4VSolid *gas_tube, G4UnionSolid *flange);
};

#endif
