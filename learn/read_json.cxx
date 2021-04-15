#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
using json = nlohmann::json;
#include <vector>
#include <fstream>

void read_json(){
json j;
//{
//  std::ifstream infile("db2/ratio_run_group_updated.json");
//  infile>>j;
//}
//std::vector<int> runs;
//runs=j["70"]["neg"]["D2"].get<std::vector<int>>();
//std::cout<<runs[0];
//std::cout<<j["1"]["neg"]["D2"]<<std::endl;
j["1"] = 1;
//std::cout<<j["1"]<<std::endl;
std::ofstream ofs;
ofs.open("try.json");
//ofs<< j <<std::endl;
//ofs<<j.dump(4)<<std::endl;
//ofs<<"hello world"<<std::endl;
}
