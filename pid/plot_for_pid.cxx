#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TCanvas.h"
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void plot_for_pid(int RunNumber = 0){
  //RunNumber = 7647;
  
  if(RunNumber == 0){
    std::cout<<"Enter RunNumber(-1 to exit)";
    std::cin>>RunNumber;
    if(RunNumber < 0)
      return;
  }
  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d("T",rootfile_name.c_str());
  auto d_good = d.Filter("-10 < P.gtr.dp && P.gtr.dp < 22").Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
  auto p_aero_npeSum = d_good.Histo1D({"","aero npeSum;npeSum;counts",100,0,50},"P.aero.npeSum");
  //auto p_aero_npeSum_neg = d_good.Histo1D({"","aero neg npeSum;npeSum;counts",100,0,100},"P.aero.negnpeSum");
  //auto p_aero_npeSum_pos = d_good.Histo1D({"","aero pos npeSum;npeSum;counts",100,0,100},"P.aero.posnpeSum");
  auto p_hgcer_npeSum = d_good.Histo1D({"","hgcer npeSum;npeSum;counts",100,0,70},"P.hgcer.npeSum");
  auto p_ngcer_npeSum = d_good.Histo1D({"","ngcer npeSum;npeSum;counts",100,0,50},"P.ngcer.npeSum");
  auto h_cer_npeSum = d_good.Histo1D({"","hms cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
  auto p_cal = d_good.Histo1D({"","shms cal;E/P;counts",100,0,2},"P.cal.etottracknorm");
  auto h_cal = d_good.Histo1D({"","hms cal;E/P;counts",100,0,2},"H.cal.etottracknorm");
  auto h_coin_time0 = d_good.Histo1D({"","cointime;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
  auto d_cuts = d_good.Filter("H.cer.npeSum > 10")
                      .Filter("H.cal.etottracknorm > 0.85")
                      .Filter("fEvtHdr.fEvtType == 4");
  auto h_coin_time = d_cuts.Histo1D({"","",800,0,100},"CTime.ePositronCoinTime_ROC2");
  int coin_peak_bin = h_coin_time->GetMaximumBin();
  double coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);

  auto d_coin = d_cuts.Filter([=](double coin_time){return std::abs(coin_time-coin_peak_center)<2.0;},{"CTime.ePositronCoinTime_ROC2"});
  auto h_coin_time2 = d_coin.Histo1D({"","2",800,0,100},"CTime.ePositronCoinTime_ROC2");
  auto p_hgcer_npeSum2 = d_coin.Histo1D({"","hgcer npeSum2;npeSum;counts",100,0,70},"P.hgcer.npeSum");
  auto p_ngcer_npeSum2 = d_coin.Histo1D({"","ngcer npeSum2;npeSum;counts",100,0,50},"P.ngcer.npeSum");
  auto p_aero_npeSum2 = d_coin.Histo1D({"","aero npeSum2;npeSum;counts",100,0,50},"P.aero.npeSum");
  auto p_cal2 = d_coin.Histo1D({"","shms cal2;E/P;counts",100,0,2},"P.cal.etottracknorm");

   

  TCanvas *c_p_cal = new TCanvas();
  p_cal->DrawCopy("hist");
  TCanvas *c_h_cal = new TCanvas();
  h_cal->DrawCopy("hist");
  
  TCanvas *c_p_aerocer = new TCanvas();
  //c_p_cer->Divide(2,3);
  //c_p_cer->cd(1);
  p_aero_npeSum->DrawCopy("hist");
  //c_p_cer->cd(2);
  //p_aero_npeSum_neg->DrawCopy("hist");
  //c_p_cer->cd(3);
  //p_aero_npeSum_pos->DrawCopy("hist");
  c_p_aerocer->SaveAs("results/pid/SHMS_aeroC_npeSum.pdf");

  TCanvas *c_p_hgcer = new TCanvas();
      TVirtualPad *pad_p_hgcer = c_p_hgcer->cd();
      pad_p_hgcer->SetLogy();
  p_hgcer_npeSum->DrawCopy("hist");
  c_p_hgcer->SaveAs("results/pid/SHMS_HGC_npeSum.pdf");

  TCanvas *c_p_ngcer = new TCanvas();
      TVirtualPad *pad_p_ngcer = c_p_ngcer->cd();
      pad_p_ngcer->SetLogy();
  p_ngcer_npeSum->DrawCopy("hist");
  c_p_ngcer->SaveAs("results/pid/SHMS_NGC_npeSum.pdf");

  TCanvas *c_hms = new TCanvas();
  c_hms->Divide(1,2);
  c_hms->cd(1);
  h_cal->DrawCopy("hist");
  c_hms->cd(2);
  h_cer_npeSum->DrawCopy("hist");
  c_hms->SaveAs("results/pid/HMS_C.pdf");

  TCanvas *c_cointime = new TCanvas();
  c_cointime->Divide(1,3);
  c_cointime->cd(1);
  h_coin_time0->DrawCopy("hist");
  c_cointime->cd(2);
  h_coin_time->DrawCopy("hist");
  c_cointime->cd(3);
  h_coin_time2->DrawCopy("hist");
  c_cointime->SaveAs("results/pid/cointime.pdf");
  
  TCanvas *c_aftercoincut = new TCanvas();
  c_aftercoincut->Divide(2,2);
  c_aftercoincut->cd(1);
  p_hgcer_npeSum2->DrawCopy("hist");
  c_aftercoincut->cd(2);
  p_ngcer_npeSum2->DrawCopy("hist");
  c_aftercoincut->cd(3);
  p_aero_npeSum2->DrawCopy("hist");
  c_aftercoincut->cd(4);
  p_cal2->DrawCopy("hist");
  c_aftercoincut->SaveAs("results/pid/aftercointime.pdf");

  auto h_P_x_fp = d_coin.Histo1D({"","",100,-40,40},"P.dc.x_fp");
  auto h_P_xp_fp = d_coin.Histo1D({"","",100,-0.2,0.2},"P.dc.xp_fp");
  TCanvas *c_fp = new TCanvas();
  c_fp->Divide(1,2);
  c_fp->cd(1);
  h_P_x_fp->DrawCopy("hist");
  c_fp->cd(2);
  h_P_xp_fp->DrawCopy("hist");
  c_fp->SaveAs("results/pid/SHMS_fp.pdf");
}
