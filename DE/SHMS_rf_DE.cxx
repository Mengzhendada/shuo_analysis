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


void SHMS_rf_DE(int RunGroup = 0){
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
  json j_rf_DE;
  {
    std::string name = "shuo_analysis/dbase/rf_eff.json";
    std::ifstream ifs(name.c_str());
    ifs>>j_rf_DE;
  }
  std::vector<double> rf_cuts = j_DE["SHMS"]["rf_time_cuts"].get<std::vector<double>>();

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
  
  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runsinfo;
  }

  if(!neg_D2.empty() && !pos_D2.empty()){

    double cointime_lowcut,cointime_highcut;
    if(RunGroup<410){
      cointime_lowcut = j_cuts["cointime_low_fall"].get<double>();
      cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
    }
    else{
      cointime_lowcut = j_cuts["cointime_low_spring"].get<double>();
      cointime_highcut = j_cuts["cointime_high_spring"].get<double>();
    }



      std::vector<int> delta_cut_num= j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>(); 
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
      auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
        //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
        .Filter(goodTrackSHMS)
        .Filter(goodTrackHMS)
        .Filter(piCutSHMS)
        .Filter(eCutHMS)
        //.Filter(aeroCutSHMS)
        .Filter(Normal_SHMS)
        .Filter(Normal_HMS)
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;


      //coin time cut for pos runs
      auto h_cointime_pos = d_pos_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
      double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
      std::cout<<"pos coin time peak "<<coin_peak_center_pos<<std::endl;
      //cointime cut
      double cointime_low_pos = coin_peak_center_pos+cointime_lowcut;
      double cointime_high_pos = coin_peak_center_pos+cointime_highcut;
      
      auto d_pos_first = d_pos_run
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto h_coin_pos = d_pos_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_poscut_rungroup = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //rftime cut
      //offset
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
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
        .Filter(aeroCutSHMS)
        ;

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
      auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
       double rf_pi_low,rf_pi_high; 
        int i_order = 0,i_which;
        for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
          if(SHMS_dp>*it){
            i_which = i_order;
            rf_pi_low = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_low"].get<double>();
            rf_pi_high = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["pos"]["rf_cut_high"].get<double>();
          }
          i_order++;
        }
        return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  
        
      };
       std::cout<<"check 1"<<std::endl;
      auto d_pos_piall  = d_mod_first
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        .Define("Pi_eff",Get_pi_eff,{"P.gtr.dp"})
        .Define("Pi_purity",Get_pi_purity,{"P.gtr.dp"})
        ;
      auto d_pos_piall_bg = d_pos_bg_norfcut
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        .Define("Pi_eff",Get_pi_eff,{"P.gtr.dp"})
        .Define("Pi_purity",Get_pi_purity,{"P.gtr.dp"})
        ;
       std::cout<<"check 2"<<std::endl;

      auto h_pieff = d_pos_piall.Histo1D({"","pi eff",10,0.99,1},"Pi_eff");
      auto h_pipurity = d_pos_piall.Histo1D({"","pi purity",50,0.5,1},"Pi_purity");
      auto h_pieff_delta = d_pos_piall.Histo2D({"","",100,-10,20,10,0.99,1},"P.gtr.dp","Pi_eff");
      auto h_pipurity_delta = d_pos_piall.Histo2D({"","",100,-10,20,50,0.75,1},"P.gtr.dp","Pi_purity");
      TCanvas* c_pi_eff_pos = new TCanvas();
      c_pi_eff_pos->Divide(1,2);
      c_pi_eff_pos->cd(1);
      h_pieff->DrawCopy("hist");
      c_pi_eff_pos->cd(2);
      h_pieff_delta->DrawCopy("colz");
      std::string c_pi_eff_pos_name = "results/pid/SHMS_pi_eff_"+std::to_string(RunNumber)+".pdf";
      c_pi_eff_pos->SaveAs(c_pi_eff_pos_name.c_str());
      TCanvas* c_pi_purity_pos = new TCanvas();
      c_pi_purity_pos->Divide(1,2);
      c_pi_purity_pos->cd(1);
      h_pipurity->DrawCopy("hist");
      c_pi_purity_pos->cd(2);
      h_pipurity_delta->DrawCopy("colz");
      std::string c_pi_purity_pos_name = "results/pid/SHMS_pi_purity_"+std::to_string(RunNumber)+".pdf";
      c_pi_purity_pos->SaveAs(c_pi_purity_pos_name.c_str());

    }


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
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;


      //coin time cut for neg runs
      auto h_cointime_neg = d_neg_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
      double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
      std::cout<<"neg coin time peak "<<coin_peak_center_neg<<std::endl;
      //cointime cut
      double cointime_low_neg = coin_peak_center_neg+cointime_lowcut;
      double cointime_high_neg = coin_peak_center_neg+cointime_highcut;
      
      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto h_coin_neg = d_neg_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_negcut_rungroup = d_neg_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //rftime cut
      //offset
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
      std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
      auto d_mod_first = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
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

      auto d_neg_bg_norfcut = d_neg_run
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        .Filter(aeroCutSHMS)
        ;
      
      auto Get_pi_eff = [=](double SHMS_dp){
        double pi_eff;
        int i_order = 0,i_which;
        for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
          if(SHMS_dp>*it){
            i_which = i_order;
            pi_eff = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["pi_eff"].get<double>();
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
            pi_purity = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["pi_purity"].get<double>();
          }
          i_order++;
        }
        return pi_purity;  
      };
      auto rf_cut = [=](double SHMS_dp,double SHMS_rftime){
       double rf_pi_low,rf_pi_high; 
        int i_order = 0,i_which;
        for(auto it = delta_cut_num.begin();it!=delta_cut_num.end();++it){
          if(SHMS_dp>*it){
            i_which = i_order;
            rf_pi_low = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["rf_cut_low"].get<double>();
            rf_pi_high = j_rf_DE[(std::to_string(RunGroup)).c_str()][(std::to_string(i_which)).c_str()]["neg"]["rf_cut_high"].get<double>();
          }
          i_order++;
        }
        return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  
        
      };

      auto d_neg_piall  = d_mod_first
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        .Define("Pi_eff",Get_pi_eff,{"P.gtr.dp"})
        .Define("Pi_purity",Get_pi_purity,{"P.gtr.dp"})
        ;
      auto d_neg_piall_bg = d_neg_bg_norfcut
        .Filter(rf_cut,{"P.gtr.dp","diff_time_mod"})
        .Define("Pi_eff",Get_pi_eff,{"P.gtr.dp"})
        .Define("Pi_purity",Get_pi_purity,{"P.gtr.dp"})
        ;
      std::cout<<"check 3"<<std::endl;
      
      auto h_pieff = d_neg_piall.Histo1D({"","pi eff",10,0.99,1},"Pi_eff");
      auto h_pipurity = d_neg_piall.Histo1D({"","pi purity",50,0.5,1},"Pi_purity");
      auto h_pieff_delta = d_neg_piall.Histo2D({"","",100,-10,20,10,0.99,1},"P.gtr.dp","Pi_eff");
      auto h_pipurity_delta = d_neg_piall.Histo2D({"","",100,-10,20,50,0.75,1},"P.gtr.dp","Pi_purity");
      TCanvas* c_pi_eff_neg = new TCanvas();
      c_pi_eff_neg->Divide(1,2);
      c_pi_eff_neg->cd(1);
      h_pieff->DrawCopy("hist");
      c_pi_eff_neg->cd(2);
      h_pieff_delta->DrawCopy("colz");
      std::string c_pi_eff_neg_name = "results/pid/SHMS_pi_eff_"+std::to_string(RunNumber)+".pdf";
      c_pi_eff_neg->SaveAs(c_pi_eff_neg_name.c_str());
      TCanvas* c_pi_purity_neg = new TCanvas();
      c_pi_purity_neg->Divide(1,2);
      c_pi_purity_neg->cd(1);
      h_pipurity->DrawCopy("hist");
      c_pi_purity_neg->cd(2);
      h_pipurity_delta->DrawCopy("colz");
      std::string c_pi_purity_neg_name = "results/pid/SHMS_pi_purity_"+std::to_string(RunNumber)+".pdf";
      c_pi_purity_neg->SaveAs(c_pi_purity_neg_name.c_str());

    }

    


  }//if normal production runs
}
