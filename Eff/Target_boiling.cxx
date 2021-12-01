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

void Target_boiling(){
  json j_info;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_info;
  }
  for(auto it = j_info.begin();it!=j_info.end();it++){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;
    if(!it.value()["current"].empty()){
      double current = it.value()["current"].get<double>();
      double Target_boiling = TB_eff(current);
      j_info[it.key()]["Target_Boiling"] =  Target_boiling;
    }
  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
