
#ifndef _PFRICH_TSTAND_PRIMARY_GENERATOR_
#define _PFRICH_TSTAND_PRIMARY_GENERATOR_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"

class G4Event;

class TestStandPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    TestStandPrimaryGeneratorAction(const char *hepmc);
    ~TestStandPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;

  double UniformRand(double from, double to) {
    return from + (to-from)*G4UniformRand();
  };
};

#endif 
