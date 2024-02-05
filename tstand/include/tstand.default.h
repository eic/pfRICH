//
// pfRICH 2024 QA station geometry (photon source, etc);
//

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_TSTAND_
#define _PFRICH_TSTAND_

#include <hrppd.h>
#include <share.h>

// -- Primary particle(s) ---------------------------------------------------------------------
//
#define _PRIMARY_PARTICLE_TYPE_   ("opticalphoton")

#define _XCOORD_AVERAGE_                   (0.0*mm)
#define _YCOORD_AVERAGE_                   (0.0*mm)
#define _SOURCE_SPOT_DIAMETER_             (0.3*mm)
#define _ZCOORD_                             (0*mm)

// FIXME: calculate slopes via NA;
#define _XSLOPE_AVERAGE_                 (0.0*mrad)
#define _XSLOPE_SIGMA_                    (50*mrad)
#define _YSLOPE_AVERAGE_                 (0.0*mrad)
#define _YSLOPE_SIGMA_                    (50*mrad)

#define _PRIMARY_PARTICLE_MOMENTUM_        (4.0*eV)
// --------------------------------------------------------------------------------------------

// -- Vessel geometry -------------------------------------------------------------------------
//
#define _FIDUCIAL_VOLUME_LENGTH_          (250.0*mm)
#define _FIDUCIAL_VOLUME_OFFSET_               (0.0)

// Can also use m_C2F6; FIXME: should make sure everything works with m_Air;
#define _GAS_RADIATOR_                     m_Nitrogen
// --------------------------------------------------------------------------------------------

// -- Aspheric lens ---------------------------------------------------------------------------
//
#define _ZCOORD_ASPHERIC_LENS_              (-25*mm)
// --------------------------------------------------------------------------------------------

#endif
