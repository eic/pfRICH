
#ifndef _PFRICH_CERN_DETECTOR_CONSTRUCTION_
#define _PFRICH_CERN_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

#include <IRT2/CherenkovDetector.h>
#include <IRT2/CherenkovDetectorCollection.h>

class CERNDetectorConstruction : public DetectorConstruction
{
public:
  CERNDetectorConstruction(IRT2::CherenkovDetectorCollection *geometry);
  virtual ~CERNDetectorConstruction() {};

  G4VPhysicalVolume* Construct( void );
};

#endif
