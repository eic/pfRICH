// root agelRIndexCurve.C
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "TH1F.h"
#include "TFile.h"

struct Point {
  double energy;
  double rindex;
};
//----------------------------------------------------------------------------//
std::vector<Point> ReadRIndexFromXML(const std::string& xmlFile,
                                     const std::string& materialName)
{
  std::ifstream in(xmlFile);
  if (!in.is_open()) {
    std::cerr << "Error: cannot open file " << xmlFile << std::endl;
    return {};
  }

  std::string line;
  bool inMaterial = false;
  bool inRIndex   = false;
  std::vector<Point> points;

  std::regex matBeginRe("<Material\\s+name=\"([^\"]+)\">");
  std::regex propBeginRe("<Property\\s+name=\"RINDEX\"[^>]*>");
  std::regex valueRe("<value\\s+energy=\"([^\"]+)\">([^<]+)</value>");

  std::smatch match;

  while (std::getline(in, line)) {
    if (!inMaterial) {
      if (std::regex_search(line, match, matBeginRe)) {
        if (match[1] == materialName) {
          inMaterial = true;
        }
      }
      continue;
    }

    if (line.find("</Material>") != std::string::npos) break;

    if (!inRIndex) {
      if (std::regex_search(line, propBeginRe)) {
        inRIndex = true;
      }
      continue;
    }

    if (line.find("</Property>") != std::string::npos) break;

    if (std::regex_search(line, match, valueRe)) {
      points.push_back({std::stod(match[1]), std::stod(match[2])});
    }
  }

  return points;
}
//----------------------------------------------------------------------------//
TH1F* MakeRIndexHist(const std::vector<Point>& pts,
                     const char* name)
{
  int n = pts.size();
  if (n == 0) return nullptr;

  std::vector<double> edges(n + 1);

  if (n == 1) {
    edges[0] = pts[0].energy - 0.5;
    edges[1] = pts[0].energy + 0.5;
  } else {
    edges[0] = pts[0].energy - 0.5 * (pts[1].energy - pts[0].energy);
    for (int i = 1; i < n; i++) {
      edges[i] = 0.5 * (pts[i - 1].energy + pts[i].energy);
    }
    edges[n] = pts[n - 1].energy + 0.5 * (pts[n - 1].energy - pts[n - 2].energy);
  }

  TH1F* h = new TH1F(name, name, n, edges.data());
  h->GetXaxis()->SetTitle("Energy [eV]");
  h->GetYaxis()->SetTitle("RINDEX");

  for (int i = 0; i < n; i++) {
    h->SetBinContent(i + 1, pts[i].rindex);
    h->SetBinError(i+1,0);
  }

  return h;
}
//----------------------------------------------------------------------------//
void SetHisto(TH1F* h, int i)
{
  h->SetStats(0);
  h->SetMarkerStyle(24+i);
  h->SetMarkerColor(kAzure+3+4*i);
  h->SetLineColor(kAzure+3+4*i);

  h->SetTitle(";E_{#gamma} (eV);n");

  h->GetXaxis()->SetNdivisions(510);
  h->GetXaxis()->SetTitleFont(43);
  h->GetXaxis()->SetTitleSize(24);
  h->GetXaxis()->SetLabelFont(43);
  h->GetXaxis()->SetLabelSize(24);
  
  h->GetYaxis()->SetRangeUser(1.035,1.07);
  h->GetYaxis()->SetNdivisions(510);
  h->GetYaxis()->SetTitleFont(43);
  h->GetYaxis()->SetTitleSize(24);
  h->GetYaxis()->SetLabelFont(43);
  h->GetYaxis()->SetLabelSize(24);
}
//----------------------------------------------------------------------------//
double GetEnergy(double wl)
{
  return 1239.84193 / wl;
}
//----------------------------------------------------------------------------//
double GetScale(TH1F* h,double E,double my_n)
{
  int bin=h->FindBin(E);

  double x1,x2,n1,n2;
  
  if (E<h->GetBinCenter(bin)) {
    x1=h->GetBinCenter(bin-1);
    x2=h->GetBinCenter(bin);
    n1=h->GetBinContent(bin-1);
    n2=h->GetBinContent(bin);
  }
  else if (E>h->GetBinCenter(bin)) {
    x1=h->GetBinCenter(bin);
    x2=h->GetBinCenter(bin+1);
    n1=h->GetBinContent(bin);
    n2=h->GetBinContent(bin+1);
  }
  else {
    cout<<"belle="<<h->GetBinContent(bin)<<", my_n="<<my_n<<endl;
    return my_n/h->GetBinContent(bin);
  }

  double m=(n2-n1)/(x2-x1);
  double b=n1-m*x1;
  double nBelle=m*E+b;

  cout<<"belle="<<nBelle<<", my_n="<<my_n<<endl;
  return my_n/nBelle;
}
//----------------------------------------------------------------------------//
void agelRIndexCurve.C(const char* xmlFile="database/BelleII.xml")
{
  double E=GetEnergy(405);
  double n405[3]={1.0377,1.0404,1.0401};
  string tag[3]={"TSA114-3","TSA120-1","TSA120-2"};
  double scale[3];
  TH1F* hn[3];
  int i;

  TGraph* gr[3];
  for (i=0;i<3;i++) {
    gr[i]=new TGraph(1);
    gr[i]->SetPoint(0,E, n405[i]);
    gr[i]->SetMarkerStyle(24+i);
    gr[i]->SetMarkerColor(kPink-3+4*i);
    gr[i]->SetLineColor(kPink-3+4*i);
  }
  
  TH1F* hBelle[2];
  for (i=0;i<2;i++) {
    auto a = ReadRIndexFromXML(xmlFile, Form("BelleIIAerogel%d",i+1));
    if (a.empty()) {// || a2.empty()) {
      std::cerr << "Error: missing RINDEX data" << std::endl;
      return;
    }

    hBelle[i] = MakeRIndexHist(a, Form("hRIndex_Aerogel%d",i+1));
    SetHisto(hBelle[i],i);
  }

  
  for (i=0;i<3;i++) {
    scale[i]=GetScale(hBelle[0],E,n405[i]);
    //cout<<"scale = "<<scale[0]<<", "<<scale[1]<<", "<<scale[2]<<endl;
    hn[i]=(TH1F*) hBelle[0]->Clone(Form("hn%d",i));
    hn[i]->Scale(scale[i]);
    SetHisto(hn[i],i);
    hn[i]->SetMarkerStyle(24+i);
    hn[i]->SetMarkerColor(kPink-3+4*i);
    hn[i]->SetLineColor(kPink-3+4*i);
  }

  //---------------------------
  // print refractive indexes
  //---------------------------
  cout<<"eRINDX["<<hBelle[0]->GetNbinsX()<<"]={";
  for (i=1;i<=hBelle[0]->GetNbinsX();i++) {
    cout<<hBelle[0]->GetBinCenter(i);
    if (i!=hBelle[0]->GetNbinsX()) cout<<", ";
    else cout<<" ";

    if (i%10==0) cout<<endl;
  }
  cout<<"};"<<endl;


  for (i=0;i<3;i++) {
    cout<<"\n"<<tag[i].c_str()<<endl;
    for (int j=1;j<=hn[i]->GetNbinsX();j++) {
      cout<<hn[i]->GetBinContent(j);
      if (i!=hn[i]->GetNbinsX()) cout<<", ";
      else cout<<" ";

      if (j%10==0) cout<<endl;
    }
  }

  //---------------------------
  // draw histo
  //---------------------------
  gStyle->SetLegendBorderSize(0);
  TLegend* leg=new TLegend(0.15,0.68,0.45,0.95);
  leg->SetFillStyle(0);
  leg->SetTextFont(43);
  leg->SetTextSize(20);
  leg->AddEntry(hBelle[0],"BelleII aerogel 1","l");
  leg->AddEntry(hBelle[1],"BelleII aerogel 2","l");
  for (i=0;i<3;i++) leg->AddEntry(hn[i],Form("%s, n_{405nm}=%.4f",tag[i].c_str(),n405[i]),"l");
  
  TCanvas* c=new TCanvas("c","",800,600);
  c->Range(0,0,1,1);
  c->cd();
  gPad->SetTopMargin(0.05);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.01);
  gPad->SetBottomMargin(0.12);
  
  hBelle[0]->Draw();//"ep");
  hBelle[1]->Draw("same");
  for (i=0;i<3;i++) hn[i]->Draw("same");
  for (i=0;i<3;i++) gr[i]->Draw("psame");
  leg->Draw();

  /*
  TFile* fout = new TFile("BelleII_RINDEX.root", "RECREATE");
  fout->cd();
  for (i=0;i<2;i++) hBelle[i]->Write();
  fout->Close();
  */
}
