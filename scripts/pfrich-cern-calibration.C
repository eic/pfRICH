//
//   root -l './pfrich-cern-calibration.C("pfrich-cern-events.root", "pfrich-cern-calibration.json")'
////
//   export SANDBOX=/home/eic/CERN-2026
//   export LD_LIBRARY_PATH=${SANDBOX}/lib:${LD_LIBRARY_PATH}
//
//   root -l './scripts/pfrich-cern-calibration.C("pfrich-cern-events.root", "pfrich-cern-calibration.json")'
//

void pfrich_cern_calibration(const char *dfname, const char *jfname, unsigned stat = 0)
{
  auto *reco = new IRT2::ReconstructionFactory(dfname, dfname, "pfRICH-2x2");//"PFRICH");

  //
  // Factory configuration part; use minimal set of options;
  {
    reco->IgnorePoissonTermInChiSquare();
    // Should be close enough to the real one; this only affects the calibration stage;
    reco->SetDefaultSinglePhotonThetaResolution(0.005);
    // Sensor active area will be pixellated NxN in digitization; '32': HRPPD;
    reco->SetSensorActiveAreaPixellation(32);
    
    // One hypothesis suffices to run the reconstruction engine;
    reco->AddHypothesis("pi+");
    
    // Enable aerogel radiator for ring imaging;
    reco->GetMyRICH()->GetRadiator("BelleIIAerogel3")->UseInRingImaging();
  }
  
  // Perform pre-calibration; second argument: statistics to use (default: all events);
  {
    reco->PerformCalibration(stat);
    
    // Export a JSON file with these calibrations;
    reco->GetMyRICH()->ExportJsonFormatCalibrations(jfname);
  }
} // pfrich_cern_calibration()
