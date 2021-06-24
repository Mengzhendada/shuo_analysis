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
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

auto make_negbincont_to_zero = [](TH1D* h){
  for(int bin = 0; bin < h->GetXaxis()->GetNbins();++bin){
    double binContent = h->GetBinContent(bin);
    if(binContent<0){
      h->SetBinContent(bin,0);
      double binCenter = h->GetBinCenter(bin);
      std::cout<<"Negative value "<<binContent<<" for bin "<<bin<<" at "<<binCenter<<std::endl;
      //std::cout<<h->GetBinContent(bin)<<std::endl;
    }
    //else{
    //  h->SetBinContent(bin,binContent);
    //}
  }
  return h;
};
void change_bincontent(TH1D* h1,TH1D* h2){
  for(int bin = 0; bin < h1->GetXaxis()->GetNbins();++bin){
    double binContent1 = h1->GetBinContent(bin);
    double binContent2 = h2->GetBinContent(bin);
    if(binContent1> binContent2){
      std::cout<<"At bin "<<bin<<","<<h1->GetBinContent(bin)<<" greater than "<<h2->GetBinContent(bin)<<std::endl;
      h1->SetBinContent(bin,binContent2);
    }
    //else{
    //  h->SetBinContent(bin,binContent);
    //}
  }
};


void SHMS_DE_Dummy(int RunGroup=0){

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
  json j_rf_DE;
  {
    std::string name = "shuo_analysis/dbase/rf_eff.json";
    std::ifstream ifs(name.c_str());
    ifs>>j_rf_DE;
  }
  std::vector<int> delta_cut_num = j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>();
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
  double current_offset = j_cuts["current_diff"].get<double>();
  std::cout<<Normal_HMS<<std::endl;
  std::cout<<Normal_SHMS<<std::endl;

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
  auto Emiss = [&](Pvec4D& p_pion, Pvec4D p_electron){
    return Eb+M_P-sqrt(p_pion.E()*p_pion.E())-sqrt(p_electron.E()*p_electron.E());
  };
  auto mmiss = [&](Pvec4D& p_pion,Pvec4D p_electron){
    Pvec4D missing_mass = Pvec4D{0.0,0.0,Eb,M_e}+Pvec4D{0.0,0.0,0.0,M_P}-p_electron-p_pion;
    return std::sqrt(missing_mass.M2());
  };

  //rf cut
  auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
    double rf_pi_low = j_DE["SHMS"]["rf_time_low"].get<double>();
    double rf_pi_high = j_DE["SHMS"]["rf_time_high"].get<double>();
    /*
    double rf_pi_low = 0.5,rf_pi_high = 1.3; 
    int i_order = 0,i_which;
    for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
      if(SHMS_dp>*it){
        i_which = i_order;
        rf_pi_low = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_low"].get<double>();
        rf_pi_high = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_high"].get<double>();
      }
      i_order++;
    }
      */
    return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  

  };


  if(!neg_Dummy.empty() && !pos_Dummy.empty()){

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
      //for rf cut  
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
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
        .Filter([&](double current){return current>current_offset;},{"current"})
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
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
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

      //for cal
      auto d_pos_first_aero = d_pos_first
        .Filter(aeroCutSHMS)
        ;
      std::cout<<"check"<<std::endl;

      //for aero
      auto d_pos_first_cal = d_pos_first
        .Filter(piCutSHMS)
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

      //for bg no rf cut, to use for shms cal efficiency  
      auto d_pos_bg_aero= d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Filter(aeroCutSHMS)
        ;
      //for aero bg
      auto d_pos_bg_cal = d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Filter(piCutSHMS)
        ;

      //for cal cuts
      auto d_pos_first_aero_rf  = d_pos_first_aero
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;
      auto d_pos_bg_aero_rf = d_pos_bg_aero
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;

      //for aero cut
      auto d_pos_first_cal_rf = d_pos_first_cal
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;
      auto d_pos_bg_cal_rf = d_pos_bg_cal
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;

      //statistics for DE efficiency
      json jout;
      auto h_cal_pos_norfcut = d_pos_first_aero.Histo1D({"","pos,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_pos_nocalcut = d_pos_first_aero.Histo1D({"","pos,cal,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_cal_pos_norfcut_bg = d_pos_bg_aero.Histo1D({"","pos,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_pos_nocalcut_bg = d_pos_bg_aero.Histo1D({"","pos,cal,norfcut;rftime,counts",100,0,4},"diff_time_mod");
      h_cal_pos_norfcut->Add(h_cal_pos_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_pos_nocalcut->Add(h_rf_pos_nocalcut_bg.GetPtr(),-1.0/6); 
      auto h_cal_pos_pi = d_pos_first_aero_rf.Histo1D({"","pos,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_cal_pos_pi_bg = d_pos_bg_aero_rf.Histo1D({"","pos,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      h_cal_pos_pi->Add(h_cal_pos_pi_bg.GetPtr(),-1.0/6);

      auto h_aero_pos_norfcut = d_pos_first_cal.Histo1D({"","pos,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_pos_noaerocut = d_pos_first_cal.Histo1D({"","pos,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_aero_pos_norfcut_bg = d_pos_bg_cal.Histo1D({"","pos,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_pos_noaerocut_bg = d_pos_bg_cal.Histo1D({"","pos,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      h_aero_pos_norfcut->Add(h_aero_pos_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_pos_noaerocut->Add(h_rf_pos_noaerocut_bg.GetPtr(),-1.0/6); 
      auto h_aero_pos_pi = d_pos_first_cal_rf.Histo1D({"","pos,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      auto h_aero_pos_pi_bg = d_pos_bg_cal_rf.Histo1D({"","pos,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      h_aero_pos_pi->Add(h_aero_pos_pi_bg.GetPtr(),-1.0/6);

      int n_pos_pi_rfcut = h_cal_pos_pi->GetEntries();
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_all"] = n_pos_pi_rfcut;

      int n_pos_pi_rfcalcut = h_aero_pos_pi->GetEntries();
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
      std::string c_DE_cal_name = "results/pid/SHMS_cal_DE_compare_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());

      TCanvas* c_DE_aero = new TCanvas();
      c_DE_aero->Divide(2,1);
      c_DE_aero->cd(1);
      TVirtualPad* Pad_rf_aerocuts = c_DE_aero->cd(1);
      //Pad_rf_aerocuts->SetLogy();
      h_rf_pos_noaerocut->SetBit(TH1::kNoStats);
      h_rf_pos_noaerocut->SetMarkerStyle(8);
      h_rf_pos_noaerocut->DrawCopy("hist");
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
      std::string c_DE_aero_name = "results/pid/SHMS_aero_DE_compare_"+std::to_string(RunNumber)+".pdf";
      c_DE_aero->SaveAs(c_DE_aero_name.c_str());

      //for cal de vs z
      auto h_cal_should_z = d_pos_first_aero_rf.Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      auto h_cal_should_z_bg = d_pos_bg_aero_rf.Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      //for(int bin = 0;bin < h_cal_should_z->GetXaxis()->GetNbins();++bin){
      //  double should_content = h_cal_should_z->GetBinContent(bin);
      //  double should_bg_content = h_cal_should_z_bg->GetBinContent(bin);
      //  double sub = should_content-should_bg_content;
      //  if(sub < 0){
      //    
      //  }
      //}
      h_cal_should_z->Add(h_cal_should_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_cal_should_z.GetPtr());
      double SHMS_cal_low_use = j_cuts["P_cal_pi_low"].get<double>();
      double SHMS_cal_high_use = j_cuts["P_cal_pi_high"].get<double>();
      std::string piCutSHMS_use = (" P.cal.etottracknorm > "+std::to_string(SHMS_cal_low_use)+" && P.cal.etottracknorm < " + std::to_string(SHMS_cal_high_use)).c_str();
      auto h_cal_did_z = d_pos_first_aero_rf.Filter(piCutSHMS_use).Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      auto h_cal_did_z_bg = d_pos_bg_aero_rf.Filter(piCutSHMS_use).Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      h_cal_did_z->Add(h_cal_did_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_cal_did_z.GetPtr());
      change_bincontent(h_cal_did_z.GetPtr(),h_cal_should_z.GetPtr()); 


      TEfficiency* P_cal_eff = 0;
      std::string pos_file_name = "results/yield/kinematics_eff_"+std::to_string(RunNumber)+".root";
      TFile* pos_file = new TFile(pos_file_name.c_str(),"recreate");
      if(TEfficiency::CheckConsistency(*h_cal_did_z,*h_cal_should_z)){
        P_cal_eff = new TEfficiency(*h_cal_did_z,*h_cal_should_z);
        TCanvas* c_cal_eff = new TCanvas();
        //h_cal_should_z->DrawCopy("hist");
        //h_cal_did_z->SetLineColor(kRed);
        //h_cal_did_z->DrawCopy("hist same");
        P_cal_eff->Draw("ap");
        std::string c_cal_eff_name = "results/pid/SHMS_cal_DE_"+std::to_string(RunNumber)+".pdf";
        c_cal_eff->SaveAs(c_cal_eff_name.c_str());
        //P_cal_eff->Write();
        pos_file->GetList()->Add(P_cal_eff);
        // P_cal_eff->SetDirectory(gDirectory);
       // pos_file->Write();
        std::cout<<"Cal eff done"<<std::endl;
      }


      auto h_aero_should_z = d_pos_first_cal_rf.Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      auto h_aero_should_z_bg = d_pos_bg_cal_rf.Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      h_aero_should_z->Add(h_aero_should_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_aero_should_z.GetPtr());
      //TH1D* h_aero_should_z_updated_2 = make_negbincont_to_zero(h_aero_should_z_updated);
      double P_aero_use = j_cuts["P_aero"].get<double>();
      std::string aeroCutSHMS_use = (" P.aero.npeSum > "+std::to_string(P_aero_use)).c_str();
      auto h_aero_did_z = d_pos_first_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      auto h_aero_did_z_bg = d_pos_bg_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      h_aero_did_z->Add(h_aero_did_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_aero_did_z.GetPtr());
      change_bincontent(h_aero_did_z.GetPtr(),h_aero_should_z.GetPtr()); 
      //std::string pos_file_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TEfficiency* P_aero_eff = 0;
      //TGraphAsymmErrors P_aero_eff = 0;
      if(TEfficiency::CheckConsistency(*h_aero_did_z,*h_aero_should_z)){
        //if(TEfficiency::CheckConsistency(*h_aero_did_z_updated,*h_aero_should_z_updated)){
        //auto P_aero_eff = new TGraphAsymmErrors(h_aero_did_z_updated,h_aero_should_z_updated);
        P_aero_eff = new TEfficiency(*h_aero_did_z,*h_aero_should_z);
        //P_aero_eff = new TEfficiency(*h_aero_did_z_updated,*h_aero_should_z_updated);
        //P_aero_eff->SetDirectory(gDirectory);
        pos_file->GetList()->Add(P_aero_eff);
        pos_file->Write();
        //TFile* pos_file = new TFile(pos_file_name.c_str(),"recreate");
        //P_aero_eff->Write();
        std::cout<<"Aero eff done"<<std::endl;
      }
      
      TCanvas* c_aero_eff = new TCanvas();
      h_aero_should_z->DrawCopy("hist");
      h_aero_did_z->SetLineColor(kRed);
      h_aero_did_z->DrawCopy("hist same");
      //P_aero_eff->Draw("ap");
      std::string c_aero_eff_name = "results/pid/SHMS_aero_DE_"+std::to_string(RunNumber)+"_counts.pdf";
      //std::string c_aero_eff_name = "results/pid/SHMS_aero_DE_"+std::to_string(RunNumber)+".pdf";
      c_aero_eff->SaveAs(c_aero_eff_name.c_str());

      //For some checks
//      auto h_coin = d_pos_run.Filter(piCutSHMS).Filter(rf_cut,{"P.gtr.dp","diff_time_mod"}).Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2");
//      auto h_aero_should_coin = d_pos_first_cal_rf.Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2");
//      auto h_aero_should_bg_coin = d_pos_bg_cal_rf.Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2");
//      auto h_aero_did_coin = d_pos_first_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2");
//      auto h_aero_did_bg_coin = d_pos_bg_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"","",500,0,100},"CTime.ePiCoinTime_ROC2");
//      TCanvas *c_coin_check = new TCanvas();
//      h_coin->SetLineColor(1);
//      h_coin->DrawCopy("hist");
//      h_aero_should_coin->DrawCopy("hist same");
//      h_aero_should_bg_coin->DrawCopy("hist same");
//      h_aero_did_coin->SetLineColor(kRed);
//      h_aero_did_coin->DrawCopy("hist same");
//      h_aero_did_bg_coin->SetLineColor(kRed);
//      h_aero_did_bg_coin->DrawCopy("hist same");
//      std::string c_coin_check_name = "results/pid/DE_coin_check.pdf";
//      c_coin_check->SaveAs(c_coin_check_name.c_str());

    }//end of pos runs

    //loop over each neg runs data
    for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
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
        .Filter([&](double current){return current>current_offset;},{"current"})
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
        .Define("emiss",Emiss,{"p_pion","p_electron"})
        .Define("mmiss",mmiss,{"p_pion","p_electron"})
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
      //for rf cut  
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      auto d_neg_first = d_neg_run
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;

      //for cal
      auto d_neg_first_aero = d_neg_first
        .Filter(aeroCutSHMS)
        ;
      std::cout<<"check"<<std::endl;

      //for aero
      auto d_neg_first_cal = d_neg_first
        .Filter(piCutSHMS)
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
      //std::cout<<bg_cut<<std::endl;

      //for bg no rf cut, to use for shms cal efficiency  
      auto d_neg_first_aero_bg= d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;
      //for aero bg
      auto d_neg_first_cal_bg = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(piCutSHMS)
        ;

      //for cal cuts
      auto d_neg_bg_aero_rf  = d_neg_first_aero
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;
      auto d_neg_bg_aero_rf_bg = d_neg_first_aero_bg
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;

      //for aero cut
      auto d_neg_first_cal_rf = d_neg_first_cal
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;
      auto d_neg_bg_cal_rf = d_neg_first_cal_bg
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        ;

      //statistics for DE efficiency
      json jout;
      auto h_cal_neg_norfcut = d_neg_first_aero.Histo1D({"","neg,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_neg_nocalcut = d_neg_first_aero.Histo1D({"","neg,cal,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_cal_neg_norfcut_bg = d_neg_first_aero_bg.Histo1D({"","neg,cal,norfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_rf_neg_nocalcut_bg = d_neg_first_aero_bg.Histo1D({"","neg,cal,norfcut;rftime,counts",100,0,4},"diff_time_mod");
      h_cal_neg_norfcut->Add(h_cal_neg_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_neg_nocalcut->Add(h_rf_neg_nocalcut_bg.GetPtr(),-1.0/6); 
      auto h_cal_neg_pi = d_neg_bg_aero_rf.Histo1D({"","neg,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      auto h_cal_neg_pi_bg = d_neg_bg_aero_rf_bg.Histo1D({"","neg,cal,e_rfcut;calorimeter;counts",100,0.001,2},"P.cal.etottracknorm");
      h_cal_neg_pi->Add(h_cal_neg_pi_bg.GetPtr(),-1.0/6);

      auto h_aero_neg_norfcut = d_neg_first_cal.Histo1D({"","neg,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_neg_noaerocut = d_neg_first_cal.Histo1D({"","neg,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      auto h_aero_neg_norfcut_bg = d_neg_first_cal_bg.Histo1D({"","neg,aero,norfcut;aeroNpeSum;counts",200,0,50},"P.aero.npeSum");
      auto h_rf_neg_noaerocut_bg = d_neg_first_cal_bg.Histo1D({"","neg,rf,norfcut;rftime;counts",100,0,4},"diff_time_mod");
      h_aero_neg_norfcut->Add(h_aero_neg_norfcut_bg.GetPtr(),-1.0/6); 
      h_rf_neg_noaerocut->Add(h_rf_neg_noaerocut_bg.GetPtr(),-1.0/6); 
      auto h_aero_neg_pi = d_neg_first_cal_rf.Histo1D({"","neg,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      auto h_aero_neg_pi_bg = d_neg_bg_cal_rf.Histo1D({"","neg,aero,pi_rfcut",200,0,50},"P.aero.npeSum");
      h_aero_neg_pi->Add(h_aero_neg_pi_bg.GetPtr(),-1.0/6);

      int n_neg_pi_rfcut = h_cal_neg_pi->GetEntries();
      jout[std::to_string(RunNumber)]["SHMS"]["rfcut"]["pi_all"] = n_neg_pi_rfcut;

      int n_neg_pi_rfcalcut = h_aero_neg_pi->GetEntries();
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
      std::string c_DE_cal_name = "results/pid/SHMS_cal_DE_compare_"+std::to_string(RunNumber)+".pdf";
      c_DE_cal->SaveAs(c_DE_cal_name.c_str());

      TCanvas* c_DE_aero = new TCanvas();
      c_DE_aero->Divide(2,1);
      c_DE_aero->cd(1);
      TVirtualPad* Pad_rf_aerocuts = c_DE_aero->cd(1);
      //Pad_rf_aerocuts->SetLogy();
      h_rf_neg_noaerocut->SetBit(TH1::kNoStats);
      h_rf_neg_noaerocut->SetMarkerStyle(8);
      h_rf_neg_noaerocut->DrawCopy("hist");
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
      std::string c_DE_aero_name = "results/pid/SHMS_aero_DE_compare_"+std::to_string(RunNumber)+".pdf";
      c_DE_aero->SaveAs(c_DE_aero_name.c_str());


      //for cal de vs z
      auto h_cal_should_z = d_neg_bg_aero_rf.Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      auto h_cal_should_z_bg = d_neg_bg_aero_rf.Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      //for(int bin = 0;bin < h_cal_should_z->GetXaxis()->GetNbins();++bin){
      //  double should_content = h_cal_should_z->GetBinContent(bin);
      //  double should_bg_content = h_cal_should_z_bg->GetBinContent(bin);
      //  double sub = should_content-should_bg_content;
      //  if(sub < 0){
      //    
      //  }
      //}
      h_cal_should_z->Add(h_cal_should_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_cal_should_z.GetPtr());
      double SHMS_cal_low_use = j_cuts["P_cal_pi_low"].get<double>();
      double SHMS_cal_high_use = j_cuts["P_cal_pi_high"].get<double>();
      std::string piCutSHMS_use = (" P.cal.etottracknorm > "+std::to_string(SHMS_cal_low_use)+" && P.cal.etottracknorm < " + std::to_string(SHMS_cal_high_use)).c_str();
      auto h_cal_did_z = d_neg_bg_aero_rf.Filter(piCutSHMS_use).Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      auto h_cal_did_z_bg = d_neg_bg_aero_rf.Filter(piCutSHMS_use).Histo1D({"cal_eff","calorimeter_eff;z;cal eff",100,0,1},"z");
      h_cal_did_z->Add(h_cal_did_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_cal_did_z.GetPtr());
      change_bincontent(h_cal_did_z.GetPtr(),h_cal_should_z.GetPtr()); 


      TEfficiency* P_cal_eff = 0;
      std::string neg_file_name = "results/yield/kinematics_eff_"+std::to_string(RunNumber)+".root";
      TFile* neg_file = new TFile(neg_file_name.c_str(),"recreate");
      if(TEfficiency::CheckConsistency(*h_cal_did_z,*h_cal_should_z)){
        P_cal_eff = new TEfficiency(*h_cal_did_z,*h_cal_should_z);
        TCanvas* c_cal_eff = new TCanvas();
        //h_cal_should_z->DrawCopy("hist");
        //h_cal_did_z->SetLineColor(kRed);
        //h_cal_did_z->DrawCopy("hist same");
        P_cal_eff->Draw("ap");
        std::string c_cal_eff_name = "results/pid/SHMS_cal_DE_"+std::to_string(RunNumber)+".pdf";
        c_cal_eff->SaveAs(c_cal_eff_name.c_str());
        //P_cal_eff->Write();
        neg_file->GetList()->Add(P_cal_eff);
        // P_cal_eff->SetDirectory(gDirectory);
       // neg_file->Write();
        std::cout<<"Cal eff done"<<std::endl;
      }


      auto h_aero_should_z = d_neg_first_cal_rf.Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      auto h_aero_should_z_bg = d_neg_bg_cal_rf.Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      h_aero_should_z->Add(h_aero_should_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_aero_should_z.GetPtr());
      //TH1D* h_aero_should_z_updated_2 = make_negbincont_to_zero(h_aero_should_z_updated);
      double P_aero_use = j_cuts["P_aero"].get<double>();
      std::string aeroCutSHMS_use = (" P.aero.npeSum > "+std::to_string(P_aero_use)).c_str();
      auto h_aero_did_z = d_neg_first_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      auto h_aero_did_z_bg = d_neg_bg_cal_rf.Filter(aeroCutSHMS_use).Histo1D({"aero_eff","aero_eff;z;aero eff",100,0,1},"z");
      h_aero_did_z->Add(h_aero_did_z_bg.GetPtr(),-1.0/6);
      make_negbincont_to_zero(h_aero_did_z.GetPtr());
      change_bincontent(h_aero_did_z.GetPtr(),h_aero_should_z.GetPtr()); 
      //std::string neg_file_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TEfficiency* P_aero_eff = 0;
      //TGraphAsymmErrors P_aero_eff = 0;
      if(TEfficiency::CheckConsistency(*h_aero_did_z,*h_aero_should_z)){
        //if(TEfficiency::CheckConsistency(*h_aero_did_z_updated,*h_aero_should_z_updated)){
        //auto P_aero_eff = new TGraphAsymmErrors(h_aero_did_z_updated,h_aero_should_z_updated);
        P_aero_eff = new TEfficiency(*h_aero_did_z,*h_aero_should_z);
        //P_aero_eff = new TEfficiency(*h_aero_did_z_updated,*h_aero_should_z_updated);
        //P_aero_eff->SetDirectory(gDirectory);
        neg_file->GetList()->Add(P_aero_eff);
        neg_file->Write();
        //TFile* neg_file = new TFile(neg_file_name.c_str(),"recreate");
        //P_aero_eff->Write();
        std::cout<<"Aero eff done"<<std::endl;
      }
      
      TCanvas* c_aero_eff = new TCanvas();
      //h_aero_should_z->DrawCopy("hist");
      //h_aero_did_z->SetLineColor(kRed);
      //h_aero_did_z->DrawCopy("hist same");
      P_aero_eff->Draw("ap");
      std::string c_aero_eff_name = "results/pid/SHMS_aero_DE_"+std::to_string(RunNumber)+".pdf";
      c_aero_eff->SaveAs(c_aero_eff_name.c_str());

    }//end of neg runs

  }//if normal production runs

}
