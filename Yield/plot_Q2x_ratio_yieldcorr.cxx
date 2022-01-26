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

double Get_weighted_mean(std::vector<double> value,std::vector<double> err){
  double weighted_mean;
  double weighted_sigma;
  for(int i = 0;i<err.size();++i){
    if(err[i]!=0){
    weighted_mean += value[i]/(err[i]*err[i]);
    weighted_sigma += 1/(err[i]*err[i]);
    }
  }
  double mean = 0;
  if(weighted_mean!=0){
    mean = weighted_mean/weighted_sigma;
  }
  return mean;
};
double Get_weighted_sigma(std::vector<double> value,std::vector<double> err){
  double weighted_sigma;
  for(int i = 0;i<err.size();++i){
    if(err[i]!=0){
    weighted_sigma += 1/(err[i]*err[i]);
    }
  }
  double sig = 0;
  if(weighted_sigma!=0){
    double sig = std::sqrt(1/weighted_sigma);
  }
  return sig;
};
std::vector<double> Get_weighted_averages(std::vector<int> RunNumbers,int bins){
  std::vector<double> weighted_yield;
  std::vector<double> weighted_yield_err;
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  
  for(int i = 1;i<=bins;++i){
    std::vector<double> yields;
    std::vector<double> yield_errs;
    double bin_center = 0;
    for(auto it = RunNumbers.begin();it!=RunNumbers.end();++it){
      int RunNumber = *it;
      double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
      TFile *root_file = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH1D *h_z = new TH1D("","",bins,0,1);
      h_z = (TH1D*)root_file->Get("z");
      TH1D *h_z_bg = new TH1D("","",bins,0,1);
      h_z_bg = (TH1D*)root_file->Get("z_bg");
      double EFF = Get_all_eff(RunNumber);
      h_z->Add(h_z_bg,-1.0/6);
      yields.push_back(h_z->GetBinContent(i)/(charge*EFF));
      yield_errs.push_back(sqrt(h_z->GetBinContent(i))/(charge*EFF));
      bin_center = h_z->GetBinCenter(i);
    }
    double yield_i = Get_weighted_mean(yields,yield_errs);
    //double yield_err_i = Get_weighted_sigma(yields,yield_errs);
    weighted_yield.push_back(yield_i);
    //weighted_yield_err.push_back(yield_err_i);
  }
  return weighted_yield;
};
std::vector<double> Get_weighted_errs(std::vector<int> RunNumbers,int bins){
  std::vector<double> weighted_yield;
  std::vector<double> weighted_yield_err;
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  
  for(int i = 1;i<=bins;++i){
    std::vector<double> yields;
    std::vector<double> yield_errs;
    double bin_center = 0;
    for(auto it = RunNumbers.begin();it!=RunNumbers.end();++it){
      int RunNumber = *it;
      double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
      TFile *root_file = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
      TH1D *h_z = new TH1D("","",bins,0,1);
      h_z = (TH1D*)root_file->Get("z");
      TH1D *h_z_bg = new TH1D("","",bins,0,1);
      h_z_bg = (TH1D*)root_file->Get("z_bg");
      double EFF = Get_all_eff(RunNumber);
      h_z->Add(h_z_bg,-1.0/6);
      yields.push_back(h_z->GetBinContent(i)/(charge*EFF));
      yield_errs.push_back(sqrt(h_z->GetBinContent(i))/(charge*EFF));
      bin_center = h_z->GetBinCenter(i);
    }
   // double yield_i = Get_weighted_mean(yields,yield_errs);
    double yield_err_i = Get_weighted_sigma(yields,yield_errs);
    //weighted_yield.push_back(yield_i);
    weighted_yield_err.push_back(yield_err_i);
  }
  return weighted_yield_err;
};
std::vector<double> Get_x_axis(std::vector<int> RunNumbers,int bins){
  std::vector<double> x_axis;
  
  for(int i = 1;i<=bins;++i){
    double bin_center = 0;
    int RunNumber = RunNumbers[0];
    TFile *root_file = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
    TH1D *h_z = new TH1D("","",bins,0,1);
    h_z = (TH1D*)root_file->Get("z");
    bin_center = h_z->GetBinCenter(i);
    x_axis.push_back(bin_center);
  }
  return x_axis;
};


int plot_Q2x_ratio_yieldcorr(){
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

  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  auto mg_all = new TMultiGraph(); 
  std::vector<int> i_which_x = {0,1,2,0,3,1,0,2,1,2,3,3};
  std::vector<int> i_which_Q2= {0,0,0,1,0,1,2,1,2,2,1,2};
  int i_whichx = 0; 
  json jout;

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
      auto mg = new TMultiGraph();
      auto mg_frag = new TMultiGraph();
      auto mg_RD = new TMultiGraph();
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
          std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
          std::vector<int> neg_D2_runs, pos_D2_runs,neg_Dummy_runs,pos_Dummy_runs;
          std::vector<double> neg_D2_yields,neg_D2_errs;
          std::vector<double> pos_D2_yields,pos_D2_errs;
          std::vector<double> neg_Dummy_yields,neg_Dummy_errs;
          std::vector<double> pos_Dummy_yields,pos_Dummy_errs;
          std::vector<double> x_axis;

          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          TFile *rootfile_neg_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_neg_sim = new TH1D("","pi- sim sum",bins,0,1);
          TH1D *h_z_neg_sim_incrad = new TH1D("","pi- sim SIDIS",bins,0,1);
          h_z_neg_sim_incrad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_rad");
          TH1D *h_z_neg_sim_incnorad = new TH1D("","pi- sim SIDIS",bins,0,1);
          h_z_neg_sim_incnorad = (TH1D*)rootfile_neg_sim->Get("z_neg_inc_norad");
          TH1D *h_z_neg_sim_excrad = new TH1D("","pi- sim exc",bins,0,1);
          h_z_neg_sim_excrad = (TH1D*)rootfile_neg_sim->Get("z_neg_exc_rad");
          TH1D *h_z_neg_sim_rho = new TH1D("","pi- sim rho",bins,0,1);
          h_z_neg_sim_rho = (TH1D*)rootfile_neg_sim->Get("z_neg_rho");
          TH1D *h_z_neg_sim_delta = new TH1D("","pi- sim delta",bins,0,1);
          h_z_neg_sim_delta = (TH1D*)rootfile_neg_sim->Get("z_neg_delta");
          //TH2D *h_xs_z_neg_sim = new TH2D("","pi- sim xs",bins,0,1,bins,0,1);
          //h_xs_z_neg_sim = (TH2D*)rootfile_neg_sim->Get("xs_z_neg_inc_rad");
          h_z_neg_sim->Add(h_z_neg_sim_incrad,1);
          h_z_neg_sim->Add(h_z_neg_sim_excrad,1);
          //h_z_neg_sim->Add(h_z_neg_sim_rho,1);
          h_z_neg_sim->Add(h_z_neg_sim_delta,1);
          TFile *rootfile_pos_sim = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root").c_str());
          TH1D *h_z_pos_sim = new TH1D("","pi+ sim sum",bins,0,1);
          TH1D *h_z_pos_sim_incrad = new TH1D("","pi+ sim SIDIS",bins,0,1);
          h_z_pos_sim_incrad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_rad");
          TH1D *h_z_pos_sim_incnorad = new TH1D("","pi+ sim SIDIS",bins,0,1);
          h_z_pos_sim_incnorad = (TH1D*)rootfile_pos_sim->Get("z_pos_inc_norad");
          TH1D *h_z_pos_sim_excrad = new TH1D("","pi+ sim exc",bins,0,1);
          h_z_pos_sim_excrad = (TH1D*)rootfile_pos_sim->Get("z_pos_exc_rad");
          TH1D *h_z_pos_sim_rho = new TH1D("","pi+ sim rho",bins,0,1);
          h_z_pos_sim_rho = (TH1D*)rootfile_pos_sim->Get("z_pos_rho");
          TH1D *h_z_pos_sim_delta = new TH1D("","pi+ sim delta",bins,0,1);
          h_z_pos_sim_delta = (TH1D*)rootfile_pos_sim->Get("z_pos_delta");
          //TH2D *h_xs_z_pos_sim = new TH2D("","pi- sim xs",bins,0,1,bins,0,1);
          //h_xs_z_pos_sim = (TH2D*)rootfile_pos_sim->Get("xs_z_pos_inc_rad");
          h_z_pos_sim->Add(h_z_pos_sim_incrad,1);
          h_z_pos_sim->Add(h_z_pos_sim_excrad,1);
          //h_z_pos_sim->Add(h_z_pos_sim_rho,1);
          h_z_pos_sim->Add(h_z_pos_sim_delta,1);

          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();
            std::cout<<"check neg first run"<<neg_D2_runs[0]<<std::endl;
            neg_D2_yields = Get_weighted_averages(neg_D2_runs,bins);
            std::cout<<neg_D2_yields[9]<<std::endl;
            neg_D2_errs = Get_weighted_errs(neg_D2_runs,bins);
            pos_D2_yields = Get_weighted_averages(pos_D2_runs,bins);
            std::cout<<pos_D2_yields[9]<<std::endl;
            pos_D2_errs = Get_weighted_errs(pos_D2_runs,bins);
            neg_Dummy_yields = Get_weighted_averages(neg_Dummy_runs,bins);
            neg_Dummy_errs = Get_weighted_errs(neg_Dummy_runs,bins);
            pos_Dummy_yields = Get_weighted_averages(pos_Dummy_runs,bins);
            pos_Dummy_errs = Get_weighted_errs(pos_Dummy_runs,bins);
            std::cout<<neg_Dummy_yields[9]<<std::endl;
            std::cout<<pos_Dummy_yields[9]<<std::endl;
            x_axis = Get_x_axis(neg_D2_runs,bins);
            std::cout<<x_axis[9]<<std::endl;
          }//if z not 0



          auto h_z_neg_incnorad = h_z_neg_sim_incnorad;
          auto h_z_neg_incrad = h_z_neg_sim_incrad;
          auto h_z_pos_incnorad = h_z_pos_sim_incnorad;
          auto h_z_pos_incrad = h_z_pos_sim_incrad;

          TH1* rp_radia_corr_neg = (TH1*)h_z_neg_incnorad->Clone("rp_radia_corr");
          TH1* rp_radia_corr_pos = (TH1*)h_z_pos_incnorad->Clone("rp_radia_corr");
          rp_radia_corr_neg ->Divide(h_z_neg_incnorad,h_z_neg_incrad);          
          rp_radia_corr_pos ->Divide(h_z_pos_incnorad,h_z_pos_incrad);          
          TCanvas* c_radia_pos = new TCanvas();
          rp_radia_corr_pos->Draw();
          std::string c_radia_pos_name = "results/yield/statistics_corr/radia_corr_neg"+q2xz_str_filename+"_pos.pdf";
          c_radia_pos->SaveAs(c_radia_pos_name.c_str());
          TCanvas* c_radia_neg = new TCanvas();
          rp_radia_corr_neg->Draw();
          std::string c_radia_neg_name = "results/yield/statistics_corr/radia_corr_neg"+q2xz_str_filename+"_neg.pdf";
          c_radia_neg->SaveAs(c_radia_neg_name.c_str());

          TGraphErrors* g_z_pos_yield = new TGraphErrors();
          g_z_pos_yield->SetTitle("pi+ data yield");
          TGraphErrors* g_z_pos_Dummy_yield = new TGraphErrors();
          g_z_pos_Dummy_yield->SetTitle("pi+ Dummy yield");
          TGraphErrors* g_z_pos_yield_corr = new TGraphErrors();
          g_z_pos_yield_corr->SetTitle("pi- data yield");
          int nbins_yield_pos = bins;
          int ii_yield_pos = 0;
          for(int i = 0;i<nbins_yield_pos;i++){
            double x = x_axis[i]; 
            double y = pos_D2_yields[i];
            double y_dummy = pos_Dummy_yields[i]; 
            std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            double error = pos_D2_errs[i]; 
            double error_dummy = pos_Dummy_errs[i]; 
            double y_corr = y-0.245*y_dummy;
            double error_corr = sqrt(error*error+0.245*0.245*error_dummy*error_dummy);
            if(y!=0){
              g_z_pos_yield->SetPoint(ii_yield_pos,x,y);
              g_z_pos_yield->SetPointError(ii_yield_pos,0,error);
              g_z_pos_Dummy_yield->SetPoint(ii_yield_pos,x,y_dummy);
              g_z_pos_Dummy_yield->SetPointError(ii_yield_pos,0,error_dummy);
              g_z_pos_yield_corr->SetPoint(ii_yield_pos,x,y_corr);
              g_z_pos_yield_corr->SetPointError(ii_yield_pos,0,error_corr);
              ii_yield_pos++;
            }
          }
          TCanvas *c_yield_pos = new TCanvas();
          gStyle->SetOptTitle(0);
          TMultiGraph* mg_z_yield_pos = new TMultiGraph(); 
          g_z_pos_yield->GetYaxis()->SetTitle("yield");
          g_z_pos_yield->GetYaxis()->SetTitleSize(0.53);
          g_z_pos_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_z_pos_yield->SetMarkerStyle(4);
          mg_z_yield_pos->Add(g_z_pos_yield,"P");
          g_z_pos_Dummy_yield->SetMarkerStyle(4);
          g_z_pos_Dummy_yield->SetMarkerColor(kRed);
          g_z_pos_Dummy_yield->SetTitle("data Dummy_yield");
          mg_z_yield_pos->Add(g_z_pos_Dummy_yield,"P");
          g_z_pos_yield_corr->SetMarkerStyle(4);
          g_z_pos_yield_corr->SetMarkerColor(kRed);
          g_z_pos_yield_corr->SetTitle("data yield final");
          mg_z_yield_pos->Add(g_z_pos_yield_corr,"P");
          TGraph* g_z_pos_sim = new TGraph(h_z_pos_sim);
          TGraph* g_z_pos_sim_incrad = new TGraph(h_z_pos_sim_incrad);
          TGraph* g_z_pos_sim_incnorad = new TGraph(h_z_pos_sim_incnorad);
          TGraph* g_z_pos_sim_excrad = new TGraph(h_z_pos_sim_excrad);
          TGraph* g_z_pos_sim_rho = new TGraph(h_z_pos_sim_rho);
          TGraph* g_z_pos_sim_delta = new TGraph(h_z_pos_sim_delta);
          mg_z_yield_pos->Add(g_z_pos_sim); 
          g_z_pos_sim_incrad->SetLineColor(kOrange);
          g_z_pos_sim_incrad->SetTitle("pi+ sim SIDIS");
          mg_z_yield_pos->Add(g_z_pos_sim_incrad); 
          g_z_pos_sim_excrad->SetLineColor(kBlue);
          g_z_pos_sim_excrad->SetTitle("pi+ sim exc");
          mg_z_yield_pos->Add(g_z_pos_sim_excrad); 
          g_z_pos_sim_rho->SetLineColor(kRed);
          g_z_pos_sim_rho->SetTitle("pi+ sim rho");
          mg_z_yield_pos->Add(g_z_pos_sim_rho); 
          g_z_pos_sim_delta->SetLineColor(46);
          g_z_pos_sim_delta->SetTitle("pi+ sim delta");
          mg_z_yield_pos->Add(g_z_pos_sim_delta); 
          g_z_pos_sim_incnorad->SetLineColor(40);
          g_z_pos_sim_incnorad->SetTitle("pi+ sim inc norad");
          mg_z_yield_pos->Add(g_z_pos_sim_incnorad,"L");
          mg_z_yield_pos->SetTitle(q2xz_str.c_str());
          mg_z_yield_pos->GetYaxis()->SetTitle("yield");
          mg_z_yield_pos->GetXaxis()->SetTitle("z");
          mg_z_yield_pos->Draw("AL");
          c_yield_pos->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_pos_name = "results/yield/statistics_corr/yield_"+q2xz_str_filename+"_pos.pdf";
          c_yield_pos->SaveAs(c_yield_pos_name.c_str());
          
          TGraphErrors* g_z_neg_yield = new TGraphErrors();
          g_z_neg_yield->SetTitle("pi+ data yield");
          TGraphErrors* g_z_neg_Dummy_yield = new TGraphErrors();
          g_z_neg_Dummy_yield->SetTitle("pi+ Dummy yield");
          TGraphErrors* g_z_neg_yield_corr = new TGraphErrors();
          g_z_neg_yield_corr->SetTitle("pi- data yield");
          int nbins_yield_neg = bins;
          int ii_yield_neg = 0;
          for(int i = 0;i<nbins_yield_neg;i++){
            double x = x_axis[i]; 
            double y = neg_D2_yields[i];
            double y_dummy = neg_Dummy_yields[i]; 
            double error = neg_D2_errs[i]; 
            double error_dummy = neg_Dummy_errs[i]; 
            double y_corr = y-0.245*y_dummy;
            double error_corr = sqrt(error*error+0.245*0.245*error_dummy*error_dummy);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              g_z_neg_yield->SetPoint(ii_yield_neg,x,y);
              g_z_neg_yield->SetPointError(ii_yield_neg,0,error);
              g_z_neg_Dummy_yield->SetPoint(ii_yield_neg,x,y_dummy);
              g_z_neg_Dummy_yield->SetPointError(ii_yield_neg,0,error_dummy);
              g_z_neg_yield_corr->SetPoint(ii_yield_neg,x,y_corr);
              g_z_neg_yield_corr->SetPointError(ii_yield_neg,0,error_corr);
              ii_yield_neg++;
            }
          }
          TCanvas *c_yield_neg = new TCanvas();
          gStyle->SetOptTitle(0);
          TMultiGraph* mg_z_yield_neg = new TMultiGraph(); 
          g_z_neg_yield->GetYaxis()->SetTitle("yield");
          g_z_neg_yield->GetYaxis()->SetTitleSize(0.53);
          g_z_neg_yield->GetXaxis()->SetRangeUser(0.3,1);
          g_z_neg_yield->SetMarkerStyle(4);
          mg_z_yield_neg->Add(g_z_neg_yield,"P");
          g_z_neg_Dummy_yield->SetMarkerStyle(4);
          g_z_neg_Dummy_yield->SetMarkerColor(kRed);
          g_z_neg_Dummy_yield->SetTitle("data Dummy_yield");
          mg_z_yield_neg->Add(g_z_neg_Dummy_yield,"P");
          g_z_neg_yield_corr->SetMarkerStyle(4);
          g_z_neg_yield_corr->SetMarkerColor(kRed);
          g_z_neg_yield_corr->SetTitle("data yield final");
          mg_z_yield_neg->Add(g_z_neg_yield_corr,"P");
          TGraph* g_z_neg_sim = new TGraph(h_z_neg_sim);
          TGraph* g_z_neg_sim_incrad = new TGraph(h_z_neg_sim_incrad);
          TGraph* g_z_neg_sim_incnorad = new TGraph(h_z_neg_sim_incnorad);
          TGraph* g_z_neg_sim_excrad = new TGraph(h_z_neg_sim_excrad);
          TGraph* g_z_neg_sim_rho = new TGraph(h_z_neg_sim_rho);
          TGraph* g_z_neg_sim_delta = new TGraph(h_z_neg_sim_delta);
          mg_z_yield_neg->Add(g_z_neg_sim); 
          g_z_neg_sim_incrad->SetLineColor(kOrange);
          g_z_neg_sim_incrad->SetTitle("pi+ sim SIDIS");
          mg_z_yield_neg->Add(g_z_neg_sim_incrad); 
          g_z_neg_sim_excrad->SetLineColor(kBlue);
          g_z_neg_sim_excrad->SetTitle("pi+ sim exc");
          mg_z_yield_neg->Add(g_z_neg_sim_excrad); 
          g_z_neg_sim_rho->SetLineColor(kRed);
          g_z_neg_sim_rho->SetTitle("pi+ sim rho");
          mg_z_yield_neg->Add(g_z_neg_sim_rho); 
          g_z_neg_sim_delta->SetLineColor(46);
          g_z_neg_sim_delta->SetTitle("pi+ sim delta");
          mg_z_yield_neg->Add(g_z_neg_sim_delta); 
          g_z_neg_sim_incnorad->SetLineColor(40);
          g_z_neg_sim_incnorad->SetTitle("pi+ sim inc norad");
          mg_z_yield_neg->Add(g_z_neg_sim_incnorad,"L");
          mg_z_yield_neg->SetTitle(q2xz_str.c_str());
          mg_z_yield_neg->GetYaxis()->SetTitle("yield");
          mg_z_yield_neg->GetXaxis()->SetTitle("z");
          mg_z_yield_neg->Draw("AL");
          c_yield_neg->BuildLegend(0.75,0.75,1,1);
          std::string c_yield_neg_name = "results/yield/statistics_corr/yield_"+q2xz_str_filename+"_neg.pdf";
          c_yield_neg->SaveAs(c_yield_neg_name.c_str());


          //delete until here
          /* 
          h_z_neg_sim_incnorad->Divide(h_z_pos_sim_incnorad);

          int nbins = bins 

          // TGraphErrors* g_yield_ratio = new TGraphErrors(h_z_neg_all);
          TGraphErrors* g_yield_ratio = new TGraphErrors();
          std::string z_string = "R_Y z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio->SetName(z_string.c_str());
          TGraphErrors* g_frag_ratio = new TGraphErrors();
          std::string frag_z_string = "frag z setting "+(std::to_string(z)).substr(0,4);
          g_frag_ratio->SetName(frag_z_string.c_str());
          TGraphErrors* g_RDmeas_ratio = new TGraphErrors();
          std::string RDmeas_z_string = "R^D_meas z setting "+(std::to_string(z)).substr(0,4);
          g_RDmeas_ratio->SetName(RDmeas_z_string.c_str());

          int ii = 0;
          for(int i = 0;i<nbins;i++){
            //std::cout<<i<<std::endl;
            double x = x_axis[i]
            double y_neg_D2 = neg_D2_yields[i]; 
            double y_neg_Dummy = neg_Dummy_yields[i]; 
            double y_neg_delta = h_z_neg_sim_delta->GetBinContent(i+1);
            double y_neg_exc = h_z_neg_sim_excrad->GetBinContent(i+1);
            double y_neg = y_neg_D2-y_neg_bg_D2-0.245*y_neg_Dummy-y_neg_delta-y_neg_exc;
            double error_neg_D2 = h_z_neg_all->GetBinError(i+1)/charge_neg_all;
            double error_neg_bg_D2 = h_z_neg_bg_all->GetBinError(i+1)/charge_neg_all;
            double error_neg_Dummy = h_z_neg_Dummy_all->GetBinError(i+1)/charge_neg_Dummy_all;
            double error_neg = std::sqrt(error_neg_D2*error_neg_D2+error_neg_bg_D2*error_neg_bg_D2+0.245*0.245*error_neg_Dummy*error_neg_Dummy);
            double y_pos_D2 = h_z_pos_all->GetBinContent(i+1)/charge_pos_all;
            double y_pos_bg_D2 = h_z_pos_bg_all->GetBinContent(i+1)/charge_pos_all;
            double y_pos_Dummy = h_z_pos_Dummy_all->GetBinContent(i+1)/charge_pos_Dummy_all;
            double y_pos_delta = h_z_pos_sim_delta->GetBinContent(i+1);
            double y_pos_exc = h_z_pos_sim_excrad->GetBinContent(i+1);
            double y_pos = y_pos_D2-y_pos_bg_D2-0.245*y_pos_Dummy-y_pos_delta-y_pos_exc;
            double error_pos_D2 = h_z_pos_all->GetBinError(i+1)/charge_pos_all;
            double error_pos_bg_D2 = h_z_pos_bg_all->GetBinError(i+1)/charge_pos_all;
            double error_pos_Dummy = h_z_pos_Dummy_all->GetBinError(i+1)/charge_pos_Dummy_all;
            double error_pos = std::sqrt(error_pos_D2*error_pos_D2+error_pos_bg_D2*error_pos_bg_D2+0.245*0.245*error_pos_Dummy*error_pos_Dummy);
            double radia_corr_neg = rp_radia_corr_neg->GetBinContent(i+1);
            double radia_corr_pos = rp_radia_corr_pos->GetBinContent(i+1);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl; rp_radia_corr_pos
            //This is for yield
            y_neg = y_neg*radia_corr_neg;
            y_pos = y_pos*radia_corr_pos;
            error_pos = error_pos*radia_corr_pos;
            error_neg = error_neg*radia_corr_neg;

            //This convert yield to cross sectio,choose yield or xsn
            //double D2_xs_z_neg,D2_xs_z_pos;
            //D2_xs_z_neg = h_xs_z_neg_sim->GetYaxis()->GetBinCenter(i);
            //D2_xs_z_pos = h_xs_z_pos_sim->GetYaxis()->GetBinCenter(i);
            //double D2_neg_yield_incrad = h_z_neg_sim_incrad->GetBinContent(i);
            //double D2_pos_yield_incrad = h_z_pos_sim_incrad->GetBinContent(i);
            //y_pos = y_D2_pos*D2_xs_z_pos/D2_pos_yield_incrad;
            //y_neg = y_D2_neg*D2_xs_z_neg/D2_neg_yield_incrad;
            //error_pos = error_pos*D2_xs_z_pos/D2_pos_yield_incrad;
            //error_neg = error_neg*D2_xs_z_neg/D2_neg_yield_incrad;


            double y = (y_neg)/(y_pos);
            double error = (y_neg/y_pos)*std::sqrt((error_neg*error_neg)/(y_neg*y_neg)+(error_pos*error_pos)/(y_pos*y_pos));
            double y_RD = (4*y-1)/(1-y);
            double error_RD = 3*error/((1-y)*(1-y));
            double y_frag = (4*y-1)/(4-y);
            double error_frag = 4*y_frag*std::sqrt((error*error)/(y*y));
            if(y>0 && error< 0.1){
              //double y_RD = (4*y-1)/(1-y);
              g_yield_ratio->SetPoint(ii,x,y);
              //g_yield_ratio->SetPoint(ii,x,y_RD);
              g_yield_ratio->SetPointError(ii,0,error);
              g_frag_ratio ->SetPoint(ii,x,y_frag);
              g_frag_ratio->SetPointError(ii,0,error_frag);
              g_RDmeas_ratio->SetPoint(ii,x,y_RD);
              g_RDmeas_ratio->SetPointError(ii,0,error_RD);
              ii++;
              std::string z_str = std::to_string(x); 
              if(y_RD>0 && error_RD < 1){
                jout[std::to_string(Q2)][std::to_string(xbj)][z_str]["value"].push_back(y_RD);
                jout[std::to_string(Q2)][std::to_string(xbj)][z_str]["error"].push_back(error_RD);
              }
            }
          }

          int nbins_sim = h_z_neg_sim_incnorad->GetXaxis()->GetNbins();
          TGraphErrors* g_yield_ratio_sim = new TGraphErrors();
          std::string z_string_sim = "z simc "+(std::to_string(z)).substr(0,4);
          g_yield_ratio_sim->SetName(z_string_sim.c_str());
          //TGraphErrors* g_yield_ratio_sim = new TGraphErrors(h_z_neg_sim_incnorad);
          int ii_sim = 0;
          for(int i = 0;i<nbins_sim;i++){
            //std::cout<<i<<std::endl;
            double x = h_z_neg_sim_incnorad->GetBinCenter(i+1);//+0.001*i_color;
            double y = h_z_neg_sim_incnorad->GetBinContent(i+1);
            double error = h_z_neg_sim_incnorad->GetBinError(i+1);
            //std::cout<<i<<" x "<<x<<" y "<<y<<std::endl;
            if(y!=0){
              //      double y_RD = (4*y-1)/(1-y);
              //       g_yield_ratio_sim->SetPoint(ii_sim,x,y_RD);
              g_yield_ratio_sim->SetPoint(ii_sim,x,y);
              //g_yield_ratio_sim->SetPointError(ii_sim,0,error);
              ii_sim++;
            }
          }

          g_yield_ratio->SetName(q2xz_str_filename.c_str());
          TFile* yield_ratio_file = new TFile("results/yield_ratio_all.root","UPDATE");
          g_yield_ratio->Write();
          yield_ratio_file->Close();  
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



          //c_Q2x_ratio->cd();
          //h_z_neg_all->SetLineColor(i_color);
          g_yield_ratio->SetMarkerStyle(4);
          g_yield_ratio->SetMarkerColor(i_color);
          g_yield_ratio->SetLineColor(i_color);
          g_yield_ratio_sim->SetMarkerStyle(5);
          g_yield_ratio_sim->SetMarkerColorAlpha(i_color,0.35);
          g_yield_ratio_sim->SetLineColor(i_color);
          //hs->Add(h_z_neg_all);
          //h_z_neg_all->Draw("same");
          mg->Add(g_yield_ratio,"P");
          mg->Add(g_yield_ratio_sim,"L");
          g_frag_ratio->SetMarkerStyle(4);
          g_frag_ratio->SetMarkerColor(i_color);
          g_frag_ratio->SetLineColor(i_color);
          mg_frag->Add(g_frag_ratio,"P");
          g_RDmeas_ratio->SetMarkerStyle(4);
          g_RDmeas_ratio->SetMarkerColor(i_color);
          g_RDmeas_ratio->SetLineColor(i_color);
          mg_RD->Add(g_RDmeas_ratio,"P");
          //c_Q2x_ratio->Update();


          TCanvas *c_Q2x_z_ratio = new TCanvas(q2x_name.c_str(),q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
          //TCanvas *c_Q2x_z_ratio = new TCanvas();
          // //h_z_neg_all->Draw();
          g_yield_ratio->GetXaxis()->SetRangeUser(0.1,1);
          //g_yield_ratio->GetYaxis()->SetRangeUser(0.1,1.2);
          g_yield_ratio->GetXaxis()->SetTitle("z");
          g_yield_ratio->GetYaxis()->SetTitle("yield_ratio");
          g_yield_ratio->GetXaxis()->SetTitleSize(0.053);
          g_yield_ratio->GetYaxis()->SetTitleSize(0.053);
          g_yield_ratio->Draw("AP"); 
          g_yield_ratio_sim->Draw("L");
          c_Q2x_z_ratio->BuildLegend(0.1,0.1,0.5,0.2,q2xz_str.c_str());
          std::string zratiopdfname = "results/yield/statistics_corr/"+q2xz_str_filename+"_ratio.pdf";

          c_Q2x_z_ratio->SaveAs(zratiopdfname.c_str());

          TGraphErrors* g_yield_ratio_all = new TGraphErrors();
          std::string z_string_all = "z setting "+(std::to_string(z)).substr(0,4);
          g_yield_ratio_all->SetName(z_string_all.c_str());

          // int ii_all = 0;
          //for(int i = 0;i<nbins;i++){
          //  //std::cout<<i<<std::endl;
          //  double x = h_z_neg_all->GetBinCenter(i)+0.001*i_color+0.6*i_which_x[i_whichx];
          //  double y = h_z_neg_all->GetBinContent(i)+i_which_Q2[i_whichx];
          //  double error = h_z_neg_all->GetBinError(i);
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
*/
        }//loop over z
        //i_whichx++;
        //TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
        //c_Q2x_ratio->cd();
        //c_Q2x_ratio->SetTitle(canvas_name.c_str());
        ////hs->Draw();
        ////TF1 *f_RD = new TF1("f1","2.5/x-2.5",0,1);
        //mg->GetXaxis()->SetTitleSize(0.053);
        //mg->GetYaxis()->SetTitleSize(0.053);
        //mg->GetXaxis()->SetLabelSize(0.05);
        //mg->GetYaxis()->SetLabelSize(0.05);
        //mg->SetMinimum(0.3);
        //mg->SetMaximum(0.9);
        //std::string mg_title = canvas_name+",z";
        //mg->SetTitle(mg_title.c_str());
        ////TPaveText *pt_mg = new TPaveText(0.5,0.8,0.8,1);
        ////pt_mg->AddText(canvas_name.c_str());
        ////pt_mg->Draw("same");
        //mg->Draw("APL");
        ////f_RD->Draw("same");
        //mg->GetHistogram()->SetTitle(canvas_name.c_str());
        //mg->GetXaxis()->SetTitle(mg_title.c_str());
        //mg->GetXaxis()->SetLimits(0.3,0.8);
        ////auto hermes_RD = [](double z){return ((1.0-z)*0.083583)/((1.0+z)*1.988);};
        ////TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0,1);
        ////fit->Draw("same");
        ////      std::string ratiopdfname = "results/yield/statistics_corr/"+canvas_filename+"_RDratio.pdf";
        //std::string ratiopdfname = "results/yield/statistics_corr/"+canvas_filename+"_ratio.pdf";
        ////c_Q2x_ratio->BuildLegend(0.6,0.6,0.95,0.95);
        //c_Q2x_ratio->SaveAs(ratiopdfname.c_str());
        //TCanvas* c_frag_ratio = new TCanvas("","",1900,1000);
        //mg_frag->GetXaxis()->SetTitleSize(0.053);
        //mg_frag->GetYaxis()->SetTitleSize(0.053);
        //mg_frag->GetXaxis()->SetLabelSize(0.05);
        //mg_frag->GetYaxis()->SetLabelSize(0.05);
        ////mg_frag->SetMinimum(0.1);
        ////mg_frag->SetMaximum(0.8);
        //std::string mg_frag_title = canvas_name+",z";
        //mg_frag->SetTitle(mg_frag_title.c_str());
        //TPaveText *pt_mg_frag = new TPaveText(0.5,0.8,0.8,1);
        //pt_mg_frag->AddText(canvas_name.c_str());
        //pt_mg_frag->Draw("same");
        //mg_frag->Draw("APL");
        //mg_frag->GetHistogram()->SetTitle(canvas_name.c_str());
        //mg_frag->GetXaxis()->SetTitle(mg_frag_title.c_str());
        //mg_frag->GetYaxis()->SetTitle("frag_Ratio");
        //mg_frag->GetXaxis()->SetLimits(0.3,0.8);
        //std::string fragpdfname = "results/yield/statistics_corr/"+canvas_filename+"_fragratio.pdf";
        //c_frag_ratio->BuildLegend(0.7,0.7,0.9,0.9);
        //c_frag_ratio->SaveAs(fragpdfname.c_str());
        //TCanvas* c_RD_ratio = new TCanvas();
        //mg_RD->GetXaxis()->SetTitleSize(0.053);
        //mg_RD->GetYaxis()->SetTitleSize(0.053);
        //mg_RD->GetXaxis()->SetLabelSize(0.05);
        //mg_RD->GetYaxis()->SetLabelSize(0.05);
        //mg_RD->SetMinimum(0);
        //mg_RD->SetMaximum(10);
        //std::string mg_RD_title = canvas_name+",z";
        //mg_RD->SetTitle(mg_RD_title.c_str());
        //TPaveText *pt_mg_RD = new TPaveText(0.5,0.8,0.8,1);
        //pt_mg_RD->AddText(canvas_name.c_str());
        //pt_mg_RD->Draw("same");
        //mg_RD->Draw("APL");
        //mg_RD->GetHistogram()->SetTitle(canvas_name.c_str());
        //mg_RD->GetXaxis()->SetTitle(mg_RD_title.c_str());
        //mg_RD->GetYaxis()->SetTitle("RD_Ratio");

        //mg_RD->GetXaxis()->SetLimits(0.3,0.8);

        //std::string RDpdfname = "results/yield/statistics_corr/"+canvas_filename+"_RDratio.pdf";
        //c_RD_ratio->BuildLegend(0.7,0.7,0.9,0.9);
        //c_RD_ratio->SaveAs(RDpdfname.c_str());
      }//if x,Q2 not 0

      //i_whichq2++;
      //      std::cout<<i_whichx<<std::endl;
    }//loop over Q2
  }//loop over x

  //std::string jout_name = "results/yield_ratio.json";
  //std::ofstream ofs_jout(jout_name.c_str());
  //ofs_jout<<jout.dump(4)<<std::endl;
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
