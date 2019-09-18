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
#include "TFile.h"
#include "TH1D.h"
#include "TH1.h"
#include "math.h"

double Average(std::vector<double> v)
{
  return std::accumulate(v.begin(),v.end(),0.0)/v.size();
}
double Standard_Derivation(std::vector<double> v, double mean){
  double stupid=0;
  for(int i = 0;i<v.size();i++){stupid+=(v[i]-mean)*(v[i]-mean);}
  return std::sqrt(stupid/v.size());
}
double Most_Common(std::vector<double> v){
  sort(v.begin(),v.end());
  auto most_value=*v.begin();
  auto test = *v.begin();
  int max_cnt = 0;
  int cnt =0;
  for(auto item : v){
    if(item == test){
      cnt++;
    }
    else{
      if(cnt>max_cnt){
        most_value = test;
        max_cnt = cnt;
      }
      test = item;
      cnt = 1;
    }
  }
  return most_value;
}

void grep_current_to_json(int RunNumber = 0){

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
 // string     RunFileNamePattern = "raw/coin_all_0"+std::to_string(RunNumber)+".dat";
  string     RunFileNamePattern = "values/strings_"+std::to_string(RunNumber)+".txt";
  std::cout<<RunFileNamePattern<<std::endl;
  //infile.ignore(50000,'cdipc_ypos');

  int ii=0;
  json j,j2;
  j = {{"RunNumber",RunNumber } };
  //  std::string filename = "Data/Epic_values_"+std::to_string(RunNumber)+".root";
  //TFile* root = new TFile(filename.c_str(),"RECREATE");
  std::string string[55] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status","hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};
  for(int i =0;i<55;i++){
    double current;
    std::vector<double> currents;
    std::ifstream infile;
    infile.open(RunFileNamePattern);
    std::string line,temp;
    int zeros=0;
    // TH1* h = new TH1D(string[i].c_str(),string[i].c_str(),1000,0,10);
    // h->SetCanExtend(TH1::kAllAxes);
    if(infile.fail()){std::cout<<"check file "<<RunFileNamePattern<<std::endl;}

    std::cout<<"check 1"<<string[i]<<std::endl;
    while(std::getline(infile,line)){
      if(line.find(string[i])<line.length())
      {
        if(line.find("not connected")<line.length()){zeros = zeros+1;}
        else{
          std::cout<<"check1 "<<string[i]<<" "<<ii<<std::endl;
          ++ii;
          for(int b = string[i].size();b<line[b];b++)
          {
            if(isdigit(line[b]))
            {
              for(int a = b;a<line.size();a++)
              {
                temp += line[a];
              }
              break;
            }
          }
          std::istringstream stream(temp);
          stream>>current;
          if(current ==0){zeros =zeros +1;}
          else{
            currents.push_back(current);
          }
          //        h->Fill(current);
          temp.clear();
        }//end if else "not connected"
      }//end if line find string
    }//end reading lines
    temp.clear();
    std::cout<<currents.size()<<std::endl; 
    double average,stdde;
    if(currents.size()!=0){
      //for root
          average = Average(currents);
          stdde = Standard_Derivation(currents,average);
      //for json
      
      double most_common = Most_Common(currents);
      auto max = *max_element(currents.begin(),currents.end());
      auto min = *min_element(currents.begin(),currents.end());
      j[string[i]]["nentries"]=currents.size();
      j[string[i]]["num_of_zeros"] = zeros;
      j[string[i]]["average"]=average;
      j[string[i]]["standard_derivation"]=stdde;
      j[string[i]]["most_common"]=most_common;
      j[string[i]]["maximum"]=max;
      j[string[i]]["minimum"]=min;
      j[string[i]]["first"]=currents[0];
    }
    else{
      j[string[i]]["nentries"]=currents.size();
      j[string[i]]["num_of_zeros"] = -1;
      j[string[i]]["average"]=0;
      j[string[i]]["standard_derivation"]=0;
      j[string[i]]["most_common"]=0;
      j[string[i]]["maximum"]=0;
      j[string[i]]["minimum"]=0;
      j[string[i]]["first"]=0;
    }
    //for root
    //double start = average-1.5*std::ceil(stdde);
    //double end = average+1.5*std::ceil(stdde);
    //TH1* h = new TH1D(string[i].c_str(),string[i].c_str(),200,start,end);
    //for(auto it = currents.begin();it!=currents.end();++it){
    //  h->Fill(*it);
    //}
    //h->Write();
  }//end string array
  //root->Close();
  std::string jsonname = fmt::format("Data/magnet_current_{}.json",RunNumber).c_str();
  std::ofstream o2(jsonname);
  //o2<<j<<std::endl;
  o2<<std::setw(4)<<j<<std::endl;
  //std::string rootfilename = "Data/Epic_values_"+std::to_string(RunNumber)+".root";
}
