
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
  double Get_average(std::vector<double> rate,std::vector<double> charge){
    double charge_sum=0;
    double rate_charge_sum =0;
    for(auto it = rate.begin();it!=rate.end();++it){
      charge_sum += charge[it-rate.begin()];
      rate_charge_sum+= charge[it-rate.begin()]*(*it);
    }
    return rate_charge_sum/charge_sum;
  }
double Get_FADC_eff(double SHMS_rate,double HMS_rate){
  return 1- 0.05*(SHMS_rate/1000 + HMS_rate/1000);
  //50ns is the slop. 50+-30
  //1- 0.05*(shms_3/4 rate in khz/1000 - hms_elReal_rate in Khz/1000)
}
double Get_FADC_eff_max(double SHMS_rate,double HMS_rate){
  return 1- 0.02*(SHMS_rate/1000 + HMS_rate/1000);
  //50ns is the slop. 50+-30
  //1- 0.05*(shms_3/4 rate in khz/1000 - hms_elReal_rate in Khz/1000)
}
double Get_FADC_eff_min(double SHMS_rate,double HMS_rate){
  return 1- 0.08*(SHMS_rate/1000 + HMS_rate/1000);
  //50ns is the slop. 50+-30
  //1- 0.05*(shms_3/4 rate in khz/1000 - hms_elReal_rate in Khz/1000)
}
void FADC_sys(){

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
  TGraph* g_FADCratio_pos_RunGroup = new TGraph();
  TGraph* g_FADCratio_neg_RunGroup = new TGraph();
  double i_FADCratio=0;
  TGraph* g_FADCratio_pos_err_RunGroup = new TGraph();
  TGraph* g_FADCratio_neg_err_RunGroup = new TGraph();
  double i_FADCratio_err = 0;
  TGraph* g_FADCratio_err_RunGroup = new TGraph();
  TGraph* g_FADCratio_spring_err_RunGroup = new TGraph();
  TGraph* g_FADCratio_fall_err_RunGroup = new TGraph();
  double i_spring =0;
  double i_fall = 0;

  TH1D* h_uncertainty = new TH1D("","FADC ratio uncertainty",10,0,0.01);

  int i_rungoup_pos = 0,i_rungroup_neg = 0,i_run_pos = 0,i_run_neg = 0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      std::vector<double> pos_SHMSrate,pos_HMSrate,pos_charge,neg_SHMSrate,neg_HMSrate,neg_charge;
      for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
        int RunNumber = *i_neg;
        std::string report_file_name = "report/replay_coin_production_"+std::to_string(RunNumber)+"_-1.report";
        std::ifstream report_file;
        report_file.open(report_file_name.c_str());
        std::string SHMS_rate_str = "SHMS 3/4 Trigger Rate";
        std::string HMS_rate_str = "HMS_hEL_REAL";
        double SHMS_rate,HMS_rate;
        std::string line;
        while(std::getline(report_file,line)){
          if(line.find(SHMS_rate_str)<line.length()){
            //std::cout<<line<<std::endl;
            size_t i = 0;
            i = line.find(':');
            std::string SHMS_rate_str2 = line.substr(i+1,line.length()-1);
            //std::cout<<SHMS_rate_str2<<std::endl;
            i = 0;
            for(;i<SHMS_rate_str2.length();i++){if(!isdigit(SHMS_rate_str2[i])) break;}
            SHMS_rate = std::stod(SHMS_rate_str2.substr(0,i-1));
            //std::cout<<"SHMS rate "<<SHMS_rate<<std::endl;    
          }
          if(line.find(HMS_rate_str)<line.length()){
            //std::cout<<line<<std::endl;
            size_t i = 0;
            i = line.find('[');
            std::string HMS_rate_str2 = line.substr(i+1,line.length()-1);
            //std::cout<<HMS_rate_str2<<std::endl;
            //std::cout<<std::stod(HMS_rate_str2)<<std::endl;
            i = 0;
            for(;i<HMS_rate_str2.length();i++){if(!isdigit(HMS_rate_str2[i])) break;}
            HMS_rate = std::stod(HMS_rate_str2.substr(0,i-1));
            //std::cout<<"HMS rate "<<HMS_rate<<std::endl;    
          }
        }
        neg_SHMSrate.push_back(SHMS_rate);
        neg_HMSrate.push_back(HMS_rate);
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        neg_charge.push_back(charge);
      }//loop over neg runs
      for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
        int RunNumber = *i_pos;
        std::string report_file_name = "report/replay_coin_production_"+std::to_string(RunNumber)+"_-1.report";
        std::ifstream report_file;
        report_file.open(report_file_name.c_str());
        std::string SHMS_rate_str = "SHMS 3/4 Trigger Rate";
        std::string HMS_rate_str = "HMS_hEL_REAL";
        double SHMS_rate,HMS_rate;
        std::string line;
        while(std::getline(report_file,line)){
          if(line.find(SHMS_rate_str)<line.length()){
            //std::cout<<line<<std::endl;
            size_t i = 0;
            i = line.find(':');
            std::string SHMS_rate_str2 = line.substr(i+1,line.length()-1);
            //std::cout<<SHMS_rate_str2<<std::endl;
            i = 0;
            for(;i<SHMS_rate_str2.length();i++){if(!isdigit(SHMS_rate_str2[i])) break;}
            SHMS_rate = std::stod(SHMS_rate_str2.substr(0,i-1));
            //std::cout<<"SHMS rate "<<SHMS_rate<<std::endl;    
          }
          if(line.find(HMS_rate_str)<line.length()){
            //std::cout<<line<<std::endl;
            size_t i = 0;
            i = line.find('[');
            std::string HMS_rate_str2 = line.substr(i+1,line.length()-1);
            //std::cout<<HMS_rate_str2<<std::endl;
            //std::cout<<std::stod(HMS_rate_str2)<<std::endl;
            i = 0;
            for(;i<HMS_rate_str2.length();i++){if(!isdigit(HMS_rate_str2[i])) break;}
            HMS_rate = std::stod(HMS_rate_str2.substr(0,i-1));
            //std::cout<<"HMS rate "<<HMS_rate<<std::endl;    
          }
        }
        pos_SHMSrate.push_back(SHMS_rate);
        pos_HMSrate.push_back(HMS_rate);
        double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
        pos_charge.push_back(charge);
      }//loop over pos runs
  
      double pos_SHMS_rate = Get_average(pos_SHMSrate,pos_charge);
      double pos_HMS_rate = Get_average(pos_HMSrate,pos_charge);
      double neg_SHMS_rate = Get_average(neg_SHMSrate,neg_charge);
      double neg_HMS_rate = Get_average(neg_HMSrate,neg_charge);

      double pos_FADC_eff = Get_FADC_eff(pos_SHMS_rate,pos_HMS_rate);
      double pos_FADC_eff_max = Get_FADC_eff_max(pos_SHMS_rate,pos_HMS_rate);
      double pos_FADC_eff_min = Get_FADC_eff_min(pos_SHMS_rate,pos_HMS_rate);
      std::cout<<pos_FADC_eff<<"pos, max "<<pos_FADC_eff_max<<",min "<<pos_FADC_eff_min<<std::endl;

      double neg_FADC_eff = Get_FADC_eff(neg_SHMS_rate,neg_HMS_rate);
      double neg_FADC_eff_max = Get_FADC_eff_max(neg_SHMS_rate,neg_HMS_rate);
      double neg_FADC_eff_min = Get_FADC_eff_min(neg_SHMS_rate,neg_HMS_rate);
      std::cout<<neg_FADC_eff<<"neg, max "<<neg_FADC_eff_max<<",min "<<neg_FADC_eff_min<<std::endl;
   
      double pos_FADC_uncertain = (pos_FADC_eff_max-pos_FADC_eff_min)/pos_FADC_eff;
      double neg_FADC_uncertain = (neg_FADC_eff_max-neg_FADC_eff_min)/neg_FADC_eff;
      double FADC_ratio_uncertain = std::abs(neg_FADC_eff_max/pos_FADC_eff_max-neg_FADC_eff_min/pos_FADC_eff_min)/(neg_FADC_eff/pos_FADC_eff);
      g_FADCratio_pos_RunGroup->SetPoint(i_FADCratio,RunGroup,pos_FADC_eff);
      g_FADCratio_neg_RunGroup->SetPoint(i_FADCratio,RunGroup,neg_FADC_eff);
      g_FADCratio_pos_err_RunGroup->SetPoint(i_FADCratio,RunGroup,pos_FADC_uncertain);
      g_FADCratio_neg_err_RunGroup->SetPoint(i_FADCratio,RunGroup,neg_FADC_uncertain);
      g_FADCratio_err_RunGroup->SetPoint(i_FADCratio,RunGroup,FADC_ratio_uncertain);
      i_FADCratio++;

      if(RunGroup<420){
        g_FADCratio_fall_err_RunGroup->SetPoint(i_fall,RunGroup,FADC_ratio_uncertain);
        i_fall++;
      }
      else{
        g_FADCratio_spring_err_RunGroup->SetPoint(i_spring,RunGroup,FADC_ratio_uncertain);
        i_spring++;
      }
    }//not empty
  }//loop over rungroup

  TCanvas* c_FADC = new TCanvas();
  g_FADCratio_pos_RunGroup->SetMarkerColor(kRed);
  g_FADCratio_pos_RunGroup->SetMarkerStyle(8);
  g_FADCratio_pos_RunGroup->GetXaxis()->SetTitle("RunGroup");
  g_FADCratio_pos_RunGroup->GetYaxis()->SetTitle("FADC eff");
  g_FADCratio_pos_RunGroup->Draw("AP");
  g_FADCratio_neg_RunGroup->SetMarkerStyle(8);
  g_FADCratio_neg_RunGroup->Draw("P same");
  c_FADC->SaveAs("results/sys/FADC.pdf");
  
  TCanvas* c_FADC_err = new TCanvas();
  g_FADCratio_pos_err_RunGroup->SetMarkerStyle(8);
  g_FADCratio_pos_err_RunGroup->SetMarkerColor(kRed);
  g_FADCratio_pos_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
  g_FADCratio_pos_err_RunGroup->GetYaxis()->SetTitle("FADC eff err");
  g_FADCratio_pos_err_RunGroup->Draw("AP");
  g_FADCratio_neg_err_RunGroup->SetMarkerStyle(8);
  g_FADCratio_neg_err_RunGroup->Draw("P same");
  c_FADC_err->SaveAs("results/sys/FADC_err.pdf");
  
  TCanvas* c_FADC_ratio_err = new TCanvas();
  g_FADCratio_err_RunGroup->SetMarkerStyle(8);
  g_FADCratio_err_RunGroup->SetMarkerColor(kRed);
  g_FADCratio_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
  g_FADCratio_err_RunGroup->GetYaxis()->SetTitle("FADC eff ratio err");
  g_FADCratio_err_RunGroup->Draw("AP");
  c_FADC_ratio_err->SaveAs("results/sys/FADC_ratio_err.pdf");

  TCanvas* c_FADC_fall_err = new TCanvas();
  g_FADCratio_fall_err_RunGroup->Fit("pol0");
  gStyle->SetOptFit(1);
  g_FADCratio_fall_err_RunGroup->SetMarkerStyle(8);
  g_FADCratio_fall_err_RunGroup->SetMarkerColor(kRed);
  g_FADCratio_fall_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
  g_FADCratio_fall_err_RunGroup->GetYaxis()->SetTitle("FADC eff ratio err");
  g_FADCratio_fall_err_RunGroup->Draw("AP");
  c_FADC_fall_err->SaveAs("results/sys/FADC_ratio_fall_err.pdf");

  TCanvas* c_FADC_spring_err = new TCanvas();
  g_FADCratio_spring_err_RunGroup->Fit("pol0");
  gStyle->SetOptFit(1);
  g_FADCratio_spring_err_RunGroup->SetMarkerStyle(8);
  g_FADCratio_spring_err_RunGroup->SetMarkerColor(kRed);
  g_FADCratio_spring_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
  g_FADCratio_spring_err_RunGroup->GetYaxis()->SetTitle("FADC eff ratio err");
  g_FADCratio_spring_err_RunGroup->Draw("AP");
  c_FADC_spring_err->SaveAs("results/sys/FADC_ratio_spring_err.pdf");

}

