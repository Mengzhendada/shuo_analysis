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

void Computer_live_time(int RunGroup = 0){
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

  json j_out;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_out;
  }

  if(!neg_D2.empty() && !pos_D2.empty()){

    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);
      ROOT::RDataFrame d_pos_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto pos_scaler_current_list = d_pos_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto pos_scaler_event_list = d_pos_scaler.Take<double>("evNumber");
      auto h_pos_current = d_pos_scaler.Histo1D({"pos current","pos current",100,3,100},"P.BCM4B.scalerCurrent");
      double pos_setcurrent = h_pos_current->GetBinCenter(h_pos_current->GetMaximumBin());
      std::cout<<"set current "<<pos_setcurrent<<std::endl;
      //std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size "<<pos_scaler_current_list->size()<<std::endl;
      //all with current cut
      double current_th = 3;
      auto pos_get_current = [&](unsigned int eventNum){
        int i = 0;
        while(eventNum>pos_scaler_event_list->at(i)){
          ++i;
          if(i>pos_scaler_event_list->size()-1)
          {
            i=pos_scaler_event_list->size()-1;
            break;
          }
        }
        return pos_scaler_current_list->at(i);
      };
      //from scaler
      auto pos_scaler_trig6_list = d_pos_scaler.Take<double>("P.pTRIG6.scaler");
      int scaler_entries = *d_pos_scaler.Count();
      double tot_trigs=0,tot_trigs_currentcut = 0,pre_trigs = 0;
      auto pos_scaler_edtm_list = d_pos_scaler.Take<double>("P.EDTM.scaler");
      double tot_edtm=0,tot_edtm_currentcut = 0,pre_edtm = 0;
      for(int i = 0;i<scaler_entries;i++){
        tot_trigs = pos_scaler_trig6_list->at(i);
        tot_edtm = pos_scaler_edtm_list->at(i);
        if(std::abs(pos_setcurrent - pos_scaler_current_list->at(i))<current_th){
          tot_trigs_currentcut = tot_trigs_currentcut+(tot_trigs-pre_trigs);
          tot_edtm_currentcut = tot_edtm_currentcut+(tot_edtm-pre_edtm);
        }
        pre_trigs = tot_trigs;
        pre_edtm = tot_edtm;
      }
      auto h_trig_all = d_pos_scaler.Histo1D({"","",100,-1000,10000},"P.pTRIG6.scaler");
      
      //all from data
      auto d_pos_run = d_pos_raw
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(pos_setcurrent - current)<current_th;},{"current"})
        ;
      //accepted 
      auto d_pos_clt = d_pos_run
        .Filter([&](int evtyp){return evtyp>3;},{"fEvtHdr.fEvtType"})
        .Filter([&](double ptrig6_data){return ptrig6_data>0.;},{"T.coin.pTRIG6_ROC2_tdcTimeRaw"})
        ;
      auto h_trig_acc = d_pos_clt.Histo1D({"","",100,-100,100},"T.coin.pTRIG6_ROC2_tdcTimeRaw");
      double trigs_acc = h_trig_acc->GetEntries();
      //accepted
      auto d_pos_clttrig = d_pos_run
        .Filter([&](int evtyp){return evtyp>3;},{"fEvtHdr.fEvtType"})
        ;
      
      //accepted edtm
      auto h_EDTM_acc = d_pos_run
        .Filter([&](double edtmtdctime){return edtmtdctime> 0;},{"T.coin.pEDTM_tdcTime"})
        .Histo1D({"","",500,120,250},"T.coin.pEDTM_tdcTime")
        ;
      double edtm_acc = h_EDTM_acc->GetEntries();
      //calculation of clt
      double clt_all = trigs_acc/tot_trigs_currentcut;
      double clt_phy = (trigs_acc-edtm_acc)/(tot_trigs_currentcut-tot_edtm_currentcut);
      std::cout<<clt_all<<" = "<<trigs_acc<<"/"<<tot_trigs_currentcut<<std::endl;
      std::cout<<clt_phy<<" = "<<trigs_acc<<"-"<<edtm_acc<<"/"<<tot_trigs_currentcut<<"-"<<tot_edtm_currentcut<<std::endl;
      j_out[(std::to_string(RunNumber)).c_str()]["CLT_all"] = clt_all;
      j_out[(std::to_string(RunNumber)).c_str()]["CLT_phy"] = clt_phy;
     // TCanvas *c_tdc = new TCanvas();
     // h_trig_acc->DrawCopy("hist");
     // c_tdc->SaveAs("results/LT/CLT_tdctime.pdf");
     // TCanvas *c_all = new TCanvas();
     // h_trig_all->DrawCopy("hist");
     // c_all->SaveAs("results/LT/CLT_all.pdf");
     //
     // TCanvas *c_check = new TCanvas();
     // //auto h_evtype = d_pos_run.Histo1D({"","",8,0,8},"fEvtHdr.fEvtType");
     // auto h_current = d_pos_run.Histo1D({"","",100,-1000,1000},"current");
     // //h_evtype->DrawCopy("hist");
     // h_current->DrawCopy("hist");
     // c_check->SaveAs("results/LT/CLT_type.pdf");
    }//loop over each pos runs
    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,3,100},"P.BCM4B.scalerCurrent");
      double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());
      std::cout<<"set current "<<neg_setcurrent<<std::endl;
      //std::cout<<"event size "<<neg_scaler_event_list->size()<<" current size "<<neg_scaler_current_list->size()<<std::endl;
      //all with current cut
      double current_th = 3;
      auto neg_get_current = [&](unsigned int eventNum){
        int i = 0;
        while(eventNum>neg_scaler_event_list->at(i)){
          ++i;
          if(i>neg_scaler_event_list->size()-1)
          {
            i=neg_scaler_event_list->size()-1;
            break;
          }
        }
        return neg_scaler_current_list->at(i);
      };
      //from scaler
      auto neg_scaler_trig6_list = d_neg_scaler.Take<double>("P.pTRIG6.scaler");
      int scaler_entries = *d_neg_scaler.Count();
      double tot_trigs=0,tot_trigs_currentcut = 0,pre_trigs = 0;
      auto neg_scaler_edtm_list = d_neg_scaler.Take<double>("P.EDTM.scaler");
      double tot_edtm=0,tot_edtm_currentcut = 0,pre_edtm = 0;
      for(int i = 0;i<scaler_entries;i++){
        tot_trigs = neg_scaler_trig6_list->at(i);
        tot_edtm = neg_scaler_edtm_list->at(i);
        if(std::abs(neg_setcurrent - neg_scaler_current_list->at(i))<current_th){
          tot_trigs_currentcut = tot_trigs_currentcut+(tot_trigs-pre_trigs);
          tot_edtm_currentcut = tot_edtm_currentcut+(tot_edtm-pre_edtm);
        }
        pre_trigs = tot_trigs;
        pre_edtm = tot_edtm;
      }
      auto h_trig_all = d_neg_scaler.Histo1D({"","",100,-1000,10000},"P.pTRIG6.scaler");
      
      //all from data
      auto d_neg_run = d_neg_raw
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(neg_setcurrent - current)<current_th;},{"current"})
        ;
      //accepted 
      auto d_neg_clt = d_neg_run
        .Filter([&](int evtyp){return evtyp>3;},{"fEvtHdr.fEvtType"})
        .Filter([&](double ptrig6_data){return ptrig6_data>0.;},{"T.coin.pTRIG6_ROC2_tdcTimeRaw"})
        ;
      auto h_trig_acc = d_neg_clt.Histo1D({"","",100,-100,100},"T.coin.pTRIG6_ROC2_tdcTimeRaw");
      double trigs_acc = h_trig_acc->GetEntries();
      //accepted
      auto d_neg_clttrig = d_neg_run
        .Filter([&](int evtyp){return evtyp>3;},{"fEvtHdr.fEvtType"})
        ;
      
      //accepted edtm
      auto h_EDTM_acc = d_neg_run
        .Filter([&](double edtmtdctime){return edtmtdctime> 0;},{"T.coin.pEDTM_tdcTime"})
        .Histo1D({"","",500,120,250},"T.coin.pEDTM_tdcTime")
        ;
      double edtm_acc = h_EDTM_acc->GetEntries();
      //calculation of clt
      double clt_all = trigs_acc/tot_trigs_currentcut;
      double clt_phy = (trigs_acc-edtm_acc)/(tot_trigs_currentcut-tot_edtm_currentcut);
      std::cout<<clt_all<<" = "<<trigs_acc<<"/"<<tot_trigs_currentcut<<std::endl;
      std::cout<<clt_phy<<" = "<<trigs_acc<<"-"<<edtm_acc<<"/"<<tot_trigs_currentcut<<"-"<<tot_edtm_currentcut<<std::endl;

      j_out[(std::to_string(RunNumber)).c_str()]["CLT_all"] = clt_all;
      j_out[(std::to_string(RunNumber)).c_str()]["CLT_phy"] = clt_phy;
      //TCanvas *c_tdc = new TCanvas();
      //h_trig_acc->DrawCopy("hist");
      //c_tdc->SaveAs("results/LT/CLT_tdctime.pdf");
      //TCanvas *c_all = new TCanvas();
      //h_trig_all->DrawCopy("hist");
      //c_all->SaveAs("results/LT/CLT_all.pdf");
     
      //TCanvas *c_check = new TCanvas();
      ////auto h_evtype = d_neg_run.Histo1D({"","",8,0,8},"fEvtHdr.fEvtType");
      //auto h_current = d_neg_run.Histo1D({"","",100,-1000,1000},"current");
      ////h_evtype->DrawCopy("hist");
      //h_current->DrawCopy("hist");
      //c_check->SaveAs("results/LT/CLT_type.pdf");
    }//loop over each neg runs
  }//if normal production rungoup
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_out.dump(4)<<std::endl;
}
