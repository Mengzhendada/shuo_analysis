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
#include "TLine.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

void Total_live_time(int RunGroup = 0){
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
  }
  
  double current_offset = j_cuts["current_diff"].get<double>();

  json j_out;
 // {
 //   std::ifstream ifs("results/yield/TLT.json");
 //   ifs>>j_out;
 // }

  if(!neg_D2.empty() && !pos_D2.empty()){

    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);
      ROOT::RDataFrame d_pos_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto pos_scaler_current_list = d_pos_scaler.Take<double>("P.BCM1.scalerCurrent");
      auto pos_scaler_event_list = d_pos_scaler.Take<double>("P.EDTM.scaler");
      auto h_pos_current = d_pos_scaler.Histo1D({"pos current","pos current",100,3,100},"P.BCM1.scalerCurrent");
      double pos_setcurrent = h_pos_current->GetBinCenter(h_pos_current->GetMaximumBin());
      std::cout<<"set current "<<pos_setcurrent<<std::endl;
      //std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size "<<pos_scaler_current_list->size()<<std::endl;
      //all with current cut
      int scaler_entries = *d_pos_scaler.Count();
      double tot_counts=0,tot_counts_currentcut = 0,pre_counts = 0;
      for(int i = 0;i<scaler_entries;i++){
        tot_counts = pos_scaler_event_list->at(i);
        if(pos_scaler_current_list->at(i)>current_offset){
          tot_counts_currentcut = tot_counts_currentcut+(tot_counts-pre_counts);
        }
        pre_counts = tot_counts;
      }
      
      auto pos_scaler_event_list_2 = d_pos_scaler.Take<double>("evNumber");
      auto pos_get_current = [&](unsigned int eventNum){
        int i = 0;
        while(eventNum>pos_scaler_event_list_2->at(i)){
          ++i;
          if(i>pos_scaler_event_list_2->size()-1)
          {
            i=pos_scaler_event_list_2->size()-1;
            break;
          }
        }
        return pos_scaler_current_list->at(i);
      };
      auto h_EDTM_all = d_pos_scaler.Histo1D({"","",100,-1000,10000},"P.EDTM.scaler");
      auto d_pos_run = d_pos_raw
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      //accepted
      auto h_EDTM_acc = d_pos_run
        .Filter([&](double edtmtdctime){return edtmtdctime != 0;},{"T.coin.pEDTM_tdcTime"})
        .Histo1D({"","",100,-10,250},"T.coin.pEDTM_tdcTime")
        ;
     // auto h_EDTM_scl = d_pos_scaler
     //   .Filter([&](double current){return current>current_offset;},{"P.BCM1.scalerCurrent"})
     //   .Histo1D({"","",500,-1000,10000},"P.EDTM.scaler")
     //   ;


      double edtm_acc = h_EDTM_acc->GetEntries();
      double edtm_all = tot_counts_currentcut; 
      double nominal_LT = edtm_acc/edtm_all;
      j_out[(std::to_string(RunNumber)).c_str()]["TLT"] = nominal_LT;
      std::cout<<nominal_LT<<" = "<<edtm_acc<<"/"<<edtm_all<<std::endl;
      //double LT_err = nominal_LT/sqrt(edtm_acc);
      //double LT_err = sqrt(edtm_acc)/edtm_all;
      double LT_err = (1/edtm_all)*sqrt(edtm_acc*(1-nominal_LT));
      std::cout<<LT_err<<std::endl;
      j_out[(std::to_string(RunNumber)).c_str()]["TLT_error"] = LT_err;
      
      //TCanvas *c_tdc = new TCanvas();
      //h_EDTM_acc->DrawCopy("hist");
      //c_tdc->SaveAs("results/LT/TLT_tdctime.pdf");
      //TCanvas *c_all = new TCanvas();
      //h_EDTM_all->DrawCopy("hist");
      //c_all->SaveAs("results/LT/TLT_all.pdf");
      //
      //TCanvas *c_check = new TCanvas();
      ////auto h_evtype = d_pos_run.Histo1D({"","",8,0,8},"fEvtHdr.fEvtType");
      //auto h_current = d_pos_run.Histo1D({"","",100,0,100},"current");
      ////h_evtype->DrawCopy("hist");
      //h_current->DrawCopy("hist");
      //c_check->SaveAs("results/LT/TLT_type.pdf");
    }//loop over each pos runs
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM1.scalerCurrent");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("P.EDTM.scaler");
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,3,100},"P.BCM1.scalerCurrent");
      double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());
      std::cout<<"set current "<<neg_setcurrent<<std::endl;
      //std::cout<<"event size "<<neg_scaler_event_list->size()<<" current size "<<neg_scaler_current_list->size()<<std::endl;
      //all with current cut
      int scaler_entries = *d_neg_scaler.Count();
      double tot_counts=0,tot_counts_currentcut = 0,pre_counts = 0;
      for(int i = 0;i<scaler_entries;i++){
        tot_counts = neg_scaler_event_list->at(i);
        if(neg_scaler_current_list->at(i)>current_offset){
          tot_counts_currentcut = tot_counts_currentcut+(tot_counts-pre_counts);
        }
        pre_counts = tot_counts;
      }
      
      auto neg_scaler_event_list_2 = d_neg_scaler.Take<double>("evNumber");
      auto neg_get_current = [&](unsigned int eventNum){
        int i = 0;
        while(eventNum>neg_scaler_event_list_2->at(i)){
          ++i;
          if(i>neg_scaler_event_list_2->size()-1)
          {
            i=neg_scaler_event_list_2->size()-1;
            break;
          }
        }
        return neg_scaler_current_list->at(i);
      };
      auto h_EDTM_all = d_neg_scaler.Histo1D({"","",100,-1000,10000},"P.EDTM.scaler");
      auto d_neg_run = d_neg_raw
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      //accepted
      auto h_EDTM_acc = d_neg_run
        .Filter([&](double edtmtdctime){return edtmtdctime != 0;},{"T.coin.pEDTM_tdcTime"})
        .Histo1D({"","",100,-10,250},"T.coin.pEDTM_tdcTime")
        ;
     // auto h_EDTM_scl = d_neg_scaler
     //   .Filter([&](double current){return current>current_offset;},{"P.BCM1.scalerCurrent"})
     //   .Histo1D({"","",500,-1000,10000},"P.EDTM.scaler")
     //   ;


      double edtm_acc = h_EDTM_acc->GetEntries();
      double edtm_all = tot_counts_currentcut; 
      double nominal_LT = edtm_acc/edtm_all;
      j_out[(std::to_string(RunNumber)).c_str()]["TLT"] = nominal_LT;
      std::cout<<nominal_LT<<" = "<<edtm_acc<<"/"<<edtm_all<<std::endl;
      //double LT_err = nominal_LT/sqrt(edtm_acc);
      //double LT_err = nominal_LT*sqrt((edtm_all-edtm_acc)/edtm_all);
      double LT_err = (1/edtm_all)*sqrt(edtm_acc*(1-nominal_LT));
      std::cout<<LT_err<<std::endl;
      j_out[(std::to_string(RunNumber)).c_str()]["TLT_error"] = LT_err;

      //TCanvas *c_tdc = new TCanvas();
      //h_EDTM_acc->DrawCopy("hist");
      //c_tdc->SaveAs("results/LT/TLT_tdctime.pdf");
      //TCanvas *c_all = new TCanvas();
      //h_EDTM_all->DrawCopy("hist");
      //c_all->SaveAs("results/LT/TLT_all.pdf");
      //
      //TCanvas *c_check = new TCanvas();
      ////auto h_evtype = d_neg_run.Histo1D({"","",8,0,8},"fEvtHdr.fEvtType");
      //auto h_current = d_neg_run.Histo1D({"","",100,0,100},"current");
      ////h_evtype->DrawCopy("hist");
      //h_current->DrawCopy("hist");
      //c_check->SaveAs("results/LT/TLT_type.pdf");
    }//loop over each neg runs
  }//if normal production rungoup
  std::string ofs_name = "results/LT/TLT_"+std::to_string(RunGroup)+".json";
  std::ofstream ofs(ofs_name.c_str());
  ofs<<j_out.dump(4)<<std::endl;
}
