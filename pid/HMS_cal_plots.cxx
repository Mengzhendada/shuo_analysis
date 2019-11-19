#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "TCanvas.h"
#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLine.h"

void HMS_cal_plots(){
  json j_info;
  {
    std::ifstream info("db2/run_info_group.json");
    info>>j_info;
  }
  json j_hms;
  {
    std::ifstream info("results/hms_cal.json");
    info>>j_hms;
  }
  TGraphErrors* g_mean = new TGraphErrors();
  g_mean->SetTitle("HMS E/p peak mean");
  g_mean->GetXaxis()->SetTitle("hms_p");
  g_mean->GetYaxis()->SetTitle("mean");
  TGraphErrors* g_sigma = new TGraphErrors();
  g_sigma->SetTitle("HMS E/p peak sigma");
  g_sigma->GetXaxis()->SetTitle("hms_p");
  g_sigma->GetYaxis()->SetTitle("sigma");
  int i=0;
  for(json::iterator it = j_info.begin();it!=j_info.end();++it){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;
    double hms_p = std::abs(it.value()["hms_p"].get<double>());
    double mean = j_hms[std::to_string(RunNumber).c_str()]["mean"].get<double>();
    double sigma = j_hms[std::to_string(RunNumber).c_str()]["sigma"].get<double>();

    g_mean->SetPoint(i,RunNumber,mean);
    g_sigma->SetPoint(i,RunNumber,sigma);
    //g_mean->SetPoint(i,hms_p,mean);
    //g_sigma->SetPoint(i,hms_p,sigma);
    ++i;
  }
  g_mean->SetMarkerStyle(8);
  g_sigma->SetMarkerStyle(8);
  TCanvas* c = new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  g_mean->Draw("ap");
  c->cd(2);
  g_sigma->Draw("ap");
  c->SaveAs("results/HMS_Cal_plot.pdf");

}
