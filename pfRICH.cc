
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

#define _GEANT_SOURCE_CODE_
#include "DetectorConstruction.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "CherenkovEvent.h"
#include "CherenkovSteppingAction.h"
#include "CherenkovStackingAction.h"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <tuning.h>

// This is indeed a cut'n'paste from one of the GEANT examples (as well as 
// quite the rest of the stuff in this file); let it be;
namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " pfrich [-m macro ] [-u UIsession] [-r seed] " << G4endl;
  }
}

#include <TFile.h>
#include <TTree.h>

#include <CherenkovRadiator.h>
#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  auto outputFile = TFile::Open("pfrich.root", "RECREATE");

  auto geometry = new CherenkovDetectorCollection();
  auto event    = new CherenkovEvent();

  auto TT = new TTree("t", "My tree");
  TT->Branch("e", "CherenkovEvent", &event, 16000, 2);

  // At most 1+4*2 command line arguments;
  if ( argc > 9 ) {
    PrintUsage();
    return 1;
  } //if

  // Parse them;
  G4String macro;
  G4String session;

  G4long myseed = 345354;
  for ( G4int i=1; i<argc; i=i+2 ) {
     if      ( G4String(argv[i]) == "-m" ) macro   = argv[i+1];
     else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
     else if ( G4String(argv[i]) == "-r" ) myseed  = atoi(argv[i+1]);
    else {
      PrintUsage();
      return 1;
    }
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // Seed the random number generator manually
  G4Random::setTheSeed(myseed);

  // Set mandatory initialization classes
  //
  // Detector construction
  {
    auto construction = new DetectorConstruction(geometry);
    geometry->AddNewDetector("pfRICH");

    runManager->SetUserInitialization(construction);
  }
  // Physics list
  {
    G4VModularPhysicsList* physicsList = new FTFP_BERT;
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);
  }

  // User action initialization
  runManager->SetUserAction(new PrimaryGeneratorAction());
  runManager->SetUserAction(new RunAction());
  {
    auto stepping = new CherenkovSteppingAction(geometry, event);
#ifdef _DISABLE_SECONDARIES_
    stepping->DisableSecondaries();
#endif
    runManager->SetUserAction(stepping);
  }
  runManager->SetUserAction(new CherenkovStackingAction(event, TT));

  // Initialize G4 kernel
  runManager->Initialize();

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
   
  if ( macro.size() ) {
     // Batch mode
     G4String command = "/control/execute ";
     UImanager->ApplyCommand(command+macro);
  }
  else // Define UI session for interactive mode
  {
     G4UIExecutive * ui = new G4UIExecutive(argc,argv,session);
     UImanager->ApplyCommand("/control/execute ./macro/vis.mac");
     ui->SessionStart();
     delete ui;
  }

  // Job termination
  delete visManager;
  delete runManager;

  geometry->Write();
  TT->Write();
  outputFile->Close();

  return 0;
} // main() 

// -------------------------------------------------------------------------------------
