
#ifndef _PFRICH_MAGNETIC_FIELD_
#define _PFRICH_MAGNETIC_FIELD_

#include "globals.hh"
#include "G4MagneticField.hh"

#include <epic.h>

#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
#include <BeastMagneticField.h>
#endif

class MagneticField : public G4MagneticField
{
  public:
    MagneticField(const char *fname);
    virtual ~MagneticField() {};
    
    virtual void GetFieldValue(const G4double point[4], double *bField ) const;
    
 private:
#if defined(BMF) && defined(_USE_MAGNETIC_FIELD_)
    BeastMagneticField *m_BMF;
#endif
};

#endif
