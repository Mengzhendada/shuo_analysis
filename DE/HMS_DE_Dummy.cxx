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



void HMS_DE_Dummy(int RunGroup=0){

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

  std::vector<int> neg_Dummy,pos_Dummy;
  neg_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["Dummy"].get<std::vector<int>>();
  pos_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["Dummy"].get<std::vector<int>>();

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
  std::vector<double> cal_cut_low;
  cal_cut_low = j_DE["HMS"]["cal_cuts"].get<std::vector<double>>();
  int n_cuts = (int)cal_cut_low.size();
  std::cout<<"check"<<std::endl;
  std::vector<double> cer_cuts;
  cer_cuts = j_DE["HMS"]["cer_cuts"].get<std::vector<double>>();

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
  double current_offset = j_cuts["current_diff"].get<double>();
  std::cout<<Normal_HMS<<std::endl;
  std::cout<<Normal_SHMS<<std::endl;

  
    if(!neg_Dummy.empty() && !pos_Dummy.empty()){
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

    //for pos runs
    for(auto it = pos_Dummy.begin();it!=pos_Dummy.end();++it){
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
      .Filter(piCutSHMS)
      //.Filter(eCutHMS)
      .Filter(aeroCutSHMS)
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
    double rf_pi_low = j_DE["SHMS"]["rf_pi_low"].get<double>();
    std::cout<<rf_pi_low<<std::endl;
    double rf_pi_high =j_DE["SHMS"]["rf_pi_high"].get<double>();
    std::cout<<rf_pi_high<<std::endl;

    //loop over each pos runs data
    for(auto it = pos_Dummy.begin();it!=pos_Dummy.end();++it){
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
        .Filter(piCutSHMS)
        //.Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      

      auto d_pos_first = d_pos_run
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
      std::string skim_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
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
      auto h_coin_poscut = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_hms_dp_after_pos = d_pos_pi.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_pos = d_pos_pi.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_pos = d_pos_pi.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_pos = d_pos_pi.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_after_pos = d_pos_pi.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_pos = d_pos_pi.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");
      
      auto h_hms_dp_bg_pos = d_pos_bg.Histo1D({"","HMS dp bg",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_bg_pos = d_pos_bg.Histo1D({"","SHMS dp bg",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_bg_pos = d_pos_bg.Histo1D({"","HMS cal bg",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_bg_pos = d_pos_bg.Histo1D({"","HMS cer bg",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_bg_pos = d_pos_bg.Histo1D({"","SHMS cal bg",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_bg_pos = d_pos_bg.Histo1D({"","SHMS aero bg",200,0,50},"P.aero.npeSum");
    
      
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
      
      //not really necessary to check this histogram. 
      auto h_coin_pos_eall = d_pos_eall.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_pos_eall_bg = d_pos_eall_bg.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      json jout;
      auto h_cal_pos_nocercut = d_pos_pi.Histo1D({"","pos,cal,nocercut;calorimeter;counts",100,0.001,2},"H.cal.etottracknorm");
      auto h_cer_pos_nocalcut = d_pos_pi.Histo1D({"","pos,cer,nocalcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cal_pos_nocercut_bg = d_pos_bg.Histo1D({"","pos,cal,nocercut;calorimeter;counts",100,0.001,2},"H.cal.etottracknorm");
      auto h_cer_pos_nocalcut_bg = d_pos_bg.Histo1D({"","pos,cer,nocalcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cal_pos_nocercut->Add(h_cal_pos_nocercut_bg.GetPtr(),-1.0/6); 
      h_cer_pos_nocalcut->Add(h_cer_pos_nocalcut_bg.GetPtr(),-1.0/6); 
      auto h_cal_pos_e = d_pos_eall.Histo1D({"","pos,cal,e_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_pos_pi = d_pos_piall.Histo1D({"","pos,cal,pi_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_pos_e_bg = d_pos_eall_bg.Histo1D({"","pos,cal,e_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_pos_pi_bg = d_pos_piall_bg.Histo1D({"","pos,cal,pi_cercut",100,0.001,2},"H.cal.etottracknorm");
      h_cal_pos_e->Add(h_cal_pos_e_bg.GetPtr(),-1.0/6);
      h_cal_pos_pi->Add(h_cal_pos_pi_bg.GetPtr(),-1.0/6);

      int n_pos_e_cercut = h_cal_pos_e->GetEntries();
       // *d_pos_eall.Count() - *d_pos_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["cercut"]["e_all"] = n_pos_e_cercut;
      //all the cer cuts
      jout[std::to_string(RunNumber)]["cercut"]["e_cernpeSum"] = pos_cal_e_cercut;
      //int n_pos_pi_cercut = *d_pos_piall.Count() - *d_pos_piall_bg.Count()/6.0;
      int n_pos_pi_cercut = h_cal_pos_pi->GetEntries();
      jout[std::to_string(RunNumber)]["cercut"]["pi_all"] = n_pos_pi_cercut;
      //all the cer cuts
      jout[std::to_string(RunNumber)]["cercut"]["pi_cernpeSum"] = pos_cal_pi_cercut;

      std::vector<double> n_pos_e_cal,n_pos_pi_cal;
      for(int i = 0;i< n_cuts;++i){
        TAxis *axis_pos_e_cal = h_cal_pos_e->GetXaxis();
        int bmin_pos_e_cal = axis_pos_e_cal->FindBin(cal_cut_low[i])+1;
        int bmax_pos_e_cal = axis_pos_e_cal->GetLast();
        double n_pos_e_cercut_cal = (double)h_cal_pos_e->Integral(bmin_pos_e_cal,bmax_pos_e_cal);
        n_pos_e_cal.push_back(n_pos_e_cercut_cal);
         
        TAxis *axis_pos_pi_cal = h_cal_pos_pi->GetXaxis();
        int bmin_pos_pi_cal = axis_pos_pi_cal->FindBin(cal_cut_low[i])+1;
        int bmax_pos_pi_cal = axis_pos_pi_cal->GetLast();
        double n_pos_pi_cercut_cal = (double)h_cal_pos_pi->Integral(bmin_pos_pi_cal,bmax_pos_pi_cal);
        n_pos_pi_cal.push_back(n_pos_pi_cercut_cal);
      }
      jout[std::to_string(RunNumber)]["cercut"]["e_calcuts"] = n_pos_e_cal;
      jout[std::to_string(RunNumber)]["cercut"]["calcuts"] = cal_cut_low;
      jout[std::to_string(RunNumber)]["cercut"]["pi_calcuts"] = n_pos_pi_cal;

      TCanvas* c_DE_cal = new TCanvas();
      c_DE_cal->Divide(2,1);
      c_DE_cal->cd(1);
      TVirtualPad* Pad_cer_calcuts = c_DE_cal->cd(1);
      Pad_cer_calcuts->SetLogy();
      h_cer_pos_nocalcut->SetBit(TH1::kNoStats);
      h_cer_pos_nocalcut->SetMarkerStyle(8);
      h_cer_pos_nocalcut->DrawCopy("hist");
      TLine* line_e_cer = new TLine(pos_cal_e_cercut_n,0,pos_cal_e_cercut_n,1000);
      TLine* line_pi_cer_low = new TLine(pos_cal_pi_cercut_low,0,pos_cal_pi_cercut_low,1000);
      TLine* line_pi_cer_high = new TLine(pos_cal_pi_cercut_high,0,pos_cal_pi_cercut_high,1000);
      //line_e_cer->SetLineColor(kRed);
      line_e_cer->Draw("same");
      line_pi_cer_low->SetLineColor(kRed);
      line_pi_cer_low->Draw("same");
      line_pi_cer_high->SetLineColor(kRed);
      line_pi_cer_high->Draw("same");
      c_DE_cal->cd(2);
      TVirtualPad* Pad_cal_calcuts = c_DE_cal->cd(2);
      Pad_cal_calcuts->SetLogy();
      h_cal_pos_nocercut->SetBit(TH1::kNoStats);
      h_cal_pos_nocercut->SetMarkerStyle(8);
      h_cal_pos_nocercut->DrawCopy();
      h_cal_pos_e->SetBit(TH1::kNoStats);
      //h_cal_pos_e->SetLineColor(kRed);
      h_cal_pos_e->DrawCopy("hist same");
      h_cal_pos_pi->SetBit(TH1::kNoStats);
      h_cal_pos_pi->SetLineColor(kRed);
      h_cal_pos_pi->DrawCopy("hist same");
      Pad_cal_calcuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_cal_name = "results/pid/cal_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());
      
      //for cer cuts
      double pos_cer_e_calcut_n = j_DE["HMS"]["cal_e"].get<double>();
      std::string pos_cer_e_calcut = "H.cal.etottracknorm > "+std::to_string(pos_cer_e_calcut_n);
      double pos_cer_pi_calcut_low = j_DE["HMS"]["cal_pi_low"].get<double>();
      double pos_cer_pi_calcut_high = j_DE["HMS"]["cal_pi_high"].get<double>();
      std::string pos_cer_pi_calcut = "H.cal.etottracknorm > "+std::to_string(pos_cer_pi_calcut_low)+"&& H.cal.etottracknorm < "+std::to_string(pos_cer_pi_calcut_high);

      auto d_pos_eall_calcut  = d_pos_pi
        .Filter(pos_cer_e_calcut)
        ;
      auto d_pos_eall_calcut_bg  = d_pos_bg
        .Filter(pos_cer_e_calcut)
        ;
      auto h_cer_pos_e = d_pos_eall_calcut.Histo1D({"","pos,cer,e_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cer_pos_e_bg = d_pos_eall_calcut_bg.Histo1D({"","pos,cer,e_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cer_pos_e->Add(h_cer_pos_e_bg.GetPtr(),-1.0/6);
      auto d_pos_piall_calcut = d_pos_pi
        .Filter(pos_cer_pi_calcut)
        ;
      auto d_pos_piall_calcut_bg = d_pos_bg
        .Filter(pos_cer_pi_calcut)
        ;
      auto h_cer_pos_pi = d_pos_piall_calcut.Histo1D({"","pos,cer,pi_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cer_pos_pi_bg = d_pos_piall_calcut_bg.Histo1D({"","pos,cer,pi_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cer_pos_pi->Add(h_cer_pos_pi_bg.GetPtr(),-1.0/6);
      

      int n_pos_e_calcut = h_cer_pos_e->GetEntries();
      //int n_pos_e_calcut = *d_pos_eall_calcut.Count();
      jout[std::to_string(RunNumber)]["calcut"]["e_all"] = n_pos_e_calcut;
      jout[std::to_string(RunNumber)]["calcut"]["e_calcut"] = pos_cer_e_calcut;
      int n_pos_pi_calcut = h_cer_pos_pi->GetEntries();
      //int n_pos_pi_calcut = *d_pos_piall_calcut.Count();
      jout[std::to_string(RunNumber)]["calcut"]["pi_all"] = n_pos_pi_calcut;
      jout[std::to_string(RunNumber)]["calcut"]["pi_calcut"] = pos_cer_pi_calcut;
      std::vector<double> n_pos_e_cer,n_pos_pi_cer;
      for(int i = 0;i< cer_cuts.size();++i){
        TAxis *axis_pos_e_cer = h_cer_pos_e->GetXaxis();
        int bmin_pos_e_cer = axis_pos_e_cer->FindBin(cer_cuts[i])+1;
        int bmax_pos_e_cer = axis_pos_e_cer->GetLast();
        double n_pos_e_calcut_cer = (double)h_cer_pos_e->Integral(bmin_pos_e_cer,bmax_pos_e_cer);
        n_pos_e_cer.push_back(n_pos_e_calcut_cer);

        TAxis *axis_pos_pi_cer = h_cer_pos_pi->GetXaxis();
        int bmin_pos_pi_cer = axis_pos_pi_cer->FindBin(cer_cuts[i])+1;
        int bmax_pos_pi_cer = axis_pos_pi_cer->GetLast();
        double n_pos_pi_calcut_cer = (double)h_cer_pos_pi->Integral(bmin_pos_pi_cer,bmax_pos_pi_cer);
        n_pos_pi_cer.push_back(n_pos_pi_calcut_cer);

      }
      jout[std::to_string(RunNumber)]["calcut"]["e_cercuts"] = n_pos_e_cer;
      jout[std::to_string(RunNumber)]["calcut"]["pi_cercuts"] = n_pos_pi_cer;
      jout[std::to_string(RunNumber)]["calcut"]["cercuts"] = cer_cuts;

      std::ofstream outfile;
      std::string outfile_name = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;

      TCanvas* c_DE_cer = new TCanvas();
      c_DE_cer->Divide(2,1);
      c_DE_cer->cd(1);
      TVirtualPad* Pad_cer_cercuts = c_DE_cer->cd(1);
      Pad_cer_cercuts->SetLogy();
      h_cer_pos_nocalcut->SetBit(TH1::kNoStats);
      h_cer_pos_nocalcut->SetMarkerStyle(8);
      h_cer_pos_nocalcut->DrawCopy();
      h_cer_pos_e->SetBit(TH1::kNoStats);
      //h_cer_pos_e->SetLineColor(kRed);
      h_cer_pos_e->DrawCopy("hist same");
      h_cer_pos_pi->SetBit(TH1::kNoStats);
      h_cer_pos_pi->SetLineColor(kRed);
      h_cer_pos_pi->DrawCopy("hist same");
      Pad_cer_cercuts->BuildLegend(0.75,0.75,0.95,0.95);
      c_DE_cer->cd(2);
      TVirtualPad* Pad_cal_cercuts = c_DE_cer->cd(2);
      Pad_cal_cercuts->SetLogy();
      h_cal_pos_nocercut->SetBit(TH1::kNoStats);
      h_cal_pos_nocercut->DrawCopy("hist");
      TLine* line_e_cal = new TLine(pos_cer_e_calcut_n,0,pos_cer_e_calcut_n,1000);
      TLine* line_pi_cal_high = new TLine(pos_cer_pi_calcut_high,0,pos_cer_pi_calcut_high,1000);
      TLine* line_pi_cal_low = new TLine(pos_cer_pi_calcut_low,0,pos_cer_pi_calcut_low,1000);
      line_e_cal->Draw("same");
      line_pi_cal_high->SetLineColor(kRed);
      line_pi_cal_high->Draw("same");
      line_pi_cal_low->SetLineColor(kRed);
      line_pi_cal_low->Draw("same");
      std::string c_DE_cer_name = "results/pid/cer_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cer->SaveAs(c_DE_cer_name.c_str());





    }


    //for neg runs, updated version in new script
    for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
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
      .Filter(piCutSHMS)
      //.Filter(eCutHMS)
      .Filter(aeroCutSHMS)
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

    //loop over each neg runs data
    for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);
      ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
      auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM1.scalerCurrent");
      auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
      auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,3,100},"P.BCM1.scalerCurrent");
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
        .Filter(piCutSHMS)
        //.Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;

      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      auto d_mod_first = d_neg_first.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
      auto d_neg_pi = d_mod_first.Filter(
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
      auto d_neg_bg = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        ;
      int pion_counts = *d_neg_pi.Count() - *d_neg_bg.Count()/6;
      std::cout<<" neg coin counts "<<*d_neg_pi.Count()<<" bg counts "<<*d_neg_bg.Count()<<std::endl;
      auto h_coin_negcut = d_neg_pi.Histo1D({"","coin_time",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_hms_dp_after_neg = d_neg_pi.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_neg = d_neg_pi.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_neg = d_neg_pi.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_neg = d_neg_pi.Histo1D({"","HMS cer",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_after_neg = d_neg_pi.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_neg = d_neg_pi.Histo1D({"","SHMS aero",200,0,50},"P.aero.npeSum");

      auto h_hms_dp_bg_neg = d_neg_bg.Histo1D({"","HMS dp bg",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_bg_neg = d_neg_bg.Histo1D({"","SHMS dp bg",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_bg_neg = d_neg_bg.Histo1D({"","HMS cal bg",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_bg_neg = d_neg_bg.Histo1D({"","HMS cer bg",200,0,50},"H.cer.npeSum"); 
      auto h_shms_cal_bg_neg = d_neg_bg.Histo1D({"","SHMS cal bg",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_bg_neg = d_neg_bg.Histo1D({"","SHMS aero bg",200,0,50},"P.aero.npeSum");

      //for cal cuts
      int neg_cal_e_cercut_n = j_DE["HMS"]["cer_e"].get<int>();
      std::string neg_cal_e_cercut = "H.cer.npeSum > "+std::to_string(neg_cal_e_cercut_n);
      // for e_all 
      auto d_neg_eall  = d_neg_pi
        .Filter(neg_cal_e_cercut)
        ;
      auto d_neg_eall_bg = d_neg_bg
        .Filter(neg_cal_e_cercut)
        ;
      double neg_cal_pi_cercut_high = j_DE["HMS"]["cer_pi_high"].get<double>();
      double neg_cal_pi_cercut_low = j_DE["HMS"]["cer_pi_low"].get<double>();
      std::cout<<"check"<<std::endl;
      std::string neg_cal_pi_cercut = "H.cer.npeSum < "+std::to_string(neg_cal_pi_cercut_high)+" && H.cer.npeSum > "+std::to_string(neg_cal_pi_cercut_low);
      auto d_neg_piall = d_neg_pi
        .Filter(neg_cal_pi_cercut)
        ;
      auto d_neg_piall_bg = d_neg_bg
        .Filter(neg_cal_pi_cercut)
        ;
      
      //not really necessary to check this histogram. 
      auto h_coin_neg_eall = d_neg_eall.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_neg_eall_bg = d_neg_eall_bg.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      json jout;
      auto h_cal_neg_nocercut = d_neg_pi.Histo1D({"","neg,cal,nocercut;calorimeter;counts",100,0.001,2},"H.cal.etottracknorm");
      auto h_cer_neg_nocalcut = d_neg_pi.Histo1D({"","neg,cer,nocalcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cal_neg_nocercut_bg = d_neg_bg.Histo1D({"","neg,cal,nocercut;calorimeter;counts",100,0.001,2},"H.cal.etottracknorm");
      auto h_cer_neg_nocalcut_bg = d_neg_bg.Histo1D({"","neg,cer,nocalcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cal_neg_nocercut->Add(h_cal_neg_nocercut_bg.GetPtr(),-1/6); 
      h_cer_neg_nocalcut->Add(h_cer_neg_nocalcut_bg.GetPtr(),-1/6); 
      auto h_cal_neg_e = d_neg_eall.Histo1D({"","neg,cal,e_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_neg_pi = d_neg_piall.Histo1D({"","neg,cal,pi_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_neg_e_bg = d_neg_eall_bg.Histo1D({"","neg,cal,e_cercut",100,0.001,2},"H.cal.etottracknorm");
      auto h_cal_neg_pi_bg = d_neg_piall_bg.Histo1D({"","neg,cal,pi_cercut",100,0.001,2},"H.cal.etottracknorm");
      h_cal_neg_e->Add(h_cal_neg_e_bg.GetPtr(),-1/6);
      h_cal_neg_pi->Add(h_cal_neg_pi_bg.GetPtr(),-1/6);

      int n_neg_e_cercut = h_cal_neg_e->GetEntries();
       // *d_neg_eall.Count() - *d_neg_eall_bg.Count()/6.0;
      jout[std::to_string(RunNumber)]["cercut"]["e_all"] = n_neg_e_cercut;
      //all the cer cuts
      jout[std::to_string(RunNumber)]["cercut"]["e_cernpeSum"] = neg_cal_e_cercut;
      //int n_neg_pi_cercut = *d_neg_piall.Count() - *d_neg_piall_bg.Count()/6.0;
      int n_neg_pi_cercut = h_cal_neg_pi->GetEntries();
      jout[std::to_string(RunNumber)]["cercut"]["pi_all"] = n_neg_pi_cercut;
      //all the cer cuts
      jout[std::to_string(RunNumber)]["cercut"]["pi_cernpeSum"] = neg_cal_pi_cercut;

      std::vector<double> n_neg_e_cal,n_neg_pi_cal;
      for(int i = 0;i< n_cuts;++i){
        TAxis *axis_neg_e_cal = h_cal_neg_e->GetXaxis();
        int bmin_neg_e_cal = axis_neg_e_cal->FindBin(cal_cut_low[i])+1;
        int bmax_neg_e_cal = axis_neg_e_cal->GetLast();
        double n_neg_e_cercut_cal = (double)h_cal_neg_e->Integral(bmin_neg_e_cal,bmax_neg_e_cal);
        n_neg_e_cal.push_back(n_neg_e_cercut_cal);
         
        TAxis *axis_neg_pi_cal = h_cal_neg_pi->GetXaxis();
        int bmin_neg_pi_cal = axis_neg_pi_cal->FindBin(cal_cut_low[i])+1;
        int bmax_neg_pi_cal = axis_neg_pi_cal->GetLast();
        double n_neg_pi_cercut_cal = (double)h_cal_neg_pi->Integral(bmin_neg_pi_cal,bmax_neg_pi_cal);
        n_neg_pi_cal.push_back(n_neg_pi_cercut_cal);
      }
      jout[std::to_string(RunNumber)]["cercut"]["e_calcuts"] = n_neg_e_cal;
      jout[std::to_string(RunNumber)]["cercut"]["calcuts"] = cal_cut_low;
      jout[std::to_string(RunNumber)]["cercut"]["pi_calcuts"] = n_neg_pi_cal;

      TCanvas* c_DE_cal = new TCanvas();
      c_DE_cal->Divide(2,1);
      c_DE_cal->cd(1);
      TVirtualPad* Pad_cer_calcuts = c_DE_cal->cd(1);
      Pad_cer_calcuts->SetLogy();
      h_cer_neg_nocalcut->SetBit(TH1::kNoStats);
      h_cer_neg_nocalcut->SetMarkerStyle(8);
      h_cer_neg_nocalcut->DrawCopy("hist");
      TLine* line_e_cer = new TLine(neg_cal_e_cercut_n,0,neg_cal_e_cercut_n,1000);
      TLine* line_pi_cer_low = new TLine(neg_cal_pi_cercut_low,0,neg_cal_pi_cercut_low,1000);
      TLine* line_pi_cer_high = new TLine(neg_cal_pi_cercut_high,0,neg_cal_pi_cercut_high,1000);
      //line_e_cer->SetLineColor(kRed);
      line_e_cer->Draw("same");
      line_pi_cer_low->SetLineColor(kRed);
      line_pi_cer_low->Draw("same");
      line_pi_cer_high->SetLineColor(kRed);
      line_pi_cer_high->Draw("same");
      c_DE_cal->cd(2);
      TVirtualPad* Pad_cal_calcuts = c_DE_cal->cd(2);
      Pad_cal_calcuts->SetLogy();
      h_cal_neg_nocercut->SetBit(TH1::kNoStats);
      h_cal_neg_nocercut->SetMarkerStyle(8);
      h_cal_neg_nocercut->DrawCopy();
      h_cal_neg_e->SetBit(TH1::kNoStats);
      //h_cal_neg_e->SetLineColor(kRed);
      h_cal_neg_e->DrawCopy("hist same");
      h_cal_neg_pi->SetBit(TH1::kNoStats);
      h_cal_neg_pi->SetLineColor(kRed);
      h_cal_neg_pi->DrawCopy("hist same");
      Pad_cal_calcuts->BuildLegend(0.75,0.75,0.95,0.95);
      std::string c_DE_cal_name = "results/pid/cal_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());
      
      //for cer cuts
      double neg_cer_e_calcut_n = j_DE["HMS"]["cal_e"].get<double>();
      std::string neg_cer_e_calcut = "H.cal.etottracknorm > "+std::to_string(neg_cer_e_calcut_n);
      double neg_cer_pi_calcut_low = j_DE["HMS"]["cal_pi_low"].get<double>();
      double neg_cer_pi_calcut_high = j_DE["HMS"]["cal_pi_high"].get<double>();
      std::string neg_cer_pi_calcut = "H.cal.etottracknorm > "+std::to_string(neg_cer_pi_calcut_low)+"&& H.cal.etottracknorm < "+std::to_string(neg_cer_pi_calcut_high);

      auto d_neg_eall_calcut  = d_neg_pi
        .Filter(neg_cer_e_calcut)
        ;
      auto d_neg_eall_calcut_bg  = d_neg_bg
        .Filter(neg_cer_e_calcut)
        ;
      auto h_cer_neg_e = d_neg_eall_calcut.Histo1D({"","neg,cer,e_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cer_neg_e_bg = d_neg_eall_calcut_bg.Histo1D({"","neg,cer,e_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cer_neg_e->Add(h_cer_neg_e_bg.GetPtr(),-1.0/6);
      auto d_neg_piall_calcut = d_neg_pi
        .Filter(neg_cer_pi_calcut)
        ;
      auto d_neg_piall_calcut_bg = d_neg_bg
        .Filter(neg_cer_pi_calcut)
        ;
      auto h_cer_neg_pi = d_neg_piall_calcut.Histo1D({"","neg,cer,pi_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      auto h_cer_neg_pi_bg = d_neg_piall_calcut_bg.Histo1D({"","neg,cer,pi_calcut;NpeSum;counts",200,0,50},"H.cer.npeSum");
      h_cer_neg_pi->Add(h_cer_neg_pi_bg.GetPtr(),-1.0/6);
      

      int n_neg_e_calcut = h_cer_neg_e->GetEntries();
      //int n_neg_e_calcut = *d_neg_eall_calcut.Count();
      jout[std::to_string(RunNumber)]["calcut"]["e_all"] = n_neg_e_calcut;
      jout[std::to_string(RunNumber)]["calcut"]["e_calcut"] = neg_cer_e_calcut;
      int n_neg_pi_calcut = h_cer_neg_pi->GetEntries();
      //int n_neg_pi_calcut = *d_neg_piall_calcut.Count();
      jout[std::to_string(RunNumber)]["calcut"]["pi_all"] = n_neg_pi_calcut;
      jout[std::to_string(RunNumber)]["calcut"]["pi_calcut"] = neg_cer_pi_calcut;
      std::vector<double> n_neg_e_cer,n_neg_pi_cer;
      for(int i = 0;i< cer_cuts.size();++i){
        TAxis *axis_neg_e_cer = h_cer_neg_e->GetXaxis();
        int bmin_neg_e_cer = axis_neg_e_cer->FindBin(cer_cuts[i])+1;
        int bmax_neg_e_cer = axis_neg_e_cer->GetLast();
        double n_neg_e_calcut_cer = (double)h_cer_neg_e->Integral(bmin_neg_e_cer,bmax_neg_e_cer);
        n_neg_e_cer.push_back(n_neg_e_calcut_cer);

        TAxis *axis_neg_pi_cer = h_cer_neg_pi->GetXaxis();
        int bmin_neg_pi_cer = axis_neg_pi_cer->FindBin(cer_cuts[i])+1;
        int bmax_neg_pi_cer = axis_neg_pi_cer->GetLast();
        double n_neg_pi_calcut_cer = (double)h_cer_neg_pi->Integral(bmin_neg_pi_cer,bmax_neg_pi_cer);
        n_neg_pi_cer.push_back(n_neg_pi_calcut_cer);

      }
      jout[std::to_string(RunNumber)]["calcut"]["e_cercuts"] = n_neg_e_cer;
      jout[std::to_string(RunNumber)]["calcut"]["pi_cercuts"] = n_neg_pi_cer;
      jout[std::to_string(RunNumber)]["calcut"]["cercuts"] = cer_cuts;

      std::ofstream outfile;
      std::string outfile_name = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
      outfile.open(outfile_name.c_str());
      outfile<<jout.dump(4)<<std::endl;

      TCanvas* c_DE_cer = new TCanvas();
      c_DE_cer->Divide(2,1);
      c_DE_cer->cd(1);
      TVirtualPad* Pad_cer_cercuts = c_DE_cer->cd(1);
      Pad_cer_cercuts->SetLogy();
      h_cer_neg_nocalcut->SetBit(TH1::kNoStats);
      h_cer_neg_nocalcut->SetMarkerStyle(8);
      h_cer_neg_nocalcut->DrawCopy();
      h_cer_neg_e->SetBit(TH1::kNoStats);
      //h_cer_neg_e->SetLineColor(kRed);
      h_cer_neg_e->DrawCopy("hist same");
      h_cer_neg_pi->SetBit(TH1::kNoStats);
      h_cer_neg_pi->SetLineColor(kRed);
      h_cer_neg_pi->DrawCopy("hist same");
      Pad_cer_cercuts->BuildLegend(0.75,0.75,0.95,0.95);
      c_DE_cer->cd(2);
      TVirtualPad* Pad_cal_cercuts = c_DE_cer->cd(2);
      Pad_cal_cercuts->SetLogy();
      h_cal_neg_nocercut->SetBit(TH1::kNoStats);
      h_cal_neg_nocercut->DrawCopy("hist");
      TLine* line_e_cal = new TLine(neg_cer_e_calcut_n,0,neg_cer_e_calcut_n,1000);
      TLine* line_pi_cal_high = new TLine(neg_cer_pi_calcut_high,0,neg_cer_pi_calcut_high,1000);
      TLine* line_pi_cal_low = new TLine(neg_cer_pi_calcut_low,0,neg_cer_pi_calcut_low,1000);
      line_e_cal->Draw("same");
      line_pi_cal_high->SetLineColor(kRed);
      line_pi_cal_high->Draw("same");
      line_pi_cal_low->SetLineColor(kRed);
      line_pi_cal_low->Draw("same");
      std::string c_DE_cer_name = "results/pid/cer_DE_"+std::to_string(RunNumber)+".pdf";
      c_DE_cer->SaveAs(c_DE_cer_name.c_str());

       }
    }
}
