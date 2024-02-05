
#include "TestStandPrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4ParticleTable.hh"

#include <tstand.h>

static G4ParticleDefinition *photon;

// -------------------------------------------------------------------------------------

TestStandPrimaryGeneratorAction::TestStandPrimaryGeneratorAction(const char *hepmc)
  : G4VUserPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

  // FIXME: well, this should depend on the vertex position along the beam line?;
  fParticleGun->SetParticleTime(0.0*ns);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  photon = particleTable->FindParticle("opticalphoton");
  fParticleGun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);
} // TestStandPrimaryGeneratorAction::TestStandPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

TestStandPrimaryGeneratorAction::~TestStandPrimaryGeneratorAction()
{
  delete fParticleGun;
} // TestStandPrimaryGeneratorAction::~TestStandPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

void TestStandPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  double dx, dy;

  for( ; ; ) {
    double radius = _SOURCE_SPOT_DIAMETER_/2;

    dx = UniformRand(-radius, radius);
    dy = UniformRand(-radius, radius);
  
    if (sqrt(dx*dx+dy*dy) <= radius) break;
  } //for inf

  double sx = _XSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _XSLOPE_SIGMA_);
  double sy = _YSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _YSLOPE_SIGMA_);
  double norm = sqrt(1.0 + sx*sx + sy*sy);

  fParticleGun->SetParticleMomentumDirection((1/norm)*G4ThreeVector(sx, sy, -1.));

  fParticleGun->SetParticlePosition(G4ThreeVector(_XCOORD_AVERAGE_ + dx, _YCOORD_AVERAGE_ + dy, _ZCOORD_));
  
  fParticleGun->SetParticleDefinition(photon);
  fParticleGun->GeneratePrimaryVertex(anEvent);
} // TestStandPrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
