#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int arrange_H2_diff_sum(){
  json j_diff_sum;
  {
    std::ifstream ifs("results/yield/H2_diff_sum.json");
    ifs>>j_diff_sum;
  }
  json jout;
  json jcheck;
  for(auto it = j_diff_sum.begin();it!=j_diff_sum.end();it++){
    double Q2 = std::stod(it.key());
    auto j_xbjz = it.value();
    std::cout<<"Q2 "<<Q2<<std::endl;    
          double Q2_arrange;
          if(Q2<4.1){
            Q2_arrange = 4;
          }//for first Q2
          if(Q2>4.1 && Q2<4.9){
            Q2_arrange = 4.75;
          }//for second Q2
          if(Q2>4.9 && Q2<5.1){
            Q2_arrange = 5;
          }//for third Q2
          if(Q2>5.1){
            Q2_arrange = 5.5;
          }//for fourth Q2
          std::string Q2_str = std::to_string(Q2_arrange);
    for(auto it = j_xbjz.begin();it!=j_xbjz.end();it++){
      double xbj = std::stod(it.key());
      auto j_z = it.value();
      std::cout<<"xbj "<<xbj;    
      for(auto it = j_z.begin();it!=j_z.end();it++){
        double z = std::stod(it.key());
        std::cout<<"z "<<z;
        auto j_value = it.value();
        std::vector<double> error_diff,error_sum,value_diff,value_sum;
        if(!j_value["error_diff"].empty()){
          error_diff = j_value["error_diff"].get<std::vector<double>>();
          value_diff = j_value["value_diff"].get<std::vector<double>>();
            for(auto it_errordiff = error_diff.begin();it_errordiff<error_diff.end();++it_errordiff){
            jout[Q2_str.c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_diff"].push_back(*it_errordiff);}
            for(auto it_valuediff = value_diff.begin();it_valuediff<value_diff.end();++it_valuediff){
            jout[Q2_str.c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff"].push_back(*it_valuediff);}
        }//if diff exist
        if(!j_value["error_sum"].empty()){
          error_sum = j_value["error_sum"].get<std::vector<double>>();
          value_sum = j_value["value_sum"].get<std::vector<double>>();
            for(auto it_errorsum = error_sum.begin();it_errorsum<error_sum.end();++it_errorsum){
            jout[Q2_str.c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_sum"].push_back(*it_errorsum);}
            for(auto it_valuesum = value_sum.begin();it_valuesum<value_sum.end();++it_valuesum){
            jout[Q2_str.c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_sum"].push_back(*it_valuesum);
            jcheck["value_sum"].push_back(*it_valuesum);
            }
        }//if sum exist
      }//loop over z
      std::cout<<std::endl;
    }//loop over xbj

  }//loop over Q2
  //std::vector<double> error_diff_raw = jout["4.000000"]["0.350000"]["0.425000"]["error_diff"].get<std::vector<double>>();
  std::ofstream ofs("results/yield/arrange_H2_diff_sum.json");
  ofs<<jout.dump(4)<<std::endl;
  std::ofstream ofs_check("results/yield/check_H2_diff_sum.json");
  ofs_check<<jcheck.dump(4)<<std::endl;
  
  
  return 0;
}
