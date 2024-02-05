
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include <TString.h>

#include <TofPetMultiColumn.h>

#include "Materials.h"
#include <G4RadiatorMaterial.h>

#include <string>
#include <vector>

#ifdef XERCES
#include <XmlTree.h>
#endif

// -------------------------------------------------------------------------------------

Materials::Materials( void ): CherenkovWaveLengthRange(_WLDIM_, _NU_MIN_, _NU_STEP_)
{
  m_N = m_O = m_C = m_F = m_Si = m_H = m_K = m_Na = m_Sb = m_Al = m_Ca = 0; 

  m_Air = m_Absorber = m_Bialkali = m_Aluminum = m_CarbonFiber = m_Ceramic = m_Silver = m_Titanium = 0;
  m_Acrylic = m_Nitrogen = m_FusedSilica = m_C2F6 = m_Sapphire = 0;

  m_FakeCarbon_1_g_cm3 = m_HalfInch_CF_HoneyComb = m_QuarterInch_CF_HoneyComb = 0;  
  m_FR4 = m_Water = m_Copper = m_Silicon = m_Delrin = m_PEEK = 0;
} // Materials::Materials()

// -------------------------------------------------------------------------------------

void Materials::DefineElements( void )
{
  G4NistManager *manager = G4NistManager::Instance();

  m_H  = manager->FindOrBuildElement("H",  false); assert(m_H);
  m_C  = manager->FindOrBuildElement("C",  false); assert(m_C);
  m_N  = manager->FindOrBuildElement("N",  false); assert(m_N);
  m_O  = manager->FindOrBuildElement("O",  false); assert(m_O);
  m_F  = manager->FindOrBuildElement("F",  false); assert(m_F);
  m_N  = manager->FindOrBuildElement("N",  false); assert(m_N);
  m_Si = manager->FindOrBuildElement("Si", false); assert(m_Si);
  m_Al = manager->FindOrBuildElement("Al", false); assert(m_Al);
  m_Ca = manager->FindOrBuildElement("Ca", false); assert(m_Ca);

  m_Na = manager->FindOrBuildElement("Na", false); assert(m_Na);
  m_K  = manager->FindOrBuildElement("K",  false); assert(m_K);
  m_Sb = manager->FindOrBuildElement("Sb", false); assert(m_Sb);
} // Materials::DefineElements()

// -------------------------------------------------------------------------------------
// As provided by Charles, see e-mail from 2023/08/14; 
// https://refractiveindex.info/?shelf=3d&book=crystals&page=sapphire

static double calculate_sapphire_n(double lambda) {
  double lambda_squared = lambda * lambda;

  double term1 = 1.4313493  * lambda_squared  / (lambda_squared - pow(0.0726631, 2));
  double term2 = 0.65054713 * lambda_squared  / (lambda_squared - pow(0.1193242, 2));
  double term3 = 5.3414021  * lambda_squared  / (lambda_squared - pow(18.028251, 2));
  
  double n_squared = 1 + term1 + term2 + term3;
  return sqrt(n_squared);
}

// -------------------------------------------------------------------------------------

void Materials::DefineMaterials( void )
{
  {
    G4NistManager *manager = G4NistManager::Instance();

    m_Aluminum = manager->FindOrBuildMaterial("G4_Al");    assert(m_Aluminum);
    m_Silver   = manager->FindOrBuildMaterial("G4_Ag");    assert(m_Silver);
    m_Copper   = manager->FindOrBuildMaterial("G4_Cu");    assert(m_Copper);
    m_Titanium = manager->FindOrBuildMaterial("G4_Ti");    assert(m_Titanium);
    m_Water    = manager->FindOrBuildMaterial("G4_WATER"); assert(m_Water);
    m_Silicon  = manager->FindOrBuildMaterial("G4_Si");    assert(m_Silicon);
  }  

  // Air;
  {
    // This will be air without optical properties; looks the easiest way to 
    // cut optical photons at the vessel envelope (and world volume) border;
    m_Air = new G4Material("Air", 1.29*mg/cm3, 2);
    
    m_Air->AddElement(m_N, 70.*perCent);
    m_Air->AddElement(m_O, 30.*perCent);
  }

  // Nitrogen: n−1=6.8552×10−5+3.243157×10−2144−λ−2
  {
    m_Nitrogen = new G4RadiatorMaterial("Nitrogen",       1.25*mg/cm3, 1);

    m_Nitrogen->AddElement(m_N, 100.*perCent);
        
    G4double refractiveIndex[_WLDIM_];
    for(int iq=0; iq<_WLDIM_; iq++) {
      // Available dispersion formula requires wavelength in [um] units;
      double wl = 1E-3 * _MAGIC_CFF_ / (GetPhotonEnergy(iq)/eV);
      
      refractiveIndex[iq] = 1.0 + 6.8552E-5 + 3.243157E-2/(144.0 - pow(wl, -2));
    } //for iq

    G4MaterialPropertiesTable* nitrogenMPT = new G4MaterialPropertiesTable();
    nitrogenMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
    
    m_Nitrogen->SetMaterialPropertiesTable(nitrogenMPT);
  }

  {
    m_Sapphire = new G4RadiatorMaterial("Sapphire",       3.98*mg/cm3, 2);

    m_Sapphire->AddElement(m_Al, 2);
    m_Sapphire->AddElement(m_O,  3);
        
    G4double refractiveIndex[_WLDIM_];
    for(int iq=0; iq<_WLDIM_; iq++) {
      // Available dispersion formula requires wavelength in [um] units;
      double wl = 1E-3 * _MAGIC_CFF_ / (GetPhotonEnergy(iq)/eV);
      
      refractiveIndex[iq] = calculate_sapphire_n(wl);
      printf("%7.2f\n", refractiveIndex[iq]);
    } //for iq
    //exit(0);

    G4MaterialPropertiesTable* sapphireMPT = new G4MaterialPropertiesTable();
    sapphireMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
    
    m_Sapphire->SetMaterialPropertiesTable(sapphireMPT);
  }

  // Import UV silica refractive index data;
  {
    auto riTable = new TofPetMultiColumn("./database/RefractiveIndex.txt", '\t');

    // FIXME: <n> hardcoded;
    m_FusedSilica = new G4RadiatorMaterial("FusedSilica", 2.20*g/cm3, 2, 1.46);
    
    m_FusedSilica->AddElement(m_Si, 1);
    m_FusedSilica->AddElement(m_O,  2);

    auto wl = riTable->GetDataPoints(0), ri = riTable->GetDataPoints(3);
    unsigned riDim = wl.size();

    // FIXME: use constant for now;
    G4double energy[riDim], refractiveIndex[riDim];
    for(int iq=riDim-1; iq>=0; iq--) {
      energy         [iq] =  (1240.0/wl[iq])*eV;
      refractiveIndex[iq] = ri[iq];
    } //for iq 

    G4MaterialPropertiesTable* fsMPT = new G4MaterialPropertiesTable();
    fsMPT->AddProperty("RINDEX", energy, refractiveIndex, riDim);
    
    m_FusedSilica->SetMaterialPropertiesTable(fsMPT);
  }

  // CLAS12 aerogel; two options for now; obviously can add more the same way
  {
    unsigned density[2] = {_AEROGEL_CLAS12_DENSITY_155_MG_CM3_, _AEROGEL_CLAS12_DENSITY_225_MG_CM3_};

    for(unsigned il=0; il<2; il++) {
      //if (!density[il]) continue;

      TString name; name.Form("Aerogel%03d", density[il]);

#ifdef _AEROGEL_FIXED_REFRACTIVE_INDEX_ 
      //assert(!(density[0] && density[1]));
      auto aerogel = new G4RadiatorMaterial(name.Data(), density[il]*mg/cm3, 2, _AEROGEL_FIXED_REFRACTIVE_INDEX_);
#else
      auto aerogel = new G4RadiatorMaterial(name.Data(), density[il]*mg/cm3, 2);
#endif
      //printf("@@@ %s ... %s\n", aerogel->GetName().c_str(), name.Data()); assert(0);
	
      aerogel->AddElement(m_Si, 1);
      aerogel->AddElement(m_O,  2);
      
#ifdef _AEROGEL_FIXED_REFRACTIVE_INDEX_ 
      // FIXME: use constant for now;
      G4double refractiveIndex[_WLDIM_];
      for(int iq=0; iq<_WLDIM_; iq++)
      // Tune to the average provided by the parameterization; FIXME: the value is too low;
	refractiveIndex[iq] = _AEROGEL_FIXED_REFRACTIVE_INDEX_;
      
      G4MaterialPropertiesTable* aerogelMPT = new G4MaterialPropertiesTable();
      aerogelMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
      
      aerogel->SetMaterialPropertiesTable(aerogelMPT);
#else
      aerogel->SetRIChOptics(new g4dRIChAerogel(aerogel));
      // FIXME: tuned model#3 to n ~ 1.02 @ 400ns (which corresponds to 110 mg/cm^3 density);
      aerogel->GetRIChOptics()->setOpticalParams(3);
      aerogel->GetRIChOptics()->pTable->GetProperty("RINDEX")->SetSpline(true);
#endif

      m_Aerogel[density[il]] = aerogel;
    } //for il
  } 

#ifdef XERCES
  // Belle II aerogel, two types;
  {
    unsigned id[2] = {_AEROGEL_BELLE_II_SMALL_REFRACTIVE_INDEX_, _AEROGEL_BELLE_II_LARGE_REFRACTIVE_INDEX_};

    G4NistManager *manager = G4NistManager::Instance();

    //std::string configFile="sample.xml"; 

    // Initialize Xerces infrastructure
    xercesc::XMLPlatformUtils::Initialize();  

    auto configFileParser = new xercesc::XercesDOMParser;
    
    configFileParser->parse( "./database/BelleII.xml");//configFile.c_str() );
    auto xmlDoc = configFileParser->getDocument();

    // Get the top-level element: Name is "Materials";
    xercesc::DOMElement* elementRoot = xmlDoc->getDocumentElement();

    auto tree = new XmlTree(elementRoot);
    //tree->Print();
    auto mats = tree->FindTags("Material");
    //printf("%d\n", (int)mats.size()); assert(0);

    // Prepare for creation of a fake Belle II aerogel with <n> ~ 1.04; 
    double densities[2] = {0.0, 0.0}, nominal_ri[2] = {1.045, 1.055}, a1040_ri = 1.040;
    // Will be used in several places;
    double dri10 = nominal_ri[1] - nominal_ri[0], dri20 = a1040_ri - nominal_ri[0];
    G4RadiatorMaterial *a1040 = 0;
    std::map<TString, std::vector<double>> e1040, v1040;

    for(unsigned im=0; im<mats.size(); im++) {
      auto mat = mats[im];

      const XMLCh* xmlch = mat->GetAttribute("name");
      auto name = xercesc::XMLString::transcode(xmlch);
	
      auto dens = mat->FindFirstTag("density");
      //printf("%s -> %7.4f [g/cm^3]\n", name, atof(dens->GetFirstString()));
      
      auto comp = mat->FindFirstTag("Components");
      auto elems = comp->FindTags("Element");
      //printf("   %d\n", (int)elems.size());
      
      densities[im] = atof(dens->GetFirstString());
      // FIXME: use densities[] once debugging is over;
      auto aerogel = new G4RadiatorMaterial(name, atof(dens->GetFirstString())*g/cm3, elems.size());

      // FIXME: assuming that element composition is the same (which is true);
      if (im) {
	// FIXME: this is not right (must account for density scaling with <n>);
	double slope = (densities[1] - densities[0])/dri10;//(nominal_ri[1] - nominal_ri[0]);

	a1040 = new G4RadiatorMaterial("BelleIIAerogel3", (densities[0] + slope*dri20)*g/cm3, elems.size());
      } //if
      
      for(auto elem: elems) {
	const XMLCh* xmlch = elem->GetAttribute("fraction");
	auto fraction = xercesc::XMLString::transcode(xmlch);
	//printf("%7.4f: %s\n", atof(fraction), elem->GetFirstString());
	char buffer[128];
	strcpy(buffer, elem->GetFirstString());
	//printf("%d\n", strlen(buffer));
	for(unsigned iq=0; iq<strlen(buffer); iq++)
	  if (buffer[iq] == 10 || buffer[iq] == 32) 
	    buffer[iq] = 0;
	//printf("%2d ", buffer[iq]);
	//printf("\n");
	//printf("%7.4f: %s\n", atof(fraction), buffer);
	
	auto ptr = manager->FindOrBuildElement(buffer, false); assert(ptr);
	aerogel->AddElement(ptr, atof(fraction)*100*perCent);

	if (im) a1040->AddElement(ptr, atof(fraction)*100*perCent);
      } //for elem
      
      G4MaterialPropertiesTable *mpt     =      new G4MaterialPropertiesTable();
      G4MaterialPropertiesTable *mpt1040 = im ? new G4MaterialPropertiesTable() : 0;
      
      auto properties = mat->FindTags("Property");
      for(auto prop: properties) {
	const XMLCh* xmlch = prop->GetAttribute("name");
	auto name = xercesc::XMLString::transcode(xmlch);
	//printf("%s (%d)\n", name, strlen(name));
	
#ifdef _DISABLE_ABSORPTION_
	if (!strcmp(name, "ABSLENGTH")) continue;
#endif
#ifdef _DISABLE_RAYLEIGH_SCATTERING_
	if (!strcmp(name, "RAYLEIGH")) continue;
#endif

	auto values = prop->FindTags("value");
	unsigned dim = values.size();
	//printf("%d\n", values.size());
	std::map<double, double> entries;
	
	for(auto value: values) {
	  const XMLCh* xmlch = value->GetAttribute("energy");
	  auto energy = xercesc::XMLString::transcode(xmlch);
	  //printf("%7.2f [eV] -> %8.3f\n", atof(energy), atof(value->GetFirstString()));
	  entries[atof(energy)] = atof(value->GetFirstString());
	} //for value
	
	double cff = strcmp(name, "RINDEX") ? mm : 1.0;

	unsigned counter = 0;
	double e[dim], v[dim], vbff[dim];
	for(auto entry: entries) {
	  //printf("%2d -> %f (%10.7f vs %10.7f .. %10.7f)\n", counter, entry.first, entry.first*eV, _NU_MIN_, _NU_MAX_);
	  
	  if (entry.first*eV >= _NU_MIN_ && entry.first*eV <= _NU_MAX_) {
	    if (im) {
	      //printf("@Q@ %s %d %d vs %d\n", name, e1040.size(), e1040[name].size(), counter);
	      // FIXME: out of range check;
	      assert(e1040[name][counter] == entry.first  * eV);
	      
	      //printf("@Q@ %7.3f\n", e1040[name][counter]);
	      double slope = (entry.second * cff - v1040[name][counter])/dri10;
	      vbff[counter] = v1040[name][counter] + slope*dri20;
	    } else {
	      // Just store <n> ~ 1.045 aerogel parameterization point;
	      e1040[name].push_back(entry.first  * eV);
	      v1040[name].push_back(entry.second * cff);
	    } //if

	    e[counter  ] = entry.first  * eV;
	    v[counter++] = entry.second * cff;
	  } //if
	} 
	
	mpt->AddProperty(name, e, v, counter);
	if (im) mpt1040->AddProperty(name, e, vbff, counter);
      } //for prop

      aerogel->SetMaterialPropertiesTable(mpt);
      m_Aerogel[id[im]] = aerogel;
      if (im) {
	a1040->SetMaterialPropertiesTable(mpt1040);
	m_Aerogel[_AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_] = a1040;
      } //if
    } //for mat
    //printf("%s\n", mat->GetName());
  }
#endif

  {
#ifdef _ACRYLIC_FIXED_REFRACTIVE_INDEX_ 
    m_Acrylic = new G4RadiatorMaterial("Acrylic",_ACRYLIC_DENSITY_, 3, _ACRYLIC_FIXED_REFRACTIVE_INDEX_);
#else
    m_Acrylic = new G4RadiatorMaterial("Acrylic",_ACRYLIC_DENSITY_, 3);
#endif
    
    m_Acrylic->AddElement(m_C , 5);
    m_Acrylic->AddElement(m_O,  2);
    m_Acrylic->AddElement(m_H,  8);
    
#ifdef _ACRYLIC_FIXED_REFRACTIVE_INDEX_ 
    // FIXME: use constant for now;
    G4double refractiveIndex[_WLDIM_];
    for(int iq=0; iq<_WLDIM_; iq++)
      // Tune to the average provided by the parameterization; FIXME: the value is too low;
      refractiveIndex[iq] = _ACRYLIC_FIXED_REFRACTIVE_INDEX_;
    
    G4MaterialPropertiesTable* acrylicMPT = new G4MaterialPropertiesTable();
    acrylicMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
    
    m_Acrylic->SetMaterialPropertiesTable(acrylicMPT);
#else
    m_Acrylic->SetRIChOptics(new g4dRIChFilter(m_Acrylic));
    m_Acrylic->GetRIChOptics()->setOpticalParams(_ACRYLIC_WL_CUTOFF_);
    m_Acrylic->GetRIChOptics()->pTable->GetProperty("RINDEX")->SetSpline(true);
#endif
  }

  // C2F6 as a gas radiator option;
  {
    m_C2F6 = new G4RadiatorMaterial("C2F6", 5.7*mg/cm3, 2);

    m_C2F6->AddElement(m_C , 2);
    m_C2F6->AddElement(m_F , 6);

    m_C2F6->SetRIChOptics(new g4dRIChGas(m_C2F6));
    m_C2F6->GetRIChOptics()->setOpticalParams();
    m_C2F6->GetRIChOptics()->pTable->GetProperty("RINDEX")->SetSpline(true);
  }

  // Bialkali photocathode;
  {
    // Well, the only part what matters here is the refractive index; density is fake;
    m_Bialkali = new G4Material("Bialkali", 1.0*g/cm3, 3);

    m_Bialkali->AddElement(m_Na, 2);
    m_Bialkali->AddElement(m_K,  1);
    m_Bialkali->AddElement(m_Sb, 1);
    
    G4double refractiveIndex[_WLDIM_];
    for(int iq=0; iq<_WLDIM_; iq++)
      refractiveIndex[iq] = _BIALKALI_REFRACTIVE_INDEX_;
    
    G4MaterialPropertiesTable* bialkaliMPT = new G4MaterialPropertiesTable();
    bialkaliMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
    
    m_Bialkali->SetMaterialPropertiesTable(bialkaliMPT);
  }

  // Ceramic imitation; assume DuPont 951 variety for both the HRPPD anode base plate and the 
  // walls; chemical composition is unknown; it is a "mixture of Al2O3, CaZr03 and glass" at 
  // 3.10 g/cm^3; take CeramTape GC (density 2.87 g/cm^2) as a closest known reference;
  {
    m_Ceramic = new G4Material("Ceramic", 3.10*g/cm3, 4);
    
    // Ignore inner traces;
    m_Ceramic->AddElement(m_O,  54.85*perCent);
    m_Ceramic->AddElement(m_Al, 30.09*perCent);
    m_Ceramic->AddElement(m_Si,  8.86*perCent);
    m_Ceramic->AddElement(m_Ca,  6.20*perCent);
  }

  // A fake absorber material (without optical properties);
  {
    m_Absorber = new G4Material("Absorber", 1.00*g/cm3, 1);
    
    m_Absorber->AddElement(m_C, 1);
  }

  // A fake carbon with 1g/cm^3 density;
  {
    m_FakeCarbon_1_g_cm3 = new G4Material("FakeCarbon", 1.00*g/cm3, 1);
    
    m_FakeCarbon_1_g_cm3->AddElement(m_C, 1);
  }

  double fake_carbon_rad_length = m_FakeCarbon_1_g_cm3->GetRadlen();

  // 1/2" and 1/4" thick honecomb "materials" of effective density;
  {    
    double core_thickness[2] = {_INCH_/4, _INCH_/2};
    const char *names[2] = {"QuarterInchHoneycomb", "HalfInchHoneyconb"};
    for(unsigned iq=0; iq<2; iq++) {
      auto mptr = iq ? &m_HalfInch_CF_HoneyComb : &m_QuarterInch_CF_HoneyComb;

      // Full thickness;
      double thickness = 2*_CF_THICKNESS_ + 2*_EG_THICKNESS_ + core_thickness[iq];
      // Full radiation length in abs. units;
      double radlen = 2*_CF_THICKNESS_/_CF_RAD_LENGTH_ + 2*_EG_THICKNESS_/_EG_RAD_LENGTH_ + 
	core_thickness[iq]/_HC_RAD_LENGTH_;
      //printf("@@@ %d %f %f %f\n", iq, thickness/mm, radlen/mm, (thickness/radlen)/mm);

      // So, need to adjust carbon density to end up at "thickness/radlen" rad.length value; 
      *mptr = new G4Material(names[iq], (1.00*fake_carbon_rad_length/((thickness/radlen)))*g/cm3, 1);

      (*mptr)->AddElement(m_C, 1);
      //printf("@@@ %f\n", (*mptr)->GetRadlen()/mm);
    }  //for iq
  }

  // A fake carbon fiber with a resonable density to match Prakhar's numbers, see Materials.h;
  {
    m_CarbonFiber = new G4Material("CarbonFiber", (1.00*fake_carbon_rad_length/_CF_RAD_LENGTH_)*g/cm3, 1);
    //m_CarbonFiber = new G4Material("CarbonFiber", 1.90*mg/cm3, 1);
    
    m_CarbonFiber->AddElement(m_C, 1);
    //printf("@@@ %f\n", m_CarbonFiber->GetRadlen()/mm);
  }

  // A fake FR4 with a resonable density to match PDG number, see Materials.h;
  {
    m_FR4 = new G4Material("FR4", (1.00*fake_carbon_rad_length/_FR4_RAD_LENGTH_)*g/cm3, 1);
    
    m_FR4->AddElement(m_C, 1);
  }

  {
    // Well, the only part what matters here is the refractive index; density is fake;
    m_Delrin = new G4Material("Delrin", 1.41*g/cm3, 3);

    m_Delrin->AddElement(m_C, 1);
    m_Delrin->AddElement(m_H, 2);
    m_Delrin->AddElement(m_O, 1);
  }

  {
    // Well, the only part what matters here is the refractive index; density is fake;
    m_PEEK = new G4Material("PEEK", 1.31*g/cm3, 3);

    m_PEEK->AddElement(m_C, 19);
    m_PEEK->AddElement(m_H, 14);
    m_PEEK->AddElement(m_O,  3);
  }

  {
    // Manage readable printout;
    auto half = m_HalfInch_CF_HoneyComb, quarter = m_QuarterInch_CF_HoneyComb;

    printf("Copper      : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_Copper->  GetRadlen()   /cm, m_Copper     ->GetDensity()/(g/cm3));
    printf("Aluminum    : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_Aluminum->GetRadlen()   /cm, m_Aluminum   ->GetDensity()/(g/cm3));
    printf("Delrin      : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_Delrin->GetRadlen()     /cm, m_Delrin     ->GetDensity()/(g/cm3));
    printf("PEEK        : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_PEEK->GetRadlen()       /cm, m_PEEK       ->GetDensity()/(g/cm3));
    printf("Water       : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_Water->GetRadlen()      /cm, m_Water      ->GetDensity()/(g/cm3));
    printf("Ceramic     : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_Ceramic->GetRadlen()    /cm, m_Ceramic    ->GetDensity()/(g/cm3));
    printf("FR4         : %8.3f [cm],  %8.3f [g/cm^3]\n",  m_FR4->GetRadlen()        /cm, m_FR4        ->GetDensity()/(g/cm3));
    printf("Fused silica: %8.3f [cm],  %8.3f [g/cm^3]\n",  m_FusedSilica->GetRadlen()/cm, m_FusedSilica->GetDensity()/(g/cm3));
    printf("1/2\" HC     : %8.3f [cm],  %8.3f [g/cm^3]\n", half->GetRadlen()         /cm, half         ->GetDensity()/(g/cm3));
    printf("1/4\" HC     : %8.3f [cm],  %8.3f [g/cm^3]\n", quarter->GetRadlen()      /cm, quarter      ->GetDensity()/(g/cm3));
  }
} // Materials::DefineMaterials()

// -------------------------------------------------------------------------------------
