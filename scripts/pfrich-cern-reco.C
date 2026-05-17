//
//   export SANDBOX=/home/eic/CERN-2026
//   export LD_LIBRARY_PATH=${SANDBOX}/lib:${LD_LIBRARY_PATH}
//
//   root -l './scripts/pfrich-cern-reco.C("pfrich-cern-events.root", "pfrich-cern-reco.json")'
//

void pfrich_cern_reco(const char *dfname, const char *jfname, unsigned stat = 0)
{
  // Initialize and configure ReconstructionFactory;
  auto *reco = new IRT2::ReconstructionFactory(dfname, dfname, "pfRICH-2x2");
  reco->JsonParser(jfname);

  // Loop through events;
  reco->BareEventLoop(stat);
  
  // Output 1D histograms; canvas sizes / offsets are tuned for a 1920 x 1200 pixel display;
  {
    auto *ra = reco->GetMyRICH()->GetRadiator("BelleIIAerogel3");
    ra  ->DisplayStandardPlots("wa", "Aerogel radiator",     -10,  10, 1250, 540);
    
    reco->DisplayStandardPlots("Track / event level plots", 1265,  10,  625, 800);
  }
} // pfrich_cern_reco()
