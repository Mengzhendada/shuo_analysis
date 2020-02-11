#include <ROOT/RDataFrame.hxx>
#include <array>
#include <iostream>
#include <TH1.h>
#include <TF1.h>
#include "TGraph.h"
#include <TH2.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void find_current(int RunGroup =0){
  json j_runlist;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_runlist;
  }
  json j_runinfo;
  {
    std::ifstream ifs("db2/run_list_csv.json");
    ifs>>j_runinfo;
  }
  json jout;
 // if(RunGroup ==0){
 //   std::cout<<"Enter RunGroup Number(-1 to exit)";
 //   std::cin>>RunGroup;
 //   if(RunGroup <0)
 //     return;
 // }
  TGraph *g_D2_neg = new TGraph();
  TGraph *g_D2_pos = new TGraph();
  auto h_D2_neg = new TH2D("h1","D2_neg",55,0,550,100,5,80);
  int i_D2_neg=0,i_D2_pos=0;
  std::ofstream outfile("results/bcm/current_D2_neg.txt");
  std::ofstream out2("results/bcm/current_D2_neg_weird.txt");
  for(auto it = j_runlist.begin();it!=j_runlist.end();++it){
    std::vector<int> runs_D2_neg,runs_D2_pos;
    RunGroup = std::stoi(it.key());
    std::cout<<RunGroup<<std::endl;
    auto runjs = it.value();
    runs_D2_neg = runjs["neg"]["D2"].get<std::vector<int>>();
    runs_D2_pos = runjs["pos"]["D2"].get<std::vector<int>>();
    if(!runs_D2_neg.empty() && !runs_D2_pos.empty()){
      for(auto it_neg = runs_D2_neg.begin();it_neg!=runs_D2_neg.end();++it_neg){
        int Run = *it_neg;
        std::cout<<Run<<std::endl; 
        //from rootfile scaler tree
        std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(Run)+"_"+std::to_string(Run)+".root";
        ROOT::RDataFrame d_scaler_neg("TSP",rootfile.c_str());
        auto h_current = d_scaler_neg.Histo1D({"current","",1000,5,100},"P.BCM4B.scalerCurrent");
        //double current = h_current->GetBinCenter(h_current->GetMaximumBin());
        double current_high = h_current->GetBinCenter(h_current->GetMaximumBin())+3;
        double current_low = current_high-6;
        h_current->Fit("gaus","0","",current_low,current_high);

        TF1 *h_current_fit = h_current->GetFunction("gaus");
        double h_current_fit_mean = 0.0;
        double h_current_fit_sigm = 0.0;
        if(h_current_fit){
          h_current_fit->SetLineColor(3);
          h_current_fit_mean = h_current_fit->GetParameter(1);
          h_current_fit_sigm = h_current_fit->GetParameter(2);
        outfile<<Run<<" mean "<<h_current_fit_mean<<" max "<<current_high-3<<" sigma "<<h_current_fit_sigm<<std::endl;
        if(h_current_fit_mean>70 || h_current_fit_mean < 20){
          out2<<Run<<" mean "<<h_current_fit_mean<<" max "<<current_high<<std::endl;
        }
        TCanvas *c_current = new TCanvas();
        h_current->DrawCopy();
        h_current_fit->Draw("same");
        std::string current_name = "results/bcm/current_"+std::to_string(Run)+".pdf";
        c_current->SaveAs(current_name.c_str());
        }
        double current = h_current_fit_mean;
        jout[(std::to_string(RunGroup)).c_str()].push_back(current);
        //from json file runlist
        //double current = j_runinfo[(std::to_string(Run)).c_str()]["run_info"]["beam_current"].get<double>();
        //g_D2_neg->SetPoint(i_D2_neg,RunGroup,current);
        h_D2_neg->Fill(RunGroup,current);
        ++i_D2_neg;
      }//loop over neg D2 runs
    }//if production rungroup
  }//loop over j_runlist
  TCanvas *c_D2_neg = new TCanvas();
  //g_D2_neg->SetMarkerStyle(8);
  c_D2_neg->SetGrid();
  h_D2_neg->SetBarWidth(0.4);
  h_D2_neg->SetBarOffset(-0.1);
  h_D2_neg->SetFillColor(kYellow);
  h_D2_neg->SetFillStyle(1001);
  h_D2_neg->Draw("candle3");
  std::string D2_neg_name = "results/bcm/current_D2_neg.pdf";
  c_D2_neg->SaveAs(D2_neg_name.c_str());
  
  std::ofstream outjson("results/bcm/current.json");
  outjson<<jout.dump(4)<<std::endl;
}//end of all
