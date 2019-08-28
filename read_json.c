#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
using nlohmann::json;
#include <vector>

void read_json(){
json j;
{
  std::ifstream infile("classify_runs.json");
  infile>>j;
}
std::vector<int> runs;
runs=j["3"]["neg"]["D2"].get<std::vector<int>>();
std::cout<<runs[0];
//std::cout<<j["1"]["neg"]["D2"]<<std::endl;
}
