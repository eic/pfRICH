
#ifndef _CHERENKOV_STEPPING_ACTION_
#define _CHERENKOV_STEPPING_ACTION_

#include "G4UserSteppingAction.hh"

class CherenkovEvent;
class CherenkovDetectorCollection;
class G4Track;
class TransientParticle;
class CherenkovRadiator;
class CherenkovPhotonDetector;

#include <TransientTrackInformation.h>

class CherenkovSteppingAction : public G4UserSteppingAction
{
 public:
  CherenkovSteppingAction(CherenkovDetectorCollection *geometry, CherenkovEvent *event);
  virtual ~CherenkovSteppingAction() {};
  
  // Method from the base class;
  virtual void UserSteppingAction(const G4Step*);

  TransientTrackInformation *AttachUserInfo(G4Track* track, TransientParticle *myself, 
					    TransientParticle *parent);

  void DisableSecondaries( void ) { m_SeconadriesDisabled = true; };

 private:
  CherenkovEvent *m_EventPtr;
  CherenkovDetectorCollection *m_Geometry;

  bool m_SeconadriesDisabled;

  double GetQE(const CherenkovPhotonDetector *pd, double eph);
  double GetAttenuationLength(const CherenkovRadiator *radiator, double eph); 
  double GetRefractiveIndex(const CherenkovRadiator *radiator, double eph); 
};

#endif
