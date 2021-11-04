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
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_all = new TMultiGraph(); 
  std::vector<int> i_which_x = {0,1,2,0,3,1,0,2,1,2,3,3};
  std::vector<int> i_which_Q2= {0,0,0,1,0,1,2,1,2,2,1,2};
  int i_whichx = 0; 
  json jout;
  json jout_2;
  json jout_sthwrong;
  //TH2D* h_Q2_1_neg =new TH2D("","3.95",100,0,1,100,0,1);
  //TH2D* h_Q2_1_pos =new TH2D("","3.95",100,0,1,100,0,1);
  //TH2D* h_Q2_2_neg =new TH2D("","4.75",100,0,1,100,0,1);
  //TH2D* h_Q2_2_pos =new TH2D("","4.75",100,0,1,100,0,1);
  //TH2D* h_Q2_3_neg =new TH2D("","5",100,0,1,100,0,1);
  //TH2D* h_Q2_3_pos =new TH2D("","5",100,0,1,100,0,1);
  //TH2D* h_Q2_4_neg =new TH2D("","5.5",100,0,1,100,0,1);
  //TH2D* h_Q2_4_pos =new TH2D("","5.5",100,0,1,100,0,1);
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
      auto j_z = it.value();
      TH2D* h_neg_q2 = new TH2D("",(q2_name).c_str(),100,0,1,100,0,1);
      TH2D* h_pos_q2 = new TH2D("",(q2_name).c_str(),100,0,1,100,0,1);
      int i_color = 1;
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          TH2D* h_xz_neg_all = new TH2D("","neg;z;x",100,0,1,100,0,1);
          h_xz_neg_all->GetXaxis()->SetTitle("z");
          h_xz_neg_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_all = new TH2D("","pos;z;x",100,0,1,100,0,1);
          h_xz_pos_all->GetXaxis()->SetTitle("z");
          h_xz_pos_all->GetYaxis()->SetTitle("x");
          h_xz_neg_all->Sumw2();
          h_xz_pos_all->Sumw2();
          TH2D* h_xz_neg_bg_all = new TH2D("","neg bg;z;x",100,0,1,100,0,1);
          h_xz_neg_bg_all->GetXaxis()->SetTitle("z");
          h_xz_neg_bg_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_bg_all = new TH2D("","pos bg;z;x",100,0,1,100,0,1);
          h_xz_pos_bg_all->GetXaxis()->SetTitle("z");
          h_xz_pos_bg_all->GetYaxis()->SetTitle("x");
          h_xz_neg_bg_all->Sumw2();
          h_xz_pos_bg_all->Sumw2();
          TH2D* h_xz_neg_Dummy_all = new TH2D("","neg;z;x",100,0,1,100,0,1);
          h_xz_neg_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_neg_Dummy_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_Dummy_all = new TH2D("","pos;z;x",100,0,1,100,0,1);
          h_xz_pos_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_pos_Dummy_all->GetXaxis()->SetTitle("x");
          h_xz_neg_Dummy_all->Sumw2();
          h_xz_pos_Dummy_all->Sumw2();
          TH2D* h_xz_neg_bg_Dummy_all = new TH2D("","neg bg;z;x",100,0,1,100,0,1);
          h_xz_neg_bg_Dummy_all->GetXaxis()->SetTitle("z");
          h_xz_neg_bg_Dummy_all->GetYaxis()->SetTitle("x");
          TH2D* h_xz_pos_bg_Dummy_all = new TH2D("","pos bg;z;x",100,0,1,100,0,1);
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
          TH2D *h_xz_neg_sim_incrad = new TH2D("","pi- sim SIDIS",100,0,1,100,0,1);
          h_xz_neg_sim_incrad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_rad");
          TH2D *h_xz_neg_sim_incnorad = new TH2D("","pi- sim SIDIS",100,0,1,100,0,1);
          h_xz_neg_sim_incnorad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_inc_norad");
          TH2D *h_xz_neg_sim_excrad = new TH2D("","pi- sim exc",100,0,1,100,0,1);
          h_xz_neg_sim_excrad = (TH2D*)rootfile_neg_sim->Get("x_z_neg_exc_rad");
          TH2D *h_xz_neg_sim_rho = new TH2D("","pi- sim rho",100,0,1,100,0,1);
          //h_xz_neg_sim_rho = (TH2D*)rootfile_neg_sim->Get("x_z_neg_rho");
          TH2D *h_xz_neg_sim_delta = new TH2D("","pi- sim delta",100,0,1,100,0,1);
          h_xz_neg_sim_delta = (TH2D*)rootfile_neg_sim->Get("x_z_neg_delta");
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH2D *h_xz_pos_sim_incrad = new TH2D("","pi+ sim SIDIS",100,0,1,100,0,1);
          h_xz_pos_sim_incrad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_rad");
          TH2D *h_xz_pos_sim_incnorad = new TH2D("","pi+ sim SIDIS",100,0,1,100,0,1);
          h_xz_pos_sim_incnorad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_inc_norad");
          TH2D *h_xz_pos_sim_excrad = new TH2D("","pi+ sim exc",100,0,1,100,0,1);
          h_xz_pos_sim_excrad = (TH2D*)rootfile_pos_sim->Get("x_z_pos_exc_rad");
          TH2D *h_xz_pos_sim_rho = new TH2D("","pi+ sim rho",100,0,1,100,0,1);
          //h_xz_pos_sim_rho = (TH2D*)rootfile_pos_sim->Get("x_z_pos_rho");
          TH2D *h_xz_pos_sim_delta = new TH2D("","pi+ sim delta",100,0,1,100,0,1);
          h_xz_pos_sim_delta = (TH2D*)rootfile_pos_sim->Get("x_z_pos_delta");
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_all += charge;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_neg = new TH2D("","",100,0,1,100,0,1);
              h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
              TH2D *h_xz_neg_bg = new TH2D("","",100,0,1,100,0,1);
              h_xz_neg_bg = (TH2D*)root_file_neg->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_neg_bg_all->Add(h_xz_neg_bg,1/(6*EFF));
              h_xz_neg_all->Add(h_xz_neg,1/EFF);
            }//loop over neg runs
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_all+=charge;
              //std::cout<<"pos DE check"<<std::endl;
              TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_pos = new TH2D("","",100,0,1,100,0,1);
              h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
              TH2D *h_xz_pos_bg = new TH2D("","",100,0,1,100,0,1);
              h_xz_pos_bg = (TH2D*)root_file_pos->Get("x_z_bg");
              double EFF = Get_all_eff(RunNumber);
              h_xz_pos_bg_all->Add(h_xz_pos_bg,1/(6*EFF));
              h_xz_pos_all->Add(h_xz_pos,1/EFF);
            }//loop over pos runs
            for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
              int RunNumber = *it;
              //std::cout<<"Dummy"<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_Dummy_all += charge;
              TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
              TH2D *h_xz_neg = new TH2D("","",100,0,1,100,0,1);
              h_xz_neg = (TH2D*)root_file_neg->Get("x_z");
              TH2D *h_xz_neg_bg = new TH2D("","",100,0,1,100,0,1);
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
              TH2D *h_xz_pos = new TH2D("","",100,0,1,100,0,1);
              h_xz_pos = (TH2D*)root_file_pos->Get("x_z");
              TH2D *h_xz_pos_bg = new TH2D("","",100,0,1,100,0,1);
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
              double x = h_xz_neg_all->GetXaxis()->GetBinCenter(i);
              double y = h_xz_neg_all->GetYaxis()->GetBinCenter(j);
              double yield_neg_D2 = h_xz_neg_all->GetBinContent(i,j)/charge_neg_all;
              double yield_neg_bg_D2 = h_xz_neg_bg_all->GetBinContent(i,j)/charge_neg_all;
              double yield_neg_Dummy = h_xz_neg_Dummy_all->GetBinContent(i,j)/charge_neg_Dummy_all;
              double yield_neg_delta = h_xz_neg_sim_delta->GetBinContent(i,j);
              double yield_neg_exc = h_xz_neg_sim_excrad->GetBinContent(i,j);
              double yield_neg = yield_neg_D2-yield_neg_bg_D2-0.245*yield_neg_Dummy-yield_neg_delta-yield_neg_exc;
              double error_neg_D2 = h_xz_neg_all->GetBinError(i,j)/charge_neg_all;
              double error_neg_bg_D2 = h_xz_neg_bg_all->GetBinError(i,j)/charge_neg_all;
              double error_neg_Dummy = h_xz_neg_Dummy_all->GetBinError(i,j)/charge_neg_Dummy_all;
              double error_neg = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy);
              double yield_pos_D2 = h_xz_pos_all->GetBinContent(i,j)/charge_pos_all;
              double yield_pos_bg_D2 = h_xz_pos_bg_all->GetBinContent(i,j)/charge_pos_all;
              double yield_pos_Dummy = h_xz_pos_Dummy_all->GetBinContent(i,j)/charge_pos_Dummy_all;
              double yield_pos_delta = h_xz_pos_sim_delta->GetBinContent(i,j);
              double yield_pos_exc = h_xz_pos_sim_excrad->GetBinContent(i,j);
              double yield_pos = yield_pos_D2-yield_pos_bg_D2-0.245*yield_pos_Dummy-yield_pos_delta-yield_pos_exc;
              double error_pos_D2 = h_xz_pos_all->GetBinError(i,j)/charge_pos_all;
              double error_pos_bg_D2 = h_xz_pos_bg_all->GetBinError(i,j)/charge_pos_all;
              double error_pos_Dummy = h_xz_pos_Dummy_all->GetBinError(i,j)/charge_pos_Dummy_all;
              double error_pos = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy);

              //for yield
              double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i,j);
              double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i,j);
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

  std::string jout_sthwrong_name = "results/yield_ratio_xz_combined_sthwrong.json";
  std::ofstream ofs_jout_sthwrong(jout_sthwrong_name.c_str());
  ofs_jout_sthwrong<<jout_sthwrong.dump(4)<<std::endl;

  return 0;
}
