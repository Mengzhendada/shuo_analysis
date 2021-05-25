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

  void shms_TE_H2runs(){
    //void shms_TE_H2runs(int RunGroup = 0){
    //    if(RunGroup == 0){
    //      std::cout<<"Enter RunGroup Number(-1 to exit) ";
    //      std::cin>>RunGroup;
    //      if(RunGroup < 0)
    //        return;
    //    }
    for(int i = 1;i<56;i++){
      int RunGroup = 10*i;
      json j_rungroup;
      {
        std::ifstream ifs("db2/ratio_run_group_updated.json");
        ifs>>j_rungroup;
      }
      std::vector<int> neg_H2,pos_H2;
      neg_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["H2"].get<std::vector<int>>();
      pos_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["H2"].get<std::vector<int>>();

      if(!neg_H2.empty() && !pos_H2.empty()){

        //for neg runs
        for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
          int RunNumber = *it;
          std::cout<<RunNumber<<std::endl;
          std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
          ROOT::RDataFrame d_neg_raw("T",rootfile_name);
          //auto h_cointime_raw_neg = d_neg_raw.Histo1D({"","cointime neg",100,20,40},"CTime.CoinTime_RAW_ROC2");
          //auto h_cal_raw_neg = d_neg_raw.Histo1D({"","etot neg",100,0.01,2},"P.cal.etotnorm");
          //auto h_hodo_goodscin_neg = d_neg_raw.Histo1D({"","goodscin neg",2,0,2},"P.hod.goodscinhit");
          //auto h_hod_betanotrack_neg = d_neg_raw.Histo1D({"","betanotrk neg",100,0,1.2},"P.hod.betanotrack");
          //auto h_dc_ntrack_neg = d_neg_raw.Histo1D({"","dcntrack neg",10,0,10},"P.dc.ntrack");
          //auto h_dc_inside_neg = d_neg_raw.Histo1D({"","dc inside neg",10,0,2},"P.dc.InsideDipoleExit");
          double cointime_low,cointime_high;
          cointime_low = 28;
          cointime_high = 32;
          auto d_neg_pi_hod = d_neg_raw
            .Filter("P.hod.goodscinhit==1")
            .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"CTime.CoinTime_RAW_ROC2"})
            .Filter([](double etot){return etot> 0.05 && etot< 0.8;},{"P.cal.etotnorm"})
            .Filter("P.aero.npeSum > 2")
            .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"P.hod.betanotrack"})
            .Filter([](double h_delta){return h_delta > -10 && h_delta < 10;},{"H.gtr.dp"})
            ;  
          auto d_neg_pi_dc = d_neg_pi_hod
            .Filter("P.dc.ntrack>=1")
            .Filter("P.dc.InsideDipoleExit == 1")
            .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
            ;
          double  neg_pi_expected = *d_neg_pi_hod.Count();
          double  neg_pi_found = *d_neg_pi_dc.Count();
          double neg_all = *d_neg_raw.Count();
          json j_out;

          std::ifstream ifs("results/TE/trackingeff_info.json");
          ifs>>j_out;


          j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"] = neg_all;
          j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = neg_pi_expected;
          j_out[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = neg_pi_found;
          std::ofstream ofs("results/TE/trackingeff_info.json");
          ofs<<j_out.dump(4)<<std::endl;
        }
        //for pos runs
        for(auto it = pos_H2.begin();it!=pos_H2.end();++it){
          int RunNumber = *it;
          std::cout<<RunNumber<<std::endl;
          std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
          ROOT::RDataFrame d_pos_raw("T",rootfile_name);
          //auto h_cointime_raw_pos = d_pos_raw.Histo1D({"","cointime pos",100,20,40},"CTime.CoinTime_RAW_ROC2");
          //auto h_cal_raw_pos = d_pos_raw.Histo1D({"","etot pos",100,0.01,2},"P.cal.etotnorm");
          //auto h_hodo_goodscin_pos = d_pos_raw.Histo1D({"","goodscin pos",2,0,2},"P.hod.goodscinhit");
          //auto h_hod_betanotrack_pos = d_pos_raw.Histo1D({"","betanotrk pos",100,0,1.2},"P.hod.betanotrack");
          //

          //auto h_dc_ntrack_pos = d_pos_raw.Histo1D({"","dcntrack pos",10,0,10},"P.dc.ntrack");
          //auto h_dc_inside_pos = d_pos_raw.Histo1D({"","dc inside pos",10,0,2},"P.dc.InsideDipoleExit");
          double cointime_low,cointime_high;
          cointime_low = 28;
          cointime_high = 32;
          auto d_pos_pi_hod = d_pos_raw
            .Filter("P.hod.goodscinhit==1")
            .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"CTime.CoinTime_RAW_ROC2"})
            .Filter([](double etot){return etot> 0.05 && etot< 0.8;},{"P.cal.etotnorm"})
            .Filter("P.aero.npeSum > 2")
            .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"P.hod.betanotrack"})
            .Filter([](double h_delta){return h_delta > -10 && h_delta < 10;},{"H.gtr.dp"})
            ;  
          auto d_pos_pi_dc = d_pos_pi_hod
            .Filter("P.dc.ntrack>=1")
            .Filter("P.dc.InsideDipoleExit == 1")
            .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
            ;
          double pos_pi_expected = *d_pos_pi_hod.Count();
          double  pos_pi_found = *d_pos_pi_dc.Count();
          double pos_all = *d_pos_raw.Count();
          json j_out;

          std::ifstream ifs("results/TE/trackingeff_info.json");
          ifs>>j_out;

          j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"] = pos_all;
          j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = pos_pi_expected;
          j_out[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = pos_pi_found;
          std::ofstream ofs("results/TE/trackingeff_info.json");
          ofs<<j_out.dump(4)<<std::endl;
        }

      }
    }//loop over rungroups
  }//over all
