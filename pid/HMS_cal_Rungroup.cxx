#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TPaveText.h"
//#include "ROOT/RInterface.hxx"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//R__LOAD_LIBRARY()
#include "nlohmann/json.hpp"
using json = nlohmann::json;
  std::string goodTrackSHMS = "-10 < P.gtr.dp && P.gtr.dp < 22";
  std::string goodTrackHMS = "-10 < H.gtr.dp && H.gtr.dp < 10";


void HMS_cal_Rungroup(int RunGroup = 0){
  std::vector<double> cal_cut_low,cal_cut_high;
  cal_cut_low = {0.6,0.65,0.7,0.75,0.8,0.85};
  cal_cut_high = {1.5,1.5,1.5,1.5,1.5,1.5};
  int n_cuts = (int)cal_cut_low.size();
  std::string cer_e_cut = "H.cer.npeSum > 10";
  std::string cer_pion_cut = "H.cer.npeSum < 0.1";
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup <0)
      return;
  }
  RunGroup = RunGroup*10;
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json k;
  std::vector<int> neg_D2,pos_D2; 
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
   pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(neg_D2.empty() || pos_D2.empty()){std::cout<<"This is not normal production rungroup"<<std::endl;}
  else{
    //TChain chain_neg("T");
    std::vector<std::string> files_neg,files_pos;
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     //chain_neg.Add(rootfile_name.c_str());
     files_neg.push_back(rootfile_name);
    }
   // TChain chain_pos("T");
  
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     files_pos.push_back(rootfile_name);
   //  chain_pos.Add(rootfile_name.c_str());
  }
   // ROOT::RDataFrame d_neg(chain_neg);
     ROOT::RDataFrame d_neg("T",files_neg);
     auto d_neg_raw_SHMS = d_neg.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    // auto d_neg_raw = d_neg_raw_SHMS.Filter(goodTrackHMS);
     //auto d_neg_e = d_neg_raw.Filter(cer_e_cut);
     //auto d_neg_pion = d_neg_raw.Filter(cer_pion_cut);
     ////ROOT::RDataFrame d_pos(chain_pos);
     // ROOT::RDataFrame d_pos("T",files_pos);
     //auto d_pos_raw = d_pos.Filter(goodTrackSHMS)
     //                      .Filter(goodTrackHMS)
     //                      ;
     //auto d_pos_e = d_pos_raw.Filter(cer_e_cut);
     //auto d_pos_pion = d_pos_raw.Filter(cer_pion_cut);
     
     auto n_neg_e = d_neg_raw_SHMS.Count();    
     //auto n_neg_e = d_neg_e.Count();    
     std::cout<<*n_neg_e<<std::endl;
     //    auto n_neg_pion = d_neg_pion.Count();
 //    auto n_pos_e = d_pos_e.Count();
 //    auto n_pos_pion = d_pos_pion.Count();

 //    k[(std::to_string(RunGroup)).c_str()]["neg"]["cer_e"] = *n_neg_e;
 //    k[(std::to_string(RunGroup)).c_str()]["neg"]["cer_pion"] = *n_neg_pion;
 //    k[(std::to_string(RunGroup)).c_str()]["pos"]["cer_e"] = *n_pos_e;
 //    k[(std::to_string(RunGroup)).c_str()]["pos"]["cer_pion"] = *n_pos_pion;
//
//     for(int i=0;i<n_cuts;++i){
//       std::string cut = std::to_string(cal_cut_low[i])+" < H.cal.etottracknorm "+" && H.cal.etottracknorm < "+std::to_string(cal_cut_high[i]);
//       auto d_neg_e_cut = d_neg_e.Filter(cut);
//       int neg_e_did = *d_neg_e_cut.Count();
//       k[(std::to_string(RunGroup)).c_str()]["neg"]["cer_e"][("cut"+std::to_string(i)).c_str()] = neg_e_did;
//       auto d_neg_pion_cut = d_neg_pion.Filter(cut);
//       int neg_pion_wrong = *d_neg_pion_cut.Count();
//       k[(std::to_string(RunGroup)).c_str()]["neg"]["cer_pion"][("cut"+std::to_string(i)).c_str()] = neg_pion_wrong;
//       auto d_pos_e_cut = d_pos_e.Filter(cut);
//       int pos_e_did = *d_pos_e_cut.Count();
//       k[(std::to_string(RunGroup)).c_str()]["pos"]["cer_e"][("cut"+std::to_string(i)).c_str()] = pos_e_did;
//       auto d_pos_pion_cut = d_pos_pion.Filter(cut);
//       int pos_pion_wrong = *d_pos_pion_cut.Count();
//       k[(std::to_string(RunGroup)).c_str()]["pos"]["cer_pion"][("cut"+std::to_string(i)).c_str()] = pos_pion_wrong;
//       
//     }

  }//if normal rungroup
  std::string outfile_name = "results/HMS_Cal_pid_"+std::to_string(RunGroup)+".json";
  std::ofstream out(outfile_name.c_str());
  out<<k.dump(4)<<std::endl;
}
