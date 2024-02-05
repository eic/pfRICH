
#ifndef _PFRICH_TSTAND_DETECTOR_CONSTRUCTION_
#define _PFRICH_TSTAND_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

class CherenkovDetector;
class CherenkovDetectorCollection;

class TestStandDetectorConstruction : public DetectorConstruction
{
public:
  TestStandDetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~TestStandDetectorConstruction() {};

  G4VPhysicalVolume* Construct( void );

private:
  //double m_gas_volume_length;
  //G4LogicalVolume *m_gas_volume_log;
  //double m_gzOffset, m_gas_volume_offset;

  //void DefinePhotonDetectors(CherenkovDetector *cdet, G4LogicalVolume *fvmother,
  //			     G4PVPlacement *gvmother, unsigned hdim);
};

#endif
