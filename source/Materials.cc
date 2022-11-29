
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include <TString.h>

#include <TofPetMultiColumn.h>

#include <tuning.h>

#include "Materials.h"
#include <G4RadiatorMaterial.h>

#include <string>
#include <vector>

#include <XmlTree.h>

// -------------------------------------------------------------------------------------

Materials::Materials( void ): CherenkovWaveLengthRange(_WLDIM_, _NU_MIN_, _NU_STEP_)
{
  m_N = m_O = m_C = m_Si = m_H = m_K = m_Na = m_Sb = 0; 

  m_Air = m_Absorber = m_Bialkali = 0;
  //m_Aerogel[0] = /*m_Aerogel[1] = 
  m_Acrylic = m_Nitrogen = m_FusedSilica = 0;
} // Materials::Materials()

// -------------------------------------------------------------------------------------

void Materials::DefineElements( void )
{
  G4NistManager *manager = G4NistManager::Instance();

  m_H  = manager->FindOrBuildElement("H",  false); assert(m_H);
  m_C  = manager->FindOrBuildElement("C",  false); assert(m_C);
  m_N  = manager->FindOrBuildElement("N",  false); assert(m_N);
  m_O  = manager->FindOrBuildElement("O",  false); assert(m_O);
  m_N  = manager->FindOrBuildElement("N",  false); assert(m_N);
  m_Si = manager->FindOrBuildElement("Si", false); assert(m_Si);

  m_Na = manager->FindOrBuildElement("Na", false); assert(m_Na);
  m_K  = manager->FindOrBuildElement("K",  false); assert(m_K);
  m_Sb = manager->FindOrBuildElement("Sb", false); assert(m_Sb);
} // Materials::DefineElements()

// -------------------------------------------------------------------------------------

void Materials::DefineMaterials( void )
{
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
    unsigned density[2] = {_AEROGEL_CLAS12_DENSITY_110_MG_CM3_, _AEROGEL_CLAS12_DENSITY_225_MG_CM3_};

    for(unsigned il=0; il<2; il++) {
      //if (!density[il]) continue;

      TString name; name.Form("Aerogel%03d", density[il]);

#ifdef _AEROGEL_REFRACTIVE_INDEX_ 
      //assert(!(density[0] && density[1]));
      auto aerogel = new G4RadiatorMaterial(name.Data(), density[il]*mg/cm3, 2, _AEROGEL_REFRACTIVE_INDEX_);
#else
      auto aerogel = new G4RadiatorMaterial(name.Data(), density[il]*mg/cm3, 2);
#endif
      //printf("@@@ %s ... %s\n", aerogel->GetName().c_str(), name.Data()); assert(0);
	
      aerogel->AddElement(m_Si, 1);
      aerogel->AddElement(m_O,  2);
      
#ifdef _AEROGEL_REFRACTIVE_INDEX_ 
      // FIXME: use constant for now;
      G4double refractiveIndex[_WLDIM_];
      for(int iq=0; iq<_WLDIM_; iq++)
      // Tune to the average provided by the parameterization; FIXME: the value is too low;
	refractiveIndex[iq] = _AEROGEL_REFRACTIVE_INDEX_;
      
      G4MaterialPropertiesTable* aerogelMPT = new G4MaterialPropertiesTable();
      aerogelMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
      
      aerogel->SetMaterialPropertiesTable(aerogelMPT);
#else
      aerogel->SetRIChOptics(new g4dRIChAerogel(aerogel));
      // FIXME: tuned model#3 to n ~ 1.02 @ 400ns (which corresponds to 110 mg/cm^3 density);
      aerogel->GetRIChOptics()->setOpticalParams(3);
      aerogel->GetRIChOptics()->pTable->GetProperty("RINDEX")->SetSpline(true);
#endif

      _m_Aerogel[density[il]] = aerogel;
    } //for il
  } 

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
    
    for(unsigned im=0; im<mats.size(); im++) {
      auto mat = mats[im];

      const XMLCh* xmlch = mat->GetAttribute("name");
      auto name = xercesc::XMLString::transcode(xmlch);
	
      auto dens = mat->FindFirstTag("density");
      printf("%s -> %7.4f [g/cm^3]\n", name, atof(dens->GetFirstString()));
      
      auto comp = mat->FindFirstTag("Components");
      auto elems = comp->FindTags("Element");
      printf("   %d\n", (int)elems.size());
      
      auto aerogel = new G4RadiatorMaterial(name, atof(dens->GetFirstString())*g/cm3, elems.size());
      
      for(auto elem: elems) {
	const XMLCh* xmlch = elem->GetAttribute("fraction");
	auto fraction = xercesc::XMLString::transcode(xmlch);
	//rintf("%7.4f: %s\n", atof(fraction), elem->GetFirstString());
	char buffer[128];
	strcpy(buffer, elem->GetFirstString());
	//printf("%d\n", strlen(buffer));
	for(unsigned iq=0; iq<strlen(buffer); iq++)
	  if (buffer[iq] == 10 || buffer[iq] == 32) 
	    buffer[iq] = 0;
	//printf("%2d ", buffer[iq]);
	//printf("\n");
	printf("%7.4f: %s\n", atof(fraction), buffer);
	
	auto ptr = manager->FindOrBuildElement(buffer, false); assert(ptr);
	aerogel->AddElement(ptr, atof(fraction)*100*perCent);
      } //for elem
      
      G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();
      
      auto properties = mat->FindTags("Property");
      for(auto prop: properties) {
	const XMLCh* xmlch = prop->GetAttribute("name");
	auto name = xercesc::XMLString::transcode(xmlch);
	printf("%s (%d)\n", name, strlen(name));
	
#ifdef _DISABLE_ABSORPTION_
	if (!strcmp(name, "ABSLENGTH")) continue;
#endif
#ifdef _DISABLE_RAYLEIGH_SCATTERING_
	if (!strcmp(name, "RAYLEIGH")) continue;
#endif

	auto values = prop->FindTags("value");
	unsigned dim = values.size();
	printf("%d\n", values.size());
	std::map<double, double> entries;
	
	for(auto value: values) {
	  const XMLCh* xmlch = value->GetAttribute("energy");
	  auto energy = xercesc::XMLString::transcode(xmlch);
	  printf("%7.2f [eV] -> %8.3f\n", atof(energy), atof(value->GetFirstString()));
	  entries[atof(energy)] = atof(value->GetFirstString());
	} //for value
	
	double cff = strcmp(name, "RINDEX") ? mm : 1.0;

	unsigned counter = 0;
	double e[dim], v[dim];
	for(auto entry: entries) {
	  printf("%2d -> %f (%10.7f vs %10.7f .. %10.7f)\n", counter, entry.first, entry.first*eV, _NU_MIN_, _NU_MAX_);
	  
	  if (entry.first*eV >= _NU_MIN_ && entry.first*eV <= _NU_MAX_) {
	    e[counter  ] = entry.first  * eV;
	    v[counter++] = entry.second * cff;
	  } //if
	} 
	
	mpt->AddProperty(name, e, v, counter);
      } //for prop

      aerogel->SetMaterialPropertiesTable(mpt);
      _m_Aerogel[id[im]] = aerogel;
    } //for mat
    //assert(0);
    //printf("%s\n", mat->GetName());
  }

#ifdef _ACRYLIC_THICKNESS_
  {
#ifdef _ACRYLIC_REFRACTIVE_INDEX_ 
    m_Acrylic = new G4RadiatorMaterial("Acrylic",_ACRYLIC_DENSITY_ * g/cm3, 3, _ACRYLIC_REFRACTIVE_INDEX_);
#else
    m_Acrylic = new G4RadiatorMaterial("Acrylic",_ACRYLIC_DENSITY_ * g/cm3, 3);
#endif
    
    m_Acrylic->AddElement(m_C , 5);
    m_Acrylic->AddElement(m_O,  2);
    m_Acrylic->AddElement(m_H,  8);
    
#ifdef _ACRYLIC_REFRACTIVE_INDEX_ 
    // FIXME: use constant for now;
    G4double refractiveIndex[_WLDIM_];
    for(int iq=0; iq<_WLDIM_; iq++)
      // Tune to the average provided by the parameterization; FIXME: the value is too low;
      refractiveIndex[iq] = _ACRYLIC_REFRACTIVE_INDEX_;
    
    G4MaterialPropertiesTable* acrylicMPT = new G4MaterialPropertiesTable();
    acrylicMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
    
    m_Acrylic->SetMaterialPropertiesTable(acrylicMPT);
#else
    m_Acrylic->SetRIChOptics(new g4dRIChFilter(m_Acrylic));
    // FIXME: tuned threshold to 350nm for now;
    m_Acrylic->GetRIChOptics()->setOpticalParams(_ACRYLIC_WL_CUTOFF_);
    m_Acrylic->GetRIChOptics()->pTable->GetProperty("RINDEX")->SetSpline(true);
#endif
  }
#endif

  // Bialkali photocathode;
  {
    // Well, th eonly part what matters here is the refractive index; density is fake;
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

  // A fake absorber material (without optical properties);
  {
    m_Absorber = new G4Material("Absorber", 1.00*g/cm3, 1);
    
    m_Absorber->AddElement(m_C, 1);
  }
} // Materials::DefineMaterials()

// -------------------------------------------------------------------------------------
