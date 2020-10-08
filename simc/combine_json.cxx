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
      std::string filename = "shuo_analysis/simc/outfiles/"+std::to_string(RunGroup)+".json"; 
      std::ifstream ifs(filename.c_str());
      if(ifs.good()){
        ifs>>jout;
        jall[std::to_string(RunGroup).c_str()]["D2"] = jout[std::to_string(RunGroup).c_str()];
        
      }
      else{std::cout<<"can't find "<<RunGroup<<std::endl;}
    }
  
  std::ofstream ofs("shuo_analysis/simc/outfiles/simc_rungroup.json");
  ofs<<jall.dump(4)<<std::endl;
  return 0;
}
