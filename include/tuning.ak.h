//
// Tuneable portion of the pfRICH parameters as of 02/2023;
//

#include <fixed.h>

#ifndef _PFRICH_TUNING_
#define _PFRICH_TUNING_

// -- Primary particle(s) ---------------------------------------------------------------------
//
// Ignore pfrich.mac altogether; this default can also be overriden by '-s' cmd line switch;
#define _STATISTICS_DEFAULT_                  (1000)

// Optional smearing of primary vertices along the beam line; applies to HEPMC3 input as well;
// 10cm (proton bunch size) is a reasonable value;
#define _PRIMARY_VERTEX_SIGMA_               (10*cm)

//
// All this can be overriden by '-i' cmd line switch (HEPMC3 input);
//
#define _PRIMARY_PARTICLE_TYPE_              ("pi+")
// Will toggle between the two types if defined;
//#define _ALTERNATIVE_PARTICLE_TYPE_        ("kaon+")

//#define _PRIMARY_PARTICLE_ETA_            (sign*2.07)
#define _PRIMARY_PARTICLE_ETA_            (sign*2.5)
// Uniform phi, if undefined;
//#define _PRIMARY_PARTICLE_PHI_         (95*degree)//110.0*degree)
#define _PRIMARY_PARTICLE_MOMENTUM_       (10.0*GeV)
// --------------------------------------------------------------------------------------------

#endif
