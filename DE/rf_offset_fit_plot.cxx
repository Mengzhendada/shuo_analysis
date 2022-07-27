#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"
#include "ROOT/RSnapshotOptions.hxx"
void rf_offset_fit_plot(){

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  TGraph* g_shmsp_D2_fall = new TGraph();
  TGraph* g_rungroup_D2_fall = new TGraph();
  TGraph* g_shmsp_D2_spring = new TGraph();
  TGraph* g_rungroup_D2_spring = new TGraph();
  TMultiGraph* mg_D2_fall = new TMultiGraph();
  TMultiGraph* mg_D2_spring = new TMultiGraph();
  int i_run_shmsp_fall = 0,i_run_rungroup_fall = 0,i_run_shmsp_spring = 0,i_run_rungroup_spring = 0;
  TGraph* g_D2_fall_high = new TGraph();
  TGraph* g_D2_fall_low = new TGraph();
  int i_run_fall_high = 0,i_run_fall_low = 0;
  TGraph* g_D2_spring_high = new TGraph();
  TGraph* g_D2_spring_low = new TGraph();
  int i_run_spring_high = 0,i_run_spring_low = 0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<RunGroup<<std::endl;

    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      json j_high;
      {
        std::string json_name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_compare.json";
        std::ifstream ifs(json_name.c_str());
        ifs>>j_high;
      }
      for(int i_dp = 0;i_dp<5;i_dp++){
        double shms_p = j_high[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["shms_p"].get<double>();
        //std::cout<<"check"<<shms_p<<std::endl;
        double pion_peak = 1;
        if(shms_p<2.8){
          json j_low;
          {
            std::string low_json_name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_low_compare.json";
            std::ifstream ifs(low_json_name.c_str());
            ifs>>j_low;
          }
          pion_peak = j_low[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pi_peak"]["neg"].get<double>();
        }
        else{
          pion_peak = j_high[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pi_peak"]["neg"].get<double>();
        }
        if(RunGroup<420){
          g_shmsp_D2_fall->SetPoint(i_run_shmsp_fall,shms_p,pion_peak);
          g_rungroup_D2_fall->SetPoint(i_run_rungroup_fall,RunGroup,pion_peak);
          i_run_shmsp_fall++;
          i_run_rungroup_fall++;
        }
        else{
          g_shmsp_D2_spring->SetPoint(i_run_shmsp_spring,shms_p,pion_peak);
          g_rungroup_D2_spring->SetPoint(i_run_rungroup_spring,RunGroup,pion_peak);
          i_run_shmsp_spring++;
          i_run_rungroup_spring++;
        }
      }     
    }//if neg and pos not empty
  }//loop over rungroups
  g_shmsp_D2_fall->SetMarkerColor(kBlack);
  g_shmsp_D2_fall->SetMarkerStyle(8);
  g_rungroup_D2_fall->SetMarkerColor(kBlack);
  g_rungroup_D2_fall->SetMarkerStyle(8);
  g_shmsp_D2_spring->SetMarkerColor(kRed);
  g_shmsp_D2_spring->SetMarkerStyle(8);
  g_rungroup_D2_spring->SetMarkerColor(kRed);
  g_rungroup_D2_spring->SetMarkerStyle(8);
  TMultiGraph *mg_rungroup = new TMultiGraph();
  mg_rungroup->Add(g_rungroup_D2_fall,"P");
  mg_rungroup->Add(g_rungroup_D2_spring,"P");
  TCanvas* c_offset_rungroup = new TCanvas();
  mg_rungroup->Draw("AP");
  mg_rungroup->GetXaxis()->SetTitle("RunGroup");
  mg_rungroup->GetYaxis()->SetTitle("neg pion peak");
  c_offset_rungroup->SaveAs("results/pid/rftime_new/offsets_RunGroup_fromfit.pdf");
  TCanvas* c_offset_shmsp = new TCanvas();
  g_shmsp_D2_fall->Draw("AP");
  g_shmsp_D2_spring->Draw("P same");
  g_shmsp_D2_fall->GetXaxis()->SetTitle("shms p");
  g_shmsp_D2_fall->GetYaxis()->SetTitle("neg pion peak");
  c_offset_shmsp->SaveAs("results/pid/rftime_new/offsets_shmsp_fromfit.pdf");
}
