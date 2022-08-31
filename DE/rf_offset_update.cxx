#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"
#include "ROOT/RSnapshotOptions.hxx"
//update D2 runs rf offset. all runs has same high/low offset
double t_pi(double p) {
  const double m = 0.139;
  const double shms_length = 20.1;
  const double c           = 299792458;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
void rf_offset_update(){
  double offset_fall_high = 401.016;
  double offset_fall_low = 399.012;
  double offset_spring_high = 400.268;
  double offset_spring_low = 398.264;
  //double shms_p_mean = 2.6;//I assumed, need to be verified
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    double shms_p = it.value()["shms_p"].get<double>();
    //double rfoffset_shift = -(t_pi(shms_p)-t_pi(shms_p_mean));
    //std::cout<<" 1 "<<t_pi(shms_p)<<std::endl;
    //std::cout<<"shift "<<rfoffset_shift<<std::endl;
    //offset_fall_high = 401.016 + rfoffset_shift;
    //offset_fall_low = 399.012 + rfoffset_shift;
    //offset_spring_high = 400.436+rfoffset_shift;
    //offset_spring_low = 398.432+ rfoffset_shift;
    //offset_spring_high = 400.016+rfoffset_shift;
    //offset_spring_low = 398.012+ rfoffset_shift;
    std::cout<<offset_fall_high<<" "<<offset_fall_low<<" "<<offset_spring_high<<" "<<offset_spring_low<<std::endl;
    std::cout<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    
    if(!neg_D2.empty() && !pos_D2.empty()){
      for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
        int RunNumber = *i_neg;
        if(RunNumber<7000){
          double rfoffset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
          if(rfoffset>400){
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_fall_high;
            //double offset_fall_high = 401.032;
          }
          else{
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_fall_low;
          }
        }//if fall runs
        else{
          double rfoffset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
          if(rfoffset>400){
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_spring_high;
            //double offset_spring_high = 401.032;
          }
          else{
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_spring_low;
          }
        }//if spring runs
      }//loop over neg D2 runs
      for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
        int RunNumber = *i_pos;
        if(RunNumber<7000){
          double rfoffset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
          if(rfoffset>400){
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_fall_high;
            //double offset_fall_high = 401.032;
          }
          else{
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_fall_low;
          }
        }//if fall runs
        else{
          double rfoffset = j_info[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
          if(rfoffset>400){
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_spring_high;
            //double offset_spring_high = 401.032;
          }
          else{
            j_info[(std::to_string(RunNumber)).c_str()]["offset"] = offset_spring_low;
          }
        }//if spring runs
      }//loop over pos D2 runs
    }//if not empty
  }//loop over rungroups
  std::ofstream ofs("results/runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
