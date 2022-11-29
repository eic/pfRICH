
#ifndef _CHERENKOV_STACKING_ACTION_
#define _CHERENKOV_STACKING_ACTION_

#include "G4UserStackingAction.hh"

class CherenkovEvent;
class TTree;

class CherenkovStackingAction : public G4UserStackingAction
{
  public:
  CherenkovStackingAction(CherenkovEvent *event, TTree *tree);
  ~CherenkovStackingAction() {};

  void NewStage();
  void PrepareNewEvent();

 private:
  CherenkovEvent *m_EventPtr;
  TTree *m_Tree;
};

#endif
