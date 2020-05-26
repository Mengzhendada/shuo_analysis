#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
  using json = nlohmann::json;
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

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <fstream>
#include <vector>
#include <string>

void(int RunGroup =0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit) ";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  json j_info;
  {
    std::ifstream ifs("db2/rungroup_info.json");
    ifs>>j_info;
  }
  if(!neg_D2.empty() && !pos_D2.empty()){
  std::vector<std::string> files_neg,files_pos;
  double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

  //for neg runs
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_neg.push_back(rootfile_name);
  }
  //for pos runs
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_pos.push_back(rootfile_name);
  }

  ROOT::RDataFrame d_neg_raw("T",files_neg);
  auto d_neg_dpcut = d_neg_raw
    .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
    .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    .Filter("H.cal.etottracknorm >0.85")
    .Filter("H.cer.npeSum>10")
    //.Filter("P.cal.etottracknorm > 0.1 && P.cal.etottracknorm < 0.8")
    .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime");
  ROOT::RDataFrame d_pos_raw("T",files_pos);
  auto d_pos_dpcut = d_pos_raw
    .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
    .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    .Filter("H.cal.etottracknorm >0.85")
    .Filter("H.cer.npeSum>10")
    //.Filter("P.cal.etottracknorm > 0.1 && P.cal.etottracknorm < 0.8")
    .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime");
    
    //pos rftime cut
    auto h_time_diff_pos = d_pos_dpcut.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
  int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
  double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
  double offset_pos = 401.8-time_diff_pos_max;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_first = d_pos_dpcut.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
  ;
  auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
  json j_rfcut;
  {
    std::ifstream ifs("db2/rftime_cut.json");
    ifs>>j_rfcut;
  }
  double rf_pi_low = j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_low"].get<double>();
  std::cout<<rf_pi_low<<std::endl;
  double rf_pi_high =j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_high"].get<double>();
  std::cout<<rf_pi_high<<std::endl;
  auto d_pos_pi = d_mod_first
    .Filter([=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
    .Filter("P.aero.npeSum > 2")
    ; 
  auto h_2d_pos = d_pos_pi.Histo2D({"2d","2d pos",100,0,4.008,100,0,30},"diff_time_mod","P.aero.npeSum");
  
  //just a check of coin time for pos pion after rftime cut
  auto h_cointime_pos = d_pos_pi.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
  double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
  std::cout<<"coin time peak "<<coin_peak_center_pos<<std::endl;
  double coin_1stpeak_content = h_cointime_pos->GetBinContent(coin_peak_bin_pos);
  auto d_first = d_pos_pi
                .Filter(
                  [=](double cointime){return std::abs(cointime-coin_peak_center_pos)<2.004;},{"CTime.ePiCoinTime_ROC2"});
  auto h_cointime_pos_first = d_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_2ndpeak = d_pos_pi.Filter([=](double coin_time){return std::abs(coin_time - coin_peak_center_pos)>2.004;},{"CTime.ePiCoinTime_ROC2"}).Histo1D({"","",800,coin_peak_center_pos,100},"CTime.ePiCoinTime_ROC2");
  int coin_2ndpeak_bin_pos = h_cointime_pos_2ndpeak->GetMaximumBin();
  double coin_2ndpeak_center_pos = h_cointime_pos_2ndpeak->GetBinCenter(coin_2ndpeak_bin_pos);
  double coin_2ndpeak_content = h_cointime_pos_2ndpeak->GetBinContent(coin_2ndpeak_bin_pos);
  bool enough;
  if(std::abs(coin_peak_center_pos-coin_2ndpeak_center_pos)>2.004 && coin_2ndpeak_content>0.2*coin_1stpeak_content){enough = true;}
  else{enough = false;}
  std::cout<<"if we can separate two coin peaks(separation > 2)? "<<enough<<" with 2nd/1st peak height " <<coin_2ndpeak_content/coin_1stpeak_content<<std::endl;

  
  // rfcut for neg runs
  auto h_time_diff_neg = d_neg_dpcut.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
  int time_diff_neg_bin_max = h_time_diff_neg->GetMaximumBin();
  double time_diff_neg_max = h_time_diff_neg->GetBinCenter(time_diff_neg_bin_max);
  double offset_neg = 401.8-time_diff_neg_max;
  std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
  auto d_mod_neg = d_neg_dpcut.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
  ;
  auto h_diff_mod_neg = d_mod_neg.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
  auto d_neg_pi = d_mod_neg.Filter(
      [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
    .Filter("P.aero.npeSum > 2")
    ; 
  auto h_2d_neg = d_neg_pi.Histo2D({"2d","2d neg",100,0,4.008,100,0,30},"diff_time_mod","P.aero.npeSum");
  //cointime plot for pion after rftime cut(just to check)
  auto h_cointime_neg = d_neg_pi.Histo1D({"","neg coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
  double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
  std::cout<<"coin time peak "<<coin_peak_center_neg<<std::endl;
  TCanvas *c_monitor = new TCanvas();
  c_monitor->Divide(2,3);
  c_monitor->cd(1);
  h_cointime_neg->DrawCopy("hist");
  c_monitor->cd(2);
  h_cointime_pos->DrawCopy("hist");
  c_monitor->cd(3);
  h_diff_mod_neg->DrawCopy("hist");
  c_monitor->cd(4);
  h_diff_mod_pos->DrawCopy("hist");
  c_monitor->cd(5);
  h_2d_neg->DrawCopy("hist");
  c_monitor->cd(6);
  h_2d_pos->DrawCopy("hist");
  std::string c_monitor_name = "results/TE/monitor_"+std::to_string(RunGroup)+".pdf";
  c_monitor->SaveAs(c_monitor_name.c_str());
  //use d_neg_pi and d_pos_pi

  json j_out;
  {
    std::ifstream ifs("results/TE/trackingeff_info.json");
    ifs>>j_out;
  }
  
  


  }
}
