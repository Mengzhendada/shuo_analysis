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
  auto h_D2_neg = new TH2D("h1","D2_neg",110,0,550,100,5,80);
  auto h_D2_pos = new TH2D("h1","D2_pos",110,0,550,100,5,80);
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
        jout[(std::to_string(RunGroup)).c_str()]["neg"].push_back(current);
        //from json file runlist
        //double current = j_runinfo[(std::to_string(Run)).c_str()]["run_info"]["beam_current"].get<double>();
        h_D2_neg->Fill(RunGroup,current);
      }//loop over neg D2 runs
      for(auto it_pos = runs_D2_pos.begin();it_pos!=runs_D2_pos.end();++it_pos){
        int Run = *it_pos;
        std::cout<<Run<<std::endl; 
        //from rootfile scaler tree
        std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(Run)+"_"+std::to_string(Run)+".root";
        ROOT::RDataFrame d_scaler_pos("TSP",rootfile.c_str());
        auto h_current = d_scaler_pos.Histo1D({"current","",1000,5,100},"P.BCM4B.scalerCurrent");
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
        jout[(std::to_string(RunGroup)).c_str()]["pos"].push_back(current);
        //from json file runlist
        //double current = j_runinfo[(std::to_string(Run)).c_str()]["run_info"]["beam_current"].get<double>();
        h_D2_pos->Fill(RunGroup,current);
      }//loop over pos D2 runs
    }//if production rungroup
  }//loop over j_runlist
  TCanvas *c_D2 = new TCanvas();
  c_D2->SetGrid();
  h_D2_neg->SetBarWidth(0.2);
  h_D2_neg->SetBarOffset(-0.1);
  h_D2_neg->SetFillColor(kYellow);
  h_D2_neg->SetFillStyle(1001);
  h_D2_neg->Draw("candle3");
  h_D2_pos->SetBarWidth(0.2);
  h_D2_pos->SetBarOffset(0.1);
  h_D2_pos->SetFillColor(kGreen);
  h_D2_pos->SetLineColor(kRed);
  h_D2_pos->Draw("candle3 same");
  gPad->BuildLegend(0.78,0.695,0.980,0.935,"","f");
  std::string D2_name = "results/bcm/current_D2.pdf";
  c_D2->SaveAs(D2_name.c_str());
  
  std::ofstream outjson("results/bcm/current.json");
  outjson<<jout.dump(4)<<std::endl;
}//end of all
