
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
double Get_average(std::vector<double> current,std::vector<double> charge){
  double charge_sum=0;
  double current_charge_sum =0;
  for(auto it = current.begin();it!=current.end();++it){
    charge_sum += charge[it-current.begin()];
    current_charge_sum+= charge[it-current.begin()]*(*it);
  }
  return current_charge_sum/charge_sum;
}
  void current_sys(){
    //get rate info from tracking json. yes too lazy
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }
    json j_info;
    {
      std::ifstream ifs("db2/runs_info.json");
      ifs>>j_info;
    }
    double current_uncertainty = 0.1;
    TGraph* g_currentratio_RunGroup = new TGraph();
    double i_currratio=0;
    TGraph* g_currentratio_err_RunGroup = new TGraph();
    double i_currentratio_err = 0;
    TGraph* g_currentratio_err_R_RunGroup = new TGraph();
    double i_currentratio_err_R = 0;

    TH1D* h_uncertainty = new TH1D("","current ratio uncertainty",10,0,0.01);

    int i_rungoup_pos = 0,i_rungroup_neg = 0,i_run_pos = 0,i_run_neg = 0;
    for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
      int RunGroup = std::stoi(it.key());
      std::cout<<RunGroup<<std::endl;
      std::vector<int> neg_D2,pos_D2;
      neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
      pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
      if(!neg_D2.empty() && !pos_D2.empty()){
        std::vector<double> pos_current,pos_charge,neg_current,neg_charge;
        for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
           int RunNumber = *i_neg;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           std::cout<<RunNumber<<", neg "<<current<<std::endl;
           neg_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           neg_charge.push_back(charge);
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
           int RunNumber = *i_pos;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           std::cout<<RunNumber<<", pos "<<current<<std::endl;
           pos_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           pos_charge.push_back(charge);
        }//loop over pos runs

        double neg_curr = Get_average(neg_current,neg_charge);
        double pos_curr = Get_average(pos_current,pos_charge);
        double curr_ratio = neg_curr/pos_curr;
        g_currentratio_RunGroup ->SetPoint(i_currratio,RunGroup,curr_ratio);
        i_currratio+=1;
        
        double neg_curr_low = neg_curr-current_uncertainty;
        double neg_curr_high = neg_curr+current_uncertainty;
        std::cout<<current_uncertainty<<", neg low "<<neg_curr_low<<" neg high "<<neg_curr_high<<std::endl;
        double pos_curr_low = pos_curr-current_uncertainty;        
        double pos_curr_high = pos_curr+current_uncertainty;
        std::cout<<current_uncertainty<<", pos low "<<pos_curr_low<<" pos high "<<pos_curr_high<<std::endl;
        double currratio_uncertainty = std::abs((neg_curr_high/pos_curr_high)-(neg_curr_low/pos_curr_low))/curr_ratio;
        g_currentratio_err_RunGroup->SetPoint(i_currentratio_err,RunGroup,currratio_uncertainty);
        i_currentratio_err +=1;

        double currratio_uncertainty_R = currratio_uncertainty/curr_ratio;
        g_currentratio_err_R_RunGroup->SetPoint(i_currentratio_err_R,RunGroup,currratio_uncertainty_R);
        i_currentratio_err_R+=1;
  
        h_uncertainty->Fill(currratio_uncertainty);

      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_currratio = new TCanvas();
    g_currentratio_RunGroup->Fit("pol0");
    g_currentratio_RunGroup->SetMarkerColor(kRed);
    g_currentratio_RunGroup->SetMarkerStyle(8);
    g_currentratio_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_RunGroup->GetYaxis()->SetTitle("neg_current/pos_current");
    g_currentratio_RunGroup->Draw("AP");
    std::string c_currratio_name = "results/sys/current_ratio.pdf";
    c_currratio->SaveAs(c_currratio_name.c_str());

    TCanvas* c_currratio_uncertainty = new TCanvas();
    g_currentratio_err_RunGroup->Fit("pol0");
    gStyle->SetOptFit(1);
    //g_currentratio_err_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_currentratio_err_RunGroup->SetMarkerColor(kRed);
    g_currentratio_err_RunGroup->SetMarkerStyle(8);
    g_currentratio_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_err_RunGroup->GetYaxis()->SetTitle("current ratio uncertainty");
    g_currentratio_err_RunGroup->Draw("AP");
    std::string c_currratio_uncertainty_name = "results/sys/current_ratio_uncertainty.pdf";
    c_currratio_uncertainty->SaveAs(c_currratio_uncertainty_name.c_str());
    
    TCanvas* c_currratio_uncertainty_R = new TCanvas();
    //g_currentratio_err_R_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_currentratio_err_R_RunGroup->SetMarkerColor(kRed);
    g_currentratio_err_R_RunGroup->SetMarkerStyle(8);
    g_currentratio_err_R_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_err_R_RunGroup->GetYaxis()->SetTitle("#sigma #Delta/#Delta");
    g_currentratio_err_R_RunGroup->Draw("AP");
    std::string c_currratio_uncertainty_R_name = "results/sys/current_ratio_uncertainty_R.pdf";
    c_currratio_uncertainty_R->SaveAs(c_currratio_uncertainty_R_name.c_str());
  
    TCanvas* c_histo = new TCanvas();
    h_uncertainty->Draw("hist");
    h_uncertainty->GetXaxis()->SetTitle("#sigma #Delta/#Delta");
    c_histo->SaveAs("results/sys/current_ratio_histo.pdf");
  }
