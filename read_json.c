#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
using nlohmann::json;

void read_json(){
json j;
{
  std::ifstream infile("classify_runs.json");
  infile>>j;
}
std::cout<<j["1"]["hms_p"]<<std::endl;
}
