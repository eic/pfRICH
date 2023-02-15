
#include <MagneticField.h>

// -------------------------------------------------------------------------------------

MagneticField::MagneticField(const char *fname)
{
#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
  m_BMF = new BeastMagneticField(fname);
#endif
} // MagneticField::MagneticField()

// -------------------------------------------------------------------------------------

void MagneticField::GetFieldValue(const G4double point[4], double *bField ) const
{
#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
  double bx, by, bz;
  /*bool ret =*/ m_BMF->GetFieldValue(point[0]/cm, point[1]/cm, point[2]/cm, bx, by, bz);
  
  bField[0] = bx*tesla; bField[1] = by*tesla; bField[2] = bz*tesla; 
#endif
} // MagneticField::GetFieldValue()

// -------------------------------------------------------------------------------------
