
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
double Get_average(std::vector<double> TE,std::vector<double> charge){
  double charge_sum=0;
  double TE_charge_sum =0;
  for(auto it = TE.begin();it!=TE.end();++it){
    charge_sum += charge[it-TE.begin()];
    TE_charge_sum+= charge[it-TE.begin()]*(*it);
  }
  return TE_charge_sum/charge_sum;
}
    double Get_SHMS_TE_neg_spring_sys(double rate){
     double TE_mean = 0.9943-0.000027*rate;
     double TE_high = 0.9952-(0.000027-0.000002)*rate;
     return TE_high-TE_mean;
    }
    double Get_SHMS_TE_pos_spring_sys(double rate){
      double TE_mean = 0.9958-0.000027*rate;
      double TE_high = 0.9967-(0.000027-0.000002)*rate;
      return TE_high-TE_mean;
    }
void TE_sys(){

    //get rate info from tracking json. yes too lazy
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }
    json j_info;
    {
      std::ifstream ifs("db2/runs_info.json");
      //std::ifstream ifs("results/Hem_eff.json");
      ifs>>j_info;
    }
    double SHMS_TE_neg_fall_sys = 0.00017;
    double SHMS_TE_pos_fall_sys = 0.00008;
    double HMS_TE_sys = 0.00006;
    TGraph* g_TEratio_RunGroup = new TGraph();
    double i_TEratio=0;
    TGraph* g_TEratio_err_RunGroup = new TGraph();
    double i_TEratio_err = 0;
    TGraph* g_TEratio_fall_err_RunGroup = new TGraph();
    double i_TEratio_fall_err = 0;
    TGraph* g_TEratio_spring_err_RunGroup = new TGraph();
    double i_TEratio_spring_err = 0;
    TGraph* g_TEratio_err_R_RunGroup = new TGraph();
    double i_TEratio_err_R = 0;

    TH1D* h_uncertainty = new TH1D("","TE ratio uncertainty",10,0,0.01);

    int i_rungoup_pos = 0,i_rungroup_neg = 0,i_run_pos = 0,i_run_neg = 0;
    for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
      int RunGroup = std::stoi(it.key());
      std::cout<<RunGroup<<std::endl;
      std::vector<int> neg_D2,pos_D2;
      neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
      pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
      if(!neg_D2.empty() && !pos_D2.empty()){
        std::vector<double> pos_TE,pos_charge,neg_TE,neg_charge,pos_HMSTE,neg_HMSTE,pos_rate,neg_rate;
        for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
           int RunNumber = *i_neg;
           double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
           std::cout<<RunNumber<<", neg "<<TE<<std::endl;
           neg_TE.push_back(TE);
           double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
           std::cout<<RunNumber<<", neg "<<HMSTE<<std::endl;
           neg_HMSTE.push_back(HMSTE);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           neg_charge.push_back(charge);
           double data_n = j_info[(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
           double time = j_info[(std::to_string(RunNumber)).c_str()]["time"].get<double>();
           double rate = data_n/(1000*time);
           neg_rate.push_back(rate);
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
           int RunNumber = *i_pos;
           double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
           std::cout<<RunNumber<<", pos "<<TE<<std::endl;
           pos_TE.push_back(TE);
           double HMSTE = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
           std::cout<<RunNumber<<", pos "<<HMSTE<<std::endl;
           pos_HMSTE.push_back(HMSTE);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           pos_charge.push_back(charge);
           //double rate = j_info[(std::to_string(RunNumber)).c_str()]["PTRIG"].get<double>();
           double data_n = j_info[(std::to_string(RunNumber)).c_str()]["data_n"].get<double>();
           double time = j_info[(std::to_string(RunNumber)).c_str()]["time"].get<double>();
           double rate = data_n/(1000*time);
           pos_rate.push_back(rate);
        }//loop over pos runs

        double neg_TE_num = Get_average(neg_TE,neg_charge);
        double pos_TE_num = Get_average(pos_TE,pos_charge);
        double TE_ratio = neg_TE_num/pos_TE_num;
        g_TEratio_RunGroup ->SetPoint(i_TEratio,RunGroup,TE_ratio);
        i_TEratio+=1;
        
        double pos_rate_num = Get_average(pos_rate,pos_charge);
        double neg_rate_num = Get_average(neg_rate,neg_charge);

        if(RunGroup<420){
        double neg_TE_low = neg_TE_num-SHMS_TE_neg_fall_sys;
        double neg_TE_high = neg_TE_num+SHMS_TE_neg_fall_sys;
        std::cout<<SHMS_TE_neg_fall_sys<<", neg low "<<neg_TE_low<<" neg high "<<neg_TE_high<<std::endl;
        double pos_TE_low = pos_TE_num-SHMS_TE_pos_fall_sys;        
        double pos_TE_high = pos_TE_num+SHMS_TE_pos_fall_sys;
        std::cout<<SHMS_TE_pos_fall_sys<<", pos low "<<pos_TE_low<<" pos high "<<pos_TE_high<<std::endl;
        double TEratio_uncertainty = std::abs((neg_TE_high/pos_TE_high)-(neg_TE_low/pos_TE_low))/TE_ratio;

          g_TEratio_fall_err_RunGroup->SetPoint(i_TEratio_fall_err,RunGroup,TEratio_uncertainty);
          i_TEratio_fall_err++;
        }
        else{
        double neg_TE_low = neg_TE_num-Get_SHMS_TE_neg_spring_sys(neg_rate_num);
        double neg_TE_high = neg_TE_num+Get_SHMS_TE_neg_spring_sys(neg_rate_num);
        std::cout<<Get_SHMS_TE_neg_spring_sys(neg_rate_num)<<", neg low "<<neg_TE_low<<" neg high "<<neg_TE_high<<std::endl;
        double pos_TE_low = pos_TE_num-Get_SHMS_TE_pos_spring_sys(pos_rate_num);        
        double pos_TE_high = pos_TE_num+Get_SHMS_TE_pos_spring_sys(pos_rate_num);
        std::cout<<Get_SHMS_TE_pos_spring_sys(pos_rate_num)<<", pos low "<<pos_TE_low<<" pos high "<<pos_TE_high<<std::endl;
        double TEratio_uncertainty = std::abs((neg_TE_high/pos_TE_high)-(neg_TE_low/pos_TE_low))/TE_ratio;

          g_TEratio_spring_err_RunGroup->SetPoint(i_TEratio_spring_err,RunGroup,TEratio_uncertainty);
          i_TEratio_spring_err++;
        }
      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_TEratio = new TCanvas();
    g_TEratio_RunGroup->Fit("pol0");
    g_TEratio_RunGroup->SetMarkerColor(kRed);
    g_TEratio_RunGroup->SetMarkerStyle(8);
    g_TEratio_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_TEratio_RunGroup->GetYaxis()->SetTitle("neg_TE/pos_TE");
    g_TEratio_RunGroup->Draw("AP");
    std::string c_TEratio_name = "results/sys/TE_ratio.pdf";
    c_TEratio->SaveAs(c_TEratio_name.c_str());

    
    TCanvas* c_TEratio_fall_uncertainty = new TCanvas();
    g_TEratio_fall_err_RunGroup->Fit("pol0");
    gStyle->SetOptFit(1);
    //g_TEratio_fall_err_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_TEratio_fall_err_RunGroup->SetMarkerColor(kRed);
    g_TEratio_fall_err_RunGroup->SetMarkerStyle(8);
    g_TEratio_fall_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_TEratio_fall_err_RunGroup->GetYaxis()->SetTitle("TE ratio uncertainty");
    g_TEratio_fall_err_RunGroup->Draw("AP");
    std::string c_TEratio_fall_uncertainty_name = "results/sys/TE_ratio_fall_uncertainty.pdf";
    c_TEratio_fall_uncertainty->SaveAs(c_TEratio_fall_uncertainty_name.c_str());
    
    TCanvas* c_TEratio_spring_uncertainty = new TCanvas();
    g_TEratio_spring_err_RunGroup->Fit("pol0");
    gStyle->SetOptFit(1);
    //g_TEratio_spring_err_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_TEratio_spring_err_RunGroup->SetMarkerColor(kRed);
    g_TEratio_spring_err_RunGroup->SetMarkerStyle(8);
    g_TEratio_spring_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_TEratio_spring_err_RunGroup->GetYaxis()->SetTitle("TE ratio uncertainty");
    g_TEratio_spring_err_RunGroup->Draw("AP");
    std::string c_TEratio_spring_uncertainty_name = "results/sys/TE_ratio_spring_uncertainty.pdf";
    c_TEratio_spring_uncertainty->SaveAs(c_TEratio_spring_uncertainty_name.c_str());
    
  
}
