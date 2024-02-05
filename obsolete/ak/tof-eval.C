

void tof_eval(const char *dfname, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  auto hnhit1d = new TH1D("nhit1d", "Digitized hit count",                       50,     0,     50);
  auto hnhit2d = new TH2D("nhit2d", "Digitized hit count",  50,    -2.5, -2.3,   50,     0,     50);
  auto htime1d = new TH1D("ntime1d","Timing",  50, 5.2, 6.2);
  auto htime2d = new TH2D("ntime2d","Timing", 100,    -2.5, -2.3, 50, 5.5, 6.5);
  auto hnpe    = new TH1D("npe",    "Photon count per hit",                      50,     0,     50);

  reco->SetSinglePhotonTimingResolution(0.050);
  reco->SetExperimentalMode();

  {
    CherenkovEvent *event;

    std::map<std::pair<unsigned, unsigned>, unsigned> stat;
    std::multimap<unsigned, std::pair<unsigned, unsigned>> istat;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      //printf("%3d\n", (int)reco->GetDigitizedHits().size());

      auto mcparticle = *event->ChargedParticles().begin();
      double eta = mcparticle->GetVertexMomentum().Eta();
      //printf("%f\n", eta);

      unsigned tstat = 0;
      double tsum = 0.0;
      for(unsigned iq=0; iq<reco->GetDigitizedHits().size(); iq++) {
	auto &hit = reco->GetDigitizedHits()[iq];
	//printf("%2d %2d\n", hit.m_iX, hit.m_iY);

	stat[std::make_pair(hit.m_iX, hit.m_iY)]++;

	//printf("%f\n", hit.GetAverageDetectionTime());
	//if (/*eta > -2.35 &&*/ hit.m_iX == 20 && hit.m_iY == 3) {
	if ((hit.m_iY >= 5 && hit.m_iY <= 6) && (hit.m_iX >= 19 && hit.m_iX <= 23) /*&&
						hit.GetPhotonCount() >= 5 && hit.GetPhotonCount() <= 15*/) {
	  //for(unsigned ip=0; ip<hit.GetPhotonCount(); ip++)
	  //printf("%2d %f\n", ip, hit.GetDetectionTime(ip));
	  
	  hnpe ->Fill(hit.GetPhotonCount());
	  if (hit.GetPhotonCount() >= 1) {
	    //printf("%f\n", hit.GetAverageDetectionTime());
	    //htime1d->Fill(hit.GetAverageDetectionTime());
	    htime1d->Fill(hit.GetDetectionTime(0));
	    //htime2d->Fill(eta, hit.GetAverageDetectionTime());
	  }

	  tstat++;
	  tsum += hit.GetDetectionTime(0);
	  //tsum += hit.GetAverageDetectionTime();
	} //if

      } //for iq

      if (tstat) {
	tsum /= tstat;//reco->GetDigitizedHits().size();
	//printf("%f\n", tsum);
	htime1d->Fill(tsum);//hit.GetDetectionTime(0));
      } 

      hnhit1d->Fill(reco->GetDigitizedHits().size());
      hnhit2d->Fill(eta, reco->GetDigitizedHits().size());
    } //while

    for(auto &entry: stat)
      istat.insert(std::make_pair(entry.second, entry.first));
    for(auto &entry: istat)
      printf("%3d -> %2d %2d\n", entry.first, entry.second.first, entry.second.second);
  }


  auto cv = new TCanvas("cv", "", 1600, 750);
  cv->Divide(4, 2);
  cv->cd(1); hnhit1d->Draw();
  cv->cd(2); hnhit2d->Draw("COLZ");
  cv->cd(3); htime1d->Fit("gaus");
  cv->cd(4); hnpe->Draw();
  cv->cd(5); htime2d->Draw("COLZ");
} // tof_eval()
