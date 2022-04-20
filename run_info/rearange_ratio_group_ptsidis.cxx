#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
using json = nlohmann::json;

void rearange_ratio_group_ptsidis(){
  json j;
  {
    std::ifstream infile("ratio_run_group_ptsidis.json");
    infile>>j;
  }
  json k;
  for (json::iterator it = j.begin(); it!= j.end(); ++it){
    auto runjs = it.value();
    int kingroup = runjs["group_num"].get<int>();

    k[std::to_string(580+kingroup)] = it.value();
  }
  std::ofstream outfile("rearange_ratio_run_group_ptsidis.json");
  outfile<<k.dump(4)<<std::endl;
}
