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



void HMS_DE_clean(int RunGroup=0){

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
  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runsinfo;
  }
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_DE;
  }

  double cal_cut_low_use = j_cuts["H_cal_low"].get<double>();
  double cal_cut_high_use = j_cuts["H_cal_high"].get<double>();
  std::string cal_cut_use = "H.cal.etottracknorm > "+std::to_string(cal_cut_low_use)+" && H.cal.etottracknorm < "+std::to_string(cal_cut_high_use);

  double H_dp_low = j_cuts["H_dp_low"].get<double>();
  double H_dp_high = j_cuts["H_dp_high"].get<double>();
  double P_dp_low = j_cuts["P_dp_low"].get<double>();
  double P_dp_high = j_cuts["P_dp_high"].get<double>();
  std::string goodTrackSHMS = "P.gtr.dp>"+std::to_string(P_dp_low)+" && P.gtr.dp<"+std::to_string(P_dp_high);
  std::string goodTrackHMS = "H.gtr.dp>"+std::to_string(H_dp_low)+" && H.gtr.dp<"+std::to_string(H_dp_high);
  std::cout<<goodTrackSHMS<<std::endl;
  std::cout<<goodTrackHMS<<std::endl;
  //use strict cut
  double SHMS_low = j_DE["SHMS"]["cal_pi_low"].get<double>();
  double SHMS_high = j_DE["SHMS"]["cal_pi_high"].get<double>();
  std::string piCutSHMS = (" P.cal.etracknorm > "+std::to_string(SHMS_low)+" && P.cal.etracknorm < " + std::to_string(SHMS_high)).c_str();
  std::cout<<"picutSHMS "<<piCutSHMS<<std::endl;
  double P_aero = j_DE["SHMS"]["aero_n"].get<double>();
  std::string aeroCutSHMS = (" P.aero.npeSum > "+std::to_string(P_aero)).c_str();
  std::cout<<"P_aerocut "<<aeroCutSHMS<<std::endl;
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
  double current_offset = j_cuts["current_diff"].get<double>();
  std::cout<<Normal_HMS<<std::endl;
  std::cout<<Normal_SHMS<<std::endl;


  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    //double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    //auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};
    //if(SHMS_P>3.2){aeroCutSHMS = aeroCutSHMS + " && P.hgcer.npeSum > "+(std::to_string(P_hgcer)).c_str();}

    //double Eb;
    //if(RunGroup < 420) {Eb = 10.597;}
    //else{Eb = 10.214;}
    //auto p_proton = [](double px, double py, double pz){
    //  return Pvec4D{px , py , pz , M_P};
    //};
    //auto p_pion = [](double px, double py, double pz) {
    //  return Pvec4D{px , py , pz , M_pion};
    //};
    //auto pion_momentum = [](double px,double py,double pz){
    //  TVector3 v(px,py,pz);
    //  return v;
    //};
    //auto p_electron = [](double px, double py, double pz) {
    //  return Pvec4D{px , py , pz , M_e};
    //};
    //auto p_q = [&](Pvec4D& pe ) {
    //  return Pvec4D{0.0,0.0,Eb, M_e}-pe;
    //};
    //auto t = [](const double Egamma, Pvec4D& jpsi) {
    //  Pvec4D beam{0, 0, Egamma, 0};
    //  return (beam - jpsi).M2();
    //};
    //auto z = [](Pvec4D& pq, Pvec4D& ph) {
    //  return ph.E()/pq.E();
    //};
    //auto xbj = [](double Q2,Pvec4D& pq) {
    //  return Q2/(2.0*0.938*pq.E());
    //};
    //auto Q2 = [](Pvec4D& pq) {
    //  return -1.0*(pq.Dot(pq));
    //};
    //auto Wprime2 = [](Pvec4D& pq,Pvec4D& ph) {
    //  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq - ph;
    //  return Ptot.Dot(Ptot);
    //};
    //auto W2 = [](Pvec4D& pq) {
    //  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq;
    //  return Ptot.Dot(Ptot);
    //};
    //auto Emiss = [&](Pvec4D& p_pion, Pvec4D p_electron){
    //  return Eb+M_P-sqrt(p_pion.E()*p_pion.E())-sqrt(p_electron.E()*p_electron.E());
    //};
    //auto mmiss = [&](Pvec4D& p_pion,Pvec4D p_electron){
    //  Pvec4D missing_mass = Pvec4D{0.0,0.0,Eb,M_e}+Pvec4D{0.0,0.0,0.0,M_P}-p_electron-p_pion;
    //  return std::sqrt(missing_mass.M2());
    //};


    //rftime cut
    double rf_pi_low = j_DE["SHMS"]["rf_time_low"].get<double>();
    std::cout<<rf_pi_low<<std::endl;
    double rf_pi_high =j_DE["SHMS"]["rf_time_high"].get<double>();
    std::cout<<rf_pi_high<<std::endl;

    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);
      ROOT::RDataFrame d_pos_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto pos_scaler_current_list = d_pos_scaler.Take<double>("P.BCM1.scalerCurrent");
      auto pos_scaler_event_list = d_pos_scaler.Take<double>("evNumber");
      auto h_pos_current = d_pos_scaler.Histo1D({"pos current","pos current",100,3,100},"P.BCM1.scalerCurrent");
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
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        //.Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      
      //coin time cut for pos runs
      auto h_cointime_pos = d_pos_run.Histo1D({"","coin_time",800,40,46},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
      double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
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

      auto d_pos_first = d_pos_run
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto d_pos_pi = d_mod_first
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        //.Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        //.Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        //.Define("p_q", p_q, {"p_electron"})
        //.Define("z", z, {"p_q","p_pion"})
        //.Define("Q2", Q2, {"p_q"})
        //.Define("xbj", xbj, {"Q2","p_q"})
        //.Define("W2", W2, {"p_q"})
        //.Define("Wp2", Wprime2, {"p_q","p_pion"})
        //.Define("W", "std::sqrt(W2)")
        //.Define("Wp", "std::sqrt(Wp2)")
        //.Define("InvMass","p_electron.Dot(p_pion)")

        ////.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
        //.Define("emiss",Emiss,{"p_pion","p_electron"})
        //.Define("mmiss",mmiss,{"p_pion","p_electron"})
        ////.Snapshot("T",skim_name.c_str());
        //.Define("pmiss","P.kin.secondary.pmiss")
        ;

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
      auto d_pos_bg = d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        ;

      //I didn't use this quantity later. This is from other script. 
      int pion_counts = *d_pos_pi.Count() - *d_pos_bg.Count()/6;
      std::cout<<" pos coin counts "<<*d_pos_pi.Count()<<" bg counts "<<*d_pos_bg.Count()/6<<std::endl;

      //for cal cuts
      int pos_cal_e_cercut_n = j_DE["HMS"]["cer_e"].get<int>();
      std::string pos_cal_e_cercut = "H.cer.npeSum > "+std::to_string(pos_cal_e_cercut_n);
      std::cout<<pos_cal_e_cercut<<std::endl;
      // for e_all 
      auto d_pos_eall  = d_pos_pi
        .Filter(pos_cal_e_cercut)
        ;
      auto d_pos_eall_bg = d_pos_bg
        .Filter(pos_cal_e_cercut)
        ;
      double pos_cal_pi_cercut_low = j_DE["HMS"]["cer_pi_low"].get<double>();
      double pos_cal_pi_cercut_high = j_DE["HMS"]["cer_pi_high"].get<double>();
      std::string pos_cal_pi_cercut = "H.cer.npeSum < "+std::to_string(pos_cal_pi_cercut_high)+" && H.cer.npeSum > "+std::to_string(pos_cal_pi_cercut_low);
      auto d_pos_piall = d_pos_pi
        .Filter(pos_cal_pi_cercut)
        ;
      auto d_pos_piall_bg = d_pos_bg
        .Filter(pos_cal_pi_cercut)
        ;

      //statistics for DE efficiency
      json jout;

      int n_pos_e_cercut = *d_pos_eall.Count() - *d_pos_eall_bg.Count()/6.0;
      //h_cal_pos_e->GetEntries();
      jout[std::to_string(RunNumber)]["cercut"]["e_all"] = n_pos_e_cercut;
      auto d_pos_eall_did = d_pos_eall.Filter(cal_cut_use);
      auto d_pos_eall_bg_did = d_pos_eall_bg.Filter(cal_cut_use);
      double n_pos_e_cal = *d_pos_eall_did.Count()-*d_pos_eall_bg_did.Count()/6.0;

      jout[std::to_string(RunNumber)]["cercut"]["e_calcut"] = n_pos_e_cal;
      std::ofstream outfile;
      std::string outfile_name = "results/pid/DE/HMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;


    }// end of pos
    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      std::cout<<rootfile_name<<std::endl;
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM1.scalerCurrent");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,3,100},"P.BCM1.scalerCurrent");
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
      auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        //.Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      
      //coin time cut for neg runs
      auto h_cointime_neg = d_neg_run.Histo1D({"","coin_time",800,40,46},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
      double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
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
      double cointime_low_neg = coin_peak_center_neg+cointime_lowcut;
      double cointime_high_neg = coin_peak_center_neg+cointime_highcut;

      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      auto d_mod_first = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto d_neg_pi = d_mod_first
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        //.Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        //.Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        //.Define("p_q", p_q, {"p_electron"})
        //.Define("z", z, {"p_q","p_pion"})
        //.Define("Q2", Q2, {"p_q"})
        //.Define("xbj", xbj, {"Q2","p_q"})
        //.Define("W2", W2, {"p_q"})
        //.Define("Wp2", Wprime2, {"p_q","p_pion"})
        //.Define("W", "std::sqrt(W2)")
        //.Define("Wp", "std::sqrt(Wp2)")
        //.Define("InvMass","p_electron.Dot(p_pion)")

        ////.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
        //.Define("emiss",Emiss,{"p_pion","p_electron"})
        //.Define("mmiss",mmiss,{"p_pion","p_electron"})
        ////.Snapshot("T",skim_name.c_str());
        //.Define("pmiss","P.kin.secondary.pmiss")
        ;

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
      auto d_neg_bg = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        ;

      //I didn't use this quantity later. This is from other script. 
      int pion_counts = *d_neg_pi.Count() - *d_neg_bg.Count()/6;
      std::cout<<" neg coin counts "<<*d_neg_pi.Count()<<" bg counts "<<*d_neg_bg.Count()/6<<std::endl;

      //for cal cuts
      int neg_cal_e_cercut_n = j_DE["HMS"]["cer_e"].get<int>();
      std::string neg_cal_e_cercut = "H.cer.npeSum > "+std::to_string(neg_cal_e_cercut_n);
      std::cout<<neg_cal_e_cercut<<std::endl;
      // for e_all 
      auto d_neg_eall  = d_neg_pi
        .Filter(neg_cal_e_cercut)
        ;
      auto d_neg_eall_bg = d_neg_bg
        .Filter(neg_cal_e_cercut)
        ;
      double neg_cal_pi_cercut_low = j_DE["HMS"]["cer_pi_low"].get<double>();
      double neg_cal_pi_cercut_high = j_DE["HMS"]["cer_pi_high"].get<double>();
      std::string neg_cal_pi_cercut = "H.cer.npeSum < "+std::to_string(neg_cal_pi_cercut_high)+" && H.cer.npeSum > "+std::to_string(neg_cal_pi_cercut_low);
      auto d_neg_piall = d_neg_pi
        .Filter(neg_cal_pi_cercut)
        ;
      auto d_neg_piall_bg = d_neg_bg
        .Filter(neg_cal_pi_cercut)
        ;

      //statistics for DE efficiency
      json jout;

      int n_neg_e_cercut = *d_neg_eall.Count() - *d_neg_eall_bg.Count()/6.0;
      //h_cal_neg_e->GetEntries();
      jout[std::to_string(RunNumber)]["cercut"]["e_all"] = n_neg_e_cercut;
      auto d_neg_eall_did = d_neg_eall.Filter(cal_cut_use);
      auto d_neg_eall_bg_did = d_neg_eall_bg.Filter(cal_cut_use);
      double n_neg_e_cal = *d_neg_eall_did.Count()-*d_neg_eall_bg_did.Count()/6.0;

      jout[std::to_string(RunNumber)]["cercut"]["e_calcut"] = n_neg_e_cal;

      std::ofstream outfile;
      std::string outfile_name = "results/pid/DE/HMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;

    }// end of neg


  }
}
