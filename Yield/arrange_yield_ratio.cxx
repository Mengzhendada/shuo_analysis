#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int arrange_yield_ratio(){
  json j_yield_ratio;
  {
    std::ifstream ifs("results/yield_ratio_xz_combined.json");
    ifs>>j_yield_ratio;
  }
  json jout;
  json jcheck;
  for(auto it = j_yield_ratio.begin();it!=j_yield_ratio.end();it++){
    double Q2 = std::stod(it.key());
    auto j_xbjz = it.value();
    std::cout<<"Q2 "<<Q2<<std::endl;    
    for(auto it = j_xbjz.begin();it!=j_xbjz.end();it++){
      double xbj = std::stod(it.key());
      auto j_z = it.value();
      std::cout<<"xbj "<<xbj;    
      for(auto it = j_z.begin();it!=j_z.end();it++){
        double z = std::stod(it.key());
        std::cout<<"z "<<z;
        auto j_value = it.value();
        std::vector<double> error_ratio,value_ratio;
        if(!j_value["value"].empty()){
          error_ratio = j_value["error"].get<std::vector<double>>();
          value_ratio = j_value["value"].get<std::vector<double>>();
          double weighted_mean_ratio = 0;
          double weighted_sigma_ratio = 0;
          for(int i = 0;i<error_ratio.size();++i){
            weighted_mean_ratio += value_ratio[i]/(error_ratio[i]*error_ratio[i]);
            weighted_sigma_ratio += 1/(error_ratio[i]*error_ratio[i]);
          }
          double mean_ratio = weighted_mean_ratio/weighted_sigma_ratio;
          double sig_ratio = std::sqrt(1/weighted_sigma_ratio);
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_ratio"] = mean_ratio;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_ratio"] = sig_ratio;

        }//if not empty
      }//loop over z
      std::cout<<std::endl;
    }//loop over xbj

  }//loop over Q2
  std::ofstream ofs("results/yield_ratio_xz_combined_weighted.json");
  ofs<<jout.dump(4)<<std::endl;


  return 0;
}
