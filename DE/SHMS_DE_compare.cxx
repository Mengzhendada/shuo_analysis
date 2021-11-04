#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "TGraphErrors.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TStyle.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void SHMS_DE_compare(){

  // RunGroup = RunGroup;
  json j_rungroup;
  {
    std::ifstream infile("db2/ratio_run_group_updated.json");
    infile>>j_rungroup;
  }
  std::cout<<"check"<<std::endl;

  //I already write cuts into 
  json j_cuts;
  {
    std::string infilename = "db2/PID_test.json";
    std::ifstream infile(infilename.c_str());
    infile>>j_cuts;
  }
  json j_runsinfo;
  {
    std::ifstream infile("db2/runs_info.json");
    infile>>j_runsinfo;
  }
  json jout;
  //int rf_e_n = 10;
  double cal_pi_n = 0.8;
  double aero_pi_n = 2;
  //std::cout<<"rf electron cut is 10, cal electron cut is 0.8"<<std::endl;
  
  TGraphErrors* g_DE_cal_momentum_neg_fall = new TGraphErrors();
  g_DE_cal_momentum_neg_fall->SetTitle("calorimeter,rfcut,cal_pi<0.8;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_neg_fall = new TGraphErrors();
  g_DE_cal_RunGroup_neg_fall->SetTitle("calorimeter,rfcut,cal_pi<0.8;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_neg_fall = new TGraphErrors();
  g_DE_cal_RunNumber_neg_fall->SetTitle("rfcut,cal_pi<0.8;RunNumber;efficiency");
  TGraphErrors* g_DE_cal_momentum_pos_fall = new TGraphErrors();
  g_DE_cal_momentum_pos_fall->SetTitle("rfcut,cal_pi<0.8;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_pos_fall = new TGraphErrors();
  g_DE_cal_RunGroup_pos_fall->SetTitle("rfcut,cal_pi<0.8;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_pos_fall = new TGraphErrors();
  g_DE_cal_RunNumber_pos_fall->SetTitle("rfcut,cal_pi<0.8;RunNumber;efficiency");
  TGraphErrors* g_DE_aero_momentum_neg_fall = new TGraphErrors();
  g_DE_aero_momentum_neg_fall->SetTitle("aero,rfcut,aero_pi>2;momentum;efficiency");
  TGraphErrors* g_DE_aero_RunGroup_neg_fall = new TGraphErrors();
  g_DE_aero_RunGroup_neg_fall->SetTitle("aero,rfcut,aero_pi>2;RunGroup;efficiency");
  TGraphErrors* g_DE_aero_RunNumber_neg_fall = new TGraphErrors();
  g_DE_aero_RunNumber_neg_fall->SetTitle("rfcut,aero_pi>2;RunNumber;efficiency");
  TGraphErrors* g_DE_aero_momentum_pos_fall = new TGraphErrors();
  g_DE_aero_momentum_pos_fall->SetTitle("rfcut,aero_pi>2;momentum;efficiency");
  TGraphErrors* g_DE_aero_RunGroup_pos_fall = new TGraphErrors();
  g_DE_aero_RunGroup_pos_fall->SetTitle("rfcut,aero_pi>2;RunGroup;efficiency");
  TGraphErrors* g_DE_aero_RunNumber_pos_fall = new TGraphErrors();
  g_DE_aero_RunNumber_pos_fall->SetTitle("rfcut,aero_pi>2;RunNumber;efficiency");
  TGraphErrors* g_DE_cal_momentum_neg_spring = new TGraphErrors();
  g_DE_cal_momentum_neg_spring->SetTitle("calorimeter,rfcut,cal_pi<0.8;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_neg_spring = new TGraphErrors();
  g_DE_cal_RunGroup_neg_spring->SetTitle("calorimeter,rfcut,cal_pi<0.8;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_neg_spring = new TGraphErrors();
  g_DE_cal_RunNumber_neg_spring->SetTitle("rfcut,cal_pi<0.8;RunNumber;efficiency");
  TGraphErrors* g_DE_cal_momentum_pos_spring = new TGraphErrors();
  g_DE_cal_momentum_pos_spring->SetTitle("rfcut,cal_pi<0.8;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_pos_spring = new TGraphErrors();
  g_DE_cal_RunGroup_pos_spring->SetTitle("rfcut,cal_pi<0.8;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_pos_spring = new TGraphErrors();
  g_DE_cal_RunNumber_pos_spring->SetTitle("rfcut,cal_pi<0.8;RunNumber;efficiency");
  TGraphErrors* g_DE_aero_momentum_neg_spring = new TGraphErrors();
  g_DE_aero_momentum_neg_spring->SetTitle("aero,rfcut,aero_pi>2;momentum;efficiency");
  TGraphErrors* g_DE_aero_RunGroup_neg_spring = new TGraphErrors();
  g_DE_aero_RunGroup_neg_spring->SetTitle("aero,rfcut,aero_pi>2;RunGroup;efficiency");
  TGraphErrors* g_DE_aero_RunNumber_neg_spring = new TGraphErrors();
  g_DE_aero_RunNumber_neg_spring->SetTitle("rfcut,aero_pi>2;RunNumber;efficiency");
  TGraphErrors* g_DE_aero_momentum_pos_spring = new TGraphErrors();
  g_DE_aero_momentum_pos_spring->SetTitle("rfcut,aero_pi>2;momentum;efficiency");
  TGraphErrors* g_DE_aero_RunGroup_pos_spring = new TGraphErrors();
  g_DE_aero_RunGroup_pos_spring->SetTitle("rfcut,aero_pi>2;RunGroup;efficiency");
  TGraphErrors* g_DE_aero_RunNumber_pos_spring = new TGraphErrors();
  g_DE_aero_RunNumber_pos_spring->SetTitle("rfcut,aero_pi>2;RunNumber;efficiency");
  //int i_cal_momentum = 0,i_cal_RunGroup = 0,i_cal_RunNumber = 0,i_rf_momentum = 0,i_rf_RunGroup = 0,i_rf_RunNumber = 0;
  int i_de_neg_fall = 0,i_de_pos_fall = 0,i_de_neg_spring = 0, i_de_pos_spring = 0; 
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    double hms_p = -j_rungroup[(std::to_string(RunGroup)).c_str()]["hms_p"].get<double>();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){ 
      std::cout<<RunGroup<<std::endl;
//      pos_D2.insert(pos_D2.end(),neg_D2.begin(),neg_D2.end());
      //for pos runs
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        //calorimeter runs part
        json j_DE;
        {
          std::string infilename = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
          std::cout<<"json file "<<infilename<<std::endl;
          std::ifstream infile(infilename.c_str());
          infile>>j_DE;
        }
        double cal_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
        double aero_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_all"].get<double>();
        std::vector<double> cal_cuts;
        std::vector<double> aero_cuts;
        std::vector<double> aero_pi,cal_pi;
        cal_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["calcuts"].get<std::vector<double>>();
        aero_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["aero_cuts"].get<std::vector<double>>();
        auto calcut_it = find(cal_cuts.begin(),cal_cuts.end(),cal_pi_n);
        int index_cal = calcut_it-cal_cuts.begin();
        cal_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_calcuts"].get<std::vector<double>>();
        auto aerocut_it = find(aero_cuts.begin(),aero_cuts.end(),aero_pi_n);
        int index_aero = aerocut_it-aero_cuts.begin();
        aero_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_aerocuts"].get<std::vector<double>>();
        if(cal_pi_all!=0 && aero_pi_all!=0){
          double cal_DE = cal_pi[index_cal]/cal_pi_all;
          double cal_DE_error = 1/cal_pi_all*sqrt(cal_pi[index_cal]*(1-cal_DE));
          std::cout<<"calDE "<<cal_DE<<" error "<<cal_DE_error<<std::endl;
          
          double aero_DE = aero_pi[index_aero]/aero_pi_all;
          double aero_DE_error = 1/aero_pi_all*sqrt(aero_pi[index_aero]*(1-aero_DE));
          std::cout<<"aeroDE "<<aero_DE<<" error "<<aero_DE_error<<std::endl;
         
          if(RunNumber < 7000){
          g_DE_cal_momentum_pos_fall->SetPoint(i_de_pos_fall,hms_p,cal_DE);
          g_DE_cal_momentum_pos_fall->SetPointError(i_de_pos_fall,0,cal_DE_error);
          g_DE_cal_RunGroup_pos_fall->SetPoint(i_de_pos_fall,RunGroup,cal_DE);
          g_DE_cal_RunGroup_pos_fall->SetPointError(i_de_pos_fall,0,cal_DE_error);
          g_DE_cal_RunNumber_pos_fall->SetPoint(i_de_pos_fall,RunNumber,cal_DE);
          g_DE_cal_RunNumber_pos_fall->SetPointError(i_de_pos_fall,0,cal_DE_error);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_cal_DE_pos"] = cal_DE;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_cal_DE_pos_err"] = cal_DE_error;
          
          g_DE_aero_momentum_pos_fall->SetPoint(i_de_pos_fall,hms_p,aero_DE);
          g_DE_aero_momentum_pos_fall->SetPointError(i_de_pos_fall,0,aero_DE_error);
          g_DE_aero_RunGroup_pos_fall->SetPoint(i_de_pos_fall,RunGroup,aero_DE);
          g_DE_aero_RunGroup_pos_fall->SetPointError(i_de_pos_fall,0,aero_DE_error);
          g_DE_aero_RunNumber_pos_fall->SetPoint(i_de_pos_fall,RunNumber,aero_DE);
          g_DE_aero_RunNumber_pos_fall->SetPointError(i_de_pos_fall,0,aero_DE_error);
          i_de_pos_fall++;
          }
          else
          {
          g_DE_cal_momentum_pos_spring->SetPoint(i_de_pos_spring,hms_p,cal_DE);
          g_DE_cal_momentum_pos_spring->SetPointError(i_de_pos_spring,0,cal_DE_error);
          g_DE_cal_RunGroup_pos_spring->SetPoint(i_de_pos_spring,RunGroup,cal_DE);
          g_DE_cal_RunGroup_pos_spring->SetPointError(i_de_pos_spring,0,cal_DE_error);
          g_DE_cal_RunNumber_pos_spring->SetPoint(i_de_pos_spring,RunNumber,cal_DE);
          g_DE_cal_RunNumber_pos_spring->SetPointError(i_de_pos_spring,0,cal_DE_error);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_cal_DE_pos"] = cal_DE;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_cal_DE_pos_err"] = cal_DE_error;
          
          g_DE_aero_momentum_pos_spring->SetPoint(i_de_pos_spring,hms_p,aero_DE);
          g_DE_aero_momentum_pos_spring->SetPointError(i_de_pos_spring,0,aero_DE_error);
          g_DE_aero_RunGroup_pos_spring->SetPoint(i_de_pos_spring,RunGroup,aero_DE);
          g_DE_aero_RunGroup_pos_spring->SetPointError(i_de_pos_spring,0,aero_DE_error);
          g_DE_aero_RunNumber_pos_spring->SetPoint(i_de_pos_spring,RunNumber,aero_DE);
          g_DE_aero_RunNumber_pos_spring->SetPointError(i_de_pos_spring,0,aero_DE_error);
          i_de_pos_spring++;
          }
        }
        else{std::cout<<"failed to calculate DE for run "<<RunNumber<<std::endl;}
      }//for pos runs
      
      //for neg runs
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;
        //calorimeter runs part
        json j_DE;
        {
          std::string infilename = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
          std::cout<<"json file "<<infilename<<std::endl;
          std::ifstream infile(infilename.c_str());
          infile>>j_DE;
        }
        double cal_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
        double aero_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_all"].get<double>();
        std::vector<double> cal_cuts;
        std::vector<double> aero_cuts;
        std::vector<double> aero_pi,cal_pi;
        cal_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["calcuts"].get<std::vector<double>>();
        aero_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["aero_cuts"].get<std::vector<double>>();
        auto calcut_it = find(cal_cuts.begin(),cal_cuts.end(),cal_pi_n);
        int index_cal = calcut_it-cal_cuts.begin();
        cal_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_calcuts"].get<std::vector<double>>();
        auto aerocut_it = find(aero_cuts.begin(),aero_cuts.end(),aero_pi_n);
        int index_aero = aerocut_it-aero_cuts.begin();
        aero_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_aerocuts"].get<std::vector<double>>();
        if(cal_pi_all!=0 && aero_pi_all!=0){
          double cal_DE = cal_pi[index_cal]/cal_pi_all;
          double cal_DE_error = 1/cal_pi_all*sqrt(cal_pi[index_cal]*(1-cal_DE));
          std::cout<<"calDE "<<cal_DE<<" error "<<cal_DE_error<<std::endl;
          
          double aero_DE = aero_pi[index_aero]/aero_pi_all;
          double aero_DE_error = 1/aero_pi_all*sqrt(aero_pi[index_aero]*(1-aero_DE));
          std::cout<<"aeroDE "<<aero_DE<<" error "<<aero_DE_error<<std::endl;
          
          if(RunNumber < 7000){
          g_DE_cal_momentum_neg_fall->SetPoint(i_de_neg_fall,hms_p,cal_DE);
          g_DE_cal_momentum_neg_fall->SetPointError(i_de_neg_fall,0,cal_DE_error);
          g_DE_cal_RunGroup_neg_fall->SetPoint(i_de_neg_fall,RunGroup,cal_DE);
          g_DE_cal_RunGroup_neg_fall->SetPointError(i_de_neg_fall,0,cal_DE_error);
          g_DE_cal_RunNumber_neg_fall->SetPoint(i_de_neg_fall,RunNumber,cal_DE);
          g_DE_cal_RunNumber_neg_fall->SetPointError(i_de_neg_fall,0,cal_DE_error);
          
          g_DE_aero_momentum_neg_fall->SetPoint(i_de_neg_fall,hms_p,aero_DE);
          g_DE_aero_momentum_neg_fall->SetPointError(i_de_neg_fall,0,aero_DE_error);
          g_DE_aero_RunGroup_neg_fall->SetPoint(i_de_neg_fall,RunGroup,aero_DE);
          g_DE_aero_RunGroup_neg_fall->SetPointError(i_de_neg_fall,0,aero_DE_error);
          g_DE_aero_RunNumber_neg_fall->SetPoint(i_de_neg_fall,RunNumber,aero_DE);
          g_DE_aero_RunNumber_neg_fall->SetPointError(i_de_neg_fall,0,aero_DE_error);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_aero_DE_neg"] = aero_DE;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_aero_DE_neg_err"] = cal_DE_error;
          i_de_neg_fall++;
          }
          else
          {
          g_DE_cal_momentum_neg_spring->SetPoint(i_de_neg_spring,hms_p,cal_DE);
          g_DE_cal_momentum_neg_spring->SetPointError(i_de_neg_spring,0,cal_DE_error);
          g_DE_cal_RunGroup_neg_spring->SetPoint(i_de_neg_spring,RunGroup,cal_DE);
          g_DE_cal_RunGroup_neg_spring->SetPointError(i_de_neg_spring,0,cal_DE_error);
          g_DE_cal_RunNumber_neg_spring->SetPoint(i_de_neg_spring,RunNumber,cal_DE);
          g_DE_cal_RunNumber_neg_spring->SetPointError(i_de_neg_spring,0,cal_DE_error);
          
          g_DE_aero_momentum_neg_spring->SetPoint(i_de_neg_spring,hms_p,aero_DE);
          g_DE_aero_momentum_neg_spring->SetPointError(i_de_neg_spring,0,aero_DE_error);
          g_DE_aero_RunGroup_neg_spring->SetPoint(i_de_neg_spring,RunGroup,aero_DE);
          g_DE_aero_RunGroup_neg_spring->SetPointError(i_de_neg_spring,0,aero_DE_error);
          g_DE_aero_RunNumber_neg_spring->SetPoint(i_de_neg_spring,RunNumber,aero_DE);
          g_DE_aero_RunNumber_neg_spring->SetPointError(i_de_neg_spring,0,aero_DE_error);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_aero_DE_neg"] = aero_DE;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(RunNumber)).c_str()]["SHMS_aero_DE_neg_err"] = cal_DE_error;
          i_de_neg_spring++;
          }
        }
        else{std::cout<<"failed to calculate DE for run "<<RunNumber<<std::endl;}
      }//for neg runs
      

    }//if normal production runs
  }//loop over rungroups
  std::cout<< "Done check"<<std::endl;
  auto mg_cal = new TMultiGraph();
  g_DE_cal_momentum_pos_fall->SetMarkerStyle(8);
  g_DE_cal_momentum_pos_fall->SetMarkerColor(kRed);
  g_DE_cal_momentum_pos_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  mg_cal->Add(g_DE_cal_momentum_pos_fall,"P");
  g_DE_cal_momentum_pos_spring->SetMarkerStyle(4);
  g_DE_cal_momentum_pos_spring->SetMarkerColor(kRed);
  g_DE_cal_momentum_pos_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  mg_cal->Add(g_DE_cal_momentum_pos_spring,"P");
  
  TCanvas *c_cal_DE_momentum = new TCanvas();
  //g_DE_cal_momentum_pos_fall->SetMarkerStyle(8);
  //g_DE_cal_momentum_pos_fall->SetMarkerColor(kRed);
  //g_DE_cal_momentum_pos_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  //g_DE_cal_momentum_pos_fall->Draw("AP");
  //g_DE_cal_momentum_pos_spring->SetMarkerStyle(4);
  //g_DE_cal_momentum_pos_spring->SetMarkerColor(kRed);
  //g_DE_cal_momentum_pos_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  //g_DE_cal_momentum_pos_spring->Draw("P same");
  //g_DE_cal_momentum_neg_fall->SetMarkerStyle(8);
  //g_DE_cal_momentum_neg_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  //g_DE_cal_momentum_neg_fall->Draw("P same");
  //g_DE_cal_momentum_neg_spring->SetMarkerStyle(8);
  //g_DE_cal_momentum_neg_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  //g_DE_cal_momentum_neg_spring->Draw("P same");
  mg_cal->Draw("AP");
  mg_cal->Fit("pol0","F");
  TF1 *Fit_cal = mg_cal->GetFunction("pol0");
  Fit_cal->SetLineColor(kRed);
  Fit_cal->Draw("L same");
  gStyle->SetOptFit(0001);
  mg_cal->GetXaxis()->SetTitle("Momentum");
  mg_cal->GetYaxis()->SetTitle("cal Eff");
  c_cal_DE_momentum->SaveAs("results/pid/SHMS_cal_DE_momentum.pdf");
  TCanvas *c_cal_DE_RunGroup = new TCanvas();
  g_DE_cal_RunGroup_pos_fall->SetMarkerStyle(8);
  g_DE_cal_RunGroup_pos_fall->SetMarkerColor(kRed);
  g_DE_cal_RunGroup_pos_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunGroup_pos_fall->Draw("AP");
  g_DE_cal_RunGroup_pos_spring->SetMarkerStyle(8);
  g_DE_cal_RunGroup_pos_spring->SetMarkerColor(kRed);
  g_DE_cal_RunGroup_pos_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunGroup_pos_spring->Draw("P same");
  g_DE_cal_RunGroup_neg_fall->SetMarkerStyle(8);
  g_DE_cal_RunGroup_neg_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunGroup_neg_fall->Draw("P same");
  g_DE_cal_RunGroup_neg_spring->SetMarkerStyle(8);
  g_DE_cal_RunGroup_neg_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunGroup_neg_spring->Draw("P same");
  c_cal_DE_RunGroup->SaveAs("results/pid/SHMS_cal_DE_RunGroup.pdf");
  TCanvas *c_cal_DE_RunNumber_fall = new TCanvas();
  g_DE_cal_RunNumber_pos_fall->SetMarkerStyle(8);
  g_DE_cal_RunNumber_pos_fall->SetMarkerColor(kRed);
  g_DE_cal_RunNumber_pos_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunNumber_pos_fall->Draw("AP");
  g_DE_cal_RunNumber_neg_fall->SetMarkerStyle(8);
  g_DE_cal_RunNumber_neg_fall->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunNumber_neg_fall->Draw("P same");
  c_cal_DE_RunNumber_fall->SaveAs("results/pid/SHMS_cal_DE_RunNumber_fall.pdf");
  TCanvas *c_cal_DE_RunNumber_spring = new TCanvas();
  g_DE_cal_RunNumber_pos_spring->SetMarkerStyle(8);
  g_DE_cal_RunNumber_pos_spring->SetMarkerColor(kRed);
  g_DE_cal_RunNumber_pos_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunNumber_pos_spring->Draw("AP");
  g_DE_cal_RunNumber_neg_spring->SetMarkerStyle(8);
  g_DE_cal_RunNumber_neg_spring->GetYaxis()->SetRangeUser(0.85,1.001);
  g_DE_cal_RunNumber_neg_spring->Draw("P same");
  c_cal_DE_RunNumber_spring->SaveAs("results/pid/SHMS_cal_DE_RunNumber_spring.pdf");
 
  TCanvas *c_aero_DE_momentum = new TCanvas();
  g_DE_aero_momentum_pos_fall->SetMarkerStyle(8);
  g_DE_aero_momentum_pos_fall->SetMarkerColor(kRed);
  g_DE_aero_momentum_pos_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_momentum_pos_fall->Draw("AP");
  g_DE_aero_momentum_pos_spring->SetMarkerStyle(4);
  g_DE_aero_momentum_pos_spring->SetMarkerColor(kRed);
  g_DE_aero_momentum_pos_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_momentum_pos_spring->Draw("P same");
  g_DE_aero_momentum_neg_fall->SetMarkerStyle(8);
  g_DE_aero_momentum_neg_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_momentum_neg_fall->Draw("P same");
  g_DE_aero_momentum_neg_spring->SetMarkerStyle(8);
  g_DE_aero_momentum_neg_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_momentum_neg_spring->Draw("P same");
  c_aero_DE_momentum->SaveAs("results/pid/SHMS_aero_DE_momentum.pdf");
  auto mg_aero = new TMultiGraph();
  g_DE_aero_RunGroup_neg_fall->SetMarkerStyle(8);
  g_DE_aero_RunGroup_neg_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  mg_aero->Add(g_DE_aero_RunGroup_neg_fall,"P");
  g_DE_aero_RunGroup_neg_spring->SetMarkerStyle(4);
  g_DE_aero_RunGroup_neg_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  mg_aero->Add(g_DE_aero_RunGroup_neg_spring,"P");
  TCanvas *c_aero_DE_RunGroup = new TCanvas();
  //g_DE_aero_RunGroup_pos_fall->SetMarkerStyle(8);
  //g_DE_aero_RunGroup_pos_fall->SetMarkerColor(kRed);
  //g_DE_aero_RunGroup_pos_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  //g_DE_aero_RunGroup_pos_fall->Draw("AP");
  //g_DE_aero_RunGroup_pos_spring->SetMarkerStyle(8);
  //g_DE_aero_RunGroup_pos_spring->SetMarkerColor(kRed);
  //g_DE_aero_RunGroup_pos_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  //g_DE_aero_RunGroup_pos_spring->Draw("P same");
  //g_DE_aero_RunGroup_neg_fall->Draw("P same");
  //g_DE_aero_RunGroup_neg_spring->Draw("P same");
  mg_aero->Draw("AP");
  mg_aero->Fit("pol0","F");
  TF1 *Fit_aero = mg_aero->GetFunction("pol0");
  double p0 = Fit_aero->GetParameter(0);
  double p0_err = Fit_aero->GetParError(0);
  Fit_aero->SetLineColor(kRed);
  Fit_aero->Draw("L same");
  gStyle->SetOptFit(0001);
  mg_aero->GetXaxis()->SetTitle("RunGroup");
  mg_aero->GetYaxis()->SetTitle("Aero Eff");
  c_aero_DE_RunGroup->SaveAs("results/pid/SHMS_aero_DE_RunGroup.pdf");
  TCanvas *c_aero_DE_RunNumber_fall = new TCanvas();
  g_DE_aero_RunNumber_pos_fall->SetMarkerStyle(8);
  g_DE_aero_RunNumber_pos_fall->SetMarkerColor(kRed);
  g_DE_aero_RunNumber_pos_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_RunNumber_pos_fall->Draw("AP");
  g_DE_aero_RunNumber_neg_fall->SetMarkerStyle(8);
  g_DE_aero_RunNumber_neg_fall->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_RunNumber_neg_fall->Draw("P same");
  c_aero_DE_RunNumber_fall->SaveAs("results/pid/SHMS_aero_DE_RunNumber_fall.pdf");
  TCanvas *c_aero_DE_RunNumber_spring = new TCanvas();
  g_DE_aero_RunNumber_pos_spring->SetMarkerStyle(8);
  g_DE_aero_RunNumber_pos_spring->SetMarkerColor(kRed);
  g_DE_aero_RunNumber_pos_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_RunNumber_pos_spring->Draw("AP");
  g_DE_aero_RunNumber_neg_spring->SetMarkerStyle(8);
  g_DE_aero_RunNumber_neg_spring->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_aero_RunNumber_neg_spring->Draw("P same");
  c_aero_DE_RunNumber_spring->SaveAs("results/pid/SHMS_aero_DE_RunNumber_spring.pdf");

  std::ofstream ofstream("results/pid/runs_info_rungroup.json");
  ofstream<<jout.dump(4)<<std::endl;
}
