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

void plot_kinematic_kin_group(){
  json j;
  {
    std::ifstream infile("db2/kin_group.json");
    infile>>j;
  }
  TFile *f_spring = new TFile("results/csv_kin/kinematics_spring.root");
  //f_spring->ls();
  //f_spring->GetName();
  //TH2D *h = (TH2D*)f_spring->Get("7593");
  TFile *f_fall = new TFile("results/csv_kin/kinematics_fall.root");  
  //TH2D *h1 = (TH2D*)f_fall->Get("6068");
  //f_fall->ls();
  int num[13] = {1,2,3,4,5,6,7,8,9,40,41,42,43};
  //  std::string color[13] = {}
  int i = 0;
  auto *c = new TCanvas("csv kinematics");
  c->Divide(2,1);
  //c->cd(1);
  //h->SetMarkerColorAlpha(2,0.35);
  //h->Draw("p");
  //h1->SetMarkerColorAlpha(3,0.35);
  //h1->Draw("ap same");
  TH2F *h_neg_kin_group = new TH2F("CSV pi-","CSV pi-",400,0,1,400,0,10);
  TH2F *h_pos_kin_group = new TH2F("CSV pi+","CSV pi+",400,0,1,400,0,10);
  for(json::iterator it = j.begin();it!=j.end();++it)
  {
    double x = std::stod(it.key());
    std::string x_2 = it.key().substr(0,4);
    auto runjs = it.value();
    if(x == 0){continue;}
    else
    {
      for(json::iterator ik = runjs.begin();ik!=runjs.end();++ik)
      {
        auto Q2 = std::stod(ik.key());
        std::string Q2_2 = ik.key().substr(0,4);
        std::string name_neg = "CSV_pi-_x_Q2_"+x_2+"_"+Q2_2;
        std::string name_pos = "CSV_pi+_x_Q2_"+x_2+"_"+Q2_2;
        TH2F *h_pos_all = new TH2F(name_neg.c_str(),name_neg.c_str(),400,0,1,400,0,10);
        h_pos_all->GetXaxis()->SetTitle("x");
        h_pos_all->GetYaxis()->SetTitle("Q2");
        TH2F *h_neg_all = new TH2F(name_pos.c_str(),name_pos.c_str(),400,0,1,400,0,10);
        h_neg_all->GetXaxis()->SetTitle("x");
        h_neg_all->GetYaxis()->SetTitle("Q2");
        // std::cout<<i<<std::endl;
        // ++i;
        auto kinjs = ik.value();
        std::vector v_D2_neg = kinjs["neg"]["D2"].get<std::vector<int>>();
        std::vector v_D2_pos = kinjs["pos"]["D2"].get<std::vector<int>>();
        if(!v_D2_neg.empty()){
          auto c_neg = new TCanvas(name_neg.c_str());
          c_neg->SetTitle(name_neg.c_str());
          for(int ii = 0;ii<v_D2_neg.size();++ii){
          TH2F *h_neg = new TH2F(name_neg.c_str(),name_neg.c_str(),400,0,1,400,0,10);
            int runnum = v_D2_neg[ii];
            if(runnum<7000){
              h_neg= (TH2F*)f_fall->Get(std::to_string(runnum).c_str());
              h_neg_all->Add(h_neg);
              std::cout<<runnum<<std::endl;
              h_neg->SetMarkerColorAlpha(num[i],0.35);
              h_neg->SetTitle(name_neg.c_str());
              h_neg->Draw("colz");
            }
            else{
              h_neg = (TH2F*)f_spring->Get(std::to_string(runnum).c_str());
              h_neg_all->Add(h_neg);
              std::cout<<runnum<<std::endl;
              h_neg->SetMarkerColorAlpha(num[i],0.35);
              h_neg->SetTitle(name_neg.c_str());
              h_neg->Draw("colz");
            }
          std::string filename = "results/csv_kin/kin_plot/"+std::to_string(runnum)+".png";
          c_neg->SetTitle(name_neg.c_str());
          c_neg->SaveAs(filename.c_str());
          
          }
          h_neg_all->SetMarkerColorAlpha(num[i],0.75);
          c->cd(1);
          h_neg_all->Draw("p same");
          h_neg_kin_group->Add(h_neg_all);
          std::string filename1 = "results/csv_kin/kin_plot/"+name_neg+".png";
          auto *c_neg_all = new TCanvas(filename1.c_str());
          c_neg_all->SetTitle(name_neg.c_str());
          h_neg_all->SetMarkerColorAlpha(num[i],1);
          h_neg_all->Draw("colz");
          c_neg_all->SaveAs(filename1.c_str());
        }
        if(!v_D2_pos.empty()){
          auto *c_pos = new TCanvas(name_pos.c_str());
          c_pos->SetTitle(name_pos.c_str());
          for(int ii = 0;ii<v_D2_pos.size();++ii){
          TH2F *h_pos = new TH2F(name_pos.c_str(),name_pos.c_str(),400,0,1,400,0,10);
            int runnum = v_D2_pos[ii];
            if(runnum<7000){
              h_pos= (TH2F*)f_fall->Get(std::to_string(runnum).c_str());
              h_pos_all->Add(h_pos);
              std::cout<<runnum<<std::endl;
              h_pos->SetMarkerColorAlpha(num[i],0.35);
              h_pos->SetTitle(name_pos.c_str());
              h_pos->Draw("colz");
            }
            else{
              h_pos = (TH2F*)f_spring->Get(std::to_string(runnum).c_str());
              h_pos_all->Add(h_pos);
              std::cout<<runnum<<std::endl;
              h_pos->SetMarkerColorAlpha(num[i],0.35);
              h_pos->SetTitle(name_pos.c_str());
              h_pos->Draw("colz");
            }
          std::string filename = "results/csv_kin/kin_plot/"+std::to_string(runnum) + ".png";
          c_pos->SaveAs(filename.c_str());
          }
          h_pos_all->SetMarkerColorAlpha(num[i],0.75);
          c->cd(2);
          h_pos_all->Draw("p same");
          h_pos_kin_group->Add(h_pos_all);
          std::string filename1 = "results/csv_kin/kin_plot/"+name_pos+".png";
          auto *c_pos_all = new TCanvas(filename1.c_str());
          h_pos_all->SetMarkerColorAlpha(num[i],1);
          h_pos_all->Draw("colz");
          c_pos_all->SaveAs(filename1.c_str());
        }
      ++i;
      }//loop through different Q2
    }//x>0
  }//loop through x
  c->cd(1);
  h_neg_kin_group->SetMarkerColorAlpha(0,0);
  h_neg_kin_group->Draw("asame");
  c->cd(2);
  h_pos_kin_group->SetMarkerColorAlpha(0,0);
  h_pos_kin_group->Draw("asame");
  c->SaveAs("results/csv_kin/kin_plot/kinematics.png");
}