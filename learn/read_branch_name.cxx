#include <iostream>
#include <fstream>
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"

void read_branch_name(){
  std::string root_name = "ROOTfiles/coin_replay_production_7647_7647.root";
  ROOT::RDataFrame d("TSP",root_name.c_str());
  auto colNames = d.GetColumnNames();
  std::ofstream of("results/tsp_name.txt");
  for (auto &&colName : colNames) of << colName << std::endl;
  std::cout<<"try"<<std::endl;
  //auto h1 = d.Filter("!P.hod.1x.negAdcPulseAmpRaw.empty()").Define("test","P.hod.1x.negAdcPulseAmpRaw[0]").Histo1D("test");
  //auto h2 = d.Histo1D("P.hod.1x.negAdcPulseAmpRaw");
 //  auto d1 = d.Define("test","P.hgcer.npe");
 //  
 // std::cout<<"try1"<<std::endl;
 //  auto h1 = d1.Histo1D({"","",100,0,50},"test");
 // //auto h1 = d.Histo1D("P.hgcer.npe[0]");
 // std::cout<<"try2"<<std::endl;
 // auto h2 = d.Histo1D({"","",100,0,50},"P.hgcer.npeSum");
 // std::cout<<"try3"<<std::endl;
 // std::cout<<h1->GetEntries()<<std::endl;
 // TCanvas *c = new TCanvas("","",1900,1000);
 // c->Divide(1,2);
 // c->cd(1);
 // std::cout<<"try4"<<std::endl;
 // h1->DrawClone();
 // c->cd(2);
 // std::cout<<"try5"<<std::endl;
 // h2->DrawClone();
 // c->SaveAs("results/test.pdf");
}
