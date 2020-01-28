#include <string>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void reads_json(){
  json j;
  std::ifstream infile("failedruns.json");
  infile>>j;
  //std::cout<<j["events"].get<int>()<<std::endl;;
  //std::cout<<j["neg"][std::to_string(0)].get<double>();
  std::ofstream outfile("failed_runs.txt");
  for(json::iterator it = j.begin();it!=j.end();++it){
    outfile<<it.key()<<std::endl;
  }
}
