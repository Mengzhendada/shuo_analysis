#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TAxis.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <fmt/core.h>
//#include <fmt/ostream.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void SHMS_PID_RunGroup_calcut(int RunGroup = 0){
  json j_cuts;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_cuts;
  }
  std::vector<double> cal_cut_low;
  cal_cut_low = j_cuts["SHMS"]["cal_cuts"].get<std::vector<double>>();
  int n_cuts = (int)cal_cut_low.size();
  std::vector<double> cer_cut;
  cer_cut = j_cuts["SHMS"]["HGC_cuts"].get<std::vector<double>>();

  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup <0)
      return;
  }
  RunGroup = RunGroup;
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json jout;
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
   pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(neg_D2.empty() || pos_D2.empty()){std::cout<<"This is not normal production rungroup"<<std::endl;}
  else{
    std::vector<std::string> files_neg,files_pos;
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     files_neg.push_back(rootfile_name);
    }
  
     ROOT::RDataFrame d_neg_raw("T",files_neg);
     auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
     auto d_neg_HMS_e = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
                            .Filter("H.cer.npeSum > 10")
                            .Filter("H.cal.etottracknorm >0.85")
                            .Filter("fEvtHdr.fEvtType == 4");
     auto h_coin_time = d_neg_HMS_e.Histo1D({"coin_time","coin_time;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
     int coin_peak_bin = h_coin_time->GetMaximumBin();
     double coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);
     auto d_neg = d_neg_HMS_e.Filter(
       [=](double coin_time){return std::abs(coin_time - coin_peak_center) < 2.0;},{"CTime,ePositronCoinTime_ROC2"});
    auto h_cal_e = d_neg.Histo1D({"","SHMS cal;E/P;counts",100,0.7,2},"P.cal.etottracknorm");
    int etottracknorm_peak_bin = h_cal_e->GetMaximumBin();
    
     
  }//if normal production runs
  std::ofstream outfile;
  std::string outfile_name = "results/pid/SHMS_cal_cuts_"+std::to_string(RunGroup)+".json";
  outfile.open(outfile_name.c_str());
  //outfile.open("results/HMS_PID_statistics.json",std::ofstream::out | std::ofstream::app);
  outfile<<jout.dump(4)<<std::endl;
}
