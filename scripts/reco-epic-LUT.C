void reco_epic_LUT(const char *dfname, const char *output, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  // const char* output="test.hist.root";
  TFile *ofile = TFile::Open(output, "recreate");

  TTree *tree = new TTree("T", "Tree of Quantities");

  Int_t partPDG;
  Double_t partMom;
  Double_t partEta;
  Double_t partTheta;
  Double_t partPhi;
  Double_t partVertX;
  Double_t partVertY;
  Double_t partVertZ;

  Int_t recoPDG;
  Int_t recoPhotons;
  Double_t recoTrackCherenkovAngle;

  tree->Branch("partPDG", &partPDG, "partPDG/I");
  tree->Branch("partMom", &partMom, "partMom/D");
  tree->Branch("partEta", &partEta, "partEta/D");
  tree->Branch("partTheta", &partTheta, "partTheta/D");
  tree->Branch("partPhi", &partPhi, "partPhi/D");
  tree->Branch("partVertX", &partVertX, "partVertX/D");
  tree->Branch("partVertY", &partVertY, "partVertY/D");
  tree->Branch("partVertZ", &partVertZ, "partVertZ/D");

  tree->Branch("recoPDG", &recoPDG, "recoPDG/I");
  tree->Branch("recoPhotons", &recoPhotons, "recoPhotons/I");
  tree->Branch("recoTrackCherenkovAngle", &recoTrackCherenkovAngle, "recoTrackCherenkovAngle/D");

  // Factory configuration part;
  //
  // reco->IgnoreTimingInChiSquare();
  // reco->IgnorePoissonTermInChiSquare();
  // reco->SetSingleHitCCDFcut(0.005);
  // reco->RemoveAmbiguousHits();
  // This only affects the calibration stage;
  // reco->SetDefaultSinglePhotonThetaResolution(0.0040);
  // Sensor active area pixelated will be pixellated NxN in digitization;
  // reco->SetSensorActiveAreaPixellation(24);
  // [rad] (should match SPE sigma) & [ns];
  // auto *a1 = reco->UseRadiator("Aerogel225",      0.0040);

  auto *a1 = reco->UseRadiator("BelleIIAerogel3");
  // auto *a1 = reco->UseRadiator("BelleIIAerogel3");

  // reco->SetSinglePhotonTimingResolution(0.030);
  // reco->SetQuietMode();
  // reco->AddHypothesis(-11);
  reco->AddHypothesis(11);
  reco->AddHypothesis("pi+");
  reco->AddHypothesis(321);
  reco->AddHypothesis(2212);
  // reco->IgnoreMcTruthPhotonDirectionSeed();

  // Mark all pads hit by "calibration" (above the QE curve) photons as "useless";
  reco->AddBlackoutRadiator("QuartzWindow");
  reco->AddBlackoutRadiator("Acrylic");
  reco->AddBlackoutRadiator("GasVolume");
  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);
  reco->ImportTrackingSmearing("./database/dtheta_seed_param.reformatted.dat", "./database/dphi_seed_param.reformatted.dat");
  
  reco->UseAutomaticCalibration();
  // This call is mandatory; second argument: statistics (default: all events);
  reco->PerformCalibration(200);

  // int numElec = 0;
  // int numPion = 0;
  // int numKaon = 0;
  // int numProt = 0;
  {
    CherenkovEvent *event;

    // Event loop;
    while ((event = reco->GetNextEvent()))
    {
      for (auto mcparticle : event->ChargedParticles())
      {
        if (!mcparticle->IsPrimary())
          continue;

        partPDG = mcparticle->GetPDG();
        partMom = mcparticle->GetVertexMomentum().Mag();
        partEta = mcparticle->GetVertexMomentum().PseudoRapidity();
        partPhi = mcparticle->GetVertexMomentum().Phi();
        partTheta = mcparticle->GetVertexMomentum().Theta();
        partVertX = mcparticle->GetVertexPosition().X();
        partVertY = mcparticle->GetVertexPosition().Y();
        partVertZ = mcparticle->GetVertexPosition().Z();

        recoPDG = mcparticle->GetRecoPdgCode();
        recoPhotons = mcparticle->GetRecoCherenkovPhotonCount();
        recoTrackCherenkovAngle = 1000.0 * mcparticle->GetRecoCherenkovAverageTheta(a1);

        tree->Fill();

      } // for mcparticle
    }   // while
  }

  tree->Write();
  ofile->Close();
}
