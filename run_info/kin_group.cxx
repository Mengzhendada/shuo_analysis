#include <vector>
#include <string>
#include <map>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>


void kin_group(){
  json j;
  {
    std::ifstream json_input_file("db2/ratio_run_group_updated.json");
    json_input_file>>j;
  }
  json j_xq2;
  json j_xq2z;
  for(json::iterator it = j.begin(); it!=j.end();++it){
    auto runjs = it.value();
    double x = runjs["x"].get<double>();
    double Q2 = runjs["Q2"].get<double>();
    double z = runjs["z"].get<double>();
    double shms_p = runjs["shms_p"].get<double>();
    double RunGroup = std::stoi(it.key());
    std::vector<int> pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    std::vector<int> neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    std::vector<int> pos_H2 = runjs["pos"]["H2"].get<std::vector<int>>();
    std::vector<int> neg_H2 = runjs["neg"]["H2"].get<std::vector<int>>();
    std::vector<int> pos_Dummy = runjs["pos"]["Dummy"].get<std::vector<int>>();
    std::vector<int> neg_Dummy = runjs["neg"]["Dummy"].get<std::vector<int>>();
    
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["shms_p"] = shms_p;
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["group_num"] = RunGroup;
    for(auto it = pos_D2.begin();it!=pos_D2.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["pos"]["D2"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["pos"]["D2"].push_back(*it);
    }
    for(auto it = neg_D2.begin();it!=neg_D2.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["neg"]["D2"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["neg"]["D2"].push_back(*it);
    }
    for(auto it = pos_H2.begin();it!=pos_H2.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["pos"]["H2"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["pos"]["H2"].push_back(*it);
    }
    for(auto it = neg_H2.begin();it!=neg_H2.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["neg"]["H2"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["neg"]["H2"].push_back(*it);
    }
    for(auto it = pos_Dummy.begin();it!=pos_Dummy.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["pos"]["Dummy"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["pos"]["Dummy"].push_back(*it);
    }
    for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();it++){
      j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["neg"]["Dummy"].push_back(*it);
      j_xq2z[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()][(std::to_string(z)).c_str()]["neg"]["Dummy"].push_back(*it);
    }
    //pos_D2_all = j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["pos"]["D2"].get<std::vector<int>>;
    //pos_D2_all.push_back(pos_D2);
    //j_xq2[(std::to_string(x)).c_str()][(std::to_string(Q2)).c_str()]["pos"]["D2"] = pos_D2_all;

  }
  std::string ofs_name = "kin_group_xq2.json";
  std::ofstream ofs(ofs_name.c_str());
  ofs<<j_xq2.dump(4)<<std::endl;
  
  std::string ofs_xq2z_name = "kin_group_xQ2z.json";
  std::ofstream ofs_xq2z(ofs_xq2z_name.c_str());
  ofs_xq2z<<j_xq2z.dump(4)<<std::endl;
}
