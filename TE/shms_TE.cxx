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

void shms_TE(int RunGroup = 0){
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
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  
  if(!neg_D2.empty() && !pos_D2.empty()){
  std::vector<std::string> files_neg,files_pos;
  double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

  //for neg runs
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_neg.push_back(rootfile_name);
  }
  //for pos runs
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_pos.push_back(rootfile_name);
  }

  ROOT::RDataFrame d_neg_raw("T",files_neg);
  ROOT::RDataFrame d_pos_raw("T",files_pos);

  d_neg_raw.Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"});
  d_pos_raw.Define("cointime_raw",[](double pTRIG1,double pTRIG4,double pstarttime,double hstarttime){return (pTRIG1+pstarttime)-(pTRIG4+hstarttime);},{"T.coin.pTRIG1_ROC2_tdcTime","T.coin.pTRIG4_ROC2_tdcTime","P.hod.starttime","H.hod.starttime"});

  auto h_cointime_raw_pos = d_pos_raw.Histo1D({"","cointime pos",100,20,40},"cointime_raw");
  auto h_cointime_raw_neg = d_neg_raw.Histo1D({"","cointime neg",100,20,40},"cointime_raw");
  auto h_cal_raw_pos = d_pos_raw.Histo1D({"","etot pos",100,0.01,2},"P.cal.etotnorm");
  auto h_cal_raw_neg = d_neg_raw.Histo1D({"","etot neg",100,0.01,2},"P.cal.etotnorm");
  auto h_hodo_goodscin_pos = d_pos_raw.Histo1D({"","goodscin pos",2,0,2},"P.hod.goodscinhit");
  auto h_hodo_goodscin_neg = d_neg_raw.Histo1D({"","goodscin neg",2,0,2},"P.hod.goodscinhit");
  auto h_hod_betanotrack_pos = d_pos_raw.Histo1D({"","betanotrk pos",100,0,1.2},"P.hod.betanotrack");
  auto h_hod_betanotrack_neg = d_neg_raw.Histo1D({"","betanotrk neg",100,0,1.2},"P.hod.betanotrack");
  

  auto h_dc_ntrack_pos = d_pos_raw.Histo1D({"","dcntrack pos",10,0,10},"P.dc.ntrack");
  auto h_dc_ntrack_neg = d_neg_raw.Histo1D({"","dcntrack neg",10,0,10},"P.dc.ntrack");
  auto h_dc_inside_pos = d_pos_raw.Histo1D({"","dc inside pos",10,0,2},"P.dc.InsideDipoleExit");
  auto h_dc_inside_neg = d_neg_raw.Histo1D({"","dc inside neg",10,0,2},"P.dc.InsideDipoleExit");

  TCanvas* c_hod_1 = new TCanvas();
  c_hod_1->Divide(2,2);
  c_hod_1->cd(1);
  h_cointime_raw_pos->DrawCopy("hist");
  c_hod_1->cd(2);
  h_cointime_raw_neg->DrawCopy("hist");
  c_hod_1->cd(3);
  h_cal_raw_pos->DrawCopy("hist");
  c_hod_1->cd(4);
  h_cal_raw_neg->DrawCopy("hist");
  std::string c_hod_1_name = "results/TE/hod_1_"+std::to_string(RunGroup)+".pdf";
  c_hod_1->SaveAs(c_hod_1_name.c_str());

  TCanvas* c_hod_2 = new TCanvas();
  c_hod_2->Divide(2,2);
  c_hod_2->cd(1);
  h_hodo_goodscin_pos->DrawCopy("hist");
  c_hod_2->cd(2);
  h_hodo_goodscin_neg->DrawCopy("hist");
  c_hod_2->cd(3);
  h_hod_betanotrack_pos->DrawCopy("hist");
  c_hod_2->cd(4);
  h_hod_betanotrack_pos->DrawCopy("hist");
  std::string c_hod_2_name = "results/TE/hod_2_"+std::to_string(RunGroup)+".pdf";
  c_hod_2->SaveAs(c_hod_2_name.c_str());

  TCanvas* c_dc = new TCanvas();
  c_dc->Divide(2,2);
  c_dc->cd(1);
  h_dc_ntrack_pos->DrawCopy("hist");
  c_dc->cd(2);
  h_dc_ntrack_neg->DrawCopy("hist");
  c_dc->cd(3);
  h_dc_inside_pos->DrawCopy("hist");
  c_dc->cd(4);
  h_dc_inside_neg->DrawCopy("hist");
  std::string c_dc_name = "results/TE/dc_"+std::to_string(RunGroup)+".pdf";
  c_dc->SaveAs(c_dc_name.c_str());
  
  double cointime_low,cointime_high;
  cointime_low = 30;
  cointime_high = 50;
  auto d_pos_calcut = d_pos_raw.Filter([](double etot){return etot> 0.05 && etot< 0.8;},{"P.cal.etotnorm"})
    .Filter([](double aero){return aero>2;},{"P.aero.npeSum"})
    ;
  auto h_cointime_pi_pos = d_pos_calcut.Histo1D({"","cointime pos",100,20,50},"cointime_raw");
  auto h_goodscin_pi_pos = d_pos_calcut.Histo1D({"","good scin pos",2,0,2},"P.hod.goodscinhit");
  TCanvas* c_coin_pi = new TCanvas();
  c_coin_pi->Divide(1,2);
  c_coin_pi->cd(1);
  h_cointime_pi_pos->DrawCopy("hist");
  c_coin_pi->cd(2);
  h_goodscin_pi_pos->DrawCopy("hist");
  std::string c_coin_pi_name = "results/TE/cointime_pi_pos"+std::to_string(RunGroup)+".pdf";
  c_coin_pi->SaveAs(c_coin_pi_name.c_str());

  auto d_pos_pi_hod = d_pos_raw
    .Filter("P.hod.goodscinhit==1")
    .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
    .Filter([](double etot){return etot> 0.05 && etot< 0.8;},{"P.cal.etotnorm"})
    .Filter("P.aero.npeSum > 2")
    .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"P.hod.betanotrack"})
    .Filter([](double h_delta){return h_delta > -10 && h_delta < 10;},{"P.gtr.dp"})
    ;  
  auto d_pos_pi_dc = d_pos_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    ;
    auto d_pos_pi_dc_2 = d_pos_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter("P.gtr.dp > -10 && P.gtr.dp< 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    //.Filter("H.cal.etottracknorm > 0.85")

    //.Filter("H.cer.npeSum > 10")
    ;
    auto d_pos_pi_dc_3 = d_pos_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter("P.gtr.dp > -10 && P.gtr.dp< 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    //.Filter("H.cal.etottracknorm > 0.85")
    //.Filter("H.cer.npeSum > 10")
    .Filter("fEvtHdr.fEvtType == 4")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    ;

  auto d_neg_pi_hod = d_neg_raw
    .Filter("P.hod.goodscinhit==1")
    .Filter([cointime_low,cointime_high](double cointime){return cointime>cointime_low && cointime< cointime_high;},{"cointime_raw"})
    .Filter([](double etot){return etot> 0.05 && etot< 0.8;},{"P.cal.etotnorm"})
    .Filter("P.aero.npeSum > 2")
    .Filter([](double beta){return beta< 1.4 && beta > 0.6;},{"P.hod.betanotrack"})
    .Filter([](double h_delta){return h_delta > -10 && h_delta < 10;},{"P.gtr.dp"})
    ;  
  auto d_neg_pi_dc = d_neg_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    ;
    auto d_neg_pi_dc_2 = d_neg_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter("P.gtr.dp > -10 && P.gtr.dp< 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    //.Filter("H.cal.etottracknorm > 0.85")

    //.Filter("H.cer.npeSum > 10")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    ;
    auto d_neg_pi_dc_3 = d_neg_pi_hod
    .Filter("P.dc.ntrack>=1")
    .Filter("P.dc.InsideDipoleExit == 1")
    .Filter("P.gtr.dp > -10 && P.gtr.dp< 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    //.Filter("H.cal.etottracknorm > 0.85")
    //.Filter("H.cer.npeSum > 10")
    .Filter("fEvtHdr.fEvtType == 4")
    .Filter([](double p_delta){return p_delta>-25 && p_delta<50;},{"P.gtr.dp"})
    ;
  double pos_pi_expected = *d_pos_pi_hod.Count();
  double  pos_pi_found = *d_pos_pi_dc.Count();
  double  pos_pi_found_2 = *d_pos_pi_dc_2.Count();
  double  pos_pi_found_3 = *d_pos_pi_dc_3.Count();
  double  neg_pi_expected = *d_neg_pi_hod.Count();
  double  neg_pi_found = *d_neg_pi_dc.Count();
  double  neg_pi_found_2 = *d_neg_pi_dc_2.Count();
  double  neg_pi_found_3 = *d_neg_pi_dc_3.Count();
  double neg_all = *d_neg_raw.Count();
  double pos_all = *d_pos_raw.Count();
  json j_out;
  { 
    std::ifstream ifs("results/TE/trackingeff_info.json");
    ifs>>j_out;
  }
  j_out[(std::to_string(RunGroup)).c_str()]["neg"]["counts"] = neg_all;
  j_out[(std::to_string(RunGroup)).c_str()]["pos"]["counts"] = pos_all;
  j_out[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_expected"] = pos_pi_expected;
  j_out[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_1"] = pos_pi_found;
  j_out[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_2"] = pos_pi_found_2;
  j_out[(std::to_string(RunGroup)).c_str()]["pos"]["SHMS_pi_found_3"] = pos_pi_found_3;
  j_out[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_expected"] = neg_pi_expected;
  j_out[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_1"] = neg_pi_found;
  j_out[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_2"] = neg_pi_found_2;
  j_out[(std::to_string(RunGroup)).c_str()]["neg"]["SHMS_pi_found_3"] = neg_pi_found_3;
  
  std::ofstream ofs("results/TE/trackingeff_info.json");
   ofs<<j_out.dump(4)<<std::endl;

  
  


  }
}
