
#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_TUNING_
#define _PFRICH_TUNING_

// -- General ---------------------------------------------------------------------------------
//
// May want to suppress secondaries;
#define _DISABLE_SECONDARIES_

// May want to disable parasitic sources of photons;
#define _DISABLE_GAS_VOLUME_PHOTONS_
#define _DISABLE_ACRYLIC_PHOTONS_
#define _DISABLE_HRPPD_WINDOW_PHOTONS_

// May want to disable Rayleight scattering and / or absorption by hand; 
//#define _DISABLE_RAYLEIGH_SCATTERING_
//#define _DISABLE_ABSORPTION_

// -- Aerogel ---------------------------------------------------------------------------------
//
// Up to two layers along the beam line; density=0 means the layer will be skipped;
//#define _AEROGEL_DENSITY_1_           (225.0*mg/cm3)
//#define _AEROGEL_DENSITY_1_                      (0)
//#define _AEROGEL_DENSITY_2_           (110.0*mg/cm3)
//#define _AEROGEL_DENSITY_2_                      (0)
// Fixed refractive index, no attenuation; works in a single-layer configuration only;
//#define _AEROGEL_REFRACTIVE_INDEX_           (1.044)

#define _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_ 0
#define _AEROGEL_BELLE_II_LARGE_REFRACTIVE_INDEX_ 1
// FIXME: this does not look nice, but suffices;
#define _AEROGEL_CLAS12_DENSITY_110_MG_CM3_     110
#define _AEROGEL_CLAS12_DENSITY_225_MG_CM3_     225

//#define _AEROGEL_1_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
#define _AEROGEL_THICKNESS_1_               (2.0*cm)
//#define _AEROGEL_2_ _AEROGEL_CLAS12_DENSITY_110_MG_CM3_
//#define _AEROGEL_2_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
#define _AEROGEL_2_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
#define _AEROGEL_THICKNESS_2_               (2.0*cm)//3.0*cm)

// Azimuthal segmentation is hardcoded, see DetectorConstruction::Construct();
#define _AEROGEL_BAND_COUNT_                     (3)

// Set to m_Nitrogen and something like <100um, for "clean" studies;
#define _AEROGEL_FRAME_WALL_THICKNESS_      (0.5*mm)
#define _AEROGEL_SPACER_MATERIAL_       (m_Nitrogen)//m_Absorber
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// A single layer right after the aerogel; density=0 means the layer will be skipped;
#define _ACRYLIC_DENSITY_                     (1.18)
//#define _ACRYLIC_THICKNESS_                 (3.0*mm)
#define _ACRYLIC_WL_CUTOFF_                 (300*nm)
// Fixed refractive index, no attenuation; 
//#define _ACRYLIC_REFRACTIVE_INDEX_            (1.50)
// --------------------------------------------------------------------------------------------

// -- Geometry --------------------------------------------------------------------------------
//
// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
#define _LAYER_TO_LAYER_GAP_                  (1*mm)

// Some "standard" value applied to all mirrors;
#define _MIRROR_REFLECTIVITY_                 (0.90)

// To the rear side of the gas volume;
#define _SENSOR_AREA_LENGTH_               (12.0*cm)

// At the downstream (sensor plane) location;
#define _CONICAL_MIRROR_INNER_RADIUS_     (110.0*mm)
#define _CONICAL_MIRROR_OUTER_RADIUS_     (550.0*mm)

// May still want to disable the conical mirror optics in IRT;
//#define _USE_CONICAL_MIRROR_OPTICS_

// Keep it simple for now; do not care about the walls;
#define _GAS_VOLUME_INNER_RADIUS_        (  70.0*mm)
#define _GAS_VOLUME_OUTER_RADIUS_        ( 635.0*mm)
// Keep all elements that far away from the gas volume boundary;
#define _FIDUCIAL_VOLUME_OFFSET_            (  1*mm)

// Given by the project;
#define _GAS_VOLUME_LENGTH_                (54.0*cm)
#define _GAS_VOLUME_OFFSET_ (1187.0*mm + _GAS_VOLUME_LENGTH_/2)
// --------------------------------------------------------------------------------------------

// -- QE and Co -------------------------------------------------------------------------------
//
// Default is to use LAPPD QE as given by Alexey;
//#define _USE_SiPM_QE_

// Some reasonably optimistic number; 
#define _SAFETY_FACTOR_                       (0.70)
// This number is 1.00 for HRPPDs (no SiPM-like fill factor or such); keep in place 
// for historical reasons;
#define _SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
// --------------------------------------------------------------------------------------------

// -- HRPPD -----------------------------------------------------------------------------------
//
// A hypothetical geometry;
#define _HRPPD_TILE_SIZE_                 (122.0*mm)
#define _HRPPD_WINDOW_THICKNESS_            (5.0*mm)
#define _HRPPD_CERAMIC_BODY_THICKNESS_      (9.0*mm)
// Area covered by the photocathode;
#define _HRPPD_ACTIVE_AREA_SIZE_          (110.0*mm)

// Gap between tiles at the sensor plane;
#define _HRPPD_INSTALLATION_GAP_          (  0.5*mm)

// These are some random numbers to the moment; are only important for time-of-flight;
#define _HRPPD_METALLIZATION_REFLECTIVITY_    (0.80)
#define _HRPPD_METALLIZATION_ROUGHNESS_       (0.05)
// --------------------------------------------------------------------------------------------


// -- Optional funneling mirrors around the HRPPD sensors ----------------------------------------------
//
//#define _USE_PYRAMIDS_
// Mirror height and width;
#define _PYRAMID_MIRROR_HEIGHT_            (30.0*mm)
#define _PYRAMID_MIRROR_APERTURE_WIDTH_    (_HRPPD_ACTIVE_AREA_SIZE_)

// May still want to disable the funneling optics in IRT;
#define _USE_PYRAMIDS_OPTICS_
// --------------------------------------------------------------------------------------------

// -- Rarely changed parameters ---------------------------------------------------------------
//
// "Nominal" wavelength at which average refractive index will be calculated; FIXME: remove;
#define _LAMBDA_NOMINAL_                     (470.0)
#define _MAGIC_CFF_                         (1239.8)

// Consider full range from 175um (fused silica "cutoff") to 800um, at most; how about MgF2?;
#define _WLDIM_                                  27
//#define _LAMBDA_MIN_                         (200.0)
#define _LAMBDA_MIN_                         (150.0)
#define _LAMBDA_MAX_                         (800.0)
#define _NU_MIN_     (eV*_MAGIC_CFF_/_LAMBDA_MAX_)
#define _NU_MAX_     (eV*_MAGIC_CFF_/_LAMBDA_MIN_)
#define _NU_STEP_    ((_NU_MAX_ - _NU_MIN_)/(_WLDIM_-1))

// Additional smearing of primary vertices along the beam line;
//#define _PRIMARY_VERTEX_SIGMA_ (10*cm)

// This needs to be verified (and actually the QE at normal incidence renormalized); 
#define _BIALKALI_REFRACTIVE_INDEX_            (2.7)
// --------------------------------------------------------------------------------------------

#endif
