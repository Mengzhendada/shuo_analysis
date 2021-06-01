
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
  void plot_LT(){
    //get rate info from tracking json. yes too lazy
    json j_te;
    { 
      std::ifstream ifs("results/TE/trackingeff_info_old.json");
      ifs>>j_te;
    }
    json j_LT;
    {
      std::ifstream ifs("shuo_analysis/dbase/TLT.json");
      ifs>>j_LT;
    }
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }

    TGraphErrors* G_TLT_all_pos = new TGraphErrors();
    TGraphErrors* G_TLT_all_neg = new TGraphErrors();
    G_TLT_all_pos->SetTitle("Pos. ;RunGroup;TLT");
    TGraphErrors* G_TLT_all_pos_vs_shmsp = new TGraphErrors();
    TGraphErrors* G_TLT_all_neg_vs_shmsp = new TGraphErrors();
    G_TLT_all_pos_vs_shmsp->SetTitle("SHMS TLT; shmsp; TLT");
    TGraphErrors* G_TLT_all_pos_vs_rate = new TGraphErrors();
    TGraphErrors* G_TLT_all_neg_vs_rate = new TGraphErrors();
    G_TLT_all_pos_vs_rate->SetTitle("Pos. Total LT; rate; TLT");
    G_TLT_all_neg_vs_rate->SetTitle("Neg. Total LT; rate; TLT");
    TGraphErrors* G_TLT_all_pos_vs_yieldrate = new TGraphErrors();
    TGraphErrors* G_TLT_all_neg_vs_yieldrate = new TGraphErrors();
    G_TLT_all_pos_vs_yieldrate->SetTitle("SHMS TLT; yieldrate;TLT");
    TGraphErrors* G_TLT_all_pos_runs = new TGraphErrors();
    TGraphErrors* G_TLT_all_neg_runs = new TGraphErrors();
    G_TLT_all_pos_runs->SetTitle("Pos. Total LT; runs;TLT");
    G_TLT_all_neg_runs->SetTitle("Neg. Total LT; runs;TLT");
    
    TGraphErrors* G_CLTA_all_pos = new TGraphErrors();
    TGraphErrors* G_CLTA_all_neg = new TGraphErrors();
    G_CLTA_all_pos->SetTitle("SHMS CLTA;RunGroup;CLTA");
    TGraphErrors* G_CLTA_all_pos_vs_shmsp = new TGraphErrors();
    TGraphErrors* G_CLTA_all_neg_vs_shmsp = new TGraphErrors();
    G_CLTA_all_pos_vs_shmsp->SetTitle("SHMS CLTA; shmsp; CLTA");
    TGraphErrors* G_CLTA_all_pos_vs_rate = new TGraphErrors();
    TGraphErrors* G_CLTA_all_neg_vs_rate = new TGraphErrors();
    G_CLTA_all_pos_vs_rate->SetTitle("Pos. Computer LT all; rate; CLTA");
    G_CLTA_all_neg_vs_rate->SetTitle("Neg. Computer LT all; rate; CLTA");
    TGraphErrors* G_CLTA_all_pos_vs_yieldrate = new TGraphErrors();
    TGraphErrors* G_CLTA_all_neg_vs_yieldrate = new TGraphErrors();
    G_CLTA_all_pos_vs_yieldrate->SetTitle("SHMS CLTA; yieldrate;CLTA");
    TGraphErrors* G_CLTA_all_pos_runs = new TGraphErrors();
    TGraphErrors* G_CLTA_all_neg_runs = new TGraphErrors();
    G_CLTA_all_pos_runs->SetTitle("Pos. Computer LT all; runs;CLTA");
    G_CLTA_all_neg_runs->SetTitle("Neg. Computer LT all; runs;CLTA");

    TGraphErrors* G_CLTP_all_pos = new TGraphErrors();
    TGraphErrors* G_CLTP_all_neg = new TGraphErrors();
    G_CLTP_all_pos->SetTitle("SHMS CLTP;RunGroup;CLTP");
    TGraphErrors* G_CLTP_all_pos_vs_shmsp = new TGraphErrors();
    TGraphErrors* G_CLTP_all_neg_vs_shmsp = new TGraphErrors();
    G_CLTP_all_pos_vs_shmsp->SetTitle("SHMS CLTP; shmsp; CLTP");
    TGraphErrors* G_CLTP_all_pos_vs_rate = new TGraphErrors();
    TGraphErrors* G_CLTP_all_neg_vs_rate = new TGraphErrors();
    G_CLTP_all_pos_vs_rate->SetTitle("Pos. Computer LT phy; rate; CLTP");
    G_CLTP_all_neg_vs_rate->SetTitle("Neg. Computer LT phy; rate; CLTP");
    TGraphErrors* G_CLTP_all_pos_vs_yieldrate = new TGraphErrors();
    TGraphErrors* G_CLTP_all_neg_vs_yieldrate = new TGraphErrors();
    G_CLTP_all_pos_vs_yieldrate->SetTitle("SHMS CLTP; yieldrate;CLTP");
    TGraphErrors* G_CLTP_all_pos_runs = new TGraphErrors();
    TGraphErrors* G_CLTP_all_neg_runs = new TGraphErrors();
    G_CLTP_all_pos_runs->SetTitle("Pos. Computer LT phy; runs;CLTP");
    G_CLTP_all_neg_runs->SetTitle("Neg. Computer LT phy; runs;CLTP");

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
           if(j_LT[(std::to_string(RunNumber)).c_str()].find("TLT") != j_LT[(std::to_string(RunNumber)).c_str()].end()){
           double TLT = j_LT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
           double TLT_error = j_LT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<double>();
           double CLTA = j_LT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
           double CLTP = j_LT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
           std::cout<<"check"<<std::endl; 
           double time = j_te[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["time"].get<double>();
           double counts = j_te[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
           double rate = counts/(1000*time);
           G_TLT_all_neg_runs->SetPoint(i_run_neg,RunNumber,TLT);
           G_TLT_all_neg_vs_rate->SetPoint(i_run_neg,rate,TLT);
           G_TLT_all_neg_vs_rate->SetPointError(i_run_neg,0,TLT_error);           
           G_CLTA_all_neg_runs->SetPoint(i_run_neg,RunNumber,CLTA);
           G_CLTA_all_neg_vs_rate->SetPoint(i_run_neg,rate,CLTA);
           G_CLTP_all_neg_runs->SetPoint(i_run_neg,RunNumber,CLTP);
           G_CLTP_all_neg_vs_rate->SetPoint(i_run_neg,rate,CLTP);
           i_run_neg++;
           }
           else std::cout<<"Failed to find LT for run "<<RunNumber<<std::endl;
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
           int RunNumber = *i_pos;
           std::cout<<RunNumber<<std::endl;
           if(j_LT[(std::to_string(RunNumber)).c_str()].find("TLT") != j_LT[(std::to_string(RunNumber)).c_str()].end()){
           double TLT = j_LT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
           double TLT_error = j_LT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<double>();
           double CLTA = j_LT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
           double CLTP = j_LT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
           std::cout<<"check"<<std::endl; 
           double time = j_te[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["time"].get<double>();
           double counts = j_te[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
           double rate = counts/(1000*time);
           G_TLT_all_pos_runs->SetPoint(i_run_pos,RunNumber,TLT);
           G_TLT_all_pos_vs_rate->SetPoint(i_run_pos,rate,TLT);
           G_TLT_all_pos_vs_rate->SetPointError(i_run_pos,0,TLT_error);           
           G_CLTA_all_pos_runs->SetPoint(i_run_pos,RunNumber,CLTA);
           G_CLTA_all_pos_vs_rate->SetPoint(i_run_pos,rate,CLTA);
           G_CLTP_all_pos_runs->SetPoint(i_run_pos,RunNumber,CLTP);
           G_CLTP_all_pos_vs_rate->SetPoint(i_run_pos,rate,CLTP);
           i_run_pos++;
           }
           else std::cout<<"Failed to find LT for run "<<RunNumber<<std::endl;
        }//loop over pos runs

      }//if normal production runs
    }//loop over rungroups
        TCanvas* c_TLT_runs = new TCanvas();
        G_TLT_all_pos_runs->SetMarkerColor(kRed);
        G_TLT_all_pos_runs->SetMarkerStyle(8);
        G_TLT_all_neg_runs->SetMarkerStyle(8);
        G_TLT_all_neg_runs->Draw("AP");
        G_TLT_all_pos_runs->Draw("P");
        std::string c_TLT_runs_name = "results/LT/TLT_runs.pdf";
        c_TLT_runs->BuildLegend(0.75,0.75,0.95,0.95);
        c_TLT_runs->SaveAs(c_TLT_runs_name.c_str());
    
        TCanvas* c_CLTA_runs = new TCanvas();
        G_CLTA_all_pos_runs->SetMarkerColor(kRed);
        G_CLTA_all_pos_runs->SetMarkerStyle(8);
        G_CLTA_all_neg_runs->SetMarkerStyle(8);
        G_CLTA_all_neg_runs->Draw("AP");
        G_CLTA_all_pos_runs->Draw("P");
        c_CLTA_runs->BuildLegend(0.75,0.75,0.95,0.95);
        std::string c_CLTA_runs_name = "results/LT/CLTA_runs.pdf";
        c_CLTA_runs->SaveAs(c_CLTA_runs_name.c_str());
        
        TCanvas* c_CLTP_runs = new TCanvas();
        G_CLTP_all_pos_runs->SetMarkerColor(kRed);
        G_CLTP_all_pos_runs->SetMarkerStyle(8);
        G_CLTP_all_neg_runs->SetMarkerStyle(8);
        G_CLTP_all_neg_runs->Draw("AP");
        G_CLTP_all_pos_runs->Draw("P");
        c_CLTP_runs->BuildLegend(0.75,0.75,0.95,0.95);
        std::string c_CLTP_runs_name = "results/LT/CLTP_runs.pdf";
        c_CLTP_runs->SaveAs(c_CLTP_runs_name.c_str());
        
        TCanvas* c_TLT_rate = new TCanvas();
        G_TLT_all_neg_vs_rate->SetMarkerColor(kRed);
        //G_TLT_all_pos_vs_rate->SetMarkerColor(kRed);
        G_TLT_all_pos_vs_rate->SetMarkerStyle(8);
        G_TLT_all_neg_vs_rate->SetMarkerStyle(8);
        auto mg = new TMultiGraph();
        mg->Add(G_TLT_all_neg_vs_rate,"P");
        mg->Add(G_TLT_all_pos_vs_rate,"P");
        mg->SetTitle("Total Live Time");
        //mg->GetXaxis()->SetTitle("SHMS 3/4 rate (kHz)");
        //mg->GetYaxis()->SetTitle("TLT");
        mg->SetMinimum(0.96);
        mg->SetMaximum(1.01);
        mg->Draw("A");
        mg->GetXaxis()->SetAxisColor(17);
        mg->GetXaxis()->CenterTitle(true);
        mg->GetYaxis()->SetAxisColor(17);
        mg->GetYaxis()->CenterTitle(true);
        //c_TLT_rate->BuildLegend(0.75,0.75,0.95,0.95);
        std::string c_TLT_rate_name = "results/LT/TLT_rate.pdf";
        c_TLT_rate->SaveAs(c_TLT_rate_name.c_str());
    
        TCanvas* c_CLTA_rate = new TCanvas();
        G_CLTA_all_pos_vs_rate->SetMarkerColor(kRed);
        G_CLTA_all_pos_vs_rate->SetMarkerStyle(8);
        G_CLTA_all_neg_vs_rate->SetMarkerStyle(8);
        G_CLTA_all_neg_vs_rate->Draw("AP");
        G_CLTA_all_pos_vs_rate->Draw("P");
        c_CLTA_rate->BuildLegend(0.75,0.75,0.95,0.95);
        std::string c_CLTA_rate_name = "results/LT/CLTA_rate.pdf";
        c_CLTA_rate->SaveAs(c_CLTA_rate_name.c_str());
        
        TCanvas* c_CLTP_rate = new TCanvas();
        G_CLTP_all_pos_vs_rate->SetMarkerColor(kRed);
        G_CLTP_all_pos_vs_rate->SetMarkerStyle(8);
        G_CLTP_all_neg_vs_rate->SetMarkerStyle(8);
        G_CLTP_all_neg_vs_rate->Draw("AP");
        G_CLTP_all_pos_vs_rate->Draw("P");
        c_CLTP_rate->BuildLegend(0.75,0.75,0.95,0.95);
        std::string c_CLTP_rate_name = "results/LT/CLTP_rate.pdf";
        c_CLTP_rate->SaveAs(c_CLTP_rate_name.c_str());
  }
