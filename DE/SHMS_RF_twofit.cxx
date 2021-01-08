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
#include "TPaveText.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"
double GeV = 1.78266192e-27;
double c = 299792458;
double t_e(double p){
  double m = 0.00051;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_pi(double p){
  double m = 0.139;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K(double p){
  double m = 0.493;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_proton(double p){
  double m = 0.938;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}

void SHMS_RF_twofit(int RunGroup = 0){
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
  json j_rungroup_info;

  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double shms_p = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
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

  double rf_pi_low = j_cuts["rf_pi_low"].get<double>();
  std::cout<<rf_pi_low<<std::endl;
  double rf_pi_high =j_cuts["rf_pi_high"].get<double>();
  std::cout<<rf_pi_high<<std::endl;

  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;

    double cointime_lowcut,cointime_highcut;
    if(RunGroup<410){
      cointime_lowcut = j_cuts["cointime_low_fall"].get<double>();
      cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
    }
    else{
      cointime_lowcut = j_cuts["cointime_low_spring"].get<double>();
      cointime_highcut = j_cuts["cointime_high_spring"].get<double>();
    }

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
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
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
    //cointime cut
    double cointime_low_pos = coin_peak_center_pos+cointime_lowcut;
    double cointime_high_pos = coin_peak_center_pos+cointime_highcut;
    auto d_pos_first = d_pos
      .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"});
    auto h_coin_pos = d_pos.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
    auto h_coin_poscut_rungroup = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

    TH1D* h_rf_pos_Kall = new TH1D("",";rftime;counts",100,0,4);
    TH1D* h_rf_neg_Kall = new TH1D("",";rftime;counts",100,0,4);

    TH1D* h_rf_pos_piall = new TH1D("",";rftime;counts",100,0,4);
    TH1D* h_rf_neg_piall = new TH1D("",";rftime;counts",100,0,4);

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
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
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
      //rftime cut
      auto h_time_diff_pos = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
      auto h_time_diff_poscheck = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,-100,100},"fptime_minus_rf");
      int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
      double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
      //offset
      double offset_pos = 401.8-time_diff_pos_max;
      std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        //    .Filter(aeroCutSHMS)
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

      auto d_pos_bg_norfcut = d_pos_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        //  .Filter(aeroCutSHMS)
        ;


      auto d_pos_piall  = d_mod_first
        ;
      auto d_pos_Kall = d_mod_first
        //.Filter("P.aero.npeSum<10")
        .Filter("P.hgcer.npeSum<2")
        //.Filter(piCutSHMS)
        ;
      auto d_pos_piall_bg = d_pos_bg_norfcut
        ;
      auto d_pos_Kall_bg = d_pos_bg_norfcut
        //.Filter("P.aero.npeSum<10")
        .Filter("P.hgcer.npeSum<2")
        //.Filter(piCutSHMS)
        ;

      //not really necessary to check this histogram. 
      auto h_coin_pos_piall = d_pos_piall.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_pos_piall_bg = d_pos_piall_bg.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      auto h_rf_pos = d_pos_piall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
      auto h_rf_pos_bg = d_pos_piall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
      h_rf_pos->Add(h_rf_pos_bg.GetPtr(),-1.0/6); 
      h_rf_pos_piall->Add(h_rf_pos.GetPtr(),1);

      auto h_rf_pos_K = d_pos_Kall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
      auto h_rf_pos_K_bg = d_pos_Kall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
      h_rf_pos_K->Add(h_rf_pos_K_bg.GetPtr(),-1.0/6); 
      h_rf_pos_Kall->Add(h_rf_pos_K.GetPtr(),1);
    }
    double time_diff = t_K(shms_p) - t_pi(shms_p);
    double time_diff_proton = t_proton(shms_p) - t_pi(shms_p);
    std::cout<<"time for kaon "<<time_diff<<std::endl;
    double par_pos[6];
    TCanvas *c_rftime_pos = new TCanvas();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    //c_rftime_pos->SetGrid();
    h_rf_pos_Kall->DrawCopy("hist");
    TF1 *f1_pos = new TF1("pi fit","gaus",0.5,1.5);
    TF1 *f2_pos = new TF1("K fit","gaus",1+0.5*time_diff,1+2*time_diff);
    f1_pos->SetLineColor(kRed);
    f2_pos->SetLineColor(kOrange);
    TF1 *all_pos = new TF1("all fit","gaus(0)+gaus(3)",0.5,1+2*time_diff);
    all_pos->SetLineColor(1);
    f1_pos->FixParameter(1,1);
    f2_pos->FixParameter(1,1+time_diff);
    h_rf_pos_Kall->Fit(f1_pos,"R");
    h_rf_pos_Kall->Fit(f2_pos,"R+");
    f1_pos->GetParameters(&par_pos[0]);
    f2_pos->GetParameters(&par_pos[3]);
    all_pos->SetParameters(par_pos);
    h_rf_pos_Kall->Fit(all_pos,"R+");
    TPaveText* pt_pos = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_pos->AddText(("RunGroup pos K "+std::to_string(RunGroup)).c_str());
    pt_pos->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_pos->AddText(("Pi: p0 = "+std::to_string(par_pos[0])).c_str());
    pt_pos->AddText(("p1 = "+std::to_string(par_pos[1])).c_str());
    pt_pos->AddText(("p3 = "+std::to_string(par_pos[2])).c_str());
    pt_pos->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_pos->AddText(("K: p0 = "+std::to_string(par_pos[3])).c_str());
    pt_pos->AddText(("p1 = "+std::to_string(par_pos[4])).c_str());
    pt_pos->AddText(("p3 = "+std::to_string(par_pos[5])).c_str());
    pt_pos->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_pos->Draw();
    //std::string f1_pos_string = std::to_string(par_pos[0])+"e("+"#frac{(x-"+std::to_string(par_pos[1])+")^2}{2("+std::to_string(par_pos[2])+")^2})";
    //std::string f1_pos_string = "K_{S}... K^{*0}... #frac{2s}{#pi#alpha^{2}} #frac{d#sigma}{dcos#theta} (e^{+}e^{-} #rightarrow f#bar{f} ) = #left| #frac{1}{1 - #Delta#alpha} #right|^{2} (1+cos^{2}#theta)";
    //std::string f2_pos_string = std::to_string(par_pos[3])+"e("+"#frac{(x-"+std::to_string(par_pos[4])+")^2}{2*"+std::to_string(par_pos[5])+"^2})";
    //std::cout<<f1_pos_string<<std::endl;
    //TLatex *K_pos_tex;
    //K_pos_tex->DrawLatex(0.75,0.6,f1_pos_string.c_str());
    //K_pos_tex->DrawLatex(0.75,0.5,f2_pos_string.c_str());
    c_rftime_pos->Update();
    std::string c_rftime_pos_name = "results/pid/rftime/rftime_pos_"+std::to_string(RunGroup)+".pdf";
    c_rftime_pos->SaveAs(c_rftime_pos_name.c_str());



    double par_pos_pi[6];
    TCanvas *c_pi_pos = new TCanvas();
    //c_pi_pos->SetGrid();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    h_rf_pos_piall->DrawCopy("hist");
    TF1 *f1_pos_pi = new TF1("pi fit","gaus",0.5,1.5);
    TF1 *f2_pos_K = new TF1("K fit","gaus",1+0.5*time_diff,1+2*time_diff);
    f1_pos_pi->SetLineColor(kRed);
    f2_pos_K->SetLineColor(kOrange);
    TF1 *all_pos_pi = new TF1("all fit","gaus(0)+gaus(3)",0.5,1+2*time_diff);
    all_pos_pi->SetLineColor(1);
    f1_pos_pi->FixParameter(1,1);
    f2_pos_K->FixParameter(1,par_pos[3]);
    f2_pos_K->FixParameter(2,par_pos[4]);
    f2_pos_K->FixParameter(3,par_pos[5]);
    h_rf_pos_piall->Fit(f1_pos_pi,"R");
    h_rf_pos_piall->Fit(f2_pos_K,"R+");
    f1_pos_pi->GetParameters(&par_pos_pi[0]);
    f2_pos_K->GetParameters(&par_pos_pi[3]);
    all_pos_pi->SetParameters(par_pos_pi);
    h_rf_pos_piall->Fit(all_pos_pi,"R+");
    TLine *l_rf_low_pos = new TLine(rf_pi_low,0,rf_pi_low,1000);
    TLine *l_rf_high_pos = new TLine(rf_pi_high,0,rf_pi_high,1000);
    l_rf_low_pos->Draw("same");
    l_rf_high_pos->Draw("same");
    double width = h_rf_pos_piall->GetXaxis()->GetBinWidth(1);
    std::cout<<"Bin width "<<width<<std::endl;
    double pos_pi_N = all_pos_pi->Integral(rf_pi_low,rf_pi_high,width);
    //double pos_pi_N = f1_pos_pi->Integral(rf_pi_low,rf_pi_high,width);
    double pos_K_N = f2_pos_K->Integral(rf_pi_low,rf_pi_high,width);
    std::cout<<"Pi number in fitting "<<pos_pi_N<<"K number in fitting "<<pos_K_N<<std::endl;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["rf_cut_eff"] = 1-pos_K_N/pos_pi_N;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["pi"] = pos_pi_N;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["K"] = pos_K_N;
    TPaveText* pt_pos_pi = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_pos_pi->AddText(("RunGroup pos pi "+std::to_string(RunGroup)).c_str());
    pt_pos_pi->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_pos_pi->AddText(("Pi: p0 = "+std::to_string(par_pos_pi[0])).c_str());
    pt_pos_pi->AddText(("p1 = "+std::to_string(par_pos_pi[1])).c_str());
    pt_pos_pi->AddText(("p3 = "+std::to_string(par_pos_pi[2])).c_str());
    pt_pos_pi->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_pos_pi->AddText(("K: p0 = "+std::to_string(par_pos_pi[3])).c_str());
    pt_pos_pi->AddText(("p1 = "+std::to_string(par_pos_pi[4])).c_str());
    pt_pos_pi->AddText(("p3 = "+std::to_string(par_pos_pi[5])).c_str());
    pt_pos_pi->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_pos_pi->Draw();
    std::string f1_pos_pi_string = std::to_string(par_pos_pi[0])+"*exp("+"\\frac{(x-"+std::to_string(par_pos_pi[1])+")^2}{2*"+std::to_string(par_pos_pi[2])+"^2})";
    std::string f2_pos_K_string = std::to_string(par_pos_pi[3])+"*exp("+"\\frac{(x-"+std::to_string(par_pos_pi[4])+")^2}{2*"+std::to_string(par_pos_pi[5])+"^2})";
    //TLatex *pi_pos_tex;
    //pi_pos_tex->DrawLatex(0.75,0.6,f1_pos_string.c_str());
    //pi_pos_tex->DrawLatex(0.75,0.5,f2_pos_string.c_str());
    c_pi_pos->Update();
    std::string c_pi_pos_name = "results/pid/rftime/rftime_pos_"+std::to_string(RunGroup)+"_pi.pdf";
    c_pi_pos->SaveAs(c_pi_pos_name.c_str());


    TCanvas *c_pi_pos_2nd = new TCanvas();
    //c_pi_pos_2nd->SetGrid();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    h_rf_pos_piall->DrawCopy("hist");
    f1_pos_pi->GetParameters(&par_pos_pi[0]);
    f2_pos_K->GetParameters(&par_pos_pi[3]);
    double sigma_pi_pos = par_pos_pi[2];
    TLine *l_rf_lowsigma_pos = new TLine(1-3*sigma_pi_pos,0,1-3*sigma_pi_pos,1000);
    TLine *l_rf_highsigma_pos = new TLine(1+3*sigma_pi_pos,0,1+3*sigma_pi_pos,1000);
    l_rf_lowsigma_pos->Draw("same");
    l_rf_highsigma_pos->Draw("same");
    double pos_pi_N_sigma = all_pos_pi->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width);
    //double pos_pi_N_sigma = f1_pos_pi->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width);
    double pos_K_N_sigma = f2_pos_K->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width);
    std::cout<<"Pi number in fitting "<<pos_pi_N_sigma<<"K number in fitting "<<pos_K_N_sigma<<std::endl;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["rf_cut_eff_sigma"] = 1-pos_K_N_sigma/pos_pi_N_sigma;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["pi_sigma"] = pos_pi_N_sigma;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["pos"]["K_sigma"] = pos_K_N_sigma;
    TPaveText* pt_pos_pi_2nd = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_pos_pi_2nd->AddText(("RunGroup pos pi in 3 sigma "+std::to_string(RunGroup)).c_str());
    pt_pos_pi_2nd->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_pos_pi_2nd->AddText(("Pi: p0 = "+std::to_string(par_pos_pi[0])).c_str());
    pt_pos_pi_2nd->AddText(("p1 = "+std::to_string(par_pos_pi[1])).c_str());
    pt_pos_pi_2nd->AddText(("p3 = "+std::to_string(par_pos_pi[2])).c_str());
    pt_pos_pi_2nd->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_pos_pi_2nd->AddText(("K: p0 = "+std::to_string(par_pos_pi[3])).c_str());
    pt_pos_pi_2nd->AddText(("p1 = "+std::to_string(par_pos_pi[4])).c_str());
    pt_pos_pi_2nd->AddText(("p3 = "+std::to_string(par_pos_pi[5])).c_str());
    pt_pos_pi_2nd->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_pos_pi_2nd->Draw();
    //pi_pos_tex->DrawLatex(0.75,0.6,f1_pos_string.c_str());
    //pi_pos_tex->DrawLatex(0.75,0.5,f2_pos_string.c_str());
    c_pi_pos_2nd->Update();
    std::string c_pi_pos_2nd_name = "results/pid/rftime/rftime_pos_"+std::to_string(RunGroup)+"_pi_2nd.pdf";
    c_pi_pos_2nd->SaveAs(c_pi_pos_2nd_name.c_str());
    
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
      .Filter(piCutSHMS)
      .Filter(eCutHMS)
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
      auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        .Filter(aeroCutSHMS)
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
      //rftime cut for neg
      auto h_time_diff_neg = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
      auto h_time_diff_negcheck = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,-100,100},"fptime_minus_rf");
      int time_diff_neg_bin_max = h_time_diff_neg->GetMaximumBin();
      double time_diff_neg_max = h_time_diff_neg->GetBinCenter(time_diff_neg_bin_max);
      double offset_neg = 401.8-time_diff_neg_max;
      std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
      auto d_mod_first = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        //.Filter(aeroCutSHMS)
        ;

      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");


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
        //.Filter(aeroCutSHMS)
        ;


      auto d_neg_piall  = d_mod_first
        ;
      auto d_neg_Kall = d_mod_first
        //.Filter("P.aero.npeSum<10")
        .Filter("P.hgcer.npeSum<2")
        //.Filter(piCutSHMS)
        ;
      auto d_neg_piall_bg = d_neg_bg_norfcut
        ;
      auto d_neg_Kall_bg = d_neg_bg_norfcut
        //.Filter("P.aero.npeSum<10")
        .Filter("P.hgcer.npeSum<2")
        //.Filter(piCutSHMS)
        ;

      //not really necessary to check this histogram. 
      auto h_coin_neg_piall = d_neg_piall.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_neg_piall_bg = d_neg_piall_bg.Histo1D({"","neg eall",800,0,100},"CTime.ePiCoinTime_ROC2");

      //statistics for DE efficiency
      auto h_rf_neg = d_neg_piall.Histo1D({"","neg,rftime,norfcut",100,0,4},"diff_time_mod");
      auto h_rf_neg_bg = d_neg_piall_bg.Histo1D({"","neg,cal,norfcut",100,0,4},"diff_time_mod");
      h_rf_neg->Add(h_rf_neg_bg.GetPtr(),-1.0/6); 
      h_rf_neg_piall->Add(h_rf_neg.GetPtr(),1);

      auto h_rf_neg_K = d_neg_Kall.Histo1D({"","neg,rftime,norfcut",100,0,4},"diff_time_mod");
      auto h_rf_neg_K_bg = d_neg_Kall_bg.Histo1D({"","neg,cal,norfcut",100,0,4},"diff_time_mod");
      h_rf_neg_K->Add(h_rf_neg_K_bg.GetPtr(),-1.0/6); 
      h_rf_neg_Kall->Add(h_rf_neg_K.GetPtr(),1);
    }

    double par_neg[6];
    TCanvas *c_rftime_neg = new TCanvas();
    //c_rftime_neg->SetGrid();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    h_rf_neg_Kall->DrawCopy("hist");
    TF1 *f1_neg = new TF1("pi fit","gaus",0.5,1.5);
    TF1 *f2_neg = new TF1("K fit","gaus",1+0.5*time_diff,1+2*time_diff);
    f1_neg->SetLineColor(kRed);
    f2_neg->SetLineColor(kOrange);
    TF1 *all_neg = new TF1("all fit","gaus(0)+gaus(3)",0,1+2*time_diff);
    all_neg->SetLineColor(1);
    f1_neg->FixParameter(1,1);
    f2_neg->FixParameter(1,1+time_diff);
    h_rf_neg_Kall->Fit(f1_neg,"R");
    h_rf_neg_Kall->Fit(f2_neg,"R+");
    f1_neg->GetParameters(&par_neg[0]);
    f2_neg->GetParameters(&par_neg[3]);
    all_neg->SetParameters(par_neg);
    h_rf_neg_Kall->Fit(all_neg,"R+");
    TPaveText* pt_neg = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_neg->AddText(("RunGroup neg K "+std::to_string(RunGroup)).c_str());
    pt_neg->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_neg->AddText(("Pi: p0 = "+std::to_string(par_neg[0])).c_str());
    pt_neg->AddText(("p1 = "+std::to_string(par_neg[1])).c_str());
    pt_neg->AddText(("p3 = "+std::to_string(par_neg[2])).c_str());
    pt_neg->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_neg->AddText(("K: p0 = "+std::to_string(par_neg[3])).c_str());
    pt_neg->AddText(("p1 = "+std::to_string(par_neg[4])).c_str());
    pt_neg->AddText(("p3 = "+std::to_string(par_neg[5])).c_str());
    pt_neg->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_neg->Draw();
    std::string f1_neg_string = std::to_string(par_neg[0])+"*exp("+"\\frac{(x-"+std::to_string(par_neg[1])+")^2}{2*"+std::to_string(par_neg[2])+"^2})";
    std::string f2_neg_string = std::to_string(par_neg[3])+"*exp("+"\\frac{(x-"+std::to_string(par_neg[4])+")^2}{2*"+std::to_string(par_neg[5])+"^2})";
    //TLatex *K_neg_tex;
    //K_neg_tex->DrawLatex(0.75,0.6,f1_neg_string.c_str());
    //K_neg_tex->DrawLatex(0.75,0.5,f2_neg_string.c_str());
    c_rftime_neg->Update();
    std::string c_rftime_neg_name = "results/pid/rftime/rftime_neg_"+std::to_string(RunGroup)+".pdf";
    c_rftime_neg->SaveAs(c_rftime_neg_name.c_str());

    double par_neg_pi[6];
    TCanvas *c_pi_neg = new TCanvas();
    //c_pi_neg->SetGrid();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    h_rf_neg_piall->DrawCopy("hist");
    TF1 *f1_neg_pi = new TF1("pi fit","gaus",0.5,1.5);
    TF1 *f2_neg_K = new TF1("K fit","gaus",1+0.5*time_diff,1+2*time_diff);
    f1_neg_pi->SetLineColor(kRed);
    f2_neg_K->SetLineColor(kOrange);
    TF1 *all_neg_pi = new TF1("all fit","gaus(0)+gaus(3)",0.5,1+2*time_diff);
    all_neg_pi->SetLineColor(1);
    f1_neg_pi->FixParameter(1,1);
    f2_neg_K->FixParameter(1,par_neg[3]);
    f2_neg_K->FixParameter(2,par_neg[4]);
    f2_neg_K->FixParameter(3,par_neg[5]);
    h_rf_neg_piall->Fit(f1_neg_pi,"R");
    h_rf_neg_piall->Fit(f2_neg_K,"R+");
    f1_neg_pi->GetParameters(&par_neg_pi[0]);
    f2_neg_K->GetParameters(&par_neg_pi[3]);
    all_neg_pi->SetParameters(par_neg_pi);
    h_rf_neg_piall->Fit(all_neg_pi,"R+");
    TLine *l_rf_low_neg = new TLine(rf_pi_low,0,rf_pi_low,1000);
    TLine *l_rf_high_neg = new TLine(rf_pi_high,0,rf_pi_high,1000);
    l_rf_low_neg->Draw("same");
    l_rf_high_neg->Draw("same");
    double width_neg = h_rf_neg_piall->GetXaxis()->GetBinWidth(1);
    std::cout<<"Bin width "<<width_neg<<std::endl;
    double neg_pi_N = all_neg_pi->Integral(rf_pi_low,rf_pi_high,width_neg);
    //double neg_pi_N = f1_neg_pi->Integral(rf_pi_low,rf_pi_high,width_neg);
    double neg_K_N = f2_neg_K->Integral(rf_pi_low,rf_pi_high,width_neg);
    std::cout<<"Pi number in fitting "<<neg_pi_N<<"K number in fitting "<<neg_K_N<<std::endl;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["rf_cut_eff"] = 1-neg_K_N/neg_pi_N;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["pi"] = neg_pi_N;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["K"] = neg_K_N;
    TPaveText* pt_neg_pi = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_neg_pi->AddText(("RunGroup neg pi "+std::to_string(RunGroup)).c_str());
    pt_neg_pi->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_neg_pi->AddText(("Pi: p0 = "+std::to_string(par_neg_pi[0])).c_str());
    pt_neg_pi->AddText(("p1 = "+std::to_string(par_neg_pi[1])).c_str());
    pt_neg_pi->AddText(("p3 = "+std::to_string(par_neg_pi[2])).c_str());
    pt_neg_pi->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_neg_pi->AddText(("K: p0 = "+std::to_string(par_neg_pi[3])).c_str());
    pt_neg_pi->AddText(("p1 = "+std::to_string(par_neg_pi[4])).c_str());
    pt_neg_pi->AddText(("p3 = "+std::to_string(par_neg_pi[5])).c_str());
    pt_neg_pi->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_neg_pi->Draw();
    std::string f1_neg_pi_string = std::to_string(par_neg_pi[0])+"*exp("+"\\frac{(x-"+std::to_string(par_neg_pi[1])+")^2}{2*"+std::to_string(par_neg_pi[2])+"^2})";
    std::string f2_neg_K_string = std::to_string(par_neg_pi[3])+"*exp("+"\\frac{(x-"+std::to_string(par_neg_pi[4])+")^2}{2*"+std::to_string(par_neg_pi[5])+"^2})";
    //TLatex *pi_neg_tex;
    //pi_neg_tex->DrawLatex(0.75,0.6,f1_neg_string.c_str());
    //pi_neg_tex->DrawLatex(0.75,0.5,f2_neg_string.c_str());
    c_pi_neg->Update();
    std::string c_pi_neg_name = "results/pid/rftime/rftime_neg_"+std::to_string(RunGroup)+"_pi.pdf";
    c_pi_neg->SaveAs(c_pi_neg_name.c_str());

    TCanvas *c_pi_neg_2nd = new TCanvas();
    //c_pi_neg_2nd->SetGrid();
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(kBird);
    h_rf_neg_piall->DrawCopy("hist");
    f1_neg_pi->GetParameters(&par_neg_pi[0]);
    f2_neg_K->GetParameters(&par_neg_pi[3]);
    double sigma_pi_neg = par_neg_pi[2];
    TLine *l_rf_lowsigma_neg = new TLine(1-3*sigma_pi_neg,0,1-3*sigma_pi_neg,1000);
    TLine *l_rf_highsigma_neg = new TLine(1+3*sigma_pi_neg,0,1+3*sigma_pi_neg,1000);
    l_rf_lowsigma_neg->Draw("same");
    l_rf_highsigma_neg->Draw("same");
    double neg_pi_N_sigma = all_neg_pi->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width);
    //double neg_pi_N_sigma = f1_neg_pi->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width);
    double neg_K_N_sigma = f2_neg_K->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width);
    std::cout<<"Pi number in fitting "<<neg_pi_N_sigma<<"K number in fitting "<<neg_K_N_sigma<<std::endl;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["rf_cut_eff_sigma"] = 1-neg_K_N_sigma/neg_pi_N_sigma;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["pi_sigma"] = neg_pi_N_sigma;
    j_rungroup_info[(std::to_string(RunGroup)).c_str()]["neg"]["K_sigma"] = neg_K_N_sigma;
    TPaveText* pt_neg_pi_2nd = new TPaveText(0.75,0.5,1,0.95,"brNDC");
    pt_neg_pi_2nd->AddText(("RunGroup neg pi in 3 sigma "+std::to_string(RunGroup)).c_str());
    pt_neg_pi_2nd->AddText(("shms p "+std::to_string(shms_p)).c_str());
    pt_neg_pi_2nd->AddText(("Pi: p0 = "+std::to_string(par_neg_pi[0])).c_str());
    pt_neg_pi_2nd->AddText(("p1 = "+std::to_string(par_neg_pi[1])).c_str());
    pt_neg_pi_2nd->AddText(("p3 = "+std::to_string(par_neg_pi[2])).c_str());
    pt_neg_pi_2nd->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
    pt_neg_pi_2nd->AddText(("K: p0 = "+std::to_string(par_neg_pi[3])).c_str());
    pt_neg_pi_2nd->AddText(("p1 = "+std::to_string(par_neg_pi[4])).c_str());
    pt_neg_pi_2nd->AddText(("p3 = "+std::to_string(par_neg_pi[5])).c_str());
    pt_neg_pi_2nd->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
    pt_neg_pi_2nd->Draw();
   // pi_neg_tex->DrawLatex(0.75,0.6,f1_neg_string.c_str());
   // pi_neg_tex->DrawLatex(0.75,0.5,f2_neg_string.c_str());
    c_pi_neg_2nd->Update();
    std::string c_pi_neg_2nd_name = "results/pid/rftime/rftime_neg_"+std::to_string(RunGroup)+"_pi_2nd.pdf";
    c_pi_neg_2nd->SaveAs(c_pi_neg_2nd_name.c_str());
    //end of neg

  }//if normal production runs
    std::string of_name = "results/pid/rftime/rf_eff_"+std::to_string(RunGroup)+".json";
    std::ofstream ofs;
    ofs.open(of_name.c_str());
    ofs<<j_rungroup_info.dump(4)<<std::endl;
}
