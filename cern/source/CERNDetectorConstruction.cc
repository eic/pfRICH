
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

//#include <share.h>
#include <hrppd.h>
#include <cern.h>

#include "CERNDetectorConstruction.h"

#include <CherenkovDetectorCollection.h>
#include <CherenkovRadiator.h>
#include <G4RadiatorMaterial.h>
#include <CherenkovMirror.h>
#include <CherenkovPhotonDetector.h>
#include <AsphericLens.h>

// -------------------------------------------------------------------------------------

CERNDetectorConstruction::CERNDetectorConstruction(CherenkovDetectorCollection *geometry,std::string AerogelTag): 
  DetectorConstruction(geometry,AerogelTag),
  aerogelTag(AerogelTag)
{
} // CERNDetectorConstruction::CERNDetectorConstruction()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume *CERNDetectorConstruction::Construct( void )
{
  //----------------------------
  // Chemical elements and materials;
  //----------------------------
  DefineElements();
  DefineMaterials(aerogelTag);  //DefineMaterials(aerogelTag, ri3, ri4);

  //----------------------------
  // The experimental hall;
  // FIXME: hardcoded;
  //----------------------------
  auto expHall_box = new G4Box("PFRICH_World",  200*cm/2, 200*cm/2, 400*cm/2);
  auto expHall_log = new G4LogicalVolume(expHall_box, m_Air, "PFRICH_World", 0, 0, 0);
  auto expHall_phys = new G4PVPlacement(0, G4ThreeVector(), expHall_log, "PFRICH_World", 0, false, 0);
    
  //G4RotationMatrix *rX = new G4RotationMatrix(CLHEP::HepRotationX(TMath::Pi()));

  //----------------------------
  // box
  //----------------------------
  for(unsigned idt=0; idt<2; idt++) {
    auto *cdet = m_Geometry->GetDetector(idt ? "pfRICH-2x2" : "pfRICH-1x1");
    //det->SetReadoutCellMask(~0x0);
    cdet->SetReadoutCellMask(0xFFFFFFFFFFFFFFFF);

    double fvOffset = idt ? _FIDUCIAL_VOLUME_OFFSET_2x2_ : _FIDUCIAL_VOLUME_OFFSET_1x1_;
    double fvLength = idt ? _FIDUCIAL_VOLUME_LENGTH_2x2_ : _FIDUCIAL_VOLUME_LENGTH_1x1_;
    
    // Do not use any conical mirrors for now; and do not use pyramids in a 1x1 box;
    auto dbox = new DarkBox(idt ? _USE_PYRAMIDS_ : false,  idt ? _USE_PYRAMID_OPTICS_ : false, false);
    //auto dbox = new DarkBox(false, idt ? _USE_CENTERPYRAMIDS_ : false, idt ? _USE_PYRAMID_OPTICS_ : false, false);
    
    // Fiducial volume (air); has to be called "PFRICH";
    auto *fiducial_volume_box = new G4Box("PFRICH", 400.0*mm/2, 400.0*mm/2, fvLength/2);
    auto fiducial_volume_log = new G4LogicalVolume(fiducial_volume_box, m_Air,  "PFRICH", 0, 0, 0);
    // All volumes are defined assuming EIC h-going endcap orientation (dRICH case was developed this way 
    // for ATHENA); therefore need to rotate by 180 degrees around Y axis;
    G4RotationMatrix *rY = new G4RotationMatrix(CLHEP::HepRotationY(flip ? 180*degree : 0));
    dbox->m_fiducial_volume_phys = 
      // Yes, do not want to rotate the 1x1 box;
      new G4PVPlacement(idt ? rY : 0, G4ThreeVector(0.0, 0.0, sign*fvOffset), fiducial_volume_log, 
    			"PFRICH", expHall_phys->GetLogicalVolume(), false, 0);
    
    // Gas container volume;
    dbox->m_gas_volume_length = fvLength - _VESSEL_FRONT_SIDE_THICKNESS_ - _SENSOR_AREA_LENGTH_;
    double gas_volume_offset = -(_SENSOR_AREA_LENGTH_ - _VESSEL_FRONT_SIDE_THICKNESS_)/2;
    auto gas_box = new G4Box("GasVolume", 399.0*mm/2, 399.0*mm/2, dbox->m_gas_volume_length/2);
    auto gas_volume_log = new G4LogicalVolume(gas_box, _GAS_RADIATOR_,  "GasVolume", 0, 0, 0);
    dbox->m_gas_volume_phys = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gas_volume_offset), 
						gas_volume_log, "GasVolume", fiducial_volume_log, false, 0);
    

    {
      // FIXME: Z-location does not really matter here, right?;
      auto boundary = new FlatSurface(TVector3(0,0,0), sign*TVector3(1,0,0), TVector3(0,-1,0));
      
      m_Geometry->SetContainerVolume(cdet, "GasVolume", 0, gas_volume_log, _GAS_RADIATOR_, boundary)
#ifdef _DISABLE_GAS_VOLUME_PHOTONS_
	->DisableOpticalPhotonGeneration()
#endif
	;
    }
    
    // A running variable;
    double gzOffset = -dbox->m_gas_volume_length/2 + _BUILDING_BLOCK_CLEARANCE_;

    //----------------------------
    // Aerogel
    //----------------------------
    if (idt) {
      // Aerogel, up to two layers;
      for(unsigned il=0; il<2; il++) {
	double agthick = 0.0;
	G4RadiatorMaterial *aerogel = 0;
#ifdef _AEROGEL_1_ 
	if (!il) {
	  unsigned agelId=GetAerogelId(_AEROGEL_1_);
	  agthick =  GetAerogelThickness(agelId);
	  aerogel   = m_Aerogel[agelId];
	  G4cout<<"_AEROGEL_1_="<<_AEROGEL_1_<<", agelId="<<agelId<<", agthick="<<agthick<<G4endl;
	} //if
#else
	if (!il) continue;
#endif
#ifdef _AEROGEL_2_ 
	if ( il) {
	  unsigned agelId=GetAerogelId(_AEROGEL_2_);
	  agthick = GetAerogelThickness(agelId)
	  aerogel   = m_Aerogel[agelId];
	} //if
#else
	if ( il) continue;
#endif

	// Kind of optimal for a 1.040 + 1.045 aerogel configuration;
	//if (il) gzOffset += 10*mm;
	if (il) gzOffset += 75*mm;
	
	{
	  gzOffset += agthick/2;

	  auto ag_tube = new G4Box(aerogel->GetName(), 100.0*mm/2, 100.0*mm/2,agthick/2);
	  auto ag_log = new G4LogicalVolume(ag_tube, aerogel, aerogel->GetName(), 0, 0, 0);
	  {
	    TVector3 nx(1*sign,0,0), ny(0,-1,0);
	    
	    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0, 0, fvOffset + 
								gas_volume_offset + gzOffset), nx, ny);
	    auto radiator = m_Geometry->AddFlatRadiator(cdet, aerogel->GetName(), CherenkovDetector::Upstream, 
							0, ag_log, aerogel, surface, agthick/mm);
#ifdef _DISABLE_AEROGEL_PHOTONS_
	    radiator->DisableOpticalPhotonGeneration();
#endif
	    m_Geometry->AddRadiatorLogicalVolume(radiator, ag_log);
	    
	    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), ag_log, aerogel->GetName(),
			      gas_volume_log, false, 0);
	  }
	  
	  // FIXME: not really needed that big between the two layers?;
	  gzOffset += agthick/2 + _BUILDING_BLOCK_CLEARANCE_;
	}
      } //for il

      //----------------------------
      // Acrylic filter;
      //----------------------------
      //#ifdef _ACRYLIC_THICKNESS_
      if ( _UVFILTER_=="acrylic")
	{
#ifdef _ACRYLIC_THICKNESS_
	  double acthick = _ACRYLIC_THICKNESS_;
	  gzOffset += acthick/2;
	  
	  auto ac_box  = new G4Box("Acrylic", 100.0/2, 100.0*mm/2, acthick/2);
	  auto ac_log = new G4LogicalVolume(ac_box, m_Acrylic,  "Acrylic", 0, 0, 0);
	  {
	    TVector3 nx(1*sign,0,0), ny(0,-1,0);
	    
	    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0, 0, fvOffset + 
								gas_volume_offset + gzOffset), nx, ny);
	    m_Geometry->AddFlatRadiator(cdet, "Acrylic", CherenkovDetector::Upstream, 
					0, ac_log, m_Acrylic, surface, acthick/mm)
#ifdef _DISABLE_ACRYLIC_PHOTONS_
	      ->DisableOpticalPhotonGeneration()
#endif
	      ;
	  }
	  
	  new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), ac_log, "Acrylic", gas_volume_log, false, 0);
	  std::cout<<"built "<<ac_log->GetName()<<" UV filter"<<std::endl;
	  
	  gzOffset += acthick/2 + _BUILDING_BLOCK_CLEARANCE_;
#endif
	}
      else if (_UVFILTER_=="LP285" || _UVFILTER_=="LP330") {
	//----------------------------
	// borosillicate filter
	//---------------------------- 
	double boroThick[2]={1.854*mm, 2.007*mm}; //mm;   [LP330, LP285]
	
	int boroID=UVfilterID[_UVFILTER_];
	
	gzOffset += boroThick[boroID]/2;
	
	const double activeXY=100.0*mm;
	
	auto boro_box  = new G4Box("Borosilicate", activeXY/2.,activeXY/2., boroThick[boroID]/2);
	auto boro_log = new G4LogicalVolume(boro_box, m_BorosilicateFilter[boroID],  "Borosilicate", 0, 0, 0);
	//auto boro_log = new G4LogicalVolume(boro_box, m_Aerogel[GetAerogelId("tsa120_1")],  "Borosilicate", 0, 0, 0);
	
	if (m_BorosilicateFilter[boroID]) std::cout<<"Borosilicate UV filter material: "<<m_BorosilicateFilter[boroID]->GetName()<<std::endl;
	else std::cout<<"No Borosilicate material"<<std::endl;
	
	// Blackhole frame around the active area.
	const double frameW   = 0.5*mm;   // blackhole border width
	const double filterXY = activeXY + 2.0*frameW;
	
	auto frame_outer = new G4Box("UVFilterFrameOuter",filterXY/2, filterXY/2, boroThick[boroID]/2);
	
	// Slightly larger in z so the subtraction punches through cleanly.
	auto frame_inner = new G4Box("UVFilterFrameInner",activeXY/2, activeXY/2,boroThick[boroID]/2 + 0.1*mm);
	auto frame_solid = new G4SubtractionSolid("UVFilterFrameSolid",frame_outer, frame_inner);
	auto frame_log = new G4LogicalVolume(frame_solid,
					     m_BlackBox,   // or your absorber material
					     "UVFilterFrame", 0, 0, 0);
	//frame_log->SetVisAttributes(G4VisAttributes::GetInvisible());
	
	{//Requires refractive index definition in Materials.cc
          TVector3 nx(1*sign,0,0), ny(0,-1,0);
	  
          auto surface = new FlatSurface(sign*(1/mm)*TVector3(0, 0, fvOffset +
                                                              gas_volume_offset + gzOffset), nx, ny);
          m_Geometry->AddFlatRadiator(cdet, "Borosilicate", CherenkovDetector::Upstream,
				      0, boro_log, m_BorosilicateFilter[boroID], surface, boroThick[boroID]/mm);//->DisableOpticalPhotonGeneration();
	}
	
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset),frame_log, "UVFilterFrame", gas_volume_log, false, 0);
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), boro_log, "Borosilicate", gas_volume_log, false, 0);
	
        gzOffset += boroThick[boroID]/2 + _BUILDING_BLOCK_CLEARANCE_;
      }
      
      //+DefineMirrors(det, flange);
    } //if

    //----------------------------
    // Mirrors
    //---------------------------- 
    //dbox->DefinePyramidMirrorGeometry(_HRPPD_TILE_SIZE_ + _HRPPD_INSTALLATION_GAP_, _PYRAMID_MIRROR_HEIGHT_);
    dbox->DefinePyramidMirrorGeometry(_HRPPD_INSTALLATION_PITCH_, _PYRAMID_MIRROR_HEIGHT_);

    /*
    {// flat mirror
      auto mirror_box = new G4Box("flatMirror", 5*cm/2., 3*cm/2., 1*cm/2.);
      auto mirror_log = new G4LogicalVolume(mirror_box, m_mirror, "flatMirror",0,0,0);
      new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), mirror_log, "flatMirror", gas_volume_log, false, 0);
    }
    */
    
    //----------------------------
    // Photon detectors;
    //----------------------------
    {
      std::vector<MisalignedLocation2D> xycoord;
    
      if (idt) {
	unsigned const hdim = 3;//3;
	//double pitch = _HRPPD_INSTALLATION_PITCH_;//_HRPPD_TILE_SIZE_ + _HRPPD_INSTALLATION_GAP_;

	for(unsigned ix=0; ix<hdim; ix++) {
	  double xOffset = _HRPPD_INSTALLATION_PITCH_*(ix - (hdim-1)/2.); 
	  
	  for(unsigned iy=0; iy<hdim; iy++) {
	    double yOffset = _HRPPD_INSTALLATION_PITCH_*(iy - (hdim-1)/2.); 

	    //if (!((ix+iy)%2)) continue;
	    if (ix==1 && iy==1) continue;
	    
	    if (!((ix+iy)%2)) xycoord.push_back(MisalignedLocation2D(xOffset, yOffset, 0.0, 0.0, 0.0, false));
	    else xycoord.push_back(MisalignedLocation2D(xOffset, yOffset, 0.0, 0.0, 0.0, true));
	    
	  } //for iy
	} //for ix
      }
      else
	// 'false' here does not really play a role since pyramids were suppressed when creating
	// a DarkBox instance (see above);
	xycoord.push_back(MisalignedLocation2D(0.0, 0.0, 0.0, 0.0, 0.0, false));
	
      //BuildPhotonDetectorMatrix(cdet, dbox, fvOffset, 
				// FIXME: is this offset correct?; front plate thickness?;
      //			fvLength/2 - _SENSOR_AREA_LENGTH_, xycoord);

      BuildPhotonDetectorMatrixCERN2026(cdet, dbox, fvOffset,fvLength/2 - _SENSOR_AREA_LENGTH_, xycoord);
				
    }

    if (!idt) {
#ifdef _ZCOORD_ASPHERIC_LENS_
      // #67-265; D 25.0mm, EFL 20.0mm; CT 14.00mm;
      auto lens = new AsphericLens(m_FusedSilica, 25.000*mm,  9.170*mm, -1.102626E+00, 0.0, 
				   8.791869E-05, 3.051652E-07, -7.950597E-10, 8.042043E-12, 0.0, 0.0, 2.27*mm);
      
      // Z location is given with respect to the upstream end of the gas volume;
      lens->Install(m_Geometry, this, cdet, dbox, _ZCOORD_ASPHERIC_LENS_);
#endif

#ifdef _ACRYLIC_THICKNESS_
      {
	auto ac_box  = new G4Box("Acrylic", 100.0/2, 100.0*mm/2, _ACRYLIC_THICKNESS_/2);
	auto ac_log = new G4LogicalVolume(ac_box, m_Acrylic,  "Acrylic", 0, 0, 0);
	
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, dbox->m_gas_volume_length/2 - _ACRYLIC_THICKNESS_/2 - 1.0*mm), 
			  ac_log, "Acrylic", gas_volume_log, false, 0);
      }
#endif
    } //if

    m_DarkBoxes.push_back(dbox);

  } //for idt

  return expHall_phys;
} // CERNDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------
