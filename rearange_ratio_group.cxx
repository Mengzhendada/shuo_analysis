#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
using json = nlohmann::json;

void rearange_ratio_group(){
  json j;
  {
    std::ifstream infile("dbase/ratio_run_group.json");
    infile>>j;
  }
  json k;
  for (json::iterator it = j.begin(); it!= j.end(); ++it){
    auto runjs = it.value();
    int kingroup = runjs["group_num"].get<int>();

    k[std::to_string(kingroup)] = it.value();
  }
  std::ofstream outfile("rearange_ratio_run_group.json");
  outfile<<k.dump(4)<<std::endl;
}
