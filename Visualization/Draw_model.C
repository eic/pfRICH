

//==============================================================================
// Global variables.
//------------------------------------------------------------------------------

// Shyam Kumar; INFN Bari, Italy;
// shyam.kumar@ba.infn.it; shyam055119@gmail.com

//// Macro for visualizing detector geometry and event
//// Modified by Ondrej Lomicky, lomicond@fjfi.cvut.cz

//// Run for detector geometry:
//// root Draw_model.C

//// Run for event display:
//// root Draw_model.C\(true\)

#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TEveElement.h"
#include "TEveGeoShape.h"
#include "TGeoTube.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TEveManager.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TEveGeoNode.h"
#include "TEveText.h"
#ifndef __RUN_EPIC_DISPLAY__

//==============================================================================
// Forward decalarations of CINT functions.
//------------------------------------------------------------------------------
void epic_load_event();
void epic_make_gui();
void SetViewers(TEveViewer *RPhi, TEveViewer *RhoZ, TEveScene *fRPhiScene, TEveScene *fRhoZScene);
TEvePointSet* AddHits(TTreeReaderArray<Double_t> *xhit,TTreeReaderArray<Double_t> *yhit,TTreeReaderArray<Double_t> *zhit, TTreeReaderArray<Int_t> *quality, TEvePointSet *hits, TString name ="");
void run_epic_display();

// Implemented in MultiView.C
class MultiView;
MultiView* gMultiView = 0;
TEveTrack  *track; TEveRecTrackD *rc;  TEvePointSetArray *hits=0;
TEveTrackList *gTrackList = 0;
Int_t iEvent = 0;
TGNumberEntry *num=0x0;

//==============================================================================
// Constants.
//------------------------------------------------------------------------------
// Reading Tree
TFile* file = new TFile("sim.edm4hep.root"); // Tree with tracks and hits
// Create the tree reader and its data containers
TTreeReader myReader("events", file); // name of tree and file

TTreeReaderArray<Float_t> charge(myReader, "MCParticles.charge");
TTreeReaderArray<Double_t> vx_mc(myReader, "MCParticles.vertex.x");
TTreeReaderArray<Double_t> vy_mc(myReader, "MCParticles.vertex.y");
TTreeReaderArray<Double_t> vz_mc(myReader, "MCParticles.vertex.z");
TTreeReaderArray<Float_t> px_mc(myReader, "MCParticles.momentum.x");
TTreeReaderArray<Float_t> py_mc(myReader, "MCParticles.momentum.y");
TTreeReaderArray<Float_t> pz_mc(myReader, "MCParticles.momentum.z");
TTreeReaderArray<Int_t> status(myReader, "MCParticles.generatorStatus");
TTreeReaderArray<Int_t> pdg(myReader, "MCParticles.PDG");

//There you can change the hits that you want to display
TTreeReaderArray<Double_t> *vtx_si_x, *vtx_si_y, *vtx_si_z;
TTreeReaderArray<Double_t> *barrel_si_x1, *barrel_si_y1, *barrel_si_z1;
TTreeReaderArray<Double_t> *disks_si_x2, *disks_si_y2, *disks_si_z2;
TTreeReaderArray<Double_t> *endcap_tof_x1, *endcap_tof_y1, *endcap_tof_z1;
TTreeReaderArray<Double_t> *barrel_mm_x2, *barrel_mm_y2, *barrel_mm_z2;
TTreeReaderArray<Double_t> *barrel_tof_x, *barrel_tof_y, *barrel_tof_z;
TTreeReaderArray<Double_t> *pfRICH_x, *pfRICH_y, *pfRICH_z;
TTreeReaderArray<Int_t> *vtx_si_quality, *barrel_si_quality, *disks_si_quality, *endcap_etof_quality, *barrel_mm_quality, *barrel_tof_quality, *pfRICH_quality;


void epic_display()
{

    // MC Track Properties

    //There you can change the hits that you want to display
    // Hits on detectors
    // Vtx
    vtx_si_x = new TTreeReaderArray<Double_t>(myReader, "VertexBarrelHits.position.x");
    vtx_si_y = new TTreeReaderArray<Double_t>(myReader, "VertexBarrelHits.position.y");
    vtx_si_z = new TTreeReaderArray<Double_t>(myReader, "VertexBarrelHits.position.z");
    vtx_si_quality = new TTreeReaderArray<Int_t>(myReader, "VertexBarrelHits.quality");
    // Barrel
    barrel_si_x1 = new TTreeReaderArray<Double_t>(myReader, "SiBarrelHits.position.x");
    barrel_si_y1 = new TTreeReaderArray<Double_t>(myReader, "SiBarrelHits.position.y");
    barrel_si_z1 = new TTreeReaderArray<Double_t>(myReader, "SiBarrelHits.position.z");
    barrel_si_quality = new TTreeReaderArray<Int_t>(myReader, "SiBarrelHits.quality");
    // Silicon Disks
    disks_si_x2 = new TTreeReaderArray<Double_t>(myReader, "TrackerEndcapHits.position.x");
    disks_si_y2 = new TTreeReaderArray<Double_t>(myReader, "TrackerEndcapHits.position.y");
    disks_si_z2 = new TTreeReaderArray<Double_t>(myReader, "TrackerEndcapHits.position.z");
    disks_si_quality = new TTreeReaderArray<Int_t>(myReader, "TrackerEndcapHits.quality");
    // ETOF Hits
    endcap_tof_x1 = new TTreeReaderArray<Double_t>(myReader, "TOFEndcapHits.position.x");
    endcap_tof_y1 = new TTreeReaderArray<Double_t>(myReader, "TOFEndcapHits.position.y");
    endcap_tof_z1 = new TTreeReaderArray<Double_t>(myReader, "TOFEndcapHits.position.z");
    endcap_etof_quality = new TTreeReaderArray<Int_t>(myReader, "TOFEndcapHits.quality");
    // MPGD
    barrel_mm_x2 = new TTreeReaderArray<Double_t>(myReader, "MPGDBarrelHits.position.x");
    barrel_mm_y2 = new TTreeReaderArray<Double_t>(myReader, "MPGDBarrelHits.position.y");
    barrel_mm_z2 = new TTreeReaderArray<Double_t>(myReader, "MPGDBarrelHits.position.z");
    barrel_mm_quality = new TTreeReaderArray<Int_t>(myReader, "MPGDBarrelHits.quality");
    // TOFBarHits
    barrel_tof_x = new TTreeReaderArray<Double_t>(myReader, "TOFBarrelHits.position.x");
    barrel_tof_y = new TTreeReaderArray<Double_t>(myReader, "TOFBarrelHits.position.y");
    barrel_tof_z = new TTreeReaderArray<Double_t>(myReader, "TOFBarrelHits.position.z");
    barrel_tof_quality = new TTreeReaderArray<Int_t>(myReader, "TOFBarrelHits.quality");
    // PFRICHHits
    pfRICH_x = new TTreeReaderArray<Double_t>(myReader, "PFRICHHits.position.x");
    pfRICH_y = new TTreeReaderArray<Double_t>(myReader, "PFRICHHits.position.y");
    pfRICH_z = new TTreeReaderArray<Double_t>(myReader, "PFRICHHits.position.z");
    pfRICH_quality = new TTreeReaderArray<Int_t>(myReader, "PFRICHHits.quality");


    gROOT->LoadMacro("MultiView.C+");
    // run_epic_display();

}

//==============================================================================
// Main - pythia_display()
//------------------------------------------------------------------------------

void run_epic_display()
{
    //========================================================================
    // Create views and containers.
    //========================================================================
    Bool_t enable_rectrack_draw = kFALSE;
    Bool_t enable_hit_draw = kTRUE;
    Bool_t drawname = kFALSE;
    Bool_t draw_transparent = false;
    Bool_t draw_full_geo = true;
    // set first node = 0 and end node = -1 for full geometry if draw_full_geo set as false
    int startnode = 3; int endnode = 9;

    TString rootfile="Recolored_detector.root"; // DD4HEP EPIC Tracker
    TEveManager::Create();
    gGeoManager = TGeoManager::Import(rootfile); // or use TGeoManager::Import(rootfile)
    if (gGeoManager == nullptr) return;


    // Set Transparency of Detectors
    if (draw_transparent){
        TObjArray* allvolumes = gGeoManager->GetListOfVolumes();
        for(Int_t i=0; i<allvolumes->GetEntries();i++){
            TGeoVolume* vol= (TGeoVolume*)allvolumes->At(i);
            TString volname = vol->GetIconName();
            if (volname.Contains("SVTX")) vol->SetLineColor(kRed);
            else if (volname.Contains("BARR")) vol->SetLineColor(kBlue);
            vol->SetTransparency(99); // set the transparency level for the volumes
        }
    }

    // TopNode has many nodes access this way
    TObjArray* allnodes = gGeoManager->GetTopNode()->GetNodes();
    const int nNodes = allnodes->GetEntries();
    if (draw_full_geo){
        TEveGeoTopNode *EPIC_Tracker = new TEveGeoTopNode(gGeoManager,gGeoManager->GetTopNode());
        EPIC_Tracker->SetVisLevel(100);
        cout << "Number of nodes in the geometry: " << nNodes << endl;
        gEve->AddGlobalElement(EPIC_Tracker);
    }
    else{
        for(Int_t i=startnode; i<endnode;i++){
            TGeoNode* node= (TGeoNode*)allnodes->At(i);
            TString volname = node->GetVolume()->GetName();
            TEveGeoTopNode *evenode = new TEveGeoTopNode(gGeoManager,node);
            evenode->SetVisLevel(5);
            gEve->AddGlobalElement(evenode);
        }
    }

    gEve->FullRedraw3D(kTRUE);
    gEve->GetBrowser()->GetTabRight()->SetTab(1);

    TEveProjectionManager *fRPhiProjManager = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    TEveProjectionManager *fRhoZProjManager = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    gEve->AddToListTree(fRPhiProjManager, kFALSE);
    gEve->AddToListTree(fRhoZProjManager, kFALSE);
    TEveProjectionAxes *fAxesPhi = new TEveProjectionAxes(fRPhiProjManager);
    TEveProjectionAxes *fAxesRho = new TEveProjectionAxes(fRhoZProjManager);

    TEveWindowSlot *RPhiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack *RPhiPack = RPhiSlot->MakePack();
    RPhiPack->SetElementName("RPhi View");
    RPhiPack->SetShowTitleBar(kFALSE);
    RPhiPack->NewSlot()->MakeCurrent();
    TEveViewer *fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
    TEveScene *fRPhiScene = gEve->SpawnNewScene("RPhi", "Scene holding axis.");
    fRPhiScene->AddElement(fAxesPhi);

    TEveWindowSlot *RhoZSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack *RhoZPack = RhoZSlot->MakePack();
    RhoZPack->SetElementName("RhoZ View");
    RhoZPack->SetShowTitleBar(kFALSE);
    RhoZPack->NewSlot()->MakeCurrent();
    TEveViewer *fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
    TEveScene *fRhoZScene = gEve->SpawnNewScene("RhoZ", "Scene holding axis.");
    fRhoZScene->AddElement(fAxesRho);

    SetViewers(fRPhiView, fRhoZView,fRPhiScene,fRhoZScene);


    gTrackList = new TEveTrackList("MCTracks");
    gTrackList->SetMainColor(kMagenta);
    gTrackList->SetLineColor(kMagenta);
    gTrackList->SetMarkerColor(kMagenta);
    gTrackList->SetMarkerStyle(4);
    gTrackList->SetMarkerSize(0.5);
    gEve->AddElement(gTrackList);
    //-----Array of Hits----------
    for (int ilayer=0; ilayer<6; ilayer++){
        hits = new TEvePointSetArray(Form("EPICHits%d",ilayer));
        hits->SetMarkerColor(kGreen);
        hits->SetMarkerStyle(47);
        hits->SetMarkerSize(1.4);
        gEve->AddElement(hits);
    }

    TEveTrackPropagator* trkProp = gTrackList->GetPropagator();
    trkProp->SetFitDaughters(kFALSE);
    trkProp->SetMaxZ(300);
    trkProp->SetMaxR(300);
    trkProp->SetMagField(0.,0.,1.7); // 3.0 Tesla in Z-direction
    trkProp->SetMaxStep(25.0);

    //========================================================================
    //========================================================================

    epic_make_gui();
    epic_load_event();

    gEve->Redraw3D(kTRUE);
}


//==============================================================================
// Next event
//------------------------------------------------------------------------------

void epic_load_event()
{
    cout<<"Event No. "<<iEvent<<endl;
    myReader.SetEntry(iEvent);
    gTrackList->DestroyElements();
    hits->DestroyElements();

    Int_t propagator = 1; // 0 default, 1 RungeKutta, 2 kHelix
    TEveTrackPropagator *trkProp = gTrackList->GetPropagator(); //default propogator
    if (propagator==1)
    {
        trkProp->SetStepper(TEveTrackPropagator::kRungeKutta);
        gTrackList->SetName("RK Propagator");
    }
    else if (propagator==2)
    {
        trkProp->SetStepper(TEveTrackPropagator::kHelix);
        gTrackList->SetName("Heix Propagator");
    }

    bool flag = true;
    for (int iParticle = 0; iParticle < charge.GetSize(); ++iParticle)
    {
        if ((status[iParticle]!=1)) continue;
        rc = new TEveRecTrackD(); // Reconstructed Track
        rc->fV.Set(vx_mc[iParticle],vy_mc[iParticle],vz_mc[iParticle]);
        rc->fP.Set(px_mc[iParticle],py_mc[iParticle],pz_mc[iParticle]);
        rc->fSign = charge[iParticle];

        Double_t prec = sqrt(px_mc[iParticle]*px_mc[iParticle]+py_mc[iParticle]*py_mc[iParticle]+pz_mc[iParticle]*pz_mc[iParticle]);
        Double_t etarec = -1.0*TMath::Log(TMath::Tan((TMath::ACos(pz_mc[iParticle]/prec))/2));
        // if (etarec<3.0 || etarec>3.5) continue;
        // if (prec>1.0) flag = true;
        track = new TEveTrack(rc, trkProp);
        // track->SetRnrPoints(kTRUE);
        track->SetMarkerStyle(4);
        track->SetLineWidth(2);
        track->SetLineColor(kMagenta);
        track->SetName(Form("Track_No%d",iParticle));
        track->SetElementTitle(Form("Event =%d \n" "TrackNo.=%d,"
                                    "P  = (%.3f, %.3f, %.3f)\n" "P  = %.3f GeV/c \n" "Eta =%.3f \n" "Pt =%.3f \n"
                ,iEvent,iParticle, px_mc[iParticle],py_mc[iParticle],pz_mc[iParticle],prec,etarec,sqrt(px_mc[iParticle]*px_mc[iParticle]+py_mc[iParticle]*py_mc[iParticle])));
        gTrackList->AddElement(track);
    }
    gTrackList->MakeTracks();
    gTrackList->SetLineColor(kMagenta);
    TEvePointSet *hits_EPIC[7];
    if (flag){
        hits->AddElement(AddHits(vtx_si_x, vtx_si_y, vtx_si_z, vtx_si_quality, hits_EPIC[0],"VTX Hits"));
        hits->AddElement(AddHits(barrel_si_x1, barrel_si_y1, barrel_si_z1, barrel_si_quality, hits_EPIC[1],"BARREL Hits"));
        hits->AddElement(AddHits(disks_si_x2, disks_si_y2, disks_si_z2, disks_si_quality, hits_EPIC[2],"Si Disks Hits"));
        hits->AddElement(AddHits(endcap_tof_x1, endcap_tof_y1, endcap_tof_z1, endcap_etof_quality, hits_EPIC[3],"ETOF Hits"));
        hits->AddElement(AddHits(barrel_mm_x2, barrel_mm_y2, barrel_mm_z2, barrel_mm_quality, hits_EPIC[4],"Barrel MPGD Hits"));
        hits->AddElement(AddHits(barrel_tof_x, barrel_tof_y, barrel_tof_z, barrel_tof_quality, hits_EPIC[5],"Barrel TOF Hits"));
        hits->AddElement(AddHits(pfRICH_x, pfRICH_y, pfRICH_z, pfRICH_quality, hits_EPIC[6],"pfRICH Hits"));
    }

    TGLViewer  *viewer = gEve->GetDefaultGLViewer();
    viewer->GetClipSet()->SetClipType(TGLClip::kClipNone); // Other options are kClipNone = 0, kClipPlane, kClipBox
    Double_t center[3] ={0.,0.,0.};
    Double_t *cent; cent = center;
    viewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOX,2.0, 1.0, cent,1.0,0.); // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY,
    // viewer->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
    viewer->RefreshPadEditor(viewer);
    //viewer->CurrentCamera().RotateRad(-0.5, 0.5);
    viewer->DoDraw();

    TEveElement* top = static_cast<TEveElement*>(gEve->GetEventScene());
    gEve->Redraw3D();
}


//==============================================================================
// GUI stuff
//------------------------------------------------------------------------------
class EventHandler
{
public:
    void Fwd()
    {
        ++iEvent;
        epic_load_event();
    }
    void Bck()
    {
        if(iEvent>0) {
            --iEvent;
            epic_load_event();
        }
    }
    void GotoEvent()
    {
        if (!num) cout<<"GotoEvent: Event Not Found !!! "<<endl;
        iEvent = num->GetIntNumber();
        if (iEvent>=0)
            epic_load_event();
    }
};

//______________________________________________________________________________
void epic_make_gui()
{
    // Create minimal GUI for event navigation.

    TEveBrowser* browser = gEve->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);

    TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
    {

        TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
        TGPictureButton* b = 0;
        Int_t width = 30;


        EventHandler *fh = new EventHandler;

        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
        // b->SetEnabled(kFALSE);
        b->SetToolTipText("Go to previous event - not supported.");
        hf->AddFrame(b);
        b->Connect("Clicked()", "EventHandler", fh, "Bck()");

        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
        b->SetToolTipText("Generate new event.");
        hf->AddFrame(b);
        b->Connect("Clicked()", "EventHandler", fh, "Fwd()");


        num = new TGNumberEntry(hf, 0, 5, 1,TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax, 0, 10000);
        hf->AddFrame(num, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,5,5));

        TGTextButton *btext = new TGTextButton(hf, "Go to Event", 100);
        hf->AddFrame(btext, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,5,5));
        btext->Connect("Clicked()", "EventHandler", fh, "GotoEvent()");//Form("GotoEvent(%ld)",num->GetIntNumber()));

    }
    frmMain->AddFrame(hf);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);
}


TEvePointSet* AddHits(TTreeReaderArray<Double_t> *xhit,TTreeReaderArray<Double_t> *yhit,TTreeReaderArray<Double_t> *zhit, TTreeReaderArray<Int_t> *quality, TEvePointSet *hits, TString name ="")
{
    // Add Hits to vertex
    Int_t nhits = xhit->GetSize();
    hits = new TEvePointSet(nhits);
    for (Int_t hitno=0; hitno<nhits; ++hitno){// Reading hits
        Int_t hitquality = quality->At(hitno);
        // if (hitquality!=0) continue; // Keep Primary Hits
        hits->SetPoint(hitno,xhit->At(hitno)*0.1,yhit->At(hitno)*0.1,zhit->At(hitno)*0.1);
        hits->SetName(Form("Hits_%s",name.Data()));
        hits->SetMarkerColor(kGreen);
        hits->SetMarkerStyle(47);
        hits->SetMarkerSize(1.4);
    }
    return hits;
}

void SetViewers(TEveViewer *RPhi, TEveViewer *RhoZ, TEveScene *fRPhiScene, TEveScene *fRhoZScene)
{

    RPhi->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    // set clip plane and camera parameters
    // RPhi->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
    // RPhi->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, fRPhiPlane);
    RPhi->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
    RPhi->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
    // switch off left, right, top and bottom light sources
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);

    RhoZ->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    // set clip plane and camera parameters
    // RhoZ->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
    // RhoZ->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, fRhoZPlane);
    RhoZ->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
    RhoZ->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
    // switch off left, right and front light sources
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);

    RPhi->AddScene(fRPhiScene);
    RPhi->AddScene(gEve->GetGlobalScene());
    RPhi->AddScene(gEve->GetEventScene());
    RhoZ->AddScene(fRhoZScene);
    RhoZ->AddScene(gEve->GetGlobalScene());
    RhoZ->AddScene(gEve->GetEventScene());

}


#endif

//main function
void Draw_model(const bool event=false)
{
    epic_display();
    TEveManager::Create();
    gGeoManager = TGeoManager::Import("Recolored_detector.root");

    //Default colors
    //gGeoManager->DefaultColors();

    if (gGeoManager == nullptr) return;

    //Vector of strings
    std::vector<TString> nodes;
    Bool_t draw_transparent = false;
    Bool_t draw_event = true;

    // Set Transparency of Detectors
    if (draw_transparent){
        TObjArray* allvolumes = gGeoManager->GetListOfVolumes();
        for(Int_t i=0; i<allvolumes->GetEntries();i++){
            TGeoVolume* vol= (TGeoVolume*)allvolumes->At(i);
            TString volname = vol->GetIconName();
            vol->SetTransparency(80); // set the transparency level for the volumes
        }
    }

    TEveGeoTopNode *EPIC_Tracker = new TEveGeoTopNode(gGeoManager,gGeoManager->GetTopNode());
    EPIC_Tracker->SetVisLevel(10);
    gEve->AddGlobalElement(EPIC_Tracker);

    // To draw the event
    if (event) {
    gEve->FullRedraw3D(kTRUE);
    gEve->GetBrowser()->GetTabRight()->SetTab(1);

    TEveProjectionManager *fRPhiProjManager = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    TEveProjectionManager *fRhoZProjManager = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    gEve->AddToListTree(fRPhiProjManager, kFALSE);
    gEve->AddToListTree(fRhoZProjManager, kFALSE);
    TEveProjectionAxes *fAxesPhi = new TEveProjectionAxes(fRPhiProjManager);
    TEveProjectionAxes *fAxesRho = new TEveProjectionAxes(fRhoZProjManager);

    TEveWindowSlot *RPhiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack *RPhiPack = RPhiSlot->MakePack();
    RPhiPack->SetElementName("RPhi View");
    RPhiPack->SetShowTitleBar(kFALSE);
    RPhiPack->NewSlot()->MakeCurrent();
    TEveViewer *fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
    TEveScene *fRPhiScene = gEve->SpawnNewScene("RPhi", "Scene holding axis.");
    fRPhiScene->AddElement(fAxesPhi);

    TEveWindowSlot *RhoZSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack *RhoZPack = RhoZSlot->MakePack();
    RhoZPack->SetElementName("RhoZ View");
    RhoZPack->SetShowTitleBar(kFALSE);
    RhoZPack->NewSlot()->MakeCurrent();
    TEveViewer *fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
    TEveScene *fRhoZScene = gEve->SpawnNewScene("RhoZ", "Scene holding axis.");
    fRhoZScene->AddElement(fAxesRho);

    SetViewers(fRPhiView, fRhoZView,fRPhiScene,fRhoZScene);


    gTrackList = new TEveTrackList("MCTracks");
    gTrackList->SetMainColor(kMagenta);
    gTrackList->SetLineColor(kMagenta);
    gTrackList->SetMarkerColor(kMagenta);
    gTrackList->SetMarkerStyle(4);
    gTrackList->SetMarkerSize(0.5);
    gEve->AddElement(gTrackList);
    //-----Array of Hits----------
    for (int ilayer=0; ilayer<6; ilayer++){
        hits = new TEvePointSetArray(Form("EPICHits%d",ilayer));
        hits->SetMarkerColor(kGreen);
        hits->SetMarkerStyle(47);
        hits->SetMarkerSize(1.4);
        gEve->AddElement(hits);
    }

    //gEve->SetVisLevel(4);
    TEveTrackPropagator* trkProp = gTrackList->GetPropagator();
    trkProp->SetFitDaughters(kFALSE);
    trkProp->SetMaxZ(300);
    trkProp->SetMaxR(300);
    trkProp->SetMagField(0.,0.,1.7); // 3.0 Tesla in Z-direction
    trkProp->SetMaxStep(25.0);

    //========================================================================
    //========================================================================


        epic_make_gui();
        epic_load_event();
    }

    gEve->Redraw3D(kTRUE);

    TGLViewer  *viewer = gEve->GetDefaultGLViewer();
    // Clip geometry    // Ax+By+CZ+D = 0 first 4 entry of data are A,B,C,D rest for the clipbox options
    double data[4] = {1.0, 0.0, 0.0, 0.0};
    //viewer->GetClipSet()->SetClipType(TGLClip::EType(1)); // Other options are kClipNone = 0, kClipPlane, kClipBox
    //viewer->GetClipSet()->SetClipType(TGLClip::EType(1));
    //viewer->GetClipSet()->SetClipState(TGLClip::EType(1),data);

    //Box cut
    // viewer->GetClipSet()->SetClipType(TGLClip::kClipBox);
    // viewer->GetClipSet()->SetClipState(TGLClip::kClipBox,data);
    // viewer->GetClipSet()->SetClipType(TGLClip::EType(1),data);
    // viewer->GetClipSet()->SetClipState(TGLClip::kClipBox,data);

    //Double_t center[3] ={0.,0.,0.};
    //Double_t *cent; cent = center;
    //viewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOX,2.0, 1.0, cent,1.0,0.); // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY,
    //viewer->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
    //viewer->RefreshPadEditor(viewer);
    //viewer->CurrentCamera().RotateRad(-0.6,3.14+0.6);
    viewer->DoDraw();


    //Command to save the picture
    //////gEve->GetDefaultGLViewer()->SavePicture("test.png");


}




