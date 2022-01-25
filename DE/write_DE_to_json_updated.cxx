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


void write_DE_to_json_updated(){

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
  
  double SHMS_cal_eff_D2 = 0.9453;
  double SHMS_cal_eff_Dummy = 0.9536;
  double SHMS_cal_eff_H2 = 0.9471;
  double HMS_cal_eff_D2 = 0.9928;
  double HMS_cal_eff_Dummy = 0.995;
  double HMS_cal_eff_H2 = 0.9937;
  double SHMS_aero_eff_D2 = 0.9686;
  double SHMS_aero_eff_H2 = 0.9667;
  double SHMS_aero_eff_Dummy = 0.9742;

  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    int RunGroup = std::stoi(it.key());
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();

    if(!neg_D2.empty() && !pos_D2.empty()){

      //loop over each pos runs data
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;

        double SHMS_cal_eff = SHMS_cal_eff_D2;
        double SHMS_aero_eff = SHMS_aero_eff_D2;
        double HMS_cal_eff = HMS_cal_eff_D2;


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
        if(HMS_cal_eff>0 && HMS_cal_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;

        double SHMS_cal_eff = SHMS_cal_eff_D2;
        double SHMS_aero_eff = SHMS_aero_eff_D2;
        double HMS_cal_eff = HMS_cal_eff_D2;
        //double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        //double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        //double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            

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
        if(HMS_cal_eff>0 && HMS_cal_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", D2"<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
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
        double SHMS_cal_eff = SHMS_cal_eff_Dummy;
        double SHMS_aero_eff = SHMS_aero_eff_Dummy;
        double HMS_cal_eff = HMS_cal_eff_Dummy;
        //double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        //double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        //double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            


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
        if(HMS_cal_eff>0 && HMS_cal_eff<1  ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
        int RunNumber = *it;

        double SHMS_cal_eff = SHMS_cal_eff_Dummy;
        double SHMS_aero_eff = SHMS_aero_eff_Dummy;
        double HMS_cal_eff = HMS_cal_eff_Dummy;
        //double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        //double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        //double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            

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
        if(HMS_cal_eff>0 && HMS_cal_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", Dummy "<<RunNumber<<" HMScal "<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
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

        double SHMS_cal_eff = SHMS_cal_eff_H2;
        double SHMS_aero_eff = SHMS_aero_eff_H2;
        double HMS_cal_eff = HMS_cal_eff_H2;

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
        if(HMS_cal_eff>0 && HMS_cal_eff<1 ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
        }
      }//loop over each pos runs data
      //loop over each neg runs data
      for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
        int RunNumber = *it;

        double SHMS_cal_eff = SHMS_cal_eff_H2;
        double SHMS_aero_eff = SHMS_aero_eff_H2;
        double HMS_cal_eff = HMS_cal_eff_H2;
        //double SHMS_cal_eff = Get_SHMS_Cal_Eff(RunNumber,SHMS_cal_cut);            
        //double SHMS_aero_eff = Get_SHMS_aero_Eff(RunNumber,SHMS_aero_cut);            
        //double HMS_cal_eff = Get_HMS_Cal_Eff(RunNumber,HMS_cal_cut);            

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
        if(HMS_cal_eff>0 && HMS_cal_eff<1  ){
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = HMS_cal_eff;
        }
        else
        {
          std::cout<<"RunGroup "<<RunGroup<<", H2 "<<RunNumber<<" HMScal "<<HMS_cal_eff<<std::endl;
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"] = 1;
        }
      }//loop over each neg runs data
    }//if normal production runs
  }//loop over each rungroup
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_runsinfo.dump(4)<<std::endl;
}
