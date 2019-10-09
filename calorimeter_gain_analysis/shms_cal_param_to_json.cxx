#include "nlohmann/json.hpp"
#include "ROOT/RDataFrame.hxx"
#include <vector>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include "SHMS_CAL_Read_Calparam.h"
using json = nlohmann::json;
#include <fstream>
#include <string>

void shms_cal_param_to_json(int RunNumber = 0){
  if(RunNumber ==0)
  {
    std::cout<<"Enter Run Number(-1 to exit)"<<std::endl;
    std::cin>>RunNumber;
    if (RunNumber <=0){return;}
  }
  json tmp;
  //std::ifstream infile;
  std::string name;
  if(RunNumber>7000){
    name = "shuo_analysis/calorimeter_gain_analysis/csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
  }
  else{
    name = "shuo_analysis/calorimeter_gain_analysis/csvfallcalib/shms_cal/pcal.param.coin_replay_production_";
  }
  name = name+std::to_string(RunNumber)+"_-1_0_-1";
  std::cout<<name<<std::endl;
  std::ifstream infile;
  infile.open(name);
  if(infile.fail()){
    std::cout<<"Can't find "<<name<<std::endl;
    exit;
  }
  auto rc= new SHMS_CAL_Read_Calparam(infile);
  tmp["RunNumber"] = RunNumber;
  tmp["events"] = rc->nentries();
  for(int i = 0;i<14;++i){
    tmp["neg"][std::to_string(i)] = rc->neg()[i];
    tmp["pos"][std::to_string(i)] = rc->pos()[i];
    for(int j = 0;j<16;++j){
      tmp["arr"][std::to_string(i)][std::to_string(j)] = rc->arr()[i][j];
    }
  }
  std::string jsonname = "shuo_analysis/calorimeter_gain_analysis/cal_gain_json/shms_cal_gain_"+std::to_string(RunNumber)+".json"; 
  std::ofstream o2(jsonname.c_str());
//  o2<<std::setw(4)<<tmp<<std::endl;
  o2<<tmp.dump(4)<<std::endl;  
//all
}
