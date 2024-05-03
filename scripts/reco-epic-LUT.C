

void reco_epic_LUT(const char *dfname, const char *output, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  //const char* output="test.hist.root";
  TFile *ofile = TFile::Open(output,"recreate");

  TTree *tree = new TTree("T","Tree of Quantities");

  Int_t partPDG;
  Double_t partMom;
  Double_t partEta;
  Double_t partPhi;
  Double_t partVertX;
  Double_t partVertY;
  Double_t partVertZ;

  Int_t recoPDG;
  Int_t recoPhotons;
  Int_t recoCherenkovHitCount;
  Double_t recoTrackCherenkovAngle;

  tree->Branch("partPDG",&partPDG,"partPDG/I");
  tree->Branch("partMom",&partMom,"partMom/D");
  tree->Branch("partEta",&partEta,"partEta/D");
  tree->Branch("partPhi",&partPhi,"partPhi/D");
  tree->Branch("partVertX",&partVertX,"partVertX/D");
  tree->Branch("partVertY",&partVertY,"partVertY/D");
  tree->Branch("partVertZ",&partVertZ,"partVertZ/D");

  tree->Branch("recoPDG",&recoPDG,"recoPDG/I");
  tree->Branch("recoPhotons",&recoPhotons,"recoPhotons/I");
  tree->Branch("recoCherenkovHitCount",&recoCherenkovHitCount,"recoCherenkovHitCount/I");
  tree->Branch("recoTrackCherenkovAngle",&recoTrackCherenkovAngle,"recoTrackCherenkovAngle/D");

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

  auto *a1 = reco->UseRadiator("BelleIIAerogel3");
  //auto *a1 = reco->UseRadiator("BelleIIAerogel3");


  //reco->SetSinglePhotonTimingResolution(0.030);
  //reco->SetQuietMode();
  //reco->AddHypothesis(-11);
  reco->AddHypothesis(11);
  reco->AddHypothesis("pi+");
  reco->AddHypothesis(321);
  reco->AddHypothesis(2212);
  //reco->IgnoreMcTruthPhotonDirectionSeed();

  // Mark all pads hit by "calibration" (above the QE curve) photons as "useless";
  reco->AddBlackoutRadiator("QuartzWindow");
  reco->AddBlackoutRadiator("Acrylic");
  reco->AddBlackoutRadiator("GasVolume");
  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);

  //auto hEta = new TH1D("hEta","",100,-5.,5.);
  //auto hMom = new TH1D("hMom","",100,0.,10.);

  //auto factoryPhotons = new TH1D("factoryPhotons","",50,0.,50.);

  //auto hmatch = new TH1D("hmatch", "PID evaluation correctness",       2,    0,      2);
  //auto hthtr1 = new TH1D("thtr1",  "Cherenkov angle (track)",        200,  220,    320);
  //auto hthtr1 = new TH1D("thtr1",  "Cherenkov angle (track)",        40,  270, 290);
  //auto hthtr1 = new TH1D("thtr1", "Cherenkov angle (track)", 580, 0., 290.);
  // For a dual aerogel configuration;
  //auto hthtr2  = new TH1D("thtr2",   "Cherenkov angle (track)",        200,  220,    320);

  reco->UseAutomaticCalibration();
  // This call is mandatory; second argument: statistics (default: all events);
  reco->PerformCalibration(200);

  //int numElec = 0;
  //int numPion = 0;
  //int numKaon = 0;
  //int numProt = 0;
  {
    CherenkovEvent *event;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      for(auto mcparticle: event->ChargedParticles()) {
	if (!mcparticle->IsPrimary()) continue;

	/*
	if (mcparticle->GetPDG() == mcparticle->GetRecoPdgCode()) {
	  hmatch->Fill(0.5);
	} else {
	  hmatch->Fill(1.5);
	} //if	 
	*/

	// GetRecoCherenkovPhotonCount

	//hEta->Fill(mcparticle->GetVertexMomentum().PseudoRapidity());
	//hMom->Fill(mcparticle->GetVertexMomentum().Mag());

	partPDG = mcparticle->GetPDG();
	partMom = mcparticle->GetVertexMomentum().Mag();
	partEta = mcparticle->GetVertexMomentum().PseudoRapidity();
	partPhi = mcparticle->GetVertexMomentum().Phi();
	partVertX = mcparticle->GetVertexPosition().X();
	partVertY = mcparticle->GetVertexPosition().Y();
	partVertZ = mcparticle->GetVertexPosition().Z();

	recoPDG = mcparticle->GetRecoPdgCode();
	recoPhotons = mcparticle->GetRecoCherenkovPhotonCount();
	recoCherenkovHitCount = mcparticle->GetRecoCherenkovHitCount();
	recoTrackCherenkovAngle = 1000.0*mcparticle->GetRecoCherenkovAverageTheta(a1);

	tree->Fill();

	//if(TMath::Abs(mcparticle->GetRecoPdgCode()) == 11) numElec++;
	//if(TMath::Abs(mcparticle->GetRecoPdgCode()) == 211) numPion++;
	//if(TMath::Abs(mcparticle->GetRecoPdgCode()) == 321) numKaon++;
	//if(TMath::Abs(mcparticle->GetRecoPdgCode()) == 2212) numProt++;

	//hthtr1->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a1));
	//hthtr2->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a2));
      } //for mcparticle
    } //while
  }

  //factoryPhotons = (TH1D *)reco->hnpetr()->Clone("factoryPhotons");

  ///hEta->Write();
  //hMom->Write();
  //factoryPhotons->Write();
  tree->Write();
  ofile->Close();

}
