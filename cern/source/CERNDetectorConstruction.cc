#include <stdio.h>

#include "G4UnitsTable.hh" 
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4DataInterpolation.hh"
#include "G4LogicalSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

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
  CherenkovPhotonDetector* pd;
  G4MaterialPropertiesTable* mirrorMPT=0;
  
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
	} //if
#else	
	if (!il) continue;
#endif
	
#ifdef _AEROGEL_2_ 
	if ( il) {
	  unsigned agelId=GetAerogelId(_AEROGEL_2_);
	  agthick = GetAerogelThickness(agelId);
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
	  
	  auto ac_box  = new G4Box("AcrylicFilter", 100.0/2, 100.0*mm/2, acthick/2);
	  auto ac_log = new G4LogicalVolume(ac_box, m_Acrylic,  "AcrylicFilter", 0, 0, 0);
	  {
	    TVector3 nx(1*sign,0,0), ny(0,-1,0);
	    
	    auto surface = new FlatSurface(sign*(1/mm)*TVector3(0, 0, fvOffset + 
								gas_volume_offset + gzOffset), nx, ny);
	    m_Geometry->AddFlatRadiator(cdet, "AcrylicFilter", CherenkovDetector::Upstream, 
					0, ac_log, m_Acrylic, surface, acthick/mm)
#ifdef _DISABLE_ACRYLIC_PHOTONS_
	      ->DisableOpticalPhotonGeneration()
#endif
	      ;
	  }
	  
       	  new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), ac_log, "AcrylicFilter", gas_volume_log, false, 0);
	  
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
	
	auto boro_box  = new G4Box("BorosilicateFilter", activeXY/2.,activeXY/2., boroThick[boroID]/2);
	auto boro_log = new G4LogicalVolume(boro_box, m_BorosilicateFilter[boroID],  "BorosilicateFilter", 0, 0, 0);
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
          m_Geometry->AddFlatRadiator(cdet, "BorosilicateFilter", CherenkovDetector::Upstream,
				      0, boro_log, m_BorosilicateFilter[boroID], surface, boroThick[boroID]/mm);//->DisableOpticalPhotonGeneration();
	}
	
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset),frame_log, "UVFilterFrame", gas_volume_log, false, 0);
	new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, gzOffset), boro_log, "BorosilicateFilter", gas_volume_log, false, 0);
	
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

      pd=BuildPhotonDetectorMatrixCERN2026(cdet, dbox, fvOffset,fvLength/2 - _SENSOR_AREA_LENGTH_, xycoord,mirrorMPT);
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

#ifdef _VALIDATE_
  PrintProperty(pd,mirrorMPT);
#endif
  
  return expHall_phys;
} // CERNDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------
void CERNDetectorConstruction::PrintProperty(CherenkovPhotonDetector* pd,G4MaterialPropertiesTable* mirrorMPT)
{
  FILE* fprop=fopen("property.txt","w");
  if (fprop==NULL) {
     printf("Error opening file.\n");
     return;
  }

  //---------------------------------------
  // HRPPD QE
  //--------------------------------------- 
#ifdef _USE_HRPPD_AVERAGE_DATA_
  const int qeEntries=31;
  double WL[qeEntries] = {
    300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
    400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
    500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
    600};
  
#elifdef _USE_HRPPD_24_DATA_
   const G4int qeEntries = 44;
   double WL[qeEntries] = {
     200,   210,   220,   230,   240,   250,   260,   270,   280,   290,
     300,   310,   320,   330,   340,   350,   360,   370,   380,   390,
     400,   410,   420,   430,   440,   450,   460,   470,   480,   490,
     500,   510,   520,   530,   540,   550,   560,   570,   580,   590,
     600,   610,   620,   630};
#else
   const G4int qeEntries = 26;
   double WL[qeEntries] = { 160,  180,  200,  220,  240,  260,  280,  300,  320,  340,  360,  380,  400,
     420,  440,  460,  480,  500,  520,  540,  560,  580,  600,  620,  640,  660};
   double QE[qeEntries] = {0.25, 0.26, 0.27, 0.30, 0.32, 0.35, 0.36, 0.36, 0.36, 0.36, 0.37, 0.35, 0.30,
     0.27, 0.24, 0.20, 0.18, 0.15, 0.13, 0.11, 0.10, 0.09, 0.08, 0.07, 0.05, 0.05};
#endif
   
  auto qeTable=pd->GetQE();
  if (!qeTable) {
    G4cout<<"CERNDetectorConstruction::PrintProperty():: No QE table"<<G4endl;
  }
  else {
    fprintf(fprop,"=================================================\n");
    fprintf(fprop,"HRPPD QE\n");
    fprintf(fprop,"Scaling down factor %f\n",_QE_DOWNSCALING_FACTOR_);
    fprintf(fprop,"-------------------------------------------------\n");
    
    for (int i=0;i<qeEntries;i++) {
      double e=1240./WL[i];  //eV
      double qe=qeTable->CubicSplineInterpolation(e*1e-6);  //MeV
      fprintf(fprop, "WL = %.0f nm\tE = %.2f eV\tqe = %.3f\n",WL[i],e,qe);
    }
  }
  
  //---------------------------------------
  // Aerogel & UV filter
  //--------------------------------------- 
  for (auto* pv : *G4PhysicalVolumeStore::GetInstance()) {
    if (pv->GetName().find("Aerogel") != std::string::npos ||
	pv->GetName().find("BorosilicateFilter") != std::string::npos ||
	pv->GetName().find("AcrylicFilter") != std::string::npos) {
      
      auto opt_log=pv->GetLogicalVolume();
      
      G4Material* mat = opt_log->GetMaterial();
      if (!mat) {
	G4cout << pv->GetName()<<"CERNDetectorConstruction::PrintProperty():: logical vol has no material assigned!" << G4endl;
	continue;
      }
      
      G4VSolid* solid = opt_log->GetSolid();
      G4Box* box = dynamic_cast<G4Box*>(solid);
      G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();
      if (!mpt) {
	  G4cout << "CERNDetectorConstruction::PrintProperty()::" << pv->GetName() << " has no MaterialPropertiesTable!" << G4endl;
	  continue;
      }
      if (!box) {
	G4cout << "CERNDetectorConstruction::PrintProperty()::" << pv->GetName() << " has no solid" << G4endl;
	continue;
      }

      fprintf(fprop,"=================================================\n");
      fprintf(fprop,"%s\n",pv->GetName().c_str());
      fprintf(fprop,"-------------------------------------------------\n");
      fprintf(fprop,"Material Name: %s\n",mat->GetName().c_str());
      fprintf(fprop,"thickness=%s\n",G4String(G4BestUnit(2.0*box->GetZHalfLength(),"Length")).c_str());
      fprintf(fprop,"density=%.2f g/cm3\n",mat->GetDensity()/(g/cm3));
      
      // Refractive index
      if (auto rindex = mpt->GetProperty("RINDEX")) {
	fprintf(fprop,"\n--- RINDEX ---\n");
	
	for (size_t i = 0; i < rindex->GetVectorLength(); ++i) {
	  G4double energy = rindex->Energy(i);
	  G4double value  = (*rindex)[i];
	  
	  fprintf(fprop, "E = %.3f eV\tn = %.3f\n",energy/eV,value);
	}
      }
      
      if (auto abslen = mpt->GetProperty("ABSLENGTH")) {
	fprintf(fprop, "\n--- ABSLENGTH ---\n");
	
	for (size_t i = 0; i < abslen->GetVectorLength(); ++i) {
	  G4double energy = abslen->Energy(i);
	  G4double value  = (*abslen)[i];
	  
	  fprintf(fprop, "E = %.3f eV\tabslen = %s\n",energy/eV,G4String(G4BestUnit(value, "Length")).c_str());
	}
      }
    }// agel & filter
  }

  //---------------------------------------
  // mirror reflectivity
  //--------------------------------------- 
  if (!mirrorMPT) {
    G4cout << "CERNDetectorConstruction::PrintProperty():: mirror surface has no MaterialPropertiesTable!" << G4endl;
  }
  else {
    fprintf(fprop,"=================================================\n");
    fprintf(fprop,"Mirror Reflectivity\n");
    fprintf(fprop,"-------------------------------------------------\n");
    if (auto reflect = mirrorMPT->GetProperty("REFLECTIVITY")) {
      for (size_t i = 0; i < reflect->GetVectorLength(); ++i) {
	G4double energy = reflect->Energy(i);
	G4double value  = (*reflect)[i];
	
	fprintf(fprop, "E = %.3f eV\treflectivity = %.3f\n",energy,value);
      }
    }// mirror
  }
  
  
  fclose(fprop);
}
