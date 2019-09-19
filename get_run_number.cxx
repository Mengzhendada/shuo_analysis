#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
using json = nlohmann::json;

void get_run_number(int run_group = 0, int polarity = 0,int target = 0){
  if(run_group == 0){
    std::cout<<"Enter a run group number(-1 to exit):";
    std::cin>>run_group;
    if(run_group <= 0)
      return;
  }
  if(polarity ==0){
    std::cout<<"Enter polarity, 1 for negative shms p runs, 2 for positive shms p runs, 3 for all,-1 to exit";
    std::cin>>polarity;
    if(polarity <0)
      return;
  }
  if(target == 0){
    std::cout<<"Enter target, 2 for H2, 3 for D2, 5 for Dummy, 1 for all, -1 to exit";
    std::cin>>target;
    if(target <0)
      return;
  }

  json j;
  {
    std::ifstream infile("results/ratio_run_group_updated.json");
    infile>>j;
  }

  std::string group_num;
  group_num = std::to_string((int)floor(run_group/10)*10);
  //std::cout<<group_num<<std::endl;
  switch(polarity){
    case 1 : {
               switch(target){
                 case 2 : {
                            std::vector<int> v1 = j[group_num]["neg"]["H2"].get<std::vector<int>>();
                            for(auto it = v1.begin();it!=v1.end();++it){ std::cout<<*it<<std::endl;}
                          }
                 case 3 : {
                            std::vector<int> v1 = j[group_num]["neg"]["D2"].get<std::vector<int>>();
                            for(auto it = v1.begin();it!=v1.end();++it){ std::cout<<*it<<std::endl;}
                          }  
                 case 5 : {
                            std::vector<int> v1 = j[group_num]["neg"]["Dummy"].get<std::vector<int>>();
                            for(auto it = v1.begin();it!=v1.end();++it){ std::cout<<*it<<std::endl;}
                          }
                 case 1 : {
                            std::vector<int> v1,v2;
                            v2 = j[group_num]["neg"]["H2"].get<std::vector<int>>();
                            v1 = v2;
                            v2 = j[group_num]["neg"]["D2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["neg"]["Dummy"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                          for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
               }
             }
    case 2 : {
               switch(target){
                 case 2 : {
                            std::vector<int> v1 = j[group_num]["pos"]["H2"].get<std::vector<int>>();
                          for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 3 : {
                            std::vector<int> v1 = j[group_num]["pos"]["D2"].get<std::vector<int>>();
                          for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 5 : {
                            std::vector<int> v1 = j[group_num]["pos"]["Dummy"].get<std::vector<int>>();
                          for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 1 : {
                            std::vector<int> v1,v2;
                            v2 = j[group_num]["pos"]["H2"].get<std::vector<int>>();
                            v1 = v2;
                            v2 = j[group_num]["pos"]["D2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["pos"]["Dummy"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                          for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
               }
             }
    case 3 : {
               switch(target){
                 case 2 : {
                            std::vector<int> v1,v2;
                            v1 = j[group_num]["neg"]["H2"].get<std::vector<int>>();
                            v2 = j[group_num]["pos"]["H2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                            for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 3 : {
                            std::vector<int> v1,v2;
                            v1 = j[group_num]["neg"]["D2"].get<std::vector<int>>();
                            v2 = j[group_num]["pos"]["D2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                            for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 5 : {
                            std::vector<int> v1,v2;
                            v1 = j[group_num]["neg"]["Dummy"].get<std::vector<int>>();
                            v2 = j[group_num]["pos"]["Dummy"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                            for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
                 case 1 : {
                            std::vector<int> v1,v2;
                            v2 = j[group_num]["pos"]["H2"].get<std::vector<int>>();
                            v1 = v2;
                            v2 = j[group_num]["pos"]["D2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["pos"]["Dummy"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["neg"]["H2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["neg"]["D2"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            v2 = j[group_num]["neg"]["Dummy"].get<std::vector<int>>();
                            v1.insert(v1.end(),v2.begin(),v2.end());
                            std::sort(v1.begin(),v1.end());
                            for(auto it = v1.begin();it!=v1.end();++it){std::cout<<*it<<std::endl;}
                          }
               }
             }
  }
}

