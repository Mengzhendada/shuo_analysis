#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
void rftime_hod_corr(int RunGroup =0){
  if(RunGroup ==0){
   std::cout<<"Enter Run group Number , -1 to exit";
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
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(!neg_D2.empty() && !pos_D2.empty()){
  std::vector<std::string> files_neg,files_pos;
  double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

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
                .Define("SHMS_length_corr",[](double shms_xptar,double shms_dp){return .11*shms_xptar*10 +0.057*shms_dp/10000.;},{"P.gtr.th","P.gtr.dp"})//length in meter
                //.Define("time_corr",[](){},{"shms_p"})
                .Define("pi_hod_starttime_corr",[lightspeed,m_pi](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_pi*m_pi))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
                .Define("K_hod_starttime_corr",[lightspeed,m_K](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_K*m_K))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
                .Define("proton_hod_starttime_corr",[lightspeed,m_proton](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_proton*m_proton))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
                .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
                .Define("fptime_minus_rf_pi_corr","P.hod.starttime+pi_hod_starttime_corr-T.coin.pRF_tdcTime")
                .Define("fptime_minus_rf_K_corr","P.hod.starttime+K_hod_starttime_corr-T.coin.pRF_tdcTime")
                .Define("fptime_minus_rf_proton_corr","P.hod.starttime+proton_hod_starttime_corr-T.coin.pRF_tdcTime")
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
  auto first_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_peak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
  auto second_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_2ndpeak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
  auto all_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
    auto d_pos_first = d_pos.Filter(first_peak,{"CTime.ePiCoinTime_ROC2"});
    auto d_pos_second = d_pos.Filter(second_peak,{"CTime.ePiCoinTime_ROC2"});
    auto d_pos_both = d_pos.Filter(
        [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;},{"CTime.ePiCoinTime_ROC2"});

  auto h_time_diff_pos = d_pos.Histo1D({"h_rf_time","type4;rf_time",200,-10,10},"fptime_minus_rf");
  int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
  double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
  double offset_pos = 401.8-time_diff_pos_max;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_first = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
  auto d_mod_second= d_pos_second.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});

  
  double shms_central_length = 18.85;
  auto t_e = [shms_central_length](double SHMS_P){
    return (shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.00051*0.00051)*1e9/(299792458*SHMS_P));};
  double te = t_e(SHMS_P);
  std::cout<<"time of flight for electron for this setting "<<te<<std::endl;
  //double offset_pre = offset_pos+te;
  //double offset_pre = offset_pos+1.3;
  //double offset_pre = te;
  double offset_pre = 1;
  auto t_hardron = [offset_pre](double *x,double *p){
    return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0]))-(p[0]*std::sqrt(x[0]*x[0]+p[2]*p[2])/(299792458*x[0]))+offset_pre,4.008);};
  //auto t_hardron = [offset_pre](double *x,double *p){
  //  return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0])+offset_pre,4.008);};
  TF1 *f_proton = new TF1("t_proton",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_proton->SetParameters(shms_central_length,0.938,0.00051);
  f_proton->SetParNames("track_length","m_proton","m_e");
 // auto t_pi = [offset_pre](double *x,double *p){
 //   return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0]))-(p[0]*std::sqrt(x[0]*x[0]+p[2]*p[2])/(299792458*x[0]))+offset_pre,4.008);};
  TF1 *f_pi = new TF1("t_pi",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_pi->SetParameters(shms_central_length,0.139,0.00051);
  f_pi->SetParNames("track_length","m_proton","m_e");
  //auto t_K = [offset_pre](double *x,double *p){
  //  return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0]))-(p[0]*std::sqrt(x[0]*x[0]+p[2]*p[2])/(299792458*x[0]))+offset_pre,4.008);};
  TF1 *f_K = new TF1("t_K",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_K->SetParameters(shms_central_length,0.493,0.00051);
  f_K->SetParNames("track_length","m_K","m_e");

  auto h_rf_shmsp_pos = d_mod_first.Histo2D({"","first coin peak;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod");
  TCanvas *c_rf_shmsp_pos = new TCanvas();
  h_rf_shmsp_pos->DrawCopy("colz");
  std::string c_rf_shmsp_pos_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+".pdf";
  c_rf_shmsp_pos->SaveAs(c_rf_shmsp_pos_name.c_str());
  
  auto h_rf_shmsp_pos_both = d_pos_both.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_pi_corr"})
                                       .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
                                       .Histo2D({"","both coin peak;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod");
  TCanvas *c_rf_shmsp_pos_both = new TCanvas();
  h_rf_shmsp_pos_both->DrawCopy("colz");
  f_proton->SetLineColor(kBlue);
  f_proton->Draw("same");
  f_pi->SetLineColor(kRed);
  f_pi->Draw("same");
  f_K->SetLineColor(kOrange);
  f_K->Draw("same");
  std::string c_rf_shmsp_pos_both_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_"+std::to_string(RunGroup)+".pdf";
  c_rf_shmsp_pos_both->SaveAs(c_rf_shmsp_pos_both_name.c_str());
  
  //
  auto h_rf_shmsp_pos_both_proton = d_pos_both
                                       .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_proton_corr"})
                                       .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
                                       //.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
                                       //.Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
                                       //.Define("diff_time_mod_corr","diff_time_mod+proton_hod_starttime_corr")
                                       .Histo2D({"","both coin peak after proton tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  
  TCanvas *c_rf_shmsp_pos_both_proton = new TCanvas();
  h_rf_shmsp_pos_both_proton->DrawCopy("colz");
  f_proton->Draw("same");
  f_K->Draw("same");
  f_pi->Draw("same");
  std::string c_rf_shmsp_pos_both_proton_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_"+std::to_string(RunGroup)+"_proton.pdf";
  c_rf_shmsp_pos_both_proton->SaveAs(c_rf_shmsp_pos_both_proton_name.c_str());
 
  auto h_rf_shmsp_pos_proton = d_pos_second 
                                       .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_proton_corr"})
                                       .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
                                       .Histo2D({"","2nd coin peak after proton tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  auto h_rf_shmsp_pos_pi = d_pos_first
                                       .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_pi_corr"})
                                       .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
                                       .Histo2D({"","1st coin peak after pi tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  //TH2D* h_rf_shmsp_pos_separated = *h_rf_shmsp_pos_pi;
 // h_rf_shmsp_pos_separated->Add(h_rf_shmsp_pos_pi);
  //h_rf_shmsp_pos_pi->Add(h_rf_shmsp_pos_proton);
  TCanvas *c_separated = new TCanvas();
  h_rf_shmsp_pos_pi->DrawCopy("colz");
  h_rf_shmsp_pos_proton->DrawCopy("colz same");
  std::string c_separated_name = "results/pid/rftime/rf_shmsp_pos_separated_"+std::to_string(RunGroup)+".pdf";
  c_separated->SaveAs(c_separated_name.c_str());

  auto h_delt_len = d_pos_both.Histo1D({"","",100,-0.1,0.1},"SHMS_length_corr");
  TCanvas *c_delt_len = new TCanvas();
  h_delt_len->DrawCopy("hist");
  std::string c_delt_len_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_length_"+std::to_string(RunGroup)+".pdf";
  c_delt_len->SaveAs(c_delt_len_name.c_str());
  auto h_delt_time_pi = d_pos_both.Histo1D({"","",100,-2,2},"pi_hod_starttime_corr");
  auto h_delt_time_proton = d_pos_both.Histo1D({"","",100,-2,2},"proton_hod_starttime_corr");
  TCanvas *c_delt_time = new TCanvas();
  h_delt_time_pi->DrawCopy("hist");
  h_delt_time_proton->SetLineColor(kBlue);
  h_delt_time_proton->DrawCopy("hist same");
      std::string c_delt_time_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_time_"+std::to_string(RunGroup)+".pdf";
  c_delt_time->SaveAs(c_delt_time_name.c_str());
  //auto h_delt_time_shmsp = d_pos_both.Histo2D({"","",100,0.9*SHMS_P,1.22*SHMS_P,100,-2,2},"shms_p","pi_hod_starttime_corr");
  //TCanvas *c_delt_time_shmsp = new TCanvas();
  //h_delt_time_shmsp->DrawCopy();
  //c_delt_time_shmsp->SaveAs("results/pid/rftime/rf_shmsp_pos_bothpeaks_time_vsp.pdf");
  }
}
