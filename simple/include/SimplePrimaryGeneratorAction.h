//
// Assume that HEPMC3 input is not really needed here; 
//

#ifndef _PFRICH_SIMPLE_PRIMARY_GENERATOR_
#define _PFRICH_SIMPLE_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "Randomize.hh"

class G4Event;

class SimplePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
  SimplePrimaryGeneratorAction(const char *hepmc);
    ~SimplePrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;

  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
