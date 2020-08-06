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
auto p_q = [](Pvec4D& pe ) {
  return Pvec4D{0.0,0.0,10.6, M_e}-pe;
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

bool shms_momentum_high = true;

void statistic_runs_Jun(int RunGroup=0){

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

  std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
  std::string goodTrackHMS = "H.gtr.dp>-10 && H.gtr.dp<10";
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
  std::string eCutHMS = ("H.cal.etottracknorm > "+std::to_string(H_cal_low)+" && H.cal.etottracknorm < "+std::to_string(H_cal_high)+" && H.cer.npeSum > "+std::to_string(H_cer)).c_str();
  std::cout<<"cCutHMS"<<eCutHMS<<std::endl;
  json jout;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>jout;
  }

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

    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
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
    std::cout<<"coin time peak "<<coin_peak_center_pos<<std::endl;
    double cointime_lowcut = j_cuts["cointime_low"].get<double>();
    double cointime_highcut = j_cuts["cointime_high"].get<double>();
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
    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);
      auto d_pos_first = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"});
      auto d_mod_first = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
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
     TCanvas* c_check = new TCanvas();
     c_check->Divide(2,2);
     c_check->cd(1);
     h_coin_pos->DrawCopy("hist");
     h_coin_poscut->SetLineColor(kRed);
     h_coin_poscut->DrawCopy("hist same");
     c_check->cd(2);
     h_time_diff_poscheck->DrawCopy("hist");
     c_check->cd(3);
     h_time_diff_pos->DrawCopy("hist");
     c_check->cd(4);
     h_diff_mod_pos->DrawCopy("hist");
     c_check->SaveAs("results/yield/check.pdf");
      int pion_counts = *d_pos_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_counts;

      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_xbj = d_pos_pi.Histo1D({"xbj","xbj",100,0,1},"xbj");
      h_xbj->Write();
      auto h_z = d_pos_pi.Histo1D({"z","z",100,0,1},"z");
      h_z->Write();
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
    std::cout<<"coin time peak "<<coin_peak_center_neg<<std::endl;
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
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      auto d_neg_first = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"});
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
     TCanvas* c_check = new TCanvas();
     c_check->Divide(2,2);
     c_check->cd(1);
     h_coin_neg->DrawCopy("hist");
     h_coin_negcut->SetLineColor(kRed);
     h_coin_negcut->DrawCopy("hist same");
     c_check->cd(2);
     h_time_diff_negcheck->DrawCopy("hist");
     c_check->cd(3);
     h_time_diff_neg->DrawCopy("hist");
     c_check->cd(4);
     h_diff_mod_neg->DrawCopy("hist");
     c_check->SaveAs("results/yield/checkneg.pdf");
      int pion_counts = *d_neg_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_counts;

      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_xbj = d_neg_pi.Histo1D({"xbj","xbj",100,0,1},"xbj");
      h_xbj->Write();
      auto h_z = d_neg_pi.Histo1D({"z","z",100,0,1},"z");
      h_z->Write();
      rootfile_out->Close();
    }
  std::ofstream outfile("results/yield/runs_info.json");
  outfile<<jout.dump(4)<<std::endl;


  }
}
