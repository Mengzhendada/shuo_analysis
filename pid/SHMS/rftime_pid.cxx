#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>
#include "nlohmann/json.hpp"
#include "TPaveText.h"
#include "TF1.h"
#include "TH1.h"
#include "TLine.h"

using json = nlohmann::json;
void rftime_pid(int RunGroup =0){
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
    h_cointime_second->SetLineColor(kBlue);
    h_cointime_second->DrawCopy("hist same");
    h_cointime_first->SetLineColor(kRed);
    h_cointime_first->DrawCopy("hist same");
    TPaveText *pt_cointime = new TPaveText(0.75,0.75,1,1,"brNDC");
    pt_cointime->AddText(("RunGroup "+std::to_string(RunGroup)).c_str());
    pt_cointime->AddText(("momentum "+std::to_string(SHMS_P)).c_str());
    pt_cointime->AddText(("1st peak"+std::to_string(coin_peak_center_pos)).c_str());
    pt_cointime->AddText(("2nd peak"+std::to_string(coin_2ndpeak_center_pos)).c_str());
    pt_cointime->AddText(("height ratio "+std::to_string(coin_1stpeak_content/coin_2ndpeak_content)).c_str());
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
  double offset_pre = 1-tpi;
  auto t_hardron = [=](double *x,double *p){
    return std::fmod((p[0]*std::sqrt(x[0]*x[0]+p[1]*p[1])*1e9/(299792458*x[0]))-(p[0]*std::sqrt(x[0]*x[0]+p[2]*p[2])*1e9/(299792458*x[0]))+offset_pre,4.008);};
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
  
  //draw first cointime peak, no matter if we can separate two peaks
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
  std::string c_rf_shmsp_pos_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+".pdf";
  c_rf_shmsp_pos->SaveAs(c_rf_shmsp_pos_name.c_str());

  //double range = t_K(1.22*SHMS_P)-1;
  //if(range > 0.2){range = 0.2;}
  //double rf_high = 1+range;
  //double rf_low = 1-range;
  //double rf_K_high = 1+2*range;
  double rf_pi_low = 0.7;
  double rf_pi_high = 1.3;
  double rf_K_low = 1.3;
  double rf_K_high = 1.5;
  

  TLine *f_pi_low = new TLine(0.9*SHMS_P,rf_pi_low,1.22*SHMS_P,rf_pi_low);
  TLine *f_pi_high = new TLine(0.9*SHMS_P,rf_pi_high,1.22*SHMS_P,rf_pi_high);
  TLine *f_K_low = new TLine(0.9*SHMS_P,rf_K_low,1.22*SHMS_P,rf_K_low);
  TLine *f_K_high = new TLine(0.9*SHMS_P,rf_K_high,1.22*SHMS_P,rf_K_high);
  TCanvas *c_cut = new TCanvas();
  h_rf_shmsp_pos->DrawCopy("colz");
  f_pi_low->Draw("same");
  f_pi_high->Draw("same");
  f_K_low->Draw("same");
  f_K_high->Draw("same");
  std::string c_cut_name = "results/pid/rftime/rf_shmsp_pos_1st_"+std::to_string(RunGroup)+"_cuts.pdf";
  c_cut->SaveAs(c_cut_name.c_str());

  //build dataframe
  auto d_mod_first_pirfcut = d_mod_first.Filter([=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"}); 
  auto d_mod_first_Krfcut = d_mod_first.Filter([=](double difftime){return difftime < rf_K_high && difftime > rf_K_low;},{"diff_time_mod"}); 
  //draw histos
  auto h_hgcer_pirfcut = d_mod_first_pirfcut.Histo1D({"","pions;shmsp;hgcer",100,0,30},"P.hgcer.npeSum");
  auto h_hgcer_Krfcut = d_mod_first_Krfcut.Histo1D({"","pions;shmsp;hgcer",100,0,30},"P.hgcer.npeSum");
  TCanvas *c_hgc = new TCanvas();
  h_hgcer_pirfcut->SetLineColor(kRed);
  h_hgcer_pirfcut->DrawCopy("hist");
  h_hgcer_Krfcut->SetLineColor(kOrange);
  h_hgcer_Krfcut->DrawCopy("hist same");
  std::string c_hgc_name = "results/pid/rftime/hgcer_rfcut_"+std::to_string(RunGroup)+".pdf";
  c_hgc->SaveAs(c_hgc_name.c_str());
  auto h_hgcer_shmsp_pirfcut = d_mod_first_pirfcut.Histo2D({"","pions;shmsp;hgcer",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.hgcer.npeSum");
  TProfile *h_hgcer_shmsp_pirfcut_prof = h_hgcer_shmsp_pirfcut->ProfileX();
  TCanvas *c_hgcer_2d = new TCanvas();
  h_hgcer_shmsp_pirfcut_prof->Draw();
  std::string c_hgcer_2d_name = "results/pid/rftime/hgcer_rfcut_2d_"+std::to_string(RunGroup)+"_picut.pdf";
  c_hgcer_2d->SaveAs(c_hgcer_2d_name.c_str());
  auto h_hgcer_shmsp_Krfcut = d_mod_first_Krfcut.Histo2D({"","K cut;shmsp;hgcer",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.hgcer.npeSum");
  TProfile *h_hgcer_shmsp_Krfcut_prof = h_hgcer_shmsp_Krfcut->ProfileX();
  TCanvas *c_hgcer_2d_K = new TCanvas();
  h_hgcer_shmsp_Krfcut_prof->Draw();
  std::string c_hgcer_2d_K_name = "results/pid/rftime/hgcer_rfcut_2d_"+std::to_string(RunGroup)+"_Kcut.pdf";
  c_hgcer_2d_K->SaveAs(c_hgcer_2d_K_name.c_str());
  
  auto h_aero_pirfcut = d_mod_first_pirfcut.Histo1D({"","pions;shmsp;aero",100,0,30},"P.aero.npeSum");
  auto h_aero_Krfcut = d_mod_first_Krfcut.Histo1D({"","pions;shmsp;aero",100,0,30},"P.aero.npeSum");
  TCanvas *c_aero = new TCanvas();
  h_aero_pirfcut->SetLineColor(kRed);
  h_aero_pirfcut->DrawCopy("hist");
  h_aero_Krfcut->SetLineColor(kOrange);
  h_aero_Krfcut->DrawCopy("hist same");
  std::string c_aero_name = "results/pid/rftime/aero_rfcut_"+std::to_string(RunGroup)+".pdf";
  c_aero->SaveAs(c_aero_name.c_str());
  
  auto h_aero_shmsp_pirfcut = d_mod_first_pirfcut.Histo2D({"","pions;shmsp;aero",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.aero.npeSum");
  TProfile *h_aero_shmsp_pirfcut_prof = h_hgcer_shmsp_pirfcut->ProfileX();
  TCanvas *c_aero_2d = new TCanvas();
  h_aero_shmsp_pirfcut_prof->Draw();
  std::string c_aero_2d_name = "results/pid/rftime/aero_rfcut_2d_"+std::to_string(RunGroup)+"_picut.pdf";
  c_aero_2d->SaveAs(c_aero_2d_name.c_str());
  auto h_aero_shmsp_Krfcut = d_mod_first_Krfcut.Histo2D({"","pions;shmsp;aero",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.aero.npeSum");
  TProfile *h_aero_shmsp_Krfcut_prof = h_hgcer_shmsp_Krfcut->ProfileX();
  TCanvas *c_aero_2d_K = new TCanvas();
  h_aero_shmsp_Krfcut_prof->Draw();
  std::string c_aero_2d_K_name = "results/pid/rftime/aero_rfcut_2d_"+std::to_string(RunGroup)+"_Kcut.pdf";
  c_aero_2d_K->SaveAs(c_aero_2d_K_name.c_str());
  

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
   
  }
}
