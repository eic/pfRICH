//
// Assume that HEPMC3 input is not really needed here; may want to return it back if
// ever want to create e.g. a pi/K/p mix matching FTBF conditions, etc;
//

#ifndef _PFRICH_FTBF_PRIMARY_GENERATOR_
#define _PFRICH_FTBF_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "Randomize.hh"

class G4Event;

class FTBFPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    FTBFPrimaryGeneratorAction(const char *hepmc);
    ~FTBFPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;

  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
