
// ? export LD_LIBRARY_PATH=/home/ayk/eic/EicSandbox/build/lib:${LD_LIBRARY_PATH}
//   export LD_LIBRARY_PATH=/home/ayk/eic/irt/build/lib:${LD_LIBRARY_PATH}
//
//   root -l './hit-map-cern-2x2.C("pfrich.root")'
//
void SetHisto(TH1* h);
void SetHisto2(TH1* h, int s=24, int c=1);
bool CheckPhotoPos(TVector3 pos);
//----------------------------------------------------------------------------------------
void hit_map_cern_2x2_ver1(const char *dfname, const char *cfname = 0)
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
  TH1::SetDefaultSumw2();
  
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
  auto hAllPhoton = new TH1D("hAllPhoton", ";Hits per track;",21 ,-0.5,20.5);
  auto hPhoton = new TH1D("hPhoton", ";Hits per track;", 21 ,-0.5,20.5);
  auto hAccept = new TH1D("hAccept", ";Accepted hits / all hits;", 20,0,1);

  int ievt=0;
  for(unsigned ev=0; ev<nEvents; ev++) {
    ievt++;
    if (ievt>100) break;
    t->GetEntry(ev);

    for(auto particle: event->ChargedParticles()) {

      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto history  = particle->GetHistory (rhistory);

	double nAcceptHit=0, nAllHit=0;
	for(auto photon: history->Photons()) {
	  if (!photon->WasDetected() ) continue;

	  TVector3 phx = photon->GetDetectionPosition();
	  //printf("%f\n", phx.Z());
	  bool fillActive=false;
          bool fillAll=false;
          if (phx.Z() < -1000) fillAll=true;
          if (CheckPhotoPos(phx)) fillActive=true;

          phx.RotateZ(TMath::Pi()/4.);
          if (fillActive) {
	    hxy->Fill(phx.X(), phx.Y());
	    nAcceptHit++;
	  }
          if (fillAll) {
	    hxyAll->Fill(phx.X(), phx.Y());
	    nAllHit++;
	  }
	} //for photon

	if (!nAllHit) continue;
        hAllPhoton->Fill(nAllHit);
        hPhoton->Fill(nAcceptHit);
        hAccept->Fill(nAcceptHit/nAllHit);

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

   auto cCnt= new TCanvas("cCnt", "hit cnt", 1200, 500);
  cCnt->Range(0,0,1,1);
  cCnt->Divide(2);

  cCnt->cd(1);
  gPad->SetTopMargin(0.01);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  gPad->SetGrid();
  SetHisto2(hAllPhoton, 24, kAzure);
  SetHisto2(hPhoton, 25, kPink);
  hAllPhoton->SetMaximum(1.2*max(hAllPhoton->GetMaximum(),hPhoton->GetMaximum()));
  hAllPhoton->Draw();
  hPhoton->SetMaximum(1.2*max(hAllPhoton->GetMaximum(),hPhoton->GetMaximum()));
  hPhoton->Draw("same");

  TLegend* legCnt=new TLegend(0.4,0.8,0.95,0.95);
  legCnt->SetFillStyle(0);
  legCnt->SetTextFont(43);
  legCnt->SetTextSize(20);
  legCnt->AddEntry(hAllPhoton,Form("All hits, mean=%.1f",hAllPhoton->GetMean()),"p");
  legCnt->AddEntry(hPhoton,Form("Accepted hits, mean=%.1f",hPhoton->GetMean()),"p");
  legCnt->Draw();

  cCnt->cd(2);
  gPad->SetTopMargin(0.01);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  gPad->SetGrid();
  SetHisto2(hAccept,24,1);
  hAccept->Draw();

  

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
//----------------------------------------------------------------------------------------
void SetHisto2(TH1* h, int s=24, int c=1)
{
  h->SetStats(0);
  h->SetMarkerStyle(s);
  h->SetMarkerColor(c);
  h->SetLineColor(c);

  h->GetXaxis()->SetNdivisions(505);
  h->GetXaxis()->SetTitleFont(43);
  h->GetXaxis()->SetTitleSize(20);
  h->GetXaxis()->SetLabelFont(43);
  h->GetXaxis()->SetLabelSize(20);

  h->GetYaxis()->SetNdivisions(505);
  h->GetYaxis()->SetTitleFont(43);
  h->GetYaxis()->SetTitleSize(20);
  h->GetYaxis()->SetLabelFont(43);
  h->GetYaxis()->SetLabelSize(20);
}
