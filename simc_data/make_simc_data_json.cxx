#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"

#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>

void make_simc_data_json(){
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_info;
  {
    std::ifstream ifs("db2/rungroup_info.json");
    ifs>>j_info;
  }
  json j_runs_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runs_info;
  }
  json j_simc_rungroup;
  {
    std::ifstream ifs("db2/simc_rungroup.json");
    ifs>>j_simc_rungroup;
  }
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    int RunGroup = std::stoi(it.key());
    auto runjs = it.value();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    std::vector<double> charge_neg,charge_pos;
    if(!neg_D2.empty() && !pos_D2.empty()){
      for(auto ineg = neg_D2.begin();ineg!=neg_D2.end();ineg++){
        int runnumber = *ineg;
        double charge = j_runs_info[(std::to_string(runnumber)).c_str()]["charge"].get<double>();    
        charge_neg.push_back(charge);

      }//go through neg runs

      for(auto ipos = pos_D2.begin();ipos!=pos_D2.end();ipos++){
        int runnumber = *ipos;
        double charge = j_runs_info[(std::to_string(runnumber)).c_str()]["charge"].get<double>();    
        charge_pos.push_back(charge);

      }//go through pos runs
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["charge"] = std::accumulate(charge_neg.begin(),charge_neg.end(),0.0);
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["normfac"] = j_simc_rungroup[(std::to_string(RunGroup)).c_str()]["D2"]["neg"];
      std::cout<<RunGroup<<" "<< j_simc_rungroup[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["inc"]["norad"]["normfac"].get<double>()<<std::endl;

      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["charge"] = std::accumulate(charge_pos.begin(),charge_pos.end(),0.0);
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["normfac"] = j_simc_rungroup[(std::to_string(RunGroup)).c_str()]["D2"]["pos"];
    }//if production runs
  }//go through all run groups
  std::ofstream outfile;
  outfile.open("results/db2/rungroup_info.json");
  outfile<<j_info.dump(4)<<std::endl;

}



