
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
  void TE_sys(){
    //get rate info from tracking json. yes too lazy
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }
    json j_info;
    {
      std::ifstream ifs("results/TE/Tracking.json");
      ifs>>j_info;
    }
    TGraphErrors* g_neg_TE_rate = new TGraphErrors();
    g_neg_TE_rate->SetTitle("Negative runs");
    double i_neg_TE=0;
    TGraphErrors* g_pos_TE_rate = new TGraphErrors();
    g_pos_TE_rate->SetTitle("Positive runs");
    double i_pos_TE = 0;
    TGraphErrors* g_TE_rate = new TGraphErrors();
    double i_TE = 0;

    TGraphErrors* g_neg_HMSTE_rate = new TGraphErrors();
    g_neg_HMSTE_rate->SetTitle("Negative runs");
    double i_neg_HMSTE=0;
    TGraphErrors* g_pos_HMSTE_rate = new TGraphErrors();
    g_pos_HMSTE_rate->SetTitle("Positive runs");
    double i_pos_HMSTE = 0;
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
          double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
          double TE_err = j_info[(std::to_string(RunNumber)).c_str()]["TE_err"].get<double>();
          double data_n = j_info[(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
          double time = j_info[(std::to_string(RunNumber)).c_str()]["time"].get<double>();
          double rate = data_n/time;
          g_neg_TE_rate->SetPoint(i_neg_TE,rate,TE);
          g_neg_TE_rate->SetPointError(i_neg_TE,0,TE_err);
          i_neg_TE+=1;
          double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
          double HMSTE_err = j_info[(std::to_string(RunNumber)).c_str()]["HMSTE_err"].get<double>();
          g_neg_HMSTE_rate->SetPoint(i_neg_HMSTE,rate,HMSTE);
          g_neg_HMSTE_rate->SetPointError(i_neg_HMSTE,0,HMSTE_err);
          i_neg_HMSTE+=1;
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
          int RunNumber = *i_pos;
          std::cout<<RunNumber<<std::endl;
          double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
          double TE_err = j_info[(std::to_string(RunNumber)).c_str()]["TE_err"].get<double>();
          double data_n = j_info[(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
          double time = j_info[(std::to_string(RunNumber)).c_str()]["time"].get<double>();
          double rate = data_n/time;
          g_pos_TE_rate->SetPoint(i_pos_TE,rate,TE);
          g_pos_TE_rate->SetPointError(i_pos_TE,0,TE_err);
          i_pos_TE+=1;
          double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
          double HMSTE_err = j_info[(std::to_string(RunNumber)).c_str()]["HMSTE_err"].get<double>();
          g_pos_HMSTE_rate->SetPoint(i_pos_HMSTE,rate,HMSTE);
          g_pos_HMSTE_rate->SetPointError(i_pos_HMSTE,0,HMSTE_err);
          i_pos_HMSTE+=1;
        }//loop over pos runs




      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_neg_TE = new TCanvas();
    g_neg_TE_rate->SetMarkerColor(kBlack);
    g_neg_TE_rate->SetMarkerStyle(8);
    g_neg_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_neg_TE_rate->GetYaxis()->SetTitle("Negative Tracking Efficiency");
    g_neg_TE_rate->Draw("AP");
    std::string c_neg_TE_name = "results/sys/TE_neg.pdf";
    c_neg_TE->SaveAs(c_neg_TE_name.c_str());

    TCanvas* c_pos_TE = new TCanvas();
    g_pos_TE_rate->SetMarkerColor(kRed);
    g_pos_TE_rate->SetMarkerStyle(8);
    g_pos_TE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_TE_rate->GetYaxis()->SetTitle("Postive Tracking Efficiency");
    g_pos_TE_rate->Draw("AP");
    std::string c_pos_TE_name = "results/sys/TE_pos.pdf";
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
    std::string c_TE_name = "results/sys/TE_all.pdf";
    c_TE->BuildLegend();
    c_TE->SaveAs(c_TE_name.c_str());

    TCanvas* c_neg_HMSTE = new TCanvas();
    g_neg_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_HMSTE_rate->SetMarkerStyle(8);
    g_neg_HMSTE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_neg_HMSTE_rate->GetYaxis()->SetTitle("Negative Tracking Efficiency");
    g_neg_HMSTE_rate->Draw("AP");
    std::string c_neg_HMSTE_name = "results/sys/HMSTE_neg.pdf";
    c_neg_HMSTE->SaveAs(c_neg_HMSTE_name.c_str());

    TCanvas* c_pos_HMSTE = new TCanvas();
    g_pos_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_HMSTE_rate->GetYaxis()->SetTitle("Postive Tracking Efficiency");
    g_pos_HMSTE_rate->Draw("AP");
    std::string c_pos_HMSTE_name = "results/sys/HMSTE_pos.pdf";
    c_pos_HMSTE->SaveAs(c_pos_HMSTE_name.c_str());
 
    g_neg_HMSTE_rate->SetMarkerColor(kBlack);
    g_neg_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->SetMarkerColor(kRed);
    g_pos_HMSTE_rate->SetMarkerStyle(8);
    g_pos_HMSTE_rate->GetXaxis()->SetTitle("SHMS 3/4 Rate");
    g_pos_HMSTE_rate->GetYaxis()->SetTitle("Tracking Efficiency");
    TMultiGraph* mg_HMSTE = new TMultiGraph();
    mg_HMSTE->Add(g_neg_HMSTE_rate);
    mg_HMSTE->Add(g_pos_HMSTE_rate);
    TCanvas* c_HMSTE = new TCanvas();
    mg_HMSTE->Draw("AP");
    std::string c_HMSTE_name = "results/sys/HMSTE_all.pdf";
    c_HMSTE->BuildLegend();
    c_HMSTE->SaveAs(c_HMSTE_name.c_str());
  }
