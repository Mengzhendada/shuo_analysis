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

  void plot_current_ratio(){
    json j;
    {
      std::ifstream json_input_file("result/ratio_run_group.json");
      if(json_input_file.fail()){std::cout<<"Couldn't find file"<<std::endl;}
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
      TGraphErrors* Graph_D2 = new TGraphErrors();
      Graph_D2->SetTitle(current.c_str());
      //TGraphErrors* Graph_D2_first = new TGraphErrors();
      //Graph_D2_first->SetTitle("First Value");
      std::cout<<"check1"<<std::endl;
      int ii = 0,ii_first = 0;
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << it.key() << std::endl;
        auto             runjs    = it.value();
        int              kingroup = runjs["kin_group"].get<int>();
        std::vector<int> runs_D2, runs_pos_D2;
        if (runjs["neg"]["D2"].get<std::vector<int>>().empty()) {
          continue;
        }
        if (runjs["pos"]["D2"].get<std::vector<int>>().empty()) {
          continue;
        }
        runs_D2     = runjs["neg"]["D2"].get<std::vector<int>>();
        runs_pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
        int run     = runs_D2[0];
        int run_pos = runs_pos_D2[0];
        //std::cout << " " << run << " " << run_pos << "\n";
        std::string   file_name     = "results/Data/magnet_current_" + std::to_string(run) + ".json";
        std::string   pos_file_name = "results/Data/magnet_current_" + std::to_string(run_pos) + ".json";
        std::cout<<"check"<<std::endl;
        json magnet_in;
        {
          std::ifstream magnet_infile(file_name);
          try {
            magnet_infile >> magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open magnet current file" << std::endl;
          }
        }
        json pos_magnet_in;
        {
          std::ifstream magnet_pos_infile(pos_file_name);
          try {
            magnet_pos_infile >> pos_magnet_in;
          } catch (json::parse_error) {
            std::cout << "Failed to open pos magnet current file" << std::endl;
          }
        }
        //magnet_infile >> magnet_in;
        //magnet_pos_infile >> pos_magnet_in;
        if (magnet_in.find(current) == magnet_in.end()) {
          std::cout << magnet_in.dump(2) << "\n";
          continue;
        }
        if (pos_magnet_in.find(current) == pos_magnet_in.end()) {
          std::cout << pos_magnet_in.dump(2) << "\n";
          continue;

        }
        double ratio;
        double value     = magnet_in[current]["average"].get<double>();
        double pos_value = pos_magnet_in[current]["average"].get<double>();
        //double error = magnet_
        //double first  = magnet_in[current]["first"].get<double>();
        //double pos_first = pos_magnet_in[current]["first"].get<double>();
        if (pos_value != value) {
          std::cout << " negative value : " << value << std::endl;
          std::cout << " postivie value : " << pos_value << std::endl;
        }
        //if (pos_first != first) {
        //  std::cout << " negative first value : " << first << std::endl;
        //  std::cout << " postivie first value : " << pos_first << std::endl;
        //}
        
        if (pos_value != 0) {
          ratio = value / pos_value;
          std::cout << ratio << " ";
          Graph_D2->SetPoint(ii, kingroup, ratio);
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

      TCanvas* c = new TCanvas("c1","c1",1900,1000);
      //c->Divide(2, 2);
      //c->cd(1);
      Graph_D2->SetMarkerStyle(21);
      Graph_D2->Draw("ap");
      Graph_D2->GetYaxis()->SetRangeUser(0.9,1.1);
      //Graph_D2_first->SetMarkerStyle(22);
      //Graph_D2_first->Draw("p");
      //Graph_D2_first->GetYaxis()->SetRangeUser(0.9,1.1);
      gPad->BuildLegend();
      gPad->Update();
      // char outname = "currentplot/"+current+".pdf";
      c->SaveAs(fmt::format("results/plot_current_ratio/{}.pdf", current).c_str());
      c->SaveAs(fmt::format("results/plot_current_ratio/{}.png", current).c_str());
    }
  }

