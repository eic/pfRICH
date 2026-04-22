
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

const AerogelConfig &Materials::GetAerogelConfig(const std::string &tag)
{
  // Replace these energies / absorption lengths with your measured values.
  static const std::vector<G4double> e = {
    6.199*eV, 6.018*eV, 5.848*eV, 5.687*eV, 5.534*eV, 5.390*eV, 5.253*eV, 5.122*eV, 4.998*eV, 4.880*eV,
    4.768*eV, 4.660*eV, 4.557*eV, 4.459*eV, 4.364*eV, 4.274*eV, 4.187*eV, 4.104*eV, 4.024*eV, 3.947*eV,
    3.873*eV, 3.802*eV, 3.733*eV, 3.667*eV, 3.603*eV, 3.541*eV, 3.481*eV, 3.423*eV, 3.368*eV, 3.314*eV,
    3.261*eV, 3.210*eV, 3.161*eV, 3.114*eV, 3.067*eV, 3.022*eV, 2.979*eV, 2.936*eV, 2.895*eV, 2.855*eV,
    2.816*eV, 2.778*eV, 2.741*eV, 2.706*eV, 2.671*eV, 2.636*eV, 2.603*eV, 2.571*eV, 2.539*eV, 2.508*eV,
    2.478*eV, 2.449*eV, 2.420*eV, 2.392*eV, 2.365*eV, 2.338*eV, 2.312*eV, 2.286*eV, 2.261*eV, 2.237*eV,
    2.213*eV, 2.189*eV, 2.166*eV, 2.144*eV, 2.122*eV, 2.100*eV, 2.079*eV, 2.058*eV, 2.038*eV, 2.018*eV,
    1.998*eV, 1.979*eV, 1.960*eV, 1.942*eV, 1.924*eV, 1.906*eV, 1.889*eV, 1.872*eV, 1.855*eV, 1.838*eV,
    1.822*eV, 1.806*eV, 1.790*eV, 1.775*eV, 1.760*eV, 1.745*eV, 1.730*eV, 1.716*eV, 1.702*eV, 1.688*eV,
    1.674*eV, 1.661*eV, 1.648*eV, 1.634*eV, 1.622*eV, 1.609*eV, 1.597*eV, 1.584*eV, 1.572*eV, 1.560*eV
  };


  static const std::map<std::string, AerogelConfig> cfg = {
    /*
      {tag,
       config{
         id,
	 name,
	 thickness (mm),
	 density,
	 refractive index,
	 photon energy,
	 absorption length (cm)
        }
       },
     */
    {"tsa114_3",
     AerogelConfig{
       _AEROGEL_TSA114_3,   
       "AerogelTSA114_3", 
       25.1,        
       0.20,        
       1.0377,      
       e,
       {0.17*cm, 0.20*cm, 0.24*cm, 0.28*cm, 0.32*cm, 0.37*cm, 0.43*cm, 0.49*cm, 0.56*cm, 0.63*cm,  
	0.71*cm, 0.79*cm, 0.89*cm, 0.99*cm, 1.09*cm, 1.21*cm, 1.33*cm, 1.46*cm, 1.60*cm, 1.74*cm,
	1.90*cm, 2.07*cm, 2.24*cm, 2.43*cm, 2.62*cm, 2.83*cm, 3.04*cm, 3.27*cm, 3.51*cm, 3.77*cm,
	4.03*cm, 4.31*cm, 4.60*cm, 4.90*cm, 5.22*cm, 5.55*cm, 5.90*cm, 6.26*cm, 6.63*cm, 7.03*cm,
	7.44*cm, 7.86*cm, 8.30*cm, 8.76*cm, 9.24*cm, 9.73*cm, 10.25*cm, 10.78*cm, 11.33*cm, 11.90*cm,
	12.49*cm, 13.10*cm, 13.73*cm, 14.39*cm, 15.06*cm, 15.76*cm, 16.48*cm, 17.22*cm, 17.98*cm, 18.77*cm,
	19.58*cm, 20.41*cm, 21.27*cm, 22.16*cm, 23.06*cm, 24.00*cm, 24.96*cm, 25.94*cm, 26.95*cm, 27.99*cm,
	29.06*cm, 30.15*cm, 31.27*cm, 32.41*cm, 33.58*cm, 34.78*cm, 36.01*cm, 37.27*cm, 38.56*cm, 39.87*cm,
	41.21*cm, 42.58*cm, 43.98*cm, 45.41*cm, 46.87*cm, 48.36*cm, 49.88*cm, 51.42*cm, 53.00*cm, 54.60*cm,
	56.24*cm, 57.90*cm, 59.59*cm, 61.31*cm, 63.06*cm, 64.84*cm, 66.65*cm, 68.49*cm, 70.35*cm, 72.24*cm}
     }
    },
    {"tsa120_1",
     AerogelConfig{
       _AEROGEL_TSA120_1,
       "AerogelTSA120_1",
       24.5,
       0.20,
       1.0404,
       e,
       {0.16*cm, 0.19*cm, 0.22*cm, 0.26*cm, 0.30*cm, 0.35*cm, 0.40*cm, 0.46*cm, 0.52*cm, 0.58*cm,
	0.66*cm, 0.73*cm, 0.82*cm, 0.91*cm, 1.01*cm, 1.11*cm, 1.22*cm, 1.34*cm, 1.46*cm, 1.60*cm,
	1.74*cm, 1.89*cm, 2.05*cm, 2.22*cm, 2.40*cm, 2.58*cm, 2.78*cm, 2.99*cm, 3.21*cm, 3.44*cm,
	3.68*cm, 3.93*cm, 4.20*cm, 4.48*cm, 4.77*cm, 5.07*cm, 5.39*cm, 5.72*cm, 6.07*cm, 6.43*cm,
	6.81*cm, 7.20*cm, 7.61*cm, 8.04*cm, 8.48*cm, 8.94*cm, 9.42*cm, 9.91*cm, 10.43*cm, 10.96*cm,
	11.52*cm, 12.09*cm, 12.68*cm, 13.30*cm, 13.94*cm, 14.60*cm, 15.28*cm, 15.99*cm, 16.72*cm, 17.47*cm,
	18.25*cm, 19.05*cm, 19.88*cm, 20.73*cm, 21.62*cm, 22.53*cm, 23.46*cm, 24.43*cm, 25.42*cm, 26.45*cm,
	27.50*cm, 28.58*cm, 29.70*cm, 30.85*cm, 32.03*cm, 33.24*cm, 34.49*cm, 35.77*cm, 37.08*cm, 38.43*cm,
	39.82*cm, 41.24*cm, 42.70*cm, 44.19*cm, 45.73*cm, 47.30*cm, 48.91*cm, 50.56*cm, 52.26*cm, 53.99*cm,
	55.77*cm, 57.59*cm, 59.45*cm, 61.35*cm, 63.30*cm, 65.30*cm, 67.33*cm, 69.42*cm, 71.55*cm, 73.73*cm}
     }
    },
    {"tsa120_2",
     AerogelConfig{
       _AEROGEL_TSA120_2,
       "AerogelTSA120_2",
       24.8,
       0.20,
       1.0401,
       e,
       {0.15*cm, 0.18*cm, 0.22*cm, 0.26*cm, 0.30*cm, 0.35*cm, 0.40*cm, 0.46*cm, 0.53*cm, 0.60*cm,
	0.67*cm, 0.76*cm, 0.85*cm, 0.94*cm, 1.05*cm, 1.16*cm, 1.28*cm, 1.41*cm, 1.54*cm, 1.69*cm,
	1.84*cm, 2.01*cm, 2.18*cm, 2.36*cm, 2.56*cm, 2.76*cm, 2.98*cm, 3.20*cm, 3.44*cm, 3.69*cm,
	3.95*cm, 4.23*cm, 4.52*cm, 4.82*cm, 5.14*cm, 5.47*cm, 5.81*cm, 6.17*cm, 6.55*cm, 6.94*cm,
	7.35*cm, 7.77*cm, 8.22*cm, 8.68*cm, 9.15*cm, 9.65*cm, 10.17*cm, 10.70*cm, 11.26*cm, 11.83*cm,
	12.43*cm, 13.05*cm, 13.68*cm, 14.34*cm, 15.03*cm, 15.73*cm, 16.46*cm, 17.22*cm, 18.00*cm, 18.80*cm,
	19.63*cm, 20.48*cm, 21.36*cm, 22.27*cm, 23.20*cm, 24.16*cm, 25.15*cm, 26.17*cm, 27.21*cm, 28.29*cm,
	29.39*cm, 30.53*cm, 31.69*cm, 32.89*cm, 34.11*cm, 35.37*cm, 36.66*cm, 37.99*cm, 39.34*cm, 40.73*cm,
	42.16*cm, 43.61*cm, 45.11*cm, 46.63*cm, 48.19*cm, 49.79*cm, 51.42*cm, 53.09*cm, 54.79*cm, 56.54*cm,
	58.31*cm, 60.13*cm, 61.98*cm, 63.87*cm, 65.80*cm, 67.77*cm, 69.77*cm, 71.81*cm, 73.90*cm, 76.02*cm}
     }
    }
  };

  auto it = cfg.find(tag);
  if (it == cfg.end())throw std::runtime_error("Unknown aerogel tag: " + tag);

  return it->second;
}
// -------------------------------------------------------------------------------------

void Materials::BuildAerogel(const AerogelConfig &cfg)
{
  auto *aerogel = new G4RadiatorMaterial(cfg.name.c_str(), cfg.density_gcm3 * g / cm3, 4);
  aerogel->AddElement(m_Si, 0.45);
  aerogel->AddElement(m_O,  0.48);
  aerogel->AddElement(m_H,  0.01);
  aerogel->AddElement(m_C,  0.06);
  
  std::vector<G4double> photonE = cfg.photon_energies;
  std::vector<G4double> absorption = cfg.absorption_lengths_mm;
  std::vector<G4double> rindex(photonE.size(), cfg.refractive_index);
  
  auto *mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("RINDEX", photonE.data(), rindex.data(), photonE.size());
  mpt->AddProperty("ABSLENGTH", photonE.data(), absorption.data(), photonE.size());
  aerogel->SetMaterialPropertiesTable(mpt);

  m_Aerogel[cfg.id] = aerogel;
  m_AerogelThicknessMM[cfg.id] = cfg.thickness_mm;
}
// ------------------------------------------------------------------------------------- 
double Materials::GetAerogelThickness(unsigned id) const
{
  auto it = m_AerogelThicknessMM.find(id);
  if (it == m_AerogelThicknessMM.end()) {
    throw std::runtime_error("Aerogel thickness not found for id");
  }
  return it->second;
}
// -------------------------------------------------------------------------------------
unsigned Materials::GetAerogelId(const G4String &tag)
{
  return GetAerogelConfig(tag).id;
}
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
  printf("@@@ Materials::DefineElements() ...\n");
  
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

void Materials::CreateBelleIIAerogel(bool native, unsigned aid, const char *aname, double ri)
{
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
    //+double densities[2] = {0.0, 0.0}, nominal_ri[2] = {1.045, 1.055}, a1040_ri = 1.040;
    double densities[2]  = {0.0, 0.0};
    double nominal_ri[2] = {1.045, 1.055};//, a1040_ri = 1.020;
    
    // Will be used in several places;
    double dri10 = nominal_ri[1] - nominal_ri[0];
    double dri20 = /*a1040_ri*/ri - nominal_ri[0];
    
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
      //if (native) 
      //auto aerogel = new G4RadiatorMaterial(name, atof(dens->GetFirstString())*g/cm3, elems.size());
      G4RadiatorMaterial *aerogel =
	native ? new G4RadiatorMaterial(name, atof(dens->GetFirstString())*g/cm3, elems.size()) : 0;

      // FIXME: assuming that element composition is the same (which is true);
      if (im) {
	// FIXME: this is not right (must account for density scaling with <n>);
	double slope = (densities[1] - densities[0])/dri10;//(nominal_ri[1] - nominal_ri[0]);

	//a1040 = new G4RadiatorMaterial("BelleIIAerogel3", (densities[0] + slope*dri20)*g/cm3, elems.size());
	a1040 = new G4RadiatorMaterial(aname, (densities[0] + slope*dri20)*g/cm3, elems.size());
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
	if (aerogel) aerogel->AddElement(ptr, atof(fraction)*100*perCent);

	if (im) a1040->AddElement(ptr, atof(fraction)*100*perCent);
      } //for elem

      //if (native) 
      //G4MaterialPropertiesTable *mpt     =      new G4MaterialPropertiesTable();
      G4MaterialPropertiesTable *mpt = native ? new G4MaterialPropertiesTable() : 0;
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

	if (native) mpt->AddProperty(name, e, v, counter);
	if (im) mpt1040->AddProperty(name, e, vbff, counter);
#if _OFF_
	if (im) {
	  char buffer[128];
	  snprintf(buffer, 128-1, "aerogel-1.040.%s.txt", name);
	  FILE *fout = fopen(buffer, "w");

	  if (!strcmp(name, "RINDEX"))
	    for(unsigned iq=0; iq<dim; iq++)
	      fprintf(fout, "%7.2f*eV %8.3f\n",    e1040[name][iq] / eV, vbff[iq]);
	  else
	    for(unsigned iq=0; iq<dim; iq++)
	      fprintf(fout, "%7.2f*eV %8.3f*mm\n", e1040[name][iq] / eV, vbff[iq]);
	  
	  fclose(fout);
	} //if
#endif
      } //for prop

      if (native) {
	aerogel->SetMaterialPropertiesTable(mpt);
	m_Aerogel[id[im]] = aerogel;
      } //if
      if (im) {
	a1040->SetMaterialPropertiesTable(mpt1040);
	//+m_Aerogel[_AEROGEL_BELLE_II_REFRACTIVE_INDEX_1_04_] = a1040;
	m_Aerogel[aid] = a1040;
      } //if
    } //for mat
    //printf("%s\n", mat->GetName());
  }
#endif

  //Sanity check
  auto *table = G4Material::GetMaterialTable();
  for (auto *mat : *table) {
    if (!mat->GetIonisation()) {
      G4cout << "Material without ionisation: " << mat->GetName() << G4endl;
    }
  }
 
} // Materials::CreateBelleIIAerogel()

// -------------------------------------------------------------------------------------

//void Materials::DefineMaterials(double ri3, double ri4)
void Materials::DefineMaterials(const G4String &aerogelTag, double ri3, double ri4)
{
  printf("@@@ Materials::DefineMaterials() ...\n");
  {
    G4NistManager *manager = G4NistManager::Instance();

    m_Aluminum = manager->FindOrBuildMaterial("G4_Al");    assert(m_Aluminum);
    m_Silver   = manager->FindOrBuildMaterial("G4_Ag");    assert(m_Silver);
    m_Copper   = manager->FindOrBuildMaterial("G4_Cu");    assert(m_Copper);
    m_Titanium = manager->FindOrBuildMaterial("G4_Ti");    assert(m_Titanium);
    m_Water    = manager->FindOrBuildMaterial("G4_WATER"); assert(m_Water);
    m_Silicon  = manager->FindOrBuildMaterial("G4_Si");    assert(m_Silicon);
  }  

  //---------------------
  // Air;
  //---------------------
  {
    // This will be air without optical properties; looks the easiest way to 
    // cut optical photons at the vessel envelope (and world volume) border;
    m_Air = new G4Material("Air", 1.29*mg/cm3, 2);
    
    m_Air->AddElement(m_N, 70.*perCent);
    m_Air->AddElement(m_O, 30.*perCent);
  }

  //---------------------
  // Nitrogen:
  // n−1=6.8552×10−5+3.243157×10−2144−λ−2
  //---------------------
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

  //---------------------
  // Sapphire
  //---------------------
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

  //---------------------
  // Import UV silica
  // refractive index data;
  //---------------------
  {
    auto riTable = new TofPetMultiColumn("./database/RefractiveIndex.txt", '\t');

    // FIXME: <n> hardcoded;
    m_FusedSilica = new G4RadiatorMaterial("FusedSilica", 2.20*g/cm3, 2, 1.46);
    
    m_FusedSilica->AddElement(m_Si, 1);
    m_FusedSilica->AddElement(m_O,  2);

    auto wl = riTable->GetDataPoints(0), ri = riTable->GetDataPoints(3);
    unsigned riDim = wl.size();

    // FIXME: use constant for now;
    {
      G4double energy[riDim], refractiveIndex[500];
      
      //for(int iq=riDim-1; iq>=0; iq--) {
      //energy         [iq] =  (1240.0/wl[iq])*eV;
      //printf("%3d -> %7.3f\n", iq, energy[iq]*1E6);
      //refractiveIndex[iq] = ri[iq];
      //} //for iq 
      for(unsigned iq=0; iq<riDim; iq++) {
	energy         [iq] =  (1240.0/wl[riDim-iq-1])*eV;
	//printf("%3d -> %7.3f\n", iq, energy[iq]*1E6);
	refractiveIndex[iq] =          ri[riDim-iq-1];
      } //for iq 
      //#if _TODAY_
      G4MaterialPropertiesTable* fsMPT = new G4MaterialPropertiesTable();
      fsMPT->AddProperty("RINDEX", energy, refractiveIndex, riDim);
      
      m_FusedSilica->SetMaterialPropertiesTable(fsMPT);
      //#endif
    }
  }

  //---------------------
  // Aerogel
  //---------------------
#if 1//_TODAY_
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
      auto [n, t, nE, Ephoton, abslength] = mapAgel[_AEROGEL_1_];
      aerogelMPT->AddProperty("RINDEX", GetPhotonEnergies(), refractiveIndex, _WLDIM_);
      aerogelMPT->AddProperty("ABSLENGTH",Ephoton, abslength, nE);
      
      aerogel->SetMaterialPropertiesTable(aerogelMPT);
#else
      aerogel->SetRIChOptics(new g4dRIChAerogel(aerogel));
      // FIXME: tuned model#3 to n ~ 1.02 @ 400ns (which corresponds to 110 mg/cm^3 density);
      aerogel->GetRIChOptics()->setOpticalParams(3);
      aerogel->GetRIChOptics()->pTable->GetProperty("RINDEX");//->SetSpline(true);
#endif

      m_Aerogel[density[il]] = aerogel;
    } //for il
  } 

  // FIXME: keep track of #define's and actual values; do it better later;
  if (!aerogelTag.empty()) BuildAerogel(GetAerogelConfig(aerogelTag));
  else {
    CreateBelleIIAerogel(true,  _AEROGEL_BELLE_II_REFRACTIVE_INDEX_Ag3_, "BelleIIAerogel3", ri3);//1.014);
    CreateBelleIIAerogel(false, _AEROGEL_BELLE_II_REFRACTIVE_INDEX_Ag4_, "BelleIIAerogel4", ri4);//1.014);
  }
  
  //---------------------
  // Acrylic
  //---------------------
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
    m_Acrylic->GetRIChOptics()->pTable->GetProperty("RINDEX");//->SetSpline(true);
#endif
  }

  //---------------------
  // C2F6 as a gas
  // radiator option;
  //---------------------
  {
    m_C2F6 = new G4RadiatorMaterial("C2F6", 5.7*mg/cm3, 2);

    m_C2F6->AddElement(m_C , 2);
    m_C2F6->AddElement(m_F , 6);

    m_C2F6->SetRIChOptics(new g4dRIChGas(m_C2F6));
    m_C2F6->GetRIChOptics()->setOpticalParams();
    m_C2F6->GetRIChOptics()->pTable->GetProperty("RINDEX");//->SetSpline(true);
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

  //---------------------
  // Ceramic imitation; assume DuPont 951 variety for both the HRPPD anode base plate and the 
  // walls; chemical composition is unknown; it is a "mixture of Al2O3, CaZr03 and glass" at 
  // 3.10 g/cm^3; take CeramTape GC (density 2.87 g/cm^2) as a closest known reference;
  //---------------------
  {
    m_Ceramic = new G4Material("Ceramic", 3.10*g/cm3, 4);
    
    // Ignore inner traces;
    m_Ceramic->AddElement(m_O,  54.85*perCent);
    m_Ceramic->AddElement(m_Al, 30.09*perCent);
    m_Ceramic->AddElement(m_Si,  8.86*perCent);
    m_Ceramic->AddElement(m_Ca,  6.20*perCent);
  }

  //---------------------
  // A fake absorber material
  // (without optical properties);
  //---------------------
  {
    m_Absorber = new G4Material("Absorber", 1.00*g/cm3, 1);
    
    m_Absorber->AddElement(m_C, 1);
  }

  //---------------------
  // A fake carbon with
  // 1g/cm^3 density;
  //---------------------
  {
    m_FakeCarbon_1_g_cm3 = new G4Material("FakeCarbon", 1.00*g/cm3, 1);
    
    m_FakeCarbon_1_g_cm3->AddElement(m_C, 1);
  }

  double fake_carbon_rad_length = m_FakeCarbon_1_g_cm3->GetRadlen();

  //---------------------
  // 1/2" and 1/4" thick
  // honecomb "materials"
  // of effective density;
  //---------------------
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

  //---------------------
  // A fake carbon fiber with a resonable density to match Prakhar's numbers, see Materials.h;
  //---------------------
  {
    m_CarbonFiber = new G4Material("CarbonFiber", (1.00*fake_carbon_rad_length/_CF_RAD_LENGTH_)*g/cm3, 1);
    //m_CarbonFiber = new G4Material("CarbonFiber", 1.90*mg/cm3, 1);
    
    m_CarbonFiber->AddElement(m_C, 1);
    //printf("@@@ %f\n", m_CarbonFiber->GetRadlen()/mm);
  }

  //---------------------
  // A fake FR4 with a resonable density to match PDG number, see Materials.h;
  //---------------------
  {
    m_FR4 = new G4Material("FR4", (1.00*fake_carbon_rad_length/_FR4_RAD_LENGTH_)*g/cm3, 1);
    
    m_FR4->AddElement(m_C, 1);
  }

  //---------------------
  // Delrin
  //---------------------
  {
    // Well, the only part what matters here is the refractive index; density is fake;
    m_Delrin = new G4Material("Delrin", 1.41*g/cm3, 3);

    m_Delrin->AddElement(m_C, 1);
    m_Delrin->AddElement(m_H, 2);
    m_Delrin->AddElement(m_O, 1);
  }

  //---------------------
  // PEEK
  //---------------------
  {
    // Well, the only part what matters here is the refractive index; density is fake;
    m_PEEK = new G4Material("PEEK", 1.31*g/cm3, 3);

    m_PEEK->AddElement(m_C, 19);
    m_PEEK->AddElement(m_H, 14);
    m_PEEK->AddElement(m_O,  3);
  }

  //---------------------
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
#endif

#ifdef _DUMP_SELECTED_MATERIALS_
  {
    DumpMaterialProperty(m_FusedSilica, "RINDEX", 1/eV, "%7.2f*eV %7.2f");
  }
#endif
} // Materials::DefineMaterials()

// -------------------------------------------------------------------------------------

void Materials::DumpMaterialProperty(const G4Material *material, const char *property,
				     double scale, const char *fmt)
{
  //printf("@@@ %s\n", material->GetName().c_str());
  char buffer[128];
  snprintf(buffer, 128-1, "%s.%s.txt", material->GetName().c_str(), property);
  FILE *fout = fopen(buffer, "w");

  auto mpt = material->GetMaterialPropertiesTable();
  auto ptr = mpt->GetProperty(property);
  //printf("@@@ -> %ld\n", ptr->GetVectorLength());
  for(unsigned iq=0; iq<ptr->GetVectorLength(); iq++)
    fprintf(fout, fmt, scale*ptr->Energy(iq), (*ptr)[iq]);
  
  fclose(fout);
} // Materials::DumpMaterialProperty()

// -------------------------------------------------------------------------------------
