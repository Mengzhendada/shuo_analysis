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
  void plot_current_ratio(){
    json j;
    {
      std::ifstream json_input_file("results/ratio_run_group.json");
      if(json_input_file.fail()){std::cout<<"Couldn't find group json file"<<std::endl;}
      json_input_file>>j;
    }
    std::cout<<"what's going on"<<std::endl;
    //json monitor_values =
   // std::string string[45] = {
   //     "ecDI_B_Set_NMR",    "ecDI_B_True_NMR",   "ecDI_I_True",       "ecDI_I_coarse",
   //     "ecDI_Set_Current",  "ecHMS_Angle",       "ecP_HMS",           "ecQ1_I_True",
   //     "ecQ1_I_coarse",     "ecQ1_Set_Current",  "ecQ2_I_True",       "ecQ2_I_coarse",
   //     "ecQ2_Set_Current",  "ecQ3_I_True",       "ecQ3_I_coarse",     "ecQ3_Set_Current",
   //     "ecSDI_B_Set_NMR",   "ecSDI_B_True_NMR",  "ecSDI_HP",          "ecSDI_I_True",
   //     "ecSDI_I_coarse",    "ecSDI_Set_Current", "ecSHB_HP",          "ecSHB_HallP",
   //     "ecSHB_I_True",      "ecSHB_I_coarse",    "ecSHB_Set_Current", "ecSHMS_Angle",
   //     "ecSP_SHMS",         "ecSQ1_HP",          "ecSQ1_HallP",       "ecSQ1_I_True",
   //     "ecSQ1_I_coarse",    "ecSQ1_Set_Current", "ecSQ2_HP",          "ecSQ2_HallP",
   //     "ecSQ2_I_True",      "ecSQ2_I_coarse",    "ecSQ2_Set_Current", "ecSQ3_HP",
   //     "ecSQ3_HallP",       "ecSQ3_I_True",      "ecSQ3_I_coarse",    "ecSQ3_Set_Current",
   //     "ecS_Shutter_status"};
  std::string string[55] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status","hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};

    //for (int i = 0; i < 45; ++i) {
      // char cstr = string[i];
      for(int i = 0;i<55;++i){
      std::string current = string[i];
      std::cout << current;
      // char cstr[current.size()+1];
      // strcpy(cstr,current.c_str());
      TGraphErrors* Graph_D2_average = new TGraphErrors();
      Graph_D2_average->SetTitle("average");
      TGraphErrors* Graph_D2_first = new TGraphErrors();
      Graph_D2_first->SetTitle("first");
      TGraphErrors* Graph_D2_most = new TGraphErrors();
      Graph_D2_most->SetTitle("most");
      TGraphErrors* Graph_D2_max = new TGraphErrors();
      Graph_D2_max->SetTitle("max");
      TGraphErrors* Graph_D2_min = new TGraphErrors();
      Graph_D2_min->SetTitle("min");
      std::cout<<"check1"<<std::endl;
      int ii = 0,ii_first = 0;
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << it.key() << std::endl;
        auto             runjs    = it.value();
        int              kingroup = runjs["kin_group"].get<int>();
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
        std::vector<double> D2_average_neg,D2_first_neg,D2_most_neg,D2_max_neg,D2_min_neg;
        std::vector<int> weight_neg;
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

          D2_average_neg.push_back(average_neg);
          D2_first_neg.push_back(first_neg);
          D2_most_neg.push_back(most_neg);
          D2_max_neg.push_back(max_neg);
          D2_min_neg.push_back(min_neg);
          weight_neg.push_back(nentries_neg);
        }
        //get values for pos_shms_p runs
        std::vector<double> D2_average_pos,D2_first_pos,D2_most_pos,D2_max_pos,D2_min_pos;
        std::vector<int> weight_pos;
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
          
          D2_average_pos.push_back(average_pos);
          D2_first_pos.push_back(first_pos);
          D2_most_pos.push_back(most_pos);
          D2_max_pos.push_back(max_pos);
          D2_min_pos.push_back(min_pos);
          weight_pos.push_back(nentries_pos);
        }
        //average weighted for all runs
        double D2_average_neg_value = weighted_average(D2_average_neg,weight_neg);
        double D2_first_neg_value = weighted_average(D2_first_neg,weight_neg);
        double D2_most_neg_value = weighted_average(D2_most_neg,weight_neg);
        double D2_max_neg_value = weighted_average(D2_max_neg,weight_neg);
        double D2_min_neg_value = weighted_average(D2_min_neg,weight_neg);
        //average weighted for all pos runs
        double D2_average_pos_value = weighted_average(D2_average_pos,weight_pos);
        double D2_first_pos_value = weighted_average(D2_first_pos,weight_pos);
        double D2_most_pos_value = weighted_average(D2_most_pos,weight_pos);
        double D2_max_pos_value = weighted_average(D2_max_pos,weight_pos);
        double D2_min_pos_value = weighted_average(D2_min_pos,weight_pos);
        std::cout<<" size "<<D2_average_neg.size()<<" D2_average_pos_value "<<D2_average_pos_value<<std::endl;
        double ratio_average,ratio_first,ratio_most,ratio_max,ratio_min;
        if (D2_average_pos_value != D2_average_neg_value) {
          std::cout << " D2 average negative value : " << D2_average_neg_value << std::endl;
          std::cout << " D2 average postivie value : " << D2_average_pos_value << std::endl;
        }
        //if (pos_first != first) {
        //  std::cout << " negative first value : " << first << std::endl;
        //  std::cout << " postivie first value : " << pos_first << std::endl;
        //}
        
        if (D2_average_pos_value != 0) {
          ratio_average = D2_average_neg_value / D2_average_pos_value;
          ratio_first = D2_first_neg_value / D2_first_pos_value;
          ratio_most = D2_most_neg_value / D2_most_pos_value;
          ratio_max = D2_max_neg_value / D2_max_pos_value;
          ratio_min = D2_min_neg_value / D2_min_pos_value;

          std::cout <<"ratio_average "<< ratio_average << " ";
          Graph_D2_average->SetPoint(ii, kingroup, ratio_average);
          Graph_D2_first->SetPoint(ii, kingroup, ratio_first);
          Graph_D2_most->SetPoint(ii, kingroup, ratio_most);
          Graph_D2_max->SetPoint(ii, kingroup, ratio_max);
          Graph_D2_min->SetPoint(ii, kingroup, ratio_min);
          ++ii;
        } else {

          // Graph_D2->SetPoint(ii,kingroup,-1);
        }
        //if (pos_first != 0) {
        //  ratio = first / pos_first;
        //  std::cout << ratio << " ";
        //  Graph_D2_first->SetPoint(ii_first, kingroup, ratio);
        //  ++ii_first;
        //}
        std::cout << ii << std::endl;
      }

      TCanvas* c = new TCanvas("c1",current.c_str(),1900,1000);
      //c->Divide(2, 2);
      //c->cd(1);
      auto mg = new TMultiGraph();
      Graph_D2_average->SetMarkerStyle(21);
      mg->Add(Graph_D2_average,"p");
      Graph_D2_first->SetMarkerStyle(22);
      mg->Add(Graph_D2_first,"p");
      Graph_D2_most->SetMarkerStyle(23);
      mg->Add(Graph_D2_most,"p");
      Graph_D2_max->SetMarkerStyle(24);
      mg->Add(Graph_D2_max,"p");
      Graph_D2_min->SetMarkerStyle(25);
      mg->Add(Graph_D2_min,"p");
      mg->Draw("a");
      mg->GetYaxis()->SetRangeUser(0.9,1.1);
      mg->SetTitle(current.c_str());
      gPad->BuildLegend();
      gPad->Update();
      // char outname = "currentplot/"+current+".pdf";
      //c->SaveAs(fmt::format("results/plot_current_ratio/{}.pdf", current).c_str());
      //c->SaveAs(fmt::format("results/plot_current_ratio/{}.png", current).c_str());
      std::string pdfname = "results/currentplot/"+current+".pdf";
      std::string pngname = "results/currentplot/"+current+".png";
      c->SaveAs(pdfname.c_str());
      c->SaveAs(pngname.c_str());
      }
  }

