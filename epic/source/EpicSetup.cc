
#include "G4RunManager.hh"

#include "EpicDetectorConstruction.h"
#include "EpicPrimaryGeneratorAction.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile) 
{
  // Detector construction
  auto construction = new EpicDetectorConstruction(geometry);
  {
    geometry->AddNewDetector("pfRICH");

    runManager->SetUserInitialization(construction);
  }

  // User action;
  runManager->SetUserAction(new EpicPrimaryGeneratorAction(infile));
};

// -------------------------------------------------------------------------------------
