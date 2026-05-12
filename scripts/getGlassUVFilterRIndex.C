#include <fstream>
#include <iostream>

void SetGraph(TGraph* g, int s, int c);
void SetPad();

//---------------------------------------------------------------------------//
void getGlassUVFilterRIndex()
{
  fstream infile("database/RefractiveIndex.txt");
  if (!infile.is_open()) {
    std::cerr << "Error: cannot open file database/RefractiveIndex.txt"<< std::endl;
    return;
  }

  string junk;
  double lambda, r;

  vector<double> vecLambda, vecEnergy, vecRIndex;

  for (int i=0;i<10;i++) {
    infile>>junk;
    //cout<<junk<<" ";
  }
  //cout<<endl;

  while (infile>>lambda>>junk>>junk>>r>>junk>>junk>>junk>>junk>>junk) {
    //cout<<"lambda="<<lambda<<endl;
    if (lambda<200 || lambda>800) continue;
    
    vecLambda.push_back(lambda);
    vecEnergy.push_back(1239.84193 / lambda);
    vecRIndex.push_back(r);
  }
  cout<<"size: "<<vecLambda.size()<<endl;
  
  TGraph* grLamdba=new TGraph((int) vecLambda.size(),vecLambda.data(),vecRIndex.data());
  TGraph* grEnergy=new TGraph((int) vecEnergy.size(),vecEnergy.data(),vecRIndex.data());

  SetGraph(grLamdba,24, 1);
  SetGraph(grEnergy,24, 1);

  grLamdba->SetTitle(";#lambda (nm);Refractive index");
  grEnergy->SetTitle(";E_{#gamma} (eV);Refractive index");
  
  TCanvas* c=new TCanvas("c","",1200,500);
  c->Range(0,0,1,1);
  c->Divide(2);

  c->cd(1);
  
  SetPad();
  grLamdba->Draw("AP");

  c->cd(2);
  SetPad();
  grEnergy->Draw("AP");

  c->SaveAs("glassUVfilterRIndex.pdf");

  //----------------------------
  cout<<"\nEboroRindex["<<vecEnergy.size()<<"]={";
  for (int i=(int) vecEnergy.size()-1 ; i>=0 ; i--) {
    cout<<Form("%.4f",vecEnergy.at(i));

    if (i==0) cout<<"}"<<endl;
    else if ((i!=vecEnergy.size()-1) && (vecEnergy.size()-i)%10==0) cout<<","<<endl;
    else cout<<", ";
  }

  cout<<"\nRIndexBoro["<<vecEnergy.size()<<"]={";
  for (int i=(int) vecEnergy.size()-1 ; i>=0 ; i--) {
    cout<<Form("%.4f",vecRIndex.at(i));

    if (i==0) cout<<"}"<<endl;
    else if ((i!=vecEnergy.size()-1) && (vecEnergy.size()-i)%10==0) cout<<","<<endl;
    else cout<<", ";
  }
  
 
}
//---------------------------------------------------------------------------//
void SetGraph(TGraph* g, int s, int c)
{
  g->SetMarkerStyle(s);
  g->SetMarkerColor(c);

  g->GetXaxis()->SetNdivisions(505);
  g->GetXaxis()->SetTitleFont(43);
  g->GetXaxis()->SetTitleSize(30);
  g->GetXaxis()->SetLabelFont(43);
  g->GetXaxis()->SetLabelSize(30);

  g->GetYaxis()->SetNdivisions(505);
  g->GetYaxis()->SetTitleFont(43);
  g->GetYaxis()->SetTitleSize(30);
  g->GetYaxis()->SetLabelFont(43);
  g->GetYaxis()->SetLabelSize(30);
}
//---------------------------------------------------------------------------//
void SetPad()
{
  gPad->SetTopMargin(0.01);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.18);
  gPad->SetRightMargin(0.01);
  gPad->SetGrid();
}
