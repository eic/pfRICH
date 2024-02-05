
#include "G4RunManager.hh"
#include "G4PVPlacement.hh"

#include "TestStandDetectorConstruction.h"
#include "TestStandPrimaryGeneratorAction.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile) 
{
  // Detector construction
  auto construction = new TestStandDetectorConstruction(geometry);
  {
    geometry->AddNewDetector("pfRICH-1x1");

    runManager->SetUserInitialization(construction);
  }

  // User action;
  runManager->SetUserAction(new TestStandPrimaryGeneratorAction(infile));
};

// -------------------------------------------------------------------------------------
