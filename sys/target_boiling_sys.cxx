
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
  void target_boiling_sys(){
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
    //for current between 15 to 65, uncertainty is 0.5%, for current less 10 and greater 65, uncertainty is 0.7% 
    TGraph* g_neg_TB_RunGroup = new TGraph();
    double i_neg_TB=0;
    TGraph* g_pos_TB_RunGroup = new TGraph();
    double i_pos_TB = 0;
    TGraph* g_TB_RunGroup = new TGraph();
    double i_TB = 0;

    TH1D* h_uncertainty = new TH1D("","Target Boiling Uncertainty",10,0,0.5);

    //D2 uncertainty is 0.3% per 100 muA
    double D2_TB_uncertainty = 0.3;
    double H2_TB_uncertainty = 0.32;

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
           std::cout<<RunNumber<<std::endl;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           neg_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           neg_charge.push_back(charge);
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
           int RunNumber = *i_pos;
           std::cout<<RunNumber<<std::endl;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           pos_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           pos_charge.push_back(charge);
        }//loop over pos runs

        double neg_curr = Get_average(neg_current,neg_charge);
        double pos_curr = Get_average(pos_current,pos_charge);
    
        double neg_curr_uncertainty = neg_curr*D2_TB_uncertainty/100;
        double pos_curr_uncertainty = pos_curr*D2_TB_uncertainty/100;

        g_neg_TB_RunGroup->SetPoint(i_neg_TB,RunGroup,neg_curr_uncertainty);
        i_neg_TB+=1;
        g_pos_TB_RunGroup->SetPoint(i_pos_TB,RunGroup,pos_curr_uncertainty);
        i_pos_TB+=1;
        double all_uncertainty = std::sqrt(neg_curr_uncertainty*neg_curr_uncertainty+pos_curr_uncertainty*pos_curr_uncertainty);
        g_TB_RunGroup->SetPoint(i_TB,RunGroup,all_uncertainty);
        i_TB+=1;
    
        h_uncertainty->Fill(all_uncertainty);

        

      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_neg_TB = new TCanvas();
    g_neg_TB_RunGroup->SetMarkerColor(kRed);
    g_neg_TB_RunGroup->SetMarkerStyle(8);
    g_neg_TB_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_neg_TB_RunGroup->GetYaxis()->SetTitle("Negative Target Boiling uncertainty(%)");
    g_neg_TB_RunGroup->Draw("AP");
    std::string c_neg_TB_name = "results/sys/TB_neg.pdf";
    c_neg_TB->SaveAs(c_neg_TB_name.c_str());

    TCanvas* c_pos_TB = new TCanvas();
    g_pos_TB_RunGroup->SetMarkerColor(kRed);
    g_pos_TB_RunGroup->SetMarkerStyle(8);
    g_pos_TB_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_pos_TB_RunGroup->GetYaxis()->SetTitle("Postive Target Boiling uncertainty(%)");
    g_pos_TB_RunGroup->Draw("AP");
    std::string c_pos_TB_name = "results/sys/TB_pos.pdf";
    c_pos_TB->SaveAs(c_pos_TB_name.c_str());
 
    TCanvas* c_TB = new TCanvas();
    g_TB_RunGroup->SetMarkerColor(kRed);
    g_TB_RunGroup->SetMarkerStyle(8);
    g_TB_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_TB_RunGroup->GetYaxis()->SetTitle("Target Boiling uncertainty(%)");
    g_TB_RunGroup->Draw("AP");
    std::string c_TB_name = "results/sys/TB_uncertainty.pdf";
    c_TB->SaveAs(c_TB_name.c_str());

    TCanvas* c_histo = new TCanvas();
    h_uncertainty->Draw("hist");
    h_uncertainty->GetXaxis()->SetTitle("Target Boiling uncertainty(%)");
    c_histo->SaveAs("results/sys/TB_uncertainty_histo.pdf");
  }
