
#include <G4RadiatorMaterial.h>

// FIXME: it seems one needs to renormalize this coefficient in order to obtain a proper effective 
// attenuation length; must have something to do with the cross-section shape; 
// FIXME: need to tune vs the wave length;
#define _RAYLEIGH_SCATTERING_NORMALIZATION_CFF_ (0.50)

// -------------------------------------------------------------------------------------

double G4RadiatorMaterial::RefractiveIndex(double energy) const 
{ 
  // FIXME: how about to check this return code?;
  bool ret;

  // If a "special" Cherenkov table is defined, use it;
  if (m_RIChOptics) return m_RIChOptics->pTable->GetProperty("RINDEX")->GetValue(energy, ret);

  // Otherwise, if a "standard" GEANT4 material table is defined, use it;
  auto mpt = GetMaterialPropertiesTable();
  if (mpt) return mpt->GetProperty("RINDEX")->GetValue(energy, ret);

  // Otherwise resort to using a constant value;
  return m_ConstantRefractiveIndex;
} // G4RadiatorMaterial::RefractiveIndex()

// -------------------------------------------------------------------------------------

double G4RadiatorMaterial::AttenuationLength(double energy) const 
{ 
  bool ret;

  // FIXME: how about to check the return code?;
  double al = m_RIChOptics && m_RIChOptics->pTable->GetProperty("ABSLENGTH")? 
    m_RIChOptics->pTable->GetProperty("ABSLENGTH")->GetValue(energy, ret) : 0.0;
  double rs = m_RIChOptics && m_RIChOptics->pTable->GetProperty("RAYLEIGH") ? 
    m_RIChOptics->pTable->GetProperty("RAYLEIGH") ->GetValue(energy, ret) : 0.0;
  
  rs *= _RAYLEIGH_SCATTERING_NORMALIZATION_CFF_;
  if (al && rs) return al*rs/(al+rs);

  return (al ? al : rs);
} // G4RadiatorMaterial::AttenuationLength()

// -------------------------------------------------------------------------------------
