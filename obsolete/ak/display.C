
void display(const char *dfname, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  // [rad] (should match SPE sigma) & [ns];
  auto *a1 = reco->UseRadiator("Aerogel225", 0.0041);
  //reco->AddHypothesis("mu+");
  reco->AddHypothesis("pi+");
  //reco->AddHypothesis(321);

  // Mark all pads hit by "calibration" (above the QE curve) photons as "useless";
  reco->AddBlackoutRadiator("QuartzWindow");
  reco->AddBlackoutRadiator("GasVolume");
  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);

  auto hxy = new TH2D("hxy", "", 650, -650., 650., 650, -650.0, 650.);

  // This call is mandatory;
  reco->PerformCalibration();
  {
    CherenkovEvent *event;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      for(auto mcparticle: event->ChargedParticles()) {
	for(auto rhistory: mcparticle->GetRadiatorHistory()) {
	  auto history  = mcparticle->GetHistory (rhistory);
	  
	  for(auto photon: history->Photons()) {
	    //printf("%d %d\n", photon->WasDetected(), photon->IsMarkedAsBlackout());
	    if (!photon->WasDetected() ) continue;
	    //if (!photon->IsUsefulForCalibration()) continue;
	    
	    if (photon->IsMarkedAsBlackout()) continue;

	    TVector3 phx = photon->GetDetectionPosition();
	    hxy->Fill(phx.X(), phx.Y());
	  } //for photon
	} //for rhistory
      } //for mcparticle
    } //while
  }

  gStyle->SetOptStat(0);
  auto cv = new TCanvas("cv", "", 1000, 1000);
  hxy->GetXaxis()->SetTitle("Sensor plane X, [mm]");
  hxy->GetYaxis()->SetTitle("Sensor plane Y, [mm]");
  hxy->Draw("COL");
} // display()
