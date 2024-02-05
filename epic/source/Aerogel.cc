#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include <G4DataInterpolation.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

#include <epic.h>

#include <CherenkovDetectorCollection.h>
#include <G4RadiatorMaterial.h>

#include <EpicDetectorConstruction.h>

// -------------------------------------------------------------------------------------

void EpicDetectorConstruction::DefineAerogel(CherenkovDetector *cdet, DarkBox *dbox, G4UnionSolid *flange)
{
  auto gvmother = dbox->m_gas_volume_phys->GetLogicalVolume();

  // FIXME: yes, for now hardcoded; FIXME: GDML export does not like 8 pieces in the inner layer.
  // namely it produces a visible spike in xray.C scan at 45 degrees; ok, make them 9;
  const unsigned adim[_AEROGEL_BAND_COUNT_] = {9, 14, 20};
  double rheight = (m_r0max - m_r0min - (_AEROGEL_BAND_COUNT_-1)*_AEROGEL_SEPARATOR_WALL_THICKNESS_ - 
		    _AEROGEL_INNER_WALL_THICKNESS_ - _AEROGEL_OUTER_WALL_THICKNESS_) / _AEROGEL_BAND_COUNT_;
  
  // Up to two aerogel layers;
  for(unsigned il=0; il<2; il++) {
    double agthick = 0.0;
    G4RadiatorMaterial *aerogel = 0;
#ifdef _AEROGEL_1_ 
    if (!il) {
      agthick = _AEROGEL_THICKNESS_1_;
      aerogel   = m_Aerogel[_AEROGEL_1_];
    } //if
#else
    if (!il) continue;
#endif
#ifdef _AEROGEL_2_ 
    if ( il) {
      agthick = _AEROGEL_THICKNESS_2_;
      aerogel = m_Aerogel[_AEROGEL_2_];
    } //if
#else
    if ( il) continue;
#endif
    
    {
      m_gzOffset += agthick/2;
      
      // First aerogel sectors and azimuthal spacers;
      CherenkovRadiator *radiator = 0;
      for(unsigned ir=0; ir<_AEROGEL_BAND_COUNT_; ir++) {
	int counter = ir ? -1 : 0;
	double apitch = 360*degree / adim[ir];
	double r0 = m_r0min + _AEROGEL_INNER_WALL_THICKNESS_ + ir*(_AEROGEL_SEPARATOR_WALL_THICKNESS_ + rheight);
	double r1 = r0 + rheight, rm = (r0+r1)/2;
	
	// Calculate angular space occupied by the spacers and by the tiles; no gas gaps for now;
	// assume that a wegde shape is good enough (GEANT visualization does not like boolean objects), 
	// rather than creating constant thicjkess azimuthal spacers; just assume that spacer thickness is 
	// _AEROGEL_FRAME_WALL_THICKNESS_ at r=rm;
	double l0 = 2*M_PI*rm/adim[ir], l1 = _AEROGEL_SEPARATOR_WALL_THICKNESS_, lsum = l0 + l1;
	
	// FIXME: names overlap in several places!;
	double wd0 = (l0/lsum)*(360*degree / adim[ir]), wd1 = (l1/lsum)*(360*degree / adim[ir]);
	TString ag_name = "Tmp", sp_name = "Tmp"; 
	if (ir) ag_name.Form("%s-%d-00", aerogel->GetName().c_str(), ir);
	if (ir) sp_name.Form("A-Spacer--%d-00",                      ir);
	G4Tubs *ag_tube  = new G4Tubs(ag_name.Data(), r0, r1, agthick/2, 0*degree, wd0);
	G4Tubs *sp_tube  = new G4Tubs(sp_name.Data(), r0, r1, agthick/2,      wd0, wd1);
	
	for(unsigned ia=0; ia<adim[ir]; ia++) {
	  G4RotationMatrix *rZ    = new G4RotationMatrix(CLHEP::HepRotationZ(    ia*apitch));
	  G4RotationMatrix *rZinv = new G4RotationMatrix(CLHEP::HepRotationZ(-1.*ia*apitch));
	  
	  G4LogicalVolume *ag_log = 0, *sp_log = 0;
	  if (ir) {
	    ag_log = new G4LogicalVolume(ag_tube,                   aerogel, ag_name.Data(), 0, 0, 0);
	    sp_log = new G4LogicalVolume(sp_tube, _AEROGEL_SPACER_MATERIAL_, sp_name.Data(), 0, 0, 0);
	    counter++;
	  } else {
	    ag_name.Form("%s-%d-%02d", aerogel->GetName().c_str(), ir, ia);
	    auto ag_shape = new G4SubtractionSolid(ag_name.Data(), ag_tube, flange, 
						   rZinv, G4ThreeVector(0.0, 0.0, 0.0));
	    ag_log = new G4LogicalVolume(ag_shape, aerogel, ag_name.Data(),   0, 0, 0);
	    
	    sp_name.Form("A-Spacer--%d-%02d",                      ir, ia);
	    auto sp_shape = new G4SubtractionSolid(sp_name.Data(), sp_tube, flange, 
						   rZinv, G4ThreeVector(0.0, 0.0, 0.0));
	    sp_log = new G4LogicalVolume(sp_shape, _AEROGEL_SPACER_MATERIAL_, sp_name.Data(),   0, 0, 0);
	  } //if
	  if (!ir && !ia) {
	    TVector3 nx(1*sign,0,0), ny(0,-1,0);
	    
	    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0,0,_FIDUCIAL_VOLUME_OFFSET_ + 
								m_gas_volume_offset + m_gzOffset), nx, ny);
	    radiator = m_Geometry->AddFlatRadiator(cdet, aerogel->GetName(), CherenkovDetector::Upstream, 
						   0, ag_log, aerogel, surface, agthick/mm);
#ifdef _DISABLE_AEROGEL_PHOTONS_
	    radiator->DisableOpticalPhotonGeneration();
#endif
	  }
	  else
	    // This of course assumes that optical surfaces are the same (no relative tilts between bands, etc);
	    m_Geometry->AddRadiatorLogicalVolume(radiator, ag_log);
	  
	  new G4PVPlacement(rZ, G4ThreeVector(0.0, 0.0, m_gzOffset), ag_log, ag_name.Data(), 
			    gvmother, false, counter);
	  new G4PVPlacement(rZ, G4ThreeVector(0.0, 0.0, m_gzOffset), sp_log, sp_name.Data(), 
			    gvmother, false, counter);
	} //for ia
      } //for ir
      
      // Then the radial spacers;
      {
	double accu = m_r0min;
	
	for(unsigned ir=0; ir<_AEROGEL_BAND_COUNT_+1; ir++) {
	  double thickness = ir ? (ir == _AEROGEL_BAND_COUNT_ ? _AEROGEL_OUTER_WALL_THICKNESS_ : 
				   _AEROGEL_SEPARATOR_WALL_THICKNESS_) : _AEROGEL_INNER_WALL_THICKNESS_;
	  double r0 = accu, r1 = r0 + thickness;
	  
	  TString sp_name = "Tmp"; if (ir) sp_name.Form("R-Spacer--%d-00", ir);
	  
	  G4Tubs *sp_tube  = new G4Tubs(sp_name.Data(), r0, r1, agthick/2, 0*degree, 360*degree);
	  
	  G4LogicalVolume *sp_log = 0;
	  if (ir) 
	    sp_log = new G4LogicalVolume(sp_tube, _AEROGEL_SPACER_MATERIAL_, sp_name.Data(), 0, 0, 0);
	  else {
	    sp_name.Form("R-Spacer--%d-00", ir);
	    auto sp_shape = new G4SubtractionSolid(sp_name.Data(), sp_tube, flange, 0, G4ThreeVector(0.0, 0.0, 0.0));
	    sp_log = new G4LogicalVolume(sp_shape, _AEROGEL_SPACER_MATERIAL_, sp_name.Data(),   0, 0, 0);
	  } //if
	  
	  new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, m_gzOffset), sp_log, sp_name.Data(), 
			    gvmother, false, 0);
	  
	  accu += thickness + rheight;
	} //for ir
      }  
      
      // FIXME: not really needed that big between the two layers?;
      m_gzOffset += agthick/2 + _BUILDING_BLOCK_CLEARANCE_;
    }
  } //for il
} // EpicDetectorConstruction::DefineAerogel()

// -------------------------------------------------------------------------------------
