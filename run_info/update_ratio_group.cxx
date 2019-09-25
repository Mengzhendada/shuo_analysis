#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <fstream>
using json = nlohmann::json;

void update_ratio_group(){
  json j;
  {
    std::ifstream infile("db2/ratio_run_group_updated.json");
    infile>>j;
  }
json k;
  for(json::iterator it = j.begin();it!=j.end();++it){
    std::string group = std::to_string(it.value()["group_num"].get<int>());
    std::cout<<group<<std::endl;
    std::string string = "y";
    std::string kingroup = std::to_string(it.value()["kin_group"].get<int>());
    while(string == "y"){
      std::cout<<"This is run group "<<kingroup<<std::endl;
      double Q2;
      std::cout<<"Enter Q2"<<std::endl;
      std::cin>>Q2;
      j[group]["Q2"] = Q2;
      k[group]["Q2"] = Q2;
      double x;
      std::cout<<"Enter x"<<std::endl;
      std::cin>>x;
      j[group]["x"] = x;
      k[group]["x"] = x;
      double z;
      std::cout<<"Enter z"<<std::endl;
      std::cin>>z;
      j[group]["z"] = z;
      k[group]["z"] = z;
      std::cout<<"Do you want to change? y for yes, n for no"<<std::endl;
      std::cin>>string;
    }
  }
  std::ofstream outfile("ratio_run_group_updated1.json");
  outfile<<k.dump(4)<<std::endl;
  std::ofstream outfile2("ratio_run_group_updated2.json");
  outfile2<<j.dump(4)<<std::endl;
}
