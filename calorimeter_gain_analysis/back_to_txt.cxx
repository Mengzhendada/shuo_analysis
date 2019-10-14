#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
using json = nlohmann::json;

void back_to_txt(){
  json j;
  {
  std::ifstream infile("results/cal_gain_json/shms_cal_gain_6068to6559.json");
  infile>>j;
  }
  std::ofstream out("results/cal_gain_json/shms_cal_gain_6068to6559.txt");
  out<<"pcal_neg_gain_cor = ";
  for(int i = 0;i<14;++i){
    out<<j["neg"][std::to_string(i)].get<double>()<<", ";
  }
  out<<std::endl;
  out<<"pcal_pos_gain_cor = ";
    for(int i = 0;i<14;++i){
      out<<j["pos"][std::to_string(i)].get<double>()<<", ";
    }
  out<<std::endl;
  out<<"pcal_arr_gain_cor = ";
  for(int i = 0;i<14;++i){

   // for( auto& key_value : j["arr"]["0"].items()) {
   //   std::cout << key_value.key() << " : " << key_value.value() << "\n";
   // }
    for(int ij = 0;ij<16;++ij){
     out<<j["arr"][std::to_string(i)][std::to_string(ij)].get<double>()<<", ";
    // std::cout<<j["arr"]["0"]["1"].dump()<<std::endl;
    }
    out<<std::endl;
  }
}
