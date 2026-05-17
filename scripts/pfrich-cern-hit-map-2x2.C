//
//   export SANDBOX=/home/eic/CERN-2026
//   export LD_LIBRARY_PATH=${SANDBOX}/lib:${LD_LIBRARY_PATH}
//
//   root -l './scripts/pfrich-cern-hit-map-2x2.C("pfrich-cern-events.root")'
//

void pfrich_cern_hit_map_2x2(const char *dfname, const char *cfname = 0)
{
  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<IRT2::CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new IRT2::CherenkovEvent();
  t->SetBranchAddress("e", &event);

  int nEvents = t->GetEntries();

  unsigned dim = 120;//81;
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
  //hxy->SetMinimum(2);
  hxy->Draw("COLZ");

  gPad->SetGrid();
} // pfrich_cern_hit_map_2x2()
