
#include <vector>

#ifndef _PFRICH_DETECTOR_CONSTRUCTION_
#define _PFRICH_DETECTOR_CONSTRUCTION_

#include "G4VUserDetectorConstruction.hh"

#include "Materials.h"

class G4Colour;
class G4PVPlacement;
class G4Box;
class G4OpticalSurface;

class CherenkovDetector;
class CherenkovDetectorCollection;
class OpticalBoundary;
class CherenkovPhotonDetector;

struct MisalignedLocation2D {
public:
MisalignedLocation2D(double x, double y, double dx, double dy, double phi, bool pyramid): 
  m_X(x), m_Y(y), m_dX(dx), m_dY(dy), m_Phi(phi), m_Pyramid(pyramid) {};

  // Nominal HRPPD location in the sensor plane (the one which defines a pyramid mirror 
  // set location), as well as small XY-offsets and a Z-rotation;
  double m_X, m_Y, m_dX, m_dY, m_Phi;

  // Well, pack this one here as well; eventually pyramid shapes can be made different;
  bool m_Pyramid;
};

struct DarkBox {
public:
DarkBox(bool pyramids, bool poptics, bool cmoptics): 
  m_fiducial_volume_phys(0), m_gas_volume_phys(0), m_gas_volume_length(0.0), 
    m_PyramidMirrorWidth(0.0), m_PyramidMirrorHeight(0.0), 
    m_UsePyramids(pyramids), m_UsePyramidOptics(poptics), m_UseConicalMirrorOptics(cmoptics) {
  // Optical boundaries of the inner and outer conical mirrors; do not want to spoil the 
  // central area (mirro-less) optical path (would happen if define them in situ);
  m_mboundaries[0] = m_mboundaries[1] = 0;
};
  ~DarkBox() {};

  G4PVPlacement *m_fiducial_volume_phys, *m_gas_volume_phys;
  double m_gas_volume_length;

  OpticalBoundary *m_mboundaries[2];

  void DefinePyramidMirrorGeometry(double width, double height) { 
    m_PyramidMirrorWidth  = width; 
    m_PyramidMirrorHeight = height; 
  };
  double m_PyramidMirrorWidth, m_PyramidMirrorHeight;
  bool m_UsePyramids, m_UsePyramidOptics, m_UseConicalMirrorOptics;
};

class DetectorConstruction : public Materials, public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(CherenkovDetectorCollection *geometry);
  virtual ~DetectorConstruction() {};

 protected:
  //virtual G4VPhysicalVolume* Construct( void );

  G4OpticalSurface *CreateLambertianMirrorSurface(const char *name, 
						  double reflectivity = 1.0, double roughness = 0.0);

  void SetColor(G4LogicalVolume *lvol, const G4Colour &color, bool visible = true, bool solid = true);
  
  CherenkovDetectorCollection *m_Geometry;

  void BuildPhotonDetectorMatrix(CherenkovDetector *cdet, DarkBox *dbox, double fvzOffset, double wzOffset, 
				 const std::vector<MisalignedLocation2D> &xycoord);

  std::vector<DarkBox*> m_DarkBoxes;

 private:
  G4LogicalVolume *BuildHRPPD(G4LogicalVolume *wnd_log, G4Box *pd_box, CherenkovPhotonDetector *pd);
};

#endif
