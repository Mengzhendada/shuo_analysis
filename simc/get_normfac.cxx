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
  std::string hist_D2_neg_delta_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_neg_delta.hist";
  std::string hist_Dummy_neg_inc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_Dummy_neg_inc_rad.hist";
  std::string hist_D2_pos_inc_norad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_norad.hist";
  std::string hist_D2_pos_inc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.hist";
  std::string hist_D2_pos_exc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_exc_rad.hist";
  std::string hist_D2_pos_rho_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_rho.hist";
  std::string hist_D2_pos_delta_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_D2_pos_delta.hist";
  std::string hist_Dummy_pos_inc_rad_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv_"+std::to_string(RunGroup)+"_Dummy_pos_inc_rad.hist";

  std::ifstream D2_neg_inc_norad;
  std::ifstream D2_neg_inc_rad;
  std::ifstream D2_neg_exc_rad;
  std::ifstream D2_neg_rho;
  std::ifstream D2_neg_delta;
  std::ifstream Dummy_neg_inc_rad;
  std::ifstream D2_pos_inc_norad;
  std::ifstream D2_pos_inc_rad;
  std::ifstream D2_pos_exc_rad;
  std::ifstream D2_pos_rho;
  std::ifstream D2_pos_delta;
  std::ifstream Dummy_pos_inc_rad;

  D2_neg_inc_norad.open(hist_D2_neg_inc_norad_name.c_str());
  D2_neg_inc_rad.open(hist_D2_neg_inc_rad_name.c_str());
  D2_neg_exc_rad.open(hist_D2_neg_exc_rad_name.c_str());
  D2_neg_rho.open(hist_D2_neg_rho_name.c_str());
  D2_neg_delta.open(hist_D2_neg_delta_name.c_str());
  Dummy_neg_inc_rad.open(hist_Dummy_neg_inc_rad_name.c_str());
  D2_pos_inc_norad.open(hist_D2_pos_inc_norad_name.c_str());
  D2_pos_inc_rad.open(hist_D2_pos_inc_rad_name.c_str());
  D2_pos_exc_rad.open(hist_D2_pos_exc_rad_name.c_str());
  D2_pos_rho.open(hist_D2_pos_rho_name.c_str());
  D2_pos_delta.open(hist_D2_pos_delta_name.c_str());
  Dummy_pos_inc_rad.open(hist_Dummy_pos_inc_rad_name.c_str());

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
      std::cout<<"neg inc norad"<<normfac_num<<std::endl;
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
      std::cout<<"neg inc rad"<<normfac_num<<std::endl;
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
      std::cout<<"neg exc rad"<<normfac_num<<std::endl;
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
      std::cout<<"neg rho"<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["rho"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_neg_delta,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<"neg delta"<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["delta"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(Dummy_neg_inc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<"Dummy neg inc rad"<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["neg"]["Dummy"]["normfac"] = normfac_num;
    }
  }
  while(std::getline(D2_pos_inc_norad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<"pos inc norad"<<normfac_num<<std::endl;
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
      std::cout<<"pos inc rad"<<normfac_num<<std::endl;
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
      std::cout<<"pos exc rad"<<normfac_num<<std::endl;
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
      std::cout<<"pos rho"<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["rho"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(D2_pos_delta,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<"pos delta"<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["delta"]["normfac"] = normfac_num;
    }
  }
  line = "";
  while(std::getline(Dummy_pos_inc_rad,line)){
    if(line.find(normfac)<line.length()){
      std::cout<<line<<std::endl;
      size_t i = 0;
      for(;i < line.length();i++){ if(isdigit(line[i])) break; }
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<"Dummy pos inc rad "<<normfac_num<<std::endl;
      j_simc[(std::to_string(RunGroup)).c_str()]["pos"]["Dummy"]["normfac"] = normfac_num;
    }
  }
  std::string of_name = "/group/c-csv/shuo/shuo_analysis/simc/outfiles/"+std::to_string(RunGroup)+".json";
  std::ofstream of(of_name.c_str());
  of<<j_simc.dump(4)<<std::endl;



}
