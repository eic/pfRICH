//#include <TText.h>

void cerenkov( void )
{
  gROOT->Reset();  

  gStyle->SetTextSize(0.02);
  gStyle->SetLabelSize(0.04,"xy");
  gStyle->SetFrameFillColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  //gStyle->SetStatBorderSize(0);
  //gStyle->SetStatColor(kWhite);
  //gStyle->SetStatFontSize(0.03);
  //gStyle->SetStatFont(52);
  //gStyle->SetStatW(.13);
  //gStyle->SetFitFormat("5.2f");

  TCanvas *c1 = new TCanvas("c1", "c1", 0, 0, 800, 500);
c1->UseCurrentStyle();
c1->SetBorderMode(0);
c1->SetFrameBorderMode(0);
c1->SetFrameLineColor(kWhite);
c1->SetFillColor(kWhite);

#define _TSIZE_ 0.05
#define _LSIZE_ 0.04

#define _PMAX_ (15.0)

TH1D *hdum  = new TH1D("hdum",   "", 50, 0.0, _PMAX_);

hdum->GetXaxis()->SetLabelFont(52);
hdum->GetYaxis()->SetLabelFont(52);
hdum->GetXaxis()->SetTitleFont(52);
hdum->GetYaxis()->SetTitleFont(52);

hdum->GetXaxis()->SetTitle("Momentum, [GeV/c]");
hdum->GetXaxis()->SetTitleSize(_TSIZE_);
hdum->GetXaxis()->SetLabelSize(_LSIZE_);
hdum->GetYaxis()->SetTitle("Cerenkov angle, [mrad]");
hdum->GetYaxis()->SetTitleSize(_TSIZE_);
hdum->GetYaxis()->SetLabelSize(_LSIZE_);
hdum->GetXaxis()->SetTitleOffset(0.90);
hdum->GetYaxis()->SetTitleOffset(0.80);
hdum->SetMinimum(   0.0);
hdum->SetMaximum( 400.0);

//hdum->GetXaxis()->SetNdivisions(408);
//hdum->GetYaxis()->SetNdivisions(804);

gPad->SetGrid();
gPad->SetBorderMode(0);
gPad->SetBorderSize(0);
gPad->SetFrameBorderMode(0);
gPad->SetFrameLineColor(kWhite);

 hdum->Draw();

#define _GNUM_          2
#define _CF4_     (1.00055)
#define _C2F6_    (1.00083) // assume this is close to the truth;
#define _C4F10_   (1.00150)
#define _AG_1_02_ (1.02000)
#define _AG_1_05_ (1.04500)
#define _C6F14_   (1.275)
#define _QQ_      (1.01000)

#define _PNUM_          3
#define _MPI_      (0.140)
#define _MK_       (0.494)
#define _MP_       (0.938)

 double masses[_PNUM_] = {_MPI_, _MK_, _MP_}, gases[_GNUM_] = {_AG_1_02_, _AG_1_05_};//_C4F10_};//_C2F6_};
 unsigned colors[_PNUM_] = {kRed, kGreen, kBlue};

 for(unsigned ig=0; ig<_GNUM_; ig++) {
   double gas = gases[ig];

   for(unsigned ip=0; ip<_PNUM_; ip++) {
     double mass = masses[ip];

     double pmin = mass/sqrt(gas*gas-1.0); //if (ip == 1) printf("%f\n", pmin);
     char name[128], buffer[512];
     sprintf(name, "g%dp%d", ig, ip);
     sprintf(buffer, "1000.*acos(sqrt(x*x+%7.3f*%7.3f)/(%7.5f*x))", mass, mass, gas);
     TF1 *hfun = new TF1(name, buffer, pmin, _PMAX_);
     hfun->SetNpx(100000);
     hfun->SetLineWidth(2);
     hfun->SetLineColor(colors[ip]);
     if (ig) hfun->SetLineStyle(7);
     hfun->Draw("SAME");
   } //for ip
 } //for ig

#if 0
 //TText *text = new TText(0.8, 225., "pions");
 //TText *text = new TText(0.4, 500., "pions");
 //TText *text = new TText(5.0, 40., "pions");
 {
   //TText *text = new TText(11.0, 27., "pions");
   TText *text = new TText(0.85, 0.4, "pions");
   text->SetNDC(true);
   text->SetTextColor(kRed);
   text->SetTextFont(52);
   text->SetTextSize(0.05);
   text->Draw();
 }
 {
   //TText *text = new TText(2.1, 175., "kaons");
   //TText *text = new TText(1.0, 450., "kaons");
   //TText *text = new TText(13.0, 35.0, "kaons");
   TText *text = new TText(0.85, 0.3, "kaons");
   text->SetNDC(true);
   text->SetTextColor(kGreen);
   text->SetTextFont(52);
   text->SetTextSize(0.05);
   text->Draw();
 }
 {
   //TText *text = new TText(3.4, 125., "protons");
   //TText *text = new TText(1.7, 400., "protons");
   //TText *text = new TText(22., 30., "protons");
   TText *text = new TText(0.85, 0.2, "protons");
   text->SetNDC(true);
   text->SetTextColor(kBlue);
   text->SetTextFont(52);
   text->SetTextSize(0.05);
   text->Draw();
 }
#endif  
}
