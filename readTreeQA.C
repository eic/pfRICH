void readTreeQA(const char *input, const char *output, const char *output_txt)
{

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
    TH1D *h_t = new TH1D("theta", "", 10, 2.7, 3.1);
    TH1D *h_phi = new TH1D("phi", "", 12, -1.0 * TMath::Pi(), TMath::Pi());

    TProfile2D *recoPhiVsEtaElecPhoton[4];
    TH2D *partPhiVsEtaElec[4];
    TH2D *partPhiVsEtaElecMatch[4];
    TH2D *partPhiVsEtaElecUnMatch[4];
    TH2D *partPhiVsEtaElecMatchPhoton[4];
    TH1D *recoElecPhoton[4];

    TProfile2D *recoPhiVsEtaPionPhoton[4];
    TH2D *partPhiVsEtaPion[4];
    TH2D *partPhiVsEtaPionMatch[4];
    TH2D *partPhiVsEtaPionUnMatch[4];
    TH1D *recoPionPhoton[4];

    TProfile2D *recoPhiVsEtaKaonPhoton[4];
    TH2D *partPhiVsEtaKaon[4];
    TH2D *partPhiVsEtaKaonMatch[4];
    TH2D *partPhiVsEtaKaonUnMatch[4];
    TH1D *recoKaonPhoton[4];

    TProfile2D *recoPhiVsEtaProtonPhoton[4];
    TH2D *partPhiVsEtaProton[4];
    TH2D *partPhiVsEtaProtonMatch[4];
    TH2D *partPhiVsEtaProtonUnMatch[4];
    TH1D *recoProtonPhoton[4];

    for (int i = 0; i < 4; i++)
    {
        recoPhiVsEtaElecPhoton[i] = new TProfile2D(Form("recoPhiVsEtaElecPhoton_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaElec[i] = new TH2D(Form("partPhiVsEtaElecMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaElecMatch[i] = new TH2D(Form("partPhiVsEtaElecMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaElecUnMatch[i] = new TH2D(Form("partPhiVsEtaElecUnMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaElecMatchPhoton[i] = new TH2D(Form("partPhiVsEtaElecMatchPhotonMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        recoElecPhoton[i] = new TH1D(Form("recoElecPhoton_%d", i), "", 25, -0.1, 24.9);

        recoPhiVsEtaPionPhoton[i] = new TProfile2D(Form("recoPhiVsEtaPionPhoton_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaPion[i] = new TH2D(Form("partPhiVsEtaPionMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaPionMatch[i] = new TH2D(Form("partPhiVsEtaPionMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaPionUnMatch[i] = new TH2D(Form("partPhiVsEtaPionUnMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        recoPionPhoton[i] = new TH1D(Form("recoPionPhoton_%d", i), "", 25, -0.1, 24.9);

        recoPhiVsEtaKaonPhoton[i] = new TProfile2D(Form("recoPhiVsEtaKaonPhoton_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaKaon[i] = new TH2D(Form("partPhiVsEtaKaonMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaKaonMatch[i] = new TH2D(Form("partPhiVsEtaKaonMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaKaonUnMatch[i] = new TH2D(Form("partPhiVsEtaKaonUnMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        recoKaonPhoton[i] = new TH1D(Form("recoKaonPhoton_%d", i), "", 25, -0.1, 24.9);

        recoPhiVsEtaProtonPhoton[i] = new TProfile2D(Form("recoPhiVsEtaProtonPhoton_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaProton[i] = new TH2D(Form("partPhiVsEtaProtonMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaProtonMatch[i] = new TH2D(Form("partPhiVsEtaProtonMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        partPhiVsEtaProtonUnMatch[i] = new TH2D(Form("partPhiVsEtaProtonUnMatchMom_%d", i), "", 50, -4., -1., 100, -TMath::Pi(), TMath::Pi());
        recoProtonPhoton[i] = new TH1D(Form("recoProtonPhoton_%d", i), "", 25, -0.1, 24.9);
    }

    TH2D *numPhotonsVsPhiEta25Mom10Pion = new TH2D("numPhotonsVsPhiEta25Mom10Pion", "", 60, -TMath::Pi(), TMath::Pi(), 25, 0., 25.);
    TH2D *numPhotonsVsPhiEta25Mom4Pion = new TH2D("numPhotonsVsPhiEta25Mom4Pion", "", 60, -TMath::Pi(), TMath::Pi(), 25, 0., 25.);

    static Float_t fee[15][10][12], fepi[15][10][12], fek[15][10][12], fep[15][10][12], feb[15][10][12];
    static Float_t fpie[15][10][12], fpipi[15][10][12], fpik[15][10][12], fpip[15][10][12], fpib[15][10][12];
    static Float_t fke[15][10][12], fkpi[15][10][12], fkk[15][10][12], fkp[15][10][12], fkb[15][10][12];
    static Float_t fpe[15][10][12], fppi[15][10][12], fpk[15][10][12], fpp[15][10][12], fpb[15][10][12];

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
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

        Int_t momSelect = -1;
        if (partMom > 1.0 && partMom < 3.0)
            momSelect = 0;
        if (partMom > 3.0 && partMom < 5.0)
            momSelect = 1;
        if (partMom > 5.0 && partMom < 7.0)
            momSelect = 2;
        if (partMom > 7.0 && partMom < 9.0)
            momSelect = 3;

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
            if (momSelect != -1 && partEta > -2.6 && partEta < -2.4 && partPhi > 2 && partPhi < 2.5)
            {
                recoElecPhoton[momSelect]->Fill(recoPhotons);
                recoPhiVsEtaElecPhoton[momSelect]->Fill(partEta, partPhi, recoPhotons);
                partPhiVsEtaElec[momSelect]->Fill(partEta, partPhi);
                if (recoPDG == 11)
                    partPhiVsEtaElecMatch[momSelect]->Fill(partEta, partPhi);
                if (recoPDG != 11)
                    partPhiVsEtaElecUnMatch[momSelect]->Fill(partEta, partPhi);
                if (recoPDG == 11 && recoPhotons > 0)
                    partPhiVsEtaElecMatchPhoton[momSelect]->Fill(partEta, partPhi);
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
            if (momSelect != -1 && partEta > -2.6 && partEta < -2.4 && partPhi > 2 && partPhi < 2.5)
            {
                recoPionPhoton[momSelect]->Fill(recoPhotons);
                recoPhiVsEtaPionPhoton[momSelect]->Fill(partEta, partPhi, recoPhotons);
                partPhiVsEtaPion[momSelect]->Fill(partEta, partPhi);
                if (recoPDG == 211)
                    partPhiVsEtaPionMatch[momSelect]->Fill(partEta, partPhi);
                if (recoPDG != 211)
                    partPhiVsEtaPionUnMatch[momSelect]->Fill(partEta, partPhi);
            }
            if (partEta < -2.49 && partEta > -2.51)
            {
                if (partMom > 9.5 && partMom < 10.5)
                {
                    numPhotonsVsPhiEta25Mom10Pion->Fill(partPhi, recoPhotons);
                }
                if (partMom > 3.5 && partMom < 4.5)
                {
                    numPhotonsVsPhiEta25Mom4Pion->Fill(partPhi, recoPhotons);
                }
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
            if (momSelect != -1 && partEta > -2.6 && partEta < -2.4 && partPhi > 2 && partPhi < 2.5)
            {
                recoKaonPhoton[momSelect]->Fill(recoPhotons);
                recoPhiVsEtaKaonPhoton[momSelect]->Fill(partEta, partPhi, recoPhotons);
                partPhiVsEtaKaon[momSelect]->Fill(partEta, partPhi);
                if (recoPDG == 321)
                    partPhiVsEtaKaonMatch[momSelect]->Fill(partEta, partPhi);
                if (recoPDG != 321)
                    partPhiVsEtaKaonUnMatch[momSelect]->Fill(partEta, partPhi);
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
            if (momSelect != -1 && partEta > -2.6 && partEta < -2.4 && partPhi > 2. && partPhi < 2.5)
            {
                recoProtonPhoton[momSelect]->Fill(recoPhotons);
                recoPhiVsEtaProtonPhoton[momSelect]->Fill(partEta, partPhi, recoPhotons);
                partPhiVsEtaProton[momSelect]->Fill(partEta, partPhi);
                if (recoPDG == 2212)
                    partPhiVsEtaProtonMatch[momSelect]->Fill(partEta, partPhi);
                if (recoPDG != 2212)
                    partPhiVsEtaProtonUnMatch[momSelect]->Fill(partEta, partPhi);
            }
        }
    }

    TH2D *h[15][10][12];
    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
            {
                h[pbin][tbin][phibin] = new TH2D(Form("p%it%iphi%i", pbin, tbin, phibin), Form("p%it%iphi%i;reco;truth", pbin, tbin, phibin), 5, 0., 5., 4, 0., 4.);
            }
        }
    }

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
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

                Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

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

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
            {
                Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

                fprintf(filePointer, "11   -1 %f %f %f %f %f %f %f %f\n", p, t, phi, fee[pbin][tbin][phibin], fepi[pbin][tbin][phibin], fek[pbin][tbin][phibin], fep[pbin][tbin][phibin], feb[pbin][tbin][phibin]);
            }
        }
    }

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
            {
                Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

                fprintf(filePointer, "211  1  %f %f %f %f %f %f %f %f\n", p, t, phi, fpie[pbin][tbin][phibin], fpipi[pbin][tbin][phibin], fpik[pbin][tbin][phibin], fpip[pbin][tbin][phibin], fpib[pbin][tbin][phibin]);
            }
        }
    }

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
            {
                Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

                fprintf(filePointer, "321  1  %f %f %f %f %f %f %f %f\n", p, t, phi, fke[pbin][tbin][phibin], fkpi[pbin][tbin][phibin], fkk[pbin][tbin][phibin], fkp[pbin][tbin][phibin], fkb[pbin][tbin][phibin]);
            }
        }
    }

    for (int pbin = 0; pbin < 15; pbin++)
    {
        for (int tbin = 0; tbin < 10; tbin++)
        {
            for (int phibin = 0; phibin < 12; phibin++)
            {
                Double_t p = h_p->GetXaxis()->GetBinLowEdge(pbin + 1);
                Double_t t = h_t->GetXaxis()->GetBinLowEdge(tbin + 1);
                Double_t phi = h_phi->GetXaxis()->GetBinLowEdge(phibin + 1);

                fprintf(filePointer, "2212 1 %f %f %f %f %f %f %f %f\n", p, t, phi, fpe[pbin][tbin][phibin], fppi[pbin][tbin][phibin], fpk[pbin][tbin][phibin], fpp[pbin][tbin][phibin], fpb[pbin][tbin][phibin]);
            }
        }
    }

    fclose(filePointer);
    ofile->Write();
    ofile->Close();
}
