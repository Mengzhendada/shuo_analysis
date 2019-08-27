#include <iostream>
#include <string>
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

void grepcurrent_to_json(int RunNumber = 0){

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
  std::ifstream infile;
  infile.open(RunFileNamePattern);
  //assert(infile.good());
  if(infile.good()){std::cout<<"Found file "<<RunFileNamePattern<<std::endl;}
  else{std::cout<<"Check file! "<<RunFileNamePattern<<"for "<<RunNumber<<std::endl;}
  //infile.ignore(50000,'cdipc_ypos');
  std::string line,temp;
  int i=0;
  double current;
  json j,tmpj;
  j = {{"RunNumber",RunNumber } };
  std::string string[45] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status"};
  while(std::getline(infile,line) && i<45){  
    
   // std::getline(infile,line);
    if(line.find(string[i])<line.length())
    {
      //std::cout<<"check"<<string[i]<<std::endl;
      //std::cout<<"check"<<line<<std::endl;
      
      for(int b = string[i].size();b<line[b];b++)
      {
        if(isdigit(line[b]))
        {
          for(int a = b;a<line.size();a++)
          {
           // std::cout<<"check2"<<std::endl;
            temp += line[a];
          }
          break;
        }
      }
      //std::cout<<temp<<std::endl;
      std::istringstream stream(temp);
      stream>>current;
      std::cout<<string[i]<<" "<<current<<std::endl;
      tmpj = {
          {string[i],current}
        };
      j.insert(tmpj.begin(),tmpj.end());
      temp.clear();
      i++;
    }
  
  }
  std::string jsonname = fmt::format("Data/magnet_current_{}.json",RunNumber).c_str();
  std::ofstream o2(jsonname);
  o2<<std::setw(4)<<j<<std::endl;
}
