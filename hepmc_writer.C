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
#include <TRandom3.h>
#include <TDatabasePDG.h>

using namespace HepMC3;

void hepmc_writer(const char *out_fname, int part, Double_t thMin, Double_t thMax, Double_t pMin, Double_t pMax, int n_events)
{
  auto *DatabasePDG = new TDatabasePDG();
  auto *electron = DatabasePDG->GetParticle(11);
  auto *pion = DatabasePDG->GetParticle(211); //
  auto *kaon = DatabasePDG->GetParticle(321); 
  auto *proton = DatabasePDG->GetParticle(2212);
  //const char * out_fname= "out.hepmc";
  WriterAscii hepmc_output(out_fname);
  int events_parsed = 0;
  GenEvent evt(Units::GEV, Units::MM);

  // Random number generator
  //TRandom *rdmn_gen = new TRandom(0x12345678);
  TRandom3 *rdmn_gen = new TRandom3(0);

  for (events_parsed = 0; events_parsed < n_events; events_parsed++) {
    // type 4 is beam
    GenParticlePtr p1 =
        std::make_shared<GenParticle>(FourVector(0.0, 0.0, 10.0, 10.0), 11, 4);
    GenParticlePtr p2 = std::make_shared<GenParticle>(
        FourVector(0.0, 0.0, 100.0, 100.004), 2212, 4); 

    GenVertexPtr v1 = std::make_shared<GenVertex>();
    v1->add_particle_in(p1);
    v1->add_particle_in(p2);

    // Set Kinematics
    //Double_t eta   = rdmn_gen->Uniform(etaMin, etaMax);
    //Double_t th    = 2*std::atan(exp(-eta));
    Double_t th    = rdmn_gen->Uniform(thMin, thMax);
    Double_t p     = rdmn_gen->Uniform(pMin, pMax);
    Double_t phi   = rdmn_gen->Uniform(0.0*M_PI/180.0, 360.0*M_PI/180.);
    //Double_t phi   = rdmn_gen->Uniform(85.0*M_PI/180.0, 95.0*M_PI/180.);

    Double_t px    = p * std::cos(phi) * std::sin(th);
    Double_t py    = p * std::sin(phi) * std::sin(th);
    Double_t pz    = p * std::cos(th);

    /*
    if(events_parsed < 50)
      {
	cout << events_parsed << " " << p << " " << eta << " " << th << " " << phi << " " << px << " " << py << " " << pz << endl;
      }
    */

    TParticlePDG *particle;
    switch(part)
      {
      case 0:
	particle = electron;
	break;
      case 1:
	particle = pion;
	break;
      case 2:
	particle = kaon;
	break;
      case 3:
	particle = proton;
	break;
      default:
	cout << "Invalid Particle Selection - Default to Pion" << endl;
	particle = pion;
	break;
      }

    //auto particle = pion;
    //if(part == 0) particle = electron;
    //if(part == 1) particle = pion;
    //if(part == 2) particle = kaon;
    //if(part == 3) particle = proton;

    GenParticlePtr pq = std::make_shared<GenParticle>(FourVector(
								 px, py, pz,
								 sqrt(p*p + pow(particle->Mass(), 2))),
						      particle->PdgCode(), 1);
    v1->add_particle_out(pq);

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
