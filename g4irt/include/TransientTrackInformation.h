
#include <G4VUserTrackInformation.hh>

#ifndef _TRANSIENT_TRACK_INFORMATION_
#define _TRANSIENT_TRACK_INFORMATION_

#include <TransientParticle.h>

class TransientTrackInformation: public G4VUserTrackInformation
{
public:
 TransientTrackInformation(TransientParticle *myself, TransientParticle *parent):
  m_Myself(myself), m_Parent(parent) {};
  ~TransientTrackInformation() {};

  void Print() const {};

  TransientParticle *Myself( void ) const { return m_Myself; };
  TransientParticle *Parent( void ) const { return m_Parent; };
  
 private:
  // NB: no const for the parent, since will modify its information in the stepping call;
  TransientParticle *m_Myself, *m_Parent;
};

#endif
