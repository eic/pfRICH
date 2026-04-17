
// ? export LD_LIBRARY_PATH=/home/ayk/eic/EicSandbox/build/lib:${LD_LIBRARY_PATH}
//   export LD_LIBRARY_PATH=/home/ayk/eic/irt/build/lib:${LD_LIBRARY_PATH}
//
//   root -l './hit-map-cern-2x2.C("pfrich.root")'
//
void SetHisto(TH1* h);
bool CheckPhotoPos(TVector3 pos);
//----------------------------------------------------------------------------------------
void hit_map_cern_2x2_ver1(const char *dfname, const char *cfname = 0)
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);

  int nEvents = t->GetEntries();

  unsigned dim = 120;//81;
  double pitch = 3.25, size = dim*pitch;
  auto hxy = new TH2D("hxy", "", dim, -size/2, size/2, dim, -size/2, size/2);
  auto hxyAll = new TH2D("hxyAll", "", dim, -size/2, size/2, dim, -size/2, size/2);
  
  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    for(auto particle: event->ChargedParticles()) {

      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto history  = particle->GetHistory (rhistory);

	for(auto photon: history->Photons()) {
	  if (!photon->WasDetected() ) continue;

	  TVector3 phx = photon->GetDetectionPosition();
	  //printf("%f\n", phx.Z());
	  bool fillActive=false;
          bool fillAll=false;
          if (phx.Z() < -1000) fillAll=true;
          if (CheckPhotoPos(phx)) fillActive=true;

          phx.RotateZ(TMath::Pi()/4.);
          if (fillActive) hxy->Fill(phx.X(), phx.Y());
          if (fillAll) hxyAll->Fill(phx.X(), phx.Y());
	} //for photon
      } //for rhistory
    } //for particle
  } //for ev

  auto cv = new TCanvas("cv", "", 1000, 1000);
  cv->Range(0,0,1,1);
  gPad->SetGrid();
  SetHisto(hxyAll);
  hxyAll->SetMarkerStyle(7);
  hxyAll->SetMarkerColor(1);
  
  SetHisto(hxy);

  hxyAll->Draw("scat");
  hxy->Draw("COLZsame");
  
  double x[5]={-26*2, -26*2,  26*2,  26*2, -26*2};
  double y[5]={136.5-26*2,136.5+26*2,136.5+26*2,136.5-26*2,136.5-26*2};
  
  double theta = 45.0 * TMath::DegToRad(); // convert to radians                                                                                                                                   
  double xr[5], yr[5];

  TPolyLine *pline[4];// = new TPolyLine(5, xr, yr);         
  for (int j=0;j<4;j++) {
    for (int i = 0; i < 5; i++) {
      xr[i] = x[i]*cos(j*TMath::PiOver2()+theta) - y[i]*sin(j*TMath::PiOver2()+theta);
      yr[i] = x[i]*sin(j*TMath::PiOver2()+theta) + y[i]*cos(j*TMath::PiOver2()+theta);
    }
    pline[j] = new TPolyLine(5, xr, yr);
    pline[j]->SetLineColor(1);
    pline[j]->Draw();
  }

} // hit_map_cern_2x2()
//----------------------------------------------------------------------------------------
bool CheckPhotoPos(TVector3 pos)
{
  //true -- good

  if (pos.Z()>-1000.) return false;
  
  if (fabs(pos.X())<26. && fabs(fabs(pos.Y())-136.5+13)<13.) return true;
  else if (fabs(pos.Y())<26. && fabs(fabs(pos.X())-136.5+13)<13.) return true;

  return false;
}
//---------------------------------------------------------------------------------------- 
void SetHisto(TH1* h)
{
  h->SetStats(0);
  h->GetXaxis()->SetTitle("Sensor plane X, [mm]");
  h->GetYaxis()->SetTitle("Sensor plane Y, [mm]");
  h->GetXaxis()->SetTitleOffset(1.20);
  h->GetYaxis()->SetTitleOffset(1.40);
}
