#include <iostream>
#include <string>
#include "nlohmann/json.hpp"

int make_swif_input(){
  using json = nlohmann::json;
  json j_input;
  {
    std::ifstream ifs("shuo_analysis/simc/simcsp18.json");
    ifs>>j_input;
  }
  std::ofstream ofs("shuo_analysis/simc/simcsp18.json");
  ofs<<j_input.dump(4)<<std::endl;
  return 0;
}
