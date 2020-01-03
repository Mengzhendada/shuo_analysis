#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>

#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TAxis.h"

void plot_current_ratio_condense(){
  std::vector<string> current_name;
  current_name = {"ecDI_B_Set_NMR","ecDI_Set_Current","ecQ1_Set_Current","ecQ2_Set_Current","ecSDI_B_Set_NMR","ecSDI_Set_Current","ecSHB_Set_Current","ecSQ1_Set_Current","ecSQ2_Set_Current","ecSQ3_Set_Current"};
  std::vector<int> multiplier;
  int i = 0;
  for(auto it = current_name.begin();it!=current_name.end();++it){
    //std::cout<<i<<std::endl;
    multiplier.push_back(i);
    ++i;
  }
  std::string filename = "results/currentplot/Epic_values.root";
  TFile *root_file = new TFile(filename.c_str());
  TCanvas *c = new TCanvas();
 // c->SetGrid();
  auto mg = new TMultiGraph();
  int k = 0;
  for(auto it = current_name.begin();it!=current_name.end();++it){
    TGraphErrors* Graph_D2_average = new TGraphErrors();
    TGraphErrors* Graph_D2_average_scale = new TGraphErrors();
    std::string name = *it+"_average";
    std::cout<<name<<std::endl;
    if(root_file->Get(name.c_str())){std::cout<<"find graph"<<std::endl;}
    Graph_D2_average = (TGraphErrors*)root_file->Get(name.c_str());
    for(int ii = 0;ii<Graph_D2_average->GetN();++ii){
      double x = Graph_D2_average->GetX()[ii];
      double y = Graph_D2_average->GetY()[ii] + (2*multiplier[k]+1);
      //std::cout<<y<<std::endl;
      Graph_D2_average_scale->SetPoint(ii,x,y);
      //std::cout<<ii<<std::endl;
    }
      Graph_D2_average_scale->SetMarkerStyle(7);
    mg->Add(Graph_D2_average_scale,"pL");
    TAxis* a = mg->GetYaxis();
    //a->Set(10,0,11);
    //a->SetBinLabel(k+1,current_name[k].c_str());
    std::size_t pos = current_name[k].find("Set");
    std::string labelname = current_name[k].substr(2,pos-3); 
    a->ChangeLabel(k+2,-1,-1,-1,-1,-1,labelname);
    ++k;
  }
  int n = 2*current_name.size()+1;
  mg->Draw("a");
  mg->GetYaxis()->SetRangeUser(0,n);
 // c->Update();
 // c->BuildLegend();
  c->SetTitle("Set");
  c->SaveAs("results/currentplot/Set_Values.pdf");
 
  //plot true values
  std::vector<string> current_true_name;
  current_true_name = {"ecDI_B_True_NMR","ecDI_I_True","ecQ1_I_True","ecQ2_I_True","ecSDI_B_True_NMR","ecSDI_I_True","ecSHB_I_True","ecSQ1_I_True","ecSQ2_I_True","ecSQ3_I_True"};
  std::vector<int> multiplier_true;
  int i_true = 0;
  for(auto it = current_true_name.begin();it!=current_true_name.end();++it){
    //std::cout<<i<<std::endl;
    multiplier_true.push_back(i_true);
    ++i_true;
  }
  TCanvas *c_true = new TCanvas();
 // c->SetGrid();
  auto mg_true = new TMultiGraph();
  int k_true = 0;
  for(auto it = current_true_name.begin();it!=current_true_name.end();++it){
    TGraphErrors* Graph_D2_average = new TGraphErrors();
    TGraphErrors* Graph_D2_average_scale = new TGraphErrors();
    std::string name = *it+"_average";
    std::cout<<name<<std::endl;
    if(root_file->Get(name.c_str())){std::cout<<"find graph"<<std::endl;}
    Graph_D2_average = (TGraphErrors*)root_file->Get(name.c_str());
    for(int ii = 0;ii<Graph_D2_average->GetN();++ii){
      double x = Graph_D2_average->GetX()[ii];
      double y = Graph_D2_average->GetY()[ii] + (2*multiplier[k_true]+1);
      //std::cout<<y<<std::endl;
      Graph_D2_average_scale->SetPoint(ii,x,y);
      //std::cout<<ii<<std::endl;
    }
      Graph_D2_average_scale->SetMarkerStyle(7);
    mg_true->Add(Graph_D2_average_scale,"pL");
    TAxis* a = mg_true->GetYaxis();
    //a->Set(10,0,11);
    //a->SetBinLabel(k+1,current_name[k].c_str());
    std::size_t pos = current_true_name[k_true].find("True");
    std::string labelname = current_true_name[k_true].substr(2,pos-3); 
    a->ChangeLabel(k_true+2,-1,-1,-1,-1,-1,labelname);
    ++k_true;
  }
  int n_true = 2*current_true_name.size()+1;
  mg_true->Draw("a");
  mg_true->GetYaxis()->SetRangeUser(0,n_true);
 // c->Update();
 // c->BuildLegend();
  c_true->SaveAs("results/currentplot/True_Values.pdf");
  
  //plot coarse values
  std::vector<string> current_coarse_name;
  current_coarse_name = {"ecDI_I_coarse","ecQ1_I_coarse","ecQ2_I_coarse","ecSDI_I_coarse","ecSHB_I_coarse","ecSQ1_I_coarse","ecSQ2_I_coarse","ecSQ3_I_coarse"};
  std::vector<int> multiplier_coarse;
  int i_coarse = 0;
  for(auto it = current_coarse_name.begin();it!=current_coarse_name.end();++it){
    //std::cout<<i<<std::endl;
    multiplier_coarse.push_back(i_coarse);
    ++i_coarse;
  }
  TCanvas *c_coarse = new TCanvas();
 // c->SetGrid();
  auto mg_coarse = new TMultiGraph();
  int k_coarse = 0;
  for(auto it = current_coarse_name.begin();it!=current_coarse_name.end();++it){
    TGraphErrors* Graph_D2_average = new TGraphErrors();
    TGraphErrors* Graph_D2_average_scale = new TGraphErrors();
    std::string name = *it+"_average";
    std::cout<<name<<std::endl;
    if(root_file->Get(name.c_str())){std::cout<<"find graph"<<std::endl;}
    Graph_D2_average = (TGraphErrors*)root_file->Get(name.c_str());
    for(int ii = 0;ii<Graph_D2_average->GetN();++ii){
      double x = Graph_D2_average->GetX()[ii];
      double y = Graph_D2_average->GetY()[ii] + (2*multiplier[k_coarse]+1);
      //std::cout<<y<<std::endl;
      Graph_D2_average_scale->SetPoint(ii,x,y);
      //std::cout<<ii<<std::endl;
    }
      Graph_D2_average_scale->SetMarkerStyle(7);
    mg_coarse->Add(Graph_D2_average_scale,"pL");
    TAxis* a = mg_coarse->GetYaxis();
    //a->Set(10,0,11);
    //a->SetBinLabel(k+1,current_name[k].c_str());
    std::size_t pos = current_coarse_name[k_coarse].find("coarse");
    std::string labelname = current_coarse_name[k_coarse].substr(2,pos-3); 
    a->ChangeLabel(k_coarse+2,-1,-1,-1,-1,-1,labelname);
    ++k_coarse;
  }
  int n_coarse = 2*current_coarse_name.size()+1;
  mg_coarse->Draw("a");
  mg_coarse->GetYaxis()->SetRangeUser(0,n_coarse);
 // c->Update();
 // c->BuildLegend();
  c_coarse->SaveAs("results/currentplot/Coarse_Values.pdf");
}
