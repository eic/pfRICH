
#ifndef _CHERENKOV_STACKING_ACTION_
#define _CHERENKOV_STACKING_ACTION_

#include "G4UserStackingAction.hh"

#include <IRT2/CherenkovEvent.h>
class TTree;

class CherenkovStackingAction : public G4UserStackingAction
{
  public:
  CherenkovStackingAction(IRT2::CherenkovEvent *event, TTree *tree);
  ~CherenkovStackingAction() {};

  void NewStage();
  void PrepareNewEvent();

 private:
  IRT2::CherenkovEvent *m_EventPtr;
  TTree *m_Tree;
};

#endif
