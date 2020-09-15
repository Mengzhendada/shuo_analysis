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


void statistic_runs_D2_scaler(int RunGroup=0){

  if(RunGroup ==0){
    std::cout<<"Enter a RunGroup (-1 to exit):";
    std::cin>>RunGroup;
    if(RunGroup<=0)
      return;
  }

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }

  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();

  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  };

  json jout;
  {
    std::string if_name = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
    std::ifstream ifs(if_name.c_str());
    ifs>>jout;
  }

  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};
    //for neg runs
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg scaler "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto neg_scaler_charge_list = d_neg_scaler.Take<double>("P.BCM4B.scalerChargeCut");
      auto neg_scaler_time_list = d_neg_scaler.Take<double>("P.1MHz.scalerTimeCut");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
      auto h_current_neg = d_neg_scaler.Histo1D({"","",100,3,100},"P.BCM4B.scalerCurrent");
      double set_cur_neg = h_current_neg->GetBinCenter(h_current_neg->GetMaximumBin());
      double tot_charge_neg,pre_charge_neg = 0,tot_charge_cut_neg = 0;
      double tot_time_neg,pre_time_neg = 0, tot_time_cut_neg = 0;
      int scaler_entries_neg = neg_scaler_event_list->size();
      for(int i = 0; i<scaler_entries_neg;i++){
          tot_charge_neg = neg_scaler_charge_list->at(i);
          tot_time_neg = neg_scaler_time_list->at(i);
        if(abs(neg_scaler_current_list->at(i)-set_cur_neg)<3){
          tot_charge_cut_neg = tot_charge_cut_neg+(tot_charge_neg-pre_charge_neg);
          tot_time_cut_neg = tot_time_cut_neg +(tot_time_neg - pre_time_neg);
          
        }
        pre_time_neg = tot_time_neg; 
        pre_charge_neg = tot_charge_neg;
      }
      std::cout<<"charge after current cut"<<tot_charge_cut_neg<<std::endl;
      std::cout<<"charge in scaler "<<*d_neg_scaler.Max("P.BCM4B.scalerChargeCut")<<std::endl;;
      jout[(std::to_string(RunNumber)).c_str()]["charge"] = tot_charge_cut_neg/1000;
      jout[(std::to_string(RunNumber)).c_str()]["time"] = tot_time_cut_neg;

    }

    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos scaler "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
      ROOT::RDataFrame d_pos_scaler("TSP",rootfile_name);
      auto scaler_current_list = d_pos_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto scaler_charge_list = d_pos_scaler.Take<double>("P.BCM4B.scalerChargeCut");
      auto scaler_time_list = d_pos_scaler.Take<double>("P.1MHz.scalerTimeCut");
      auto scaler_event_list = d_pos_scaler.Take<double>("evNumber");
      auto h_current = d_pos_scaler.Histo1D({"","",100,3,100},"P.BCM4B.scalerCurrent");
      double set_cur = h_current->GetBinCenter(h_current->GetMaximumBin());
      double tot_charge,pre_charge = 0,tot_charge_cut = 0;
      double tot_time,pre_time = 0, tot_time_cut = 0;
      int scaler_entries = scaler_event_list->size();
      for(int i = 0; i<scaler_entries;i++){
          tot_charge = scaler_charge_list->at(i);
          tot_time = scaler_time_list->at(i);
        if(abs(scaler_current_list->at(i)-set_cur)<3){
          tot_charge_cut = tot_charge_cut+(tot_charge-pre_charge);
          tot_time_cut = tot_time_cut +(tot_time - pre_time);
          
        }
        pre_time = tot_time; 
        pre_charge = tot_charge;
      }
      std::cout<<"charge after current cut"<<tot_charge_cut<<std::endl;
      std::cout<<"charge in scaler "<<*d_pos_scaler.Max("P.BCM4B.scalerChargeCut")<<std::endl;;
      //double charge = *d_pos_scale.Max("P.BCM4B.scalerChargeCut")/1000.0;
      //double time_1MHz_cut = *d_pos_scale.Max("P.1MHz.scalerTimeCut");
      //int scaler_events  = *d_pos_scale.Count();
      //long int datacounts = *d_pos_scale.Max("P.pTRIG1.scaler");
      ////std::cout<<charge<<std::endl;
      jout[(std::to_string(RunNumber)).c_str()]["charge"] = tot_charge_cut/1000;
      jout[(std::to_string(RunNumber)).c_str()]["time"] = tot_time_cut;
      //jout[(std::to_string(RunNumber)).c_str()]["scaler_n"] = scaler_events;
      //jout[(std::to_string(RunNumber)).c_str()]["data_n"] = datacounts;
    }
    
    std::string of = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
    std::ofstream outfile(of.c_str());
    outfile<<jout.dump(4)<<std::endl;
   
  }
}
