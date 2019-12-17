#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <vector>

void json_test(){
  json j;
  {
  std::ifstream ifs("db2/HMS_cuts.json");
  ifs>>j;
  }
  std::vector<double> v1;
  v1 = j["cal_cuts"].get<std::vector<double>>();
  for(auto it = v1.begin();it!=v1.end();++it){
    std::cout<<*it<<std::endl;
  }
}
