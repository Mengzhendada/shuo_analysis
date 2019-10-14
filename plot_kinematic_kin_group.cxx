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
    std::ifstream infile("db2/kin_group_xq2.json");
    infile>>j;
  }
  //TFile *f_spring = new TFile("results/csv_kin/kinematics_spring.root");
  //f_spring->ls();
  //f_spring->GetName();
  //TH2D *h = (TH2D*)f_spring->Get("7593");
  //TFile *f_fall = new TFile("results/csv_kin/kinematics_fall.root");  
  //TH2D *h1 = (TH2D*)f_fall->Get("6068");
  //f_fall->ls();
  int num[13] = {46,47,40,48,30,49,31,41,32,42,33,43,44};
  //  int num[13] = {1,2,3,4,5,6,7,8,9,40,41,42,43};
  //  std::string color[13] = {}
  int i = 0;
  auto *c_neg_kin_group = new TCanvas("csv pi-","csv pi-",2200,1450);
  auto *c_pos_kin_group = new TCanvas("csv pi+","csv pi+",2200,1450);
  //auto *c = new TCanvas("csv kinematics","csv kinematic",2200,1450);
  //c->Divide(2,1);
  //c->cd(1);
  //h->SetMarkerColorAlpha(2,0.35);
  //h->Draw("p");
  //h1->SetMarkerColorAlpha(3,0.35);
  //h1->Draw("ap same");
  TH2F *h_neg_kin_group = new TH2F("CSV pi-","CSV pi-",400,0,1,400,0,10);
  TH2F *h_pos_kin_group = new TH2F("CSV pi+","CSV pi+",400,0,1,400,0,10);
  //auto *c_xz = new TCanvas("x_z","x vs z",2200,1450);
  //c_xz->Divide(2,1);
  //TH2F *h_neg_xz = new TH2F("x_z","x vs z",400,0,3,400,0,1);
  //TH2F *h_pos_xz = new TH2F("x_z","x vs z",400,0,3,400,0,1);
  //auto *c_q2z = new TCanvas("Q2_z","Q2 vs z",2200,1450);
  //c_q2z->Divide(2,1);
  //TH2F *h_neg_q2z = new TH2F("Q2_z","Q2 vs z",400,0,3,400,0,10);
  //TH2F *h_pos_q2z = new TH2F("Q2_z","Q2 vs z",400,0,3,400,0,10);

  std::ofstream outfile("failedruns.json");
  json j_failed;

  TFile* kin_root = new TFile("results/csv_kin/kin_plot/kinematics.root","RECREATE");

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
              std::string f_fall_name = "results/csv_kin/kinematics_"+std::to_string(runnum)+".root";
              TFile* f_fall = new TFile(f_fall_name.c_str());
              std::string histo_name = "x_Q2_"+std::to_string(runnum);
              if(f_fall->Get(histo_name.c_str()))
              {
                h_neg= (TH2F*)f_fall->Get(histo_name.c_str());
                h_neg_all->Add(h_neg);
                std::cout<<runnum<<std::endl;
                h_neg->SetMarkerColorAlpha(num[i],0.35);
                h_neg->SetTitle(name_neg.c_str());
                h_neg->Draw("colz");
              }
              else{
                j_failed[std::to_string(runnum)] = runnum;
                //j_failed.push_back(runnum);
                //outfile<<"can't find "<<runnum<<std::endl;
                std::cout<<"Can't find "<<f_fall_name<<std::endl;
              }
            f_fall->Close();
            }
            else{
              std::string f_spring_name = "results/csv_kin/kinematics_"+std::to_string(runnum)+".root";
              TFile* f_spring = new TFile(f_spring_name.c_str());
              std::string histo_name = "x_Q2_"+std::to_string(runnum);
              if(f_spring->Get(histo_name.c_str()))
              {
                h_neg = (TH2F*)f_spring->Get(histo_name.c_str());
                h_neg_all->Add(h_neg);
                std::cout<<runnum<<std::endl;
                h_neg->SetMarkerColorAlpha(num[i],0.35);
                h_neg->SetTitle(name_neg.c_str());
                h_neg->Draw("colz");
              }
              else{
                j_failed[std::to_string(runnum)] = runnum;
                //j_failed.push_back(runnum);
                //outfile<<"can't find "<<runnum<<std::endl;
                std::cout<<"Can't find "<<f_spring_name<<std::endl;
              }
            f_spring->Close();
            }
            std::string filename = "results/csv_kin/kin_plot/"+std::to_string(runnum)+".png";
            c_neg->SetTitle(name_neg.c_str());
            c_neg->SaveAs(filename.c_str());

          }
          h_neg_all->SetMarkerStyle(8);
          h_neg_all->SetMarkerColorAlpha(num[i],0.75);
          //c->cd(1);
          c_neg_kin_group->cd();
          h_neg_all->Draw("p same");
          h_neg_kin_group->Add(h_neg_all);
          std::string filename1 = "results/csv_kin/kin_plot/"+name_neg+".png";
          auto *c_neg_all = new TCanvas(filename1.c_str());
          c_neg_all->SetTitle(name_neg.c_str());
          h_neg_all->SetMarkerColorAlpha(num[i],1);
          h_neg_all->Draw("colz");
          c_neg_all->SaveAs(filename1.c_str());
          kin_root->ReOpen("UPDATE");
          c_neg_all->Write(name_neg.c_str());
        }
        if(!v_D2_pos.empty()){
          auto *c_pos = new TCanvas(name_pos.c_str());
          c_pos->SetTitle(name_pos.c_str());
          for(int ii = 0;ii<v_D2_pos.size();++ii){
            TH2F *h_pos = new TH2F(name_pos.c_str(),name_pos.c_str(),400,0,1,400,0,10);
            int runnum = v_D2_pos[ii];
            if(runnum<7000){
              std::string f_fall_name = "results/csv_kin/kinematics_"+std::to_string(runnum)+".root";
              TFile* f_fall = new TFile(f_fall_name.c_str());
              std::string histo_name = "x_Q2_"+std::to_string(runnum);
              if(f_fall->Get(histo_name.c_str()))
              {
                h_pos= (TH2F*)f_fall->Get(histo_name.c_str());
                h_pos_all->Add(h_pos);
                std::cout<<runnum<<std::endl;
                h_pos->SetMarkerColorAlpha(num[i],0.35);
                h_pos->SetTitle(name_pos.c_str());
                h_pos->Draw("colz");
              }
              else{
                j_failed[std::to_string(runnum)] = runnum;
                //j_failed.push_back(runnum);
                // outfile<<"can't find "<<runnum<<std::endl;
              }
            }
            else{
              std::string f_spring_name = "results/csv_kin/kinematics_"+std::to_string(runnum)+".root";
              TFile* f_spring = new TFile(f_spring_name.c_str());
              std::string histo_name = "x_Q2_"+std::to_string(runnum);
              if(f_spring->Get(histo_name.c_str()))
              {
                h_pos = (TH2F*)f_spring->Get(histo_name.c_str());
                h_pos_all->Add(h_pos);
                std::cout<<runnum<<std::endl;
                h_pos->SetMarkerColorAlpha(num[i],0.35);
                h_pos->SetTitle(name_pos.c_str());
                h_pos->Draw("colz");
              }
              else{
                j_failed[std::to_string(runnum)] = runnum;
                //j_failed.push_back(runnum);
                // outfile<<"can't find "<<runnum<<std::endl;
              }
            }
            std::string filename = "results/csv_kin/kin_plot/"+std::to_string(runnum) + ".png";
            c_pos->SaveAs(filename.c_str());
          }
          h_pos_all->SetMarkerStyle(8);
          h_pos_all->SetMarkerColorAlpha(num[i],0.75);
          //c->cd(2);
          c_pos_kin_group->cd();
          h_pos_all->Draw("p same");
          h_pos_kin_group->Add(h_pos_all);
          std::string filename1 = "results/csv_kin/kin_plot/"+name_pos+".png";
          auto *c_pos_all = new TCanvas(filename1.c_str());
          h_pos_all->SetMarkerColorAlpha(num[i],1);
          h_pos_all->Draw("colz");
          c_pos_all->SaveAs(filename1.c_str());
          kin_root->ReOpen("UPDATE");
          c_pos_all->Write(name_pos.c_str());
        }
        ++i;
      }//loop through different Q2
    }//x>0
  }//loop through x

//  auto c_runplan = new TCanvas();
//  for(json::iterator it = j.begin();it!=j.end();++it){
//    double x = std::stod(it.key());
//    auto runjs = it.value();
//    if(x>0){
//      for(json::iterator ik = runjs.begin();ik!=runjs.end();++ik){
//        double Q2 = std::stod(ik.key());
//        std::string title = "x_Q2_"+std::to_string(x).substr(0,4)+"_"+std::to_string(Q2).substr(0,4); 
//        TH2F *h_run_plan = new TH2F("CSV run plan","CSV run plan",400,0,1,400,0,10);
//        //TH2F *h_run_plan = new TH2F("CSV run plan",title.c_str(),400,0,1,400,0,10);
//        h_run_plan->GetXaxis()->SetTitle("x");
//        h_run_plan->GetYaxis()->SetTitle("Q2");
//        h_run_plan->Fill(x,Q2);
//        h_run_plan->SetMarkerStyle(kFullDotLarge);
//        h_run_plan->SetMarkerColorAlpha(num[i],1);
//        h_run_plan->SetMarkerSize(1.1);
//        h_run_plan->Draw("p same");
//        ++i;
//      }//Q2 loop
//    }//if x > 0
//
//  }//x loop
  //c_runplan->BuildLegend(0.75,0.65,1,1,"csv run plan","p");

 // kin_root->ReOpen("UPDATE");
  outfile<<j_failed.dump(4)<<std::endl;
  c_neg_kin_group->cd();
  c_neg_kin_group->BuildLegend(0.75,0.65,1,1,"CSV kinematics pi-","p");
//  c_runplan->BuildLegend(0.75,0.65,1,1,"csv run plan","p");
  c_neg_kin_group->SetTitle("CSV Kinematics pi-");
 // c_neg_kin_group->Write();
  //h_neg_kin_group->SetMarkerColorAlpha(0,0);
  //h_neg_kin_group->Draw("ap");
  c_pos_kin_group->cd();
  c_pos_kin_group->BuildLegend(0.75,0.65,1,1,"CSV kinematics pi+","p");
  //c_runplan->BuildLegend(0.75,0.65,1,1,"csv run plan","p");
  c_pos_kin_group->SetTitle("CSV Kinematics pi+");
 // c_pos_kin_group->Write();
  //h_pos_kin_group->SetMarkerColorAlpha(0,0);
  //h_pos_kin_group->Draw("ap");
  //c->SaveAs("results/csv_kin/kin_plot/kinematics.png");
  c_neg_kin_group->SaveAs("results/csv_kin/kin_plot/kinematics_neg.png");
  c_pos_kin_group->SaveAs("results/csv_kin/kin_plot/kinematics_pos.png");
 // c_runplan->SaveAs("results/csv_kin/kin_plot/kinematics_runplan.png");
  kin_root->Close();
}
