#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TH1.h"
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

bool root_file_exists(std::string rootfile) {
  bool found_good_file = false;
  if (!gSystem->AccessPathName(rootfile.c_str())) {
    TFile file(rootfile.c_str());
    if (file.IsZombie()) {
      std::cout << rootfile << " is a zombie.\n";
      std::cout
          << " Did your replay finish?  Check that the it is done before running this script.\n";
      return false;
      // return;
    } else {
      std::cout << " using : " << rootfile << "\n";
      return true;
    }
  }
  return false;
}

void statistic_runs(){
  json j_info;
  {
    std::ifstream infile("db2/run_info_group.json");
    infile>>j_info;
  }
  json jout;
  for(json::iterator it = j_info.begin();it!=j_info.end();++it){
    int RunNumber = std::stoi(it.key());
    auto runjs = it.value();
    std::string rootfile_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
    int entries=0;
    if(root_file_exists(rootfile_name.c_str())){
      double shms_p = std::abs(runjs["shms_p"].get<double>());
      if(shms_p<2.7){
        TH1D *h_xbj = new TH1D({"xbj_aero","aeroCut;xbj;counts",100,0,1});
        TFile *kinfile = new TFile(rootfile_name.c_str());
        h_xbj = (TH1D*)kinfile->Get("xbj_aero");
        entries = (int) h_xbj->GetEntries();
      }
      else{
        TH1D *h_xbj = new TH1D({"xbj_hgcer","aeroCut;xbj;counts",100,0,1});
        TFile *kinfile = new TFile(rootfile_name.c_str());
        h_xbj = (TH1D*)kinfile->Get("xbj_hgcer");
        entries = (int) h_xbj->GetEntries();
        
      }
    }
    std::string root_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d("TSP",root_name);
    double charge = *d.Max("P.BCM4B.scalerChargeCut");
    std::cout<<charge<<std::endl;
    std::cout<<entries<<std::endl;
    jout[it.key()]["nentries"] = entries;
  jout[it.key()]["charge"] = charge;
  }
 std::ofstream outfile("results/statistics/statistics_run.json");
 outfile<<jout.dump(4)<<std::endl;
}
