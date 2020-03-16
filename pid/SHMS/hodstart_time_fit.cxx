#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TProfile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include <iostream>
void hodstart_time_fit(int RunGroup =0){
  std::ofstream ofs;
  ofs.open("rftime_hod_param.txt",std::ofstream::out | std::ofstream::app);
  if(RunGroup ==0){
    std::cout<<"Enter Run group Number , -1 to exit";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  RunGroup = 10*RunGroup;
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

    ofs<<RunGroup<<" ";
    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
    }

    double lightspeed = 299792458;
    double m_pi = 0.139;
    double m_proton = 0.98;
    double m_K = 0.493;
    ROOT::RDataFrame d_pos_raw("T",files_pos);
    auto d_pos = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
      .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
      .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
      .Filter("H.cal.etottracknorm >0.85")
      .Filter("H.cer.npeSum>10")
      .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
      ;
    auto h_cointime_pos = d_pos.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");
    int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
    double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
    double coin_1stpeak_content = h_cointime_pos->GetBinContent(coin_peak_bin_pos);
    auto d_first = d_pos
      .Filter(
          [=](double cointime){return std::abs(cointime-coin_peak_center_pos)<1.5;},{"CTime.ePiCoinTime_ROC2"});
    auto h_cointime_pos_first = d_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_pos_2ndpeak = d_pos.Filter([=](double coin_time){return std::abs(coin_time - coin_peak_center_pos)>1.5;},{"CTime.ePiCoinTime_ROC2"}).Histo1D({"","",800,coin_peak_center_pos,100},"CTime.ePiCoinTime_ROC2");
    int coin_2ndpeak_bin_pos = h_cointime_pos_2ndpeak->GetMaximumBin();
    double coin_2ndpeak_center_pos = h_cointime_pos_2ndpeak->GetBinCenter(coin_2ndpeak_bin_pos);
    double coin_2ndpeak_content = h_cointime_pos_2ndpeak->GetBinContent(coin_2ndpeak_bin_pos);
    bool enough;
    if(std::abs(coin_peak_center_pos-coin_2ndpeak_center_pos)>2 && coin_2ndpeak_content>0.33*coin_1stpeak_content){enough = true;}
    else{enough = false;}
    std::cout<<"if we can separate two coin peaks(separation > 2)? "<<enough<<" with 2nd/1st peak height " <<coin_2ndpeak_content/coin_1stpeak_content<<std::endl;
    ofs<<enough<<" "<<coin_2ndpeak_content/coin_1stpeak_content<<" ";
    auto first_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_peak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
    auto second_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_2ndpeak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
    auto all_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
    auto d_pos_first = d_pos.Filter(first_peak,{"CTime.ePiCoinTime_ROC2"});
    auto d_pos_second = d_pos.Filter(second_peak,{"CTime.ePiCoinTime_ROC2"});
    auto d_pos_both = d_pos.Filter(
        [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;},{"CTime.ePiCoinTime_ROC2"});
    h_cointime_pos_first = d_pos_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_pos_second = d_pos_second.Histo1D({"","second peak",800,0,100},"CTime.ePiCoinTime_ROC2");
    TCanvas *c_cointime_pos = new TCanvas();
    h_cointime_pos->DrawCopy("hist");
    h_cointime_pos_first->SetLineColor(kRed);
    h_cointime_pos_first->DrawCopy("hist same");
    h_cointime_pos_second->SetLineColor(kBlue);
    h_cointime_pos_second->DrawCopy("hist same");
    std::string c_cointime_pos_name = "results/pid/rftime/coin_time_pos_"+std::to_string(RunGroup)+".pdf";
    c_cointime_pos->SaveAs(c_cointime_pos_name.c_str());

    auto h_time_diff_pos = d_pos.Histo1D({"h_rf_time","type4;rf_time",200,0,4},"fptime_minus_rf");
    int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
    double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
    double offset_pos = 401.8-time_diff_pos_max;
    ofs<<offset_pos<<" ";
    std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
    auto d_mod_first = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
      .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
    auto d_mod_second= d_pos_second.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
      .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
    if(enough){
      auto t_pion = [](double *x,double *p){
        return std::fmod((p[0]*std::sqrt(x[0]*x[0]+0.139*0.139)*1e9/(299792458*x[0])),4.008);};
      TF1 *f_pion = new TF1("t_pi",t_pion,0.9*SHMS_P,1.22*SHMS_P,1);

      auto h_rf_shmsp_pos = d_mod_first.Histo2D({"","first coin peak;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod");
      TProfile *prof_h_rf_shmsp = h_rf_shmsp_pos->ProfileX("h1",1,-1,"d");
      prof_h_rf_shmsp->Fit(f_pion,"0");
      double shms_track_len = f_pion->GetParameter(0);
      ofs<<shms_track_len<<std::endl;

      TCanvas *c_length = new TCanvas();
      prof_h_rf_shmsp->Draw();
      h_rf_shmsp_pos->DrawCopy("colz same");
      f_pion->Draw("same");
      std::string c_length_name = "results/pid/rftime/find_shms_track_length_"+std::to_string(RunGroup)+".pdf";
      c_length->SaveAs(c_length_name.c_str());
      //auto t_K = [shms_track_length](double *x){}

      //auto h_rf_shmsp_pos_both = d_mod_both.Histo2D({"","both coin peak;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod");

    }
  }
}
