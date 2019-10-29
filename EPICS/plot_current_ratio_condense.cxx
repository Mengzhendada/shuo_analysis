#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>

#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

void plot_current_ratio_condense(){
  std::vector<string> current_name;
  current_name = {"ecDI_B_Set_NMR","ecDI_Set_Current","ecQ1_Set_Current","ec_Q2_Set_Current","ecSDI_B_Set_NMR","ecSDI_Set_Current","ecSHB_Set_Current","ecSQ1_Set_Current","ecSQ2_Set_Current","ecSQ3_Set_Current"};
  std::vector<int> multiplier;
  int i = 0;
  for(auto it = current_name.begin();it!=current_name.end();++it){
    multiplier.push_back(i);
    ++i;
  }
  std::string filename = "results/currentplot/Epic_values.root";
  TFile root_file = new TFile(file_name.c_str());
  TCanvas *c = new TCanvas();
  for(auto it = current_name.begin();it!=current_name.end();++it){
  TGraphErrors* Graph_D2_average = new TGraphErrors();
  std::string name = *it+"_average";
  Graph_D2_average = (TGraphErrors*)root_file->Get(name.c_str());
  for(int i = 0;i<Graph_D2_average.GetN();++i){
    Graph_D2_average.GetY()[i] += multiplier;
  }
  Graph_D2_average->Draw("pl");
}
c->Update();
c->SaveAs("results/currentplot/Set_Values.pdf");
}
