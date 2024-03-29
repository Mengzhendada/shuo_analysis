#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "Get_all_eff.h"
#include "Get_weighted.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

int Q2_xz_ratio_combined(){
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_q2xz_combined.json");
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
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_all = new TMultiGraph(); 
  std::vector<int> i_which_x = {0,1,2,0,3,1,0,2,1,2,3,3};
  std::vector<int> i_which_Q2= {0,0,0,1,0,1,2,1,2,2,1,2};
  int i_whichx = 0; 
  json jout;
  json jout_2;
  json jout_3;
  json jout_4;
  json jout_5;
  json jout_sthwrong;
  std::ofstream csv_file;
  csv_file.open("results/csv_datasub.csv");
  //remember to change xbj_str to xbj , z_str to z
  csv_file<<"Q2,Q2_corr,Q2_corr_err,xbj_set,xbj,xbj_corr,xbj_corr_err,z_set,z,z_corr,z_corr_err,RunGroup,y_RD,error_RD,y_RD_rho,y_RD_1p8rho,y_RD_2rho,RY,RY_rho,RY_1p8rho,RY_noexc,RY_nodelta,error,yield_neg,yield_pos,radia_corr_neg,radia_corr_pos,yield_neg_data,error_neg,yield_pos_data,error_pos,charge_neg_all,charge_pos_all,yield_neg_exc,yield_neg_delta,yield_neg_norho,yield_neg_rho,yield_pos_exc,yield_pos_delta,yield_pos_norho,yield_pos_rho,yield_neg_incnorad,yield_neg_incrad,yield_pos_incnorad,yield_pos_incrad,W2_corr,Wp2_corr,xprime_corr,zprime_corr,shms_p,shms_dp_corr"<<std::endl;                  
  //csv_file<<"Q2,Q2_corr,Q2_corr_err,xbj_set,xbj,xbj_corr,xbj_corr_err,z_set,z,z_corr,z_corr_err,RunGroup,y_RD,error_RD,y_RD_rho,RY,RY_rho,RY_1p8rho,RY_noexc,RY_nodelta,error,yield_neg,yield_pos,yield_neg_data,error_neg,yield_pos_data,error_pos,charge_neg_all,charge_pos_all,yield_neg_exc,yield_neg_delta,yield_neg_rho,yield_pos_exc,yield_pos_delta,yield_pos_rho,yield_neg_incnorad,yield_neg_incrad,yield_pos_incnorad,yield_pos_incrad,W2_corr,Wp2_corr,xprime_corr,zprime_corr,shms_p,shms_dp"<<std::endl;                  
 
  //TH2D* h_Q2_1_neg =new TH2D("","3.95",bins,0,1,bins,0,1);
  //TH2D* h_Q2_1_pos =new TH2D("","3.95",bins,0,1,bins,0,1);
  //TH2D* h_Q2_2_neg =new TH2D("","4.75",bins,0,1,bins,0,1);
  //TH2D* h_Q2_2_pos =new TH2D("","4.75",bins,0,1,bins,0,1);
  //TH2D* h_Q2_3_neg =new TH2D("","5",bins,0,1,bins,0,1);
  //TH2D* h_Q2_3_pos =new TH2D("","5",bins,0,1,bins,0,1);
  //TH2D* h_Q2_4_neg =new TH2D("","5.5",bins,0,1,bins,0,1);
  //TH2D* h_Q2_4_pos =new TH2D("","5.5",bins,0,1,bins,0,1);
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double Q2 = std::stod(it.key());
    auto j_xz = it.value();
    TMultiGraph* mg_RY;
    TMultiGraph* mg_frag;
    TMultiGraph* mg_RDmeas;
    std::string canvas_name = "Q2:"+std::to_string(Q2).substr(0,5);
    std::string canvas_filename = "Q2_"+std::to_string(1000*Q2).substr(0,4);
    std::string q2_name = "Q2:"+std::to_string(Q2).substr(0,5)+"_yieldratio";
    std::string q2_filename = "Q2_"+std::to_string(1000*Q2).substr(0,4)+"_yieldratio";
    TCanvas* c_RY = new TCanvas();
    TCanvas* c_frag = new TCanvas();
    TCanvas* c_RDmeas = new TCanvas();
    for(json::iterator it  = j_xz.begin();it!=j_xz.end();++it){
      double xbj = std::stod(it.key());
      double xbj_set = xbj;
      auto j_z = it.value();
      TH2D* h_neg_q2 = new TH2D("",(q2_name).c_str(),bins,0,1,bins,0,1);
      TH2D* h_pos_q2 = new TH2D("",(q2_name).c_str(),bins,0,1,bins,0,1);
      int i_color = 1;
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          double z_set = z;
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH2D* h_xz_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_neg_allraw = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_neg_all->GetXaxis()->SetTitle("z");
          h_xz_neg_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_xbjmean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_zmean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_xbjmean_neg_all->Sumw2();
          h_xz_zmean_neg_all->Sumw2();
          TH2D* h_xz_xbjmean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_zmean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_xbjmean2_neg_all->Sumw2();
          h_xz_zmean2_neg_all->Sumw2();
          TH2D* h_xz_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_pos_allraw = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_xbjmean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_zmean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_xbjmean_pos_all->Sumw2();
          h_xz_zmean_pos_all->Sumw2();
          TH2D* h_xz_xbjmean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          TH2D* h_xz_zmean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_xbjmean2_pos_all->Sumw2();
          h_xz_zmean2_pos_all->Sumw2();
          h_xz_pos_all->GetXaxis()->SetTitle("z");
          h_xz_pos_all->GetYaxis()->SetTitle("x");
          h_xz_neg_all->Sumw2();
          h_xz_pos_all->Sumw2();
          //for shms_p mean
          TH2D* h_xz_shms_pmean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_shms_pmean_pos_all->Sumw2();
          TH2D* h_xz_shms_pmean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_shms_pmean2_pos_all->Sumw2();
          TH2D* h_xz_shms_pmean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_shms_pmean_neg_all->Sumw2();
          TH2D* h_xz_shms_pmean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_shms_pmean2_neg_all->Sumw2();
          //for shms_dp mean
          TH2D* h_xz_shms_dpmean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_shms_dpmean_pos_all->Sumw2();
          TH2D* h_xz_shms_dpmean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_shms_dpmean2_pos_all->Sumw2();
          TH2D* h_xz_shms_dpmean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_shms_dpmean_neg_all->Sumw2();
          TH2D* h_xz_shms_dpmean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_shms_dpmean2_neg_all->Sumw2();
          //for Q2 mean
          TH2D* h_xz_Q2mean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_Q2mean_pos_all->Sumw2();
          TH2D* h_xz_Q2mean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_Q2mean2_pos_all->Sumw2();
          TH2D* h_xz_Q2mean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_Q2mean_neg_all->Sumw2();
          TH2D* h_xz_Q2mean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_Q2mean2_neg_all->Sumw2();
          //for W2 mean
          TH2D* h_xz_W2mean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_W2mean_pos_all->Sumw2();
          TH2D* h_xz_W2mean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_W2mean2_pos_all->Sumw2();
          TH2D* h_xz_W2mean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_W2mean_neg_all->Sumw2();
          TH2D* h_xz_W2mean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_W2mean2_neg_all->Sumw2();
          //for Wp2 mean
          TH2D* h_xz_Wp2mean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_Wp2mean_pos_all->Sumw2();
          TH2D* h_xz_Wp2mean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_Wp2mean2_pos_all->Sumw2();
          TH2D* h_xz_Wp2mean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_Wp2mean_neg_all->Sumw2();
          TH2D* h_xz_Wp2mean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_Wp2mean2_neg_all->Sumw2();
          //for xprime mean
          TH2D* h_xz_xprimemean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_xprimemean_pos_all->Sumw2();
          TH2D* h_xz_xprimemean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_xprimemean2_pos_all->Sumw2();
          TH2D* h_xz_xprimemean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_xprimemean_neg_all->Sumw2();
          TH2D* h_xz_xprimemean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_xprimemean2_neg_all->Sumw2();
          //std::cout<<"check xprime"<<h_xz_xprimemean_pos_all->GetBinContent(10,10)<<std::endl;
          //for zprime mean
          TH2D* h_xz_zprimemean_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_zprimemean_pos_all->Sumw2();
          TH2D* h_xz_zprimemean2_pos_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_zprimemean2_pos_all->Sumw2();
          TH2D* h_xz_zprimemean_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_zprimemean_neg_all->Sumw2();
          TH2D* h_xz_zprimemean2_neg_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_zprimemean2_neg_all->Sumw2();
          

          TH2D* h_xz_neg_bg_all = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
          h_xz_neg_bg_all->GetXaxis()->SetTitle("z");
          h_xz_neg_bg_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_bg_all = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
          h_xz_pos_bg_all->GetXaxis()->SetTitle("z");
          h_xz_pos_bg_all->GetYaxis()->SetTitle("x");
          h_xz_neg_bg_all->Sumw2();
          h_xz_pos_bg_all->Sumw2();
          TH2D* h_xz_neg_Dummy_all = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
          h_xz_neg_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_neg_Dummy_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_Dummy_all = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
          h_xz_pos_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_pos_Dummy_all->GetXaxis()->SetTitle("x");
          h_xz_neg_Dummy_all->Sumw2();
          h_xz_pos_Dummy_all->Sumw2();
          TH2D* h_xz_neg_bg_Dummy_all = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
          h_xz_neg_bg_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_neg_bg_Dummy_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_bg_Dummy_all = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
          h_xz_pos_bg_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_pos_bg_Dummy_all->GetYaxis()->SetTitle("x");
          h_xz_neg_bg_Dummy_all->Sumw2();
          h_xz_pos_bg_Dummy_all->Sumw2();
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          double charge_neg_all = 0,charge_pos_all=0;
          double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;

          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH2D *h_xz_neg_sim_incrad = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
          h_xz_neg_sim_incrad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_rad");
          TH2D *h_xz_neg_sim_incnorad = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
          h_xz_neg_sim_incnorad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_norad");
          TH2D *h_xz_neg_sim_excrad = new TH2D("","pi- sim exc",bins,0,1,bins,0,1);
          h_xz_neg_sim_excrad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_exc_rad");
          TH2D *h_xz_neg_sim_rho = new TH2D("","pi- sim rho",bins,0,1,bins,0,1);
          h_xz_neg_sim_rho = (TH2D*)rootfile_neg_sim->Get("x_z_neg_rho");
          TH2D *h_xz_neg_sim_delta = new TH2D("","pi- sim delta",bins,0,1,bins,0,1);
          h_xz_neg_sim_delta = (TH2D*)rootfile_neg_sim->Get("x_z_neg_delta");
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH2D *h_xz_pos_sim_incrad = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
          h_xz_pos_sim_incrad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_rad");
          TH2D *h_xz_pos_sim_incnorad = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
          h_xz_pos_sim_incnorad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_norad");
          TH2D *h_xz_pos_sim_excrad = new TH2D("","pi+ sim exc",bins,0,1,bins,0,1);
          h_xz_pos_sim_excrad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_exc_rad");
          TH2D *h_xz_pos_sim_rho = new TH2D("","pi+ sim rho",bins,0,1,bins,0,1);
          h_xz_pos_sim_rho = (TH2D*)rootfile_pos_sim->Get("x_z_pos_rho");
          TH2D *h_xz_pos_sim_delta = new TH2D("","pi+ sim delta",bins,0,1,bins,0,1);
          h_xz_pos_sim_delta = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta");
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            int N_neg_D2 = (int)neg_D2_runs.size();
            int N_pos_D2 = (int)pos_D2_runs.size();
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_neg = new TH2D("","",20,0,1,20,0,1);
              h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
              if(!(TH2D*)root_file_neg->Get("x_z")){
                std::cout<<"failed to get 2d histogram for "<<RunNumber<<std::endl;
              }
              else{
                charge_neg_all += charge;
              }
              TH2D *h_xz_neg_bg = new TH2D("","",20,0,1,20,0,1);
              h_xz_neg_bg = (TH2D*)root_file_neg->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_neg_bg_all->Add(h_xz_neg_bg,1/(6*EFF));
              h_xz_neg_all->Add(h_xz_neg,1/EFF);
              TH2D* h_xz_negraw = (TH2D*)root_file_neg->Get("x_z_raw");
              h_xz_neg_allraw->Add(h_xz_negraw);
              
              TH2D* h_xz_xbjmean_neg = new TH2D("","",20,0,1,20,0,1);
              h_xz_xbjmean_neg = (TH2D*)root_file_neg->Get("weighted_xbj");
              //h_xz_xbjmean_neg->Divide(h_xz_neg);
              TH2D* h_xz_zmean_neg = new TH2D("","",20,0,1,20,0,1);
              h_xz_zmean_neg = (TH2D*)root_file_neg->Get("weighted_z");
              //h_xz_zmean_neg->Divide(h_xz_neg);

              h_xz_xbjmean_neg_all->Add(h_xz_xbjmean_neg);
              h_xz_zmean_neg_all->Add(h_xz_zmean_neg);
              TH2D* h_xz_xbjmean2_neg = new TH2D("","",20,0,1,20,0,1);
              h_xz_xbjmean2_neg = (TH2D*)root_file_neg->Get("weighted_xbj2");
              //h_xz_xbjmean_neg->Divide(h_xz_neg);
              TH2D* h_xz_zmean2_neg = new TH2D("","",20,0,1,20,0,1);
              h_xz_zmean2_neg = (TH2D*)root_file_neg->Get("weighted_z2");
              //h_xz_zmean_neg->Divide(h_xz_neg);
              h_xz_xbjmean2_neg_all->Add(h_xz_xbjmean2_neg);
              h_xz_zmean2_neg_all->Add(h_xz_zmean2_neg);
              //for shms_p mean
              TH2D* h_xz_shms_pmean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_pmean_neg = (TH2D*)root_file_neg->Get("weighted_shms_p");
              h_xz_shms_pmean_neg_all->Add(h_xz_shms_pmean_neg);
              TH2D* h_xz_shms_pmean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_pmean2_neg = (TH2D*)root_file_neg->Get("weighted_shms_p2");
              h_xz_shms_pmean2_neg_all->Add(h_xz_shms_pmean2_neg);
              //std::cout<<"check shms p "<<h_xz_shms_pmean_neg->GetBinContent(10,10)<<std::endl;
              //for shms_dp mean
              TH2D* h_xz_shms_dpmean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_dpmean_neg = (TH2D*)root_file_neg->Get("weighted_shms_dp");
              h_xz_shms_dpmean_neg_all->Add(h_xz_shms_dpmean_neg);
              TH2D* h_xz_shms_dpmean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_dpmean2_neg = (TH2D*)root_file_neg->Get("weighted_shms_dp2");
              h_xz_shms_dpmean2_neg_all->Add(h_xz_shms_dpmean2_neg);
              //std::cout<<"check shms dp"<<h_xz_shms_dpmean_neg->GetBinContent(10,10)<<std::endl;
              //for Q2 mean
              TH2D* h_xz_Q2mean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Q2mean_neg = (TH2D*)root_file_neg->Get("weighted_Q2");
              h_xz_Q2mean_neg_all->Add(h_xz_Q2mean_neg);
              TH2D* h_xz_Q2mean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Q2mean2_neg = (TH2D*)root_file_neg->Get("weighted_Q22");
              h_xz_Q2mean2_neg_all->Add(h_xz_Q2mean2_neg);
              //for W2 mean
              TH2D* h_xz_W2mean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_W2mean_neg = (TH2D*)root_file_neg->Get("weighted_W2");
              h_xz_W2mean_neg_all->Add(h_xz_W2mean_neg);
              TH2D* h_xz_W2mean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_W2mean2_neg = (TH2D*)root_file_neg->Get("weighted_W22");
              h_xz_W2mean2_neg_all->Add(h_xz_W2mean2_neg);
              //for Wp2 mean
              TH2D* h_xz_Wp2mean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Wp2mean_neg = (TH2D*)root_file_neg->Get("weighted_Wp2");
              h_xz_Wp2mean_neg_all->Add(h_xz_Wp2mean_neg);
              TH2D* h_xz_Wp2mean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Wp2mean2_neg = (TH2D*)root_file_neg->Get("weighted_Wp22");
              h_xz_Wp2mean2_neg_all->Add(h_xz_Wp2mean2_neg);
              //for xprime mean
              TH2D* h_xz_xprimemean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_xprimemean_neg = (TH2D*)root_file_neg->Get("weighted_xprime");
              h_xz_xprimemean_neg_all->Add(h_xz_xprimemean_neg);
              TH2D* h_xz_xprimemean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_xprimemean2_neg = (TH2D*)root_file_neg->Get("weighted_xprime2");
              h_xz_xprimemean2_neg_all->Add(h_xz_xprimemean2_neg);
              //for zprime mean
              TH2D* h_xz_zprimemean_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_zprimemean_neg = (TH2D*)root_file_neg->Get("weighted_zprime");
              h_xz_zprimemean_neg_all->Add(h_xz_zprimemean_neg);
              TH2D* h_xz_zprimemean2_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_zprimemean2_neg = (TH2D*)root_file_neg->Get("weighted_zprime2");
              h_xz_zprimemean2_neg_all->Add(h_xz_zprimemean2_neg);

            }//loop over neg runs
            h_xz_xbjmean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_zmean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_xbjmean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_zmean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_shms_pmean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_shms_pmean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_shms_dpmean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_shms_dpmean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_Q2mean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_Q2mean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_W2mean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_W2mean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_Wp2mean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_Wp2mean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_xprimemean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_xprimemean2_neg_all->Divide(h_xz_neg_allraw);
            h_xz_zprimemean_neg_all->Divide(h_xz_neg_allraw);
            h_xz_zprimemean2_neg_all->Divide(h_xz_neg_allraw);
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_pos = new TH2D("","",20,0,1,20,0,1);
              h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
              if(!(TH2D*)root_file_pos->Get("x_z")){
                std::cout<<"failed to get 2d histogram for "<<RunNumber<<std::endl;
              }
              else{
                charge_pos_all+=charge;
              }
              TH2D *h_xz_pos_bg = new TH2D("","",20,0,1,20,0,1);
              h_xz_pos_bg = (TH2D*)root_file_pos->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_pos_bg_all->Add(h_xz_pos_bg,1/(6*EFF));
              h_xz_pos_all->Add(h_xz_pos,1/EFF);
              TH2D* h_xz_posraw = (TH2D*)root_file_pos->Get("x_z_raw");
              h_xz_pos_allraw->Add(h_xz_posraw);
              TH2D* h_xz_xbjmean_pos = new TH2D("","",20,0,1,20,0,1);
              h_xz_xbjmean_pos = (TH2D*)root_file_pos->Get("weighted_xbj");
              //h_xz_xbjmean_pos->Divide(h_xz_pos);
              TH2D* h_xz_zmean_pos = new TH2D("","",20,0,1,20,0,1);
              h_xz_zmean_pos = (TH2D*)root_file_pos->Get("weighted_z");
              //h_xz_zmean_pos->Divide(h_xz_pos);
              h_xz_xbjmean_pos_all->Add(h_xz_xbjmean_pos);
              h_xz_zmean_pos_all->Add(h_xz_zmean_pos);
              TH2D* h_xz_xbjmean2_pos = new TH2D("","",20,0,1,20,0,1);
              h_xz_xbjmean2_pos = (TH2D*)root_file_pos->Get("weighted_xbj2");
              //h_xz_xbjmean_pos->Divide(h_xz_pos);
              TH2D* h_xz_zmean2_pos = new TH2D("","",20,0,1,20,0,1);
              h_xz_zmean2_pos = (TH2D*)root_file_pos->Get("weighted_z2");
              //h_xz_zmean_pos->Divide(h_xz_pos);
              h_xz_xbjmean2_pos_all->Add(h_xz_xbjmean2_pos);
              h_xz_zmean2_pos_all->Add(h_xz_zmean2_pos);
              //for shms_p mean
              TH2D* h_xz_shms_pmean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_pmean_pos = (TH2D*)root_file_pos->Get("weighted_shms_p");
              h_xz_shms_pmean_pos_all->Add(h_xz_shms_pmean_pos);
              TH2D* h_xz_shms_pmean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_pmean2_pos = (TH2D*)root_file_pos->Get("weighted_shms_p2");
              h_xz_shms_pmean2_pos_all->Add(h_xz_shms_pmean2_pos);
              //for shms_dp mean
              TH2D* h_xz_shms_dpmean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_dpmean_pos = (TH2D*)root_file_pos->Get("weighted_shms_dp");
              h_xz_shms_dpmean_pos_all->Add(h_xz_shms_dpmean_pos);
              TH2D* h_xz_shms_dpmean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_shms_dpmean2_pos = (TH2D*)root_file_pos->Get("weighted_shms_dp2");
              h_xz_shms_dpmean2_pos_all->Add(h_xz_shms_dpmean2_pos);
              //for Q2 mean
              TH2D* h_xz_Q2mean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Q2mean_pos = (TH2D*)root_file_pos->Get("weighted_Q2");
              h_xz_Q2mean_pos_all->Add(h_xz_Q2mean_pos);
              TH2D* h_xz_Q2mean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Q2mean2_pos = (TH2D*)root_file_pos->Get("weighted_Q22");
              h_xz_Q2mean2_pos_all->Add(h_xz_Q2mean2_pos);
              //for W2 mean
              TH2D* h_xz_W2mean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_W2mean_pos = (TH2D*)root_file_pos->Get("weighted_W2");
              h_xz_W2mean_pos_all->Add(h_xz_W2mean_pos);
              TH2D* h_xz_W2mean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_W2mean2_pos = (TH2D*)root_file_pos->Get("weighted_W22");
              h_xz_W2mean2_pos_all->Add(h_xz_W2mean2_pos);
              //for Wp2 mean
              TH2D* h_xz_Wp2mean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Wp2mean_pos = (TH2D*)root_file_pos->Get("weighted_Wp2");
              h_xz_Wp2mean_pos_all->Add(h_xz_Wp2mean_pos);
              TH2D* h_xz_Wp2mean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_Wp2mean2_pos = (TH2D*)root_file_pos->Get("weighted_Wp22");
              h_xz_Wp2mean2_pos_all->Add(h_xz_Wp2mean2_pos);
              //for xprime mean
              TH2D* h_xz_xprimemean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_xprimemean_pos = (TH2D*)root_file_pos->Get("weighted_xprime");
              h_xz_xprimemean_pos_all->Add(h_xz_xprimemean_pos);
              TH2D* h_xz_xprimemean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_xprimemean2_pos = (TH2D*)root_file_pos->Get("weighted_xprime2");
              h_xz_xprimemean2_pos_all->Add(h_xz_xprimemean2_pos);
              //for zprime mean
              TH2D* h_xz_zprimemean_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_zprimemean_pos = (TH2D*)root_file_pos->Get("weighted_zprime");
              h_xz_zprimemean_pos_all->Add(h_xz_zprimemean_pos);
              TH2D* h_xz_zprimemean2_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_zprimemean2_pos = (TH2D*)root_file_pos->Get("weighted_zprime2");
              h_xz_zprimemean2_pos_all->Add(h_xz_zprimemean2_pos);
            }//loop over pos runs
            h_xz_xbjmean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_zmean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_xbjmean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_zmean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_shms_pmean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_shms_pmean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_shms_dpmean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_shms_dpmean2_pos_all->Divide(h_xz_pos_allraw);
            //TCanvas* c_shms_p = new TCanvas();
            //h_xz_shms_pmean_pos_all->Draw("box");
            //c_shms_p->SaveAs("results/test.pdf");
            h_xz_Q2mean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_Q2mean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_W2mean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_W2mean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_Wp2mean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_Wp2mean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_xprimemean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_xprimemean2_pos_all->Divide(h_xz_pos_allraw);
            h_xz_zprimemean_pos_all->Divide(h_xz_pos_allraw);
            h_xz_zprimemean2_pos_all->Divide(h_xz_pos_allraw);
            for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<"Dummy"<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_neg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
              if(!(TH2D*)root_file_neg->Get("x_z")){
                std::cout<<"failed to get 2d histogram for "<<RunNumber<<std::endl;
              }
              else{
                charge_neg_Dummy_all += charge;
              }
              TH2D *h_xz_neg_bg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_neg_bg = (TH2D*)root_file_neg->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_neg_bg_Dummy_all->Add(h_xz_neg_bg,1/(6*EFF));
              h_xz_neg_Dummy_all->Add(h_xz_neg,1/EFF);
            }//loop over neg runs
            for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<"Dummy "<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_pos = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
              if(!(TH2D*)root_file_pos->Get("x_z")){
                std::cout<<"failed to get 2d histogram for "<<RunNumber<<std::endl;
              }
              else{
                charge_pos_Dummy_all+=charge;
              }
              TH2D *h_xz_pos_bg = new TH2D("","",bins,0,1,bins,0,1);
              h_xz_pos_bg = (TH2D*)root_file_pos->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_pos_bg_Dummy_all->Add(h_xz_pos_bg,1/(6*EFF));
              h_xz_pos_Dummy_all->Add(h_xz_pos,1/EFF);
            }//loop over pos runs
          }//if z not 0

          //std::cout<<"check"<<std::endl;
          //bg subtracted
          //h_xz_neg_all->Add(h_xz_neg_bg_all,-1.0);
          //h_xz_pos_all->Add(h_xz_pos_bg_all,-1.0);

          //Dummy bg subtract
          h_xz_neg_Dummy_all->Add(h_xz_neg_bg_Dummy_all,-1.0);
          h_xz_pos_Dummy_all->Add(h_xz_pos_bg_Dummy_all,-1.0);


          auto h_xz_neg_incnorad = h_xz_neg_sim_incnorad;
          auto h_xz_neg_incrad = h_xz_neg_sim_incrad;
          auto h_xz_pos_incnorad = h_xz_pos_sim_incnorad;
          auto h_xz_pos_incrad = h_xz_pos_sim_incrad;

          /*
          //std::cout<<"rebin before check"<<std::endl;
          //h_xz_neg_all->RebinX(5);
          //h_xz_neg_bg_all->RebinX(5);
          h_xz_neg_incnorad->RebinX(5);
          h_xz_neg_incrad->RebinX(5);
          h_xz_neg_sim_excrad->RebinX(5);
          h_xz_neg_sim_delta->RebinX(5);
          //          h_xz_neg_sim_rho->RebinX(5);
          //h_xz_pos_all->RebinX(5);
          //h_xz_pos_bg_all->RebinX(5);
          h_xz_pos_incnorad->RebinX(5);
          h_xz_pos_incrad->RebinX(5);
          h_xz_pos_sim_excrad->RebinX(5);
          h_xz_pos_sim_delta->RebinX(5);
          //          h_xz_pos_sim_rho->RebinX(5);
          //h_xz_neg_all->RebinY(5);
          //h_xz_neg_bg_all->RebinY(5);
          h_xz_neg_incnorad->RebinY(5);
          h_xz_neg_incrad->RebinY(5);
          h_xz_neg_sim_excrad->RebinY(5);
          h_xz_neg_sim_delta->RebinY(5);
          //          h_xz_neg_sim_rho->RebinY(5);
          //h_xz_pos_all->RebinY(5);
          //h_xz_pos_bg_all->RebinY(5);
          h_xz_pos_incnorad->RebinY(5);
          h_xz_pos_incrad->RebinY(5);
          h_xz_pos_sim_excrad->RebinY(5);
          h_xz_pos_sim_delta->RebinY(5);
          //          h_xz_pos_sim_rho->RebinY(5);
          //std::cout<<"rebin check"<<std::endl;
          */
          TH2* rp_radia_corr_neg = (TH2*)h_xz_neg_incnorad->Clone("rp_radia_corr");
          TH2* rp_radia_corr_pos = (TH2*)h_xz_pos_incnorad->Clone("rp_radia_corr");
          rp_radia_corr_neg ->Divide(h_xz_neg_incnorad,h_xz_neg_incrad);          
          rp_radia_corr_pos ->Divide(h_xz_pos_incnorad,h_xz_pos_incrad);          

          TCanvas* c_check = new TCanvas();
          h_xz_neg_all->Draw("AP");
          std::string c_check_name = "results/yield/2Dgraphs/"+canvas_filename+"_check.pdf";
          c_check->SaveAs(c_check_name.c_str());

          std::cout<<"ratio check"<<std::endl;
          int xnbins = h_xz_pos_all->GetXaxis()->GetNbins();
          int ynbins = h_xz_pos_all->GetYaxis()->GetNbins();
          std::cout<<"xbins "<<xnbins<<" ybins "<<ynbins<<std::endl;

          h_xz_neg_all->Sumw2();
          h_xz_pos_all->Sumw2();


          TGraph2DErrors* g_yield_ratio = new TGraph2DErrors();
          std::string z_string = "R_Y z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio->SetName(z_string.c_str());
          TGraph2DErrors* g_frag_ratio = new TGraph2DErrors();
          std::string frag_z_string = "frag z setting "+(std::to_string(z)).substr(0,4);
          g_frag_ratio->SetName(frag_z_string.c_str());
          TGraph2DErrors* g_RDmeas_ratio = new TGraph2DErrors();
          std::string RDmeas_z_string = "R^D_meas z setting "+(std::to_string(z)).substr(0,4);
          g_RDmeas_ratio->SetName(RDmeas_z_string.c_str());

          int ii = 0;
          for(int i = 0;i<xnbins;i++){
            for(int j = 0;j<ynbins;j++){
              //std::cout<<i<<std::endl;
              double x = h_xz_neg_all->GetXaxis()->GetBinCenter(i+1);
              double y = h_xz_neg_all->GetYaxis()->GetBinCenter(j+1);
              double yield_neg_D2 = h_xz_neg_all->GetBinContent(i+1,j+1)/charge_neg_all;
              double yield_neg_bg_D2 = h_xz_neg_bg_all->GetBinContent(i+1,j+1)/charge_neg_all;
              double yield_neg_Dummy = h_xz_neg_Dummy_all->GetBinContent(i+1,j+1)/charge_neg_Dummy_all;
              double yield_neg_delta = h_xz_neg_sim_delta->GetBinContent(i+1,j+1);
              double yield_neg_exc = h_xz_neg_sim_excrad->GetBinContent(i+1,j+1);
              double yield_neg_rho = h_xz_neg_sim_rho->GetBinContent(i+1,j+1);
              double error_neg_delta = h_xz_neg_sim_delta->GetBinError(i+1,j+1);
              double error_neg_exc = h_xz_neg_sim_excrad->GetBinError(i+1,j+1);
              double error_neg_rho = h_xz_neg_sim_rho->GetBinError(i+1,j+1);
              double yield_neg = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc;
              double yield_neg_data = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy;
              double yield_neg_noexc = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_rho;
              double yield_neg_nodelta = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_exc-yield_neg_rho;
              double yield_negwithrho = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc-1*yield_neg_rho;
              double yield_neg_norho = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc;
              double yield_negwith_1p8rho = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc-1.8*yield_neg_rho;
              double yield_negwith_2rho = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc-2*yield_neg_rho;
              double error_neg_D2 = h_xz_neg_all->GetBinError(i+1,j+1)/charge_neg_all;
              double error_neg_D2_2 = std::sqrt(h_xz_neg_all->GetBinContent(i+1,j+1))/charge_neg_all;
              if((error_neg_D2_2 - error_neg_D2_2)!=0){std::cout<<"check two errors"<<error_neg_D2/error_neg_D2_2<<std::endl;}
              double error_neg_bg_D2 = h_xz_neg_bg_all->GetBinError(i+1,j+1)/charge_neg_all;
              double error_neg_Dummy = h_xz_neg_Dummy_all->GetBinError(i+1,j+1)/charge_neg_Dummy_all;
              double error_neg = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy);
              double error_neg_all = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy+pow(error_neg_delta,2)+pow(error_neg_exc,2)+pow(error_neg_rho,2));
              double error_neg_nodelta = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy+pow(error_neg_exc,2)+pow(error_neg_rho,2));
              double error_neg_noexc = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy+pow(error_neg_delta,2)+pow(error_neg_rho,2));
              double error_neg_norho = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy+pow(error_neg_delta,2)+pow(error_neg_exc,2));
              double yield_pos_D2 = h_xz_pos_all->GetBinContent(i+1,j+1)/charge_pos_all;
              double yield_pos_bg_D2 = h_xz_pos_bg_all->GetBinContent(i+1,j+1)/charge_pos_all;
              double yield_pos_Dummy = h_xz_pos_Dummy_all->GetBinContent(i+1,j+1)/charge_pos_Dummy_all;
              double yield_pos_delta = h_xz_pos_sim_delta->GetBinContent(i+1,j+1);
              double yield_pos_exc = h_xz_pos_sim_excrad->GetBinContent(i+1,j+1);
              double yield_pos_rho = h_xz_pos_sim_rho->GetBinContent(i+1,j+1);
              double error_pos_delta = h_xz_pos_sim_delta->GetBinError(i+1,j+1);
              double error_pos_exc = h_xz_pos_sim_excrad->GetBinError(i+1,j+1);
              double error_pos_rho = h_xz_pos_sim_rho->GetBinError(i+1,j+1);
              double yield_pos_data = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy;
              double yield_pos_noexc = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_rho;
              double yield_pos_nodelta = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_exc-yield_pos_rho;
              double yield_pos = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc;
              double yield_poswithrho = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc-1*yield_pos_rho;
              double yield_pos_norho = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc;
              double yield_poswith_1p8rho = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc-1.8*yield_pos_rho;
              double yield_poswith_2rho = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc-2*yield_pos_rho;
              double error_pos_D2 = h_xz_pos_all->GetBinError(i+1,j+1)/charge_pos_all;
              double error_pos_bg_D2 = h_xz_pos_bg_all->GetBinError(i+1,j+1)/charge_pos_all;
              double error_pos_Dummy = h_xz_pos_Dummy_all->GetBinError(i+1,j+1)/charge_pos_Dummy_all;
              double error_pos = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy);
              double error_pos_all = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy+pow(error_pos_delta,2)+pow(error_pos_exc,2)+pow(error_pos_rho,2));
              double error_pos_nodelta = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy+pow(error_pos_exc,2)+pow(error_pos_rho,2));
              double error_pos_noexc = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy+pow(error_pos_delta,2)+pow(error_pos_rho,2));
              double error_pos_norho = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy+pow(error_pos_delta,2)+pow(error_pos_exc,2));

              double yield_neg_incrad = h_xz_neg_incrad->GetBinContent(i+1,j+1);
              double yield_pos_incrad = h_xz_pos_incrad->GetBinContent(i+1,j+1);
              double yield_neg_incnorad = h_xz_neg_incnorad->GetBinContent(i+1,j+1);
              double yield_pos_incnorad = h_xz_pos_incnorad->GetBinContent(i+1,j+1);

              //for yield
              double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i+1,j+1);
              double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i+1,j+1);
              //std::cout<<"check neg radiative correction"<<radia_corr_neg<<std::endl;
              if(yield_neg_incrad!=0 && yield_neg_incnorad/yield_neg_incrad!=radia_corr_neg){
                  std::cout<<"wrong neg radiative correction"<<yield_neg_incnorad/yield_neg_incrad<< " rp "<<radia_corr_neg<<std::endl;
              }
              //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
              yield_neg = yield_neg*radia_corr_neg;
              yield_pos = yield_pos*radia_corr_pos;
              error_pos = error_pos*radia_corr_pos;
              error_neg = error_neg*radia_corr_neg;
              yield_negwithrho = yield_negwithrho*radia_corr_neg;
              yield_negwith_1p8rho = yield_negwith_1p8rho*radia_corr_neg;
              yield_negwith_2rho = yield_negwith_2rho*radia_corr_neg;
              yield_poswithrho = yield_poswithrho*radia_corr_pos;
              yield_poswith_1p8rho = yield_poswith_1p8rho*radia_corr_pos;
              yield_poswith_2rho = yield_poswith_2rho*radia_corr_pos;
              yield_neg_data = yield_neg_data*radia_corr_neg;
              yield_pos_data = yield_pos_data*radia_corr_pos;
              yield_neg_noexc = yield_neg_noexc*radia_corr_neg;
              yield_pos_noexc = yield_pos_noexc*radia_corr_pos;
              yield_neg_nodelta = yield_neg_nodelta*radia_corr_neg;
              yield_pos_nodelta = yield_pos_nodelta*radia_corr_pos;
              //need to think of how to convert to xs

              error_neg_nodelta = error_neg_nodelta*radia_corr_neg;
              error_neg_noexc = error_neg_noexc*radia_corr_neg;
              error_neg_norho = error_neg_norho*radia_corr_neg;
              error_pos_nodelta = error_pos_nodelta*radia_corr_pos;
              error_pos_noexc = error_pos_noexc*radia_corr_pos;
              error_pos_norho = error_pos_norho*radia_corr_pos;

              double RY = (yield_neg)/(yield_pos);
              double RY_rho = yield_negwithrho/yield_poswithrho;
              double RY_1p8rho = yield_negwith_1p8rho/yield_poswith_1p8rho;
              double RY_2rho = yield_negwith_2rho/yield_poswith_2rho;
              double RY_noexc = yield_neg_noexc/yield_pos_noexc; 
              double RY_nodelta = yield_neg_nodelta/yield_pos_nodelta; 
              //std::cout<<"RY "<<RY<<std::endl;
              double error = RY*std::sqrt((error_neg*error_neg)/(yield_neg*yield_neg)+(error_pos*error_pos)/(yield_pos*yield_pos));
              double error_rho = RY_rho*std::sqrt((error_neg*error_neg)/(yield_neg_rho*yield_neg_rho)+(error_pos*error_pos)/(yield_pos_rho*yield_pos_rho));
              double y_RD = (4*RY-1)/(1-RY);
              double error_RD = 3*error/((1-RY)*(1-RY));
              double y_RD_rho = (4*RY_rho-1)/(1-RY_rho);
              double y_RD_1p8rho = (4*RY_1p8rho-1)/(1-RY_1p8rho);
              double y_RD_2rho = (4*RY_2rho-1)/(1-RY_2rho);
              double error_RD_rho = 3*error_rho/((1-RY_rho)*(1-RY_rho));
              double y_frag = (4*RY-1)/(4-RY);
              double error_frag = y_frag*std::sqrt((error*error)/(RY*RY));
              //for bin center correction
              double xbj_neg_corr = h_xz_xbjmean_neg_all->GetBinContent(i+1,j+1);
              ///h_xz_neg_allraw->GetBinContent(i+1,j+1);
              
              double z_neg_corr = h_xz_zmean_neg_all->GetBinContent(i+1,j+1);
              ///h_xz_neg_allraw->GetBinContent(i+1,j+1);
              double xbj_pos_corr = h_xz_xbjmean_pos_all->GetBinContent(i+1,j+1);
              ///h_xz_pos_allraw->GetBinContent(i+1,j+1);
              double z_pos_corr = h_xz_zmean_pos_all->GetBinContent(i+1,j+1);
              ///h_xz_pos_allraw->GetBinContent(i+1,j+1);
              double xbj_neg_corr_flag = (xbj_neg_corr);
              double xbj_pos_corr_flag = (xbj_pos_corr);

              //double xbj_neg_corr_err = h_xz_xbjmean_neg_all->GetBinError(i+1,j+1);
              //double z_neg_corr_err = h_xz_zmean_neg_all->GetBinError(i+1,j+1);
              //double xbj_pos_corr_err = h_xz_xbjmean_pos_all->GetBinError(i+1,j+1);
              //double z_pos_corr_err = h_xz_zmean_pos_all->GetBinError(i+1,j+1);
              double xbj_neg_corr_err2 = h_xz_xbjmean2_neg_all->GetBinContent(i+1,j+1)-xbj_neg_corr*xbj_neg_corr;
              double xbj_pos_corr_err2 = h_xz_xbjmean2_pos_all->GetBinContent(i+1,j+1)-xbj_pos_corr*xbj_pos_corr;
              double z_neg_corr_err2 = h_xz_zmean2_neg_all->GetBinContent(i+1,j+1)-z_neg_corr*z_neg_corr;
              double z_pos_corr_err2 = h_xz_zmean2_pos_all->GetBinContent(i+1,j+1)-z_pos_corr*z_pos_corr;

              double xbj_neg_corr_err = sqrt(xbj_neg_corr_err2);
              double xbj_pos_corr_err = sqrt(xbj_pos_corr_err2);
              double z_neg_corr_err = sqrt(z_neg_corr_err2);
              double z_pos_corr_err = sqrt(z_pos_corr_err2);

              double xbj_corr = (xbj_neg_corr/xbj_neg_corr_err2+xbj_pos_corr/xbj_pos_corr_err2)/(1/xbj_neg_corr_err2+1/xbj_pos_corr_err2);
              double xbj_corr_err = 1/(1/xbj_neg_corr_err2+1/xbj_pos_corr_err2);
              double xbj_corr_flag = xbj_corr-y;
              if(abs(xbj_corr_flag)>0.025){
                //std::cout<<"xbj center="<<xbj<<", xbj flag "<<xbj_corr_flag<<", pi+ "<<xbj_pos_corr_flag<<", pi- "<<xbj_neg_corr_flag<<std::endl;
                std::cout<<"xbj center "<<y<<", xbj corr "<<xbj_corr<<", pi+ "<<xbj_pos_corr<<" +- "<<xbj_pos_corr_err<<", pi- "<<xbj_neg_corr<<" +- "<<xbj_neg_corr_err<<std::endl;
              }
              double z_corr = (z_neg_corr/z_neg_corr_err2+z_pos_corr/z_pos_corr_err2)/(1/z_neg_corr_err2+1/z_pos_corr_err2);
              double z_corr_err = 1/(1/z_neg_corr_err2+1/z_pos_corr_err2);
              if(abs(z_corr-x)>0.025){
                std::cout<<"z center "<<x<<",z corr "<<z_corr<<", pi+ "<<z_pos_corr<<" +- "<<z_pos_corr_err<<", pi- "<<z_neg_corr<<" +- "<<z_neg_corr_err<<std::endl;
              } 
              //for shms_pmean
              double shms_p_neg_corr = h_xz_shms_pmean_neg_all->GetBinContent(i+1,j+1);
              double shms_p_pos_corr = h_xz_shms_pmean_pos_all->GetBinContent(i+1,j+1);
              double shms_p_neg_corr_err2 = h_xz_shms_pmean2_neg_all->GetBinContent(i+1,j+1)-shms_p_neg_corr*shms_p_neg_corr;
              double shms_p_pos_corr_err2 = h_xz_shms_pmean2_pos_all->GetBinContent(i+1,j+1)-shms_p_pos_corr*shms_p_pos_corr;
              double shms_p_corr = (shms_p_neg_corr/shms_p_neg_corr_err2+shms_p_pos_corr/shms_p_pos_corr_err2)/(1/shms_p_neg_corr_err2+1/shms_p_pos_corr_err2);
              double shms_p_corr_err = 1/(1/shms_p_neg_corr_err2+1/shms_p_pos_corr_err2);
              //for shms_dpmean
              double shms_dp_neg_corr = h_xz_shms_dpmean_neg_all->GetBinContent(i+1,j+1);
              double shms_dp_pos_corr = h_xz_shms_dpmean_pos_all->GetBinContent(i+1,j+1);
              double shms_dp_neg_corr_err2 = h_xz_shms_dpmean2_neg_all->GetBinContent(i+1,j+1)-shms_dp_neg_corr*shms_dp_neg_corr;
              double shms_dp_pos_corr_err2 = h_xz_shms_dpmean2_pos_all->GetBinContent(i+1,j+1)-shms_dp_pos_corr*shms_dp_pos_corr;
              double shms_dp_corr = (shms_dp_neg_corr/shms_dp_neg_corr_err2+shms_dp_pos_corr/shms_dp_pos_corr_err2)/(1/shms_dp_neg_corr_err2+1/shms_dp_pos_corr_err2);
              double shms_dp_corr_err = 1/(1/shms_dp_neg_corr_err2+1/shms_dp_pos_corr_err2);
             
              //std::cout<<"shms p neg "<<shms_p_neg_corr<<" shms p pos "<<shms_p_pos_corr<<std::endl;
              //if(shms_p_corr_err<0){
                //std::cout<<"shms_p2 pos "<<h_xz_shms_pmean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_shms_pmean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                //std::cout<<"shms_p corr err pos "<<shms_p_pos_corr_err2<<" neg "<<shms_p_neg_corr_err2<<std::endl;
              //}
              //for Q2mean
              double Q2_neg_corr = h_xz_Q2mean_neg_all->GetBinContent(i+1,j+1);
              double Q2_pos_corr = h_xz_Q2mean_pos_all->GetBinContent(i+1,j+1);
              double Q2_neg_corr_err2 = h_xz_Q2mean2_neg_all->GetBinContent(i+1,j+1)-Q2_neg_corr*Q2_neg_corr;
              double Q2_pos_corr_err2 = h_xz_Q2mean2_pos_all->GetBinContent(i+1,j+1)-Q2_pos_corr*Q2_pos_corr;
              double Q2_corr = (Q2_neg_corr/Q2_neg_corr_err2+Q2_pos_corr/Q2_pos_corr_err2)/(1/Q2_neg_corr_err2+1/Q2_pos_corr_err2);
              double Q2_corr_err = 1/(1/Q2_neg_corr_err2+1/Q2_pos_corr_err2);
             
              if(Q2_corr_err<0){
                std::cout<<"Q22 pos "<<h_xz_Q2mean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_Q2mean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                std::cout<<"Q2 corr err pos "<<Q2_pos_corr_err2<<" neg "<<Q2_neg_corr_err2<<std::endl;
              }
              //for W2mean
              double W2_neg_corr = h_xz_W2mean_neg_all->GetBinContent(i+1,j+1);
              double W2_pos_corr = h_xz_W2mean_pos_all->GetBinContent(i+1,j+1);
              double W2_neg_corr_err2 = h_xz_W2mean2_neg_all->GetBinContent(i+1,j+1)-W2_neg_corr*W2_neg_corr;
              double W2_pos_corr_err2 = h_xz_W2mean2_pos_all->GetBinContent(i+1,j+1)-W2_pos_corr*W2_pos_corr;
              double W2_corr = (W2_neg_corr/W2_neg_corr_err2+W2_pos_corr/W2_pos_corr_err2)/(1/W2_neg_corr_err2+1/W2_pos_corr_err2);
              double W2_corr_err = 1/(1/W2_neg_corr_err2+1/W2_pos_corr_err2);
             
              if(W2_corr_err<0){
                std::cout<<"W22 pos "<<h_xz_W2mean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_W2mean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                std::cout<<"W2 corr err pos "<<W2_pos_corr_err2<<" neg "<<W2_neg_corr_err2<<std::endl;
              }
              //for Wp2mean
              double Wp2_neg_corr = h_xz_Wp2mean_neg_all->GetBinContent(i+1,j+1);
              double Wp2_pos_corr = h_xz_Wp2mean_pos_all->GetBinContent(i+1,j+1);
              double Wp2_neg_corr_err2 = h_xz_Wp2mean2_neg_all->GetBinContent(i+1,j+1)-Wp2_neg_corr*Wp2_neg_corr;
              double Wp2_pos_corr_err2 = h_xz_Wp2mean2_pos_all->GetBinContent(i+1,j+1)-Wp2_pos_corr*Wp2_pos_corr;
              double Wp2_corr = (Wp2_neg_corr/Wp2_neg_corr_err2+Wp2_pos_corr/Wp2_pos_corr_err2)/(1/Wp2_neg_corr_err2+1/Wp2_pos_corr_err2);
              double Wp2_corr_err = 1/(1/Wp2_neg_corr_err2+1/Wp2_pos_corr_err2);
             
              if(Wp2_corr_err<0){
                std::cout<<"Wp22 pos "<<h_xz_Wp2mean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_Wp2mean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                std::cout<<"Wp2 corr err pos "<<Wp2_pos_corr_err2<<" neg "<<Wp2_neg_corr_err2<<std::endl;
              }
              //for xprimemean
              double xprime_neg_corr = h_xz_xprimemean_neg_all->GetBinContent(i+1,j+1);
              double xprime_pos_corr = h_xz_xprimemean_pos_all->GetBinContent(i+1,j+1);
              double xprime_neg_corr_err2 = h_xz_xprimemean2_neg_all->GetBinContent(i+1,j+1)-xprime_neg_corr*xprime_neg_corr;
              double xprime_pos_corr_err2 = h_xz_xprimemean2_pos_all->GetBinContent(i+1,j+1)-xprime_pos_corr*xprime_pos_corr;
              double xprime_corr = (xprime_neg_corr/xprime_neg_corr_err2+xprime_pos_corr/xprime_pos_corr_err2)/(1/xprime_neg_corr_err2+1/xprime_pos_corr_err2);
              double xprime_corr_err = 1/(1/xprime_neg_corr_err2+1/xprime_pos_corr_err2);
             
              if(xprime_corr_err<0){
                std::cout<<"xprime2 pos "<<h_xz_xprimemean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_xprimemean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                std::cout<<"xprime corr err pos "<<xprime_pos_corr_err2<<" neg "<<xprime_neg_corr_err2<<std::endl;
              }
              //for zprimemean
              double zprime_neg_corr = h_xz_zprimemean_neg_all->GetBinContent(i+1,j+1);
              double zprime_pos_corr = h_xz_zprimemean_pos_all->GetBinContent(i+1,j+1);
              double zprime_neg_corr_err2 = h_xz_zprimemean2_neg_all->GetBinContent(i+1,j+1)-zprime_neg_corr*zprime_neg_corr;
              double zprime_pos_corr_err2 = h_xz_zprimemean2_pos_all->GetBinContent(i+1,j+1)-zprime_pos_corr*zprime_pos_corr;
              double zprime_corr = (zprime_neg_corr/zprime_neg_corr_err2+zprime_pos_corr/zprime_pos_corr_err2)/(1/zprime_neg_corr_err2+1/zprime_pos_corr_err2);
              double zprime_corr_err = 1/(1/zprime_neg_corr_err2+1/zprime_pos_corr_err2);
             
              if(zprime_corr_err<0){
                std::cout<<"zprime2 pos "<<h_xz_zprimemean2_pos_all->GetBinContent(i+1,j+1)<<" neg "<<h_xz_zprimemean2_neg_all->GetBinContent(i+1,j+1)<<std::endl;
                std::cout<<"zprime corr err pos "<<zprime_pos_corr_err2<<" neg "<<zprime_neg_corr_err2<<std::endl;
              }
              if(RY>0 && error< 0.1){
                //double y_RD = (4*y-1)/(1-y);
                g_yield_ratio->SetPoint(ii,x,y,RY);
                //g_yield_ratio->SetPoint(ii,x,y_RD);
                g_yield_ratio->SetPointError(ii,0,0,error);
                g_frag_ratio ->SetPoint(ii,x,y,y_frag);
                g_frag_ratio->SetPointError(ii,0,0,error_frag);
                g_RDmeas_ratio->SetPoint(ii,x,y,y_RD);
                g_RDmeas_ratio->SetPointError(ii,0,0,error_RD);
                ii++;
                std::string z_str = std::to_string(x); 
                std::string xbj_str = std::to_string(y); 

                if(y_RD>0 && y_RD<10 && error_RD<20 && error_RD>0 ){
                //if(y_RD>0 && y_RD<10 && error_RD<20 && error_RD>0 && yield_neg_incrad>3){
                  jout[std::to_string(Q2)][xbj_str][z_str]["value"].push_back(y_RD);
                  jout[std::to_string(Q2)][xbj_str][z_str]["error"].push_back(error_RD);
                  jout[std::to_string(Q2)][xbj_str][z_str]["RD_rho"].push_back(y_RD_rho);
                  jout[std::to_string(Q2)][xbj_str][z_str]["RDerror_rho"].push_back(error_RD_rho);
                  jout[std::to_string(Q2)][xbj_str][z_str]["yield_neg"].push_back(yield_neg);
                  jout[std::to_string(Q2)][xbj_str][z_str]["yield_pos"].push_back(yield_pos);
                  jout[std::to_string(Q2)][xbj_str][z_str]["charge_neg"].push_back(charge_neg_all);
                  jout[std::to_string(Q2)][xbj_str][z_str]["charge_pos"].push_back(charge_pos_all);
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["value"] = y_RD;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error"] = error_RD;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["RD_rho"] = y_RD_rho;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["RDerror_rho"] = error_RD_rho;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg"] = yield_neg;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos"] = yield_pos;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error_neg"] = error_neg;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error_pos"] = error_pos;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg_rho"] = yield_neg_rho;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos_rho"] = yield_pos_rho;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg_incrad"] = yield_neg_incrad;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos_incrad"] = yield_pos_incrad;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_neg"] = charge_neg_all;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_pos"] = charge_pos_all;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_neg_corr"] = xbj_neg_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_neg_corr"] = z_neg_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_pos_corr"] = xbj_pos_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_pos_corr"] = z_pos_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr"] = xbj_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_err"] = xbj_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["shms_p_corr"] = shms_p_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["shms_p_corr_err"] = shms_p_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["shms_dp_corr"] = shms_dp_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["shms_dp_corr_err"] = shms_dp_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["Q2_corr"] = Q2_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["Q2_corr_err"] = Q2_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["W2_corr"] = W2_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["W2_corr_err"] = W2_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["Wp2_corr"] = Wp2_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["Wp2_corr_err"] = Wp2_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xprime_corr"] = xprime_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xprime_corr_err"] = xprime_corr_err;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["zprime_corr"] = zprime_corr;
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["zprime_corr_err"] = zprime_corr_err;
                  if(xbj_corr_flag>0.025){
                    jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_flag"] = "outofrange";
                    //std::cout<<"outofrange alert"<<std::endl;
                  }
                  jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_corr"] = z_corr;
                  csv_file<<Q2<<","<<Q2_corr<<","<<Q2_corr_err<<","<<xbj_set<<","<<xbj_str<<","<<xbj_corr<<","<<xbj_corr_err<<","<<z_set<<","<<z_str<<","<<z_corr<<","<<z_corr_err<<","<<RunGroup<<","<<y_RD<<","<<error_RD<<","<<y_RD_rho<<","<<y_RD_1p8rho<<","<<y_RD_2rho<<","<<RY<<","<<RY_rho<<","<<RY_1p8rho<<","<<RY_noexc<<","<<RY_nodelta<<","<<error<<","<<yield_neg<<","<<yield_pos<<","<<radia_corr_neg<<","<<radia_corr_pos<<","<<yield_neg_data<<","<<error_neg<<","<<yield_pos_data<<","<<error_pos<<","<<charge_neg_all<<","<<charge_pos_all<<","<<yield_neg_exc<<","<<yield_neg_delta<<","<<yield_neg_norho<<","<<yield_neg_rho<<","<<yield_pos_exc<<","<<yield_pos_delta<<","<<yield_pos_norho<<","<<yield_pos_rho<<","<<yield_neg_incnorad<<","<<yield_neg_incrad<<","<<yield_pos_incnorad<<","<<yield_pos_incrad<<","<<W2_corr<<","<<Wp2_corr<<","<<xprime_corr<<","<<zprime_corr<<","<<shms_p_corr<<","<<shms_dp_corr<<std::endl;                  
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["RD_rho"] = y_RD_rho;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["RDerror_rho"] = error_RD_rho;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error_neg"] = error_neg;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error_pos"] = error_pos;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg_rho"] = yield_neg_rho;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos_rho"] = yield_pos_rho;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg_incrad"] = yield_neg_incrad;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos_incrad"] = yield_pos_incrad;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["shms_p_corr"] = shms_p_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["shms_p_corr_err"] = shms_p_corr_err;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["shms_dp_corr"] = shms_dp_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["shms_dp_corr_err"] = shms_dp_corr_err;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["Q2_corr"] = Q2_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["Q2_corr_err"] = Q2_corr_err;
                  if(xbj_corr_flag>0.025){
                    jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_flag"] = "outofrange";
                  }
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr;
                  jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["RD_rho"] = y_RD_rho;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["RDerror_rho"] = error_RD_rho;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error_neg"] = error_neg;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error_pos"] = error_pos;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg_rho"] = yield_neg_rho;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos_rho"] = yield_pos_rho;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg_incrad"] = yield_neg_incrad;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos_incrad"] = yield_pos_incrad;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["shms_p_corr"] = shms_p_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["shms_p_corr_err"] = shms_p_corr_err;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["shms_dp_corr"] = shms_dp_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["shms_dp_corr_err"] = shms_dp_corr_err;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["Q2_corr"] = Q2_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["Q2_corr_err"] = Q2_corr_err;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr;
                  jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["value"] = y_RD;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error"] = error_RD;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["RD_rho"] = y_RD_rho;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["RDerror_rho"] = error_RD_rho;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg"] = yield_neg;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos"] = yield_pos;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error_neg"] = error_neg;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error_pos"] = error_pos;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg_rho"] = yield_neg_rho;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos_rho"] = yield_pos_rho;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg_incrad"] = yield_neg_incrad;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos_incrad"] = yield_pos_incrad;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_neg"] = charge_neg_all;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_pos"] = charge_pos_all;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_neg_corr"] = xbj_neg_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_neg_corr"] = z_neg_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_pos_corr"] = xbj_pos_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_pos_corr"] = z_pos_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr"] = xbj_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr_err"] = xbj_corr_err;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["shms_p_corr"] = shms_p_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["shms_p_corr_err"] = shms_p_corr_err;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["shms_dp_corr"] = shms_dp_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["shms_dp_corr_err"] = shms_dp_corr_err;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["Q2_corr"] = Q2_corr;
                  jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["Q2_corr_err"] = Q2_corr_err;
                  jout_sthwrong[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["rungroup"] = RunGroup;

                }
              }
            }
          }

          g_yield_ratio->SetMarkerStyle(20);
          g_yield_ratio->SetLineColor(i_color);
          g_yield_ratio->SetMarkerColor(i_color);
          c_RY->cd();
          g_yield_ratio->Draw("pcol");
          //mg_RY->Add(g_yield_ratio,"P");
          g_frag_ratio->SetMarkerStyle(20);
          g_frag_ratio->SetLineColor(i_color);
          g_frag_ratio->SetMarkerColor(i_color);
          c_frag->cd();
          g_frag_ratio->Draw("pcol");
          //mg_frag->Add(g_frag_ratio,"P");
          g_RDmeas_ratio->SetMarkerStyle(20);
          g_RDmeas_ratio->SetLineColor(i_color);
          g_RDmeas_ratio->SetMarkerColor(i_color);
          c_RDmeas->cd();
          g_RDmeas_ratio->Draw("pcol");
          //mg_RDmeas->Add(g_RDmeas_ratio,"P");
          i_color++;
        }//loop over z
        i_whichx++;
      }//if x,Q2 not 0
      //i_whichq2++;
    }//loop over x
    //TCanvas* c_RY = new TCanvas();
    //gStyle->SetPalette(1);
    //mg_RY->Draw("acol");
    std::string c_RY_name = "results/yield/2Dgraphs/"+canvas_filename+"_RY.pdf";
    c_RY->SaveAs(c_RY_name.c_str());
    //TCanvas* c_frag = new TCanvas();
    //gStyle->SetPalette(1);
    //mg_frag->Draw("acol");
    std::string c_frag_name = "results/yield/2Dgraphs/"+canvas_filename+"_frag.pdf";
    c_frag->SaveAs(c_frag_name.c_str());
    //TCanvas* c_RDmeas = new TCanvas();
    //gStyle->SetPalette(1);
    //mg_RDmeas->Draw("acol");
    std::string c_RDmeas_name = "results/yield/2Dgraphs/"+canvas_filename+"_RDmeas.pdf";
    c_RDmeas->SaveAs(c_RDmeas_name.c_str());
  }//loop over Q2 

  std::string jout_name = "results/yield_ratio_xz_combined.json";
  std::ofstream ofs_jout(jout_name.c_str());
  ofs_jout<<jout.dump(4)<<std::endl;
  
  std::string jout_2_name = "results/yield_ratio_xz_combined_withRungroups.json";
  std::ofstream ofs_jout_2(jout_2_name.c_str());
  ofs_jout_2<<jout_2.dump(4)<<std::endl;
  
  std::string jout_3_name = "results/yield_ratio_Q2xzRunG_combined.json";
  std::ofstream ofs_jout_3(jout_3_name.c_str());
  ofs_jout_3<<jout_3.dump(4)<<std::endl;

  std::string jout_4_name = "results/yield_ratio_Q2zxRunG_combined.json";
  std::ofstream ofs_jout_4(jout_4_name.c_str());
  ofs_jout_4<<jout_4.dump(4)<<std::endl;
  
  std::string jout_5_name = "results/yield_ratio_Q2zRunGx_combined.json";
  std::ofstream ofs_jout_5(jout_5_name.c_str());
  ofs_jout_5<<jout_5.dump(5)<<std::endl;
  
  std::string jout_sthwrong_name = "results/yield_ratio_xz_combined_sthwrong.json";
  std::ofstream ofs_jout_sthwrong(jout_sthwrong_name.c_str());
  ofs_jout_sthwrong<<jout_sthwrong.dump(4)<<std::endl;

  return 0;
}
