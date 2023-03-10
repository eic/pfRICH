
#include <map>

#ifndef _PFRICH_MATERIALS_
#define _PFRICH_MATERIALS_

#include "G4VUserDetectorConstruction.hh"

class G4Element;
class G4Material;
class G4RadiatorMaterial;

#include <CherenkovWaveLengthRange.h>
  
// Rad. length of the carbon fiber, epoxy glue and honeycomb as provided by 
// Prakhar on 02/25/2023;
#define _CF_RAD_LENGTH_    (23.4*cm)
#define _EG_RAD_LENGTH_    (35.7*cm)
#define _HC_RAD_LENGTH_   (845.4*cm)

#define _CF_THICKNESS_    (10*_MIL_)
#define _EG_THICKNESS_    (10*_MIL_)

#define _FR4_RAD_LENGTH_   (19.4*cm)

class Materials: public CherenkovWaveLengthRange {
public:
  Materials();
  virtual ~Materials() {};

protected:
  void DefineElements( void );
  void DefineMaterials( void );

  // Basic elements;
  G4Element *m_C, *m_N, *m_O, *m_F, *m_H, *m_Si, *m_K, *m_Na, *m_Sb, *m_Al, *m_Ca;

  // Materials;
  G4Material *m_Air, *m_Absorber, *m_Bialkali, *m_Aluminum, *m_CarbonFiber, *m_Ceramic, *m_Silver;
  // Fake carbon to calibrate radiation length of the honeycomb samples; will be created 
  // consisting of pure carbon, density 1g/cm^3; 
  G4Material *m_FakeCarbon_1_g_cm3; 
  // A honeycomb sandwich with epoxy glue (10mil+10mil+1/2"+10mil+10mil), or a 1/4"-based combination;
  G4Material *m_HalfInch_CF_HoneyComb, *m_QuarterInch_CF_HoneyComb;  
  // FR4 imitation with appropriate rad. length;
  G4Material *m_FR4, *m_Water, *m_Copper, *m_Silicon, *m_Delrin, *m_PEEK;

  //G4RadiatorMaterial *m_Aerogel[2], *m_Nitrogen, *m_Acrylic, *m_FusedSilica;
  G4RadiatorMaterial *m_Nitrogen, *m_Acrylic, *m_FusedSilica, *m_C2F6;
  std::map<unsigned, G4RadiatorMaterial*> _m_Aerogel;
};

#endif
