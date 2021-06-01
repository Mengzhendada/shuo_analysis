#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <vector>

int write_te_to_json(){
  json j_te;
  { 
    std::ifstream ifs("results/TE/trackingeff_info.json");
    ifs>>j_te;
  }
  json j_yield;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_yield;
  }
  std::ofstream ofs("need_redo.txt");
  for(auto it = j_te.begin();it!=j_te.end();it++){
    int RunGroup = std::stoi(it.key());
    auto runjs = it.value();
    //neg runs  
    for(auto it_neg = runjs["neg"].begin();it_neg!=runjs["neg"].end();it_neg++){
      std::string neg_key = it_neg.key();
      if(std::isdigit(neg_key[0])){
        std::cout<<neg_key<<std::endl;
        int RunNumber = std::stoi(it_neg.key());
        auto ik_neg = it_neg.value();
        int pi_found = ik_neg["SHMS_pi_found_1"].get<int>(); 
        int pi_expected = ik_neg["SHMS_pi_expected"].get<int>(); 
        int e_found = ik_neg["HMS_e_found_1"].get<int>();
        int e_expected = ik_neg["HMS_e_expected"].get<int>();
        if(pi_found!=0 && pi_expected!=0){
          double te = (double)pi_found/pi_expected;
          j_yield[(std::to_string(RunNumber)).c_str()]["TE"] = te;
        }
        else{
          std::cout<<"no SHMS tracking infos for "<<RunGroup<<" "<<neg_key<<std::endl;
          ofs<<neg_key<<std::endl;
          j_yield[(std::to_string(RunNumber)).c_str()]["TE"] = 1;
        }//if SHMS tracking info found
        if(e_found!=0 && e_expected!=0){
          double te_HMS = (double)e_found/e_expected;
          j_yield[(std::to_string(RunNumber)).c_str()]["TEHMS"] = te_HMS;
        }
        else{
          std::cout<<"no HMS tracking infos for "<<RunGroup<<" "<<neg_key<<std::endl;
          ofs<<neg_key<<std::endl;
          j_yield[(std::to_string(RunNumber)).c_str()]["TEHMS"] = 1;
        }
      }//if is digit 
    }//neg runs
    //pos runs
    for(auto it_pos = runjs["pos"].begin();it_pos!=runjs["pos"].end();it_pos++){
      std::string pos_key = it_pos.key();
      if(std::isdigit(pos_key[0])){
        std::cout<<pos_key<<std::endl;
        int RunNumber = std::stoi(it_pos.key());
        auto ik_pos = it_pos.value();
        int pi_found = ik_pos["SHMS_pi_found_1"].get<int>(); 
        int pi_expected = ik_pos["SHMS_pi_expected"].get<int>(); 
        int e_found = ik_pos["HMS_e_found_1"].get<int>();
        int e_expected = ik_pos["HMS_e_expected"].get<int>();
        if(pi_found!=0 && pi_expected!=0){
          double te = (double)pi_found/pi_expected;
          j_yield[(std::to_string(RunNumber)).c_str()]["TE"] = te;
        }
        else{
          std::cout<<"no SHMS tracking infos for "<<RunGroup<<" "<<pos_key<<std::endl;
          ofs<<pos_key<<std::endl;
          j_yield[(std::to_string(RunNumber)).c_str()]["TE"] = 1;
        }//if SHMS tracking info found
        if(e_found!=0 && e_expected!=0){
          double te_HMS = (double)e_found/e_expected;
          j_yield[(std::to_string(RunNumber)).c_str()]["TEHMS"] = te_HMS;
        }
        else{
          std::cout<<"no HMS tracking infos for "<<RunGroup<<" "<<pos_key<<std::endl;
          ofs<<pos_key<<std::endl;
          j_yield[(std::to_string(RunNumber)).c_str()]["TEHMS"] = 1;
        }

      }//if is digit 
    }//pos runs
  }//file rungroups
  std::ofstream outfile("results/yield/runs_info.json");
  outfile<<j_yield.dump(4)<<std::endl;
  return 0;
}//all
