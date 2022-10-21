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
void rf_offset_plot(){

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_info;
  {
    std::ifstream ifs("db2/runs_info_onlyoffset.json");
    ifs>>j_info;
  }
  TGraph* g_neg_D2_fall = new TGraph();
  TGraph* g_pos_D2_fall = new TGraph();
  TGraph* g_neg_D2_spring = new TGraph();
  TGraph* g_pos_D2_spring = new TGraph();
  TMultiGraph* mg_D2_fall = new TMultiGraph();
  TMultiGraph* mg_D2_spring = new TMultiGraph();
  int i_run_neg_fall = 0,i_run_pos_fall = 0,i_run_neg_spring = 0,i_run_pos_spring = 0;
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
      for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){

        int RunNumber = *i_neg;
        double offset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
        if(RunNumber<7000){
          g_neg_D2_fall->SetPoint(i_run_neg_fall,RunNumber,offset);
          i_run_neg_fall++;
          if(offset>400){
            g_D2_fall_high->SetPoint(i_run_fall_high,RunNumber,offset);
            i_run_fall_high++;
          }
          else{
            offset = offset+2.004;
            g_D2_fall_high->SetPoint(i_run_fall_high,RunNumber,offset);
            i_run_fall_high++;
            //g_D2_fall_low->SetPoint(i_run_fall_low,RunNumber,offset);
            //i_run_fall_low++;
          }
        }
        else{
          g_neg_D2_spring->SetPoint(i_run_neg_spring,RunNumber,offset);
          i_run_neg_spring++;
          if(offset>399){
            g_D2_spring_high->SetPoint(i_run_spring_high,RunNumber,offset);
            i_run_spring_high++;
          }
          else{
            offset = offset+2.004;
            g_D2_spring_high->SetPoint(i_run_spring_high,RunNumber,offset);
            i_run_spring_high++;
            //g_D2_spring_low->SetPoint(i_run_spring_low,RunNumber,offset);
            //i_run_spring_low++;
          }
        }

      }//loop over neg D2 runs
      for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){

        int RunNumber = *i_pos;
        double offset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
        if(RunNumber<7000){
          g_pos_D2_fall->SetPoint(i_run_pos_fall,RunNumber,offset);
          i_run_pos_fall++;
          if(offset>400){
            g_D2_fall_high->SetPoint(i_run_fall_high,RunNumber,offset);
            i_run_fall_high++;
          }
          else{
            offset = offset+2.004;
            g_D2_fall_high->SetPoint(i_run_fall_high,RunNumber,offset);
            i_run_fall_high++;
            //g_D2_fall_low->SetPoint(i_run_fall_low,RunNumber,offset);
            //i_run_fall_low++;
          }
        }
        else{
          g_pos_D2_spring->SetPoint(i_run_pos_spring,RunNumber,offset);
          i_run_pos_spring++;
          if(offset>399){
            g_D2_spring_high->SetPoint(i_run_spring_high,RunNumber,offset);
            i_run_spring_high++;
          }
          else{
            offset = offset+2.004;
            g_D2_spring_high->SetPoint(i_run_spring_high,RunNumber,offset);
            i_run_spring_high++;
            //g_D2_spring_low->SetPoint(i_run_spring_low,RunNumber,offset);
            //i_run_spring_low++;
          }
        }

      }//loop over pos D2 runs
    }//if neg and pos not empty
  }//loop over rungroups
  g_neg_D2_fall->SetMarkerColor(kBlack);
  g_neg_D2_fall->SetMarkerStyle(8);
  g_pos_D2_fall->SetMarkerColor(kRed);
  g_pos_D2_fall->SetMarkerStyle(8);
  mg_D2_fall->Add(g_neg_D2_fall,"P");
  mg_D2_fall->Add(g_pos_D2_fall,"P");
  TCanvas* c_offset_fall = new TCanvas();
  g_D2_fall_high->Fit("pol0");
  //g_D2_fall_low->Fit("pol0");
  mg_D2_fall->Draw("AP");
  mg_D2_fall->GetXaxis()->SetTitle("RunNumber");
  mg_D2_fall->GetYaxis()->SetTitle("offset");
  c_offset_fall->SaveAs("results/pid/rftime_new/fall_offsets_fromMaxBin.pdf");
  g_neg_D2_spring->SetMarkerColor(kBlack);
  g_neg_D2_spring->SetMarkerStyle(8);
  g_pos_D2_spring->SetMarkerColor(kRed);
  g_pos_D2_spring->SetMarkerStyle(8);
  mg_D2_spring->Add(g_neg_D2_spring,"P");
  mg_D2_spring->Add(g_pos_D2_spring,"P");
  TCanvas* c_offset_spring = new TCanvas();
  g_D2_spring_high->Fit("pol0");
  //g_D2_spring_low->Fit("pol0");
  mg_D2_spring->Draw("AP");
  mg_D2_spring->GetXaxis()->SetTitle("RunNumber");
  mg_D2_spring->GetYaxis()->SetTitle("offset");
  c_offset_spring->SaveAs("results/pid/rftime_new/spring_offsets_fromMaxBin.pdf");
}
