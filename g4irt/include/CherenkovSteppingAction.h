
#ifndef _CHERENKOV_STEPPING_ACTION_
#define _CHERENKOV_STEPPING_ACTION_

#include "G4UserSteppingAction.hh"

#include <IRT2/CherenkovEvent.h>
#include <IRT2/CherenkovDetectorCollection.h>
class G4Track;
#include <IRT2/TransientParticle.h>
#include <IRT2/CherenkovRadiator.h>
#include <IRT2/CherenkovPhotonDetector.h>

#include <TransientTrackInformation.h>

class CherenkovSteppingAction : public G4UserSteppingAction
{
 public:
  CherenkovSteppingAction(IRT2::CherenkovDetectorCollection *geometry, IRT2::CherenkovEvent *event);
  virtual ~CherenkovSteppingAction() {};
  
  // Method from the base class;
  virtual void UserSteppingAction(const G4Step*);

  TransientTrackInformation *AttachUserInfo(G4Track* track, IRT2::TransientParticle *myself, 
					    IRT2::TransientParticle *parent);

  void DisableSecondaries( void ) { m_SeconadriesDisabled = true; };

 private:
  IRT2::CherenkovEvent *m_EventPtr;
  IRT2::CherenkovDetectorCollection *m_Geometry;

  bool m_SeconadriesDisabled;

  double GetQE(const IRT2::CherenkovPhotonDetector *pd, double eph);
  double GetAttenuationLength(const IRT2::CherenkovRadiator *radiator, double eph); 
  double GetRefractiveIndex(const IRT2::CherenkovRadiator *radiator, double eph); 
};

#endif
