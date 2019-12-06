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
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void HMS_PID_RunGroup_cercut(int RunGroup = 0){
  json j_cuts;
  {
    std::ifstream ifs("db2/HMS_cuts.json");
    ifs>>j_cuts;
  }
  std::vector<double> cer_cut;
  cer_cut = j_cuts["cer_cuts"].get<std::vector<int>>();
  int n_cuts = (int)cer_cut.size();
  std::vector<int> coolcolor;
  coolcolor = j_cuts["coolcolor"].get<std::vector<int>>();

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
     auto d_neg = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
     int neg_cal_pion_calcut_n = j_cuts["cal_pi"].get<int>();
     std::string neg_cal_pion_calcut = "H.cal.etottracknorm < "+std::to_string(neg_cal_pion_calcut_n);
     auto d_neg_cal_pion = d_neg.Filter(neg_cal_pion_calcut);
     auto h_neg_pion_calcut = d_neg_cal_pion.Histo1D({"","",100,0,30},"H.cer.npeSum");
     int n_neg_pion_calcut = (int)h_neg_pion_calcut->GetEntries();
     jout[std::to_string(RunGroup)]["neg_cal"]["pion_all"] = n_neg_pion_calcut;
     int neg_cal_e_calcut_n = j_cuts["cal_e"].get<int>();
       std::string neg_cal_e_calcut = "H.cal.etottracknorm > "+std::to_string(neg_cal_e_calcut_n);
       auto d_neg_cal_e = d_neg.Filter(neg_cal_e_calcut);
     auto h_neg_e_calcut = d_neg_cal_e.Histo1D({"","",100,0,30},"H.cer.npeSum");
     auto h_neg_e_calcut_cal = d_neg.Histo1D({"","",100,0,30},"H.cal.etottracknorm");
     int n_neg_e_calcut = (int)h_neg_e_calcut->GetEntries();
     jout[std::to_string(RunGroup)]["neg_cal"]["e_all"] = n_neg_e_calcut;
     std::vector<double> n_neg_e_cer,n_neg_pion_cer,n_neg_e_cer_cal;
     for(int i = 0;i<n_cuts;++i){
       TAxis *axis_neg_e_cer = h_neg_e_calcut->GetXaxis();
       int bmin_neg_e_cer = axis_neg_e_cer->FindBin(cer_cut[i]);
       int n_neg_e_calcut_cer = (int)h_neg_e_calcut->Integral(bmin_neg_e_cer,100);
       std::cout<<i<<std::endl;
       std::cout<<"e "<<n_neg_e_calcut_cer<<std::endl;
       n_neg_e_cer.push_back(n_neg_e_calcut_cer);

      // auto h_neg_e_calcut_cal_cercut = d_neg
      // TAxis *axis_neg_e_cal = h_neg_e_calcut_cal->GetXaxis();
      // int bmin_neg_e_cer_cal = axis_neg_e_cal->FinBin(0

       TAxis *axis_neg_pion_cer = h_neg_pion_calcut->GetXaxis();
       int bmin_neg_pion_cer = axis_neg_pion_cer->FindBin(cer_cut[i]);
       int n_neg_pion_calcut_cer = (int)h_neg_pion_calcut->Integral(bmin_neg_pion_cer,100);
       std::cout<<"pion "<<n_neg_pion_calcut_cer<<std::endl;
       n_neg_pion_cer.push_back(n_neg_pion_calcut_cer);
     }
     jout[std::to_string(RunGroup)]["neg_cal"]["e"] = n_neg_e_cer;
     jout[std::to_string(RunGroup)]["neg_cal"]["pi"] = n_neg_pion_cer;
     
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     files_pos.push_back(rootfile_name);
    }
  
     ROOT::RDataFrame d_pos_raw("T",files_pos);
     auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
     auto d_pos = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
     int pos_cal_pion_calcut_n = j_cuts["cal_pi"].get<int>();
       std::string pos_cal_pion_calcut = "H.cal.etottracknorm < "+std::to_string(pos_cal_pion_calcut_n);
     auto d_pos_cal_pion = d_pos.Filter(pos_cal_pion_calcut);
     auto h_pos_pion_calcut = d_pos_cal_pion.Histo1D({"","",100,0,30},"H.cer.npeSum");
     int n_pos_pion_calcut = (int)h_pos_pion_calcut->GetEntries();
     jout[std::to_string(RunGroup)]["pos_cal"]["pion_all"] = n_pos_pion_calcut;
     int pos_cal_e_calcut_n = j_cuts["cal_e"].get<int>();
       std::string pos_cal_e_calcut = "H.cal.etottracknorm > "+std::to_string(pos_cal_e_calcut_n);
     auto d_pos_cal_e = d_pos.Filter(pos_cal_e_calcut);
     auto h_pos_e_calcut = d_pos_cal_e.Histo1D({"","",100,0,30},"H.cer.npeSum");
     auto h_pos_e_calcut_cal = d_pos.Histo1D({"","",100,0,30},"H.cal.etottracknorm");
     int n_pos_e_calcut = (int)h_pos_e_calcut->GetEntries();
     jout[std::to_string(RunGroup)]["pos_cal"]["e_all"] = n_pos_e_calcut;
     std::vector<double> n_pos_e_cer,n_pos_pion_cer,n_pos_e_cer_cal;
     for(int i = 0;i<n_cuts;++i){
       TAxis *axis_pos_e_cer = h_pos_e_calcut->GetXaxis();
       int bmin_pos_e_cer = axis_pos_e_cer->FindBin(cer_cut[i]);
       int n_pos_e_calcut_cer = (int)h_pos_e_calcut->Integral(bmin_pos_e_cer,100);
       std::cout<<i<<std::endl;
       std::cout<<"e "<<n_pos_e_calcut_cer<<std::endl;
       n_pos_e_cer.push_back(n_pos_e_calcut_cer);

      // auto h_pos_e_calcut_cal_cercut = d_pos
      // TAxis *axis_pos_e_cal = h_pos_e_calcut_cal->GetXaxis();
      // int bmin_pos_e_cer_cal = axis_pos_e_cal->FinBin(0

       TAxis *axis_pos_pion_cer = h_pos_pion_calcut->GetXaxis();
       int bmin_pos_pion_cer = axis_pos_pion_cer->FindBin(cer_cut[i]);
       int n_pos_pion_calcut_cer = (int)h_pos_pion_calcut->Integral(bmin_pos_pion_cer,100);
       std::cout<<"pion "<<n_pos_pion_calcut_cer<<std::endl;
       n_pos_pion_cer.push_back(n_pos_pion_calcut_cer);
     }
     jout[std::to_string(RunGroup)]["pos_cal"]["e"] = n_pos_e_cer;
     jout[std::to_string(RunGroup)]["pos_cal"]["pi"] = n_pos_pion_cer;
  }//if normal production runs
  std::ofstream outfile;
  std::string outfile_name = "results/pid/HMS_cer_cuts_"+std::to_string(RunGroup)+".json";
  outfile.open(outfile_name.c_str());
  //outfile.open("results/HMS_PID_statistics.json",std::ofstream::out | std::ofstream::app);
  outfile<<jout.dump(4)<<std::endl;
}
