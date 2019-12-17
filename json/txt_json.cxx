#include <iostream>
#include <fstream>
#include <string>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void txt_json(){
  std::ifstream ifs("results/hms_cal.txt");
  int RunNumber;
  double mean;
  double sigma;
  json j;
  {
    std::ifstream ifs("results/hms_cal.json");
    ifs>>j;
  }
  while(ifs>>RunNumber>>mean>>sigma){
    j[std::to_string(RunNumber)]["mean"] = mean;
    j[std::to_string(RunNumber)]["sigma"] = sigma;
    
  }
  std::ofstream ofs("results/hms_cal.json");
  ofs<<j.dump(4)<<std::endl;
}
