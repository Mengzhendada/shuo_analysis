
#include "nlohmann/json.hpp"
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//int readroot_d(){
// std::string rootfile = std::string("ROOTfiles/coin_replay_production_6291_6291.root");
// ROOT::RDataFrame d0("T",rootfile);
// auto d = d0;
// auto h = d.Histo1D({"","",100,40,50},"CTime.ePiCoinTime_ROC2");
// //auto h = d.Histo1D({"","",100,-1,2},"H.cal.etottracknorm");
int readroot_d(int RunNumber = 0){
  while(RunNumber >-1){
    std::cout<<"Enter RunNumber "<<std::endl;
    std::cin>>RunNumber;
    std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
 ROOT::RDataFrame d0("T",rootfile);
 auto d = d0;
 auto h = d.Histo1D({"","",100,0,100},"CTime.ePiCoinTime_ROC2");
 //auto h = d.Histo1D({"","",100,-1,1},"H.cal.etottracknorm");
 std::cout<<h->GetBinCenter(h->GetMaximumBin())<<std::endl;
 //std::cout<<h->GetMean()<<std::endl;
  auto *c = new TCanvas;
 h->DrawCopy();
 c->SaveAs("HMS_cal.pdf");
  }
  return 0;
}
