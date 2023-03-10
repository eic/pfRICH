//
// export LD_LIBRARY_PATH=/home/eic/lib64:${LD_LIBRARY_PATH}
//
// root -l
//
// root [0] gSystem->AddIncludePath("-I/home/eic/include");
// root [1] .x hepmc_writer_two_particles.C+("out.hepmc", 1000)
//

#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/Print.h"

#include <iostream>
#include <random>
#include <cmath>
#include <math.h>

#include <TMath.h>
#include <TRandom.h>
#include <TDatabasePDG.h>

using namespace HepMC3;

void hepmc_writer_two_particles(const char* out_fname, int n_events)
{
  auto *DatabasePDG = new TDatabasePDG();
  auto *pion = DatabasePDG->GetParticle(211), *kaon = DatabasePDG->GetParticle(321);

  WriterAscii hepmc_output(out_fname);
  int events_parsed = 0;
  GenEvent evt(Units::GEV, Units::MM);

  // Random number generator
  TRandom *rdmn_gen = new TRandom(0x12345678);

  for (events_parsed = 0; events_parsed < n_events; events_parsed++) {
    // type 4 is beam
    GenParticlePtr p1 =
        std::make_shared<GenParticle>(FourVector(0.0, 0.0, 12.0, 12.0), 11, 4);
    GenParticlePtr p2 = std::make_shared<GenParticle>(
        FourVector(0.0, 0.0, 100.0, 100.004), 2212, 4); 

    GenVertexPtr v1 = std::make_shared<GenVertex>();
    v1->add_particle_in(p1);
    v1->add_particle_in(p2);

    for(unsigned iq=0; iq</*2*/2; iq++) {
      Double_t eta   = rdmn_gen->Uniform(-1.91, -1.90);
      Double_t th    = 2*std::atan(exp(-eta));
      Double_t p     = rdmn_gen->Uniform(7.0, 7.1);
      Double_t phi   = iq%2 ? 0.*M_PI/180. : 10.*M_PI/180.;

      Double_t px    = p * std::cos(phi) * std::sin(th);
      Double_t py    = p * std::sin(phi) * std::sin(th);
      Double_t pz    = p * std::cos(th);

      //auto particle = pion;//events_parsed%2 ? pion : kaon;
      auto particle = iq%2 ? pion : kaon;
      GenParticlePtr pq = std::make_shared<GenParticle>(FourVector(
								   px, py, pz,
								   sqrt(p*p + pow(particle->Mass(), 2))),
							particle->PdgCode(), 1);
      v1->add_particle_out(pq);
    } //for iq

    evt.add_vertex(v1);

    if (events_parsed == 0) {
      std::cout << "First event: " << std::endl;
      Print::listing(evt);
    }

    hepmc_output.write_event(evt);
    if (events_parsed % 10000 == 0) {
      std::cout << "Event: " << events_parsed << std::endl;
    }
    evt.clear();
  }
  hepmc_output.close();
  std::cout << "Events parsed and written: " << events_parsed << std::endl;
  exit(0);
}
