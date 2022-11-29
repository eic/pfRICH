
#include "PrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4ParticleTable.hh"

#include "tuning.h"

using namespace CLHEP;

#define _PARTICLE_NUM_ 1

//static G4ParticleDefinition *pion, *kaon;

// -------------------------------------------------------------------------------------

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
   fParticleGun(0)
{
  //G4int n_particle = _PARTICLE_NUM_;
  //fParticleGun = new EicSandboxParticleGun(n_particle);
  fParticleGun = new G4ParticleGun(_PARTICLE_NUM_);//n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  //pion = particleTable->FindParticle("pi+");
  //kaon = particleTable->FindParticle("kaon+");
  G4ParticleDefinition* particle = particleTable->FindParticle("pi+");
  //G4ParticleDefinition* particle = particleTable->FindParticle("kaon+");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleTime(0.0*ns);
  //fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
  {
    //double theta = 15*degree, phi = 90.0*degree + 0*degree;
    //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), 
    //							     sin(theta)*sin(phi), 
    //							     cos(theta)));
  }
  fParticleGun->SetParticleMomentum(7.0*GeV);
} // PrimaryGeneratorAction::PrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
} // PrimaryGeneratorAction::~PrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

//void EicSandboxParticleGun::GeneratePrimaryVertex(G4Event* evt)
//{
//G4ParticleGun::GeneratePrimaryVertex(evt);
//} // EicSandboxParticleGun::GeneratePrimaryVertex()

// -------------------------------------------------------------------------------------

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  // eta: 1.5, 1.8, 2.0, 2.5, 3.0, 3.5;
  //double theta = 25.157*degree, phi = 90.0*degree;
  //double theta = 18.772*degree, phi = 90.0*degree;
  //double theta = 15.415*degree, phi = 90.0*degree;
  double theta =  9.385*degree, phi = 92.0*degree;
  //double theta =  5.700*degree, phi = 90.0*degree;
  //double theta =  3.459*degree, phi = 95.0*degree;
  // eta: 1.6, 3.3, 3.4
  //double theta = 22.829*degree, phi = 90.0*degree;
  //double theta = 4.225*degree, phi = 95.0*degree;
  //double theta =  3.823*degree, phi = 95.0*degree;
  //
  //50% double theta = 20.000*degree, phi = 90.0*degree;
  //double theta = 20.000*degree, phi = 95.0*degree;
  //double theta = 15.000*degree, phi = 75*degree;
  //double theta = 26.000*degree, phi = 90.0*degree;

  //phi = UniformRand(0.0, 360.0*degree);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), 
							   sin(theta)*sin(phi), 
							   cos(theta)));

#ifdef _PRIMARY_VERTEX_SIGMA_ 
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, G4RandGauss::shoot(0.0, _PRIMARY_VERTEX_SIGMA_)));
#else
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
#endif

  {
    //static unsigned toggle;
    //fParticleGun->SetParticleDefinition((toggle++)%2 ? pion : kaon);
  }

  fParticleGun->GeneratePrimaryVertex(anEvent);
} // PrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
