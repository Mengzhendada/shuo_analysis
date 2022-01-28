#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "Get_all_eff.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void plot_yield_Hem(){

  json j_rungroup;
  {
     std::ifstream rungroup("");
     rungroup>>j_rungroup;
  }
  //you want to loop over rungroups
  //int RunGroup = 110;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<RunGroup<<std::endl;
    std::vector<int> D2_pos,Dummy_pos;
    D2_pos = it.value()["pos"]["D2"].get<std::vector<int>>();
    //Dummy_pos ;
//    double Charge_RunGroup = 100;//charge for the rungroup, read from json file
    double Q_RunGroup_D2_pos = 0;
    TH1D* yield_D2_pos_rungroup = new TH1D("","",100,0,1);
    for(auto it_D2_pos = D2_pos.begin();it_D2_pos!=D2_pos.end();++it_D2_pos){
      int RunNumber = *it_D2_pos;
      double charge = 30;//charge for this run, read from json file
      TFile* rootfile_D2_pos = new TFile(("rootfile path"+std::to_string(RunNumber)+".root").c_str());
      TH1D* yield_D2_pos = new TH1D("","",100,0,1);
      yield_D2_pos = (TH1D*)rootfile_D2_pos->Get("name of branch");
      //yield_D2_pos->Scale(charge/Charge_RUnGroup);
      yield_D2_pos->Scale(charge);
      Q_RunGroup_D2_pos+=charge;
      //Add(yield_D2_pos_rungroup,yield_D2_pos)
      yield_D2_pos_rungroup->Add(yield_D2_pos,1);//add the histogram of the runnumber in () to the rungroup histogram on the left
    }//loop over D2 pos runs
    yield_D2_pos_rungroup->Scale(Q_RunGroup_D2_pos);//charge weighted average yield for D2 pos runs in this rungroup
    
    double Q_RunGroup_Dummy_pos = 0;
    TH1D* yield_Dummy_pos_rungroup = new TH1D("","",100,0,1);
    for(auto it_Dummy_pos = Dummy_pos.begin();it_Dummy_pos!=Dummy_pos.end();++it_Dummy_pos){
      int RunNumber = *it_Dummy_pos;
      double charge = 30;//charge for this run, read from json file
      TFile* rootfile_Dummy_pos = new TFile(("rootfile path"+std::to_string(RunNumber)+".root").c_str());
      TH1D* yield_Dummy_pos = new TH1D("","",100,0,1);
      yield_Dummy_pos = (TH1D*)rootfile_Dummy_pos->Get("name of branch");
      //yield_Dummy_pos->Scale(charge/Charge_RUnGroup);
      yield_Dummy_pos->Scale(charge);
      Q_RunGroup_Dummy_pos+=charge;
      //Add(yield_Dummy_pos_rungroup,yield_Dummy_pos)
      yield_Dummy_pos_rungroup->Add(yield_Dummy_pos,1);//add the histogram of the runnumber in () to the rungroup histogram on the left
    }//loop over Dummy pos runs
    yield_Dummy_pos_rungroup->Scale(Q_RunGroup_Dummy_pos);//charge weighted average yield for Dummy pos runs in this rungroup
    // now you get the charge weighted average yield for the Dummy pos runs in this rungroup
    
    yield_D2_pos_rungroup->Add(yield_Dummy_pos_rungroup,-0.245);//dummy subtraction

    //simc yield
    TFile* rootfile_simc = new TFile(("path to simc yield rootfile"+std::to_string(RunGroup)+".root").c_str());
    TH1D* D2_pos_inc_rad = new TH1D("","",100,0,1);
    D2_pos_inc_rad = (TH1D*)rootfile_simc->Get("branch name");//this is yield histogram
    TH1D* D2_pos_exc = ;
    TH1D* D2_pos_delta = ;
    TH1D* D2_pos_simc_sum = new TH1D();
    D2_pos_simc_sum->Add(D2_pos_inc_rad,1);
    D2_pos_simc_sum->Add(D2_pos_exc,1);
    D2_pos_simc_sum->Add(D2_pos_delta,1);
    //do same thing for inc norad, exc, delta
    TMultyGraph* mg_yield = new TMultiGraph();
    mg_yield->Add(yield_D2_pos_rungroup,"P");
    mg_yield->Add(yield_Dummy_pos_rungroup,"P");
    //add simc histograme
    TCanvas* c_yield = new TCanvas();
    mg_yield->Draw("ap");
    c_yield->SaveAs();
  }//loop over rungroup 


}
