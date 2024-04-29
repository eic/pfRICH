
// ? export LD_LIBRARY_PATH=/home/ayk/eic/EicSandbox/build/lib:${LD_LIBRARY_PATH}
//   export LD_LIBRARY_PATH=/home/ayk/eic/irt/build/lib:${LD_LIBRARY_PATH}
//
//   root -l './hit-map-tstand.C("pfrich.root")'
//

void hit_map_tstand(const char *dfname, const char *cfname = 0)
{
  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);

  int nEvents = t->GetEntries();

  auto hxy = new TH2D("hxy", "", 100, -2.5, 2.5, 100, -2.5, 2.5);
  auto hx  = new TH1D("hx",  "", 100, -2.5, 2.5);
  auto hy  = new TH1D("hy",  "", 100, -2.5, 2.5);

  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    printf("%d\n", event->OrphanPhotons().size());

    for(auto photon: event->OrphanPhotons()) {
      if (!photon->WasDetected() ) continue;

      printf("Here!\n");
      TVector3 phx = photon->GetDetectionPosition();
      hxy->Fill(phx.X(), phx.Y());
      hx->Fill(phx.X());
      hy->Fill(phx.Y());
    } //for photon
  } //for ev

  gStyle->SetOptStat(0);
  auto cv = new TCanvas("cv", "", 1200, 400);
  cv->Divide(3);
  cv->cd(1);
  hxy->GetXaxis()->SetTitle("Sensor plane X, [mm]");
  hxy->GetYaxis()->SetTitle("Sensor plane Y, [mm]");
  hxy->GetXaxis()->SetTitleOffset(1.20);
  hxy->GetYaxis()->SetTitleOffset(1.40);
  hxy->Draw("COL");
  cv->cd(2); hx->Draw();
  cv->cd(3); hy->Draw();
} // hit_map_tstand()
