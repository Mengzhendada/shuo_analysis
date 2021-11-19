#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
int make_swif_input_lazy(){
  using json = nlohmann::json;
  json j_input;
  {
    std::ifstream ifs("shuo_analysis/simc/csv_simc_new.json");
    ifs>>j_input;
  }
  std::vector<json> j_jobs;
  j_jobs = j_input["jobs"].get<std::vector<json>>();
  json j_each = j_jobs[0];
  std::cout<<j_each["command"].get<string>()<<std::endl;
  std::vector<json> j_jobs_new;
  for(int i = 1;i<59;i++){
    //  read out 
    //  std::cout<<j_jobs[i]["command"].get<string>()<<std::endl; 
    std::string command = "/home/shuojia/simc_gfortran/script/run"+std::to_string(10*i)+".sh";
    j_each["command"] = command;
    j_jobs_new.push_back(j_each); 
  }
  j_input["jobs"] = j_jobs_new;
  std::ofstream ofs("shuo_analysis/simc/csv_simc_new.json");
  ofs<<j_input.dump(4)<<std::endl;
  return 0;
}
