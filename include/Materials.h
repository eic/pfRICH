
#include <map>

#ifndef _PFRICH_MATERIALS_
#define _PFRICH_MATERIALS_

#include "G4VUserDetectorConstruction.hh"

class G4Element;
class G4Material;
class G4RadiatorMaterial;

#include <CherenkovWaveLengthRange.h>
  
class Materials: public CherenkovWaveLengthRange {
public:
  Materials();
  virtual ~Materials() {};

protected:
  void DefineElements( void );
  void DefineMaterials( void );

  // Basic elements;
  G4Element *m_C, *m_N, *m_O, *m_H, *m_Si, *m_K, *m_Na, *m_Sb;

  // Materials;
  G4Material *m_Air, *m_Absorber, *m_Bialkali;
  //G4RadiatorMaterial *m_Aerogel[2], *m_Nitrogen, *m_Acrylic, *m_FusedSilica;
  G4RadiatorMaterial *m_Nitrogen, *m_Acrylic, *m_FusedSilica;
  std::map<unsigned, G4RadiatorMaterial*> _m_Aerogel;
};

#endif
