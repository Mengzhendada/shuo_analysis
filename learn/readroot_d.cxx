
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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int readroot_d(int RunNumber = 0){
  std::cout<<"Enter RunNumber"<<std::endl;
  std::cin>>RunNumber;
 std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
 ROOT::RDataFrame d0("T",rootfile);
 auto d = d0;
 auto h = d.Histo1D({"","",100,0,100},"CTime.ePiCoinTime_ROC2");
// //auto h = d.Histo1D({"","",100,-1,2},"H.cal.etottracknorm");
//int readroot_d(int RunNumber = 0){
//  while(RunNumber >-1){
//    std::cout<<"Enter RunNumber "<<std::endl;
//    std::cin>>RunNumber;
//    std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
// ROOT::RDataFrame d0("T",rootfile);
// auto d = d0;
// auto h = d.Histo1D({"","",100,0,100},"CTime.ePiCoinTime_ROC2");
// //auto h = d.Histo1D({"","",100,-1,1},"H.cal.etottracknorm");
// std::cout<<h->GetBinCenter(h->GetMaximumBin())<<std::endl;
 //std::cout<<h->GetMean()<<std::endl;
  auto *c = new TCanvas;
 h->DrawCopy();
 c->SaveAs("results/yield/check/learn_readroot_d.pdf");
  //}
  return 0;
}
