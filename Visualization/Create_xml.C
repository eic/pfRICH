// Macro for creating xml file defining color and transparency of volumes
// Ondrej Lomicky, lomicond@fjfi.cvut.cz

//run
//root Create_xml\(\"detector_geometry.root\"\)
//to create xml file
//Used colors have root names (kBlue, kAzure-3, 20, etc.)
//Transparency is set to 0.0 (not transparent) by default

//Libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TString.h>
#include <TGeoManager.h>

//Recursive function for finding all subnodes
void RECURSIVE( TGeoNode * node, double tra, TString color, std::ofstream& myfile,std::vector<TString> &nodes){
    if (node->GetNdaughters() == 0) return; //if there are no daughters, return
    for (Int_t i = 0; i < node->GetNdaughters(); i++) {
        TString NodaName = node->GetDaughter(i)->GetVolume()->GetName();
            if (std::find(nodes.begin(), nodes.end(), NodaName) == nodes.end()) {
                nodes.push_back(NodaName);
                myfile << "\t\t<part name=\"" << NodaName << "\" color=\""<<color<<"\" transparency=\""<<tra<<"\" />\n";
                RECURSIVE(node->GetDaughter(i), tra, color, myfile,nodes) ;
            }

    }
    return;
}

//Main function
void Create_xml(const char *inputFile = "detector_geometry.root")
{
    TEveManager::Create();
    gGeoManager = TGeoManager::Import(inputFile); //Import geometry

    //Default colors
    //gGeoManager->DefaultColors();

    if (gGeoManager == nullptr) return;

    //Default color
    TString color = "20";
    //Default transparency
    double tra = 0.0;

    //Vector of strings
    std::vector<TString> nodes;

    ofstream myfile;
    myfile.open ("detector_colors.xml"); //Open xml file

    myfile << "<config>\n";

    cout << "Nodes: " << endl;
    TGeoNode * noda = gGeoManager->GetTopNode();
        for (Int_t k = 0; k < noda->GetNdaughters(); k++) {
            TString NodaName = noda->GetDaughter(k)->GetVolume()->GetName();

            if(NodaName.Contains("World")) {
                NodaName = noda->GetDaughter(k)->GetDaughter(0)->GetVolume()->GetName();
                TGeoNode * noda2 = noda->GetDaughter(k)->GetDaughter(0);
                myfile << "    <detector name=\"" << NodaName << "\">\n";
                if (noda2->GetNdaughters() == 0){
                    myfile << "\t\t<part name=\"" << NodaName << "\" color=\""<<color<<"\" transparency=\""<<tra<<"\" />\n";
                }
                RECURSIVE(noda->GetDaughter(k)->GetDaughter(0), tra, color, myfile, nodes);
                myfile << "</detector>\n";
            } else {
            myfile << "    <detector name=\"" << NodaName << "\">\n";
            if (noda->GetNdaughters() == 0){
                myfile << "\t\t<part name=\"" << NodaName << "\" color=\""<<color<<"\" transparency=\""<<tra<<"\" />\n";
            }
            RECURSIVE(noda->GetDaughter(k), tra, color, myfile, nodes);
            myfile << "</detector>\n";
            }
            cout << NodaName << endl;
            //Clean vector
            nodes.clear();
        }
    myfile << "</config>\n";


  //Close the file
  myfile.close();

  //Close the manager
  TEveManager::Terminate();


}




