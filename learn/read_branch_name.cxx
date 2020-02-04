#include <iostream>
#include <fstream>
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"

void read_branch_name(){
  std::string root_name = "rootfiles/coin_replay_production_6482_20000.root";
  ROOT::RDataFrame d("T",root_name.c_str());
  auto colNames = d.GetColumnNames();
  std::ofstream of("name.txt");
  for (auto &&colName : colNames) of << colName << std::endl;
  std::cout<<"try"<<std::endl;
  //auto h1 = d.Filter("!P.hod.1x.negAdcPulseAmpRaw.empty()").Define("test","P.hod.1x.negAdcPulseAmpRaw[0]").Histo1D("test");
  //auto h2 = d.Histo1D("P.hod.1x.negAdcPulseAmpRaw");
   d.Define("test","P.hgcer.npeSum");
  auto h1 = d.Histo1D("test");
  //auto h1 = d.Histo1D("P.hgcer.npe[0]");
  auto h2 = d.Histo1D("P.hgcer.npeSum");
  TCanvas *c = new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  h1->DrawClone();
  c->cd(2);
  h2->DrawClone();
  c->SaveAs("test.pdf");
}
