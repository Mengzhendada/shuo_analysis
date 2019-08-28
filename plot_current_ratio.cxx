#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
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
      std::ifstream json_input_file("classify_runs.json");
      json_input_file>>j;
    }
    std::string string[45] = {"ecDI_B_Set_NMR","ecDI_B_True_NMR","ecDI_I_True","ecDI_I_coarse","ecDI_Set_Current","ecHMS_Angle","ecP_HMS","ecQ1_I_True","ecQ1_I_coarse","ecQ1_Set_Current","ecQ2_I_True","ecQ2_I_coarse","ecQ2_Set_Current","ecQ3_I_True","ecQ3_I_coarse","ecQ3_Set_Current","ecSDI_B_Set_NMR","ecSDI_B_True_NMR","ecSDI_HP","ecSDI_I_True","ecSDI_I_coarse","ecSDI_Set_Current","ecSHB_HP","ecSHB_HallP","ecSHB_I_True","ecSHB_I_coarse","ecSHB_Set_Current","ecSHMS_Angle","ecSP_SHMS","ecSQ1_HP","ecSQ1_HallP","ecSQ1_I_True","ecSQ1_I_coarse","ecSQ1_Set_Current","ecSQ2_HP","ecSQ2_HallP","ecSQ2_I_True","ecSQ2_I_coarse","ecSQ2_Set_Current","ecSQ3_HP","ecSQ3_HallP","ecSQ3_I_True","ecSQ3_I_coarse","ecSQ3_Set_Current","ecS_Shutter_status"};
    for(int i=0;i<45;++i){
      //char cstr = string[i];
      std::string current = string[i];
      std::cout<<current;
      char cstr[current.size()+1];
      strcpy(cstr,current.c_str());
      TGraphErrors *Graph_D2 = new TGraphErrors();
      Graph_D2->SetTitle(cstr);
      int ii=0;
      for(json::iterator it = j.begin();it!=j.end();++it){
        std::cout<<it.key();
        auto runjs = it.value();
        int kingroup = std::stoi(it.key());
        std::vector<int> runs_D2,runs_pos_D2;
        if(runjs["neg"]["D2"].get<std::vector<int>>().empty()){continue;}
        else{
          if(runjs["pos"]["D2"].get<std::vector<int>>().empty()){continue;}
          else{
            runs_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
            runs_pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
            int run = runs_D2[0];
            int run_pos = runs_pos_D2[0];
            std::cout<<" "<<run<<" "<<run_pos;
            std::string file_name = "Data/magnet_current_"+std::to_string(run)+".json";
            std::string pos_file_name = "Data/magnet_current_"+std::to_string(run)+".json";
            std::ifstream magnet_infile(file_name);
            if(magnet_infile.fail()){ std::cout<<"Failed to open magnet current file"<<std::endl;}
            std::ifstream magnet_pos_infile(pos_file_name);
            if(magnet_pos_infile.fail()){std::cout<<"Failed to open pos magnet current file"<<std::endl;}
            json magnet_in;
            json pos_magnet_in;
            magnet_infile>>magnet_in;
            magnet_pos_infile>>pos_magnet_in;
            double value=  magnet_in[current].get<double>();
            double pos_value = pos_magnet_in[current].get<double>();
            double ratio;
            std::cout<<" value "<<value<<std::endl;
            if(pos_value!=0){
              ratio = value/pos_value;
              std::cout<<ratio<<" ";
              Graph_D2->SetPoint(ii,kingroup,ratio);
              ++ii;
            }
            else{
              
              //Graph_D2->SetPoint(ii,kingroup,-1);
            }
            
            std::cout<<ii<<std::endl;
          }
        }
      }

      TCanvas *c = new TCanvas();
      c->Divide(2,2);
      c->cd(1);
      Graph_D2->SetMarkerStyle(21);
      Graph_D2->Draw();
      //char outname = "currentplot/"+current+".pdf";
      c->SaveAs(fmt::format("currentplot/{}.pdf",current).c_str());
    }
  }

