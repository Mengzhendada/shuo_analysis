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


constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;


bool shms_momentum_high = true;

void statistic_runs_D2(int RunGroup=0){

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

  double H_dp_low = j_cuts["H_dp_low"].get<double>();
  double H_dp_high = j_cuts["H_dp_high"].get<double>();
  double P_dp_low = j_cuts["P_dp_low"].get<double>();
  double P_dp_high = j_cuts["P_dp_high"].get<double>();
  std::string goodTrackSHMS = "P.gtr.dp>"+std::to_string(P_dp_low)+" && P.gtr.dp<"+std::to_string(P_dp_low);
  std::string goodTrackHMS = "H.gtr.dp>"+std::to_string(H_dp_low)+" && H.gtr.dp<"+std::to_string(H_dp_high);
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
    if(SHMS_P>3.2){aeroCutSHMS = aeroCutSHMS + " && P.hgcer.npeSum > "+(std::to_string(P_hgcer)).c_str();}
    //for neg runs, updated version in new script
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg scaler "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
      ROOT::RDataFrame d_neg_scale("TSP",rootfile_name);
      double charge = *d_neg_scale.Max("P.BCM4B.scalerChargeCut")/1000.0;
      double time_1MHz_cut = *d_neg_scale.Max("P.1MHz.scalerTimeCut");
      int scaler_events  = *d_neg_scale.Count();
      long int datacounts = *d_neg_scale.Max("P.pTRIG1.scaler");
      //std::cout<<charge<<std::endl;
      jout[(std::to_string(RunNumber)).c_str()]["charge"] = charge;
      jout[(std::to_string(RunNumber)).c_str()]["time"] = time_1MHz_cut;
      jout[(std::to_string(RunNumber)).c_str()]["scaler_n"] = scaler_events;
      jout[(std::to_string(RunNumber)).c_str()]["data_n"] = datacounts;

    }
    double Eb;
    if(RunGroup < 420) {Eb = 10.597;}
    else{Eb = 10.214;}
    auto p_proton = [](double px, double py, double pz){
      return Pvec4D{px , py , pz , M_P};
    };
    auto p_pion = [](double px, double py, double pz) {
      return Pvec4D{px , py , pz , M_pion};
    };
    auto pion_momentum = [](double px,double py,double pz){
      TVector3 v(px,py,pz);
      return v;
    };
    auto p_electron = [](double px, double py, double pz) {
      return Pvec4D{px , py , pz , M_e};
    };
    auto p_q = [&](Pvec4D& pe ) {
      return Pvec4D{0.0,0.0,Eb, M_e}-pe;
    };
    auto t = [](const double Egamma, Pvec4D& jpsi) {
      Pvec4D beam{0, 0, Egamma, 0};
      return (beam - jpsi).M2();
    };
    auto z = [](Pvec4D& pq, Pvec4D& ph) {
      return ph.E()/pq.E();
    };
    auto xbj = [](double Q2,Pvec4D& pq) {
      return Q2/(2.0*0.938*pq.E());
    };
    auto Q2 = [](Pvec4D& pq) {
      return -1.0*(pq.Dot(pq));
    };
    auto Wprime2 = [](Pvec4D& pq,Pvec4D& ph) {
      auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq - ph;
      return Ptot.Dot(Ptot);
    };
    auto W2 = [](Pvec4D& pq) {
      auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq;
      return Ptot.Dot(Ptot);
    };
    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos scaler "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
      ROOT::RDataFrame d_pos_scale("TSP",rootfile_name);
      double charge = *d_pos_scale.Max("P.BCM4B.scalerChargeCut")/1000.0;
      double time_1MHz_cut = *d_pos_scale.Max("P.1MHz.scalerTimeCut");
      int scaler_events  = *d_pos_scale.Count();
      long int datacounts = *d_pos_scale.Max("P.pTRIG1.scaler");
      //std::cout<<charge<<std::endl;
      jout[(std::to_string(RunNumber)).c_str()]["charge"] = charge;
      jout[(std::to_string(RunNumber)).c_str()]["time"] = time_1MHz_cut;
      jout[(std::to_string(RunNumber)).c_str()]["scaler_n"] = scaler_events;
      jout[(std::to_string(RunNumber)).c_str()]["data_n"] = datacounts;
    }

    ROOT::RDataFrame d_pos_raw("T",files_pos);
    auto d_pos = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
      .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
      .Filter(aeroCutSHMS)
      .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
      ;
    //coin time cut for pos runs
    auto h_cointime_pos = d_pos.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
    int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
    double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
    auto d_pos_2ndpeak = d_pos.Filter([=](double cointime){return cointime<coin_peak_center_pos-2.004;},{"CTime.ePiCoinTime_ROC2"});
    auto h_cointime_2nd = d_pos_2ndpeak.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
    int coin_peak_2ndbin_pos = h_cointime_2nd->GetMaximumBin();
    double coin_peak_2nd_pos = h_cointime_2nd->GetBinCenter(coin_peak_2ndbin_pos);
    //if(coin_peak_2nd_pos<coin_peak_center_pos){coin_peak_center_pos = coin_peak_2nd_pos;}
    std::cout<<"coin time peak "<<coin_peak_center_pos<<std::endl;
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
    auto h_coin_poscut = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
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
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(current-pos_setcurrent)<3;},{"current"})
        ;
        auto d_pos_first = d_pos_run
          .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
      auto d_pos_pi = d_mod_first
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")

        //.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
        //.Define("emiss",Emiss,{"p_pion","p_electron"})
        //.Define("mmiss",mmiss,{"p_pion","p_electron"})
        ;
      int pion_counts = *d_pos_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_counts;


      // for bg
      auto d_pos_forbg = d_pos_run
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_difftime_forbg = d_pos_forbg.Histo1D({"","diff for bg",100,0,4.008},"diff_time_mod");
      auto d_pos_bg  = d_pos_forbg
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        // .Filter(
        //     [=](double bg_cointime){return ((bg_cointime > bg_left_low && bg_cointime <bg_left_high) || (bg_cointime > bg_right_low && bg_cointime < bg_right_high));},{"CTime.ePiCoinTime_ROC2"})
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")
        ;
      auto h_coin_pos_bg = d_pos_bg.Histo1D({"","pos bg",800,0,100},"CTime.ePiCoinTime_ROC2");
      TCanvas* c_check = new TCanvas();
      c_check->Divide(2,2);
      c_check->cd(1);
      h_coin_pos->DrawCopy("hist");
      h_coin_poscut->SetLineColor(kRed);
      h_coin_poscut->DrawCopy("hist same");
      h_coin_pos_bg->SetLineColor(kBlue);
      h_coin_pos_bg->DrawCopy("hist same");
      c_check->cd(2);
      h_time_diff_poscheck->DrawCopy("hist");
      c_check->cd(3);
      h_time_diff_pos->DrawCopy("hist");
      c_check->cd(4);
      h_diff_mod_pos->DrawCopy("hist");
      h_difftime_forbg->SetLineColor(kBlue);
      h_difftime_forbg->DrawCopy("hist same");
      std::string c_check_name = "results/yield/check/"+std::to_string(RunNumber)+"pos.png";
      c_check->SaveAs(c_check_name.c_str());

      int bg_counts = *d_pos_bg.Count()/6;
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_counts;

      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_pos = d_pos_pi.Histo2D({"Q2_x","Q2_x",100,0,1,100,0,10},"xbj","Q2");
      h_Q2_x_pos->Write();
      auto h_xbj = d_pos_pi.Histo1D({"xbj","xbj",100,0,1},"xbj");
      h_xbj->Write();
      auto h_z = d_pos_pi.Histo1D({"z","z",100,0,1},"z");
      h_z->Write();
      auto h_xbj_bg = d_pos_bg.Histo1D({"xbj_bg","xbj_bg",100,0,1},"xbj");
      h_xbj_bg->Write();
      auto h_z_bg = d_pos_bg.Histo1D({"z_bg","z_bg",100,0,1},"z");
      h_z_bg->Write();
      rootfile_out->Close();
    }



    //for neg runs
    ROOT::RDataFrame d_neg_raw("T",files_neg);
    auto d_neg = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
      .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
      .Filter(goodTrackSHMS)
      .Filter(goodTrackHMS)
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
      .Filter(aeroCutSHMS)
      .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
      ;
    //coin time cut for neg
    auto h_cointime_neg = d_neg.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
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
    auto h_coin_negcut = d_neg_coin.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");

    //rftime cut for neg
    auto h_time_diff_neg = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
    auto h_time_diff_negcheck = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,-100,100},"fptime_minus_rf");
    int time_diff_neg_bin_max = h_time_diff_neg->GetMaximumBin();
    double time_diff_neg_max = h_time_diff_neg->GetBinCenter(time_diff_neg_bin_max);
    double offset_neg = 401.8-time_diff_neg_max;
    std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
    auto d_mod_neg = d_neg_coin.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
      .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
      ;
    auto h_diff_mod_neg = d_mod_neg.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
    auto d_neg_pi = d_mod_neg.Filter(
        [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
      ; 

    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,0,100},"P.BCM4B.scalerCurrent");
      double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());

      auto neg_get_current = [&](unsigned int eventNum){
        long unsigned int i = 0;
        while(eventNum>neg_scaler_event_list->at(i)){
          ++i;
          if(i>neg_scaler_current_list->size()-1)
          {
            i=neg_scaler_current_list->size()-1;
            break;
          }
        }
        return neg_scaler_current_list->at(i);
      };
      auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(current-neg_setcurrent)<3;},{"current"})
        ;
      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto d_mod_first = d_neg_first.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
      json j_rfcut;
      {
        std::ifstream ifs("db2/rftime_cut.json");
        ifs>>j_rfcut;
      }
      double rf_pi_low = j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_low"].get<double>();
      std::cout<<rf_pi_low<<std::endl;
      double rf_pi_high =j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_high"].get<double>();
      std::cout<<rf_pi_high<<std::endl;
      auto d_neg_pi = d_mod_first.Filter(
          [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")
        //.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
        //.Define("emiss",Emiss,{"p_pion","p_electron"})
        //.Define("mmiss",mmiss,{"p_pion","p_electron"})
        ;
      int pion_counts = *d_neg_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_counts;

      //for bg
      int bg_left_low = j_cuts["random_bg_left_low"].get<double>();
      int bg_left_high = j_cuts["random_bg_left_high"].get<double>();
      int bg_right_low = j_cuts["random_bg_right_low"].get<double>();
      int bg_right_high = j_cuts["random_bg_right_high"].get<double>();

      auto d_neg_forbg = d_neg_run
        //.Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_difftime_forbg = d_neg_forbg.Histo1D({"","diff for bg",100,0,4.008},"diff_time_mod");
      auto d_neg_bg = d_neg_forbg
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        //.Filter(
        //    [=](double bg_cointime){return ((bg_cointime > bg_left_low && bg_cointime <bg_left_high) || (bg_cointime > bg_right_low && bg_cointime < bg_right_high));},{"CTime.ePiCoinTime_ROC2"})
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")
        ;
      auto h_coin_neg_bg = d_neg_bg.Histo1D({"","neg bg",800,0,100},"CTime.ePiCoinTime_ROC2");
      TCanvas* c_check = new TCanvas();
      c_check->Divide(2,2);
      c_check->cd(1);
      h_coin_neg->DrawCopy("hist");
      h_coin_negcut->SetLineColor(kRed);
      h_coin_negcut->DrawCopy("hist same");
      h_coin_neg_bg->SetLineColor(kBlue);
      h_coin_neg_bg->DrawCopy("hist same");
      c_check->cd(2);
      h_time_diff_negcheck->DrawCopy("hist");
      c_check->cd(3);
      h_time_diff_neg->DrawCopy("hist");
      c_check->cd(4);
      h_diff_mod_neg->DrawCopy("hist");
      h_difftime_forbg->SetLineColor(kBlue);
      h_difftime_forbg->DrawCopy("hist same");
      std::string c_check_name = "results/yield/check/"+std::to_string(RunNumber)+"neg.png";
      c_check->SaveAs(c_check_name.c_str());

      int bg_counts = *d_neg_bg.Count()/6;
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_counts;
      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_neg = d_neg_pi.Histo2D({"Q2_x","Q2_x",100,0,1,100,0,10},"xbj","Q2");
      h_Q2_x_neg->Write();
      auto h_xbj = d_neg_pi.Histo1D({"xbj","xbj",100,0,1},"xbj");
      h_xbj->Write();
      auto h_z = d_neg_pi.Histo1D({"z","z",100,0,1},"z");
      h_z->Write();
      auto h_xbj_bg = d_neg_bg.Histo1D({"xbj_bg","xbj_bg",100,0,1},"xbj");
      h_xbj_bg->Write();
      auto h_z_bg = d_neg_bg.Histo1D({"z_bg","z_bg",100,0,1},"z");
      h_z_bg->Write();
      rootfile_out->Close();
    }
    std::string of = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
    std::ofstream outfile(of.c_str());
    outfile<<jout.dump(4)<<std::endl;


  }
}
