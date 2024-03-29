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

int plot_pt_Q2x(){
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
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_all = new TMultiGraph(); 
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    auto mg_x_all = new TMultiGraph();
    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::string q2x_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_pt";
      std::string q2x_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_pt";
      TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
      TH1D* h_neg_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
      TH1D* h_pos_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
      //TGraphPolar* pt_phi_polar_all = new TGraphPolar();
      int i_color = 1;
      auto mg = new TMultiGraph();
      auto mg_polar_pos = new TMultiGraph();
      auto mg_polar_neg = new TMultiGraph();
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_pt_neg_all = new TH1D("","neg;pt;",100,0,0.5);
          h_pt_neg_all->GetXaxis()->SetTitle("pt");
          //h_pt_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_pt_pos_all = new TH1D("","pos;pt;",100,0,0.5);
          h_pt_pos_all->GetXaxis()->SetTitle("pt");
          h_pt_neg_all->Sumw2();
          h_pt_pos_all->Sumw2();
          TH1D* h_pt_neg_bg_all = new TH1D("","neg bg;pt;",100,0,0.5);
          h_pt_neg_bg_all->GetXaxis()->SetTitle("pt");
          TH1D* h_pt_pos_bg_all = new TH1D("","pos bg;pt;",100,0,0.5);
          h_pt_pos_bg_all->GetXaxis()->SetTitle("pt");
          h_pt_neg_bg_all->Sumw2();
          h_pt_pos_bg_all->Sumw2();
          auto pt_phi_polar_pos = new TMultiGraph();
          auto pt_phi_polar_neg = new TMultiGraph();
          //          TH1D* h_pt_neg_Dummy_all = new TH1D("","neg;z;",100,0,1);
          //          h_pt_neg_Dummy_all->GetXaxis()->SetTitle("z");
          //          h_pt_neg_Dummy_all->GetYaxis()->SetRangeUser(0,2);
          //          TH1D* h_pt_pos_Dummy_all = new TH1D("","pos;z;",100,0,1);
          //          h_pt_pos_Dummy_all->GetXaxis()->SetTitle("z");
          //          h_pt_neg_Dummy_all->Sumw2();
          //          h_pt_pos_Dummy_all->Sumw2();
          //          TH1D* h_pt_neg_bg_Dummy_all = new TH1D("","neg bg;z;",100,0,1);
          //          h_pt_neg_bg_Dummy_all->GetXaxis()->SetTitle("z");
          //          TH1D* h_pt_pos_bg_Dummy_all = new TH1D("","pos bg;z;",100,0,1);
          //          h_pt_pos_bg_Dummy_all->GetXaxis()->SetTitle("z");
          //          h_pt_neg_bg_Dummy_all->Sumw2();
          //          h_pt_pos_bg_Dummy_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          //          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          //          TH1D *h_pt_neg_sim = new TH1D("","pi- sim sum",100,0,1);
          //          TH1D *h_pt_neg_sim_incrad = new TH1D("","pi- sim SIDIS",100,0,1);
          //          h_pt_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          //          TH1D *h_pt_neg_sim_incnorad = new TH1D("","pi- sim SIDIS",100,0,1);
          //          h_pt_neg_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_norad");
          //          TH1D *h_pt_neg_sim_excrad = new TH1D("","pi- sim exc",100,0,1);
          //          h_pt_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          //          TH1D *h_pt_neg_sim_rho = new TH1D("","pi- sim rho",100,0,1);
          //          h_pt_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("z_neg_rho");
          //          TH1D *h_pt_neg_sim_delta = new TH1D("","pi- sim delta",100,0,1);
          //          h_pt_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          //          h_pt_neg_sim->Add(h_pt_neg_sim_incrad,1);
          //          h_pt_neg_sim->Add(h_pt_neg_sim_excrad,1);
          //          //h_pt_neg_sim->Add(h_pt_neg_sim_rho,1);
          //          h_pt_neg_sim->Add(h_pt_neg_sim_delta,1);
          //          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          //          TH1D *h_pt_pos_sim = new TH1D("","pi+ sim sum",100,0,1);
          //          TH1D *h_pt_pos_sim_incrad = new TH1D("","pi+ sim SIDIS",100,0,1);
          //          h_pt_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          //          TH1D *h_pt_pos_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,0,1);
          //          h_pt_pos_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_norad");
          //          TH1D *h_pt_pos_sim_excrad = new TH1D("","pi+ sim exc",100,0,1);
          //          h_pt_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          //          TH1D *h_pt_pos_sim_rho = new TH1D("","pi+ sim rho",100,0,1);
          //          h_pt_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("z_pos_rho");
          //          TH1D *h_pt_pos_sim_delta = new TH1D("","pi+ sim delta",100,0,1);
          //          h_pt_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          //          h_pt_pos_sim->Add(h_pt_pos_sim_incrad,1);
          //          h_pt_pos_sim->Add(h_pt_pos_sim_excrad,1);
          //          //h_pt_pos_sim->Add(h_pt_pos_sim_rho,1);
          //          h_pt_pos_sim->Add(h_pt_pos_sim_delta,1);

          double charge_neg_all = 0,charge_pos_all=0;
          double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            // neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            // pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
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
              double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"neg DE check "<<std::endl;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+"_pt.root").c_str());
              TH1D *h_pt_neg = new TH1D("","",100,0,0.5);
              h_pt_neg = (TH1D*)root_file_neg->Get("pt");
              TH1D *h_pt_neg_bg = new TH1D("","",100,0,0.5);
              h_pt_neg_bg = (TH1D*)root_file_neg->Get("pt_bg");
              TGraphPolar* pt_phi_polar = new TGraphPolar();
              pt_phi_polar = (TGraphPolar*)root_file_neg->Get("pt_phi_polar");
              pt_phi_polar->SetTitle(q2xz_str.c_str());
              pt_phi_polar->SetMarkerStyle(7);
              pt_phi_polar->SetMarkerColorAlpha(coolcolor[i_color],0.35);
              pt_phi_polar_neg->Add(pt_phi_polar,"P");
              //h_pt_neg_all->Add(h_pt_neg_bg,-1/(charge*TE));
              //h_pt_neg_all->Add(h_pt_neg,1/(charge*TE));
              h_pt_neg_bg_all->Add(h_pt_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_pt_neg_all->Add(h_pt_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
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
              double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
              double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
              double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+"_pt.root").c_str());
              TH1D *h_pt_pos = new TH1D("","",100,0,0.5);
              h_pt_pos = (TH1D*)root_file_pos->Get("pt");
              TH1D *h_pt_pos_bg = new TH1D("","",100,0,0.5);
              h_pt_pos_bg = (TH1D*)root_file_pos->Get("pt_bg");
              TGraphPolar* pt_phi_polar = new TGraphPolar();
              pt_phi_polar = (TGraphPolar*)root_file_pos->Get("pt_phi_polar");
              pt_phi_polar->SetMarkerStyle(7);
              pt_phi_polar->SetTitle(q2xz_str.c_str());
              pt_phi_polar->SetMarkerColorAlpha(warmcolor[i_color],0.35);
              pt_phi_polar_pos->Add(pt_phi_polar,"P");
              //h_pt_pos_all->Add(h_pt_pos_bg,-1/(charge*TE));
              //h_pt_pos_all->Add(h_pt_pos,1/(charge*TE));
              h_pt_pos_bg_all->Add(h_pt_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              h_pt_pos_all->Add(h_pt_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            }//loop over pos runs
            //for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
            //  int RunNumber = *it;
            //  //std::cout<<"Dummy"<<RunNumber<<std::endl;
            //  double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
            //  charge_neg_Dummy_all += charge;
            //  double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
            //  double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
            //  double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
            //  //double TE = 1;
            //  //std::cout<<"neg Dummy TE check "<<std::endl;
            //  double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
            //  double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
            //  double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
            //  double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
            //  //std::cout<<"neg Dummy DE check "<<std::endl;
            //  TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            //  TH1D *h_pt_neg = new TH1D("","",100,0,1);
            //  h_pt_neg = (TH1D*)root_file_neg->Get("z");
            //  TH1D *h_pt_neg_bg = new TH1D("","",100,0,1);
            //  h_pt_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
            //  h_pt_neg_bg_Dummy_all->Add(h_pt_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            //  h_pt_neg_Dummy_all->Add(h_pt_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            //}//loop over neg runs
            //for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
            //  int RunNumber = *it;
            //  //std::cout<<"Dummy "<<RunNumber<<std::endl;
            //  double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
            //  charge_pos_Dummy_all+=charge;
            //  double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
            //  double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
            //  double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
            //  //std::cout<<"pos TE check"<<std::endl;
            //  double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
            //  double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
            //  double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
            //  double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
            //  //std::cout<<"pos DE check"<<std::endl;
            //  TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            //  TH1D *h_pt_pos = new TH1D("","",100,0,1);
            //  h_pt_pos = (TH1D*)root_file_pos->Get("z");
            //  TH1D *h_pt_pos_bg = new TH1D("","",100,0,1);
            //  h_pt_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
            //  h_pt_pos_bg_Dummy_all->Add(h_pt_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            //  h_pt_pos_Dummy_all->Add(h_pt_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
            //}//loop over pos runs
          }//if z not 0

          TCanvas *c_counts_pos = new TCanvas();
          h_pt_pos_all->GetYaxis()->SetTitle("counts");
          //h_pt_pos_all->GetXaxis()->SetRangeUser(0.3,1);
          h_pt_pos_all->DrawCopy("hist");
          h_pt_pos_bg_all->SetLineColor(kOrange);
          //h_pt_pos_bg_all->GetXaxis()->SetRangeUser(0.3,1);
          h_pt_pos_bg_all->DrawCopy("hist same");
          std::string c_counts_pos_name = "results/pt/pt_counts_"+q2xz_str_filename+"_pos.pdf";
          c_counts_pos->SaveAs(c_counts_pos_name.c_str());

          TCanvas *c_counts_neg = new TCanvas();
          h_pt_neg_all->GetYaxis()->SetTitle("counts");
          //h_pt_neg_all->GetXaxis()->SetRangeUser(0.3,1);
          h_pt_neg_all->DrawCopy("hist");
          h_pt_neg_bg_all->SetLineColor(kOrange);
          h_pt_neg_bg_all->DrawCopy("hist same");
          std::string c_counts_neg_name = "results/pt/pt_counts_"+q2xz_str_filename+"_neg.pdf";
          c_counts_neg->SaveAs(c_counts_neg_name.c_str());


          h_pt_neg_all->Scale(1/charge_neg_all);
          h_pt_pos_all->Scale(1/charge_pos_all);

          h_pt_neg_bg_all->Scale(1/charge_neg_all);
          h_pt_pos_bg_all->Scale(1/charge_pos_all);

          //  h_pt_neg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          //  h_pt_pos_Dummy_all->Scale(1/charge_pos_Dummy_all);

          //  h_pt_neg_bg_Dummy_all->Scale(1/charge_neg_Dummy_all);
          //  h_pt_pos_bg_Dummy_all->Scale(1/charge_pos_Dummy_all);

          //bg subtracted
          h_pt_neg_all->Add(h_pt_neg_bg_all,-1.0);
          h_pt_pos_all->Add(h_pt_pos_bg_all,-1.0);

          //  //Dummy subtract
          //  h_pt_neg_Dummy_all->Add(h_pt_neg_bg_Dummy_all,1.0);
          //  h_pt_pos_Dummy_all->Add(h_pt_pos_bg_Dummy_all,1.0);
          //  
          //  h_pt_neg_all->Add(h_pt_neg_Dummy_all,-0.245);
          //  h_pt_pos_all->Add(h_pt_pos_Dummy_all,-0.245);

          //  //bgs from SIMC
          //  h_pt_neg_all->Add(h_pt_neg_sim_excrad,-1);
          //  h_pt_pos_all->Add(h_pt_pos_sim_excrad,-1);
          //  h_pt_neg_all->Add(h_pt_neg_sim_delta,-1);
          //  h_pt_pos_all->Add(h_pt_pos_sim_delta,-1);

          auto h_pt_neg_yield = h_pt_neg_all;
          auto h_pt_neg_bg_yield = h_pt_neg_bg_all;
          auto h_pt_pos_yield = h_pt_pos_all;
          auto h_pt_pos_bg_yield = h_pt_pos_bg_all;

          //  auto h_pt_neg_incnorad = h_pt_neg_sim_incnorad;
          //  auto h_pt_neg_incrad = h_pt_neg_sim_incrad;
          //  auto h_pt_pos_incnorad = h_pt_pos_sim_incnorad;
          //  auto h_pt_pos_incrad = h_pt_pos_sim_incrad;

          h_pt_neg_all->Rebin(5);
          h_pt_pos_all->Rebin(5);
          //  h_pt_neg_sim->Rebin(2);
          //  h_pt_pos_sim->Rebin(2);
          //  h_pt_neg_incnorad->Rebin(2);
          //  h_pt_neg_incrad->Rebin(2);
          //  h_pt_neg_sim_excrad->Rebin(2);
          //  h_pt_neg_sim_delta->Rebin(2);
          //  h_pt_neg_sim_rho->Rebin(2);
          //  h_pt_pos_incnorad->Rebin(2);
          //  h_pt_pos_incrad->Rebin(2);
          //  h_pt_pos_sim_excrad->Rebin(2);
          //  h_pt_pos_sim_delta->Rebin(2);
          //  h_pt_pos_sim_rho->Rebin(2);
          ///auto rp_radia_corr_neg = new TRatioPlot(h_pt_neg_sim_incnorad,h_pt_neg_sim_incrad);
          ///auto rp_radia_corr_pos = new TRatioPlot(h_pt_pos_sim_incnorad,h_pt_pos_sim_incrad);
          //  TH1* rp_radia_corr_neg = (TH1*)h_pt_neg_incnorad->Clone("rp_radia_corr");
          //  TH1* rp_radia_corr_pos = (TH1*)h_pt_pos_incnorad->Clone("rp_radia_corr");
          //  rp_radia_corr_neg ->Divide(h_pt_neg_incnorad,h_pt_neg_incrad);          
          //  rp_radia_corr_pos ->Divide(h_pt_pos_incnorad,h_pt_pos_incrad);          
          //  TCanvas* c_radia_corr_sim = new TCanvas();
          //  c_radia_corr_sim->Divide(2,1);
          //  c_radia_corr_sim->cd(1);
          //  rp_radia_corr_neg->Draw();
          //  c_radia_corr_sim->cd(2);
          //  rp_radia_corr_pos->Draw();
          //  std::string c_radia_corr_sim_name = "results/yield/statistics_corr/radia_corr_"+q2xz_str_filename+".pdf";
          //  c_radia_corr_sim->SaveAs(c_radia_corr_sim_name.c_str());

          //  h_pt_neg_yield->Multiply(h_pt_neg_yield,rp_radia_corr_neg);
          //  h_pt_pos_yield->Multiply(h_pt_pos_yield,rp_radia_corr_pos);

          TGraphErrors* g_pt_pos_yield = new TGraphErrors();
          g_pt_pos_yield->SetTitle("pi+ data yield");
          int nbins_yield_pos = h_pt_pos_yield->GetXaxis()->GetNbins();
          int ii_yield_pos = 0;
          for(int i = 0;i<nbins_yield_pos;i++){
            //std::cout<<i<<std::endl;
            double x = h_pt_pos_yield->GetBinCenter(i);
            double y = h_pt_pos_yield->GetBinContent(i);
            double error = h_pt_pos_yield->GetBinError(i);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_pt_pos_yield->SetPoint(ii_yield_pos,x,y);
              g_pt_pos_yield->SetPointError(ii_yield_pos,0,error);
              ii_yield_pos++;
            }
          }
          TCanvas *c_yield_pos = new TCanvas();
          gStyle->SetOptTitle(0);
          h_pt_pos_yield->GetYaxis()->SetTitle("yield");
          h_pt_pos_yield->GetYaxis()->SetTitleSize(0.53);
          //h_pt_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          h_pt_pos_yield->SetMarkerStyle(4);
          h_pt_pos_yield->DrawCopy();
          ////h_pt_pos_bg_yield->SetLineColor(kOrange);
          ////h_pt_pos_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          ////h_pt_pos_bg_yield->DrawCopy("hist same");
          //h_pt_pos_sim->DrawCopy("hist same");
          //h_pt_pos_sim_incrad->SetLineColor(kOrange);
          //h_pt_pos_sim_incrad->SetTitle("pi+ sim SIDIS");
          //h_pt_pos_sim_incrad->DrawCopy("hist same");
          //h_pt_pos_sim_excrad->SetLineColor(kBlue);
          //h_pt_pos_sim_excrad->SetTitle("pi+ sim exc");
          //h_pt_pos_sim_excrad->DrawCopy("hist same");
          //h_pt_pos_sim_rho->SetLineColor(kRed);
          //h_pt_pos_sim_rho->SetTitle("pi+ sim rho");
          //h_pt_pos_sim_rho->DrawCopy("hist same");
          //h_pt_pos_sim_delta->SetLineColor(6);
          //h_pt_pos_sim_delta->SetTitle("pi+ sim delta");
          //h_pt_pos_sim_delta->DrawCopy("hist same");
          TMultiGraph* mg_pt_yield_pos = new TMultiGraph(); 
          //TGraphErrors* g_pt_pos_yield = new TGraphErrors(h_pt_pos_yield);
          g_pt_pos_yield->GetYaxis()->SetTitle("yield");
          g_pt_pos_yield->GetYaxis()->SetTitleSize(0.53);
          //g_pt_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_pt_pos_yield->SetMarkerStyle(4);
          mg_pt_yield_pos->Add(g_pt_pos_yield,"P");
          //TGraph* g_pt_pos_sim = new TGraph(h_pt_pos_sim);
          //TGraph* g_pt_pos_sim_incrad = new TGraph(h_pt_pos_sim_incrad);
          //TGraph* g_pt_pos_sim_incnorad = new TGraph(h_pt_pos_sim_incnorad);
          //TGraph* g_pt_pos_sim_excrad = new TGraph(h_pt_pos_sim_excrad);
          //TGraph* g_pt_pos_sim_rho = new TGraph(h_pt_pos_sim_rho);
          //TGraph* g_pt_pos_sim_delta = new TGraph(h_pt_pos_sim_delta);
          //mg_pt_yield_pos->Add(g_pt_pos_sim); 
          //g_pt_pos_sim_incrad->SetLineColor(kOrange);
          //g_pt_pos_sim_incrad->SetTitle("pi+ sim SIDIS");
          //mg_pt_yield_pos->Add(g_pt_pos_sim_incrad); 
          //g_pt_pos_sim_excrad->SetLineColor(kBlue);
          //g_pt_pos_sim_excrad->SetTitle("pi+ sim exc");
          //mg_pt_yield_pos->Add(g_pt_pos_sim_excrad); 
          //g_pt_pos_sim_rho->SetLineColor(kRed);
          //g_pt_pos_sim_rho->SetTitle("pi+ sim rho");
          //mg_pt_yield_pos->Add(g_pt_pos_sim_rho); 
          //g_pt_pos_sim_delta->SetLineColor(46);
          //g_pt_pos_sim_delta->SetTitle("pi+ sim delta");
          //mg_pt_yield_pos->Add(g_pt_pos_sim_delta); 
          //g_pt_pos_sim_incnorad->SetLineColor(40);
          //g_pt_pos_sim_incnorad->SetTitle("pi+ sim inc norad");
          //mg_pt_yield_pos->Add(g_pt_pos_sim_incnorad,"L");
          //mg_pt_yield_pos->SetTitle(q2xz_str.c_str());
          //mg_pt_yield_pos->GetYaxis()->SetTitle("yield");
          //mg_pt_yield_pos->GetXaxis()->SetTitle("z");
          mg_pt_yield_pos->Draw("AP");
          c_yield_pos->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_pos_name = "results/pt/pt_yield_"+q2xz_str_filename+"_pos.pdf";
          c_yield_pos->SaveAs(c_yield_pos_name.c_str());

          //h_pt_neg_all->Rebin(2);
          //h_pt_pos_all->Rebin(2);
          TGraphErrors* g_pt_neg_yield = new TGraphErrors();
          g_pt_neg_yield->SetTitle("pi- data yield");
          int nbins_yield_neg = h_pt_neg_yield->GetXaxis()->GetNbins();
          int ii_yield_neg = 0;
          for(int i = 0;i<nbins_yield_neg;i++){
            //std::cout<<i<<std::endl;
            double x = h_pt_neg_yield->GetBinCenter(i);
            double y = h_pt_neg_yield->GetBinContent(i);
            double error = h_pt_neg_yield->GetBinError(i);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_pt_neg_yield->SetPoint(ii_yield_neg,x,y);
              g_pt_neg_yield->SetPointError(ii_yield_neg,0,error);
              ii_yield_neg++;
            }
          }
          TCanvas *c_yield_neg = new TCanvas();
          gStyle->SetOptTitle(0);
          //g_pt_neg_yield->DrawCopy();
          //h_pt_neg_bg_yield->SetLineColor(kOrange);
          //h_pt_neg_bg_yield->GetXaxis()->SetRangeUser(0.3,1);
          //h_pt_neg_bg_yield->DrawCopy("hist same");
          TMultiGraph* mg_pt_yield_neg = new TMultiGraph(); 
          //TGraphErrors* g_pt_neg_yield = new TGraphErrors(h_pt_neg_yield);
          g_pt_neg_yield->GetYaxis()->SetTitle("yield");
          g_pt_neg_yield->GetYaxis()->SetTitleSize(0.53);
          //g_pt_neg_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_pt_neg_yield->SetMarkerStyle(4);
          mg_pt_yield_neg->Add(g_pt_neg_yield,"P");
          //TGraph* g_pt_neg_sim = new TGraph(h_pt_neg_sim);
          //TGraph* g_pt_neg_sim_incrad = new TGraph(h_pt_neg_sim_incrad);
          //TGraph* g_pt_neg_sim_incnorad = new TGraph(h_pt_neg_sim_incnorad);
          //TGraph* g_pt_neg_sim_excrad = new TGraph(h_pt_neg_sim_excrad);
          //TGraph* g_pt_neg_sim_rho = new TGraph(h_pt_neg_sim_rho);
          //TGraph* g_pt_neg_sim_delta = new TGraph(h_pt_neg_sim_delta);
          //mg_pt_yield_neg->Add(g_pt_neg_sim,"L"); 
          //g_pt_neg_sim_incrad->SetLineColor(kOrange);
          //g_pt_neg_sim_incrad->SetTitle("pi- sim SIDIS");
          //mg_pt_yield_neg->Add(g_pt_neg_sim_incrad,"L"); 
          //g_pt_neg_sim_excrad->SetLineColor(kBlue);
          //g_pt_neg_sim_excrad->SetTitle("pi- sim exc");
          //mg_pt_yield_neg->Add(g_pt_neg_sim_excrad,"L"); 
          //g_pt_neg_sim_rho->SetLineColor(kRed);
          //g_pt_neg_sim_rho->SetTitle("pi- sim rho");
          //mg_pt_yield_neg->Add(g_pt_neg_sim_rho,"L"); 
          //g_pt_neg_sim_delta->SetLineColor(46);
          //g_pt_neg_sim_delta->SetTitle("pi- sim delta");
          //mg_pt_yield_neg->Add(g_pt_neg_sim_delta,"L"); 
          //g_pt_neg_sim_incnorad->SetLineColor(40);
          //g_pt_neg_sim_incnorad->SetTitle("pi+ sim inc norad");
          //mg_pt_yield_neg->Add(g_pt_neg_sim_incnorad,"L");
          //mg_pt_yield_neg->SetTitle(q2xz_str.c_str());
          //mg_pt_yield_neg->GetYaxis()->SetTitle("yield");
          //mg_pt_yield_neg->GetXaxis()->SetTitle("z");
          mg_pt_yield_neg->Draw("AP");
          //mg_pt_yield_neg->GetXaxis()->SetLimits(0.3,0.7);
          //h_pt_neg_sim->DrawCopy("hist same");
          //h_pt_neg_sim_incrad->SetLineColor(kOrange);
          //h_pt_neg_sim_incrad->SetTitle("pi- sim SIDIS");
          //h_pt_neg_sim_incrad->DrawCopy("hist same");
          //h_pt_neg_sim_excrad->SetLineColor(kBlue);
          //h_pt_neg_sim_excrad->SetTitle("pi- sim exc");
          //h_pt_neg_sim_excrad->DrawCopy("hist same");
          //h_pt_neg_sim_rho->SetLineColor(kRed);
          //h_pt_neg_sim_rho->SetTitle("pi- sim rho");
          //h_pt_neg_sim_rho->DrawCopy("hist same");
          //h_pt_neg_sim_delta->SetLineColor(6);
          //h_pt_neg_sim_delta->SetTitle("pi- sim delta");
          //h_pt_neg_sim_delta->DrawCopy("hist same");
          c_yield_neg->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_neg_name = "results/pt/pt_neg_"+q2xz_str_filename+"_neg.pdf";
          c_yield_neg->SaveAs(c_yield_neg_name.c_str());

          /*
             auto h_pos_yield = (TH1D*)h_pt_pos_all->Clone("pos yield");
             auto h_neg_yield = (TH1D*)h_pt_neg_all->Clone("neg yield");
             h_pos_yield->Add(h_neg_yield,-1);
             TGraphErrors *g_fragmentation = new TGraphErrors();
             int n_frag = h_pos_yield->GetXaxis()->GetNbins();
             int i_frag = 0;
             for(int i = 0;i<n_frag;i++){
             double x = h_pt_neg_all->GetBinCenter(i);
             double y = h_pt_neg_all->GetBinContent(i);
             double error = h_pt_neg_all->GetBinError(i);
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
          //h_pt_neg_all->Rebin(2);
          //h_pt_pos_all->Rebin(2);
          //h_pt_neg_sim->Rebin(2);
          //h_pt_pos_sim->Rebin(2);

          //// h_neg_q2x->Sumw2();
          //// auto rp = new TRatioPlot(h_pt_neg_all,h_pt_pos_all);
          //// c_Q2x_ratio->SetTicks(0,1);
          //// rp->Draw();
          //// rp->GetLowerRefGraph()->SetMinimum(0);
          //// rp->GetLowerRefGraph()->SetMaximum(1);

          h_pt_neg_all->Sumw2();
          h_pt_pos_all->Sumw2();

          //h_pt_neg_all->Divide(h_pt_pos_all);
          //h_pt_neg_sim_incnorad->Divide(h_pt_pos_sim_incnorad);

          h_pt_neg_all->SetTitle(q2xz_str_filename.c_str());
          h_pt_neg_all->GetYaxis()->SetTitle("pi-/pi+ ratio");
          //TFile* yield_ratio_file = new TFile("results/yield_ratio_all.root","UPDATE");
          //h_pt_neg_all->Write();
          //yield_ratio_file->Close();  

          int nbins = h_pt_neg_all->GetXaxis()->GetNbins();

          // TGraphErrors* g_yield_ratio = new TGraphErrors(h_pt_neg_all);
          TGraphErrors* g_yield_ratio = new TGraphErrors();
          std::string z_string = "R_Y z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio->SetName(z_string.c_str());

          int ii = 0;
          for(int i = 0;i<nbins;i++){
            //std::cout<<i<<std::endl;
            double x = h_pt_neg_all->GetBinCenter(i)+0.001*i_color;
            double y_neg = h_pt_neg_all->GetBinContent(i);
            double error_neg = h_pt_neg_all->GetBinError(i);
            double y_pos = h_pt_pos_all->GetBinContent(i);
            double error_pos = h_pt_pos_all->GetBinError(i);
            //double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i);
            //double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
            y_neg = y_neg;
            //*radia_corr_neg;
            y_pos = y_pos;
            //*radia_corr_pos;
            double y = (y_neg)/(y_pos);
            double error = (y_neg/y_pos)*std::sqrt((error_neg*error_neg)/(y_neg*y_neg)+(error_pos*error_pos)/(y_pos*y_pos));
            if(y!=0 && error< 0.2){
              //double y_RD = (4*y-1)/(1-y);
              g_yield_ratio->SetPoint(ii,x,y);
              //g_yield_ratio->SetPoint(ii,x,y_RD);
              g_yield_ratio->SetPointError(ii,0,error);
              ii++;
            }
          }

          // int nbins_sim = h_pt_neg_sim_incnorad->GetXaxis()->GetNbins();
          // TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
          // std::string z_string_sim = "z simc "+(std::to_string(z)).substr(0,4);
          // g_yield_ratio_sim->SetName(z_string_sim.c_str());
          // //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_pt_neg_sim_incnorad);
          //int ii_sim = 0;
          //for(int i = 0;i<nbins_sim;i++){
          //  //std::cout<<i<<std::endl;
          //  double x = h_pt_neg_sim_incnorad->GetBinCenter(i)+0.001*i_color;
          //  double y = h_pt_neg_sim_incnorad->GetBinContent(i);
          //  double error = h_pt_neg_sim_incnorad->GetBinError(i);
          //  //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
          //  if(y!=0){
          // //     double y_RD = (4*y-1)/(1-y);
          // //      g_yield_ratio_sim->SetPoint(ii_sim,x,y_RD);
          //    g_yield_ratio_sim->SetPoint(ii_sim,x,y);
          //    //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
          //    ii_sim++;
          //  }
          //}

          // int nbins = h_pt_pos_all->GetXaxis()->GetNbins();
          // TGraphErrors* g_yield_ratio = new TGraphErrors();
          // std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
          // g_yield_ratio->SetName(z_string.c_str());
          // int ii = 0;
          // for(int i = 0;i<nbins;i++){
          //   //std::cout<<i<<std::endl;
          //   double x = h_pt_pos_all->GetBinCenter(i);
          //   double y = h_pt_pos_all->GetBinContent(i);
          //   double error = h_pt_pos_all->GetBinError(i);
          //   //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
          //   if(y!=0){
          //     g_yield_ratio->SetPoint(ii,x,y);
          //     g_yield_ratio->SetPointError(ii,0,error);
          //     ii++;
          //   }
          // }



          //c_Q2x_ratio->cd();
          //h_pt_neg_all->SetLineColor(i_color);
          g_yield_ratio->SetMarkerStyle(4);
          g_yield_ratio->SetMarkerColor(i_color);
          g_yield_ratio->SetLineColor(i_color);
          //g_yield_ratio_sim->SetMarkerStyle(5);
          //g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
          //g_yield_ratio_sim->SetLineColor(i_color);
          //hs->Add(h_pt_neg_all);
          //h_pt_neg_all->Draw("same");
          mg->Add(g_yield_ratio,"P");
          //mg->Add(g_yield_ratio_sim,"L");
          //c_Q2x_ratio->Update();
          


          TCanvas *c_Q2x_z_ratio = new TCanvas(q2x_name.c_str(),q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_z_ratio = new TCanvas();
          // //h_pt_neg_all->Draw();
          //g_yield_ratio->GetXaxis()->SetRangeUser(0,1);
          //g_yield_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
          g_yield_ratio->GetXaxis()->SetTitle("pt");
          g_yield_ratio->GetYaxis()->SetTitle("yield_ratio");
          g_yield_ratio->GetXaxis()->SetTitleSize(0.053);
          g_yield_ratio->GetYaxis()->SetTitleSize(0.053);
          g_yield_ratio->Draw("AP"); 
          //g_yield_ratio_sim->Draw("L");
          c_Q2x_z_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
          std::string zratiopdfname = "results/pt/pt_ratio_"+q2xz_str_filename+"_ratio.pdf";

          c_Q2x_z_ratio->SaveAs(zratiopdfname.c_str());

          TGraphErrors* g_yield_ratio_all = new TGraphErrors();
          std::string z_string_all = "z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio_all->SetName(z_string_all.c_str());

          // int ii_all = 0;
          //for(int i = 0;i<nbins;i++){
          //  //std::cout<<i<<std::endl;
          //  double x = h_pt_neg_all->GetBinCenter(i)+0.001*i_color+0.6*i_which_x[i_whichx];
          //  double y = h_pt_neg_all->GetBinContent(i)+i_which_Q2[i_whichx];
          //  double error = h_pt_neg_all->GetBinError(i);
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
      TCanvas* c_polar_pos = new TCanvas();
      pt_phi_polar_pos->Draw("P");
      c_polar_pos->BuildLegend(0.8,0.7,0.95,0.95);
      std::string c_polar_pos_name = "results/pt/polar_pos_"+q2xz_str_filename+".pdf";
      c_polar_pos->SaveAs(c_polar_pos_name.c_str());
      TCanvas* c_polar_neg = new TCanvas();
      pt_phi_polar_neg->Draw("P");
      c_polar_neg->BuildLegend(0.8,0.7,0.95,0.95);
      std::string c_polar_neg_name = "results/pt/polar_neg_"+q2xz_str_filename+".pdf";
      c_polar_neg->SaveAs(c_polar_neg_name.c_str());
        }//loop over z
        //i_whichx++;
      }//if x,Q2 not 0
      c_Q2x_ratio->cd();
      c_Q2x_ratio->SetTitle(canvas_name.c_str());
      //hs->Draw();
      //TF1 *f_RD = new TF1("f1","2.5/x-2.5",0,1);
      mg->GetXaxis()->SetTitleSize(0.053);
      mg->GetYaxis()->SetTitleSize(0.053);
      mg->GetXaxis()->SetLabelSize(0.05);
      mg->GetYaxis()->SetLabelSize(0.05);
      mg->SetMinimum(0.1);
      mg->SetMaximum(1.2);
      std::string mg_title = canvas_name+",pt";
      mg->SetTitle(mg_title.c_str());
      TPaveText *pt_mg = new TPaveText(0.5,0.8,0.8,1);
      pt_mg->AddText(canvas_name.c_str());
      pt_mg->Draw("same");
      mg->Draw("APL");
      //f_RD->Draw("same");
      mg->GetHistogram()->SetTitle(canvas_name.c_str());
      mg->GetXaxis()->SetTitle(mg_title.c_str());
//      mg->GetXaxis()->SetLimits(0.3,0.8);
      //auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
      //TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
      //fit->Draw("same");
      //      std::string ratiopdfname = "results/yield/statistics_corr/"+canvas_filename+"_RDratio.pdf";
      std::string ratiopdfname = "results/pt/pt_ratio_"+canvas_filename+"_ratio.pdf";
      c_Q2x_ratio->BuildLegend(0.7,0.7,0.9,0.9);
      c_Q2x_ratio->SaveAs(ratiopdfname.c_str());

      //i_whichq2++;
      //std::cout<<i_whichx<<std::endl;
    }//loop over Q2
  }//loop over x


  //TCanvas* c_yield_ratio_all = new TCanvas();
  //mg_all->GetXaxis()->SetTitle("z");
  //mg_all->GetYaxis()->SetTitle("R^D_meas");
  //mg_all->GetXaxis()->SetTitleSize(0.053);
  //mg_all->GetYaxis()->SetTitleSize(0.053);
  //mg_all->GetXaxis()->SetLabelSize(0.05);
  //mg_all->GetYaxis()->SetLabelSize(0.05);
  //mg_all->Draw("A");
  //std::string c_yield_ratio_all_name = "results/yield/yield_ratio_all.pdf";
  //c_yield_ratio_all->SaveAs(c_yield_ratio_all_name.c_str());

  return 0;
}
