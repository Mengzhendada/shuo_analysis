#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <array>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "TCanvas.h"
#include "TGraphErrors.h"
using nlohmann::json;
#include <cstring>
#include <bits/stdc++.h>
#include <numeric>
using namespace std;


void plot_te(){
 json j;
 {
   std::ifstream json_file("kin_group.json");
   if(!json_file){
     cout << "the file can not be opened" << endl;
   }
    else{
    json_file>>j;
    }
 }
   
    TGraphErrors* pte = new TGraphErrors(); 
    int ii = 0;
     std::vector<int> run;
     run = j["phase3"]["good_run_list"].get<std::vector<int>>();
       for(std::vector<int>::iterator it = run.begin(); it != run.end(); ++it)
       {
         std::string file_name = "phase3/yield" + std::to_string(*it) + ".json";
         //std::cout << file_name << std::endl;
             json file_in;
             {
              std::ifstream infile(file_name);
              try {
                infile >> file_in;
              } catch (json::parse_error) {
              std::cout << "Failed to open magnet current file" << std::endl;
          }
       }
     
              std::string run = std::to_string(*it);
             
	      //std::cout << std::setprecision(2) << value<< '\n';
//              value = RoundToSignificantDigits(value, 4);
              double current = file_in[run.c_str()]["set current"].get<double>();
              double s1xrate = file_in[run.c_str()]["SHMS 3/4 rates"].get<double>();
              double te = file_in[run.c_str()]["tracking eff"].get<double>();
              double te_error = file_in[run.c_str()]["tracking eff error"].get<double>();
              pte->SetPoint(ii, s1xrate, te);
              pte->SetPointError(ii, 0 , te_error);
              ++ii;
              
             cout<<"run:"<<" "<<*it<<" "<<"current:"<<current<<" "<<"tracking:"<<te<<" "<<"3/4 rates:"<<s1xrate<<"te_error: "<<te_error<<endl;
}

      TCanvas* c4 = new TCanvas("c4","c4",1000,1000);
      pte->SetTitle("tracking efficiencies vs 3/4 rates - phase3");
      pte->SetMarkerStyle(kFullCircle);
      pte->SetMarkerSize(1);
      pte->SetMarkerColor(kMagenta);
      pte->GetYaxis()->SetTitle("tracking efficiency");
      pte->GetXaxis()->SetTitle("3/4 rates (kHz)");
      pte->Draw("ap");
      gPad->Update();
      std::string name3 = "phase3_te.pdf";
      c4->SaveAs(name3.c_str());
  }
