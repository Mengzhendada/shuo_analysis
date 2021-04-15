#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void print_run_info(){
  json j;
  {
    std::ifstream infile("db2/ratio_run_group_updated.json");
    infile>>j;
  }
  json k,l;
  json l2;
  json l3;
  json l4;
  //{
  //  std::ifstream infile("kin_group.json");
  //  infile>>l2;
  //}
  double hms_p,shms_p,x,Q2,z,run_group;

  for(json::iterator it = j.begin();it!=j.end();++it){
    auto runjs = it.value();
    hms_p = runjs["hms_p"].get<double>();
    shms_p = runjs["shms_p"].get<double>();
    x = runjs["x"].get<double>();
    Q2 = runjs["Q2"].get<double>();
    z = runjs["z"].get<double>();
    run_group = runjs["group_num"].get<int>();
    std::vector<int> v,v_all,v_runs;
    v = runjs["neg"]["D2"].get<std::vector<int>>();
    v_all = v;
      l2[std::to_string(x)][std::to_string(z)]["group_num"] = run_group;;
      l3[std::to_string(Q2)][std::to_string(z)]["group_num"] = run_group;;
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["group_num"] = run_group;;
      
      if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = 0-shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 3;
      l2[std::to_string(x)][std::to_string(z)]["neg"]["D2"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["neg"]["D2"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["neg"]["D2"].push_back(runnum);
      }
      v_runs.clear();
    }
    v = runjs["neg"]["H2"].get<std::vector<int>>();
    v_all.insert(v_all.end(),v.begin(),v.end());
    if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = 0-shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 2;
      v_runs.push_back(runnum);
      l2[std::to_string(x)][std::to_string(z)]["neg"]["H2"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["neg"]["H2"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["neg"]["H2"].push_back(runnum);
      }
      v_runs.clear();
    }
    v = runjs["neg"]["Dummy"].get<std::vector<int>>();
    v_all.insert(v_all.end(),v.begin(),v.end());
    if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = 0-shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 5;
      v_runs.push_back(runnum);
      l2[std::to_string(x)][std::to_string(z)]["neg"]["Dummy"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["neg"]["Dummy"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["neg"]["Dummy"].push_back(runnum);
      }
      v_runs.clear();
    }
    v = runjs["pos"]["D2"].get<std::vector<int>>();
    v_all.insert(v_all.end(),v_all.begin(),v_all.end());
    if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 3;
      v_runs.push_back(runnum);
      l2[std::to_string(x)][std::to_string(z)]["pos"]["D2"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["pos"]["D2"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["pos"]["D2"].push_back(runnum);
      }
      v_runs.clear();
    }
    v = runjs["pos"]["H2"].get<std::vector<int>>();
    v_all.insert(v_all.end(),v.begin(),v.end());
    if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 2;
      v_runs.push_back(runnum);
      l2[std::to_string(x)][std::to_string(z)]["pos"]["H2"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["pos"]["H2"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["pos"]["H2"].push_back(runnum);
      }
      v_runs.clear();
    }
    v = runjs["pos"]["Dummy"].get<std::vector<int>>();
    v_all.insert(v_all.end(),v.begin(),v.end());
    if(!v.empty()){
      for(auto ik = v.begin();ik!=v.end();++ik){
      int runnum = *ik;
      //std::cout<<runnum<<std::endl;
      k[std::to_string(runnum)]["x"] = x;
      k[std::to_string(runnum)]["Q2"] = Q2;
      k[std::to_string(runnum)]["z"] = z;
      k[std::to_string(runnum)]["hms_p"] = hms_p;
      k[std::to_string(runnum)]["shms_p"] = shms_p;
      k[std::to_string(runnum)]["run_group"] = run_group;
      k[std::to_string(runnum)]["target_id"] = 5;
      v_runs.push_back(runnum);
      l2[std::to_string(x)][std::to_string(z)]["pos"]["Dummy"].push_back(runnum);
      l3[std::to_string(Q2)][std::to_string(z)]["pos"]["Dummy"].push_back(runnum);
      l4[std::to_string(Q2)][std::to_string(z)][std::to_string(x)]["pos"]["Dummy"].push_back(runnum);
      }
      v_runs.clear();
    }

 //   for(auto ik = v_all.begin();ik!=v_all.end();++ik){
 //   }
    //std::cout<<v_all.size()<<std::endl;
    //std::cout<<"check"<<std::endl;
    v.clear();
    v_all.clear();
  }//end of json iterator for loop
  std::ofstream outfile("run_info_group.json");
  outfile<<k.dump(4)<<std::endl;
  std::ofstream out2("kin_group_xz.json");
  out2<<l2.dump(4)<<std::endl;
  std::ofstream out3("kin_group_Q2z.json");
  out3<<l3.dump(4)<<std::endl;
  std::ofstream out4("kin_group_Q2zx.json");
  out4<<l4.dump(4)<<std::endl;
}
