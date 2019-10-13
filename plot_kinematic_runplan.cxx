#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "TGraph.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
using json = nlohmann::json;

void plot_kinematic_runplan(){
json j;
{
  std::ifstream infile("db2/kin_group.json");
  infile>>j;
}
int num[13] = {46,47,40,48,30,49,31,41,32,42,33,43,44};
int i =0;
auto c = new TCanvas();
for(json::iterator it = j.begin();it!=j.end();++it){
  double x = std::stod(it.key());
  auto runjs = it.value();
  if(x>0){
    for(json::iterator ik = runjs.begin();ik!=runjs.end();++ik){
    double Q2 = std::stod(ik.key());
    std::string title = "x_Q2_"+std::to_string(x).substr(0,4)+"_"+std::to_string(Q2).substr(0,4); 
      TH2F *h_run_plan = new TH2F("CSV run plan","CSV run plan",400,0,1,400,0,10);
      //TH2F *h_run_plan = new TH2F("CSV run plan",title.c_str(),400,0,1,400,0,10);
h_run_plan->GetXaxis()->SetTitle("x");
h_run_plan->GetYaxis()->SetTitle("Q2");
    h_run_plan->Fill(x,Q2);
    h_run_plan->SetMarkerStyle(kFullDotLarge);
    h_run_plan->SetMarkerColorAlpha(num[i],1);
    h_run_plan->SetMarkerSize(1.1);
    h_run_plan->Draw("p same");
    ++i;
    }//Q2 loop
  }//if x > 0
  
}//x loop
c->BuildLegend(0.75,0.65,1,1,"csv run plan","p");
c->SetTitle("CSV Run Plan");
c->SaveAs("results/csv_kin/kin_plot/kinematics_runplan.pdf");
}
