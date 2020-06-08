#include <iostream>
#include "TH1.h"
#include "TLine.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "TFile.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void draw_hist(){
  std::vector<int> neg_D2_runs = {6550,6551};
  std::vector<int> pos_D2_runs = {6545,6548,6549};
  double z = 0.5;
  TH1D *h_z_neg_all = new TH1D("","",100,0,1);
  TH1D *h_z_pos_all = new TH1D("","",100,0,1);

  for(auto it = neg_D2_runs.begin();it!= neg_D2_runs.end();++it){
    int RunNumber = *it;
    TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
    TH1D *h_z_neg = new TH1D("","",100,0,1);
    h_z_neg = (TH1D*)root_file_neg->Get("z");
    h_z_neg_all->Add(h_z_neg);
  }
  int n_neg = neg_D2_runs.size();
  for(auto it = pos_D2_runs.begin();it!= pos_D2_runs.end();++it){
    int RunNumber = *it;
    TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
    TH1D *h_z_pos = new TH1D("","",100,0,1);
    h_z_pos = (TH1D*)root_file_pos->Get("z");
    h_z_pos_all->Add(h_z_pos);
  }
  double n_pos = 1./pos_D2_runs.size();
  std::cout<<n_pos<<std::endl;
  //h_z_neg_all->Scale(1/n_neg);
  h_z_pos_all->Scale(n_pos);
  TCanvas *c = new TCanvas();
  h_z_pos_all->SetLineStyle(2);
  h_z_pos_all->SetFillColorAlpha(2,0.35);
  h_z_pos_all->Draw();
  //h_z_neg_all->SetLineStyle(3);
  //h_z_neg_all->Draw("same");
  TLine *z_line = new TLine(z,0,z,1);
  z_line->Draw("same");
}
