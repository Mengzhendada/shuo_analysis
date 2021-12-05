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

int Q2xz_ratio_Q2cut(){
  json j_RunGroup;
  {
    std::ifstream runs("db2/ratio_run_group_updated.json");
    runs>>j_RunGroup;
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
  auto mg_all = new TMultiGraph(); 
  int i_whichx = 0; 
  json jout;
  json jout_2;
  json jout_3;
  json jout_4;
  json jout_5;
  json jout_sthwrong;
  for(json::iterator it = j_RunGroup.begin();it!=j_RunGroup.end();++it){
    int RunGroup = std::stoi(it.key());
    TH2D* h_xz_neg_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_neg_allraw_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_all_1->GetXaxis()->SetTitle("z");
    h_xz_neg_all_1->GetYaxis()->SetTitle("x");
    TH2D* h_xz_xbjmean_neg_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_neg_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_neg_all_1->Sumw2();
    h_xz_zmean_neg_all_1->Sumw2();
    TH2D* h_xz_xbjmean2_neg_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_neg_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_neg_all_1->Sumw2();
    h_xz_zmean2_neg_all_1->Sumw2();
    TH2D* h_xz_pos_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_pos_allraw_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_xbjmean_pos_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_pos_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_pos_all_1->Sumw2();
    h_xz_zmean_pos_all_1->Sumw2();
    TH2D* h_xz_xbjmean2_pos_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_pos_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_pos_all_1->Sumw2();
    h_xz_zmean2_pos_all_1->Sumw2();
    h_xz_pos_all_1->GetXaxis()->SetTitle("z");
    h_xz_pos_all_1->GetYaxis()->SetTitle("x");
    h_xz_neg_all_1->Sumw2();
    h_xz_pos_all_1->Sumw2();
    TH2D* h_xz_neg_bg_all_1 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_all_1->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_all_1->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_all_1 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_all_1->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_all_1->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_all_1->Sumw2();
    h_xz_pos_bg_all_1->Sumw2();
    TH2D* h_xz_neg_Dummy_all_1 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_Dummy_all_1->GetXaxis()->SetTitle("z");
    h_xz_neg_Dummy_all_1->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_Dummy_all_1 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_pos_Dummy_all_1->GetXaxis()->SetTitle("z");
    h_xz_pos_Dummy_all_1->GetXaxis()->SetTitle("x");
    h_xz_neg_Dummy_all_1->Sumw2();
    h_xz_pos_Dummy_all_1->Sumw2();
    TH2D* h_xz_neg_bg_Dummy_all_1 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_Dummy_all_1->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_Dummy_all_1->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_Dummy_all_1 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_Dummy_all_1->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_Dummy_all_1->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_Dummy_all_1->Sumw2();
    h_xz_pos_bg_Dummy_all_1->Sumw2();

    TH2D* h_xz_neg_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_neg_allraw_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_all_2->GetXaxis()->SetTitle("z");
    h_xz_neg_all_2->GetYaxis()->SetTitle("x");
    TH2D* h_xz_xbjmean_neg_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_neg_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_neg_all_2->Sumw2();
    h_xz_zmean_neg_all_2->Sumw2();
    TH2D* h_xz_xbjmean2_neg_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_neg_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_neg_all_2->Sumw2();
    h_xz_zmean2_neg_all_2->Sumw2();
    TH2D* h_xz_pos_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_pos_allraw_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_xbjmean_pos_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_pos_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_pos_all_2->Sumw2();
    h_xz_zmean_pos_all_2->Sumw2();
    TH2D* h_xz_xbjmean2_pos_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_pos_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_pos_all_2->Sumw2();
    h_xz_zmean2_pos_all_2->Sumw2();
    h_xz_pos_all_2->GetXaxis()->SetTitle("z");
    h_xz_pos_all_2->GetYaxis()->SetTitle("x");
    h_xz_neg_all_2->Sumw2();
    h_xz_pos_all_2->Sumw2();
    TH2D* h_xz_neg_bg_all_2 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_all_2->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_all_2->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_all_2 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_all_2->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_all_2->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_all_2->Sumw2();
    h_xz_pos_bg_all_2->Sumw2();
    TH2D* h_xz_neg_Dummy_all_2 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_Dummy_all_2->GetXaxis()->SetTitle("z");
    h_xz_neg_Dummy_all_2->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_Dummy_all_2 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_pos_Dummy_all_2->GetXaxis()->SetTitle("z");
    h_xz_pos_Dummy_all_2->GetXaxis()->SetTitle("x");
    h_xz_neg_Dummy_all_2->Sumw2();
    h_xz_pos_Dummy_all_2->Sumw2();
    TH2D* h_xz_neg_bg_Dummy_all_2 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_Dummy_all_2->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_Dummy_all_2->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_Dummy_all_2 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_Dummy_all_2->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_Dummy_all_2->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_Dummy_all_2->Sumw2();
    h_xz_pos_bg_Dummy_all_2->Sumw2();

    TH2D* h_xz_neg_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_neg_allraw_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_all_3->GetXaxis()->SetTitle("z");
    h_xz_neg_all_3->GetYaxis()->SetTitle("x");
    TH2D* h_xz_xbjmean_neg_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_neg_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_neg_all_3->Sumw2();
    h_xz_zmean_neg_all_3->Sumw2();
    TH2D* h_xz_xbjmean2_neg_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_neg_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_neg_all_3->Sumw2();
    h_xz_zmean2_neg_all_3->Sumw2();
    TH2D* h_xz_pos_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_pos_allraw_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_xbjmean_pos_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean_pos_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean_pos_all_3->Sumw2();
    h_xz_zmean_pos_all_3->Sumw2();
    TH2D* h_xz_xbjmean2_pos_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    TH2D* h_xz_zmean2_pos_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_xbjmean2_pos_all_3->Sumw2();
    h_xz_zmean2_pos_all_3->Sumw2();
    h_xz_pos_all_3->GetXaxis()->SetTitle("z");
    h_xz_pos_all_3->GetYaxis()->SetTitle("x");
    h_xz_neg_all_3->Sumw2();
    h_xz_pos_all_3->Sumw2();
    TH2D* h_xz_neg_bg_all_3 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_all_3->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_all_3->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_all_3 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_all_3->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_all_3->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_all_3->Sumw2();
    h_xz_pos_bg_all_3->Sumw2();
    TH2D* h_xz_neg_Dummy_all_3 = new TH2D("","neg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_Dummy_all_3->GetXaxis()->SetTitle("z");
    h_xz_neg_Dummy_all_3->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_Dummy_all_3 = new TH2D("","pos;z;x",bins,0,1,bins,0,1);
    h_xz_pos_Dummy_all_3->GetXaxis()->SetTitle("z");
    h_xz_pos_Dummy_all_3->GetXaxis()->SetTitle("x");
    h_xz_neg_Dummy_all_3->Sumw2();
    h_xz_pos_Dummy_all_3->Sumw2();
    TH2D* h_xz_neg_bg_Dummy_all_3 = new TH2D("","neg bg;z;x",bins,0,1,bins,0,1);
    h_xz_neg_bg_Dummy_all_3->GetXaxis()->SetTitle("z");
    h_xz_neg_bg_Dummy_all_3->GetYaxis()->SetTitle("x");
    TH2D* h_xz_pos_bg_Dummy_all_3 = new TH2D("","pos bg;z;x",bins,0,1,bins,0,1);
    h_xz_pos_bg_Dummy_all_3->GetXaxis()->SetTitle("z");
    h_xz_pos_bg_Dummy_all_3->GetYaxis()->SetTitle("x");
    h_xz_neg_bg_Dummy_all_3->Sumw2();
    h_xz_pos_bg_Dummy_all_3->Sumw2();
    std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
    auto runjs = it.value();


    double charge_neg_all = 0,charge_pos_all=0;
    double charge_neg_Dummy_all = 0,charge_pos_Dummy_all=0;

    neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
    neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
    pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
    int N_neg_D2 = (int)neg_D2_runs.size();
    int N_pos_D2 = (int)pos_D2_runs.size();
    std::cout<<"check rungroup"<<RunGroup<<" neg "<<N_neg_D2<<" pos "<<N_pos_D2<<std::endl;
    if(N_neg_D2>0 && N_pos_D2>0){
      TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
      TH2D *h_xz_neg_sim_incrad_1 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incrad_1 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_rad_1");
      TH2D *h_xz_neg_sim_incnorad_1 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incnorad_1 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_norad_1");
      TH2D *h_xz_neg_sim_excrad_1 = new TH2D("","pi- sim exc",bins,0,1,bins,0,1);
      h_xz_neg_sim_excrad_1 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_exc_rad_1");
      TH2D *h_xz_neg_sim_delta_1 = new TH2D("","pi- sim delta",bins,0,1,bins,0,1);
      h_xz_neg_sim_delta_1 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_delta_1");
      TH2D *h_xz_neg_sim_incrad_2 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incrad_2 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_rad_2");
      TH2D *h_xz_neg_sim_incnorad_2 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incnorad_2 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_norad_2");
      TH2D *h_xz_neg_sim_excrad_2 = new TH2D("","pi- sim exc",bins,0,1,bins,0,1);
      h_xz_neg_sim_excrad_2 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_exc_rad_2");
      TH2D *h_xz_neg_sim_delta_2 = new TH2D("","pi- sim delta",bins,0,1,bins,0,1);
      h_xz_neg_sim_delta_2 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_delta_2");
      TH2D *h_xz_neg_sim_incrad_3 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incrad_3 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_rad_3");
      TH2D *h_xz_neg_sim_incnorad_3 = new TH2D("","pi- sim SIDIS",bins,0,1,bins,0,1);
      h_xz_neg_sim_incnorad_3 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_norad_3");
      TH2D *h_xz_neg_sim_excrad_3 = new TH2D("","pi- sim exc",bins,0,1,bins,0,1);
      h_xz_neg_sim_excrad_3 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_exc_rad_3");
      TH2D *h_xz_neg_sim_delta_3 = new TH2D("","pi- sim delta",bins,0,1,bins,0,1);
      h_xz_neg_sim_delta_3 = (TH2D*)rootfile_neg_sim->Get("x_z_neg_delta_3");
      TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
      TH2D *h_xz_pos_sim_incrad_1 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incrad_1 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_rad");
      TH2D *h_xz_pos_sim_incnorad_1 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incnorad_1 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_norad_1");
      TH2D *h_xz_pos_sim_excrad_1 = new TH2D("","pi+ sim exc",bins,0,1,bins,0,1);
      h_xz_pos_sim_excrad_1 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_exc_rad_1");
      TH2D *h_xz_pos_sim_delta_1 = new TH2D("","pi+ sim delta",bins,0,1,bins,0,1);
      h_xz_pos_sim_delta_1 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta_1");
      TH2D *h_xz_pos_sim_incrad_2 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incrad_2 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_rad");
      TH2D *h_xz_pos_sim_incnorad_2 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incnorad_2 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_norad_2");
      TH2D *h_xz_pos_sim_excrad_2 = new TH2D("","pi+ sim exc",bins,0,1,bins,0,1);
      h_xz_pos_sim_excrad_2 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_exc_rad_2");
      TH2D *h_xz_pos_sim_delta_2 = new TH2D("","pi+ sim delta",bins,0,1,bins,0,1);
      h_xz_pos_sim_delta_2 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta_2");
      TH2D *h_xz_pos_sim_incrad_3 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incrad_3 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_rad");
      TH2D *h_xz_pos_sim_incnorad_3 = new TH2D("","pi+ sim SIDIS",bins,0,1,bins,0,1);
      h_xz_pos_sim_incnorad_3 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_norad_3");
      TH2D *h_xz_pos_sim_excrad_3 = new TH2D("","pi+ sim exc",bins,0,1,bins,0,1);
      h_xz_pos_sim_excrad_3 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_exc_rad_3");
      TH2D *h_xz_pos_sim_delta_3 = new TH2D("","pi+ sim delta",bins,0,1,bins,0,1);
      h_xz_pos_sim_delta_3 = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta_3");
      for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        charge_neg_all += charge;
        TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
        TH2D *h_xz_neg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_1 = (TH2D*)root_file_neg->Get("x_z_1");
        TH2D *h_xz_neg_bg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_bg_1 = (TH2D*)root_file_neg->Get("x_z_bg_1");
        double EFF = Get_all_eff(RunNumber);
        h_xz_neg_bg_all_1->Add(h_xz_neg_bg_1,1/(6*EFF));
        h_xz_neg_all_1->Add(h_xz_neg_1,1/EFF);

        TH2D *h_xz_negraw_1 = (TH2D*)root_file_neg->Get("x_z_raw1");
        h_xz_neg_allraw_1->Add(h_xz_negraw_1);

        TH2D* h_xz_xbjmean_neg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_neg_1 = (TH2D*)root_file_neg->Get("weighted_xbj_1");
        TH2D* h_xz_zmean_neg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_neg_1 = (TH2D*)root_file_neg->Get("weighted_z_1");
        h_xz_xbjmean_neg_all_1->Add(h_xz_xbjmean_neg_1);
        h_xz_zmean_neg_all_1->Add(h_xz_zmean_neg_1);
        TH2D* h_xz_xbjmean2_neg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_neg_1 = (TH2D*)root_file_neg->Get("weighted_xbj2_1");
        TH2D* h_xz_zmean2_neg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_neg_1 = (TH2D*)root_file_neg->Get("weighted_z2_1");
        h_xz_xbjmean2_neg_all_1->Add(h_xz_xbjmean2_neg_1);
        h_xz_zmean2_neg_all_1->Add(h_xz_zmean2_neg_1);

        TH2D *h_xz_neg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_2 = (TH2D*)root_file_neg->Get("x_z_2");
        TH2D *h_xz_neg_bg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_bg_2 = (TH2D*)root_file_neg->Get("x_z_bg_2");
        h_xz_neg_bg_all_2->Add(h_xz_neg_bg_2,1/(6*EFF));
        h_xz_neg_all_2->Add(h_xz_neg_2,1/EFF);
        
        TH2D *h_xz_negraw_2 = (TH2D*)root_file_neg->Get("x_z_raw2");
        h_xz_neg_allraw_2->Add(h_xz_negraw_2);

        TH2D* h_xz_xbjmean_neg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_neg_2 = (TH2D*)root_file_neg->Get("weighted_xbj_2");
        TH2D* h_xz_zmean_neg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_neg_2 = (TH2D*)root_file_neg->Get("weighted_z_2");
        h_xz_xbjmean_neg_all_2->Add(h_xz_xbjmean_neg_2);
        h_xz_zmean_neg_all_2->Add(h_xz_zmean_neg_2);
        TH2D* h_xz_xbjmean2_neg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_neg_2 = (TH2D*)root_file_neg->Get("weighted_xbj2_2");
        TH2D* h_xz_zmean2_neg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_neg_2 = (TH2D*)root_file_neg->Get("weighted_z2_2");
        h_xz_xbjmean2_neg_all_2->Add(h_xz_xbjmean2_neg_2);
        h_xz_zmean2_neg_all_2->Add(h_xz_zmean2_neg_2);

        TH2D *h_xz_neg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_3 = (TH2D*)root_file_neg->Get("x_z_3");
        TH2D *h_xz_neg_bg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_neg_bg_3 = (TH2D*)root_file_neg->Get("x_z_bg_3");
        h_xz_neg_bg_all_3->Add(h_xz_neg_bg_3,1/(6*EFF));
        h_xz_neg_all_3->Add(h_xz_neg_3,1/EFF);
        
        TH2D *h_xz_negraw_3 = (TH2D*)root_file_neg->Get("x_z_raw3");
        h_xz_neg_allraw_3->Add(h_xz_negraw_3);

        TH2D* h_xz_xbjmean_neg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_neg_3 = (TH2D*)root_file_neg->Get("weighted_xbj_3");
        TH2D* h_xz_zmean_neg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_neg_3 = (TH2D*)root_file_neg->Get("weighted_z_3");
        h_xz_xbjmean_neg_all_3->Add(h_xz_xbjmean_neg_3);
        h_xz_zmean_neg_all_3->Add(h_xz_zmean_neg_3);
        TH2D* h_xz_xbjmean2_neg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_neg_3 = (TH2D*)root_file_neg->Get("weighted_xbj2_3");
        TH2D* h_xz_zmean2_neg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_neg_3 = (TH2D*)root_file_neg->Get("weighted_z2_3");
        h_xz_xbjmean2_neg_all_3->Add(h_xz_xbjmean2_neg_3);
        h_xz_zmean2_neg_all_3->Add(h_xz_zmean2_neg_3);
      }//loop over neg runs
      h_xz_xbjmean_neg_all_1->Divide(h_xz_neg_allraw_1);
      h_xz_zmean_neg_all_1->Divide(h_xz_neg_allraw_1);
      h_xz_xbjmean2_neg_all_1->Divide(h_xz_neg_allraw_1);
      h_xz_zmean2_neg_all_1->Divide(h_xz_neg_allraw_1);
      h_xz_xbjmean_neg_all_2->Divide(h_xz_neg_allraw_2);
      h_xz_zmean_neg_all_2->Divide(h_xz_neg_allraw_2);
      h_xz_xbjmean2_neg_all_2->Divide(h_xz_neg_allraw_2);
      h_xz_zmean2_neg_all_2->Divide(h_xz_neg_allraw_2);
      h_xz_xbjmean_neg_all_3->Divide(h_xz_neg_allraw_3);
      h_xz_zmean_neg_all_3->Divide(h_xz_neg_allraw_3);
      h_xz_xbjmean2_neg_all_3->Divide(h_xz_neg_allraw_3);
      h_xz_zmean2_neg_all_3->Divide(h_xz_neg_allraw_3);

      for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        charge_pos_all+=charge;
        //std::cout<<"pos DE check"<<std::endl;
        TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
        TH2D *h_xz_pos_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_1 = (TH2D*)root_file_pos->Get("x_z_1");
        TH2D *h_xz_pos_bg_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_bg_1 = (TH2D*)root_file_pos->Get("x_z_bg_1");
        double EFF = Get_all_eff(RunNumber);
        h_xz_pos_bg_all_1->Add(h_xz_pos_bg_1,1/(6*EFF));
        h_xz_pos_all_1->Add(h_xz_pos_1,1/EFF);
        TH2D *h_xz_posraw_1 = (TH2D*)root_file_pos->Get("x_z_raw1");
        h_xz_pos_allraw_1->Add(h_xz_posraw_1);

        TH2D* h_xz_xbjmean_pos_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_pos_1 = (TH2D*)root_file_pos->Get("weighted_xbj_1");
        TH2D* h_xz_zmean_pos_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_pos_1 = (TH2D*)root_file_pos->Get("weighted_z_1");
        h_xz_xbjmean_pos_all_1->Add(h_xz_xbjmean_pos_1);
        h_xz_zmean_pos_all_1->Add(h_xz_zmean_pos_1);
        TH2D* h_xz_xbjmean2_pos_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_pos_1 = (TH2D*)root_file_pos->Get("weighted_xbj2_1");
        TH2D* h_xz_zmean2_pos_1 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_pos_1 = (TH2D*)root_file_pos->Get("weighted_z2_1");
        h_xz_xbjmean2_pos_all_1->Add(h_xz_xbjmean2_pos_1);
        h_xz_zmean2_pos_all_1->Add(h_xz_zmean2_pos_1);

        TH2D *h_xz_pos_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_2 = (TH2D*)root_file_pos->Get("x_z_2");
        TH2D *h_xz_pos_bg_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_bg_2 = (TH2D*)root_file_pos->Get("x_z_bg_2");
        h_xz_pos_bg_all_2->Add(h_xz_pos_bg_2,1/(6*EFF));
        h_xz_pos_all_2->Add(h_xz_pos_2,1/EFF);
        TH2D *h_xz_posraw_2 = (TH2D*)root_file_pos->Get("x_z_raw2");
        h_xz_pos_allraw_2->Add(h_xz_posraw_2);

        TH2D* h_xz_xbjmean_pos_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_pos_2 = (TH2D*)root_file_pos->Get("weighted_xbj_2");
        TH2D* h_xz_zmean_pos_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_pos_2 = (TH2D*)root_file_pos->Get("weighted_z_2");
        h_xz_xbjmean_pos_all_2->Add(h_xz_xbjmean_pos_2);
        h_xz_zmean_pos_all_2->Add(h_xz_zmean_pos_2);
        TH2D* h_xz_xbjmean2_pos_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_pos_2 = (TH2D*)root_file_pos->Get("weighted_xbj2_2");
        TH2D* h_xz_zmean2_pos_2 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_pos_2 = (TH2D*)root_file_pos->Get("weighted_z2_2");
        h_xz_xbjmean2_pos_all_2->Add(h_xz_xbjmean2_pos_2);
        h_xz_zmean2_pos_all_2->Add(h_xz_zmean2_pos_2);

        TH2D *h_xz_pos_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_3 = (TH2D*)root_file_pos->Get("x_z_3");
        TH2D *h_xz_pos_bg_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_pos_bg_3 = (TH2D*)root_file_pos->Get("x_z_bg_3");
        h_xz_pos_bg_all_3->Add(h_xz_pos_bg_3,1/(6*EFF));
        h_xz_pos_all_3->Add(h_xz_pos_3,1/EFF);
        
        TH2D *h_xz_posraw_3 = (TH2D*)root_file_pos->Get("x_z_raw3");
        h_xz_pos_allraw_3->Add(h_xz_posraw_3);

        TH2D* h_xz_xbjmean_pos_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean_pos_3 = (TH2D*)root_file_pos->Get("weighted_xbj_3");
        TH2D* h_xz_zmean_pos_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean_pos_3 = (TH2D*)root_file_pos->Get("weighted_z_3");
        h_xz_xbjmean_pos_all_3->Add(h_xz_xbjmean_pos_3);
        h_xz_zmean_pos_all_3->Add(h_xz_zmean_pos_3);
        TH2D* h_xz_xbjmean2_pos_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_xbjmean2_pos_3 = (TH2D*)root_file_pos->Get("weighted_xbj2_3");
        TH2D* h_xz_zmean2_pos_3 = new TH2D("","",20,0,1,20,0,1);
        h_xz_zmean2_pos_3 = (TH2D*)root_file_pos->Get("weighted_z2_3");
        h_xz_xbjmean2_pos_all_3->Add(h_xz_xbjmean2_pos_3);
        h_xz_zmean2_pos_all_3->Add(h_xz_zmean2_pos_3);

      }//loop over pos runs
      h_xz_xbjmean_pos_all_1->Divide(h_xz_pos_allraw_1);
      h_xz_zmean_pos_all_1->Divide(h_xz_pos_allraw_1);
      h_xz_xbjmean2_pos_all_1->Divide(h_xz_pos_allraw_1);
      h_xz_zmean2_pos_all_1->Divide(h_xz_pos_allraw_1);
      h_xz_xbjmean_pos_all_2->Divide(h_xz_pos_allraw_2);
      h_xz_zmean_pos_all_2->Divide(h_xz_pos_allraw_2);
      h_xz_xbjmean2_pos_all_2->Divide(h_xz_pos_allraw_2);
      h_xz_zmean2_pos_all_2->Divide(h_xz_pos_allraw_2);
      h_xz_xbjmean_pos_all_3->Divide(h_xz_pos_allraw_3);
      h_xz_zmean_pos_all_3->Divide(h_xz_pos_allraw_3);
      h_xz_xbjmean2_pos_all_3->Divide(h_xz_pos_allraw_3);
      h_xz_zmean2_pos_all_3->Divide(h_xz_pos_allraw_3);

      for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
        int RunNumber = *it;
        //std::cout<<"Dummy"<<RunNumber<<std::endl;
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        charge_neg_Dummy_all += charge;
        TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
        TH2D *h_xz_neg_1 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_1 = (TH2D*)root_file_neg->Get("x_z_1");
        TH2D *h_xz_neg_bg_1 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_bg_1 = (TH2D*)root_file_neg->Get("x_z_bg_1");
        double EFF = Get_all_eff(RunNumber);
        h_xz_neg_bg_Dummy_all_1->Add(h_xz_neg_bg_1,1/(6*EFF));
        h_xz_neg_Dummy_all_1->Add(h_xz_neg_1,1/EFF);
        TH2D *h_xz_neg_2 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_2 = (TH2D*)root_file_neg->Get("x_z_2");
        TH2D *h_xz_neg_bg_2 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_bg_2 = (TH2D*)root_file_neg->Get("x_z_bg_2");
        h_xz_neg_bg_Dummy_all_2->Add(h_xz_neg_bg_2,1/(6*EFF));
        h_xz_neg_Dummy_all_2->Add(h_xz_neg_2,1/EFF);
        TH2D *h_xz_neg_3 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_3 = (TH2D*)root_file_neg->Get("x_z_3");
        TH2D *h_xz_neg_bg_3 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_neg_bg_3 = (TH2D*)root_file_neg->Get("x_z_bg_3");
        h_xz_neg_bg_Dummy_all_3->Add(h_xz_neg_bg_3,1/(6*EFF));
        h_xz_neg_Dummy_all_3->Add(h_xz_neg_3,1/EFF);
      }//loop over neg runs
      for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
        int RunNumber = *it;
        //std::cout<<"Dummy "<<RunNumber<<std::endl;
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        charge_pos_Dummy_all+=charge;
        TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
        TH2D *h_xz_pos_1 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_1 = (TH2D*)root_file_pos->Get("x_z_1");
        TH2D *h_xz_pos_bg_1 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_bg_1 = (TH2D*)root_file_pos->Get("x_z_bg_1");
        double EFF = Get_all_eff(RunNumber);
        h_xz_pos_bg_Dummy_all_1->Add(h_xz_pos_bg_1,1/(6*EFF));
        h_xz_pos_Dummy_all_1->Add(h_xz_pos_1,1/EFF);
        TH2D *h_xz_pos_2 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_2 = (TH2D*)root_file_pos->Get("x_z_2");
        TH2D *h_xz_pos_bg_2 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_bg_2 = (TH2D*)root_file_pos->Get("x_z_bg_2");
        h_xz_pos_bg_Dummy_all_2->Add(h_xz_pos_bg_2,1/(6*EFF));
        h_xz_pos_Dummy_all_2->Add(h_xz_pos_2,1/EFF);
        TH2D *h_xz_pos_3 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_3 = (TH2D*)root_file_pos->Get("x_z_3");
        TH2D *h_xz_pos_bg_3 = new TH2D("","",bins,0,1,bins,0,1);
        h_xz_pos_bg_3 = (TH2D*)root_file_pos->Get("x_z_bg_3");
        h_xz_pos_bg_Dummy_all_3->Add(h_xz_pos_bg_3,1/(6*EFF));
        h_xz_pos_Dummy_all_3->Add(h_xz_pos_3,1/EFF);
      }//loop over pos runs


      //Dummy bg subtract
      h_xz_neg_Dummy_all_1->Add(h_xz_neg_bg_Dummy_all_1,-1.0);
      h_xz_pos_Dummy_all_1->Add(h_xz_pos_bg_Dummy_all_1,-1.0);
      h_xz_neg_Dummy_all_2->Add(h_xz_neg_bg_Dummy_all_2,-1.0);
      h_xz_pos_Dummy_all_2->Add(h_xz_pos_bg_Dummy_all_2,-1.0);
      h_xz_neg_Dummy_all_3->Add(h_xz_neg_bg_Dummy_all_3,-1.0);
      h_xz_pos_Dummy_all_3->Add(h_xz_pos_bg_Dummy_all_3,-1.0);


      auto h_xz_neg_incnorad_1 = h_xz_neg_sim_incnorad_1;
      auto h_xz_neg_incrad_1 = h_xz_neg_sim_incrad_1;
      auto h_xz_pos_incnorad_1 = h_xz_pos_sim_incnorad_1;
      auto h_xz_pos_incrad_1 = h_xz_pos_sim_incrad_1;
      auto h_xz_neg_incnorad_2 = h_xz_neg_sim_incnorad_2;
      auto h_xz_neg_incrad_2 = h_xz_neg_sim_incrad_2;
      auto h_xz_pos_incnorad_2 = h_xz_pos_sim_incnorad_2;
      auto h_xz_pos_incrad_2 = h_xz_pos_sim_incrad_2;
      auto h_xz_neg_incnorad_3 = h_xz_neg_sim_incnorad_3;
      auto h_xz_neg_incrad_3 = h_xz_neg_sim_incrad_3;
      auto h_xz_pos_incnorad_3 = h_xz_pos_sim_incnorad_3;
      auto h_xz_pos_incrad_3 = h_xz_pos_sim_incrad_3;

      TH2* rp_radia_corr_neg_1 = (TH2*)h_xz_neg_incnorad_1->Clone("rp_radia_corr_1");
      TH2* rp_radia_corr_pos_1 = (TH2*)h_xz_pos_incnorad_1->Clone("rp_radia_corr_1");
      //std::cout<<" bins "<<h_xz_neg_incnorad_1->GetXaxis()->GetNbins()<<" "<<h_xz_neg_incrad_1->GetXaxis()->GetNbins()<<std::endl;
      //TH2* rp_radia_corr_neg_1 = new TH2D(); 
      //TH2* rp_radia_corr_pos_1 = new TH2D();
      rp_radia_corr_neg_1 ->Divide(h_xz_neg_incnorad_1,h_xz_neg_incrad_1);          
      rp_radia_corr_pos_1 ->Divide(h_xz_pos_incnorad_1,h_xz_pos_incrad_1);          
      TH2* rp_radia_corr_neg_2 = (TH2*)h_xz_neg_incnorad_2->Clone("rp_radia_corr_2");
      TH2* rp_radia_corr_pos_2 = (TH2*)h_xz_pos_incnorad_2->Clone("rp_radia_corr_2");
      //TH2* rp_radia_corr_neg_2 = new TH2D(); 
      //TH2* rp_radia_corr_pos_2 = new TH2D();
      //std::cout<<" bins "<<h_xz_neg_incnorad_2->GetXaxis()->GetNbins()<<" "<<h_xz_neg_incrad_2->GetXaxis()->GetNbins()<<std::endl;
      rp_radia_corr_neg_2 ->Divide(h_xz_neg_incnorad_2,h_xz_neg_incrad_2);          
      rp_radia_corr_pos_2 ->Divide(h_xz_pos_incnorad_2,h_xz_pos_incrad_2);          
      TH2* rp_radia_corr_neg_3 = (TH2*)h_xz_neg_incnorad_3->Clone("rp_radia_corr_3");
      TH2* rp_radia_corr_pos_3 = (TH2*)h_xz_pos_incnorad_3->Clone("rp_radia_corr_3");
      //std::cout<<" bins "<<h_xz_neg_incnorad_3->GetXaxis()->GetNbins()<<" "<<h_xz_neg_incrad_3->GetXaxis()->GetNbins()<<std::endl;
      //TH2* rp_radia_corr_neg_3 = new TH2D(); 
      //TH2* rp_radia_corr_pos_3 = new TH2D();
      rp_radia_corr_neg_3 ->Divide(h_xz_neg_incnorad_3,h_xz_neg_incrad_3);          
      rp_radia_corr_pos_3 ->Divide(h_xz_pos_incnorad_3,h_xz_pos_incrad_3);          

      //std::cout<<"ratio check"<<std::endl;
      int xnbins_1 = h_xz_pos_all_1->GetXaxis()->GetNbins();
      int ynbins_1 = h_xz_pos_all_1->GetYaxis()->GetNbins();
      //std::cout<<"xbins "<<xnbins_1<<" ybins "<<ynbins_1<<std::endl;
      int xnbins_2 = h_xz_pos_all_2->GetXaxis()->GetNbins();
      int ynbins_2 = h_xz_pos_all_2->GetYaxis()->GetNbins();
      //std::cout<<"xbins "<<xnbins_2<<" ybins "<<ynbins_2<<std::endl;
      int xnbins_3 = h_xz_pos_all_3->GetXaxis()->GetNbins();
      int ynbins_3 = h_xz_pos_all_3->GetYaxis()->GetNbins();
      //std::cout<<"xbins "<<xnbins_3<<" ybins "<<ynbins_3<<std::endl;


      for(int i = 0;i<xnbins_1;i++){
        for(int j = 0;j<ynbins_1;j++){
          //std::cout<<i<<std::endl;
          double x_1 = h_xz_neg_all_1->GetXaxis()->GetBinCenter(i+1);
          double y_1 = h_xz_neg_all_1->GetYaxis()->GetBinCenter(j+1);
          double yield_neg_D2_1 = h_xz_neg_all_1->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_bg_D2_1 = h_xz_neg_bg_all_1->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_Dummy_1 = h_xz_neg_Dummy_all_1->GetBinContent(i+1,j+1)/charge_neg_Dummy_all;
          double yield_neg_delta_1 = h_xz_neg_sim_delta_1->GetBinContent(i+1,j+1);
          double yield_neg_exc_1 = h_xz_neg_sim_excrad_1->GetBinContent(i+1,j+1);
          double yield_neg_1 = yield_neg_D2_1-yield_neg_bg_D2_1-0.245*yield_neg_Dummy_1-yield_neg_delta_1-yield_neg_exc_1;
          //std::cout<<" z "<<x_1<<" xbj "<<y_1<<" yield neg 1 "<<yield_neg_1<<std::endl;
          double error_neg_D2_1 = h_xz_neg_all_1->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_bg_D2_1 = h_xz_neg_bg_all_1->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_Dummy_1 = h_xz_neg_Dummy_all_1->GetBinError(i+1,j+1)/charge_neg_Dummy_all;
          double error_neg_1 = std::sqrt(error_neg_D2_1*error_neg_D2_1+error_neg_bg_D2_1*error_neg_bg_D2_1+0.245*0.245*error_neg_Dummy_1*error_neg_Dummy_1);
          double yield_pos_D2_1 = h_xz_pos_all_1->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_bg_D2_1 = h_xz_pos_bg_all_1->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_Dummy_1 = h_xz_pos_Dummy_all_1->GetBinContent(i+1,j+1)/charge_pos_Dummy_all;
          double yield_pos_delta_1 = h_xz_pos_sim_delta_1->GetBinContent(i+1,j+1);
          double yield_pos_exc_1 = h_xz_pos_sim_excrad_1->GetBinContent(i+1,j+1);
          double yield_pos_1 = yield_pos_D2_1-yield_pos_bg_D2_1-0.245*yield_pos_Dummy_1-yield_pos_delta_1-yield_pos_exc_1;
          double error_pos_D2_1 = h_xz_pos_all_1->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_bg_D2_1 = h_xz_pos_bg_all_1->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_Dummy_1 = h_xz_pos_Dummy_all_1->GetBinError(i+1,j+1)/charge_pos_Dummy_all;
          double error_pos_1 = std::sqrt(error_pos_D2_1*error_pos_D2_1+error_pos_bg_D2_1*error_pos_bg_D2_1+0.245*0.245*error_pos_Dummy_1*error_pos_Dummy_1);

          //for yield
          double radia_corr_neg_1 = rp_radia_corr_neg_1->GetBinContent(i+1,j+1);
          double radia_corr_pos_1 = rp_radia_corr_pos_1->GetBinContent(i+1,j+1);
          //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
          yield_neg_1 = yield_neg_1*radia_corr_neg_1;
          yield_pos_1 = yield_pos_1*radia_corr_pos_1;
          error_pos_1 = error_pos_1*radia_corr_pos_1;
          error_neg_1 = error_neg_1*radia_corr_neg_1;

          //need to think of how to convert to xs


          double RY_1 = (yield_neg_1)/(yield_pos_1);
          //std::cout<<"RY "<<RY<<std::endl;
          double error_1 = RY_1*std::sqrt((error_neg_1*error_neg_1)/(yield_neg_1*yield_neg_1)+(error_pos_1*error_pos_1)/(yield_pos_1*yield_pos_1));
          double y_RD_1 = (4*RY_1-1)/(1-RY_1);
          //double error_RD = error;
          double error_RD_1 = 3*error_1/((1-RY_1)*(1-RY_1));
          //for bin center correction
          double xbj_neg_corr_1 = h_xz_xbjmean_neg_all_1->GetBinContent(i+1,j+1);
          double z_neg_corr_1 = h_xz_zmean_neg_all_1->GetBinContent(i+1,j+1);
          double xbj_pos_corr_1 = h_xz_xbjmean_pos_all_1->GetBinContent(i+1,j+1);
          double z_pos_corr_1 = h_xz_zmean_pos_all_1->GetBinContent(i+1,j+1);

          double xbj_neg_corr_err2_1 = h_xz_xbjmean2_neg_all_1->GetBinContent(i+1,j+1)-xbj_neg_corr_1*xbj_neg_corr_1;
          double z_neg_corr_err2_1 = h_xz_zmean2_neg_all_1->GetBinContent(i+1,j+1)-z_neg_corr_1*z_neg_corr_1;
          double xbj_pos_corr_err2_1 = h_xz_xbjmean2_pos_all_1->GetBinContent(i+1,j+1)-xbj_pos_corr_1*xbj_pos_corr_1;
          double z_pos_corr_err2_1 = h_xz_zmean2_pos_all_1->GetBinContent(i+1,j+1)-z_pos_corr_1*z_pos_corr_1;

          double xbj_neg_corr_err_1 = sqrt(xbj_neg_corr_err2_1);
          double xbj_pos_corr_err_1 = sqrt(xbj_pos_corr_err2_1);
          double z_neg_corr_err_1 = sqrt(z_neg_corr_err2_1);
          double z_pos_corr_err_1 = sqrt(z_pos_corr_err2_1);

          double xbj_corr_1 = (xbj_neg_corr_1/xbj_neg_corr_err2_1+xbj_pos_corr_1/xbj_pos_corr_err2_1)/(1/xbj_neg_corr_err2_1+1/xbj_pos_corr_err2_1);
          if(abs(xbj_corr_1-y_1)>0.025){
            std::cout<<"1 xbj center "<<y_1<<", xbj corr "<<xbj_corr_1<<", pi+ "<<xbj_pos_corr_1<<" +- "<<xbj_pos_corr_err_1<<", pi- "<<xbj_neg_corr_1<<" +- "<<xbj_neg_corr_err_1<<std::endl;
          }
          double xbj_corr_err_1 = 1/(1/xbj_neg_corr_err2_1+1/xbj_pos_corr_err2_1);
          double z_corr_1 = (z_neg_corr_1/z_neg_corr_err2_1+z_pos_corr_1/z_pos_corr_err2_1)/(1/z_neg_corr_err2_1+1/z_pos_corr_err2_1);
          double z_corr_err_1 = 1/(1/z_neg_corr_err2_1+1/z_pos_corr_err2_1);

          if(RY_1>0){
            //double y_RD = (4*y-1)/(1-y);
            std::string xbj_str = std::to_string(y_1); 
            std::string z_str = std::to_string(x_1);

            if(y_RD_1>0 && y_RD_1<10 && error_RD_1<20 && error_RD_1>0){
              //std::cout<<"check "<<y_RD_1<<std::endl;
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["value"].push_back(y_RD_1);
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["error"].push_back(error_RD_1);
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["yield_neg"].push_back(yield_neg_1);
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["yield_pos"].push_back(yield_pos_1);
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["charge_neg"].push_back(charge_neg_all);
              jout["Q2_1"][xbj_str.c_str()][z_str.c_str()]["charge_pos"].push_back(charge_pos_all);
              jout_2["Q2_1"][xbj_str.c_str()][(std::to_string(RunGroup)).c_str()][z_str.c_str()]["value"] = y_RD_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error"] = error_RD_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg"] = yield_neg_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos"] = yield_pos_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_neg"] = charge_neg_all;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_pos"] = charge_pos_all;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_neg_corr"] = xbj_neg_corr_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_neg_corr"] = z_neg_corr_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_pos_corr"] = xbj_pos_corr_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_pos_corr"] = z_pos_corr_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr"] = xbj_corr_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_err"] = xbj_corr_err_1;
              jout_2["Q2_1"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_corr"] = z_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_1;
              jout_3["Q2_1"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_1;
              jout_4["Q2_1"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["value"] = y_RD_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error"] = error_RD_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg"] = yield_neg_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos"] = yield_pos_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_neg"] = charge_neg_all;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_pos"] = charge_pos_all;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_neg_corr"] = xbj_neg_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_neg_corr"] = z_neg_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_pos_corr"] = xbj_pos_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_pos_corr"] = z_pos_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr"] = xbj_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr_err"] = xbj_corr_err_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr"] = z_corr_1;
              jout_5["Q2_1"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr_err"] = z_corr_err_1;


            }
            else{
              jout_sthwrong["Q2_1"][xbj_str][z_str]["value"] = y_RD_1;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["error"] = error_RD_1;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["yield_neg"] = yield_neg_1;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["yield_pos"] = yield_pos_1;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["charge_neg"] = charge_neg_all;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["charge_pos"] = charge_pos_all;
              jout_sthwrong["Q2_1"][xbj_str][z_str]["rungroup"] = RunGroup;

            }
          }//if RY reasonable
        }//loop y axis for 1
      }//loop x axis for 1

      for(int i = 0;i<xnbins_2;i++){
        for(int j = 0;j<ynbins_2;j++){
          //std::cout<<i<<std::endl;
          double x_2 = h_xz_neg_all_2->GetXaxis()->GetBinCenter(i+1);
          double y_2 = h_xz_neg_all_2->GetYaxis()->GetBinCenter(j+1);
          double yield_neg_D2_2 = h_xz_neg_all_2->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_bg_D2_2 = h_xz_neg_bg_all_2->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_Dummy_2 = h_xz_neg_Dummy_all_2->GetBinContent(i+1,j+1)/charge_neg_Dummy_all;
          double yield_neg_delta_2 = h_xz_neg_sim_delta_2->GetBinContent(i+1,j+1);
          double yield_neg_exc_2 = h_xz_neg_sim_excrad_2->GetBinContent(i+1,j+1);
          double yield_neg_2 = yield_neg_D2_2-yield_neg_bg_D2_2-0.245*yield_neg_Dummy_2-yield_neg_delta_2-yield_neg_exc_2;
          //std::cout<<" z "<<x_2<<" xbj "<<y_2<<" yield neg 1 "<<yield_neg_2<<std::endl;
          double error_neg_D2_2 = h_xz_neg_all_2->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_bg_D2_2 = h_xz_neg_bg_all_2->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_Dummy_2 = h_xz_neg_Dummy_all_2->GetBinError(i+1,j+1)/charge_neg_Dummy_all;
          double error_neg_2 = std::sqrt(error_neg_D2_2*error_neg_D2_2+error_neg_bg_D2_2*error_neg_bg_D2_2+0.245*0.245*error_neg_Dummy_2*error_neg_Dummy_2);
          double yield_pos_D2_2 = h_xz_pos_all_2->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_bg_D2_2 = h_xz_pos_bg_all_2->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_Dummy_2 = h_xz_pos_Dummy_all_2->GetBinContent(i+1,j+1)/charge_pos_Dummy_all;
          double yield_pos_delta_2 = h_xz_pos_sim_delta_2->GetBinContent(i+1,j+1);
          double yield_pos_exc_2 = h_xz_pos_sim_excrad_2->GetBinContent(i+1,j+1);
          double yield_pos_2 = yield_pos_D2_2-yield_pos_bg_D2_2-0.245*yield_pos_Dummy_2-yield_pos_delta_2-yield_pos_exc_2;
          double error_pos_D2_2 = h_xz_pos_all_2->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_bg_D2_2 = h_xz_pos_bg_all_2->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_Dummy_2 = h_xz_pos_Dummy_all_2->GetBinError(i+1,j+1)/charge_pos_Dummy_all;
          double error_pos_2 = std::sqrt(error_pos_D2_2*error_pos_D2_2+error_pos_bg_D2_2*error_pos_bg_D2_2+0.245*0.245*error_pos_Dummy_2*error_pos_Dummy_2);

          //for yield
          double radia_corr_neg_2 = rp_radia_corr_neg_2->GetBinContent(i+1,j+1);
          double radia_corr_pos_2 = rp_radia_corr_pos_2->GetBinContent(i+1,j+1);
          //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
          yield_neg_2 = yield_neg_2*radia_corr_neg_2;
          yield_pos_2 = yield_pos_2*radia_corr_pos_2;
          error_pos_2 = error_pos_2*radia_corr_pos_2;
          error_neg_2 = error_neg_2*radia_corr_neg_2;

          //need to think of how to convert to xs


          double RY_2 = (yield_neg_2)/(yield_pos_2);
          //std::cout<<"RY "<<RY<<std::endl;
          double error_2 = RY_2*std::sqrt((error_neg_2*error_neg_2)/(yield_neg_2*yield_neg_2)+(error_pos_2*error_pos_2)/(yield_pos_2*yield_pos_2));
          double y_RD_2 = (4*RY_2-1)/(1-RY_2);
          //double error_RD = error;
          double error_RD_2 = 3*error_2/((1-RY_2)*(1-RY_2));
          //for bin center correction
          double xbj_neg_corr_2 = h_xz_xbjmean_neg_all_2->GetBinContent(i+1,j+1);
          double z_neg_corr_2 = h_xz_zmean_neg_all_2->GetBinContent(i+1,j+1);
          double xbj_pos_corr_2 = h_xz_xbjmean_pos_all_2->GetBinContent(i+1,j+1);
          double z_pos_corr_2 = h_xz_zmean_pos_all_2->GetBinContent(i+1,j+1);

          double xbj_neg_corr_err2_2 = h_xz_xbjmean2_neg_all_2->GetBinContent(i+1,j+1)-xbj_neg_corr_2*xbj_neg_corr_2;
          double z_neg_corr_err2_2 = h_xz_zmean2_neg_all_2->GetBinContent(i+1,j+1)-z_neg_corr_2*z_neg_corr_2;
          double xbj_pos_corr_err2_2 = h_xz_xbjmean2_pos_all_2->GetBinContent(i+1,j+1)-xbj_pos_corr_2*xbj_pos_corr_2;
          double z_pos_corr_err2_2 = h_xz_zmean2_pos_all_2->GetBinContent(i+1,j+1)-z_pos_corr_2*z_pos_corr_2;

          double xbj_neg_corr_err_2 = sqrt(xbj_neg_corr_err2_2);
          double xbj_pos_corr_err_2 = sqrt(xbj_pos_corr_err2_2);
          double z_neg_corr_err_2 = sqrt(z_neg_corr_err2_2);
          double z_pos_corr_err_2 = sqrt(z_pos_corr_err2_2);

          double xbj_corr_2 = (xbj_neg_corr_2/xbj_neg_corr_err2_2+xbj_pos_corr_2/xbj_pos_corr_err2_2)/(1/xbj_neg_corr_err2_2+1/xbj_pos_corr_err2_2);
          if(abs(xbj_corr_2-y_2)>0.025){
            std::cout<<"2 xbj center "<<y_2<<", xbj corr "<<xbj_corr_2<<", pi+ "<<xbj_pos_corr_2<<" +- "<<xbj_pos_corr_err_2<<", pi- "<<xbj_neg_corr_2<<" +- "<<xbj_neg_corr_err_2<<std::endl;
          }
          double xbj_corr_err_2 = 1/(1/xbj_neg_corr_err2_2+1/xbj_pos_corr_err2_2);
          double z_corr_2 = (z_neg_corr_2/z_neg_corr_err2_2+z_pos_corr_2/z_pos_corr_err2_2)/(1/z_neg_corr_err2_2+1/z_pos_corr_err2_2);
          double z_corr_err_2 = 1/(1/z_neg_corr_err2_2+1/z_pos_corr_err2_2);

          if(RY_2>0 && yield_neg_2>0.005){
            //double y_RD = (4*y-1)/(1-y);
            std::string xbj_str = std::to_string(y_2); 
            std::string z_str = std::to_string(x_2);

            if(y_RD_2>0 && y_RD_2<10 && error_RD_2<20 && error_RD_2>0){
              //std::cout<<"check "<<y_RD_2<<std::endl;
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["value"].push_back(y_RD_2);
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["error"].push_back(error_RD_2);
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["yield_neg"].push_back(yield_neg_2);
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["yield_pos"].push_back(yield_pos_2);
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["charge_neg"].push_back(charge_neg_all);
              jout["Q2_2"][xbj_str.c_str()][z_str.c_str()]["charge_pos"].push_back(charge_pos_all);
              jout_2["Q2_2"][xbj_str.c_str()][(std::to_string(RunGroup)).c_str()][z_str.c_str()]["value"] = y_RD_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error"] = error_RD_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg"] = yield_neg_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos"] = yield_pos_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_neg"] = charge_neg_all;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_pos"] = charge_pos_all;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_neg_corr"] = xbj_neg_corr_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_neg_corr"] = z_neg_corr_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_pos_corr"] = xbj_pos_corr_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_pos_corr"] = z_pos_corr_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr"] = xbj_corr_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_err"] = xbj_corr_err_2;
              jout_2["Q2_2"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_corr"] = z_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_2;
              jout_3["Q2_2"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_2;
              jout_4["Q2_2"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["value"] = y_RD_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error"] = error_RD_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg"] = yield_neg_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos"] = yield_pos_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_neg"] = charge_neg_all;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_pos"] = charge_pos_all;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_neg_corr"] = xbj_neg_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_neg_corr"] = z_neg_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_pos_corr"] = xbj_pos_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_pos_corr"] = z_pos_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr"] = xbj_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr_err"] = xbj_corr_err_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr"] = z_corr_2;
              jout_5["Q2_2"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr_err"] = z_corr_err_2;


            }
            else{
              jout_sthwrong["Q2_2"][xbj_str][z_str]["value"] = y_RD_2;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["error"] = error_RD_2;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["yield_neg"] = yield_neg_2;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["yield_pos"] = yield_pos_2;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["charge_neg"] = charge_neg_all;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["charge_pos"] = charge_pos_all;
              jout_sthwrong["Q2_2"][xbj_str][z_str]["rungroup"] = RunGroup;

            }
          }//if RY reasonable
        }//loop y axis for 2
      }//loop x axis for 2
      
      for(int i = 0;i<xnbins_3;i++){
        for(int j = 0;j<ynbins_3;j++){
          //std::cout<<i<<std::endl;
          double x_3 = h_xz_neg_all_3->GetXaxis()->GetBinCenter(i+1);
          double y_3 = h_xz_neg_all_3->GetYaxis()->GetBinCenter(j+1);
          double yield_neg_D2_3 = h_xz_neg_all_3->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_bg_D2_3 = h_xz_neg_bg_all_3->GetBinContent(i+1,j+1)/charge_neg_all;
          double yield_neg_Dummy_3 = h_xz_neg_Dummy_all_3->GetBinContent(i+1,j+1)/charge_neg_Dummy_all;
          double yield_neg_delta_3 = h_xz_neg_sim_delta_3->GetBinContent(i+1,j+1);
          double yield_neg_exc_3 = h_xz_neg_sim_excrad_3->GetBinContent(i+1,j+1);
          double yield_neg_3 = yield_neg_D2_3-yield_neg_bg_D2_3-0.245*yield_neg_Dummy_3-yield_neg_delta_3-yield_neg_exc_3;
          //std::cout<<" z "<<x_3<<" xbj "<<y_3<<" yield neg 1 "<<yield_neg_3<<std::endl;
          double error_neg_D2_3 = h_xz_neg_all_3->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_bg_D2_3 = h_xz_neg_bg_all_3->GetBinError(i+1,j+1)/charge_neg_all;
          double error_neg_Dummy_3 = h_xz_neg_Dummy_all_3->GetBinError(i+1,j+1)/charge_neg_Dummy_all;
          double error_neg_3 = std::sqrt(error_neg_D2_3*error_neg_D2_3+error_neg_bg_D2_3*error_neg_bg_D2_3+0.245*0.245*error_neg_Dummy_3*error_neg_Dummy_3);
          double yield_pos_D2_3 = h_xz_pos_all_3->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_bg_D2_3 = h_xz_pos_bg_all_3->GetBinContent(i+1,j+1)/charge_pos_all;
          double yield_pos_Dummy_3 = h_xz_pos_Dummy_all_3->GetBinContent(i+1,j+1)/charge_pos_Dummy_all;
          double yield_pos_delta_3 = h_xz_pos_sim_delta_3->GetBinContent(i+1,j+1);
          double yield_pos_exc_3 = h_xz_pos_sim_excrad_3->GetBinContent(i+1,j+1);
          double yield_pos_3 = yield_pos_D2_3-yield_pos_bg_D2_3-0.245*yield_pos_Dummy_3-yield_pos_delta_3-yield_pos_exc_3;
          double error_pos_D2_3 = h_xz_pos_all_3->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_bg_D2_3 = h_xz_pos_bg_all_3->GetBinError(i+1,j+1)/charge_pos_all;
          double error_pos_Dummy_3 = h_xz_pos_Dummy_all_3->GetBinError(i+1,j+1)/charge_pos_Dummy_all;
          double error_pos_3 = std::sqrt(error_pos_D2_3*error_pos_D2_3+error_pos_bg_D2_3*error_pos_bg_D2_3+0.245*0.245*error_pos_Dummy_3*error_pos_Dummy_3);

          //for yield
          double radia_corr_neg_3 = rp_radia_corr_neg_3->GetBinContent(i+1,j+1);
          double radia_corr_pos_3 = rp_radia_corr_pos_3->GetBinContent(i+1,j+1);
          //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
          yield_neg_3 = yield_neg_3*radia_corr_neg_3;
          yield_pos_3 = yield_pos_3*radia_corr_pos_3;
          error_pos_3 = error_pos_3*radia_corr_pos_3;
          error_neg_3 = error_neg_3*radia_corr_neg_3;

          //need to think of how to convert to xs


          double RY_3 = (yield_neg_3)/(yield_pos_3);
          //std::cout<<"RY "<<RY<<std::endl;
          double error_3 = RY_3*std::sqrt((error_neg_3*error_neg_3)/(yield_neg_3*yield_neg_3)+(error_pos_3*error_pos_3)/(yield_pos_3*yield_pos_3));
          double y_RD_3 = (4*RY_3-1)/(1-RY_3);
          //double error_RD = error;
          double error_RD_3 = 3*error_3/((1-RY_3)*(1-RY_3));
          //for bin center correction
          double xbj_neg_corr_3 = h_xz_xbjmean_neg_all_3->GetBinContent(i+1,j+1);
          double z_neg_corr_3 = h_xz_zmean_neg_all_3->GetBinContent(i+1,j+1);
          double xbj_pos_corr_3 = h_xz_xbjmean_pos_all_3->GetBinContent(i+1,j+1);
          double z_pos_corr_3 = h_xz_zmean_pos_all_3->GetBinContent(i+1,j+1);

          double xbj_neg_corr_err2_3 = h_xz_xbjmean2_neg_all_3->GetBinContent(i+1,j+1)-xbj_neg_corr_3*xbj_neg_corr_3;
          double z_neg_corr_err2_3 = h_xz_zmean2_neg_all_3->GetBinContent(i+1,j+1)-z_neg_corr_3*z_neg_corr_3;
          double xbj_pos_corr_err2_3 = h_xz_xbjmean2_pos_all_3->GetBinContent(i+1,j+1)-xbj_pos_corr_3*xbj_pos_corr_3;
          double z_pos_corr_err2_3 = h_xz_zmean2_pos_all_3->GetBinContent(i+1,j+1)-z_pos_corr_3*z_pos_corr_3;

          double xbj_neg_corr_err_3 = sqrt(xbj_neg_corr_err2_3);
          double xbj_pos_corr_err_3 = sqrt(xbj_pos_corr_err2_3);
          double z_neg_corr_err_3 = sqrt(z_neg_corr_err2_3);
          double z_pos_corr_err_3 = sqrt(z_pos_corr_err2_3);

          double xbj_corr_3 = (xbj_neg_corr_3/xbj_neg_corr_err2_3+xbj_pos_corr_3/xbj_pos_corr_err2_3)/(1/xbj_neg_corr_err2_3+1/xbj_pos_corr_err2_3);
          if(abs(xbj_corr_3-y_3)>0.025){
            std::cout<<"3 xbj center "<<y_3<<", xbj corr "<<xbj_corr_3<<", pi+ "<<xbj_pos_corr_3<<" +- "<<xbj_pos_corr_err_3<<", pi- "<<xbj_neg_corr_3<<" +- "<<xbj_neg_corr_err_3<<std::endl;
            std::cout<<"neg bin content "<<h_xz_xbjmean2_neg_all_3->GetBinContent(i+1,j+1)<<" neg err2 "<<xbj_neg_corr_err2_3<<std::endl;
          }
          double xbj_corr_err_3 = 1/(1/xbj_neg_corr_err2_3+1/xbj_pos_corr_err2_3);
          double z_corr_3 = (z_neg_corr_3/z_neg_corr_err2_3+z_pos_corr_3/z_pos_corr_err2_3)/(1/z_neg_corr_err2_3+1/z_pos_corr_err2_3);
          double z_corr_err_3 = 1/(1/z_neg_corr_err2_3+1/z_pos_corr_err2_3);

          if(RY_3>0 && yield_neg_3>0.005){
            //double y_RD = (4*y-1)/(1-y);
            std::string xbj_str = std::to_string(y_3); 
            std::string z_str = std::to_string(x_3);

            if(yield_neg_3>0 && y_RD_3>0 && y_RD_3<10 && error_RD_3<20 && error_RD_3>0){
              //std::cout<<"check "<<y_RD_3<<std::endl;
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["value"].push_back(y_RD_3);
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["error"].push_back(error_RD_3);
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["yield_neg"].push_back(yield_neg_3);
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["yield_pos"].push_back(yield_pos_3);
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["charge_neg"].push_back(charge_neg_all);
              jout["Q2_3"][xbj_str.c_str()][z_str.c_str()]["charge_pos"].push_back(charge_pos_all);
              jout_2["Q2_3"][xbj_str.c_str()][(std::to_string(RunGroup)).c_str()][z_str.c_str()]["value"] = y_RD_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error"] = error_RD_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg"] = yield_neg_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos"] = yield_pos_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_neg"] = charge_neg_all;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_pos"] = charge_pos_all;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_neg_corr"] = xbj_neg_corr_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_neg_corr"] = z_neg_corr_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_pos_corr"] = xbj_pos_corr_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_pos_corr"] = z_pos_corr_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr"] = xbj_corr_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_err"] = xbj_corr_err_3;
              jout_2["Q2_3"][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_corr"] = z_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_3;
              jout_3["Q2_3"][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr_3;
              jout_4["Q2_3"][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["value"] = y_RD_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error"] = error_RD_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg"] = yield_neg_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos"] = yield_pos_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_neg"] = charge_neg_all;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_pos"] = charge_pos_all;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_neg_corr"] = xbj_neg_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_neg_corr"] = z_neg_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_pos_corr"] = xbj_pos_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_pos_corr"] = z_pos_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr"] = xbj_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr_err"] = xbj_corr_err_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr"] = z_corr_3;
              jout_5["Q2_3"][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr_err"] = z_corr_err_3;


            }
            else{
              jout_sthwrong["Q2_3"][xbj_str][z_str]["value"] = y_RD_3;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["error"] = error_RD_3;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["yield_neg"] = yield_neg_3;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["yield_pos"] = yield_pos_3;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["charge_neg"] = charge_neg_all;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["charge_pos"] = charge_pos_all;
              jout_sthwrong["Q2_3"][xbj_str][z_str]["rungroup"] = RunGroup;

            }
          }//if RY reasonable
        }//loop y axis for 3
      }//loop x axis for 3
    }//if not empty()

  }//loop over rungroup

  std::string jout_name = "results/yield_ratio_xz_Q2cut.json";
  std::ofstream ofs_jout(jout_name.c_str());
  ofs_jout<<jout.dump(4)<<std::endl;

  std::string jout_2_name = "results/yield_ratio_xz_Q2cut_withRungroups.json";
  std::ofstream ofs_jout_2(jout_2_name.c_str());
  ofs_jout_2<<jout_2.dump(4)<<std::endl;

  std::string jout_3_name = "results/yield_ratio_Q2xzRunG_Q2cut.json";
  std::ofstream ofs_jout_3(jout_3_name.c_str());
  ofs_jout_3<<jout_3.dump(4)<<std::endl;

  std::string jout_4_name = "results/yield_ratio_Q2zxRunG_Q2cut.json";
  std::ofstream ofs_jout_4(jout_4_name.c_str());
  ofs_jout_4<<jout_4.dump(4)<<std::endl;

  std::string jout_5_name = "results/yield_ratio_Q2zRunGx_Q2cut.json";
  std::ofstream ofs_jout_5(jout_5_name.c_str());
  ofs_jout_5<<jout_5.dump(4)<<std::endl;

  std::string jout_sthwrong_name = "results/yield_ratio_xz_Q2cut_sthwrong.json";
  std::ofstream ofs_jout_sthwrong(jout_sthwrong_name.c_str());
  ofs_jout_sthwrong<<jout_sthwrong.dump(4)<<std::endl;

  return 0;
}
