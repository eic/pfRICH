
#include <map>

#include "G4SystemOfUnits.hh"

#ifndef _PFRICH_MATERIALS_
#define _PFRICH_MATERIALS_

class G4Element;
class G4Material;
class G4RadiatorMaterial;

#include <CherenkovWaveLengthRange.h>
  
// Uncomment to disable Rayleigh scattering and / or absorption by hand; 
//#define _DISABLE_RAYLEIGH_SCATTERING_
//#define _DISABLE_ABSORPTION_

// Eventually this is a single place where these two are defined;
#define _INCH_                             (25.4*mm)
#define _MIL_                          (_INCH_/1000)

// -- Aerogel ---------------------------------------------------------------------------------
//
#define _AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_ 0
#define _AEROGEL_BELLE_II_LARGE_REFRACTIVE_INDEX_ 1
#define _AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_  2
// FIXME: this does not look nice, but suffices;
#define _AEROGEL_CLAS12_DENSITY_155_MG_CM3_     155
#define _AEROGEL_CLAS12_DENSITY_225_MG_CM3_     225

// If uncommented: fixed refractive index, no attenuation (single-layer CLAS12 config only);
//#define _AEROGEL_FIXED_REFRACTIVE_INDEX_    (1.044)
// --------------------------------------------------------------------------------------------

// -- Acrylic filter --------------------------------------------------------------------------
//
#define _ACRYLIC_WL_CUTOFF_                 (300*nm)

// Does not need to be precise;
#define _ACRYLIC_DENSITY_               (1.18*g/cm3)

// If uncommented: fixed refractive index, no attenuation; 
//#define _ACRYLIC_FIXED_REFRACTIVE_INDEX_      (1.50)
// --------------------------------------------------------------------------------------------

// -- Carbon fiber honeycomb sandwich ---------------------------------------------------------
//
// Rad. length of the carbon fiber, epoxy glue and honeycomb as provided by 
// Prakhar on 02/25/2023;
#define _CF_RAD_LENGTH_                    (23.4*cm)
#define _EG_RAD_LENGTH_                    (35.7*cm)
#define _HC_RAD_LENGTH_                   (845.4*cm)
// Used only in the material composition calculation;
#define _CF_THICKNESS_                    (10*_MIL_)
#define _EG_THICKNESS_                    (10*_MIL_)

#define _FR4_RAD_LENGTH_                   (19.4*cm)
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

// As 2023/03/09 consider wavelength range from 150um to 800um; 
#define _WLDIM_                                  27
#define _LAMBDA_MIN_                         (150.0)
#define _LAMBDA_MAX_                         (800.0)
#define _NU_MIN_       (eV*_MAGIC_CFF_/_LAMBDA_MAX_)
#define _NU_MAX_       (eV*_MAGIC_CFF_/_LAMBDA_MIN_)
#define _NU_STEP_ ((_NU_MAX_ - _NU_MIN_)/(_WLDIM_-1))
// --------------------------------------------------------------------------------------------

// FIXME: this needs to be verified (and actually the QE at normal incidence renormalized); 
// The problem here is that when the QE is measured on a test stand, a fraction of photons 
// actually bounces back off the window-photocathode boundary (I think); since this same 
// bouncing is then taken care by GEANT, a double counting must occur; just making this 
// refractive index more or less equal to the window one would solve the normalization problem, 
// but will also eliminate bouncing all together, which is not realistic either since a 
// non-negligible number of photons (especially in case of a photon flash produced in a 
// window) actually *will* be bouncing in the window, moving away from their initial 
// impact (creation) point;   
#define _BIALKALI_REFRACTIVE_INDEX_           (2.80)//(1.47)

class Materials: public CherenkovWaveLengthRange {
public:
  Materials();
  virtual ~Materials() {};

  // The only two needed outside of the DetectorConstruction class;
  G4RadiatorMaterial *Nitrogen( void )    { return m_Nitrogen; };
  G4RadiatorMaterial *FusedSilica( void ) { return m_FusedSilica; };
  G4Material *Absorber( void )            { return m_Absorber; };

 protected:
  void DefineElements( void );
  void DefineMaterials( void );

  // Basic elements;
  G4Element *m_C, *m_N, *m_O, *m_F, *m_H, *m_Si, *m_K, *m_Na, *m_Sb, *m_Al, *m_Ca;

  // Materials;
  G4Material *m_Air, *m_Absorber, *m_Bialkali, *m_Aluminum, *m_CarbonFiber, *m_Ceramic, *m_Silver, *m_Titanium;
  // Fake carbon to calibrate radiation length of the honeycomb samples; will be created 
  // consisting of pure carbon, density 1g/cm^3; 
  G4Material *m_FakeCarbon_1_g_cm3; 
  // A honeycomb sandwich with epoxy glue (10mil+10mil+1/2"+10mil+10mil), or a 1/4"-based combination;
  G4Material *m_HalfInch_CF_HoneyComb, *m_QuarterInch_CF_HoneyComb;  
  // FR4 imitation with appropriate rad. length;
  G4Material *m_FR4, *m_Water, *m_Copper, *m_Silicon, *m_Delrin, *m_PEEK;

  G4RadiatorMaterial *m_Nitrogen, *m_Acrylic, *m_FusedSilica, *m_C2F6, *m_Sapphire;
  std::map<unsigned, G4RadiatorMaterial*> m_Aerogel;
};

#endif
