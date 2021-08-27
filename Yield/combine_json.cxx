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
  json jsim;
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
      
      json jout_sim;
      std::string sim_filename = "results/yield/run_info/simc_"+std::to_string(RunGroup)+"_info.json"; 
      std::ifstream sim_ifs(sim_filename.c_str());
      if(sim_ifs.good()){
        sim_ifs>>jout_sim;
        for(auto it = jout_sim.begin();it!=jout_sim.end();it++){
          //std::cout<<it.key()<<std::endl;
          jsim[(std::to_string(RunGroup)).c_str()] = *it;
          std::cout<<it.value()["D2"]["neg"]["inc_norad"].get<double>()<<std::endl;;
          //std::cout<<jout_sim[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["inc_norad"].get<double>()<<std::endl;;
        }
        
      }
      else{std::cout<<"can't find sim "<<RunGroup<<std::endl;}
    }
  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<jall.dump(4)<<std::endl;
  std::ofstream ofs_sim("results/yield/simc_rungroups_info.json");
  ofs_sim<<jsim.dump(4)<<std::endl;
  return 0;
}
