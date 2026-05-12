//
// Assume that HEPMC3 input is not really needed here; may want to return it back if
// ever want to create e.g. a pi/K/p mix matching CERN conditions, etc;
//

#ifndef _PFRICH_CERN_PRIMARY_GENERATOR_
#define _PFRICH_CERN_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "Randomize.hh"

#include <memory>

class G4Event;

namespace HepMC3 {
  class ReaderAscii;
}

class CERNPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  CERNPrimaryGeneratorAction(const char *hepmc);
  ~CERNPrimaryGeneratorAction();
  
  void GeneratePrimaries(G4Event*);
  
private:
  G4ParticleGun* fParticleGun;

  std::unique_ptr<HepMC3::ReaderAscii> fReader;
  bool fUseHepMC{false};
  
  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
