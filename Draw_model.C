// Macro for visualizing, changing color and transparency of volumes using Eve
// Ondrej Lomicky, lomicond@fjfi.cvut.cz

void RecursiveColor(TGeoVolume *vol, Color_t color)
{
    if(!vol) return;
    vol->SetLineColor(color);
    Int_t nd = vol->GetNdaughters();
    for (Int_t i=0; i<nd; i++) {
        RecursiveColor(vol->GetNode(i)->GetVolume(),color);
    }
}

void RecursiveTransparency(TGeoVolume *vol, double tra){
    if(!vol) return;
    vol->SetTransparency(tra);
    Int_t nd = vol->GetNdaughters();
    for (Int_t i=0; i<nd; i++) {
        RecursiveTransparency(vol->GetNode(i)->GetVolume(),tra);
    }
}

void pfRICHcolor(TGeoVolume *vol){
    if(!vol) return;
    Int_t nd = vol->GetNdaughters();
    for (Int_t i=0; i<nd; i++) {
        TGeoVolume *voldaughter = vol->GetNode(i)->GetVolume();
        TString volname = voldaughter->GetIconName();
        volname.Remove(volname.Index("_shape"));
        //Write out all subnodes
        //cout << volname << endl;
        if (volname.Contains("Mirror")) voldaughter->SetLineColor(kAzure-4);
        else if (volname.Contains("QuartzWindow")) voldaughter->SetLineColor(kYellow);
        else voldaughter->SetLineColor(kGray+2);
        pfRICHcolor(vol->GetNode(i)->GetVolume());
    }
}

void Draw_model()
{
    TEveManager::Create();
    gGeoManager = TGeoManager::Import("geo/detector_geometry.root");

    //Default colors
    gGeoManager->DefaultColors();

    if (gGeoManager == nullptr) return;

    //Vector of strings
    std::vector<TString> nodes;

    //cycle over all nodes
    TObjArray* allnodes = gGeoManager->GetTopNode()->GetNodes();
    const int nNodes = allnodes->GetEntries();
    for(Int_t i=0; i<nNodes;i++) {
        TGeoNode *node = (TGeoNode *) allnodes->At(i);
        TGeoVolume *vol = node->GetVolume();
        TString volname2 = vol->GetIconName();
        volname2.Remove(volname2.Index("_shape"));
        nodes.push_back(volname2);
        //Write out all nodes
        //cout << nodes[i] << endl;
    }

    //RecursiveColor(gGeoManager->GetVolume("PFRICH"), kGreen);
    //RecursiveTransparency(gGeoManager->GetVolume("PFRICH"),0);

    //Set transparency and colors to all nodes
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]=="PFRICH") pfRICHcolor(gGeoManager->GetVolume(nodes[i]));
        //if (nodes[i]=="PFRICH") RecursiveColor(gGeoManager->GetVolume(nodes[i]), kMagenta);
        //else if (nodes[i]=="EcalEndcapP");
        else RecursiveColor(gGeoManager->GetVolume(nodes[i]), 20);
        //RecursiveTransparency(gGeoManager->GetVolume(nodes[i]),100);
        //if (nodes[i]!="PFRICH") RecursiveColor(gGeoManager->GetVolume(nodes[i]), 20);
    }


    TEveGeoTopNode *EPIC_Tracker = new TEveGeoTopNode(gGeoManager,gGeoManager->GetTopNode());

    EPIC_Tracker->SetVisLevel(100);
    gEve->AddGlobalElement(EPIC_Tracker);
    gEve->FullRedraw3D(kTRUE);

    TGLViewer  *viewer = gEve->GetDefaultGLViewer();
    viewer->GetClipSet()->SetClipType(TGLClip::EType(1));
    viewer->DoDraw();

    //Save it to a root file
    TFile *f = new TFile("model.root","RECREATE");
    f->cd();
    gGeoManager->Write();
    f->Close();

//////gEve->GetDefaultGLViewer()->SavePicture("test.png");

}




