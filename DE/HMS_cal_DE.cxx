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
#include<bits/stdc++.h>
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
#include "TLine.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

double Get_HMS_Cal_Eff(int RunNumber){
  json j_HMS;
  {
    std::string HMS_DE_name = "results/pid/DE/HMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_HMS(HMS_DE_name.c_str());
    ifs_HMS>>j_HMS;
  }
  //for electron eff
  double e_all = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
  double e_did = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcut"].get<double>();  
  double eff = e_did/e_all;
    return eff;

}
double Get_HMS_Cal_Eff_err(int RunNumber){
  json j_HMS;
  {
    std::string HMS_DE_name = "results/pid/DE/HMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_HMS(HMS_DE_name.c_str());
    ifs_HMS>>j_HMS;
  }
  //for electron eff
  double e_all = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
  double e_did = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcut"].get<double>();  
  double eff = e_did/e_all;
  return sqrt(eff*(1-eff)/e_all);

}

void HMS_cal_DE(){

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_runsinfo;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_runsinfo;
  }
  TGraphErrors* g_HMScalDE_D2_pos_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_pos_RunNumber_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_D2_neg_RunNumber_sidis = new TGraphErrors();

  auto mg_HMScalDE_D2_momentum = new TMultiGraph();
  auto mg_HMScalDE_D2_RunGroup = new TMultiGraph();
  auto mg_HMScalDE_D2_RunNumber = new TMultiGraph();

  int i_HMScalDE_D2_pos_momentum_fall = 0,i_HMScalDE_D2_pos_momentum_spring = 0,i_HMScalDE_D2_pos_momentum_sidis = 0,i_HMScalDE_D2_pos_RunGroup_fall = 0,i_HMScalDE_D2_pos_RunGroup_spring = 0,i_HMScalDE_D2_pos_RunGroup_sidis = 0,i_HMScalDE_D2_pos_RunNumber_fall = 0,i_HMScalDE_D2_pos_RunNumber_spring = 0,i_HMScalDE_D2_pos_RunNumber_sidis = 0;
  int i_HMScalDE_D2_neg_momentum_fall = 0,i_HMScalDE_D2_neg_momentum_spring = 0,i_HMScalDE_D2_neg_momentum_sidis = 0,i_HMScalDE_D2_neg_RunGroup_fall = 0,i_HMScalDE_D2_neg_RunGroup_spring = 0,i_HMScalDE_D2_neg_RunGroup_sidis = 0,i_HMScalDE_D2_neg_RunNumber_fall = 0,i_HMScalDE_D2_neg_RunNumber_spring = 0,i_HMScalDE_D2_neg_RunNumber_sidis = 0;

  TGraphErrors* g_HMScalDE_Dummy_pos_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_pos_RunNumber_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_Dummy_neg_RunNumber_sidis = new TGraphErrors();

  auto mg_HMScalDE_Dummy_momentum = new TMultiGraph();
  auto mg_HMScalDE_Dummy_RunGroup = new TMultiGraph();
  auto mg_HMScalDE_Dummy_RunNumber = new TMultiGraph();

  int i_HMScalDE_Dummy_pos_momentum_fall = 0,i_HMScalDE_Dummy_pos_momentum_spring = 0,i_HMScalDE_Dummy_pos_momentum_sidis = 0,i_HMScalDE_Dummy_pos_RunGroup_fall = 0,i_HMScalDE_Dummy_pos_RunGroup_spring = 0,i_HMScalDE_Dummy_pos_RunGroup_sidis = 0,i_HMScalDE_Dummy_pos_RunNumber_fall = 0,i_HMScalDE_Dummy_pos_RunNumber_spring = 0,i_HMScalDE_Dummy_pos_RunNumber_sidis = 0;
  int i_HMScalDE_Dummy_neg_momentum_fall = 0,i_HMScalDE_Dummy_neg_momentum_spring = 0,i_HMScalDE_Dummy_neg_momentum_sidis = 0,i_HMScalDE_Dummy_neg_RunGroup_fall = 0,i_HMScalDE_Dummy_neg_RunGroup_spring = 0,i_HMScalDE_Dummy_neg_RunGroup_sidis = 0,i_HMScalDE_Dummy_neg_RunNumber_fall = 0,i_HMScalDE_Dummy_neg_RunNumber_spring = 0,i_HMScalDE_Dummy_neg_RunNumber_sidis = 0;

  TGraphErrors* g_HMScalDE_H2_pos_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_pos_RunNumber_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_momentum_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_momentum_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_momentum_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunGroup_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunGroup_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunGroup_sidis = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunNumber_fall = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunNumber_spring = new TGraphErrors();
  TGraphErrors* g_HMScalDE_H2_neg_RunNumber_sidis = new TGraphErrors();

  auto mg_HMScalDE_H2_momentum = new TMultiGraph();
  auto mg_HMScalDE_H2_RunGroup = new TMultiGraph();
  auto mg_HMScalDE_H2_RunNumber = new TMultiGraph();

  int i_HMScalDE_H2_pos_momentum_fall = 0,i_HMScalDE_H2_pos_momentum_spring = 0,i_HMScalDE_H2_pos_momentum_sidis = 0,i_HMScalDE_H2_pos_RunGroup_fall = 0,i_HMScalDE_H2_pos_RunGroup_spring = 0,i_HMScalDE_H2_pos_RunGroup_sidis = 0,i_HMScalDE_H2_pos_RunNumber_fall = 0,i_HMScalDE_H2_pos_RunNumber_spring = 0,i_HMScalDE_H2_pos_RunNumber_sidis = 0;
  int i_HMScalDE_H2_neg_momentum_fall = 0,i_HMScalDE_H2_neg_momentum_spring = 0,i_HMScalDE_H2_neg_momentum_sidis = 0,i_HMScalDE_H2_neg_RunGroup_fall = 0,i_HMScalDE_H2_neg_RunGroup_spring = 0,i_HMScalDE_H2_neg_RunGroup_sidis = 0,i_HMScalDE_H2_neg_RunNumber_fall = 0,i_HMScalDE_H2_neg_RunNumber_spring = 0,i_HMScalDE_H2_neg_RunNumber_sidis = 0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    int RunGroup = std::stoi(it.key());
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    double hms_p = j_rungroup[(std::to_string(RunGroup)).c_str()]["hms_p"].get<double>();
    if(!neg_D2.empty() && !pos_D2.empty()){

      //loop over each pos runs data
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_D2_pos_momentum_spring->SetPoint(i_HMScalDE_D2_pos_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_pos_momentum_spring->SetPointError(i_HMScalDE_D2_pos_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_momentum_spring;
            g_HMScalDE_D2_pos_RunGroup_spring->SetPoint(i_HMScalDE_D2_pos_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunGroup_spring->SetPointError(i_HMScalDE_D2_pos_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunGroup_spring;
            g_HMScalDE_D2_pos_RunNumber_spring->SetPoint(i_HMScalDE_D2_pos_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunNumber_spring->SetPointError(i_HMScalDE_D2_pos_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_D2_pos_momentum_fall->SetPoint(i_HMScalDE_D2_pos_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_pos_momentum_fall->SetPointError(i_HMScalDE_D2_pos_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_momentum_fall;
            g_HMScalDE_D2_pos_RunGroup_fall->SetPoint(i_HMScalDE_D2_pos_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunGroup_fall->SetPointError(i_HMScalDE_D2_pos_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunGroup_fall;
            g_HMScalDE_D2_pos_RunNumber_fall->SetPoint(i_HMScalDE_D2_pos_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunNumber_fall->SetPointError(i_HMScalDE_D2_pos_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunNumber_fall;
          }
          else {
            g_HMScalDE_D2_pos_momentum_sidis->SetPoint(i_HMScalDE_D2_pos_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_pos_momentum_sidis->SetPointError(i_HMScalDE_D2_pos_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_momentum_sidis;
            g_HMScalDE_D2_pos_RunGroup_sidis->SetPoint(i_HMScalDE_D2_pos_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunGroup_sidis->SetPointError(i_HMScalDE_D2_pos_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunGroup_sidis;
            g_HMScalDE_D2_pos_RunNumber_sidis->SetPoint(i_HMScalDE_D2_pos_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_pos_RunNumber_sidis->SetPointError(i_HMScalDE_D2_pos_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_pos_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_D2_neg_momentum_spring->SetPoint(i_HMScalDE_D2_neg_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_neg_momentum_spring->SetPointError(i_HMScalDE_D2_neg_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_momentum_spring;
            g_HMScalDE_D2_neg_RunGroup_spring->SetPoint(i_HMScalDE_D2_neg_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunGroup_spring->SetPointError(i_HMScalDE_D2_neg_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunGroup_spring;
            g_HMScalDE_D2_neg_RunNumber_spring->SetPoint(i_HMScalDE_D2_neg_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunNumber_spring->SetPointError(i_HMScalDE_D2_neg_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_D2_neg_momentum_fall->SetPoint(i_HMScalDE_D2_neg_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_neg_momentum_fall->SetPointError(i_HMScalDE_D2_neg_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_momentum_fall;
            g_HMScalDE_D2_neg_RunGroup_fall->SetPoint(i_HMScalDE_D2_neg_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunGroup_fall->SetPointError(i_HMScalDE_D2_neg_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunGroup_fall;
            g_HMScalDE_D2_neg_RunNumber_fall->SetPoint(i_HMScalDE_D2_neg_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunNumber_fall->SetPointError(i_HMScalDE_D2_neg_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunNumber_fall;
          }
          else {
            g_HMScalDE_D2_neg_momentum_sidis->SetPoint(i_HMScalDE_D2_neg_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_D2_neg_momentum_sidis->SetPointError(i_HMScalDE_D2_neg_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_momentum_sidis;
            g_HMScalDE_D2_neg_RunGroup_sidis->SetPoint(i_HMScalDE_D2_neg_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunGroup_sidis->SetPointError(i_HMScalDE_D2_neg_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunGroup_sidis;
            g_HMScalDE_D2_neg_RunNumber_sidis->SetPoint(i_HMScalDE_D2_neg_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_D2_neg_RunNumber_sidis->SetPointError(i_HMScalDE_D2_neg_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_D2_neg_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each neg runs data
    }//if normal production runs
    std::vector<int> neg_Dummy,pos_Dummy;
    neg_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["Dummy"].get<std::vector<int>>();
    pos_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["Dummy"].get<std::vector<int>>();

    if(!neg_Dummy.empty() && !pos_Dummy.empty()){
      //loop over each pos runs data
      for(auto it = pos_Dummy.begin();it!=pos_Dummy.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_Dummy_pos_momentum_spring->SetPoint(i_HMScalDE_Dummy_pos_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_momentum_spring->SetPointError(i_HMScalDE_Dummy_pos_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_momentum_spring;
            g_HMScalDE_Dummy_pos_RunGroup_spring->SetPoint(i_HMScalDE_Dummy_pos_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunGroup_spring->SetPointError(i_HMScalDE_Dummy_pos_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunGroup_spring;
            g_HMScalDE_Dummy_pos_RunNumber_spring->SetPoint(i_HMScalDE_Dummy_pos_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunNumber_spring->SetPointError(i_HMScalDE_Dummy_pos_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_Dummy_pos_momentum_fall->SetPoint(i_HMScalDE_Dummy_pos_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_momentum_fall->SetPointError(i_HMScalDE_Dummy_pos_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_momentum_fall;
            g_HMScalDE_Dummy_pos_RunGroup_fall->SetPoint(i_HMScalDE_Dummy_pos_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunGroup_fall->SetPointError(i_HMScalDE_Dummy_pos_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunGroup_fall;
            g_HMScalDE_Dummy_pos_RunNumber_fall->SetPoint(i_HMScalDE_Dummy_pos_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunNumber_fall->SetPointError(i_HMScalDE_Dummy_pos_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunNumber_fall;
          }
          else {
            g_HMScalDE_Dummy_pos_momentum_sidis->SetPoint(i_HMScalDE_Dummy_pos_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_momentum_sidis->SetPointError(i_HMScalDE_Dummy_pos_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_momentum_sidis;
            g_HMScalDE_Dummy_pos_RunGroup_sidis->SetPoint(i_HMScalDE_Dummy_pos_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunGroup_sidis->SetPointError(i_HMScalDE_Dummy_pos_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunGroup_sidis;
            g_HMScalDE_Dummy_pos_RunNumber_sidis->SetPoint(i_HMScalDE_Dummy_pos_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_pos_RunNumber_sidis->SetPointError(i_HMScalDE_Dummy_pos_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_pos_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_Dummy_neg_momentum_spring->SetPoint(i_HMScalDE_Dummy_neg_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_momentum_spring->SetPointError(i_HMScalDE_Dummy_neg_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_momentum_spring;
            g_HMScalDE_Dummy_neg_RunGroup_spring->SetPoint(i_HMScalDE_Dummy_neg_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunGroup_spring->SetPointError(i_HMScalDE_Dummy_neg_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunGroup_spring;
            g_HMScalDE_Dummy_neg_RunNumber_spring->SetPoint(i_HMScalDE_Dummy_neg_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunNumber_spring->SetPointError(i_HMScalDE_Dummy_neg_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_Dummy_neg_momentum_fall->SetPoint(i_HMScalDE_Dummy_neg_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_momentum_fall->SetPointError(i_HMScalDE_Dummy_neg_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_momentum_fall;
            g_HMScalDE_Dummy_neg_RunGroup_fall->SetPoint(i_HMScalDE_Dummy_neg_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunGroup_fall->SetPointError(i_HMScalDE_Dummy_neg_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunGroup_fall;
            g_HMScalDE_Dummy_neg_RunNumber_fall->SetPoint(i_HMScalDE_Dummy_neg_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunNumber_fall->SetPointError(i_HMScalDE_Dummy_neg_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunNumber_fall;
          }
          else {
            g_HMScalDE_Dummy_neg_momentum_sidis->SetPoint(i_HMScalDE_Dummy_neg_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_momentum_sidis->SetPointError(i_HMScalDE_Dummy_neg_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_momentum_sidis;
            g_HMScalDE_Dummy_neg_RunGroup_sidis->SetPoint(i_HMScalDE_Dummy_neg_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunGroup_sidis->SetPointError(i_HMScalDE_Dummy_neg_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunGroup_sidis;
            g_HMScalDE_Dummy_neg_RunNumber_sidis->SetPoint(i_HMScalDE_Dummy_neg_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_Dummy_neg_RunNumber_sidis->SetPointError(i_HMScalDE_Dummy_neg_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_Dummy_neg_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each neg runs data

    }//if normal production runs
    std::vector<int> neg_H2,pos_H2;
    neg_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["H2"].get<std::vector<int>>();
    pos_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["H2"].get<std::vector<int>>();

    if(!neg_H2.empty() && !pos_H2.empty()){
      //loop over each pos runs data
      for(auto it = pos_H2.begin();it!=pos_H2.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_H2_pos_momentum_spring->SetPoint(i_HMScalDE_H2_pos_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_pos_momentum_spring->SetPointError(i_HMScalDE_H2_pos_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_momentum_spring;
            g_HMScalDE_H2_pos_RunGroup_spring->SetPoint(i_HMScalDE_H2_pos_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunGroup_spring->SetPointError(i_HMScalDE_H2_pos_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunGroup_spring;
            g_HMScalDE_H2_pos_RunNumber_spring->SetPoint(i_HMScalDE_H2_pos_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunNumber_spring->SetPointError(i_HMScalDE_H2_pos_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_H2_pos_momentum_fall->SetPoint(i_HMScalDE_H2_pos_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_pos_momentum_fall->SetPointError(i_HMScalDE_H2_pos_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_momentum_fall;
            g_HMScalDE_H2_pos_RunGroup_fall->SetPoint(i_HMScalDE_H2_pos_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunGroup_fall->SetPointError(i_HMScalDE_H2_pos_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunGroup_fall;
            g_HMScalDE_H2_pos_RunNumber_fall->SetPoint(i_HMScalDE_H2_pos_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunNumber_fall->SetPointError(i_HMScalDE_H2_pos_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunNumber_fall;
          }
          else {
            g_HMScalDE_H2_pos_momentum_sidis->SetPoint(i_HMScalDE_H2_pos_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_pos_momentum_sidis->SetPointError(i_HMScalDE_H2_pos_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_momentum_sidis;
            g_HMScalDE_H2_pos_RunGroup_sidis->SetPoint(i_HMScalDE_H2_pos_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunGroup_sidis->SetPointError(i_HMScalDE_H2_pos_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunGroup_sidis;
            g_HMScalDE_H2_pos_RunNumber_sidis->SetPoint(i_HMScalDE_H2_pos_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_pos_RunNumber_sidis->SetPointError(i_HMScalDE_H2_pos_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_pos_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
        int RunNumber = *it;

        //double HMS_cal_eff = 0.94;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        //double HMS_aero_eff = 0.9937;//Get_HMS_aero_Eff(RunNumber,HMS_aero_cut);            
        //double HMS_cal_eff = 0.9928;//Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber);            
        double HMS_cal_eff_err = Get_HMS_Cal_Eff_err(RunNumber);            
        if(HMS_cal_eff>0 && HMS_cal_eff<1){
          if( RunNumber > 7000){
            g_HMScalDE_H2_neg_momentum_spring->SetPoint(i_HMScalDE_H2_neg_momentum_spring,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_neg_momentum_spring->SetPointError(i_HMScalDE_H2_neg_momentum_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_momentum_spring;
            g_HMScalDE_H2_neg_RunGroup_spring->SetPoint(i_HMScalDE_H2_neg_RunGroup_spring,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunGroup_spring->SetPointError(i_HMScalDE_H2_neg_RunGroup_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunGroup_spring;
            g_HMScalDE_H2_neg_RunNumber_spring->SetPoint(i_HMScalDE_H2_neg_RunNumber_spring,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunNumber_spring->SetPointError(i_HMScalDE_H2_neg_RunNumber_spring,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunNumber_spring;
          }
          else if(RunNumber > 6000){
            g_HMScalDE_H2_neg_momentum_fall->SetPoint(i_HMScalDE_H2_neg_momentum_fall,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_neg_momentum_fall->SetPointError(i_HMScalDE_H2_neg_momentum_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_momentum_fall;
            g_HMScalDE_H2_neg_RunGroup_fall->SetPoint(i_HMScalDE_H2_neg_RunGroup_fall,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunGroup_fall->SetPointError(i_HMScalDE_H2_neg_RunGroup_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunGroup_fall;
            g_HMScalDE_H2_neg_RunNumber_fall->SetPoint(i_HMScalDE_H2_neg_RunNumber_fall,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunNumber_fall->SetPointError(i_HMScalDE_H2_neg_RunNumber_fall,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunNumber_fall;
          }
          else {
            g_HMScalDE_H2_neg_momentum_sidis->SetPoint(i_HMScalDE_H2_neg_momentum_sidis,hms_p,HMS_cal_eff);
            g_HMScalDE_H2_neg_momentum_sidis->SetPointError(i_HMScalDE_H2_neg_momentum_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_momentum_sidis;
            g_HMScalDE_H2_neg_RunGroup_sidis->SetPoint(i_HMScalDE_H2_neg_RunGroup_sidis,RunGroup,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunGroup_sidis->SetPointError(i_HMScalDE_H2_neg_RunGroup_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunGroup_sidis;
            g_HMScalDE_H2_neg_RunNumber_sidis->SetPoint(i_HMScalDE_H2_neg_RunNumber_sidis,RunNumber,HMS_cal_eff);
            g_HMScalDE_H2_neg_RunNumber_sidis->SetPointError(i_HMScalDE_H2_neg_RunNumber_sidis,0,HMS_cal_eff_err);
            ++ i_HMScalDE_H2_neg_RunNumber_sidis;
          }
        }//if eff make sense
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
        }
      }//loop over each neg runs data

    }//if normal production runs
  }//loop over each rungroup
  
  g_HMScalDE_D2_pos_momentum_fall->SetMarkerStyle(8);
  g_HMScalDE_D2_pos_momentum_fall->SetTitle("Fall");
  g_HMScalDE_D2_pos_momentum_spring->SetMarkerStyle(4);
  g_HMScalDE_D2_pos_momentum_spring->SetTitle("Spring");
  g_HMScalDE_D2_pos_momentum_sidis->SetMarkerStyle(5);
  g_HMScalDE_D2_pos_momentum_sidis->SetTitle("ptsidis");
  mg_HMScalDE_D2_momentum->Add(g_HMScalDE_D2_pos_momentum_fall,"P");
  mg_HMScalDE_D2_momentum->Add(g_HMScalDE_D2_pos_momentum_spring,"P");
  mg_HMScalDE_D2_momentum->Add(g_HMScalDE_D2_pos_momentum_sidis,"P");
  TCanvas *c_HMScalDE_D2_momentum = new TCanvas();
  mg_HMScalDE_D2_momentum->Draw("A");
  mg_HMScalDE_D2_momentum->GetXaxis()->SetTitle("HMS momentum [GeV]");
  mg_HMScalDE_D2_momentum->GetYaxis()->SetTitle("HMS cal eff");
  mg_HMScalDE_D2_momentum->Fit("pol0","F");
  TF1 *Fit_HMScalDE_D2_momentum = mg_HMScalDE_D2_momentum->GetFunction("pol0");
  Fit_HMScalDE_D2_momentum->SetLineColor(kRed);
  Fit_HMScalDE_D2_momentum->Draw("L same");
  gStyle->SetOptFit(0001);
  c_HMScalDE_D2_momentum->BuildLegend(0.75,0.1,0.95,0.3);
  std::string c_HMScalDE_D2_momentum_name = "results/pid/DE/HMScalDE_D2_momentum.pdf";
  c_HMScalDE_D2_momentum->SaveAs(c_HMScalDE_D2_momentum_name.c_str());
  g_HMScalDE_D2_pos_RunGroup_fall->SetMarkerStyle(8);
  g_HMScalDE_D2_pos_RunGroup_fall->SetTitle("Fall");
  g_HMScalDE_D2_pos_RunGroup_spring->SetMarkerStyle(4);
  g_HMScalDE_D2_pos_RunGroup_spring->SetTitle("Spring");
  g_HMScalDE_D2_pos_RunGroup_sidis->SetMarkerStyle(5);
  g_HMScalDE_D2_pos_RunGroup_sidis->SetTitle("ptsidis");
  mg_HMScalDE_D2_RunGroup->Add(g_HMScalDE_D2_pos_RunGroup_fall,"P");
  mg_HMScalDE_D2_RunGroup->Add(g_HMScalDE_D2_pos_RunGroup_spring,"P");
  mg_HMScalDE_D2_RunGroup->Add(g_HMScalDE_D2_pos_RunGroup_sidis,"P");
  TCanvas *c_HMScalDE_D2_RunGroup = new TCanvas();
  mg_HMScalDE_D2_RunGroup->Draw("A");
  mg_HMScalDE_D2_RunGroup->GetXaxis()->SetTitle("HMS RunGroup");
  mg_HMScalDE_D2_RunGroup->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_D2_RunGroup->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_D2_RunGroup_name = "results/pid/DE/HMScalDE_D2_RunGroup.pdf";
  c_HMScalDE_D2_RunGroup->SaveAs(c_HMScalDE_D2_RunGroup_name.c_str());
  g_HMScalDE_D2_pos_RunNumber_fall->SetMarkerStyle(8);
  g_HMScalDE_D2_pos_RunNumber_fall->SetTitle("Fall");
  g_HMScalDE_D2_pos_RunNumber_spring->SetMarkerStyle(4);
  g_HMScalDE_D2_pos_RunNumber_spring->SetTitle("Spring");
  g_HMScalDE_D2_pos_RunNumber_sidis->SetMarkerStyle(5);
  g_HMScalDE_D2_pos_RunNumber_sidis->SetTitle("ptsidis");
  mg_HMScalDE_D2_RunNumber->Add(g_HMScalDE_D2_pos_RunNumber_fall,"P");
  mg_HMScalDE_D2_RunNumber->Add(g_HMScalDE_D2_pos_RunNumber_spring,"P");
  mg_HMScalDE_D2_RunNumber->Add(g_HMScalDE_D2_pos_RunNumber_sidis,"P");
  TCanvas *c_HMScalDE_D2_RunNumber = new TCanvas();
  mg_HMScalDE_D2_RunNumber->Draw("A");
  mg_HMScalDE_D2_RunNumber->GetXaxis()->SetTitle("HMS RunNumber");
  mg_HMScalDE_D2_RunNumber->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_D2_RunNumber->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_D2_RunNumber_name = "results/pid/DE/HMScalDE_D2_RunNumber.pdf";
  c_HMScalDE_D2_RunNumber->SaveAs(c_HMScalDE_D2_RunNumber_name.c_str());

  g_HMScalDE_H2_pos_momentum_fall->SetMarkerStyle(8);
  g_HMScalDE_H2_pos_momentum_fall->SetTitle("Fall");
  g_HMScalDE_H2_pos_momentum_spring->SetMarkerStyle(4);
  g_HMScalDE_H2_pos_momentum_spring->SetTitle("Spring");
  g_HMScalDE_H2_pos_momentum_sidis->SetMarkerStyle(5);
  g_HMScalDE_H2_pos_momentum_sidis->SetTitle("ptsidis");
  mg_HMScalDE_H2_momentum->Add(g_HMScalDE_H2_pos_momentum_fall,"P");
  mg_HMScalDE_H2_momentum->Add(g_HMScalDE_H2_pos_momentum_spring,"P");
  mg_HMScalDE_H2_momentum->Add(g_HMScalDE_H2_pos_momentum_sidis,"P");
  TCanvas *c_HMScalDE_H2_momentum = new TCanvas();
  mg_HMScalDE_H2_momentum->Draw("A");
  mg_HMScalDE_H2_momentum->Fit("pol0","F");
  TF1 *Fit_HMScalDE_H2_momentum = mg_HMScalDE_H2_momentum->GetFunction("pol0");
  Fit_HMScalDE_H2_momentum->SetLineColor(kRed);
  Fit_HMScalDE_H2_momentum->Draw("L same");
  gStyle->SetOptFit(0001);
  mg_HMScalDE_H2_momentum->GetXaxis()->SetTitle("HMS momentum [GeV]");
  mg_HMScalDE_H2_momentum->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_H2_momentum->BuildLegend(0.75,0.1,0.95,0.3);
  std::string c_HMScalDE_H2_momentum_name = "results/pid/DE/HMScalDE_H2_momentum.pdf";
  c_HMScalDE_H2_momentum->SaveAs(c_HMScalDE_H2_momentum_name.c_str());
  g_HMScalDE_H2_pos_RunGroup_fall->SetMarkerStyle(8);
  g_HMScalDE_H2_pos_RunGroup_fall->SetTitle("Fall");
  g_HMScalDE_H2_pos_RunGroup_spring->SetMarkerStyle(4);
  g_HMScalDE_H2_pos_RunGroup_spring->SetTitle("Spring");
  g_HMScalDE_H2_pos_RunGroup_sidis->SetMarkerStyle(5);
  g_HMScalDE_H2_pos_RunGroup_sidis->SetTitle("ptsidis");
  mg_HMScalDE_H2_RunGroup->Add(g_HMScalDE_H2_pos_RunGroup_fall,"P");
  mg_HMScalDE_H2_RunGroup->Add(g_HMScalDE_H2_pos_RunGroup_spring,"P");
  mg_HMScalDE_H2_RunGroup->Add(g_HMScalDE_H2_pos_RunGroup_sidis,"P");
  TCanvas *c_HMScalDE_H2_RunGroup = new TCanvas();
  mg_HMScalDE_H2_RunGroup->Draw("A");
  mg_HMScalDE_H2_RunGroup->GetXaxis()->SetTitle("HMS RunGroup");
  mg_HMScalDE_H2_RunGroup->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_H2_RunGroup->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_H2_RunGroup_name = "results/pid/DE/HMScalDE_H2_RunGroup.pdf";
  c_HMScalDE_H2_RunGroup->SaveAs(c_HMScalDE_H2_RunGroup_name.c_str());
  g_HMScalDE_H2_pos_RunNumber_fall->SetMarkerStyle(8);
  g_HMScalDE_H2_pos_RunNumber_fall->SetTitle("Fall");
  g_HMScalDE_H2_pos_RunNumber_spring->SetMarkerStyle(4);
  g_HMScalDE_H2_pos_RunNumber_spring->SetTitle("Spring");
  g_HMScalDE_H2_pos_RunNumber_sidis->SetMarkerStyle(5);
  g_HMScalDE_H2_pos_RunNumber_sidis->SetTitle("ptsidis");
  mg_HMScalDE_H2_RunNumber->Add(g_HMScalDE_H2_pos_RunNumber_fall,"P");
  mg_HMScalDE_H2_RunNumber->Add(g_HMScalDE_H2_pos_RunNumber_spring,"P");
  mg_HMScalDE_H2_RunNumber->Add(g_HMScalDE_H2_pos_RunNumber_sidis,"P");
  TCanvas *c_HMScalDE_H2_RunNumber = new TCanvas();
  mg_HMScalDE_H2_RunNumber->Draw("A");
  mg_HMScalDE_H2_RunNumber->GetXaxis()->SetTitle("HMS RunNumber");
  mg_HMScalDE_H2_RunNumber->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_H2_RunNumber->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_H2_RunNumber_name = "results/pid/DE/HMScalDE_H2_RunNumber.pdf";
  c_HMScalDE_H2_RunNumber->SaveAs(c_HMScalDE_H2_RunNumber_name.c_str());

  g_HMScalDE_Dummy_pos_momentum_fall->SetMarkerStyle(8);
  g_HMScalDE_Dummy_pos_momentum_fall->SetTitle("Fall");
  g_HMScalDE_Dummy_pos_momentum_spring->SetMarkerStyle(4);
  g_HMScalDE_Dummy_pos_momentum_spring->SetTitle("Spring");
  g_HMScalDE_Dummy_pos_momentum_sidis->SetMarkerStyle(5);
  g_HMScalDE_Dummy_pos_momentum_sidis->SetTitle("ptsidis");
  mg_HMScalDE_Dummy_momentum->Add(g_HMScalDE_Dummy_pos_momentum_fall,"P");
  mg_HMScalDE_Dummy_momentum->Add(g_HMScalDE_Dummy_pos_momentum_spring,"P");
  mg_HMScalDE_Dummy_momentum->Add(g_HMScalDE_Dummy_pos_momentum_sidis,"P");
  TCanvas *c_HMScalDE_Dummy_momentum = new TCanvas();
  mg_HMScalDE_Dummy_momentum->Draw("A");
  mg_HMScalDE_Dummy_momentum->Fit("pol0","F");
  TF1 *Fit_HMScalDE_Dummy_momentum = mg_HMScalDE_Dummy_momentum->GetFunction("pol0");
  Fit_HMScalDE_Dummy_momentum->SetLineColor(kRed);
  Fit_HMScalDE_Dummy_momentum->Draw("L same");
  gStyle->SetOptFit(0001);
  mg_HMScalDE_Dummy_momentum->GetXaxis()->SetTitle("HMS momentum [GeV]");
  mg_HMScalDE_Dummy_momentum->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_Dummy_momentum->BuildLegend(0.75,0.1,0.95,0.3);
  std::string c_HMScalDE_Dummy_momentum_name = "results/pid/DE/HMScalDE_Dummy_momentum.pdf";
  c_HMScalDE_Dummy_momentum->SaveAs(c_HMScalDE_Dummy_momentum_name.c_str());
  g_HMScalDE_Dummy_pos_RunGroup_fall->SetMarkerStyle(8);
  g_HMScalDE_Dummy_pos_RunGroup_fall->SetTitle("Fall");
  g_HMScalDE_Dummy_pos_RunGroup_spring->SetMarkerStyle(4);
  g_HMScalDE_Dummy_pos_RunGroup_spring->SetTitle("Spring");
  g_HMScalDE_Dummy_pos_RunGroup_sidis->SetMarkerStyle(5);
  g_HMScalDE_Dummy_pos_RunGroup_sidis->SetTitle("ptsidis");
  mg_HMScalDE_Dummy_RunGroup->Add(g_HMScalDE_Dummy_pos_RunGroup_fall,"P");
  mg_HMScalDE_Dummy_RunGroup->Add(g_HMScalDE_Dummy_pos_RunGroup_spring,"P");
  mg_HMScalDE_Dummy_RunGroup->Add(g_HMScalDE_Dummy_pos_RunGroup_sidis,"P");
  TCanvas *c_HMScalDE_Dummy_RunGroup = new TCanvas();
  mg_HMScalDE_Dummy_RunGroup->Draw("A");
  mg_HMScalDE_Dummy_RunGroup->GetXaxis()->SetTitle("HMS RunGroup");
  mg_HMScalDE_Dummy_RunGroup->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_Dummy_RunGroup->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_Dummy_RunGroup_name = "results/pid/DE/HMScalDE_Dummy_RunGroup.pdf";
  c_HMScalDE_Dummy_RunGroup->SaveAs(c_HMScalDE_Dummy_RunGroup_name.c_str());
  g_HMScalDE_Dummy_pos_RunNumber_fall->SetMarkerStyle(8);
  g_HMScalDE_Dummy_pos_RunNumber_fall->SetTitle("Fall");
  g_HMScalDE_Dummy_pos_RunNumber_spring->SetMarkerStyle(4);
  g_HMScalDE_Dummy_pos_RunNumber_spring->SetTitle("Spring");
  g_HMScalDE_Dummy_pos_RunNumber_sidis->SetMarkerStyle(5);
  g_HMScalDE_Dummy_pos_RunNumber_sidis->SetTitle("ptsidis");
  mg_HMScalDE_Dummy_RunNumber->Add(g_HMScalDE_Dummy_pos_RunNumber_fall,"P");
  mg_HMScalDE_Dummy_RunNumber->Add(g_HMScalDE_Dummy_pos_RunNumber_spring,"P");
  mg_HMScalDE_Dummy_RunNumber->Add(g_HMScalDE_Dummy_pos_RunNumber_sidis,"P");
  TCanvas *c_HMScalDE_Dummy_RunNumber = new TCanvas();
  mg_HMScalDE_Dummy_RunNumber->Draw("A");
  mg_HMScalDE_Dummy_RunNumber->GetXaxis()->SetTitle("HMS RunNumber");
  mg_HMScalDE_Dummy_RunNumber->GetYaxis()->SetTitle("HMS cal eff");
  c_HMScalDE_Dummy_RunNumber->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_HMScalDE_Dummy_RunNumber_name = "results/pid/DE/HMScalDE_Dummy_RunNumber.pdf";
  c_HMScalDE_Dummy_RunNumber->SaveAs(c_HMScalDE_Dummy_RunNumber_name.c_str());

  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_runsinfo.dump(4)<<std::endl;
}
