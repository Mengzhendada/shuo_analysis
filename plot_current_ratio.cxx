#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
using nlohmann::json;
#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)
#include <cstring>
#include "TAxis.h"
#include "TLegend.h"
#include "TGraphBentErrors.h"
#include <bits/stdc++.h>
#include <numeric>
#include "math.h"


  double weighted_average(std::vector<double> v1,std::vector<int> v2){
    double sum1,sum2;
    for(int i = 0; i<v1.size();i++){
      sum1 += v1[i]*v2[i];
      sum2 +=v2[i];
      std::cout<<"v1 "<<v1[i]<<" v2 "<<v2[i]<<std::endl;
    }
    //std::cout<<"in function "<<sum1/sum2<<std::endl;
    return sum1/sum2;
  }
  double find_most_common(std::vector<double> v1,std::vector<int> v2){
   // const int N = sizeof(v2)/sizeof(int); 
    int n = std::distance(v2.begin(), std::max_element(v2.begin(),v2.end()));
    return v1[n];
  }

void plot_current_ratio(){
  json j;
  {
    std::ifstream json_input_file("results/ratio_run_group.json");
    if(json_input_file.fail()){std::cout<<"Couldn't find group json file"<<std::endl;}
    json_input_file>>j;
  }
  std::cout<<"what's going on"<<std::endl;
  std::string string[55] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status","hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};

  //for (int i = 0; i < 45; ++i) {
  // char cstr = string[i];
    std::string filename = "results/currentplot/Epic_values.root";
    TFile* root = new TFile(filename.c_str(),"RECREATE");
  for(int i = 0;i<55;++i){
    std::string current = string[i];
    std::cout << current;
    // char cstr[current.size()+1];
    // strcpy(cstr,current.c_str());
    TGraphBentErrors* Graph_D2_average = new TGraphBentErrors();
    Graph_D2_average->SetTitle("average D2");
    TGraphErrors* Graph_D2_first_neg = new TGraphErrors();
    Graph_D2_first_neg->SetTitle("first neg D2");
    TGraphErrors* Graph_D2_first_pos = new TGraphErrors();
    Graph_D2_first_pos->SetTitle("first pos D2");
    TGraphErrors* Graph_D2_most_neg = new TGraphErrors();
    Graph_D2_most_neg->SetTitle("most neg D2");
    TGraphErrors* Graph_D2_most_pos = new TGraphErrors();
    Graph_D2_most_pos->SetTitle("most pos D2");
    TGraphErrors* Graph_D2_max_neg = new TGraphErrors();
    Graph_D2_max_neg->SetTitle("max neg D2");
    TGraphErrors* Graph_D2_max_pos = new TGraphErrors();
    Graph_D2_max_pos->SetTitle("max pos D2");
    TGraphErrors* Graph_D2_min_neg = new TGraphErrors();
    Graph_D2_min_neg->SetTitle("min neg D2");
    TGraphErrors* Graph_D2_min_pos = new TGraphErrors();
    Graph_D2_min_pos->SetTitle("min pos D2");
    TGraphBentErrors* Graph_H2_average = new TGraphBentErrors();
    Graph_H2_average->SetTitle("average H2");
    TGraphErrors* Graph_H2_first_neg = new TGraphErrors();
    Graph_H2_first_neg->SetTitle("first neg H2");
    TGraphErrors* Graph_H2_first_pos = new TGraphErrors();
    Graph_H2_first_pos->SetTitle("first pos H2");
    TGraphErrors* Graph_H2_most_neg = new TGraphErrors();
    Graph_H2_most_neg->SetTitle("most neg H2");
    TGraphErrors* Graph_H2_most_pos = new TGraphErrors();
    Graph_H2_most_pos->SetTitle("most pos H2");
    TGraphErrors* Graph_H2_max_neg = new TGraphErrors();
    Graph_H2_max_neg->SetTitle("max neg H2");
    TGraphErrors* Graph_H2_max_pos = new TGraphErrors();
    Graph_H2_max_pos->SetTitle("max pos H2");
    TGraphErrors* Graph_H2_min_neg = new TGraphErrors();
    Graph_H2_min_neg->SetTitle("min neg H2");
    TGraphErrors* Graph_H2_min_pos = new TGraphErrors();
    Graph_H2_min_pos->SetTitle("min pos H2");
    TGraphBentErrors* Graph_Dummy_average = new TGraphBentErrors();
    Graph_Dummy_average->SetTitle("average Dummy");
    TGraphErrors* Graph_Dummy_first_neg = new TGraphErrors();
    Graph_Dummy_first_neg->SetTitle("first neg Dummy");
    TGraphErrors* Graph_Dummy_first_pos = new TGraphErrors();
    Graph_Dummy_first_pos->SetTitle("first pos Dummy");
    TGraphErrors* Graph_Dummy_most_neg = new TGraphErrors();
    Graph_Dummy_most_neg->SetTitle("most neg Dummy");
    TGraphErrors* Graph_Dummy_most_pos = new TGraphErrors();
    Graph_Dummy_most_pos->SetTitle("most pos Dummy");
    TGraphErrors* Graph_Dummy_max_neg = new TGraphErrors();
    Graph_Dummy_max_neg->SetTitle("max neg Dummy");
    TGraphErrors* Graph_Dummy_max_pos = new TGraphErrors();
    Graph_Dummy_max_pos->SetTitle("max pos Dummy");
    TGraphErrors* Graph_Dummy_min_neg = new TGraphErrors();
    Graph_Dummy_min_neg->SetTitle("min neg Dummy");
    TGraphErrors* Graph_Dummy_min_pos = new TGraphErrors();
    Graph_Dummy_min_pos->SetTitle("min pos Dummy");
    std::cout<<"check1"<<std::endl;
    int ii_D2 = 0,ii_H2 = 0,ii_Dummy = 0;
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
      std::cout << it.key() << std::endl;
      auto             runjs    = it.value();
      int              kingroup = runjs["group_num"].get<int>();
      //For D2
      std::vector<int> runs_D2, runs_D2_pos;
      if (runjs["neg"]["D2"].get<std::vector<int>>().empty()) {
        continue;
      }
      if (runjs["pos"]["D2"].get<std::vector<int>>().empty()) {
        continue;
      }
      runs_D2     = runjs["neg"]["D2"].get<std::vector<int>>();//get neg shms momentum runs
      runs_D2_pos = runjs["pos"]["D2"].get<std::vector<int>>();//get pos shms momentum runs
      //get values for neg_shms_p runs
      std::vector<double> D2_average_neg,D2_first_neg,D2_most_neg,D2_max_neg,D2_min_neg,D2_stdder_neg;
      std::vector<int> weight_neg_D2;
      for (auto runnums = runs_D2.begin();runnums!=runs_D2.end();runnums++){
        int run     = *runnums;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file "<<file_name << std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_neg     = magnet_in[current]["average"].get<double>();
        double first_neg       = magnet_in[current]["first"].get<double>();
        double most_neg        = magnet_in[current]["most_common"].get<double>();
        double max_neg         = magnet_in[current]["maximum"].get<double>();
        double min_neg         = magnet_in[current]["minimum"].get<double>();
        double nentries_neg    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        D2_average_neg.push_back(average_neg);
        D2_first_neg.push_back(first_neg);
        D2_most_neg.push_back(most_neg);
        D2_max_neg.push_back(max_neg);
        D2_min_neg.push_back(min_neg);
        weight_neg_D2.push_back(nentries_neg);
        D2_stdder_neg.push_back(stdder);
      }
      //get values for pos_shms_p runs
      std::vector<double> D2_average_pos,D2_first_pos,D2_most_pos,D2_max_pos,D2_min_pos,D2_stdder_pos;
      std::vector<int> weight_pos_D2;
      for (auto runnums_pos = runs_D2_pos.begin();runnums_pos!=runs_D2_pos.end();runnums_pos++){
        int run     = *runnums_pos;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file " <<file_name<< std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_pos     = magnet_in[current]["average"].get<double>();
        double first_pos       = magnet_in[current]["first"].get<double>();
        double most_pos        = magnet_in[current]["most_common"].get<double>();
        double max_pos         = magnet_in[current]["maximum"].get<double>();
        double min_pos         = magnet_in[current]["minimum"].get<double>();
        double nentries_pos    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        D2_average_pos.push_back(average_pos);
        D2_first_pos.push_back(first_pos);
        D2_most_pos.push_back(most_pos);
        D2_max_pos.push_back(max_pos);
        D2_min_pos.push_back(min_pos);
        weight_pos_D2.push_back(nentries_pos);
        D2_stdder_pos.push_back(stdder);
      }
      //average weighted for all runs
      double D2_average_neg_value = weighted_average(D2_average_neg,weight_neg_D2);
      //double D2_first_neg_value = weighted_average(D2_first_neg,weight_neg_D2);
      double D2_first_neg_value = D2_first_neg[0];
      //double D2_most_neg_value = weighted_average(D2_most_neg,weight_neg_D2);
      double D2_most_neg_value = find_most_common(D2_most_neg,weight_neg_D2);
      //double D2_max_neg_value = weighted_average(D2_max_neg,weight_neg_D2);
      double D2_max_neg_value = *max_element(D2_max_neg.begin(),D2_max_neg.end());
      //double D2_min_neg_value = weighted_average(D2_min_neg,weight_neg_D2);
      double D2_min_neg_value = *min_element(D2_min_neg.begin(),D2_min_neg.end());
      double D2_stdder_neg_value = weighted_average(D2_stdder_neg,weight_neg_D2);
      //average weighted for all pos runs
      double D2_average_pos_value = weighted_average(D2_average_pos,weight_pos_D2);
      //double D2_first_pos_value = weighted_average(D2_first_pos,weight_pos_D2);
      double D2_first_pos_value = D2_first_pos[0];
      //double D2_most_pos_value = weighted_average(D2_most_pos,weight_pos_D2);
      double D2_most_pos_value = find_most_common(D2_most_pos,weight_pos_D2);
      //double D2_max_pos_value = weighted_average(D2_max_pos,weight_pos_D2);
      double D2_max_pos_value = *max_element(D2_max_pos.begin(),D2_max_pos.end());
      //double D2_min_pos_value = weighted_average(D2_min_pos,weight_pos_D2);
      double D2_min_pos_value = *min_element(D2_min_pos.begin(),D2_min_pos.end());
      double D2_stdder_pos_value = weighted_average(D2_stdder_pos,weight_pos_D2);
      std::cout<<" size "<<D2_average_neg.size()<<" D2_average_pos_value "<<D2_average_pos_value<<std::endl;
      double ratio_D2_average,ratio_D2_first_neg,ratio_D2_first_pos,ratio_D2_most_neg,ratio_D2_most_pos,ratio_D2_max_neg,ratio_D2_max_pos,ratio_D2_min_pos,ratio_D2_min_neg,ratio_D2_stdder;
      if (D2_average_pos_value != D2_average_neg_value) {
        std::cout << " D2 average negative value : " << D2_average_neg_value << std::endl;
        std::cout << " D2 average postivie value : " << D2_average_pos_value << std::endl;
      }
      //if (pos_first != first) {
      //  std::cout << " negative first value : " << first << std::endl;
      //  std::cout << " postivie first value : " << pos_first << std::endl;
      //}

      if (D2_average_pos_value != 0) {
        ratio_D2_average = D2_average_neg_value / D2_average_pos_value;
        ratio_D2_first_neg = D2_first_neg_value / D2_average_pos_value;
        ratio_D2_first_pos = D2_average_neg_value / D2_first_pos_value;
        ratio_D2_most_neg = D2_most_neg_value / D2_average_pos_value;
        ratio_D2_most_pos = D2_average_neg_value / D2_most_pos_value;
        ratio_D2_max_neg = D2_max_neg_value / D2_average_pos_value;
        ratio_D2_max_pos = D2_average_neg_value / D2_min_pos_value;
        ratio_D2_min_neg = D2_min_neg_value / D2_average_pos_value;
        ratio_D2_min_pos = D2_average_neg_value / D2_max_pos_value;
        ratio_D2_stdder = sqrt(pow(D2_average_neg_value/D2_average_pos_value,2)*(pow(D2_stdder_neg_value/D2_average_neg_value,2)+pow(D2_stdder_pos_value/D2_average_pos_value,2)));

        std::cout <<"ratio_D2_average "<< ratio_D2_average << " ";
        Graph_D2_average->SetPoint(ii_D2, kingroup+3, ratio_D2_average);
        Graph_D2_average->SetPointError(ii_D2,0,0,ratio_D2_stdder,ratio_D2_stdder);
        Graph_D2_first_neg->SetPoint(ii_D2, kingroup+3, ratio_D2_first_neg);
        Graph_D2_first_pos->SetPoint(ii_D2, kingroup+3, ratio_D2_first_pos);
        Graph_D2_most_neg->SetPoint(ii_D2, kingroup+3, ratio_D2_most_neg);
        Graph_D2_most_pos->SetPoint(ii_D2, kingroup+3, ratio_D2_most_pos);
        Graph_D2_max_neg->SetPoint(ii_D2, kingroup+3, ratio_D2_max_neg);
        Graph_D2_max_pos->SetPoint(ii_D2,kingroup+3,ratio_D2_max_pos);
        Graph_D2_min_neg->SetPoint(ii_D2,kingroup+3,ratio_D2_min_neg);
        Graph_D2_min_pos->SetPoint(ii_D2, kingroup+3, ratio_D2_min_pos);
        ++ii_D2;
      } else { }
      std::cout << ii_D2 << std::endl;
      
      //For H2
      std::vector<int> runs_H2, runs_H2_pos;
      if (runjs["neg"]["H2"].get<std::vector<int>>().empty()) {
        continue;
      }
      if (runjs["pos"]["H2"].get<std::vector<int>>().empty()) {
        continue;
      }
      runs_H2     = runjs["neg"]["H2"].get<std::vector<int>>();//get neg shms momentum runs
      runs_H2_pos = runjs["pos"]["H2"].get<std::vector<int>>();//get pos shms momentum runs
      //get values for neg_shms_p runs
      std::vector<double> H2_average_neg,H2_first_neg,H2_most_neg,H2_max_neg,H2_min_neg,H2_stdder_neg;
      std::vector<int> weight_neg_H2;
      for (auto runnums = runs_H2.begin();runnums!=runs_H2.end();runnums++){
        int run     = *runnums;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file "<<file_name << std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_neg     = magnet_in[current]["average"].get<double>();
        double first_neg       = magnet_in[current]["first"].get<double>();
        double most_neg        = magnet_in[current]["most_common"].get<double>();
        double max_neg         = magnet_in[current]["maximum"].get<double>();
        double min_neg         = magnet_in[current]["minimum"].get<double>();
        double nentries_neg    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        H2_average_neg.push_back(average_neg);
        H2_first_neg.push_back(first_neg);
        H2_most_neg.push_back(most_neg);
        H2_max_neg.push_back(max_neg);
        H2_min_neg.push_back(min_neg);
        weight_neg_H2.push_back(nentries_neg);
        H2_stdder_neg.push_back(stdder);
      }
      //get values for pos_shms_p runs
      std::vector<double> H2_average_pos,H2_first_pos,H2_most_pos,H2_max_pos,H2_min_pos,H2_stdder_pos;
      std::vector<int> weight_pos_H2;
      for (auto runnums_pos = runs_H2_pos.begin();runnums_pos!=runs_H2_pos.end();runnums_pos++){
        int run     = *runnums_pos;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file " <<file_name<< std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_pos     = magnet_in[current]["average"].get<double>();
        double first_pos       = magnet_in[current]["first"].get<double>();
        double most_pos        = magnet_in[current]["most_common"].get<double>();
        double max_pos         = magnet_in[current]["maximum"].get<double>();
        double min_pos         = magnet_in[current]["minimum"].get<double>();
        double nentries_pos    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        H2_average_pos.push_back(average_pos);
        H2_first_pos.push_back(first_pos);
        H2_most_pos.push_back(most_pos);
        H2_max_pos.push_back(max_pos);
        H2_min_pos.push_back(min_pos);
        weight_pos_H2.push_back(nentries_pos);
        H2_stdder_pos.push_back(stdder);
      }
      //average weighted for all runs
      double H2_average_neg_value = weighted_average(H2_average_neg,weight_neg_H2);
      //double H2_first_neg_value = weighted_average(H2_first_neg,weight_neg_H2);
      double H2_first_neg_value = H2_first_neg[0];
      //double H2_most_neg_value = weighted_average(H2_most_neg,weight_neg_H2);
      double H2_most_neg_value = find_most_common(H2_most_neg,weight_neg_H2);
      //double H2_max_neg_value = weighted_average(H2_max_neg,weight_neg_H2);
      double H2_max_neg_value = *max_element(H2_max_neg.begin(),H2_max_neg.end());
      //double H2_min_neg_value = weighted_average(H2_min_neg,weight_neg_H2);
      double H2_min_neg_value = *min_element(H2_min_neg.begin(),H2_min_neg.end());
      double H2_stdder_neg_value = weighted_average(H2_stdder_neg,weight_neg_H2);
      //average weighted for all pos runs
      double H2_average_pos_value = weighted_average(H2_average_pos,weight_pos_H2);
      //double H2_first_pos_value = weighted_average(H2_first_pos,weight_pos_H2);
      double H2_first_pos_value = H2_first_pos[0];
      //double H2_most_pos_value = weighted_average(H2_most_pos,weight_pos_H2);
      double H2_most_pos_value = find_most_common(H2_most_pos,weight_pos_H2);
      //double H2_max_pos_value = weighted_average(H2_max_pos,weight_pos_H2);
      double H2_max_pos_value = *max_element(H2_max_pos.begin(),H2_max_pos.end());
      //double H2_min_pos_value = weighted_average(H2_min_pos,weight_pos_H2);
      double H2_min_pos_value = *min_element(H2_min_pos.begin(),H2_min_pos.end());
      double H2_stdder_pos_value = weighted_average(H2_stdder_pos,weight_pos_H2);
      std::cout<<" size "<<H2_average_neg.size()<<" H2_average_pos_value "<<H2_average_pos_value<<std::endl;
      double ratio_H2_average,ratio_H2_first_neg,ratio_H2_first_pos,ratio_H2_most_neg,ratio_H2_most_pos,ratio_H2_max_neg,ratio_H2_max_pos,ratio_H2_min_pos,ratio_H2_min_neg,ratio_H2_stdder;
      if (H2_average_pos_value != H2_average_neg_value) {
        std::cout << " H2 average negative value : " << H2_average_neg_value << std::endl;
        std::cout << " H2 average postivie value : " << H2_average_pos_value << std::endl;
      }
      //if (pos_first != first) {
      //  std::cout << " negative first value : " << first << std::endl;
      //  std::cout << " postivie first value : " << pos_first << std::endl;
      //}

      if (H2_average_pos_value != 0) {
        ratio_H2_average = H2_average_neg_value / H2_average_pos_value;
        ratio_H2_first_neg = H2_first_neg_value / H2_average_pos_value;
        ratio_H2_first_pos = H2_average_neg_value / H2_first_pos_value;
        ratio_H2_most_neg = H2_most_neg_value / H2_average_pos_value;
        ratio_H2_most_pos = H2_average_neg_value / H2_most_pos_value;
        ratio_H2_max_neg = H2_max_neg_value / H2_average_pos_value;
        ratio_H2_max_pos = H2_average_neg_value / H2_min_pos_value;
        ratio_H2_min_neg = H2_min_neg_value / H2_average_pos_value;
        ratio_H2_min_pos = H2_average_neg_value / H2_max_pos_value;
        ratio_H2_stdder = sqrt(pow(H2_average_neg_value/H2_average_pos_value,2)*(pow(H2_stdder_neg_value/H2_average_neg_value,2)+pow(H2_stdder_pos_value/H2_average_pos_value,2)));

        std::cout <<"ratio_H2_average "<< ratio_H2_average << " ";
        Graph_H2_average->SetPoint(ii_H2, kingroup+2, ratio_H2_average);
        Graph_H2_average->SetPointError(ii_H2,0,0,ratio_H2_stdder,ratio_H2_stdder);
        Graph_H2_first_neg->SetPoint(ii_H2, kingroup+2, ratio_H2_first_neg);
        Graph_H2_first_pos->SetPoint(ii_H2, kingroup+2, ratio_H2_first_pos);
        Graph_H2_most_neg->SetPoint(ii_H2, kingroup+2, ratio_H2_most_neg);
        Graph_H2_most_pos->SetPoint(ii_H2, kingroup+2, ratio_H2_most_pos);
        Graph_H2_max_neg->SetPoint(ii_H2, kingroup+2, ratio_H2_max_neg);
        Graph_H2_max_pos->SetPoint(ii_H2,kingroup+2,ratio_H2_max_pos);
        Graph_H2_min_neg->SetPoint(ii_H2,kingroup+2,ratio_H2_min_neg);
        Graph_H2_min_pos->SetPoint(ii_H2, kingroup+2, ratio_H2_min_pos);
        ++ii_H2;
      } else {
      }
      std::cout << ii_H2 << std::endl;
      //For Dummy
      std::vector<int> runs_Dummy, runs_Dummy_pos;
      if (runjs["neg"]["Dummy"].get<std::vector<int>>().empty()) {
        continue;
      }
      if (runjs["pos"]["Dummy"].get<std::vector<int>>().empty()) {
        continue;
      }
      runs_Dummy     = runjs["neg"]["Dummy"].get<std::vector<int>>();//get neg shms momentum runs
      runs_Dummy_pos = runjs["pos"]["Dummy"].get<std::vector<int>>();//get pos shms momentum runs
      //get values for neg_shms_p runs
      std::vector<double> Dummy_average_neg,Dummy_first_neg,Dummy_most_neg,Dummy_max_neg,Dummy_min_neg,Dummy_stdder_neg;
      std::vector<int> weight_neg_Dummy;
      for (auto runnums = runs_Dummy.begin();runnums!=runs_Dummy.end();runnums++){
        int run     = *runnums;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file "<<file_name << std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_neg     = magnet_in[current]["average"].get<double>();
        double first_neg       = magnet_in[current]["first"].get<double>();
        double most_neg        = magnet_in[current]["most_common"].get<double>();
        double max_neg         = magnet_in[current]["maximum"].get<double>();
        double min_neg         = magnet_in[current]["minimum"].get<double>();
        double nentries_neg    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        Dummy_average_neg.push_back(average_neg);
        Dummy_first_neg.push_back(first_neg);
        Dummy_most_neg.push_back(most_neg);
        Dummy_max_neg.push_back(max_neg);
        Dummy_min_neg.push_back(min_neg);
        weight_neg_Dummy.push_back(nentries_neg);
        Dummy_stdder_neg.push_back(stdder);
      }
      //get values for pos_shms_p runs
      std::vector<double> Dummy_average_pos,Dummy_first_pos,Dummy_most_pos,Dummy_max_pos,Dummy_min_pos,Dummy_stdder_pos;
      std::vector<int> weight_pos_Dummy;
      for (auto runnums_pos = runs_Dummy_pos.begin();runnums_pos!=runs_Dummy_pos.end();runnums_pos++){
        int run     = *runnums_pos;
        std::string   file_name     = "Data/magnet_current_" + std::to_string(run) + ".json";
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file " <<file_name<< std::endl;
          }
        }
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        double average_pos     = magnet_in[current]["average"].get<double>();
        double first_pos       = magnet_in[current]["first"].get<double>();
        double most_pos        = magnet_in[current]["most_common"].get<double>();
        double max_pos         = magnet_in[current]["maximum"].get<double>();
        double min_pos         = magnet_in[current]["minimum"].get<double>();
        double nentries_pos    = magnet_in[current]["nentries"].get<int>();
        double stdder          = magnet_in[current]["standard_derivation"].get<double>();
        Dummy_average_pos.push_back(average_pos);
        Dummy_first_pos.push_back(first_pos);
        Dummy_most_pos.push_back(most_pos);
        Dummy_max_pos.push_back(max_pos);
        Dummy_min_pos.push_back(min_pos);
        weight_pos_Dummy.push_back(nentries_pos);
        Dummy_stdder_pos.push_back(stdder);
      }
      //average weighted for all runs
      double Dummy_average_neg_value = weighted_average(Dummy_average_neg,weight_neg_Dummy);
      //double Dummy_first_neg_value = weighted_average(Dummy_first_neg,weight_neg_Dummy);
      double Dummy_first_neg_value = Dummy_first_neg[0];
      //double Dummy_most_neg_value = weighted_average(Dummy_most_neg,weight_neg_Dummy);
      double Dummy_most_neg_value = find_most_common(Dummy_most_neg,weight_neg_Dummy);
      //double Dummy_max_neg_value = weighted_average(Dummy_max_neg,weight_neg_Dummy);
      double Dummy_max_neg_value = *max_element(Dummy_max_neg.begin(),Dummy_max_neg.end());
      //double Dummy_min_neg_value = weighted_average(Dummy_min_neg,weight_neg_Dummy);
      double Dummy_min_neg_value = *min_element(Dummy_min_neg.begin(),Dummy_min_neg.end());
      double Dummy_stdder_neg_value = weighted_average(Dummy_stdder_neg,weight_neg_Dummy);
      //average weighted for all pos runs
      double Dummy_average_pos_value = weighted_average(Dummy_average_pos,weight_pos_Dummy);
      //double Dummy_first_pos_value = weighted_average(Dummy_first_pos,weight_pos_Dummy);
      double Dummy_first_pos_value = Dummy_first_pos[0];
      //double Dummy_most_pos_value = weighted_average(Dummy_most_pos,weight_pos_Dummy);
      double Dummy_most_pos_value = find_most_common(Dummy_most_pos,weight_pos_Dummy);
      //double Dummy_max_pos_value = weighted_average(Dummy_max_pos,weight_pos_Dummy);
      double Dummy_max_pos_value = *max_element(Dummy_max_pos.begin(),Dummy_max_pos.end());
      //double Dummy_min_pos_value = weighted_average(Dummy_min_pos,weight_pos_Dummy);
      double Dummy_min_pos_value = *min_element(Dummy_min_pos.begin(),Dummy_min_pos. end());
      double Dummy_stdder_pos_value = weighted_average(Dummy_stdder_pos,weight_pos_Dummy);
      std::cout<<" size "<<Dummy_average_neg.size()<<" Dummy_average_pos_value "<<Dummy_average_pos_value<<std::endl;
      double ratio_Dummy_average,ratio_Dummy_first_neg,ratio_Dummy_first_pos,ratio_Dummy_most_neg,ratio_Dummy_most_pos,ratio_Dummy_max_neg,ratio_Dummy_max_pos,ratio_Dummy_min_pos,ratio_Dummy_min_neg,ratio_Dummy_stdder;
      if (Dummy_average_pos_value != Dummy_average_neg_value) {
        std::cout << " Dummy average negative value : " << Dummy_average_neg_value << std::endl;
        std::cout << " Dummy average postivie value : " << Dummy_average_pos_value << std::endl;
      }
      //if (pos_first != first) {
      //  std::cout << " negative first value : " << first << std::endl;
      //  std::cout << " postivie first value : " << pos_first << std::endl;
      //}

      if (Dummy_average_pos_value != 0) {
        ratio_Dummy_average = Dummy_average_neg_value / Dummy_average_pos_value;
        ratio_Dummy_first_neg = Dummy_first_neg_value / Dummy_average_pos_value;
        ratio_Dummy_first_pos = Dummy_average_neg_value / Dummy_first_pos_value;
        ratio_Dummy_most_neg = Dummy_most_neg_value / Dummy_average_pos_value;
        ratio_Dummy_most_pos = Dummy_average_neg_value / Dummy_most_pos_value;
        ratio_Dummy_max_neg = Dummy_max_neg_value / Dummy_average_pos_value;
        ratio_Dummy_max_pos = Dummy_average_neg_value / Dummy_min_pos_value;
        ratio_Dummy_min_neg = Dummy_min_neg_value / Dummy_average_pos_value;
        ratio_Dummy_min_pos = Dummy_average_neg_value / Dummy_max_pos_value;
        ratio_Dummy_stdder = sqrt(pow(Dummy_average_neg_value/Dummy_average_pos_value,2)*(pow(Dummy_stdder_neg_value/Dummy_average_neg_value,2)+pow(Dummy_stdder_pos_value/Dummy_average_pos_value,2)));

        std::cout <<"ratio_Dummy_average "<< ratio_Dummy_average << " ";
        Graph_Dummy_average->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_average);
        Graph_Dummy_average->SetPointError(ii_Dummy,0,0,ratio_Dummy_stdder,ratio_Dummy_stdder);
        Graph_Dummy_first_neg->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_first_neg);
        Graph_Dummy_first_pos->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_first_pos);
        Graph_Dummy_most_neg->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_most_neg);
        Graph_Dummy_most_pos->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_most_pos);
        Graph_Dummy_max_neg->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_max_neg);
        Graph_Dummy_max_pos->SetPoint(ii_Dummy,kingroup+5,ratio_Dummy_max_pos);
        Graph_Dummy_min_neg->SetPoint(ii_Dummy,kingroup+5,ratio_Dummy_min_neg);
        Graph_Dummy_min_pos->SetPoint(ii_Dummy, kingroup+5, ratio_Dummy_min_pos);
        ++ii_Dummy;
      } else {
      }
      std::cout << ii_Dummy << std::endl;
   
      //json for loop
    }
   // std::string filename = "results/currentplot/"+current+".root";
   // TFile* root = new TFile(filename.c_str(),"RECREATE");
    TCanvas* c = new TCanvas("c1",current.c_str(),1900,1000);
    //c->Divide(2, 2);
    //c->cd(1);
    auto mg = new TMultiGraph();
    Graph_D2_average->SetMarkerStyle(21);
    Graph_D2_average->SetMarkerSize(1.2);
    mg->Add(Graph_D2_average,"p");
          Graph_D2_first_neg->SetMarkerStyle(22);
          mg->Add(Graph_D2_first_neg,"p");
          Graph_D2_first_pos->SetMarkerStyle(23);
          mg->Add(Graph_D2_first_pos,"p");
          Graph_D2_most_neg->SetMarkerStyle(24);
          mg->Add(Graph_D2_most_neg,"p");
          Graph_D2_most_pos->SetMarkerStyle(25);
          mg->Add(Graph_D2_most_pos,"p");
          Graph_D2_max_neg->SetMarkerStyle(26);
          mg->Add(Graph_D2_max_neg,"p");
          Graph_D2_max_pos->SetMarkerStyle(27);
          mg->Add(Graph_D2_max_pos,"p");
          Graph_D2_min_neg->SetMarkerStyle(28);
          mg->Add(Graph_D2_min_neg,"p");
          Graph_D2_min_pos->SetMarkerStyle(29);
          mg->Add(Graph_D2_min_pos,"p");
    //Graph_H2_average->SetMarkerStyle(36);
    //Graph_H2_average->SetMarkerSize(1.2);
   
    //mg->Add(Graph_H2_average,"p");
    //Graph_Dummy_average->SetMarkerStyle(17);
    //Graph_Dummy_average->SetMarkerSize(1.2);
    //mg->Add(Graph_Dummy_average,"p");
    mg->Draw("a");
    mg->GetYaxis()->SetRangeUser(0.99,1.01);
    mg->SetTitle(current.c_str());
    mg->Write(current.c_str());
    auto legend = gPad->BuildLegend();
    legend->SetHeader(current.c_str(),"C");
    gPad->Update();
    gPad->Write(current.c_str());
    // char outname = "currentplot/"+current+".pdf";
    //c->SaveAs(fmt::format("results/plot_current_ratio/{}.pdf", current).c_str());
    //c->SaveAs(fmt::format("results/plot_current_ratio/{}.png", current).c_str());
    std::string pdfname = "results/currentplot/"+current+".pdf";
    std::string pngname = "results/currentplot/"+current+".png";
    c->SaveAs(pdfname.c_str());
    c->SaveAs(pngname.c_str());
    //root->Close();
    auto mg_all = new TMultiGraph();
    mg_all->Add(Graph_D2_average,"p");
          mg_all->Add(Graph_D2_first_neg,"p");
          mg_all->Add(Graph_D2_first_pos,"p");
          mg_all->Add(Graph_D2_most_neg,"p");
          mg_all->Add(Graph_D2_most_pos,"p");
          mg_all->Add(Graph_D2_max_neg,"p");
          mg_all->Add(Graph_D2_max_pos,"p");
          mg_all->Add(Graph_D2_min_neg,"p");
          mg_all->Add(Graph_D2_min_pos,"p");
    Graph_H2_average->SetMarkerStyle(36);
    Graph_H2_average->SetMarkerSize(1.2);
   
    mg_all->Add(Graph_H2_average,"p");
    Graph_Dummy_average->SetMarkerStyle(17);
    Graph_Dummy_average->SetMarkerSize(1.2);
    mg_all->Add(Graph_Dummy_average,"p");
    mg_all->Draw("a");
    mg_all->GetYaxis()->SetRangeUser(0.99,1.01);
    mg_all->SetTitle(current.c_str());
    std::string name = current+"_all";
    mg_all->Write(name.c_str());
    auto legend_2 = gPad->BuildLegend();
    legend_2->SetHeader(current.c_str(),"C");
    gPad->Update();
    gPad->Write(name.c_str());

  }
  root->Close();
}

