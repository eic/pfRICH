
#ifndef _PFRICH_FTBF_DETECTOR_CONSTRUCTION_
#define _PFRICH_FTBF_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

class CherenkovDetector;
class CherenkovDetectorCollection;

class FTBFDetectorConstruction : public DetectorConstruction
{
public:
  FTBFDetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~FTBFDetectorConstruction() {};

  G4VPhysicalVolume* Construct( void );
};

#endif
