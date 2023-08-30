
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
  // At most 1+4*2 command line arguments;
  if ( argc > 9 ) {
    PrintUsage();
    return 1;
  } //if

  // Parse them;
  G4String macro;
  G4String session;

  // Default is to use tuning.h hardcoded values;
  const char *infile = 0;
  // These two may have default values;
  unsigned stat = _STATISTICS_DEFAULT_;
  G4String outfile = "pfrich.root";

  G4long myseed = 345354;
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro   =      argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session =      argv[i+1];
#if defined(HEPMC3) 
    else if ( G4String(argv[i]) == "-i" ) infile  =      argv[i+1];
#endif
    else if ( G4String(argv[i]) == "-o" ) outfile =      argv[i+1];
    else if ( G4String(argv[i]) == "-r" ) myseed  = atoi(argv[i+1]);
    else if ( G4String(argv[i]) == "-s" ) stat    = atoi(argv[i+1]);
    else {
      PrintUsage();
      return 1;
    } //if
  }

  auto outputFile = TFile::Open(outfile, "RECREATE");

  auto geometry = new CherenkovDetectorCollection();
  auto event    = new CherenkovEvent();

  auto TT = new TTree("t", "My tree");
  TT->Branch("e", "CherenkovEvent", &event, 16000, 2);

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // Seed the random number generator manually
  G4Random::setTheSeed(myseed);

  // Set mandatory initialization classes
  //
  // Detector construction
  auto construction = new DetectorConstruction(geometry);
  {
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
  runManager->SetUserAction(new PrimaryGeneratorAction(infile));
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
   
  // As of 2023/02/22 "-m" is used for interactive mode; in batch mode pfrich.mac
  // is ignored; it really makes no sense to hardcode everything in tuning.h, and
  // then leave the requested statistics parameter alone in the macro;
  if ( macro.size() ) {
    // Presumably interactive mode with macro/vis.mac;
    G4UIExecutive * ui = new G4UIExecutive(argc,argv,session);
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);

#ifdef _GEOMETRY_CHECK_
     UImanager->ApplyCommand("/geometry/test/run");
#endif

     ui->SessionStart();
     delete ui;
  } else {
    // Batch mode;
    TString cmd; cmd.Form("/run/beamOn %d", stat);
    UImanager->ApplyCommand(cmd.Data());
  }

#ifdef _GENERATE_GDML_OUTPUT_
  construction->ExportGdmlFile();
#endif

  // Job termination
  delete visManager;
  delete runManager;

  geometry->Write();
  TT->Write();
  outputFile->Close();

  return 0;
} // main() 

// -------------------------------------------------------------------------------------
