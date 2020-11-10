#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

int plot_Q2x_zall_ratio(){
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j_Q2x;
  }
  json j_info;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_info;
  }
  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();

    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5);
    std::string q2x_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5)+"_yieldratio";
    TCanvas *c_Q2x_ratio = new TCanvas("",q2x_name.c_str(),1900,1000);
    TH1D* h_neg_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
    TH1D* h_pos_q2x = new TH1D("",(q2x_name).c_str(),100,0,1);
    if(xbj !=0 && Q2!=0){  
    for(json::iterator it = j_z.begin();it!=j_z.end();++it){
        TH1D* h_z_neg_all = new TH1D("",(canvas_name).c_str(),100,0,1);
        h_z_neg_all->GetXaxis()->SetTitle("z");
        h_z_neg_all->GetYaxis()->SetTitle("counts");
        TH1D* h_z_pos_all = new TH1D("",(canvas_name).c_str(),100,0,1);
        h_z_pos_all->GetXaxis()->SetTitle("z");
        h_z_pos_all->GetYaxis()->SetTitle("counts");

        double z = std::stod(it.key());
        std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
        std::vector<int> neg_D2_runs, pos_D2_runs;
        auto runjs = it.value();
        if(xbj != 0 && Q2!=0 && z!=0){
          neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
          pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
          for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
            int RunNumber = *it;
            std::cout<<RunNumber<<std::endl;
            double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
            TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            TH1D *h_z_neg = new TH1D("","",100,0,1);
            h_z_neg = (TH1D*)root_file_neg->Get("z");
            h_z_neg_all->Add(h_z_neg,1/charge);
          }//loop over neg runs
          for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
            int RunNumber = *it;
            std::cout<<RunNumber<<std::endl;
            double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
            TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            TH1D *h_z_pos = new TH1D("","",100,0,1);
            h_z_pos = (TH1D*)root_file_pos->Get("z");
            h_z_pos_all->Add(h_z_pos,1/charge);
          }//loop over pos runs
          h_neg_q2x->Add(h_z_neg_all);
          h_pos_q2x->Add(h_z_pos_all);
        }//if not 0
      TCanvas *c_z = new TCanvas();
      h_z_neg->SumW2();
      h_z_neg->Divide(h_z_pos);
      //tobecontinued .......................
    }//loop over z
     
    c_Q2x_ratio->cd();
    h_neg_q2x->Sumw2();
    h_neg_q2x->SetLineColor(kBlack);
    h_pos_q2x->SetLineColor(kBlue);
    h_pos_q2x->DrawCopy("hist");
    h_neg_q2x->DrawCopy("hist same");
    //auto rp = new TRatioPlot(h_neg_q2x,h_pos_q2x);
    //c_Q2x_ratio->SetTicks(0,1);
    //rp->Draw();
    //rp->GetLowerRefGraph()->SetMinimum(0);
    //rp->GetLowerRefGraph()->SetMaximum(1);
    //c_Q2x_ratio->Update();
      std::string ratiopdfname = "results/yield/statistics/"+canvas_name+"_ratio.pdf";
    
    c_Q2x_ratio->SaveAs(ratiopdfname.c_str());
    }//if x,Q2 not 0
    }//loop over Q2
  }//loop over x
return 0;
}
