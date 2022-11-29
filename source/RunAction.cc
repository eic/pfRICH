
// Make this appear first!
#include "G4Timer.hh"
#include "G4Run.hh"

#include "RunAction.h"

// -------------------------------------------------------------------------------------

RunAction::RunAction(): G4UserRunAction(), fTimer(0)
{
  fTimer = new G4Timer;
} // RunAction::RunAction()

// -------------------------------------------------------------------------------------

RunAction::~RunAction()
{
  delete fTimer;
} // RunAction::~RunAction()

// -------------------------------------------------------------------------------------

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  fTimer->Start();
} // RunAction::BeginOfRunAction()

// -------------------------------------------------------------------------------------

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  fTimer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent()
         << " " << *fTimer << G4endl;
} // RunAction::EndOfRunAction()

// -------------------------------------------------------------------------------------
