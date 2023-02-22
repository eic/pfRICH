//
// Fixed portion of the pfRICH parameters for the purposes of March 2023 review;
//

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_FIXED_
#define _PFRICH_FIXED_

// -- Vessel geometry -------------------------------------------------------------------------
//
#define _VESSEL_OUTER_RADIUS_             (643.0*mm)

// Given by the project;
#define _VESSEL_LENGTH_                   (541.5*mm)
#define _VESSEL_OFFSET_ (1185.5*mm + _VESSEL_LENGTH_/2)

// Taken from Alex's drawing; 
#define _FLANGE_EPIPE_DIAMETER_           (105.3*mm)
#define _FLANGE_HPIPE_DIAMETER_           ( 44.7*mm)
#define _FLANGE_HPIPE_OFFSET_              (67.6*mm)

// Make the project people happy: vessel will be that far away from the flange
// during the installation procedure;
#define _FLANGE_CLEARANCE_                  (5.0*mm)

// Tune them later to something more reasonable, from a construction point of view;
#define _VESSEL_FRONT_SIDE_THICKNESS_       (3.5*mm)
#define _VESSEL_REAR_SIDE_THICKNESS_        (5.0*mm)
#define _VESSEL_INNER_WALL_THICKNESS_       (2.0*mm)
#define _VESSEL_OUTER_WALL_THICKNESS_       (5.0*mm)
#define _VESSEL_MATERIAL_            (m_CarbonFiber)

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
#define _MIRROR_MATERIAL_            (m_CarbonFiber)
#define _INNER_MIRROR_THICKNESS_            (1.0*mm)
#define _OUTER_MIRROR_THICKNESS_            (2.0*mm)

#define _HRPPD_SUPPORT_GRID_BAR_HEIGHT_     (3.0*mm)
#define _HRPPD_SUPPORT_GRID_BAR_WIDTH_     (12.0*mm)
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

// These are some random numbers to the moment; are only important for time-of-flight;
#define _HRPPD_METALLIZATION_REFLECTIVITY_    (0.80)
#define _HRPPD_METALLIZATION_ROUGHNESS_       (0.05)
// --------------------------------------------------------------------------------------------

// -- Wavelength range ------------------------------------------------------------------------
//
// "Nominal" wavelength at which average refractive index will be calculated; FIXME: remove;
#define _LAMBDA_NOMINAL_                     (470.0)
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
#define _AEROGEL_INNER_WALL_THICKNESS_      (2.0*mm)
#define _AEROGEL_OUTER_WALL_THICKNESS_      (5.0*mm)
// All other separators are thin; FIXME: there are no gaps between aerogel tiles and separators;
#define _AEROGEL_SEPARATOR_WALL_THICKNESS_  (0.5*mm)
// --------------------------------------------------------------------------------------------

// -- Other parameters ------------------------------------------------------------------------
//
// This needs to be verified (and actually the QE at normal incidence renormalized); 
#define _BIALKALI_REFRACTIVE_INDEX_            (2.7)

// This number is 1.00 for HRPPDs (no SiPM-like fill factor or such); keep in place 
// for historical reasons;
#define _SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
// --------------------------------------------------------------------------------------------

#endif