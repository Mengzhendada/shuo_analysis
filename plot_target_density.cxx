#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TMultiGraph.h"
#include "TAxis.h"
using json = nlohmann::json;

  double weighted_average(std::vector<double> v1,std::vector<int> v2){
    double sum1,sum2;
    for(int i = 0; i<v1.size();i++){
      sum1 += v1[i]*v2[i];
      sum2 +=v2[i];
//      std::cout<<"v1 "<<v1[i]<<" v2 "<<v2[i]<<std::endl;
    }
    //std::cout<<"in function "<<sum1/sum2<<std::endl;
    return sum1/sum2;
  }

void plot_target_density(){
json j;
{
  std::ifstream infile("results/ratio_run_group_updated.json");
  infile>>j;
}
std::string string[10] = {"hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};
TGraphErrors* Graph_D2 = new TGraphErrors();
Graph_D2->SetTitle("D2 density");
TGraphErrors* Graph_H2 = new TGraphErrors();
Graph_H2->SetTitle("H2 density");
int i_D2=0,i_H2=0;
for(json::iterator it=j.begin();it!=j.end();++it){
auto runjs = it.value();
int kingroup = std::stoi(it.key());
std::vector runs_D2_neg = runjs["neg"]["D2"].get<std::vector<int>>();
std::vector runs_D2_pos = runjs["pos"]["D2"].get<std::vector<int>>();
std::vector<double> D2_tmp1_neg,D2_tmp2_neg,D2_tmp3_neg,D2_psiin_neg,D2_psiout_neg;
std::vector<int> weight_D2_neg;
if(runs_D2_neg.empty()){continue;}
else{
for(auto runnums = runs_D2_neg.begin();runnums!=runs_D2_neg.end();++runnums){
  int run = *runnums;
  std::cout<<" check runnum "<<run<<std::endl;
  std::string file_name = "Data/magnet_current_" + std::to_string(run) + ".json";
  json magnet_in;
  {
    std::ifstream magnet_infile(file_name);
    try{
      magnet_infile>>magnet_in;
    } catch (json::parse_error){
      std::cout<<"Failed to open magnet current file"<<file_name<<std::endl;
    }
  }
  double tmp1_neg = magnet_in["hcLS218_3_T5"]["average"].get<double>();
  double tmp2_neg = magnet_in["hcLS218_3_T6"]["average"].get<double>();
  double tmp3_neg = magnet_in["hcLS218_3_T7"]["average"].get<double>();
  double psiin_neg = magnet_in["hcD2_P_Fill_Target_R"]["average"].get<double>();
  double psiout_neg = magnet_in["hcD2_P_Exhaust_R"]["average"].get<double>();
  double nentries_neg = magnet_in["hcLS218_3_T5"]["nentries"].get<int>();
  D2_tmp1_neg.push_back(tmp1_neg);
  D2_tmp2_neg.push_back(tmp2_neg);
  D2_tmp3_neg.push_back(tmp3_neg);
  D2_psiin_neg.push_back(psiin_neg);
  D2_psiout_neg.push_back(psiout_neg);
  weight_D2_neg.push_back(nentries_neg);
}
double D2_tmp1_neg_value = weighted_average(D2_tmp1_neg,weight_D2_neg);
double D2_tmp2_neg_value = weighted_average(D2_tmp2_neg,weight_D2_neg);
double D2_tmp3_neg_value = weighted_average(D2_tmp3_neg,weight_D2_neg);
double D2_psiin_neg_value = weighted_average(D2_psiin_neg,weight_D2_neg);
double D2_psiout_neg_value = weighted_average(D2_psiout_neg,weight_D2_neg);

double D2_tmp_neg_value = (D2_tmp1_neg_value+D2_tmp2_neg_value+D2_tmp3_neg_value)/3;
double D2_psi_neg_value = (D2_psiin_neg_value+D2_psiout_neg_value)/2;

if(D2_tmp_neg_value!=0){
double density_neg = D2_psi_neg_value/D2_tmp_neg_value*2.013553*6.89476/8.314462;
std::cout<<"density_D2 neg "<<density_neg<<" rungroup "<<kingroup<<" psi "<<D2_psi_neg_value<<" tmp "<<D2_tmp_neg_value<<std::endl;
Graph_D2->SetPoint(i_D2,kingroup+3,density_neg);
++i_D2;
}
}//end if else {empty()}

std::vector<double> D2_tmp1_pos,D2_tmp2_pos,D2_tmp3_pos,D2_psiin_pos,D2_psiout_pos;
std::vector<int> weight_D2_pos;
if(runs_D2_pos.empty()){continue;}
else{
for(auto runnums = runs_D2_pos.begin();runnums!=runs_D2_pos.end();runnums++){
  int run = *runnums;
  std::cout<<" check "<<run<<std::endl;
  std::string file_name = "Data/magnet_current_" + std::to_string(run) + ".json";
  json magnet_in;
  {
    std::ifstream magnet_infile(file_name);
    try{
      magnet_infile>>magnet_in;
    } catch (json::parse_error){
      std::cout<<"Failed to open magnet current file"<<file_name<<std::endl;
    }
  }
  double tmp1_pos = magnet_in["hcLS218_3_T5"]["average"].get<double>();
  double tmp2_pos = magnet_in["hcLS218_3_T6"]["average"].get<double>();
  double tmp3_pos = magnet_in["hcLS218_3_T7"]["average"].get<double>();
  double psiin_pos = magnet_in["hcD2_P_Fill_Target_R"]["average"].get<double>();
  double psiout_pos = magnet_in["hcD2_P_Exhaust_R"]["average"].get<double>();
  double nentries_pos = magnet_in["hcLS218_3_T5"]["nentries"].get<int>();
  D2_tmp1_pos.push_back(tmp1_pos);
  D2_tmp2_pos.push_back(tmp2_pos);
  D2_tmp3_pos.push_back(tmp3_pos);
  D2_psiin_pos.push_back(psiin_pos);
  D2_psiout_pos.push_back(psiout_pos);
  weight_D2_pos.push_back(nentries_pos);
}
double D2_tmp1_pos_value = weighted_average(D2_tmp1_pos,weight_D2_pos);
double D2_tmp2_pos_value = weighted_average(D2_tmp2_pos,weight_D2_pos);
double D2_tmp3_pos_value = weighted_average(D2_tmp3_pos,weight_D2_pos);
double D2_psiin_pos_value = weighted_average(D2_psiin_pos,weight_D2_pos);
double D2_psiout_pos_value = weighted_average(D2_psiout_pos,weight_D2_pos);

double D2_tmp_pos_value = (D2_tmp1_pos_value+D2_tmp2_pos_value+D2_tmp3_pos_value)/3;
double D2_psi_pos_value = (D2_psiin_pos_value+D2_psiout_pos_value)/2;

if(D2_tmp_pos_value!=0){
double density_pos = D2_psi_pos_value/D2_tmp_pos_value*2.013553*6.89476/8.314462;
std::cout<<"density_D2 "<<density_pos<<" rungroup "<<kingroup<<" psi "<<D2_psi_pos_value<<" tmp "<<D2_tmp_pos_value<<std::endl;
Graph_D2->SetPoint(i_D2,kingroup+6,density_pos);
++i_D2;
}
}//end ifelse(empty)

std::vector runs_H2_neg = runjs["neg"]["H2"].get<std::vector<int>>();
std::vector runs_H2_pos = runjs["pos"]["H2"].get<std::vector<int>>();
std::vector<double> H2_tmp1_neg,H2_tmp2_neg,H2_tmp3_neg,H2_psiin_neg,H2_psiout_neg;
std::vector<int> weight_H2_neg;
if(runs_H2_neg.empty()){continue;}
else{
for(auto runnums = runs_H2_neg.begin();runnums!=runs_H2_neg.end();runnums++){
  int run = *runnums;
  std::cout<<" check "<<run<<std::endl;
  std::string file_name = "Data/magnet_current_" + std::to_string(run) + ".json";
  json magnet_in;
  {
    std::ifstream magnet_infile(file_name);
    try{
      magnet_infile>>magnet_in;
    } catch (json::parse_error){
      std::cout<<"Failed to open magnet current file"<<file_name<<std::endl;
    }
  }
  double tmp1_neg = magnet_in["hcLS218_3_T5"]["average"].get<double>();
  double tmp2_neg = magnet_in["hcLS218_3_T6"]["average"].get<double>();
  double tmp3_neg = magnet_in["hcLS218_3_T7"]["average"].get<double>();
  double psiin_neg = magnet_in["hcH2_P_Fill_Target_R"]["average"].get<double>();
  double psiout_neg = magnet_in["hcH2_P_Exhaust_R"]["average"].get<double>();
  double nentries_neg = magnet_in["hcLS218_3_T5"]["nentries"].get<int>();
  H2_tmp1_neg.push_back(tmp1_neg);
  H2_tmp2_neg.push_back(tmp2_neg);
  H2_tmp3_neg.push_back(tmp3_neg);
  H2_psiin_neg.push_back(psiin_neg);
  H2_psiout_neg.push_back(psiout_neg);
  weight_H2_neg.push_back(nentries_neg);
}
double H2_tmp1_neg_value = weighted_average(H2_tmp1_neg,weight_H2_neg);
double H2_tmp2_neg_value = weighted_average(H2_tmp2_neg,weight_H2_neg);
double H2_tmp3_neg_value = weighted_average(H2_tmp3_neg,weight_H2_neg);
double H2_psiin_neg_value = weighted_average(H2_psiin_neg,weight_H2_neg);
double H2_psiout_neg_value = weighted_average(H2_psiout_neg,weight_H2_neg);

double H2_tmp_neg_value = (H2_tmp1_neg_value+H2_tmp3_neg_value)/2;
double H2_psi_neg_value = (H2_psiin_neg_value+H2_psiout_neg_value)/2;

if(H2_tmp_neg_value!=0){
double density_neg = H2_psi_neg_value/H2_tmp_neg_value*2.013553*6.89476/8.314462;
std::cout<<"density_H2 neg "<<density_neg<<" rungroup "<<kingroup<<std::endl;
Graph_H2->SetPoint(i_H2,kingroup+3,density_neg);
++i_H2;
}


}//end ifelse(empty())

std::vector<double> H2_tmp1_pos,H2_tmp2_pos,H2_tmp3_pos,H2_psiin_pos,H2_psiout_pos;
std::vector<int> weight_H2_pos;
if(runs_H2_pos.empty()){continue;}
else{
for(auto runnums = runs_H2_pos.begin();runnums!=runs_H2_pos.end();runnums++){
  int run = *runnums;
  std::cout<<" check "<<run<<std::endl;
  std::string file_name = "Data/magnet_current_" + std::to_string(run) + ".json";
  json magnet_in;
  {
    std::ifstream magnet_infile(file_name);
    try{
      magnet_infile>>magnet_in;
    } catch (json::parse_error){
      std::cout<<"Failed to open magnet current file"<<file_name<<std::endl;
    }
  }
  double tmp1_pos = magnet_in["hcLS218_3_T5"]["average"].get<double>();
  double tmp2_pos = magnet_in["hcLS218_3_T6"]["average"].get<double>();
  double tmp3_pos = magnet_in["hcLS218_3_T7"]["average"].get<double>();
  double psiin_pos = magnet_in["hcH2_P_Fill_Target_R"]["average"].get<double>();
  double psiout_pos = magnet_in["hcH2_P_Exhaust_R"]["average"].get<double>();
  double nentries_pos = magnet_in["hcLS218_3_T5"]["nentries"].get<int>();
  H2_tmp1_pos.push_back(tmp1_pos);
  H2_tmp2_pos.push_back(tmp2_pos);
  H2_tmp3_pos.push_back(tmp3_pos);
  H2_psiin_pos.push_back(psiin_pos);
  H2_psiout_pos.push_back(psiout_pos);
  weight_H2_pos.push_back(nentries_pos);
}
double H2_tmp1_pos_value = weighted_average(H2_tmp1_pos,weight_H2_pos);
double H2_tmp2_pos_value = weighted_average(H2_tmp2_pos,weight_H2_pos);
double H2_tmp3_pos_value = weighted_average(H2_tmp3_pos,weight_H2_pos);
double H2_psiin_pos_value = weighted_average(H2_psiin_pos,weight_H2_pos);
double H2_psiout_pos_value = weighted_average(H2_psiout_pos,weight_H2_pos);

double H2_tmp_pos_value = (H2_tmp1_pos_value+H2_tmp3_pos_value)/2;
double H2_psi_pos_value = (H2_psiin_pos_value+H2_psiout_pos_value)/2;

if(H2_tmp_pos_value!=0){
double density_pos = H2_psi_pos_value/H2_tmp_pos_value*2.013553*6.89476/8.314462;
std::cout<<"density_H2 "<<density_pos<<" rungroup "<<kingroup<<std::endl;
Graph_H2->SetPoint(i_H2,kingroup+6,density_pos);
++i_H2;
}
}//end ifelse(empty())
}//end of json iterator

TFile* root = new TFile("results/currentplot/Target_values.root","RECREATE");
Graph_D2->SetMarkerStyle(23);
Graph_D2->Draw("p");
auto mg_D2 = new TMultiGraph();
mg_D2->Add(Graph_D2);
mg_D2->Draw("a");
Graph_H2->SetMarkerStyle(24);
Graph_H2->Draw("p");
Graph_H2->GetXaxis()->SetTitle("run_group");
Graph_H2->GetYaxis()->SetTitle("P/T");
auto mg_H2 = new TMultiGraph();
mg_H2->Add(Graph_H2);
mg_H2->Draw("a");
mg_D2->Write("D2 density");
mg_H2->Write("H2 density");
root->Close();
}//end of program
