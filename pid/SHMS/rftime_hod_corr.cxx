#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>
#include "nlohmann/json.hpp"
#include "TPaveText.h"
#include "TF1.h"
#include "TH1.h"

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
   //             .Define("SHMS_length_corr",[](double shms_xptar,double shms_dp){return .11*shms_xptar*10 +0.057*shms_dp/10000.;},{"P.gtr.th","P.gtr.dp"})//length in meter
   //             //.Define("time_corr",[](){},{"shms_p"})
   //             .Define("pi_hod_starttime_corr",[lightspeed,m_pi](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_pi*m_pi))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
   //             .Define("K_hod_starttime_corr",[lightspeed,m_K](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_K*m_K))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
   //             .Define("proton_hod_starttime_corr",[lightspeed,m_proton](double delta_len,double shms_p){return (1e9*delta_len*sqrt(shms_p*shms_p+m_proton*m_proton))/(lightspeed*shms_p);},{"SHMS_length_corr","shms_p"})
                .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
   //             .Define("fptime_minus_rf_pi_corr","P.hod.starttime+pi_hod_starttime_corr-T.coin.pRF_tdcTime")
   //             .Define("fptime_minus_rf_K_corr","P.hod.starttime+K_hod_starttime_corr-T.coin.pRF_tdcTime")
   //             .Define("fptime_minus_rf_proton_corr","P.hod.starttime+proton_hod_starttime_corr-T.coin.pRF_tdcTime")
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
    
    //plot cointime
    auto h_cointime_all = d_pos.Histo1D({"","",100,40,55},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_first = d_pos_first.Histo1D({"","",100,40,55},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_second = d_pos_second.Histo1D({"","",100,40,55},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_both = d_pos_both.Histo1D({"","",100,40,55},"CTime.ePiCoinTime_ROC2");
    TCanvas *c_cointime = new TCanvas();
    h_cointime_all->DrawCopy("hist");
    //h_cointime_both->SetLineC
    h_cointime_second->SetLineColor(kBlue);
    h_cointime_second->DrawCopy("hist same");
    h_cointime_first->SetLineColor(kRed);
    h_cointime_first->DrawCopy("hist same");
    TPaveText *pt_cointime = new TPaveText(0.75,0.75,0.95,0.95,"brNDC");
    pt_cointime->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_cointime->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_cointime->AddText(("1st peak"+std::to_string(coin_peak_center_pos)).c_str());
    pt_cointime->AddText(("2nd peak"+std::to_string(coin_2ndpeak_center_pos)).c_str());
    //pt_cointime->AddText(("height ratio "+std::to_string(coin_1stpeak_content/coin_2ndpeak_content)).c_str());
    c_cointime->cd();
    pt_cointime->Draw();
    std::string c_cointime_name = "results/pid/rftime/coin_time_"+std::to_string(RunGroup)+".pdf";
    c_cointime->SaveAs(c_cointime_name.c_str());


    //move pi peak to 1
  auto h_time_diff_pos = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,-10,10},"fptime_minus_rf");
  int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
  double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
  double offset_pos = 401.8-time_diff_pos_max;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_first = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
  ;
  auto d_mod_second= d_pos_second.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});

  
  // prediction
  double shms_central_length = 20.1;
  auto t_pi = [shms_central_length](double SHMS_P){
    return (shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.00051*0.00051)*1e9/(299792458*SHMS_P));};
  auto t_K = [shms_central_length](double SHMS_P){
    return (shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.493*0.493)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.00051*0.00051)*1e9/(299792458*SHMS_P));};
  auto t_proton = [shms_central_length](double SHMS_P){
    return (shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.938*0.938)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.00051*0.00051)*1e9/(299792458*SHMS_P));};
  double tpi = t_pi(SHMS_P);
  std::cout<<"time of flight for pi-e for this setting "<<tpi<<std::endl;
    //"offset for predictions "<<1-tpi<<std::endl;
  //double offset_pre = 1-tpi;
  auto t_hardron = [=](double *x,double *p){
    return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0]))-(p[0]*std::sqrt(x[0]*x[0]+p[2]*p[2])*1e9/(299792458*x[0]))+1,4.008);};
  //no time correction(electron time corrected)
  TF1 *f_proton = new TF1("t_proton",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_proton->SetParameters(shms_central_length,0.938,0.00051);
  f_proton->SetParNames("track_length","m_proton","m_e");
  TF1 *f_pi = new TF1("t_pi",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_pi->SetParameters(shms_central_length,0.139,0.00051);
  f_pi->SetParNames("track_length","m_pi","m_e");
  TF1 *f_K = new TF1("t_K",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_K->SetParameters(shms_central_length,0.493,0.00051);
  f_K->SetParNames("track_length","m_K","m_e");
  
  //pi time corrected
  TF1 *f_proton_picorr = new TF1("t_proton_picorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_proton_picorr->SetParameters(shms_central_length,0.938,0.139);
  f_proton_picorr->SetParNames("track_length","m_proton","m_pi");
  TF1 *f_pi_picorr = new TF1("t_pi_picorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_pi_picorr->SetParameters(shms_central_length,0.139,0.139);
  f_pi_picorr->SetParNames("track_length","m_pi","m_pi");
  TF1 *f_K_picorr = new TF1("t_K_picorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_K_picorr->SetParameters(shms_central_length,0.493,0.139);
  f_K_picorr->SetParNames("track_length","m_K","m_pi");

  //K time corrected
  TF1 *f_proton_Kcorr = new TF1("t_proton_Kcorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_proton_Kcorr->SetParameters(shms_central_length,0.938,0.493);
  f_proton_Kcorr->SetParNames("track_length","m_proton","m_K");
  TF1 *f_pi_Kcorr = new TF1("t_pi_Kcorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_pi_Kcorr->SetParameters(shms_central_length,0.139,0.493);
  f_pi_Kcorr->SetParNames("track_length","m_pi","m_K");
  TF1 *f_K_Kcorr = new TF1("t_K_Kcorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_K_Kcorr->SetParameters(shms_central_length,0.493,0.493);
  f_K_Kcorr->SetParNames("track_length","m_K","m_K");
  
  //proton time corrected
  TF1 *f_proton_protoncorr = new TF1("t_proton_protoncorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_proton_protoncorr->SetParameters(shms_central_length,0.938,0.938);
  f_proton_protoncorr->SetParNames("track_length","m_proton","m_proton");
  TF1 *f_pi_protoncorr = new TF1("t_pi_protoncorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_pi_protoncorr->SetParameters(shms_central_length,0.139,0.938);
  f_pi_protoncorr->SetParNames("track_length","m_pi","m_proton");
  TF1 *f_K_protoncorr = new TF1("t_K_protoncorr",t_hardron,0.9*SHMS_P,1.22*SHMS_P,3);
  f_K_protoncorr->SetParameters(shms_central_length,0.493,0.938);
  f_K_protoncorr->SetParNames("track_length","m_K","m_proton");
  //correct time on data
  auto pi_corrected_time = [=](double difftime,double shmsp){
  //return difftime-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P);
  return difftime+shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P);
  };
  auto K_corrected_time = [=](double difftime,double shmsp){
  //return difftime-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.493*0.493)*1e9/(299792458*SHMS_P);
  return difftime+shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.493*0.493)*1e9/(299792458*SHMS_P);
  };
  auto proton_corrected_time = [=](double difftime,double shmsp){
  //return difftime-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.938*0.938)*1e9/(299792458*SHMS_P);
  return difftime+shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.139*0.139)*1e9/(299792458*SHMS_P)-shms_central_length*std::sqrt(SHMS_P*SHMS_P+0.938*0.938)*1e9/(299792458*SHMS_P);
  };
  //draw first cointime peak, no matter if we can separate two peaks
  auto d_first_corr = d_mod_first
    .Define("pi_corrected_diff_shift",pi_corrected_time,{"diff_time_shift","shms_p"})
    .Define("pi_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"pi_corrected_diff_shift"})
    .Define("K_corrected_diff_shift",K_corrected_time,{"diff_time_shift","shms_p"})
    .Define("K_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"K_corrected_diff_shift"})
    .Define("proton_corrected_diff_shift",proton_corrected_time,{"diff_time_shift","shms_p"})
    .Define("proton_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"proton_corrected_diff_shift"})
    ;
  //no correction
  auto h_rf_shmsp_pos = d_mod_first.Histo2D({"","first coin peak;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod");
  TCanvas *c_rf_shmsp_pos = new TCanvas();
  h_rf_shmsp_pos->DrawCopy("colz");
  f_pi->SetLineColor(kRed);
  f_pi->Draw("same");
  f_K->SetLineColor(kOrange);
  f_K->Draw("same");
  TPaveText *pt_1st = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_1st->AddText("no hardron time corrected");
    pt_1st->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_1st->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_1st->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos->cd();
    pt_1st->Draw();
  std::string c_rf_shmsp_pos_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+"_nocorr.pdf";
  c_rf_shmsp_pos->SaveAs(c_rf_shmsp_pos_name.c_str());
  //pi time corrected
  auto h_rf_shmsp_pos_pi = d_first_corr.Histo2D({"","pi_corrected;shms_p;pi_corrected",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","pi_corrected_mod");
  TCanvas *c_rf_shmsp_pos_pi = new TCanvas();
  h_rf_shmsp_pos_pi->DrawCopy("colz");
  f_pi_picorr->SetLineColor(kRed);
  f_pi_picorr->Draw("same");
  f_K_picorr->SetLineColor(kOrange);
  f_K_picorr->Draw("same");
  f_proton_picorr->SetLineColor(kBlue);
  f_proton_picorr->Draw("same");
  TPaveText *pt_1st_pi = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_1st_pi->AddText("pi time corrected");
    pt_1st_pi->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_1st_pi->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_1st_pi->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_pi->cd();
    pt_1st_pi->Draw();
  std::string c_rf_shmsp_pos_pi_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+"_picorr.pdf";
  c_rf_shmsp_pos_pi->SaveAs(c_rf_shmsp_pos_pi_name.c_str());

  auto h_rf_shmsp_pos_K = d_first_corr.Histo2D({"","1st coin time peak, Kcorrected;shms_p;K_corrected",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","K_corrected_mod");
  TCanvas *c_rf_shmsp_pos_K = new TCanvas();
  h_rf_shmsp_pos_K->DrawCopy("colz");
  f_pi_Kcorr->SetLineColor(kRed);
  f_pi_Kcorr->Draw("same");
  f_K_Kcorr->SetLineColor(kOrange);
  f_K_Kcorr->Draw("same");
  f_proton_Kcorr->SetLineColor(kBlue);
  f_proton_Kcorr->Draw("same");
  TPaveText *pt_1st_K = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_1st_K->AddText("K time corrected");
    pt_1st_K->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_1st_K->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_1st_K->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_K->cd();
    pt_1st_K->Draw();
  std::string c_rf_shmsp_pos_k_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+"_Kcorr.pdf";
  c_rf_shmsp_pos_K->SaveAs(c_rf_shmsp_pos_k_name.c_str());

  auto h_rf_shmsp_pos_proton = d_first_corr.Histo2D({"","1st coin time peak, proton_corrected;shms_p;proton_corrected",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","proton_corrected_mod");
  TCanvas *c_rf_shmsp_pos_proton = new TCanvas();
  h_rf_shmsp_pos_proton->DrawCopy("colz");
  f_pi_protoncorr->SetLineColor(kRed);
  f_pi_protoncorr->Draw("same");
  f_K_protoncorr->SetLineColor(kOrange);
  f_K_protoncorr->Draw("same");
  f_proton_protoncorr->SetLineColor(kBlue);
  f_proton_protoncorr->Draw("same");
  TPaveText *pt_1st_proton = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_1st_proton->AddText("proton time corrected");
    pt_1st_proton->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_1st_proton->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_1st_proton->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_proton->cd();
    pt_1st_proton->Draw();
  std::string c_rf_shmsp_pos_proton_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+"_protoncorr.pdf";
  c_rf_shmsp_pos_proton->SaveAs(c_rf_shmsp_pos_proton_name.c_str());

  //draw both peaks, no matter if we can separate coin peaks
  //no correction
  auto h_rf_shmsp_pos_both = d_pos_both.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
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
  TPaveText *pt_both_2d = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_both_2d->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_both_2d->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_both_2d->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_both->cd();
    pt_both_2d->Draw();
  std::string c_rf_shmsp_pos_both_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_"+std::to_string(RunGroup)+".pdf";
  c_rf_shmsp_pos_both->SaveAs(c_rf_shmsp_pos_both_name.c_str());
 
  auto d_both_corr = d_pos_both
    .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
    .Define("pi_corrected_diff_shift",pi_corrected_time,{"diff_time_shift","shms_p"})
    .Define("pi_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"pi_corrected_diff_shift"})
    .Define("K_corrected_diff_shift",K_corrected_time,{"diff_time_shift","shms_p"})
    .Define("K_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"K_corrected_diff_shift"})
   .Define("proton_corrected_diff_shift",proton_corrected_time,{"diff_time_shift","shms_p"})
    .Define("proton_corrected_mod",[=](double difftime){return std::fmod(difftime,4.008);},{"proton_corrected_diff_shift"})
    ;
  //pi time corrected  
  auto h_rf_shmsp_pos_both_pi = d_both_corr.Histo2D({"","both coin peak pi corrected;shms_p;pi_corrected_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","pi_corrected_mod");
  TCanvas *c_rf_shmsp_pos_both_pi = new TCanvas();
  h_rf_shmsp_pos_both_pi->DrawCopy("colz");
  f_pi_picorr->SetLineColor(kRed);
  f_pi_picorr->Draw("same");
  f_K_picorr->SetLineColor(kOrange);
  f_K_picorr->Draw("same");
  f_pi_picorr->SetLineColor(kBlue);
  f_pi_picorr->Draw("same");
  TPaveText *pt_both_pi = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_both_pi->AddText("pi time corrected");
    pt_both_pi->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_both_pi->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_both_pi->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_both_pi->cd();
    pt_both_pi->Draw();
  std::string c_rf_shmsp_pos_both_pi_name = "results/pid/rftime/rf_shmsp_pos_both_"+std::to_string(RunGroup)+"_picorr.pdf";
  c_rf_shmsp_pos_both_pi->SaveAs(c_rf_shmsp_pos_both_pi_name.c_str());
  //K corrected
  auto h_rf_shmsp_pos_both_K = d_both_corr.Histo2D({"","both coin peak K corrected;shms_p;K_corrected_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","K_corrected_mod");
  TCanvas *c_rf_shmsp_pos_both_K = new TCanvas();
  h_rf_shmsp_pos_both_K->DrawCopy("colz");
  f_pi_Kcorr->SetLineColor(kRed);
  f_pi_Kcorr->Draw("same");
  f_K_Kcorr->SetLineColor(kOrange);
  f_K_Kcorr->Draw("same");
  f_proton_Kcorr->SetLineColor(kBlue);
  f_proton_Kcorr->Draw("same");
  TPaveText *pt_both_K = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_both_K->AddText("K time corrected");
    pt_both_K->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_both_K->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_both_K->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_both_K->cd();
    pt_both_K->Draw();
  std::string c_rf_shmsp_pos_both_K_name = "results/pid/rftime/rf_shmsp_pos_both_"+std::to_string(RunGroup)+"_Kcorr.pdf";
  c_rf_shmsp_pos_both_K->SaveAs(c_rf_shmsp_pos_both_K_name.c_str());
  //proton time corrected  
  auto h_rf_shmsp_pos_both_proton = d_both_corr.Histo2D({"","both coin peak proton corrected;shms_p;proton_corrected_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","proton_corrected_mod");
  TCanvas *c_rf_shmsp_pos_both_proton = new TCanvas();
  h_rf_shmsp_pos_both_proton->DrawCopy("colz");
  f_pi_protoncorr->SetLineColor(kRed);
  f_pi_protoncorr->Draw("same");
  f_K_protoncorr->SetLineColor(kOrange);
  f_K_protoncorr->Draw("same");
  f_proton_protoncorr->SetLineColor(kBlue);
  f_proton_protoncorr->Draw("same");
  TPaveText *pt_both_proton = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_both_proton->AddText("proton time corrected");
    pt_both_proton->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_both_proton->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_both_proton->AddText(enough?"two_cointime_peak":"mixed_cointime_peak");
    c_rf_shmsp_pos_both_proton->cd();
    pt_both_proton->Draw();
  std::string c_rf_shmsp_pos_both_proton_name = "results/pid/rftime/rf_shmsp_pos_both_"+std::to_string(RunGroup)+"_protoncorr.pdf";
  c_rf_shmsp_pos_both_proton->SaveAs(c_rf_shmsp_pos_both_proton_name.c_str());

  //
  //auto h_rf_shmsp_pos_both_proton = d_pos_both
  //                                     .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_proton_corr"})
  //                                     .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
                                       //.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
                                       //.Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
                                       //.Define("diff_time_mod_corr","diff_time_mod+proton_hod_starttime_corr")
  //                                     .Histo2D({"","both coin peak after proton tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  
  //TCanvas *c_rf_shmsp_pos_both_proton = new TCanvas();
  //h_rf_shmsp_pos_both_proton->DrawCopy("colz");
  //f_proton->Draw("same");
  //f_K->Draw("same");
  //f_pi->Draw("same");
  //std::string c_rf_shmsp_pos_both_proton_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_"+std::to_string(RunGroup)+"_proton.pdf";
  //c_rf_shmsp_pos_both_proton->SaveAs(c_rf_shmsp_pos_both_proton_name.c_str());
 
  //auto h_rf_shmsp_pos_proton = d_pos_second 
  //                                     .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_proton_corr"})
  //                                     .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
  //                                     .Histo2D({"","2nd coin peak after proton tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  //auto h_rf_shmsp_pos_pi = d_pos_first
  //                                     .Define("diff_time_shift_corr",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf_pi_corr"})
  //                                     .Define("diff_time_mod_corr",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift_corr"})
  //                                     .Histo2D({"","1st coin peak after pi tf corr;shms_p;diff_time_mod",100,0.9*SHMS_P,1.22*SHMS_P,100,-1,5},"shms_p","diff_time_mod_corr");
  //TH2D* h_rf_shmsp_pos_separated = *h_rf_shmsp_pos_pi;
 // h_rf_shmsp_pos_separated->Add(h_rf_shmsp_pos_pi);
  //h_rf_shmsp_pos_pi->Add(h_rf_shmsp_pos_proton);
  //TCanvas *c_separated = new TCanvas();
  //h_rf_shmsp_pos_pi->DrawCopy("colz");
  //h_rf_shmsp_pos_proton->DrawCopy("colz same");
  //std::string c_separated_name = "results/pid/rftime/rf_shmsp_pos_separated_"+std::to_string(RunGroup)+".pdf";
  //c_separated->SaveAs(c_separated_name.c_str());

  //auto h_delt_len = d_pos_both.Histo1D({"","",100,-0.1,0.1},"SHMS_length_corr");
  //TCanvas *c_delt_len = new TCanvas();
  //h_delt_len->DrawCopy("hist");
  //std::string c_delt_len_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_length_"+std::to_string(RunGroup)+".pdf";
  //c_delt_len->SaveAs(c_delt_len_name.c_str());
  //auto h_delt_time_pi = d_pos_both.Histo1D({"","",100,-2,2},"pi_hod_starttime_corr");
  //auto h_delt_time_proton = d_pos_both.Histo1D({"","",100,-2,2},"proton_hod_starttime_corr");
  //TCanvas *c_delt_time = new TCanvas();
  //h_delt_time_pi->DrawCopy("hist");
  //h_delt_time_proton->SetLineColor(kBlue);
  //h_delt_time_proton->DrawCopy("hist same");
  //    std::string c_delt_time_name = "results/pid/rftime/rf_shmsp_pos_bothpeaks_time_"+std::to_string(RunGroup)+".pdf";
  //c_delt_time->SaveAs(c_delt_time_name.c_str());
  //auto h_delt_time_shmsp = d_pos_both.Histo2D({"","",100,0.9*SHMS_P,1.22*SHMS_P,100,-2,2},"shms_p","pi_hod_starttime_corr");
  //TCanvas *c_delt_time_shmsp = new TCanvas();
  //h_delt_time_shmsp->DrawCopy();
  //c_delt_time_shmsp->SaveAs("results/pid/rftime/rf_shmsp_pos_bothpeaks_time_vsp.pdf");
  }
}
