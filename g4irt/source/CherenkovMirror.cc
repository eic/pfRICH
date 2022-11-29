#include "G4LogicalBorderSurface.hh"

#define _GEANT_SOURCE_CODE_
#include <CherenkovMirror.h>
#include <CherenkovWaveLengthRange.h>
#include <G4RadiatorMaterial.h>

// -------------------------------------------------------------------------------------

void CherenkovMirror::SetReflectivity(double reflectivity, CherenkovWaveLengthRange *wlrange)
{
  // Mirror surface; assume a metal one with a constant reflectivity;
  m_MirrorSurface = new G4OpticalSurface(m_Solid->GetName());
  m_MirrorSurface->SetType(dielectric_metal);
  // NB: assume specular reflection, right?;
  m_MirrorSurface->SetFinish(polished);
  m_MirrorSurface->SetModel(unified);    
    
  unsigned WLDim = wlrange->GetWLDim();

  G4double table[WLDim];
  for(unsigned iq=0; iq<WLDim; iq++)
    table[iq] = reflectivity;
  
  G4MaterialPropertiesTable *mirrorST = new G4MaterialPropertiesTable();
  mirrorST->AddProperty("REFLECTIVITY", wlrange->GetPhotonEnergies(), table, WLDim);
  m_MirrorSurface->SetMaterialPropertiesTable(mirrorST);
} // CherenkovMirror::SetReflectivity()

// -------------------------------------------------------------------------------------

