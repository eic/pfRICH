
#include <string>

#include "G4Event.hh"
#include "G4ParticleTable.hh"

#include <TRandom.h>

#include <cern.h>

#include "CERNrunConfig.h"

#include "CERNPrimaryGeneratorAction.h"

// May be pion and electron as well, see tuning.h;
static G4ParticleDefinition *primary;//, *pion, *kaon;

// -------------------------------------------------------------------------------------

CERNPrimaryGeneratorAction::CERNPrimaryGeneratorAction(const char *hepmc)
  : G4VUserPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

  // FIXME: well, this should depend on the vertex position along the beam line?;
  fParticleGun->SetParticleTime(0.0*ns);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  
  if (gPrimaryParticle == "pi+" || gPrimaryParticle == "kaon+") {
    primary = particleTable->FindParticle(gPrimaryParticle.c_str());
  }
  else {
    G4cout << "Invalid particle '" << gPrimaryParticle.c_str()
           << "'. Falling back to pi+." << G4endl;
    primary = particleTable->FindParticle("pi+");
  }
  
  if (!primary) {
    G4cerr << "Failed to find particle definition." << G4endl;
    return;
  }
  
  if (gPrimaryMomentumGeV>0) fParticleGun->SetParticleMomentum(gPrimaryMomentumGeV * GeV);
  else fParticleGun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);                           //default momentum

} // CERNPrimaryGeneratorAction::CERNPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

CERNPrimaryGeneratorAction::~CERNPrimaryGeneratorAction()
{
  delete fParticleGun;
} // CERNPrimaryGeneratorAction::~CERNPrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

void CERNPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  double  x = _XCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _XCOORD_SIGMA_);
  double  y = _YCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _YCOORD_SIGMA_);
  double sx = _XSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _XSLOPE_SIGMA_);
  double sy = _YSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _YSLOPE_SIGMA_);
  double norm = sqrt(1.0 + sx*sx + sy*sy);

  fParticleGun->SetParticleMomentumDirection((1/norm)*G4ThreeVector(sx, sy, -1.));
    
  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, 0.0*cm));
  
  {
    //static unsigned toggle;
    
    // Re-define every time new even if a single particle type was defined;
    G4cout << "Using particle: " << primary->GetParticleName() << G4endl;
    fParticleGun->SetParticleDefinition(primary);
    
    std::cout<<"particle definition: "<<fParticleGun->GetParticleDefinition()->GetParticleName().c_str()<<std::endl;
  }
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
} // CERNPrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
