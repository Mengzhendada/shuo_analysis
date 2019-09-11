#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
using nlohmann::json;
#include <vector>
#include <fstream>

void read_json(){
json j;
{
  std::ifstream infile("results/ratio_run_group.json");
  infile>>j;
}
std::vector<int> runs;
runs=j["3"]["neg"]["D2"].get<std::vector<int>>();
std::cout<<runs[0];
//std::cout<<j["1"]["neg"]["D2"]<<std::endl;
}
