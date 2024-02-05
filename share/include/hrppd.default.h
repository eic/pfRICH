
#ifndef _HRPPD_
#define _HRPPD_

#include <Materials.h>

// -- Overall HRPPD geometry ------------------------------------------------------------------
//
// A formfactor presently assumed for the final production;
#define _HRPPD_TILE_SIZE_                 (120.0*mm)
// Walls ~3mm thick are assumed;
#define _HRPPD_OPEN_AREA_SIZE_            (114.0*mm)
// Area covered by the photocathode in the geometry description; should be a good 
// enough description of a fully efficient pixellated anode surface of the real sensors;
// As of 2024/01/28: 108mm -> 104mm, since it is not clear whether Incom can deliver 80% 
// OAR (Open Area Ratio) in their final sensor design; stick to 75% (first five EIC HRPPDs); 
#define _HRPPD_ACTIVE_AREA_SIZE_          (104.0*mm)

// Stick to 5mm fused silica (first five EIC HRPPDs), since it is not clear whether sapphire
// windows will be used for a final production;
#define _HRPPD_WINDOW_THICKNESS_            (5.0*mm)//(3.8mm)
#define _HRPPD_WINDOW_MATERIAL_        m_FusedSilica//m_Sapphire

// Anode base plate and walls; more or less close to reality;
#define _HRPPD_CERAMIC_BODY_THICKNESS_      (9.0*mm)
#define _HRPPD_BASEPLATE_THICKNESS_         (3.0*mm)

// --------------------------------------------------------------------------------------------

// Just to fit everything in;
#define _HRPPD_CONTAINER_VOLUME_HEIGHT_    (32.0*mm)

// -- Dead material ---------------------------------------------------------------------------
//
// Assume 100% coverage pure silver plating 15um thick, 4 layers total (pads on both sides 
// and ground around signal layer); FIXME: not true for EIC HRPPDs;
#define _HRPPD_PLATING_LAYER_THICKNESS_    (0.06*mm)

// These are some random numbers to the moment; are only important for time-of-flight;
//+#define _HRPPD_METALLIZATION_REFLECTIVITY_    (0.80)
//+#define _HRPPD_METALLIZATION_ROUGHNESS_       (0.05)

// Assume two plates, 600um thick, ~70% OAR as counted for MCP pore coverage;
#define _EFFECTIVE_MCP_THICKNESS_     (2*0.6*mm*0.3)
// FIXME: change to glass; should not really matter I guess;
#define _EFFECTIVE_MCP_MATERIAL_       m_FusedSilica

#define _READOUT_PCB_THICKNESS_             (2.0*mm)
// Want to decrease material budget around the walls;
#define _READOUT_PCB_SIZE_     (_HRPPD_OPEN_AREA_SIZE_ - 2.0*mm)

// This stuff is pretty vague, merely a placeholder;
#define _ASIC_SIZE_XY_                        (16.0)
#define _ASIC_THICKNESS_                       (1.0)
#define _ASIC_MATERIAL_                   m_Silicon

// XY-size is calculated based on the ASIC size and location;
#define _COLD_PLATE_THICKNESS_                 (1.5)
#define _COLD_PLATE_MATERIAL_            m_Aluminum

// Water cooling pipe;
#define _COOLING_PIPE_INNER_DIAMETER_  (_INCH_*0.218)//3/8 )
#define _COOLING_PIPE_WALL_THICKNESS_  (_INCH_*0.016)//1/16)
#define _COOLING_PIPE_MATERIAL_          m_Titanium

// --------------------------------------------------------------------------------------------

// -- QE and Co -------------------------------------------------------------------------------
//
#define _QE_DOWNSCALING_FACTOR_          (30.0/37.0)

// Some reasonably optimistic number; assume that it includes an unknown 
// HRPPD Collection Efficiency (CE) as well;
#define _SAFETY_FACTOR_                       (0.70)

// This number is 1.00 for HRPPDs (no SiPM-like fill factor or such); keep in place 
// for historical reasons;
#define _SENSOR_PLANE_GEOMETRIC_EFFICIENCY_   (1.00)
// --------------------------------------------------------------------------------------------

#endif
