#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int combine_json(){
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json jall;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    json jout;
    int RunGroup = std::stoi(it.key());
    std::vector<int> neg_D2,pos_D2;
    auto runjs = it.value();
    neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      std::string filename = "results/yield/run_info/"+std::to_string(RunGroup)+".json"; 
      std::ifstream ifs(filename.c_str());
      if(ifs.good()){
        ifs>>jout;
        for(auto it = jout.begin();it!=jout.end();it++){
          int RunNumber = std::stoi(it.key());
         // std::cout<<it.key()<<std::endl;
          jall[(std::to_string(RunNumber)).c_str()] = *it;
        }
        //jall.insert(jout.begin(),jout.end());
        
      }
      else{std::cout<<"can't find "<<RunGroup<<std::endl;}
    }
  }
  std::ofstream ofs("results/yield/run_info/runs_info.json");
  ofs<<jall.dump(4)<<std::endl;
  return 0;
}
