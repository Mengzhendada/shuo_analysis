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
#include "TGraphErrors.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TEfficiency.h"
#include "TEllipse.h"
constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;


bool shms_momentum_high = true;
//double center_region(double x){
//  return sqrt(36-(x-1.33)*(x-1.33))+0.83;
//}

void SHMS_hgcer(int RunGroup=0){

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
  if(!neg_D2.empty() && !pos_D2.empty()){
    double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    std::cout<<"SHMS momentum is "<<SHMS_P<<std::endl;
    auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};
    /*
    std::vector<std::string> files_neg,files_pos;
    //if(SHMS_P>3.2){aeroCutSHMS = aeroCutSHMS + " && P.hgcer.npeSum > "+(std::to_string(P_hgcer)).c_str();}
    //for neg runs, updated version in new script
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg "<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
    }
    */
    //for pos runs
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      //files_pos.push_back(rootfile_name);
    }

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
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      //coin time cut for pos runs
      auto h_cointime_pos = d_pos_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
      double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
      double cointime_lowcut,cointime_highcut;
      if(RunGroup<420){
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

      double rf_pi_low = j_cuts["rf_cut_low"].get<double>();
      std::cout<<rf_pi_low<<std::endl;
      double rf_pi_high =j_cuts["rf_cut_high"].get<double>();
      std::cout<<rf_pi_high<<std::endl;


      auto d_pos_first = d_pos_run
        .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto h_coin_pos = d_pos_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_poscut = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //offset
      double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>(); 
      std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
      auto d_mod_first = d_pos_first
        .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
      auto d_pos_pi = d_mod_first
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        ;

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
      std::cout<<bg_cut<<std::endl;
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
        ;
      auto h_coin_pos_bg = d_pos_bg.Histo1D({"","pos bg",800,0,100},"CTime.ePiCoinTime_ROC2");

      std::vector<double> HGC_cuts = j_DE["SHMS"]["HGC_cuts"].get<std::vector<double>>();
      for(auto it = HGC_cuts.begin();it!=HGC_cuts.end();++it){
        double hgc_cut = *it;
        std::cout<<"HGC is "<<hgc_cut<<std::endl;
        std::string hgc_eff_str = "HGC cut "+std::to_string(hgc_cut);
        auto h_hgcer_pi_should = d_pos_pi.Histo1D({"hgcer_dp","should",100,-10,22},"P.gtr.dp");
        auto h_hgcer_bg_should = d_pos_bg.Histo1D({"hgcer_dp","should",100,-10,22},"P.gtr.dp");
        h_hgcer_pi_should->Sumw2();
        h_hgcer_pi_should->Add(h_hgcer_bg_should.GetPtr(),-1.0/6);

        auto d_pos_pi_did = d_pos_pi.Filter([=](double hgcer_npe){return hgcer_npe>hgc_cut;},{"P.hgcer.npeSum"});
        auto d_pos_bg_did = d_pos_bg.Filter([=](double hgcer_npe){return hgcer_npe>hgc_cut;},{"P.hgcer.npeSum"});
        auto h_hgcer_pi_did = d_pos_pi_did.Histo1D({"hgcer_dp","did",100,-10,22},"P.gtr.dp");
        auto h_hgcer_bg_did = d_pos_bg_did.Histo1D({"hgcer_dp","did",100,-10,22},"P.gtr.dp");
        h_hgcer_pi_did->Sumw2();
        h_hgcer_pi_did->Add(h_hgcer_bg_did.GetPtr(),-1.0/6);
        //TH2F* h_hgcer_eff = new TH2F("","hgcer eff",100,-10,22,100,0,1);
        h_hgcer_pi_did->Sumw2();
        TEfficiency *eff_hgcer_dp = new TEfficiency(*h_hgcer_pi_did,*h_hgcer_pi_should);
        //h_hgcer_pi_did->Divide(h_hgcer_pi_should.GetPtr());
        //TProfile *prof_hgcer_eff = h_hgcer_pi_did->ProfileX("h1",1,-1,"d");
        //TGraphErrors* g_hgcer_eff = new TGraphErrors(eff_hgcer_dp);
        
        TCanvas* c_hgc_eff = new TCanvas();
        //h_hgcer_pi_did->DrawCopy();
        TPaveText* pt_hgc_eff = new TPaveText(0.8,0.8,1,1);
        pt_hgc_eff->AddText(("RunNumber "+std::to_string(RunNumber)).c_str());
        pt_hgc_eff->AddText(("shms p "+std::to_string(SHMS_P)).c_str());
        pt_hgc_eff->AddText(("hgc cut "+std::to_string(hgc_cut)).c_str());
        pt_hgc_eff->Draw();
        gStyle->SetOptTitle(0);
        //g_hgcer_eff->Draw("ap");
        eff_hgcer_dp->Draw("AP");
        std::string c_hgc_eff_name = "results/pid/hgcer/SHMS_hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,3)+".pdf";
        c_hgc_eff->SaveAs(c_hgc_eff_name.c_str());
 
        std::string histo_name = std::to_string(RunNumber)+"_"+std::to_string(SHMS_P);
        auto h_hgcer_xcer_pishould = d_pos_pi.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        auto h_hgcer_xcer_bgshould = d_pos_bg.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        h_hgcer_xcer_pishould->Sumw2();
        h_hgcer_xcer_pishould->Add(h_hgcer_xcer_bgshould.GetPtr(),-1.0/6);
        auto h_hgcer_xcer_pidid = d_pos_pi_did.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        auto h_hgcer_xcer_bgdid = d_pos_bg_did.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        h_hgcer_xcer_pidid->Sumw2();
        h_hgcer_xcer_pidid->Add(h_hgcer_xcer_bgdid.GetPtr(),-1.0/6);
        h_hgcer_xcer_pidid->Sumw2();
        TEfficiency *eff_hgcer_xcer= new TEfficiency(*h_hgcer_xcer_pidid,*h_hgcer_xcer_pishould);
        
        auto h_hgcer_shmsp_pishould = d_pos_pi.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        auto h_hgcer_shmsp_bgshould = d_pos_bg.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        h_hgcer_shmsp_pishould->Sumw2();
        h_hgcer_shmsp_pishould->Add(h_hgcer_shmsp_bgshould.GetPtr(),-1.0/6);
        auto h_hgcer_shmsp_pidid = d_pos_pi_did.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        auto h_hgcer_shmsp_bgdid = d_pos_bg_did.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        h_hgcer_shmsp_pidid->Sumw2();
        h_hgcer_shmsp_pidid->Add(h_hgcer_shmsp_bgdid.GetPtr(),-1.0/6);
        h_hgcer_shmsp_pidid->Sumw2();
        TEfficiency *eff_hgcer_shmsp= new TEfficiency(*h_hgcer_shmsp_pidid,*h_hgcer_shmsp_pishould);

        std::string rootfile_out_name = "results/pid/hgcer/hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,1)+".root"; 
        TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
       // h_hgcer_pi_did->Write();
        eff_hgcer_dp->Write();
        eff_hgcer_shmsp->Write();
        eff_hgcer_xcer->Write();
        rootfile_out->Close();

        auto h_hgcer_pi_should_2d = d_pos_pi.Histo2D({"",";yCer;xCer",80,-40,40,80,-40,40},"P.hgcer.yAtCer","P.hgcer.xAtCer");
        auto h_hgcer_pi_did_2d = d_pos_pi_did.Histo2D({"",";yCer;xCer",80,-40,40,80,-40,40},"P.hgcer.yAtCer","P.hgcer.xAtCer");
        h_hgcer_pi_did_2d->Divide(h_hgcer_pi_should_2d.GetPtr());
        TCanvas* c_hgc_eff_2d = new TCanvas();
        h_hgcer_pi_did_2d->SetBit(TH2::kNoStats);
        //h_hgcer_pi_did_2d->GetXaixs()->SetTitle("yCer");
        //h_hgcer_pi_did_2d->GetYaixs()->SetTitle("xCer");
        //gPad()->Modified();
        h_hgcer_pi_did_2d->DrawCopy("colz");
        //TF1* center_cir = new TF1("center","center_region(x)",-5.33,7.33);
        TEllipse *center_cir = new TEllipse(1.33,0.83,6,0);
        //center_cir->SetFillColor(6);
        center_cir->SetFillStyle(0);
        //center_cir->SetLineColor(6);
        center_cir->Draw("same");
        TLine *line_up = new TLine(-40,0,40,0);
        TLine *line_down = new TLine(-40,3,40,3);
        line_up->Draw("same");
        line_down->Draw("same");
        //TF2* left = new TF1("left","1.33",)
        gStyle->SetOptTitle(0);
        std::string c_hgc_eff_2d_name = "results/pid/hgcer/SHMS_hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,3)+"_2d.pdf";
        c_hgc_eff_2d->SaveAs(c_hgc_eff_2d_name.c_str());

      }//loop over each hgc cuts
    }//for each pos runs

    //need to copy pos part to do neg part. 
    //for neg runs
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg"<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      //files_neg.push_back(rootfile_name);
    }

    // ROOT::RDataFrame d_neg_raw("T",files_neg);
    // auto d_neg = d_neg_raw
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
        .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
        .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return current>current_offset;},{"current"})
        ;
      //coin time cut for neg runs
      auto h_cointime_neg = d_neg_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
      int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
      double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
      double cointime_lowcut,cointime_highcut;
      if(RunGroup<420){
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

      double rf_pi_low = j_cuts["rf_cut_low"].get<double>();
      std::cout<<rf_pi_low<<std::endl;
      double rf_pi_high =j_cuts["rf_cut_high"].get<double>();
      std::cout<<rf_pi_high<<std::endl;


      auto d_neg_first = d_neg_run
        .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
        ;
      auto h_coin_neg = d_neg_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
      auto h_coin_negcut = d_neg_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

      //offset
      double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>(); 
      std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
      auto d_mod_first = d_neg_first
        .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
        .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
        ;
      auto h_diff_mod_neg = d_mod_first.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
      auto d_neg_pi = d_mod_first
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        ;

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
        ;
      auto h_difftime_forbg = d_neg_forbg.Histo1D({"","diff for bg",100,0,4.008},"diff_time_mod");
      auto d_neg_bg  = d_neg_forbg
        .Filter(
            [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
        // .Filter(
        //     [=](double bg_cointime){return ((bg_cointime > bg_left_low && bg_cointime <bg_left_high) || (bg_cointime > bg_right_low && bg_cointime < bg_right_high));},{"CTime.ePiCoinTime_ROC2"})
        .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
        .Filter(bg_cut)
        ;
      auto h_coin_neg_bg = d_neg_bg.Histo1D({"","neg bg",800,0,100},"CTime.ePiCoinTime_ROC2");

      std::vector<double> HGC_cuts = j_DE["SHMS"]["HGC_cuts"].get<std::vector<double>>();
      for(auto it = HGC_cuts.begin();it!=HGC_cuts.end();++it){
        double hgc_cut = *it;
        std::cout<<"HGC is "<<hgc_cut<<std::endl;
        std::string hgc_eff_str = "HGC cut "+std::to_string(hgc_cut);
        auto h_hgcer_pi_should = d_neg_pi.Histo1D({"hgcer_dp","should",100,-10,22},"P.gtr.dp");
        auto h_hgcer_bg_should = d_neg_bg.Histo1D({"hgcer_dp","should",100,-10,22},"P.gtr.dp");
        h_hgcer_pi_should->Sumw2();
        h_hgcer_pi_should->Add(h_hgcer_bg_should.GetPtr(),-1.0/6);

        auto d_neg_pi_did = d_neg_pi.Filter([=](double hgcer_npe){return hgcer_npe>hgc_cut;},{"P.hgcer.npeSum"});
        auto d_neg_bg_did = d_neg_bg.Filter([=](double hgcer_npe){return hgcer_npe>hgc_cut;},{"P.hgcer.npeSum"});
        auto h_hgcer_pi_did = d_neg_pi_did.Histo1D({"hgcer_dp","did",100,-10,22},"P.gtr.dp");
        auto h_hgcer_bg_did = d_neg_bg_did.Histo1D({"hgcer_dp","did",100,-10,22},"P.gtr.dp");
        h_hgcer_pi_did->Sumw2();
        h_hgcer_pi_did->Add(h_hgcer_bg_did.GetPtr(),-1.0/6);
        //TH2F* h_hgcer_eff = new TH2F("","hgcer eff",100,-10,22,100,0,1);
        h_hgcer_pi_did->Sumw2();
        TEfficiency *eff_hgcer_dp = new TEfficiency(*h_hgcer_pi_did,*h_hgcer_pi_should);
        //h_hgcer_pi_did->Divide(h_hgcer_pi_should.GetPtr());
        //TProfile *prof_hgcer_eff = h_hgcer_pi_did->ProfileX("h1",1,-1,"d");
        //TGraphErrors* g_hgcer_eff = new TGraphErrors(eff_hgcer_dp);
        
        TCanvas* c_hgc_eff = new TCanvas();
        //h_hgcer_pi_did->DrawCopy();
        TPaveText* pt_hgc_eff = new TPaveText(0.8,0.8,1,1);
        pt_hgc_eff->AddText(("RunNumber "+std::to_string(RunNumber)).c_str());
        pt_hgc_eff->AddText(("shms p "+std::to_string(SHMS_P)).c_str());
        pt_hgc_eff->AddText(("hgc cut "+std::to_string(hgc_cut)).c_str());
        pt_hgc_eff->Draw();
        gStyle->SetOptTitle(0);
        //g_hgcer_eff->Draw("ap");
        eff_hgcer_dp->Draw("AP");
        std::string c_hgc_eff_name = "results/pid/hgcer/SHMS_hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,3)+".pdf";
        c_hgc_eff->SaveAs(c_hgc_eff_name.c_str());
 
        std::string histo_name = std::to_string(RunNumber)+"_"+std::to_string(SHMS_P);
        auto h_hgcer_xcer_pishould = d_neg_pi.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        auto h_hgcer_xcer_bgshould = d_neg_bg.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        h_hgcer_xcer_pishould->Sumw2();
        h_hgcer_xcer_pishould->Add(h_hgcer_xcer_bgshould.GetPtr(),-1.0/6);
        auto h_hgcer_xcer_pidid = d_neg_pi_did.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        auto h_hgcer_xcer_bgdid = d_neg_bg_did.Histo1D({"hgcer_xcer",histo_name.c_str(),100,-40,40},"P.hgcer.xAtCer");
        h_hgcer_xcer_pidid->Sumw2();
        h_hgcer_xcer_pidid->Add(h_hgcer_xcer_bgdid.GetPtr(),-1.0/6);
        h_hgcer_xcer_pidid->Sumw2();
        TEfficiency *eff_hgcer_xcer= new TEfficiency(*h_hgcer_xcer_pidid,*h_hgcer_xcer_pishould);
        
        auto h_hgcer_shmsp_pishould = d_neg_pi.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        auto h_hgcer_shmsp_bgshould = d_neg_bg.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        h_hgcer_shmsp_pishould->Sumw2();
        h_hgcer_shmsp_pishould->Add(h_hgcer_shmsp_bgshould.GetPtr(),-1.0/6);
        auto h_hgcer_shmsp_pidid = d_neg_pi_did.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        auto h_hgcer_shmsp_bgdid = d_neg_bg_did.Histo1D({"hgcer_shmsp",histo_name.c_str(),200,1,6},"shms_p");
        h_hgcer_shmsp_pidid->Sumw2();
        h_hgcer_shmsp_pidid->Add(h_hgcer_shmsp_bgdid.GetPtr(),-1.0/6);
        h_hgcer_shmsp_pidid->Sumw2();
        TEfficiency *eff_hgcer_shmsp= new TEfficiency(*h_hgcer_shmsp_pidid,*h_hgcer_shmsp_pishould);

        std::string rootfile_out_name = "results/pid/hgcer/hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,1)+".root"; 
        TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
       // h_hgcer_pi_did->Write();
        eff_hgcer_dp->Write();
        eff_hgcer_shmsp->Write();
        eff_hgcer_xcer->Write();
        rootfile_out->Close();

        auto h_hgcer_pi_should_2d = d_neg_pi.Histo2D({"",";yCer;xCer",80,-40,40,80,-40,40},"P.hgcer.yAtCer","P.hgcer.xAtCer");
        auto h_hgcer_pi_did_2d = d_neg_pi_did.Histo2D({"",";yCer;xCer",80,-40,40,80,-40,40},"P.hgcer.yAtCer","P.hgcer.xAtCer");
        h_hgcer_pi_did_2d->Divide(h_hgcer_pi_should_2d.GetPtr());
        TCanvas* c_hgc_eff_2d = new TCanvas();
        h_hgcer_pi_did_2d->SetBit(TH2::kNoStats);
        //h_hgcer_pi_did_2d->GetXaixs()->SetTitle("yCer");
        //h_hgcer_pi_did_2d->GetYaixs()->SetTitle("xCer");
        //gPad()->Modified();
        h_hgcer_pi_did_2d->DrawCopy("colz");
        //TF1* center_cir = new TF1("center","center_region(x)",-5.33,7.33);
        TEllipse *center_cir = new TEllipse(1.33,0.83,6,0);
        //center_cir->SetFillColor(6);
        center_cir->SetFillStyle(0);
        //center_cir->SetLineColor(6);
        center_cir->Draw("same");
        TLine *line_up = new TLine(-40,0,40,0);
        TLine *line_down = new TLine(-40,3,40,3);
        line_up->Draw("same");
        line_down->Draw("same");
        //TF2* left = new TF1("left","1.33",)
        gStyle->SetOptTitle(0);
        std::string c_hgc_eff_2d_name = "results/pid/hgcer/SHMS_hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(hgc_cut).substr(0,3)+"_2d.pdf";
        c_hgc_eff_2d->SaveAs(c_hgc_eff_2d_name.c_str());

      }//loop over each hgc cuts
    }//for each neg runs


  }//if normal rungroup
}
