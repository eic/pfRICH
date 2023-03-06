

void multi_eval(const char *dfname, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  //
  // Factory configuration part;
  //
  //reco->IgnoreTimingInChiSquare();
  //reco->IgnorePoissonTermInChiSquare();
  //reco->SetSingleHitCCDFcut(0.005);
  //reco->RemoveAmbiguousHits();
  // This only affects the calibration stage;
  //reco->SetDefaultSinglePhotonThetaResolution(0.0040);
  // Sensor active area pixelated will be pixellated NxN in digitization;
  //reco->SetSensorActiveAreaPixellation(24);
  // [rad] (should match SPE sigma) & [ns];
  //auto *a1 = reco->UseRadiator("Aerogel225",      0.0040);
  auto *a1 = reco->UseRadiator("BelleIIAerogel1");
  //reco->SetSinglePhotonTimingResolution(0.030);
  //reco->SetQuietMode();
  reco->AddHypothesis("pi+");
  reco->AddHypothesis(321);
  //reco->IgnoreMcTruthPhotonDirectionSeed();

  // Mark all pads hit by "calibration" (above the QE curve) photons as "useless";
  reco->AddBlackoutRadiator("QuartzWindow");
  reco->AddBlackoutRadiator("Acrylic");
  reco->AddBlackoutRadiator("GasVolume");
  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);

  auto hmatch = new TH1D("hmatch", "PID evaluation correctness",       2,    0,      2);
  auto hthtr1 = new TH1D("thtr1",  "Cherenkov angle (track)",        200,  220,    320);
  // For a dual aerogel configuration;
  //auto hthtr2  = new TH1D("thtr2",   "Cherenkov angle (track)",        200,  220,    320);

  reco->UseAutomaticCalibration();
  // This call is mandatory; second argument: statistics (default: all events);
  reco->PerformCalibration(200);
  {
    CherenkovEvent *event;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      for(auto mcparticle: event->ChargedParticles()) {
	if (!mcparticle->IsPrimary()) continue;

	if (mcparticle->GetPDG() == mcparticle->GetRecoPdgCode()) {
	  hmatch->Fill(0.5);
	} else {
	  hmatch->Fill(1.5);
	} //if	  

	hthtr1->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a1));
	//hthtr2->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a2));
      } //for mcparticle
    } //while
  }

  auto cv = new TCanvas("cv", "", 1600, 1000);
  cv->Divide(4, 3);
  cv->cd(1); reco->hthph()->Fit("gaus");
  cv->cd(2); reco->hccdfph()->SetMinimum(0); reco->hccdfph()->Draw();
  cv->cd(3); reco->hccdftr()->SetMinimum(0); reco->hccdftr()->Draw();
  cv->cd(4); reco->hccdfev()->SetMinimum(0); reco->hccdfev()->Draw();
  cv->cd(5); reco->hnpetr()->Draw();
  cv->cd(6); reco->hmatch()->SetMinimum(0); reco->hmatch()->Draw();
  cv->cd(7);       hmatch  ->SetMinimum(0);       hmatch  ->Draw();
  cv->cd(8); reco->hdtph()->Fit("gaus");
  cv->cd(9); hthtr1->Fit("gaus");
  //cv->cd(10); hthtr2->Fit("gaus");
  cv->cd(10); reco->hwl()->Draw();
  cv->cd(11); reco->hvtx()->Draw();
  cv->cd(12); reco->hri()->Draw();
} // multi_eval()
