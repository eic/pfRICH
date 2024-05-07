void readTree(const char *input, const char *output, const char *output_txt)
{

      cout << "running readTree.C" << endl;

      TFile *fa = new TFile(input);

      TTree *ta = (TTree *)fa->Get("T");

      TFile *ofile = TFile::Open(output, "recreate");

      Int_t partPDG, recoPDG, recoPhotons, recoCherenkovHitCount;
      Double_t partMom, partEta, partTheta, partPhi, partVertX, partVertY, partVertZ, recoTrackCherenkovAngle;
      Double_t zero = pow(10., -50.);

      ta->SetBranchAddress("partPDG", &partPDG);
      ta->SetBranchAddress("partMom", &partMom);
      ta->SetBranchAddress("partEta", &partEta);
      ta->SetBranchAddress("partTheta", &partTheta);
      ta->SetBranchAddress("partPhi", &partPhi);
      ta->SetBranchAddress("partVertX", &partVertX);
      ta->SetBranchAddress("partVertY", &partVertY);
      ta->SetBranchAddress("partVertZ", &partVertZ);
      ta->SetBranchAddress("recoPDG", &recoPDG);
      ta->SetBranchAddress("recoPhotons", &recoPhotons);
      ta->SetBranchAddress("recoTrackCherenkovAngle", &recoTrackCherenkovAngle);

      TH1D *h_p = new TH1D("p", "", 37, 0.4, 15.2);
      TH1D *h_t = new TH1D("theta", "", 20, 2.65, 3.1);
      TH1D *h_phi = new TH1D("phi", "", 120, -1.0 * TMath::Pi(), TMath::Pi());

      static Float_t fee[37][20][120], fepi[37][20][120], fek[37][20][120], fep[37][20][120], feb[37][20][120];
      static Float_t fpie[37][20][120], fpipi[37][20][120], fpik[37][20][120], fpip[37][20][120], fpib[37][20][120];
      static Float_t fke[37][20][120], fkpi[37][20][120], fkk[37][20][120], fkp[37][20][120], fkb[37][20][120];
      static Float_t fpe[37][20][120], fppi[37][20][120], fpk[37][20][120], fpp[37][20][120], fpb[37][20][120];

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        fee[pbin][tbin][phibin] = fepi[pbin][tbin][phibin] = fek[pbin][tbin][phibin] = fep[pbin][tbin][phibin] = feb[pbin][tbin][phibin] = 0.;
                        fpie[pbin][tbin][phibin] = fpipi[pbin][tbin][phibin] = fpik[pbin][tbin][phibin] = fpip[pbin][tbin][phibin] = fpib[pbin][tbin][phibin] = 0.;
                        fke[pbin][tbin][phibin] = fkpi[pbin][tbin][phibin] = fkk[pbin][tbin][phibin] = fkp[pbin][tbin][phibin] = fkb[pbin][tbin][phibin] = 0.;
                        fpe[pbin][tbin][phibin] = fppi[pbin][tbin][phibin] = fpk[pbin][tbin][phibin] = fpp[pbin][tbin][phibin] = fpb[pbin][tbin][phibin] = 0.;
                  }
            }
      }

      Int_t nEntries = (Int_t)ta->GetEntries();
      for (int i = 0; i < nEntries; i++)
      {
            if (i % 1000000 == 0)
                  cout << "Processing Event " << i << endl;
            ta->GetEntry(i);

            Int_t pbin = h_p->FindBin(partMom) - 1;
            Int_t tbin = h_t->FindBin(partTheta) - 1;
            Int_t phibin = h_phi->FindBin(partPhi) - 1;

            // if (partMom < 1) cout << pbin << endl;// -> bin number starts from 1

            if (pbin < 0)
                  continue;
            //if (tbin < 0)
            //    continue;
            //if (phibin < 0)
            //      continue;
                  
            if (partPDG == 11) // Look at Thrown Electrons
            {
                  if (recoPhotons < 3)
                  {
                        feb[pbin][tbin][phibin]++;
                  }
                  else
                  {
                        if (recoPDG == 11)
                              fee[pbin][tbin][phibin]++;
                        if (recoPDG == 211)
                              fepi[pbin][tbin][phibin]++;
                        if (recoPDG == 321)
                              fek[pbin][tbin][phibin]++;
                        if (recoPDG == 2212)
                              fep[pbin][tbin][phibin]++;
                  }
            }

            if (partPDG == 211) // Look at Thrown Pions
            {
                  if (recoPhotons < 3)
                  {
                        fpib[pbin][tbin][phibin]++;
                  }
                  else
                  {
                        if (recoPDG == 11)
                              fpie[pbin][tbin][phibin]++;
                        if (recoPDG == 211)
                              fpipi[pbin][tbin][phibin]++;
                        if (recoPDG == 321)
                              fpik[pbin][tbin][phibin]++;
                        if (recoPDG == 2212)
                              fpip[pbin][tbin][phibin]++;
                  }
            }

            if (partPDG == 321) // Look at Thrown Kaons
            {
                  if (recoPhotons < 3)
                  {
                        fkb[pbin][tbin][phibin]++;
                  }
                  else
                  {
                        if (recoPDG == 11)
                              fke[pbin][tbin][phibin]++;
                        if (recoPDG == 211)
                              fkpi[pbin][tbin][phibin]++;
                        if (recoPDG == 321)
                              fkk[pbin][tbin][phibin]++;
                        if (recoPDG == 2212)
                              fkp[pbin][tbin][phibin]++;
                  }
            }

            if (partPDG == 2212) // Look at Thrown Protons
            {
                  if (recoPhotons < 3)
                  {
                        fpb[pbin][tbin][phibin]++;
                  }
                  else
                  {
                        if (recoPDG == 11)
                              fpe[pbin][tbin][phibin]++;
                        if (recoPDG == 211)
                              fppi[pbin][tbin][phibin]++;
                        if (recoPDG == 321)
                              fpk[pbin][tbin][phibin]++;
                        if (recoPDG == 2212)
                              fpp[pbin][tbin][phibin]++;
                  }
            }
      }

      TH2D *h[37][20][120];
      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        h[pbin][tbin][phibin] = new TH2D(Form("p%it%iphi%i", pbin, tbin, phibin), Form("p%it%iphi%i;reco;truth", pbin, tbin, phibin), 5, 0., 5., 4, 0., 4.);
                  }
            }
      }

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {

                        Double_t fe_tot = fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero;
                        fee[pbin][tbin][phibin] /= fe_tot;
                        fepi[pbin][tbin][phibin] /= fe_tot;
                        fek[pbin][tbin][phibin] /= fe_tot;
                        fep[pbin][tbin][phibin] /= fe_tot;
                        feb[pbin][tbin][phibin] /= fe_tot;

                        Double_t fpi_tot = fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero;
                        fpie[pbin][tbin][phibin] /= fpi_tot;
                        fpipi[pbin][tbin][phibin] /= fpi_tot;
                        fpik[pbin][tbin][phibin] /= fpi_tot;
                        fpip[pbin][tbin][phibin] /= fpi_tot;
                        fpib[pbin][tbin][phibin] /= fpi_tot;

                        Double_t fk_tot = fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero;
                        fke[pbin][tbin][phibin] /= fk_tot;
                        fkpi[pbin][tbin][phibin] /= fk_tot;
                        fkk[pbin][tbin][phibin] /= fk_tot;
                        fkp[pbin][tbin][phibin] /= fk_tot;
                        fkb[pbin][tbin][phibin] /= fk_tot;

                        Double_t fp_tot = fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero;
                        fpe[pbin][tbin][phibin] /= fp_tot;
                        fppi[pbin][tbin][phibin] /= fp_tot;
                        fpk[pbin][tbin][phibin] /= fp_tot;
                        fpp[pbin][tbin][phibin] /= fp_tot;
                        fpb[pbin][tbin][phibin] /= fp_tot;

                        h[pbin][tbin][phibin]->SetBinContent(1, 4, fee[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(2, 4, fepi[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(3, 4, fek[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(4, 4, fep[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(5, 4, feb[pbin][tbin][phibin]);

                        h[pbin][tbin][phibin]->SetBinContent(1, 3, fpie[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(2, 3, fpipi[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(3, 3, fpik[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(4, 3, fpip[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(5, 3, fpib[pbin][tbin][phibin]);

                        h[pbin][tbin][phibin]->SetBinContent(1, 2, fke[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(2, 2, fkpi[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(3, 2, fkk[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(4, 2, fkp[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(5, 2, fkb[pbin][tbin][phibin]);

                        h[pbin][tbin][phibin]->SetBinContent(1, 1, fpe[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(2, 1, fppi[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(3, 1, fpk[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(4, 1, fpp[pbin][tbin][phibin]);
                        h[pbin][tbin][phibin]->SetBinContent(5, 1, fpb[pbin][tbin][phibin]);

                        Double_t p = h_p->GetXaxis()->GetBinCenter(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinCenter(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinCenter(phibin + 1);

                        if (pbin == 0 && tbin == 0 && phibin == 0)
                        {
                              cout << "truth PID, charge, p, theta, phi, prob(e), prob(pi), prob(k), prob(p), prob(fail)" << endl;
                              cout << "11   -1 " << p << " " << t << " " << phi << " " << fee[pbin][tbin][phibin] << " " << fepi[pbin][tbin][phibin] << " " << fek[pbin][tbin][phibin] << " " << fep[pbin][tbin][phibin] << " " << feb[pbin][tbin][phibin] << endl;
                              cout << "211  1  " << p << " " << t << " " << phi << " " << fpie[pbin][tbin][phibin] << " " << fpipi[pbin][tbin][phibin] << " " << fpik[pbin][tbin][phibin] << " " << fpip[pbin][tbin][phibin] << " " << fpib[pbin][tbin][phibin] << endl;
                              cout << "321  1  " << p << " " << t << " " << phi << " " << fke[pbin][tbin][phibin] << " " << fkpi[pbin][tbin][phibin] << " " << fkk[pbin][tbin][phibin] << " " << fkp[pbin][tbin][phibin] << " " << fkb[pbin][tbin][phibin] << endl;
                              cout << "2212 1  " << p << " " << t << " " << phi << " " << fpe[pbin][tbin][phibin] << " " << fppi[pbin][tbin][phibin] << " " << fpk[pbin][tbin][phibin] << " " << fpp[pbin][tbin][phibin] << " " << fpb[pbin][tbin][phibin] << endl;
                        }
                  }
            }
      }

      FILE *filePointer;
      filePointer = fopen(output_txt, "w");

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        Double_t p = h_p->GetXaxis()->GetBinCenter(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinCenter(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinCenter(phibin + 1);

                        fprintf(filePointer, "11   -1 %f %f %f %f %f %f %f %f\n", p, t, phi, fee[pbin][tbin][phibin], fepi[pbin][tbin][phibin], fek[pbin][tbin][phibin], fep[pbin][tbin][phibin], feb[pbin][tbin][phibin]);
                  }
            }
      }

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        Double_t p = h_p->GetXaxis()->GetBinCenter(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinCenter(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinCenter(phibin + 1);

                        fprintf(filePointer, "211  1  %f %f %f %f %f %f %f %f\n", p, t, phi, fpie[pbin][tbin][phibin], fpipi[pbin][tbin][phibin], fpik[pbin][tbin][phibin], fpip[pbin][tbin][phibin], fpib[pbin][tbin][phibin]);
                  }
            }
      }

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        Double_t p = h_p->GetXaxis()->GetBinCenter(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinCenter(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinCenter(phibin + 1);

                        fprintf(filePointer, "321  1  %f %f %f %f %f %f %f %f\n", p, t, phi, fke[pbin][tbin][phibin], fkpi[pbin][tbin][phibin], fkk[pbin][tbin][phibin], fkp[pbin][tbin][phibin], fkb[pbin][tbin][phibin]);
                  }
            }
      }

      for (int pbin = 0; pbin < 37; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 120; phibin++)
                  {
                        Double_t p = h_p->GetXaxis()->GetBinCenter(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinCenter(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinCenter(phibin + 1);

                        fprintf(filePointer, "2212 1 %f %f %f %f %f %f %f %f\n", p, t, phi, fpe[pbin][tbin][phibin], fppi[pbin][tbin][phibin], fpk[pbin][tbin][phibin], fpp[pbin][tbin][phibin], fpb[pbin][tbin][phibin]);
                  }
            }
      }

      fclose(filePointer);
      ofile->Write();
      ofile->Close();
}
