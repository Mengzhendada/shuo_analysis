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
double D2_TB_eff(double current){
  return 1-(2.7/10000)*current;
}

void Target_boiling(){
  json j_info;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_info;
  }
  json j_fall;
  {
    std::ifstream ifs("db2/run_list.json");
    ifs>>j_fall;
  }
  json j_spring;
  {
    std::ifstream ifs("db2/run_list_update.json");
    ifs>>j_spring;
  }
  for(auto it = j_info.begin();it!=j_info.end();it++){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;
    if(RunNumber < 7000){
      if(j_fall[(std::to_string(RunNumber)).c_str()]["target"]["target_if"]==2){
        if(!it.value()["current"].empty()){
          double current = it.value()["current"].get<double>();
          double Target_boiling = TB_eff(current);
          j_info[it.key()]["Target_Boiling"] =  Target_boiling;
        }
      }//H2 runs
      else{
        if(!it.value()["current"].empty()){
          double current = it.value()["current"].get<double>();
          double Target_boiling = D2_TB_eff(current);
          j_info[it.key()]["Target_Boiling"] =  Target_boiling;
        }
      }//D2 runs
    }//fall runs
    else{
      if(j_spring[(std::to_string(RunNumber)).c_str()]["target"]["target_if"]==2){
        if(!it.value()["current"].empty()){
          double current = it.value()["current"].get<double>();
          double Target_boiling = TB_eff(current);
          j_info[it.key()]["Target_Boiling"] =  Target_boiling;
        }
      }//H2 runs
      else{
        if(!it.value()["current"].empty()){
          double current = it.value()["current"].get<double>();
          double Target_boiling = D2_TB_eff(current);
          j_info[it.key()]["Target_Boiling"] =  Target_boiling;
        }
      }//D2 runs
    }//spring runs
  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
