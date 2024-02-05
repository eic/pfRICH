

#include "G4Event.hh"
#include "G4ParticleTable.hh"

#include <TRandom.h>

#include <ftbf.h>

#include "FTBFPrimaryGeneratorAction.h"

// May be pion and electron as well, see tuning.h;
static G4ParticleDefinition *pion, *kaon;

// -------------------------------------------------------------------------------------

FTBFPrimaryGeneratorAction::FTBFPrimaryGeneratorAction(const char *hepmc)
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
} // FTBFPrimaryGeneratorAction::FTBFPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

FTBFPrimaryGeneratorAction::~FTBFPrimaryGeneratorAction()
{
  delete fParticleGun;
} // FTBFPrimaryGeneratorAction::~FTBFPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

void FTBFPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  double  x = _XCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _XCOORD_SIGMA_);
  double  y = _YCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _YCOORD_SIGMA_);
  double sx = _XSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _XSLOPE_SIGMA_);
  double sy = _YSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _YSLOPE_SIGMA_);
  double norm = sqrt(1.0 + sx*sx + sy*sy);

  fParticleGun->SetParticleMomentumDirection((1/norm)*G4ThreeVector(sx, sy, -1.));
    
  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, 0.0*cm));
  
  {
    static unsigned toggle;
    
    // Re-define every time new even if a single particle type was defined;
    fParticleGun->SetParticleDefinition((toggle++)%2 ? kaon : pion);
  }
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
} // FTBFPrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
