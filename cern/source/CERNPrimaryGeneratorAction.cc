#include <cmath>
#include <cstring>
#include <exception>
#include <memory>

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <cern.h>
#include "CERNrunConfig.h"
#include "CERNPrimaryGeneratorAction.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/ReaderAscii.h"

using namespace HepMC3;

namespace {

const GenParticle *FindIncidentParticle(const GenEvent &evt)
{
  const GenParticle *fallback = nullptr;

  for (const auto &p : evt.particles()) {
    if (!p) {
      continue;
    }

    if (!fallback) {
      fallback = p.get();
    }

    if (p->status() == 1) {
      return p.get();
    }
  }

  return fallback;
}

void ApplyHepMCParticleToGun(const GenParticle *hp, G4ParticleGun *gun)
{
  if (!hp || !gun) {
    return;
  }

  auto *particleTable = G4ParticleTable::GetParticleTable();
  auto *definition = particleTable->FindParticle(hp->pid());

  if (!definition) {
    G4cerr << "Warning: cannot map PDG " << hp->pid()
           << " to a Geant4 particle. Falling back to pi+." << G4endl;
    definition = particleTable->FindParticle("pi+");
  }

  gun->SetParticleDefinition(definition);

  const auto &mom = hp->momentum();
  const G4ThreeVector pvec(mom.px(), mom.py(), mom.pz());
  const double pmag = pvec.mag();

  if (pmag > 0.0) {
    gun->SetParticleMomentumDirection(pvec.unit());
    gun->SetParticleMomentum(pmag * GeV);
  } else {
    gun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, -1.0));
    gun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);
  }

  if (auto vtx = hp->production_vertex()) {
    const auto &pos = vtx->position();
    gun->SetParticlePosition(G4ThreeVector(pos.x() * mm, pos.y() * mm, pos.z() * mm));
  } else {
    gun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0));
  }

  gun->SetParticleTime(0.0 * ns);
}

} // namespace

CERNPrimaryGeneratorAction::CERNPrimaryGeneratorAction(const char *hepmc)
  : G4VUserPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);
  fParticleGun->SetParticleTime(0.0 * ns);

  if (hepmc && std::strlen(hepmc) > 0) {
    try {
      fReader = std::make_unique<ReaderAscii>(hepmc);
      fUseHepMC = (fReader && !fReader->failed());

      if (fUseHepMC) {
        G4cout << "CERNPrimaryGeneratorAction: using HepMC input file "
               << hepmc << G4endl;
      } else {
        G4cerr << "CERNPrimaryGeneratorAction: failed to open HepMC file "
               << hepmc << G4endl;
      }
    } catch (const std::exception &e) {
      G4cerr << "CERNPrimaryGeneratorAction: exception while opening HepMC file "
             << hepmc << " : " << e.what() << G4endl;
      fUseHepMC = false;
    }
  }

  if (!fUseHepMC) {
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();

    G4String primaryName = gPrimaryParticle;
    if (primaryName != "pi+" && primaryName != "kaon+") {
      G4cout << "Invalid particle '" << gPrimaryParticle.c_str()
             << "'. Falling back to pi+." << G4endl;
      primaryName = "pi+";
    }

    G4ParticleDefinition *primary = particleTable->FindParticle(primaryName.c_str());
    if (!primary) {
      G4cerr << "Failed to find fallback particle definition." << G4endl;
      return;
    }

    fParticleGun->SetParticleDefinition(primary);

    if (gPrimaryMomentumGeV > 0) {
      fParticleGun->SetParticleMomentum(gPrimaryMomentumGeV * GeV);
    } else {
      fParticleGun->SetParticleMomentum(_PRIMARY_PARTICLE_MOMENTUM_);
    }
  }
}

CERNPrimaryGeneratorAction::~CERNPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void CERNPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
  if (fUseHepMC && fReader) {
    GenEvent evt(Units::GEV, Units::MM);

    if (!fReader->read_event(evt)) {
      G4cerr << "CERNPrimaryGeneratorAction: reached end of HepMC input file." << G4endl;
      return;
    }

    const GenParticle *hp = FindIncidentParticle(evt);
    if (!hp) {
      G4cerr << "CERNPrimaryGeneratorAction: no particle found in HepMC event." << G4endl;
      return;
    }

    ApplyHepMCParticleToGun(hp, fParticleGun);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    return;
  }

  double x = _XCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _XCOORD_SIGMA_);
  double y = _YCOORD_AVERAGE_ + G4RandGauss::shoot(0.0, _YCOORD_SIGMA_);
  double sx = _XSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _XSLOPE_SIGMA_);
  double sy = _YSLOPE_AVERAGE_ + G4RandGauss::shoot(0.0, _YSLOPE_SIGMA_);

  double norm = std::sqrt(1.0 + sx * sx + sy * sy);
  fParticleGun->SetParticleMomentumDirection((1.0 / norm) * G4ThreeVector(sx, sy, -1.0));
  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, 0.0 * cm));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
