#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RSnapshotOptions.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TVector3.h"
#include "Math/IFunction.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include <iostream>

const double GeV         = 1.78266192e-27;
const double c           = 299792458;
const double shms_length = 20.1;

double t_e(double p) {
  const double m = 0.00051;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_pi(double p) {
  const double m = 0.139;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_K(double p) {
  const double m = 0.493;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_proton(double p) {
  const double m = 0.938;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
void SHMS_rf_Kpi(int RunGroup = 0) {
  if (RunGroup == 0) {
    std::cout << "Enter a RunGroup (-1 to exit):";
    std::cin >> RunGroup;
    if (RunGroup <= 0)
      return;
  }

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs >> j_rungroup;
    std::cout << "derp\n";
  }

  json j_rungroup_info;

  std::vector<int> neg_D2, pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double shms_p_central = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  double shms_p;
  json   j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs >> j_cuts;
  };
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs >> j_DE;
  }
  std::vector<double> rf_cuts = j_DE["SHMS"]["rf_time_right_cuts"].get<std::vector<double>>();

  double      H_dp_low  = j_cuts["H_dp_low"].get<double>();
  double      H_dp_high = j_cuts["H_dp_high"].get<double>();
  double      P_dp_low  = j_cuts["P_dp_low"].get<double>();
  double      P_dp_high = j_cuts["P_dp_high"].get<double>();
  std::string goodTrackSHMS =
    "P.gtr.dp>" + std::to_string(P_dp_low) + " && P.gtr.dp<" + std::to_string(P_dp_high);
  std::string goodTrackHMS =
    "H.gtr.dp>" + std::to_string(H_dp_low) + " && H.gtr.dp<" + std::to_string(H_dp_high);
  std::cout << goodTrackSHMS << std::endl;
  std::cout << goodTrackHMS << std::endl;
  double      SHMS_low  = j_cuts["P_cal_pi_low"].get<double>();
  double      SHMS_high = j_cuts["P_cal_pi_high"].get<double>();
  std::string piCutSHMS = (" P.cal.etottracknorm > " + std::to_string(SHMS_low) +
      " && P.cal.etottracknorm < " + std::to_string(SHMS_high))
    .c_str();
  std::cout << "picutSHMS " << piCutSHMS << std::endl;
  // double P_aero = j_cuts["P_aero"].get<double>();
  double      P_aero      = j_DE["SHMS"]["aero_n"].get<double>();
  std::string aeroCutSHMS = (" P.aero.npeSum > " + std::to_string(P_aero)).c_str();
  std::cout << "P_aerocut " << aeroCutSHMS << std::endl;
  double      H_cal_low     = j_cuts["H_cal_low"].get<double>();
  double      H_cal_high    = j_cuts["H_cal_high"].get<double>();
  double      H_cer         = j_cuts["H_cer"].get<double>();
  double      P_hgcer       = j_cuts["P_hgcer"].get<double>();
  std::string hgcCutSHMS    = (" P.hgcer.npeSum > " + std::to_string(P_hgcer)).c_str();
  //std::string SHMS_hgc_aero = aeroCutSHMS;
  std::string SHMS_anti_hgc_aero = aeroCutSHMS+" && P.hgcer.npeSum<2";
  std::string eCutHMS =
    ("H.cal.etottracknorm > " + std::to_string(H_cal_low) + " && H.cal.etottracknorm < " +
     std::to_string(H_cal_high) + " && H.cer.npeSum > " + std::to_string(H_cer))
    .c_str();
  std::cout << "cCutHMS" << eCutHMS << std::endl;
  double      H_xptar_low  = j_cuts["H_xptar_low"].get<double>();
  double      H_xptar_high = j_cuts["H_xptar_high"].get<double>();
  double      P_xptar_low  = j_cuts["P_xptar_low"].get<double>();
  double      P_xptar_high = j_cuts["P_xptar_high"].get<double>();
  double      H_yptar_low  = j_cuts["H_yptar_low"].get<double>();
  double      H_yptar_high = j_cuts["H_yptar_high"].get<double>();
  double      P_yptar_low  = j_cuts["P_yptar_low"].get<double>();
  double      P_yptar_high = j_cuts["P_yptar_high"].get<double>();
  std::string Normal_HMS   = "H.gtr.th > " + std::to_string(H_xptar_low) + " && H.gtr.th < " +
    std::to_string(H_xptar_high) + " && H.gtr.ph > " +
    std::to_string(H_yptar_low) + " && H.gtr.ph < " +
    std::to_string(H_yptar_high);
  std::string Normal_SHMS = "P.gtr.th > " + std::to_string(P_xptar_low) + " && P.gtr.th < " +
    std::to_string(P_xptar_high) + " && P.gtr.ph > " +
    std::to_string(P_yptar_low) + " && P.gtr.ph < " +
    std::to_string(P_yptar_high);
  double current_offset = j_cuts["current_diff"].get<double>();
  std::cout << Normal_HMS << std::endl;
  std::cout << Normal_SHMS << std::endl;

  //HGC bad region cut not used
  std::string HGC_bad_center = "(P.hgcer.yAtCer-1.33)*(P.hgcer.yAtCer-1.33) +  (P.hgcer.xAtCer-0.83)*(P.hgcer.xAtCer-0.83) >= 36";
  std::string HGC_bad_strip = "(P.hgcer.xAtCer<0 || P.hgcer.xAtCer>3)";
  std::string HGC_bad = HGC_bad_center + " && "+ HGC_bad_strip;
  
  //momentum greater than 2.8
  auto SHMS_p_cut = [=](double shms_dp){
    double shms_p = (shms_dp+100)*shms_p_central/100; 
    return shms_p>2.8;
  };
  //HGC_aero cut event level
  auto SHMS_hgc_aero =[=](double shms_dp,double hgc_Npe,double aero_Npe){

    if((shms_dp+100)*shms_p_central/100>2.8){
        return hgc_Npe>P_hgcer && aero_Npe>P_aero;
    }
    else{
      return aero_Npe>P_aero;
    }
  };
  /*
  if (shms_p_central > 3) {
    SHMS_hgc_aero = aeroCutSHMS 
      +" && "+hgcCutSHMS;
  }
  else {
    SHMS_hgc_aero = aeroCutSHMS;
  }
  */

  //double shms_p_average = 2.6;//average of all the runs, roughly
  auto rftime_K = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_K((100+SHMS_dp)*shms_p_central/100)-t_K(shms_p_central);
    return rftime+rf_shift;
  };
  auto rftime_pi = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_pi((100+SHMS_dp)*shms_p_central/100)-t_pi(shms_p_central);
    return rftime+rf_shift;
  };
  auto rftime_proton = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_proton((100+SHMS_dp)*shms_p_central/100)-t_proton(shms_p_central);
    return rftime+rf_shift;
  };


  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs >> j_runsinfo;
  }

  if (!neg_D2.empty() && !pos_D2.empty()) {

    double cointime_lowcut, cointime_highcut;
    if (RunGroup < 410) {
      cointime_lowcut  = j_cuts["cointime_low_fall"].get<double>();
      cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
    } else {
      cointime_lowcut  = j_cuts["cointime_low_spring"].get<double>();
      cointime_highcut = j_cuts["cointime_high_spring"].get<double>();
    }

    std::unique_ptr<TFile> fout(
        TFile::Open(string("results/pid/rf_histograms" + std::to_string(RunGroup) + "_aero" +
            std::to_string(int(P_aero))+ ".root")
          //std::to_string(int(P_aero))+ "_withhgc.root")
        .c_str(),
        "RECREATE"));


    // loop over each pos runs data
    std::cout << "check" << std::endl;



    TH1D* h_rf_pos_Kall = new TH1D("", ";rftime_K;counts", 150, -1, 5.008);
    TH1D* h_rf_neg_Kall = new TH1D("", ";rftime_K;counts", 150, -1, 5.008);

    TH1D* h_rfpi_pos_Kall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
    TH1D* h_rfpi_neg_Kall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
    TH1D* h_rfproton_pos_Kall = new TH1D("", ";rftime_proton;counts", 150, -1, 5.008);
    TH1D* h_rfproton_neg_Kall = new TH1D("", ";rftime_proton;counts", 150, -1, 5.008);
    
    TH1D* h_rf_pos_piall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
    TH1D* h_rf_neg_piall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);


  for (auto it = pos_D2.begin(); it != pos_D2.end(); ++it) {
    int RunNumber = *it;
    std::cout << "pos data" << RunNumber << std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_" +
      std::to_string(RunNumber) + "_" + std::to_string(RunNumber) +
      ".root";
    ROOT::RDataFrame d_pos_raw("T", rootfile_name);
    ROOT::RDataFrame d_pos_scaler("TSP", rootfile_name);
    std::cout << rootfile_name << std::endl;
    auto   pos_scaler_current_list = d_pos_scaler.Take<double>("P.BCM1.scalerCurrent");
    auto   pos_scaler_event_list   = d_pos_scaler.Take<double>("evNumber");
    auto   h_pos_current  = d_pos_scaler.Histo1D({"pos current", "pos current", 100, 3, 100},
        "P.BCM1.scalerCurrent");
    double pos_setcurrent = h_pos_current->GetBinCenter(h_pos_current->GetMaximumBin());
    std::cout << "set current " << pos_setcurrent << std::endl;
    // std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size
    // "<<pos_scaler_current_list->size()<<std::endl;
    auto pos_get_current = [&](unsigned int eventNum) {
      int i = 0;
      while (eventNum > pos_scaler_event_list->at(i)) {
        ++i;
        if (i > pos_scaler_event_list->size() - 1) {
          i = pos_scaler_event_list->size() - 1;
          break;
        }
      }
      return pos_scaler_current_list->at(i);
      // std::cout<< pos_scaler_current_list->at(i)<<std::endl;
    };
    auto d_pos_run =
      d_pos_raw
      .Filter("fEvtHdr.fEvtType == 4")
      //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
      //.Filter(aeroCutSHMS)
      .Filter(Normal_SHMS)
      .Filter(Normal_HMS)
      .Define("fptime_minus_rf", "P.hod.fpHitsTime - T.coin.pRF_tdcTime")
      .Define("current", pos_get_current, {"fEvtHdr.fEvtNum"})
      //.Filter(HGC_bad)
      .Filter([&](double current) { return current > current_offset; }, {"current"});

    // coin time cut for pos runs
    auto h_cointime_pos =
      d_pos_run.Histo1D({"", "coin_time", 800, 40, 46}, "CTime.ePiCoinTime_ROC2");
    int    coin_peak_bin_pos    = h_cointime_pos->GetMaximumBin();
    double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
    std::cout << "pos coin time peak " << coin_peak_center_pos << std::endl;
    // cointime cut
    double cointime_low_pos  = coin_peak_center_pos + cointime_lowcut;
    double cointime_high_pos = coin_peak_center_pos + cointime_highcut;

    auto d_pos_first = d_pos_run.Filter(
        [cointime_low_pos, cointime_high_pos](double cointime) {
        return cointime > cointime_low_pos && cointime < cointime_high_pos;
        },
        {"CTime.ePiCoinTime_ROC2"});
    auto h_coin_pos = d_pos_run.Histo1D({"", "", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");
    auto h_coin_poscut_rungroup =
      d_pos_first.Histo1D({"", "", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");



    // rftime cut
    // offset
    double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
    std::cout << "offset for pos runs " << offset_pos << std::endl;
    auto d_mod_first =
      d_pos_first
      .Define("diff_time_shift",
          [offset_pos](double difftime) { return difftime + offset_pos; },
          {"fptime_minus_rf"})
    .Define("diff_time_mod", [](double difftime) { return std::fmod(difftime, 4.008); },
        {"diff_time_shift"})
    .Define("rf_k",rftime_K,{"P.gtr.dp","diff_time_mod"}) 
    .Define("rf_proton",rftime_proton,{"P.gtr.dp","diff_time_mod"}) 
      .Define("rf_pi",rftime_pi,{"P.gtr.dp","diff_time_mod"}) 
      //    .Filter(aeroCutSHMS)
      ;


    std::string bg_cut = " ";
    // for bg
    int bg_left_low   = j_cuts["random_bg_left_low"].get<int>();
    int bg_left_high  = j_cuts["random_bg_left_high"].get<int>();
    int bg_right_low  = j_cuts["random_bg_right_low"].get<int>();
    int bg_right_high = j_cuts["random_bg_right_high"].get<int>();
    for (int i = bg_left_low; i < bg_left_high; i = i + 2) {
      double bg_main  = coin_peak_center_pos + i * 4.008;
      double bg_left  = bg_main + cointime_lowcut;
      double bg_right = bg_main + cointime_highcut;
      bg_cut = bg_cut + " (bg_cointime > " + std::to_string(bg_left) + " && bg_cointime < " +
        std::to_string(bg_right) + ") ||";
    }
    for (int i = bg_right_low; i < bg_right_high; i = i + 2) {
      double bg_main  = coin_peak_center_pos + i * 4.008;
      double bg_left  = bg_main + cointime_lowcut;
      double bg_right = bg_main + cointime_highcut;
      bg_cut = bg_cut + " (bg_cointime > " + std::to_string(bg_left) + " && bg_cointime < " +
        std::to_string(bg_right) + ") ||";
    }
    bg_cut = bg_cut.substr(0, bg_cut.size() - 2);
    std::cout << bg_cut << std::endl;

    auto d_pos_bg_norfcut =
      d_pos_run
      .Define("bg_cointime", [](double cointime) { return cointime; },
          {"CTime.ePiCoinTime_ROC2"})
    .Filter(bg_cut)
      .Define("diff_time_shift",
          [offset_pos](double difftime) { return difftime + offset_pos; },
          {"fptime_minus_rf"})
    .Define("diff_time_mod", [](double difftime) { return std::fmod(difftime, 4.008); },
        {"diff_time_shift"})
    .Define("rf_k",rftime_K,{"P.gtr.dp","diff_time_mod"}) 
    .Define("rf_proton",rftime_proton,{"P.gtr.dp","diff_time_mod"}) 
      .Define("rf_pi",rftime_pi,{"P.gtr.dp","diff_time_mod"}) 
      //  .Filter(aeroCutSHMS)
      ;
    auto d_pos_piall = d_mod_first
      //.Filter(aeroCutSHMS)
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_hgc_aero,{"P.gtr.dp","P.hgcer.npeSum","P.aero.npeSum"});
    auto d_pos_Kall = d_mod_first
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_anti_hgc_aero)
      ;
    auto d_pos_piall_bg = d_pos_bg_norfcut
      //.Filter(aeroCutSHMS)
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_hgc_aero,{"P.gtr.dp","P.hgcer.npeSum","P.aero.npeSum"});
    auto d_pos_Kall_bg = d_pos_bg_norfcut
      //.Filter("P.aero.npeSum<10")
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_anti_hgc_aero)
      ;

    // not really necessary to check this histogram.
    auto h_coin_pos_piall =
      d_pos_piall.Histo1D({"", "pos eall", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");
    auto h_coin_pos_piall_bg =
      d_pos_piall_bg.Histo1D({"", "pos eall", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");

    // statistics for DE efficiency
    auto h_rf_pos = d_pos_piall.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
    auto h_rf_pos_bg =
      d_pos_piall_bg.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_pi");
    h_rf_pos->Add(h_rf_pos_bg.GetPtr(), -1.0 / 6);
    h_rf_pos_piall->Add(h_rf_pos.GetPtr(), 1);

    auto h_rf_pos_K =
      d_pos_Kall.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_k");
    auto h_rf_pos_K_bg =
      d_pos_Kall_bg.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_k");
    h_rf_pos_K->Add(h_rf_pos_K_bg.GetPtr(), -1.0 / 6);
    h_rf_pos_Kall->Add(h_rf_pos_K.GetPtr(), 1);
    
    auto h_rfpi_pos_K =
      d_pos_Kall.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
    auto h_rfpi_pos_K_bg =
      d_pos_Kall_bg.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_pi");
    h_rfpi_pos_K->Add(h_rfpi_pos_K_bg.GetPtr(), -1.0 / 6);
    h_rfpi_pos_Kall->Add(h_rfpi_pos_K.GetPtr(), 1);
    auto h_rfproton_pos_K =
      d_pos_Kall.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_proton");
    auto h_rfproton_pos_K_bg =
      d_pos_Kall_bg.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_proton");
    h_rfproton_pos_K->Add(h_rfproton_pos_K_bg.GetPtr(), -1.0 / 6);
    h_rfproton_pos_Kall->Add(h_rfproton_pos_K.GetPtr(), 1);

  }


  // loop over each neg runs data
  for (auto it = neg_D2.begin(); it != neg_D2.end(); ++it) {
    int RunNumber = *it;
    std::cout << "neg data" << RunNumber << std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_" +
      std::to_string(RunNumber) + "_" + std::to_string(RunNumber) +
      ".root";
    ROOT::RDataFrame d_neg_raw("T", rootfile_name);
    ROOT::RDataFrame d_neg_scaler("TSP", rootfile_name);
    std::cout << rootfile_name << std::endl;
    auto   neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM1.scalerCurrent");
    auto   neg_scaler_event_list   = d_neg_scaler.Take<double>("evNumber");
    auto   h_neg_current  = d_neg_scaler.Histo1D({"neg current", "neg current", 100, 3, 100},
        "P.BCM1.scalerCurrent");
    double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());
    std::cout << "set current " << neg_setcurrent << std::endl;
    // std::cout<<"event size "<<neg_scaler_event_list->size()<<" current size
    // "<<neg_scaler_current_list->size()<<std::endl;
    auto neg_get_current = [&](unsigned int eventNum) {
      int i = 0;
      while (eventNum > neg_scaler_event_list->at(i)) {
        ++i;
        if (i > neg_scaler_event_list->size() - 1) {
          i = neg_scaler_event_list->size() - 1;
          break;
        }
      }
      return neg_scaler_current_list->at(i);
      // std::cout<< neg_scaler_current_list->at(i)<<std::endl;
    };
    auto d_neg_run =
      d_neg_raw
      .Filter("fEvtHdr.fEvtType == 4")
      //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
      //.Filter(aeroCutSHMS)
      .Filter(Normal_SHMS)
      .Filter(Normal_HMS)
      .Define("fptime_minus_rf", "P.hod.fpHitsTime - T.coin.pRF_tdcTime")
      .Define("current", neg_get_current, {"fEvtHdr.fEvtNum"})
      //.Filter(HGC_bad)
      .Filter([&](double current) { return current > current_offset; }, {"current"});

    // coin time cut for neg runs
    auto h_cointime_neg =
      d_neg_run.Histo1D({"", "coin_time", 800, 30, 55}, "CTime.ePiCoinTime_ROC2");
    int    coin_peak_bin_neg    = h_cointime_neg->GetMaximumBin();
    double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
    std::cout << "neg coin time peak " << coin_peak_center_neg << std::endl;
    // cointime cut
    double cointime_low_neg  = coin_peak_center_neg + cointime_lowcut;
    double cointime_high_neg = coin_peak_center_neg + cointime_highcut;

    auto d_neg_first = d_neg_run.Filter(
        [cointime_low_neg, cointime_high_neg](double cointime) {
        return cointime > cointime_low_neg && cointime < cointime_high_neg;
        },
        {"CTime.ePiCoinTime_ROC2"});
    auto h_coin_neg = d_neg_run.Histo1D({"", "", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");
    auto h_coin_negcut_rungroup =
      d_neg_first.Histo1D({"", "", 800, 0, 100}, "CTime.ePiCoinTime_ROC2");

    // rftime cut
    // offset
    double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
    std::cout << "offset for neg runs " << offset_neg << std::endl;
    auto d_mod_first =
      d_neg_first
      .Define("diff_time_shift",
          [offset_neg](double difftime) { return difftime + offset_neg; },
          {"fptime_minus_rf"})
    .Define("diff_time_mod", [](double difftime) { return std::fmod(difftime, 4.008); },
        {"diff_time_shift"})
    .Define("rf_k",rftime_K,{"P.gtr.dp","diff_time_mod"}) 
      .Define("rf_proton",rftime_proton,{"P.gtr.dp","diff_time_mod"}) 
      .Define("rf_pi",rftime_pi,{"P.gtr.dp","diff_time_mod"}) 
      //    .Filter(aeroCutSHMS)
      ;

    std::string bg_cut = " ";
    // for bg
    int bg_left_low   = j_cuts["random_bg_left_low"].get<int>();
    int bg_left_high  = j_cuts["random_bg_left_high"].get<int>();
    int bg_right_low  = j_cuts["random_bg_right_low"].get<int>();
    int bg_right_high = j_cuts["random_bg_right_high"].get<int>();
    for (int i = bg_left_low; i < bg_left_high; i = i + 2) {
      double bg_main  = coin_peak_center_neg + i * 4.008;
      double bg_left  = bg_main + cointime_lowcut;
      double bg_right = bg_main + cointime_highcut;
      bg_cut = bg_cut + " (bg_cointime > " + std::to_string(bg_left) + " && bg_cointime < " +
        std::to_string(bg_right) + ") ||";
    }
    for (int i = bg_right_low; i < bg_right_high; i = i + 2) {
      double bg_main  = coin_peak_center_neg + i * 4.008;
      double bg_left  = bg_main + cointime_lowcut;
      double bg_right = bg_main + cointime_highcut;
      bg_cut = bg_cut + " (bg_cointime > " + std::to_string(bg_left) + " && bg_cointime < " +
        std::to_string(bg_right) + ") ||";
    }
    bg_cut = bg_cut.substr(0, bg_cut.size() - 2);
    std::cout << bg_cut << std::endl;

    auto d_neg_bg_norfcut =
      d_neg_run
      .Define("bg_cointime", [](double cointime) { return cointime; },
          {"CTime.ePiCoinTime_ROC2"})
    .Filter(bg_cut)
      .Define("diff_time_shift",
          [offset_neg](double difftime) { return difftime + offset_neg; },
          {"fptime_minus_rf"})
    .Define("diff_time_mod", [](double difftime) { return std::fmod(difftime, 4.008); },
        {"diff_time_shift"})
    .Define("rf_k",rftime_K,{"P.gtr.dp","diff_time_mod"}) 
    .Define("rf_proton",rftime_proton,{"P.gtr.dp","diff_time_mod"}) 
      .Define("rf_pi",rftime_pi,{"P.gtr.dp","diff_time_mod"}) 
      //  .Filter(aeroCutSHMS)
      ;

    auto d_neg_piall = d_mod_first
      //.Filter(aeroCutSHMS)
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_hgc_aero,{"P.gtr.dp","P.hgcer.npeSum","P.aero.npeSum"});
    auto d_neg_Kall = d_mod_first
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_anti_hgc_aero)
      ;
    auto d_neg_piall_bg = d_neg_bg_norfcut
      //.Filter(aeroCutSHMS)
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_hgc_aero,{"P.gtr.dp","P.hgcer.npeSum","P.aero.npeSum"});
    auto d_neg_Kall_bg = d_neg_bg_norfcut
      .Filter(SHMS_p_cut,{"P.gtr.dp"})
      .Filter(SHMS_anti_hgc_aero)
      ;

    // statistics for DE efficiency
    auto h_rf_neg = d_neg_piall.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
    auto h_rf_neg_bg =
      d_neg_piall_bg.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_pi");
    h_rf_neg->Add(h_rf_neg_bg.GetPtr(), -1.0 / 6);
    h_rf_neg_piall->Add(h_rf_neg.GetPtr(), 1);

    auto h_rf_neg_K =
      d_neg_Kall.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_k");
    auto h_rf_neg_K_bg =
      d_neg_Kall_bg.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_k");
    h_rf_neg_K->Add(h_rf_neg_K_bg.GetPtr(), -1.0 / 6);
    h_rf_neg_Kall->Add(h_rf_neg_K.GetPtr(), 1);
    
    auto h_rfpi_neg_K =
      d_neg_Kall.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
    auto h_rfpi_neg_K_bg =
      d_neg_Kall_bg.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_pi");
    h_rfpi_neg_K->Add(h_rfpi_neg_K_bg.GetPtr(), -1.0 / 6);
    h_rfpi_neg_Kall->Add(h_rfpi_neg_K.GetPtr(), 1);
    auto h_rfproton_neg_K =
      d_neg_Kall.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_proton");
    auto h_rfproton_neg_K_bg =
      d_neg_Kall_bg.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_proton");
    h_rfproton_neg_K->Add(h_rfproton_neg_K_bg.GetPtr(), -1.0 / 6);
    h_rfproton_neg_Kall->Add(h_rfproton_neg_K.GetPtr(), 1);

  }

  fout->cd();
  h_rf_pos_piall->Write(std::string("rftime_pos_" + std::to_string(RunGroup)).c_str());
  h_rf_neg_piall->Write(std::string("rftime_neg_" + std::to_string(RunGroup)).c_str());
  h_rf_pos_Kall->Write(std::string("rftime_pos_K_" + std::to_string(RunGroup)).c_str());
  h_rf_neg_Kall->Write(std::string("rftime_neg_K_" + std::to_string(RunGroup)).c_str());
  h_rfpi_pos_Kall->Write(std::string("rftimepi_pos_K_" + std::to_string(RunGroup)).c_str());
  h_rfpi_neg_Kall->Write(std::string("rftimepi_neg_K_" + std::to_string(RunGroup)).c_str());
  h_rfproton_pos_Kall->Write(std::string("rftimeproton_pos_K_" + std::to_string(RunGroup)).c_str());
  h_rfproton_neg_Kall->Write(std::string("rftimeproton_neg_K_" + std::to_string(RunGroup)).c_str());
}   // if normal production runs
}
