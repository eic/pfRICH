//
// ePIC pfRICH default configuration;
//

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_EPIC_DEFAULT_
#define _PFRICH_EPIC_DEFAULT_

#include <share.h>
#include <Materials.h>

// -- Primary particle(s) ---------------------------------------------------------------------
//
// Optional smearing of primary vertices along the beam line; applies to HEPMC3 input as well;
// 10cm (proton bunch size) is a reasonable value;
#define _PRIMARY_VERTEX_SIGMA_               (35*mm)//(10*cm)

//
// All this can be overriden by '-i' cmd line switch (HEPMC3 input); also see '-s <statistics>' key;
//
#define _PRIMARY_PARTICLE_TYPE_              ("pi+")
// Will toggle between the two types if defined;
//#define _ALTERNATIVE_PARTICLE_TYPE_        ("kaon+")

#define _PRIMARY_PARTICLE_ETA_           (sign*2.50)
//#define _PRIMARY_PARTICLE_ETA_            (sign*1.90)
// If commented out: uniform phi;
//#define _PRIMARY_PARTICLE_PHI_         (95*degree)
//#define _PRIMARY_PARTICLE_PHI_         (90*degree)

#define _PRIMARY_PARTICLE_MOMENTUM_        (7.0*GeV)
// --------------------------------------------------------------------------------------------

// -- General ---------------------------------------------------------------------------------
//
// Comment out if want to run without MARCO magnetic field;
#define _USE_MAGNETIC_FIELD_ "./database/MARCO_v.7.6.2.2.11_1.7T_Magnetic_Field_Map_2024_05_02_rad_coords_cm_T.BMap.txt"
//#define _USE_MAGNETIC_FIELD_ "./database/MARCO_v.6.4.1.1.3_1.7T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt"
//#define _USE_MAGNETIC_FIELD_ "./database/MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt"

// Import other parts of ePIC geometry;
//#define _IMPORT_TRACKER_GDML_FILE_ ("./gdml/tracking_only.modified.gdml")
//#define _IMPORT_BEAMPIPE_GDML_FILE_ ("./gdml/ip6_brycecanyon.modified.gdml")

// Generate GDML output file (for material scans, etc);
//#define _GENERATE_GDML_OUTPUT_       ("pfRICH.gdml")
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// At the downstream (sensor plane) location; upstream radii are calculated automatically;
#define _CONICAL_MIRROR_DS_INNER_RADIUS_  (120.0*mm)
#define _CONICAL_MIRROR_DS_OUTER_RADIUS_  (570.0*mm)
// Fixed values matching a CAD model, as of 2024/04/29;
#define _CONICAL_MIRROR_US_INNER_RADIUS_  ( 66.0*mm)
#define _CONICAL_MIRROR_US_OUTER_RADIUS_  (623.0*mm)

// Comment out if want to disable the conical mirror optics in IRT;
#define _USE_CONICAL_MIRROR_OPTICS_           (true)
// --------------------------------------------------------------------------------------------

// -- Vessel geometry -------------------------------------------------------------------------
//
// FIXME: as of Fall 2023, this number is actually 650mm; however, a gap between the outer
// shell and the outer conical mirror was defined in a bit optimistic fashion; need to 
// revisit all this before doing the TDR simulations; 
//#define _VESSEL_OUTER_RADIUS_             (638.0*mm)
// Let it be 640mm for the time being; 638mm creates a conflict between the outer HRPPD 
// mirrors and the gas volume; FIXME: why?;
//#define _VESSEL_OUTER_RADIUS_             (640.0*mm)
#define _VESSEL_OUTER_RADIUS_             (650.0*mm)

// Given by the project; see e-mail from Alex Eslinger from 2023/08/16;
#define _FIDUCIAL_VOLUME_LENGTH_          (491.0*mm)
#define _FIDUCIAL_VOLUME_OFFSET_ (1236.0*mm + _FIDUCIAL_VOLUME_LENGTH_/2)

// Taken from Alex's drawing; 
#define _FLANGE_EPIPE_DIAMETER_           (105.3*mm)
#define _FLANGE_HPIPE_DIAMETER_           ( 44.7*mm)
#define _FLANGE_HPIPE_OFFSET_              (67.6*mm)

// Make the project people happy: vessel will be that far away from the flange
// during the installation procedure;
#define _FLANGE_CLEARANCE_                  (5.0*mm)

// FIXME: all these thickness values and material definitions need to be revisited;
//
// 0.29" -> 1/4" as of 2024/05/01; 
#define _VESSEL_FRONT_SIDE_THICKNESS_  (0.25*_INCH_)
// 2*10mil CF + 1/4" HC + 2*10mil CF;
#define _VESSEL_INNER_WALL_THICKNESS_  (0.29*_INCH_)
// 2*10mil CF + 1/2" HC + 2*10mil CF;
#define _VESSEL_OUTER_WALL_THICKNESS_  (0.54*_INCH_)

// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
//#define _BUILDING_BLOCK_CLEARANCE_          (1.0*mm)
// 2024/05/01: this value allows one to get a mirror length of ~402mm in a no-pyramid case,
// to match Alex's CAD model; we are not fighting for mismatches between CAD and GEANT to
// anything better than a couple off mm anyway;
#define _BUILDING_BLOCK_CLEARANCE_          (0.3*mm)

// Because of the beam pipe flange; 
#define _HRPPD_CENTRAL_ROW_OFFSET_         (40.0*mm)

// Well, a fake material for now;
//#define _MIRROR_MATERIAL_            (m_QuarterInch_CF_HoneyComb)//m_CarbonFiber)
// 2*10mil CF + 1/4" HC + 2*10mil CF;
#define _INNER_MIRROR_THICKNESS_               (1.0)//0.29*_INCH_)
// 2*10mil CF + 1/2" HC + 2*10mil CF;
#define _OUTER_MIRROR_THICKNESS_               (2.0)//0.54*_INCH_)

// Gap between tiles at the sensor plane; as of 2023/02/15, assume a "windowed grid" 
// support plate on the upstream sensor plane end + individual pockets with 
// ~1mm thick walls for each HRPPD (<10mm high?); insertion from the downstream side;
//#define _HRPPD_INSTALLATION_GAP_            (1.5*mm)
// 2024/04/29: ~3mm is the most one can get; keep it like this for the time being;
#define _HRPPD_INSTALLATION_GAP_            (3.0*mm)
#define _HRPPD_SUPPORT_GRID_BAR_HEIGHT_     (3.0*mm)
#define _HRPPD_SUPPORT_GRID_BAR_WIDTH_     (12.0*mm)

// Can also use m_C2F6;
#define _GAS_RADIATOR_                     m_Nitrogen
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
// Azimuthal segmentation is hardcoded, see DetectorConstruction::Construct();
//#define _AEROGEL_BAND_COUNT_                     (3)
#define _AEROGEL_BAND_COUNT_                     (4)

// Inner and outer radial walls are thicker;
#define _AEROGEL_INNER_WALL_THICKNESS_      (1.0*mm)
// No need in a thick wall here, since aluminum rings embedded in honeycomb edges will be used;
#define _AEROGEL_OUTER_WALL_THICKNESS_      (1.0*mm)
// All other separators are thin; 
#define _AEROGEL_SEPARATOR_WALL_THICKNESS_  (0.5*mm)
// Introduced on 2024/04/29;
#define _AEROGEL_CLEARANCE_GAP_             (0.5*mm)

// Up to two layers along the beam line; 
//#define _AEROGEL_1_ _AEROGEL_CLAS12_DENSITY_225_MG_CM3_
#define _AEROGEL_1_ _AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_
//#define _AEROGEL_1_ _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_
#define _AEROGEL_THICKNESS_1_               (2.5*cm)
//#define _AEROGEL_2_ _AEROGEL_CLAS12_DENSITY_155_MG_CM3_
//#define _AEROGEL_THICKNESS_2_               (2.0*cm)

// Set to m_Nitrogen and something like <100um thickness, for "clean" studies only;
#define _AEROGEL_SPACER_MATERIAL_       (m_Absorber)
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// If _ACRYLIC_THICKNESS_ is defined, a single layer right after the aerogel is installed; 
#define _ACRYLIC_THICKNESS_                 (3.0*mm)
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// Mirror height and width;
#define _PYRAMID_MIRROR_HEIGHT_            (30.0*mm)
#define _PYRAMID_MIRROR_APERTURE_WIDTH_    (_HRPPD_ACTIVE_AREA_SIZE_)
// --------------------------------------------------------------------------------------------

#endif
