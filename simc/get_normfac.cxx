#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void get_normfac(int RunGroup = 0){

  if(RunGroup == 0){
    std::cout<<"Enter Rungroup Number: "<<std::endl;
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;

  }
  std::string hist_D2_neg_inc_norad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_norad.hist";
  std::string hist_D2_neg_inc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_rad.hist";
  std::string hist_D2_neg_exc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_neg_exc_rad.hist";
  std::string hist_D2_neg_rho_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_neg_rho.hist";
  std::string hist_D2_pos_inc_norad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_norad.hist";
  std::string hist_D2_pos_inc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.hist";
  std::string hist_D2_pos_exc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_exc_rad.hist";
  std::string hist_D2_pos_rho_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_rho.hist";

  std::ifstream D2_neg_inc_norad;
  std::ifstream D2_neg_inc_rad;
  std::ifstream D2_neg_exc_rad;
  std::ifstream D2_neg_rho;
  std::ifstream D2_pos_inc_norad;
  std::ifstream D2_pos_inc_rad;
  std::ifstream D2_pos_exc_rad;
  std::ifstream D2_pos_rho;

  D2_neg_inc_norad.open(hist_D2_neg_inc_norad_name.c_str());
  D2_neg_inc_rad.open(hist_D2_neg_inc_rad_name.c_str());
  D2_neg_exc_rad.open(hist_D2_neg_exc_rad_name.c_str());
  D2_neg_rho.open(hist_D2_neg_rho_name.c_str());
  D2_pos_inc_norad.open(hist_D2_pos_inc_norad_name.c_str());
  D2_pos_inc_rad.open(hist_D2_pos_inc_rad_name.c_str());
  D2_pos_exc_rad.open(hist_D2_pos_exc_rad_name.c_str());
  D2_pos_rho.open(hist_D2_pos_rho_name.c_str());

  json j_simc;

  std::string line;
  std::string normfac = "normfac";
  double normfac_num;
  while(std::getline(D2_neg_inc_norad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["inc"]["norad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_neg_inc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["inc"]["rad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_neg_exc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["exc"]["norad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_neg_rho,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["rho"]["normfac"] = normfac_num;
    }
  }
  while(std::getline(D2_pos_inc_norad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["inc"]["norad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_pos_inc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["inc"]["rad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_pos_exc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["exc"]["norad"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_pos_rho,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["rho"]["normfac"] = normfac_num;
    }
  }
  std::string of_name = "/group/c-csv/shuo/shuo_analysis/simc/outfiles/"+std::to_string(RunGroup)+".json";
  std::ofstream of(of_name.c_str());
  of<<j_simc.dump(4)<<std::endl;



}
