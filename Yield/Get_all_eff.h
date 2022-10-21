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
static double Get_posKpi_ratio_large(double xbj, double Q2, double z, bool need){
  json j_kpi;
  {
    std::ifstream ifs("results/pid/Kpi_ratio.json");
    ifs>>j_kpi;
  }
  if(need){
    double p0 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["largex"]["p0"].get<double>();
    double p1 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["largex"]["p1"].get<double>();
    return p0+p1*z;
  }
  else return 1;
}
static double Get_posKpi_ratio_small(double xbj, double Q2, double z, bool need){
  json j_kpi;
  {
    std::ifstream ifs("results/pid/Kpi_ratio.json");
    ifs>>j_kpi;
  }
  if(need){
    double p0 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["smallx"]["p0"].get<double>();
    double p1 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["smallx"]["p1"].get<double>();
    return p0+p1*z;
  }
  else return 1;
}
static double Get_negKpi_ratio_large(double xbj, double Q2, double z, bool need){
  json j_kpi;
  {
    std::ifstream ifs("results/pid/Kpi_ratio.json");
    ifs>>j_kpi;
  }
  if(need){
    double p0 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["largex"]["p0_neg"].get<double>();
    double p1 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["largex"]["p1_neg"].get<double>();
    return p0+p1*z;
  }
  else return 1;
}
static double Get_negKpi_ratio_small(double xbj, double Q2, double z, bool need){
  json j_kpi;
  {
    std::ifstream ifs("results/pid/Kpi_ratio.json");
    ifs>>j_kpi;
  }
  if(need){
    double p0 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["smallx"]["p0_neg"].get<double>();
    double p1 = j_kpi[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["smallx"]["p1_neg"].get<double>();
    return p0+p1*z;
  }
  else return 1;
}
static double Get_D2_neg_delta_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_neg_delta_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_pos_delta_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_pos_delta_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_neg_exc_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_neg_exc_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_pos_exc_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_pos_exc_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_neg_rho_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_neg_rho_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_pos_rho_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_pos_rho_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_neg_Dummy_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_neg_Dummy_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_pos_Dummy_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_pos_Dummy_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_neg_incnorad_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_neg_incnorad_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_D2_pos_incnorad_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["D2_pos_incnorad_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_neg_delta_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_neg_delta_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_pos_delta_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_pos_delta_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_pos_exc_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_pos_exc_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_neg_rho_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_neg_rho_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_pos_rho_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_pos_rho_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_neg_Dummy_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_neg_Dummy_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_pos_Dummy_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_pos_Dummy_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_neg_incnorad_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_neg_incnorad_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};
static double Get_H2_pos_incnorad_ratio(double xbj,double Q2,double z){
  json j_bgratio;
  {
    std::ifstream ifs("db2/bg_ratio.json");
    //std::ifstream ifs("results/yield/statistics_check/bg_ratio.json");
    ifs>>j_bgratio;
  }
  std::vector<double> pars = j_bgratio[(std::to_string(xbj)).c_str()][(std::to_string(Q2)).c_str()]["H2_pos_incnorad_pars"].get<std::vector<double>>();
  double ratio = pars[0]+pars[1]*z+pars[2]*z*z;
  return ratio;
};

#endif
