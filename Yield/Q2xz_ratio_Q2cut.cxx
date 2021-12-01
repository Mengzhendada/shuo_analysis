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
    TH2D* h_xz_neg_all = new TH2D("","neg;z;x",20,0,1,20,0,1);
    h_xz_neg_all->GetXaxis()->SetTitle("z");
    h_xz_neg_all->GetYaxis()->SetTitle("x");
    TH2D* h_xz_xbjmean_neg_all = new TH2D("","neg;z;x",20,0,1,20,0,1);
    TH2D* h_xz_zmean_neg_all = new TH2D("","neg;z;x",20,0,1,20,0,1);
    h_xz_xbjmean_neg_all->Sumw2();
    h_xz_zmean_neg_all->Sumw2();
    TH2D* h_xz_pos_all = new TH2D("","pos;z;x",20,0,1,20,0,1);
    TH2D* h_xz_xbjmean_pos_all = new TH2D("","pos;z;x",20,0,1,20,0,1);
    TH2D* h_xz_zmean_pos_all = new TH2D("","pos;z;x",20,0,1,20,0,1);
    h_xz_xbjmean_pos_all->Sumw2();
    h_xz_zmean_pos_all->Sumw2();
    h_xz_pos_all->GetXaxis()->SetTitle("z");
    h_xz_pos_all->GetYaxis()->SetTitle("x");
    h_xz_neg_all->Sumw2();
    h_xz_pos_all->Sumw2();
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
    std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
    auto runjs = it.value();


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
    //h_xz_neg_sim_rho = (TH2D*)rootfile_neg_sim->Get("x_z_neg_rho");
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
    //h_xz_pos_sim_rho = (TH2D*)rootfile_pos_sim->Get("x_z_pos_rho");
    TH2D *h_xz_pos_sim_delta = new TH2D("","pi+ sim delta",bins,0,1,bins,0,1);
    h_xz_pos_sim_delta = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta");
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
      charge_neg_all += charge;
      TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH2D *h_xz_neg = new TH2D("","",20,0,1,20,0,1);
      h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
      TH2D *h_xz_neg_bg = new TH2D("","",20,0,1,20,0,1);
      h_xz_neg_bg = (TH2D*)root_file_neg->Get("x_z_bg");
      double EFF = Get_all_eff(RunNumber);
      h_xz_neg_bg_all->Add(h_xz_neg_bg,1/(6*EFF));
      h_xz_neg_all->Add(h_xz_neg,1/EFF);

      TH2D* h_xz_xbjmean_neg = new TH2D("","",20,0,1,20,0,1);
      h_xz_xbjmean_neg = (TH2D*)root_file_neg->Get("weighted_xbj");
      TH2D* h_xz_zmean_neg = new TH2D("","",20,0,1,20,0,1);
      h_xz_zmean_neg = (TH2D*)root_file_neg->Get("weighted_z");
      h_xz_xbjmean_neg_all->Add(h_xz_xbjmean_neg,1.0/N_neg_D2);
      h_xz_zmean_neg_all->Add(h_xz_zmean_neg,1.0/N_neg_D2);
    }//loop over neg runs
    for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
      int RunNumber = *it;
      //std::cout<<RunNumber<<std::endl;
      double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
      charge_pos_all+=charge;
      //std::cout<<"pos DE check"<<std::endl;
      TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH2D *h_xz_pos = new TH2D("","",20,0,1,20,0,1);
      h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
      TH2D *h_xz_pos_bg = new TH2D("","",20,0,1,20,0,1);
      h_xz_pos_bg = (TH2D*)root_file_pos->Get("x_z_bg");
      double EFF = Get_all_eff(RunNumber);
      h_xz_pos_bg_all->Add(h_xz_pos_bg,1/(6*EFF));
      h_xz_pos_all->Add(h_xz_pos,1/EFF);
      TH2D* h_xz_xbjmean_pos = new TH2D("","",20,0,1,20,0,1);
      h_xz_xbjmean_pos = (TH2D*)root_file_pos->Get("weighted_xbj");
      TH2D* h_xz_zmean_pos = new TH2D("","",20,0,1,20,0,1);
      h_xz_zmean_pos = (TH2D*)root_file_pos->Get("weighted_z");
      h_xz_xbjmean_pos_all->Add(h_xz_xbjmean_pos,1.0/N_pos_D2);
      h_xz_zmean_pos_all->Add(h_xz_zmean_pos,1.0/N_pos_D2);
    }//loop over pos runs
    for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
      int RunNumber = *it;
      //std::cout<<"Dummy"<<RunNumber<<std::endl;
      double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
      charge_neg_Dummy_all += charge;
      TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH2D *h_xz_neg = new TH2D("","",bins,0,1,bins,0,1);
      h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
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
      charge_pos_Dummy_all+=charge;
      TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH2D *h_xz_pos = new TH2D("","",bins,0,1,bins,0,1);
      h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
      TH2D *h_xz_pos_bg = new TH2D("","",bins,0,1,bins,0,1);
      h_xz_pos_bg = (TH2D*)root_file_pos->Get("x_z_bg");
      double EFF = Get_all_eff(RunNumber);
      h_xz_pos_bg_Dummy_all->Add(h_xz_pos_bg,1/(6*EFF));
      h_xz_pos_Dummy_all->Add(h_xz_pos,1/EFF);
    }//loop over pos runs


    //Dummy bg subtract
    h_xz_neg_Dummy_all->Add(h_xz_neg_bg_Dummy_all,-1.0);
    h_xz_pos_Dummy_all->Add(h_xz_pos_bg_Dummy_all,-1.0);


    auto h_xz_neg_incnorad = h_xz_neg_sim_incnorad;
    auto h_xz_neg_incrad = h_xz_neg_sim_incrad;
    auto h_xz_pos_incnorad = h_xz_pos_sim_incnorad;
    auto h_xz_pos_incrad = h_xz_pos_sim_incrad;

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
        double yield_neg = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc;
        double error_neg_D2 = h_xz_neg_all->GetBinError(i+1,j+1)/charge_neg_all;
        double error_neg_D2_2 = std::sqrt(h_xz_neg_all->GetBinContent(i+1,j+1))/charge_neg_all;
        if(error_neg_D2_2!=0){std::cout<<"check two errors"<<error_neg_D2/error_neg_D2_2<<std::endl;}
        double error_neg_bg_D2 = h_xz_neg_bg_all->GetBinError(i+1,j+1)/charge_neg_all;
        double error_neg_Dummy = h_xz_neg_Dummy_all->GetBinError(i+1,j+1)/charge_neg_Dummy_all;
        double error_neg = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy);
        double yield_pos_D2 = h_xz_pos_all->GetBinContent(i+1,j+1)/charge_pos_all;
        double yield_pos_bg_D2 = h_xz_pos_bg_all->GetBinContent(i+1,j+1)/charge_pos_all;
        double yield_pos_Dummy = h_xz_pos_Dummy_all->GetBinContent(i+1,j+1)/charge_pos_Dummy_all;
        double yield_pos_delta = h_xz_pos_sim_delta->GetBinContent(i+1,j+1);
        double yield_pos_exc = h_xz_pos_sim_excrad->GetBinContent(i+1,j+1);
        double yield_pos = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc;
        double error_pos_D2 = h_xz_pos_all->GetBinError(i+1,j+1)/charge_pos_all;
        double error_pos_bg_D2 = h_xz_pos_bg_all->GetBinError(i+1,j+1)/charge_pos_all;
        double error_pos_Dummy = h_xz_pos_Dummy_all->GetBinError(i+1,j+1)/charge_pos_Dummy_all;
        double error_pos = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy);

        //for yield
        double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i+1,j+1);
        double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i+1,j+1);
        //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
        yield_neg = yield_neg*radia_corr_neg;
        yield_pos = yield_pos*radia_corr_pos;
        error_pos = error_pos*radia_corr_pos;
        error_neg = error_neg*radia_corr_neg;

        //need to think of how to convert to xs


        double RY = (yield_neg)/(yield_pos);
        //std::cout<<"RY "<<RY<<std::endl;
        double error = RY*std::sqrt((error_neg*error_neg)/(yield_neg*yield_neg)+(error_pos*error_pos)/(yield_pos*yield_pos));
        double y_RD = (4*RY-1)/(1-RY);
        //double error_RD = error;
        double error_RD = 3*error/((1-y)*(1-y));
        double y_frag = (4*y-1)/(4-y);
        double error_frag = y_frag*std::sqrt((error*error)/(RY*RY));
        //for bin center correction
        double xbj_neg_corr = h_xz_xbjmean_neg_all->GetBinContent(i+1,j+1);
        double z_neg_corr = h_xz_zmean_neg_all->GetBinContent(i+1,j+1);
        double xbj_pos_corr = h_xz_xbjmean_pos_all->GetBinContent(i+1,j+1);
        double z_pos_corr = h_xz_zmean_pos_all->GetBinContent(i+1,j+1);

        double xbj_neg_corr_err = h_xz_xbjmean_neg_all->GetBinError(i+1,j+1);
        double z_neg_corr_err = h_xz_zmean_neg_all->GetBinError(i+1,j+1);
        double xbj_pos_corr_err = h_xz_xbjmean_pos_all->GetBinError(i+1,j+1);
        double z_pos_corr_err = h_xz_zmean_pos_all->GetBinError(i+1,j+1);

        double xbj_neg_corr_err2 = xbj_neg_corr_err*xbj_neg_corr_err;
        double xbj_pos_corr_err2 = xbj_pos_corr_err*xbj_pos_corr_err;
        double z_neg_corr_err2 = z_neg_corr_err*z_neg_corr_err;
        double z_pos_corr_err2 = z_pos_corr_err*z_pos_corr_err;

        double xbj_corr = (xbj_neg_corr/xbj_neg_corr_err2+xbj_pos_corr/xbj_pos_corr_err2)/(1/xbj_neg_corr_err2+1/xbj_pos_corr_err2);
        double xbj_corr_err = 1/(1/xbj_neg_corr_err2+1/xbj_pos_corr_err2);
        double z_corr = (z_neg_corr/z_neg_corr_err2+z_pos_corr/z_pos_corr_err2)/(1/z_neg_corr_err2+1/z_pos_corr_err2);
        double z_corr_err = 1/(1/z_neg_corr_err2+1/z_pos_corr_err2);

        if(RY>0 && error< 0.1){
          //double y_RD = (4*y-1)/(1-y);
          g_yield_ratio->SetPoint(ii,x,y,RY);
          std::string xbj_str = std::to_string(y); 

          if(y_RD>0 && y_RD<10 && error_RD<2 && error_RD>0){
            jout[std::to_string(Q2)][xbj_str][z_str]["value"].push_back(y_RD);
            jout[std::to_string(Q2)][xbj_str][z_str]["error"].push_back(error_RD);
            jout[std::to_string(Q2)][xbj_str][z_str]["yield_neg"].push_back(yield_neg);
            jout[std::to_string(Q2)][xbj_str][z_str]["yield_pos"].push_back(yield_pos);
            jout[std::to_string(Q2)][xbj_str][z_str]["charge_neg"].push_back(charge_neg_all);
            jout[std::to_string(Q2)][xbj_str][z_str]["charge_pos"].push_back(charge_pos_all);
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["value"] = y_RD;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["error"] = error_RD;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_neg"] = yield_neg;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["yield_pos"] = yield_pos;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_neg"] = charge_neg_all;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["charge_pos"] = charge_pos_all;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_neg_corr"] = xbj_neg_corr;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_neg_corr"] = z_neg_corr;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_pos_corr"] = xbj_pos_corr;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_pos_corr"] = z_pos_corr;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr"] = xbj_corr;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["xbj_corr_err"] = xbj_corr_err;
            jout_2[std::to_string(Q2)][xbj_str][(std::to_string(RunGroup)).c_str()][z_str]["z_corr"] = z_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr;
            jout_3[std::to_string(Q2)][xbj_str][z_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["value"] = y_RD;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["error"] = error_RD;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_neg"] = yield_neg;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["yield_pos"] = yield_pos;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_neg"] = charge_neg_all;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["charge_pos"] = charge_pos_all;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_neg_corr"] = xbj_neg_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_neg_corr"] = z_neg_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_pos_corr"] = xbj_pos_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_pos_corr"] = z_pos_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr"] = xbj_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["xbj_corr_err"] = xbj_corr_err;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr"] = z_corr;
            jout_4[std::to_string(Q2)][z_str][xbj_str][(std::to_string(RunGroup)).c_str()]["z_corr_err"] = z_corr_err;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["value"] = y_RD;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["error"] = error_RD;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_neg"] = yield_neg;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["yield_pos"] = yield_pos;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_neg"] = charge_neg_all;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["charge_pos"] = charge_pos_all;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_neg_corr"] = xbj_neg_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_neg_corr"] = z_neg_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_pos_corr"] = xbj_pos_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_pos_corr"] = z_pos_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr"] = xbj_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["xbj_corr_err"] = xbj_corr_err;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr"] = z_corr;
            jout_5[std::to_string(Q2)][z_str][(std::to_string(RunGroup)).c_str()][xbj_str]["z_corr_err"] = z_corr_err;


          }
          else{
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["value"] = y_RD;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["error"] = error_RD;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["yield_neg"] = yield_neg;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["yield_pos"] = yield_pos;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["charge_neg"] = charge_neg_all;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["charge_pos"] = charge_pos_all;
            jout_sthwrong[std::to_string(Q2)][xbj_str][z_str]["rungroup"] = RunGroup;

          }
        }
      }
    }


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
  ofs_jout_5<<jout_5.dump(5)<<std::endl;
  
  std::string jout_sthwrong_name = "results/yield_ratio_xz_Q2cut_sthwrong.json";
  std::ofstream ofs_jout_sthwrong(jout_sthwrong_name.c_str());
  ofs_jout_sthwrong<<jout_sthwrong.dump(4)<<std::endl;

  return 0;
}
