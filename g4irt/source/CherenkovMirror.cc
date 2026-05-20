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
void CherenkovMirror::SetReflectivity()
{
  std::cout<<"Set up reflectivity curve for the pyramid mirrors"<<std::endl;
  // Mirror surface; assume a metal one with a constant reflectivity;
  m_MirrorSurface = new G4OpticalSurface(m_Solid->GetName());
  m_MirrorSurface->SetType(dielectric_metal);
  
  // NB: assume specular reflection, right?;
  m_MirrorSurface->SetFinish(polished);
  m_MirrorSurface->SetModel(unified);

  int WLDim=121;

  double E[WLDim]={
    1.392, 1.401, 1.409, 1.418, 1.428, 1.437, 1.446, 1.456, 1.465, 1.475, 
    1.485, 1.495, 1.505, 1.515, 1.525, 1.536, 1.547, 1.557, 1.568, 1.580, 
    1.591, 1.602, 1.614, 1.626, 1.638, 1.650, 1.662, 1.675, 1.687, 1.700, 
    1.713, 1.727, 1.740, 1.754, 1.768, 1.782, 1.796, 1.811, 1.826, 1.841, 
    1.856, 1.872, 1.888, 1.904, 1.920, 1.937, 1.954, 1.971, 1.989, 2.007, 
    2.025, 2.044, 2.063, 2.082, 2.101, 2.121, 2.142, 2.163, 2.184, 2.206, 
    2.228, 2.250, 2.273, 2.297, 2.321, 2.345, 2.370, 2.395, 2.422, 2.448, 
    2.475, 2.503, 2.532, 2.561, 2.591, 2.621, 2.652, 2.684, 2.717, 2.751, 
    2.785, 2.820, 2.857, 2.894, 2.932, 2.971, 3.011, 3.053, 3.095, 3.139, 
    3.183, 3.230, 3.277, 3.326, 3.377, 3.429, 3.482, 3.538, 3.595, 3.654, 
    3.715, 3.778, 3.843, 3.911, 3.981, 4.053, 4.128, 4.206, 4.287, 4.371, 
    4.459, 4.550, 4.645, 4.744, 4.847, 4.955, 5.068, 5.186, 5.310, 5.440, 
    5.576};
  
  double reflectivity[WLDim]={
    0.836, 0.829, 0.833, 0.834, 0.837, 0.838, 0.839, 0.838, 0.838, 0.832, 
    0.833, 0.832, 0.825, 0.794, 0.823, 0.823, 0.819, 0.815, 0.811, 0.820, 
    0.815, 0.816, 0.818, 0.812, 0.824, 0.818, 0.815, 0.819, 0.815, 0.814, 
    0.825, 0.824, 0.822, 0.826, 0.826, 0.833, 0.828, 0.827, 0.833, 0.830, 
    0.831, 0.834, 0.838, 0.829, 0.830, 0.827, 0.832, 0.831, 0.833, 0.833, 
    0.831, 0.835, 0.831, 0.834, 0.835, 0.836, 0.831, 0.827, 0.824, 0.817, 
    0.821, 0.818, 0.820, 0.821, 0.818, 0.816, 0.816, 0.813, 0.814, 0.814, 
    0.822, 0.822, 0.823, 0.817, 0.816, 0.811, 0.815, 0.814, 0.762, 0.818, 
    0.819, 0.819, 0.824, 0.822, 0.785, 0.819, 0.822, 0.822, 0.824, 0.819, 
    0.824, 0.825, 0.813, 0.812, 0.810, 0.792, 0.785, 0.741, 0.760, 0.752, 
    0.751, 0.738, 0.727, 0.739, 0.726, 0.726, 0.715, 0.702, 0.699, 0.690, 
    0.673, 0.660, 0.645, 0.634, 0.610, 0.593, 0.553, 0.541, 0.482, 0.207, 
    0.120};
  
  G4MaterialPropertiesTable *mirrorST = new G4MaterialPropertiesTable();
  mirrorST->AddProperty("REFLECTIVITY", E, reflectivity, WLDim);
  m_MirrorSurface->SetMaterialPropertiesTable(mirrorST);
}
