
#ifndef _PFRICH_PRIMARY_GENERATOR_
#define _PFRICH_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"

class G4Event;

#include <tuning.h>

#if defined(HEPMC3) && defined(_USE_HEPMC3_INPUT_)
#include "HepMC3/ReaderAscii.h"
//class HepMC3::ReaderAscii;
#endif

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;

#if defined(HEPMC3) && defined(_USE_HEPMC3_INPUT_)
    HepMC3::ReaderAscii *m_hepmc_input;
#endif

  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
