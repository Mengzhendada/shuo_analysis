#include <vector>
#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <fstream>
using nlohmann::json;

void runnumclassify1_update(){
json j;
{
  std::ifstream json_input_file("ratio_run_group.json");
  json_input_file>>j;
}
std::vector<int> run_group;
  for(json::iterator it = j.begin(); it!=j.end();++it){
    int group;
    group = it.value()["kin_group"].get<int>();
    run_group.push_back(group);
  }
//std:::vector<pair<int,int>> t;
//for(int i=0;i<run_group.size();++i){
//  t.push_back(make_pair(run_group[i],i));
//  sort(t.begin(),t.end(),[](const auto& a,const auto& b){return a.first<b.first})
//}
   sort(run_group.begin(),run_group.end());
   json k;
   for(int i = 0;i<run_group.size();++i){
     int group = run_group[i];
     k[std::to_string(group)] = i+1;
   }
std::ofstream outfile;
outfile.open("group_num.json");
outfile<<k<<std::endl;
for(json::iterator it = j.begin();it!=j.end();++it){
  int kin_group;
  int group_num;
  kin_group = it.value()["kin_group"].get<int>();
  group_num = k[std::to_string(kin_group)].get<int>();
  it.value()["group_num"]=group_num*10+3;
}
std::ofstream outfile_j;
outfile_j.open("ratio_run_group.json");
outfile_j<<j.dump(4)<<std::endl;
}
