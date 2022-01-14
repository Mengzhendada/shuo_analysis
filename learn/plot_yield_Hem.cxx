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
      yield_D2_pos_rungroup->Add(yield_D2_pos);
    }//loop over D2 pos runs
    yield_D2_pos_rungroup->Scale(Q_RunGroup_D2_pos);//charge weighted average yield for D2 pos runs in this rungroup
    //do the same thing to get Dummy yield
    TH1D* yield_Dummy_pos_rungroup = new TH1D();
    // now you get the charge weighted average yield for the Dummy pos runs in this rungroup

    //simc yield
    TFile* rootfile_simc = new TFile(("path to simc yield rootfile"+std::to_string(RunGroup)+".root").c_str());
    TH1D* D2_pos_inc_rad = new TH1D;
    D2_pos_inc_rad = (TH1D*)rootfile_simc->Get("branch name");//this is yield histogram
    //do same thing for inc norad, exc, delta
    TMultyGraph* mg_yield = new TMultiGraph();
    mg_yield->Add(yield_D2_pos_rungroup,"P");
    mg_yield->Add(yield_Dummy_pos_rungroup,"P");
    //add simc histograme
    TCanvas* c_yield = new TCanvas();
    mg_yield->Draw("ap");
    c_yield->SaveAs();
  }///loop over rungroup 


}
