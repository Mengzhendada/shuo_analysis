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
#include "Get_all_eff.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>


void acceptance_sys_incrad(){
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j_Q2x;
  }
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  }
  int bins = j_cuts["bins"].get<int>();
  double a_rho = 1;

  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_all = new TMultiGraph(); 
  std::vector<int> i_which_x = {0,1,2,0,3,1,0,2,1,2,3,3};
  std::vector<int> i_which_Q2= {0,0,0,1,0,1,2,1,2,2,1,2};
  int i_whichx = 0; 
  json jout;
  std::ofstream file("results/acceptance/SHMS_p0file_incrad.txt");
  TGraphErrors* g_p0 = new TGraphErrors();
  TGraphErrors* g_p0_z = new TGraphErrors();
  double i_p0 = 0;

  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    auto mg_x_all = new TMultiGraph();
    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::string q2x_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_yieldratio";
      std::string q2x_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_yieldratio";
      int i_color = 1;
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_shmsdp_neg_all = new TH1D("","neg;z;",100,-10,25);
          //THStack *h_shmsdp_neg_all = new THStack("","");  
          h_shmsdp_neg_all->GetXaxis()->SetTitle("shmsdp");
          //h_shmsdp_neg_all->GetYaxis()->SetTitle("counts");
          h_shmsdp_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_shmsdp_pos_all = new TH1D("","pos;z;",100,-10,25);
          //THStack *h_shmsdp_pos_all = new THStack("","");
          h_shmsdp_pos_all->GetXaxis()->SetTitle("shmsdp");
          //h_shmsdp_pos_all->GetYaxis()->SetTitle("counts");
          //h_shmsdp_pos_all->GetYaxis()->SetRangeUser(0,2);
          h_shmsdp_neg_all->Sumw2();
          h_shmsdp_pos_all->Sumw2();
          TH1D* h_shmsdp_neg_bg_all = new TH1D("","neg bg;z;",100,-10,25);
          //THStack *h_shmsdp_neg_all = new THStack("","");  
          h_shmsdp_neg_bg_all->GetXaxis()->SetTitle("shmsdp");
          //h_shmsdp_neg_bg_all->GetYaxis()->SetTitle("bg counts");
          //h_shmsdp_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_shmsdp_pos_bg_all = new TH1D("","pos bg;z;",100,-10,25);
          //THStack *h_shmsdp_pos_all = new THStack("","");
          h_shmsdp_pos_bg_all->GetXaxis()->SetTitle("shmsdp");
          //h_shmsdp_pos_bg_all->GetYaxis()->SetTitle("bg counts");
          //h_shmsdp_pos_bg_all->GetYaxis()->SetRangeUser(0,2);
          h_shmsdp_neg_bg_all->Sumw2();
          h_shmsdp_pos_bg_all->Sumw2();
          TH1D* h_shmsdp_neg_Dummy_all = new TH1D("","neg;z;",100,-10,25);
          h_shmsdp_neg_Dummy_all->GetXaxis()->SetTitle("shmsdp");
          h_shmsdp_neg_Dummy_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_shmsdp_pos_Dummy_all = new TH1D("","pos;z;",100,-10,25);
          h_shmsdp_pos_Dummy_all->GetXaxis()->SetTitle("shmsdp");
          h_shmsdp_neg_Dummy_all->Sumw2();
          h_shmsdp_pos_Dummy_all->Sumw2();
          TH1D* h_shmsdp_neg_bg_Dummy_all = new TH1D("","neg bg;z;",100,-10,25);
          h_shmsdp_neg_bg_Dummy_all->GetXaxis()->SetTitle("shmsdp");
          TH1D* h_shmsdp_pos_bg_Dummy_all = new TH1D("","pos bg;z;",100,-10,25);
          h_shmsdp_pos_bg_Dummy_all->GetXaxis()->SetTitle("shmsdp");
          h_shmsdp_neg_bg_Dummy_all->Sumw2();
          h_shmsdp_pos_bg_Dummy_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_shmsdp_neg_sim = new TH1D("","pi- sim sum",100,-10,25);
          TH1D *h_shmsdp_neg_sim_incrad = new TH1D("","pi- sim SIDIS",100,-10,25);
          h_shmsdp_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("shmsdp_neg_inc_rad");
          TH1D *h_shmsdp_neg_sim_incnorad = new TH1D("","pi- sim SIDIS",100,-10,25);
          h_shmsdp_neg_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("shmsdp_neg_inc_norad");
          TH1D *h_shmsdp_neg_sim_excrad = new TH1D("","pi- sim exc",100,-10,25);
          h_shmsdp_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("shmsdp_neg_exc_rad");
          TH1D *h_shmsdp_neg_sim_rho = new TH1D("","pi- sim rho",100,-10,25);
          h_shmsdp_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("shmsdp_neg_rho");
          TH1D *h_shmsdp_neg_sim_delta = new TH1D("","pi- sim delta",100,-10,25);
          h_shmsdp_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("shmsdp_neg_delta");
          h_shmsdp_neg_sim->Add(h_shmsdp_neg_sim_incrad,1);
          //h_shmsdp_neg_sim->Add(h_shmsdp_neg_sim_excrad,1);
          //h_shmsdp_neg_sim->Add(h_shmsdp_neg_sim_rho,1);
          //h_shmsdp_neg_sim->Add(h_shmsdp_neg_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_shmsdp_pos_sim = new TH1D("","pi+ sim sum",100,-10,25);
          TH1D *h_shmsdp_pos_sim_incrad = new TH1D("","pi+ sim SIDIS",100,-10,25);
          h_shmsdp_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("shmsdp_pos_inc_rad");
          TH1D *h_shmsdp_pos_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,-10,25);
          h_shmsdp_pos_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("shmsdp_pos_inc_norad");
          TH1D *h_shmsdp_pos_sim_excrad = new TH1D("","pi+ sim exc",100,-10,25);
          h_shmsdp_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("shmsdp_pos_exc_rad");
          TH1D *h_shmsdp_pos_sim_rho = new TH1D("","pi+ sim rho",100,-10,25);
          h_shmsdp_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("shmsdp_pos_rho");
          TH1D *h_shmsdp_pos_sim_delta = new TH1D("","pi+ sim delta",100,-10,25);
          h_shmsdp_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("shmsdp_pos_delta");
          h_shmsdp_pos_sim->Add(h_shmsdp_pos_sim_incrad,1);
          //h_shmsdp_pos_sim->Add(h_shmsdp_pos_sim_excrad,1);
          //h_shmsdp_pos_sim->Add(h_shmsdp_pos_sim_rho,1);
          //h_shmsdp_pos_sim->Add(h_shmsdp_pos_sim_delta,1);

          double charge_neg_all = 0,charge_pos_all=0;
          double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_all += charge;
              //double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
              //double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
              //double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
              //double TE = 1;
              //std::cout<<"neg TE check "<<std::endl;
              //double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
              //double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              //double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              //double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"neg DE check "<<std::endl;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_shmsdp_neg = new TH1D("","",100,-10,25);
              h_shmsdp_neg = (TH1D*)root_file_neg->Get("shmsdp");
              TH1D *h_shmsdp_neg_bg = new TH1D("","",100,-10,25);
              h_shmsdp_neg_bg = (TH1D*)root_file_neg->Get("shmsdp_bg");
              //h_shmsdp_neg_all->Add(h_shmsdp_neg_bg,-1/(charge*TE));
              //h_shmsdp_neg_all->Add(h_shmsdp_neg,1/(charge*TE));
              //h_shmsdp_neg_bg_all->Add(h_shmsdp_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff*HMS_cer_eff));
              //h_shmsdp_neg_all->Add(h_shmsdp_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff*HMS_cer_eff));
              //h_shmsdp_neg_bg_all->Add(h_shmsdp_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff));
              //h_shmsdp_neg_all->Add(h_shmsdp_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff));
              double EFF = Get_all_eff(RunNumber);
              h_shmsdp_neg_bg_all->Add(h_shmsdp_neg_bg,1/(6*EFF));
              h_shmsdp_neg_all->Add(h_shmsdp_neg,1/EFF);
            }//loop over neg runs
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_all+=charge;
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_shmsdp_pos = new TH1D("","",100,-10,25);
              h_shmsdp_pos = (TH1D*)root_file_pos->Get("shmsdp");
              TH1D *h_shmsdp_pos_bg = new TH1D("","",100,-10,25);
              h_shmsdp_pos_bg = (TH1D*)root_file_pos->Get("shmsdp_bg");
              double EFF = Get_all_eff(RunNumber);
              h_shmsdp_pos_bg_all->Add(h_shmsdp_pos_bg,1/(6*EFF));
              h_shmsdp_pos_all->Add(h_shmsdp_pos,1/EFF);
            }//loop over pos runs
            for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
              int RunNumber = *it;
              std::cout<<"Dummy"<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_Dummy_all += charge;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_shmsdp_neg = new TH1D("","",100,-10,25);
              h_shmsdp_neg = (TH1D*)root_file_neg->Get("shmsdp");
              TH1D *h_shmsdp_neg_bg = new TH1D("","",100,-10,25);
              h_shmsdp_neg_bg = (TH1D*)root_file_neg->Get("shmsdp_bg");
              //h_shmsdp_neg_bg_Dummy_all->Add(h_shmsdp_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff*HMS_cer_eff));
              //h_shmsdp_neg_Dummy_all->Add(h_shmsdp_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff*HMS_cer_eff));
              double EFF = Get_all_eff(RunNumber);
              h_shmsdp_neg_bg_Dummy_all->Add(h_shmsdp_neg_bg,1/(6*EFF));
              h_shmsdp_neg_Dummy_all->Add(h_shmsdp_neg,1/EFF);
            }//loop over neg runs
            for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
              int RunNumber = *it;
              std::cout<<"Dummy neg "<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_Dummy_all+=charge;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_shmsdp_pos = new TH1D("","",100,-10,25);
              h_shmsdp_pos = (TH1D*)root_file_pos->Get("shmsdp");
              TH1D *h_shmsdp_pos_bg = new TH1D("","",100,-10,25);             
              h_shmsdp_pos_bg = (TH1D*)root_file_pos->Get("shmsdp_bg");
              double EFF = Get_all_eff(RunNumber);
              h_shmsdp_pos_bg_Dummy_all->Add(h_shmsdp_pos_bg,1/(6*EFF));
              h_shmsdp_pos_Dummy_all->Add(h_shmsdp_pos,1/EFF);
            }//loop over pos runs
          }//if z not 0

          //TCanvas *c_counts_pos = new TCanvas();
          //h_shmsdp_pos_all->GetYaxis()->SetTitle("counts");
          //h_shmsdp_pos_all->GetXaxis()->SetRangeUser(0.3,1);
          //h_shmsdp_pos_all->DrawCopy("hist");
          //h_shmsdp_pos_bg_all->SetLineColor(kOrange);
          //h_shmsdp_pos_bg_all->GetXaxis()->SetRangeUser(0.3,1);
          //h_shmsdp_pos_bg_all->DrawCopy("hist same");
          //std::string c_counts_pos_name = "results/yield/statistics_corr/counts_"+q2xz_str_filename+"_pos.pdf";
          //c_counts_pos->SaveAs(c_counts_pos_name.c_str());

          //TCanvas *c_counts_neg = new TCanvas();
          //h_shmsdp_neg_all->GetYaxis()->SetTitle("counts");
          //h_shmsdp_neg_all->GetXaxis()->SetRangeUser(0.3,1);
          //h_shmsdp_neg_all->DrawCopy("hist");
          //h_shmsdp_neg_bg_all->SetLineColor(kOrange);
          //h_shmsdp_neg_bg_all->DrawCopy("hist same");
          //std::string c_counts_neg_name = "results/yield/statistics_corr/counts_"+q2xz_str_filename+"_neg.pdf";
          //c_counts_neg->SaveAs(c_counts_neg_name.c_str());


          //h_shmsdp_neg_all->Scale(1/charge_neg_all);
          //h_shmsdp_pos_all->Scale(1/charge_pos_all);

          //h_shmsdp_neg_bg_all->Scale(1/charge_neg_all);
          //h_shmsdp_pos_bg_all->Scale(1/charge_pos_all);
          //
          //h_shmsdp_neg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          //h_shmsdp_pos_Dummy_all->Scale(1/charge_pos_Dummy_all);

          //h_shmsdp_neg_bg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          //h_shmsdp_pos_bg_Dummy_all->Scale(1/charge_pos_Dummy_all);

          //bg subtracted
          //h_shmsdp_neg_all->Add(h_shmsdp_neg_bg_all,-1.0);
          //h_shmsdp_pos_all->Add(h_shmsdp_pos_bg_all,-1.0);

          //Dummy subtract
          h_shmsdp_neg_Dummy_all->Add(h_shmsdp_neg_bg_Dummy_all,-1.0);
          h_shmsdp_pos_Dummy_all->Add(h_shmsdp_pos_bg_Dummy_all,-1.0);
          //h_shmsdp_neg_Dummy_all->Scale(charge_neg_all/charge_neg_Dummy_all);
          //h_shmsdp_pos_Dummy_all->Scale(charge_pos_all/charge_pos_Dummy_all);

          //h_shmsdp_neg_all->Add(h_shmsdp_neg_Dummy_all,-0.245);
          //h_shmsdp_pos_all->Add(h_shmsdp_pos_Dummy_all,-0.245);

          //bgs from SIMC
          //h_shmsdp_neg_all->Add(h_shmsdp_neg_sim_excrad,-1);
          //h_shmsdp_pos_all->Add(h_shmsdp_pos_sim_excrad,-1);
          //h_shmsdp_neg_all->Add(h_shmsdp_neg_sim_delta,-1);
          //h_shmsdp_pos_all->Add(h_shmsdp_pos_sim_delta,-1);

          //auto h_shmsdp_neg_yield = h_shmsdp_neg_all->Scale(1/charge_neg_all);
          //auto h_shmsdp_neg_bg_yield = h_shmsdp_neg_bg_all->Scale(1/charge_neg_all);
          //auto h_shmsdp_pos_yield = h_shmsdp_pos_all->Scale(1/charge_pos_all);
          //auto h_shmsdp_pos_bg_yield = h_shmsdp_pos_bg_all->Scale(1/charge_pos_all);
          //auto h_shmsdp_neg_Dummy_yield = h_neg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          //auto h_shmsdp_pos_Dummy_yield = h_pos_Dummy_all->Scale(1/charge_pos_Dummy_all);


          auto h_shmsdp_neg_incnorad = h_shmsdp_neg_sim_incnorad;
          auto h_shmsdp_neg_incrad = h_shmsdp_neg_sim_incrad;
          auto h_shmsdp_pos_incnorad = h_shmsdp_pos_sim_incnorad;
          auto h_shmsdp_pos_incrad = h_shmsdp_pos_sim_incrad;

          /*
          //h_shmsdp_neg_all->Rebin(5);
          //h_shmsdp_pos_all->Rebin(5);
          //h_shmsdp_neg_bg_all->Rebin(5);
          //h_shmsdp_pos_bg_all->Rebin(5);
          h_shmsdp_neg_Dummy_all->Rebin(5);
          h_shmsdp_pos_Dummy_all->Rebin(5);
          h_shmsdp_neg_sim->Rebin(5);
          h_shmsdp_pos_sim->Rebin(5);
          h_shmsdp_neg_incnorad->Rebin(5);
          h_shmsdp_neg_incrad->Rebin(5);
          h_shmsdp_neg_sim_excrad->Rebin(5);
          h_shmsdp_neg_sim_delta->Rebin(5);
          //h_shmsdp_neg_sim_rho->Rebin(5);
          h_shmsdp_pos_incnorad->Rebin(5);
          h_shmsdp_pos_incrad->Rebin(5);
          h_shmsdp_pos_sim_excrad->Rebin(5);
          h_shmsdp_pos_sim_delta->Rebin(5);
          //h_shmsdp_pos_sim_rho->Rebin(5);
          //h_xs_z_neg_sim->RebinX(5);
          //h_xs_z_neg_sim->RebinY(5);
          //h_xs_z_pos_sim->RebinX(5);
          //h_xs_z_pos_sim->RebinY(5);
          */  
          //TGraph* g_xs_z_neg_sim = new TGraph(h_xs_z_neg_sim);
          //TGraph* g_xs_z_pos_sim = new TGraph(h_xs_z_pos_sim);
          //TGraph* g_xs_z_neg_H2_sim = new TGraph(h_xs_z_neg_H2_sim);
          //TGraph* g_xs_z_pos_H2_sim = new TGraph(h_xs_z_pos_H2_sim);
          ///auto rp_radia_corr_neg = new TRatioPlot(h_shmsdp_neg_sim_incnorad,h_shmsdp_neg_sim_incrad);
          ///auto rp_radia_corr_pos = new TRatioPlot(h_shmsdp_pos_sim_incnorad,h_shmsdp_pos_sim_incrad);
          //TH1* rp_radia_corr_neg = (TH1*)h_shmsdp_neg_incnorad->Clone("rp_radia_corr");
          //TH1* rp_radia_corr_pos = (TH1*)h_shmsdp_pos_incnorad->Clone("rp_radia_corr");
          //rp_radia_corr_neg ->Divide(h_shmsdp_neg_incnorad,h_shmsdp_neg_incrad);          
          //rp_radia_corr_pos ->Divide(h_shmsdp_pos_incnorad,h_shmsdp_pos_incrad);          
          //TCanvas* c_radia_corr_sim = new TCanvas();
          //c_radia_corr_sim->Divide(2,1);
          //c_radia_corr_sim->cd(1);
          //rp_radia_corr_neg->Draw();
          //c_radia_corr_sim->cd(2);
          //rp_radia_corr_pos->Draw();
          //std::string c_radia_corr_sim_name = "results/yield/statistics_corr/radia_corr_"+q2xz_str_filename+".pdf";
          //c_radia_corr_sim->SaveAs(c_radia_corr_sim_name.c_str());

          //h_shmsdp_neg_yield->Multiply(h_shmsdp_neg_yield,rp_radia_corr_neg);
          //h_shmsdp_pos_yield->Multiply(h_shmsdp_pos_yield,rp_radia_corr_pos);

          TGraphErrors* g_z_pos_yield = new TGraphErrors();
          g_z_pos_yield->SetTitle("pi+ data yield");
          TGraphErrors* g_z_pos_bg_yield = new TGraphErrors();
          g_z_pos_bg_yield->SetTitle("pi+ acc bg yield");
          TGraphErrors* g_z_pos_yield_corr = new TGraphErrors();
          g_z_pos_yield_corr->SetTitle("pi- data yield");
          int nbins_yield_pos = h_shmsdp_pos_all->GetXaxis()->GetNbins();
          int ii_yield_pos = 0;
          for(int i = 0;i<nbins_yield_pos;i++){
            //std::cout<<i<<std::endl;
            double x = h_shmsdp_pos_all->GetBinCenter(i+1);
            double y = h_shmsdp_pos_all->GetBinContent(i+1)/charge_pos_all;
            double y_bg = h_shmsdp_pos_bg_all->GetBinContent(i+1)/charge_pos_all;
            double y_dummy = h_shmsdp_pos_Dummy_all->GetBinContent(i+1)/charge_pos_Dummy_all;
            //double error = h_shmsdp_pos_yield->GetBinError(i);
            double error = h_shmsdp_pos_all->GetBinError(i+1)/charge_pos_all;
            double error_bg = h_shmsdp_pos_bg_all->GetBinError(i+1)/charge_pos_all;
            double error_dummy = h_shmsdp_pos_Dummy_all->GetBinError(i+1)/charge_pos_Dummy_all;
            double y_corr = y-y_bg-0.245*y_dummy;
            double error_corr = sqrt(error*error+error_bg*error_bg+error_dummy*error_dummy);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_z_pos_yield->SetPoint(ii_yield_pos,x,y);
              g_z_pos_yield->SetPointError(ii_yield_pos,0,error);
              g_z_pos_bg_yield->SetPoint(ii_yield_pos,x,y_bg);
              g_z_pos_bg_yield->SetPointError(ii_yield_pos,0,error_bg);
              g_z_pos_yield_corr->SetPoint(ii_yield_pos,x,y_corr);
              g_z_pos_yield_corr->SetPointError(ii_yield_pos,0,error_corr);
              ii_yield_pos++;
            }
          }
          TCanvas *c_yield_pos = new TCanvas();
          gStyle->SetOptTitle(0);
          //h_shmsdp_pos_yield->GetYaxis()->SetTitle("yield");
          //h_shmsdp_pos_yield->GetYaxis()->SetTitleSize(0.53);
          //h_shmsdp_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          //h_shmsdp_pos_yield->SetMarkerStyle(4);
          //h_shmsdp_pos_yield->DrawCopy();
          ////h_shmsdp_pos_bg_yield->SetLineColor(kOrange);
          ////h_shmsdp_pos_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          ////h_shmsdp_pos_bg_yield->DrawCopy("hist same");
          //h_shmsdp_pos_sim->DrawCopy("hist same");
          //h_shmsdp_pos_sim_incrad->SetLineColor(kOrange);
          //h_shmsdp_pos_sim_incrad->SetTitle("pi+ sim SIDIS");
          //h_shmsdp_pos_sim_incrad->DrawCopy("hist same");
          //h_shmsdp_pos_sim_excrad->SetLineColor(kBlue);
          //h_shmsdp_pos_sim_excrad->SetTitle("pi+ sim exc");
          //h_shmsdp_pos_sim_excrad->DrawCopy("hist same");
          //h_shmsdp_pos_sim_rho->SetLineColor(kRed);
          //h_shmsdp_pos_sim_rho->SetTitle("pi+ sim rho");
          //h_shmsdp_pos_sim_rho->DrawCopy("hist same");
          //h_shmsdp_pos_sim_delta->SetLineColor(6);
          //h_shmsdp_pos_sim_delta->SetTitle("pi+ sim delta");
          //h_shmsdp_pos_sim_delta->DrawCopy("hist same");
          TMultiGraph* mg_z_yield_pos = new TMultiGraph(); 
          //TGraphErrors* g_z_pos_yield = new TGraphErrors(h_shmsdp_pos_yield);
          g_z_pos_yield->GetYaxis()->SetTitle("yield");
          g_z_pos_yield->GetYaxis()->SetTitleSize(0.53);
          g_z_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_z_pos_yield->SetMarkerStyle(4);
          g_z_pos_bg_yield->SetTitle("acc bg");
          g_z_pos_bg_yield->SetMarkerStyle(5);
          mg_z_yield_pos->Add(g_z_pos_yield,"P");
          mg_z_yield_pos->Add(g_z_pos_bg_yield,"P");
          g_z_pos_yield_corr->SetMarkerStyle(4);
          g_z_pos_yield_corr->SetMarkerColor(kRed);
          g_z_pos_yield_corr->SetTitle("data yield final");
          mg_z_yield_pos->Add(g_z_pos_yield_corr,"P");
          TH1D* h_shmsdp_pos_Dummy_yield = (TH1D*)h_shmsdp_pos_Dummy_all->Clone();
          h_shmsdp_pos_Dummy_yield->Scale(0.245/charge_pos_Dummy_all);
          TGraphErrors* g_z_pos_yield_Dummy = new TGraphErrors(h_shmsdp_pos_Dummy_yield);
          g_z_pos_yield_Dummy->SetMarkerStyle(2);
          g_z_pos_yield_Dummy->SetTitle("Dummy");
          mg_z_yield_pos->Add(g_z_pos_yield_Dummy);
          TGraph* g_z_pos_sim = new TGraph(h_shmsdp_pos_sim);
          TGraph* g_z_pos_sim_incrad = new TGraph(h_shmsdp_pos_sim_incrad);
          TGraph* g_z_pos_sim_incnorad = new TGraph(h_shmsdp_pos_sim_incnorad);
          //TGraph* g_z_pos_sim_excrad = new TGraph(h_shmsdp_pos_sim_excrad);
          //TGraph* g_z_pos_sim_rho = new TGraph(h_shmsdp_pos_sim_rho);
          //TGraph* g_z_pos_sim_delta = new TGraph(h_shmsdp_pos_sim_delta);
          mg_z_yield_pos->Add(g_z_pos_sim); 
          g_z_pos_sim_incrad->SetLineColor(kOrange);
          g_z_pos_sim_incrad->SetTitle("pi+ sim SIDIS");
          mg_z_yield_pos->Add(g_z_pos_sim_incrad); 
          //g_z_pos_sim_excrad->SetLineColor(kBlue);
          //g_z_pos_sim_excrad->SetTitle("pi+ sim exc");
          //mg_z_yield_pos->Add(g_z_pos_sim_excrad); 
          //g_z_pos_sim_rho->SetLineColor(kRed);
          //g_z_pos_sim_rho->SetTitle("pi+ sim rho");
          //mg_z_yield_pos->Add(g_z_pos_sim_rho); 
          //g_z_pos_sim_delta->SetLineColor(46);
          //g_z_pos_sim_delta->SetTitle("pi+ sim delta");
          //mg_z_yield_pos->Add(g_z_pos_sim_delta); 
          g_z_pos_sim_incnorad->SetLineColor(40);
          g_z_pos_sim_incnorad->SetTitle("pi+ sim inc norad");
          mg_z_yield_pos->Add(g_z_pos_sim_incnorad,"L");
          mg_z_yield_pos->SetTitle(q2xz_str.c_str());
          mg_z_yield_pos->GetYaxis()->SetTitle("yield");
          mg_z_yield_pos->GetXaxis()->SetTitle("shmsdp");
          mg_z_yield_pos->Draw("AL");
          c_yield_pos->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_pos_name = "results/acceptance/"+q2xz_str_filename+"_yield_pos.pdf";
          c_yield_pos->SaveAs(c_yield_pos_name.c_str());

          //h_shmsdp_neg_all->Rebin(2);
          //h_shmsdp_pos_all->Rebin(2);
          TGraphErrors* g_z_neg_yield = new TGraphErrors();
          g_z_neg_yield->SetTitle("pi- data before");
          TGraphErrors* g_z_neg_yield_corr = new TGraphErrors();
          g_z_neg_yield_corr->SetTitle("pi- data yield");
          TGraphErrors* g_z_neg_bg_yield = new TGraphErrors();
          g_z_neg_bg_yield->SetTitle("pi- acc bg yield");
          int nbins_yield_neg = h_shmsdp_neg_all->GetXaxis()->GetNbins();
          int ii_yield_neg = 0;
          for(int i = 0;i<nbins_yield_neg;i++){
            //std::cout<<i<<std::endl;
            double x = h_shmsdp_neg_all->GetBinCenter(i+1);
            double y = h_shmsdp_neg_all->GetBinContent(i+1)/charge_neg_all;
            double y_bg = h_shmsdp_neg_bg_all->GetBinContent(i+1)/charge_neg_all;
            double y_dummy = h_shmsdp_neg_Dummy_all->GetBinContent(i+1)/charge_neg_Dummy_all;
            //double error = h_shmsdp_neg_yield->GetBinError(i);
            double error = h_shmsdp_neg_all->GetBinError(i+1)/charge_neg_all;
            double error_bg = h_shmsdp_neg_bg_all->GetBinError(i+1)/charge_neg_all;
            double error_dummy = h_shmsdp_neg_Dummy_all->GetBinError(i+1)/charge_neg_Dummy_all;
            double y_corr = y-y_bg-0.245*y_dummy;
            double error_corr = sqrt(error*error+error_bg*error_bg+error_dummy*error_dummy);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_z_neg_yield->SetPoint(ii_yield_neg,x,y);
              g_z_neg_yield->SetPointError(ii_yield_neg,0,error);
              g_z_neg_bg_yield->SetPoint(ii_yield_neg,x,y_bg);
              g_z_neg_bg_yield->SetPointError(ii_yield_neg,0,error_bg);
              g_z_neg_yield_corr->SetPoint(ii_yield_neg,x,y_corr);
              g_z_neg_yield_corr->SetPointError(ii_yield_neg,0,error_corr);
              ii_yield_neg++;
            }
          }
          TCanvas *c_yield_neg = new TCanvas();
          gStyle->SetOptTitle(0);
          //g_z_neg_yield->DrawCopy();
          //h_shmsdp_neg_bg_yield->SetLineColor(kOrange);
          //h_shmsdp_neg_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          //h_shmsdp_neg_bg_yield->DrawCopy("hist same");
          TMultiGraph* mg_z_yield_neg = new TMultiGraph(); 
          //TGraphErrors* g_z_neg_yield = new TGraphErrors(h_shmsdp_neg_yield);
          g_z_neg_yield->GetYaxis()->SetTitle("yield");
          g_z_neg_yield->GetYaxis()->SetTitleSize(0.53);
          g_z_neg_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_z_neg_yield->SetMarkerStyle(4);
          g_z_neg_bg_yield->SetTitle("acc bg");
          g_z_neg_bg_yield->SetMarkerStyle(5);
          mg_z_yield_neg->Add(g_z_neg_yield,"P");
          mg_z_yield_neg->Add(g_z_neg_bg_yield,"P");
          g_z_neg_yield_corr->SetMarkerStyle(4);
          g_z_neg_yield_corr->SetMarkerColor(kBlue);
          g_z_neg_yield_corr->SetTitle("data yield final");
          mg_z_yield_neg->Add(g_z_neg_yield_corr,"P");
          TH1D* h_shmsdp_neg_Dummy_yield = (TH1D*)h_shmsdp_neg_Dummy_all->Clone();
          h_shmsdp_neg_Dummy_yield->Scale(0.245/charge_neg_Dummy_all);
          TGraphErrors* g_z_neg_yield_Dummy = new TGraphErrors(h_shmsdp_neg_Dummy_yield);
          g_z_neg_yield_Dummy->SetTitle("Dummy");
          g_z_neg_yield_Dummy->SetMarkerStyle(2);
          mg_z_yield_neg->Add(g_z_neg_yield_Dummy,"P");
          TGraph* g_z_neg_sim = new TGraph(h_shmsdp_neg_sim);
          TGraph* g_z_neg_sim_incrad = new TGraph(h_shmsdp_neg_sim_incrad);
          TGraph* g_z_neg_sim_incnorad = new TGraph(h_shmsdp_neg_sim_incnorad);
          //TGraph* g_z_neg_sim_excrad = new TGraph(h_shmsdp_neg_sim_excrad);
          //TGraph* g_z_neg_sim_rho = new TGraph(h_shmsdp_neg_sim_rho);
          //TGraph* g_z_neg_sim_delta = new TGraph(h_shmsdp_neg_sim_delta);
          mg_z_yield_neg->Add(g_z_neg_sim,"L"); 
          g_z_neg_sim_incrad->SetLineColor(kOrange);
          g_z_neg_sim_incrad->SetTitle("pi- sim SIDIS");
          mg_z_yield_neg->Add(g_z_neg_sim_incrad,"L"); 
          //g_z_neg_sim_excrad->SetLineColor(kBlue);
          //g_z_neg_sim_excrad->SetTitle("pi- sim exc");
          //mg_z_yield_neg->Add(g_z_neg_sim_excrad,"L"); 
          //g_z_neg_sim_rho->SetLineColor(kRed);
          //g_z_neg_sim_rho->SetTitle("pi- sim rho");
          //mg_z_yield_neg->Add(g_z_neg_sim_rho,"L"); 
          //g_z_neg_sim_delta->SetLineColor(46);
          //g_z_neg_sim_delta->SetTitle("pi- sim delta");
          //mg_z_yield_neg->Add(g_z_neg_sim_delta,"L"); 
          g_z_neg_sim_incnorad->SetLineColor(40);
          g_z_neg_sim_incnorad->SetTitle("pi+ sim inc norad");
          mg_z_yield_neg->Add(g_z_neg_sim_incnorad,"L");
          mg_z_yield_neg->SetTitle(q2xz_str.c_str());
          mg_z_yield_neg->GetYaxis()->SetTitle("yield");
          mg_z_yield_neg->GetXaxis()->SetTitle("shmsdp");
          mg_z_yield_neg->Draw("A");
          //mg_z_yield_neg->GetXaxis()->SetLimits(0.3,0.7);
          //h_shmsdp_neg_sim->DrawCopy("hist same");
          //h_shmsdp_neg_sim_incrad->SetLineColor(kOrange);
          //h_shmsdp_neg_sim_incrad->SetTitle("pi- sim SIDIS");
          //h_shmsdp_neg_sim_incrad->DrawCopy("hist same");
          //h_shmsdp_neg_sim_excrad->SetLineColor(kBlue);
          //h_shmsdp_neg_sim_excrad->SetTitle("pi- sim exc");
          //h_shmsdp_neg_sim_excrad->DrawCopy("hist same");
          //h_shmsdp_neg_sim_rho->SetLineColor(kRed);
          //h_shmsdp_neg_sim_rho->SetTitle("pi- sim rho");
          //h_shmsdp_neg_sim_rho->DrawCopy("hist same");
          //h_shmsdp_neg_sim_delta->SetLineColor(6);
          //h_shmsdp_neg_sim_delta->SetTitle("pi- sim delta");
          //h_shmsdp_neg_sim_delta->DrawCopy("hist same");
          c_yield_neg->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_neg_name = "results/acceptance/"+q2xz_str_filename+"_yield_neg.pdf";
          c_yield_neg->SaveAs(c_yield_neg_name.c_str());

          /*
             auto h_pos_yield = (TH1D*)h_shmsdp_pos_all->Clone("pos yield");
             auto h_neg_yield = (TH1D*)h_shmsdp_neg_all->Clone("neg yield");
             h_pos_yield->Add(h_neg_yield,-1);
             TGraphErrors *g_fragmentation = new TGraphErrors();
             int n_frag = h_pos_yield->GetXaxis()->GetNbins();
             int i_frag = 0;
             for(int i = 0;i<n_frag;i++){
             double x = h_shmsdp_neg_all->GetBinCenter(i);
             double y = h_shmsdp_neg_all->GetBinContent(i);
             double error = h_shmsdp_neg_all->GetBinError(i);
             if(y!=0 && error< 0.2){
             double y_frag = y*(1+x)/(2*x);
             g_fragmentation->SetPoint(i_frag,x,y_frag);
             g_fragmentation->SetPointError(i_frag,0,error);
             i_frag++;
             }
             }
             TCanvas *c_D_fragmentation = new TCanvas();
             h_pos_yield->Draw("AP");
          //g_fragmentation->Draw("AP");
          std::string c_D_fragmentation_name = "results/yield/statistics_corr/fragmentation_"+q2xz_str_filename+".pdf";
          c_D_fragmentation->SaveAs(c_D_fragmentation_name.c_str());
          */
          //h_shmsdp_neg_all->Rebin(2);
          //h_shmsdp_pos_all->Rebin(2);
          //h_shmsdp_neg_sim->Rebin(2);
          //h_shmsdp_pos_sim->Rebin(2);

          //// h_neg_q2x->Sumw2();
          //// auto rp = new TRatioPlot(h_shmsdp_neg_all,h_shmsdp_pos_all);
          //// c_Q2x_ratio->SetTicks(0,1);
          //// rp->Draw();
          //// rp->GetLowerRefGraph()->SetMinimum(0);
          //// rp->GetLowerRefGraph()->SetMaximum(1);

          h_shmsdp_neg_all->Sumw2();
          h_shmsdp_pos_all->Sumw2();

          //h_shmsdp_neg_all->Divide(h_shmsdp_pos_all);
          h_shmsdp_neg_sim_incrad->Divide(h_shmsdp_pos_sim_incrad);
          //h_shmsdp_neg_sim_incnorad->Divide(h_shmsdp_pos_sim_incnorad);

          std::string neg_yield_str = q2xz_str_filename+"_neg";
          h_shmsdp_neg_all->SetTitle(neg_yield_str.c_str());
          std::string pos_yield_str = q2xz_str_filename+"_pos";
          h_shmsdp_pos_all->SetTitle(pos_yield_str.c_str());
          h_shmsdp_neg_all->GetYaxis()->SetTitle("pi- yield");
          TFile* yield_file = new TFile("results/yield_all.root","UPDATE");
          h_shmsdp_neg_all->Write();
          h_shmsdp_pos_all->Write();
          yield_file->Close();  

          int nbins = h_shmsdp_neg_all->GetXaxis()->GetNbins();

          // TGraphErrors* g_yield_ratio = new TGraphErrors(h_shmsdp_neg_all);
          TGraphErrors* g_yield_ratio = new TGraphErrors();
          std::string z_string = "R_Y z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio->SetName(z_string.c_str());
          g_yield_ratio->SetTitle((std::to_string(RunGroup)).c_str());

          TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
          std::string z_string_sim = "z simc "+(std::to_string(z)).substr(0,4);
          g_yield_ratio_sim->SetName(z_string_sim.c_str());
          //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_shmsdp_neg_sim_incnorad);
          TGraphErrors* g_datasimratio_ratio = new TGraphErrors();
          //std::string datasimratio_z_string = "data yield ratio/ sim yield ratio";//" "+(std::to_string(z)).substr(0,4);
          //g_datasimratio_ratio->SetName(datasimratio_z_string.c_str());
          g_datasimratio_ratio->SetTitle((std::to_string(RunGroup)).c_str());
          int ii_sim = 0;
          int ii = 0;
          for(int i = 0;i<nbins;i++){
            //std::cout<<i<<std::endl;
            double x = h_shmsdp_neg_all->GetBinCenter(i+1);//+0.001*i_color;
            double y_neg_D2 = h_shmsdp_neg_all->GetBinContent(i+1)/charge_neg_all;
            double y_neg_bg_D2 = h_shmsdp_neg_bg_all->GetBinContent(i+1)/charge_neg_all;
            double y_neg_Dummy = h_shmsdp_neg_Dummy_all->GetBinContent(i+1)/charge_neg_Dummy_all;
            double y_neg_delta = h_shmsdp_neg_sim_delta->GetBinContent(i+1);
            double y_neg_exc = h_shmsdp_neg_sim_excrad->GetBinContent(i+1);
            double y_neg_rho = h_shmsdp_neg_sim_rho->GetBinContent(i+1);
            double y_neg = y_neg_D2-y_neg_bg_D2-0.245*y_neg_Dummy-y_neg_delta-y_neg_exc-a_rho*y_neg_rho;
            double error_neg_D2 = h_shmsdp_neg_all->GetBinError(i+1)/charge_neg_all;
            double error_neg_bg_D2 = h_shmsdp_neg_bg_all->GetBinError(i+1)/charge_neg_all;
            double error_neg_Dummy = h_shmsdp_neg_Dummy_all->GetBinError(i+1)/charge_neg_Dummy_all;
            double error_neg = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy);
            double y_pos_D2 = h_shmsdp_pos_all->GetBinContent(i+1)/charge_pos_all;
            double y_pos_bg_D2 = h_shmsdp_pos_bg_all->GetBinContent(i+1)/charge_pos_all;
            double y_pos_Dummy = h_shmsdp_pos_Dummy_all->GetBinContent(i+1)/charge_pos_Dummy_all;
            double y_pos_delta = h_shmsdp_pos_sim_delta->GetBinContent(i+1);
            double y_pos_exc = h_shmsdp_pos_sim_excrad->GetBinContent(i+1);
            double y_pos_rho = h_shmsdp_pos_sim_rho->GetBinContent(i+1);
            double y_pos = y_pos_D2-y_pos_bg_D2-0.245*y_pos_Dummy-y_pos_delta-y_pos_exc-a_rho*y_pos_rho;
            double error_pos_D2 = h_shmsdp_pos_all->GetBinError(i+1)/charge_pos_all;
            double error_pos_bg_D2 = h_shmsdp_pos_bg_all->GetBinError(i+1)/charge_pos_all;
            double error_pos_Dummy = h_shmsdp_pos_Dummy_all->GetBinError(i+1)/charge_pos_Dummy_all;
            double error_pos = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy);
            //double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i+1);
            //double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i+1);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
            //This is for yield
            //y_neg = y_neg*radia_corr_neg;
            //y_pos = y_pos*radia_corr_pos;
            //error_pos = error_pos*radia_corr_pos;
            //error_neg = error_neg*radia_corr_neg;

            //This convert yield to cross sectio,choose yield or xsn
            //double D2_xs_z_neg,D2_xs_z_pos;
            //D2_xs_z_neg = h_xs_z_neg_sim->GetYaxis()->GetBinCenter(i);
            //D2_xs_z_pos = h_xs_z_pos_sim->GetYaxis()->GetBinCenter(i);
            //double D2_neg_yield_incrad = h_shmsdp_neg_sim_incrad->GetBinContent(i);
            //double D2_pos_yield_incrad = h_shmsdp_pos_sim_incrad->GetBinContent(i);
            //y_pos = y_D2_pos*D2_xs_z_pos/D2_pos_yield_incrad;
            //y_neg = y_D2_neg*D2_xs_z_neg/D2_neg_yield_incrad;
            //error_pos = error_pos*D2_xs_z_pos/D2_pos_yield_incrad;
            //error_neg = error_neg*D2_xs_z_neg/D2_neg_yield_incrad;


            double y = (y_neg)/(y_pos);
            double error = (y_neg/y_pos)*std::sqrt((error_neg*error_neg)/(y_neg*y_neg)+(error_pos*error_pos)/(y_pos*y_pos));
            if(y>0){
              //double y_RD = (4*y-1)/(1-y);
              g_yield_ratio->SetPoint(ii,x,y);
              //g_yield_ratio->SetPoint(ii,x,y_RD);
              g_yield_ratio->SetPointError(ii,0,error);
              ii++;
              std::string z_str = std::to_string(x); 
            }
          //}

          //int nbins_sim = h_shmsdp_neg_sim_incnorad->GetXaxis()->GetNbins();
          //for(int i = 0;i<nbins_sim;i++){
            //std::cout<<i<<std::endl;
            double x_sim = h_shmsdp_neg_sim_incrad->GetBinCenter(i+1);//+0.001*i_color;
            double y_sim = h_shmsdp_neg_sim_incrad->GetBinContent(i+1);
            double error_sim = h_shmsdp_neg_sim_incrad->GetBinError(i+1);
            //double x_sim = h_shmsdp_neg_sim_incnorad->GetBinCenter(i+1);//+0.001*i_color;
            //double y_sim = h_shmsdp_neg_sim_incnorad->GetBinContent(i+1);
            //double error_sim = h_shmsdp_neg_sim_incnorad->GetBinError(i+1);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y>0 && y_sim>0){
              //      double y_RD = (4*y-1)/(1-y);
              //       g_yield_ratio_sim->SetPoint(ii_sim,x,y_RD);
              g_yield_ratio_sim->SetPoint(ii_sim,x_sim,y_sim);
              //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
              double ratio = y/y_sim;
              g_datasimratio_ratio->SetPoint(ii_sim,x_sim,ratio);
              g_datasimratio_ratio->SetPointError(ii_sim,0,error);
              ii_sim++;
            }
          }

          g_yield_ratio->SetName(q2xz_str_filename.c_str());
          // int nbins = h_shmsdp_pos_all->GetXaxis()->GetNbins();
          // TGraphErrors* g_yield_ratio = new TGraphErrors();
          // std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
          // g_yield_ratio->SetName(z_string.c_str());
          // int ii = 0;
          // for(int i = 0;i<nbins;i++){
          //   //std::cout<<i<<std::endl;
          //   double x = h_shmsdp_pos_all->GetBinCenter(i);
          //   double y = h_shmsdp_pos_all->GetBinContent(i);
          //   double error = h_shmsdp_pos_all->GetBinError(i);
          //   //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
          //   if(y!=0){
          //     g_yield_ratio->SetPoint(ii,x,y);
          //     g_yield_ratio->SetPointError(ii,0,error);
          //     ii++;
          //   }
          // }



          TMultiGraph* mg = new TMultiGraph();
          //c_Q2x_ratio->cd();
          //h_shmsdp_neg_all->SetLineColor(i_color);
          g_yield_ratio->SetMarkerStyle(4);
          g_yield_ratio->SetMarkerColor(i_color);
          g_yield_ratio->SetLineColor(i_color);
          g_yield_ratio_sim->SetMarkerStyle(5);
          g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
          g_yield_ratio_sim->SetLineColor(i_color);
          //hs->Add(h_shmsdp_neg_all);
          //h_shmsdp_neg_all->Draw("same");
          mg->Add(g_yield_ratio,"P");
          mg->Add(g_yield_ratio_sim,"L");


          TCanvas *c_Q2x_z_ratio = new TCanvas(q2x_name.c_str(),q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_z_ratio = new TCanvas();
          // //h_shmsdp_neg_all->Draw();
          //g_yield_ratio->GetXaxis()->SetRangeUser(0.1,1);
          //g_yield_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
          g_yield_ratio->GetYaxis()->SetRangeUser(0,2);
          g_yield_ratio->GetXaxis()->SetTitle("shmsdp");
          g_yield_ratio->GetYaxis()->SetTitle("yield_ratio");
          g_yield_ratio->GetXaxis()->SetTitleSize(0.053);
          g_yield_ratio->GetYaxis()->SetTitleSize(0.053);
          g_yield_ratio->Draw("AP"); 
          g_yield_ratio_sim->Draw("L");
          c_Q2x_z_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
          std::string zratiopdfname = "results/acceptance/"+q2xz_str_filename+"_ratio.pdf";

          c_Q2x_z_ratio->SaveAs(zratiopdfname.c_str());

          TGraphErrors* g_yield_ratio_all = new TGraphErrors();
          std::string z_string_all = "z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio_all->SetName(z_string_all.c_str());

          TCanvas* c_data_simc_ratio = new TCanvas();
          g_datasimratio_ratio->SetTitle((std::to_string(RunGroup)).c_str());
          g_datasimratio_ratio->GetYaxis()->SetRangeUser(0,2);
          g_datasimratio_ratio->Fit("pol0");
          gStyle->SetOptFit(1);
          g_datasimratio_ratio->GetXaxis()->SetTitle("shmsdp");
          g_datasimratio_ratio->GetYaxis()->SetTitle("data/simc");
          g_datasimratio_ratio->SetMarkerStyle(4);
          g_datasimratio_ratio->Draw("AP");

          std::string c_data_simc_ratio_name = "results/acceptance/"+q2xz_str_filename+"_datasimcratio.pdf";
          c_data_simc_ratio->SaveAs(c_data_simc_ratio_name.c_str());
          TF1 *Fit = g_datasimratio_ratio->GetFunction("pol0");
          double p0 = Fit->GetParameter(0);
          double p0_err = Fit->GetParError(0);
          file<<RunGroup<<", x"<<xbj<<",Q2"<<Q2<<", z"<<z<<","<<p0<<std::endl;
          g_p0->SetPoint(i_p0,RunGroup,p0);
          g_p0->SetPointError(i_p0,0,p0_err);
          g_p0_z->SetPoint(i_p0,z,p0);
          g_p0_z->SetPointError(i_p0,0,p0_err);
          i_p0++;
          // int ii_all = 0;
          //for(int i = 0;i<nbins;i++){
          //  //std::cout<<i<<std::endl;
          //  double x = h_shmsdp_neg_all->GetBinCenter(i)+0.001*i_color+0.6*i_which_x[i_whichx];
          //  double y = h_shmsdp_neg_all->GetBinContent(i)+i_which_Q2[i_whichx];
          //  double error = h_shmsdp_neg_all->GetBinError(i);
          //  //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
          //  if(y!=0 && y!= 1 && y!=2 && error< 0.2){
          //    g_yield_ratio_all->SetPoint(ii_all,x,y);
          //    g_yield_ratio_all->SetPointError(ii_all,0,error);
          //    ii_all++;
          //  }
          //}
          //  g_yield_ratio_all->SetMarkerStyle(4);
          //  g_yield_ratio_all->SetMarkerColor(i_color);
          //  g_yield_ratio_all->SetLineColor(i_color);
          //  mg_all->Add(g_yield_ratio_all,"P");
          i_color++;
        }//loop over z
      
      }//if x,Q2 not 0

    }//loop over Q2
  }//loop over x
TCanvas *c_p0 = new TCanvas();
//g_p0->Fit("pol0");
g_p0->GetYaxis()->SetTitle("data/simc");
g_p0->GetXaxis()->SetTitle("RunGroup");
g_p0->GetYaxis()->SetRangeUser(0.9,1.1);
g_p0->SetMarkerStyle(8);
g_p0->Draw("AP");
c_p0->SaveAs("results/acceptance/SHMS_p0_incrad.pdf");
TCanvas *c_p0_z = new TCanvas();
//g_p0_z->Fit("pol0");
g_p0_z->GetYaxis()->SetTitle("data/simc");
g_p0_z->GetXaxis()->SetTitle("z");
g_p0_z->GetYaxis()->SetRangeUser(0.9,1.1);
g_p0_z->SetMarkerStyle(8);
g_p0_z->Draw("AP");
c_p0_z->SaveAs("results/acceptance/SHMS_p0_z_incrad.pdf");

}
