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
 
void acc_subtract(int RunGroup = 0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit) ";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  //int n;
  //std::cout<<"which cut do you want?"<<std::endl;
  //std::cin>>n;
  std::string pos_sim_rootfile = "sim/csv"+std::to_string(RunGroup)+"pos.root";
  std::string neg_sim_rootfile = "sim/csv"+std::to_string(RunGroup)+"neg.root";
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
  std::string c_monitor_name = "results/acc_subtract/monitor_"+std::to_string(RunGroup)+".pdf";
  c_monitor->SaveAs(c_monitor_name.c_str());

  auto h_cointime_pos_all = d_pos_pi.Histo1D({"","pion",500,-40,120},"CTime.ePiCoinTime_ROC2");
  auto d_pos_pi_pimain = d_pos_pi 
                .Filter(
                  [=](double cointime){return std::abs(cointime-coin_peak_center_pos)<2.004;},{"CTime.ePiCoinTime_ROC2"})
                .Define("pi_shift",[=](double cointime){return cointime-coin_peak_center_pos+2.004;},{"CTime.ePiCoinTime_ROC2"})
                ;
  auto h_cointime_pos_pimain = d_pos_pi_pimain.Histo1D({"","pions;cointime;counts",100,0,4.008},"pi_shift");
  double coin_bg_pos_before_1,coin_bg_pos_before_2,coin_bg_pos_after_1,coin_bg_pos_after_2;
  double bg_pos_2_before_1,bg_pos_2_before_2,bg_pos_2_after_1,bg_pos_2_after_2;
  double bg_pos_3_after_1,bg_pos_3_after_2,bg_pos_3_before_1,bg_pos_3_before_2;
  double bg_pos_4_after_1,bg_pos_4_after_2,bg_pos_4_before_1,bg_pos_4_before_2;
  double scale_1 = 1;
  double scale_2 = 1;
  double scale_3 = 1;
  double scale_4 = 1;
  for(int n = 1;n<4;n++){
  switch(n){
    case 1 : 
  //10 bgs first bg selection, left and right 5 bg peaks
   coin_bg_pos_before_1 = coin_peak_center_pos-6.5*4.008;
   coin_bg_pos_before_2 = coin_peak_center_pos-1.5*4.008;
   coin_bg_pos_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
   coin_bg_pos_after_2 = enough ? coin_peak_center_pos+7.5*4.008 : coin_peak_center_pos + 6.5*4.008; 
  //10 bgs second selection, left 10
  bg_pos_2_before_1 = coin_peak_center_pos-10.5*4.008;
  bg_pos_2_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_2_after_1 = coin_peak_center_pos+0.5*4.008;
  bg_pos_2_after_2 = coin_peak_center_pos+0.5*4.008;
  //10 bgs third selection, right 10
  bg_pos_3_before_1 = coin_peak_center_pos-0.5*4.008;
  bg_pos_3_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_3_after_1 = coin_peak_center_pos+6.5*4.008;
  bg_pos_3_after_2 = coin_peak_center_pos+16.5*4.008;
  //10 bgs forth selection, right 10, invalid here
  bg_pos_4_before_1 = coin_peak_center_pos-0.5*4.008;
  bg_pos_4_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_4_after_1 = coin_peak_center_pos+16.5*4.008;
  bg_pos_4_after_2 = coin_peak_center_pos+16.5*4.008;
  scale_1 = 10;
  scale_2 = 10;
  scale_3 = 10;
  scale_4 = 10;
  break;
   case 2 :
  //4 bgs first bg selection, left and right 2 bg peaks
   coin_bg_pos_before_1 = coin_peak_center_pos-3.5*4.008;
   coin_bg_pos_before_2 = coin_peak_center_pos-1.5*4.008;
   coin_bg_pos_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
   coin_bg_pos_after_2 = enough ? coin_peak_center_pos+4.5*4.008 : coin_peak_center_pos + 3.5*4.008; 
  //10 bgs second selection, left 10
  bg_pos_2_before_1 = coin_peak_center_pos-7.5*4.008;
  bg_pos_2_before_2 = coin_peak_center_pos-3.5*4.008;
  bg_pos_2_after_1 = coin_peak_center_pos+2.5*4.008;
  bg_pos_2_after_2 = coin_peak_center_pos+2.5*4.008;
  //10 bgs third selection, right 10
  bg_pos_3_before_1 = coin_peak_center_pos-0.5*4.008;
  bg_pos_3_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_3_after_1 = coin_peak_center_pos+6.5*4.008;
  bg_pos_3_after_2 = coin_peak_center_pos+10.5*4.008;
  //10 bgs forth selection, right 10
  bg_pos_4_before_1 = coin_peak_center_pos-0.5*4.008;
  bg_pos_4_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_4_after_1 = coin_peak_center_pos+12.5*4.008;
  bg_pos_4_after_2 = coin_peak_center_pos+16.5*4.008;
  scale_1 = 4;
  scale_2 = 4;
  scale_3 = 4;
  scale_4 = 4;
  break;
   case 3 :
  //change bgs first bg selection, left and right bg peaks
   coin_bg_pos_before_1 = coin_peak_center_pos-2.5*4.008;
   coin_bg_pos_before_2 = coin_peak_center_pos-0.5*4.008;
   coin_bg_pos_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
   coin_bg_pos_after_2 = enough ? coin_peak_center_pos+4.5*4.008 : coin_peak_center_pos + 3.5*4.008; 
  // bgs second selection, left 10
  bg_pos_2_before_1 = coin_peak_center_pos-4.5*4.008;
  bg_pos_2_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_2_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
  bg_pos_2_after_2 = enough ? coin_peak_center_pos+6.5*4.008 : coin_peak_center_pos + 5.5*4.008; 
  // bgs third selection, right 10
  bg_pos_3_before_1 = coin_peak_center_pos-6.5*4.008;
  bg_pos_3_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_3_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
  bg_pos_3_after_2 = enough ? coin_peak_center_pos+8.5*4.008 : coin_peak_center_pos + 7.5*4.008; 
  //bgs forth selection
  bg_pos_4_before_1 = coin_peak_center_pos-8.5*4.008;
  bg_pos_4_before_2 = coin_peak_center_pos-0.5*4.008;
  bg_pos_4_after_1 = enough ? coin_peak_center_pos+2.5*4.008 : coin_peak_center_pos + 1.5*4.008; 
  bg_pos_4_after_2 = enough ? coin_peak_center_pos+10.5*4.008 : coin_peak_center_pos + 9.5*4.008; 
  scale_1 = 4;
  scale_2 = 8;
  scale_3 = 12;
  scale_4 = 16;
  break;
  }
  auto d_pos_pi_bg_1 = d_pos_pi
    .Filter([=](double bg_cointime){return ((bg_cointime>coin_bg_pos_before_1 && bg_cointime<coin_bg_pos_before_2) || (bg_cointime>coin_bg_pos_after_1 && bg_cointime<coin_bg_pos_after_2));},{"CTime.ePiCoinTime_ROC2"})
    .Define("bg_cointime_mod",[=](double bg_cointime){return std::fmod((bg_cointime-coin_bg_pos_before_1),4.008);},{"CTime.ePiCoinTime_ROC2"});
  auto d_pos_pi_bg_2 = d_pos_pi
    .Filter([=](double bg_cointime){return ((bg_cointime>bg_pos_2_before_1 && bg_cointime<bg_pos_2_before_2) || (bg_cointime>bg_pos_2_after_1 && bg_cointime<bg_pos_2_after_2));},{"CTime.ePiCoinTime_ROC2"})
    .Define("bg_cointime_mod",[=](double bg_cointime){return std::fmod((bg_cointime-bg_pos_2_before_1),4.008);},{"CTime.ePiCoinTime_ROC2"});
  auto d_pos_pi_bg_3 = d_pos_pi
    .Filter([=](double bg_cointime){return ((bg_cointime>bg_pos_3_before_1 && bg_cointime<bg_pos_3_before_2) || (bg_cointime>bg_pos_3_after_1 && bg_cointime<bg_pos_3_after_2));},{"CTime.ePiCoinTime_ROC2"})
    .Define("bg_cointime_mod",[=](double bg_cointime){return std::fmod((bg_cointime-bg_pos_3_before_1),4.008);},{"CTime.ePiCoinTime_ROC2"});
  auto d_pos_pi_bg_4 = d_pos_pi
    .Filter([=](double bg_cointime){return ((bg_cointime>bg_pos_4_before_1 && bg_cointime<bg_pos_4_before_2) || (bg_cointime>bg_pos_4_after_1 && bg_cointime<bg_pos_4_after_2));},{"CTime.ePiCoinTime_ROC2"})
    .Define("bg_cointime_mod",[=](double bg_cointime){return std::fmod((bg_cointime-bg_pos_4_before_1),4.008);},{"CTime.ePiCoinTime_ROC2"});
  auto h_cointime_pos_bg_1 = d_pos_pi_bg_1.Histo1D({"","bg1",500,-40,120},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_bg_2 = d_pos_pi_bg_2.Histo1D({"","bg2",500,-40,120},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_bg_3 = d_pos_pi_bg_3.Histo1D({"","bg3",500,-40,120},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_bg_4 = d_pos_pi_bg_4.Histo1D({"","bg3",500,-40,120},"CTime.ePiCoinTime_ROC2");
  TCanvas *c_cointime_pos = new TCanvas();
  //c_cointime_pos->SetLogy();
  h_cointime_pos_all->DrawCopy("hist");
  h_cointime_pos_bg_1->SetLineColor(kRed);
  h_cointime_pos_bg_1->DrawCopy("hist same");
  h_cointime_pos_bg_2->SetLineColor(kBlue);
  h_cointime_pos_bg_2->DrawCopy("hist same");
  h_cointime_pos_bg_3->SetLineColor(kOrange);
  h_cointime_pos_bg_3->DrawCopy("hist same");
  h_cointime_pos_bg_4->SetLineColor(28);
  h_cointime_pos_bg_4->DrawCopy("hist same");
  c_cointime_pos->BuildLegend(0.8,0.8,1,1);
  std::string c_cointime_pos_name = "results/acc_subtract/coin_pos_bg_"+std::to_string(RunGroup)+"_"+std::to_string(n)+".pdf";
  c_cointime_pos->SaveAs(c_cointime_pos_name.c_str());
  TCanvas *c_cointime_pos_log = new TCanvas();
  c_cointime_pos_log->SetLogy();
  h_cointime_pos_all->DrawCopy("hist");
  h_cointime_pos_bg_1->SetLineColor(kRed);
  h_cointime_pos_bg_1->DrawCopy("hist same");
  h_cointime_pos_bg_2->SetLineColor(kBlue);
  h_cointime_pos_bg_2->DrawCopy("hist same");
  h_cointime_pos_bg_3->SetLineColor(kOrange);
  h_cointime_pos_bg_3->DrawCopy("hist same");
  h_cointime_pos_bg_4->SetLineColor(28);
  h_cointime_pos_bg_4->DrawCopy("hist same");
  c_cointime_pos->BuildLegend(0.8,0.8,1,1);
  std::string c_cointime_pos_log_name = "results/acc_subtract/coin_pos_bg_"+std::to_string(RunGroup)+"_"+std::to_string(n)+"_log.pdf";
  c_cointime_pos_log->SaveAs(c_cointime_pos_log_name.c_str());

  std::cout<<"check"<<std::endl;

  auto h_pos_mod_pimain = d_pos_pi_pimain.Histo1D({"","pion",100,0,4.008},"pi_shift");
  auto h_pos_mod_bg_1 = d_pos_pi_bg_1.Histo1D({"","bg1",100,0,4.008},"bg_cointime_mod");
  auto h_pos_mod_bg_2 = d_pos_pi_bg_2.Histo1D({"","bg2",100,0,4.008},"bg_cointime_mod");
  auto h_pos_mod_bg_3 = d_pos_pi_bg_3.Histo1D({"","bg3",100,0,4.008},"bg_cointime_mod");
  auto h_pos_mod_bg_4 = d_pos_pi_bg_4.Histo1D({"","bg3",100,0,4.008},"bg_cointime_mod");
  double error_pos_mod_bg_1_100 = h_pos_mod_bg_1->GetBinError(100);
  double stddev_pos_mod_bg_1_100 = h_pos_mod_bg_1->GetStdDev(100);
  double error_pos_mod_bg_2_100 = h_pos_mod_bg_2->GetBinError(100);
  double stddev_pos_mod_bg_2_100 = h_pos_mod_bg_2->GetStdDev(100);
  double error_pos_mod_bg_3_100 = h_pos_mod_bg_3->GetBinError(100);
  double stddev_pos_mod_bg_3_100 = h_pos_mod_bg_3->GetStdDev(100);
  std::cout<<"for the "<<n<<"kind of sellection"<<std::endl;
  std::cout<<"sellection 1,bin at "<<h_pos_mod_bg_1->GetBinCenter(100)<<",bin error "<<error_pos_mod_bg_1_100<<",stddev: "<<stddev_pos_mod_bg_1_100<<std::endl;
  std::cout<<"sellection 2,bin at "<<h_pos_mod_bg_2->GetBinCenter(100)<<",bin error "<<error_pos_mod_bg_2_100<<",stddev: "<<stddev_pos_mod_bg_2_100<<std::endl;
  std::cout<<"sellection 3,bin at "<<h_pos_mod_bg_3->GetBinCenter(100)<<",bin error "<<error_pos_mod_bg_3_100<<",stddev: "<<stddev_pos_mod_bg_3_100<<std::endl;
  TH1D* h_pos_mod_bg_all = new TH1D("","all",100,0,4.008);
  h_pos_mod_bg_all->Sumw2();
  h_pos_mod_bg_all->Add(h_pos_mod_bg_1.GetPtr(),1.0/scale_1);
  h_pos_mod_bg_all->Add(h_pos_mod_bg_2.GetPtr(),1.0/scale_2);
  h_pos_mod_bg_all->Add(h_pos_mod_bg_3.GetPtr(),1.0/scale_3);
  h_pos_mod_bg_all->Add(h_pos_mod_bg_4.GetPtr(),1.0/scale_4);
  TCanvas *c_bg_pos = new TCanvas();
  //h_pos_mod_pimain->Scale(0.05);
  //h_pos_mod_pimain->DrawCopy("hist");
  h_pos_mod_bg_all->Scale(0.25);
  h_pos_mod_bg_all->DrawCopy("hist e");
  h_pos_mod_bg_1->Scale(1.0/scale_1);
  h_pos_mod_bg_1->SetLineColor(kRed);
  h_pos_mod_bg_1->DrawCopy("hist same e");
  //h_pos_mod_bg_1->DrawCopy("hist same");
  h_pos_mod_bg_2->Scale(1.0/scale_2);
  h_pos_mod_bg_2->SetLineColor(kBlue);
  h_pos_mod_bg_2->DrawCopy("hist same e");
  h_pos_mod_bg_3->Scale(1.0/scale_3);
  h_pos_mod_bg_3->SetLineColor(kOrange);
  h_pos_mod_bg_3->DrawCopy("hist same e");
  h_pos_mod_bg_4->Scale(1.0/scale_4);
  h_pos_mod_bg_4->SetLineColor(28);
  h_pos_mod_bg_4->DrawCopy("hist same e");
  c_bg_pos->BuildLegend(0.75,0.75,1,1);
  std::string c_bg_pos_name = "results/acc_subtract/pos_bg_mod_"+std::to_string(RunGroup)+"_"+std::to_string(n)+".pdf";
  c_bg_pos->SaveAs(c_bg_pos_name.c_str());

  TH1D* h_pos_pi_corrected_1 = new TH1D("","bg1",100,0,4.008); 
  TH1D* h_pos_pi_corrected_2 = new TH1D("","bg2",100,0,4.008); 
  TH1D* h_pos_pi_corrected_3 = new TH1D("","bg3",100,0,4.008); 
  TH1D* h_pos_pi_corrected_4 = new TH1D("","bg4",100,0,4.008); 
    h_pos_pi_corrected_1->Sumw2();
    h_pos_pi_corrected_2->Sumw2();
    h_pos_pi_corrected_3->Sumw2();
    h_pos_pi_corrected_4->Sumw2();
    h_pos_pi_corrected_1->Add(h_pos_mod_pimain.GetPtr(),h_pos_mod_bg_1.GetPtr(),1,-1.0/scale_1);
    h_pos_pi_corrected_2->Add(h_pos_mod_pimain.GetPtr(),h_pos_mod_bg_2.GetPtr(),1,-1.0/scale_2);
    h_pos_pi_corrected_3->Add(h_pos_mod_pimain.GetPtr(),h_pos_mod_bg_3.GetPtr(),1,-1.0/scale_3);
    h_pos_pi_corrected_4->Add(h_pos_mod_pimain.GetPtr(),h_pos_mod_bg_4.GetPtr(),1,-1.0/scale_4);
  TCanvas *c_pos_corrected = new TCanvas();
  h_pos_mod_pimain->DrawCopy("hist e");
  h_pos_pi_corrected_1->SetLineColor(kRed);
  h_pos_pi_corrected_1->DrawCopy("hist same e");
  h_pos_pi_corrected_2->SetLineColor(kBlue);
  h_pos_pi_corrected_2->DrawCopy("hist same e");
  h_pos_pi_corrected_3->SetLineColor(kOrange);
  h_pos_pi_corrected_3->DrawCopy("hist same e");
  h_pos_pi_corrected_4->SetLineColor(28);
  h_pos_pi_corrected_4->DrawCopy("hist same e");
  std::string c_pos_corrected_name = "results/acc_subtract/pos_bg_sub_"+std::to_string(RunGroup)+"_"+std::to_string(n)+".pdf";
  c_pos_corrected->SaveAs(c_pos_corrected_name.c_str());
  double error_pos_bg_1_100 = h_pos_pi_corrected_1->GetBinError(100);
  double error_pos_bg_2_100 = h_pos_pi_corrected_2->GetBinError(100);
  double error_pos_bg_3_100 = h_pos_pi_corrected_3->GetBinError(100);
  std::cout<<"for the "<<n<<"kind of sellection"<<std::endl;
  std::cout<<"sellection 1,bin at "<<h_pos_pi_corrected_1->GetBinCenter(100)<<",bin error "<<error_pos_bg_1_100<<std::endl;
  std::cout<<"sellection 2,bin at "<<h_pos_pi_corrected_2->GetBinCenter(100)<<",bin error "<<error_pos_bg_2_100<<std::endl;
  std::cout<<"sellection 3,bin at "<<h_pos_pi_corrected_3->GetBinCenter(100)<<",bin error "<<error_pos_bg_3_100<<std::endl;
  }
  ////for simc root files
  //std::string pos_sim_rootfile = "sim/csv"+std::to_string(RunGroup)+"pos.root";
  //std::string neg_sim_rootfile = "sim/csv"+std::to_string(RunGroup)+"neg.root";
  //
  //ROOT::RDataFrame d_sim_pos_raw_1("h666",pos_sim_rootfile);
  //ROOT::RDataFrame d_sim_neg_raw_1("h666",neg_sim_rootfile);
  //auto d_sim_pos_raw = d_sim_pos_raw_1
  //  .Filter("hsdelta > -10 && hsdelta < 10")
  //  .Filter("ssdelta > -10 && ssdelta < 22")
  //  ;
  //auto d_sim_neg_raw = d_sim_neg_raw_1
  //  .Filter("hsdelta > -10 && hsdelta < 10")
  //  .Filter("ssdelta > -10 && ssdelta < 22")
  //  ;

  //double dcharge_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["charge"].get<double>();
  //double d_shmsTE_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["shmsTE"].get<double>();
  //double d_hmsTE_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["hmsTE"].get<double>();
  //double d_coinlive_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["coinlive"].get<double>();
  //double normfac_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["normfac"].get<double>();
  //double factor_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["factor"].get<double>();
  //double nentries_pos = *d_sim_pos_raw.Count();
  //double nentries_neg = *d_sim_neg_raw.Count();
  //std::cout<<"neg sim entries "<<nentries_neg<<" pos "<<nentries_pos<<std::endl;
  //double wfac_neg = (normfac_neg/nentries_ng)*(dcharge_neg/1);
  //auto weightcalculate_neg = [wfac_neg,d_coinlive_neg,d_shmsTE_neg,d_hmsTE_neg,factor_neg](float weight){return wfac_neg*d_coinlive_neg*d_shmsTE_neg*d_hmsTE_neg*weight*factor_neg;};
  //double dcharge_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["charge"].get<double>();
  //double d_shmsTE_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["shmsTE"].get<double>();
  //double d_hmsTE_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["hmsTE"].get<double>();
  //double d_coinlive_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["coinlive"].get<double>();
  //double factor_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["factor"].get<double>();
  //double normfac_pos = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["normfac"].get<double>();
  //double wfac_pos = (normfac_pos/nentries_pos)*(dcharge_pos/1);
  //auto weightcalculate_pos = [wfac_pos,d_coinlive_pos,d_shmsTE_pos,d_hmsTE_pos,factor_pos](float weight){return wfac_pos*d_coinlive_pos*d_shmsTE_pos*d_hmsTE_pos*weight*factor_pos;};
  //auto d_sim_pos = d_sim_pos_raw
  //  //.Define("weight_neg",weightcalculate_neg,{"Weight"})
  //  .Define("weight_pos",weightcalculate_pos,{"Weight"})
  //  //.Filter("hsdelta > -10 && hsdelta < 10")
  //  //.Filter("ssdelta > -10 && ssdelta < 22")
  //  ;
  //auto d_sim_neg = d_sim_neg_raw
  //  .Define("weight_neg",weightcalculate_neg,{"Weight"})
  //  //.Define("weight_pos",weightcalculate_pos,{"Weight"})
  //  //.Filter("hsdelta > -10 && hsdelta < 10")
  //  //.Filter("ssdelta > -10 && ssdelta < 22")
  //  ;


  //auto p_xptar_pos_pimain = d_pos_pi_pimain.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  //auto p_xptar_pos_bg1 = d_pos_pi_bg_1.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  //auto p_xptar_pos_bg2 = d_pos_pi_bg_2.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  //auto p_xptar_pos_bg3 = d_pos_pi_bg_3.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  //TH1D* p_xptar_pos_corrected1 = new TH1D("","bg1corrected",100,-0.1,0.1);
  //TH1D* p_xptar_pos_corrected2 = new TH1D("","bg2corrected",100,-0.1,0.1);
  //TH1D* p_xptar_pos_corrected3 = new TH1D("","bg3corrected",100,-0.1,0.1);
  //p_xptar_pos_corrected1->Add(p_xptar_pos_pimain.GetPtr(),p_xptar_pos_bg1.GetPtr(),1,-0.1);
  //p_xptar_pos_corrected2->Add(p_xptar_pos_pimain.GetPtr(),p_xptar_pos_bg2.GetPtr(),1,-0.1);
  //p_xptar_pos_corrected3->Add(p_xptar_pos_pimain.GetPtr(),p_xptar_pos_bg3.GetPtr(),1,-0.1);
  //auto p_yptar_pos_pimain = d_pos_pi_pimain.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  //auto p_yptar_pos_bg1 = d_pos_pi_bg_1.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  //auto p_yptar_pos_bg2 = d_pos_pi_bg_2.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  //auto p_yptar_pos_bg3 = d_pos_pi_bg_3.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  //TH1D* p_yptar_pos_corrected1 = new TH1D("","bg1corrected",100,-0.1,0.1);
  //TH1D* p_yptar_pos_corrected2 = new TH1D("","bg2corrected",100,-0.1,0.1);
  //TH1D* p_yptar_pos_corrected3 = new TH1D("","bg3corrected",100,-0.1,0.1);
  //p_yptar_pos_corrected1->Add(p_yptar_pos_pimain.GetPtr(),p_yptar_pos_bg1.GetPtr(),1,-0.1);
  //p_yptar_pos_corrected2->Add(p_yptar_pos_pimain.GetPtr(),p_yptar_pos_bg2.GetPtr(),1,-0.1);
  //p_yptar_pos_corrected3->Add(p_yptar_pos_pimain.GetPtr(),p_yptar_pos_bg3.GetPtr(),1,-0.1);
  //auto p_ytar_pos_pimain = d_pos_pi_pimain.Histo1D({"shms y_tar","shms y_tar",100,-4,4},"P.gtr.y");
  //auto p_ytar_pos_bg1 = d_pos_pi_bg_1.Histo1D({"shms y_tar","shms y_tar",100,-4,4},"P.gtr.y");
  //auto p_ytar_pos_bg2 = d_pos_pi_bg_2.Histo1D({"shms y_tar","shms y_tar",100,-4,4},"P.gtr.y");
  //auto p_ytar_pos_bg3 = d_pos_pi_bg_3.Histo1D({"shms y_tar","shms y_tar",100,-4,4},"P.gtr.y");
  //TH1D* p_ytar_pos_corrected1 = new TH1D("","bg1corrected",100,-4,4);
  //TH1D* p_ytar_pos_corrected2 = new TH1D("","bg2corrected",100,-4,4);
  //TH1D* p_ytar_pos_corrected3 = new TH1D("","bg3corrected",100,-4,4);
  //p_ytar_pos_corrected1->Add(p_ytar_pos_pimain.GetPtr(),p_ytar_pos_bg1.GetPtr(),1,-0.1);
  //p_ytar_pos_corrected2->Add(p_ytar_pos_pimain.GetPtr(),p_ytar_pos_bg2.GetPtr(),1,-0.1);
  //p_ytar_pos_corrected3->Add(p_ytar_pos_pimain.GetPtr(),p_ytar_pos_bg3.GetPtr(),1,-0.1);
  //auto p_delta_pos_pimain = d_pos_pi_pimain.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  //auto p_delta_pos_bg1 = d_pos_pi_bg_1.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  //auto p_delta_pos_bg2 = d_pos_pi_bg_2.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  //auto p_delta_pos_bg3 = d_pos_pi_bg_3.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  //TH1D* p_delta_pos_corrected1 = new TH1D("","bg1corrected",100,-10,22);
  //TH1D* p_delta_pos_corrected2 = new TH1D("","bg2corrected",100,-10,22);
  //TH1D* p_delta_pos_corrected3 = new TH1D("","bg3corrected",100,-10,22);
  //p_delta_pos_corrected1->Add(p_delta_pos_pimain.GetPtr(),p_delta_pos_bg1.GetPtr(),1,-0.1);
  //p_delta_pos_corrected2->Add(p_delta_pos_pimain.GetPtr(),p_delta_pos_bg2.GetPtr(),1,-0.1);
  //p_delta_pos_corrected3->Add(p_delta_pos_pimain.GetPtr(),p_delta_pos_bg3.GetPtr(),1,-0.1);
  //auto p_xptar_sim_pos = d_sim_pos.Histo1D({"shms xp_tar","shms xp_tar sim",100,-0.1,0.1},"ssxptar","weight_pos");
  //auto p_yptar_sim_pos = d_sim_pos.Histo1D({"shms yp_tar","shms yp_tar sim",100,-0.1,0.1},"ssyptar","weight_pos");
  //auto p_ytar_sim_pos = d_sim_pos.Histo1D({"shms y_tar","shms y_tar sim",100,-4,4},"ssytar","weight_pos");
  //auto p_delta_sim_pos = d_sim_pos.Histo1D({"shms dp","shms dp sim",100,-10,22},"ssdelta","weight_pos");
  //TCanvas *c_tar_p_pos = new TCanvas();
  //c_tar_p_pos->Divide(2,2);
  //c_tar_p_pos->cd(1);
  //p_xptar_pos_pimain->DrawCopy("hist");
  //p_xptar_pos_corrected1->SetLineColor(kRed);
  //p_xptar_pos_corrected1->DrawCopy("hist same");
  //p_xptar_pos_corrected2->SetLineColor(kBlue);
  //p_xptar_pos_corrected2->DrawCopy("hist same");
  //p_xptar_pos_corrected3->SetLineColor(kOrange);
  //p_xptar_pos_corrected3->DrawCopy("hist same");
  //p_xptar_sim_pos->SetMarkerStyle(3);
 //// p_xptar_sim_pos->SetLineColor(kRed);
  //p_xptar_sim_pos->DrawCopy("p same");
  //c_tar_p_pos->cd(2);
  //p_yptar_pos_pimain->DrawCopy("hist");
  //p_yptar_pos_corrected1->SetLineColor(kRed);
  //p_yptar_pos_corrected1->DrawCopy("hist same");
  //p_yptar_pos_corrected2->SetLineColor(kBlue);
  //p_yptar_pos_corrected2->DrawCopy("hist same");
  //p_yptar_pos_corrected3->SetLineColor(kOrange);
  //p_yptar_pos_corrected3->DrawCopy("hist same");
  //p_yptar_sim_pos->SetMarkerStyle(3);
 //// p_yptar_sim_pos->SetLineColor(kRed);
  //p_yptar_sim_pos->DrawCopy("p same");
  //c_tar_p_pos->cd(3);
  //p_delta_pos_pimain->DrawCopy("hist");
  //p_delta_pos_corrected1->SetLineColor(kRed);
  //p_delta_pos_corrected1->DrawCopy("hist same");
  //p_delta_pos_corrected2->SetLineColor(kBlue);
  //p_delta_pos_corrected2->DrawCopy("hist same");
  //p_delta_pos_corrected3->SetLineColor(kOrange);
  //p_delta_pos_corrected3->DrawCopy("hist same");
  //p_delta_sim_pos->SetMarkerStyle(3);
 //// p_delta_sim_pos->SetLineColor(kRed);
  //p_delta_sim_pos->DrawCopy("p same");
  //c_tar_p_pos->cd(4);
  //p_ytar_pos_pimain->DrawCopy("hist");
  //p_ytar_pos_corrected1->SetLineColor(kRed);
  //p_ytar_pos_corrected1->DrawCopy("hist same");
  //p_ytar_pos_corrected2->SetLineColor(kBlue);
  //p_ytar_pos_corrected2->DrawCopy("hist same");
  //p_ytar_pos_corrected3->SetLineColor(kOrange);
  //p_ytar_pos_corrected3->DrawCopy("hist same");
  //p_ytar_sim_pos->SetMarkerStyle(3);
 //// p_ytar_sim_pos->SetLineColor(kRed);
  //p_ytar_sim_pos->DrawCopy("p same");
  //std::string c_tar_p_pos_name = "results/acc_subtract/shms_tar_"+std::to_string(RunGroup)+"_pos.pdf";
  //c_tar_p_pos->SaveAs(c_tar_p_pos_name.c_str());
  
  //auto d_pos_pi_accsub = d_coin_pos_withbg.Define("coin_bg_mod",[=](double bg_cointime){return std::fmod((bg_cointime-coin_bg_pos_before_1),4.008);},{"cointime_bg"})//four times bg
  //.Define("coin_pi_main",[=](double cointime){return (std::abs(cointime-coin_peak_center_pos)<2.004)?cointime:0;},{"CTime.ePiCoinTime_ROC2"})
  //.Define("coin_pi_shift",[=](double cointime){return cointime-coin_peak_center_pos+2.004;},{"coin_pi_main"})
  //;
  //auto h_coin_pos_bg_mod = d_pos_pi_accsub.Histo1D({"","bgtimes4",100,0,4.008},"coin_bg_mod");
  //auto h_coin_pos_pi = d_pos_pi_accsub.Histo1D({"","pi peak",100,0,4.008},"coin_pi_shift");
  //TCanvas *c_cointime_pos_sub = new TCanvas();
  //h_coin_pos_pi->DrawCopy("hist");
  //h_coin_pos_bg_mod->Scale(0.25);
  //h_coin_pos_bg_mod->SetLineColor(kRed);
  //h_coin_pos_bg_mod->DrawCopy("hist same");
  //std::string c_cointime_pos_sub_name = "results/acc_subtract/coin_pos_bg_sub_"+std::to_string(RunGroup)+".pdf";
  //c_cointime_pos_sub->SaveAs(c_cointime_pos_sub_name.c_str());
  
  }// if production runs
}
