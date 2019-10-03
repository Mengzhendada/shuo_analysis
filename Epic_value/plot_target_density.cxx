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

double Get_Density(std::string target, double T,double P){
  std::cout<<"Get Density for "<<target<<std::endl;
  if(target == "H2"){
    int T_1 = T;
    int T_2 = (T-T_1)*100;
    std::string name = "shuo_analysis/Epic_value/Target_fluid_property/"+target+"_"+std::to_string(T_1)+"d"+std::to_string(T_2)+"K.txt";
    std::cout<<"file name "<<name<<std::endl;
    std::ifstream infile(name);
    if(infile.fail()){std::cout<<"H2 fluid property file doesn't exit"<<std::endl;}
    double a,b,t;
    std::vector<double> Pressure,Density;
    std::string line;
    while (!infile.eof()) 
    {
      getline(infile,line);

      if (line.length() == 0 || line[0] == 'T'){}
      else
      {
      infile>>t>>a>>b;
      Pressure.push_back(a);
      Density.push_back(b);
      infile.ignore(100,'\n');
      }
    }
    int entries = Pressure.size();
    int i = 0, j = entries -1,k;
    if(P<Pressure.front()){std::cout<<"Pressure out of range"<<std::endl;
      return 0;}
    else{
      if(P>Pressure.back()){std::cout<<"Pressure out of range too large"<<std::endl;
        return 0;}
      else{
        while(i!=j)
        {
          k = i+(j-i)/2;
          Pressure[k]>=P ? j=k : i=k+1;
        }
      }
    }
    return Density[k];
  }
  else{
    if(target == "D2"){
      int T_1 = T;
      int T_2 = (T-T_1)*100;
      std::string name = "shuo_analysis/Epic_value/Target_fluid_property/"+target+"_"+std::to_string(T_1)+"d"+std::to_string(T_2)+"K.txt";
      std::ifstream infile(name);
      if(infile.fail()){std::cout<<"D2 fluid property file doesn't exit"<<std::endl;}
      double a,b;
      std::vector<double> Pressure,Density;
      while(!infile.eof()){
        infile>>a>>b;
        Pressure.push_back(a);
        Density.push_back(b);
        infile.ignore(100,'\n');
      }
      int entries = Pressure.size();
      int i = 0, j = entries -1,k;
      if(P<Pressure.front()){std::cout<<"Pressure out of range"<<std::endl;
        return 0;}
      else{
        if(P>Pressure.back()){std::cout<<"Pressure out of range too large"<<std::endl;
          return 0;}
        else{
          while(i!=j)
          {
            k = i+(j-i)/2;
            Pressure[k]>=P ? j=k : i=k+1;
          }
        }
      }
      return Density[k];

    }
    else{std::cout<<"wrong target input"<<std::endl;
      return 0;}
  }
}

void plot_target_density(){
json j;
{
  std::ifstream infile("results/ratio_run_group_updated.json");
  infile>>j;
}
std::string string[11] = {"hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};
TGraphErrors* Graph_D2 = new TGraphErrors();
Graph_D2->SetTitle("D2 density");
TGraphErrors* Graph_H2 = new TGraphErrors();
Graph_H2->SetTitle("H2 density");
TGraphErrors* Graph_D2_Temperature = new TGraphErrors();
Graph_D2_Temperature->SetTitle("D2_T");
TGraphErrors* Graph_D2_Pressure = new TGraphErrors();
Graph_D2_Pressure->SetTitle("D2_P");
TGraphErrors* Graph_H2_Temperature = new TGraphErrors();
Graph_H2_Temperature->SetTitle("H2_T");
TGraphErrors* Graph_H2_Pressure = new TGraphErrors();
Graph_H2_Pressure->SetTitle("H2_P");

int i_D2=0,i_H2=0,i_D2_psi=0,i_H2_psi=0,i_D2_Density = 0,i_H2_Density = 0;
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

Graph_D2_Temperature->SetPoint(i_D2,kingroup+3,D2_tmp_neg_value);
++i_D2;
Graph_D2_Pressure->SetPoint(i_D2,kingroup+3,D2_psi_neg_value);
++i_D2_psi;
double density = Get_Density("D2",D2_tmp_neg_value,D2_psi_neg_value);
Graph_D2->SetPoint(i_D2_Density,kingroup+3,density);
++i_D2_Density;
//if(D2_tmp_neg_value!=0){
//double density_neg = D2_psi_neg_value/D2_tmp_neg_value*2.013553*6.89476/8.314462;
//std::cout<<"density_D2 neg "<<density_neg<<" rungroup "<<kingroup<<" psi "<<D2_psi_neg_value<<" tmp "<<D2_tmp_neg_value<<std::endl;
//Graph_D2->SetPoint(i_D2,kingroup+3,density_neg);
//++i_D2;
//}
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

Graph_D2_Temperature->SetPoint(i_D2,kingroup+3,D2_tmp_pos_value);
++i_D2;
Graph_D2_Pressure->SetPoint(i_D2,kingroup+3,D2_psi_pos_value);
++i_D2_psi;
double density = Get_Density("D2",D2_tmp_pos_value,D2_psi_pos_value);
Graph_D2->SetPoint(i_D2_Density,kingroup+3,density);
++i_D2_Density;
//if(D2_tmp_pos_value!=0){
//double density_pos = D2_psi_pos_value/D2_tmp_pos_value*2.013553*6.89476/8.314462;
//std::cout<<"density_D2 "<<density_pos<<" rungroup "<<kingroup<<" psi "<<D2_psi_pos_value<<" tmp "<<D2_tmp_pos_value<<std::endl;
//Graph_D2->SetPoint(i_D2,kingroup+6,density_pos);
//++i_D2;
//}
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

Graph_H2_Temperature->SetPoint(i_H2,kingroup+3,H2_tmp_neg_value);
++i_H2;
Graph_H2_Pressure->SetPoint(i_H2,kingroup+3,H2_psi_neg_value);
++i_H2_psi;
double density = Get_Density("H2",H2_tmp_neg_value,H2_psi_neg_value);
Graph_H2->SetPoint(i_H2_Density,kingroup+3,density);
++i_H2_Density;
//if(H2_tmp_neg_value!=0){
//double density_neg = H2_psi_neg_value/H2_tmp_neg_value*2.013553*6.89476/8.314462;
//std::cout<<"density_H2 neg "<<density_neg<<" rungroup "<<kingroup<<std::endl;
//Graph_H2->SetPoint(i_H2,kingroup+3,density_neg);
//++i_H2;
//}


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

Graph_H2_Temperature->SetPoint(i_H2,kingroup+3,H2_tmp_pos_value);
++i_H2;
Graph_H2_Pressure->SetPoint(i_H2,kingroup+3,H2_psi_pos_value);
++i_H2_psi;
double density = Get_Density("H2",H2_tmp_pos_value,H2_psi_pos_value);
Graph_H2->SetPoint(i_H2_Density,kingroup+3,density);
++i_H2_Density;
//if(H2_tmp_pos_value!=0){
//double density_pos = H2_psi_pos_value/H2_tmp_pos_value*2.013553*6.89476/8.314462;
//std::cout<<"density_H2 "<<density_pos<<" rungroup "<<kingroup<<std::endl;
//Graph_H2->SetPoint(i_H2,kingroup+6,density_pos);
//++i_H2;
//}
}//end ifelse(empty())
}//end of json iterator
TFile* root = new TFile("results/currentplot/Target_values.root","RECREATE");
auto c = new TCanvas();
Graph_D2_Temperature->SetMarkerStyle(23);
Graph_D2_Temperature->Draw("ap");
Graph_D2_Temperature->Write();

c->SaveAs("results/currentplot/D2_T.pdf");
auto c_density = new TCanvas();
Graph_D2->SetMarkerStyle(24);
Graph_D2->Draw("p");
auto mg_D2 = new TMultiGraph();
mg_D2->Add(Graph_D2);
mg_D2->Draw("a");
mg_D2->Write("D2_density");
c_density->SaveAs("results/currentplot/D2_density.pdf");

auto c_D2_P = new TCanvas();
Graph_D2_Pressure->SetMarkerStyle(24);
Graph_D2_Pressure->Draw("ap");
Graph_D2_Pressure->Write();
c_D2_P->SaveAs("results/currentplot/D2_P.pdf");
auto c1 = new TCanvas();
Graph_H2_Temperature->SetMarkerStyle(23);
Graph_H2_Temperature->Draw("ap");
Graph_H2_Temperature->GetXaxis()->SetTitle("run_group");
Graph_H2_Temperature->Write();

c1->SaveAs("results/currentplot/H2_T.pdf");
auto c_H2_density = new TCanvas();
Graph_H2->SetMarkerStyle(24);
Graph_H2->Draw("p");
Graph_H2->GetYaxis()->SetTitle("Density");
auto mg_H2 = new TMultiGraph();
mg_H2->Add(Graph_H2);
mg_H2->Draw("a");
mg_H2->Write("H2_density");
c_H2_density->SaveAs("results/currentplot/H2_Density.pdf");
//mg_D2->BuildLegend();
//mg_H2->BuildLegend();
//mg_D2->Write("D2 density");
//mg_H2->Write("H2 density");
auto c1_H2_P = new TCanvas;
Graph_H2_Pressure->SetMarkerStyle(24);
Graph_H2_Pressure->Draw("ap");
Graph_H2_Pressure->Write();
c1_H2_P->SaveAs("results/currentplot/H2_P.pdf");
root->Close();
}//end of program
