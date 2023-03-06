 
#define _DISTANCE_ 1185.5
#define _X0_         0.0
#define _Y0_         0.0
//#define _SIZE_      250.0
//#define _XYDIM_       300
#define _SIZE_     400.0
#define _XYDIM_       300
//#define _SIZE_     1300.0
//#define _XYDIM_       300
#define _BWIDTH_ (_SIZE_/_XYDIM_)

// Help stepping not get stuck; fine with the pfRICH; may not work well for other detectors;
#define _MAX_STEP_COUNT_              50
// In [cm]; suppress fake IP file air;
#define _MAX_POSSIBLE_STEP_LENGTH_  1000
// Suppress vacuum entries;
#define _MAX_ACCOUNTABLE_RAD_LENGTH_ 1E6
// Not really interested in anything beyond this length along the scan line from the IP;
#define _MAX_SCAN_DEPTH_             500

void xray(const char *fname)
{
  TGeoManager::Import(fname);
  
  auto hrlen = new TH2D("hrlen", "pfRICH radiation length scan [%]",  
			_XYDIM_, _X0_ - _SIZE_/2, _X0_ + _SIZE_/2, 
			_XYDIM_, _Y0_ - _SIZE_/2, _Y0_ + _SIZE_/2);
  hrlen->GetXaxis()->SetTitle("pfRICH vessel front side X, [mm]");
  hrlen->GetYaxis()->SetTitle("pfRICH vessel front side Y, [mm]");
  hrlen->GetXaxis()->SetTitleOffset(1.20);
  hrlen->GetYaxis()->SetTitleOffset(1.40);

  double z = -_DISTANCE_;
  for(unsigned i = 0; i < _XYDIM_; i++) {
    double x = _X0_ - _SIZE_/2 + _BWIDTH_*(i + 0.5);

    for(unsigned j = 0; j < _XYDIM_; j++) {
      double y = _Y0_ - _SIZE_/2 + _BWIDTH_*(j + 0.5);
      double r = sqrt(x*x + y*y + z*z);
      double xx[3] = {0.0, 0.0, 0.0}, nn[3] = {x, y, z};
      for(auto &coord: nn)
	coord /= r;
    
      gGeoManager->SetCurrentPoint    (xx);
      gGeoManager->SetCurrentDirection(nn);
      
      unsigned counter = 0;
      double accu = 0.0;//, length = 0.0;
      for(TGeoNode *node = gGeoManager->GetCurrentNode(); ; ) {
	TGeoMaterial *material = node->GetVolume()->GetMaterial();
	double radlen = material->GetRadLen();
	
	auto xx = gGeoManager->GetCurrentPoint();
	//printf("%f %f %f\n", xx[0], xx[1], xx[2]);
	gGeoManager->FindNextBoundary();
	double thickness = gGeoManager->GetStep(), distance = sqrt(xx[0]*xx[0] + xx[1]*xx[1] + xx[2]*xx[2]);;
	//length += thickness;
	if (distance < _MAX_SCAN_DEPTH_ && thickness < _MAX_POSSIBLE_STEP_LENGTH_ && 
	    radlen < _MAX_ACCOUNTABLE_RAD_LENGTH_) {
	  //if (100 * thickness / radlen > 10)
	  //printf("%7.2f [cm] %10.2f [cm] (D = %7.2f [cm]) --> %7.3f%%\n", 
	  //	   thickness, radlen, distance, 100 * thickness / radlen);
	  if (thickness && radlen) accu += 100 * thickness / radlen;
	} //if

	// Switch to next node along {xx, nn[]} 3D line;
	node = gGeoManager->Step();
	assert(gGeoManager->IsEntering());
	
	// Once out of volume, break;
	if (gGeoManager->IsOutside()) break; 

	if (counter++ >= _MAX_STEP_COUNT_) break;
      } //for inf

      hrlen->SetBinContent(i+1, j+1, accu);
    } //for j
  } //for i

  gStyle->SetOptStat(0);
  auto cv = new TCanvas("cv", "", 800, 800);
  hrlen->SetMinimum( 0);
  //hrlen->SetMaximum(47);
  hrlen->Draw("COLZ");

  for(unsigned ir=0; ir<2; ir++) {
    double r = _DISTANCE_*tan(2*atan(exp(ir ? -3.5 : -4.0)));
    TEllipse *el = new TEllipse(0, 0, r, r);
    el->SetFillStyle(kNone);
    el->SetLineColor(ir ? kGreen : kRed);
    el->SetLineStyle(kDashed);
    el->SetLineWidth(2);
    el->Draw("SAME");
  } //for ir
} // xray()
