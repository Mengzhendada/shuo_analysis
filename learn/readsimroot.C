#include "ROOT/RDataFrame.hxx"
#include <iostream>
#include "TCanvas.h"
int readsimroot(){
  std::string rootfile = std::string("/u/home/shuojia/simc_gfortran/worksim/csv6009.root");
  ROOT::RDataFrame d("h666",rootfile);
  auto h_P_etottracknorm = d.Histo1D({"hmsgtrdp","hms goodtr dp",100,-14,14},"hsdelta");

  auto *c = new TCanvas;
  h_P_etottracknorm->DrawCopy();

  return 0;
}
