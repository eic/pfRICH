
#include <G4VUserTrackInformation.hh>

#ifndef _TRANSIENT_TRACK_INFORMATION_
#define _TRANSIENT_TRACK_INFORMATION_

#include <IRT2/TransientParticle.h>

class TransientTrackInformation: public G4VUserTrackInformation
{
public:
  TransientTrackInformation(IRT2::TransientParticle *myself, IRT2::TransientParticle *parent):
  m_Myself(myself), m_Parent(parent) {};
  ~TransientTrackInformation() {};

  void Print() const {};

  IRT2::TransientParticle *Myself( void ) const { return m_Myself; };
  IRT2::TransientParticle *Parent( void ) const { return m_Parent; };
  
 private:
  // NB: no const for the parent, since will modify its information in the stepping call;
  IRT2::TransientParticle *m_Myself, *m_Parent;
};

#endif
