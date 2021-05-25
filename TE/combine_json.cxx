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
    std::vector<int> neg_D2,pos_D2,pos_Dummy,neg_Dummy;
    auto runjs = it.value();
    neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    neg_Dummy = runjs["neg"]["Dummy"].get<std::vector<int>>();
    pos_Dummy = runjs["pos"]["Dummy"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      std::string filename = "results/TE/trackingeff_info_"+std::to_string(RunGroup)+".json"; 
      std::ifstream ifs(filename.c_str());
      std::string HMSfilename = "results/TE/trackingeff_info_"+std::to_string(RunGroup)+"_HMS.json"; 
      std::ifstream ifsHMS(HMSfilename.c_str());
      if(ifs.good() && ifsHMS.good()){
        json j_HMS;
        ifsHMS>>j_HMS;
        json j_SHMS;
        ifs>>j_SHMS;
        
        for(auto its = neg_D2.begin();its!=neg_D2.end();its++){
          int RunNumber = *its;
          std::cout<<"neg"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"].get<int>();

          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>();
         //std::cout<<j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>()<<std::endl;
        }//loop over neg runs
        for(auto its = pos_D2.begin();its!=pos_D2.end();its++){
          int RunNumber = *its;
          std::cout<<"pos"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"].get<int>();

          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = j_HMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = j_HMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>();
        }//loop over pos runs
        for(auto its = neg_Dummy.begin();its!=neg_Dummy.end();its++){
          int RunNumber = *its;
          std::cout<<"neg"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["counts"].get<int>();

          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>();
         //std::cout<<j_HMS[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>()<<std::endl;
        }//loop over neg runs
        for(auto its = pos_Dummy.begin();its!=pos_Dummy.end();its++){
          int RunNumber = *its;
          std::cout<<"pos"<<RunNumber<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["SHMS_pi_found_1"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"] = j_SHMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["counts"].get<int>();

          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"] = j_HMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_expected"].get<int>();
          jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"] = j_HMS[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["HMS_e_found_1"].get<int>();
        }//loop over pos runs
      }//if good tracking infos
      else{std::cout<<"can't find "<<RunGroup<<" tracking infos."<<std::endl;}

    }//if not empty
  }//loov over rungroups
  std::ofstream ofs("results/TE/trackingeff_info.json");
  ofs<<jout.dump(4)<<std::endl;
  return 0;
}
