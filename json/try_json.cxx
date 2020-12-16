#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <>

void try_json(){
  std::vector<int> v1;
  v1 = {21021213,321321234,1324532,1324231};
  json j;
  int i = 3;
  //j[std::to_string(i)] = v1[1];
  j[std::to_string(i)]["i"] = v1[1];
  j[std::to_string(i)][std::to_string(i)] = v1[2];
  j["i"][std::to_string(i)]["i"] = i;
  j[std::to_string(i)]["this"]["that"]["what"] = v1;
  j["this"]["that"] = "i";
  
  std::ofstream out("test.json");
  out<<j.dump(4)<<std::endl;
}
