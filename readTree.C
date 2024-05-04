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

      TH1D *h_p = new TH1D("p", "", 15, 0.1, 15.1);
      TH1D *h_t = new TH1D("theta", "", 20, 2.7, 3.1);
      TH1D *h_phi = new TH1D("phi", "", 360, -3.1415926, 3.1415926);

      static Int_t fee[15][20][360], fepi[15][20][360], fek[15][20][360], fep[15][20][360], feb[15][20][360];
      static Int_t fpie[15][20][360], fpipi[15][20][360], fpik[15][20][360], fpip[15][20][360], fpib[15][20][360];
      static Int_t fke[15][20][360], fkpi[15][20][360], fkk[15][20][360], fkp[15][20][360], fkb[15][20][360];
      static Int_t fpe[15][20][360], fppi[15][20][360], fpk[15][20][360], fpp[15][20][360], fpb[15][20][360];

      for (int pbin = 0; pbin < 15; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 360; phibin++)
                  {
                        fee[pbin][tbin][phibin] = fepi[pbin][tbin][phibin] = fek[pbin][tbin][phibin] = fep[pbin][tbin][phibin] = feb[pbin][tbin][phibin] = 0;
                        fpie[pbin][tbin][phibin] = fpipi[pbin][tbin][phibin] = fpik[pbin][tbin][phibin] = fpip[pbin][tbin][phibin] = fpib[pbin][tbin][phibin] = 0;
                        fke[pbin][tbin][phibin] = fkpi[pbin][tbin][phibin] = fkk[pbin][tbin][phibin] = fkp[pbin][tbin][phibin] = fkb[pbin][tbin][phibin] = 0;
                        fpe[pbin][tbin][phibin] = fppi[pbin][tbin][phibin] = fpk[pbin][tbin][phibin] = fpp[pbin][tbin][phibin] = fpb[pbin][tbin][phibin] = 0;
                  }
            }
      }

      Int_t nEntries = (Int_t)ta->GetEntries();
      for (int i = 0; i < nEntries; i++)
      {
            if (i % 100000 == 0)
                  cout << "Processing Event " << i << endl;
            ta->GetEntry(i);

            Int_t pbin = h_p->FindBin(partMom) - 1;
            Int_t tbin = h_t->FindBin(partTheta) - 1;
            Int_t phibin = h_phi->FindBin(partPhi) - 1;

            // if (partMom < 1) cout << pbin << endl;// -> bin number starts from 1

            if (partPDG == 11) // Look at Thrown Electrons
            {
                  if (recoPhotons < 4)
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
                  if (recoPhotons < 4)
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
                  if (recoPhotons < 4)
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
                  if (recoPhotons < 4)
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

      TH2D *h[15][20][360];
      for (int pbin = 0; pbin < 15; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 360; phibin++)
                  {
                        h[pbin][tbin][phibin] = new TH2D(Form("p%it%iphi%i", pbin, tbin, phibin), Form("p%it%iphi%i;reco;truth", pbin, tbin, phibin), 5, 0., 5., 4, 0., 4.);
                  }
            }
      }

      FILE *filePointer;
      filePointer = fopen(output_txt, "w");

      for (int pbin = 0; pbin < 15; pbin++)
      {
            for (int tbin = 0; tbin < 20; tbin++)
            {
                  for (int phibin = 0; phibin < 360; phibin++)
                  {

                        Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                        Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                        Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

                        Double_t mc_e_rec_e = (1.0 * fee[pbin][tbin][phibin]) / (1.0 * (fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero));
                        Double_t mc_e_rec_pi = (1.0 * fepi[pbin][tbin][phibin]) / (1.0 * (fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero));
                        Double_t mc_e_rec_k = (1.0 * fek[pbin][tbin][phibin]) / (1.0 * (fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero));
                        Double_t mc_e_rec_p = (1.0 * fep[pbin][tbin][phibin]) / (1.0 * (fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero));
                        Double_t mc_e_rec_b = (1.0 * feb[pbin][tbin][phibin]) / (1.0 * (fee[pbin][tbin][phibin] + fepi[pbin][tbin][phibin] + fek[pbin][tbin][phibin] + fep[pbin][tbin][phibin] + feb[pbin][tbin][phibin] + zero));

                        Double_t mc_pi_rec_e = (1.0 * fpie[pbin][tbin][phibin]) / (1.0 * (fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero));
                        Double_t mc_pi_rec_pi = (1.0 * fpipi[pbin][tbin][phibin]) / (1.0 * (fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero));
                        Double_t mc_pi_rec_k = (1.0 * fpik[pbin][tbin][phibin]) / (1.0 * (fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero));
                        Double_t mc_pi_rec_p = (1.0 * fpip[pbin][tbin][phibin]) / (1.0 * (fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero));
                        Double_t mc_pi_rec_b = (1.0 * fpib[pbin][tbin][phibin]) / (1.0 * (fpie[pbin][tbin][phibin] + fpipi[pbin][tbin][phibin] + fpik[pbin][tbin][phibin] + fpip[pbin][tbin][phibin] + fpib[pbin][tbin][phibin] + zero));

                        Double_t mc_k_rec_e = (1.0 * fke[pbin][tbin][phibin]) / (1.0 * (fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero));
                        Double_t mc_k_rec_pi = (1.0 * fkpi[pbin][tbin][phibin]) / (1.0 * (fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero));
                        Double_t mc_k_rec_k = (1.0 * fkk[pbin][tbin][phibin]) / (1.0 * (fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero));
                        Double_t mc_k_rec_p = (1.0 * fkp[pbin][tbin][phibin]) / (1.0 * (fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero));
                        Double_t mc_k_rec_b = (1.0 * fkb[pbin][tbin][phibin]) / (1.0 * (fke[pbin][tbin][phibin] + fkpi[pbin][tbin][phibin] + fkk[pbin][tbin][phibin] + fkp[pbin][tbin][phibin] + fkb[pbin][tbin][phibin] + zero));

                        Double_t mc_p_rec_e = (1.0 * fpe[pbin][tbin][phibin]) / (1.0 * (fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero));
                        Double_t mc_p_rec_pi = (1.0 * fppi[pbin][tbin][phibin]) / (1.0 * (fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero));
                        Double_t mc_p_rec_k = (1.0 * fpk[pbin][tbin][phibin]) / (1.0 * (fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero));
                        Double_t mc_p_rec_p = (1.0 * fpp[pbin][tbin][phibin]) / (1.0 * (fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero));
                        Double_t mc_p_rec_b = (1.0 * fpb[pbin][tbin][phibin]) / (1.0 * (fpe[pbin][tbin][phibin] + fppi[pbin][tbin][phibin] + fpk[pbin][tbin][phibin] + fpp[pbin][tbin][phibin] + fpb[pbin][tbin][phibin] + zero));

                        h[pbin][tbin][phibin]->SetBinContent(1, 4, mc_e_rec_e);
                        h[pbin][tbin][phibin]->SetBinContent(2, 4, mc_e_rec_pi);
                        h[pbin][tbin][phibin]->SetBinContent(3, 4, mc_e_rec_k);
                        h[pbin][tbin][phibin]->SetBinContent(4, 4, mc_e_rec_p);
                        h[pbin][tbin][phibin]->SetBinContent(5, 4, mc_e_rec_b);

                        h[pbin][tbin][phibin]->SetBinContent(1, 3, mc_pi_rec_e);
                        h[pbin][tbin][phibin]->SetBinContent(2, 3, mc_pi_rec_pi);
                        h[pbin][tbin][phibin]->SetBinContent(3, 3, mc_pi_rec_k);
                        h[pbin][tbin][phibin]->SetBinContent(4, 3, mc_pi_rec_p);
                        h[pbin][tbin][phibin]->SetBinContent(5, 3, mc_pi_rec_b);

                        h[pbin][tbin][phibin]->SetBinContent(1, 2, mc_k_rec_e);
                        h[pbin][tbin][phibin]->SetBinContent(2, 2, mc_k_rec_pi);
                        h[pbin][tbin][phibin]->SetBinContent(3, 2, mc_k_rec_k);
                        h[pbin][tbin][phibin]->SetBinContent(4, 2, mc_k_rec_p);
                        h[pbin][tbin][phibin]->SetBinContent(5, 2, mc_k_rec_b);

                        h[pbin][tbin][phibin]->SetBinContent(1, 1, mc_p_rec_e);
                        h[pbin][tbin][phibin]->SetBinContent(2, 1, mc_p_rec_pi);
                        h[pbin][tbin][phibin]->SetBinContent(3, 1, mc_p_rec_k);
                        h[pbin][tbin][phibin]->SetBinContent(4, 1, mc_p_rec_p);
                        h[pbin][tbin][phibin]->SetBinContent(5, 1, mc_p_rec_b);

                        // h[pbin][tbin][phibin]->Write();

                        fprintf(filePointer, "11   %f %f %f %f %f %f %f %f\n", p, t, phi, mc_e_rec_e, mc_e_rec_pi, mc_e_rec_k, mc_e_rec_p, mc_e_rec_b);
                        fprintf(filePointer, "211  %f %f %f %f %f %f %f %f\n", p, t, phi, mc_pi_rec_e, mc_pi_rec_pi, mc_pi_rec_k, mc_pi_rec_p, mc_pi_rec_b);
                        fprintf(filePointer, "321  %f %f %f %f %f %f %f %f\n", p, t, phi, mc_k_rec_e, mc_k_rec_pi, mc_k_rec_k, mc_k_rec_p, mc_k_rec_b);
                        fprintf(filePointer, "2212 %f %f %f %f %f %f %f %f\n", p, t, phi, mc_p_rec_e, mc_p_rec_pi, mc_p_rec_k, mc_p_rec_p, mc_p_rec_b);

                        if (pbin == 0 && tbin == 0 && phibin == 0)
                        {
                              cout << "truth PID, p, theta, phi, prob(e), prob(pi), prob(k), prob(p), prob(fail)" << endl;
                              cout << "11 " << p << " " << t << " " << phi << " " << mc_e_rec_e << " " << mc_e_rec_pi << " " << mc_e_rec_k << " " << mc_e_rec_p << " " << mc_e_rec_b << endl;
                              cout << "211 " << p << " " << t << " " << phi << " " << mc_pi_rec_e << " " << mc_pi_rec_pi << " " << mc_pi_rec_k << " " << mc_pi_rec_p << " " << mc_pi_rec_b << endl;
                              cout << "321 " << p << " " << t << " " << phi << " " << mc_k_rec_e << " " << mc_k_rec_pi << " " << mc_k_rec_k << " " << mc_k_rec_p << " " << mc_k_rec_b << endl;
                              cout << "2212 " << p << " " << t << " " << phi << " " << mc_p_rec_e << " " << mc_p_rec_pi << " " << mc_p_rec_k << " " << mc_p_rec_p << " " << mc_p_rec_b << endl;
                        }
                        if (pbin == 14 && tbin == 9 && phibin == 11)
                        {
                              cout << "11 " << p << " " << t << " " << phi << " " << mc_e_rec_e << " " << mc_e_rec_pi << " " << mc_e_rec_k << " " << mc_e_rec_p << " " << mc_e_rec_b << endl;
                              cout << "211 " << p << " " << t << " " << phi << " " << mc_pi_rec_e << " " << mc_pi_rec_pi << " " << mc_pi_rec_k << " " << mc_pi_rec_p << " " << mc_pi_rec_b << endl;
                              cout << "321 " << p << " " << t << " " << phi << " " << mc_k_rec_e << " " << mc_k_rec_pi << " " << mc_k_rec_k << " " << mc_k_rec_p << " " << mc_k_rec_b << endl;
                              cout << "2212 " << p << " " << t << " " << phi << " " << mc_p_rec_e << " " << mc_p_rec_pi << " " << mc_p_rec_k << " " << mc_p_rec_p << " " << mc_p_rec_b << endl;
                        }
                  }
            }
      }

      fclose(filePointer);
      ofile->Write();
      ofile->Close();
}
