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

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;



void SHMS_DE(int RunGroup=0){

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
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_DE;
  }
  std::vector<double> cal_cut_low;
  cal_cut_low = j_DE["SHMS"]["cal_cuts"].get<std::vector<double>>();
  int n_cuts = (int)cal_cut_low.size();
  std::vector<double> rf_cuts;
  rf_cuts = j_DE["SHMS"]["rf_time_cuts"].get<std::vector<double>>();
  std::vector<double> aero_cuts;
  aero_cuts = j_DE["SHMS"]["aero_cuts"].get<std::vector<double>>();

  double H_dp_low = j_cuts["H_dp_low"].get<double>();
  double H_dp_high = j_cuts["H_dp_high"].get<double>();
  double P_dp_low = j_cuts["P_dp_low"].get<double>();
  double P_dp_high = j_cuts["P_dp_high"].get<double>();
  std::string goodTrackSHMS = "P.gtr.dp>"+std::to_string(P_dp_low)+" && P.gtr.dp<"+std::to_string(P_dp_high);
  std::string goodTrackHMS = "H.gtr.dp>"+std::to_string(H_dp_low)+" && H.gtr.dp<"+std::to_string(H_dp_high);
  std::cout<<goodTrackSHMS<<std::endl;
  std::cout<<goodTrackHMS<<std::endl;
  double SHMS_low = j_cuts["P_cal_pi_low"].get<double>();
  double SHMS_high = j_cuts["P_cal_pi_high"].get<double>();
  std::string piCutSHMS = (" P.cal.etottracknorm > "+std::to_string(SHMS_low)+" && P.cal.etottracknorm < " + std::to_string(SHMS_high)).c_str();
  std::cout<<"picutSHMS "<<piCutSHMS<<std::endl;
  double P_aero = j_cuts["P_aero"].get<double>();
  std::string aeroCutSHMS = (" P.aero.npeSum > "+std::to_string(P_aero)).c_str();
  std::cout<<"P_aerocut "<<aeroCutSHMS<<std::endl;
  double H_cal_low = j_cuts["H_cal_low"].get<double>();
  double H_cal_high = j_cuts["H_cal_high"].get<double>();
  double H_cer = j_cuts["H_cer"].get<double>();
  int P_hgcer = j_cuts["P_hgcer"].get<double>();
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


  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;

    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
    }

    ROOT::RDataFrame d_pos_raw("T",files_pos);
    auto d_pos = d_pos_raw
      //.Filter("fEvtHdr.fEvtType == 4")
      //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      //.Filter(piCutSHMS)
      .Filter(eCutHMS)
      //.Filter(aeroCutSHMS)
      .Filter(Normal_SHMS)
      .Filter(Normal_HMS)
      .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
      ;


    //coin time cut for pos runs
    auto h_cointime_pos = d_pos.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
    int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
    double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
    auto d_pos_2ndpeak = d_pos.Filter([=](double cointime){return cointime<coin_peak_center_pos-2.004;},{"CTime.ePiCoinTime_ROC2"});
    auto h_cointime_2nd = d_pos_2ndpeak.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
    int coin_peak_2ndbin_pos = h_cointime_2nd->GetMaximumBin();
    double coin_peak_2nd_pos = h_cointime_2nd->GetBinCenter(coin_peak_2ndbin_pos);
    //if(coin_peak_2nd_pos<coin_peak_center_pos){coin_peak_center_pos = coin_peak_2nd_pos;}
    std::cout<<"pos coin time peak "<<coin_peak_center_pos<<std::endl;
    double cointime_lowcut,cointime_highcut;
    if(RunGroup<410){
      cointime_lowcut = j_cuts["cointime_low_fall"].get<double>();
      cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
    }
    else{
      cointime_lowcut = j_cuts["cointime_low_spring"].get<double>();
      cointime_highcut = j_cuts["cointime_high_spring"].get<double>();
    }
    //cointime cut
    double cointime_low_pos = coin_peak_center_pos+cointime_lowcut;
    double cointime_high_pos = coin_peak_center_pos+cointime_highcut;
    auto d_pos_first = d_pos
      .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"});
    auto h_coin_pos = d_pos.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_coin_poscut_rungroup = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

    //rftime cut
    auto h_time_diff_pos = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
    auto h_time_diff_poscheck = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,-100,100},"fptime_minus_rf");
    int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
    double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
    //offset
    double offset_pos = 401.8-time_diff_pos_max;
    std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
    json j_rfcut;
    {
      std::ifstream ifs("db2/rftime_cut.json");
      ifs>>j_rfcut;
    }
    double rf_pi_low = j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_low"].get<double>();
    std::cout<<rf_pi_low<<std::endl;
    double rf_pi_high =j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_high"].get<double>();
    std::cout<<rf_pi_high<<std::endl;

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
      auto h_pos_current = d_pos_scaler.Histo1D({"pos current","pos current",100,10,100},"P.BCM4B.scalerCurrent");
      double pos_setcurrent = h_pos_current->GetBinCenter(h_pos_current->GetMaximumBin());
      std::cout<<"set current "<<pos_setcurrent<<std::endl;
      //std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size "<<pos_scaler_current_list->size()<<std::endl;
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
        //std::cout<< pos_scaler_current_list->at(i)<<std::endl;

      };
      //std::cout<<"check"<<std::endl;
      auto h_hms_dp_before_pos = d_pos_raw.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_before_pos = d_pos_raw.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_before_pos = d_pos_raw.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_before_pos = d_pos_raw.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_before_pos = d_pos_raw.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_before_pos = d_pos_raw.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        //.Filter(piCutSHMS)
        .Filter(eCutHMS)
        //.Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(current-pos_setcurrent)<3;},{"current"})
        ;

      auto h_current_before_pos = d_pos_run.Histo1D({"","current",100,10,100},"current");
      TCanvas* c_pos_current = new TCanvas("","coin time",2200,1450);
      h_current_before_pos->DrawCopy("hist");
      std::string c_pos_current_name = "results/yield/check/current_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_current->SaveAs(c_pos_current_name.c_str());

      auto d_pos_first = d_pos_run
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;
      std::cout<<"check"<<std::endl;
      auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
      auto d_first_foraero = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(piCutSHMS)
        ;


      // auto d_pos_pi = d_mod_first
      //   .Filter(
      //       [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
      //   ;
      //for coin pion no rf cut, d_pos_first
      std::string bg_cut = " ";  
      //for bg
      int bg_left_low = j_cuts["random_bg_left_low"].get<int>();
      int bg_left_high = j_cuts["random_bg_left_high"].get<int>();
      int bg_right_low = j_cuts["random_bg_right_low"].get<int>();
      int bg_right_high = j_cuts["random_bg_right_high"].get<int>();
      for(int i = bg_left_low;i<bg_left_high;i=i+2){
        double bg_main = coin_peak_center_pos+i*4.008;
        double bg_left = bg_main+cointime_lowcut;
        double bg_right = bg_main+cointime_highcut;
        bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
      }
      for(int i = bg_right_low;i<bg_right_high;i=i+2){
        double bg_main = coin_peak_center_pos+i*4.008;
        double bg_left = bg_main+cointime_lowcut;
        double bg_right = bg_main+cointime_highcut;
        bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
      }
      bg_cut = bg_cut.substr(0,bg_cut.size()-2);
      std::cout<<bg_cut<<std::endl;
      //for bg no rf cut, to use for shms PID efficiency  
      auto d_pos_bg_norfcut = d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;
      auto d_first_foraero_bg = d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(piCutSHMS)
        ;
      //auto d_pos_bg = d_pos_bg_norfcut
      //rfcut for bg
      //   .Filter(
      //       [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
      //   ;

      //I didn't use this quantity later. This is from other script. 
      int pion_counts = *d_mod_first.Count() - *d_pos_bg_norfcut.Count()/6;
      std::cout<<" pos coin counts "<<*d_mod_first.Count()<<" bg counts "<<*d_pos_bg_norfcut.Count()/6<<std::endl;
      auto h_coin_poscut = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_hms_dp_after_pos = d_mod_first.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_pos = d_mod_first.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_pos = d_mod_first.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_pos = d_mod_first.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_after_pos = d_mod_first.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_pos = d_mod_first.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");

      auto h_hms_dp_bg_pos = d_pos_bg_norfcut.Histo1D({"","HMS dp bg",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_bg_pos = d_pos_bg_norfcut.Histo1D({"","SHMS dp bg",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_bg_pos = d_pos_bg_norfcut.Histo1D({"","HMS cal bg",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_bg_pos = d_pos_bg_norfcut.Histo1D({"","HMS cer bg",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_bg_pos = d_pos_bg_norfcut.Histo1D({"","SHMS cal bg",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_bg_pos = d_pos_bg_norfcut.Histo1D({"","SHMS aero bg",200,0,50},"P.aero.npeSum");


      //for cal cuts
      double pos_cal_pi_rfcut_low = j_DE["SHMS"]["rf_time_low"].get<double>();
      double pos_cal_pi_rfcut_high = j_DE["SHMS"]["rf_time_high"].get<double>();
      std::string pos_cal_pi_rfcut = "diff_time_mod > "+std::to_string(pos_cal_pi_rfcut_low)+" && diff_time_mod < "+std::to_string(pos_cal_pi_rfcut_high);
      std::cout<<pos_cal_pi_rfcut<<std::endl;
      // for pi_all 
      auto d_pos_piall  = d_mod_first
        .Filter(pos_cal_pi_rfcut)
        ;
      auto d_pos_piall_bg = d_pos_bg_norfcut
        .Filter(pos_cal_pi_rfcut)
        ;

      //for aero cut
      auto d_first_piall = d_first_foraero
        .Filter(pos_cal_pi_rfcut)
        ;
      auto d_first_piall_bg = d_first_foraero_bg
        .Filter(pos_cal_pi_rfcut)
        ;
      //not really necessary to check this histogram. 
      auto h_coin_pos_piall = d_pos_piall.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_pos_piall_bg = d_pos_piall_bg.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      json jout;
      auto h_cal_pos_norfcut = d_mod_first.Histo1D({"","pos,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_pos_nocalcut = d_mod_first.Histo1D({"","pos,cal,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_cal_pos_norfcut_bg = d_pos_bg_norfcut.Histo1D({"","pos,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_pos_nocalcut_bg = d_pos_bg_norfcut.Histo1D({"","pos,cal,norfcut;rftime,counts",100,0,4},"diff_time_mod");
      h_cal_pos_norfcut->Add(h_cal_pos_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_pos_nocalcut->Add(h_rf_pos_nocalcut_bg.GetPtr(),-1.0/6); 
      auto h_cal_pos_pi = d_pos_piall.Histo1D({"","pos,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_cal_pos_pi_bg = d_pos_piall_bg.Histo1D({"","pos,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      h_cal_pos_pi->Add(h_cal_pos_pi_bg.GetPtr(),-1.0/6);

      auto h_aero_pos_norfcut = d_first_foraero.Histo1D({"","pos,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_pos_noaerocut = d_first_foraero.Histo1D({"","pos,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_aero_pos_norfcut_bg = d_first_foraero_bg.Histo1D({"","pos,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_pos_noaerocut_bg = d_first_foraero_bg.Histo1D({"","pos,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      h_aero_pos_norfcut->Add(h_aero_pos_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_pos_noaerocut->Add(h_rf_pos_noaerocut_bg.GetPtr(),-1.0/6); 
      auto h_aero_pos_pi = d_first_piall.Histo1D({"","pos,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      auto h_aero_pos_pi_bg = d_first_piall_bg.Histo1D({"","pos,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      h_aero_pos_pi->Add(h_aero_pos_pi_bg.GetPtr(),-1.0/6);

      int n_pos_pi_rfcut = h_cal_pos_pi->GetEntries();
      // *d_pos_eall.Count() - *d_pos_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_all"] = n_pos_pi_rfcut;

      int n_pos_pi_rfcalcut = h_aero_pos_pi->GetEntries();
      // *d_pos_eall.Count() - *d_pos_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["pi_all"] = n_pos_pi_rfcalcut;

      std::vector<double> n_pos_pi_cal,n_pos_pi_aero;
      for(int i = 0;i< n_cuts;++i){
        TAxis *axis_pos_pi_cal = h_cal_pos_pi->GetXaxis();
        int bmin_pos_pi_cal = axis_pos_pi_cal->FindBin(SHMS_low)+1;
        int bmax_pos_pi_cal = axis_pos_pi_cal->FindBin(cal_cut_low[i])-1;
        double n_pos_pi_rfcut_cal = (double)h_cal_pos_pi->Integral(bmin_pos_pi_cal,bmax_pos_pi_cal);
        n_pos_pi_cal.push_back(n_pos_pi_rfcut_cal);
      }
      for(int i = 0;i<aero_cuts.size();++i){
        TAxis *axis_pos_pi_aero = h_aero_pos_pi->GetXaxis();
        int bmin_pos_pi_aero = axis_pos_pi_aero->FindBin(aero_cuts[i])+1;
        int bmax_pos_pi_aero = axis_pos_pi_aero->GetLast();
        double n_pos_pi_rfcut_aero = (double)h_aero_pos_pi->Integral(bmin_pos_pi_aero,bmax_pos_pi_aero);
        n_pos_pi_aero.push_back(n_pos_pi_rfcut_aero);
      }
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["calcuts"] = cal_cut_low;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_calcuts"] = n_pos_pi_cal;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["aero_cuts"] = aero_cuts;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["pi_aerocuts"] = n_pos_pi_aero;
      std::ofstream outfile;
      std::string outfile_name = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;

      TCanvas* c_DE_cal = new TCanvas();
      c_DE_cal->Divide(2,1);
      c_DE_cal->cd(1);
      TVirtualPad* Pad_rf_calcuts = c_DE_cal->cd(1);
      //Pad_rf_calcuts->SetLogy();
      h_rf_pos_nocalcut->SetBit(TH1::kNoStats);
      h_rf_pos_nocalcut->SetMarkerStyle(8);
      h_rf_pos_nocalcut->DrawCopy("hist");
      TLine* line_pi_rf_low = new TLine(pos_cal_pi_rfcut_low,0,pos_cal_pi_rfcut_low,1000);
      TLine* line_pi_rf_high = new TLine(pos_cal_pi_rfcut_high,0,pos_cal_pi_rfcut_high,1000);
      //line_e_rf->SetLineColor(kRed);
      line_pi_rf_low->SetLineColor(kRed);
      line_pi_rf_low->Draw("same");
      line_pi_rf_high->SetLineColor(kRed);
      line_pi_rf_high->Draw("same");
      c_DE_cal->cd(2);
      TVirtualPad* Pad_cal_calcuts = c_DE_cal->cd(2);
      Pad_cal_calcuts->SetLogy();
      h_cal_pos_norfcut->SetBit(TH1::kNoStats);
      h_cal_pos_norfcut->SetMarkerStyle(8);
      h_cal_pos_norfcut->DrawCopy();
      h_cal_pos_pi->SetBit(TH1::kNoStats);
      h_cal_pos_pi->SetLineColor(kRed);
      h_cal_pos_pi->DrawCopy("hist same");
      Pad_cal_calcuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_cal_name = "results/pid/SHMS_cal_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());

      TCanvas* c_DE_aero = new TCanvas();
      c_DE_aero->Divide(2,1);
      c_DE_aero->cd(1);
      TVirtualPad* Pad_rf_aerocuts = c_DE_aero->cd(1);
      //Pad_rf_aerocuts->SetLogy();
      h_rf_pos_noaerocut->SetBit(TH1::kNoStats);
      h_rf_pos_noaerocut->SetMarkerStyle(8);
      h_rf_pos_noaerocut->DrawCopy("hist");
      //line_e_rf->SetLineColor(kRed);
      line_pi_rf_low->SetLineColor(kRed);
      line_pi_rf_low->Draw("same");
      line_pi_rf_high->SetLineColor(kRed);
      line_pi_rf_high->Draw("same");
      c_DE_aero->cd(2);
      TVirtualPad* Pad_aero_aerocuts = c_DE_aero->cd(2);
      Pad_aero_aerocuts->SetLogy();
      h_aero_pos_norfcut->SetBit(TH1::kNoStats);
      h_aero_pos_norfcut->SetMarkerStyle(8);
      h_aero_pos_norfcut->DrawCopy();
      h_aero_pos_pi->SetBit(TH1::kNoStats);
      h_aero_pos_pi->SetLineColor(kRed);
      h_aero_pos_pi->DrawCopy("hist same");
      Pad_aero_aerocuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_aero_name = "results/pid/SHMS_aero_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_aero->SaveAs(c_DE_aero_name.c_str());

      //some check plots
      TCanvas* c_pos_cointime = new TCanvas("","coin time",2200,1450);
      c_pos_cointime->Divide(2,1);
      c_pos_cointime->cd(1);
      h_coin_poscut_rungroup->DrawCopy("hist");
      //h_coin_pos->DrawCopy("hist");
      //h_coin_poscut->SetLineColor(kRed);
      c_pos_cointime->cd(2);
      h_coin_poscut->DrawCopy("hist");
      h_coin_pos_piall->SetLineColor(kRed);
      h_coin_pos_piall->DrawCopy("hist same");
      h_coin_pos_piall_bg->SetLineColor(kBlue);
      h_coin_pos_piall_bg->DrawCopy("hist same");
      std::string c_pos_cointime_name = "results/pid/check/cointime_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_cointime->SaveAs(c_pos_cointime_name.c_str());
      TCanvas* c_pos_hms_dp = new TCanvas("","HMS",2200,1450);
      //h_hms_dp_before_pos->DrawCopy("hist");
      h_hms_dp_after_pos->SetLineColor(kRed);
      h_hms_dp_after_pos->DrawCopy("hist");
      h_hms_dp_bg_pos->SetLineColor(kBlue);
      h_hms_dp_bg_pos->Scale(1.0/6);
      h_hms_dp_bg_pos->DrawCopy("hist same");
      std::string c_pos_hms_dp_name = "results/pid/check/hms_dp_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_hms_dp->SaveAs(c_pos_hms_dp_name.c_str());      
      TCanvas* c_pos_shms_dp = new TCanvas("","shms",2200,1450);
      //h_shms_dp_before_pos->DrawCopy("hist");
      h_shms_dp_after_pos->SetLineColor(kRed);
      h_shms_dp_after_pos->DrawCopy("hist");
      h_shms_dp_bg_pos->SetLineColor(kBlue);
      h_shms_dp_bg_pos->Scale(1.0/6);
      h_shms_dp_bg_pos->DrawCopy("hist same");
      std::string c_pos_shms_dp_name = "results/pid/check/shms_dp_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_shms_dp->SaveAs(c_pos_shms_dp_name.c_str());      
      TCanvas* c_pos_hms_cal = new TCanvas("","HMS",2200,1450);
      c_pos_hms_cal->SetLogy();
      //h_hms_cal_before_pos->DrawCopy("hist");
      h_hms_cal_after_pos->SetLineColor(kRed);
      h_hms_cal_after_pos->DrawCopy("hist");
      h_hms_cal_bg_pos->SetLineColor(kBlue);
      h_hms_cal_bg_pos->Scale(1.0/6);
      h_hms_cal_bg_pos->DrawCopy("hist same");
      std::string c_pos_hms_cal_name = "results/pid/check/hms_cal_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_hms_cal->SaveAs(c_pos_hms_cal_name.c_str());      
      TCanvas* c_pos_hms_cer = new TCanvas("","HMS",2200,1450);
      c_pos_hms_cer->SetLogy();
      //h_hms_cer_before_pos->DrawCopy("hist");
      h_hms_cer_after_pos->SetLineColor(kRed);
      h_hms_cer_after_pos->DrawCopy("hist");
      h_hms_cer_bg_pos->SetLineColor(kBlue);
      h_hms_cer_bg_pos->Scale(1.0/6);
      h_hms_cer_bg_pos->DrawCopy("hist same");
      std::string c_pos_hms_cer_name = "results/pid/check/hms_cer_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_hms_cer->SaveAs(c_pos_hms_cer_name.c_str());      
      TCanvas* c_pos_shms_cal = new TCanvas("","shms",2200,1450);
      c_pos_shms_cal->SetLogy();
      //h_shms_cal_before_pos->DrawCopy("hist");
      h_shms_cal_after_pos->SetLineColor(kRed);
      h_shms_cal_after_pos->DrawCopy("hist");
      h_shms_cal_bg_pos->SetLineColor(kBlue);
      h_shms_cal_bg_pos->Scale(1.0/6);
      h_shms_cal_bg_pos->DrawCopy("hist same");
      std::string c_pos_shms_cal_name = "results/pid/check/shms_cal_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_shms_cal->SaveAs(c_pos_shms_cal_name.c_str());      
      TCanvas* c_pos_shms_aero = new TCanvas("","shms",2200,1450);
      c_pos_shms_aero->SetLogy();
      //h_shms_aero_before_pos->DrawCopy("hist");
      h_shms_aero_after_pos->SetLineColor(kRed);
      h_shms_aero_after_pos->DrawCopy("hist");
      h_shms_aero_bg_pos->SetLineColor(kBlue);
      h_shms_aero_bg_pos->Scale(1.0/6);
      h_shms_aero_bg_pos->DrawCopy("hist same");
      std::string c_pos_shms_aero_name = "results/pid/check/shms_aero_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_shms_aero->SaveAs(c_pos_shms_aero_name.c_str());      

      TCanvas* c_pos_time_diff = new TCanvas("","time_diff",2200,1450);
      c_pos_time_diff->Divide(2,2);
      c_pos_time_diff->cd(1);
      h_time_diff_poscheck->DrawCopy("hist");
      c_pos_time_diff->cd(2);
      h_time_diff_pos->DrawCopy("hist");
      c_pos_time_diff->cd(3);
      h_diff_mod_pos->DrawCopy("hist");
      std::string c_pos_time_diff_name = "results/pid/check/time_diff_"+std::to_string(RunNumber)+"_pos.png";
      c_pos_time_diff->SaveAs(c_pos_time_diff_name.c_str());
      //========finish check plots

    }

    //for neg runs, updated version in new script
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
    }

    //for neg runs
    ROOT::RDataFrame d_neg_raw("T",files_neg);
    auto d_neg = d_neg_raw
      .Filter("fEvtHdr.fEvtType == 4")
      //  .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      //  .Filter(piCutSHMS)
      .Filter(eCutHMS)
      //.Filter(aeroCutSHMS)
      .Filter(Normal_SHMS)
      .Filter(Normal_HMS)
      .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
      ;
    //coin time cut for neg
    auto h_cointime_neg = d_neg.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
    auto h_coin_neg = d_neg.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");
    int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
    double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
    std::cout<<"neg coin time peak "<<coin_peak_center_neg<<std::endl;
    //cointime cut
    double cointime_low_neg = coin_peak_center_neg+cointime_lowcut;
    double cointime_high_neg = coin_peak_center_neg+cointime_highcut;
    // auto neg_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_neg)<2;};
    auto d_neg_coin = d_neg
      .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"});
    //.Filter(neg_peak,{"CTime.ePiCoinTime_ROC2"})
    ;
    auto h_coin_negcut_rungroup = d_neg_coin.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");

    //rftime cut for neg
    auto h_time_diff_neg = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
    auto h_time_diff_negcheck = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,-100,100},"fptime_minus_rf");
    int time_diff_neg_bin_max = h_time_diff_neg->GetMaximumBin();
    double time_diff_neg_max = h_time_diff_neg->GetBinCenter(time_diff_neg_bin_max);
    double offset_neg = 401.8-time_diff_neg_max;
    std::cout<<"offset for neg runs "<<offset_neg<<std::endl;

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
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,10,100},"P.BCM4B.scalerCurrent");
      double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());
      std::cout<<"set current "<<neg_setcurrent<<std::endl;
      //std::cout<<"event size "<<neg_scaler_event_list->size()<<" current size "<<neg_scaler_current_list->size()<<std::endl;
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
        //std::cout<< neg_scaler_current_list->at(i)<<std::endl;

      };
      //std::cout<<"check"<<std::endl;
      auto h_hms_dp_before_neg = d_neg_raw.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_before_neg = d_neg_raw.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_before_neg = d_neg_raw.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_before_neg = d_neg_raw.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_before_neg = d_neg_raw.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_before_neg = d_neg_raw.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");
      auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        //.Filter(piCutSHMS)
        .Filter(eCutHMS)
        //.Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(current-neg_setcurrent)<3;},{"current"})
        ;

      auto h_current_before_neg = d_neg_run.Histo1D({"","current",100,10,100},"current");
      TCanvas* c_neg_current = new TCanvas("","coin time",2200,1450);
      h_current_before_neg->DrawCopy("hist");
      std::string c_neg_current_name = "results/yield/check/current_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_current->SaveAs(c_neg_current_name.c_str());

      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto d_mod_first = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;

      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");

      //double cal_pi_low = j_DE 
      auto d_first_foraero = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(piCutSHMS)
        ;

      // auto d_neg_pi = d_mod_first
      //   .Filter(
      //       [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
      //   ;
      //for coin pion no rf cut, d_neg_first
      std::string bg_cut = " ";  
      //for bg
      int bg_left_low = j_cuts["random_bg_left_low"].get<int>();
      int bg_left_high = j_cuts["random_bg_left_high"].get<int>();
      int bg_right_low = j_cuts["random_bg_right_low"].get<int>();
      int bg_right_high = j_cuts["random_bg_right_high"].get<int>();
      for(int i = bg_left_low;i<bg_left_high;i=i+2){
        double bg_main = coin_peak_center_neg+i*4.008;
        double bg_left = bg_main+cointime_lowcut;
        double bg_right = bg_main+cointime_highcut;
        bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
      }
      for(int i = bg_right_low;i<bg_right_high;i=i+2){
        double bg_main = coin_peak_center_neg+i*4.008;
        double bg_left = bg_main+cointime_lowcut;
        double bg_right = bg_main+cointime_highcut;
        bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
      }
      bg_cut = bg_cut.substr(0,bg_cut.size()-2);
      std::cout<<bg_cut<<std::endl;
      //for bg no rf cut, to use for shms PID efficiency  
      auto d_neg_bg_norfcut = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;
      auto d_first_foraero_bg = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(piCutSHMS)
        ;
      //auto d_neg_bg = d_neg_bg_norfcut
      //rfcut for bg
      //   .Filter(
      //       [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
      //   ;

      //I didn't use this quantity later. This is from other script. 
      int pion_counts = *d_mod_first.Count() - *d_neg_bg_norfcut.Count()/6;
      std::cout<<" neg coin counts "<<*d_mod_first.Count()<<" bg counts "<<*d_neg_bg_norfcut.Count()/6<<std::endl;
      auto h_coin_negcut = d_neg_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_hms_dp_after_neg = d_mod_first.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_neg = d_mod_first.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_neg = d_mod_first.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_neg = d_mod_first.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_after_neg = d_mod_first.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_neg = d_mod_first.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");

      auto h_hms_dp_bg_neg = d_neg_bg_norfcut.Histo1D({"","HMS dp bg",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_bg_neg = d_neg_bg_norfcut.Histo1D({"","SHMS dp bg",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_bg_neg = d_neg_bg_norfcut.Histo1D({"","HMS cal bg",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_bg_neg = d_neg_bg_norfcut.Histo1D({"","HMS cer bg",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_bg_neg = d_neg_bg_norfcut.Histo1D({"","SHMS cal bg",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_bg_neg = d_neg_bg_norfcut.Histo1D({"","SHMS aero bg",200,0,50},"P.aero.npeSum");


      //for cal cuts
      double neg_cal_pi_rfcut_low = j_DE["SHMS"]["rf_time_low"].get<double>();
      double neg_cal_pi_rfcut_high = j_DE["SHMS"]["rf_time_high"].get<double>();
      std::string neg_cal_pi_rfcut = "diff_time_mod > "+std::to_string(neg_cal_pi_rfcut_low)+" && diff_time_mod < "+std::to_string(neg_cal_pi_rfcut_high);
      std::cout<<neg_cal_pi_rfcut<<std::endl;
      // for pi_all 
      auto d_neg_piall  = d_mod_first
        .Filter(neg_cal_pi_rfcut)
        ;
      auto d_neg_piall_bg = d_neg_bg_norfcut
        .Filter(neg_cal_pi_rfcut)
        ;

      //for aero cut
      auto d_first_piall = d_first_foraero
        .Filter(neg_cal_pi_rfcut)
        ;
      auto d_first_piall_bg = d_first_foraero_bg
        .Filter(neg_cal_pi_rfcut)
        ;

      //not really necessary to check this histogram. 
      auto h_coin_neg_piall = d_neg_piall.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_neg_piall_bg = d_neg_piall_bg.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      json jout;
      auto h_cal_neg_norfcut = d_mod_first.Histo1D({"","neg,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_neg_nocalcut = d_mod_first.Histo1D({"","neg,rftime,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_cal_neg_norfcut_bg = d_neg_bg_norfcut.Histo1D({"","neg,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_neg_nocalcut_bg = d_neg_bg_norfcut.Histo1D({"","neg,cal,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      h_cal_neg_norfcut->Add(h_cal_neg_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_neg_nocalcut->Add(h_rf_neg_nocalcut_bg.GetPtr(),-1.0/6); 
      auto h_cal_neg_pi = d_neg_piall.Histo1D({"","neg,cal,pi_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_cal_neg_pi_bg = d_neg_piall_bg.Histo1D({"","neg,cal,pi_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      h_cal_neg_pi->Add(h_cal_neg_pi_bg.GetPtr(),-1.0/6);

      auto h_aero_neg_norfcut = d_first_foraero.Histo1D({"","neg,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_neg_noaerocut = d_first_foraero.Histo1D({"","neg,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_aero_neg_norfcut_bg = d_first_foraero_bg.Histo1D({"","neg,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_neg_noaerocut_bg = d_first_foraero_bg.Histo1D({"","neg,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      h_aero_neg_norfcut->Add(h_aero_neg_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_neg_noaerocut->Add(h_rf_neg_noaerocut_bg.GetPtr(),-1.0/6); 
      auto h_aero_neg_pi = d_first_piall.Histo1D({"","neg,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      auto h_aero_neg_pi_bg = d_first_piall_bg.Histo1D({"","neg,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      h_aero_neg_pi->Add(h_aero_neg_pi_bg.GetPtr(),-1.0/6);

      int n_neg_pi_rfcut = h_cal_neg_pi->GetEntries();
      // *d_neg_eall.Count() - *d_neg_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_all"] = n_neg_pi_rfcut;

      int n_neg_pi_rfcalcut = h_aero_neg_pi->GetEntries();
      // *d_neg_eall.Count() - *d_neg_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["pi_all"] = n_neg_pi_rfcalcut;

      std::vector<double> n_neg_pi_cal,n_neg_pi_aero;
      for(int i = 0;i< n_cuts;++i){
        TAxis *axis_neg_pi_cal = h_cal_neg_pi->GetXaxis();
        int bmin_neg_pi_cal = axis_neg_pi_cal->FindBin(SHMS_low)+1;
        int bmax_neg_pi_cal = axis_neg_pi_cal->FindBin(cal_cut_low[i])-1;
        double n_neg_pi_rfcut_cal = (double)h_cal_neg_pi->Integral(bmin_neg_pi_cal,bmax_neg_pi_cal);
        n_neg_pi_cal.push_back(n_neg_pi_rfcut_cal);
      }
      for(int i = 0;i<aero_cuts.size();++i){
        TAxis *axis_neg_pi_aero = h_aero_neg_pi->GetXaxis();
        int bmin_neg_pi_aero = axis_neg_pi_aero->FindBin(aero_cuts[i])+1;
        int bmax_neg_pi_aero = axis_neg_pi_aero->GetLast();
        double n_neg_pi_rfcut_aero = (double)h_aero_neg_pi->Integral(bmin_neg_pi_aero,bmax_neg_pi_aero);
        n_neg_pi_aero.push_back(n_neg_pi_rfcut_aero);
      }
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["calcuts"] = cal_cut_low;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_calcuts"] = n_neg_pi_cal;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["aero_cuts"] = aero_cuts;
      jout[std::to_string(RunNumber)]["SHMS"]["rfcutcalcut"]["pi_aerocuts"] = n_neg_pi_aero;

      std::ofstream outfile;
      std::string outfile_name = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;

      TCanvas* c_DE_cal = new TCanvas();
      c_DE_cal->Divide(2,1);
      c_DE_cal->cd(1);
      TVirtualPad* Pad_rf_calcuts = c_DE_cal->cd(1);
      //Pad_rf_calcuts->SetLogy();
      h_rf_neg_nocalcut->SetBit(TH1::kNoStats);
      h_rf_neg_nocalcut->SetMarkerStyle(8);
      h_rf_neg_nocalcut->DrawCopy("hist");
      TLine* line_pi_rf_low = new TLine(neg_cal_pi_rfcut_low,0,neg_cal_pi_rfcut_low,1000);
      TLine* line_pi_rf_high = new TLine(neg_cal_pi_rfcut_high,0,neg_cal_pi_rfcut_high,1000);
      //line_e_rf->SetLineColor(kRed);
      line_pi_rf_low->SetLineColor(kRed);
      line_pi_rf_low->Draw("same");
      line_pi_rf_high->SetLineColor(kRed);
      line_pi_rf_high->Draw("same");
      c_DE_cal->cd(2);
      TVirtualPad* Pad_cal_calcuts = c_DE_cal->cd(2);
      Pad_cal_calcuts->SetLogy();
      h_cal_neg_norfcut->SetBit(TH1::kNoStats);
      h_cal_neg_norfcut->SetMarkerStyle(8);
      h_cal_neg_norfcut->DrawCopy();
      h_cal_neg_pi->SetBit(TH1::kNoStats);
      h_cal_neg_pi->SetLineColor(kRed);
      h_cal_neg_pi->DrawCopy("hist same");
      Pad_cal_calcuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_cal_name = "results/pid/SHMS_cal_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());


      TCanvas* c_DE_aero = new TCanvas();
      c_DE_aero->Divide(2,1);
      c_DE_aero->cd(1);
      TVirtualPad* Pad_rf_aerocuts = c_DE_aero->cd(1);
      //Pad_rf_aerocuts->SetLogy();
      h_rf_neg_noaerocut->SetBit(TH1::kNoStats);
      h_rf_neg_noaerocut->SetMarkerStyle(8);
      h_rf_neg_noaerocut->DrawCopy("hist");
      //line_e_rf->SetLineColor(kRed);
      line_pi_rf_low->SetLineColor(kRed);
      line_pi_rf_low->Draw("same");
      line_pi_rf_high->SetLineColor(kRed);
      line_pi_rf_high->Draw("same");
      c_DE_aero->cd(2);
      TVirtualPad* Pad_aero_aerocuts = c_DE_aero->cd(2);
      Pad_aero_aerocuts->SetLogy();
      h_aero_neg_norfcut->SetBit(TH1::kNoStats);
      h_aero_neg_norfcut->SetMarkerStyle(8);
      h_aero_neg_norfcut->DrawCopy();
      h_aero_neg_pi->SetBit(TH1::kNoStats);
      h_aero_neg_pi->SetLineColor(kRed);
      h_aero_neg_pi->DrawCopy("hist same");
      Pad_aero_aerocuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_aero_name = "results/pid/SHMS_aero_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_aero->SaveAs(c_DE_aero_name.c_str());


      //some check plots
      TCanvas* c_neg_cointime = new TCanvas("","coin time",2200,1450);
      c_neg_cointime->Divide(2,1);
      c_neg_cointime->cd(1);
      h_coin_negcut_rungroup->DrawCopy("hist");
      //h_coin_neg->DrawCopy("hist");
      //h_coin_negcut->SetLineColor(kRed);
      c_neg_cointime->cd(2);
      h_coin_negcut->DrawCopy("hist");
      h_coin_neg_piall->SetLineColor(kRed);
      h_coin_neg_piall->DrawCopy("hist same");
      h_coin_neg_piall_bg->SetLineColor(kBlue);
      h_coin_neg_piall_bg->DrawCopy("hist same");
      std::string c_neg_cointime_name = "results/pid/check/SHMS/cointime_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_cointime->SaveAs(c_neg_cointime_name.c_str());
      TCanvas* c_neg_hms_dp = new TCanvas("","HMS",2200,1450);
      //h_hms_dp_before_neg->DrawCopy("hist");
      h_hms_dp_after_neg->SetLineColor(kRed);
      h_hms_dp_after_neg->DrawCopy("hist");
      h_hms_dp_bg_neg->SetLineColor(kBlue);
      h_hms_dp_bg_neg->Scale(1.0/6);
      h_hms_dp_bg_neg->DrawCopy("hist same");
      std::string c_neg_hms_dp_name = "results/pid/check/SHMS/hms_dp_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_hms_dp->SaveAs(c_neg_hms_dp_name.c_str());      
      TCanvas* c_neg_shms_dp = new TCanvas("","shms",2200,1450);
      //h_shms_dp_before_neg->DrawCopy("hist");
      h_shms_dp_after_neg->SetLineColor(kRed);
      h_shms_dp_after_neg->DrawCopy("hist");
      h_shms_dp_bg_neg->SetLineColor(kBlue);
      h_shms_dp_bg_neg->Scale(1.0/6);
      h_shms_dp_bg_neg->DrawCopy("hist same");
      std::string c_neg_shms_dp_name = "results/pid/check/SHMS/shms_dp_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_shms_dp->SaveAs(c_neg_shms_dp_name.c_str());      
      TCanvas* c_neg_hms_cal = new TCanvas("","HMS",2200,1450);
      c_neg_hms_cal->SetLogy();
      //h_hms_cal_before_neg->DrawCopy("hist");
      h_hms_cal_after_neg->SetLineColor(kRed);
      h_hms_cal_after_neg->DrawCopy("hist");
      h_hms_cal_bg_neg->SetLineColor(kBlue);
      h_hms_cal_bg_neg->Scale(1.0/6);
      h_hms_cal_bg_neg->DrawCopy("hist same");
      std::string c_neg_hms_cal_name = "results/pid/check/SHMS/hms_cal_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_hms_cal->SaveAs(c_neg_hms_cal_name.c_str());      
      TCanvas* c_neg_hms_cer = new TCanvas("","HMS",2200,1450);
      c_neg_hms_cer->SetLogy();
      //h_hms_cer_before_neg->DrawCopy("hist");
      h_hms_cer_after_neg->SetLineColor(kRed);
      h_hms_cer_after_neg->DrawCopy("hist");
      h_hms_cer_bg_neg->SetLineColor(kBlue);
      h_hms_cer_bg_neg->Scale(1.0/6);
      h_hms_cer_bg_neg->DrawCopy("hist same");
      std::string c_neg_hms_cer_name = "results/pid/check/SHMS/hms_cer_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_hms_cer->SaveAs(c_neg_hms_cer_name.c_str());      
      TCanvas* c_neg_shms_cal = new TCanvas("","shms",2200,1450);
      c_neg_shms_cal->SetLogy();
      //h_shms_cal_before_neg->DrawCopy("hist");
      h_shms_cal_after_neg->SetLineColor(kRed);
      h_shms_cal_after_neg->DrawCopy("hist");
      h_shms_cal_bg_neg->SetLineColor(kBlue);
      h_shms_cal_bg_neg->Scale(1.0/6);
      h_shms_cal_bg_neg->DrawCopy("hist same");
      std::string c_neg_shms_cal_name = "results/pid/check/SHMS/shms_cal_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_shms_cal->SaveAs(c_neg_shms_cal_name.c_str());      
      TCanvas* c_neg_shms_aero = new TCanvas("","shms",2200,1450);
      c_neg_shms_aero->SetLogy();
      //h_shms_aero_before_neg->DrawCopy("hist");
      h_shms_aero_after_neg->SetLineColor(kRed);
      h_shms_aero_after_neg->DrawCopy("hist");
      h_shms_aero_bg_neg->SetLineColor(kBlue);
      h_shms_aero_bg_neg->Scale(1.0/6);
      h_shms_aero_bg_neg->DrawCopy("hist same");
      std::string c_neg_shms_aero_name = "results/pid/check/SHMS/shms_aero_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_shms_aero->SaveAs(c_neg_shms_aero_name.c_str());      

      TCanvas* c_neg_time_diff = new TCanvas("","time_diff",2200,1450);
      c_neg_time_diff->Divide(2,2);
      c_neg_time_diff->cd(1);
      h_time_diff_negcheck->DrawCopy("hist");
      c_neg_time_diff->cd(2);
      h_time_diff_neg->DrawCopy("hist");
      c_neg_time_diff->cd(3);
      h_diff_mod_neg->DrawCopy("hist");
      std::string c_neg_time_diff_name = "results/pid/check/SHMS/time_diff_"+std::to_string(RunNumber)+"_neg.png";
      c_neg_time_diff->SaveAs(c_neg_time_diff_name.c_str());
      //========finish check plots

    }

  }//if normal production runs

}
