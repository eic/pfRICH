//
// A simple pfRICH configuration;
//

#ifdef _STANDALONE_GEANT_CODE_
#include "G4SystemOfUnits.hh"
#endif

#ifndef _PFRICH_SIMPLE_
#define _PFRICH_SIMPLE_

#include <share.h>
#include <Materials.h>

// -- Primary particle(s) ---------------------------------------------------------------------
//
#define _PRIMARY_PARTICLE_TYPE_               ("pi+")
// Will toggle between the two types if defined;
//#define _ALTERNATIVE_PARTICLE_TYPE_        ("kaon+")

#define _PRIMARY_PARTICLE_ETA_                 (2.40)
// If commented out: uniform phi;
//#define _PRIMARY_PARTICLE_PHI_         (90*degree)

#define _PRIMARY_PARTICLE_MOMENTUM_         (7.0*GeV)
// --------------------------------------------------------------------------------------------

// -- Vessel geometry -------------------------------------------------------------------------
//
// Mimic ePIC pfRICH vessel length;
#define _FIDUCIAL_VOLUME_LENGTH_SIMPLE_    (500.0*mm)
// Choose ePIC dRICH location;
#define _FIDUCIAL_VOLUME_OFFSET_SIMPLE_   (1550.0*mm)
// This one does not really matter;
#define _FIDUCIAL_VOLUME_WIDTH_SIMPLE_    (2000.0*mm)

// FIXME: this is getting to be really obsolete; just want to be in sync with epic.default.h; 
#define _VESSEL_FRONT_SIDE_THICKNESS_        (5.0*mm)

// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
#define _BUILDING_BLOCK_CLEARANCE_           (1.0*mm)

// Can also use m_C2F6; 
#define _GAS_RADIATOR_                     m_Nitrogen
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
#define _AEROGEL_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_//_AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_
#define _AEROGEL_THICKNESS_                  (2.5*cm)
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// If _ACRYLIC_THICKNESS_ is defined, a single layer right after the aerogel is installed; 
#define _ACRYLIC_THICKNESS_                  (3.0*mm)
// --------------------------------------------------------------------------------------------

#endif
