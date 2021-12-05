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
#include "TLegend.h"

double Get_Wprime_line(double *x, double *par)
{
  float xx = x[0];
  double Wprime2 = 2.6;
  double f = (Wprime2-0.938272*0.938272-0.139*0.139)*0.938272*xx/(0.98272*(1-xx)-0.139*(par[0]+1));
  return f;
}

int plot_kin_Q2x_pos(){
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xq2.json");
    runs>>j_Q2x;
  }
  json j_allcuts;
  {
    std::ifstream allcuts("db2/all_cut.json");
    allcuts>>j_allcuts;
  }
  int bins = j_allcuts["bins"].get<int>();
  //int coolcolor[14] = {46,47,40,48,30,49,31,41,32,33,43,44,45};
  int coolcolor[14] = {1,2,3,4,6,7,8,9,30,42,35,28,38,46};
  TCanvas* c_kin_pos = new TCanvas("","CSV",1900,1000);
  TLegend* legend = new TLegend(0.15,0.5,0.35,0.95);
  legend->SetHeader("CSV kinematic settings","C");
  legend->AddEntry((TObject*)0,"  x,   Q2","");
  int i_color = 0;
  TH2F* h_pos_all_xQ2 = new TH2F("","",bins,0,1,bins,0,10);
  for(auto it = j_Q2x.begin();it!=j_Q2x.end();it++){
    double x = std::stod(it.key());
    std::string x_2 = it.key().substr(0,4);
    auto runjs = it.value();
    for (auto ik = runjs.begin();ik!=runjs.end();++ik){
      double Q2 = std::stod(ik.key());
      std::string Q2_2 = ik.key().substr(0,4);
      std::cout<<"x_Q2_"<<x<<"_"<<Q2<<std::endl;
      std::string x_Q2 = x_2+","+Q2_2;
      if(x!= 0 && Q2!=0){
        TH2F* h_pos_all = new TH2F("",x_Q2.c_str(),bins,0,1,bins,0,10);
        h_pos_all->GetXaxis()->SetTitle("x");
        h_pos_all->GetYaxis()->SetTitle("Q2 (GeV^2)");
        auto kinjs = ik.value();
        std::vector<int> neg_D2,pos_D2;
        neg_D2 = kinjs["neg"]["D2"].get<std::vector<int>>();
        pos_D2 = kinjs["pos"]["D2"].get<std::vector<int>>();
        if(!neg_D2.empty() && !pos_D2.empty()){
          for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
            int RunNumber = *i_pos;
            //std::cout<<"pos "<<RunNumber;
            std::string rootfile_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
            TFile *rootfile = new TFile(rootfile_name.c_str(),"RECTEATE");
            TH2F* h_pos = new TH2F("","",100,0,1,100,0,1);
            if(rootfile->Get("Q2_x")){
              h_pos = (TH2F*)rootfile->Get("Q2_x");
              //std::cout<<" counts "<<h_pos->Integral()<<", ";
              //c_kin->cd();
              //h_pos->SetMarkerStyle(8);
              //h_pos->SetMarkerColorAlpha(coolcolor[i_color],0.75);
              //h_pos->GetXaxis()->SetRangeUser(0.2,0.8);
              //h_pos->GetYaxis()->SetRangeUser(0.2,0.8);
              //h_pos->Draw("p same");
              h_pos_all->Add(h_pos);

            }
            else{std::cout<<"No x_Q2 histo for "<<RunNumber<<std::endl;}
          }//loop over pos runs
          h_pos_all_xQ2->Add(h_pos_all);
          c_kin_pos->cd();
          //h_pos_all->SetMarkerStyle(8);
          //h_pos_all->SetMarkerColorAlpha(coolcolor[i_color],0.75);
          //h_pos_all->GetXaxis()->SetRangeUser(0.2,0.8);
          //h_pos_all->GetYaxis()->SetRangeUser(2,8);
          h_pos_all->SetBit(TH1::kNoStats);
          h_pos_all->SetFillColorAlpha(coolcolor[i_color],0.35);
          h_pos_all->GetXaxis()->SetRangeUser(0.2,0.8);
          h_pos_all->GetYaxis()->SetRangeUser(2,8);
          //TGraph *g_pos_all = new TGraph(h_pos_all);
          //g_pos_all->Draw("p same");
          h_pos_all->Draw("box same");
          
          legend->AddEntry(h_pos_all,x_Q2.c_str(),"f"); 
          i_color++;
        }//if normal production runs
      }//if x Q2 not 0
    }// loop over Q2
  }//loop over x
  gStyle->SetOptTitle(0);
  
  TF1* W2_1 = new TF1("W2","(4-0.938272*0.938272)*(x/(1-x))",0.2,0.8);
  W2_1->Draw("L same");
  TF1* Wprime2_1 = new TF1("Wp2:2.6,z:0.4",Get_Wprime_line,0.2,0.8,1);
  Wprime2_1->SetParameter(0,0.4);
  Wprime2_1->SetLineColor(kBlue);
  Wprime2_1->SetTitle("Wpi^2,z:0.4");
  Wprime2_1->Draw("L same");
  TF1* Wprime2_2 = new TF1("Wp2:2.6,z:0.8",Get_Wprime_line,0.2,0.8,1);
  Wprime2_2->SetParameter(0,0.8);
  Wprime2_2->SetLineColor(8);
  Wprime2_2->SetTitle("Wpi^2,z:0.8");
  Wprime2_2->Draw("L same");
  legend->AddEntry(W2_1,"W2","l");
  legend->AddEntry(Wprime2_1,"","l");
  legend->AddEntry(Wprime2_2,"","l");

  TLine* Q2_1 = new TLine(0.2,4,0.8,4);
  TLine* Q2_2 = new TLine(0.2,4.5,0.8,4.5);
  Q2_1->Draw("same");
  Q2_2->Draw("same");
  int bin_Q2_low = h_pos_all_xQ2->GetYaxis()->FindBin(4);
  int bin_Q2_high = h_pos_all_xQ2->GetYaxis()->FindBin(4.5);
  h_pos_all_xQ2->GetYaxis()->SetRange(0,bin_Q2_low);
  double Q2mean_low = h_pos_all_xQ2->GetMean(2);
  h_pos_all_xQ2->GetYaxis()->SetRange(bin_Q2_low,bin_Q2_high);
  double Q2mean_middle = h_pos_all_xQ2->GetMean(2);
  h_pos_all_xQ2->GetYaxis()->SetRange(bin_Q2_high,bins+1);
  double Q2mean_high = h_pos_all_xQ2->GetMean(2);
  std::cout<<"Q2 mean low "<<Q2mean_low<<", middle "<<Q2mean_middle<<", high "<<Q2mean_high<<std::endl;
  std::string Q2_mean_str = "Q2mean : "+std::to_string(Q2mean_low)+","+std::to_string(Q2mean_middle)+","+std::to_string(Q2mean_high);
  legend->AddEntry((TObject*)0,Q2_mean_str.c_str(),"");
  legend->Draw(); 
  //  c_kin_pos->BuildLegend(0.15,0.5,0.35,0.95);
  c_kin_pos->SaveAs("results/yield/kin_x_Q2_pos.png");
  return 0;
}
