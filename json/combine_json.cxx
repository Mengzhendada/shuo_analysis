#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>

void combine_json(){
  json j_fall;
  {
    std::ifstream ifs("db2/run_list.json");
    ifs>>j_fall;
  }
  json j_spring;
  {
    std::ifstream ifs("db2/run_list_coin.json");
    ifs>>j_spring;
  }
  std::ifstream fall_good("db2/fall_good_runlist.txt");
  std::ifstream spring_good("db2/spring_good_runlist.txt");
  json j_all;
  //j_all.push_back(j_fall);
  //j_all.push_back(j_spring);
  std::string RunNumber;
  while(fall_good>>RunNumber){
    //std::cout<<RunNumber<<std::endl;
    auto it = j_fall.find(RunNumber);
    j_all[RunNumber.c_str()] = *it;
  }
  while(spring_good>>RunNumber){
    auto it = j_spring.find(RunNumber);
    j_all[RunNumber.c_str()] = *it;
  }
  std::ofstream ofs("run_list_csv.json");
  ofs<<j_all.dump(4)<<std::endl;
}
