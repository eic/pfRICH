
#ifndef _PFRICH_SHARE_
#define _PFRICH_SHARE_

// Can be overriden by a '-s' cmd line switch;
#define _STATISTICS_DEFAULT_                  (1000)

// May want to suppress secondaries;
#define _DISABLE_SECONDARIES_

// May want to disable parasitic sources of photons during G4 stepping;
#define _DISABLE_GAS_VOLUME_PHOTONS_
//#define _DISABLE_AEROGEL_PHOTONS_
#define _DISABLE_ACRYLIC_PHOTONS_
#define _DISABLE_HRPPD_WINDOW_PHOTONS_

// Call /geometry/test/run if uncommented; activated in '-m' (visualization) mode only;
#define _GEOMETRY_CHECK_

// To the rear side of the gas volume; 5cm should be sufficient for EICROC configuration;
#define _SENSOR_AREA_LENGTH_                (5.0*cm)

// -- Mirrors ---------------------------------------------------------------------------------
//
// If true, optional funneling mirrors around the HRPPD sensors are installed in 
// ePIC configuration and in a 2x2 box in FTBF configuration; 
#define _USE_PYRAMIDS_                          true

// May still want to disable the funneling optics in IRT;
#define _USE_PYRAMID_OPTICS_                    true

// Some "standard" value applied to all mirrors;
#define _MIRROR_REFLECTIVITY_                 (0.90)
// --------------------------------------------------------------------------------------------

// All geometry (and optics!) defined for the hadron-going endcap, for historical reasons;
static const bool flip = true;
static const double sign = flip ? -1.0 : 1.0;

class G4RunManager;
class CherenkovDetectorCollection;

void setup(G4RunManager *runManager, CherenkovDetectorCollection *geometry, const char *infile);

#endif
