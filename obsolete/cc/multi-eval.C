

void multi_eval()
{
  const char *dfname = "pfrich.root";
  const char *cfname =0;
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

  // Mark all pads hit by "calibration" (above the QE curve) photons as "useless";
  reco->AddBlackoutRadiator("QuartzWindow");
  reco->AddBlackoutRadiator("Acrylic");
  reco->AddBlackoutRadiator("GasVolume");
  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);
  TH2D* hthphi[5]; for(int i=0; i<5; i++){TString hName; hName.Form("theta_phi_%d",i);  hthphi[i]= new TH2D(hName,hName, 360,-180,180,200,270,310);}
  auto hmatch = new TH1D("hmatch", "PID evaluation correctness",       2,    0,      2);
  auto hthtr1 = new TH1D("thtr1",  "Cherenkov angle (track)",        400,  -100,    100);
  // For a dual aerogel configuration;
  //auto hthtr2  = new TH1D("thtr2",   "Cherenkov angle (track)",        200,  220,    320);
  auto htt   = new TH2D("htt","Cherenkov photon residual for particle thetas",5,151,175,100,-30,30);
  reco->UseAutomaticCalibration();
  // This call is mandatory; second argument: statistics (default: all events);
  reco->PerformCalibration(200);
  {
    CherenkovEvent *event;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      for(auto mcparticle: event->ChargedParticles()) {
	if (mcparticle->GetPDG() == mcparticle->GetRecoPdgCode()) {
	  hmatch->Fill(0.5);
	} else {
	  hmatch->Fill(1.5);
	} //if	  
        auto pp = mcparticle->GetVertexMomentum();
	double pmag = pp.Mag();
	double thhypo = 1000* (TMath::ACos(TMath::Sqrt(pmag*pmag+(0.139*0.139))/(1.04556*pmag)));    //costheta= 1/nbeta --> theta =arccos((p^2+m^2)/n.p) 
	double theta = pp.Theta()*(180./M_PI);
        double eta = pp.PseudoRapidity();
	for(unsigned id =0; id<mcparticle->GetRecoCherenkovHitCount(); id++){
	  double phth  = 1000*mcparticle->GetRecoCherenkovPhotonTheta(id); 
          double phphi = (180./M_PI)*mcparticle->GetRecoCherenkovPhotonPhi(id); 
	   if(theta>151 && theta<=156)hthphi[0]->Fill(phphi,phth);
	   if(theta>156 && theta<=161)hthphi[1]->Fill(phphi,phth);
	   if(theta>161 && theta<=166)hthphi[2]->Fill(phphi,phth);
	   if(theta>166 && theta<=171)hthphi[3]->Fill(phphi,phth);
	   if(theta>171 && theta<=176)hthphi[4]->Fill(phphi,phth);
          //for(int ii=0; ii<100; ii++)htt->Fill(theta,reco->hthph()->GetBinCenter(ii),reco->hthph()->GetBinContent(ii));}
	  //htt->Fill(theta,reco->GetThetaDiff());
	  //if(theta>151. && theta<=156.) hthphi->Fill(phphi,phth);
	}// Cherenkov Hit Loop
	for(int ii=1; ii<=100; ii++){htt->Fill(theta,reco->hthph()->GetBinCenter(ii),reco->hthph()->GetBinContent(ii));}
	hthtr1->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a1) - thhypo);
	//hthtr2->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(a2));
      } //for mcparticle
    } //while
  }
  vector<double> sigma; vector<double>mean;
  vector<double> thetabin; vector<double> binsize;
  vector<TH1D *> hh;
  /*
  for(int bin =1; bin<=htt->GetNbinsX()-1; bin++) {
	TString hName;
        hName.Form("h_%lf_%lf_py",((TAxis*)htt->GetXaxis())->GetBinCenter(bin),((TAxis*) htt->GetXaxis())->GetBinCenter(bin+1));
        thetabin.push_back(((TAxis*)htt->GetXaxis())->GetBinCenter(bin));
        binsize.push_back(((TAxis*)htt->GetXaxis())->GetBinCenter(bin+1)-((TAxis*)htt->GetXaxis())->GetBinCenter(bin));	
  	auto htmp= htt->ProjectionY(hName, bin, bin+1, "d");
	htmp->Fit("gaus","R","",-15,15);
	auto f1 =  htmp->GetFunction("gaus");
	sigma.push_back(f1->GetParameter(2));
	mean.push_back(f1->GetParameter(1));
	hh.push_back(htmp);
  }
  int size = sigma.size();
  auto GG = new TGraphErrors(size,&thetabin[0],&mean[0],&binsize[0],&sigma[0]);
  */
  auto cc = new TCanvas("cc","",1600,1000);
  //cc->Divide(3,1);
  //cc->cd(1);
  //hthphi->Draw("colz");
  //cc->cd(2);
  htt->Draw("COLZ");
  //cc->cd(3);
  //GG->Draw("AP *");

  auto cc1 = new TCanvas("cc1","",1600,1000);
  cc1->Divide(3,2);
  for(int i=0; i<5; i++){cc1->cd(i+1); hthphi[i]->Draw("colz");}

  
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
  cv->cd(9); hthtr1->Draw();
  //cv->cd(10); hthtr2->Fit("gaus");
  cv->cd(10); reco->hwl()->Draw();
  cv->cd(11); reco->hvtx()->Draw();
  cv->cd(12); reco->hri()->Draw();
  auto f = new TFile("out.root","RECREATE");
  reco->hthph()->Write();
  hthtr1->Write();
  reco->hnpetr()->Write();
  reco->hri()->Write();
  f->Close();
} // multi_eval()
