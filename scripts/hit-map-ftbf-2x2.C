
// ? export LD_LIBRARY_PATH=/home/ayk/eic/EicSandbox/build/lib:${LD_LIBRARY_PATH}
//   export LD_LIBRARY_PATH=/home/ayk/eic/irt/build/lib:${LD_LIBRARY_PATH}
//
//   root -l './hit-map-ftbf-2x2.C("pfrich.root")'
//

void hit_map_ftbf_2x2(const char *dfname, const char *cfname = 0)
{
  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);

  int nEvents = t->GetEntries();

  unsigned dim = 81;
  double pitch = 3.25, size = dim*pitch;
  auto hxy = new TH2D("hxy", "", dim, -size/2, size/2, dim, -size/2, size/2);

  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    for(auto particle: event->ChargedParticles()) {

      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto history  = particle->GetHistory (rhistory);

	for(auto photon: history->Photons()) {
	  if (!photon->WasDetected() ) continue;

	  TVector3 phx = photon->GetDetectionPosition();
	  //printf("%f\n", phx.Z());
	  if (phx.Z() < -1000) hxy->Fill(phx.X(), phx.Y());
	} //for photon
      } //for rhistory
    } //for particle
  } //for ev

  gStyle->SetOptStat(0);
  auto cv = new TCanvas("cv", "", 1000, 1000);
  hxy->GetXaxis()->SetTitle("Sensor plane X, [mm]");
  hxy->GetYaxis()->SetTitle("Sensor plane Y, [mm]");
  hxy->GetXaxis()->SetTitleOffset(1.20);
  hxy->GetYaxis()->SetTitleOffset(1.40);
  //hxy->SetMaximum(300);
  hxy->Draw("COL");
} // hit_map_ftbf_2x2()
