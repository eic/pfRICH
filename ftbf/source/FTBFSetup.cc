
#include "G4RunManager.hh"
#include "G4PVPlacement.hh"

#include "FTBFDetectorConstruction.h"
#include "FTBFPrimaryGeneratorAction.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile) 
{
  // Detector construction
  auto construction = new FTBFDetectorConstruction(geometry);
  {
    geometry->AddNewDetector("pfRICH-1x1");
    geometry->AddNewDetector("pfRICH-2x2");

    runManager->SetUserInitialization(construction);
  }

  // User action;
  runManager->SetUserAction(new FTBFPrimaryGeneratorAction(infile));
};

// -------------------------------------------------------------------------------------
