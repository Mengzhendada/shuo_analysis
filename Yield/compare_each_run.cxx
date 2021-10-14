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

int compare_each_run(){
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
      std::string canvas_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5);
      std::string q2x_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5)+"_yieldratio";
      TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
      TH1D* h_neg_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
      TH1D* h_pos_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x_Q2_z_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5)+"_"+std::to_string(z).substr(0,4);
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs;
          auto runjs = it.value();
            
          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;
          
          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_neg_sim = new TH1D("","neg sim sum",100,0,1);
          TH1D *h_z_neg_sim_incrad = new TH1D("","neg sim inc rad",100,0,1);
          h_z_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          TH1D *h_z_neg_sim_excrad = new TH1D("","neg sim exc rad",100,0,1);
          h_z_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          TH1D *h_z_neg_sim_rho = new TH1D("","neg sim rho",100,0,1);
          h_z_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("z_neg_rho");
          TH1D *h_z_neg_sim_delta = new TH1D("","neg sim delta",100,0,1);
          h_z_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          h_z_neg_sim->Add(h_z_neg_sim_incrad,1);
          h_z_neg_sim->Add(h_z_neg_sim_excrad,1);
          //h_z_neg_sim->Add(h_z_neg_sim_rho,1);
          h_z_neg_sim->Add(h_z_neg_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_pos_sim = new TH1D("","pos sim sum",100,0,1);
          TH1D *h_z_pos_sim_incrad = new TH1D("","pos sim inc rad",100,0,1);
          h_z_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          TH1D *h_z_pos_sim_excrad = new TH1D("","pos sim exc rad",100,0,1);
          h_z_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          TH1D *h_z_pos_sim_rho = new TH1D("","pos sim rho",100,0,1);
          h_z_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("z_pos_rho");
          TH1D *h_z_pos_sim_delta = new TH1D("","pos sim delta",100,0,1);
          h_z_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          h_z_pos_sim->Add(h_z_pos_sim_incrad,1);
          h_z_pos_sim->Add(h_z_pos_sim_excrad,1);
          //h_z_pos_sim->Add(h_z_pos_sim_rho,1);
          h_z_pos_sim->Add(h_z_pos_sim_delta,1);
          
          double charge_neg_all = 0,charge_pos_all=0;
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            
            TMultiGraph *mg_neg = new TMultiGraph();
            int i_color = 0;
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_all += charge;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_z_neg = new TH1D("","",100,0,1);
              h_z_neg = (TH1D*)root_file_neg->Get("z");
              TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
              h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_z_neg->Scale(1/(charge*EFF));
              h_z_neg->Add(h_z_neg_bg,-1/(charge*6*EFF));
              
              TGraphErrors* g_negrun = new TGraphErrors(h_z_neg);
              g_negrun->SetMarkerColor(coolcolor[i_color]);
              g_negrun->SetLineColor(coolcolor[i_color]);
              g_negrun->SetTitle(std::to_string(RunNumber).c_str());
              mg_neg->Add(g_negrun,"P");
          
              i_color++;
            }//loop over neg runs
              TGraph* g_yield_negsim = new TGraph(h_z_neg_sim);
              g_yield_negsim->SetTitle("simc");
              mg_neg->Add(g_yield_negsim,"L");

            TCanvas* c_yield_neg = new TCanvas();
            mg_neg->GetXaxis()->SetTitle("z");
            mg_neg->GetYaxis()->SetTitle("yield");
            mg_neg->Draw("A");
            c_yield_neg->BuildLegend(0.85,0.65,0.95,0.95);
            std::string c_yield_neg_name = "results/yield/check/yieldcheck_"+std::to_string(RunGroup)+"_neg.pdf";
            c_yield_neg->SaveAs(c_yield_neg_name.c_str());
            TMultiGraph *mg_pos = new TMultiGraph();
            i_color = 0;
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_all+=charge;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH1D *h_z_pos = new TH1D("","",100,0,1);
              h_z_pos = (TH1D*)root_file_pos->Get("z");
              TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
              h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_z_pos->Scale(1/(charge*EFF));
              h_z_pos->Add(h_z_pos_bg,-1/(6*charge*EFF));
            
              TGraphErrors* g_posrun = new TGraphErrors(h_z_pos);
              g_posrun->SetMarkerColor(warmcolor[i_color]);
              g_posrun->SetLineColor(warmcolor[i_color]);
              g_posrun->SetTitle(std::to_string(RunNumber).c_str());
              mg_pos->Add(g_posrun,"P");
              i_color++;
            }//loop over pos runs
              TGraph* g_yield_possim = new TGraph(h_z_pos_sim);
              g_yield_possim->SetTitle("simc");
              mg_pos->Add(g_yield_possim,"L");
            TCanvas* c_yield_pos = new TCanvas();
            mg_pos->GetXaxis()->SetTitle("z");
            mg_pos->GetYaxis()->SetTitle("yield");
            mg_pos->Draw("A");
            c_yield_pos->BuildLegend(0.85,0.65,0.95,0.95);
            std::string c_yield_pos_name = "results/yield/check/yieldcheck_"+std::to_string(RunGroup)+"_pos.pdf";
            c_yield_pos->SaveAs(c_yield_pos_name.c_str());
          }//if z not 0
          
        }//loop over z
      }//if x,Q2 not 0
    }//loop over Q2
  }//loop over x
  return 0;
}
