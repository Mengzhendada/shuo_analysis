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

void HMS_DE_compare(){

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
  int cer_e_n = 2;
  double cal_e_n = 0.8;
  std::cout<<"cer electron cut is 10, cal electron cut is 0.8"<<std::endl;
  
  TGraphErrors* g_DE_cal_momentum_neg = new TGraphErrors();
  g_DE_cal_momentum_neg->SetTitle("calorimeter;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_neg = new TGraphErrors();
  g_DE_cal_RunGroup_neg->SetTitle("calorimeter;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_neg = new TGraphErrors();
  g_DE_cal_RunNumber_neg->SetTitle("calorimeter;RunNumber;efficiency");
  TGraphErrors* g_DE_cer_momentum_neg = new TGraphErrors();
  g_DE_cer_momentum_neg->SetTitle("cer;momentum;efficiency");
  TGraphErrors* g_DE_cer_RunGroup_neg = new TGraphErrors();
  g_DE_cer_RunGroup_neg->SetTitle("cer;RunGroup;efficiency");
  TGraphErrors* g_DE_cer_RunNumber_neg = new TGraphErrors();
  g_DE_cer_RunNumber_neg->SetTitle("cer;RunNumber;efficiency");
  TGraphErrors* g_DE_cal_momentum_pos = new TGraphErrors();
  g_DE_cal_momentum_pos->SetTitle("calorimeter;momentum;efficiency");
  TGraphErrors* g_DE_cal_RunGroup_pos = new TGraphErrors();
  g_DE_cal_RunGroup_pos->SetTitle("calorimeter;RunGroup;efficiency");
  TGraphErrors* g_DE_cal_RunNumber_pos = new TGraphErrors();
  g_DE_cal_RunNumber_pos->SetTitle("calorimeter;RunNumber;efficiency");
  TGraphErrors* g_DE_cer_momentum_pos = new TGraphErrors();
  g_DE_cer_momentum_pos->SetTitle("cer;momentum;efficiency");
  TGraphErrors* g_DE_cer_RunGroup_pos = new TGraphErrors();
  g_DE_cer_RunGroup_pos->SetTitle("cer;RunGroup;efficiency");
  TGraphErrors* g_DE_cer_RunNumber_pos = new TGraphErrors();
  g_DE_cer_RunNumber_pos->SetTitle("cer;RunNumber;efficiency");
  //int i_cal_momentum = 0,i_cal_RunGroup = 0,i_cal_RunNumber = 0,i_cer_momentum = 0,i_cer_RunGroup = 0,i_cer_RunNumber = 0;
  int i_de_neg = 0,i_de_pos = 0; 
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    double hms_p = -j_rungroup[(std::to_string(RunGroup)).c_str()]["hms_p"].get<double>();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){ 
      std::cout<<RunGroup<<std::endl;
//      pos_D2.insert(pos_D2.end(),neg_D2.begin(),neg_D2.end());
      //for pos runs, I don't have to separate pos or neg here
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        //calorimeter runs part
        json j_DE;
        {
          std::string infilename = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
          std::cout<<"json file "<<infilename<<std::endl;
          std::ifstream infile(infilename.c_str());
          infile>>j_DE;
        }
        double cal_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
        double cer_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_all"].get<double>();
        std::vector<int> cer_cuts;
        std::vector<double> cal_cuts;
        std::vector<double> cer_e,cal_e;
        cal_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["calcuts"].get<std::vector<double>>();
        cer_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["cercuts"].get<std::vector<int>>();
        auto cercut_it = find(cer_cuts.begin(),cer_cuts.end(),cer_e_n);
        int index_cer = cercut_it-cer_cuts.begin();
        auto calcut_it = find(cal_cuts.begin(),cal_cuts.end(),cal_e_n);
        int index_cal = calcut_it-cal_cuts.begin();
        std::cout<<"which cut "<<index_cal<<std::endl;
        cer_e = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_cercuts"].get<std::vector<double>>();
        cal_e = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
        if(cal_e_all!=0){
          double cal_DE = cal_e[index_cal]/cal_e_all;
          double cer_DE = cer_e[index_cer]/cer_e_all;
          double cal_DE_error = 1/cal_e_all*sqrt(cal_e[index_cal]*(1-cal_DE));
          double cer_DE_error = 1/cer_e_all*sqrt(cer_e[index_cer]*(1-cer_DE));
          std::cout<<"calDE "<<cal_DE<<" error "<<cal_DE_error<<std::endl;
          std::cout<<"cerDE "<<cer_DE<<" error "<<cer_DE_error<<std::endl;
          g_DE_cal_momentum_pos->SetPoint(i_de_pos,hms_p,cal_DE);
          g_DE_cal_momentum_pos->SetPointError(i_de_pos,0,cal_DE_error);
          g_DE_cal_RunGroup_pos->SetPoint(i_de_pos,RunGroup,cal_DE);
          g_DE_cal_RunGroup_pos->SetPointError(i_de_pos,0,cal_DE_error);
          g_DE_cal_RunNumber_pos->SetPoint(i_de_pos,RunNumber,cal_DE);
          g_DE_cal_RunNumber_pos->SetPointError(i_de_pos,0,cal_DE_error);
          g_DE_cer_momentum_pos->SetPoint(i_de_pos,hms_p,cer_DE);
          g_DE_cer_momentum_pos->SetPointError(i_de_pos,0,cer_DE_error);
          g_DE_cer_RunGroup_pos->SetPoint(i_de_pos,RunGroup,cer_DE);
          g_DE_cer_RunGroup_pos->SetPointError(i_de_pos,0,cer_DE_error);
          g_DE_cer_RunNumber_pos->SetPoint(i_de_pos,RunNumber,cer_DE);
          g_DE_cer_RunNumber_pos->SetPointError(i_de_pos,0,cer_DE_error);
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_DE"] = cal_DE;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_DE"] = cer_DE;
          //j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_DE_error"] = cal_DE_error;
          //j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_DE_error"] = cer_DE_error;
          i_de_pos++;
        }
      }//for pos runs
      
      //for neg runs
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;
        //calorimeter runs part
        json j_DE;
        {
          std::string infilename = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
          std::cout<<"json file "<<infilename<<std::endl;
          std::ifstream infile(infilename.c_str());
          infile>>j_DE;
        }
        double cal_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
        double cer_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_all"].get<double>();
        std::vector<int> cer_cuts;
        std::vector<double> cal_cuts;
        std::vector<double> cer_e,cal_e;
        cal_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["calcuts"].get<std::vector<double>>();
        cer_cuts = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["cercuts"].get<std::vector<int>>();
        auto cercut_it = find(cer_cuts.begin(),cer_cuts.end(),cer_e_n);
        int index_cer = cercut_it-cer_cuts.begin();
        auto calcut_it = find(cal_cuts.begin(),cal_cuts.end(),cal_e_n);
        int index_cal = calcut_it-cal_cuts.begin();
        cer_e = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_cercuts"].get<std::vector<double>>();
        cal_e = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
        if(cal_e_all!=0){
          double cal_DE = cal_e[index_cal]/cal_e_all;
          double cer_DE = cer_e[index_cer]/cer_e_all;
          double cal_DE_error = 1/cal_e_all*sqrt(cal_e[index_cal]*(1-cal_DE));
          double cer_DE_error = 1/cer_e_all*sqrt(cer_e[index_cer]*(1-cer_DE));
          std::cout<<"calDE "<<cal_DE<<" error "<<cal_DE_error<<std::endl;
          std::cout<<"cerDE "<<cer_DE<<" error "<<cer_DE_error<<std::endl;
          g_DE_cal_momentum_neg->SetPoint(i_de_neg,hms_p,cal_DE);
          g_DE_cal_momentum_neg->SetPointError(i_de_neg,0,cal_DE_error);
          g_DE_cal_RunGroup_neg->SetPoint(i_de_neg,RunGroup,cal_DE);
          g_DE_cal_RunGroup_neg->SetPointError(i_de_neg,0,cal_DE_error);
          g_DE_cal_RunNumber_neg->SetPoint(i_de_neg,RunNumber,cal_DE);
          g_DE_cal_RunNumber_neg->SetPointError(i_de_neg,0,cal_DE_error);
          g_DE_cer_momentum_neg->SetPoint(i_de_neg,hms_p,cer_DE);
          g_DE_cer_momentum_neg->SetPointError(i_de_neg,0,cer_DE_error);
          g_DE_cer_RunGroup_neg->SetPoint(i_de_neg,RunGroup,cer_DE);
          g_DE_cer_RunGroup_neg->SetPointError(i_de_neg,0,cer_DE_error);
          g_DE_cer_RunNumber_neg->SetPoint(i_de_neg,RunNumber,cer_DE);
          g_DE_cer_RunNumber_neg->SetPointError(i_de_neg,0,cer_DE_error);
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_DE"] = cal_DE;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_DE"] = cer_DE;
          i_de_neg++;
        }
      }//for neg runs
      

    }//if normal production runs
  }//loop over rungroups
  std::cout<< "Done check"<<std::endl;
  auto mg_cal = new TMultiGraph();
  g_DE_cal_momentum_pos->SetMarkerStyle(8);
  g_DE_cal_momentum_pos->SetMarkerColor(kRed);
  g_DE_cal_momentum_pos->GetYaxis()->SetRangeUser(0.98,1.001);
  mg_cal->Add(g_DE_cal_momentum_pos,"P");
  //g_DE_cal_momentum_neg->SetMarkerStyle(8);
  //g_DE_cal_momentum_neg->GetYaxis()->SetRangeUser(0.98,1.001);
  //mg_cal->Add(g_DE_cal_momentum_neg,"P");
  TCanvas *c_cal_DE_momentum = new TCanvas();
  mg_cal->Draw("AP");
  mg_cal->Fit("pol0","F");
  TF1 *Fit_cal = mg_cal->GetFunction("pol0");
  Fit_cal->SetLineColor(kRed);
  Fit_cal->Draw("L same");
  gStyle->SetOptFit(0001);
  mg_cal->GetXaxis()->SetTitle("Momentum");
  mg_cal->GetYaxis()->SetTitle("cal Eff");
  c_cal_DE_momentum->SaveAs("results/pid/HMS_cal_DE_momentum.pdf");
  TCanvas *c_cal_DE_RunGroup = new TCanvas();
  g_DE_cal_RunGroup_pos->SetMarkerStyle(8);
  g_DE_cal_RunGroup_pos->SetMarkerColor(kRed);
  g_DE_cal_RunGroup_pos->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_cal_RunGroup_pos->Draw("AP");
  g_DE_cal_RunGroup_neg->SetMarkerStyle(8);
  g_DE_cal_RunGroup_neg->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_cal_RunGroup_neg->Draw("P same");
  c_cal_DE_RunGroup->SaveAs("results/pid/HMS_cal_DE_RunGroup.pdf");
  TCanvas *c_cal_DE_RunNumber = new TCanvas();
  g_DE_cal_RunNumber_pos->SetMarkerStyle(8);
  g_DE_cal_RunNumber_pos->SetMarkerColor(kRed);
  g_DE_cal_RunNumber_pos->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_cal_RunNumber_pos->Draw("AP");
  g_DE_cal_RunNumber_neg->SetMarkerStyle(8);
  g_DE_cal_RunNumber_neg->GetYaxis()->SetRangeUser(0.98,1.001);
  g_DE_cal_RunNumber_neg->Draw("P same");
  c_cal_DE_RunNumber->SaveAs("results/pid/HMS_cal_DE_RunNumber.pdf");
 
  TCanvas *c_cer_DE_momentum = new TCanvas();
  g_DE_cer_momentum_pos->SetMarkerStyle(8);
  g_DE_cer_momentum_pos->SetMarkerColor(kRed);
  g_DE_cer_momentum_pos->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_momentum_pos->Draw("AP");
  g_DE_cer_momentum_neg->SetMarkerStyle(8);
  g_DE_cer_momentum_neg->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_momentum_neg->Draw("P same");
  c_cer_DE_momentum->SaveAs("results/pid/HMS_cer_DE_momentum.pdf");
  TCanvas *c_cer_DE_RunGroup = new TCanvas();
  g_DE_cer_RunGroup_pos->SetMarkerStyle(8);
  g_DE_cer_RunGroup_pos->SetMarkerColor(kRed);
  g_DE_cer_RunGroup_pos->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_RunGroup_pos->Draw("AP");
  g_DE_cer_RunGroup_neg->SetMarkerStyle(8);
  g_DE_cer_RunGroup_neg->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_RunGroup_neg->Draw("P same");
  c_cer_DE_RunGroup->SaveAs("results/pid/HMS_cer_DE_RunGroup.pdf");
  TCanvas *c_cer_DE_RunNumber = new TCanvas();
  g_DE_cer_RunNumber_pos->SetMarkerStyle(8);
  g_DE_cer_RunNumber_pos->SetMarkerColor(kRed);
  g_DE_cer_RunNumber_pos->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_RunNumber_pos->Draw("AP");
  g_DE_cer_RunNumber_neg->SetMarkerStyle(8);
  g_DE_cer_RunNumber_neg->GetYaxis()->SetRangeUser(0.9,1.001);
  g_DE_cer_RunNumber_neg->Draw("P same");
  c_cer_DE_RunNumber->SaveAs("results/pid/HMS_cer_DE_RunNumber.pdf");

  std::ofstream ofstream("runs_info.json");
  ofstream<<j_runsinfo.dump(4)<<std::endl;
}
