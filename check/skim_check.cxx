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
#include "TStyle.h"

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

double Get_HMS_P_corr(double hmsp){
  return -0.000276*hmsp*hmsp*hmsp+0.002585*hmsp*hmsp-0.008697*hmsp+1.006440;
}
double Get_SHMS_P_corr(double shmsp){
  return 0.998;
} 
double Get_pi_eff(double shms_p){
  //double pi_eff = 0.9824+0.002969*shms_p;
  //if(pi_eff>1) pi_eff = 1;
  double pi_eff = 1;
  return pi_eff;
}
double Get_pos_pi_purity(double shms_p){
  if(shms_p>2.8){
  //double pi_purity = 1.978-0.4815*shms_p+0.05161*shms_p*shms_p;
    double pi_purity = 0.8381+0.1812*shms_p-0.04437*shms_p*shms_p;
    if(pi_purity>1) pi_purity = 1;
  return pi_purity;
  }
  else{
    return 1;
  }
}
double Get_neg_pi_purity(double shms_p){
  if(shms_p>2.8){
  //double pi_purity = 1.478-0.2423*shms_p+0.0687*shms_p*shms_p;
    double pi_purity = 0.9079+0.09864*shms_p-0.02375*shms_p*shms_p;
    if(pi_purity>1) pi_purity = 1;
  return pi_purity;
  }
  else{
    return 1;
  }
}

bool shms_momentum_high = true;

void skim_check(int RunGroup=0){

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

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
  double W2_cut_num = j_cuts["W2"].get<double>();
  std::string W2_cut = "W2 > "+std::to_string(W2_cut_num);
  double Mx2_cut_num = j_cuts["Mx2"].get<double>();
  std::string Mx2_cut = "Mx2>"+std::to_string(Mx2_cut_num);
  std::string Wp2_cut = "Wp2>"+std::to_string(Mx2_cut_num);
  //auto Mx2_cut = [=](double Mx2){return Mx2>Mx2_cut_num;};
  double current_offset = j_cuts["current_diff"].get<double>();

  json j_runsinfo;
  {
    std::string if_name = "db2/runs_info.json";
    std::ifstream ifs(if_name.c_str());
    ifs>>j_runsinfo;
  }
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_DE;
  }
      std::vector<int> delta_cut_num= j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>(); 
  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    SHMS_P = SHMS_P*Get_SHMS_P_corr(SHMS_P);
    auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};
    //if(SHMS_P>3.2){aeroCutSHMS = aeroCutSHMS + " && P.hgcer.npeSum > "+(std::to_string(P_hgcer)).c_str();}
    
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
    auto Mx2 = [](double nu,Pvec4D& ph, double pmiss){
      return (M_P + nu -ph.E())*(M_P + nu -ph.E())-abs(pmiss)*abs(pmiss);
    };
    auto W2 = [](Pvec4D& pq) {
      auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq;
      return Ptot.Dot(Ptot);
    };
    auto Emiss = [&](Pvec4D& p_pion, Pvec4D p_electron){
      return Eb+M_P-sqrt(p_pion.E()*p_pion.E())-sqrt(p_electron.E()*p_electron.E());
    };
    auto mmiss = [&](Pvec4D& p_pion,Pvec4D p_electron){
      Pvec4D missing_mass = Pvec4D{0.0,0.0,Eb,M_e}+Pvec4D{0.0,0.0,0.0,M_P}-p_electron-p_pion;
      return std::sqrt(missing_mass.M2());
    };


   // ROOT::RDataFrame d_pos_raw("T",files_pos);
   // auto d_pos = d_pos_raw
   //   //.Filter("fEvtHdr.fEvtType == 4")
   //   .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
   //   .Filter(goodTrackSHMS)
   //   .Filter(goodTrackHMS)
   //   .Filter(piCutSHMS)
   //   .Filter(eCutHMS)
   //   .Filter(aeroCutSHMS)
   //   .Filter(Normal_SHMS)
   //   .Filter(Normal_HMS)
   //   .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
   //   ;




    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
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
      auto h_hms_cer_before_pos = d_pos_raw.Histo1D({"","HMS cer",100,0,20},"H.cer.npeSum"); 
      auto h_shms_cal_before_pos = d_pos_raw.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_before_pos = d_pos_raw.Histo1D({"","SHMS aero",100,0,15},"P.aero.npeSum");
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Filter("P.dc.InsideDipoleExit == 1")
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        //.Filter([&](double current){return std::abs(current-pos_setcurrent)<current_offset;},{"current"})
        ;

      auto h_current_before_pos = d_pos_run.Histo1D({"","current",100,3,100},"current");
      TCanvas* c_pos_current = new TCanvas("","coin time",2200,1450);
      gStyle->SetOptTitle(0);
      h_current_before_pos->DrawCopy("hist");
      h_current_before_pos->GetXaxis()->SetTitle("Current");
      h_current_before_pos->GetYaxis()->SetTitle("Count");
      c_pos_current->Update();
      std::string c_pos_current_name = "results/yield/check/current_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_current->SaveAs(c_pos_current_name.c_str());
      
      //coin time cut for pos runs
      auto h_cointime_pos = d_pos_run.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
      double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
      std::cout<<"coin center"<<coin_peak_center_pos<<std::endl;
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
        .Define("diff_time_mod_beforeshift",[](double difftime){return std::fmod(difftime,4.008);},{"fptime_minus_rf"})
        ;
      auto h_coin_pos = d_pos_run.Histo1D({"",";Coincidence time;Counts",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_poscut = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //rftime cut
      //offset
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      std::cout<<"rf offset"<<offset_pos<<std::endl;
      double rf_pi_low = j_cuts["rf_cut_low"].get<double>();
      double rf_pi_high = j_cuts["rf_cut_high"].get<double>();
      auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
        return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  
        
      };
      
      auto d_mod_first = d_pos_first
        .Define("diff_time_mod",[offset_pos](double difftime){return std::fmod(difftime+offset_pos,4.008);},{"fptime_minus_rf"})
        .Define("Pi_eff",Get_pi_eff,{"shms_p"})
        .Define("Pi_purity",Get_pos_pi_purity,{"shms_p"})
        .Define("weight","Pi_purity/Pi_eff")
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
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
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
        //.Snapshot("T",skim_name.c_str());
        .Define("pmiss","P.kin.secondary.pmiss")
        .Define("Mx2",Mx2,{"H.kin.primary.nu","p_pion","P.kin.secondary.pmiss"})
        ;
      auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
      std::string skim_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      auto d_pos_pi = d_mod_first
      //  .Filter(
      //      [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        //.Filter(Wp2_cut)
        //.Filter(Mx2_cut)
        //.Filter(W2_cut)
        ;
      ROOT::RDF::RSnapshotOptions opts;
      //= {"UPDATE", ROOT::kZLIB, 0, 0, 99, true};
      opts.fMode = "UPDATE";
      d_pos_pi.Snapshot("T",skim_name.c_str(),{"shms_p","xbj","z","Q2","W2","W","Wp","Wp2","emiss","mmiss","InvMass","Mx2","pmiss","weight","P.gtr.th","P.gtr.ph","P.gtr.y","P.gtr.dp","P.gtr.p","H.gtr.dp","P.kin.secondary.th_xq","P.kin.secondary.ph_xq","H.kin.primary.omega"});
      //d_pos_pi.Snapshot("T",skim_name.c_str(),{"xbj","z","Q2","W2","W","Wp","emiss","mmiss","InvMass","weight"});
      std::cout<<"check"<<std::endl;
      int pion_counts = *d_pos_pi.Count();
      std::cout<<"counts "<<pion_counts<<std::endl;

      auto h_hms_dp_after_pos = d_pos_pi.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_pos = d_pos_pi.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_pos = d_pos_pi.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_pos = d_pos_pi.Histo1D({"","HMS cer",100,0,20},"H.cer.npeSum"); 
      auto h_shms_cal_after_pos = d_pos_pi.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_pos = d_pos_pi.Histo1D({"","SHMS aero",100,0,15},"P.aero.npeSum");

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
    //std::cout<<bg_cut<<std::endl;
      // for bg
      auto d_pos_forbg = d_pos_run
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Define("Pi_eff",Get_pi_eff,{"shms_p"})
        .Define("Pi_purity",Get_pos_pi_purity,{"shms_p"})
        .Define("weight","Pi_purity/Pi_eff")
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
        .Define("Mx2",Mx2,{"H.kin.primary.nu","p_pion","P.kin.secondary.pmiss"})
        .Define("pmiss","P.kin.secondary.pmiss")
        ;
      auto h_difftime_forbg = d_pos_forbg.Histo1D({"","diff for bg",100,0,4.008},"diff_time_mod");
      auto d_pos_bg  = d_pos_forbg
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        //.Filter(Mx2_cut)
        //.Filter(Wp2_cut)
        //.Filter(W2_cut)
        ;
      //d_pos_bg.Snapshot("T_bg",skim_name.c_str());
      d_pos_bg.Snapshot("T_bg",skim_name.c_str(),{"shms_p","xbj","z","Q2","W2","W","Wp","Wp2","emiss","mmiss","InvMass","pmiss","Mx2","weight","P.gtr.th","P.gtr.ph","P.gtr.y","P.gtr.dp","P.gtr.p","H.gtr.dp","P.kin.secondary.th_xq","P.kin.secondary.ph_xq","H.kin.primary.omega"},opts);
      //d_pos_bg.Snapshot("T_bg",skim_name.c_str(),{"xbj","z","Q2","W2","W","Wp","emiss","mmiss","InvMass","weight"});
      auto h_coin_pos_bg = d_pos_bg.Histo1D({"","pos bg",800,0,100},"CTime.ePiCoinTime_ROC2");

      TCanvas* c_pos_cointime = new TCanvas("","coin time",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      h_coin_pos->DrawCopy("hist");
      h_coin_poscut->SetLineColor(kRed);
      h_coin_poscut->DrawCopy("hist same");
      h_coin_pos_bg->SetLineColor(kBlue);
      h_coin_pos_bg->DrawCopy("hist same");
      c_pos_cointime->Update();
      std::string c_pos_cointime_name = "results/yield/check/cointime_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_cointime->SaveAs(c_pos_cointime_name.c_str());
      TCanvas* c_pos_hms_dp = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_pos_hms_dp->Divide(2,1);
      c_pos_hms_dp->cd(1);
      h_hms_dp_before_pos->DrawCopy("hist");
      c_pos_hms_dp->cd(2);
      h_hms_dp_after_pos->SetLineColor(kRed);
      h_hms_dp_after_pos->DrawCopy("hist");
      std::string c_pos_hms_dp_name = "results/yield/check/hms_dp_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_hms_dp->SaveAs(c_pos_hms_dp_name.c_str());      
      TCanvas* c_pos_shms_dp = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_pos_shms_dp->Divide(2,1);
      c_pos_shms_dp->cd(1);
      h_shms_dp_before_pos->DrawCopy("hist");
      c_pos_shms_dp->cd(2);
      h_shms_dp_after_pos->SetLineColor(kRed);
      h_shms_dp_after_pos->DrawCopy("hist");
      std::string c_pos_shms_dp_name = "results/yield/check/shms_dp_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_shms_dp->SaveAs(c_pos_shms_dp_name.c_str());      
      TCanvas* c_pos_hms_cal = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_pos_hms_cal->SetLogy();
      c_pos_hms_cal->Divide(2,1);
      c_pos_hms_cal->cd(1);
      h_hms_cal_before_pos->DrawCopy("hist");
      c_pos_hms_cal->cd(2);
      h_hms_cal_after_pos->SetLineColor(kRed);
      h_hms_cal_after_pos->DrawCopy("hist");
      std::string c_pos_hms_cal_name = "results/yield/check/hms_cal_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_hms_cal->SaveAs(c_pos_hms_cal_name.c_str());      
      TCanvas* c_pos_hms_cer = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_pos_hms_cer->SetLogy();
      c_pos_hms_cer->Divide(2,1);
      c_pos_hms_cer->cd(1);
      h_hms_cer_before_pos->DrawCopy("hist");
      c_pos_hms_cer->cd(2);
      h_hms_cer_after_pos->SetLineColor(kRed);
      h_hms_cer_after_pos->DrawCopy("hist");
      std::string c_pos_hms_cer_name = "results/yield/check/hms_cer_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_hms_cer->SaveAs(c_pos_hms_cer_name.c_str());      
      TCanvas* c_pos_shms_cal = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_pos_shms_cal->SetLogy();
      c_pos_shms_cal->Divide(2,1);
      c_pos_shms_cal->cd(1);
      h_shms_cal_before_pos->DrawCopy("hist");
      c_pos_shms_cal->cd(2);
      h_shms_cal_after_pos->SetLineColor(kRed);
      h_shms_cal_after_pos->DrawCopy("hist");
      std::string c_pos_shms_cal_name = "results/yield/check/shms_cal_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_shms_cal->SaveAs(c_pos_shms_cal_name.c_str());      
      TCanvas* c_pos_shms_aero = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_pos_shms_aero->SetLogy();
      c_pos_shms_aero->Divide(2,1);
      c_pos_shms_aero->cd(1);
      h_shms_aero_before_pos->DrawCopy("hist");
      c_pos_shms_aero->cd(2);
      h_shms_aero_after_pos->SetLineColor(kRed);
      h_shms_aero_after_pos->DrawCopy("hist");
      std::string c_pos_shms_aero_name = "results/yield/check/shms_aero_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_shms_aero->SaveAs(c_pos_shms_aero_name.c_str());      

      TCanvas* c_pos_time_diff = new TCanvas("","time_diff",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_pos_time_diff->Divide(2,1);
      c_pos_time_diff->cd(1);
      h_diff_mod_pos->DrawCopy("hist");
      c_pos_time_diff->cd(2);
      h_difftime_forbg->SetLineColor(kBlue);
      h_difftime_forbg->DrawCopy("hist");
      std::string c_pos_time_diff_name = "results/yield/check/time_diff_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_time_diff->SaveAs(c_pos_time_diff_name.c_str());

      TCanvas* c_pos_mx2 = new TCanvas();
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      auto h_Mx2_pos_before = d_mod_first.Histo1D({"","",100,0,5},"Mx2"); 
      auto h_Mx2_pos_after = d_pos_pi.Histo1D({"","",100,0,5},"Mx2"); 
      h_Mx2_pos_after->SetLineColor(kRed);
      h_Mx2_pos_before->DrawCopy("hist");
      h_Mx2_pos_after->DrawCopy("hist same");
      std::string c_pos_mx2_name = "results/yield/check/mx2_"+std::to_string(RunNumber)+"_pos.pdf";
      c_pos_mx2->SaveAs(c_pos_mx2_name.c_str());


      int bg_counts = *d_pos_bg.Count()/6;
      //jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_counts;

    }

    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      //std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_-1.root";
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
      auto h_hms_dp_before_neg = d_neg_raw.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_before_neg = d_neg_raw.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_before_neg = d_neg_raw.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_before_neg = d_neg_raw.Histo1D({"","HMS cer",100,0,20},"H.cer.npeSum"); 
      auto h_shms_cal_before_neg = d_neg_raw.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_before_neg = d_neg_raw.Histo1D({"","SHMS aero",100,0,15},"P.aero.npeSum");
      auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Filter("P.dc.InsideDipoleExit == 1")
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        //.Filter([&](double current){return std::abs(current-neg_setcurrent)<current_offset;},{"current"})
        ;
    //coin time cut for neg runs
    auto h_cointime_neg = d_neg_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
    int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
    double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
      std::cout<<"coin center"<<coin_peak_center_neg<<std::endl;
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
    
    //rftime cut
      
      auto h_current_before_neg = d_neg_run.Histo1D({"","current",100,3,100},"current");
      TCanvas* c_neg_current = new TCanvas("","coin time",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      h_current_before_neg->DrawCopy("hist");
      h_current_before_neg->GetXaxis()->SetTitle("Current");
      h_current_before_neg->GetYaxis()->SetTitle("Count");
      std::string c_neg_current_name = "results/yield/check/current_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_current->SaveAs(c_neg_current_name.c_str());

      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        .Define("diff_time_mod_beforeshift",[](double difftime){return std::fmod(difftime,4.008);},{"fptime_minus_rf"})
        ;
    auto h_coin_neg = d_neg_run.Histo1D({"",";Coincidence time;Counts",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_coin_negcut = d_neg_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //rftime cut
      //offset
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      double rf_pi_low = j_cuts["rf_cut_low"].get<double>();
      double rf_pi_high = j_cuts["rf_cut_high"].get<double>();
      auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
        return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  
        
      };
      /*
      auto Get_pi_eff = [=](double SHMS_dp){
        double pi_eff;
        int i_order = 0,i_which;
        for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
          if(SHMS_dp>*it){
            i_which = i_order;
            pi_eff = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["pi_eff"].get<double>();
          }
          i_order++;
        }
        return pi_eff;  
      };
      auto Get_pi_purity = [=](double SHMS_dp){
        double pi_purity;
        int i_order = 0,i_which;
        for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
          if(SHMS_dp>*it){
            i_which = i_order;
            pi_purity = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["pi_purity"].get<double>();
          }
          i_order++;
        }
        return pi_purity;  
      };
      */
      //std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
      //jout[(std::to_string(RunNumber)).c_str()]["offset"] = offset_neg;
      auto d_mod_first = d_neg_first
        //.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        //.Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Define("diff_time_mod",[offset_neg](double difftime){return std::fmod(difftime+offset_neg,4.008);},{"fptime_minus_rf"})
        .Define("Pi_eff",Get_pi_eff,{"shms_p"})
        .Define("Pi_purity",Get_neg_pi_purity,{"shms_p"})
        .Define("weight","Pi_purity/Pi_eff")
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
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
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
        .Define("pmiss","P.kin.secondary.pmiss")
        .Define("Mx2",Mx2,{"H.kin.primary.nu","p_pion","P.kin.secondary.pmiss"})
        ;
      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
      std::string skim_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      auto d_neg_pi = d_mod_first
        //.Filter(
        //    [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        //.Filter(Wp2_cut)
        //.Filter(Mx2_cut)
        //.Filter(W2_cut)
        ;
      ROOT::RDF::RSnapshotOptions opts;
      //= {"UPDATE", ROOT::kZLIB, 0, 0, 99, true};
      opts.fMode = "UPDATE";
      d_neg_pi.Snapshot("T",skim_name.c_str(),{"shms_p","xbj","z","Q2","W2","W","Wp","Wp2","emiss","mmiss","InvMass","Mx2","pmiss","weight","P.gtr.th","P.gtr.ph","P.gtr.y","P.gtr.dp","P.gtr.p","H.gtr.dp","P.kin.secondary.th_xq","P.kin.secondary.ph_xq","H.kin.primary.omega"});
      //d_neg_pi.Snapshot("T",skim_name.c_str(),{"xbj","z","Q2","W2","W","Wp","emiss","mmiss","InvMass","weight"});
      std::cout<<"check"<<std::endl;
      int pion_counts = *d_neg_pi.Count();
      std::cout<<"counts "<<pion_counts<<std::endl;

      auto h_hms_dp_after_neg = d_neg_pi.Histo1D({"","HMS dp",100,-15,15},"H.gtr.dp"); 
      auto h_shms_dp_after_neg = d_neg_pi.Histo1D({"","SHMS dp",100,-25,25},"P.gtr.dp"); 
      auto h_hms_cal_after_neg = d_neg_pi.Histo1D({"","HMS cal",100,0.001,2},"H.cal.etottracknorm"); 
      auto h_hms_cer_after_neg = d_neg_pi.Histo1D({"","HMS cer",100,0,20},"H.cer.npeSum"); 
      auto h_shms_cal_after_neg = d_neg_pi.Histo1D({"","SHMS cal",100,0.001,2.5},"P.cal.etottracknorm");
      auto h_shms_aero_after_neg = d_neg_pi.Histo1D({"","SHMS aero",100,0,15},"P.aero.npeSum");

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
      // for bg
      auto d_neg_forbg = d_neg_run
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Define("Pi_eff",Get_pi_eff,{"shms_p"})
        .Define("Pi_purity",Get_neg_pi_purity,{"shms_p"})
        .Define("weight","Pi_purity/Pi_eff")
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_pion"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_pion"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_pion)")
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
        .Define("Mx2",Mx2,{"H.kin.primary.nu","p_pion","P.kin.secondary.pmiss"})
        .Define("pmiss","P.kin.secondary.pmiss")
        ;
      auto h_difftime_forbg = d_neg_forbg.Histo1D({"","diff for bg",100,0,4.008},"diff_time_mod");
      auto d_neg_bg  = d_neg_forbg
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        //.Filter(Mx2_cut)
        //.Filter(Wp2_cut)
        //.Filter(W2_cut)
        ;
      //d_neg_bg.Snapshot("T_bg",skim_name.c_str());
      d_neg_bg.Snapshot("T_bg",skim_name.c_str(),{"shms_p","xbj","z","Q2","W2","W","Wp","Wp2","emiss","mmiss","InvMass","Mx2","pmiss","weight","P.gtr.th","P.gtr.ph","P.gtr.y","P.gtr.dp","P.gtr.p","H.gtr.dp","P.kin.secondary.th_xq","P.kin.secondary.ph_xq","H.kin.primary.omega"},opts);
      //d_neg_bg.Snapshot("T_bg",skim_name.c_str(),{"xbj","z","Q2","W2","W","Wp","emiss","mmiss","InvMass","weight"});
      auto h_coin_neg_bg = d_neg_bg.Histo1D({"","neg bg",800,0,100},"CTime.ePiCoinTime_ROC2");

      TCanvas* c_neg_cointime = new TCanvas("","coin time",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      h_coin_neg->DrawCopy("hist");
      h_coin_negcut->SetLineColor(kRed);
      h_coin_negcut->DrawCopy("hist same");
      h_coin_neg_bg->SetLineColor(kBlue);
      h_coin_neg_bg->DrawCopy("hist same");
      std::string c_neg_cointime_name = "results/yield/check/cointime_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_cointime->SaveAs(c_neg_cointime_name.c_str());
      TCanvas* c_neg_hms_dp = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_neg_hms_dp->Divide(2,1);
      c_neg_hms_dp->cd(1);
      h_hms_dp_before_neg->DrawCopy("hist");
      c_neg_hms_dp->cd(2);
      h_hms_dp_after_neg->SetLineColor(kRed);
      h_hms_dp_after_neg->DrawCopy("hist");
      std::string c_neg_hms_dp_name = "results/yield/check/hms_dp_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_hms_dp->SaveAs(c_neg_hms_dp_name.c_str());      
      TCanvas* c_neg_shms_dp = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_neg_shms_dp->Divide(2,1);
      c_neg_shms_dp->cd(1);
      h_shms_dp_before_neg->DrawCopy("hist");
      c_neg_shms_dp->cd(2);
      h_shms_dp_after_neg->SetLineColor(kRed);
      h_shms_dp_after_neg->DrawCopy("hist");
      std::string c_neg_shms_dp_name = "results/yield/check/shms_dp_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_shms_dp->SaveAs(c_neg_shms_dp_name.c_str());      
      TCanvas* c_neg_hms_cal = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_neg_hms_cal->SetLogy();
      c_neg_hms_cal->Divide(2,1);
      c_neg_hms_cal->cd(1);
      h_hms_cal_before_neg->DrawCopy("hist");
      c_neg_hms_cal->cd(2);
      h_hms_cal_after_neg->SetLineColor(kRed);
      h_hms_cal_after_neg->DrawCopy("hist");
      std::string c_neg_hms_cal_name = "results/yield/check/hms_cal_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_hms_cal->SaveAs(c_neg_hms_cal_name.c_str());      
      TCanvas* c_neg_hms_cer = new TCanvas("","HMS",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_neg_hms_cer->SetLogy();
      c_neg_hms_cer->Divide(2,1);
      c_neg_hms_cer->cd(1);
      h_hms_cer_before_neg->DrawCopy("hist");
      c_neg_hms_cer->cd(2);
      h_hms_cer_after_neg->SetLineColor(kRed);
      h_hms_cer_after_neg->DrawCopy("hist");
      std::string c_neg_hms_cer_name = "results/yield/check/hms_cer_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_hms_cer->SaveAs(c_neg_hms_cer_name.c_str());      
      TCanvas* c_neg_shms_cal = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_neg_shms_cal->SetLogy();
      c_neg_shms_cal->Divide(2,1);
      c_neg_shms_cal->cd(1);
      h_shms_cal_before_neg->DrawCopy("hist");
      c_neg_shms_cal->cd(2);
      h_shms_cal_after_neg->SetLineColor(kRed);
      h_shms_cal_after_neg->DrawCopy("hist");
      std::string c_neg_shms_cal_name = "results/yield/check/shms_cal_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_shms_cal->SaveAs(c_neg_shms_cal_name.c_str());      
      TCanvas* c_neg_shms_aero = new TCanvas("","shms",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      //c_neg_shms_aero->SetLogy();
      c_neg_shms_aero->Divide(2,1);
      c_neg_shms_aero->cd(1);
      h_shms_aero_before_neg->DrawCopy("hist");
      c_neg_shms_aero->cd(2);
      h_shms_aero_after_neg->SetLineColor(kRed);
      h_shms_aero_after_neg->DrawCopy("hist");
      std::string c_neg_shms_aero_name = "results/yield/check/shms_aero_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_shms_aero->SaveAs(c_neg_shms_aero_name.c_str());      

      TCanvas* c_neg_time_diff = new TCanvas("","time_diff",2200,1450);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      c_neg_time_diff->Divide(2,1);
      c_neg_time_diff->cd(1);
      h_diff_mod_neg->DrawCopy("hist");
      c_neg_time_diff->cd(2);
      h_difftime_forbg->SetLineColor(kBlue);
      h_difftime_forbg->DrawCopy("hist");
      std::string c_neg_time_diff_name = "results/yield/check/time_diff_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_time_diff->SaveAs(c_neg_time_diff_name.c_str());

      TCanvas* c_neg_mx2 = new TCanvas();
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
      auto h_Mx2_neg_before = d_mod_first.Histo1D({"","",100,0,5},"Mx2"); 
      auto h_Mx2_neg_after = d_neg_pi.Histo1D({"","",100,0,5},"Mx2"); 
      h_Mx2_neg_after->SetLineColor(kRed);
      h_Mx2_neg_before->DrawCopy("hist");
      h_Mx2_neg_after->DrawCopy("hist same");
      std::string c_neg_mx2_name = "results/yield/check/mx2_"+std::to_string(RunNumber)+"_neg.pdf";
      c_neg_mx2->SaveAs(c_neg_mx2_name.c_str());

      int bg_counts = *d_neg_bg.Count()/6;
    }




  }
}
