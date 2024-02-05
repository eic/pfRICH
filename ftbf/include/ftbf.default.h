//
// pfRICH 2024 FTBF beam test configuration;
//

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_FTBF_
#define _PFRICH_FTBF_

#include <share.h>
#include <Materials.h>

// -- Primary particle(s) ---------------------------------------------------------------------
//
#define _PRIMARY_PARTICLE_TYPE_              ("pi+")
// Will toggle between the two types if defined;
//#define _ALTERNATIVE_PARTICLE_TYPE_        ("kaon+")

// Just something resembling FTBF primary beam (120 GeV protons);
#define _XCOORD_AVERAGE_                   (0.0*mm)
#define _XCOORD_SIGMA_                     (2.0*mm)

#define _XSLOPE_AVERAGE_                 (0.0*mrad)
#define _XSLOPE_SIGMA_                   (0.2*mrad)

#define _YCOORD_AVERAGE_                   (0.0*mm)
#define _YCOORD_SIGMA_                     (1.0*mm)

#define _YSLOPE_AVERAGE_                 (0.0*mrad)
#define _YSLOPE_SIGMA_                   (0.1*mrad)

#define _PRIMARY_PARTICLE_MOMENTUM_       (7.0*GeV)
// --------------------------------------------------------------------------------------------

// -- Vessel geometry -------------------------------------------------------------------------
//
// Prefer to keep it in sync with the ePIC setup; 450mm allows one to fit photons reflected 
// off the pyramid mirrors into the 2x2 HRPPD matrix acceptance; 
//#define _FIDUCIAL_VOLUME_LENGTH_2x2_          (491.0*mm)
#define _FIDUCIAL_VOLUME_LENGTH_2x2_          (450.0*mm)
#define _FIDUCIAL_VOLUME_OFFSET_2x2_ (1236.0*mm + _FIDUCIAL_VOLUME_LENGTH_2x2_/2)

// These are some random numbers for now;
#define _FIDUCIAL_VOLUME_LENGTH_1x1_          (250.0*mm)
#define _FIDUCIAL_VOLUME_OFFSET_1x1_ ( 800.0*mm + _FIDUCIAL_VOLUME_LENGTH_1x1_/2)

// FIXME: this is getting to be really obsolete; just want to be in sync with epic.default.h; 
#define _VESSEL_FRONT_SIDE_THICKNESS_  (0.29*_INCH_)

// Some moderately optimistic number for aerogel-to-aerogel, aerogel-to-acrylic, 
// acrylic-to-mirror and such spacing;
#define _BUILDING_BLOCK_CLEARANCE_          (1.0*mm)

// Gap between tiles at the sensor plane; as of 2023/02/15, assume a "windowed grid" 
// support plate on the upstream sensor plane end + individual pockets with 
// ~1mm thick walls for each HRPPD (<10mm high?); insertion from the downstream side;
#define _HRPPD_INSTALLATION_GAP_            (0.25*mm + 10*3.25*mm)//1.5*mm)

// Can also use m_C2F6; FIXME: should make sure everything works with m_Air;
#define _GAS_RADIATOR_                     m_Nitrogen
// --------------------------------------------------------------------------------------------

// -- Aerogel ---------------------------------------------------------------------------------
//
#define _AEROGEL_1_ _AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_
#define _AEROGEL_THICKNESS_1_               (2.5*cm)
// --------------------------------------------------------------------------------------------

// -- Aspheric lens ---------------------------------------------------------------------------
//
#define _ZCOORD_ASPHERIC_LENS_              (-25*mm)
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
// If _ACRYLIC_THICKNESS_ is defined, a single layer right after the aerogel is installed; 
#define _ACRYLIC_THICKNESS_                 (3.0*mm)
// --------------------------------------------------------------------------------------------

// -- Mirrors ---------------------------------------------------------------------------------
//
// See share.h, where one can enable / disable the pyramids;
#define _PYRAMID_MIRROR_HEIGHT_            (50.0*mm)
// --------------------------------------------------------------------------------------------

#endif
