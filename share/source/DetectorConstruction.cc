
#include "G4Box.hh"
#include "G4Trd.hh"

#include "G4PVPlacement.hh"
#include <G4SubtractionSolid.hh>
#include <G4LogicalBorderSurface.hh>

#define _GEANT_SOURCE_CODE_
//#include <G4Object.h>

#include <hrppd.h>
#include <share.h>

#include "DetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>

// -------------------------------------------------------------------------------------

DetectorConstruction::DetectorConstruction(CherenkovDetectorCollection *geometry): 
  G4VUserDetectorConstruction(),
  m_Geometry(geometry)
{
} // DetectorConstruction::DetectorConstruction()

// -------------------------------------------------------------------------------------

G4OpticalSurface *DetectorConstruction::CreateLambertianMirrorSurface(const char *name, 
								      double reflectivity, double roughness)
{
  G4OpticalSurface* opSurface = new G4OpticalSurface(name);
  opSurface->SetType(dielectric_metal);
	
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();

  if (roughness) {
    // 'ground': yes, I need specular lobe reflection in this case;
    opSurface->SetFinish(ground);
    opSurface->SetModel(unified);
    // NB: Fresnel refraction (or specular lobe reflection) will be sampled with respect 
    // to the surface with a bit "smeared" normal direction vector; 
    opSurface->SetSigmaAlpha(roughness);
    
    G4double specularLobe[_WLDIM_];
    
    for(int iq=0; iq<_WLDIM_; iq++) 
      // NB: yes, it is only a specular lobe reflection here;
      specularLobe[iq] = 1.00;
    
    mpt->AddProperty("SPECULARLOBECONSTANT", GetPhotonEnergies(), specularLobe, _WLDIM_);
  }
  else
    // 'polished': pure Fresnel reflection;
    opSurface->SetFinish(polished);

  G4double reflectivity_array[_WLDIM_];
  for(int iq=0; iq<_WLDIM_; iq++) 
    reflectivity_array[iq] = reflectivity;
  mpt->AddProperty("REFLECTIVITY", GetPhotonEnergies(), reflectivity_array, _WLDIM_);
  opSurface->SetMaterialPropertiesTable(mpt);

  return opSurface;
} // DetectorConstruction::CreateLambertianMirrorSurface()

// -------------------------------------------------------------------------------------

void DetectorConstruction::BuildPhotonDetectorMatrix(CherenkovDetector *cdet, DarkBox *dbox, 
						     double fvzOffset, double wzOffset, 
						      const std::vector<MisalignedLocation2D> &xycoord)
{
  double pdthick = 0.01*mm, zpdc = wzOffset + _HRPPD_WINDOW_THICKNESS_ + pdthick/2;
  G4Box *pd_box  = new G4Box("PhotoDetector", _HRPPD_ACTIVE_AREA_SIZE_/2, _HRPPD_ACTIVE_AREA_SIZE_/2, pdthick/2);
  auto pd = new CherenkovPhotonDetector(pd_box, m_Bialkali);

  // Full size quartz window; FIXME: Sapphire, here and in all other places;
  auto wnd_box = new G4Box("QuartzWindow", _HRPPD_TILE_SIZE_/2, _HRPPD_TILE_SIZE_/2, _HRPPD_WINDOW_THICKNESS_/2);
  auto wnd_log = new G4LogicalVolume(wnd_box, _HRPPD_WINDOW_MATERIAL_,  "QuartzWindow", 0, 0, 0);
  auto hrppd_log = BuildHRPPD(wnd_log, pd_box, pd);

  {	
    TVector3 nx(1*sign,0,0), ny(0,-1,0);
    
    // A single entry; this assumes of course that all the windows are at the same Z, and parallel to each other;
    auto surface = 
      new FlatSurface(sign*(1/mm)*TVector3(0,0,fvzOffset + wzOffset + _HRPPD_WINDOW_THICKNESS_/2), nx, ny);

    m_Geometry->AddFlatRadiator(cdet, "QuartzWindow", CherenkovDetector::Downstream, 
				0, wnd_log, m_FusedSilica, surface, _HRPPD_WINDOW_THICKNESS_/mm)
#ifdef _DISABLE_HRPPD_WINDOW_PHOTONS_
      ->DisableOpticalPhotonGeneration()
#endif
      ;
  }	

  CherenkovMirror *pyramid = 0;
  if (dbox->m_UsePyramids) {
    auto pyra_box = new G4Box("Dummy", dbox->m_PyramidMirrorWidth/2, dbox->m_PyramidMirrorWidth/2, dbox->m_PyramidMirrorHeight/2);
    auto pyra_cut = new G4Trd("Dummy", dbox->m_PyramidMirrorWidth/2, _HRPPD_ACTIVE_AREA_SIZE_/2, dbox->m_PyramidMirrorWidth/2, 
			      // FIXME: a mismatch of 11um extra does not hurt IRT ray tracing; NB: contrary to 10um, 11um) does 
			      // not cause a warning message "BooleanProcessor::createPolyhedron : too many edges" message in 
			      // the GEANT Qt display; FIXME: why??!;
				_HRPPD_ACTIVE_AREA_SIZE_/2, dbox->m_PyramidMirrorHeight/2 + 0.011*mm);
    auto *pyra_shape = new G4SubtractionSolid("MirrorPyramid", pyra_box, pyra_cut);
    // NB: geometry will be saved in [mm] throughout the code;
    pyramid = new CherenkovMirror(pyra_shape, m_Absorber);
    
    pyramid->SetColor(G4Colour(0, 1, 1, 0.5));
    pyramid->SetReflectivity(_MIRROR_REFLECTIVITY_, this);
    
    // Mimic the essential part of the mirror->PlaceWedgeCopies() call;
    pyramid->DefineLogicalVolume();
    m_Geometry->AddMirrorLookupEntry(pyramid->GetLogicalVolume(), pyramid);
  } //if
  
  m_Geometry->AddPhotonDetector(cdet, pd->GetLogicalVolume(), pd);
   
  {
    int counter = 0;
    
    for(auto xyptr: xycoord) {
      new G4PVPlacement(0, G4ThreeVector(xyptr.m_X, xyptr.m_Y, wzOffset + _HRPPD_CONTAINER_VOLUME_HEIGHT_/2), 
			hrppd_log, "HRPPD", dbox->m_fiducial_volume_phys->GetLogicalVolume(), false, counter);
      
      // Photocathode surface;
      auto surface = new FlatSurface((1/mm)*TVector3(sign*xyptr.m_X, xyptr.m_Y,sign*(fvzOffset + zpdc)), 
				     TVector3(1*sign,0,0), TVector3(0,-1,0));
      
      // FIXME: just remove four pyrapid assemblies around the beam pipe completely;
      // this is suboptimal -> do it better later;
      if (dbox->m_UsePyramids && xyptr.m_Pyramid) {
	// FIXME: need to take care about overlap with the inner wall;
	auto pyra_phys =  
	  new G4PVPlacement(0, G4ThreeVector(xyptr.m_X, xyptr.m_Y, dbox->m_gas_volume_length/2 - dbox->m_PyramidMirrorHeight/2), 
			    pyramid->GetLogicalVolume(), pyramid->GetSolid()->GetName(), 
			    dbox->m_gas_volume_phys->GetLogicalVolume(), false, counter);
	
	new G4LogicalBorderSurface(pyramid->GetSolid()->GetName(), dbox->m_gas_volume_phys, pyra_phys, pyramid->GetMirrorSurface());
      } //if
      
      {
	// Calculate and store the pyramid mirror flat surfaces; 
	OpticalBoundary *pboundaries[4] = {0, 0, 0, 0,};
	if (dbox->m_UsePyramids && dbox->m_UsePyramidOptics) 
	  for(unsigned iq=0; iq<4; iq++)
	    switch (iq) {
	    case 0:;
	    case 1:;
	    case 2:;
	    case 3:;
	      // Reflection off one of the four pyramid mirrors;
	      {		  
		// FIXME: only true for no-gap mirror installation?;
		double vx = (dbox->m_PyramidMirrorWidth - _HRPPD_ACTIVE_AREA_SIZE_)/2;
		double vy = dbox->m_PyramidMirrorHeight, norm = sqrt(vx*vx+vy*vy);
		double z0 = fvzOffset + wzOffset - dbox->m_PyramidMirrorHeight/2;
		
		TVector3 nx( vx/norm, 0, -vy/norm), ny(0,-1,0), nz(0,0,1), nv(0,1,0);
		nx.Rotate(iq*M_PI/2, nz); ny.Rotate(iq*M_PI/2, nz);
		
		TVector3 center((dbox->m_PyramidMirrorWidth + _HRPPD_ACTIVE_AREA_SIZE_)/4, 0.0, z0); 
		center.Rotate(iq*M_PI/2, nz); center += TVector3(xyptr.m_X, xyptr.m_Y, 0.0);
		if (flip) {
		  center.Rotate(M_PI, nv);
		  nx.Rotate(M_PI, nv);
		  ny.Rotate(M_PI, nv);
		} //if
		auto qsurface = new FlatSurface((1/mm)*center, nx, ny);
		
		auto boundary = 
		  new OpticalBoundary(m_Geometry->FindRadiator(dbox->m_gas_volume_phys->GetLogicalVolume()), qsurface, false);
		pboundaries[iq] = boundary;
		cdet->StoreOpticalBoundary(boundary);
	      }
	      break;
	    } //if .. for iq .. switch 
	
	// Four "global" optical configurations considered, see the first switch below;
	for(unsigned iq=0; iq<4; iq++) {
	  //#ifndef _USE_CONICAL_MIRROR_OPTICS_
	  //if (iq) continue;
	  //#endif
	  if (!dbox->m_UseConicalMirrorOptics && iq) continue;

	  // And five more cases at the sensor end, see the second switch below;
	  for(unsigned ip=0; ip<5; ip++) {
	    if ((!dbox->m_UsePyramids || !dbox->m_UsePyramidOptics) && ip != 4) continue;

	    // Mimic det->CreatePhotonDetectorInstance();
	    unsigned sector = 0, icopy = counter;
	    auto irt = pd->AllocateIRT(sector, icopy);
	    
	    // Aerogel and acrylic;
	    if (cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].find(sector) != 
		cdet->m_OpticalBoundaries[CherenkovDetector::Upstream].end())
	      for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Upstream][sector])
		irt->AddOpticalBoundary(boundary);
	    
	    // FIXME: will the gas-quartz boundary be described correctly in this sequence?;
	    switch (iq) {
	    case 0: 
	      // Direct hit;
	      break;
	    case 1:
	    case 2:         
	      // Reflection on either inner or outer mirrors;
	      irt->AddOpticalBoundary(dbox->m_mboundaries[iq-1]);
	      break;
	    case 3:
	      // Reflection on outer, then on inner mirror; happens at large angles; if the pyramids are
	      // too high, these photons will undergo more reflections, and cannot be saved;
	      irt->AddOpticalBoundary(dbox->m_mboundaries[1]);
	      irt->AddOpticalBoundary(dbox->m_mboundaries[0]);
	      break;
	    } //switch
	    
	    if (dbox->m_UsePyramids && dbox->m_UsePyramidOptics) 
	      switch (ip) {
	      case 0:;
	      case 1:;
	      case 2:;
	      case 3:;
		irt->AddOpticalBoundary(pboundaries[ip]);
		break;
	      case 4: 
		// Direct hit;
		break;
	      } //if .. switch
	    
	    // Fused silica windows;
	    if (cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].find(sector) != 
		cdet->m_OpticalBoundaries[CherenkovDetector::Downstream].end())
	      for(auto boundary: cdet->m_OpticalBoundaries[CherenkovDetector::Downstream][sector])
		irt->AddOpticalBoundary(boundary);
	    
	    // Terminate the optical path;
	    pd->AddItselfToOpticalBoundaries(irt, surface);
	  }
	} //for ip
      } //for iq
      
      counter++;
    } //for xy
  } 
  
  if (dbox->m_UseConicalMirrorOptics)
    for(unsigned im=0; im<2; im++)
      // FIXME: they are not really upstream (just need to store them);
      cdet->AddOpticalBoundary(CherenkovDetector::Upstream, 0, dbox->m_mboundaries[im]);

  for(auto radiator: cdet->Radiators())
    radiator.second->SetReferenceRefractiveIndex(radiator.second->GetMaterial()->RefractiveIndex(eV*_MAGIC_CFF_/_LAMBDA_NOMINAL_));
} // DetectorConstruction::BuildPhotonDetectorMatrix()

// -------------------------------------------------------------------------------------

void DetectorConstruction::SetColor(G4LogicalVolume *lvol, const G4Colour &color, 
				    bool visible, bool solid)
{
  G4VisAttributes* visAtt = new G4VisAttributes(color);

  visAtt->SetVisibility(visible);
  visAtt->SetForceSolid(solid);
	
  lvol->SetVisAttributes(visAtt);
} // DetectorConstruction::SetColor()

// -------------------------------------------------------------------------------------
