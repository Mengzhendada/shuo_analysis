#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TLine.h"
#include "TH2.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void HMS_cer_pion(int RunGroup = 0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  json j_cuts;
  {
    std::ifstream ifs("db2/HMS_cuts.json");
    ifs>>j_cuts;
  }
  std::vector<int> coolcolor;
  coolcolor = j_cuts["coolcolor"].get<std::vector<int>>();

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  RunGroup = 10*RunGroup;
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double HMS_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
  //double hms_p_calculate = [HMS_P](double hms_dp){return HMS_P*(1+hms_dp/100);};
  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    for(std::vector<int>::iterator it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
    }
    for(std::vector<int>::iterator it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
    }
    ROOT::RDataFrame d_neg_raw("T",files_neg);
    ROOT::RDataFrame d_pos_raw("T",files_pos);
    auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_neg = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
    auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_pos = d_pos_SHMS = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 22");
    
//    d_neg.Define("hms_p",hms_p_calculate,{"HMS.gtr.dp"});
    auto h_dp = d_neg.Histo1D({"","HMS momentum;momentum;counts",100,-10,10},"H.gtr.dp");
    //auto h_dp = d_neg.Histo1D({"","HMS momentum;momentum;counts",100,3,8},"hms_p");
    auto h_cer_dp = d_neg.Histo2D({"","HMS cer npeSum vs p;momentum;cer npesum",100,3,8,100,0,30},"H.gtr.dp","H.cer.npeSum");
    TCanvas *c_neg = new TCanvas();
    c_neg->Divide(1,2);
    c_neg->cd(1);
    h_dp->Draw("hist");
    c_neg->cd(2);
    h_cer_dp->Draw("colz");
    std::string c_neg_name = "results/pid/HMS_cer_pion_"+std::to_string(RunGroup)+".pdf";
    c_neg->SaveAs(c_neg_name.c_str());
  }//if normal rungroup
}//end 

