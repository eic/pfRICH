
#ifndef _PFRICH_EPIC_PRIMARY_GENERATOR_
#define _PFRICH_EPIC_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"

class G4Event;

//#include <tuning.h>
//#include <epic.h>

#if defined(HEPMC3) 
#include "HepMC3/ReaderAscii.h"
#endif

class EpicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    EpicPrimaryGeneratorAction(const char *hepmc);
    ~EpicPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;

#if defined(HEPMC3) 
    HepMC3::ReaderAscii *m_hepmc_input;
#endif

  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
