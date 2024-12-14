
#include "G4RunManager.hh"
#include "G4PVPlacement.hh"

#include "SimpleDetectorConstruction.h"
#include "SimplePrimaryGeneratorAction.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile) 
{
  // Detector construction
  auto construction = new SimpleDetectorConstruction(geometry);
  {
    geometry->AddNewDetector("pfRICH-Simple");

    runManager->SetUserInitialization(construction);
  }

  // User action;
  runManager->SetUserAction(new SimplePrimaryGeneratorAction(infile));
};

// -------------------------------------------------------------------------------------
