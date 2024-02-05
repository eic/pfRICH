// Macro for reading xml file containing color and transparency of volumes
// tinyxml2 library is borrowed from https://github.com/leethomason/tinyxml2
// Ondrej Lomicky, lomicond@fjfi.cvut.cz

//run
//root Recoloring.C\(\"detector_geometry.root","detector_colors.xml","Recolored_detector.root\"\)
//to recolor geometry and change transparency
//final geometry is saved in Recolored_detector.root


#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include <iostream>
#include <vector>
#include "Rtypes.h"  // Pro Color_t
#include "TEveManager.h"
#include <fstream>
#include <string>
#include <TString.h>
#include <TGeoManager.h>

//Struct for storing color and transparency of volume
struct PartColor {
    std::string detectorName;
    std::string partName;
    Color_t color;
    double transparency;
};

//Function for converting string to color
//It is not possible to easily transform string to Color_t so I have to create this function
Color_t TStringToColor(const TString& colorString) {
    std::map<TString, Color_t> ColorMap = {{"kWhite", kWhite}, {"kWhite-10", kWhite-10}, {"kWhite-9", kWhite-9}, {"kWhite-8", kWhite-8}, {"kWhite-7", kWhite-7}, {"kWhite-6", kWhite-6}, {"kWhite-5", kWhite-5}, {"kWhite-4", kWhite-4}, {"kWhite-3", kWhite-3}, {"kWhite-2", kWhite-2}, {"kWhite-1", kWhite-1}, {"kWhite+0", kWhite+0}, {"kWhite+1", kWhite+1}, {"kWhite+2", kWhite+2}, {"kWhite+3", kWhite+3}, {"kWhite+4", kWhite+4}, {"kWhite+5", kWhite+5}, {"kWhite+6", kWhite+6}, {"kWhite+7", kWhite+7}, {"kWhite+8", kWhite+8}, {"kWhite+9", kWhite+9}, {"kWhite+10", kWhite+10}, {"kBlack", kBlack}, {"kBlack-10", kBlack-10}, {"kBlack-9", kBlack-9}, {"kBlack-8", kBlack-8}, {"kBlack-7", kBlack-7}, {"kBlack-6", kBlack-6}, {"kBlack-5", kBlack-5}, {"kBlack-4", kBlack-4}, {"kBlack-3", kBlack-3}, {"kBlack-2", kBlack-2}, {"kBlack-1", kBlack-1}, {"kBlack+0", kBlack+0}, {"kBlack+1", kBlack+1}, {"kBlack+2", kBlack+2}, {"kBlack+3", kBlack+3}, {"kBlack+4", kBlack+4}, {"kBlack+5", kBlack+5}, {"kBlack+6", kBlack+6}, {"kBlack+7", kBlack+7}, {"kBlack+8", kBlack+8}, {"kBlack+9", kBlack+9}, {"kBlack+10", kBlack+10}, {"kGray", kGray}, {"kGray-10", kGray-10}, {"kGray-9", kGray-9}, {"kGray-8", kGray-8}, {"kGray-7", kGray-7}, {"kGray-6", kGray-6}, {"kGray-5", kGray-5}, {"kGray-4", kGray-4}, {"kGray-3", kGray-3}, {"kGray-2", kGray-2}, {"kGray-1", kGray-1}, {"kGray+0", kGray+0}, {"kGray+1", kGray+1}, {"kGray+2", kGray+2}, {"kGray+3", kGray+3}, {"kGray+4", kGray+4}, {"kGray+5", kGray+5}, {"kGray+6", kGray+6}, {"kGray+7", kGray+7}, {"kGray+8", kGray+8}, {"kGray+9", kGray+9}, {"kGray+10", kGray+10}, {"kRed", kRed}, {"kRed-10", kRed-10}, {"kRed-9", kRed-9}, {"kRed-8", kRed-8}, {"kRed-7", kRed-7}, {"kRed-6", kRed-6}, {"kRed-5", kRed-5}, {"kRed-4", kRed-4}, {"kRed-3", kRed-3}, {"kRed-2", kRed-2}, {"kRed-1", kRed-1}, {"kRed+0", kRed+0}, {"kRed+1", kRed+1}, {"kRed+2", kRed+2}, {"kRed+3", kRed+3}, {"kRed+4", kRed+4}, {"kRed+5", kRed+5}, {"kRed+6", kRed+6}, {"kRed+7", kRed+7}, {"kRed+8", kRed+8}, {"kRed+9", kRed+9}, {"kRed+10", kRed+10}, {"kGreen", kGreen}, {"kGreen-10", kGreen-10}, {"kGreen-9", kGreen-9}, {"kGreen-8", kGreen-8}, {"kGreen-7", kGreen-7}, {"kGreen-6", kGreen-6}, {"kGreen-5", kGreen-5}, {"kGreen-4", kGreen-4}, {"kGreen-3", kGreen-3}, {"kGreen-2", kGreen-2}, {"kGreen-1", kGreen-1}, {"kGreen+0", kGreen+0}, {"kGreen+1", kGreen+1}, {"kGreen+2", kGreen+2}, {"kGreen+3", kGreen+3}, {"kGreen+4", kGreen+4}, {"kGreen+5", kGreen+5}, {"kGreen+6", kGreen+6}, {"kGreen+7", kGreen+7}, {"kGreen+8", kGreen+8}, {"kGreen+9", kGreen+9}, {"kGreen+10", kGreen+10}, {"kBlue", kBlue}, {"kBlue-10", kBlue-10}, {"kBlue-9", kBlue-9}, {"kBlue-8", kBlue-8}, {"kBlue-7", kBlue-7}, {"kBlue-6", kBlue-6}, {"kBlue-5", kBlue-5}, {"kBlue-4", kBlue-4}, {"kBlue-3", kBlue-3}, {"kBlue-2", kBlue-2}, {"kBlue-1", kBlue-1}, {"kBlue+0", kBlue+0}, {"kBlue+1", kBlue+1}, {"kBlue+2", kBlue+2}, {"kBlue+3", kBlue+3}, {"kBlue+4", kBlue+4}, {"kBlue+5", kBlue+5}, {"kBlue+6", kBlue+6}, {"kBlue+7", kBlue+7}, {"kBlue+8", kBlue+8}, {"kBlue+9", kBlue+9}, {"kBlue+10", kBlue+10}, {"kYellow", kYellow}, {"kYellow-10", kYellow-10}, {"kYellow-9", kYellow-9}, {"kYellow-8", kYellow-8}, {"kYellow-7", kYellow-7}, {"kYellow-6", kYellow-6}, {"kYellow-5", kYellow-5}, {"kYellow-4", kYellow-4}, {"kYellow-3", kYellow-3}, {"kYellow-2", kYellow-2}, {"kYellow-1", kYellow-1}, {"kYellow+0", kYellow+0}, {"kYellow+1", kYellow+1}, {"kYellow+2", kYellow+2}, {"kYellow+3", kYellow+3}, {"kYellow+4", kYellow+4}, {"kYellow+5", kYellow+5}, {"kYellow+6", kYellow+6}, {"kYellow+7", kYellow+7}, {"kYellow+8", kYellow+8}, {"kYellow+9", kYellow+9}, {"kYellow+10", kYellow+10}, {"kMagenta", kMagenta}, {"kMagenta-10", kMagenta-10}, {"kMagenta-9", kMagenta-9}, {"kMagenta-8", kMagenta-8}, {"kMagenta-7", kMagenta-7}, {"kMagenta-6", kMagenta-6}, {"kMagenta-5", kMagenta-5}, {"kMagenta-4", kMagenta-4}, {"kMagenta-3", kMagenta-3}, {"kMagenta-2", kMagenta-2}, {"kMagenta-1", kMagenta-1}, {"kMagenta+0", kMagenta+0}, {"kMagenta+1", kMagenta+1}, {"kMagenta+2", kMagenta+2}, {"kMagenta+3", kMagenta+3}, {"kMagenta+4", kMagenta+4}, {"kMagenta+5", kMagenta+5}, {"kMagenta+6", kMagenta+6}, {"kMagenta+7", kMagenta+7}, {"kMagenta+8", kMagenta+8}, {"kMagenta+9", kMagenta+9}, {"kMagenta+10", kMagenta+10}, {"kCyan", kCyan}, {"kCyan-10", kCyan-10}, {"kCyan-9", kCyan-9}, {"kCyan-8", kCyan-8}, {"kCyan-7", kCyan-7}, {"kCyan-6", kCyan-6}, {"kCyan-5", kCyan-5}, {"kCyan-4", kCyan-4}, {"kCyan-3", kCyan-3}, {"kCyan-2", kCyan-2}, {"kCyan-1", kCyan-1}, {"kCyan+0", kCyan+0}, {"kCyan+1", kCyan+1}, {"kCyan+2", kCyan+2}, {"kCyan+3", kCyan+3}, {"kCyan+4", kCyan+4}, {"kCyan+5", kCyan+5}, {"kCyan+6", kCyan+6}, {"kCyan+7", kCyan+7}, {"kCyan+8", kCyan+8}, {"kCyan+9", kCyan+9}, {"kCyan+10", kCyan+10}, {"kOrange", kOrange}, {"kOrange-10", kOrange-10}, {"kOrange-9", kOrange-9}, {"kOrange-8", kOrange-8}, {"kOrange-7", kOrange-7}, {"kOrange-6", kOrange-6}, {"kOrange-5", kOrange-5}, {"kOrange-4", kOrange-4}, {"kOrange-3", kOrange-3}, {"kOrange-2", kOrange-2}, {"kOrange-1", kOrange-1}, {"kOrange+0", kOrange+0}, {"kOrange+1", kOrange+1}, {"kOrange+2", kOrange+2}, {"kOrange+3", kOrange+3}, {"kOrange+4", kOrange+4}, {"kOrange+5", kOrange+5}, {"kOrange+6", kOrange+6}, {"kOrange+7", kOrange+7}, {"kOrange+8", kOrange+8}, {"kOrange+9", kOrange+9}, {"kOrange+10", kOrange+10}, {"kSpring", kSpring}, {"kSpring-10", kSpring-10}, {"kSpring-9", kSpring-9}, {"kSpring-8", kSpring-8}, {"kSpring-7", kSpring-7}, {"kSpring-6", kSpring-6}, {"kSpring-5", kSpring-5}, {"kSpring-4", kSpring-4}, {"kSpring-3", kSpring-3}, {"kSpring-2", kSpring-2}, {"kSpring-1", kSpring-1}, {"kSpring+0", kSpring+0}, {"kSpring+1", kSpring+1}, {"kSpring+2", kSpring+2}, {"kSpring+3", kSpring+3}, {"kSpring+4", kSpring+4}, {"kSpring+5", kSpring+5}, {"kSpring+6", kSpring+6}, {"kSpring+7", kSpring+7}, {"kSpring+8", kSpring+8}, {"kSpring+9", kSpring+9}, {"kSpring+10", kSpring+10}, {"kTeal", kTeal}, {"kTeal-10", kTeal-10}, {"kTeal-9", kTeal-9}, {"kTeal-8", kTeal-8}, {"kTeal-7", kTeal-7}, {"kTeal-6", kTeal-6}, {"kTeal-5", kTeal-5}, {"kTeal-4", kTeal-4}, {"kTeal-3", kTeal-3}, {"kTeal-2", kTeal-2}, {"kTeal-1", kTeal-1}, {"kTeal+0", kTeal+0}, {"kTeal+1", kTeal+1}, {"kTeal+2", kTeal+2}, {"kTeal+3", kTeal+3}, {"kTeal+4", kTeal+4}, {"kTeal+5", kTeal+5}, {"kTeal+6", kTeal+6}, {"kTeal+7", kTeal+7}, {"kTeal+8", kTeal+8}, {"kTeal+9", kTeal+9}, {"kTeal+10", kTeal+10}, {"kAzure", kAzure}, {"kAzure-10", kAzure-10}, {"kAzure-9", kAzure-9}, {"kAzure-8", kAzure-8}, {"kAzure-7", kAzure-7}, {"kAzure-6", kAzure-6}, {"kAzure-5", kAzure-5}, {"kAzure-4", kAzure-4}, {"kAzure-3", kAzure-3}, {"kAzure-2", kAzure-2}, {"kAzure-1", kAzure-1}, {"kAzure+0", kAzure+0}, {"kAzure+1", kAzure+1}, {"kAzure+2", kAzure+2}, {"kAzure+3", kAzure+3}, {"kAzure+4", kAzure+4}, {"kAzure+5", kAzure+5}, {"kAzure+6", kAzure+6}, {"kAzure+7", kAzure+7}, {"kAzure+8", kAzure+8}, {"kAzure+9", kAzure+9}, {"kAzure+10", kAzure+10}, {"kViolet", kViolet}, {"kViolet-10", kViolet-10}, {"kViolet-9", kViolet-9}, {"kViolet-8", kViolet-8}, {"kViolet-7", kViolet-7}, {"kViolet-6", kViolet-6}, {"kViolet-5", kViolet-5}, {"kViolet-4", kViolet-4}, {"kViolet-3", kViolet-3}, {"kViolet-2", kViolet-2}, {"kViolet-1", kViolet-1}, {"kViolet+0", kViolet+0}, {"kViolet+1", kViolet+1}, {"kViolet+2", kViolet+2}, {"kViolet+3", kViolet+3}, {"kViolet+4", kViolet+4}, {"kViolet+5", kViolet+5}, {"kViolet+6", kViolet+6}, {"kViolet+7", kViolet+7}, {"kViolet+8", kViolet+8}, {"kViolet+9", kViolet+9}, {"kViolet+10", kViolet+10}, {"kPink", kPink}, {"kPink-10", kPink-10}, {"kPink-9", kPink-9}, {"kPink-8", kPink-8}, {"kPink-7", kPink-7}, {"kPink-6", kPink-6}, {"kPink-5", kPink-5}, {"kPink-4", kPink-4}, {"kPink-3", kPink-3}, {"kPink-2", kPink-2}, {"kPink-1", kPink-1}, {"kPink+0", kPink+0}, {"kPink+1", kPink+1}, {"kPink+2", kPink+2}, {"kPink+3", kPink+3}, {"kPink+4", kPink+4}, {"kPink+5", kPink+5}, {"kPink+6", kPink+6}, {"kPink+7", kPink+7}, {"kPink+8", kPink+8}, {"kPink+9", kPink+9}, {"kPink+10", kPink+10}, { "0", 0 }, { "1", 1 }, { "2", 2 }, { "3", 3 }, { "4", 4 }, { "5", 5 }, { "6", 6 }, { "7", 7 }, { "8", 8 }, { "9", 9 }, { "10", 10 }, { "11", 11 }, { "12", 12 }, { "13", 13 }, { "14", 14 }, { "15", 15 }, { "16", 16 }, { "17", 17 }, { "18", 18 }, { "19", 19 }, { "20", 20 }, { "21", 21 }, { "22", 22 }, { "23", 23 }, { "24", 24 }, { "25", 25 }, { "26", 26 }, { "27", 27 }, { "28", 28 }, { "29", 29 }, { "30", 30 }, { "31", 31 }, { "32", 32 }, { "33", 33 }, { "34", 34 }, { "35", 35 }, { "36", 36 }, { "37", 37 }, { "38", 38 }, { "39", 39 }, { "40", 40 }, { "41", 41 }, { "42", 42 }, { "43", 43 }, { "44", 44 }, { "45", 45 }, { "46", 46 }, { "47", 47 }, { "48", 48 }, { "49", 49 }    };
    auto it = ColorMap.find(colorString);

    if (it != ColorMap.end()) {
        return it->second;
    } else {
        return kWhite;
    }
}

//Load colors from xml file
std::vector<PartColor> LoadColorsFromXml(const char* filename) {
    std::vector<PartColor> partColors;
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename) == tinyxml2::XML_SUCCESS) {
        tinyxml2::XMLElement* configElement = doc.FirstChildElement("config");
        if (configElement) {
            for (tinyxml2::XMLElement* detectorElement = configElement->FirstChildElement("detector"); detectorElement; detectorElement = detectorElement->NextSiblingElement("detector")) {
                const char* detectorName = detectorElement->Attribute("name");
                if (detectorName) {
                    for (tinyxml2::XMLElement* partElement = detectorElement->FirstChildElement("part"); partElement; partElement = partElement->NextSiblingElement("part")) {
                        const char* partName = partElement->Attribute("name");
                        const char* colorStr = partElement->Attribute("color");
                        const char* transparencyStr = partElement->Attribute("transparency");
                        if (partName && colorStr && transparencyStr) {
                            PartColor partColor;
                            partColor.detectorName = detectorName;
                            partColor.partName = partName;
                            partColor.color = TStringToColor(colorStr);
                            partColor.transparency = std::stod(transparencyStr);
                            partColors.push_back(partColor);
                        }
                    }
                }
            }
        }
    }

    return partColors;
}

//Recursive function for finding all subnodes and recoloring them
bool RECURSIVE( TString volName, TGeoNode * node, double tra, Color_t color, std::vector<string>& nodes){
    if (node->GetNdaughters() == 0) return false;
    for (Int_t i = 0; i < node->GetNdaughters(); i++) {
        TString NodaName = node->GetDaughter(i)->GetVolume()->GetName();
        string NodaName2 = node->GetDaughter(i)->GetVolume()->GetPointerName();
                if (NodaName == volName){
                    //cout << "found2" << endl;
                    TGeoVolume *vol = node->GetDaughter(i)->GetVolume();
                    if (std::find(nodes.begin(), nodes.end(), NodaName2) == nodes.end()) {
                        vol->SetLineColor(color);
                        if (tra != 100){
                        vol->SetTransparency(tra);
                        }
                        else
                        {
                            vol->SetVisibility(false);
                        }
                         nodes.push_back(vol->GetPointerName());
                        gEve->Redraw3D(kTRUE);

                    }
                }
        if(RECURSIVE(volName, node->GetDaughter(i), tra, color,nodes))
        {return true;
        }

            }
    return false;
    }


//Main function
void Recoloring(const char *inputFile = "detector_geometry.root",
                  const char* xmlFile = "detector_colors.xml",
                  const char* outputFile = "Recolored_detector.root")
                  {

    std::vector<PartColor> loadedColors = LoadColorsFromXml(xmlFile);
    if (loadedColors.empty()) return;

    //Vector of strings
    std::vector<string> nodes;

    TEveManager::Create();
    gGeoManager = TGeoManager::Import(inputFile);

    if (gGeoManager == nullptr) return;

    int u = 0;
    string name_check ="";
    for (const PartColor& partColor : loadedColors) {
        TGeoNode * noda = gGeoManager->GetTopNode();
        if (name_check != partColor.detectorName){
            name_check = partColor.detectorName;
            cout << "Searching for " << partColor.detectorName << endl;
            //Clean nodes
            nodes.clear();
        }
        for (Int_t k = 0; k < noda->GetNdaughters(); k++) {
            TString NodaName = noda->GetDaughter(k)->GetVolume()->GetName();
            if (NodaName.Contains("World")){
                NodaName = noda->GetDaughter(k)->GetDaughter(0)->GetVolume()->GetName();
            }
            if (NodaName == partColor.detectorName){
                RECURSIVE(partColor.partName, noda->GetDaughter(k), partColor.transparency, partColor.color, nodes);
            }
        }
        //Simple progress bar
        u++;
        cout << u << "/" << loadedColors.size() << endl;
    }



    TEveGeoTopNode *EPIC_Tracker = new TEveGeoTopNode(gGeoManager,gGeoManager->GetTopNode());

    EPIC_Tracker->SetVisLevel(100);
    gEve->AddGlobalElement(EPIC_Tracker);
    gEve->FullRedraw3D(kTRUE);

    TGLViewer  *viewer = gEve->GetDefaultGLViewer();
    viewer->GetClipSet()->SetClipType(TGLClip::EType(1));
    viewer->DoDraw();

    TFile *f = new TFile(outputFile,"RECREATE");
    f->cd();
    gGeoManager->Write();
    f->Close();

    return;
}
