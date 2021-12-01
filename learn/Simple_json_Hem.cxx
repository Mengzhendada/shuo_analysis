#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
double TB_eff(double current){
  return 1-(2.3/10000)*current;
}

void Simple_json_Hem(){
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  json jout;
  for(auto it = j_info.begin();it!=j_info.end();it++){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;
    if(!it.value()["charge"].empty()){
      double charge = it.value()["charge"].get<double>();
      double time = it.value()["time"].get<double>();
      double current = charge/(time/1000);
      //double current = it.value()["current"].get<double>();
      double Target_boiling = TB_eff(current);
      j_info[it.key()]["Target_Boiling"] =  Target_boiling;
      j_info[it.key()]["current"] =  current;
      double SHMS_cal_eff = 0.96;
      j_info[it.key()]["SHMS_cal_eff"] = 0.96;
      
    }
    TString rootfile_name = "ROOTfile/coin_"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_scaler("TSP",rootfile_name);
    Ttree ****;
    double data_n = rootfile_scalertree.;
    double charge = scalertree;
    j_info[it.key()]["charge"] = charge_sum_corr;
  }//for loop over each runNumbers
  std::ofstream ofs("runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
