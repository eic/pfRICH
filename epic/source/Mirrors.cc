#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>
#include <G4LogicalBorderSurface.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <epic.h>

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>

#include <EpicDetectorConstruction.h>

// -------------------------------------------------------------------------------------

void EpicDetectorConstruction::DefineMirrors(CherenkovDetector *cdet, DarkBox *dbox, G4VSolid *mother, 
					     G4UnionSolid *flange)
{
  const char *names[2] = {"InnerMirror", "OuterMirror"};
  double mlen = dbox->m_gas_volume_length/2 - m_gzOffset - _BUILDING_BLOCK_CLEARANCE_;
#ifdef _USE_PYRAMIDS_
  mlen -= _BUILDING_BLOCK_CLEARANCE_ + _PYRAMID_MIRROR_HEIGHT_;
#else
  mlen -= _BUILDING_BLOCK_CLEARANCE_ + _HRPPD_SUPPORT_GRID_BAR_HEIGHT_;
#endif
  double mpos = m_gzOffset + mlen/2;
  double r0[2] = {m_r0min, m_r0max}, r1[2] = {_CONICAL_MIRROR_DS_INNER_RADIUS_, _CONICAL_MIRROR_DS_OUTER_RADIUS_};
  //printf("%f %f\n", m_r0min, m_r0max); exit(0);
  
  for(unsigned im=0; im<2; im++) {
    //auto material = im ? m_HalfInch_CF_HoneyComb : m_QuarterInch_CF_HoneyComb;
    auto material = m_CarbonFiber;//QuarterInch_CF_HoneyComb;
    double thickness = im ? _OUTER_MIRROR_THICKNESS_ : _INNER_MIRROR_THICKNESS_;
    //auto mgroup = new CherenkovMirrorGroup();
    
    {
      auto mshape = im ? new G4Cons(names[im], r0[im], r0[im] + thickness, r1[im], r1[im] + thickness, 
				    mlen/2, 0*degree, 360*degree) :
	new G4Cons(names[im], r0[im] - thickness, r0[im], r1[im] - thickness, r1[im], mlen/2, 0*degree, 360*degree);
      
      // There should be a cutaway on the inner mirror because of the beam pipe flange;
      G4LogicalVolume *solid_log = 0;
      if (im) {
	auto solid = new G4IntersectionSolid(names[im], mshape, mother, 0, G4ThreeVector(0.0, 0.0, 0.0));
	solid_log = new G4LogicalVolume(solid, material, names[im], 0, 0, 0);
      } else {
	auto solid = new G4SubtractionSolid(names[im], mshape, flange,  0, G4ThreeVector(0.0, 0.0, 0.0));
	solid_log = new G4LogicalVolume(solid, material, names[im], 0, 0, 0);
      } //if
      
      SetColor(solid_log, G4Colour(0, 0, 1, 0.5));
      
      // NB: geometry will be saved in [mm] throughout the code;
      auto mirror = 
	new ConicalMirror(mshape, material, sign*(1/mm)*TVector3(0.0, 0.0, _FIDUCIAL_VOLUME_OFFSET_ + 
								 m_gas_volume_offset + mpos),
			  sign*TVector3(0,0,1), r0[im]/mm, r1[im]/mm, mlen/mm);
      
      mirror->SetColor(G4Colour(0, 0, 1, 0.5));
      mirror->SetReflectivity(_MIRROR_REFLECTIVITY_, this);
      
      // Mimic mirror->PlaceWedgeCopies() call; FIXME: can be vastly simplified for this simple case;
      mirror->DefineLogicalVolume();
      G4VPhysicalVolume *phys = new G4PVPlacement(/*rZ*/0, G4ThreeVector(0,0,mpos), solid_log,
						  mirror->GetSolid()->GetName(), 
						  dbox->m_gas_volume_phys->GetLogicalVolume(), false, 0);//m_Copies.size());
      mirror->AddCopy(mirror->CreateCopy(phys));
      {
	auto msurface = mirror->GetMirrorSurface();
	
	if (msurface)
	  // Do I really need them separately?;
	  //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
	  new G4LogicalBorderSurface(mirror->GetSolid()->GetName(), dbox->m_gas_volume_phys, phys, msurface);
      } 
      
      auto mcopy = dynamic_cast<SurfaceCopy*>(mirror->GetCopy(0));//m_Copies[iq]);
      mcopy->m_Surface = dynamic_cast<ParametricSurface*>(mirror)->_Clone(0.0, TVector3(0,0,1));
      if (!im) dynamic_cast<ConicalSurface*>(mcopy->m_Surface)->SetConvex();
      
      {
	//mgroup->AddMirror(mirror);
	m_Geometry->AddMirrorLookupEntry(mirror->GetLogicalVolume(), mirror);
	
	auto surface = dynamic_cast<SurfaceCopy*>(mirror->GetCopy(0))->m_Surface;
	dbox->m_mboundaries[im] = 
	  new OpticalBoundary(m_Geometry->FindRadiator(dbox->m_gas_volume_phys->GetLogicalVolume()), surface, false);
	
	// Complete the radiator volume description; this is the rear side of the container gas volume;
	//+? det->GetRadiator("GasVolume")->m_Borders[0].second = surface;
      }
    }

    //cdet->AddOpticalBoundary(CherenkovDetector::Upstream, 0, m_mboundaries[im]);
  } //for im
} // EpicDetectorConstruction::DefineMirrors()

// -------------------------------------------------------------------------------------
