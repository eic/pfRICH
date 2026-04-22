
#include "G4RunManager.hh"
#include "G4PVPlacement.hh"

#include "CERNDetectorConstruction.h"
#include "CERNPrimaryGeneratorAction.h"

#include "cern.default.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile, const G4String &AerogelTag) 
{
  // Detector construction
  auto construction = new CERNDetectorConstruction(geometry,AerogelTag);
  {
    geometry->AddNewDetector("pfRICH-1x1");
    geometry->AddNewDetector("pfRICH-2x2");

    runManager->SetUserInitialization(construction);
  }

  // User action;
  runManager->SetUserAction(new CERNPrimaryGeneratorAction(infile));
};

// -------------------------------------------------------------------------------------
