
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
 //std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
 std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
 //std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_100000.root";
 ROOT::RDataFrame d0("T",rootfile);
 auto d = d0;
 auto h = d.Histo1D({"","",100,0,100},"CTime.ePiCoinTime_ROC2");
 std::cout<<h->GetBinCenter(h->GetMaximumBin())<<std::endl;
 // //auto h = d.Histo1D({"","",100,-1,2},"H.cal.etottracknorm");
 auto h_raw = d.Histo1D({"","",100,0,100},"CTime.CoinTime_RAW_ROC2");
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
  c->Divide(2,1);
  c->cd(1);
 h->DrawCopy();
 c->cd(2);
 h_raw->DrawCopy();
 std::string c_name = "results/yield/check/learn_readroot_"+std::to_string(RunNumber)+".pdf";
 c->SaveAs(c_name.c_str());
  //}
  return 0;
}
