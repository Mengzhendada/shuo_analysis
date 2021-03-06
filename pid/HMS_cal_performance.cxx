//check cal E/P and plot
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

void HMS_cal_performance(){
  json j_info;
  {
    std::ifstream info("db2/run_info_group.json");
    info>>j_info;
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
    auto runjs = it.value();
    std::string rootfile = "ROOTfiles/coin_replay_production_"+it.key()+"_"+it.key()+".root";
    ROOT::RDataFrame d("T",rootfile);
    auto h_hms_cal = d.Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm");
    h_hms_cal->Fit("gaus","O","",0.5,1.5);
    TF1 *fit = h_hms_cal->GetFunction("gaus");
    double mean = fit->GetParameter(1);
    double sigma = fit->GetParameter(2);
    g_mean->SetPoint(i,hms_p,mean);
    g_sigma->SetPoint(i,hms_p,sigma);
  }
  TCanvas* c = new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  g_mean->Draw("ap");
  c->cd(2);
  g_sigma->Draw("ap");
  c->SaveAs("results/HMS_Cal_performance.pdf");

}
