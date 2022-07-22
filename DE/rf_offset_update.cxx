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
void rf_offset_update(){
  double offset_fall_high = 401.032;
  double offset_fall_low = 399.006;
  double offset_spring_high = 400.448;
  double offset_spring_low = 398.361;
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
