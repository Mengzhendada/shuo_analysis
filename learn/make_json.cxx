#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_json(){
  std::ofstream ofs("rftime_cut.json");
  json j;
  for(int i = 0;i<58;i++){
  int RunGroup = 10*(i+1);
  j[(std::to_string(RunGroup)).c_str()]["rf_pi_low"] = 0.6;
  j[(std::to_string(RunGroup)).c_str()]["rf_pi_high"] = 1.4;
  j[(std::to_string(RunGroup)).c_str()]["rf_K_low"] = 1.4;
  j[(std::to_string(RunGroup)).c_str()]["rf_K_high"] = 2.2;
  j[(std::to_string(RunGroup)).c_str()]["rf_proton_low"] = 2.2;
  j[(std::to_string(RunGroup)).c_str()]["rf_proton_high"] = 4;
  j[(std::to_string(RunGroup)).c_str()]["pi_aero_npe_low"] = 6;
  j[(std::to_string(RunGroup)).c_str()]["pi_aero_npe_high"] = 30;
  j[(std::to_string(RunGroup)).c_str()]["K_aero_npe_low"] = 1;
  j[(std::to_string(RunGroup)).c_str()]["K_aero_npe_high"] = 12;
  }
  ofs<<j.dump(4)<<std::endl;

}
