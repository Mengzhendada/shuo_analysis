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

int plot_xs(){
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
  json jout;
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_xs_H2_pos = new TMultiGraph();
  auto mg_xs_H2_neg = new TMultiGraph();
  auto mg_xs_D2_pos = new TMultiGraph();
  auto mg_xs_D2_neg = new TMultiGraph();
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      int i_color = 1;
      bool H2_runs = false;
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
        TGraphErrors* g_z_pos_H2_xs = new TGraphErrors();
        TGraphErrors* g_z_neg_H2_xs = new TGraphErrors();
        TGraphErrors* g_z_pos_D2_xs = new TGraphErrors();
        TGraphErrors* g_z_neg_D2_xs = new TGraphErrors();
        g_z_pos_H2_xs->SetTitle(canvas_name.c_str());
        g_z_neg_H2_xs->SetTitle(canvas_name.c_str());
        g_z_pos_D2_xs->SetTitle(canvas_name.c_str());
        g_z_neg_D2_xs->SetTitle(canvas_name.c_str());
        int i_H2_pos = 0,i_H2_neg = 0,i_D2_pos = 0,i_D2_neg = 0;
          double z = std::stod(it.key());
          std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_z_neg_D2_all = new TH1D("","neg;z;",100,0,1);
          h_z_neg_D2_all->GetXaxis()->SetTitle("z");
          //h_z_neg_D2_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_D2_all = new TH1D("","pos;z;",100,0,1);
          h_z_pos_D2_all->GetXaxis()->SetTitle("z");
          h_z_neg_D2_all->Sumw2();
          h_z_pos_D2_all->Sumw2();
          TH1D* h_z_neg_D2_bg_all = new TH1D("","neg bg;z;",100,0,1);
          h_z_neg_D2_bg_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_D2_bg_all = new TH1D("","pos bg;z;",100,0,1);
          h_z_pos_D2_bg_all->GetXaxis()->SetTitle("z");
          h_z_neg_D2_bg_all->Sumw2();
          h_z_pos_D2_bg_all->Sumw2();
          TH1D* h_z_neg_Dummy_all = new TH1D("","neg;z;",100,0,1);
          h_z_neg_Dummy_all->GetXaxis()->SetTitle("z");
          //h_z_neg_Dummy_all->GetYaxis()->SetRangeUser(0,2);
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
          //h_z_neg_H2_all->GetYaxis()->SetRangeUser(0,2);
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
          TH1D *h_z_neg_D2_sim = new TH1D("","pi- sim sum",100,0,1);
          TH1D *h_z_neg_D2_sim_incrad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_z_neg_D2_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          TH1D *h_z_neg_D2_sim_incnorad = new TH1D("","pi- sim SIDIS",100,0,1);
          h_z_neg_D2_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_norad");
          TH1D *h_z_neg_D2_sim_rho = new TH1D("","pi- sim rho",100,0,1);
          h_z_neg_D2_sim_rho = (TH1D*)rootfile_neg_sim->Get("z_neg_rho");
          TH1D *h_z_neg_D2_sim_excrad = new TH1D("","pi- sim exc",100,0,1);
          h_z_neg_D2_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          TH1D *h_z_neg_D2_sim_delta = new TH1D("","pi- sim delta",100,0,1);
          h_z_neg_D2_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          TH2D *h_xs_z_neg_D2 = new TH2D();
          h_xs_z_neg_D2 = (TH2D*)rootfile_neg_sim->Get("xs_z_neg_inc_norad");
          h_xs_z_neg_D2->RebinX(5);
          h_xs_z_neg_D2->RebinY(5);
          auto xs_z_neg_D2 = h_xs_z_neg_D2->ProfileX("xs_z_neg",1,-1,"d");
          h_xs_z_neg_D2->SetTitle(z_string.c_str());
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_incrad,1);
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_excrad,1);
          //h_z_neg_D2_sim->Add(h_z_neg_D2_sim_rho,1);
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_pos_D2_sim = new TH1D("","pi+ sim sum",100,0,1);
          TH1D *h_z_pos_D2_sim_incrad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_z_pos_D2_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          TH1D *h_z_pos_D2_sim_incnorad = new TH1D("","pi+ sim SIDIS",100,0,1);
          h_z_pos_D2_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_norad");
          TH1D *h_z_pos_D2_sim_excrad = new TH1D("","pi+ sim exc",100,0,1);
          h_z_pos_D2_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          TH1D *h_z_pos_D2_sim_rho = new TH1D("","pi+ sim rho",100,0,1);
          h_z_pos_D2_sim_rho = (TH1D*)rootfile_pos_sim->Get("z_pos_rho");
          TH1D *h_z_pos_D2_sim_delta = new TH1D("","pi+ sim delta",100,0,1);
          h_z_pos_D2_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          TH2D *h_xs_z_pos_D2 = new TH2D();
          h_xs_z_pos_D2 = (TH2D*)rootfile_pos_sim->Get("xs_z_pos_inc_norad");
          h_xs_z_pos_D2->SetTitle(z_string.c_str());
          h_xs_z_pos_D2->RebinX(5);
          h_xs_z_pos_D2->RebinY(5);
          auto xs_z_pos_D2 = h_xs_z_pos_D2->ProfileX("xs_z_pos_D2",1,-1,"d");
          h_z_pos_D2_sim->Add(h_z_pos_D2_sim_incrad,1);
          h_z_pos_D2_sim->Add(h_z_pos_D2_sim_excrad,1);
          //h_z_pos_D2_sim->Add(h_z_pos_D2_sim_rho,1);
          h_z_pos_D2_sim->Add(h_z_pos_D2_sim_delta,1);

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
              TH2D *h_xs_z_neg_H2 = new TH2D();
              h_xs_z_neg_H2 = (TH2D*)rootfile_neg_H2_sim->Get("xs_z_neg_inc_norad");
              h_xs_z_neg_H2->SetTitle(z_string.c_str());
              h_xs_z_neg_H2->RebinX(5);
              h_xs_z_neg_H2->RebinY(5);
              auto xs_z_neg_H2 = h_xs_z_neg_H2->ProfileX("xs_z_neg_H2",1,-1,"d");
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
              TH2D *h_xs_z_pos_H2 = new TH2D();
              h_xs_z_pos_H2 = (TH2D*)rootfile_pos_H2_sim->Get("xs_z_pos_inc_norad");
              h_xs_z_pos_H2->SetTitle(z_string.c_str());
              h_xs_z_pos_H2->RebinX(5);
              h_xs_z_pos_H2->RebinY(5);
              auto xs_z_pos_H2 = h_xs_z_pos_H2->ProfileX("xs_z_pos_H2",1,-1,"d");
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_incrad,1);
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_excrad,1);
              //h_z_pos_H2_sim->Add(h_z_pos_H2_sim_rho,1);
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_delta,1);
              TCanvas *c_xs_H2_neg = new TCanvas();
              xs_z_neg_H2->GetYaxis()->SetTitle("H2 neg xs");
              xs_z_neg_H2->GetXaxis()->SetTitle("z");
              xs_z_neg_H2->SetTitle((canvas_name+";z;H2 neg xs").c_str());
              xs_z_neg_H2->Draw();
              c_xs_H2_neg->BuildLegend();
              gPad->Modified();
              gPad->Update();
              std::string c_xs_H2_neg_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_neg_xs.pdf";
              c_xs_H2_neg->SaveAs(c_xs_H2_neg_name.c_str());
              TCanvas *c_xs_H2_pos = new TCanvas();
              xs_z_pos_H2->GetYaxis()->SetTitle("H2 pos xs");
              xs_z_pos_H2->GetXaxis()->SetTitle("z");
              xs_z_pos_H2->SetTitle((canvas_name+";z;H2 pos xs").c_str());
              xs_z_pos_H2->Draw();
              c_xs_H2_pos->BuildLegend();
              gPad->Modified();
              gPad->Update();
              std::string c_xs_H2_pos_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_pos_xs.pdf";
              c_xs_H2_pos->SaveAs(c_xs_H2_pos_name.c_str());
              TCanvas *c_xs_D2_neg = new TCanvas();
              xs_z_neg_D2->GetYaxis()->SetTitle("D2 neg xs");
              xs_z_neg_D2->GetXaxis()->SetTitle("z");
              xs_z_neg_D2->SetTitle((canvas_name+";z;D2 neg xs").c_str());
              xs_z_neg_D2->Draw();
              c_xs_D2_neg->BuildLegend();
              gPad->Modified();
              gPad->Update();
              std::string c_xs_D2_neg_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_neg_xs.pdf";
              c_xs_D2_neg->SaveAs(c_xs_D2_neg_name.c_str());
              TCanvas *c_xs_D2_pos = new TCanvas();
              xs_z_pos_D2->GetYaxis()->SetTitle("D2 pos xs");
              xs_z_pos_D2->GetXaxis()->SetTitle("z");
              xs_z_pos_D2->SetTitle((canvas_name+";z;D2 pos xs").c_str());
              xs_z_pos_D2->Draw();
              c_xs_D2_pos->BuildLegend();
              gPad->Modified();
              gPad->Update();
              std::string c_xs_D2_pos_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_pos_xs.pdf";
              c_xs_D2_pos->SaveAs(c_xs_D2_pos_name.c_str());

              for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_neg_all += charge;
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                //h_z_neg_D2_all->Add(h_z_neg_bg,-1/(charge*TE));
                //h_z_neg_D2_all->Add(h_z_neg,1/(charge*TE));
                //h_z_neg_D2_bg_all->Add(h_z_neg_bg,1/(6));
                double EFF = Get_all_eff(RunNumber);
                h_z_neg_D2_bg_all->Add(h_z_neg_bg,1/(6*EFF));
                h_z_neg_D2_all->Add(h_z_neg,1/EFF);
              }//loop over neg runs
              for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_all+=charge;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                double EFF = Get_all_eff(RunNumber);
                h_z_pos_D2_bg_all->Add(h_z_pos_bg,1/(6*EFF));
                h_z_pos_D2_all->Add(h_z_pos,1/EFF);
              }//loop over pos runs
              for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<"Dummy"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_neg_Dummy_all += charge;
                //std::cout<<"neg Dummy DE check "<<std::endl;
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                double EFF = Get_all_eff(RunNumber);
                h_z_neg_bg_Dummy_all->Add(h_z_neg_bg,1/(6*EFF));
                h_z_neg_Dummy_all->Add(h_z_neg,1/EFF);
              }//loop over neg runs
              for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<"Dummy "<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_Dummy_all+=charge;
                //std::cout<<"pos DE check"<<std::endl;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                double EFF = Get_all_eff(RunNumber);
                h_z_pos_bg_Dummy_all->Add(h_z_pos_bg,1/(6*EFF));
                h_z_pos_Dummy_all->Add(h_z_pos,1/EFF);
              }//loop over pos runs
              for(auto it = neg_H2_runs.begin();it!=neg_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"H2"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_neg_H2_all += charge;
                std::cout<<"neg H2 DE check "<<std::endl;
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",100,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",100,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                //h_z_neg_bg_H2_all->Add(h_z_neg_bg,1/(6));
                double EFF = Get_all_eff(RunNumber);
                h_z_neg_bg_H2_all->Add(h_z_neg_bg,1/(6*EFF));
                h_z_neg_H2_all->Add(h_z_neg,1/EFF);
              }//loop over neg runs
              for(auto it = pos_H2_runs.begin();it!=pos_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"H2 "<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_H2_all+=charge;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",100,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",100,0,1);
                h_z_pos_bg = (TH1D*)root_file_pos->Get("z_bg");
                //h_z_pos_bg_H2_all->Add(h_z_pos_bg,1/(6));
                double EFF = Get_all_eff(RunNumber);
                h_z_pos_bg_H2_all->Add(h_z_pos_bg,1/(6*EFF));
                h_z_pos_H2_all->Add(h_z_pos,1/EFF);
              }//loop over pos runs

              //bg subtracted
              h_z_neg_D2_all->Add(h_z_neg_D2_bg_all,-1.0);
              h_z_pos_D2_all->Add(h_z_pos_D2_bg_all,-1.0);
              h_z_neg_H2_all->Add(h_z_neg_bg_H2_all,-1.0);
              h_z_pos_H2_all->Add(h_z_pos_bg_H2_all,-1.0);

              auto h_z_neg_D2_incnorad = h_z_neg_D2_sim_incnorad;
              auto h_z_neg_D2_incrad = h_z_neg_D2_sim_incrad;
              auto h_z_pos_D2_incnorad = h_z_pos_D2_sim_incnorad;
              auto h_z_pos_D2_incrad = h_z_pos_D2_sim_incrad;

              auto h_z_neg_H2_incnorad = h_z_neg_H2_sim_incnorad;
              auto h_z_neg_H2_incrad = h_z_neg_H2_sim_incrad;
              auto h_z_pos_H2_incnorad = h_z_pos_H2_sim_incnorad;
              auto h_z_pos_H2_incrad = h_z_pos_H2_sim_incrad;


              h_z_neg_D2_all->Rebin(5);
              h_z_pos_D2_all->Rebin(5);
              h_z_neg_H2_all->Rebin(5);
              h_z_pos_H2_all->Rebin(5);
              h_z_neg_Dummy_all->Rebin(5);
              h_z_pos_Dummy_all->Rebin(5);
              h_z_neg_D2_incnorad->Rebin(5);
              h_z_neg_D2_incrad->Rebin(5);
              h_z_neg_D2_sim_excrad->Rebin(5);
              h_z_neg_D2_sim_delta->Rebin(5);
              h_z_neg_D2_sim_rho->Rebin(5);

              h_z_pos_D2_incnorad->Rebin(5);
              h_z_pos_D2_incrad->Rebin(5);
              h_z_pos_D2_sim_excrad->Rebin(5);
              h_z_pos_D2_sim_delta->Rebin(5);
              h_z_pos_D2_sim_rho->Rebin(5);

              h_z_neg_H2_sim_delta->Rebin(5);
              h_z_neg_H2_sim_incnorad->Rebin(5);
              h_z_neg_H2_sim_incrad->Rebin(5);
              h_z_pos_H2_sim_excrad->Rebin(5);
              h_z_pos_H2_sim_delta->Rebin(5);
              h_z_pos_H2_sim_incnorad->Rebin(5);
              h_z_pos_H2_sim_incrad->Rebin(5);


              TH1D* h_z_neg_H2_yield = (TH1D*)h_z_neg_H2_all->Clone();
              TH1D* h_z_pos_H2_yield = (TH1D*)h_z_pos_H2_all->Clone();
              h_z_neg_H2_yield->Scale(1/charge_neg_H2_all);
              h_z_pos_H2_yield->Scale(1/charge_pos_H2_all);
              TH1D* h_z_neg_Dummy_yield = (TH1D*)h_z_neg_Dummy_all->Clone();
              TH1D* h_z_pos_Dummy_yield = (TH1D*)h_z_pos_Dummy_all->Clone();
              h_z_neg_Dummy_yield->Scale(0.262/charge_neg_Dummy_all);
              h_z_pos_Dummy_yield->Scale(0.262/charge_pos_Dummy_all);
              TGraphErrors* g_z_neg_H2_yield = new TGraphErrors(h_z_neg_H2_yield);
              TGraphErrors* g_z_pos_H2_yield = new TGraphErrors(h_z_pos_H2_yield);
              TGraphErrors* g_z_neg_Dummy_yield = new TGraphErrors(h_z_neg_Dummy_yield);
              TGraphErrors* g_z_pos_Dummy_yield = new TGraphErrors(h_z_pos_Dummy_yield);
              TGraph* g_z_neg_H2_sim_incrad = new TGraph(h_z_neg_H2_sim_incrad);
              TGraph* g_z_neg_H2_sim_incnorad = new TGraph(h_z_neg_H2_sim_incnorad);
              TGraph* g_z_neg_H2_sim_delta = new TGraph(h_z_neg_H2_sim_delta);
              TGraph* g_z_pos_H2_sim_incrad = new TGraph(h_z_pos_H2_sim_incrad);
              TGraph* g_z_pos_H2_sim_incnorad = new TGraph(h_z_pos_H2_sim_incnorad);
              TGraph* g_z_pos_H2_sim_delta = new TGraph(h_z_pos_H2_sim_delta);
              TGraph* g_z_pos_H2_sim_excrad = new TGraph(h_z_pos_H2_sim_excrad);
              g_z_neg_H2_sim_incrad->SetLineColor(kOrange);
              g_z_neg_H2_sim_incnorad->SetLineColor(40);
              g_z_neg_H2_sim_delta->SetLineColor(46);
              g_z_pos_H2_sim_incrad->SetLineColor(kOrange);
              g_z_pos_H2_sim_excrad->SetLineColor(kBlue);
              g_z_pos_H2_sim_incnorad->SetLineColor(40);
              g_z_pos_H2_sim_delta->SetLineColor(46);

              TCanvas *c_yield_H2_neg = new TCanvas();
              g_z_neg_H2_yield->Draw("AP");
              g_z_neg_Dummy_yield->Draw("P same");
              g_z_neg_H2_sim_incrad->Draw("L same");              
              g_z_neg_H2_sim_incnorad->Draw("L same");              
              g_z_neg_H2_sim_delta->Draw("L same");              
              c_yield_H2_neg->BuildLegend();
              std::string c_yield_H2_neg_name = "results/yield/statistics_H2/yield_"+q2xz_str_filename+"_neg.pdf";
              c_yield_H2_neg->SaveAs(c_yield_H2_neg_name.c_str());
              TCanvas *c_yield_H2_pos = new TCanvas();
              g_z_pos_H2_yield->Draw("AP");
              g_z_pos_Dummy_yield->Draw("P same");
              g_z_pos_H2_sim_incrad->Draw("L same");              
              g_z_pos_H2_sim_incnorad->Draw("L same");              
              g_z_pos_H2_sim_delta->Draw("L same");              
              g_z_pos_H2_sim_excrad->Draw("L same");              
              c_yield_H2_pos->BuildLegend();
              std::string c_yield_H2_pos_name = "results/yield/statistics_H2/yield_"+q2xz_str_filename+"_pos.pdf";
              c_yield_H2_pos->SaveAs(c_yield_H2_pos_name.c_str());


              int nbins = h_z_pos_H2_all->GetXaxis()->GetNbins();

              int ii_posH2D2ratio = 0,ii_negH2D2ratio = 0,i_H2_pos_datasim = 0,i_H2_neg_datasim = 0,i_D2_pos_datasim = 0,i_D2_neg_datasim = 0;
              for(int i = 0;i<nbins;i++){
                double x = h_z_pos_H2_all->GetBinCenter(i);
                //+0.001*i_color;
                double x_center = h_z_pos_H2_all->GetBinCenter(i);
                double y_H2_pos_raw = h_z_pos_H2_all->GetBinContent(i)/charge_pos_H2_all;
                double y_H2_neg_raw = h_z_neg_H2_all->GetBinContent(i)/charge_neg_H2_all;
                double y_H2_pos_delta = h_z_pos_H2_sim_delta->GetBinContent(i);
                double y_H2_neg_delta = h_z_neg_H2_sim_delta->GetBinContent(i);
                double y_H2_pos_exc = h_z_pos_H2_sim_excrad->GetBinContent(i);
                //double y_H2_neg_exc = h_z_neg_H2_sim_excrad->GetBinContent(i);
                double y_D2_pos_raw = h_z_pos_D2_all->GetBinContent(i)/charge_pos_all;
                double y_D2_neg_raw = h_z_neg_D2_all->GetBinContent(i)/charge_neg_all;
                double y_D2_pos_delta = h_z_pos_D2_sim_delta->GetBinContent(i);
                double y_D2_neg_delta = h_z_neg_D2_sim_delta->GetBinContent(i);
                double y_D2_pos_exc = h_z_pos_D2_sim_excrad->GetBinContent(i);
                double y_D2_neg_exc = h_z_neg_D2_sim_excrad->GetBinContent(i);
                double y_Dummy_pos = h_z_pos_Dummy_all->GetBinContent(i)/charge_pos_Dummy_all;
                double y_Dummy_neg = h_z_neg_Dummy_all->GetBinContent(i)/charge_neg_Dummy_all;
                double error_H2_pos = h_z_pos_H2_all->GetBinError(i)/charge_pos_H2_all;
                double error_H2_neg = h_z_neg_H2_all->GetBinError(i)/charge_neg_H2_all;
                double error_D2_pos = h_z_pos_D2_all->GetBinError(i)/charge_pos_all;
                double error_D2_neg = h_z_neg_D2_all->GetBinError(i)/charge_neg_all;
                double error_Dummy_pos = h_z_pos_Dummy_all->GetBinError(i)/charge_pos_Dummy_all;
                double error_Dummy_neg = h_z_neg_Dummy_all->GetBinError(i)/charge_neg_Dummy_all;


                double H2_neg_yield_incrad = h_z_neg_H2_sim_incrad->GetBinContent(i);
                double H2_pos_yield_incrad = h_z_pos_H2_sim_incrad->GetBinContent(i);
                double D2_neg_yield_incrad = h_z_neg_D2_sim_incrad->GetBinContent(i);
                double D2_pos_yield_incrad = h_z_pos_D2_sim_incrad->GetBinContent(i);

                double y_H2_pos = y_H2_pos_raw-0.262*y_Dummy_pos;
                H2_pos_yield_incrad = H2_pos_yield_incrad+y_H2_pos_delta+y_H2_pos_exc;
                //if(y_H2_pos>0){
                //std::cout<<"H2 pos yield raw "<<y_H2_pos_raw<<" Dummy "<<y_Dummy_pos<<" delta "<<y_H2_pos_delta<<" exc "<<y_H2_pos_exc<<std::endl;}
                double y_H2_neg = y_H2_neg_raw-0.262*y_Dummy_neg;
                H2_neg_yield_incrad = H2_neg_yield_incrad+y_H2_neg_delta;
                double y_D2_pos = y_D2_pos_raw-0.245*y_Dummy_pos;
                D2_pos_yield_incrad = D2_pos_yield_incrad+y_D2_pos_delta+y_D2_pos_exc;
                double y_D2_neg = y_D2_neg_raw-0.245*y_Dummy_neg;
                D2_neg_yield_incrad = D2_neg_yield_incrad+y_D2_neg_delta+y_D2_neg_exc;
                error_H2_pos = std::sqrt(error_H2_pos*error_H2_pos+0.262*0.262*error_Dummy_pos*error_Dummy_pos);
                error_H2_neg = std::sqrt(error_H2_neg*error_H2_neg+0.262*0.262*error_Dummy_neg*error_Dummy_neg);
                error_D2_pos = std::sqrt(error_D2_pos*error_D2_pos+0.245*0.245*error_Dummy_pos*error_Dummy_pos);
                error_D2_neg = std::sqrt(error_D2_neg*error_D2_neg+0.245*0.245*error_Dummy_neg*error_Dummy_neg);
                std::cout<<"incrad "<<H2_pos_yield_incrad<<"data"<<y_H2_pos<<std::endl;

                double H2_xs_z_neg,H2_xs_z_pos,D2_xs_z_neg,D2_xs_z_pos,D2_xs_z_neg_2;
                //g_xs_z_neg_H2_sim->GetPoint(i,x_center,H2_xs_z_neg);
                //g_xs_z_pos_H2_sim->GetPoint(i,x_center,H2_xs_z_pos);
                D2_xs_z_neg = xs_z_neg_D2->GetBinContent(i);
                //D2_xs_z_neg_2 = xs_z_neg_D2->GetBinContent(xs_z_neg_D2->GetXaxis()->FindBin(x_center));
                D2_xs_z_pos = xs_z_pos_D2->GetBinContent(xs_z_pos_D2->GetXaxis()->FindBin(x_center));
                H2_xs_z_neg = xs_z_neg_H2->GetBinContent(xs_z_neg_H2->GetXaxis()->FindBin(x_center));
                H2_xs_z_pos = xs_z_pos_H2->GetBinContent(xs_z_pos_H2->GetXaxis()->FindBin(x_center));



                double xs_H2_pos = y_H2_pos*H2_xs_z_pos/(H2_pos_yield_incrad);
                double xs_H2_neg = y_H2_neg*H2_xs_z_neg/(H2_neg_yield_incrad);
                double xs_D2_pos = y_D2_pos*D2_xs_z_pos/(D2_pos_yield_incrad);
                double xs_D2_neg = y_D2_neg*D2_xs_z_neg/(D2_neg_yield_incrad);
                double  error_xs_H2_pos = error_H2_pos*H2_xs_z_pos/(H2_pos_yield_incrad);
                double  error_xs_H2_neg = error_H2_neg*H2_xs_z_neg/(H2_neg_yield_incrad);
                double  error_xs_D2_pos = error_D2_pos*D2_xs_z_pos/(D2_pos_yield_incrad);
                double  error_xs_D2_neg = error_D2_neg*D2_xs_z_neg/(D2_neg_yield_incrad);
                
                std::cout<<"H2 xs pos "<<xs_H2_pos<<"H2 xs neg "<<xs_H2_neg<<" D2 xs pos "<<xs_D2_pos<<" D2 xs neg "<<xs_D2_neg<<std::endl;
                std::cout<<" data yield"<<y_H2_pos<<"xs"<<H2_xs_z_pos<<"sim yield "<<H2_pos_yield_incrad<<std::endl;

                if(xs_H2_neg>0){
                g_z_neg_H2_xs->SetPoint(i_H2_neg,x_center,xs_H2_neg);
                g_z_neg_H2_xs->SetPointError(i_H2_neg,0,error_xs_H2_neg);
                i_H2_neg++;
                }
                if(xs_H2_pos>0){
                g_z_pos_H2_xs->SetPoint(i_H2_pos,x_center,xs_H2_pos);
                g_z_pos_H2_xs->SetPointError(i_H2_pos,0,error_xs_H2_pos);
                i_H2_pos++;
                }
                if(xs_D2_neg>0){
                g_z_neg_D2_xs->SetPoint(i_D2_neg,x_center,xs_D2_neg);
                g_z_neg_D2_xs->SetPointError(i_D2_neg,0,error_xs_D2_neg);
                i_D2_neg++;
                }
                if(xs_D2_pos>0){
                g_z_pos_D2_xs->SetPoint(i_D2_pos,x_center,xs_D2_pos);
                g_z_pos_D2_xs->SetPointError(i_D2_pos,0,error_xs_D2_pos);
                i_D2_pos++;
                }
  TCanvas *c_H2_pos= new TCanvas();
  g_z_pos_H2_xs->Draw("AP");
  c_H2_pos->BuildLegend();
  c_H2_pos->SaveAs("results/yield/statistics_H2/H2_pos_xs.pdf");
              }
            }//if H2 runs exist 
            else{std::cout<<"No H2 runs"<<std::endl;}
          }//if z not 0
        if(H2_runs){
          g_z_pos_H2_xs->SetMarkerColor(warmcolor[i_color]);
          g_z_pos_H2_xs->SetMarkerStyle(4);
          mg_xs_H2_pos->Add(g_z_pos_H2_xs,"P");
          g_z_neg_H2_xs->SetMarkerColor(warmcolor[i_color]);
          g_z_neg_H2_xs->SetMarkerStyle(4);
          mg_xs_H2_neg->Add(g_z_neg_H2_xs,"P");
          g_z_pos_D2_xs->SetMarkerColor(warmcolor[i_color]);
          g_z_pos_D2_xs->SetMarkerStyle(4);
          mg_xs_D2_pos->Add(g_z_pos_D2_xs,"P");
          g_z_neg_D2_xs->SetMarkerColor(warmcolor[i_color]);
          g_z_neg_D2_xs->SetMarkerStyle(4);
          mg_xs_D2_neg->Add(g_z_neg_D2_xs,"P");
        }//if H2 runs exist, then ploting
        }//loop over z
      }//if x,Q2 not 0
    }//loop over Q2
  }//loop over x
  TCanvas *c_H2_pos= new TCanvas();
  mg_xs_H2_pos->Draw("A");
  c_H2_pos->BuildLegend();
  c_H2_pos->SaveAs("results/yield/statistics_H2/H2_pos_xs.pdf");
  TCanvas *c_H2_neg= new TCanvas();
  mg_xs_H2_neg->Draw("A");
  c_H2_neg->BuildLegend();
  c_H2_neg->SaveAs("results/yield/statistics_H2/H2_neg_xs.pdf");
  TCanvas *c_D2_pos= new TCanvas();
  mg_xs_D2_pos->Draw("A");
  c_D2_pos->BuildLegend();
  c_D2_pos->SaveAs("results/yield/statistics_H2/D2_pos_xs.pdf");
  TCanvas *c_D2_neg= new TCanvas();
  mg_xs_D2_neg->Draw("A");
  c_D2_neg->BuildLegend();
  c_D2_neg->SaveAs("results/yield/statistics_H2/D2_neg_xs.pdf");

  return 0;
}
