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

double Get_SHMS_Cal_Eff(int RunNumber,double cal_cut){
  json j_SHMS;
  {
    std::string SHMS_DE_name = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_SHMS(SHMS_DE_name.c_str());
    ifs_SHMS>>j_SHMS;
  }
  double pi_all = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
  std::vector<double> calcuts,pi_calcuts;
  calcuts = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["calcuts"].get<std::vector<double>>();
  pi_calcuts = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_calcuts"].get<std::vector<double>>();
  auto it = find(calcuts.begin(),calcuts.end(),cal_cut);
  if(it!=calcuts.end()){
    int index = it - calcuts.begin();
    double eff = pi_calcuts[index]/pi_all;
    return eff;
  }
  else {return -1;}

}
double Get_SHMS_aero_Eff(int RunNumber,double aero_cut){
  json j_SHMS;
  {
    std::string SHMS_DE_name = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_SHMS(SHMS_DE_name.c_str());
    ifs_SHMS>>j_SHMS;
  }
  double pi_all = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
  std::vector<double> aerocuts,pi_aerocuts;
  aerocuts = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["aero_cuts"].get<std::vector<double>>();
  pi_aerocuts = j_SHMS[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_aerocuts"].get<std::vector<double>>();
  auto it = find(aerocuts.begin(),aerocuts.end(),aero_cut);
  if(it!=aerocuts.end()){
    int index = it - aerocuts.begin();
    double eff = pi_aerocuts[index]/pi_all;
    return eff;
  }
  else {return -1;}

}
double Get_HMS_Cal_Eff(int RunNumber,double cal_cut){
  json j_HMS;
  {
    std::string HMS_DE_name = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_HMS(HMS_DE_name.c_str());
    ifs_HMS>>j_HMS;
  }
  //for electron eff
  double e_all = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
  std::vector<double> calcuts,e_calcuts;
  calcuts = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["calcuts"].get<std::vector<double>>();
  e_calcuts = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
  //for pi rejection ratio need to finish this
  double pi_all = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["pi_all"].get<double>();
  std::vector<double> pi_calcuts;
  pi_calcuts = j_HMS[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
  auto it = find(calcuts.begin(),calcuts.end(),cal_cut);
  if(it!=calcuts.end()){
    int index = it - calcuts.begin();
    double e_eff = e_calcuts[index]/e_all;
    //double pi_rej = 1-pi_calcuts[index]/pi_all;
    double eff = e_eff;
    return eff;
  }
  else {return -1;}

}
double Get_HMS_cer_Eff(int RunNumber,double cer_cut){
  json j_HMS;
  {
    std::string HMS_DE_name = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
    std::ifstream ifs_HMS(HMS_DE_name.c_str());
    ifs_HMS>>j_HMS;
  }
  double e_all = j_HMS[(std::to_string(RunNumber)).c_str()]["calcut"]["e_all"].get<double>();
  std::vector<double> cercuts,e_cercuts;
  cercuts = j_HMS[(std::to_string(RunNumber)).c_str()]["calcut"]["cercuts"].get<std::vector<double>>();
  e_cercuts = j_HMS[(std::to_string(RunNumber)).c_str()]["calcut"]["e_cercuts"].get<std::vector<double>>();
  auto it = find(cercuts.begin(),cercuts.end(),cer_cut);
  if(it!=cercuts.end()){
    int index = it - cercuts.begin();
    double e_eff = e_cercuts[index]/e_all;
    double eff = e_eff;
    return eff;
  }
  else {return -1;}

}

void write_DE_to_json(){

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  };
  json j_runsinfo;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_runsinfo;
  }

  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    int RunGroup = std::stoi(it.key());
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();

    if(!neg_D2.empty() && !pos_D2.empty()){

      //loop over each pos runs data
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            


        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            

        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2"<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
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
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            


        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy"<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
        int RunNumber = *it;
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            

        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
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
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            


        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2"<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
        int RunNumber = *it;
        double SHMS_cal_cut = j_cuts["P_cal_pi_high"].get<double>();
        double SHMS_aero_cut = j_cuts["P_aero"].get<double>();
        double HMS_cal_cut = j_cuts["H_cal_low"].get<double>();
        double HMS_cer_cut = j_cuts["H_cer"].get<double>();

        double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            
        double HMS_cer_eff = Get_HMS_cer_Eff(RunNumber,HMS_cer_cut);            

        if(SHMS_cal_eff>0 && SHMS_cal_eff<1 && SHMS_aero_eff>0 && SHMS_aero_eff<1)
        {
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = SHMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = SHMS_aero_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" SHMScal "<<SHMS_cal_eff<<" SHMSaero "<<SHMS_aero_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"] = 1;
        }
        if(HMS_cal_eff>0 && HMS_cal_eff<1 && HMS_cer_eff>0 && HMS_cer_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = HMS_cer_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<" HMScer "<<HMS_cer_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"] = 1;
        }
      }//loop over each neg runs data
    }//if normal production runs
  }//loop over each rungroup
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_runsinfo.dump(4)<<std::endl;
}
