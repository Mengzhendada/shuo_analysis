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
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  }
  int bins = j_cuts["bins"].get<int>();
  json jout;
  json jout_H2D2;
  double length_D2 = 10;//in cm ,check unit!
  double length_H2 = 10;
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
      auto mg_z_H2_datasimratio = new TMultiGraph();
      auto mg_z_D2_datasimratio = new TMultiGraph();
      auto mg_z_H2_pos_excratio = new TMultiGraph();
      auto mg_z_D2_pos_excratio = new TMultiGraph();
      auto mg_z_H2_neg_excratio = new TMultiGraph();
      auto mg_z_D2_neg_excratio = new TMultiGraph();
      auto mg_z_H2_pos_deltaratio = new TMultiGraph();
      auto mg_z_D2_pos_deltaratio = new TMultiGraph();
      auto mg_z_H2_neg_deltaratio = new TMultiGraph();
      auto mg_z_D2_neg_deltaratio = new TMultiGraph();
      auto mg_xs_H2_pos = new TMultiGraph();
      auto mg_xs_H2_neg = new TMultiGraph();
      auto mg_xs_D2_pos = new TMultiGraph();
      auto mg_xs_D2_neg = new TMultiGraph();
      bool H2_runs = false;
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH1D* h_z_neg_D2_all = new TH1D("","neg;z;",bins,0,1);
          h_z_neg_D2_all->GetXaxis()->SetTitle("z");
          //h_z_neg_D2_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_D2_all = new TH1D("","pos;z;",bins,0,1);
          h_z_pos_D2_all->GetXaxis()->SetTitle("z");
          h_z_neg_D2_all->Sumw2();
          h_z_pos_D2_all->Sumw2();
          TH1D* h_z_neg_D2_bg_all = new TH1D("","neg bg;z;",bins,0,1);
          h_z_neg_D2_bg_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_D2_bg_all = new TH1D("","pos bg;z;",bins,0,1);
          h_z_pos_D2_bg_all->GetXaxis()->SetTitle("z");
          h_z_neg_D2_bg_all->Sumw2();
          h_z_pos_D2_bg_all->Sumw2();
          TH1D* h_z_neg_Dummy_all = new TH1D("","neg;z;",bins,0,1);
          h_z_neg_Dummy_all->GetXaxis()->SetTitle("z");
          //h_z_neg_Dummy_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_Dummy_all = new TH1D("","pos;z;",bins,0,1);
          h_z_pos_Dummy_all->GetXaxis()->SetTitle("z");
          h_z_neg_Dummy_all->Sumw2();
          h_z_pos_Dummy_all->Sumw2();
          TH1D* h_z_neg_bg_Dummy_all = new TH1D("","neg bg;z;",bins,0,1);
          h_z_neg_bg_Dummy_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_bg_Dummy_all = new TH1D("","pos bg;z;",bins,0,1);
          h_z_pos_bg_Dummy_all->GetXaxis()->SetTitle("z");
          h_z_neg_bg_Dummy_all->Sumw2();
          h_z_pos_bg_Dummy_all->Sumw2();
          TH1D* h_z_neg_H2_all = new TH1D("","neg;z;",bins,0,1);
          h_z_neg_H2_all->GetXaxis()->SetTitle("z");
          //h_z_neg_H2_all->GetYaxis()->SetRangeUser(0,2);
          TH1D* h_z_pos_H2_all = new TH1D("","pos;z;",bins,0,1);
          h_z_pos_H2_all->GetXaxis()->SetTitle("z");
          h_z_neg_H2_all->Sumw2();
          h_z_pos_H2_all->Sumw2();
          TH1D* h_z_neg_bg_H2_all = new TH1D("","neg bg;z;",bins,0,1);
          h_z_neg_bg_H2_all->GetXaxis()->SetTitle("z");
          TH1D* h_z_pos_bg_H2_all = new TH1D("","pos bg;z;",bins,0,1);
          h_z_pos_bg_H2_all->GetXaxis()->SetTitle("z");
          h_z_neg_bg_H2_all->Sumw2();
          h_z_pos_bg_H2_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs,neg_H2_runs,pos_H2_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;


          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_neg_D2_sim = new TH1D("","pi- sim sum",bins,0,1);
          TH1D *h_z_neg_D2_sim_incrad = new TH1D("","pi- sim SIDIS",bins,0,1);
          h_z_neg_D2_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          TH1D *h_z_neg_D2_sim_incnorad = new TH1D("","pi- sim SIDIS",bins,0,1);
          h_z_neg_D2_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_norad");
          TH1D *h_z_neg_D2_sim_excrad = new TH1D("","pi- sim exc",bins,0,1);
          h_z_neg_D2_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          TH1D *h_z_neg_D2_sim_delta = new TH1D("","pi- sim delta",bins,0,1);
          h_z_neg_D2_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          TH2D *h_xs_z_neg_D2 = new TH2D();
          h_xs_z_neg_D2 = (TH2D*)rootfile_neg_sim->Get("xs_z_neg_inc_norad");
          //h_xs_z_neg_D2->RebinX(5);
          //h_xs_z_neg_D2->RebinY(5);
          auto xs_z_neg_D2 = h_xs_z_neg_D2->ProfileX("xs_z_neg",1,-1,"d");
          h_xs_z_neg_D2->SetTitle(z_string.c_str());
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_incrad,1);
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_excrad,1);
          h_z_neg_D2_sim->Add(h_z_neg_D2_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_pos_D2_sim = new TH1D("","pi+ sim sum",bins,0,1);
          TH1D *h_z_pos_D2_sim_incrad = new TH1D("","pi+ sim SIDIS",bins,0,1);
          h_z_pos_D2_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          TH1D *h_z_pos_D2_sim_incnorad = new TH1D("","pi+ sim SIDIS",bins,0,1);
          h_z_pos_D2_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_norad");
          TH1D *h_z_pos_D2_sim_excrad = new TH1D("","pi+ sim exc",bins,0,1);
          h_z_pos_D2_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          TH1D *h_z_pos_D2_sim_delta = new TH1D("","pi+ sim delta",bins,0,1);
          h_z_pos_D2_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          TH2D *h_xs_z_pos_D2 = new TH2D();
          h_xs_z_pos_D2 = (TH2D*)rootfile_pos_sim->Get("xs_z_pos_inc_norad");
          h_xs_z_pos_D2->SetTitle(z_string.c_str());
          //h_xs_z_pos_D2->RebinX(5);
          //h_xs_z_pos_D2->RebinY(5);
          auto xs_z_pos_D2 = h_xs_z_pos_D2->ProfileX("xs_z_pos_D2",1,-1,"d");
          h_z_pos_D2_sim->Add(h_z_pos_D2_sim_incrad,1);
          h_z_pos_D2_sim->Add(h_z_pos_D2_sim_excrad,1);
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
              TH1D *h_z_neg_H2_sim = new TH1D("","pi- sim sum",bins,0,1);
              TH1D *h_z_neg_H2_sim_incrad = new TH1D("","pi- sim SIDIS",bins,0,1);
              h_z_neg_H2_sim_incrad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_inc_rad");
              TH1D *h_z_neg_H2_sim_incnorad = new TH1D("","pi- sim SIDIS",bins,0,1);
              h_z_neg_H2_sim_incnorad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_inc_norad");
              //TH1D *h_z_neg_H2_sim_excrad = new TH1D("","pi- sim exc",bins,0,1);
              //h_z_neg_H2_sim_excrad = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_exc_rad");
              TH1D *h_z_neg_H2_sim_delta = new TH1D("","pi- sim delta",bins,0,1);
              h_z_neg_H2_sim_delta = (TH1D*)rootfile_neg_H2_sim->Get("z_neg_delta");
              TH2D *h_xs_z_neg_H2 = new TH2D();
              h_xs_z_neg_H2 = (TH2D*)rootfile_neg_H2_sim->Get("xs_z_neg_inc_norad");
              h_xs_z_neg_H2->SetTitle(z_string.c_str());
              //h_xs_z_neg_H2->RebinX(5);
              //h_xs_z_neg_H2->RebinY(5);
              auto xs_z_neg_H2 = h_xs_z_neg_H2->ProfileX("xs_z_neg_H2",1,-1,"d");
              h_z_neg_H2_sim->Add(h_z_neg_H2_sim_incrad,1);
              //h_z_neg_H2_sim->Add(h_z_neg_H2_sim_excrad,1);
              h_z_neg_H2_sim->Add(h_z_neg_H2_sim_delta,1);
              TFile *rootfile_pos_H2_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_H2_simc.root").c_str());
              TH1D *h_z_pos_H2_sim = new TH1D("","pi+ sim sum",bins,0,1);
              TH1D *h_z_pos_H2_sim_incrad = new TH1D("","pi+ sim SIDIS",bins,0,1);
              h_z_pos_H2_sim_incrad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_inc_rad");
              TH1D *h_z_pos_H2_sim_incnorad = new TH1D("","pi+ sim SIDIS",bins,0,1);
              h_z_pos_H2_sim_incnorad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_inc_norad");
              TH1D *h_z_pos_H2_sim_excrad = new TH1D("","pi+ sim exc",bins,0,1);
              h_z_pos_H2_sim_excrad = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_exc_rad");
              TH1D *h_z_pos_H2_sim_delta = new TH1D("","pi+ sim delta",bins,0,1);
              h_z_pos_H2_sim_delta = (TH1D*)rootfile_pos_H2_sim->Get("z_pos_delta");
              TH2D *h_xs_z_pos_H2 = new TH2D();
              h_xs_z_pos_H2 = (TH2D*)rootfile_pos_H2_sim->Get("xs_z_pos_inc_norad");
              h_xs_z_pos_H2->SetTitle(z_string.c_str());
              //h_xs_z_pos_H2->RebinX(5);
              //h_xs_z_pos_H2->RebinY(5);
              auto xs_z_pos_H2 = h_xs_z_pos_H2->ProfileX("xs_z_pos_H2",1,-1,"d");
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_incrad,1);
              h_z_pos_H2_sim->Add(h_z_pos_H2_sim_excrad,1);
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
                TH1D *h_z_neg = new TH1D("","",bins,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",bins,0,1);
                h_z_neg_bg = (TH1D*)root_file_neg->Get("z_bg");
                double EFF = Get_all_eff(RunNumber);
                h_z_neg_D2_bg_all->Add(h_z_neg_bg,1/(6*EFF));
                h_z_neg_D2_all->Add(h_z_neg,1/EFF);
              }//loop over neg runs
              for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
                int RunNumber = *it;
                //std::cout<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                charge_pos_all+=charge;
                //std::cout<<"pos DE check"<<std::endl;
                TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_pos = new TH1D("","",bins,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",bins,0,1);
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
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",bins,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",bins,0,1);
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
                TH1D *h_z_pos = new TH1D("","",bins,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",bins,0,1);
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
                TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
                TH1D *h_z_neg = new TH1D("","",bins,0,1);
                h_z_neg = (TH1D*)root_file_neg->Get("z");
                TH1D *h_z_neg_bg = new TH1D("","",bins,0,1);
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
                TH1D *h_z_pos = new TH1D("","",bins,0,1);
                h_z_pos = (TH1D*)root_file_pos->Get("z");
                TH1D *h_z_pos_bg = new TH1D("","",bins,0,1);
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


              //h_z_neg_D2_all->Rebin(5);
              //h_z_pos_D2_all->Rebin(5);
              //h_z_neg_H2_all->Rebin(5);
              //h_z_pos_H2_all->Rebin(5);
              //h_z_neg_Dummy_all->Rebin(5);
              //h_z_pos_Dummy_all->Rebin(5);
              //h_z_neg_D2_incnorad->Rebin(5);
              //h_z_neg_D2_incrad->Rebin(5);
              //h_z_neg_D2_sim_excrad->Rebin(5);
              //h_z_neg_D2_sim_delta->Rebin(5);

              //h_z_pos_D2_incnorad->Rebin(5);
              //h_z_pos_D2_incrad->Rebin(5);
              //h_z_pos_D2_sim_excrad->Rebin(5);
              //h_z_pos_D2_sim_delta->Rebin(5);
              ///auto rp_radia_corr_neg = new TRatioPlot(h_z_neg_D2_sim_incnorad,h_z_neg_D2_sim_incrad);
              ///auto rp_radia_corr_pos = new TRatioPlot(h_z_pos_D2_sim_incnorad,h_z_pos_D2_sim_incrad);
              TH1* rp_radia_corr_neg = (TH1*)h_z_neg_D2_incnorad->Clone("rp_radia_corr");
              TH1* rp_radia_corr_pos = (TH1*)h_z_pos_D2_incnorad->Clone("rp_radia_corr");
              rp_radia_corr_neg ->Divide(h_z_neg_D2_incnorad,h_z_neg_D2_incrad);          
              rp_radia_corr_pos ->Divide(h_z_pos_D2_incnorad,h_z_pos_D2_incrad);          
              //TCanvas* c_radia_corr_sim = new TCanvas();
              //c_radia_corr_sim->Divide(2,1);
              //c_radia_corr_sim->cd(1);
              //rp_radia_corr_neg->Draw();
              //c_radia_corr_sim->cd(2);
              //rp_radia_corr_pos->Draw();
              //std::string c_radia_corr_sim_name = "results/yield/statistics_corr/radia_corr_"+q2xz_str_filename+".pdf";
              //c_radia_corr_sim->SaveAs(c_radia_corr_sim_name.c_str());

              //h_z_neg_H2_sim_delta->Rebin(5);
              //h_z_neg_H2_sim_incnorad->Rebin(5);
              //h_z_neg_H2_sim_incrad->Rebin(5);
              //h_z_pos_H2_sim_excrad->Rebin(5);
              //h_z_pos_H2_sim_delta->Rebin(5);
              //h_z_pos_H2_sim_incnorad->Rebin(5);
              //h_z_pos_H2_sim_incrad->Rebin(5);


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

              TH1* rp_radia_corr_neg_H2 = (TH1*)h_z_neg_H2_incnorad->Clone("rp_radia_corr");
              TH1* rp_radia_corr_pos_H2 = (TH1*)h_z_pos_H2_incnorad->Clone("rp_radia_corr");
              rp_radia_corr_neg_H2 ->Divide(h_z_neg_H2_incnorad,h_z_neg_H2_incrad);          
              rp_radia_corr_pos_H2 ->Divide(h_z_pos_H2_incnorad,h_z_pos_H2_incrad);          


              int nbins = h_z_pos_H2_all->GetXaxis()->GetNbins();

              TGraphErrors* g_diff_ratio = new TGraphErrors();
              g_diff_ratio->SetName(z_string.c_str());
              TGraphErrors* g_diff_simratio = new TGraphErrors();

              int ii = 0;
              TGraphErrors* g_sum_ratio = new TGraphErrors();
              g_sum_ratio->SetName(z_string.c_str());
              TGraphErrors* g_sum_simratio = new TGraphErrors();

              int ii_sum = 0;
              TGraphErrors* g_z_H2D2_pos_ratio = new TGraphErrors();
              TGraphErrors* g_z_H2D2_neg_ratio = new TGraphErrors();
              TGraphErrors* g_z_H2D2_pos_simratio = new TGraphErrors();
              TGraphErrors* g_z_H2D2_neg_simratio = new TGraphErrors();
              TGraphErrors* g_z_pos_H2_datasimratio = new TGraphErrors();
              TGraphErrors* g_z_neg_H2_datasimratio = new TGraphErrors();
              TGraphErrors* g_z_pos_D2_datasimratio = new TGraphErrors();
              TGraphErrors* g_z_neg_D2_datasimratio = new TGraphErrors();
              TGraphErrors* g_z_pos_H2_excratio = new TGraphErrors();
              TGraphErrors* g_z_pos_H2_deltaratio = new TGraphErrors();
              TGraphErrors* g_z_neg_H2_excratio = new TGraphErrors();
              TGraphErrors* g_z_neg_H2_deltaratio = new TGraphErrors();
              TGraphErrors* g_z_pos_D2_excratio = new TGraphErrors();
              TGraphErrors* g_z_pos_D2_deltaratio = new TGraphErrors();
              TGraphErrors* g_z_neg_D2_excratio = new TGraphErrors();
              TGraphErrors* g_z_neg_D2_deltaratio = new TGraphErrors();
              TGraphErrors* g_z_pos_H2_datasim = new TGraphErrors();
              TGraphErrors* g_z_neg_H2_datasim = new TGraphErrors();
              TGraphErrors* g_z_pos_D2_datasim = new TGraphErrors();
              TGraphErrors* g_z_neg_D2_datasim = new TGraphErrors();
              int ii_posH2D2ratio = 0,ii_negH2D2ratio = 0,i_H2_pos_datasim = 0,i_H2_neg_datasim = 0,i_D2_pos_datasim = 0,i_D2_neg_datasim = 0,i_D2_pos_exc=0,i_D2_pos_delta=0,i_D2_neg_exc=0,i_D2_neg_delta=0,i_H2_pos_exc=0,i_H2_pos_delta=0,i_H2_neg_exc=0,i_H2_neg_delta=0;
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

                double radia_corr_neg_D2 = rp_radia_corr_neg->GetBinContent(i);
                double radia_corr_pos_D2 = rp_radia_corr_pos->GetBinContent(i);
                double radia_corr_neg_H2 = rp_radia_corr_neg_H2->GetBinContent(i);
                double radia_corr_pos_H2 = rp_radia_corr_pos_H2->GetBinContent(i);

                double H2_neg_yield_incrad = h_z_neg_H2_sim_incrad->GetBinContent(i);
                double H2_pos_yield_incrad = h_z_pos_H2_sim_incrad->GetBinContent(i);
                double D2_neg_yield_incrad = h_z_neg_D2_sim_incrad->GetBinContent(i);
                double D2_pos_yield_incrad = h_z_pos_D2_sim_incrad->GetBinContent(i);

                // bgs added in simc
                double y_H2_pos_data = y_H2_pos_raw-0.262*y_Dummy_pos;
                double H2_pos_yield_incrad_simdeltaexc = H2_pos_yield_incrad+y_H2_pos_delta+y_H2_pos_exc;
                double y_H2_neg_data = y_H2_neg_raw-0.262*y_Dummy_neg;
                double H2_neg_yield_incrad_simdeltaexc = H2_neg_yield_incrad+y_H2_neg_delta;
                double y_D2_pos_data = y_D2_pos_raw-0.245*y_Dummy_pos;
                double D2_pos_yield_incrad_simdeltaexc = D2_pos_yield_incrad+y_D2_pos_delta+y_D2_pos_exc;
                double y_D2_neg_data = y_D2_neg_raw-0.245*y_Dummy_neg;
                double D2_neg_yield_incrad_simdeltaexc = D2_neg_yield_incrad+y_D2_neg_delta+y_D2_neg_exc;
                // bgs subtracted in data
                double y_H2_pos_datadeltaexc = y_H2_pos_raw-0.262*y_Dummy_pos-y_H2_pos_delta-y_H2_pos_exc;
                double H2_pos_yield_incrad_sim = H2_pos_yield_incrad;
                double y_H2_neg_datadeltaexc = y_H2_neg_raw-0.262*y_Dummy_neg-y_H2_neg_delta;
                double H2_neg_yield_incrad_sim = H2_neg_yield_incrad;
                double y_D2_pos_datadeltaexc = y_D2_pos_raw-0.245*y_Dummy_pos-y_D2_pos_delta-y_D2_pos_exc;
                double D2_pos_yield_incrad_sim = D2_pos_yield_incrad;
                double y_D2_neg_datadeltaexc = y_D2_neg_raw-0.245*y_Dummy_neg-y_D2_neg_delta-y_D2_neg_exc;
                double D2_neg_yield_incrad_sim = D2_neg_yield_incrad;
                // bg subtracted in data
                double y_H2_pos_datadelta = y_H2_pos_raw-0.262*y_Dummy_pos-y_H2_pos_delta;
                //H2_pos_yield_incrad = H2_pos_yield_incrad;
                double y_H2_neg_datadelta = y_H2_neg_raw-0.262*y_Dummy_neg-y_H2_neg_delta;
                //H2_neg_yield_incrad = H2_neg_yield_incrad;
                double y_D2_pos_datadelta = y_D2_pos_raw-0.245*y_Dummy_pos-y_D2_pos_delta;
                //D2_pos_yield_incrad = D2_pos_yield_incrad;
                double y_D2_neg_datadelta = y_D2_neg_raw-0.245*y_Dummy_neg-y_D2_neg_delta;
                //D2_neg_yield_incrad = D2_neg_yield_incrad;
                //exc subtracted in data
                double y_H2_pos_dataexc = y_H2_pos_raw-0.262*y_Dummy_pos-y_H2_pos_exc;
                //H2_pos_yield_incrad = H2_pos_yield_incrad;
                double y_H2_neg_dataexc = y_H2_neg_raw-0.262*y_Dummy_neg;
                //H2_neg_yield_incrad = H2_neg_yield_incrad;
                double y_D2_pos_dataexc = y_D2_pos_raw-0.245*y_Dummy_pos-y_D2_pos_exc;
                //D2_pos_yield_incrad = D2_pos_yield_incrad;
                double y_D2_neg_dataexc = y_D2_neg_raw-0.245*y_Dummy_neg-y_D2_neg_exc;
                //D2_neg_yield_incrad = D2_neg_yield_incrad;

                //what actually used
                double y_H2_pos = y_H2_pos_datadeltaexc;
                double y_H2_neg = y_H2_neg_datadeltaexc;
                double y_D2_pos = y_D2_pos_datadeltaexc;
                double y_D2_neg = y_D2_neg_datadeltaexc;
                double y_H2_pos_sim = H2_pos_yield_incrad_sim;
                double y_H2_neg_sim = H2_neg_yield_incrad_sim;
                double y_D2_pos_sim = D2_pos_yield_incrad_sim;
                double y_D2_neg_sim = D2_neg_yield_incrad_sim;


                error_H2_pos = std::sqrt(error_H2_pos*error_H2_pos+0.262*0.262*error_Dummy_pos*error_Dummy_pos);
                error_H2_neg = std::sqrt(error_H2_neg*error_H2_neg+0.262*0.262*error_Dummy_neg*error_Dummy_neg);
                error_D2_pos = std::sqrt(error_D2_pos*error_D2_pos+0.245*0.245*error_Dummy_pos*error_Dummy_pos);
                error_D2_neg = std::sqrt(error_D2_neg*error_D2_neg+0.245*0.245*error_Dummy_neg*error_Dummy_neg);

                y_H2_pos = y_H2_pos*radia_corr_pos_H2;
                y_H2_neg = y_H2_neg*radia_corr_neg_H2;
                y_D2_pos = y_D2_pos*radia_corr_pos_D2;
                y_D2_neg = y_D2_neg*radia_corr_neg_D2;
                error_H2_pos = error_H2_pos*radia_corr_pos_H2;
                error_H2_neg = error_H2_neg*radia_corr_neg_H2;
                error_D2_pos = error_D2_pos*radia_corr_pos_D2;
                error_D2_neg = error_D2_neg*radia_corr_neg_D2;

                double H2_xs_z_neg,H2_xs_z_pos,D2_xs_z_neg,D2_xs_z_pos,D2_xs_z_neg_2;
                //g_xs_z_neg_H2_sim->GetPoint(i,x_center,H2_xs_z_neg);
                //g_xs_z_pos_H2_sim->GetPoint(i,x_center,H2_xs_z_pos);
                D2_xs_z_neg = xs_z_neg_D2->GetBinContent(i);
                //D2_xs_z_neg_2 = xs_z_neg_D2->GetBinContent(xs_z_neg_D2->GetXaxis()->FindBin(x_center));
                D2_xs_z_pos = xs_z_pos_D2->GetBinContent(xs_z_pos_D2->GetXaxis()->FindBin(x_center));
                H2_xs_z_neg = xs_z_neg_H2->GetBinContent(xs_z_neg_H2->GetXaxis()->FindBin(x_center));
                H2_xs_z_pos = xs_z_pos_H2->GetBinContent(xs_z_pos_H2->GetXaxis()->FindBin(x_center));

                std::cout<<"H2 xs pos "<<H2_xs_z_pos<<"H2 xs neg "<<H2_xs_z_neg<<" D2 xs pos "<<D2_xs_z_pos<<" D2 xs neg "<<D2_xs_z_neg<<std::endl;

                //not xs
                double yield_H2_pos = y_H2_pos;//*radia_corr_pos_H2;
                double yield_H2_neg = y_H2_neg;//*radia_corr_neg_H2;
                double yield_D2_pos = y_D2_pos;//*radia_corr_pos_D2;
                double yield_D2_neg = y_D2_neg;//*radia_corr_neg_D2;
                double error_yield_H2_pos = error_H2_pos;//*radia_corr_pos_H2;
                double error_yield_H2_neg = error_H2_neg;//*radia_corr_pos_H2;
                double error_yield_D2_pos = error_D2_pos;//*radia_corr_pos_H2;
                double error_yield_D2_neg = error_D2_neg;//*radia_corr_pos_H2;

                //y_H2_pos = y_H2_pos*radia_corr_pos_H2;
                //y_H2_neg = y_H2_neg*radia_corr_neg_H2;
                //y_D2_pos = y_D2_pos*radia_corr_pos_D2;
                //y_D2_neg = y_D2_neg*radia_corr_neg_D2;
                //error_H2_pos = error_H2_pos*radia_corr_pos_H2;
                //error_H2_neg = error_H2_neg*radia_corr_neg_H2;
                //error_D2_pos = error_D2_pos*radia_corr_pos_D2;
                //error_D2_neg = error_D2_neg*radia_corr_neg_D2;

                ////xs for the yield, no radia corr required. 
                //y_H2_pos = y_H2_pos*H2_xs_z_pos/(y_H2_pos_sim);
                //y_H2_neg = y_H2_neg*H2_xs_z_neg/(y_H2_neg_sim);
                //y_D2_pos = y_D2_pos*D2_xs_z_pos/(y_D2_pos_sim);
                //y_D2_neg = y_D2_neg*D2_xs_z_neg/(y_D2_neg_sim);
                //error_H2_pos = error_H2_pos*H2_xs_z_pos/(y_H2_pos_sim);
                //error_H2_neg = error_H2_neg*H2_xs_z_neg/(y_H2_neg_sim);
                //error_D2_pos = error_D2_pos*D2_xs_z_pos/(y_D2_pos_sim);
                //error_D2_neg = error_D2_neg*D2_xs_z_neg/(y_D2_neg_sim);

                //for ratio
                double y_H2_diff_datadeltaexc = y_H2_pos_datadeltaexc-y_H2_neg_datadeltaexc;
                double y_D2_diff_datadeltaexc = y_D2_pos_datadeltaexc-y_D2_neg_datadeltaexc;
                double y_diff_datadeltaexc = y_H2_diff_datadeltaexc/y_D2_diff_datadeltaexc;
                
                double y_H2_diff_datadelta = y_H2_pos_datadelta-y_H2_neg_datadelta;
                double y_D2_diff_datadelta = y_D2_pos_datadelta-y_D2_neg_datadelta;
                double y_diff_datadelta = y_H2_diff_datadelta/y_D2_diff_datadelta;
                
                double y_H2_diff_dataexc = y_H2_pos_dataexc-y_H2_neg_dataexc;
                double y_D2_diff_dataexc = y_D2_pos_dataexc-y_D2_neg_dataexc;
                double y_diff_dataexc = y_H2_diff_dataexc/y_D2_diff_dataexc;

                double y_H2_diff_data = y_H2_pos_data-y_H2_neg_data;
                double y_D2_diff_data = y_D2_pos_data-y_D2_neg_data;
                double y_diff_data = y_H2_diff_data/y_D2_diff_data;
                //for ratio
                double y_H2_sum_datadeltaexc = y_H2_pos_datadeltaexc-y_H2_neg_datadeltaexc;
                double y_D2_sum_datadeltaexc = y_D2_pos_datadeltaexc-y_D2_neg_datadeltaexc;
                double y_sum_datadeltaexc = y_H2_sum_datadeltaexc/y_D2_sum_datadeltaexc;
                
                double y_H2_sum_datadelta = y_H2_pos_datadelta-y_H2_neg_datadelta;
                double y_D2_sum_datadelta = y_D2_pos_datadelta-y_D2_neg_datadelta;
                double y_sum_datadelta = y_H2_sum_datadelta/y_D2_sum_datadelta;
                
                double y_H2_sum_dataexc = y_H2_pos_dataexc-y_H2_neg_dataexc;
                double y_D2_sum_dataexc = y_D2_pos_dataexc-y_D2_neg_dataexc;
                double y_sum_dataexc = y_H2_sum_dataexc/y_D2_sum_dataexc;

                double y_H2_sum_data = y_H2_pos_data-y_H2_neg_data;
                double y_D2_sum_data = y_D2_pos_data-y_D2_neg_data;
                double y_sum_data = y_H2_sum_data/y_D2_sum_data;

                //for diff sum
                double y_H2_diff = y_H2_pos-y_H2_neg;
                double y_D2_diff = y_D2_pos-y_D2_neg;
                double y_diff = y_H2_diff/y_D2_diff;
                double y_simdiff = (H2_xs_z_pos-H2_xs_z_neg)/(D2_xs_z_pos-D2_xs_z_neg);
                double error_H2_diff = std::sqrt((error_H2_pos*error_H2_pos)+(error_H2_neg*error_H2_neg));
                double error_D2_diff = std::sqrt((error_D2_pos*error_D2_pos)+(error_D2_neg*error_D2_neg));
                double error_diff = y_diff*std::sqrt((error_H2_diff*error_H2_diff)/(y_H2_diff*y_H2_diff)+(error_D2_diff*error_D2_diff)/(y_D2_diff*y_D2_diff));
                if(y_diff>0 && error_diff<20){
                  //if(true){
                  g_diff_ratio->SetPoint(ii,x,y_diff);
                  g_diff_ratio->SetPointError(ii,0,error_diff);
                  g_diff_simratio->SetPoint(ii,x,y_simdiff);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_diff"].push_back(y_diff);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["error_diff"].push_back(error_diff);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_diff_datadeltaexc"].push_back(y_diff_datadeltaexc);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_diff_datadelta"].push_back(y_diff_datadelta);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_diff_dataexc"].push_back(y_diff_dataexc);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_diff_data"].push_back(y_diff_data);
                  
                  ii++;
                }
                double y_H2_sum = y_H2_pos+y_H2_neg;
                double y_D2_sum = y_D2_pos+y_D2_neg;
                double y_sum = y_H2_sum/y_D2_sum;
                double y_simsum = (H2_xs_z_pos+H2_xs_z_neg)/(D2_xs_z_pos+D2_xs_z_neg);
                double error_H2_sum = std::sqrt((error_H2_pos*error_H2_pos)+(error_H2_neg*error_H2_neg));
                double error_D2_sum = std::sqrt((error_D2_pos*error_D2_pos)+(error_D2_neg*error_D2_neg));
                double error_sum = y_sum*std::sqrt(error_H2_sum*error_H2_sum/(y_H2_sum*y_H2_sum)+error_D2_sum*error_D2_sum/(y_D2_sum*y_D2_sum));
                if(y_sum>0 && error_sum< 0.5){
                  g_sum_ratio->SetPoint(ii_sum,x,y_sum);
                  g_sum_ratio->SetPointError(ii_sum,0,error_sum);
                  g_sum_simratio->SetPoint(ii_sum,x,y_simsum);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_sum"].push_back(y_sum);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["error_sum"].push_back(error_sum);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_sum_datadeltaexc"].push_back(y_sum_datadeltaexc);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_sum_datadelta"].push_back(y_sum_datadelta);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_sum_dataexc"].push_back(y_sum_dataexc);
                  jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["value_sum_data"].push_back(y_sum_data);
                  ii_sum++;
                }


                //for pos neg h2/d2 ratio
                double y_pos = y_H2_pos/y_D2_pos;
                double y_neg = y_H2_neg/y_D2_neg;
                double y_pos_sim = y_H2_pos_sim/y_D2_pos_sim; 
                  //H2_xs_z_pos/D2_xs_z_pos;
                double y_neg_sim = y_H2_neg_sim/y_D2_neg_sim;
                  //H2_xs_z_neg/D2_xs_z_neg;
                double error_pos = y_pos*std::sqrt((error_H2_pos*error_H2_pos)/(y_H2_pos*y_H2_pos)+(error_D2_pos*error_D2_pos)/(y_D2_pos*y_D2_pos));
                double error_neg = y_neg*std::sqrt((error_H2_neg*error_H2_neg)/(y_H2_neg*y_H2_neg)+(error_D2_neg*error_D2_neg)/(y_D2_neg*y_D2_neg));

                if(y_pos>0 && error_pos< 0.2){
                  g_z_H2D2_pos_ratio->SetPoint(ii_posH2D2ratio,x,y_pos);
                  g_z_H2D2_pos_ratio->SetPointError(ii_posH2D2ratio,0,error_pos);
                  g_z_H2D2_pos_simratio->SetPoint(ii_posH2D2ratio,x,y_pos_sim);
                  ii_posH2D2ratio++; 
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_pos"].push_back(y_pos);
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_pos_sim"].push_back(y_pos_sim);
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_pos_err"].push_back(error_pos);
                }
                if(y_neg>0 && error_neg< 0.2){
                  g_z_H2D2_neg_ratio->SetPoint(ii_negH2D2ratio,x,y_neg);
                  g_z_H2D2_neg_ratio->SetPointError(ii_negH2D2ratio,0,error_neg);
                  g_z_H2D2_neg_simratio->SetPoint(ii_negH2D2ratio,x,y_neg_sim);
                  ii_negH2D2ratio++; 
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_neg"].push_back(y_neg);
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_neg_sim"].push_back(y_neg_sim);
                  jout_H2D2[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(x)).c_str()]["H2D2_neg_err"].push_back(error_neg);
                }

                double y_pos_H2_datasimratio = yield_H2_pos/H2_pos_yield_incrad;
                double y_neg_H2_datasimratio = yield_H2_neg/H2_neg_yield_incrad;
                double y_pos_D2_datasimratio = yield_D2_pos/D2_pos_yield_incrad;
                double y_neg_D2_datasimratio = yield_D2_neg/D2_neg_yield_incrad;
                double error_pos_H2_datasimratio = error_yield_H2_pos/H2_pos_yield_incrad;
                double error_neg_H2_datasimratio = error_yield_H2_neg/H2_neg_yield_incrad;
                double error_pos_D2_datasimratio = error_yield_D2_pos/D2_pos_yield_incrad;
                double error_neg_D2_datasimratio = error_yield_D2_neg/D2_neg_yield_incrad;

                double y_pos_H2_excratio = y_H2_pos_exc/H2_pos_yield_incrad;
                double y_pos_H2_deltaratio = y_H2_pos_delta/H2_pos_yield_incrad;
                //double y_neg_H2_excratio = y_H2_neg_exc/H2_neg_yield_incrad;
                double y_neg_H2_deltaratio = y_H2_neg_delta/H2_neg_yield_incrad;
                double y_pos_D2_excratio = y_D2_pos_exc/D2_pos_yield_incrad;
                double y_pos_D2_deltaratio = y_D2_pos_delta/D2_pos_yield_incrad;
                double y_neg_D2_excratio = y_D2_neg_exc/D2_neg_yield_incrad;
                double y_neg_D2_deltaratio = y_D2_neg_delta/D2_neg_yield_incrad;
                if(y_pos_H2_excratio>0){
                  g_z_pos_H2_excratio->SetPoint(i_H2_pos_exc,x,y_pos_H2_excratio);
                  i_H2_pos_exc++;
                }
                if(y_pos_H2_deltaratio>0){
                  g_z_pos_H2_deltaratio->SetPoint(i_H2_pos_delta,x,y_pos_H2_deltaratio);
                  i_H2_pos_delta++;
                }
                //if(y_neg_h2_excratio>0){
                //  g_z_neg_H2_excratio->SetPoint(i_H2_neg_exc,x,y_neg_h2_excratio);
                //  i_H2_neg_exc++;
                //}
                if(y_neg_H2_deltaratio>0){
                  g_z_neg_H2_deltaratio->SetPoint(i_H2_neg_delta,x,y_neg_H2_deltaratio);
                  i_H2_neg_delta++;
                }
                if(y_pos_D2_excratio>0){
                  g_z_pos_D2_excratio->SetPoint(i_D2_pos_exc,x,y_pos_D2_excratio);
                  i_D2_pos_exc++;
                }
                if(y_pos_D2_deltaratio>0){
                  g_z_pos_D2_deltaratio->SetPoint(i_D2_pos_delta,x,y_pos_D2_deltaratio);
                  i_D2_pos_delta++;
                }
                if(y_neg_D2_excratio>0){
                  g_z_neg_D2_excratio->SetPoint(i_D2_neg_exc,x,y_neg_D2_excratio);
                  i_D2_neg_exc++;
                }
                if(y_neg_D2_deltaratio>0){
                  g_z_neg_D2_deltaratio->SetPoint(i_D2_neg_delta,x,y_neg_D2_deltaratio);
                  i_D2_neg_delta++;
                }

                if(y_pos_H2_datasimratio>0 && error_pos_H2_datasimratio < 10){
                  g_z_pos_H2_datasimratio->SetPoint(i_H2_pos_datasim,x,y_pos_H2_datasimratio);
                  g_z_pos_H2_datasimratio->SetPointError(i_H2_pos_datasim,0,error_pos_H2_datasimratio);
                  //g_z_pos_H2_datasim->SetPoint(i_H2_pos_datasim,x,H2_pos_yield_incrad);
                  i_H2_pos_datasim++;
                }
                if(y_neg_H2_datasimratio>0 && error_neg_H2_datasimratio<10){
                  g_z_neg_H2_datasimratio->SetPoint(i_H2_neg_datasim,x,y_neg_H2_datasimratio);
                  g_z_neg_H2_datasimratio->SetPointError(i_H2_neg_datasim,0,error_neg_H2_datasimratio);
                  //g_z_neg_H2_datasim->SetPoint(i_H2_neg_datasim,x,H2_neg_yield_incrad);
                  i_H2_neg_datasim++;
                }
                if(y_pos_D2_datasimratio>0 && error_pos_D2_datasimratio<10){
                  g_z_pos_D2_datasimratio->SetPoint(i_D2_pos_datasim,x,y_pos_D2_datasimratio);
                  g_z_pos_D2_datasimratio->SetPointError(i_D2_pos_datasim,0,error_pos_D2_datasimratio);
                  // g_z_pos_D2_datasim->SetPoint(i_D2_pos_datasim,x,D2_pos_yield_incrad);
                  i_D2_pos_datasim++;
                }
                if(y_neg_D2_datasimratio>0 && error_neg_D2_datasimratio<10){
                  g_z_neg_D2_datasimratio->SetPoint(i_D2_neg_datasim,x,y_neg_D2_datasimratio);
                  g_z_neg_D2_datasimratio->SetPointError(i_D2_neg_datasim,0,error_neg_D2_datasimratio);
                  // g_z_neg_D2_datasim->SetPoint(i_D2_neg_datasim,x,D2_neg_yield_incrad);
                  i_D2_neg_datasim++;
                }
                }

                g_z_H2D2_pos_ratio->SetTitle(("pos z "+std::to_string(z)).c_str()); 
                g_z_H2D2_neg_ratio->SetTitle(("neg z "+std::to_string(z)).c_str());
                g_z_H2D2_pos_simratio->SetTitle(("pos sim "+std::to_string(z)).c_str()); 
                g_z_H2D2_neg_simratio->SetTitle(("neg sim "+std::to_string(z)).c_str());

                g_z_H2D2_pos_ratio->SetMarkerColor(i_color);
                g_z_H2D2_neg_ratio->SetMarkerColor(i_color);
                g_z_H2D2_pos_simratio->SetLineColor(i_color);
                g_z_H2D2_neg_simratio->SetLineColor(i_color);
                g_z_H2D2_pos_ratio->SetMarkerStyle(4);
                g_z_H2D2_neg_ratio->SetMarkerStyle(4);
                mg_z_H2D2_pos_ratio->Add(g_z_H2D2_pos_ratio,"P");
                mg_z_H2D2_neg_ratio->Add(g_z_H2D2_neg_ratio,"P");
                //mg_z_H2D2_pos_ratio->Add(g_z_H2D2_pos_simratio,"L");
                //mg_z_H2D2_neg_ratio->Add(g_z_H2D2_neg_simratio,"L");

                g_z_pos_H2_datasimratio->SetMarkerColor(i_color);
                g_z_pos_H2_datasimratio->SetMarkerStyle(4);
                g_z_pos_H2_datasimratio->SetTitle(("H2 pos "+std::to_string(z)).c_str());
                g_z_pos_H2_datasim->SetTitle(("H2 pos sim"+std::to_string(z)).c_str());
                g_z_pos_H2_datasim->SetLineColor(i_color);
                g_z_pos_H2_datasim->SetLineStyle(1);
                mg_z_H2_datasimratio->Add(g_z_pos_H2_datasimratio,"P");
                //mg_z_H2_datasimratio->Add(g_z_pos_H2_datasim,"L");
                g_z_neg_H2_datasimratio->SetMarkerColor(i_color);
                g_z_neg_H2_datasimratio->SetMarkerStyle(8);
                g_z_neg_H2_datasimratio->SetTitle(("H2 neg "+std::to_string(z)).c_str());
                g_z_neg_H2_datasim->SetTitle(("H2 neg sim"+std::to_string(z)).c_str());
                g_z_neg_H2_datasim->SetLineColor(i_color);
                g_z_neg_H2_datasim->SetLineStyle(2);
                mg_z_H2_datasimratio->Add(g_z_neg_H2_datasimratio,"P");
                //mg_z_H2_datasimratio->Add(g_z_neg_H2_datasim,"L");
                g_z_pos_D2_datasimratio->SetMarkerColor(i_color);
                g_z_pos_D2_datasimratio->SetMarkerStyle(5);
                g_z_pos_D2_datasimratio->SetTitle(("D2 pos "+std::to_string(z)).c_str());
                g_z_pos_D2_datasim->SetTitle(("D2 pos sim"+std::to_string(z)).c_str());
                g_z_pos_D2_datasim->SetLineColor(i_color);
                g_z_pos_D2_datasim->SetLineStyle(3);
                mg_z_D2_datasimratio->Add(g_z_pos_D2_datasimratio,"P");
                //mg_z_D2_datasimratio->Add(g_z_pos_D2_datasim,"L");
                g_z_neg_D2_datasimratio->SetMarkerColor(i_color);
                g_z_neg_D2_datasimratio->SetMarkerStyle(47);
                g_z_neg_D2_datasimratio->SetTitle(("D2 neg "+std::to_string(z)).c_str());
                g_z_neg_D2_datasim->SetTitle(("D2 neg sim"+std::to_string(z)).c_str());
                g_z_neg_D2_datasim->SetLineColor(i_color);
                g_z_neg_D2_datasim->SetLineStyle(4);
                mg_z_D2_datasimratio->Add(g_z_neg_D2_datasimratio,"P");
                //mg_z_D2_datasimratio->Add(g_z_neg_D2_datasim,"L");

                g_z_pos_H2_excratio->SetLineColor(i_color);
                g_z_pos_H2_excratio->SetTitle(("H2 pos exc "+std::to_string(z).substr(0,4)).c_str());
                mg_z_H2_pos_excratio->Add(g_z_pos_H2_excratio,"L");
                //g_z_neg_H2_excratio->SetLineColor(i_color);
                //g_z_neg_H2_excratio->SetTitle(("H2 neg exc "+std::to_string(z)).c_str());
                //mg_z_H2_neg_excratio->Add(g_z_neg_h2_excratio,"L");
                g_z_pos_D2_excratio->SetLineColor(i_color);
                g_z_pos_D2_excratio->SetTitle(("D2 pos exc "+std::to_string(z)).c_str());
                mg_z_D2_pos_excratio->Add(g_z_pos_D2_excratio,"L");
                g_z_neg_D2_excratio->SetLineColor(i_color);
                g_z_neg_D2_excratio->SetTitle(("D2 neg exc "+std::to_string(z)).c_str());
                mg_z_D2_neg_excratio->Add(g_z_neg_D2_excratio,"L");
                g_z_pos_H2_deltaratio->SetLineColor(i_color);
                g_z_pos_H2_deltaratio->SetTitle(("H2 pos delta "+std::to_string(z)).c_str());
                mg_z_H2_pos_deltaratio->Add(g_z_pos_H2_deltaratio,"L");
                g_z_neg_H2_deltaratio->SetLineColor(i_color);
                g_z_neg_H2_deltaratio->SetTitle(("H2 neg delta "+std::to_string(z)).c_str());
                mg_z_H2_neg_deltaratio->Add(g_z_neg_H2_deltaratio,"L");
                g_z_pos_D2_deltaratio->SetLineColor(i_color);
                g_z_pos_D2_deltaratio->SetTitle(("D2 pos delta "+std::to_string(z)).c_str());
                mg_z_D2_pos_deltaratio->Add(g_z_pos_D2_deltaratio,"L");
                g_z_neg_D2_deltaratio->SetLineColor(i_color);
                g_z_neg_D2_deltaratio->SetTitle(("D2 neg delta "+std::to_string(z)).c_str());
                mg_z_D2_neg_deltaratio->Add(g_z_neg_D2_deltaratio,"L");
                // TCanvas* c_H2D2 = new TCanvas();
                // //h_z_H2D2_pos_ratio->Draw();
                // g_z_H2D2_pos_ratio->Draw("AP");
                // g_z_H2D2_pos_ratio->SetMinimum(0.2);
                // g_z_H2D2_pos_ratio->SetMaximum(1);
                // std::string c_H2D2_name = "results/yield/statistics_H2/H2D2_"+q2xz_str_filename+"_pos.pdf";
                // c_H2D2->SaveAs(c_H2D2_name.c_str());
                /* 
                   int nbins_sim = h_z_neg_D2_sim_incnorad->GetXaxis()->GetNbins();
                   TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
                   std::string z_string_sim = "z simc "+(std::to_string(z)).substr(0,4);
                   g_yield_ratio_sim->SetName(z_string_sim.c_str());
                //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_z_neg_D2_sim_incnorad);
                int ii_sim = 0;
                for(int i = 0;i<nbins_sim;i++){
                //std::cout<<i<<std::endl;
                double x = h_z_neg_D2_sim_incnorad->GetBinCenter(i)+0.001*i_color;
                double y = h_z_neg_D2_sim_incnorad->GetBinContent(i);
                double error = h_z_neg_D2_sim_incnorad->GetBinError(i);
                //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
                if(y!=0){
                g_yield_ratio_sim->SetPoint(ii_sim,x,y);
                //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
                ii_sim++;
                }
                }

                // int nbins = h_z_pos_D2_all->GetXaxis()->GetNbins();
                // TGraphErrors* g_yield_ratio = new TGraphErrors();
                // std::string z_string = "z setting "+(std::to_string(z)).substr(0,4);
                // g_yield_ratio->SetName(z_string.c_str());
                // int ii = 0;
                // for(int i = 0;i<nbins;i++){
                //   //std::cout<<i<<std::endl;
                //   double x = h_z_pos_D2_all->GetBinCenter(i);
                //   double y = h_z_pos_D2_all->GetBinContent(i);
                //   double error = h_z_pos_D2_all->GetBinError(i);
                //   //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
                //   if(y!=0){
                //     g_yield_ratio->SetPoint(ii,x,y);
                //     g_yield_ratio->SetPointError(ii,0,error);
                //     ii++;
                //   }
                // }
                */


                //c_Q2x_ratio->cd();
                //h_z_neg_D2_all->SetLineColor(i_color);
                g_diff_ratio->SetMarkerStyle(4);
                g_diff_ratio->SetMarkerColor(i_color);
                g_diff_simratio->SetLineColor(i_color);
                g_sum_ratio->SetMarkerStyle(4);
                g_sum_ratio->SetMarkerColor(i_color);
                g_sum_simratio->SetLineColor(i_color);
                //  g_yield_ratio_sim->SetMarkerStyle(5);
                //  g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
                //  g_yield_ratio_sim->SetLineColor(i_color);
                //hs->Add(h_z_neg_D2_all);
                //h_z_neg_D2_all->Draw("same");
                mg_diff->Add(g_diff_ratio,"P");
                mg_diff->Add(g_diff_simratio,"L");
                mg_sum->Add(g_sum_ratio,"P");
                //mg_sum->Add(g_sum_simratio,"L");
                // mg->Add(g_yield_ratio_sim,"L");
                //c_Q2x_ratio->Update();


                TCanvas *c_Q2x_z_diff_ratio = new TCanvas();
                //TCanvas *c_Q2x_z_diff_ratio = new TCanvas(q2xdiff_name.c_str(),q2xdiff_name.c_str(),1900,1000);
                //g_diff_ratio->GetXaxis()->SetRangeUser(0.1,1);
                //g_diff_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
                g_diff_ratio->GetXaxis()->SetTitle("z");
                g_diff_ratio->GetYaxis()->SetTitle("diff_ratio");
                g_diff_ratio->Draw("AP"); 
                //          g_diff_ratio_sim->Draw("L");
                //c_Q2x_z_diff_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
                std::string c_Q2x_z_diff_ratio_name = "results/yield/statistics_H2/"+q2xz_str_filename+"_diff_ratio.pdf";

                c_Q2x_z_diff_ratio->SaveAs(c_Q2x_z_diff_ratio_name.c_str());

                TCanvas *c_Q2x_z_sum_ratio = new TCanvas();
                //TCanvas *c_Q2x_z_sum_ratio = new TCanvas(q2xsum_name.c_str(),q2xsum_name.c_str(),1900,1000);
                //TCanvas *c_Q2x_z_ratio = new TCanvas();
                // //h_z_neg_D2_all->Draw();
                //g_sum_ratio->GetXaxis()->SetRangeUser(0.1,1);
                //    g_sum_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
                g_sum_ratio->GetXaxis()->SetTitle("z");
                g_sum_ratio->GetYaxis()->SetTitle("sum_ratio");
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
            mg_diff->GetXaxis()->SetLabelSize(0.05);
            mg_diff->GetYaxis()->SetLabelSize(0.05);
            std::string diff_name = canvas_name+",z";
            mg_diff->GetXaxis()->SetTitle(diff_name.c_str());
            //mg_diff->SetMinimum(0.8);
            //mg_diff->SetMaximum(2);
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
            mg_sum->GetXaxis()->SetLabelSize(0.05);
            mg_sum->GetYaxis()->SetLabelSize(0.05);
            std::string sum_name = canvas_name+",z";
            mg_sum->GetXaxis()->SetTitle(sum_name.c_str());
            //mg_sum->SetMinimum(0.8);
            //mg_sum->SetMaximum(1.5);
            mg_sum->Draw("A");
            mg_sum->GetXaxis()->SetLimits(0.35,1);
            //auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
            //TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
            //fit->Draw("same");
            std::string c_Q2x_sum_ratio_name = "results/yield/statistics_H2/"+sum_canvas_filename+"_ratio.pdf";
            c_Q2x_sum_ratio->BuildLegend(0.7,0.7,0.9,0.9);
            c_Q2x_sum_ratio->SaveAs(c_Q2x_sum_ratio_name.c_str());

            TCanvas* c_z_H2D2_pos_ratio = new TCanvas();
            mg_z_H2D2_pos_ratio->GetXaxis()->SetTitle("z");
            mg_z_H2D2_pos_ratio->GetYaxis()->SetTitle("H2/D2 pos Yield ratio");
            std::string H2D2_pos_name = canvas_name+",z";
            mg_z_H2D2_pos_ratio->GetXaxis()->SetTitle(H2D2_pos_name.c_str());
            mg_z_H2D2_pos_ratio->Draw("A");
            mg_z_H2D2_pos_ratio->GetXaxis()->SetLimits(0.25,1);
            //mg_z_H2D2_pos_ratio->SetMinimum(0.3);
            //mg_z_H2D2_pos_ratio->SetMaximum(0.9);
            c_z_H2D2_pos_ratio->BuildLegend(0.7,0.7,0.9,0.9);
            std::string c_z_H2D2_pos_ratio_name = "results/yield/statistics_H2/"+canvas_filename+"_H2D2_pos_ratio.pdf";
            c_z_H2D2_pos_ratio->SaveAs(c_z_H2D2_pos_ratio_name.c_str());
            TCanvas* c_z_H2D2_neg_ratio = new TCanvas();
            mg_z_H2D2_neg_ratio->GetXaxis()->SetTitle("z");
            mg_z_H2D2_neg_ratio->GetYaxis()->SetTitle("H2/D2 neg Yield ratio");
            std::string H2D2_neg_name = canvas_name+",z";
            mg_z_H2D2_neg_ratio->GetXaxis()->SetTitle(H2D2_neg_name.c_str());
            mg_z_H2D2_neg_ratio->Draw("A");
            mg_z_H2D2_neg_ratio->GetXaxis()->SetLimits(0.25,1);
            //mg_z_H2D2_neg_ratio->SetMinimum(0.3);
            //mg_z_H2D2_neg_ratio->SetMaximum(0.9);
            c_z_H2D2_neg_ratio->BuildLegend(0.7,0.7,0.9,0.9);
            std::string c_z_H2D2_neg_ratio_name = "results/yield/statistics_H2/"+canvas_filename+"_H2D2_neg_ratio.pdf";
            c_z_H2D2_neg_ratio->SaveAs(c_z_H2D2_neg_ratio_name.c_str());

            TCanvas* c_datasimratio_H2 = new TCanvas();

            mg_z_H2_datasimratio->GetXaxis()->SetTitle("z");
            //mg_z_H2_datasimratio->GetYaxis()->SetTitle("data/sim ratio");
            mg_z_H2_datasimratio->GetXaxis()->SetTitle(H2D2_pos_name.c_str());
            mg_z_H2_datasimratio->Draw("A");
            //mg_z_H2_datasimratio->SetMinimum(0.5);
            //mg_z_H2_datasimratio->SetMaximum(1.5);
            std::string c_datasimratio_H2_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_datasimratio.pdf";
            c_datasimratio_H2->BuildLegend(0.6,0.6,1,1);
            c_datasimratio_H2->SaveAs(c_datasimratio_H2_name.c_str());
            TCanvas* c_datasimratio_D2 = new TCanvas();

            mg_z_D2_datasimratio->GetXaxis()->SetTitle("z");
            //mg_z_D2_datasimratio->GetYaxis()->SetTitle("data/sim ratio");
            mg_z_D2_datasimratio->GetXaxis()->SetTitle(H2D2_pos_name.c_str());
            mg_z_D2_datasimratio->Draw("A");
            //mg_z_D2_datasimratio->SetMinimum(0.5);
            //mg_z_D2_datasimratio->SetMaximum(1.5);
            std::string c_datasimratio_D2_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_datasimratio.pdf";
            c_datasimratio_D2->BuildLegend(0.6,0.6,1,1);
            c_datasimratio_D2->SaveAs(c_datasimratio_D2_name.c_str());

            TCanvas* c_pos_excratio_H2 = new TCanvas();
            mg_z_H2_pos_excratio->GetXaxis()->SetTitle("z");
            mg_z_H2_pos_excratio->Draw("A");
            std::string c_pos_excratio_H2_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_pos_excratio.pdf";
            c_pos_excratio_H2->BuildLegend(0.1,0.5,0.5,0.9);
            c_pos_excratio_H2->SaveAs(c_pos_excratio_H2_name.c_str());
            TCanvas* c_pos_deltaratio_H2 = new TCanvas();
            mg_z_H2_pos_deltaratio->GetXaxis()->SetTitle("z");
            mg_z_H2_pos_deltaratio->Draw("A");
            //mg_z_H2_pos_deltaratio->SetMinimum(0);
            //mg_z_H2_pos_deltaratio->SetMaximum(1);
            std::string c_pos_deltaratio_H2_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_pos_deltaratio.pdf";
            c_pos_deltaratio_H2->BuildLegend(0.1,0.5,0.5,0.9);
            c_pos_deltaratio_H2->SaveAs(c_pos_deltaratio_H2_name.c_str());
            TCanvas* c_neg_deltaratio_H2 = new TCanvas();
            mg_z_H2_neg_deltaratio->GetXaxis()->SetTitle("z");
            mg_z_H2_neg_deltaratio->Draw("A");
            //mg_z_H2_neg_deltaratio->SetMinimum(0);
            //mg_z_H2_neg_deltaratio->SetMaximum(1);
            std::string c_neg_deltaratio_H2_name = "results/yield/statistics_H2/"+canvas_filename+"_H2_neg_deltaratio.pdf";
            c_neg_deltaratio_H2->BuildLegend(0.1,0.5,0.5,0.9);
            c_neg_deltaratio_H2->SaveAs(c_neg_deltaratio_H2_name.c_str());
            TCanvas* c_pos_excratio_D2 = new TCanvas();
            mg_z_D2_pos_excratio->GetXaxis()->SetTitle("z");
            mg_z_D2_pos_excratio->Draw("A");
            std::string c_pos_excratio_D2_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_pos_excratio.pdf";
            c_pos_excratio_D2->BuildLegend(0.1,0.5,0.5,0.9);
            c_pos_excratio_D2->SaveAs(c_pos_excratio_D2_name.c_str());
            TCanvas* c_neg_excratio_D2 = new TCanvas();
            mg_z_D2_neg_excratio->GetXaxis()->SetTitle("z");
            mg_z_D2_neg_excratio->Draw("A");
            std::string c_neg_excratio_D2_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_neg_excratio.pdf";
            c_neg_excratio_D2->BuildLegend(0.1,0.5,0.5,0.9);
            c_neg_excratio_D2->SaveAs(c_neg_excratio_D2_name.c_str());
            TCanvas* c_pos_deltaratio_D2 = new TCanvas();
            mg_z_D2_pos_deltaratio->GetXaxis()->SetTitle("z");
            mg_z_D2_pos_deltaratio->Draw("A");
            //mg_z_D2_pos_deltaratio->SetMinimum(0);
            //mg_z_D2_pos_deltaratio->SetMaximum(1);
            std::string c_pos_deltaratio_D2_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_pos_deltaratio.pdf";
            c_pos_deltaratio_D2->BuildLegend(0.1,0.5,0.5,0.9);
            c_pos_deltaratio_D2->SaveAs(c_pos_deltaratio_D2_name.c_str());
            TCanvas* c_neg_deltaratio_D2 = new TCanvas();
            mg_z_D2_neg_deltaratio->GetXaxis()->SetTitle("z");
            mg_z_D2_neg_deltaratio->Draw("A");
            //mg_z_D2_neg_deltaratio->SetMinimum(0);
            //mg_z_D2_neg_deltaratio->SetMaximum(1);
            std::string c_neg_deltaratio_D2_name = "results/yield/statistics_H2/"+canvas_filename+"_D2_neg_deltaratio.pdf";
            c_neg_deltaratio_D2->BuildLegend(0.1,0.5,0.5,0.9);
            c_neg_deltaratio_D2->SaveAs(c_neg_deltaratio_D2_name.c_str());
          }//if H2 runs exist, then ploting
        }//if x,Q2 not 0
      }//loop over Q2
    }//loop over x
    std::ofstream ofs("results/yield/H2_diff_sum.json");
    ofs<<jout.dump(4)<<std::endl;
    std::ofstream ofs_H2D2("results/yield/H2D2_ratio.json");
    ofs_H2D2<<jout_H2D2.dump(4)<<std::endl;
    return 0;
  }
