//
// Tuneable portion of the pfRICH parameters as of 02/2023;
//

#include <fixed.h>

#ifndef _PFRICH_TUNING_
#define _PFRICH_TUNING_

static const bool flip = true;
static const double sign = flip ? -1.0 : 1.0;

// -- General ---------------------------------------------------------------------------------
//
// May want to suppress secondaries;
#define _DISABLE_SECONDARIES_

// Comment out if want to run without MARCO magnetic field;
#define _USE_MAGNETIC_FIELD_ "./database/MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt"

// May want to disable parasitic sources of photons during G4 stepping;
//#define _DISABLE_GAS_VOLUME_PHOTONS_
//#define _DISABLE_ACRYLIC_PHOTONS_
//#define _DISABLE_HRPPD_WINDOW_PHOTONS_

// May want to disable Rayleigh scattering and / or absorption by hand; 
//#define _DISABLE_RAYLEIGH_SCATTERING_
//#define _DISABLE_ABSORPTION_

// Call /geometry/test/run if uncommented;
#define _GEOMETRY_CHECK_
//#define _GENERATE_GDML_OUTPUT_
// --------------------------------------------------------------------------------------------

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
#define _PRIMARY_PARTICLE_ETA_            (sign*2.0)
// Uniform phi, if undefined;
//#define _PRIMARY_PARTICLE_PHI_         (95*degree)//110.0*degree)
#define _PRIMARY_PARTICLE_MOMENTUM_        (10.0*GeV)
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
//#define _AEROGEL_1_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
#define _AEROGEL_1_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
#define _AEROGEL_THICKNESS_1_               (2.0*cm)
//#define _AEROGEL_2_ _AEROGEL_CLAS12_DENSITY_155_MG_CM3_
//#define _AEROGEL_THICKNESS_2_               (2.0*cm)

// Set to m_Nitrogen and something like <100um thickness, for "clean" studies only;
#define _AEROGEL_SPACER_MATERIAL_       (m_Absorber)

// If uncommented: fixed refractive index, no attenuation (single-layer CLAS12 config only);
//#define _AEROGEL_REFRACTIVE_INDEX_           (1.044)
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// If _ACRYLIC_THICKNESS_ is defined, a single layer right after the aerogel is installed; 
#define _ACRYLIC_THICKNESS_                 (3.0*mm)
#define _ACRYLIC_WL_CUTOFF_                 (300*nm)

// If uncommented: fixed refractive index, no attenuation; 
//#define _ACRYLIC_REFRACTIVE_INDEX_            (1.50)
// --------------------------------------------------------------------------------------------

// -- QE and Co -------------------------------------------------------------------------------
//
// Default is to use LAPPD QE as given by Alexey;
//#define _USE_SiPM_QE_

// Some reasonably optimistic number; assume that it includes an unknown 
// HRPPD Collection Efficiency (CE) as well;
#define _SAFETY_FACTOR_                       (0.70)
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// Some "standard" value applied to all mirrors;
#define _MIRROR_REFLECTIVITY_                 (0.90)

// If uncommented, optional funneling mirrors around the HRPPD sensors are installed;
//#define _USE_PYRAMIDS_
// Mirror height and width;
//#define _PYRAMID_MIRROR_HEIGHT_            (30.0*mm)
//#define _PYRAMID_MIRROR_APERTURE_WIDTH_    (_HRPPD_ACTIVE_AREA_SIZE_)

// May still want to disable the funneling optics in IRT;
//#define _USE_PYRAMIDS_OPTICS_

// At the downstream (sensor plane) location; upstream radii are calculated automatically;
#define _CONICAL_MIRROR_INNER_RADIUS_     (120.0*mm)
#define _CONICAL_MIRROR_OUTER_RADIUS_     (540.0*mm)

// May still want to disable the conical mirror optics in IRT;
#define _USE_CONICAL_MIRROR_OPTICS_
// --------------------------------------------------------------------------------------------

#endif
