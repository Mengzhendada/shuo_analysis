#include "ROOT/RDataFrame.hxx"
#include <iostream>
#include "TCanvas.h"
int readrawroot(){
  std::string rootfile = std::string("ROOTfiles/coin_replay_production_6068_6068.root");
  ROOT::RDataFrame d("T",rootfile);
  auto h_P_etottracknorm = d.Histo1D({"shmscal","etottracknorm",100,0.1,2},"P.cal.etottracknorm");
  //d.Histo1D({"hmsgtrdp","hms goodtr dp",100,-14,14},"H.gtr.dp");
  std::cout<<"nentries "<<*d.Count()<<std::endl;
  auto *c = new TCanvas;
  h_P_etottracknorm->DrawCopy();
  c->SaveAs("results/readrawroot.pdf");
  return 0;
}
