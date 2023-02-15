
#ifndef _PFRICH_DETECTOR_CONSTRUCTION_
#define _PFRICH_DETECTOR_CONSTRUCTION_

#include "Materials.h"

class G4OpticalSurface;
class CherenkovDetectorCollection;
  
class DetectorConstruction : public Materials, public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~DetectorConstruction() {};

  void ConstructSDandField();

  G4VPhysicalVolume* Construct( void );

private:
  CherenkovDetectorCollection *m_Geometry;

  G4OpticalSurface *CreateLambertianMirrorSurface(const char *name, 
						  double reflectivity = 1.0, double roughness = 0.0);
};

#endif
