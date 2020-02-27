#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TAxis.h"
#include "TF1.h"
#include "TLine.h"
#include "TProfile.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <fmt/core.h>
//#include <fmt/ostream.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
bool reject;

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

    ROOT::RDataFrame d_neg_raw("T",files_neg);
    auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_neg_HMS_e = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
      .Filter("H.cer.npeSum > 10")
      .Filter("H.cal.etottracknorm >0.85")
      .Filter("fEvtHdr.fEvtType == 4")
      .Define("shms_p",shms_p_calculate,{"P.gtr.dp"});

    //coin time cut
    auto h_coin_time = d_neg_HMS_e.Histo1D({"coin_time","coin_time;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
    int coin_peak_bin = h_coin_time->GetMaximumBin();
    double coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);
    auto d_neg = d_neg_HMS_e.Filter(
        [=](double coin_time){return std::abs(coin_time - coin_peak_center) < 2.0;},{"CTime.ePositronCoinTime_ROC2"});
    auto h_coin_time_cut = d_neg.Histo1D({"","",800,0,100},"CTime.ePositronCoinTime_ROC2");
    TCanvas *c_coin_time_neg = new TCanvas();
    h_coin_time->DrawCopy("hist");
    h_coin_time_cut->SetLineColor(kRed);
    h_coin_time_cut->DrawCopy("hist same");
    std::string coin_time_neg_name = "results/pid/SHMS_coin_time_"+std::to_string(RunGroup)+"_neg.pdf";
    c_coin_time_neg->SaveAs(coin_time_neg_name.c_str());

    //hgcer and aerocer no cuts
    auto h_hgcer_npeSum = d_neg.Histo1D({"","cointime cut;npeSum;counts",100,0,50},"P.hgcer.npeSum");
    auto h_aero_npeSum =  d_neg.Histo1D({"","cointime cut;npeSum;counts",100,0,50},"P.aero.npeSum");

    //cut electrons from calorimeter
    auto h_cal_e = d_neg.Histo1D({"","SHMS cal;E/P;counts",100,0.01,2},"P.cal.etottracknorm");
    h_cal_e->Fit("gaus","O","",1,1.2);
    TF1 *fit_cal = h_cal_e->GetFunction("gaus");
    double cal_mean = fit_cal->GetParameter(1);
    double cal_sigma = fit_cal->GetParameter(2);
    double cal_e_cut_low = cal_mean - 3*cal_sigma;
    double cal_e_cut_high = cal_mean + 3*cal_sigma;
    double cal_e_cut_low_set = j_cuts["SHMS"]["cal_e"].get<double>();
    if(cal_e_cut_low < cal_e_cut_low_set){cal_e_cut_low = cal_e_cut_low_set;}
    TLine *l_cal_e_cut_low = new TLine(cal_e_cut_low,0,cal_e_cut_low,1000);
    TLine *l_cal_e_cut_high = new TLine(cal_e_cut_high,0,cal_e_cut_high,1000);

    double cal_pi_cut_low = j_cuts["SHMS"]["cal_pi_low"].get<double>();
    TLine *l_cal_pi_cut_low = new TLine(cal_pi_cut_low,0,cal_pi_cut_low,1000);

    TCanvas *c_cal = new TCanvas();
    h_cal_e->DrawCopy("hist");
    fit_cal->SetLineColor(kRed);
    fit_cal->Draw("same");
    l_cal_e_cut_low->SetLineColor(kRed);
    l_cal_e_cut_low->Draw("same");
    l_cal_e_cut_high->SetLineColor(kRed);
    l_cal_e_cut_high->Draw("same");
    l_cal_pi_cut_low->SetLineColor(kBlue);
    l_cal_pi_cut_low->Draw("same");
    std::string c_cal_name = "results/pid/SHMS_cal_e_cut_"+std::to_string(RunGroup)+".pdf";
    c_cal->SaveAs(c_cal_name.c_str());


    std::string shms_cal_pi_cut = "P.cal.etottracknorm >" +std::to_string(cal_pi_cut_low)+" && P.cal.etottracknorm < "+ std::to_string(cal_e_cut_low);
    auto d_neg_pi = d_neg.Filter(shms_cal_pi_cut);
    auto h_hgcer_npeSum_picut = d_neg_pi.Histo1D({"","cal pi cut;npeSum;counts",100,0,50},"P.hgcer.npeSum");
    auto h_aero_npeSum_picut = d_neg_pi.Histo1D({"","cal pi cut;npeSum;counts",100,0,50},"P.aero.npeSum");

    //1D histos for cerenkov
    TCanvas *c_hgcer = new TCanvas();
    h_hgcer_npeSum->DrawCopy("hist");
    h_hgcer_npeSum_picut->SetLineColor(kRed);
    h_hgcer_npeSum_picut->DrawCopy("hist same");
    c_hgcer->BuildLegend(0.7,0.7,1,1,"hgcer","f");
    std::string c_hgcer_name = "results/pid/SHMS_hgcer_"+std::to_string(RunGroup)+"_neg.pdf";
    c_hgcer->SaveAs(c_hgcer_name.c_str());

    //cerenkov pion efficiency
    int pion_calcut = (int)h_hgcer_npeSum_picut->GetEntries();
    std::vector<int> hgcer_cuts = j_cuts["SHMS"]["HGC_cuts"].get<std::vector<int>>();
    TGraph *g_hgcer_pi = new TGraph();
    g_hgcer_pi->SetTitle("pi efficiency;hgcer cut;efficiency");
    g_hgcer_pi->SetMarkerStyle(8);
    g_hgcer_pi->SetMarkerSize(0.75);
    g_hgcer_pi->SetMarkerColor(1);
    g_hgcer_pi->SetLineColor(1);
    g_hgcer_pi->SetFillColor(3);
    int i = 0;
    for(auto it = hgcer_cuts.begin();it!=hgcer_cuts.end();++it){
      TAxis *axis_hgcer_pi_neg = h_hgcer_npeSum_picut->GetXaxis();
      int bmin_hgcer_pi_neg = axis_hgcer_pi_neg->FindBin(*it);
      int bmax_hgcer_pi_neg = axis_hgcer_pi_neg->GetLast();
      double n_hgcer_pi_neg = (double)h_hgcer_npeSum_picut->Integral(bmin_hgcer_pi_neg,bmax_hgcer_pi_neg);
      double efficiency = n_hgcer_pi_neg/pion_calcut;
      g_hgcer_pi->SetPoint(i,*it,efficiency);
      ++i;
    }
    TCanvas *c_hgcer_pi_efficiency = new TCanvas();
    TPad *p1_hgcer_pi_efficiency = new TPad("p1_cal_neg","",0,0,1,1);
    p1_hgcer_pi_efficiency->SetGrid();
    p1_hgcer_pi_efficiency->Draw();
    p1_hgcer_pi_efficiency->cd();
    g_hgcer_pi->Draw("ALP");
    std::string c_hgcer_pi_efficiency_name = "results/pid/SHMS_hgcer_pi_eff_"+std::to_string(RunGroup)+"_neg.pdf";
    c_hgcer_pi_efficiency->SaveAs(c_hgcer_pi_efficiency_name.c_str());

    auto h_hgcer_dp = d_neg_pi.Histo2D({"","hgcer npeSum vs. shms_p;shms_p;npeSum",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.hgcer.npeSum");
    TProfile *prof_hgcer_p = h_hgcer_dp->ProfileX("h1",1,-1,"d");

    auto fline = [SHMS_P](double* x, double *par){
      if(reject && x[0] > 0.93*SHMS_P && x[0] < 1.05*SHMS_P){
        TF1::RejectPoint();
        return 0.0;
      }
      return par[0]*((1-par[1]*(x[0]*x[0]+0.139*0.139)/(x[0]*x[0]*1.00137*1.00137)));
    }; 
    TF1 *f1 = new TF1("f1",fline,0.9*SHMS_P,1.22*SHMS_P,2);
    //f1->SetParameters(8000,1.000137);
    reject = true;
    prof_hgcer_p->Fit(f1,"0");
    reject = false;

    TCanvas *c_npe_vs_dp = new TCanvas();

    TF1 *fleft = new TF1("fleft",fline,0.9*SHMS_P,0.93*SHMS_P,2);
    fleft->SetParameters(f1->GetParameters());
    prof_hgcer_p->GetListOfFunctions()->Add(fleft);
    gROOT->GetListOfFunctions()->Remove(fleft);
    TF1 *fright = new TF1("fright",fline,1.05*SHMS_P,1.22*SHMS_P,2);
    fright->SetParameters(f1->GetParameters());
    prof_hgcer_p->GetListOfFunctions()->Add(fright);
    gROOT->GetListOfFunctions()->Remove(fright);
    double p0_neg = *f1->GetParameters();
    gPad->BuildLegend(0.7,0.7,1,1,(std::to_string(p0_neg)).c_str());
    prof_hgcer_p->Draw();
    std::string c_npe_vs_dp_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_neg.pdf";
    c_npe_vs_dp->SaveAs(c_npe_vs_dp_name.c_str());

    auto h_hgcernpeSum_xfp = d_neg_pi.Histo2D({"","hgcer npeSum vs. x_fp;x_fp;hgcer npeSum",100,-30,30,100,0,30},"P.dc.x_fp","P.hgcer.npeSum");
    TProfile* prof_hgcer_xfp = h_hgcernpeSum_xfp->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_xfp = new TCanvas();
    h_hgcernpeSum_xfp->SetMaximum(50);
    //h_hgcernpeSum_xfp->DrawCopy("colz");
    prof_hgcer_xfp->Draw();
    std::string c_npe_vs_xfp_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_xfp_neg.pdf";
    c_npe_vs_xfp->SaveAs(c_npe_vs_xfp_name.c_str());
    auto h_hgcernpeSum_yfp = d_neg_pi.Histo2D({"","hgcer npeSum vs. y_fp;y_fp;hgcer npeSum",100,-20,20,100,0,30},"P.dc.y_fp","P.hgcer.npeSum");
    TProfile* prof_hgcer_yfp = h_hgcernpeSum_yfp->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_yfp = new TCanvas();
    h_hgcernpeSum_yfp->SetMaximum(80);
    //h_hgcernpeSum_yfp->DrawCopy("colz");
    prof_hgcer_yfp->Draw();
    std::string c_npe_vs_yfp_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_yfp_neg.pdf";
    c_npe_vs_yfp->SaveAs(c_npe_vs_yfp_name.c_str());
    auto h_hgcernpeSum_xpfp = d_neg_pi.Histo2D({"","hgcer npeSum vs. xp_fp;xp_fp;hgcer npeSum",100,-0.1,0.1,100,0,30},"P.dc.xp_fp","P.hgcer.npeSum");
    TProfile *prof_hgcernpeSum_xpfp = h_hgcernpeSum_xpfp->ProfileX();
    TCanvas *c_npe_vs_xpfp = new TCanvas();
    //h_hgcernpeSum_xpfp->SetMaximum(30);
    //h_hgcernpeSum_xpfp->DrawCopy("colz");
    prof_hgcernpeSum_xpfp->Draw();
    std::string c_npe_vs_xpfp_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_xpfp_neg.pdf";
    c_npe_vs_xpfp->SaveAs(c_npe_vs_xpfp_name.c_str());
    auto h_hgcernpeSum_ypfp = d_neg_pi.Histo2D({"","hgcer npeSum vs. yp_fp;yp_fp;hgcer npeSum",100,-0.1,0.1,100,0,30},"P.dc.yp_fp","P.hgcer.npeSum");
    TProfile *prof_hgcer_ypfp = h_hgcernpeSum_ypfp->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_ypfp = new TCanvas();
    //h_hgcernpeSum_xpfp->SetMaximum(30);
    //h_hgcernpeSum_ypfp->DrawCopy("colz");
    prof_hgcer_ypfp->Draw();
    std::string c_npe_vs_ypfp_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_ypfp_neg.pdf";
    c_npe_vs_ypfp->SaveAs(c_npe_vs_ypfp_name.c_str());
    

    double hgcer_pi = j_cuts["SHMS"]["HGC_pi"].get<double>();
    std::string hgcer_pi_cut = "P.hgcer.npeSum < "+std::to_string(hgcer_pi);
    auto d_neg_k = d_neg_pi.Filter(hgcer_pi_cut.c_str());
    auto h_aero_npeSum_kcut = d_neg_k.Histo1D({"","karon cut on HGC;npeSum;counts",100,0,50},"P.aero.npeSum");
    TCanvas *c_aero = new TCanvas();
    h_aero_npeSum->DrawCopy("hist");
    h_aero_npeSum_picut->SetLineColor(kRed);
    h_aero_npeSum_picut->DrawCopy("hist same");
    h_aero_npeSum_kcut->SetLineColor(kBlue);
    h_aero_npeSum_kcut->DrawCopy("hist same");
    gPad->BuildLegend(0.7,0.7,1,1,"aero","f");
    std::string c_aero_name = "results/pid/SHMS_aero_"+std::to_string(RunGroup)+"_neg.pdf";
    c_aero->SaveAs(c_aero_name.c_str());

    //for pos part
   for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
     int RunNumber = *it;
     std::cout<<RunNumber<<std::endl;
     std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
     files_pos.push_back(rootfile_name);
   }

   //add momentum
   ROOT::RDataFrame d_pos_raw("T",files_pos);
   auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
   auto d_pos_HMS_e = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
     .Filter("H.cer.npeSum > 10")
     .Filter("H.cal.etottracknorm >0.85")
     .Filter("fEvtHdr.fEvtType == 4")
     .Define("shms_p",shms_p_calculate,{"P.gtr.dp"});

   //coin time cut
   auto h_coin_time_pos = d_pos_HMS_e.Histo1D({"coin_time","coin_time;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
   int coin_peak_bin_pos = h_coin_time_pos->GetMaximumBin();
   double coin_peak_center_pos = h_coin_time_pos->GetBinCenter(coin_peak_bin_pos);
   auto d_pos = d_pos_HMS_e.Filter(
       [=](double coin_time){return std::abs(coin_time - coin_peak_center_pos) < 1.5;},{"CTime.ePositronCoinTime_ROC2"});
   auto h_cointime_2ndpeak = d_pos_HMS_e.Filter([=](double coin_time){return coin_time > coin_peak_center+2;},{"CTime.ePositronCoinTime_ROC2"}).Histo1D({"","",800,coin_peak_center,100},"CTime.ePositronCoinTime_ROC2");
   int coin_2ndpeak_bin_pos = h_cointime_2ndpeak->GetMaximumBin();
   double coin_2ndpeak_center_pos = h_cointime_2ndpeak->GetBinCenter(coin_2ndpeak_bin_pos);
   //coin_2ndpeak_center_pos = 46;
   std::cout<<"first peak "<<coin_peak_center_pos<<" second peak "<<coin_2ndpeak_center_pos <<std::endl;
   auto d_pos_proton = d_pos_HMS_e.Filter(
   [=](double coin_time){return std::abs(coin_time - coin_2ndpeak_center_pos) < 1.5;},{"CTime.ePositronCoinTime_ROC2"});
   std::cout<<"first peak "<<*d_pos.Count()<<" second peak "<<*d_pos_proton.Count()<<std::endl;
  auto h_1stpeak_pos = d_pos.Histo1D({"","1st peak",800,0,100},"CTime.ePositronCoinTime_ROC2");
  auto h_2ndpeak_pos = d_pos_proton.Histo1D({"","2nd peak",800,0,100},"CTime.ePositronCoinTime_ROC2");
  TCanvas *c_pos_cointime = new TCanvas();
  h_coin_time_pos->DrawCopy("hist");
  h_1stpeak_pos->SetLineColor(kRed);
  h_1stpeak_pos->DrawCopy("hist same");
  h_2ndpeak_pos->SetLineColor(kGreen);
  h_2ndpeak_pos->DrawCopy("hist same");
  std::string c_pos_cointime_name = "results/pid/SHMS_coin_time_"+std::to_string(RunGroup)+"_pos.pdf";
  c_pos_cointime->SaveAs(c_pos_cointime_name.c_str());
  
   //hgcer and aerocer no cuts
   auto h_hgcer_npeSum_pos = d_pos.Histo1D({"","coin time 1st;npeSum;counts",100,0,50},"P.hgcer.npeSum");
   auto h_aero_npeSum_pos =  d_pos.Histo1D({"","coin time 1st;npeSum;counts",100,0,50},"P.aero.npeSum");
   auto h_hgcer_npeSum_pos2 = d_pos_proton.Histo1D({"","coin time 2nd;npeSum;counts",100,0,50},"P.hgcer.npeSum");
   auto h_aero_npeSum_pos2 =  d_pos_proton.Histo1D({"","coin time 2nd;npeSum;counts",100,0,50},"P.aero.npeSum");

   //cut electrons from calorimeter
   auto h_cal_e_pos = d_pos.Histo1D({"","SHMS cal e;E/P;counts",100,0.01,2},"P.cal.etottracknorm");

   TCanvas *c_cal_pos = new TCanvas();
   h_cal_e_pos->DrawCopy("hist");
   l_cal_e_cut_low->SetLineColor(kRed);
   l_cal_e_cut_low->Draw("same");
   l_cal_e_cut_high->SetLineColor(kRed);
   l_cal_e_cut_high->Draw("same");
   l_cal_pi_cut_low->SetLineColor(kBlue);
   l_cal_pi_cut_low->Draw("same");
   std::string c_cal_pos_name = "results/pid/SHMS_cal_e_cut_"+std::to_string(RunGroup)+"_pos.pdf";
   c_cal_pos->SaveAs(c_cal_pos_name.c_str());


   auto d_pos_pi = d_pos.Filter(shms_cal_pi_cut);
   auto h_hgcer_npeSum_picut_pos = d_pos_pi.Histo1D({"","cal pi;npeSum;counts",100,0,50},"P.hgcer.npeSum");
   auto h_aero_npeSum_picut_pos = d_pos_pi.Histo1D({"","cal pi;npeSum;counts",100,0,50},"P.aero.npeSum");
   auto h_hgcer_npeSum_proton_pos = d_pos_proton.Histo1D({"","coin time proton;npeSum;counts",100,0,50},"P.hgcer.npeSum");
   auto h_aero_npeSum_proton_pos = d_pos_proton.Histo1D({"","coin time proton;npeSum;counts",100,0,50},"P.aero.npeSum");

   //1D histos for cerenkov
   TCanvas *c_hgcer_pos = new TCanvas();
   h_hgcer_npeSum_proton_pos->SetLineColor(kGreen);
   h_hgcer_npeSum_proton_pos->DrawCopy("hist");
   h_hgcer_npeSum_pos->DrawCopy("hist same");
   h_hgcer_npeSum_picut_pos->SetLineColor(kRed);
   h_hgcer_npeSum_picut_pos->DrawCopy("hist same");
   gPad->BuildLegend(0.7,0.7,0.9,0.9,"hgcer npeSum","f");
   std::string c_hgcer_pos_name = "results/pid/SHMS_hgcer_"+std::to_string(RunGroup)+"_pos.pdf";
   c_hgcer_pos->SaveAs(c_hgcer_pos_name.c_str());

   //2Ds for cerenkov for pos runs
    auto h_hgcer_dp_pos = d_pos_pi.Histo2D({"","hgcer npeSum vs. shms_p;shms_p;npeSum",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.hgcer.npeSum");
    TProfile* prof_hgcer_p_pos = h_hgcer_dp_pos->ProfileX("h1",1,-1,"d");
    reject = true;
    prof_hgcer_p_pos->Fit(f1,"0");
    reject = false;

    TCanvas *c_npe_vs_dp_pos = new TCanvas();

    TF1 *fleft_p = new TF1("fleft_pos",fline,0.9*SHMS_P,0.95*SHMS_P,1);
    fleft_p->SetParameters(f1->GetParameters());
    prof_hgcer_p_pos->GetListOfFunctions()->Add(fleft_p);
    gROOT->GetListOfFunctions()->Remove(fleft_p);
    TF1 *fright_p = new TF1("fright",fline,1.05*SHMS_P,1.22*SHMS_P,1);
    fright_p->SetParameters(f1->GetParameters());
    prof_hgcer_p_pos->GetListOfFunctions()->Add(fright_p);
    gROOT->GetListOfFunctions()->Remove(fright_p);
    double p0_pos = *f1->GetParameters();
    gPad->BuildLegend(0.7,0.7,1,1,(std::to_string(p0_pos)).c_str());

    prof_hgcer_p_pos->Draw();
    std::string c_npe_vs_dp_pos_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_pos.pdf";
    c_npe_vs_dp_pos->SaveAs(c_npe_vs_dp_pos_name.c_str());

    auto h_hgcernpeSum_xfp_pos = d_pos_pi.Histo2D({"","hgcer npeSum vs. x_fp;x_fp;hgcer npeSum",100,-30,30,100,0,30},"P.dc.x_fp","P.hgcer.npeSum");
    TProfile* prof_hgcer_xfp_pos = h_hgcernpeSum_xfp_pos->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_xfp_pos = new TCanvas();
    h_hgcernpeSum_xfp_pos->SetMaximum(50);
    //h_hgcernpeSum_xfp->DrawCopy("colz");
    prof_hgcer_xfp_pos->Draw();
    std::string c_npe_vs_xfp_pos_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_xfp_pos.pdf";
    c_npe_vs_xfp_pos->SaveAs(c_npe_vs_xfp_pos_name.c_str());
    auto h_hgcernpeSum_yfp_pos = d_pos_pi.Histo2D({"","hgcer npeSum vs. y_fp;y_fp;hgcer npeSum",100,-20,20,100,0,30},"P.dc.y_fp","P.hgcer.npeSum");
    TProfile* prof_hgcer_yfp_pos = h_hgcernpeSum_yfp_pos->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_yfp_pos = new TCanvas();
    h_hgcernpeSum_yfp_pos->SetMaximum(80);
    //h_hgcernpeSum_yfp->DrawCopy("colz");
    prof_hgcer_yfp_pos->Draw();
    std::string c_npe_vs_yfp_pos_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_yfp_pos.pdf";
    c_npe_vs_yfp_pos->SaveAs(c_npe_vs_yfp_pos_name.c_str());
    auto h_hgcernpeSum_xpfp_pos = d_pos_pi.Histo2D({"","hgcer npeSum vs. xp_fp;xp_fp;hgcer npeSum",100,-0.1,0.1,100,0,30},"P.dc.xp_fp","P.hgcer.npeSum");
    TProfile *prof_hgcernpeSum_xpfp_pos = h_hgcernpeSum_xpfp_pos->ProfileX();
    TCanvas *c_npe_vs_xpfp_pos = new TCanvas();
    //h_hgcernpeSum_xpfp->SetMaximum(30);
    //h_hgcernpeSum_xpfp->DrawCopy("colz");
    prof_hgcernpeSum_xpfp_pos->Draw();
    std::string c_npe_vs_xpfp_pos_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_xpfp_pos.pdf";
    c_npe_vs_xpfp_pos->SaveAs(c_npe_vs_xpfp_pos_name.c_str());
    auto h_hgcernpeSum_ypfp_pos = d_pos_pi.Histo2D({"","hgcer npeSum vs. yp_fp;yp_fp;hgcer npeSum",100,-0.1,0.1,100,0,30},"P.dc.yp_fp","P.hgcer.npeSum");
    TProfile *prof_hgcer_ypfp_pos = h_hgcernpeSum_ypfp_pos->ProfileX("h1",1,-1,"d");
    TCanvas *c_npe_vs_ypfp_pos = new TCanvas();
    //h_hgcernpeSum_xpfp->SetMaximum(30);
    //h_hgcernpeSum_ypfp->DrawCopy("colz");
    prof_hgcer_ypfp_pos->Draw();
    std::string c_npe_vs_ypfp_pos_name = "results/pid/SHMS_hgcer_2D_"+std::to_string(RunGroup)+"_ypfp_pos.pdf";
    c_npe_vs_ypfp_pos->SaveAs(c_npe_vs_ypfp_pos_name.c_str());
    
    //for karons, use same cut as neg runs
    auto d_pos_k = d_pos_pi.Filter(hgcer_pi_cut.c_str());
    auto h_aero_npeSum_kcut_pos = d_pos_k.Histo1D({"","karon cut on HGC;npeSum;counts",100,0,50},"P.aero.npeSum");
    TCanvas *c_aero_pos = new TCanvas();
    h_aero_npeSum_proton_pos->SetLineColor(kGreen);
    h_aero_npeSum_proton_pos->DrawCopy("hist");
    h_aero_npeSum_pos->DrawCopy("hist same");
    h_aero_npeSum_picut_pos->SetLineColor(kRed);
    h_aero_npeSum_picut_pos->DrawCopy("hist same");
    h_aero_npeSum_kcut_pos->SetLineColor(kBlue);
    h_aero_npeSum_kcut_pos->DrawCopy("hist same");
    gPad->BuildLegend(0.7,0.7,0.9,0.9,"aero npeSum");
    std::string c_aero_pos_name = "results/pid/SHMS_aero_"+std::to_string(RunGroup)+"_pos.pdf";
    c_aero_pos->SaveAs(c_aero_pos_name.c_str());
  
    auto h_beta_p = d_pos.Histo2D({"","1st coin peak;momentum;beta",100,0.9*SHMS_P,1.1*SHMS_P,100,0.8,1.1},"shms_p","P.gtr.beta");
    TProfile *prof_beta_p = h_beta_p->ProfileX("h1",1,-1,"d");
    TCanvas* c_beta_pos = new TCanvas();
    prof_beta_p->Draw();
    //h_beta_p_proton->Draw("colz");
    std::string c_beta_pos_name = "results/pid/SHMS_beta_p_"+std::to_string(RunGroup)+"_pion_pos.pdf";
    c_beta_pos->SaveAs(c_beta_pos_name.c_str());

    auto h_beta_p_proton = d_pos_proton.Histo2D({"","2nd coin peak;momentum;beta",100,0.9*SHMS_P,1.1*SHMS_P,100,0.8,1.1},"shms_p","P.gtr.beta");
    TProfile *prof_beta_p_proton = h_beta_p_proton->ProfileX("h1",1,-1,"d");
    TCanvas *c_beta_pos_proton = new TCanvas();
    prof_beta_p_proton->Draw();
    std::string c_beta_pos_proton_name = "results/pid/SHMS_beta_p_"+std::to_string(RunGroup)+"_proton_pos.pdf";
    c_beta_pos_proton->SaveAs(c_beta_pos_proton_name.c_str());
  }//if normal production runs
  // std::ofstream outfile;
  // std::string outfile_name = "results/pid/SHMS_cal_cuts_"+std::to_string(RunGroup)+".json";
  // outfile.open(outfile_name.c_str());
  // //outfile.open("results/HMS_PID_statistics.json",std::ofstream::out | std::ofstream::app);
  // outfile<<jout.dump(4)<<std::endl;
}
