#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void coin_time(int RunGroup = 0){
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
  //for neg part
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_neg.push_back(rootfile_name);
  }

  //add momentum
  double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

  
  ROOT::RDataFrame d_raw("T",files_neg);
  
  auto d_neg = d_raw.Filter("fEvtHdr.fEvtType == 4")
                .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
                .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
                .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
                .Filter("H.cal.etottracknorm > 0.85")
                .Filter("H.cer.npeSum > 10")
                .Define("rf_minus_fp_time","T.coin.pRF_tdcTime - P.hod.starttime");
  auto h_pi_coin_time = d_neg.Histo1D({"coin_time","pi",500,0,100},"CTime.ePiCoinTime_ROC2");
  int coin_time_bin_neg = h_pi_coin_time->GetMaximumBin();
  double coin_time_peak_neg = h_pi_coin_time->GetBinCenter(coin_time_bin_neg);
  auto d_coin = d_neg.Filter(
                  [=](double cointime){return std::abs(cointime-coin_time_peak_neg)<2;},{"CTime.ePiCoinTime_ROC2"});
  auto h_pi_coin_time_after = d_coin.Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2"); 

  auto h_pi_coin_time_zoom = d_neg.Histo1D({"coin_time","pi",500,40,50},"CTime.ePiCoinTime_ROC2");
  auto h_pi_coin_time_after_zoom = d_coin.Histo1D({"","",500,40,50},"CTime.ePiCoinTime_ROC2"); 
  TCanvas *c_cointime_zoom_neg = new TCanvas();
  h_pi_coin_time_zoom->DrawCopy("hist");
  h_pi_coin_time_after_zoom ->SetLineColor(kRed);
  h_pi_coin_time_after_zoom->DrawCopy("hist same");
  c_cointime_zoom_neg->SaveAs("results/pid/coin_time_zoom.pdf");


  auto h_rf_time_raw = d_neg.Histo1D({"","type 4",200,-100,100},"T.coin.pRF_tdcTime");
 auto h_rf_time_raw2 = d_neg.Histo1D({"","type 4",100,-10,10},"T.coin.pRF_tdcTime");
  auto h_rf_time = d_coin.Histo1D({"h_rf_time","Type 4, cointime cut;rf_time",200,-100,100},"T.coin.pRF_tdcTime");
  auto h_rf_time3 = d_coin.Histo1D({"h_rf_time","Type 4, cointime cut;rf_time",200,-10,10},"T.coin.pRF_tdcTime");
  auto h_rf_time_pi = d_coin.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.015")
                       .Histo1D({"","cal hardron cut",200,-10,10},"T.coin.pRF_tdcTime");
  

  
  TCanvas *c_coin_time = new TCanvas();
  c_coin_time->Divide(1,3);
  c_coin_time->cd(1);
//  h_pi_coin_time->DrawCopy("hist");
  //h_rf_time_raw->DrawCopy("hist");
  //c_coin_time->cd(2);

  h_rf_time_raw2->DrawCopy("hist");
  //c_coin_time->cd(3);
  //h_rf_time2->DrawCopy("hist");  
  //h_hod_starttime->DrawCopy("hist");
  //c_coin_time->cd(4);
  //h_rf_time->DrawCopy("hist");
  c_coin_time->cd(2);
  h_rf_time3->DrawCopy("hist");
  c_coin_time->cd(3);
  //h_time_diff->DrawCopy("hist");
  h_rf_time_pi->DrawCopy("hist");
  //c_coin_time->cd(6);
  //h_time_diff2->DrawCopy("hist");
 // c_coin_time->cd(6);
 // h_time_diff3->DrawCopy("hist");
  c_coin_time->SaveAs("results/pid/coin_time.pdf");
  
  auto h_rf_time2_4 = d_raw.Filter("fEvtHdr.fEvtType == 4")
                           .Histo1D({"h_rf_time","Type 4;rf_time",500,-1000,1000},"T.coin.pRF_tdcTime");
  auto h_rf_time2_1 = d_raw.Filter("fEvtHdr.fEvtType == 2")
                           .Histo1D({"h_rf_time","Type 2;rf_time",500,-1000,1000},"T.coin.pRF_tdcTime");
  TCanvas *c_types = new TCanvas();
  double type4_n = (double)h_rf_time2_4->GetEntries();
  double type1_n = (double)h_rf_time2_1->GetEntries();
  std::cout<<"type 4 "<<type4_n<<" type 2 "<<type1_n<<std::endl;
  h_rf_time2_4->Scale(1000.0/type4_n);
  h_rf_time2_4->DrawCopy("hist");
  h_rf_time2_1->Scale(1000.0/type1_n);
  h_rf_time2_1->SetLineColor(kRed);
  h_rf_time2_1->DrawCopy("hist same");
  TPaveText *pt_types_neg = new TPaveText(0,0.7,0.2,0.85,"brNDC");
  std::string type4 = "Type 4 entries "+std::to_string((int)h_rf_time2_4->GetEntries());
  std::string type1 = "Type 2 entries "+std::to_string((int)h_rf_time2_1->GetEntries());
  std::string momentum = "shms_p is "+std::to_string(SHMS_P);
  pt_types_neg->AddText(type4.c_str());
  pt_types_neg->AddText(type1.c_str());
  pt_types_neg->AddText(momentum.c_str());
  c_types->cd();
  pt_types_neg->Draw();
  c_types->BuildLegend(0,0.85,0.2,1);
  std::string c_type_name = "results/pid/coin_time_rftime_types_"+std::to_string(RunGroup)+".pdf";
  c_types->SaveAs(c_type_name.c_str());
  
  
  
  auto h_hod_starttime = d_coin.Histo1D({"","hodstarttime",200,0,100},"P.hod.starttime");
  
  auto h_time_diff = d_coin.Histo1D({"h_rf_time","type4,cointime cut;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff3 = d_coin.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15")
                        .Histo1D({"h_rf_time","type4, cointime cut, calhardroncut;rf_time",100,-10,10},"rf_minus_fp_time");
  auto h_time_diff4 = d_neg.Histo1D({"","type 4;rf_time",100,-10,10},"rf_minus_fp_time");
  
  int hod_starttime_bin_neg = h_hod_starttime->GetMaximumBin();
  double hod_starttime_center_neg = h_hod_starttime->GetBinCenter(hod_starttime_bin_neg);
  auto h2 = d_coin.Histo2D({"","",100,-8,8,100,hod_starttime_center_neg-10,hod_starttime_center_neg+10},"T.coin.pRF_tdcTime","P.hod.starttime");
  auto h2_2 = d_raw.Histo2D({"","",100,-8,8,100,hod_starttime_center_neg-10,hod_starttime_center_neg+10},"T.coin.pRF_tdcTime","P.hod.starttime");
  TCanvas *c2 = new TCanvas();
  h2->DrawCopy();
  c2->SaveAs("results/pid/coin_time2.pdf");
  TCanvas *c2_2 = new TCanvas();
  h2_2->DrawCopy();
  c2_2->SaveAs("results/pid/coin_time2_2.pdf");
  TCanvas *c_3= new TCanvas();
  h_pi_coin_time->DrawCopy("hist");
  h_pi_coin_time_after->SetLineColor(kRed);
  h_pi_coin_time_after->DrawCopy("hist same");
  c_3->BuildLegend(0.85,0.85,1,1);
  c_3->SaveAs("results/pid/coin_time3.pdf");

  TCanvas *c_hod = new TCanvas();
  c_hod->Divide(2,2);
  c_hod->cd(1);
  h_hod_starttime->DrawCopy("hist");
  c_hod->cd(2);
  h_time_diff3->DrawCopy("hist");
  c_hod->cd(3);
  h_time_diff->DrawCopy("hist");
  //h_time_diff2->DrawCopy("hist");
  c_hod->cd(4);
  h_time_diff4->DrawCopy("hist");
  c_hod->SaveAs("results/pid/coin_time4.pdf");
  
  auto h_time_diff_one =d_coin.Histo1D({"h_rf_time","rf_time - hodstarttime;rf_time",100,0,4},"rf_minus_fp_time");
  int diff_bin_neg = h_time_diff_one->GetMaximumBin();
  double diff_bin_center_neg = h_time_diff_one->GetBinCenter(diff_bin_neg);
  double offset;
 // if(diff_bin_center_neg > 1 && diff_bin_center_neg < 3){
 //   offset = 400;
 //   std::cout<<"rf time offset "<<offset<<std::endl;
 // }
 // else{
 //   offset = 402;
 //   std::cout<<"rf time offset "<<offset<<std::endl;
 // }
  offset = 401.8-diff_bin_center_neg;
  std::cout<<"offset "<<offset<<std::endl;
  auto d_mod = d_coin.Define("diff_time_shift",[offset](double difftime){return difftime+offset;},{"rf_minus_fp_time"})
    .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
  
  auto h_mod = d_mod.Histo1D({"","mod",100,-1,5},"diff_time_mod");
  //TCanvas *c_mod = new TCanvas();
  //h_mod->DrawCopy("hist");
  //c_mod->SaveAs("results/pid/coin_time5.pdf");

  h_mod->Fit("gaus","0","",0,4);
  TF1 *fit_mod = h_mod->GetFunction("gaus");
  double mod_mean = fit_mod->GetParameter(1);
  double mod_sigma = fit_mod->GetParameter(2);
  TCanvas *c_mod = new TCanvas();
  h_mod->DrawCopy("hist");
  fit_mod->Draw("same");
  c_mod->SaveAs("results/pid/coin_time5.pdf");
  
  double time_cut_low = mod_mean-3*mod_sigma;
  double time_cut_high = mod_mean+3*mod_sigma;
  std::string time_cut = "diff_time_mod > "+std::to_string(time_cut_low)+" && diff_time_mod < "+std::to_string(time_cut_high);
  auto d_time_cut = d_mod.Filter(time_cut);
  auto h_shmsp_diffcut = d_time_cut
     .Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
    .Histo1D({"","type4,cointime,modcut,calhardroncut",100,-10,22},"P.gtr.dp");
  auto h_shmsp = d_coin.Histo1D({"","type4,cointime",100,-10,22},"P.gtr.dp");
  auto h_shmsp_pi = d_coin.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                     .Histo1D({"","type4,cointime,calhardroncut",100,-10,22},"P.gtr.dp");
  //auto h_shmsp_diffcut = d_time_cut.Histo1D({"","type4,cointime,modcut",100,0.9*SHMS_P,1.22*SHMS_P},"shms_p");
  //auto h_shmsp = d.Histo1D({"","type4,cointime",100,0.9*SHMS_P,1.22*SHMS_P},"shms_p");
  TCanvas *c_counts = new TCanvas();
  c_counts->SetLogy();
  h_shmsp->DrawCopy("hist");
  h_shmsp_diffcut->SetLineColor(kRed);
  h_shmsp_diffcut->DrawCopy("hist same");
  h_shmsp_pi->SetLineColor(kBlue);
  h_shmsp_pi->DrawCopy("hist same");
  c_counts->BuildLegend(0.7,0.7,1,1);
  TPaveText *pt_counts_neg = new TPaveText(0.7,0.6,1,0.7,"brNDC");
  pt_counts_neg->AddText(("shms_p = "+std::to_string(SHMS_P)).c_str());
  c_counts->cd();
  pt_counts_neg->Draw();
  c_counts->SaveAs("results/pid/coin_time6.pdf");

  auto h_npeSum = d_time_cut.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                            .Histo1D({"","mod cut",100,0,30},"P.hgcer.npeSum");
  auto h_npeSum_coin = d_coin.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                        .Histo1D({"","cointimecut",100,0,30},"P.hgcer.npeSum");
  TCanvas *c_hgcernpeSum = new TCanvas();
  h_npeSum->DrawCopy("hist");
  h_npeSum_coin->SetLineColor(kRed);
  h_npeSum_coin->DrawCopy("hist same");
  c_hgcernpeSum->SaveAs("results/pid/coin_time7.pdf");

  auto h_npe_shmsp = d_time_cut.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_1 = new TCanvas();
  h_npe_shmsp->Draw();
  c_2d_1->SaveAs("results/pid/coin_time7_1.pdf");
  auto h_npe_shmsp_pi = d_coin.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_2 = new TCanvas();
  h_npe_shmsp_pi->Draw();
  c_2d_2->SaveAs("results/pid/coin_time7_2.pdf");
  
  //for pos runs
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_pos.push_back(rootfile_name);
  }

  
  ROOT::RDataFrame d_pos_raw("T",files_pos);
  auto d_pos = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
                .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
                .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
                .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
                .Filter("H.cal.etottracknorm >0.85")
                .Filter("H.cer.npeSum>10")
                .Define("rf_minus_fp_time","T.coin.pRF_tdcTime - P.hod.starttime");
  auto h_cointime_pos = d_pos.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
  double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
  auto d_first = d_pos
                .Filter(
                  [=](double cointime){return std::abs(cointime-coin_peak_center_pos)<1.5;},{"CTime.ePiCoinTime_ROC2"});
  auto h_cointime_pos_first = d_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_2ndpeak = d_pos.Filter([=](double coin_time){return std::abs(coin_time - coin_peak_center_pos)>2;},{"CTime.ePiCoinTime_ROC2"}).Histo1D({"","",800,coin_peak_center_pos,100},"CTime.ePiCoinTime_ROC2");
  int coin_2ndpeak_bin_pos = h_cointime_pos_2ndpeak->GetMaximumBin();
  double coin_2ndpeak_center_pos = h_cointime_pos_2ndpeak->GetBinCenter(coin_2ndpeak_bin_pos);
  bool enough;
  if(std::abs(coin_peak_center_pos-coin_2ndpeak_center_pos)<2){enough = false;}
  else{enough = true;}
  std::cout<<"if we can separate two coin peaks(separation > 2)? "<<enough<<std::endl;
  auto first_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_peak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
  auto second_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_2ndpeak_center_pos)<1.5 : std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
  auto all_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<1.5 || std::abs(coin_time-coin_2ndpeak_center_pos)<1.5;};
    auto d_pos_first = d_pos.Filter(first_peak,{"CTime.ePiCoinTime_ROC2"});
    auto d_pos_second = d_pos.Filter(second_peak,{"CTime.ePiCoinTime_ROC2"});
    h_cointime_pos_first = d_pos_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_cointime_pos_second = d_pos_second.Histo1D({"","second peak",800,0,100},"CTime.ePiCoinTime_ROC2");
  TCanvas *c_cointime_pos = new TCanvas();
  h_cointime_pos->DrawCopy("hist");
  h_cointime_pos_first->SetLineColor(kRed);
  h_cointime_pos_first->DrawCopy("hist same");
  h_cointime_pos_second->SetLineColor(kBlue);
  h_cointime_pos_second->DrawCopy("hist same");
  c_cointime_pos->SaveAs("results/pid/coin_time_pos.pdf");
  
  auto h_cointime_pos_zoom = d_pos.Histo1D({"coin_time","pi",500,40,50},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_first_zoom = d_pos_first.Histo1D({"","",500,40,50},"CTime.ePiCoinTime_ROC2"); 
  auto h_cointime_second_zoom = d_pos_second.Histo1D({"","",500,40,50},"CTime.ePiCoinTime_ROC2"); 
  TCanvas *c_cointime_zoom_pos = new TCanvas();
  h_cointime_pos_zoom->DrawCopy("hist");
  h_cointime_first_zoom ->SetLineColor(kRed);
  h_cointime_first_zoom->DrawCopy("hist same");
  h_cointime_second_zoom ->SetLineColor(kBlue);
  h_cointime_second_zoom->DrawCopy("hist same");
  c_cointime_zoom_pos->SaveAs("results/pid/coin_time_pos_zoom.pdf");

  if(enough){
  auto h_rf_time_pos = d_pos_raw.Histo1D({"","type4",200,-100,100},"T.coin.pRF_tdcTime");
  auto h_rf_time_pos_first = d_pos_first.Histo1D({"","first coin peak",200,-100,100},"T.coin.pRF_tdcTime");
  auto h_rf_time_pos_second = d_pos_second.Histo1D({"","second coin peak",200,-100,100},"T.coin.pRF_tdcTime");
  auto h_rf_time_pos_raw_1 = d_pos.Histo1D({"","type4",200,-10,10},"T.coin.pRF_tdcTime");
  auto h_rf_time_pos_first_1 = d_pos_first.Histo1D({"","first coin peak",200,-10,10},"T.coin.pRF_tdcTime");
  auto h_rf_time_pos_second_1 = d_pos_second.Histo1D({"","second coin peak",200,-10,10},"T.coin.pRF_tdcTime");
  TCanvas *c_rftime_pos = new TCanvas();
  //c_rftime_pos->Divide(3,1);
  //c_rftime_pos->cd(1);
  //h_rf_time_pos_raw->DrawCopy("hist");
  //h_rf_time_pos_first->SetLineColor(kRed);
  //h_rf_time_pos_first->DrawCopy("hist same");
  //h_rf_time_pos_second->SetLineColor(kBlue);
  //h_rf_time_pos_second->DrawCopy("hist same");
  h_rf_time_pos_raw_1->DrawCopy("hist");
  //c_rftime_pos->cd(2);
  h_rf_time_pos_first_1->SetLineColor(kRed);
  h_rf_time_pos_first_1->DrawCopy("hist same");
  //c_rftime_pos->cd(3);
  h_rf_time_pos_second_1->SetLineColor(kBlue);
  h_rf_time_pos_second_1->DrawCopy("hist same");

  c_rftime_pos->SaveAs("results/pid/coin_time_pos2.pdf");

  
  auto h_hod_starttime_pos = d_pos.Histo1D({"","hodstarttime",200,0,100},"P.hod.starttime");
  auto h_hod_starttime_1st = d_pos_first.Histo1D({"","hodstarttime",200,0,100},"P.hod.starttime");
  auto h_hod_starttime_2nd = d_pos_second.Histo1D({"","hodstarttime",200,0,100},"P.hod.starttime");
  //diff and diff4 are same
  auto h_time_diff_pos = d_pos.Histo1D({"h_rf_time","type4;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff_1st = d_pos_first.Histo1D({"h_rf_time","type4,cointime cut;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff_2nd = d_pos_second.Histo1D({"h_rf_time","type4,cointime cut;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff3_pos = d_pos.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15")
                        .Histo1D({"h_rf_time","type4, cointime cut, calhardroncut;rf_time",100,-10,10},"rf_minus_fp_time");
  auto h_time_diff3_1st = d_pos_first.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15")
                        .Histo1D({"h_rf_time","type4, cointime cut, calhardroncut;rf_time",100,-10,10},"rf_minus_fp_time");
  auto h_time_diff3_2nd = d_pos_second.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15")
                        .Histo1D({"h_rf_time","type4, cointime cut, calhardroncut;rf_time",100,-10,10},"rf_minus_fp_time");
  auto h_time_diff4_pos = d_pos.Histo1D({"","type 4;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff4_1st = d_pos_first.Histo1D({"","type 4,cointime;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff4_2nd = d_pos_second.Histo1D({"","type 4,cointime;rf_time",200,-10,10},"rf_minus_fp_time");
  TCanvas *c_hod_pos = new TCanvas();
  c_hod_pos->Divide(2,2);
  c_hod_pos->cd(1);
  //c_hod_pos->SetLogy();
  gPad->SetLogy();
  h_hod_starttime_pos->DrawCopy("hist");
  h_hod_starttime_1st->SetLineColor(kRed);
  h_hod_starttime_1st->DrawCopy("hist same");
  h_hod_starttime_2nd->SetLineColor(kBlue);
  h_hod_starttime_2nd->DrawCopy("hist same");
  gPad->BuildLegend(0.85,0.85,1,1,"hod start");
  c_hod->cd(2);
  h_time_diff3_pos->DrawCopy("hist");
  h_time_diff3_1st->SetLineColor(kRed);
  h_time_diff3_1st->DrawCopy("hist same");
  h_time_diff3_2nd->SetLineColor(kBlue);
  h_time_diff3_2nd->DrawCopy("hist same");
  c_hod->cd(3);
 // h_time_diff_pos->DrawCopy("hist");
  h_time_diff_1st->SetLineColor(kRed);
  h_time_diff_1st->DrawCopy("hist");
  h_time_diff_2nd->SetLineColor(kBlue);
  h_time_diff_2nd->DrawCopy("hist same");
  c_hod->cd(4);
  h_time_diff4_pos->DrawCopy("hist");
  h_time_diff4_1st->SetLineColor(kRed);
  h_time_diff4_1st->DrawCopy("hist same");
  h_time_diff4_2nd->SetLineColor(kBlue);
  h_time_diff4_2nd->DrawCopy("hist same");
  c_hod->SaveAs("results/pid/coin_time_pos4.pdf");

  auto h_npeSum_pos = d_pos.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15")
                           .Histo1D({"","cal hardron cut",100,0,30},"P.hgcer.npeSum");
  auto h_npeSum_1st = d_pos_first.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15").Histo1D({"","coin time cut,cal hardron cut",100,0,30},"P.hgcer.npeSum");
  auto h_npeSum_2nd = d_pos_second.Filter("P.cal.etottracknorm < 0.85 && P.cal.etottracknorm > 0.15").Histo1D({"","coin time cut,cal hardron cut",100,0,30},"P.hgcer.npeSum");
  TCanvas *c_npeSum_pos = new TCanvas();
  h_npeSum_1st->SetLineColor(kRed);
  h_npeSum_1st->DrawCopy("hist");
  h_npeSum_2nd->SetLineColor(kBlue);
  h_npeSum_2nd->DrawCopy("hist same");
  c_npeSum_pos->SaveAs("results/pid/coin_time_pos_hgcer.pdf");

  //below are same for both cases
  //auto h_time_diff_1st = d_pos_first.Histo1D({"h_rf_time","type4,cointime cut;rf_time",200,-10,10},"rf_minus_fp_time");
  int time_diff_pos_bin_min = h_time_diff_pos->GetMinimumBin();
  double time_diff_pos_min = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_min);
  double offset_pos = 400.8-time_diff_pos_min;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_pos = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"rf_minus_fp_time"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
  auto d_mod_second= d_pos_second.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"rf_minus_fp_time"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
  auto h_mod_pos = d_mod_pos.Histo1D({"","mod",100,-1,5},"diff_time_mod");
  auto h_mod_2nd = d_mod_second.Histo1D({"","mod",100,-1,5},"diff_time_mod");
  h_mod_pos->Fit("gaus","0","",0,4);
  TF1 *fit_mod_pos = h_mod_pos->GetFunction("gaus");
  double mod_mean_pos = fit_mod_pos->GetParameter(1);
  double mod_sigma_pos = fit_mod_pos->GetParameter(2);
  TCanvas *c_mod_pos = new TCanvas();
  h_mod_pos->DrawCopy("hist");
  fit_mod_pos->Draw("same");
  h_mod_2nd->SetLineColor(kBlue);
  h_mod_2nd->DrawCopy("hist same");
  TPaveText *pt_mod_pos = new TPaveText(0.9,0.9,1,1);
  pt_mod_pos->AddText(("shme_p = "+std::to_string(SHMS_P)).c_str());
  c_mod_pos->cd();
  pt_mod_pos->Draw();
  c_mod_pos->SaveAs("results/pid/coin_time_pos5.pdf");
  
  double time_cut_low_pos = mod_mean_pos-3*mod_sigma_pos;
  double time_cut_high_pos = mod_mean_pos+3*mod_sigma_pos;
  std::string time_cut_pos = "diff_time_mod > "+std::to_string(time_cut_low_pos)+" && diff_time_mod < "+std::to_string(time_cut_high_pos);
  auto d_time_cut_pos = d_mod_pos.Filter(time_cut_pos);
  auto h_shmsp_diffcut_pos = d_time_cut_pos
     .Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
    .Histo1D({"","type4,cointime,modcut,calhardroncut",100,-10,22},"P.gtr.dp");
  auto h_shmsp_pos = d_pos_first.Histo1D({"","type4,cointime",100,-10,22},"P.gtr.dp");
  auto h_shmsp_pi_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                     .Histo1D({"","type4,cointime,calhardroncut",100,-10,22},"P.gtr.dp");
  TCanvas *c_counts_pos = new TCanvas();
  c_counts_pos->SetLogy();
  h_shmsp_pos->DrawCopy("hist");
  h_shmsp_diffcut_pos->SetLineColor(kRed);
  h_shmsp_diffcut_pos->DrawCopy("hist same");
  h_shmsp_pi_pos->SetLineColor(kBlue);
  h_shmsp_pi_pos->DrawCopy("hist same");
  c_counts_pos->BuildLegend(0.7,0.7,1,1);
  TPaveText *pt_counts_pos = new TPaveText(0.7,0.6,1,0.7,"brNDC");
  pt_counts_pos->AddText(("shms_p = +"+std::to_string(SHMS_P)).c_str());
  c_counts->cd();
  pt_counts_pos->Draw();
  c_counts->SaveAs("results/pid/coin_time_pos6.pdf");

  //auto h_npeSum_pos = d_time_cut_pos.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
  //                          .Histo1D({"","mod cut",100,0,30},"P.hgcer.npeSum");
  //auto h_npeSum_coin_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
  //                      .Histo1D({"","cointimecut",100,0,30},"P.hgcer.npeSum");
  //TCanvas *c_hgcernpeSum_pos = new TCanvas();
  //h_npeSum_pos->DrawCopy("hist");
  //h_npeSum_coin_pos->SetLineColor(kRed);
  //h_npeSum_coin_pos->DrawCopy("hist same");
  //c_hgcernpeSum_pos->SaveAs("results/pid/coin_time_pos7.pdf");

  auto h_npe_shmsp_pos = d_time_cut_pos.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_1_pos = new TCanvas();
  h_npe_shmsp_pos->Draw();
  c_2d_1_pos->SaveAs("results/pid/coin_time_pos7_1.pdf");
  auto h_npe_shmsp_pi_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_2_pos = new TCanvas();
  h_npe_shmsp_pi_pos->Draw();
  c_2d_2_pos->SaveAs("results/pid/coin_time_pos7_2.pdf");
  }
  else{
  auto h_time_diff_pos = d_pos.Histo1D({"h_rf_time","type4;rf_time",200,-10,10},"rf_minus_fp_time");
  int time_diff_pos_bin_min = h_time_diff_pos->GetMinimumBin();
  double time_diff_pos_min = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_min);
  double offset_pos = 400.8-time_diff_pos_min;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_pos = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"rf_minus_fp_time"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"});
  auto h_mod_pos = d_mod_pos.Histo1D({"","mod",100,-1,5},"diff_time_mod");
  h_mod_pos->Fit("gaus","0","",0,4);
  TF1 *fit_mod_pos = h_mod_pos->GetFunction("gaus");
  double mod_mean_pos = fit_mod_pos->GetParameter(1);
  double mod_sigma_pos = fit_mod_pos->GetParameter(2);
  TCanvas *c_mod_pos = new TCanvas();
  h_mod_pos->DrawCopy("hist");
  fit_mod_pos->Draw("same");
  c_mod_pos->SaveAs("results/pid/coin_time_pos5.pdf");
  
  double time_cut_low_pos = mod_mean_pos-3*mod_sigma_pos;
  double time_cut_high_pos = mod_mean_pos+3*mod_sigma_pos;
  std::string time_cut_pos = "diff_time_mod > "+std::to_string(time_cut_low_pos)+" && diff_time_mod < "+std::to_string(time_cut_high_pos);
  auto d_time_cut_pos = d_mod_pos.Filter(time_cut_pos);
  auto h_shmsp_diffcut_pos = d_time_cut_pos
     .Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
    .Histo1D({"","type4,cointime,modcut,calhardroncut",100,-10,22},"P.gtr.dp");
  auto h_shmsp_pos = d_pos_first.Histo1D({"","type4,cointime",100,-10,22},"P.gtr.dp");
  auto h_shmsp_pi_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                     .Histo1D({"","type4,cointime,calhardroncut",100,-10,22},"P.gtr.dp");
  TCanvas *c_counts_pos = new TCanvas();
  c_counts_pos->SetLogy();
  h_shmsp_pos->DrawCopy("hist");
  h_shmsp_diffcut_pos->SetLineColor(kRed);
  h_shmsp_diffcut_pos->DrawCopy("hist same");
  h_shmsp_pi_pos->SetLineColor(kBlue);
  h_shmsp_pi_pos->DrawCopy("hist same");
  c_counts_pos->BuildLegend(0.7,0.7,1,1);
  TPaveText *pt_counts_pos = new TPaveText(0.7,0.6,1,0.7,"brNDC");
  pt_counts_pos->AddText(("shms_p = +"+std::to_string(SHMS_P)).c_str());
  c_counts->cd();
  pt_counts_pos->Draw();
  c_counts->SaveAs("results/pid/coin_time_pos6.pdf");

  auto h_npeSum_pos = d_time_cut_pos.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                            .Histo1D({"","mod cut",100,0,30},"P.hgcer.npeSum");
  auto h_npeSum_coin_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                        .Histo1D({"","cointimecut",100,0,30},"P.hgcer.npeSum");
  TCanvas *c_hgcernpeSum_pos = new TCanvas();
  h_npeSum_pos->DrawCopy("hist");
  h_npeSum_coin_pos->SetLineColor(kRed);
  h_npeSum_coin_pos->DrawCopy("hist same");
  c_hgcernpeSum_pos->SaveAs("results/pid/coin_time_pos7.pdf");

  auto h_npe_shmsp_pos = d_time_cut_pos.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_1_pos = new TCanvas();
  h_npe_shmsp_pos->Draw();
  c_2d_1_pos->SaveAs("results/pid/coin_time_pos7_1.pdf");
  auto h_npe_shmsp_pi_pos = d_pos_first.Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85")
                      .Histo2D({"","",100,-10,22,100,0,30},"P.gtr.dp","P.hgcer.npeSum")
                      ->ProfileX();
  TCanvas *c_2d_2_pos = new TCanvas();
  h_npe_shmsp_pi_pos->Draw();
  c_2d_2_pos->SaveAs("results/pid/coin_time_pos7_2.pdf");
  }
  
  }
}
