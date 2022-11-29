
#ifndef _PFRICH_RUN_ACTION_
#define _PFRICH_RUN_ACTION_

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Timer;
class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

  public:
    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

  private:
    G4Timer* fTimer;
};

#endif 
