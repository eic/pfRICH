
#ifndef _PFRICH_SIMPLE_DETECTOR_CONSTRUCTION_
#define _PFRICH_SIMPLE_DETECTOR_CONSTRUCTION_

#include "DetectorConstruction.h"

class CherenkovDetector;
class CherenkovDetectorCollection;

class SimpleDetectorConstruction : public DetectorConstruction
{
public:
  SimpleDetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~SimpleDetectorConstruction() {};

  void BuildFakePhotonDetectorMatrix(CherenkovDetector *cdet, DarkBox *dbox, 
				     double fvzOffset, double wzOffset);
  G4LogicalVolume *BuildFakeHRPPD(G4LogicalVolume *wnd_log, G4Box *pd_box, CherenkovPhotonDetector *pd);
    
  G4VPhysicalVolume* Construct( void );
};

#endif
