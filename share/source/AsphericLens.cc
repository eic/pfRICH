#include <G4Cons.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>

#include <TMath.h>

#define _GEANT_SOURCE_CODE_
#include <CherenkovDetectorCollection.h>
#include <DetectorConstruction.h>

#include <AsphericLens.h>

#include <G4RadiatorMaterial.h>

// -------------------------------------------------------------------------------------

void AsphericLens::Install(CherenkovDetectorCollection *geometry, 
			   DetectorConstruction *detector, CherenkovDetector *cdet, 
			   DarkBox *dbox,
			   double zOffset)
{
  unsigned dim = _ASPHERIC_LENS_SLICE_COUNT_;
  double step = (mDiameter/2)/dim, z[dim+2];

  // Calculate slices;
  for(unsigned iq=0; iq<dim+1; iq++)
    z[iq] = Z(iq*step);
  z[dim+1] = z[dim] + mET;

  // Calculate the central thickness;
  double CT = z[dim+1] - z[0];
  printf("CT %f\n", CT);
  //printf("--> %10.3f\n", Z(12.5)); exit(0);
  
  // Create the (air) container volume; FIXME: margin of 1mm hardcoded;
  //double zc = (z[0] + z[dim+1])/2;
  double q = 1.0;
  //G4RotationMatrix *rX = 0;//new G4RotationMatrix(CLHEP::HepRotationX(TMath::Pi()));
  G4RotationMatrix *rX = new G4RotationMatrix(CLHEP::HepRotationX(TMath::Pi()));
  auto co_tubs = new G4Tubs("QContainer", 0.0, (mDiameter + q)/2, (CT + q)/2, 0.0, 2*TMath::Pi());
  //auto co_log  = new G4LogicalVolume(co_tubs, detector->m_Nitrogen, "Container", 0, 0, 0);
  auto co_log  = new G4LogicalVolume(co_tubs, detector->Nitrogen(), "Container", 0, 0, 0);
  // Yes, lens container volume will be placed at (0,0,0), and lens itself is in the center of it;
  // this way it is much more intuitively clear who sits where; the actual distance between the 
  // window surface and the lens surface can always be recalculated based on the lens CT and the window 
  //auto co_phys = 
  new G4PVPlacement(rX, G4ThreeVector(0, 0, dbox->m_gas_volume_length/2 + zOffset), 
		    co_log, "QContainer", dbox->m_gas_volume_phys->GetLogicalVolume(), false, 0);
  
  //for(unsigned iq=0; iq<dim+2; iq++)
  //printf("--> %10.3f\n", z[iq]);
  //#ifdef _FIRST_LAYERS_BLACK_HOLE_
  //for(unsigned iq=_FIRST_LAYERS_BLACK_HOLE_; iq<dim+1; iq++) {
  //#else
  for(unsigned iq=0; iq<dim+1; iq++) {
    //#endif
    char name[128]; snprintf(name, 128-1, "QRadiator%04d", iq);
    //#ifdef _FIRST_LAYERS_BLACK_HOLE_
    //if (iq == _FIRST_LAYERS_BLACK_HOLE_) snprintf(name, 128-1, "FrontBlackHole");
    //#endif
    //#ifdef _LAST_LAYER_BLACK_HOLE_
    //if (iq == dim) snprintf(name, 128-1, "RearBlackHole");
    //#endif
    
    double z0 = (z[iq+1] + z[iq])/2, dz = z[iq+1] - z[iq];
    auto ra_cons = new G4Cons(name, 0.00*mm, iq*step, 0.00*mm, iq == dim ? iq*step : (iq+1)*step, dz/2, 0.0, 2*TMath::Pi());
    auto ra_log  = new G4LogicalVolume(ra_cons, mMaterial, name, 0, 0, 0);
    //auto ra_phys = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, -CT/2 + z0), ra_log, name, co_log, false, 0);

    geometry->FindOrAddRadiator(cdet, name, ra_log, detector->FusedSilica());
  } //for iq

  // Optional black band on the lens side;
#ifdef _WITH_BLACK_TAPE_ON_LENS_SIDE_
  {
    double rmin = dim*step, rmax = rmin + _PHOTON_DETECTOR_THICKNESS_;
    double z0 = (z[dim+1] + z[dim])/2, dz = z[dim+1] - z[dim]; 
    auto bs_tubs = new G4Tubs("BlackBand", rmin, rmax, dz/2, 0.0, 2*TMath::Pi());
    // FIXME: find a better way to get mSilicon;
    //auto bs_log  = new G4LogicalVolume(bs_tubs, /*detector->mSilicon*/mMaterial, "BlackBand", 0, 0, 0);
    auto bs_log  = new G4LogicalVolume(bs_tubs, detector->Absorber(), "BlackBand", 0, 0, 0);
    //auto bs_phys = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, -CT/2 + z0), bs_log, "BlackBand", co_log, false, 0);
  } 
#endif

#if _LATER_
  // Optional black spot;
#ifdef _BLACK_SPOT_DIAMETER_ 
  {
    auto bs_tubs = new G4Tubs("BlackSpot", 0.0, _BLACK_SPOT_DIAMETER_/2, _PHOTON_DETECTOR_THICKNESS_/2, 0.0, 2*TMath::Pi());
    //auto bs_log  = new G4LogicalVolume(bs_tubs, /*detector->mSilicon*/mMaterial, "BlackSpot", 0, 0, 0);
    auto bs_log  = new G4LogicalVolume(bs_tubs, detector->Absorber(), "BlackSpot", 0, 0, 0);
    //auto bs_phys = 
    //new G4PVPlacement(0, G4ThreeVector(0, 0, _PHOTON_DETECTOR_THICKNESS_/2), 
    new G4PVPlacement(0, G4ThreeVector(0, 0, _PHOTON_DETECTOR_THICKNESS_/2), 
		      bs_log, "BlackSpot", mother, false, 0);
  }
#endif
#endif
} // AsphericLens::Install()

// -------------------------------------------------------------------------------------
