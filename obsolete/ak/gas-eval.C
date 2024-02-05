

void gas_eval(const char *dfname, const char *cfname = 0)
{
  auto *reco = new ReconstructionFactory(dfname, cfname, "pfRICH");

  // Mark all pads hit by "calibration" (above the QE curve) photons in these 
  // radiators as "useless" for the purposes of "GasVolume" generated photons;
  reco->AddBlackoutRadiator("QuartzWindow");
  //reco->AddBlackoutRadiator("Acrylic");
  //reco->AddBlackoutRadiator("BelleIIAerogel1");

  // Carelessly remove (0x1 << n)x(0x1 << n) square area "around" these hits;
  reco->SetBlackoutBlowupValue(3);

  // Whatever we want it;
  reco->SetSinglePhotonTimingResolution(0.050);
  // Bypass imaging;
  reco->SetExperimentalMode();

  {
    CherenkovEvent *event;

    // Event loop;
    while((event = reco->GetNextEvent())) {
      printf("%3d\n", (int)reco->GetDigitizedHits().size());

      // Loop over digitized hits;
      for(unsigned iq=0; iq<reco->GetDigitizedHits().size(); iq++) {
	auto &hit = reco->GetDigitizedHits()[iq];

	// From this point on see DigitizedHit.h class methods;
      } //for iq
    } //while
  }
} // gas_eval()
