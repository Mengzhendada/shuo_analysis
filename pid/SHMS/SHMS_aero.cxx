#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TAxis.h"
#include "TF1.h"
#include "TLine.h"
#include "TProfile.h"
#include "TPaveText.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
bool reject;

void SHMS_aero(int RunGroup = 0){
  json j_cuts;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_cuts;
  }

  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup <0)
      return;
  }
  RunGroup = RunGroup;
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json jout;
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(neg_D2.empty() || pos_D2.empty()){std::cout<<"This is not normal production rungroup"<<std::endl;}
  else{
    std::vector<std::string> files_neg,files_pos;
   
    //for neg part
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
    }

    //add momentum
    double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
    auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

    ROOT::RDataFrame d_neg_raw("T",files_neg);
    //general cut
    auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_neg_HMS_e = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
    //HMS good electron cut
      .Filter("H.cer.npeSum > 10")
      .Filter("H.cal.etottracknorm >0.85")
      .Filter("fEvtHdr.fEvtType == 4")
      .Define("shms_p",shms_p_calculate,{"P.gtr.dp"});
    auto h_coin_time = d_neg_HMS_e.Histo1D({"coin_time","coin_time;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
    int coin_peak_bin = h_coin_time->GetMaximumBin();
    double coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);
    //coin time cut
    auto d_neg_coin = d_neg_HMS_e.Filter(
        [=](double coin_time){return std::abs(coin_time - coin_peak_center) < 2.0;},{"CTime.ePositronCoinTime_ROC2"});
    double hgc_pi = j_cuts["SHMS"]["HGC_pi"].get<double>();
    std::string hgc_pi_cut = "P.hgcer.npeSum > "+std::to_string(hgc_pi);
    double cal_pi_low = j_cuts["SHMS"]["cal_pi_low"].get<double>();
    double cal_pi_high = j_cuts["SHMS"]["cal_pi_high"].get<double>();
    std::string cal_pi_cut = "P.cal.etottracknorm < "+std::to_string(cal_pi_high)+" && P.cal.etottracknorm > "+std::to_string(cal_pi_low);
    //SHMS pi cut
    auto d_neg_pi = d_neg_coin.Filter(cal_pi_cut)
                              .Filter(hgc_pi_cut);
    //SHMS heavier than pi cut
    auto d_neg_h = d_neg_coin.Filter(("P.hgcer.npeSum < "+std::to_string(hgc_pi)).c_str());
    
    auto h_aero_2d_neg = d_neg_pi.Histo2D({"","pi",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.aero.npeSum");
    TProfile *h_aero_2d_neg_pro = h_aero_2d_neg->ProfileX("pi",1,-1,"d");

    auto h_aero_2d_neg_h = d_neg_h.Histo2D({"","hardron",100,0.9*SHMS_P,1.22*SHMS_P,100,0,30},"shms_p","P.aero.npeSum");
    TProfile *h_aero_2d_neg_h_pro = h_aero_2d_neg_h->ProfileX("h",1,-1,"d");
    TPaveText *pt_aero_2d_neg = new TPaveText(0.05,0.9,0.15,0.95,"brNDC");
    pt_aero_2d_neg->AddText(("shms p = "+std::to_string(SHMS_P)).c_str() );
    TLine *l_SHMSP = new TLine(SHMS_P,0,SHMS_P,30);
    TCanvas *c_aero_neg = new TCanvas();
    //h_aero_2d_neg->DrawCopy("colz");
    h_aero_2d_neg_pro->GetYaxis()->SetRangeUser(0,30);
    h_aero_2d_neg_pro->Draw();
    l_SHMSP->Draw("same");
    h_aero_2d_neg_h_pro->SetLineColor(kBlue);
    h_aero_2d_neg_h_pro->Draw("same");    
    c_aero_neg->cd();
    pt_aero_2d_neg->Draw();
    
    std::string aero_neg_name = "results/pid/aero_2d_"+std::to_string(RunGroup)+".pdf";
    c_aero_neg->SaveAs(aero_neg_name.c_str());

  
  }
}
