#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void yield_ratio(){

  json j;
  {
    std::ifstream runs("db2/ratio_run_group_updated.json");
    runs>>j;
  }

  for(json::iterator it = j.begin();it!=j.end();++it){
    std::vector<int> neg_D2_runs, pos_D2_runs;
    auto runjs = it.value();
    neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
    
  }

}
