#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int combine_json(){
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json jout;

  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    int RunGroup = std::stoi(it.key());
    std::cout<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2,neg_Dummy,pos_Dummy;
    auto runjs = it.value();
    neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    neg_Dummy = runjs["neg"]["Dummy"].get<std::vector<int>>();
    pos_Dummy = runjs["pos"]["Dummy"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      std::string filename = "results/LT/TLT_"+std::to_string(RunGroup)+".json"; 
      std::ifstream ifs(filename.c_str());
      std::string CLTfilename = "results/LT/CLT_"+std::to_string(RunGroup)+".json"; 
      std::ifstream ifsCLT(CLTfilename.c_str());
      if(ifs.good() && ifsCLT.good()){
        json j_TLT;
        ifs>>j_TLT;
        json j_CLT;
        ifsCLT>>j_CLT;
        
        for(auto its = neg_D2.begin();its!=neg_D2.end();its++){
          int RunNumber = *its;
          std::cout<<"neg"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["TLT"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["TLT_error"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["CLT_all"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["CLT_phy"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
        }//loop over neg runs
        for(auto its = pos_D2.begin();its!=pos_D2.end();its++){
          int RunNumber = *its;
          std::cout<<"pos"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["TLT"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["TLT_error"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["CLT_all"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["CLT_phy"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
        }//loop over pos runs
        for(auto its = neg_Dummy.begin();its!=neg_Dummy.end();its++){
          int RunNumber = *its;
          std::cout<<"neg Dummy "<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["TLT"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["TLT_error"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<int>();
          //jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["CLT_all"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
          //jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["CLT_phy"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
        }//loop over neg runs
        for(auto its = pos_Dummy.begin();its!=pos_Dummy.end();its++){
          int RunNumber = *its;
          std::cout<<"pos Dummy "<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["TLT"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT"].get<double>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["TLT_error"] = j_TLT[(std::to_string(RunNumber)).c_str()]["TLT_error"].get<int>();
          //jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["CLT_all"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_all"].get<double>();
          //jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["CLT_phy"] = j_CLT[(std::to_string(RunNumber)).c_str()]["CLT_phy"].get<double>();
        }//loop over pos runs
      }//if good tracking infos
      else{std::cout<<"can't find "<<RunGroup<<" LT infos."<<std::endl;}

    }//if not empty
  }//loov over rungroups
  std::ofstream ofs("results/LT/LT.json");
  ofs<<jout.dump(4)<<std::endl;
  return 0;
}
