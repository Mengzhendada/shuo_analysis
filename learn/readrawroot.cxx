#include "ROOT/RDataFrame.hxx"
#include <iostream>
#include "TCanvas.h"
int readrawroot(){
  std::string rootfile = std::string("ROOTfiles/coin_replay_production_7647_7647.root");
  ROOT::RDataFrame d("T",rootfile);
  //d.Define("P_cal_showertracknorm",[](double tot, double pr){return tot-pr;},{"P.cal.etottracknorm","P.cal.eprtracknorm"});
  auto h_1 = d.Histo1D({"","shms theta",100,-0.1,0.1},"P.gtr.th");
  //auto h_2 = d.Histo2D({"","",100,0.01,2,100,0.01,2},"P.cal.eprtracknorm","P_cal_showertracknorm");
  //d.Histo1D({"hmsgtrdp","hms goodtr dp",100,-14,14},"H.gtr.dp");
  std::cout<<"nentries "<<*d.Count()<<std::endl;
  auto *c = new TCanvas();
  h_1->DrawCopy();
  c->SaveAs("results/readrawroot.pdf");
  //auto *c2 = new TCanvas();
  //h_2->DrawCopy();
  //c2->SaveAs("results/readrawroot2.pdf");
  return 0;
}