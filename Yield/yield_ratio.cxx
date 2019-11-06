#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TGraphErrors.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TChain.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
std::string goodTrackHMS = "H.gtr.dp>-10 && H.gtr.dp<10";

std::string piCutSHMS = "P.aero.npeSum >1.0 && P.cal.eprtracknorm <0.2 && P.cal.etottracknorm<1.0";
std::string eCutHMS = "H.cal.etottracknorm >0.8 && H.cal.etottracknorm <2. && "
                       "H.cer.npeSum > 1.";
std::string epiCut = "P.aero.npeSum > 1.0 && P.cal.eprtracknorm <0.2 && "
                     "H.cer.npeSum > 1.0 && H.cal.etottracknorm > 0.6 && "
                     "H.cal.etottracknorm < 2.0 && P.cal.etottracknorm <1.0";

void yield_ratio(){
  json j;
  {
    std::ifstream runs("db2/ratio_run_group_updated.json");
    runs>>j;
  }
  TGraphErrors *G_yield_z = new TGraphErrors();
  G_yield_z->SetTitle("ratio vs z");
  TGraphErrors *G_yield_x = new TGraphErrors();
  G_yield_x->SetTitle("ratio vs x");
  int i_z=0,i_x=0;
  for(json::iterator it = j.begin();it!=j.end();++it){
    std::vector<int> neg_D2_runs,pos_D2_runs;
    auto runjs = it.value();
    neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
    std::cout<<"Rungroup "<<it.key()<<" neg runs "<<neg_D2_runs.size()<<" pos runs "<<pos_D2_runs.size()<<std::endl;
  if(neg_D2_runs.size()!=0 && pos_D2_runs.size()!=0){
    TChain chain_neg("T");
    double neg_D2_charge = 0;
    for(std::vector<int>::iterator it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
      int RunNumber = *it;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_sh("TSP",rootfile_name);
      double charge = *d_sh.Max("P.BCM4B.scalerChargeCut");
      neg_D2_charge += charge;
      chain_neg.Add(rootfile_name.c_str());
      //ROOT::RDataFrame d("T",rootfile_name);
      //auto dCoinGoodTrack = d.Filter(goodTrackHMS)
      //  .Filter(goodTrackSHMS)
      //  .Filter(epiCut)
      //  ;
    }//loop over all neg D2 runs
    ROOT::RDataFrame d_neg(chain_neg);
    auto dCoinGoodTrack_neg = d_neg.Filter(goodTrackHMS)
      .Filter(goodTrackSHMS)
      ;
    auto dCoinSidis_neg = dCoinGoodTrack_neg.Filter("fEvtHdr.fEvtType ==4 ")
      .Filter(epiCut)
      ;
    double neg_statistics = *dCoinSidis_neg.Count();
    double yield_neg = neg_statistics/neg_D2_charge;
    
    TChain chain_pos("T");
    double pos_D2_charge = 0;
    for(std::vector<int>::iterator it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
      int RunNumber = *it;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_sh("TSP",rootfile_name);
      double charge = *d_sh.Max("P.BCM4B.scalerChargeCut");
      pos_D2_charge += charge;
      chain_pos.Add(rootfile_name.c_str());
      //ROOT::RDataFrame d("T",rootfile_name);
      //auto dCoinGoodTrack = d.Filter(goodTrackHMS)
      //  .Filter(goodTrackSHMS)
      //  .Filter(epiCut)
      //  ;
    }//loop over all pos D2 runs
    ROOT::RDataFrame d_pos(chain_pos);
    auto dCoinGoodTrack_pos = d_pos.Filter(goodTrackHMS)
      .Filter(goodTrackSHMS)
      ;
    auto dCoinSidis_pos = dCoinGoodTrack_pos.Filter("fEvtHdr.fEvtType ==4 ")
      .Filter(epiCut)
      ;
    double pos_statistics = *dCoinSidis_pos.Count();
    double yield_pos = pos_statistics/pos_D2_charge;
    
    double z = runjs["z"].get<double>();
    double x = runjs["x"].get<double>();
    double ratio = yield_neg/yield_pos;
    G_yield_x ->SetPoint(i_x,x,ratio);
    G_yield_z ->SetPoint(i_z,z,ratio);
    ++i_x;
    ++i_z;
  }//if pos neg runs exist
  }//loop over json file
  TCanvas *c = new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  G_yield_x->Draw();
  c->cd(2);
  G_yield_z->Draw();
  c->SaveAs("results/ratio_x_z.pdf");
}

