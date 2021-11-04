#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

static double Get_all_eff(int RunNumber){
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
  double TLT = j_info[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
  double TEHMS = j_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
  double HMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
  //double HMS_cer_eff = j_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
  double SHMS_cal_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
  double SHMS_aero_eff = j_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
  double FADC_eff = j_info[(std::to_string(RunNumber)).c_str()]["FADC_eff"].get<double>();
  double Target_Boiling = j_info[(std::to_string(RunNumber)).c_str()]["Target_Boiling"].get<double>();
  return TE*TLT*TEHMS*HMS_cal_eff*SHMS_cal_eff*SHMS_aero_eff*FADC_eff*Target_Boiling;
};

#endif
