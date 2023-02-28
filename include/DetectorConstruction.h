
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
  void BuildVesselWalls( void );

private:
  CherenkovDetectorCollection *m_Geometry;

  double m_gas_volume_length, m_gas_volume_radius;
  G4LogicalVolume *m_fiducial_volume_log, *m_gas_volume_log;

  G4OpticalSurface *CreateLambertianMirrorSurface(const char *name, 
						  double reflectivity = 1.0, double roughness = 0.0);
};

#endif
