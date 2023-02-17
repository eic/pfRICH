
#define _MASS_      (0.140)
#define _PDG_         211
//#define _MASS_       (0.494)
//#define _PDG_         321

//#define _AEROGEL_ ("Aerogel155")
#define _AEROGEL_ ("Aerogel225")
//#define _AEROGEL_ ("BelleIIAerogel1")

// Kind of digitization; of course only one of the two must be used;
//#define _GAUSSIAN_SMEARING_            1.0
#define _DISCRETE_PIXEL_PITCH_         4.0

// [rad] & [mm];
#define _PHOTON_DETECTION_WINDOW_    0.012
#define _AVERAGE_ATTENUATION_LENGTH_   8.4

void e_eval(const char *dfname, const char *cfname = 0)
{

  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);
  auto ri = new TH1D("ri", "1.0 - Refractive Index",  500, 0.015,  0.055);
  auto qh = new TH1D("qh", "Cherenkov angle (SPE)",   100,   -30,     30);
  auto th = new TH1D("th", "Cherenkov angle (track)", 100,   -10,     10);
  auto np = new TH1D("np", "Photon count (pion?)",     50,     0,     50);
  auto nq = new TH1D("nq", "Photon count (radiator)",  50,     0,     50);
  auto wl = new TH1D("wl", "Wave Length",             200, 150.0,  800.0);
  auto tt = new TH2D("tt", "Cherenkov angle vs time",  50,  -0.2, 0.6, 50, -100,  100);
  auto z0 = new TH1D("z0", "True emission point",      80,   -40,     40);
  auto al = new TH1D("al", "True attenuation length", 100,     0,    100);

  int nEvents = t->GetEntries();

  auto rich = geometry->GetDetector("pfRICH");
  auto aerogel = rich->GetRadiator(_AEROGEL_);

  // First calculate average refractive index and theta for all detected photons
  // for every radiator;
  {
    for(unsigned ev=0; ev<nEvents; ev++) {
      t->GetEntry(ev);
      
      for(auto particle: event->ChargedParticles()) {
	if (particle->GetPDG() != _PDG_) continue;

	for(auto rhistory: particle->GetRadiatorHistory()) {
	  auto radiator = particle->GetRadiator(rhistory);
	  auto history  = particle->GetHistory (rhistory);
	  
	  if (history->StepCount() >= 2) {
	    auto from = history->GetStep(0), to = history->GetStep(history->StepCount()-1);
	    TVector3 ntrack = (from->GetMomentum() + to->GetMomentum()).Unit();
	    
	    for(auto photon: history->Photons()) {
	      if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;
	      
	      radiator->m_Stat++;
	      radiator->m_AverageTheta += acos(ntrack.Dot(photon->GetVertexMomentum().Unit()));
	      radiator->m_AverageRefractiveIndex += photon->GetVertexRefractiveIndex();
	      radiator->m_AverageVertexPosition  += photon->GetVertexPosition();
	    } //for photon
	  } //if
	} //for radiator
      } //for particle
    } //for ev

    for(auto rptr: rich->Radiators()) {
      printf("%s\n", rptr.first.Data());
      auto radiator = rptr.second;

      if (radiator->m_Stat) {
	radiator->m_AverageTheta           /= radiator->m_Stat;
	radiator->m_AverageRefractiveIndex /= radiator->m_Stat;
	radiator->m_AverageVertexPosition  *= 1.0/radiator->m_Stat;
      } //if
      
      printf("%5d photons, <theta> = %7.2f mrad; <n> = %8.5f\n", radiator->m_Stat,
	     1000*radiator->m_AverageTheta, radiator->m_AverageRefractiveIndex);
    } //for dt..radiator
  }

  aerogel->SetTrajectoryBinCount(10);
  aerogel->SetUniformSmearing(_PHOTON_DETECTION_WINDOW_);
  //aerogel->SetReferenceRefractiveIndex(_AVERAGE_REFRACTIVE_INDEX_);
  aerogel->SetReferenceAttenuationLength(_AVERAGE_ATTENUATION_LENGTH_);

  unsigned false_assignment_stat = 0;

  printf("%d\n", nEvents);
  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    for(auto particle: event->ChargedParticles()) {
      TVector3 mid;

      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto history  = particle->GetHistory (rhistory);
	auto radiator = particle->GetRadiator(rhistory);
	const unsigned zdim = radiator->GetTrajectoryBinCount();

	radiator->ResetLocations();
	if (history->StepCount() >= 2) {
	  auto fstep = history->GetStep(0), lstep = history->GetStep(history->StepCount()-1);

	  auto s1 = radiator->GetFrontSide(0);//ifsec);
	  auto s2 = radiator->GetRearSide (0);//ilsec);

	  // FIXME: check what's wrong with 's2';
	  if (s1 && s2) {
	    TVector3 from, to;//, p0;
	    
	    auto x0 = fstep->GetPosition();
	    auto n0 = fstep->GetDirection(); 
	    // Go backwards and ignore surface orientation mismatch;
	    bool b1 = s1->GetCrossing(x0, -1*n0, &from, false);
	    
	    auto x1 = lstep->GetPosition();
	    auto n1 = lstep->GetDirection();
	    bool b2 = s2->GetCrossing(x1,    n1, &to);
	    
	    assert(b1 && b2);
	    
	    //auto fstep = history->GetStep(0), lstep = history->GetStep(stCount-1);
	    //auto from = history->GetStep(0), to = history->GetStep(history->StepCount()-1);
	    TVector3 p0 = 0.5*(fstep->GetMomentum() + lstep->GetMomentum());
	    
	    const unsigned zbins = radiator->GetTrajectoryBinCount();
	    TVector3 nn = (to - from).Unit();
	    // FIXME: hardcoded;
	    from += (0.010)*nn;
	    to   -= (0.010)*nn;

	    if (radiator == aerogel) mid = 0.5*(from + to);
	    
	    // FIXME: assume a straight line to the moment;
	    auto span = to - from;
	    double tlen = span.Mag();
	    double step = tlen / zbins;
	    for(unsigned iq=0; iq<zbins+1; iq++) {
	      radiator->AddLocation(from + iq*step*nn, p0);
	      //printf("(2) %f %f %f\n", 
	      //	   (from + iq*step*nn).x(),
	      //	   (from + iq*step*nn).y(),
	      //	   (from + iq*step*nn).z());
	    } //for iq
	  } //if
	  //TVector3 ptnx = (to->GetPosition() - from->GetPosition()).Unit();
	  
	  //+auto vstart = from->GetPosition() + 0.001*ptnx, vend = to->GetPosition() - 0.001*ptnx;
	  //auto vstart = from->GetPosition() + 0.001*ptnx, vend = vstart + 30.0*ptnx;
	  //double vlen = (vend - vstart).Mag(), step = vlen/zdim;
	  //if (radiator == aerogel) printf("%f %f -> %f\n", from->GetPosition().Z(), to->GetPosition().Z(), vlen);
	  
	  //for(unsigned iq=0; iq<zdim+1; iq++) 
	  //radiator->AddLocation(vstart + iq*step*ptnx, p);
	} //if
      } //for rhistory

      // Now that all internal track-level structures are populated, run IRT code;
      CherenkovPID pid; 

      // Consider just pi/K case for now;
      pid.AddMassHypothesis(0.140);
      pid.AddMassHypothesis(0.494);
      
      for(auto rhistory: particle->GetRadiatorHistory()) 
	for(auto photon: particle->GetHistory(rhistory)->Photons()) {
	  TVector3 phx = photon->GetDetectionPosition();

#ifdef _GAUSSIAN_SMEARING_
	  phx += TVector3(gRandom->Gaus(0.0, _GAUSSIAN_SMEARING_), gRandom->Gaus(0.0, _GAUSSIAN_SMEARING_), 0.0);
#endif
#ifdef _DISCRETE_PIXEL_PITCH_
	  double pitch = _DISCRETE_PIXEL_PITCH_;
	  phx = TVector3(pitch*rint(phx.X()/pitch), pitch*rint(phx.Y()/pitch), phx.Z());
#endif
	  photon->SetDetectionPosition(phx);
	} //for rhistory..photon
      
      // FIXME: use true photon 3D direction at birth; otherwise conical mirror case 
      // is problematic; do it better later;
      particle->PIDReconstruction(pid, true);
      {
	auto pion = pid.GetHypothesis(0), kaon = pid.GetHypothesis(1);
	double wt0 = pion->GetWeight(aerogel), wt1 = kaon->GetWeight(aerogel);
	
	printf("%4d -> %10.3f (%10.3f) vs %10.3f (%10.3f) ...  %3d %d\n", 
	       ev, wt0, pion->GetNpe(aerogel), wt1, kaon->GetNpe(aerogel), 
	       particle->GetPDG(), wt0 > wt1);
	np->Fill(pion->GetNpe(aerogel));
	
	if (wt0 <= wt1) false_assignment_stat++;
      }

      {
	unsigned npe = 0;
	double dtheta = 0.0;
	//double ridx = 0.0;
	double alsum = 0.0;
	//double wlen = 0.0;
	double wtsum = 0.0;
	double dth = aerogel->GetSmearing();

	for(auto rhistory: particle->GetRadiatorHistory()) {
	  auto radiator = particle->GetRadiator(rhistory);
	  if (radiator != aerogel) continue;

	  auto history  = particle->GetHistory (rhistory);

	  // This loop goes only over the photons which belong to a given radiator;
	  for(auto photon: history->Photons()) {
	    bool selected = false;
	    // Check whether this photon was selected by at least one mass hypothesis;
	    for(auto entry: photon->_m_Selected)
	      if (entry.second == aerogel) {
		selected = true;
		break;
	      } //if

	    if (selected) {
	      npe++;
	      double wt = 1.0;//photon->_m_PDF[aerogel].GetWeight();//1.0;//fabs(sin(photon->m_Phi[radiator]));
	      wtsum += wt;
	      {
		double m = _MASS_, pp = photon->GetVertexParentMomentum().Mag();
		double thp = acos(sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp));
		dtheta += wt*photon->_m_PDF[aerogel].GetAverage(thp - dth, thp + dth) - thp;
		for(auto member: photon->_m_PDF[aerogel].GetMembers())
		  qh->Fill(1000*(member->GetAverage() - thp), member->GetWeight());

		double len = (photon->GetDetectionPosition() - mid).Mag();
		double beta = 1/sqrt(1.0 + pow(m/pp, 2)), lspeed = 299.792458, velocity = beta*lspeed;
		double te = len / velocity;
		//printf("%f\n", photon->GetDetectionTime() - te);
		tt->Fill(photon->GetDetectionTime() - te - 4.25, 1000*(photon->_m_PDF[aerogel].GetAverage() - thp));
	      }
	      //printf("%f\n", photon->GetVertexRefractiveIndex());//ridx);
	      //ridx  +=    photon->GetVertexRefractiveIndex();
	      ri->Fill(photon->GetVertexRefractiveIndex() - 1.0);
	      // FIXME: hardcoded;
	      //wlen  +=    1239.8/(photon->GetVertexMomentum().Mag());
	      wl->Fill(1239.8/(photon->GetVertexMomentum().Mag()));

	      //printf("%f\n", photon->GetVertexPosition().Z());
	      z0->Fill(photon->GetVertexPosition().Z() - 1185.5);
	      //if (1239.8/(photon->GetVertexMomentum().Mag()) > 400)
	      alsum += photon->GetVertexAttenuationLength();
	    } //if
	  } //for photon
	} //for rhistory

	nq->Fill(npe);
	if (wtsum) th->Fill(1000.*(dtheta / wtsum));
	if (npe) {
	  //ri->Fill((ridx  / npe) - 1.0);
	  //wl->Fill( wlen  / npe);
	  al->Fill( alsum / npe);
	} //if
      }
    } //for particle
  } //for ev

  printf("%d false out of %lld\n", false_assignment_stat, t->GetEntries());

  auto cv = new TCanvas("cv", "", 1700, 800);
  cv->Divide(4, 2);
  cv->cd(1); np->Draw();
  cv->cd(2); nq->Draw();
  z0->GetXaxis()->SetTitle("Local coordinate in the aerogel block, [mm]");
  z0->GetYaxis()->SetTitle("Detected photon count");
  cv->cd(3); z0->Draw();//wl->Draw();
  cv->cd(4); th->Fit("gaus");
  cv->cd(5); ri->Draw();
  cv->cd(6); wl->Draw();//tt->Draw("COLZ");
  cv->cd(7); qh->Fit("gaus");//Draw();
  cv->cd(8); al->Draw();
} // e_eval()
