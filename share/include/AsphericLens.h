#include <math.h>

class G4Material;
class DetectorConstruction;
class CherenkovDetector;
class CherenkovDetectorCollection;

#ifndef _ASPHERIC_LENS_
#define _ASPHERIC_LENS_

// 300 conical slices is a good enough approximation;
#define _ASPHERIC_LENS_SLICE_COUNT_ 300

// If uncommented, absorb photons which reach this side surface; 
// FIXME: should probably implement a diffuse scattering?;
#define _WITH_BLACK_TAPE_ON_LENS_SIDE_
#define _PHOTON_DETECTOR_THICKNESS_ (0.1*mm)

// Uncomment if want to kill photons which reach either front or rear side of the lens;
// FIXME: and how is it supposed to work for the tip slice?!;
//#define _FIRST_LAYERS_BLACK_HOLE_ 100
//#define _LAST_LAYER_BLACK_HOLE_   

// Uncomment if want to place a black blinding circle in the center;
//#define _BLACK_SPOT_DIAMETER_ (25.0)

class AsphericLens {
 public:
 AsphericLens(G4Material *material, double dia, double cr, double k, double d, double e, double f, double g,
              double h, double j, double l, double et): mMaterial(material), mDiameter(dia), 
    mConvexRadius(cr), mK(k), mD(d), mE(e), mF(f), mG(g), mH(h), mJ(j), mL(l), mET(et) {};
  ~AsphericLens() {};

  double Z(double Y) const {
    return (Y*Y/mConvexRadius)/(1.0 + sqrt(1.0 - (1.0+mK)*Y*Y/(mConvexRadius*mConvexRadius))) + 
      mD*pow(Y,2) + mE*pow(Y,4) + mF*pow(Y,6) + mG*pow(Y,8) + mH*pow(Y,10) + mJ*pow(Y,12) + mL*pow(Y,14);
  };

  void Install(CherenkovDetectorCollection *geometry,
	       DetectorConstruction *detector, CherenkovDetector *cdet, DarkBox *dbox,
	       double zOffset);

  G4Material *mMaterial;
  double mDiameter, mConvexRadius, mK, mD, mE, mF, mG, mH, mJ, mL, mET;
};

#endif



