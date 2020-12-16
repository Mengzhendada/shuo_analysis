#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <>

void try_json_read(){
  json j_test;
  {
    std::ifstream ifs("test.json");
    ifs>>j_test;
  }
  int n1 = j_test["i"]["3"]["i"].get<int>();
  std::cout<<n1<<std::endl;
}
