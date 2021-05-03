#include "nlohmann/json.hpp"
  using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <vector>

  int write_lt_to_json(){
    json j_lt;
    { 
      std::ifstream ifs("results/LT/LT.json");
      ifs>>j_lt;
    }
    json j_runsinfo;
    {
      std::ifstream ifs("results/yield/runs_info.json");
      ifs>>j_runsinfo;
    }
    for(auto it = j_lt.begin();it!=j_lt.end();it++){
      int RunGroup = std::stoi(it.key());
      auto runjs = it.value();
      //neg runs  
      for(auto it_neg = runjs["neg"].begin();it_neg!=runjs["neg"].end();it_neg++){
        std::string neg_key = it_neg.key();
        if(std::isdigit(neg_key[0])){
        std::cout<<neg_key<<std::endl;
          int RunNumber = std::stoi(it_neg.key());
          auto ik_neg = it_neg.value();
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["TLT"] = j_lt[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
        }//if is digit 
      }//neg runs
      //pos runs
      for(auto it_pos = runjs["pos"].begin();it_pos!=runjs["pos"].end();it_pos++){
        std::string pos_key = it_pos.key();
        if(std::isdigit(pos_key[0])){
        std::cout<<pos_key<<std::endl;
          int RunNumber = std::stoi(it_pos.key());
          auto ik_pos = it_pos.value();
          j_runsinfo[(std::to_string(RunNumber)).c_str()]["TLT"] = j_lt[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
        }//if is digit 
      }//pos runs
    }//file rungroups
    std::ofstream outfile("results/yield/runs_info.json");
    outfile<<j_runsinfo.dump(4)<<std::endl;
    return 0;
  }//all
