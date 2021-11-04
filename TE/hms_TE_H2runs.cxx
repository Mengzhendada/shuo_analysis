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

#include <fstream>
#include <vector>
#include <string>

  void hms_TE_H2runs(int RunGroup = 0){
    if(RunGroup == 0){
      std::cout<<"Enter RunGroup Number(-1 to exit) ";
      std::cin>>RunGroup;
      if(RunGroup < 0)
        return;
    }
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }
    std::vector<int> neg_H2,pos_H2;
    neg_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["H2"].get<std::vector<int>>();
    pos_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["H2"].get<std::vector<int>>();
    json j_runsinfo;
    {
      std::string if_name = "db2/runs_info.json";
      std::ifstream ifs(if_name.c_str());
      ifs>>j_runsinfo;
    }
    json j_cuts;
    {
      std::ifstream ifs("db2/all_cut.json");
      ifs>>j_cuts;
    }
    json j_rf_DE;
    {
      std::string name = "shuo_analysis/dbase/rf_eff.json";
      std::ifstream ifs(name.c_str());
      ifs>>j_rf_DE;
    }
    json j_DE;
    {
      std::ifstream ifs("db2/PID_test.json");
      ifs>>j_DE;
    }
    std::cout<<"check"<<std::endl;
    std::vector<int> delta_cut_num= j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>(); 
    json j_out;
    {
      std::string ifs_name =  "results/TE/trackingeff_info_"+std::to_string(RunGroup)+"_HMS.json";
      std::ifstream ifs(ifs_name.c_str());
      ifs>>j_out;
    }

    //use only strict HMS cuts, and shms without tracking cuts
    double H_dp_low = j_cuts["H_dp_low"].get<double>();
    double H_dp_high = j_cuts["H_dp_high"].get<double>();
    double P_dp_low = j_cuts["P_dp_low"].get<double>();
    double P_dp_high = j_cuts["P_dp_high"].get<double>();
    std::string goodTrackSHMS = "P.gtr.dp>"+std::to_string(P_dp_low)+" && P.gtr.dp<"+std::to_string(P_dp_high);
    std::string goodTrackHMS = "H.gtr.dp>"+std::to_string(H_dp_low)+" && H.gtr.dp<"+std::to_string(H_dp_high);
    std::cout<<goodTrackSHMS<<std::endl;
    std::cout<<goodTrackHMS<<std::endl;

    //use strict cuts
    double SHMS_low = j_DE["SHMS"]["cal_pi_low"].get<double>();
    double SHMS_high = j_DE["SHMS"]["cal_pi_high"].get<double>();
    std::string piCutSHMS = (" P.cal.etottracknorm > "+std::to_string(SHMS_low)+" && P.cal.etottracknorm < " + std::to_string(SHMS_high)).c_str();
    std::cout<<"picutSHMS "<<piCutSHMS<<std::endl;
    double P_aero = j_DE["SHMS"]["aero_n"].get<double>();
    std::string aeroCutSHMS = (" P.aero.npeSum > "+std::to_string(P_aero)).c_str();
    std::cout<<"P_aerocut "<<aeroCutSHMS<<std::endl;
    double H_cal_low = j_DE["HMS"]["cal_e"].get<double>();
    double H_cal_high = j_DE["HMS"]["cal_e_high"].get<double>();
    double H_cer = j_DE["HMS"]["cer_e"].get<double>();
    std::string eCutHMS = ("H.cal.etottracknorm > "+std::to_string(H_cal_low)+" && H.cal.etottracknorm < "+std::to_string(H_cal_high)+" && H.cer.npeSum > "+std::to_string(H_cer)).c_str();
    std::cout<<"cCutHMS"<<eCutHMS<<std::endl;

    double H_xptar_low = j_cuts["H_xptar_low"].get<double>();
    double H_xptar_high = j_cuts["H_xptar_high"].get<double>();
    double P_xptar_low = j_cuts["P_xptar_low"].get<double>();
    double P_xptar_high = j_cuts["P_xptar_high"].get<double>();
    double H_yptar_low = j_cuts["H_yptar_low"].get<double>();
    double H_yptar_high = j_cuts["H_yptar_high"].get<double>();
    double P_yptar_low = j_cuts["P_yptar_low"].get<double>();
    double P_yptar_high = j_cuts["P_yptar_high"].get<double>();
    std::string Normal_HMS = "H.gtr.th > "+std::to_string(H_xptar_low)+" && H.gtr.th < "+std::to_string(H_xptar_high)+" && H.gtr.ph > "+std::to_string(H_yptar_low)+" && H.gtr.ph < "+std::to_string(H_yptar_high); 
    std::string Normal_SHMS = "P.gtr.th > "+std::to_string(P_xptar_low)+" && P.gtr.th < "+std::to_string(P_xptar_high)+" && P.gtr.ph > "+std::to_string(P_yptar_low)+" && P.gtr.ph < "+std::to_string(P_yptar_high); 
    std::cout<<Normal_HMS<<std::endl;
    std::cout<<Normal_SHMS<<std::endl;


    if(!neg_H2.empty() && !pos_H2.empty()){

      //for neg runs
      for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        ROOT::RDataFrame d_neg_raw("T",rootfile_name);
        //d_neg_raw
        //.Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"})
        //;
        //auto h_cointime_raw_neg = d_neg_raw.Histo1D({"","cointime neg",100,20,40},"cointime_raw");
        //auto h_cal_raw_neg = d_neg_raw.Histo1D({"","etot neg",100,0.01,2},"P.cal.etotnorm");
        //auto h_hodo_goodscin_neg = d_neg_raw.Histo1D({"","goodscin neg",2,0,2},"P.hod.goodscinhit");
        //auto h_hod_betanotrack_neg = d_neg_raw.Histo1D({"","betanotrk neg",100,0,1.2},"P.hod.betanotrack");
        //auto h_dc_ntrack_neg = d_neg_raw.Histo1D({"","dcntrack neg",10,0,10},"P.dc.ntrack");
        //auto h_dc_inside_neg = d_neg_raw.Histo1D({"","dc inside neg",10,0,2},"P.dc.InsideDipoleExit");

        //rftime cut
        //offset
        double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
        auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
          double rf_pi_low,rf_pi_high; 
          int i_order = 0,i_which;
          for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
            if(SHMS_dp>*it){
              i_which = i_order;
              rf_pi_low = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["rf_cut_low"].get<double>();
              rf_pi_high = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["rf_cut_high"].get<double>();
            }
            i_order++;
          }
          return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  

        };
        auto d_neg_pi = d_neg_raw
          .Filter(piCutSHMS)
          .Filter(Normal_SHMS)
          .Filter(goodTrackSHMS)
          .Filter(aeroCutSHMS)
          .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
          .Define("diff_time_mod",[offset_neg](double difftime){return std::fmod(difftime+offset_neg,4.008);},{"fptime_minus_rf"})
          .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
          .Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"})
          //  .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
          .Filter([](double etot){return etot> 1 && etot< 2;},{"H.cal.etotnorm"})
          //.Filter("P.aero.npeSum > 2")
          .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"H.hod.betanotrack"})
          ;  
        auto h_cointime_neg = d_neg_pi.Histo1D({"","cointime",800,25,55},"cointime_raw");
        int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
        double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
        std::cout<<"coin peak "<<coin_peak_center_neg<<std::endl;
        double cointime_low,cointime_high;
        //if(RunNumber < 7000){
          cointime_low = coin_peak_center_neg+j_cuts["cointime_low_fall"].get<double>();
          cointime_high = coin_peak_center_neg+j_cuts["cointime_high_fall"].get<double>();
        //}
        //else{
        //  cointime_low = coin_peak_center_neg+j_cuts["cointime_low_spring"].get<double>();
        //  cointime_high = coin_peak_center_neg+j_cuts["cointime_high_spring"].get<double>();
        //}
        auto d_neg_pi_hod = d_neg_pi
          .Filter("H.hod.goodscinhit==1")
        .Filter("H.hod.goodstarttime==1")
        .Filter("T.coin.pT2_tdcMultiplicity<5")
          .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
          ;
        auto d_neg_pi_dc = d_neg_pi_hod
          .Filter("H.dc.ntrack>=1")
          //.Filter("H.dc.InsideDipoleExit == 1")
          .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"H.gtr.dp"})
          ;
        double  neg_pi_expected = *d_neg_pi_hod.Count();
        double  neg_pi_found = *d_neg_pi_dc.Count();
        double neg_all = *d_neg_raw.Count();

        auto h_rawcoin = d_neg_pi
          .Filter("H.hod.goodscinhit==1")
        .Filter("H.hod.goodstarttime==1")
        .Filter("T.coin.pT2_tdcMultiplicity<5")
          .Histo1D({"","raw coin time",100,0,100},"cointime_raw")
          ;
        auto h_rawcoin_cut = d_neg_pi_hod
          .Histo1D({"","raw coin time",100,0,100},"cointime_raw")
          ;
        TCanvas* c_rawcoin_te = new TCanvas();
        h_rawcoin->DrawCopy("hist");
        h_rawcoin_cut->SetLineColor(kRed);
        h_rawcoin_cut->DrawCopy("hist");
        std::string c_rawcoin_te_name = "results/TE/check/rawcointime_"+std::to_string(RunNumber)+".pdf";
        c_rawcoin_te->SaveAs(c_rawcoin_te_name.c_str());

        j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"] = neg_all;
        j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = neg_pi_expected;
        j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = neg_pi_found;
        j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["target"] = "H2";
      }//for neg runs
      //for pos runs
      for(auto it = pos_H2.begin();it!=pos_H2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        ROOT::RDataFrame d_pos_raw("T",rootfile_name);
        //d_pos_raw
        //.Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"})
        //;
        //auto h_cointime_raw_pos = d_pos_raw.Histo1D({"","cointime pos",100,20,40},"cointime_raw");
        //auto h_cal_raw_pos = d_pos_raw.Histo1D({"","etot pos",100,0.01,2},"P.cal.etotnorm");
        //auto h_hodo_goodscin_pos = d_pos_raw.Histo1D({"","goodscin pos",2,0,2},"P.hod.goodscinhit");
        //auto h_hod_betanotrack_pos = d_pos_raw.Histo1D({"","betanotrk pos",100,0,1.2},"P.hod.betanotrack");
        //

        //auto h_dc_ntrack_pos = d_pos_raw.Histo1D({"","dcntrack pos",10,0,10},"P.dc.ntrack");
        //auto h_dc_inside_pos = d_pos_raw.Histo1D({"","dc inside pos",10,0,2},"P.dc.InsideDipoleExit");
        
        //rftime cut
        //offset
        double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
        auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
          double rf_pi_low,rf_pi_high; 
          int i_order = 0,i_which;
          for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
            if(SHMS_dp>*it){
              i_which = i_order;
              rf_pi_low = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_low"].get<double>();
              rf_pi_high = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_high"].get<double>();
            }
            i_order++;
          }
          return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  

        };
        auto d_pos_pi = d_pos_raw
          .Filter(piCutSHMS)
          .Filter(Normal_SHMS)
          .Filter(goodTrackSHMS)
          .Filter(aeroCutSHMS)
          .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
          .Define("diff_time_mod",[offset_pos](double difftime){return std::fmod(difftime+offset_pos,4.008);},{"fptime_minus_rf"})
          .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
          .Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"})
          //  .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
          .Filter([](double etot){return etot> 1 && etot< 2;},{"H.cal.etotnorm"})
          //.Filter("P.aero.npeSum > 2")
          .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"H.hod.betanotrack"})
          ;  
        auto h_cointime_pos = d_pos_pi.Histo1D({"","cointime",800,25,55},"cointime_raw");
        int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
        double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
        std::cout<<"coin peak "<<coin_peak_center_pos<<std::endl;
        double cointime_low,cointime_high;
        //if(RunNumber < 7000){
          cointime_low = coin_peak_center_pos+j_cuts["cointime_low_fall"].get<double>();
          cointime_high = coin_peak_center_pos+j_cuts["cointime_high_fall"].get<double>();
       // }
       // else{
       //   cointime_low = coin_peak_center_pos+j_cuts["cointime_low_spring"].get<double>();
       //   cointime_high = coin_peak_center_pos+j_cuts["cointime_high_spring"].get<double>();
       // }
        auto d_pos_pi_hod = d_pos_pi
          .Filter("H.hod.goodscinhit==1")
        .Filter("H.hod.goodstarttime==1")
        .Filter("T.coin.pT2_tdcMultiplicity<5")
          .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
          ;
        auto d_pos_pi_dc = d_pos_pi_hod
          .Filter("H.dc.ntrack>=1")
          //.Filter("H.dc.InsideDipoleExit == 1")
          .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"H.gtr.dp"})
          ;
        double  pos_pi_expected = *d_pos_pi_hod.Count();
        double  pos_pi_found = *d_pos_pi_dc.Count();
        double pos_all = *d_pos_raw.Count();

        auto h_rawcoin = d_pos_pi
          .Filter("H.hod.goodscinhit==1")
        .Filter("H.hod.goodstarttime==1")
        .Filter("T.coin.pT2_tdcMultiplicity<5")
          .Histo1D({"","raw coin time",100,0,100},"cointime_raw")
          ;
        auto h_rawcoin_cut = d_pos_pi_hod
          .Histo1D({"","raw coin time",100,0,100},"cointime_raw")
          ;
        TCanvas* c_rawcoin_te = new TCanvas();
        h_rawcoin->DrawCopy("hist");
        h_rawcoin_cut->SetLineColor(kRed);
        h_rawcoin_cut->DrawCopy("hist");
        std::string c_rawcoin_te_name = "results/TE/check/rawcointime_"+std::to_string(RunNumber)+".pdf";
        c_rawcoin_te->SaveAs(c_rawcoin_te_name.c_str());

        j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"] = pos_all;
        j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = pos_pi_expected;
        j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = pos_pi_found;
        j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["target"] = "H2";
      }//for pos runs

    }//if not empty
    std::string ofs_name =  "results/TE/trackingeff_info_"+std::to_string(RunGroup)+"_HMS.json";
    std::ofstream ofs(ofs_name.c_str());
    //std::ofstream ofs("results/TE/trackingeff_info.json");
    ofs<<j_out.dump(4)<<std::endl;
  }
