
void FitHisto_Iterative(const char *fname, const char *hname, double peakEx, int nsigma){
  auto f = new TFile(fname);
  auto h = (TH1D *) f->Get(hname);
  if(!f || !h) printf("No File or Histo Found\n");
  float tolerance = 2;
  float startingvalue = h->GetXaxis()->GetBinCenter(1);
  printf("Starting value of bin: %f\n",startingvalue);
  double binwidth= h->GetBinWidth(1);
  double nbins = h->GetNbinsX();
  //Now search for the peak in the histo and test its goodness  	  
  double peakLocation = h->GetMaximumBin();
  if(TMath::Abs((peakLocation*binwidth)+startingvalue - peakEx)>tolerance) {
    printf("Peak found in Unnatural location \n");
    printf("## %f compared to %f\n",(peakLocation*binwidth)+startingvalue, peakEx);
  }
  else{printf("peak found in good region; continuing ...\n");} 

  //Now the Fitting part; well at this level we don't care much of percision; Max bin --> +- half max
  // Get the first bin with diff = (BinContent - DesiredBinContent)<=maxdiff;
  int l=0, u=0;
  double difftol = 100;   // once should tune
  double halfcontent =(h->GetBinContent(h->GetMaximumBin())/2);
  printf("maxconetnt %f %f %f\n", h->GetBinContent(h->GetMaximumBin()),halfcontent, difftol );
  h->GetBinWithContent(halfcontent,l,1,peakLocation,difftol); //100 tolerance 
  h->GetBinWithContent(halfcontent,u,peakLocation,nbins,difftol); //100 tolerance 
  printf("First Estimate of Half Width %f %f\n ",h->GetBinContent(l), h->GetBinContent(u));
  
  //Geting Bins to run first fit
  float lowedge = (l*binwidth)+startingvalue;
  float upperedge = (u*binwidth)+startingvalue;  

  auto f1 = new TF1("f1","gaus",200,300);
  h->Fit(f1,"R","",lowedge,upperedge);
  //auto f1 = h->GetFunction("gaus");

  double mean =0; 
  mean = f1->GetParameter(1); double sigma = 0;
  sigma = f1->GetParameter(2);
  f1->SetParameter(1,mean); 
  f1->SetParameter(2,sigma); 
  
  h->Fit(f1,"R","",mean-2*sigma,mean+2*sigma); 
  mean = f1->GetParameter(1);sigma = f1->GetParameter(2);
  f1->SetParameter(1,mean);
  f1->SetParameter(2,sigma);
  vector<double> SNR;
  vector<double> Sigma;
  auto cc1 = new TCanvas(); cc1->Divide(3,2);
  for(int i=1; i<=nsigma; i++){
    //cc1->cd(i);
    //h->Draw();
    float r1 =  mean - i*sigma;
    float r2 =  mean + i*sigma;
    printf("Fitting From %f %f \n",r1,r2);
    h->Fit(f1,"R","",r1,r2);
    mean = f1->GetParameter(1);sigma = f1->GetParameter(2);
    f1->SetParameter(1,mean);
    f1->SetParameter(2,sigma);

    SNR.push_back(mean/sigma);
    Sigma.push_back(i);
  }
  auto cc = new TCanvas();
  auto G = new TGraph(5,&Sigma[0],&SNR[0]);
  G->Draw("AP*");
}
