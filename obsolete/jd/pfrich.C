
// ? export LD_LIBRARY_PATH=/home/ayk/eic/EicSandbox/build/lib:${LD_LIBRARY_PATH}
//   export LD_LIBRARY_PATH=/home/ayk/eic/irt/build/lib:${LD_LIBRARY_PATH}
//
//   root -l './pfrich.C("rich.root")'
//

void pfrich(const char *dfname, const char *cfname = 0)
{
  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);

  int nEvents = t->GetEntries();

  auto hxy = new TH2D("hxy", "", 650, -650., 650., 650, -650.0, 650.);

  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    //printf("@@@ %2lu\n", event->ChargedParticles().size());
    for(auto particle: event->ChargedParticles()) {
      //if (particle->GetPDG() != 211) continue;

      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto history  = particle->GetHistory (rhistory);

	for(auto photon: history->Photons()) {
	  //printf("Here! %d\n", photon->WasDetected());
	  if (!photon->WasDetected() ) continue;

	  TVector3 phx = photon->GetDetectionPosition();
	  hxy->Fill(phx.X(), phx.Y());
	} //for photon
      } //for rhistory
    } //for particle
  } //for ev

  gStyle->SetOptStat(0);
  auto cv = new TCanvas("cv", "", 1000, 1000);
  hxy->GetXaxis()->SetTitle("Sensor plane X, [mm]");
  hxy->GetYaxis()->SetTitle("Sensor plane Y, [mm]");
  hxy->Draw("COL");
} // pfrich()
