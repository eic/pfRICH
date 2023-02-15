
#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_TUNING_
#define _PFRICH_TUNING_

// -- General ---------------------------------------------------------------------------------
//
// May want to suppress secondaries;
#define _DISABLE_SECONDARIES_

// Comment out if want to run without MARCO magnetic field;
//#define _USE_MAGNETIC_FIELD_ "./MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt"

// May want to disable parasitic sources of photons;
#define _DISABLE_GAS_VOLUME_PHOTONS_
#define _DISABLE_ACRYLIC_PHOTONS_
#define _DISABLE_HRPPD_WINDOW_PHOTONS_

// May want to disable Rayleight scattering and / or absorption by hand; 
//#define _DISABLE_RAYLEIGH_SCATTERING_
//#define _DISABLE_ABSORPTION_
// --------------------------------------------------------------------------------------------

// -- Primary particle ------------------------------------------------------------------------
//

// HepMC3 input; overrides other settings in this section;
//#define _USE_HEPMC3_INPUT_ "./out.hepmc"

#define _PRIMARY_PARTICLE_TYPE_              ("pi+")
// Will toggle between the two types if defined;
//#define _ALTERNATIVE_PARTICLE_TYPE_        ("kaon+")

#define _PRIMARY_PARTICLE_ETA_                 (2.5)
// Uniform phi, if undefined;
//#define _PRIMARY_PARTICLE_PHI_         (92.0*degree)
#define _PRIMARY_PARTICLE_MOMENTUM_        (7.0*GeV)

// Optional smearing of primary vertices along the beam line; 10cm (proton bunch size) is a reasonable value;
//#define _PRIMARY_VERTEX_SIGMA_               (10*cm)
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
// Up to two layers along the beam line; 
#define _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_ 0
#define _AEROGEL_BELLE_II_LARGE_REFRACTIVE_INDEX_ 1
// FIXME: this does not look nice, but suffices;
#define _AEROGEL_CLAS12_DENSITY_110_MG_CM3_     110
#define _AEROGEL_CLAS12_DENSITY_225_MG_CM3_     225

// If _AEROGEL_1_ is undefined, this layer will be skipped;
#define _AEROGEL_1_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
//#define _AEROGEL_1_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
#define _AEROGEL_THICKNESS_1_               (2.0*cm)
// If _AEROGEL_2_ is undefined, this layer will be skipped;
//#define _AEROGEL_2_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
//#define _AEROGEL_THICKNESS_2_               (2.0*cm)

// Fixed refractive index, no attenuation; works in a single-layer CLAS12 configuration only;
//#define _AEROGEL_REFRACTIVE_INDEX_           (1.046)

// Azimuthal segmentation is hardcoded, see DetectorConstruction::Construct();
#define _AEROGEL_BAND_COUNT_                     (3)

// Set to m_Nitrogen and something like <100um thickness, for "clean" studies;
#define _AEROGEL_FRAME_WALL_THICKNESS_      (0.5*mm)
#define _AEROGEL_SPACER_MATERIAL_       (m_Nitrogen)//m_Absorber
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// If _ACRYLIC_THICKNESS_ is defined, a single layer right after the aerogel is installed; 
//#define _ACRYLIC_THICKNESS_                 (3.0*mm)

#define _ACRYLIC_WL_CUTOFF_                 (300*nm)
// Fixed refractive index, no attenuation; 
//#define _ACRYLIC_REFRACTIVE_INDEX_            (1.50)
// --------------------------------------------------------------------------------------------

// -- Geometry --------------------------------------------------------------------------------
//
#define _VESSEL_OUTER_RADIUS_            ( 640.0*mm)

// Given by the project;
#define _VESSEL_LENGTH_                    (54.0*cm)
#define _VESSEL_OFFSET_ (1187.0*mm + _VESSEL_LENGTH_/2)

// Taken from Alex's drawing; 
#define _FLANGE_EPIPE_DIAMETER_           (105.3*mm)
#define _FLANGE_HPIPE_DIAMETER_           ( 44.7*mm)
#define _FLANGE_HPIPE_OFFSET_              (67.6*mm)

// Make the project happy: vessel will be that far away from the flange;
#define _FLANGE_CLEARANCE_                  (5.0*mm)

// Tune them later to something more reasonable, from a construction point of view;
#define _VESSEL_FRONT_SIDE_THICKNESS_       (1.0*mm)
#define _VESSEL_REAR_SIDE_THICKNESS_        (5.0*mm)
#define _VESSEL_INNER_WALL_THICKNESS_       (1.0*mm)
#define _VESSEL_OUTER_WALL_THICKNESS_       (3.0*mm)

// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
#define _BUILDING_BLOCK_CLEARANCE_          (1.0*mm)

// Some "standard" value applied to all mirrors;
#define _MIRROR_REFLECTIVITY_                 (0.90)

// To the rear side of the gas volume; 5cm should be sufficient for EICROC configuration;
#define _SENSOR_AREA_LENGTH_                (5.0*cm)

// Because of the beam pipe flange; 
#define _HRPPD_CENTRAL_ROW_OFFSET_         (40.0*mm)

// Well, a fake material for now;
#define _MIRROR_MATERIAL_            (m_CarbonFiber)
#define _MIRROR_THICKNESS_                  (1.0*mm)

// At the downstream (sensor plane) location; upstream radii are calculated automatically;
#define _CONICAL_MIRROR_INNER_RADIUS_     (120.0*mm)
#define _CONICAL_MIRROR_OUTER_RADIUS_     (540.0*mm)

// May still want to disable the conical mirror optics in IRT;
//#define _USE_CONICAL_MIRROR_OPTICS_
// --------------------------------------------------------------------------------------------

// -- QE and Co -------------------------------------------------------------------------------
//
// Default is to use LAPPD QE as given by Alexey;
//#define _USE_SiPM_QE_

// Some reasonably optimistic number; assume that it includes an unknown 
// HRPPD Collection Efficiency (CE) as well;
#define _SAFETY_FACTOR_                       (0.70)
// This number is 1.00 for HRPPDs (no SiPM-like fill factor or such); keep in place 
// for historical reasons;
#define _SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
// --------------------------------------------------------------------------------------------

// -- HRPPD -----------------------------------------------------------------------------------
//
// A geometry presently assumed for the final production;
#define _HRPPD_TILE_SIZE_                 (120.0*mm)
#define _HRPPD_WINDOW_THICKNESS_            (5.0*mm)
// Anode base plate and walls; 
#define _HRPPD_CERAMIC_BODY_THICKNESS_      (9.0*mm)
#define _HRPPD_BASEPLATE_THICKNESS_         (3.0*mm)
// Assume 100% coverage pure silver plating 15um thick, 4 layers total (pads on both sides 
// and ground around signal layer); 
#define _HRPPD_PLATING_LAYER_THICKNESS_    (0.06*mm)
// Walls ~3mm thick are assumed;
#define _HRPPD_OPEN_AREA_SIZE_            (114.0*mm)
// Area covered by the photocathode in the geometry description; should be a good 
// enough description of a fully efficient pixellated surface of the real sensors;
#define _HRPPD_ACTIVE_AREA_SIZE_          (108.0*mm)

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
//#define _USE_PYRAMIDS_OPTICS_
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

// Call /geometry/test/run if uncommented;
//#define _GEOMETRY_CHECK_

// This needs to be verified (and actually the QE at normal incidence renormalized); 
#define _BIALKALI_REFRACTIVE_INDEX_            (2.7)
// --------------------------------------------------------------------------------------------

#endif
