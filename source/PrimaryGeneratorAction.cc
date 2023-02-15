
#include "PrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4ParticleTable.hh"

#if defined(HEPMC3) && defined(_USE_HEPMC3_INPUT_)
#include "HepMC3/GenEvent.h"
#include "HepMC3/Data/GenEventData.h"

using namespace HepMC3;
#else
// May be pion and electron as well, see tuning.h;
static G4ParticleDefinition *pion, *kaon;
#endif

// -------------------------------------------------------------------------------------

PrimaryGeneratorAction::PrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

#if defined(HEPMC3) && defined(_USE_HEPMC3_INPUT_)
  m_hepmc_input = new ReaderAscii(_USE_HEPMC3_INPUT_);
#else
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  pion = particleTable->FindParticle(_PRIMARY_PARTICLE_TYPE_);
#ifdef _ALTERNATIVE_PARTICLE_TYPE_
  kaon = particleTable->FindParticle(_ALTERNATIVE_PARTICLE_TYPE_);
#else
  kaon = pion;
#endif
  fParticleGun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);
#endif

  fParticleGun->SetParticleTime(0.0*ns);
} // PrimaryGeneratorAction::PrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
} // PrimaryGeneratorAction::~PrimaryGeneratorAction()

// -------------------------------------------------------------------------------------

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
#if defined(HEPMC3) && defined(_USE_HEPMC3_INPUT_)
  {
    GenEvent evt(Units::GEV,Units::MM);
    
    m_hepmc_input->read_event(evt);
    
    if (m_hepmc_input->failed()) {
      m_hepmc_input->close();
      return;
    } //if
    
    {
      GenEventData data;
      evt.write_data(data);
      for(auto const &part: data.particles) {
	if (part.status != 4) {
	  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	  
	  //printf("   %8d %8d -> %f %f %f\n", part.pid, part.status, part.momentum.x(), part.momentum.y(), part.momentum.z());
	  auto particle = particleTable->FindParticle(part.pid);
	  
	  fParticleGun->SetParticleDefinition(particle);
	  // FIXME: for now assume IP;
	  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
	  
	  double px = part.momentum.x(), py = part.momentum.y(), pz = part.momentum.z();
	  double pp = sqrt(px*px + py*py + pz*pz);
	  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px/pp, py/pp, pz/pp));
	  
	  fParticleGun->SetParticleMomentum(pp*GeV);

	  // FIXME: for now assume a single particle;
	  break;
	} //if
      } //for part
    }
  }
#else
  double theta =  2*atan(exp(-_PRIMARY_PARTICLE_ETA_));
#ifdef _PRIMARY_PARTICLE_PHI_
  double phi = _PRIMARY_PARTICLE_PHI_;
#else
  double phi = UniformRand(0.0, 360.0*degree);
#endif
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), 
							   sin(theta)*sin(phi), 
							   cos(theta)));

#ifdef _PRIMARY_VERTEX_SIGMA_ 
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, G4RandGauss::shoot(0.0, _PRIMARY_VERTEX_SIGMA_)));
#else
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
#endif

  {
    static unsigned toggle;

    // Re-define every time new even if a single particle type was defined;
    fParticleGun->SetParticleDefinition((toggle++)%2 ? kaon : pion);
  }
#endif

  fParticleGun->GeneratePrimaryVertex(anEvent);
} // PrimaryGeneratorAction::GeneratePrimaries()

// -------------------------------------------------------------------------------------
