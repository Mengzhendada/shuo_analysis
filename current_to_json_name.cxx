#include <iostream>
#include <string>
#include <array>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
//R__LOAD__LIBRARY(libfmt.so)
#include "spdlog/spdlog.h"
using json = nlohmann::json;
using namespace std;
#include <bits/stdc++.h>
#include <numeric>

void current_to_json_name(int RunNumber = 0){

  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_every(std::chrono::seconds(5));

  // Get RunNumber if not provided.
  if (RunNumber == 0) {
    cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if (RunNumber <= 0)
      return;
  }
  // Create file name patterns.
  string     RunFileNamePattern = "raw/coin_all_0"+std::to_string(RunNumber)+".dat";
  std::cout<<RunFileNamePattern<<std::endl;
  //infile.ignore(50000,'cdipc_ypos');
  std::ofstream outfile;
  outfile.open("interested_valuess.txt");
  int ii=0;
  std::string string[55] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status","hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};
  std::ifstream infile;
  infile.open(RunFileNamePattern);
  std::string line,temp;
  if(infile.fail()){std::cout<<"check file "<<RunFileNamePattern<<std::endl;}
  int i = 0;
    std::cout<<"check 1"<<string[i]<<std::endl;
    while(std::getline(infile,line) && i<55){
      std::cout<<"check 2"<<string[i]<<std::endl;
      if(line.find(string[i])<line.length())
      {
        std::cout<<"check1 "<<string[i]<<" "<<ii<<std::endl;
        outfile<<line<<std::endl;        
    i++;
      }//end if line find string
    }//end reading lines
  
}
