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
#include<bits/stdc++.h>
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
#include "TLine.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"
double Get_pi_eff(double shms_p){
  return 0.9824+0.002969*shms_p;
}
double Get_pos_pi_purity(double shms_p){
  if(shms_p>3){
  return 1.978-0.4815*shms_p+0.05161*shms_p*shms_p;
  }
  else{
    return 1;
  }
}
double Get_neg_pi_purity(double shms_p){
  if(shms_p>3){
  return 1.478-0.2423*shms_p+0.0687*shms_p*shms_p;
  }
  else{
    return 1;
  }
}
void write_rf_to_json(){
  json jout;
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }

  json j_PID;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_PID;
  }
  std::vector<double> delta_cuts = j_PID["SHMS"]["delta_cuts_forrf"].get<std::vector<double>>();
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!pos_D2.empty() && !neg_D2.empty()){
      double shms_p_central = it.value()["shms_p"].get<double>();
      int i_dp = 0;
      for(auto ik = delta_cuts.begin();ik!=delta_cuts.end()-1;++ik){    
        double rf_right = 1.54;
        double rf_left = 2-1.54;
        double delta_low = *ik;
        double shms_p = shms_p_central*(100+delta_low)/100;
  
        std::cout<<"Rungroup"<<RunGroup<<"delta"<<delta_low<<", momentum "<<shms_p<<std::endl;
          std::cout<<Get_pi_eff(shms_p)<<std::endl;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff"] = Get_pi_eff(shms_p);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff"] = Get_pi_eff(shms_p);
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_purity"] = 1;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_purity"] = 1;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_high"] = rf_right;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_low"] = rf_left;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_high"] = rf_right;
          jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_low"] = rf_left;
        i_dp++;
      }//loop over which dp
    }//if not empty
  }//loop over rungroup
  //std::string out_name = "rf_eff.json";
  std::string out_name = "shuo_analysis/dbase/rf_eff.json";
  std::ofstream ofs(out_name.c_str());
  ofs<<jout.dump(4)<<std::endl;
}
