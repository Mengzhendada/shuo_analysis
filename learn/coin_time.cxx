#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>

void coin_time(){



  std::string rootfile_name = "ROOTfiles/coin_replay_production_7684_-1.root";
  ROOT::RDataFrame d_raw("T",rootfile_name.c_str());
  auto d = d_raw.Filter("fEvtHdr.fEvtType == 4")
                .Filter(
                  [](double cointime){return std::abs(cointime-43)<2;},{"CTime.ePiCoinTime_ROC2"});
  auto h_pi_coin_time = d_raw.Histo1D({"coin_time","coin_time",500,0,100},"CTime.ePiCoinTime_ROC2");
  auto h_post_coin_time = d_raw.Histo1D({"coin_time","positron",500,0,100},"CTime.ePositronCoinTime_ROC2");
  auto h_k_coin_time = d_raw.Histo1D({"coin_time","k",500,0,100},"CTime.eKCoinTime_ROC2");
  auto h_proton_coin_time = d_raw.Histo1D({"coin_time","proton",500,0,100},"CTime.epCoinTime_ROC2");

  auto h_rf_time = d.Histo1D({"h_rf_time","rf_time;rf_time",500,-100,100},"T.coin.pRF_tdcTime");
  auto h_rf_time2 = d.Histo1D({"h_rf_time","rf_time;rf_time",200,-300,400},"T.coin.pRF_tdcTime");
  auto h_rf_time3 = d.Histo1D({"h_rf_time","rf_time;rf_time",100,-10,10},"T.coin.pRF_tdcTime");
  auto h_hod_starttime = d.Histo1D({"","hodstarttime",200,0,100},"P.hod.starttime");
  auto d1 = d.Define("rf_minus_fp_time","T.coin.pRF_tdcTime - P.hod.starttime");
  auto h_time_diff = d1.Histo1D({"h_rf_time","rf_time - hodstarttime;rf_time",200,-10,10},"rf_minus_fp_time");
  auto h_time_diff2 = d1.Histo1D({"h_rf_time","rf_time - hodstarttime; rf_time",500,-100,100},"rf_minus_fp_time");
 // auto h_time_diff2 = d1.Filter("P.hgcer.npeSum>1.0").Histo1D({"h_rf_time","hgcer>1;rf_time",1000,-10,10},"rf_minus_fp_time");
  auto h_time_diff3 = d1.Filter("P.hgcer.npeSum>1.0")
//                        .Filter("H.Cer.npeSum>10")
//                        .Filter("H.cal.etottracknorm>0.85")
                        .Filter([](double coin_time){return std::abs(coin_time-43)<2;},{"CTime.ePiCoinTime_ROC2"})
                        .Histo1D({"h_rf_time","cointime cut;rf_time",100,-10,10},"rf_minus_fp_time");

  
  TCanvas *c_coin_time = new TCanvas();
  c_coin_time->Divide(2,3);
  c_coin_time->cd(1);
//  h_pi_coin_time->DrawCopy("hist");
  h_rf_time2->DrawCopy("hist");  
  c_coin_time->cd(2);
  h_hod_starttime->DrawCopy("hist");
  c_coin_time->cd(3);
  h_rf_time->DrawCopy("hist");
  c_coin_time->cd(4);
  h_rf_time3->DrawCopy("hist");
  c_coin_time->cd(5);
  h_time_diff->DrawCopy("hist");
  c_coin_time->cd(6);
  h_time_diff2->DrawCopy("hist");
 // c_coin_time->cd(6);
 // h_time_diff3->DrawCopy("hist");
  c_coin_time->SaveAs("csvresult/coin_time.pdf");
  
  auto h2 = d.Histo2D({"","",100,-8,8,100,70,90},"T.coin.pRF_tdcTime","P.hod.starttime");
  auto h2_2 = d_raw.Histo2D({"","",100,-8,8,100,70,90},"T.coin.pRF_tdcTime","P.hod.starttime");
  TCanvas *c2 = new TCanvas();
  h2->DrawCopy();
  c2->SaveAs("csvresult/coin_time2.pdf");
  TCanvas *c2_2 = new TCanvas();
  h2_2->DrawCopy();
  c2_2->SaveAs("csvresult/coin_time2_2.pdf");
  TCanvas *c_3= new TCanvas();
  h_pi_coin_time->DrawCopy("hist");
  h_post_coin_time->SetLineColor(kRed);
  h_post_coin_time->DrawCopy("hist same");
  h_k_coin_time->SetLineColor(kBlue);
  h_k_coin_time->DrawCopy("hist same");
  h_proton_coin_time->SetLineColor(8);
  h_proton_coin_time->DrawCopy("hist same");
  c_3->SaveAs("csvresult/coin_time3.pdf");
}
