
#include "SimplePrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

#include <simple.h>

// May be pion and electron as well, see simple.h;
static G4ParticleDefinition *pion, *kaon;

// -------------------------------------------------------------------------------------

SimplePrimaryGeneratorAction::SimplePrimaryGeneratorAction(const char *hepmc)
  : G4VUserPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

  // FIXME: well, this should depend on the vertex position along the beam line?;
  fParticleGun->SetParticleTime(0.0*ns);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  pion = particleTable->FindParticle(_PRIMARY_PARTICLE_TYPE_);
#ifdef _ALTERNATIVE_PARTICLE_TYPE_
  kaon = particleTable->FindParticle(_ALTERNATIVE_PARTICLE_TYPE_);
#else
  kaon = pion;
#endif
  fParticleGun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);
} // SimplePrimaryGeneratorAction::SimplePrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

SimplePrimaryGeneratorAction::~SimplePrimaryGeneratorAction()
{
  delete fParticleGun;
} // SimplePrimaryGeneratorAction::~SimplePrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

void SimplePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  {
    double theta =  2*atan(exp(-_PRIMARY_PARTICLE_ETA_));
#ifdef _PRIMARY_PARTICLE_PHI_
    double phi = _PRIMARY_PARTICLE_PHI_;
#else
    double phi = UniformRand(0.0, 360.0*degree);
#endif
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), 
							     sin(theta)*sin(phi), 
							     cos(theta)));
    
    fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
    
    {
      static unsigned toggle;
      
      // Re-define every time new even if a single particle type was defined;
      fParticleGun->SetParticleDefinition((toggle++)%2 ? kaon : pion);
    }
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
} // SimplePrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
