
#ifndef _PFRICH_CERN_DETECTOR_CONSTRUCTION_
#define _PFRICH_CERN_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

class CherenkovDetector;
class CherenkovDetectorCollection;

class CERNDetectorConstruction : public DetectorConstruction
{
public:
  CERNDetectorConstruction(CherenkovDetectorCollection *geometry,std::string AerogelTag);
  virtual ~CERNDetectorConstruction() {};

  G4VPhysicalVolume* Construct( void );

private:
  std::string aerogelTag;
};

#endif
