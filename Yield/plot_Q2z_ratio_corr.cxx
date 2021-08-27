#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

int plot_Q2z_ratio_corr(){
  json j_Q2z;
  {
    std::ifstream runs("db2/kin_group_Q2zx.json");
    runs>>j_Q2z;
  }
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  json jout;
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  for(json::iterator it = j_Q2z.begin();it!=j_Q2z.end();++it){
    double Q2 = std::stod(it.key());
    auto j_zx = it.value();

    for(json::iterator it  = j_zx.begin();it!=j_zx.end();++it){
      double z = std::stod(it.key());
      auto j_x = it.value();
      std::string canvas_name = "Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
      std::string canvas_filename = "Q2_z_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
      std::string q2z_name = canvas_name+"_yieldratio";
      std::string q2z_filename = canvas_name+"_yieldratio";
      std::cout<<"canvas name "<<canvas_name<<std::endl;
      TCanvas *c_q2z_ratio = new TCanvas("",q2z_name.c_str(),1900,1000);
      TH1D* h_neg_q2z = new TH1D("",(q2z_name).c_str(),100,0,1);
      TH1D* h_pos_q2z = new TH1D("",(q2z_name).c_str(),100,0,1);
      int i_color = 1;
      auto mg = new TMultiGraph();
      auto mg_frag = new TMultiGraph();
      auto mg_RD = new TMultiGraph();
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(z !=0 && Q2!=0){  
        for(json::iterator it = j_x.begin();it!=j_x.end();++it){
          double xbj = std::stod(it.key());
          //std::cout<<"xbj "<<xbj<<std::endl;
          std::string q2xz_str = "Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4)+",x:"+std::to_string(xbj).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_xbj_neg_all = new TH1D("","neg;x;",100,0,1);
          //THStack *h_xbj_neg_all = new THStack("","");  
          h_xbj_neg_all->GetXaxis()->SetTitle("x");
          //h_xbj_neg_all->GetYaxis()->SetTitle("counts");
          h_xbj_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_xbj_pos_all = new TH1D("","pos;x;",100,0,1);
          //THStack *h_xbj_pos_all = new THStack("","");
          h_xbj_pos_all->GetXaxis()->SetTitle("x");
          //h_xbj_pos_all->GetYaxis()->SetTitle("counts");
          //h_xbj_pos_all->GetYaxis()->SetRangeUser(0,2);
          h_xbj_neg_all->Sumw2();
          h_xbj_pos_all->Sumw2();
          TH1D* h_xbj_neg_bg_all = new TH1D("","neg bg;x;",100,0,1);
          //THStack *h_xbj_neg_all = new THStack("","");  
          h_xbj_neg_bg_all->GetXaxis()->SetTitle("x");
          //h_xbj_neg_bg_all->GetYaxis()->SetTitle("bg counts");
          //h_xbj_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_xbj_pos_bg_all = new TH1D("","pos bg;x;",100,0,1);
          //THStack *h_xbj_pos_all = new THStack("","");
          h_xbj_pos_bg_all->GetXaxis()->SetTitle("x");
          //h_xbj_pos_bg_all->GetYaxis()->SetTitle("bg counts");
          //h_xbj_pos_bg_all->GetYaxis()->SetRangeUser(0,2);
          h_xbj_neg_bg_all->Sumw2();
          h_xbj_pos_bg_all->Sumw2();
          TH1D* h_xbj_neg_Dummy_all = new TH1D("","neg Dummy;x;",100,0,1);
          h_xbj_neg_Dummy_all->GetXaxis()->SetTitle("x");
          TH1D* h_xbj_pos_Dummy_all = new TH1D("","pos Dummy;x;",100,0,1);
          h_xbj_pos_Dummy_all->GetXaxis()->SetTitle("x");
          h_xbj_neg_Dummy_all->Sumw2();
          h_xbj_pos_Dummy_all->Sumw2();
          TH1D* h_xbj_neg_Dummy_bg_all = new TH1D("","neg Dummy;x;",100,0,1);
          h_xbj_neg_Dummy_bg_all->GetXaxis()->SetTitle("x");
          TH1D* h_xbj_pos_Dummy_bg_all = new TH1D("","pos Dummy;x;",100,0,1);
          h_xbj_pos_Dummy_bg_all->GetXaxis()->SetTitle("x");
          h_xbj_neg_Dummy_bg_all->Sumw2();
          h_xbj_pos_Dummy_bg_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs;
          std::vector<int> neg_Dummy_runs, pos_Dummy_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_xbj_neg_sim = new TH1D("","pi- sim sum",100,0,1);
          TH1D *h_xbj_neg_sim_incrad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_xbj_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("xbj_neg_inc_rad");
          TH1D *h_xbj_neg_sim_incnorad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_xbj_neg_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("xbj_neg_inc_norad");
          TH1D *h_xbj_neg_sim_excrad = new TH1D("","pi- sim exc",100,0,1);
          h_xbj_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("xbj_neg_exc_rad");
          TH1D *h_xbj_neg_sim_rho = new TH1D("","pi- sim rho",100,0,1);
          h_xbj_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("xbj_neg_rho");
          TH1D *h_xbj_neg_sim_delta = new TH1D("","pi- sim delta",100,0,1);
          h_xbj_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("xbj_neg_delta");
          h_xbj_neg_sim->Add(h_xbj_neg_sim_incrad,1);
          h_xbj_neg_sim->Add(h_xbj_neg_sim_excrad,1);
          //h_xbj_neg_sim->Add(h_xbj_neg_sim_rho,1);
          h_xbj_neg_sim->Add(h_xbj_neg_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_xbj_pos_sim = new TH1D("","pi+ sim sum",100,0,1);
          TH1D *h_xbj_pos_sim_incrad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_xbj_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("xbj_pos_inc_rad");
          TH1D *h_xbj_pos_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_xbj_pos_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("xbj_pos_inc_norad");
          TH1D *h_xbj_pos_sim_excrad = new TH1D("","pi+ sim exc",100,0,1);
          h_xbj_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("xbj_pos_exc_rad");
          TH1D *h_xbj_pos_sim_rho = new TH1D("","pi+ sim rho",100,0,1);
          h_xbj_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("xbj_pos_rho");
          TH1D *h_xbj_pos_sim_delta = new TH1D("","pi+ sim delta",100,0,1);
          h_xbj_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("xbj_pos_delta");
          h_xbj_pos_sim->Add(h_xbj_pos_sim_incrad,1);
          h_xbj_pos_sim->Add(h_xbj_pos_sim_excrad,1);
          //h_xbj_pos_sim->Add(h_xbj_pos_sim_rho,1);
          h_xbj_pos_sim->Add(h_xbj_pos_sim_delta,1);

          double charge_neg_all = 0,charge_pos_all=0;
          double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;
          if(xbj!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_all += charge;
              double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
              double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
              double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
              //double TE = 1;
              //std::cout<<"neg TE check "<<std::endl;
              double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
              double HMS_cer_eff = 1;
              //double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"neg DE check "<<std::endl;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_xbj_neg = new TH1D("","",100,0,1);
              h_xbj_neg = (TH1D*)root_file_neg->Get("xbj");
              TH1D *h_xbj_neg_bg = new TH1D("","",100,0,1);
              h_xbj_neg_bg = (TH1D*)root_file_neg->Get("xbj_bg");
              //h_xbj_neg_all->Add(h_xbj_neg_bg,-1/(charge*TE));
              //h_xbj_neg_all->Add(h_xbj_neg,1/(charge*TE));
              h_xbj_neg_bg_all->Add(h_xbj_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_xbj_neg_all->Add(h_xbj_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            }//loop over neg runs
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_all+=charge;
              double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
              double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
              double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
              //std::cout<<"pos TE check"<<std::endl;
              double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
              double HMS_cer_eff = 1;
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_xbj_pos = new TH1D("","",100,0,1);
              h_xbj_pos = (TH1D*)root_file_pos->Get("xbj");
              TH1D *h_xbj_pos_bg = new TH1D("","",100,0,1);
              h_xbj_pos_bg = (TH1D*)root_file_pos->Get("xbj_bg");
              //h_xbj_pos_all->Add(h_xbj_pos_bg,-1/(charge*TE));
              //h_xbj_pos_all->Add(h_xbj_pos,1/(charge*TE));
              h_xbj_pos_bg_all->Add(h_xbj_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_xbj_pos_all->Add(h_xbj_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            }//loop over pos runs
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_Dummy_all += charge;
              double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
              double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
              double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
              //double TE = 1;
              //std::cout<<"neg TE check "<<std::endl;
              double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
              double HMS_cer_eff = 1;
              //double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"neg DE check "<<std::endl;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_xbj_neg = new TH1D("","",100,0,1);
              h_xbj_neg = (TH1D*)root_file_neg->Get("xbj");
              TH1D *h_xbj_neg_bg = new TH1D("","",100,0,1);
              h_xbj_neg_bg = (TH1D*)root_file_neg->Get("xbj_bg");
              //h_xbj_neg_all->Add(h_xbj_neg_bg,-1/(charge*TE));
              //h_xbj_neg_all->Add(h_xbj_neg,1/(charge*TE));
              h_xbj_neg_Dummy_bg_all->Add(h_xbj_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_xbj_neg_Dummy_all->Add(h_xbj_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            }//loop over neg runs
            for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_Dummy_all += charge;
              double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
              double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
              double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
              //std::cout<<"pos TE check"<<std::endl;
              double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
              double HMS_cer_eff = 1;
              //double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_xbj_pos = new TH1D("","",100,0,1);
              h_xbj_pos = (TH1D*)root_file_pos->Get("xbj");
              TH1D *h_xbj_pos_bg = new TH1D("","",100,0,1);
              h_xbj_pos_bg = (TH1D*)root_file_pos->Get("xbj_bg");
              //h_xbj_pos_all->Add(h_xbj_pos_bg,-1/(charge*TE));
              //h_xbj_pos_all->Add(h_xbj_pos,1/(charge*TE));
              h_xbj_pos_Dummy_bg_all->Add(h_xbj_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_xbj_pos_Dummy_all->Add(h_xbj_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            }//loop over pos runs
          }//if xbj not 0

          TCanvas *c_counts_pos = new TCanvas();
          h_xbj_pos_all->GetYaxis()->SetTitle("counts");
          h_xbj_pos_all->GetXaxis()->SetRangeUser(0.3,1);
          h_xbj_pos_all->DrawCopy("hist");
          h_xbj_pos_bg_all->SetLineColor(kOrange);
          h_xbj_pos_bg_all->GetXaxis()->SetRangeUser(0.3,1);
          h_xbj_pos_bg_all->DrawCopy("hist same");
          std::string c_counts_pos_name = "results/yield/statistics_q2zcorr/counts_"+q2xz_str_filename+"_pos.pdf";
          c_counts_pos->SaveAs(c_counts_pos_name.c_str());

          TCanvas *c_counts_neg = new TCanvas();
          h_xbj_neg_all->GetYaxis()->SetTitle("counts");
          h_xbj_neg_all->GetXaxis()->SetRangeUser(0.3,1);
          h_xbj_neg_all->DrawCopy("hist");
          h_xbj_neg_bg_all->SetLineColor(kOrange);
          h_xbj_neg_bg_all->DrawCopy("hist same");
          std::string c_counts_neg_name = "results/yield/statistics_q2zcorr/counts_"+q2xz_str_filename+"_neg.pdf";
          c_counts_neg->SaveAs(c_counts_neg_name.c_str());


          h_xbj_neg_all->Scale(1/charge_neg_all);
          h_xbj_pos_all->Scale(1/charge_pos_all);

          h_xbj_neg_bg_all->Scale(1/charge_neg_all);
          h_xbj_pos_bg_all->Scale(1/charge_pos_all);

          //bg subtracted
          h_xbj_neg_all->Add(h_xbj_neg_bg_all,-1.0);
          h_xbj_pos_all->Add(h_xbj_pos_bg_all,-1.0);
          
          h_xbj_neg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          h_xbj_pos_Dummy_all->Scale(1/charge_pos_Dummy_all);

          h_xbj_neg_Dummy_bg_all->Scale(1/charge_neg_Dummy_all);
          h_xbj_pos_Dummy_bg_all->Scale(1/charge_pos_Dummy_all);
          
          h_xbj_neg_Dummy_all->Add(h_xbj_neg_Dummy_bg_all,-1.0);
          h_xbj_pos_Dummy_all->Add(h_xbj_pos_Dummy_bg_all,-1.0);

          h_xbj_neg_all->Add(h_xbj_neg_Dummy_all,-0.245);
          h_xbj_pos_all->Add(h_xbj_pos_Dummy_all,-0.245);

          //bgs from SIMC
          h_xbj_neg_all->Add(h_xbj_neg_sim_excrad,-1);
          h_xbj_pos_all->Add(h_xbj_neg_sim_excrad,-1);
          h_xbj_neg_all->Add(h_xbj_neg_sim_delta,-1);
          h_xbj_pos_all->Add(h_xbj_neg_sim_delta,-1);
          
          auto h_xbj_neg_yield = h_xbj_neg_all;
          auto h_xbj_neg_bg_yield = h_xbj_neg_bg_all;
          auto h_xbj_pos_yield = h_xbj_pos_all;
          auto h_xbj_pos_bg_yield = h_xbj_pos_bg_all;

          TCanvas *c_yield_pos = new TCanvas();
          gStyle->SetOptTitle(0);
          h_xbj_pos_yield->GetYaxis()->SetTitle("yield");
          h_xbj_pos_yield->GetYaxis()->SetTitleSize(0.53);
          h_xbj_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          h_xbj_pos_yield->SetMarkerStyle(4);
          h_xbj_pos_yield->DrawCopy();
          //h_xbj_pos_bg_yield->SetLineColor(kOrange);
          //h_xbj_pos_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          //h_xbj_pos_bg_yield->DrawCopy("hist same");
          h_xbj_pos_sim->DrawCopy("hist same");
          h_xbj_pos_sim_incrad->SetLineColor(kOrange);
          h_xbj_pos_sim_incrad->DrawCopy("hist same");
          h_xbj_pos_sim_excrad->SetLineColor(kBlue);
          h_xbj_pos_sim_excrad->DrawCopy("hist same");
          h_xbj_pos_sim_rho->SetLineColor(kRed);
          h_xbj_pos_sim_rho->DrawCopy("hist same");
          h_xbj_pos_sim_delta->SetLineColor(6);
          h_xbj_pos_sim_delta->DrawCopy("hist same");
          c_yield_pos->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_pos_name = "results/yield/statistics_q2zcorr/yield_"+q2xz_str_filename+"_pos.pdf";
          c_yield_pos->SaveAs(c_yield_pos_name.c_str());

          //h_xbj_neg_all->Rebin(2);
          //h_xbj_pos_all->Rebin(2);
          TCanvas *c_yield_neg = new TCanvas();
          gStyle->SetOptTitle(0);
          h_xbj_neg_yield->GetYaxis()->SetTitle("yield");
          h_xbj_neg_yield->GetYaxis()->SetTitleSize(0.53);
          h_xbj_neg_yield->GetXaxis()->SetRangeUser(0.3,1);
          h_xbj_neg_yield->SetMarkerStyle(4);
          h_xbj_neg_yield->DrawCopy();
          //h_xbj_neg_bg_yield->SetLineColor(kOrange);
          //h_xbj_neg_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          //h_xbj_neg_bg_yield->DrawCopy("hist same");
          h_xbj_neg_sim->DrawCopy("hist same");
          h_xbj_neg_sim_incrad->SetLineColor(kOrange);
          h_xbj_neg_sim_incrad->DrawCopy("hist same");
          h_xbj_neg_sim_excrad->SetLineColor(kBlue);
          h_xbj_neg_sim_excrad->DrawCopy("hist same");
          h_xbj_neg_sim_rho->SetLineColor(kRed);
          h_xbj_neg_sim_rho->DrawCopy("hist same");
          h_xbj_neg_sim_delta->SetLineColor(6);
          h_xbj_neg_sim_delta->DrawCopy("hist same");
          c_yield_neg->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_neg_name = "results/yield/statistics_q2zcorr/yield_"+q2xz_str_filename+"_neg.pdf";
          c_yield_neg->SaveAs(c_yield_neg_name.c_str());

          h_xbj_neg_all->Rebin(2);
          h_xbj_pos_all->Rebin(2);
          h_xbj_neg_sim->Rebin(2);
          h_xbj_pos_sim->Rebin(2);


         // h_xbj_neg_all->Divide(h_xbj_pos_all);
         // h_xbj_neg_sim->Divide(h_xbj_pos_sim);

          int nbins = h_xbj_neg_all->GetXaxis()->GetNbins();

          // TGraphErrors* g_yield_ratio = new TGraphErrors(h_xbj_neg_all);
          TGraphErrors* g_yield_ratio = new TGraphErrors();
          std::string xbj_string = "xbj setting "+(std::to_string(xbj)).substr(0,4);
          g_yield_ratio->SetName(xbj_string.c_str());
          TGraphErrors* g_frag_ratio = new TGraphErrors();
          std::string frag_xbj_string = "frag xbj setting "+(std::to_string(xbj)).substr(0,4);
          g_frag_ratio->SetName(frag_xbj_string.c_str());
          TGraphErrors* g_RD_ratio = new TGraphErrors();
          std::string RD_xbj_string = "RD xbj setting "+(std::to_string(xbj)).substr(0,4);
          g_RD_ratio->SetName(RD_xbj_string.c_str());

          int ii = 0;
          for(int i = 0;i<nbins;i++){
            //std::cout<<i<<std::endl;
            double x = h_xbj_neg_all->GetBinCenter(i)+0.001*i_color;
            double y_neg = h_xbj_neg_all->GetBinContent(i);
            double error_neg = h_xbj_neg_all->GetBinError(i);
            double y_pos = h_xbj_pos_all->GetBinContent(i);
            double error_pos = h_xbj_pos_all->GetBinError(i);
            double y = y_neg/y_pos;
            double error = (y_neg/y_pos)*std::sqrt((error_neg*error_neg)/(y_neg*y_neg)+(error_pos*error_pos)/(y_pos*y_pos));
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            double y_RD = (4*y-1)/(4-y);
            double error_RD = 3*error/((1-y)*(1-y));
            double y_frag = (4*y-1)/(4-y);
            double error_frag = y_frag*std::sqrt((error*error)/(y*y));
            if(y!=0 && error_frag < 0.2){
              g_yield_ratio->SetPoint(ii,x,y);
              g_yield_ratio->SetPointError(ii,0,error);
              g_frag_ratio ->SetPoint(ii,x,y_frag);
              g_frag_ratio->SetPointError(ii,0,error_frag);
              g_RD_ratio ->SetPoint(ii,x,y_RD);
              g_RD_ratio->SetPointError(ii,0,error_RD);
              ii++;
              std::string xbj_str = std::to_string(x);
              if(error_RD<2){
                jout[std::to_string(Q2)][std::to_string(z)][xbj_str]["value"] = y_RD;
                jout[std::to_string(Q2)][std::to_string(z)][xbj_str]["error"] = error_RD;
              }

            }
          }

          int nbins_sim = h_xbj_neg_sim->GetXaxis()->GetNbins();
          TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
          std::string xbj_string_sim = "xbj simc "+(std::to_string(xbj)).substr(0,4);
          g_yield_ratio_sim->SetName(xbj_string_sim.c_str());
          //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_xbj_neg_sim);
          int ii_sim = 0;
          for(int i = 0;i<nbins_sim;i++){
            //std::cout<<i<<std::endl;
            double x = h_xbj_neg_sim->GetBinCenter(i)+0.001*i_color;
            double y = h_xbj_neg_sim->GetBinContent(i);
            double error = h_xbj_neg_sim->GetBinError(i);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_yield_ratio_sim->SetPoint(ii_sim,x,y);
              //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
              ii_sim++;
            }
          }

          // int nbins = h_xbj_pos_all->GetXaxis()->GetNbins();
          // TGraphErrors* g_yield_ratio = new TGraphErrors();
          // std::string xbj_string = "xbj setting "+(std::to_string(xbj)).substr(0,4);
          // g_yield_ratio->SetName(xbj_string.c_str());
          // int ii = 0;
          // for(int i = 0;i<nbins;i++){
          //   //std::cout<<i<<std::endl;
          //   double x = h_xbj_pos_all->GetBinCenter(i);
          //   double y = h_xbj_pos_all->GetBinContent(i);
          //   double error = h_xbj_pos_all->GetBinError(i);
          //   //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
          //   if(y!=0){
          //     g_yield_ratio->SetPoint(ii,x,y);
          //     g_yield_ratio->SetPointError(ii,0,error);
          //     ii++;
          //   }
          // }



          //h_xbj_neg_all->SetLineColor(i_color);
          g_yield_ratio->SetMarkerStyle(4);
          g_yield_ratio->SetMarkerColor(i_color);
          g_yield_ratio->SetLineColor(i_color);
          g_yield_ratio_sim->SetMarkerStyle(5);
          g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
          g_yield_ratio_sim->SetLineColor(i_color);
          //hs->Add(h_z_neg_all);
          //h_z_neg_all->Draw("same");
          mg->Add(g_yield_ratio,"P");
          mg->Add(g_yield_ratio_sim,"L");
          g_frag_ratio->SetMarkerStyle(4);
          g_frag_ratio->SetMarkerColor(i_color);
          g_frag_ratio->SetLineColor(i_color);
          mg_frag->Add(g_frag_ratio,"P");
          g_RD_ratio->SetMarkerStyle(4);
          g_RD_ratio->SetMarkerColor(i_color);
          g_RD_ratio->SetLineColor(i_color);
          mg_RD->Add(g_RD_ratio,"P");
          i_color++;

          TCanvas *c_Q2z_xbj_ratio = new TCanvas(q2z_name.c_str(),q2z_name.c_str(),1900,1000);
          // //h_xbj_neg_all->Draw();
          g_yield_ratio->GetXaxis()->SetRangeUser(0.1,1);
          g_yield_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
          g_yield_ratio->GetXaxis()->SetTitle("xbj");
          g_yield_ratio->GetYaxis()->SetTitle("yield_ratio");
          g_yield_ratio->GetXaxis()->SetTitleSize(0.053);
          g_yield_ratio->GetYaxis()->SetTitleSize(0.053);
          g_yield_ratio->Draw("AP"); 
          g_yield_ratio_sim->Draw("P");
          c_Q2z_xbj_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
          std::string xbjratiopdfname = "results/yield/statistics_q2zcorr/"+q2xz_str_filename+"_ratio.pdf";

          c_Q2z_xbj_ratio->SaveAs(xbjratiopdfname.c_str());
        }//loop over xbj
      c_q2z_ratio->cd();
      c_q2z_ratio->SetTitle(canvas_name.c_str());
      //hs->Draw();
      mg->SetTitle(canvas_name.c_str());
      mg->GetXaxis()->SetTitle("xbj");
      mg->GetYaxis()->SetTitle("ratio");
      mg->GetXaxis()->SetTitleSize(0.053);
      mg->GetYaxis()->SetTitleSize(0.053);
      mg->GetXaxis()->SetLabelSize(0.05);
      mg->GetYaxis()->SetLabelSize(0.05);
      std::string mg_title = canvas_name+",xbj";
      mg->GetHistogram()->SetTitle(canvas_name.c_str());
      mg->GetXaxis()->SetTitle(mg_title.c_str());
      mg->SetMinimum(0.1);
      mg->SetMaximum(0.8);
      mg->Draw("A");
      mg->GetXaxis()->SetLimits(0.3,0.7);
      //auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
      //TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
      //fit->Draw("same");
      std::string ratiopdfname = "results/yield/statistics_q2zcorr/"+canvas_filename+"_ratio.pdf";
      c_q2z_ratio->BuildLegend(0.7,0.7,0.9,0.9);
      c_q2z_ratio->SaveAs(ratiopdfname.c_str());

      TCanvas *c_q2z_RD = new TCanvas();
      mg_RD->SetTitle(canvas_name.c_str());
      mg_RD->GetXaxis()->SetTitle("xbj");
      mg_RD->GetYaxis()->SetTitle("RD ratio");
      std::string mg_RD_title = canvas_name+",xbj";
      mg_RD->GetHistogram()->SetTitle(canvas_name.c_str());
      mg_RD->GetXaxis()->SetTitle(mg_RD_title.c_str());
      mg_RD->Draw("A");
      mg_RD->GetXaxis()->SetLimits(0.3,0.7);
      mg_RD->SetMinimum(0.1);
      mg_RD->SetMaximum(10);
      std::string RDpdfname = "results/yield/statistics_q2zcorr/"+canvas_filename+"_RDratio.pdf";
      c_q2z_RD->BuildLegend();
      c_q2z_RD->SaveAs(RDpdfname.c_str());
      }//if x,Q2 not 0
    }//loop over Q2
  }//loop over z 
  std::string jout_name = "results/yield_ratio_xbj.json";
  std::ofstream ofs_jout(jout_name.c_str());
  ofs_jout<<jout.dump(4)<<std::endl;
  return 0;
}
