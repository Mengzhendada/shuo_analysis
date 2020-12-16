#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "TH2.h"
#include "TFile.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TColor.h"
#include "TList.h"
#include "TCollection.h"

int plot_kin_Q2x(){
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xq2.json");
    runs>>j_Q2x;
  }
  //int coolcolor[14] = {46,47,40,48,30,49,31,41,32,33,43,44,45};
  int coolcolor[14] = {1,2,3,4,6,7,8,9,30,42,35,28,38,46};
  TCanvas* c_kin = new TCanvas("","",1900,1000);
  int i_color = 0;
  for(auto it = j_Q2x.begin();it!=j_Q2x.end();it++){
    double x = std::stod(it.key());
    std::string x_2 = it.key().substr(0,4);
    auto runjs = it.value();
    for (auto ik = runjs.begin();ik!=runjs.end();++ik){
      double Q2 = std::stod(ik.key());
      std::string Q2_2 = ik.key().substr(0,4);
      std::cout<<"x_Q2_"<<x<<"_"<<Q2<<std::endl;
      std::string x_Q2 = "x_Q2_"+x_2+"_"+Q2_2;
      if(x!= 0 && Q2!=0){
        TH2F* h_pos_all = new TH2F("",x_Q2.c_str(),100,0,1,100,0,10);
        h_pos_all->GetXaxis()->SetTitle("x");
        h_pos_all->GetYaxis()->SetTitle("Q2");
        TH2F* h_neg_all = new TH2F("",x_Q2.c_str(),100,0,1,100,0,10);
        h_neg_all->GetXaxis()->SetTitle("x");
        h_neg_all->GetYaxis()->SetTitle("Q2");
        auto kinjs = ik.value();
        std::vector<int> neg_D2,pos_D2;
        neg_D2 = kinjs["neg"]["D2"].get<std::vector<int>>();
        pos_D2 = kinjs["pos"]["D2"].get<std::vector<int>>();
        if(!neg_D2.empty() && !pos_D2.empty()){
          for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
            int RunNumber = *i_neg;
            std::cout<<RunNumber<<std::endl;
            std::string rootfile_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
            TFile *rootfile = new TFile(rootfile_name.c_str(),"RECTEATE");
            TH2F* h_neg = new TH2F("","",100,0,1,100,0,1);
            if(rootfile->Get("Q2_x")){
              h_neg = (TH2F*)rootfile->Get("Q2_x");
              std::cout<<"counts "<<h_neg->Integral()<<std::endl;
              //c_kin->cd();
              //h_neg->SetMarkerStyle(8);
              //h_neg->SetMarkerColorAlpha(coolcolor[i_color],0.75);
              //h_neg->GetXaxis()->SetRangeUser(0.2,0.8);
              //h_neg->GetYaxis()->SetRangeUser(0.2,0.8);
              //h_neg->Draw("p same");
              h_neg_all->Add(h_neg);

            }
            else{std::cout<<"No x_Q2 histo for "<<RunNumber<<std::endl;}
          }//loop over neg runs
          c_kin->cd();
          //h_neg_all->SetMarkerStyle(8);
          //h_neg_all->SetMarkerColorAlpha(coolcolor[i_color],0.75);
          //h_neg_all->GetXaxis()->SetRangeUser(0.2,0.8);
          //h_neg_all->GetYaxis()->SetRangeUser(2,8);
          h_neg_all->SetFillColor(coolcolor[i_color]);
          h_neg_all->GetXaxis()->SetRangeUser(0.2,0.8);
          h_neg_all->GetYaxis()->SetRangeUser(2,8);
          //TGraph *g_neg_all = new TGraph(h_neg_all);
          //g_neg_all->Draw("p same");
          h_neg_all->Draw("box same");
          i_color++;
        }//if normal production runs
      }//if x Q2 not 0
    }// loop over Q2
  }//loop over x
  c_kin->BuildLegend(0.8,0.5,1,1);
  c_kin->SaveAs("results/yield/kin_x_Q2.png");
  return 0;
}
