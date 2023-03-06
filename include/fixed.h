//
// Fixed portion of the pfRICH parameters for the purposes of March 2023 review;
//

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_FIXED_
#define _PFRICH_FIXED_

static const bool flip = true;
static const double sign = flip ? -1.0 : 1.0;

#define _INCH_                             (25.4*mm)
#define _MIL_                          (_INCH_/1000)

// -- Vessel geometry -------------------------------------------------------------------------
//
#define _VESSEL_OUTER_RADIUS_             (643.0*mm)

// Given by the project;
#define _FIDUCIAL_VOLUME_LENGTH_          (541.5*mm)
#define _FIDUCIAL_VOLUME_OFFSET_ (1185.5*mm + _FIDUCIAL_VOLUME_LENGTH_/2)

// Taken from Alex's drawing; 
#define _FLANGE_EPIPE_DIAMETER_           (105.3*mm)
#define _FLANGE_HPIPE_DIAMETER_           ( 44.7*mm)
#define _FLANGE_HPIPE_OFFSET_              (67.6*mm)

// Make the project people happy: vessel will be that far away from the flange
// during the installation procedure;
#define _FLANGE_CLEARANCE_                  (5.0*mm)

// 2*10mil CF + 1/4" HC + 2*10mil CF;
#define _VESSEL_FRONT_SIDE_THICKNESS_  (0.29*_INCH_)
// 2*10mil CF + 1/4" HC + 2*10mil CF;
#define _VESSEL_INNER_WALL_THICKNESS_  (0.29*_INCH_)
// 2*10mil CF + 1/2" HC + 2*10mil CF;
#define _VESSEL_OUTER_WALL_THICKNESS_  (0.54*_INCH_)
//#define _HONEYCOMB_REINFORCEMENT_MATERIAL_ m_Aluminum//Delrin

// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
#define _BUILDING_BLOCK_CLEARANCE_          (1.0*mm)

// To the rear side of the gas volume; 5cm should be sufficient for EICROC configuration;
#define _SENSOR_AREA_LENGTH_                (5.0*cm)

// Because of the beam pipe flange; 
#define _HRPPD_CENTRAL_ROW_OFFSET_         (40.0*mm)

// Gap between tiles at the sensor plane; as of 2023/02/15, assume a "windowed grid" 
// support plate on the upstream sensor plane end + individual pockets with 
// ~1mm thick walls for each HRPPD (<10mm high?); insertion from the downstream side;
#define _HRPPD_INSTALLATION_GAP_          (  1.5*mm)

// Well, a fake material for now;
//#define _MIRROR_MATERIAL_            (m_QuarterInch_CF_HoneyComb)//m_CarbonFiber)
// 2*10mil CF + 1/4" HC + 2*10mil CF;
#define _INNER_MIRROR_THICKNESS_       (0.29*_INCH_)
// 2*10mil CF + 1/2" HC + 2*10mil CF;
#define _OUTER_MIRROR_THICKNESS_       (0.54*_INCH_)

#define _HRPPD_SUPPORT_GRID_BAR_HEIGHT_     (3.0*mm)
#define _HRPPD_SUPPORT_GRID_BAR_WIDTH_     (12.0*mm)
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// Some "standard" value applied to all mirrors;
#define _MIRROR_REFLECTIVITY_                 (0.90)

// At the downstream (sensor plane) location; upstream radii are calculated automatically;
#define _CONICAL_MIRROR_INNER_RADIUS_     (120.0*mm)
#define _CONICAL_MIRROR_OUTER_RADIUS_     (570.0*mm)
// --------------------------------------------------------------------------------------------

// -- HRPPD geometry --------------------------------------------------------------------------
//
// A formfactor presently assumed for the final production;
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

// This needs to be verified (and actually the QE at normal incidence renormalized); 
#define _BIALKALI_REFRACTIVE_INDEX_           (1.47)

// These are some random numbers to the moment; are only important for time-of-flight;
#define _HRPPD_METALLIZATION_REFLECTIVITY_    (0.80)
#define _HRPPD_METALLIZATION_ROUGHNESS_       (0.05)

// Assume two plates, 600um thick, ~70% OAR;
#define _EFFECTIVE_MCP_THICKNESS_     (2*0.6*mm*0.3)
// FIXME: change to glass; should not really matter I guess;
#define _EFFECTIVE_MCP_MATERIAL_  m_FusedSilica

// --------------------------------------------------------------------------------------------

// -- Wavelength range ------------------------------------------------------------------------
//
// "Nominal" wavelength at which average refractive index will be calculated; FIXME: remove;
// should rather store a fine step n(nu) lookup table in the CherenkovRadiator structure
// after the GEANT pass; the problem is that n(<lambda>) is needed not only for the active 
// radiator, where calibration photons are readily available, but also for say quartz to 
// calculate the optical path correctly (and photon generation in the window may be suppressed); 
//#define _LAMBDA_NOMINAL_                     (470.0)
#define _LAMBDA_NOMINAL_                     (365.0)
#define _MAGIC_CFF_                         (1239.8)

// Consider full range from 175um (fused silica "cutoff") to 800um, at most; 
#define _WLDIM_                                  26
#define _LAMBDA_MIN_                         (175.0)
#define _LAMBDA_MAX_                         (800.0)
#define _NU_MIN_       (eV*_MAGIC_CFF_/_LAMBDA_MAX_)
#define _NU_MAX_       (eV*_MAGIC_CFF_/_LAMBDA_MIN_)
#define _NU_STEP_ ((_NU_MAX_ - _NU_MIN_)/(_WLDIM_-1))
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
// Up to two layers along the beam line; 
#define _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_ 0
#define _AEROGEL_BELLE_II_LARGE_REFRACTIVE_INDEX_ 1
// FIXME: this does not look nice, but suffices;
#define _AEROGEL_CLAS12_DENSITY_155_MG_CM3_     155
#define _AEROGEL_CLAS12_DENSITY_225_MG_CM3_     225

// Azimuthal segmentation is hardcoded, see DetectorConstruction::Construct();
#define _AEROGEL_BAND_COUNT_                     (3)

// Inner and outer radial walls are thicker;
#define _AEROGEL_INNER_WALL_THICKNESS_      (1.0*mm)
// No need in a thick wall here, since aluminum rings embedded in honeycomb edges will be used;
#define _AEROGEL_OUTER_WALL_THICKNESS_      (1.0*mm)
// All other separators are thin; FIXME: there are no gaps between aerogel tiles and separators;
#define _AEROGEL_SEPARATOR_WALL_THICKNESS_  (0.5*mm)
// --------------------------------------------------------------------------------------------

// -- Materials behind the sensor plane -------------------------------------------------------
//
#define _READOUT_PCB_THICKNESS_             (2.0*mm)
// Want to decrease material budget around the walls;
#define _READOUT_PCB_SIZE_     (_HRPPD_OPEN_AREA_SIZE_ - 2.0*mm)

// This stuff is pretty vague, merely a placeholder;
#define _ASIC_SIZE_XY_                        (16.0)
#define _ASIC_THICKNESS_                       (1.0)
#define _ASIC_MATERIAL_                    m_Silicon
// --------------------------------------------------------------------------------------------

// -- General ---------------------------------------------------------------------------------
//
// May want to suppress secondaries;
#define _DISABLE_SECONDARIES_

// Comment out if want to run without MARCO magnetic field;
#define _USE_MAGNETIC_FIELD_ "./database/MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt"

// May want to disable parasitic sources of photons during G4 stepping;
//#define _DISABLE_GAS_VOLUME_PHOTONS_
//#define _DISABLE_AEROGEL_PHOTONS_
//#define _DISABLE_ACRYLIC_PHOTONS_
//#define _DISABLE_HRPPD_WINDOW_PHOTONS_

// May want to disable Rayleigh scattering and / or absorption by hand; 
//#define _DISABLE_RAYLEIGH_SCATTERING_
//#define _DISABLE_ABSORPTION_

// Import other parts of ePIC geometry;
//#define _IMPORT_TRACKER_GDML_FILE_ ("./gdml/tracking_only.modified.gdml")
//#define _IMPORT_BEAMPIPE_GDML_FILE_ ("./gdml/ip6_brycecanyon.modified.gdml")

// Call /geometry/test/run if uncommented;
//#define _GEOMETRY_CHECK_

// Generate GDML output file (for material scans, etc);
//#define _GENERATE_GDML_OUTPUT_       ("pfRICH.gdml")

// Disable pfRICH altogether;
//#define _DISABLE_PFRICH_GEOMETRY_
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
//#define _AEROGEL_1_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
#define _AEROGEL_1_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
#define _AEROGEL_THICKNESS_1_               (2.5*cm)
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

#define _QE_DOWNSCALING_FACTOR_          (30.0/37.0)

// Some reasonably optimistic number; assume that it includes an unknown 
// HRPPD Collection Efficiency (CE) as well;
#define _SAFETY_FACTOR_                       (0.70)

// This number is 1.00 for HRPPDs (no SiPM-like fill factor or such); keep in place 
// for historical reasons;
#define _SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// If uncommented, optional funneling mirrors around the HRPPD sensors are installed;
//#define _USE_PYRAMIDS_
// Mirror height and width;
//#define _PYRAMID_MIRROR_HEIGHT_            (30.0*mm)
//#define _PYRAMID_MIRROR_APERTURE_WIDTH_    (_HRPPD_ACTIVE_AREA_SIZE_)

// May still want to disable the funneling optics in IRT;
//#define _USE_PYRAMIDS_OPTICS_

// May still want to disable the conical mirror optics in IRT;
#define _USE_CONICAL_MIRROR_OPTICS_
// --------------------------------------------------------------------------------------------

#endif
