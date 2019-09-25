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
  std::ifstream infile("db2/ratio_run_group_updated.json");
  infile>>j;
}
auto gr_fall = new TGraph();
auto gr_spring = new TGraph();
int i_fall = 0,i_spring = 0;
for(json::iterator it = j.begin();it!=j.end();++it){
  double x = it.value()["x"].get<double>();
  double Q2 = it.value()["Q2"].get<double>();
  std::cout<<"x "<<x<<" Q2 "<<Q2<<std::endl;
  int runnum = it.value()["kin_group"].get<int>();
  if(runnum<7000){
  gr_fall->SetPoint(i_fall,x,Q2);
  ++i_fall; 
  }
  else{
    gr_spring->SetPoint(i_spring,x,Q2);
    ++i_spring;
  }
  
}
auto c = new TCanvas();
gr_fall->SetMarkerStyle(21);
gr_fall->SetMarkerColorAlpha(2,0.5);
gr_fall->GetXaxis()->SetRangeUser(0.1,1);
gr_fall->GetXaxis()->SetTitle("x");
gr_fall->GetYaxis()->SetTitle("Q2");
gr_fall->Draw("ap");
gr_spring->SetMarkerStyle(21);
gr_spring->SetMarkerColorAlpha(3,0.5);
gr_spring->GetXaxis()->SetRangeUser(0.1,1);
gr_spring->GetXaxis()->SetTitle("x");
gr_spring->GetYaxis()->SetTitle("Q2");
gr_spring->Draw("p");
c->SaveAs("kinematics.pdf");
}
