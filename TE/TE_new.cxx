
#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
  using json = nlohmann::json;
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
  R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
  namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TDatime.h"

#include <fstream>
#include <vector>
#include <string>
#include <locale>
  void TE_new(){
    //get rate info from tracking json. yes too lazy
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_Hem.json");
      ifs>>j_rungroup;
    }
    json j_info;
    {
      std::ifstream ifs("results/Hem_eff.json");
      ifs>>j_info;
    }
    TGraphErrors* g_neg_TE_rate = new TGraphErrors();
    g_neg_TE_rate->SetTitle("Negative runs");
    double i_neg_TE=0;
    TGraphErrors* g_pos_TE_rate = new TGraphErrors();
    g_pos_TE_rate->SetTitle("Positive runs");
    double i_pos_TE = 0;
    TGraphErrors* g_neg_spring_TE_rate = new TGraphErrors();
    g_neg_spring_TE_rate->SetTitle("Spring Negative runs");
    TGraphErrors* g_pos_spring_TE_rate = new TGraphErrors();
    g_pos_spring_TE_rate->SetTitle("Spring Positive runs");
    double i_spring_pos_TE=0;
    double i_spring_neg_TE=0;
    TGraphErrors* g_neg_fall_TE_rate = new TGraphErrors();
    g_neg_fall_TE_rate->SetTitle("Fall Negative runs");
    TGraphErrors* g_pos_fall_TE_rate = new TGraphErrors();
    g_pos_fall_TE_rate->SetTitle("Fall Positive runs");
    double i_fall_pos_TE = 0;
    double i_fall_neg_TE = 0;
    TGraphErrors* g_TE_rate = new TGraphErrors();
    double i_TE = 0;

    TGraphErrors* g_neg_HMSTE_rate = new TGraphErrors();
    g_neg_HMSTE_rate->SetTitle("Negative runs");
    double i_neg_HMSTE=0;
    TGraphErrors* g_pos_HMSTE_rate = new TGraphErrors();
    g_pos_HMSTE_rate->SetTitle("Positive runs");
    double i_pos_HMSTE = 0;
    TGraphErrors* g_neg_spring_HMSTE_rate = new TGraphErrors();
    g_neg_spring_HMSTE_rate->SetTitle("Spring Negative runs");
    TGraphErrors* g_pos_spring_HMSTE_rate = new TGraphErrors();
    g_pos_spring_HMSTE_rate->SetTitle("Spring Positive runs");
    TGraphErrors* g_neg_fall_HMSTE_rate = new TGraphErrors();
    g_neg_fall_HMSTE_rate->SetTitle("Fall Negative runs");
    TGraphErrors* g_pos_fall_HMSTE_rate = new TGraphErrors();
    g_pos_fall_HMSTE_rate->SetTitle("Fall Positive runs");
    TGraphErrors* g_HMSTE_rate = new TGraphErrors();
    double i_HMSTE = 0;


    int i_rungoup_pos = 0,i_rungroup_neg = 0,i_run_pos = 0,i_run_neg = 0;
    for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
      int RunGroup = std::stoi(it.key());
      std::cout<<RunGroup<<std::endl;
      std::vector<int> neg_D2,pos_D2;
      neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
      pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
      if(!neg_D2.empty() && !pos_D2.empty()){
        for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
          int RunNumber = *i_neg;
          std::cout<<RunNumber<<std::endl;
          double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE_SHMS"].get<double>();
          double rate = j_info[(std::to_string(RunNumber)).c_str()]["PTRIG"].get<double>();
          g_neg_TE_rate->SetPoint(i_neg_TE,rate,TE);
          i_neg_TE+=1;
          double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TE_HMS"].get<double>();
          g_neg_HMSTE_rate->SetPoint(i_neg_HMSTE,RunNumber,HMSTE);
          i_neg_HMSTE+=1;
          if(RunNumber<7000){
            g_neg_fall_TE_rate->SetPoint(i_fall_neg_TE,rate,TE);
            g_neg_fall_HMSTE_rate->SetPoint(i_fall_neg_TE,RunNumber,HMSTE);
            i_fall_neg_TE++;
          }
          else{
            g_neg_spring_TE_rate->SetPoint(i_spring_neg_TE,rate,TE);
            g_neg_spring_HMSTE_rate->SetPoint(i_spring_neg_TE,RunNumber,HMSTE);
            i_spring_neg_TE++;
            
          }
        }//loop over neg runs

        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
          int RunNumber = *i_pos;
          std::cout<<RunNumber<<std::endl;
          double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE_SHMS"].get<double>();
          double rate = j_info[(std::to_string(RunNumber)).c_str()]["PTRIG"].get<double>();
          g_pos_TE_rate->SetPoint(i_pos_TE,rate,TE);
          i_pos_TE+=1;
          double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TE_HMS"].get<double>();
          g_pos_HMSTE_rate->SetPoint(i_pos_HMSTE,RunNumber,HMSTE);
          i_pos_HMSTE+=1;
          if(RunNumber<7000){
            g_pos_fall_TE_rate->SetPoint(i_fall_pos_TE,rate,TE);
            g_pos_fall_HMSTE_rate->SetPoint(i_fall_pos_TE,RunNumber,HMSTE);
            i_fall_pos_TE++;
          }
          else{
            g_pos_spring_TE_rate->SetPoint(i_spring_pos_TE,rate,TE);
            g_pos_spring_HMSTE_rate->SetPoint(i_spring_pos_TE,RunNumber,HMSTE);
            i_spring_pos_TE++;
            
          }
        }//loop over pos runs



      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_neg_TE = new TCanvas();
    g_neg_TE_rate->SetMarkerColor(kBlack);
    g_neg_TE_rate->SetMarkerStyle(8);
    g_neg_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_neg_TE_rate->GetYaxis()->SetTitle("Negative Tracking Efficiency");
    g_neg_TE_rate->Draw("AP");
    std::string c_neg_TE_name = "results/sys/Hem_TE_neg.pdf";
    c_neg_TE->SaveAs(c_neg_TE_name.c_str());

    TCanvas* c_pos_TE = new TCanvas();
    g_pos_TE_rate->SetMarkerColor(kRed);
    g_pos_TE_rate->SetMarkerStyle(8);
    g_pos_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_TE_rate->GetYaxis()->SetTitle("Postive Tracking Efficiency");
    g_pos_TE_rate->Draw("AP");
    std::string c_pos_TE_name = "results/sys/Hem_TE_pos.pdf";
    c_pos_TE->SaveAs(c_pos_TE_name.c_str());
 
    g_neg_TE_rate->SetMarkerColor(kBlack);
    g_neg_TE_rate->SetMarkerStyle(8);
    g_pos_TE_rate->SetMarkerColor(kRed);
    g_pos_TE_rate->SetMarkerStyle(8);
    g_pos_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_TE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    TMultiGraph* mg_TE = new TMultiGraph();
    mg_TE->Add(g_neg_TE_rate);
    mg_TE->Add(g_pos_TE_rate);
    TCanvas* c_TE = new TCanvas();
    mg_TE->Draw("AP");
    std::string c_TE_name = "results/sys/Hem_TE_all.pdf";
    c_TE->BuildLegend();
    c_TE->SaveAs(c_TE_name.c_str());

    TCanvas* c_neg_HMSTE = new TCanvas();
    g_neg_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_HMSTE_rate->SetMarkerStyle(8);
    g_neg_HMSTE_rate->GetXaxis()->SetTitle("RunNumber");
    g_neg_HMSTE_rate->GetYaxis()->SetTitle("Negative Tracking Efficiency");
    g_neg_HMSTE_rate->Draw("AP");
    std::string c_neg_HMSTE_name = "results/sys/Hem_HMSTE_neg.pdf";
    c_neg_HMSTE->SaveAs(c_neg_HMSTE_name.c_str());

    TCanvas* c_pos_HMSTE = new TCanvas();
    g_pos_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->GetXaxis()->SetTitle("RunNumber");
    g_pos_HMSTE_rate->GetYaxis()->SetTitle("Postive Tracking Efficiency");
    g_pos_HMSTE_rate->Draw("AP");
    std::string c_pos_HMSTE_name = "results/sys/Hem_HMSTE_pos.pdf";
    c_pos_HMSTE->SaveAs(c_pos_HMSTE_name.c_str());
 
    g_neg_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->GetXaxis()->SetTitle("RunNumber");
    g_pos_HMSTE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    TMultiGraph* mg_HMSTE = new TMultiGraph();
    mg_HMSTE->Add(g_neg_HMSTE_rate);
    mg_HMSTE->Add(g_pos_HMSTE_rate);
    TCanvas* c_HMSTE = new TCanvas();
    mg_HMSTE->Draw("AP");
    std::string c_HMSTE_name = "results/sys/Hem_HMSTE_all.pdf";
    c_HMSTE->BuildLegend();
    c_HMSTE->SaveAs(c_HMSTE_name.c_str());
  
    TCanvas* c_neg_fall_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_fall_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 rate");
    g_neg_fall_TE_rate->GetYaxis()->SetTitle("SHMS TE");
    g_neg_fall_TE_rate->Fit("pol0");
    g_neg_fall_TE_rate->SetMarkerColor(kBlack);
    g_neg_fall_TE_rate->SetMarkerStyle(8);
    g_neg_fall_TE_rate->Draw("AP");
    c_neg_fall_TE->SaveAs("results/sys/Fall_SHMS_TE_neg.pdf");
    TCanvas* c_pos_fall_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_pos_fall_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 rate");
    g_pos_fall_TE_rate->GetYaxis()->SetTitle("SHMS TE");
    g_pos_fall_TE_rate->Fit("pol0");
    g_pos_fall_TE_rate->SetMarkerColor(kRed);
    g_pos_fall_TE_rate->SetMarkerStyle(8);
    g_pos_fall_TE_rate->Draw("AP");
    c_pos_fall_TE->SaveAs("results/sys/Fall_SHMS_TE_pos.pdf");
    
    TCanvas* c_neg_spring_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_spring_TE_rate->Fit("pol1");
    g_neg_spring_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 rate");
    g_neg_spring_TE_rate->GetYaxis()->SetTitle("SHMS TE");
    g_neg_spring_TE_rate->SetMarkerColor(kBlack);
    g_neg_spring_TE_rate->SetMarkerStyle(8);
    g_neg_spring_TE_rate->Draw("AP");
    c_neg_spring_TE->SaveAs("results/sys/spring_SHMS_TE_neg.pdf");
    TCanvas* c_pos_spring_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_pos_spring_TE_rate->Fit("pol1");
    g_pos_spring_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 rate");
    g_pos_spring_TE_rate->GetYaxis()->SetTitle("SHMS TE");
    g_pos_spring_TE_rate->SetMarkerColor(kRed);
    g_pos_spring_TE_rate->SetMarkerStyle(8);
    g_pos_spring_TE_rate->Draw("AP");
    c_pos_spring_TE->SaveAs("results/sys/spring_SHMS_TE_pos.pdf");

    TCanvas* c_fall_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_fall_TE_rate->Fit("pol0");
    g_neg_fall_TE_rate->SetMarkerColor(kBlack);
    g_neg_fall_TE_rate->SetMarkerStyle(8);
    //g_neg_fall_TE_rate->Draw("AP");
    g_pos_fall_TE_rate->Fit("pol0");
    g_pos_fall_TE_rate->SetMarkerColor(kRed);
    g_pos_fall_TE_rate->SetMarkerStyle(8);
    g_pos_fall_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_fall_TE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    //g_pos_fall_TE_rate->Draw("P same");
    TMultiGraph* mg_fall_TE = new TMultiGraph();
    mg_fall_TE->Add(g_neg_fall_TE_rate,"P");
    mg_fall_TE->Add(g_pos_fall_TE_rate,"P");
    mg_fall_TE->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    mg_fall_TE->GetYaxis()->SetTitle("Tracking Efficiency");
    mg_fall_TE->Draw("AP");
    c_fall_TE->SaveAs("results/sys/Fall_SHMS_TE.pdf");
    
    TCanvas* c_spring_TE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_spring_TE_rate->Fit("pol1");
    g_neg_spring_TE_rate->SetMarkerColor(kBlack);
    g_neg_spring_TE_rate->SetMarkerStyle(8);
    //g_neg_spring_TE_rate->Draw("AP");
    g_pos_spring_TE_rate->Fit("pol1");
    g_pos_spring_TE_rate->SetMarkerColor(kRed);
    g_pos_spring_TE_rate->SetMarkerStyle(8);
    g_pos_spring_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_spring_TE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    //g_pos_spring_TE_rate->Draw("P same");
    TMultiGraph* mg_spring_TE = new TMultiGraph();
    mg_spring_TE->Add(g_neg_spring_TE_rate,"P");
    mg_spring_TE->Add(g_pos_spring_TE_rate,"P");
    mg_spring_TE->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    mg_spring_TE->GetYaxis()->SetTitle("Tracking Efficiency");
    mg_spring_TE->Draw("AP");
    c_spring_TE->SaveAs("results/sys/Spring_SHMS_TE.pdf");
    
    TCanvas* c_neg_fall_HMSTE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_fall_HMSTE_rate->Fit("pol0");
    g_neg_fall_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_fall_HMSTE_rate->SetMarkerStyle(8);
    //g_neg_fall_HMSTE_rate->Draw("AP");
    g_pos_fall_HMSTE_rate->Fit("pol0");
    g_pos_fall_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_fall_HMSTE_rate->SetMarkerStyle(8);
    g_pos_fall_HMSTE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_fall_HMSTE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    //g_pos_fall_HMSTE_rate->Draw("P same");
    TMultiGraph* mg_fall_HMSTE = new TMultiGraph();
    mg_fall_HMSTE->Add(g_neg_fall_HMSTE_rate,"P");
    mg_fall_HMSTE->Add(g_pos_fall_HMSTE_rate,"P");
    mg_fall_HMSTE->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    mg_fall_HMSTE->GetYaxis()->SetTitle("Tracking Efficiency");
    mg_fall_HMSTE->Draw("AP");
    c_neg_fall_HMSTE->SaveAs("results/sys/Fall_HMSTE.pdf");
    
    TCanvas* c_neg_spring_HMSTE = new TCanvas();
    gStyle->SetOptFit(1);
    g_neg_spring_HMSTE_rate->Fit("pol0");
    g_neg_spring_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_spring_HMSTE_rate->SetMarkerStyle(8);
    //g_neg_spring_HMSTE_rate->Draw("AP");
    g_pos_spring_HMSTE_rate->Fit("pol0");
    g_pos_spring_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_spring_HMSTE_rate->SetMarkerStyle(8);
    g_pos_spring_HMSTE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_spring_HMSTE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    //g_pos_spring_HMSTE_rate->Draw("P same");
    TMultiGraph* mg_spring_HMSTE = new TMultiGraph();
    mg_spring_HMSTE->Add(g_neg_spring_HMSTE_rate,"P");
    mg_spring_HMSTE->Add(g_pos_spring_HMSTE_rate,"P");
    mg_spring_HMSTE->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    mg_spring_HMSTE->GetYaxis()->SetTitle("Tracking Efficiency");
    mg_spring_HMSTE->Draw("AP");
    c_neg_spring_HMSTE->SaveAs("results/sys/Spring_HMS_TE.pdf");
  }
