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

int plot_H2_D2(){
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
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::string diff_canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",diff";
      std::string sum_canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"sum";
      std::string diff_canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_diff";
      std::string sum_canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_sum";
      std::string q2xdiff_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_diff_Ratio";
      std::string q2xsum_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_sum_Ratio";
      TCanvas *c_Q2x_diff_ratio = new TCanvas("",q2xdiff_name.c_str(),1900,1000);
      TCanvas *c_Q2x_sum_ratio = new TCanvas("",q2xsum_name.c_str(),1900,1000);
      int i_color = 1;
      auto mg_diff = new TMultiGraph();
      auto mg_sum = new TMultiGraph();
      auto mg_z_H2D2_pos_ratio = new TMultiGraph();
      auto mg_z_H2D2_neg_ratio = new TMultiGraph();
      bool H2_runs = false;
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_z_neg_all = new TH1D("","neg;z;",100,0,1);
          h_z_neg_all->GetXaxis()->SetTitle("z");
          h_z_neg_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_all = new TH1D("","pos;z;",100,0,1);
          h_z_pos_all->GetXaxis()->SetTitle("z");
          h_z_neg_all->Sumw2();
          h_z_pos_all->Sumw2();
          TH1D* h_z_neg_bg_all = new TH1D("","neg bg;z;",100,0,1);
          h_z_neg_bg_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_bg_all = new TH1D("","pos bg;z;",100,0,1);
          h_z_pos_bg_all->GetXaxis()->SetTitle("z");
          h_z_neg_bg_all->Sumw2();
          h_z_pos_bg_all->Sumw2();
          TH1D* h_z_neg_Dummy_all = new TH1D("","neg;z;",100,0,1);
          h_z_neg_Dummy_all->GetXaxis()->SetTitle("z");
          h_z_neg_Dummy_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_Dummy_all = new TH1D("","pos;z;",100,0,1);
          h_z_pos_Dummy_all->GetXaxis()->SetTitle("z");
          h_z_neg_Dummy_all->Sumw2();
          h_z_pos_Dummy_all->Sumw2();
          TH1D* h_z_neg_bg_Dummy_all = new TH1D("","neg bg;z;",100,0,1);
          h_z_neg_bg_Dummy_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_bg_Dummy_all = new TH1D("","pos bg;z;",100,0,1);
          h_z_pos_bg_Dummy_all->GetXaxis()->SetTitle("z");
          h_z_neg_bg_Dummy_all->Sumw2();
          h_z_pos_bg_Dummy_all->Sumw2();
          TH1D* h_z_neg_H2_all = new TH1D("","neg;z;",100,0,1);
          h_z_neg_H2_all->GetXaxis()->SetTitle("z");
          h_z_neg_H2_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_H2_all = new TH1D("","pos;z;",100,0,1);
          h_z_pos_H2_all->GetXaxis()->SetTitle("z");
          h_z_neg_H2_all->Sumw2();
          h_z_pos_H2_all->Sumw2();
          TH1D* h_z_neg_bg_H2_all = new TH1D("","neg bg;z;",100,0,1);
          h_z_neg_bg_H2_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_bg_H2_all = new TH1D("","pos bg;z;",100,0,1);
          h_z_pos_bg_H2_all->GetXaxis()->SetTitle("z");
          h_z_neg_bg_H2_all->Sumw2();
          h_z_pos_bg_H2_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs,neg_H2_runs,pos_H2_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;


          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_neg_sim = new TH1D("","pi- sim sum",100,0,1);
          TH1D *h_z_neg_sim_incrad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_z_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          TH1D *h_z_neg_sim_incnorad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_z_neg_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_norad");
          TH1D *h_z_neg_sim_rho = new TH1D("","pi- sim rho",100,0,1);
          h_z_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("z_neg_rho");
          TH1D *h_z_neg_sim_excrad = new TH1D("","pi- sim exc",100,0,1);
          h_z_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          TH1D *h_z_neg_sim_delta = new TH1D("","pi- sim delta",100,0,1);
          h_z_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          h_z_neg_sim->Add(h_z_neg_sim_incrad,1);
          h_z_neg_sim->Add(h_z_neg_sim_excrad,1);
          //h_z_neg_sim->Add(h_z_neg_sim_rho,1);
          h_z_neg_sim->Add(h_z_neg_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_pos_sim = new TH1D("","pi+ sim sum",100,0,1);
          TH1D *h_z_pos_sim_incrad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_z_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          TH1D *h_z_pos_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_z_pos_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_norad");
          TH1D *h_z_pos_sim_excrad = new TH1D("","pi+ sim exc",100,0,1);
          h_z_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          TH1D *h_z_pos_sim_rho = new TH1D("","pi+ sim rho",100,0,1);
          h_z_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("z_pos_rho");
          TH1D *h_z_pos_sim_delta = new TH1D("","pi+ sim delta",100,0,1);
          h_z_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          h_z_pos_sim->Add(h_z_pos_sim_incrad,1);
          h_z_pos_sim->Add(h_z_pos_sim_excrad,1);
          //h_z_pos_sim->Add(h_z_pos_sim_rho,1);
          h_z_pos_sim->Add(h_z_pos_sim_delta,1);

          double charge_neg_all = 0,charge_pos_all=0;
          double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;
          double charge_neg_H2_all = 0,charge_pos_H2_all=0;
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            if(!runjs["neg"]["H2"].empty() && !runjs["pos"]["H2"].empty()){
              neg_H2_runs = runjs["neg"]["H2"].get<std::vector<int>>();
              pos_H2_runs = runjs["pos"]["H2"].get<std::vector<int>>();
              TFile *rootfile_neg_H2_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_H2_simc.root").c_str());
              TH1D *h_z_neg_H2_sim = new TH1D("","pi- sim sum",100,0,1);
              TH1D *h_z_neg_H2_sim_incrad = new TH1D("","pi- sim SIDIS",100,0,1);
              h_z_neg_H2_sim_incrad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_inc_rad");
              TH1D *h_z_neg_H2_sim_incnorad = new TH1D("","pi- sim SIDIS",100,0,1);
              h_z_neg_H2_sim_incnorad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_inc_norad");
              //TH1D *h_z_neg_H2_sim_excrad = new TH1D("","pi- sim exc",100,0,1);
              //h_z_neg_H2_sim_excrad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_exc_rad");
              TH1D *h_z_neg_H2_sim_rho = new TH1D("","pi- sim rho",100,0,1);
              h_z_neg_H2_sim_rho = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_rho");
              TH1D *h_z_neg_H2_sim_delta = new TH1D("","pi- sim delta",100,0,1);
              h_z_neg_H2_sim_delta = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_delta");
              h_z_neg_H2_sim->Add(h_z_neg_H2_sim_incrad,1);
              //h_z_neg_H2_sim->Add(h_z_neg_H2_sim_excrad,1);
              //h_z_neg_H2_sim->Add(h_z_neg_H2_sim_rho,1);
              h_z_neg_H2_sim->Add(h_z_neg_H2_sim_delta,1);
              TFile *rootfile_pos_H2_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_H2_simc.root").c_str());
              TH1D *h_z_pos_H2_sim = new TH1D("","pi+ sim sum",100,0,1);
              TH1D *h_z_pos_H2_sim_incrad = new TH1D("","pi+ sim SIDIS",100,0,1);
              h_z_pos_H2_sim_incrad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_inc_rad");
              TH1D *h_z_pos_H2_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,0,1);
              h_z_pos_H2_sim_incnorad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_inc_norad");
              TH1D *h_z_pos_H2_sim_excrad = new TH1D("","pi+ sim exc",100,0,1);
              h_z_pos_H2_sim_excrad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_exc_rad");
              TH1D *h_z_pos_H2_sim_rho = new TH1D("","pi+ sim rho",100,0,1);
              h_z_pos_H2_sim_rho = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_rho");
              TH1D *h_z_pos_H2_sim_delta = new TH1D("","pi+ sim delta",100,0,1);
              h_z_pos_H2_sim_delta = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_delta");
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_incrad,1);
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_excrad,1);
              //h_z_pos_H2_sim->Add(h_z_pos_H2_sim_rho,1);
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_delta,1);

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
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                //h_z_neg_all->Add(h_z_neg_bg,-1/(charge*TE));
                //h_z_neg_all->Add(h_z_neg,1/(charge*TE));
                h_z_neg_bg_all->Add(h_z_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_neg_all->Add(h_z_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
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
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                //h_z_pos_all->Add(h_z_pos_bg,-1/(charge*TE));
                //h_z_pos_all->Add(h_z_pos,1/(charge*TE));
                h_z_pos_bg_all->Add(h_z_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_pos_all->Add(h_z_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              }//loop over pos runs
              for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<"Dummy"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_neg_Dummy_all += charge;
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
                double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
                double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
                //double TE = 1;
                //std::cout<<"neg Dummy TE check "<<std::endl;
                double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
                double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
                double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
                double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
                //std::cout<<"neg Dummy DE check "<<std::endl;
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                h_z_neg_bg_Dummy_all->Add(h_z_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_neg_Dummy_all->Add(h_z_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              }//loop over neg runs
              for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<"Dummy "<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_Dummy_all+=charge;
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
                double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
                double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
                //std::cout<<"pos TE check"<<std::endl;
                double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
                double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
                double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
                double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
                //std::cout<<"pos DE check"<<std::endl;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                h_z_pos_bg_Dummy_all->Add(h_z_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_pos_Dummy_all->Add(h_z_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              }//loop over pos runs
              for(auto it = neg_H2_runs.begin();it!=neg_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"H2"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_neg_H2_all += charge;
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
                double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
                double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
                //double TE = 1;
                std::cout<<"neg H2 TE check "<<std::endl;
                double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
                double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
                double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
                double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
                std::cout<<"neg H2 DE check "<<std::endl;
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                h_z_neg_bg_H2_all->Add(h_z_neg_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_neg_H2_all->Add(h_z_neg,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              }//loop over neg runs
              for(auto it = pos_H2_runs.begin();it!=pos_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"H2 "<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_H2_all+=charge;
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
                double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
                double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
                std::cout<<"pos TE check"<<std::endl;
                double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
                double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
                double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
                double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
                std::cout<<"pos DE check"<<std::endl;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                h_z_pos_bg_H2_all->Add(h_z_pos_bg,1/(6*TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
                h_z_pos_H2_all->Add(h_z_pos,1/(TLT*TEHMS*TE*HMS_cal_eff*HMS_cer_eff*SHMS_cal_eff*SHMS_aero_eff));
              }//loop over pos runs


              h_z_neg_all->Scale(1/charge_neg_all);
              h_z_pos_all->Scale(1/charge_pos_all);

              h_z_neg_bg_all->Scale(1/charge_neg_all);
              h_z_pos_bg_all->Scale(1/charge_pos_all);

              h_z_neg_Dummy_all->Scale(1/charge_neg_Dummy_all);
              h_z_pos_Dummy_all->Scale(1/charge_pos_Dummy_all);

              h_z_neg_bg_Dummy_all->Scale(1/charge_neg_Dummy_all);
              h_z_pos_bg_Dummy_all->Scale(1/charge_pos_Dummy_all);

              h_z_neg_H2_all->Scale(1/charge_neg_H2_all);
              h_z_pos_H2_all->Scale(1/charge_pos_H2_all);

              h_z_neg_bg_H2_all->Scale(1/charge_neg_H2_all);
              h_z_pos_bg_H2_all->Scale(1/charge_pos_H2_all);

              //bg subtracted
              h_z_neg_all->Add(h_z_neg_bg_all,-1.0);
              h_z_pos_all->Add(h_z_pos_bg_all,-1.0);

              //Dummy subtract
              h_z_neg_Dummy_all->Add(h_z_neg_bg_Dummy_all,1.0);
              h_z_pos_Dummy_all->Add(h_z_pos_bg_Dummy_all,1.0);

              h_z_neg_all->Add(h_z_neg_Dummy_all,-0.245);
              h_z_pos_all->Add(h_z_pos_Dummy_all,-0.245);

              h_z_neg_H2_all->Add(h_z_neg_Dummy_all,-0.262);
              h_z_pos_H2_all->Add(h_z_pos_Dummy_all,-0.262);

              //h_z_neg_all->Rebin(2);
              //h_z_pos_all->Rebin(2);
              //h_z_neg_H2_all->Rebin(2);
              //h_z_pos_H2_all->Rebin(2);
             // 
             // h_z_neg_H2_sim_delta->Rebin(2);
             // h_z_neg_H2_sim_incnorad->Rebin(2);
             // h_z_neg_H2_sim_incrad->Rebin(2);
             // h_z_pos_H2_sim_excrad->Rebin(2);
             // h_z_pos_H2_sim_delta->Rebin(2);
             // h_z_pos_H2_sim_incnorad->Rebin(2);
             // h_z_pos_H2_sim_incrad->Rebin(2);
             // h_z_neg_sim_excrad->Rebin(2);
             // h_z_neg_sim_delta->Rebin(2);
             // h_z_neg_sim_incnorad->Rebin(2);
             // h_z_neg_sim_incrad->Rebin(2);
             // h_z_pos_sim_excrad->Rebin(2);
             // h_z_pos_sim_delta->Rebin(2);
             // h_z_pos_sim_incnorad->Rebin(2);
             // h_z_pos_sim_incrad->Rebin(2);

              //bgs from SIMC
              //h_z_neg_all->Add(h_z_neg_sim_excrad,-1);
              //h_z_pos_all->Add(h_z_pos_sim_excrad,-1);
              //h_z_neg_all->Add(h_z_neg_sim_delta,-1);
              //h_z_pos_all->Add(h_z_pos_sim_delta,-1);

              ////h_z_neg_H2_all->Add(h_z_neg_H2_sim_excrad,-1);
              //h_z_pos_H2_all->Add(h_z_pos_H2_sim_excrad,-1);
              //h_z_neg_H2_all->Add(h_z_neg_H2_sim_delta,-1);
              //h_z_pos_H2_all->Add(h_z_pos_H2_sim_delta,-1);

              TH1D* h_z_neg_D2_yield = new TH1D("","D2yield;yield;z",100,0,1);
              TH1D* h_z_neg_H2_yield = new TH1D("","H2yield;yield;z",100,0,1);
              TH1D* h_z_pos_D2_yield = new TH1D("","D2yield;yield;z",100,0,1);
              TH1D* h_z_pos_H2_yield = new TH1D("","H2yield;yield;z",100,0,1);
              h_z_neg_D2_yield->Sumw2();
              h_z_neg_H2_yield->Sumw2();
              h_z_pos_D2_yield->Sumw2();
              h_z_pos_H2_yield->Sumw2();
              h_z_neg_D2_yield = h_z_neg_all;
              h_z_neg_H2_yield = h_z_neg_H2_all;
              h_z_pos_D2_yield = h_z_pos_all;
              h_z_pos_H2_yield = h_z_pos_H2_all;

              h_z_neg_D2_yield->Rebin(4);
              h_z_neg_H2_yield->Rebin(4);
              h_z_pos_D2_yield->Rebin(4);
              h_z_pos_H2_yield->Rebin(4);


              auto h_z_neg_incnorad = h_z_neg_sim_incnorad;
              auto h_z_neg_incrad = h_z_neg_sim_incrad;
              auto h_z_pos_incnorad = h_z_pos_sim_incnorad;
              auto h_z_pos_incrad = h_z_pos_sim_incrad;

              auto h_z_neg_H2_incnorad = h_z_neg_H2_sim_incnorad;
              auto h_z_neg_H2_incrad = h_z_neg_H2_sim_incrad;
              auto h_z_pos_H2_incnorad = h_z_pos_H2_sim_incnorad;
              auto h_z_pos_H2_incrad = h_z_pos_H2_sim_incrad;


              //h_z_neg_all->Rebin(5);
              //h_z_pos_all->Rebin(5);
              //h_z_neg_sim->Rebin(5);
              //h_z_pos_sim->Rebin(5);
              //h_z_neg_incnorad->Rebin(5);
              //h_z_neg_incrad->Rebin(5);
              //h_z_neg_sim_delta->Rebin(5);
              //h_z_neg_sim_rho->Rebin(5);
              //h_z_pos_incnorad->Rebin(5);
              //h_z_pos_incrad->Rebin(5);
              //h_z_pos_sim_excrad->Rebin(5);
              //h_z_pos_sim_delta->Rebin(5);
              //h_z_pos_sim_rho->Rebin(5);
              ///auto rp_radia_corr_neg = new TRatioPlot(h_z_neg_sim_incnorad,h_z_neg_sim_incrad);
              ///auto rp_radia_corr_pos = new TRatioPlot(h_z_pos_sim_incnorad,h_z_pos_sim_incrad);
              TH1* rp_radia_corr_neg = (TH1*)h_z_neg_incnorad->Clone("rp_radia_corr");
              TH1* rp_radia_corr_pos = (TH1*)h_z_pos_incnorad->Clone("rp_radia_corr");
              rp_radia_corr_neg ->Divide(h_z_neg_incnorad,h_z_neg_incrad);          
              rp_radia_corr_pos ->Divide(h_z_pos_incnorad,h_z_pos_incrad);          
              //TCanvas* c_radia_corr_sim = new TCanvas();
              //c_radia_corr_sim->Divide(2,1);
              //c_radia_corr_sim->cd(1);
              //rp_radia_corr_neg->Draw();
              //c_radia_corr_sim->cd(2);
              //rp_radia_corr_pos->Draw();
              //std::string c_radia_corr_sim_name = "results/yield/statistics_corr/radia_corr_"+q2xz_str_filename+".pdf";
              //c_radia_corr_sim->SaveAs(c_radia_corr_sim_name.c_str());

              h_z_neg_D2_yield->Multiply(h_z_neg_D2_yield,rp_radia_corr_neg);
              h_z_pos_D2_yield->Multiply(h_z_pos_D2_yield,rp_radia_corr_pos);

              TH1* rp_radia_corr_neg_H2 = (TH1*)h_z_neg_H2_incnorad->Clone("rp_radia_corr");
              TH1* rp_radia_corr_pos_H2 = (TH1*)h_z_pos_H2_incnorad->Clone("rp_radia_corr");
              rp_radia_corr_neg_H2 ->Divide(h_z_neg_H2_incnorad,h_z_neg_H2_incrad);          
              rp_radia_corr_pos_H2 ->Divide(h_z_pos_H2_incnorad,h_z_pos_H2_incrad);          
              //TCanvas* c_radia_corr_sim_H2 = new TCanvas();
              //c_radia_corr_sim_H2->Divide(2,1);
              //c_radia_corr_sim_H2->cd(1);
              //rp_radia_corr_neg_H2->Draw();
              //c_radia_corr_sim_H2->cd(2);
              //rp_radia_corr_pos_H2->Draw();
              //std::string c_radia_corr_sim_H2_name = "results/yield/statistics_corr/radia_corr_"+q2xz_str_filename+"_H2.pdf";
              //c_radia_corr_sim_H2->SaveAs(c_radia_corr_sim_H2_name.c_str());

              h_z_neg_H2_yield->Multiply(h_z_neg_H2_yield,rp_radia_corr_neg_H2);
              h_z_pos_H2_yield->Multiply(h_z_pos_H2_yield,rp_radia_corr_pos_H2);


              TGraphErrors* g_z_pos_H2_yield = new TGraphErrors(h_z_pos_H2_yield);
              g_z_pos_H2_yield->SetTitle("pi+ H2 data yield");
              TGraphErrors* g_z_pos_D2_yield = new TGraphErrors(h_z_pos_D2_yield);
              g_z_pos_D2_yield->SetTitle("pi+ D2 data yield");

              //int nbins_yield_pos = h_z_pos_H2_yield->GetXaxis()->GetNbins();
              //int ii_yield_pos = 0;
              //for(int i = 0;i<nbins_yield_pos;i++){
              //  //std::cout<<i<<std::endl;
              //  double x = h_z_pos_H2_yield->GetBinCenter(i);
              //  double y = h_z_pos_H2_yield->GetBinContent(i);
              //  double error = h_z_pos_H2_yield->GetBinError(i);
              //  //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
              //  if(y!=0){
              //    g_z_pos_H2_yield->SetPoint(ii_yield_pos,x,y);
              //    g_z_pos_H2_yield->SetPointError(ii_yield_pos,0,error);
              //    ii_yield_pos++;
              //  }
              //}
              TCanvas *c_H2_yield_pos = new TCanvas();
              gStyle->SetOptTitle(0);
              TMultiGraph* mg_z_H2_yield_pos = new TMultiGraph(); 
              g_z_pos_H2_yield->GetYaxis()->SetTitle("yield");
              g_z_pos_H2_yield->GetYaxis()->SetTitleSize(0.53);
              g_z_pos_H2_yield->GetXaxis()->SetRangeUser(0.3,1);
              g_z_pos_H2_yield->SetMarkerStyle(4);
              mg_z_H2_yield_pos->Add(g_z_pos_H2_yield,"P");
              mg_z_H2_yield_pos->Add(g_z_pos_D2_yield,"P");
              TGraph* g_z_pos_sim = new TGraph(h_z_pos_H2_sim);
              TGraph* g_z_pos_sim_incrad = new TGraph(h_z_pos_H2_sim_incrad);
              TGraph* g_z_pos_sim_incnorad = new TGraph(h_z_pos_H2_sim_incnorad);
              TGraph* g_z_pos_sim_excrad = new TGraph(h_z_pos_H2_sim_excrad);
              TGraph* g_z_pos_sim_delta = new TGraph(h_z_pos_H2_sim_delta);
              mg_z_H2_yield_pos->Add(g_z_pos_sim); 
              g_z_pos_sim_incrad->SetLineColor(kOrange);
              g_z_pos_sim_incrad->SetTitle("pi- sim SIDIS");
              mg_z_H2_yield_pos->Add(g_z_pos_sim_incrad); 
              g_z_pos_sim_excrad->SetLineColor(kBlue);
              g_z_pos_sim_excrad->SetTitle("pi- sim exc");
              mg_z_H2_yield_pos->Add(g_z_pos_sim_excrad); 
              g_z_pos_sim_delta->SetLineColor(46);
              g_z_pos_sim_delta->SetTitle("pi- sim delta");
              mg_z_H2_yield_pos->Add(g_z_pos_sim_delta); 
              g_z_pos_sim_incnorad->SetLineColor(40);
              g_z_pos_sim_incnorad->SetTitle("pi- sim inc norad");
              mg_z_H2_yield_pos->Add(g_z_pos_sim_incnorad,"L");

              mg_z_H2_yield_pos->SetTitle(q2xz_str.c_str());
              mg_z_H2_yield_pos->GetYaxis()->SetTitle("yield");
              mg_z_H2_yield_pos->GetXaxis()->SetTitle("z");
              mg_z_H2_yield_pos->Draw("A");
              c_H2_yield_pos->BuildLegend(0.75,0.75,1,1);
              std::string c_H2_yield_pos_name = "results/yield/statistics_H2/yield_"+q2xz_str_filename+"_H2_pos.pdf";
              c_H2_yield_pos->SaveAs(c_H2_yield_pos_name.c_str());

              TGraphErrors* g_z_neg_H2_yield = new TGraphErrors(h_z_neg_H2_yield);
              g_z_neg_H2_yield->SetTitle("pi- H2 data yield");
              TGraphErrors* g_z_neg_D2_yield = new TGraphErrors(h_z_neg_D2_yield);
              g_z_neg_D2_yield->SetTitle("pi- D2 data yield");
              //int nbins_yield_neg = h_z_neg_H2_yield->GetXaxis()->GetNbins();
              //int ii_yield_neg = 0;
              //for(int i = 0;i<nbins_yield_neg;i++){
              //  //std::cout<<i<<std::endl;
              //  double x = h_z_neg_H2_yield->GetBinCenter(i);
              //  double y = h_z_neg_H2_yield->GetBinContent(i);
              //  double error = h_z_neg_H2_yield->GetBinError(i);
              //  //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
              //  if(y!=0){
              //    g_z_neg_H2_yield->SetPoint(ii_yield_neg,x,y);
              //    g_z_neg_H2_yield->SetPointError(ii_yield_neg,0,error);
              //    ii_yield_neg++;
              //  }
              //}
              TCanvas *c_H2_yield_neg = new TCanvas();
              gStyle->SetOptTitle(0);
              TMultiGraph* mg_z_H2_yield_neg = new TMultiGraph(); 
              g_z_neg_H2_yield->GetYaxis()->SetTitle("yield");
              g_z_neg_H2_yield->GetYaxis()->SetTitleSize(0.53);
              g_z_neg_H2_yield->GetXaxis()->SetRangeUser(0.3,1);
              g_z_neg_H2_yield->SetMarkerStyle(4);
              mg_z_H2_yield_neg->Add(g_z_neg_H2_yield,"P");
              mg_z_H2_yield_neg->Add(g_z_neg_D2_yield,"P");

              TGraph* g_z_neg_sim = new TGraph(h_z_neg_H2_sim);
              TGraph* g_z_neg_sim_incrad = new TGraph(h_z_neg_H2_sim_incrad);
              TGraph* g_z_neg_sim_incnorad = new TGraph(h_z_neg_H2_sim_incnorad);
              TGraph* g_z_neg_sim_delta = new TGraph(h_z_neg_H2_sim_delta);
              mg_z_H2_yield_neg->Add(g_z_neg_sim); 
              g_z_neg_sim_incrad->SetLineColor(kOrange);
              g_z_neg_sim_incrad->SetTitle("pi- sim SIDIS");
              mg_z_H2_yield_neg->Add(g_z_neg_sim_incrad); 
              g_z_neg_sim_delta->SetLineColor(46);
              g_z_neg_sim_delta->SetTitle("pi- sim delta");
              mg_z_H2_yield_neg->Add(g_z_neg_sim_delta); 
              g_z_neg_sim_incnorad->SetLineColor(40);
              g_z_neg_sim_incnorad->SetTitle("pi- sim inc norad");
              mg_z_H2_yield_neg->Add(g_z_neg_sim_incnorad,"L");

              mg_z_H2_yield_neg->SetTitle(q2xz_str.c_str());
              mg_z_H2_yield_neg->GetYaxis()->SetTitle("yield");
              mg_z_H2_yield_neg->GetXaxis()->SetTitle("z");
              mg_z_H2_yield_neg->Draw("A");
              c_H2_yield_neg->BuildLegend(0.75,0.75,1,1);
              std::string c_H2_yield_neg_name = "results/yield/statistics_H2/yield_"+q2xz_str_filename+"_H2_neg.pdf";
              c_H2_yield_neg->SaveAs(c_H2_yield_neg_name.c_str());

              TH1D* h_z_H2D2_pos_ratio = (TH1D*)h_z_pos_H2_yield->Clone("pos_ratio");
              TH1D* h_z_H2D2_neg_ratio = (TH1D*)h_z_neg_H2_yield->Clone("neg_ratio");
              h_z_H2D2_pos_ratio->Divide(h_z_pos_H2_yield,h_z_pos_D2_yield);
              h_z_H2D2_neg_ratio->Divide(h_z_neg_H2_yield,h_z_neg_D2_yield);
              TGraphErrors* g_z_H2D2_pos_ratio = new TGraphErrors(h_z_H2D2_pos_ratio);
              TGraphErrors* g_z_H2D2_neg_ratio = new TGraphErrors(h_z_H2D2_neg_ratio);
              g_z_H2D2_pos_ratio->SetMarkerColor(i_color);
              g_z_H2D2_neg_ratio->SetMarkerColor(i_color);
              g_z_H2D2_pos_ratio->SetMarkerStyle(4);
              g_z_H2D2_neg_ratio->SetMarkerStyle(4);
              mg_z_H2D2_pos_ratio->Add(g_z_H2D2_pos_ratio,"P");
              mg_z_H2D2_neg_ratio->Add(g_z_H2D2_neg_ratio,"P");
              TCanvas* c_H2D2 = new TCanvas();
              //h_z_H2D2_pos_ratio->Draw();
              g_z_H2D2_pos_ratio->Draw("AP");
              g_z_H2D2_pos_ratio->SetMinimum(0.2);
              g_z_H2D2_pos_ratio->SetMaximum(1);
              std::string c_H2D2_name = "results/yield/statistics_H2/H2D2_"+q2xz_str_filename+"_pos.pdf";
              c_H2D2->SaveAs(c_H2D2_name.c_str());



              TH1D* h_z_H2_diff = new TH1D("","protondiff",100,0,1);
              TH1D* h_z_H2_sum = new TH1D("","proton sum",100,0,1);
              TH1D* h_z_D2_diff = new TH1D("","deuterium diff",100,0,1);
              TH1D* h_z_D2_sum = new TH1D("","deuterium sum",100,0,1);

              //h_z_H2_diff->Rebin(4);
              //h_z_H2_sum->Rebin(4);
              //h_z_D2_diff->Rebin(4);
              //h_z_D2_sum->Rebin(4);

              h_z_H2_diff->Sumw2();
              h_z_H2_sum->Sumw2();
              h_z_D2_diff->Sumw2();
              h_z_D2_sum->Sumw2();

              h_z_H2_diff->Add(h_z_pos_H2_yield,h_z_neg_H2_yield,1,-1);
              h_z_H2_sum->Add(h_z_pos_H2_yield,h_z_neg_H2_yield,1,1);
              h_z_D2_diff->Add(h_z_pos_D2_yield,h_z_neg_D2_yield,1,-1);
              h_z_D2_sum->Add(h_z_pos_D2_yield,h_z_neg_D2_yield,1,1);

              //TCanvas *c_diff = new TCanvas();
              //h_z_H2_diff->SetMarkerStyle(4);
              //h_z_H2_diff->SetTitle("H2 pi+ - pi-");
              //h_z_D2_diff->SetTitle("D2 pi+ - pi-");
              //h_z_D2_diff->GetXaxis()->SetTitle("z");
              //h_z_D2_diff->GetYaxis()->SetTitle("pi+pi-diff");
              //h_z_D2_diff->Draw("AP");
              //h_z_H2_diff->Draw("P same");
              //c_diff->BuildLegend(0.75,0.75,1,1);
              //std::string c_diff_name = "results/yield/statistics_H2/"+q2xz_str_filename+"_diff.pdf";
              //c_diff->SaveAs(c_diff_name.c_str());
              //TCanvas *c_sum = new TCanvas();
              //h_z_H2_sum->SetMarkerStyle(4);
              //h_z_H2_sum->SetTitle("H2 pi+ - pi-");
              //h_z_D2_sum->SetTitle("D2 pi+ - pi-");
              //h_z_D2_sum->GetXaxis()->SetTitle("z");
              //h_z_D2_sum->GetYaxis()->SetTitle("pi+pi-sum");
              //h_z_D2_sum->Draw("AP");
              //h_z_H2_sum->Draw("P same");
              //c_sum->BuildLegend(0.75,0.75,1,1);
              //std::string c_sum_name = "results/yield/statistics_H2/"+q2xz_str_filename+"_sum.pdf";
              //c_sum->SaveAs(c_sum_name.c_str());

              TH1D* h_diff_ratio = new TH1D("","H2 diff/ D2 diff",100,0,1);
              TH1D* h_sum_ratio = new TH1D("","H2 sum/ D2 sum",100,0,1);

              h_diff_ratio->Sumw2();
              h_sum_ratio->Sumw2();
              //h_diff_ratio->Divide(h_z_H2_diff,h_z_D2_diff);
              //h_sum_ratio->Divide(h_z_H2_sum,h_z_D2_sum);

              int nbins = h_diff_ratio->GetXaxis()->GetNbins();

              TGraphErrors* g_diff_ratio = new TGraphErrors();
              std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
              g_diff_ratio->SetName(z_string.c_str());

              int ii = 0;
              for(int i = 0;i<nbins;i++){
                double x = h_z_pos_H2_yield->GetBinCenter(i)+0.001*i_color;
                double y_H2_pos = h_z_pos_H2_yield->GetBinContent(i);
                double y_H2_neg = h_z_neg_H2_yield->GetBinContent(i);
                double y_D2_pos = h_z_pos_D2_yield->GetBinContent(i);
                double y_D2_neg = h_z_neg_D2_yield->GetBinContent(i);
                double error_H2_pos = h_z_pos_H2_yield->GetBinError(i);
                double error_H2_neg = h_z_neg_H2_yield->GetBinError(i);
                double error_D2_pos = h_z_pos_D2_yield->GetBinError(i);
                double error_D2_neg = h_z_neg_D2_yield->GetBinError(i);
                double y_H2 = y_H2_pos-y_H2_neg;
                double y_D2 = y_D2_pos-y_D2_neg;
                double y = y_H2/y_D2;
                double error_H2 = std::sqrt((error_H2_pos*error_H2_pos)/charge_pos_H2_all+(error_H2_neg*error_H2_neg)/charge_neg_H2_all);
                double error_D2 = std::sqrt((error_D2_pos*error_D2_pos)/charge_pos_all+(error_D2_neg*error_D2_neg)/charge_neg_all);
                double error = std::sqrt(error_H2*error_H2/(y_H2*y_H2)+error_D2*error_D2/(y_D2*y_D2));
                //std::cout<<i<<std::endl;
                //double x = h_diff_ratio->GetBinCenter(i)+0.001*i_color;
                //double y = h_diff_ratio->GetBinContent(i);
                /*double x = h_z_H2_diff->GetBinCenter(i)+0.001*i_color;
                double y_H2 = h_z_H2_diff->GetBinContent(i);
                double y_D2 = h_z_D2_diff->GetBinContent(i);
                double error_H2 = h_z_H2_diff->GetBinError(i);
                double error_D2 = h_z_D2_diff->GetBinError(i);
                double y = y_H2/y_D2;
                double error = std::sqrt(error_H2*error_H2/(y_H2*y_H2)+error_D2*error_D2/(y_D2*y_D2));
                */
                //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
                if(y!=0 && error< 1){
                  g_diff_ratio->SetPoint(ii,x,y);
                  g_diff_ratio->SetPointError(ii,0,error);
                  ii++;
                }
              }
              TGraphErrors* g_sum_ratio = new TGraphErrors();
              g_sum_ratio->SetName(z_string.c_str());

              int ii_sum = 0;
              for(int i = 0;i<nbins;i++){
                double x = h_z_pos_H2_yield->GetBinCenter(i)+0.001*i_color;
                double y_H2_pos = h_z_pos_H2_yield->GetBinContent(i);
                double y_H2_neg = h_z_neg_H2_yield->GetBinContent(i);
                double y_D2_pos = h_z_pos_D2_yield->GetBinContent(i);
                double y_D2_neg = h_z_neg_D2_yield->GetBinContent(i);
                double error_H2_pos = h_z_pos_H2_yield->GetBinError(i);
                double error_H2_neg = h_z_neg_H2_yield->GetBinError(i);
                double error_D2_pos = h_z_pos_D2_yield->GetBinError(i);
                double error_D2_neg = h_z_neg_D2_yield->GetBinError(i);
                double y_H2 = y_H2_pos+y_H2_neg;
                double y_D2 = y_D2_pos+y_D2_neg;
                double y = y_H2/y_D2;
                double error_H2 = std::sqrt((error_H2_pos*error_H2_pos)/charge_pos_H2_all+(error_H2_neg*error_H2_neg)/charge_neg_H2_all);
                double error_D2 = std::sqrt((error_D2_pos*error_D2_pos)/charge_pos_all+(error_D2_neg*error_D2_neg)/charge_neg_all);
                double error = std::sqrt(error_H2*error_H2/(y_H2*y_H2)+error_D2*error_D2/(y_D2*y_D2));
                //std::cout<<i<<std::endl;
                //double x = h_sum_ratio->GetBinCenter(i)+0.001*i_color;
                //double y = h_sum_ratio->GetBinContent(i);
                //double error = h_sum_ratio->GetBinError(i);
                /*double x = h_z_H2_sum->GetBinCenter(i)+0.001*i_color;
                double y_H2 = h_z_H2_sum->GetBinContent(i);
                double y_D2 = h_z_D2_sum->GetBinContent(i);
                double error_H2 = h_z_H2_sum->GetBinError(i);
                double error_D2 = h_z_D2_sum->GetBinError(i);
                double y = y_H2/y_D2;
                double error = std::sqrt(error_H2*error_H2/(y_H2*y_H2)+error_D2*error_D2/(y_D2*y_D2));
               */
                //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
                if(y!=0 && error< 0.2){
                  g_sum_ratio->SetPoint(ii_sum,x,y);
                  g_sum_ratio->SetPointError(ii_sum,0,error);
                  ii_sum++;
                }
              }
              /* 
                 int nbins_sim = h_z_neg_sim_incnorad->GetXaxis()->GetNbins();
                 TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
                 std::string z_string_sim = "z simc "+(std::to_string(z)).substr(0,4);
                 g_yield_ratio_sim->SetName(z_string_sim.c_str());
              //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_z_neg_sim_incnorad);
              int ii_sim = 0;
              for(int i = 0;i<nbins_sim;i++){
              //std::cout<<i<<std::endl;
              double x = h_z_neg_sim_incnorad->GetBinCenter(i)+0.001*i_color;
              double y = h_z_neg_sim_incnorad->GetBinContent(i);
              double error = h_z_neg_sim_incnorad->GetBinError(i);
              //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
              if(y!=0){
              g_yield_ratio_sim->SetPoint(ii_sim,x,y);
              //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
              ii_sim++;
              }
              }

              // int nbins = h_z_pos_all->GetXaxis()->GetNbins();
              // TGraphErrors* g_yield_ratio = new TGraphErrors();
              // std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
              // g_yield_ratio->SetName(z_string.c_str());
              // int ii = 0;
              // for(int i = 0;i<nbins;i++){
              //   //std::cout<<i<<std::endl;
              //   double x = h_z_pos_all->GetBinCenter(i);
              //   double y = h_z_pos_all->GetBinContent(i);
              //   double error = h_z_pos_all->GetBinError(i);
              //   //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
              //   if(y!=0){
              //     g_yield_ratio->SetPoint(ii,x,y);
              //     g_yield_ratio->SetPointError(ii,0,error);
              //     ii++;
              //   }
              // }
              */


              //c_Q2x_ratio->cd();
              //h_z_neg_all->SetLineColor(i_color);
              g_diff_ratio->SetMarkerStyle(4);
              g_diff_ratio->SetMarkerColor(i_color);
              g_diff_ratio->SetLineColor(i_color);
              g_sum_ratio->SetMarkerStyle(4);
              g_sum_ratio->SetMarkerColor(i_color);
              g_sum_ratio->SetLineColor(i_color);
              //  g_yield_ratio_sim->SetMarkerStyle(5);
              //  g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
              //  g_yield_ratio_sim->SetLineColor(i_color);
              //hs->Add(h_z_neg_all);
              //h_z_neg_all->Draw("same");
              mg_diff->Add(g_diff_ratio,"P");
              mg_sum->Add(g_sum_ratio,"P");
              // mg->Add(g_yield_ratio_sim,"L");
              //c_Q2x_ratio->Update();


              TCanvas *c_Q2x_z_diff_ratio = new TCanvas(q2xdiff_name.c_str(),q2xdiff_name.c_str(),1900,1000);
              g_diff_ratio->GetXaxis()->SetRangeUser(0.1,1);
              g_diff_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
              g_diff_ratio->GetXaxis()->SetTitle("z");
              g_diff_ratio->GetYaxis()->SetTitle("diff_ratio");
              g_diff_ratio->GetXaxis()->SetTitleSize(0.053);
              g_diff_ratio->GetYaxis()->SetTitleSize(0.053);
              g_diff_ratio->Draw("AP"); 
              //          g_diff_ratio_sim->Draw("L");
              c_Q2x_z_diff_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
              std::string c_Q2x_z_diff_ratio_name = "results/yield/statistics_H2/"+q2xz_str_filename+"_diff_ratio.pdf";

              c_Q2x_z_diff_ratio->SaveAs(c_Q2x_z_diff_ratio_name.c_str());

              TCanvas *c_Q2x_z_sum_ratio = new TCanvas(q2xsum_name.c_str(),q2xsum_name.c_str(),1900,1000);
              //TCanvas *c_Q2x_z_ratio = new TCanvas();
              // //h_z_neg_all->Draw();
              g_sum_ratio->GetXaxis()->SetRangeUser(0.1,1);
              g_sum_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
              g_sum_ratio->GetXaxis()->SetTitle("z");
              g_sum_ratio->GetYaxis()->SetTitle("sum_ratio");
              g_sum_ratio->GetXaxis()->SetTitleSize(0.053);
              g_sum_ratio->GetYaxis()->SetTitleSize(0.053);
              g_sum_ratio->Draw("AP"); 
              //          g_sum_ratio_sim->Draw("L");
              c_Q2x_z_sum_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
              std::string c_Q2x_z_sum_ratio_name = "results/yield/statistics_H2/"+q2xz_str_filename+"_sum_ratio.pdf";

              c_Q2x_z_sum_ratio->SaveAs(c_Q2x_z_sum_ratio_name.c_str());

              i_color++;
              H2_runs = true;
            }//if H2 runs exist 
              else{std::cout<<"No H2 runs"<<std::endl;}
          }//if z not 0
        }//loop over z
        if(H2_runs){
          c_Q2x_diff_ratio->cd();
          c_Q2x_diff_ratio->SetTitle(diff_canvas_name.c_str());
          //hs->Draw();
          mg_diff->SetTitle(diff_canvas_name.c_str());
          mg_diff->GetXaxis()->SetTitle("z");
          mg_diff->GetYaxis()->SetTitle("H2/D2 diff ratio");
          mg_diff->GetXaxis()->SetTitleSize(0.053);
          mg_diff->GetYaxis()->SetTitleSize(0.053);
          mg_diff->GetXaxis()->SetLabelSize(0.05);
          mg_diff->GetYaxis()->SetLabelSize(0.05);
          std::string diff_name = canvas_name+",z";
          mg_diff->GetXaxis()->SetTitle(diff_name.c_str());
          mg_diff->SetMinimum(0.1);
          mg_diff->SetMaximum(1.2);
          mg_diff->Draw("A");
          mg_diff->GetXaxis()->SetLimits(0.35,1);
          //auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
          //TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
          //fit->Draw("same");
          std::string c_Q2x_diff_ratio_name = "results/yield/statistics_H2/"+diff_canvas_filename+"_ratio.pdf";
          c_Q2x_diff_ratio->BuildLegend(0.7,0.7,0.9,0.9);
          c_Q2x_diff_ratio->SaveAs(c_Q2x_diff_ratio_name.c_str());

          c_Q2x_sum_ratio->cd();
          c_Q2x_sum_ratio->SetTitle(sum_canvas_name.c_str());
          //hs->Draw();
          mg_sum->SetTitle(sum_canvas_name.c_str());
          mg_sum->GetXaxis()->SetTitle("z");
          mg_sum->GetYaxis()->SetTitle("H2/D2 sum ratio");
          mg_sum->GetXaxis()->SetTitleSize(0.053);
          mg_sum->GetYaxis()->SetTitleSize(0.053);
          mg_sum->GetXaxis()->SetLabelSize(0.05);
          mg_sum->GetYaxis()->SetLabelSize(0.05);
          std::string sum_name = canvas_name+",z";
          mg_sum->GetXaxis()->SetTitle(sum_name.c_str());
          mg_sum->SetMinimum(0.1);
          mg_sum->SetMaximum(1.2);
          mg_sum->Draw("A");
          mg_sum->GetXaxis()->SetLimits(0.35,1);
          //auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
          //TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
          //fit->Draw("same");
          std::string c_Q2x_sum_ratio_name = "results/yield/statistics_H2/"+sum_canvas_filename+"_ratio.pdf";
          c_Q2x_sum_ratio->BuildLegend(0.7,0.7,0.9,0.9);
          c_Q2x_sum_ratio->SaveAs(c_Q2x_sum_ratio_name.c_str());
          
          TCanvas* c_z_H2D2_pos_ratio = new TCanvas();
          mg_z_H2D2_pos_ratio->SetTitle(sum_canvas_name.c_str());
          mg_z_H2D2_pos_ratio->GetXaxis()->SetTitle("z");
          mg_z_H2D2_pos_ratio->GetYaxis()->SetTitle("H2/D2 Yield ratio");
          mg_z_H2D2_pos_ratio->GetXaxis()->SetTitleSize(0.053);
          mg_z_H2D2_pos_ratio->GetYaxis()->SetTitleSize(0.053);
          mg_z_H2D2_pos_ratio->GetXaxis()->SetLabelSize(0.05);
          mg_z_H2D2_pos_ratio->GetYaxis()->SetLabelSize(0.05);
          std::string H2D2_pos_name = canvas_name+",z";
          mg_z_H2D2_pos_ratio->GetXaxis()->SetTitle(H2D2_pos_name.c_str());
          mg_z_H2D2_pos_ratio->Draw("A");
          mg_z_H2D2_pos_ratio->GetXaxis()->SetLimits(0.25,1);
          mg_z_H2D2_pos_ratio->SetMinimum(0.2);
          mg_z_H2D2_pos_ratio->SetMaximum(1.2);
          std::string c_z_H2D2_pos_ratio_name = "results/yield/statistics_H2/"+canvas_filename+"_H2D2_ratio.pdf";
          c_z_H2D2_pos_ratio->SaveAs(c_z_H2D2_pos_ratio_name.c_str());
          TCanvas* c_z_H2D2_neg_ratio = new TCanvas();
          mg_z_H2D2_neg_ratio->SetTitle(sum_canvas_name.c_str());
          mg_z_H2D2_neg_ratio->GetXaxis()->SetTitle("z");
          mg_z_H2D2_neg_ratio->GetYaxis()->SetTitle("H2/D2 Yield ratio");
          mg_z_H2D2_neg_ratio->GetXaxis()->SetTitleSize(0.053);
          mg_z_H2D2_neg_ratio->GetYaxis()->SetTitleSize(0.053);
          mg_z_H2D2_neg_ratio->GetXaxis()->SetLabelSize(0.05);
          mg_z_H2D2_neg_ratio->GetYaxis()->SetLabelSize(0.05);
          std::string H2D2_neg_name = canvas_name+",z";
          mg_z_H2D2_neg_ratio->GetXaxis()->SetTitle(H2D2_neg_name.c_str());
          mg_z_H2D2_neg_ratio->Draw("A");
          mg_z_H2D2_neg_ratio->GetXaxis()->SetLimits(0.25,1);
          mg_z_H2D2_neg_ratio->SetMinimum(0.2);
          mg_z_H2D2_neg_ratio->SetMaximum(1.2);
          std::string c_z_H2D2_neg_ratio_name = "results/yield/statistics_H2/"+canvas_filename+"_H2D2_ratio.pdf";
          c_z_H2D2_neg_ratio->SaveAs(c_z_H2D2_neg_ratio_name.c_str());
        }//if H2 runs exist, then ploting
      }//if x,Q2 not 0
    }//loop over Q2
  }//loop over x


  return 0;
}
