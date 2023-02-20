
// Kind of digitization; may want to comment out (or set very fine) for debugging;
#define _HRPPD_ACTIVE_AREA_PIXELLATION_      (32)
// Active area must be in sync with fixed.h;
#define _HRPPD_PIXEL_PITCH_ (108.0/_HRPPD_ACTIVE_AREA_PIXELLATION_)

// Timing digitization in [ns];
#define _SINGLE_PHOTON_TIMING_RESOLUTION_ (0.050)

// [rad] & [mm]; smearing portion, excluding emission point uncertainty 
// (so to first order only dn/dl and sensor plane resolution);
#define _THETA_GAUSSIAN_SMEARING_         (0.0044)
#define _AVERAGE_ATTENUATION_LENGTH_        (20.0)//8.4)

// [mm]; FIXME: should use fixed.h value;
#define _VESSEL_FRONT_SIDE_OFFSET_       (1185.5)

// If uncommented, keep at most one track owner for each hit; if commented out, delete 
// hits, which would fit more than one track for a given combination of hypotheses;
#define _RESOLVE_HIT_OWNERSHIP_

// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_             (0.003)

// Comment out if do not want to deal with the Poissonic terms in chi^2; FIXME: account 
// for yield drop at smaller momentum and yield gain when primary particle crosses aerogel 
// at a large angle;
//#define _EXPECTED_TRUE_PHOTON_COUNT_       (10.5)
// Especially this one is problematic since it should account for the size of radial bands;
//#define _EXPECTED_BG_PHOTON_COUNT_         ( 0.5)

// Helps a bit in case of reflections;
#define _USE_TIMING_IN_CHI_SQUARE_

void multi_eval(const char *dfname, const char *cfname = 0)
{
  auto fcfg  = new TFile(cfname ? cfname : dfname);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("t")); 
  auto event = new CherenkovEvent();
  t->SetBranchAddress("e", &event);

  auto hthph   = new TH1D("thph",   "Cherenkov angle (SPE)",          100,  -30,     30);
  auto hthtr   = new TH1D("thtr",   "Cherenkov angle (track)",        100,  -30,     30);
  auto hccdfph = new TH1D("ccdfph", "Single photon angle chi^2 CCDF",  50,    0,      1);
  auto hccdftr = new TH1D("ccdftr", "Track-level chi^2 CCDF",          50,    0,      1);
  auto hccdfev = new TH1D("ccdfev", "Event-level chi^2 CCDF",          50,    0,      1);
  auto hnpetr  = new TH1D("npetr",  "Photon count per track",          50,    0,     50);
  auto hdtph   = new TH1D("dtph",   "Cherenkov timing (SPE)",         100, -500,    500);
  auto hmatch  = new TH1D("match",  "PID evaluation correctness",       2,    0,      2);

  int nEvents = t->GetEntries();

  auto rich = geometry->GetDetector("pfRICH");
  // FIXME: need to fix this;
  auto aerogel = rich->GetRadiator("Aerogel225");
  if (!aerogel) aerogel = rich->GetRadiator("Aerogel155");

  TRandom rndm;
  auto *DatabasePDG = new TDatabasePDG();

  // First calculate average refractive index for all detected photons for every radiator;
  {
    for(unsigned ev=0; ev<nEvents; ev++) {
      t->GetEntry(ev);
      
      for(auto particle: event->ChargedParticles()) {
	//if (particle->GetPDG() != _PDG_) continue;

	for(auto rhistory: particle->GetRadiatorHistory()) {
	  auto radiator = particle->GetRadiator(rhistory);
	  auto history  = particle->GetHistory (rhistory);
	    
	  for(auto photon: history->Photons()) {
	    if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;
	      
	    radiator->m_Stat++;
	    radiator->m_AverageRefractiveIndex += photon->GetVertexRefractiveIndex();
	  } //for photon
	} //for radiator
      } //for particle
    } //for ev

    for(auto rptr: rich->Radiators()) {
      auto radiator = rptr.second;

      if (radiator->m_Stat) radiator->m_AverageRefractiveIndex /= radiator->m_Stat;
    } //for dt..radiator
  }

  aerogel->SetTrajectoryBinCount(10);
  aerogel->SetGaussianSmearing(_THETA_GAUSSIAN_SMEARING_);
  aerogel->SetReferenceAttenuationLength(_AVERAGE_ATTENUATION_LENGTH_);

  for(unsigned ev=0; ev<nEvents; ev++) {
    t->GetEntry(ev);

    std::vector<DigitizedHit> hits;

    // First mix all photons from all particles and "digitize" them; FIXME: add stray background;
    for(auto particle: event->ChargedParticles())
      for(auto rhistory: particle->GetRadiatorHistory()) 
	for(auto photon: particle->GetHistory(rhistory)->Photons()) {
	  if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;

	  auto pd = photon->GetPhotonDetector();

	  {
	    DigitizedHit hit, *duplicate = 0;

	    hit.m_PhotonDetector = pd;
	    hit.m_Copy = photon->GetVolumeCopy();

	    hit.m_IRTs = pd->GetIRTs(photon->GetVolumeCopy());
	    if (!hit.m_IRTs) {
	      printf("No photosensor with this cellID found!\n");
	      continue;
	    } //if

	    TVector3 phx = photon->GetDetectionPosition();
#ifdef _HRPPD_PIXEL_PITCH_
	    // All IRTs are terminated at the photosensor; use the first one; extract 3D parameterization;
	    auto sensor = dynamic_cast<const FlatSurface*>((*hit.m_IRTs)[0]->tail()->GetSurface());
	    double half = _HRPPD_PIXEL_PITCH_*_HRPPD_ACTIVE_AREA_PIXELLATION_/2;
	    double lx = sensor->GetLocalX(phx) + half, ly = sensor->GetLocalY(phx) + half;

	    // Smear the coordinate measurement (call it digitization :-); 
	    double pitch = _HRPPD_PIXEL_PITCH_, x0 = -half, y0 = -half;
	    hit.m_iX = (int)floor(lx/pitch);
	    hit.m_iY = (int)floor(ly/pitch);
	    //printf("%3d %3d\n", hit.m_iX, hit.m_iY);
	    double dx = pitch*(hit.m_iX + 0.5), dy = pitch*(hit.m_iY + 0.5);
	    phx = sensor->GetSpacePoint(x0 + dx, y0 + dy);

	    for(auto &ptr: hits)
	      if (ptr.m_PhotonDetector == pd && ptr.m_Copy == hit.m_Copy && 
		  ptr.m_iX == hit.m_iX && ptr.m_iY == hit.m_iY) {
		duplicate = &ptr;
		break;
	      } //for ptr .. if
#endif
	    (duplicate ? duplicate : &hit)->m_DirectionSeeds.push_back(photon->GetVertexMomentum().Unit());
	    // Convert to [ps];
	    double timing = rndm.Gaus(photon->GetDetectionTime(), _SINGLE_PHOTON_TIMING_RESOLUTION_);
	    (duplicate ? duplicate : &hit)->m_DetectionTimes.push_back(timing);

	    if (!duplicate) {
	      // For duplicate hits this setting will be identical;
	      hit.m_DetectionPosition = phx;

	      hits.push_back(hit);
	    } //if
	  }
	} //for particle..photon
    
    //
    //   There are no optical photons beyond this point; their information was merged into 
    // the DigitizedHit array; the only cheating left are direction seeds, but this is rather a
    // procedural issue than using MC truth information illegally; and t0 resolution is supposed to 
    // be ideal (but it will anyway be much better than anticipated HRPPD single photon TTS);
    //

    unsigned const hdim = 2;
    //TParticlePDG *hypotheses[hdim] = {DatabasePDG->GetParticle(211)};
    TParticlePDG *hypotheses[hdim] = {DatabasePDG->GetParticle(211), DatabasePDG->GetParticle(321)};

    unsigned cdim = 1, pdim = event->ChargedParticles().size(), icbest = 0;
    // FIXME: come on!;
    for(unsigned iq=0; iq<event->ChargedParticles().size(); iq++)
      cdim *= hdim;
    // Save intermediate chi^2 estimates;
    double  ccdfevsave[cdim];       memset(ccdfevsave, 0x00, sizeof(ccdfevsave));
    double  ccdftrsave[cdim][pdim]; memset(ccdftrsave, 0x00, sizeof(ccdftrsave));
    unsigned npetrsave[cdim][pdim]; memset(npetrsave,  0x00, sizeof(npetrsave));

    // Now loop through all [particle,hypothesis] combinations and evaluate their chi^2;
    for(unsigned ic=0; ic<cdim; ic++) {
      // A running variable, to simplify hypothesis calculation;
      unsigned id = ic;

      // Clear hit attachment table;
      for(auto &hit: hits) {
	hit.m_BackgroundCandidate = false;
	hit.m_Selected.clear();
      } //for hit      

      for(auto mcparticle: event->ChargedParticles()) {
	unsigned hypo = id%hdim;
	auto hyparticle = hypotheses[hypo];

	std::map<CherenkovRadiator*, double> momenta;

	// Prepare radiators (calculate nodes along the trajectory); FIXME: very inefficient, 
	// need to do this only once;
	for(auto rhistory: mcparticle->GetRadiatorHistory()) {
	  auto history  = mcparticle->GetHistory (rhistory);
	  auto radiator = mcparticle->GetRadiator(rhistory);
	  const unsigned zdim = radiator->GetTrajectoryBinCount();
	  
	  radiator->ResetLocations();
	  radiator->ResetTimes();
	  if (history->StepCount() >= 2) {
	    auto fstep = history->GetStep(0), lstep = history->GetStep(history->StepCount()-1);
	    
	    auto s1 = radiator->GetFrontSide(0);//ifsec);
	    auto s2 = radiator->GetRearSide (0);//ilsec);
	    
	    // FIXME: check what's wrong with 's2';
	    if (s1 && s2) {
	      TVector3 from, to;
	      
	      auto x0 = fstep->GetPosition();
	      auto n0 = fstep->GetDirection(); 
	      // Go backwards and ignore surface orientation mismatch;
	      bool b1 = s1->GetCrossing(x0, -1*n0, &from, false);
	      
	      auto x1 = lstep->GetPosition();
	      auto n1 = lstep->GetDirection();
	      bool b2 = s2->GetCrossing(x1,    n1, &to);
	      
	      assert(b1 && b2);
	      
	      TVector3 p0 = 0.5*(fstep->GetMomentum() + lstep->GetMomentum());
	      momenta[radiator] = p0.Mag();
	      double t0 = fstep->GetTime();
	      
	      const unsigned zbins = radiator->GetTrajectoryBinCount();
	      TVector3 nn = (to - from).Unit();
	      // FIXME: hardcoded;
	      from += (0.010)*nn;
	      to   -= (0.010)*nn;
	      	      
	      // FIXME: assume a straight line to the moment;
	      auto span = to - from;
	      double tlen = span.Mag();
	      double step = tlen / zbins;
	      for(unsigned iq=0; iq<zbins+1; iq++) {
		radiator->AddLocation(from + iq*step*nn, p0);
		// FIXME: ignore velocity; also assume speed of light is 300 mm/ns;
		radiator->AddTime(t0 + iq*step/300.0);
	      } //for iq
	    } //if
	  } //if
	} //for rhistory
	
	// FIXME: 'true': use 3D direction seeds (MC truth); otherwise conical mirror case 
	// is problematic; do it better later;
	mcparticle->ProcessHits(hits, true);

	//
	// By this moment every detected hit is evaluated with respect to this particle 
	// trajectory as originated from every radiator via every possible optical path; 
	//

	// Loop through all hits; 
	for(auto &hit: hits) {
	  // Will never have means to know from which radiator 
	  // a given photon originated, and via which optical path it arrived to the sensor;
	  // just choose "the best" [radiator,irt] combination, which this particular 
	  // particle PID hypothesis can find; the criterion is "closest in theta and closest 
	  // in timing" as evaluated via a chi^2 estimate with ndf=2;
	  VectorPDF *best = 0;
	  double thp0 = 0.0, dt = 0.0, tsigma = _SINGLE_PHOTON_TIMING_RESOLUTION_;
	  double ccdfmax = 0.0;

	  double hsigma = aerogel->GetSmearing();

	  for(auto &tag: hit.m_PDF) {
	    auto radiator = tag.first.first;
	    auto &pdf = tag.second;

	    // FIXME: handle this correctly;
	    if (radiator != aerogel) continue;

	    double pp = momenta[radiator];

	    for(unsigned ih=0; ih<hdim; ih++) {
	      double m = hypotheses[ih]->Mass();
	      
	      // FIXME: exception;
	      double thp = acos(sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp));
	      double thdiff = pdf.GetAverage() - thp;
	      double tmdiff = hit.m_ExpectedTime - hit.GetAverageDetectionTime();
	      
	      unsigned ndf = 1;
	      double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
#ifdef _USE_TIMING_IN_CHI_SQUARE_
	      ndf++;
	      chi2 += pow(tmdiff, 2)/pow(tsigma, 2);
#endif
	      double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);

	      if (ih == hypo) {
		if (!best || ccdf > ccdfmax) {
		  best = &pdf;
		  // FIXME: do it better later;
		  thp0 = thp;
		  dt = tmdiff;
		  ccdfmax = ccdf;
		} //if
	      } else {
		// The logic behind this is that all hits within +/-N*sigma window
		// with respect to at least one hypothesis in at least one radiator
		// for at least one optical path for every particle belong to the 
		// club "which needs to be explained" in terms of Poisson statistics;
		if (ccdf > _SINGLE_HIT_CCDF_CUT_) hit.m_BackgroundCandidate = true;
	      } //if
	    } //for ih
	  } //for tag 

	  if (best) {
	    unsigned ndf = 1;
	    double thdiff = best->GetAverage() - thp0;
	    double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
#ifdef _USE_TIMING_IN_CHI_SQUARE_
	    ndf++;
	    chi2 += pow(dt, 2)/pow(tsigma, 2);
#endif
	    double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);

	    // FIXME: should be radiator-specific;
	    if (ccdf > _SINGLE_HIT_CCDF_CUT_) {
	      hit.m_Selected.insert(std::make_pair(mcparticle, chi2));
	     
	      // Sure of interest is only to see a distribution for the right hypothesis; 
	      if (mcparticle->GetPDG() == hyparticle->PdgCode()) {
		hthph->Fill(1000*thdiff);
		hdtph->Fill(1000*dt);

		hccdfph->Fill(ccdf);
	      } //if
	    } //if
	  } //if
	} //for hits

	id /= event->ChargedParticles().size();

	// FIXME: memory leak; looks like nothing else needs to be cleaned up here?;
	for(auto &hit: hits) 
	  hit.m_PDF.clear();
      } //mcparticle

      // Clean up background candidate list; obviously if at least one particle selected this 
      // hit for a hypothesis considered in this iteration, the hit is useful;
      for(auto &hit: hits) 
	if (hit.m_Selected.size())
	  hit.m_BackgroundCandidate = false;
      
      // Now either resolve ambiguous assignments, based on the hit-to-track chi^2 match for this 
      // PID hypothesis, or eliminate these hits everywhere; FIXME: any better idea?;
      {
#ifdef _RESOLVE_HIT_OWNERSHIP_ 
	for(auto &hit: hits) 
	  if (hit.m_Selected.size() >= 1) {
	    double chi2min = 0.0;
	    // FIXME: perhaps order them at the creation time?;
	    for(auto it = hit.m_Selected.begin(); it != hit.m_Selected.end(); it++)
	      if (it == hit.m_Selected.begin() || it->second < chi2min)
		chi2min = it->second;
	    for(auto it = hit.m_Selected.begin(); it != hit.m_Selected.end(); )
	      if (it->second != chi2min)
		it = hit.m_Selected.erase(it);
	      else
		it++;

	    //printf("%7.1f %7.1f\n", hit.m_Selected.begin()->second, hit.m_Selected.rbegin()->second);
	  } //for .. if
#else
	for(auto &hit: hits) 
	  if (hit.m_Selected.size() != 1)
	    hit.m_Selected.clear();
#endif
      }

      // And eventually calculate the overall event chi^2;
      {
	unsigned nbg = 0;
	for(auto &hit: hits) 
	  if (hit.m_BackgroundCandidate) nbg++;

	unsigned ndfev = 0, idx = 0;
	double chi2ev = 0;
	for(auto mcparticle: event->ChargedParticles()) {
	  unsigned npe = 0;
	  double chi2tr = 0.0;
	   
	  for(auto &hit: hits) 
	    for(auto selected: hit.m_Selected)
	      if (selected.first == mcparticle) {
		npe++;
		double chi2ph = selected.second;
		chi2tr += chi2ph;

		break;
	      } //for..if

#ifdef _USE_TIMING_IN_CHI_SQUARE_
	  unsigned ndftr = 2*npe;
#else
	  unsigned ndftr = 1*npe;
#endif
#ifdef _EXPECTED_TRUE_PHOTON_COUNT_
	  double texp = _EXPECTED_TRUE_PHOTON_COUNT_;
	  chi2tr += npe ? 2*(texp - npe + npe*log(npe/texp)) : 0.0;
	  ndftr++;
#endif
	  ccdftrsave[ic][idx] = ROOT::Math::chisquared_cdf_c(chi2tr, ndftr);
	  npetrsave [ic][idx] = npe;
	  
	  ndfev  += ndftr;
	  chi2ev += chi2tr;
	  // This is dumb of course, but do not want to change std::set to std::vector;
	  idx++;
	} //for mcparticle

#ifdef _EXPECTED_BG_PHOTON_COUNT_
	if (hdim >= 2) {
	  ndfev++;
	  double bexp = _EXPECTED_BG_PHOTON_COUNT_;
	  chi2ev += nbg ? 2*(bexp - nbg + nbg*log(nbg/bexp)) : 0.0;
	} //if
#endif
	double ccdfev = ROOT::Math::chisquared_cdf_c(chi2ev, ndfev);
	ccdfevsave[ic] = ccdfev;

	if (!ic || ccdfev > ccdfevsave[icbest]) icbest = ic;
      }
    } //for ic

    // Now check how the PID procedure performed;
    {
      // Sure of interest is only to see a flat CCDF distribution for the right combination 
      // of event-level hypotheses; 
      hccdfev->Fill(ccdfevsave[icbest]);

      for(unsigned ip=0; ip<event->ChargedParticles().size(); ip++) {
	hccdftr->Fill(ccdftrsave[icbest][ip]);
	hnpetr ->Fill(npetrsave [icbest][ip]);
      } //for ip

      {
	// A running variable, to simplify hypothesis calculation;
	unsigned id = icbest;
	
	for(auto mcparticle: event->ChargedParticles()) {
	  unsigned hypo = id%hdim;
	  auto rcparticle = hypotheses[hypo];
	
	  if (mcparticle->GetPDG() == rcparticle->PdgCode()) {
	    hmatch->Fill(0.5);
	  } else {
	    hmatch->Fill(1.5);
	    printf("Failure!\n");
	  } //if	  

	  id /= event->ChargedParticles().size();
	} //for ip
      }
    }

    if (!(ev%100)) printf("Event %5d ...\n", ev);
  } //for ev

  auto cv = new TCanvas("cv", "", 1600, 800);
  cv->Divide(4, 2);
  cv->cd(1); hthph->Fit("gaus");
  cv->cd(2); hccdfph->SetMinimum(0); hccdfph->Draw();
  cv->cd(3); hccdftr->SetMinimum(0); hccdftr->Draw();
  cv->cd(4); hccdfev->SetMinimum(0); hccdfev->Draw();
  cv->cd(5); hnpetr->Draw();
  cv->cd(6); hmatch->SetMinimum(0); hmatch->Draw();
  cv->cd(7); hdtph->Fit("gaus");
} // multi_eval()
