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

#include <fstream>
#include <vector>
#include <string>
#include <locale>
  void plot_TE(){
    json j_te;
    { 
      std::ifstream ifs("results/TE/trackingeff_info.json");
      ifs>>j_te;
    }
    TGraphErrors* G_te_compare_pos = new TGraphErrors();
    TGraphErrors* G_te_compare_neg = new TGraphErrors();

    int RunGroup = 500;
    double pi_expected_pos = j_te[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_expected"].get<double>();
    double pi_found_pos_1 = j_te[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_1"].get<double>();
    double pi_found_pos_2 = j_te[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_2"].get<double>();
    double pi_found_pos_3 = j_te[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_3"].get<double>();
    double pi_expected_neg = j_te[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_expected"].get<double>();
    double pi_found_neg_1 = j_te[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_1"].get<double>();
    double pi_found_neg_2 = j_te[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_2"].get<double>();
    double pi_found_neg_3 = j_te[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_3"].get<double>();
    double te_pos_1 = pi_found_pos_1/pi_expected_pos;
    double te_pos_2 = pi_found_pos_2/pi_expected_pos;
    double te_pos_3 = pi_found_pos_3/pi_expected_pos;
    double te_neg_1 = pi_found_neg_1/pi_expected_neg;
    double te_neg_2 = pi_found_neg_2/pi_expected_neg;
    double te_neg_3 = pi_found_neg_3/pi_expected_neg;
    G_te_compare_pos->SetPoint(0,1,te_pos_1);
    G_te_compare_pos->SetPoint(1,2,te_pos_2);
    G_te_compare_pos->SetPoint(2,3,te_pos_3);
    G_te_compare_neg->SetPoint(0,1,te_neg_1);
    G_te_compare_neg->SetPoint(1,2,te_neg_2);
    G_te_compare_neg->SetPoint(2,3,te_neg_3);
    TCanvas *c_compare = new TCanvas();
    G_te_compare_pos->SetMarkerStyle(8);
    G_te_compare_pos->SetMarkerColor(kRed);
    G_te_compare_pos->Draw("ap");
    G_te_compare_neg->SetMarkerStyle(8);
    G_te_compare_neg->SetMarkerColor(kBlack);
    G_te_compare_neg->Draw("p");
    std::string c_compare_name = "results/TE/te_compare_500.pdf";
    c_compare->SaveAs(c_compare_name.c_str());
    int i = 0;
    int i_neg = 0;
    int i_pos = 0;
    TGraphErrors* G_te_all_pos = new TGraphErrors();
    TGraphErrors* G_te_all_neg = new TGraphErrors();
    G_te_all_pos->SetTitle("SHMS TE;RunGroup;TE");
    TGraphErrors* G_te_all_pos_vs_shmsp = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_shmsp = new TGraphErrors();
    G_te_all_pos_vs_shmsp->SetTitle("SHMS TE; shmsp; TE");
    TGraphErrors* G_te_all_pos_vs_rate = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_rate = new TGraphErrors();
    G_te_all_pos_vs_rate->SetTitle("SHMS TE; rate; TE");
    TGraphErrors* G_te_all_pos_vs_yield = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_yield = new TGraphErrors();
    G_te_all_pos_vs_yield->SetTitle("SHMS TE; yield;TE");
    TGraphErrors* G_te_all_pos_vs_yieldrate = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_yieldrate = new TGraphErrors();
    G_te_all_pos_vs_yieldrate->SetTitle("SHMS TE; yieldrate;TE");
    TGraphErrors* G_te_all_pos_runs = new TGraphErrors();
    TGraphErrors* G_te_all_neg_runs = new TGraphErrors();
    G_te_all_neg_runs->SetTitle("SHMS TE; runs;TE");
    for(auto it = j_te.begin();it!=j_te.end();it++){
      int RunGroup;
      RunGroup = std::stoi(it.key());
      if(RunGroup>10){
        auto runjs = it.value();
        double pi_expected_pos = runjs["pos"]["SHMS_pi_expected"].get<double>();
        double pi_found_pos_1 = runjs["pos"]["SHMS_pi_found_1"].get<double>();
        double pi_found_pos_2 = runjs["pos"]["SHMS_pi_found_2"].get<double>();
        double pi_found_pos_3 = runjs["pos"]["SHMS_pi_found_3"].get<double>();
        double pi_expected_neg = runjs["neg"]["SHMS_pi_expected"].get<double>();
        double pi_found_neg_1 = runjs["neg"]["SHMS_pi_found_1"].get<double>();
        double pi_found_neg_2 = runjs["neg"]["SHMS_pi_found_2"].get<double>();
        double pi_found_neg_3 = runjs["neg"]["SHMS_pi_found_3"].get<double>();
        double te_pos_1 = pi_found_pos_1/pi_expected_pos;
        double te_neg_1 = pi_found_neg_1/pi_expected_neg;
        G_te_all_pos->SetPoint(i,RunGroup,te_pos_1);
        G_te_all_neg->SetPoint(i,RunGroup,te_neg_1);
        i++;
        std::cout<<"check rungroup "<<it.key()<<std::endl;
        for(auto it_neg = runjs["neg"].begin();it_neg!=runjs["neg"].end();it_neg++){
          std::string neg_key = it_neg.key();
          if(std::isdigit(neg_key[0])){
            int RunNumber = std::stoi(it_neg.key());
            auto ik_neg = it_neg.value();
         if(ik_neg.find("data_n") != ik_neg.end() && ik_neg.find("SHMS_pi_expected") != ik_neg.end()){
          double pi_expected = ik_neg["SHMS_pi_expected"].get<double>();
          double pi_found = ik_neg["SHMS_pi_found_1"].get<double>();
          double charge = ik_neg["charge"].get<double>();
          double time = ik_neg["time"].get<double>();
          double counts = ik_neg["data_n"].get<double>();
          double te = pi_found/pi_expected;
          double yield = pi_expected/charge;
          double rate = counts/(1000*time);
          if(rate < 0){std::cout<< "Alert "<<it_neg.key()<<std::endl;}
          double yieldrate = pi_expected/(charge*time);
          G_te_all_neg_vs_rate->SetPoint(i_neg,rate,te);
          G_te_all_neg_vs_yield->SetPoint(i_neg,yield,te);
          G_te_all_neg_vs_yieldrate->SetPoint(i_neg,yieldrate,te);
          G_te_all_neg_runs->SetPoint(i_neg,RunNumber,te);
          i_neg++;
          std::cout<<"check neg "<<it_neg.key()<<std::endl;
         }
         else{std::cout<<"no counts for "<<it_neg.key()<<std::endl;;}
          }
        }//neg runs
        runjs = it.value();
       for(auto it_pos = runjs["pos"].begin();it_pos!=runjs["pos"].end();it_pos++){
          std::string pos_key = it_pos.key();
         if(std::isdigit(pos_key[0])){
            int RunNumber = std::stoi(it_pos.key());
         auto ik_pos = it_pos.value();
         if(ik_pos.find("data_n") != ik_pos.end() && ik_pos.find("SHMS_pi_expected")!=ik_pos.end()){
         double pi_expected = ik_pos["SHMS_pi_expected"].get<double>();
         double pi_found = ik_pos["SHMS_pi_found_1"].get<double>();
         double charge = ik_pos["charge"].get<double>();
         double time = ik_pos["time"].get<double>();
         double counts = ik_pos["data_n"].get<double>();
         double te = pi_found/pi_expected;
         double yield = pi_expected/charge;
         double rate = counts/(1000*time);
         if(rate<0){std::cout<<"Alert "<<it_pos.key()<<std::endl;}
         double yieldrate = pi_expected/(charge*time);
         G_te_all_pos_vs_rate->SetPoint(i_pos,rate,te);
         G_te_all_pos_vs_yield->SetPoint(i_pos,yield,te);
         G_te_all_pos_vs_yieldrate->SetPoint(i_pos,yieldrate,te);
         G_te_all_pos_runs->SetPoint(i_pos,RunNumber,te);
         i_pos++;
         std::cout<<"check pos"<<it_pos.key()<<std::endl;
         }
         else{std::cout<<"no counts for "<<it_pos.key()<<std::endl;;}
         }
         }//pos runs
      }//if rungroup greater than 10 
    }//rungroups
    TCanvas* c_te_all = new TCanvas();
    c_te_all->SetGrid();
    G_te_all_pos->SetMarkerColor(kRed);
    G_te_all_pos->SetMarkerStyle(8);
    G_te_all_pos->Draw("ap");
    G_te_all_neg->SetMarkerColor(kBlack);
    G_te_all_neg->SetMarkerStyle(8);
    G_te_all_neg->Draw("p");
    std::string c_te_all_name = "results/TE/pi_te_all.pdf";
    c_te_all->SaveAs(c_te_all_name.c_str());
    TCanvas* c_te_all_yield = new TCanvas();
    c_te_all_yield->SetGrid();
    G_te_all_pos_vs_yield->SetMarkerColor(kRed);
    G_te_all_pos_vs_yield->SetMarkerStyle(8);
    G_te_all_pos_vs_yield->Draw("ap");
    G_te_all_neg_vs_yield->SetMarkerColor(kBlack);
    G_te_all_neg_vs_yield->SetMarkerStyle(8);
    G_te_all_neg_vs_yield->Draw("p");
    std::string c_te_all_yield_name = "results/TE/pi_te_all_vs_yield.pdf";
    c_te_all_yield->SaveAs(c_te_all_yield_name.c_str());

    TCanvas* c_te_all_rate = new TCanvas();
    c_te_all_rate->SetGrid();
    G_te_all_pos_vs_rate->SetMarkerColor(kRed);
    G_te_all_pos_vs_rate->SetMarkerStyle(8);
    G_te_all_pos_vs_rate->Draw("ap");
    G_te_all_neg_vs_rate->SetMarkerColor(kBlack);
    G_te_all_neg_vs_rate->SetMarkerStyle(8);
    G_te_all_neg_vs_rate->Draw("p");
    std::string c_te_all_rate_name = "results/TE/pi_te_all_vs_rate.pdf";
    c_te_all_rate->SaveAs(c_te_all_rate_name.c_str());

    TCanvas* c_te_all_yieldrate = new TCanvas();
    c_te_all_yieldrate->SetGrid();
    G_te_all_pos_vs_yieldrate->SetMarkerColor(kRed);
    G_te_all_pos_vs_yieldrate->SetMarkerStyle(8);
    G_te_all_pos_vs_yieldrate->Draw("ap");
    G_te_all_neg_vs_yieldrate->SetMarkerColor(kBlack);
    G_te_all_neg_vs_yieldrate->SetMarkerStyle(8);
    G_te_all_neg_vs_yieldrate->Draw("p");
    std::string c_te_all_yieldrate_name = "results/TE/pi_te_all_vs_yieldrate.pdf";
    c_te_all_yieldrate->SaveAs(c_te_all_yieldrate_name.c_str());
  
    TCanvas* c_te_all_runs = new TCanvas();
    c_te_all_runs->SetGrid();
    G_te_all_pos_runs->SetMarkerColor(kRed);
    G_te_all_pos_runs->SetMarkerStyle(8);
    G_te_all_pos_runs->Draw("ap");
    G_te_all_neg_runs->SetMarkerColor(kBlack);
    G_te_all_neg_runs->SetMarkerStyle(8);
    G_te_all_neg_runs->Draw("p");
    std::string c_te_all_runs_name = "results/TE/pi_te_all_runs.pdf";
    c_te_all_runs->SaveAs(c_te_all_runs_name.c_str());
  
  }
