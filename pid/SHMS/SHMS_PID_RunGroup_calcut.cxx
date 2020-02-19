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
     auto d_neg = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
                            .Filter("H.cer.npeSum > 10")
                            .Filter("H.cal.etottracknorm >0.85")
                            .Filter("");
     int neg_cal_e_cercut_n = j_cuts["SHMS"]["HGC_e"].get<int>();
     std::string neg_cal_e_cercut = "P.hgcer.npeSum > "+std::to_string(neg_cal_e_cercut_n);
     auto d_neg_cal_e = d_neg.Filter(neg_cal_e_cercut);
     //auto h_neg_cal_e = d_neg.Histo1D({"","",100,0.1,2},"P.cal.etottracknorm");
     auto h_neg_e_cercut = d_neg_cal_e.Histo1D({"","",100,0.001,2},"P.cal.etottracknorm");
     double n_neg_e_cercut = (double)h_neg_e_cercut->GetEntries();
     std::cout<<n_neg_e_cercut<<std::endl;
     jout[std::to_string(RunGroup)]["neg_c"]["e_all"] = n_neg_e_cercut;
     int neg_cal_pion_cercut_n = j_cuts["SHMS"]["HGC_pi"].get<int>();
     std::string neg_cal_pion_cercut = "P.hgcer.npeSum < "+std::to_string(neg_cal_pion_cercut_n);
     auto d_neg_cal_pion = d_neg.Filter(neg_cal_pion_cercut);
     auto h_neg_pion_cercut = d_neg_cal_pion.Histo1D({"","",100,0.1,2},"P.cal.etottracknorm");
     double n_neg_pion_cercut = (double)h_neg_pion_cercut->GetEntries();
     jout[std::to_string(RunGroup)]["neg_c"]["pi_all"] = n_neg_pion_cercut;
     std::vector<double> n_neg_e_cal,n_neg_pion_cal;
     for(int i = 0;i<n_cuts;++i){
       TAxis *axis_neg_e_cal = h_neg_e_cercut->GetXaxis();
       int bmin_neg_e_cal = axis_neg_e_cal->FindBin(cal_cut_low[i]);
       int bmax_neg_e_cal = axis_neg_e_cal->GetLast();
       double n_neg_e_cercut_cal = (double)h_neg_e_cercut->Integral(bmin_neg_e_cal,bmax_neg_e_cal);
       std::cout<<i<<std::endl;
       std::cout<<"e "<<n_neg_e_cercut_cal<<std::endl;
       n_neg_e_cal.push_back(n_neg_e_cercut_cal);

       TAxis *axis_neg_pion_cal = h_neg_pion_cercut->GetXaxis();
       int bmin_neg_pion_cal = axis_neg_pion_cal->FindBin(cal_cut_low[i]);
       int bmax_neg_pion_cal = axis_neg_pion_cal->GetLast();
       double n_neg_pion_cercut_cal = (double)h_neg_pion_cercut->Integral(bmin_neg_pion_cal,bmax_neg_pion_cal);
       std::cout<<"pion "<<n_neg_pion_cercut_cal<<std::endl;
       n_neg_pion_cal.push_back(n_neg_pion_cercut_cal);
     }
     jout[std::to_string(RunGroup)]["neg_c"]["e"] = n_neg_e_cal;
     jout[std::to_string(RunGroup)]["neg_c"]["pi"] = n_neg_pion_cal;
     
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     files_pos.push_back(rootfile_name);
  }
     ROOT::RDataFrame d_pos_raw("T",files_pos);
     auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
     auto d_pos = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
     int pos_cal_e_cercut_n = j_cuts["SHMS"]["HGC_e"].get<int>();
     std::string pos_cal_e_cercut = "P.hgcer.npeSum > "+std::to_string(pos_cal_e_cercut_n);
     auto d_pos_cal_e = d_pos.Filter(pos_cal_e_cercut);
     //auto h_pos_cal_e = d_pos.Histo1D({"","",100,0.1,2},"P.cal.etottracknorm");
     auto h_pos_e_cercut = d_pos_cal_e.Histo1D({"","",100,0.1,2},"P.cal.etottracknorm");
     double n_pos_e_cercut = (double)h_pos_e_cercut->GetEntries();
     std::cout<<n_pos_e_cercut<<std::endl;
     jout[std::to_string(RunGroup)]["pos_c"]["e_all"] = n_pos_e_cercut;
     int pos_cal_pion_cercut_n = j_cuts["SHMS"]["HGC_pi"].get<int>();
     std::string pos_cal_pion_cercut = "P.hgcer.npeSum < "+std::to_string(pos_cal_pion_cercut_n);
     auto d_pos_cal_pion = d_pos.Filter(pos_cal_pion_cercut);
     auto h_pos_pion_cercut = d_pos_cal_pion.Histo1D({"","",100,0.1,2},"P.cal.etottracknorm");
     double n_pos_pion_cercut = (double)h_pos_pion_cercut->GetEntries();
     jout[std::to_string(RunGroup)]["pos_c"]["pi_all"] = n_pos_pion_cercut;
     std::vector<double> n_pos_e_cal,n_pos_pion_cal;
     for(int i = 0;i<n_cuts;++i){
       TAxis *axis_pos_e_cal = h_pos_e_cercut->GetXaxis();
       int bmin_pos_e_cal = axis_pos_e_cal->FindBin(cal_cut_low[i]);
       int bmax_pos_e_cal = axis_pos_e_cal->GetLast();
       double n_pos_e_cercut_cal = (double)h_pos_e_cercut->Integral(bmin_pos_e_cal,bmax_pos_e_cal);
       std::cout<<i<<std::endl;
       std::cout<<"e "<<n_pos_e_cercut_cal<<std::endl;
       n_pos_e_cal.push_back(n_pos_e_cercut_cal);

       TAxis *axis_pos_pion_cal = h_pos_pion_cercut->GetXaxis();
       int bmin_pos_pion_cal = axis_pos_pion_cal->FindBin(cal_cut_low[i]);
       int bmax_pos_pion_cal = axis_pos_pion_cal->GetLast();
       double n_pos_pion_cercut_cal = (double)h_pos_pion_cercut->Integral(bmin_pos_pion_cal,bmax_pos_pion_cal);
       std::cout<<"pion "<<n_pos_pion_cercut_cal<<std::endl;
       n_pos_pion_cal.push_back(n_pos_pion_cercut_cal);
     }
     jout[std::to_string(RunGroup)]["pos_c"]["e"] = n_pos_e_cal;
     jout[std::to_string(RunGroup)]["pos_c"]["pi"] = n_pos_pion_cal;
     //Cerkov e efficiency and pion rejection factor part
//     auto d_neg_cer_e = d_neg.Filter("0.85 < P.cal.etottracknorm && P.cal.etottracknorm <1.5");
//     auto h_neg_     
  }//if normal production runs
  std::ofstream outfile;
  std::string outfile_name = "results/pid/SHMS_cal_cuts_"+std::to_string(RunGroup)+".json";
  outfile.open(outfile_name.c_str());
  //outfile.open("results/HMS_PID_statistics.json",std::ofstream::out | std::ofstream::app);
  outfile<<jout.dump(4)<<std::endl;
}
