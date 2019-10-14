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


double Get_Density(std::string target, double T,double P){
  //std::cout<<"Get Density for "<<target<<std::endl;
  if(target == "H2"){
    int T_1 = T;
    int T_2 = (T-T_1)*100;
    std::string name = "shuo_analysis/Epic_value/Target_fluid_property/"+target+"_"+std::to_string(T_1)+"d"+std::to_string(T_2)+"K.txt";
    //std::cout<<"file name "<<name<<std::endl;
    std::ifstream infile(name);
    if(infile.fail()){std::cout<<"H2 fluid property file doesn't exist "<<name<<std::endl;}
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
      return -1;}
    else{
      if(P>Pressure.back()){std::cout<<"Pressure out of range too large"<<std::endl;
        return 1;}
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
      if(infile.fail()){std::cout<<"D2 fluid property file doesn't exist "<<name<<std::endl;}
      double a,b,t;
      std::string line;
      std::vector<double> Pressure,Density;
      while(!infile.eof())
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
        return -1;}
      else{
        if(P>Pressure.back()){std::cout<<"Pressure out of range too large"<<std::endl;
          return 1;}
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

void plot_target_density_runs(){
  json j;
  {
    std::ifstream infile("db2/run_info_group.json");
    infile>>j;
  }
  std::string string[11] = {"hcLS218_2_T5","hcLS218_2_T6","hcLS218_2_T7","hcLS218_3_T5","hcLS218_3_T6","hcLS218_3_T7","hcD2_P_Exhaust_R","hcD2_P_Fill_Target_R","hcH2_P_Exhaust_R","hcH2_P_Fill_Target_R"};
  TGraphErrors* Graph_D2 = new TGraphErrors();
  Graph_D2->SetTitle("D2 density");
  TGraphErrors* Graph_H2 = new TGraphErrors();
  Graph_H2->SetTitle("H2 density");
  TGraphErrors* Graph_D2_Temperature = new TGraphErrors();
  Graph_D2_Temperature->SetTitle("D2 Temperature");
  Graph_D2_Temperature->GetXaxis()->SetTitle("run");
  Graph_D2_Temperature->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_D2_Pressure = new TGraphErrors();
  Graph_D2_Pressure->SetTitle("D2 Pressure");
  Graph_D2_Pressure->GetXaxis()->SetTitle("run");
  Graph_D2_Pressure->GetYaxis()->SetTitle("psia");
  TGraphErrors* Graph_H2_Temperature = new TGraphErrors();
  Graph_H2_Temperature->SetTitle("H2 Temperature");
  Graph_H2_Temperature->GetXaxis()->SetTitle("run");
  Graph_H2_Temperature->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_H2_Pressure = new TGraphErrors();
  Graph_H2_Pressure->SetTitle("H2 Pressure");
  Graph_H2_Pressure->GetXaxis()->SetTitle("run");
  Graph_H2_Pressure->GetYaxis()->SetTitle("psia");
  
  TGraphErrors* Graph_D2_fall = new TGraphErrors();
  Graph_D2_fall->SetTitle("D2 density");
  TGraphErrors* Graph_H2_fall = new TGraphErrors();
  Graph_H2_fall->SetTitle("H2 density");
  TGraphErrors* Graph_D2_Temperature_fall = new TGraphErrors();
  Graph_D2_Temperature_fall->SetTitle("D2 Temperature");
  Graph_D2_Temperature_fall->GetXaxis()->SetTitle("run");
  Graph_D2_Temperature_fall->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_D2_Pressure_fall = new TGraphErrors();
  Graph_D2_Pressure_fall->SetTitle("D2 Pressure");
  Graph_D2_Pressure_fall->GetXaxis()->SetTitle("run");
  Graph_D2_Pressure_fall->GetYaxis()->SetTitle("psia");
  TGraphErrors* Graph_H2_Temperature_fall = new TGraphErrors();
  Graph_H2_Temperature_fall->SetTitle("H2 Temperature");
  Graph_H2_Temperature_fall->GetXaxis()->SetTitle("run");
  Graph_H2_Temperature_fall->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_H2_Pressure_fall = new TGraphErrors();
  Graph_H2_Pressure_fall->SetTitle("H2 Pressure");
  Graph_H2_Pressure_fall->GetXaxis()->SetTitle("run");
  Graph_H2_Pressure_fall->GetYaxis()->SetTitle("psia");

  TGraphErrors* Graph_D2_spring = new TGraphErrors();
  Graph_D2_spring->SetTitle("D2 density");
  TGraphErrors* Graph_H2_spring = new TGraphErrors();
  Graph_H2_spring->SetTitle("H2 density");
  TGraphErrors* Graph_D2_Temperature_spring = new TGraphErrors();
  Graph_D2_Temperature_spring->SetTitle("D2 Temperature");
  Graph_D2_Temperature_spring->GetXaxis()->SetTitle("run");
  Graph_D2_Temperature_spring->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_D2_Pressure_spring = new TGraphErrors();
  Graph_D2_Pressure_spring->SetTitle("D2 Pressure");
  Graph_D2_Pressure_spring->GetXaxis()->SetTitle("run");
  Graph_D2_Pressure_spring->GetYaxis()->SetTitle("psia");
  TGraphErrors* Graph_H2_Temperature_spring = new TGraphErrors();
  Graph_H2_Temperature_spring->SetTitle("H2 Temperature");
  Graph_H2_Temperature_spring->GetXaxis()->SetTitle("run");
  Graph_H2_Temperature_spring->GetYaxis()->SetTitle("Kelvin");
  TGraphErrors* Graph_H2_Pressure_spring = new TGraphErrors();
  Graph_H2_Pressure_spring->SetTitle("H2 Pressure");
  Graph_H2_Pressure_spring->GetXaxis()->SetTitle("run");
  Graph_H2_Pressure_spring->GetYaxis()->SetTitle("psia");
  
  int i_D2=0,i_H2=0,i_D2_P=0,i_H2_P=0,i_D2_T = 0,i_H2_T = 0;
  int i_D2_fall = 0,i_H2_fall = 0,i_D2_P_fall = 0,i_H2_P_fall = 0,i_D2_T_fall = 0,i_H2_T_fall = 0;
  int i_D2_spring = 0,i_H2_spring = 0,i_D2_P_spring = 0,i_H2_P_spring = 0,i_D2_T_spring = 0,i_H2_T_spring = 0;
  
  for(json::iterator it=j.begin();it!=j.end();++it){
    auto runjs = it.value();
    int runnum = std::stoi(it.key());
    int target_id = runjs["target_id"].get<int>();
    std::cout<<"for run "<<runnum<<" target "<<target_id<<std::endl;
    switch(target_id){
      case 2:
        {
      std::string magnet_file = "Data/magnet_current_" + std::to_string(runnum) +".json";
      std::ifstream magnet_infile(magnet_file);
      if(magnet_infile.good()){
        json magnet_in;
        {
          magnet_infile>>magnet_in;
        }
        double tmp1 = magnet_in["hcLS218_3_T5"]["average"].get<double>();
        double tmp3 = magnet_in["hcLS218_3_T7"]["average"].get<double>();
        double psi_in = magnet_in["hcH2_P_Fill_Target_R"]["average"].get<double>();
        double psi_out = magnet_in["hcH2_P_Exhaust_R"]["average"].get<double>();
        double tmp = (tmp1+tmp3)/2;
        double psi = (psi_in+psi_out)/2;
        double density = Get_Density("H2",tmp,psi);
        std::cout<<"tmp "<<tmp<<" psi "<<psi<<" density "<<density<<std::endl;
        Graph_H2->SetPoint(i_H2,runnum,density);
        ++i_H2;
        Graph_H2_Temperature->SetPoint(i_H2_T,runnum,tmp);
        ++i_H2_T;
        Graph_H2_Pressure->SetPoint(i_H2_P,runnum,psi);
        ++i_H2_P;
        if(runnum<7000){
        Graph_H2_fall->SetPoint(i_H2_fall,runnum,density);
        ++i_H2_fall;
        Graph_H2_Temperature_fall->SetPoint(i_H2_T_fall,runnum,tmp);
        ++i_H2_T_fall;
        Graph_H2_Pressure_fall->SetPoint(i_H2_P_fall,runnum,psi);
        ++i_H2_P_fall;
        }
        else{
        Graph_H2_spring->SetPoint(i_H2_spring,runnum,density);
        ++i_H2_spring;
        Graph_H2_Temperature_spring->SetPoint(i_H2_T_spring,runnum,tmp);
        ++i_H2_T_spring;
        Graph_H2_Pressure_spring->SetPoint(i_H2_P_spring,runnum,psi);
        ++i_H2_P_spring;
        }
      }//if magnet file exit
      else{std::cout<<"H2 magnet file not exit"<<std::endl;}
       break;
        }
      case 3:
        {
        std::string magnet_file = "Data/magnet_current_" + std::to_string(runnum) +".json";
        std::ifstream magnet_infile(magnet_file);
        if(magnet_infile.good()){
          json magnet_in;
          {
            magnet_infile>>magnet_in;
          }
          double tmp1 = magnet_in["hcLS218_3_T5"]["average"].get<double>();
          double tmp2 = magnet_in["hcLS218_3_T6"]["average"].get<double>();
          double tmp3 = magnet_in["hcLS218_3_T7"]["average"].get<double>();
          double psi_in = magnet_in["hcD2_P_Fill_Target_R"]["average"].get<double>();
          double psi_out = magnet_in["hcD2_P_Exhaust_R"]["average"].get<double>();
          double tmp = (tmp1+tmp2+tmp3)/3;
          double psi = (psi_in+psi_out)/2;
          double density = Get_Density("D2",tmp,psi);
          //std::cout<<"for run "<<runnum<<" target "<<target_id<<std::endl;
          std::cout<<"tmp "<<tmp<<" psi "<<psi<<" density "<<density<<std::endl;
        if(density!=0){  
        Graph_D2->SetPoint(i_D2,runnum,density);
        ++i_D2;
        Graph_D2_Temperature->SetPoint(i_D2_T,runnum,tmp);
        ++i_D2_T;
        Graph_D2_Pressure->SetPoint(i_D2_P,runnum,psi);
        ++i_D2_P;
        if(runnum<7000){
        Graph_D2_fall->SetPoint(i_D2_fall,runnum,density);
        ++i_D2_fall;
        Graph_D2_Temperature_fall->SetPoint(i_D2_T_fall,runnum,tmp);
        ++i_D2_T_fall;
        Graph_D2_Pressure_fall->SetPoint(i_D2_P_fall,runnum,psi);
        ++i_D2_P_fall;
        }
        else{
        Graph_D2_spring->SetPoint(i_D2_spring,runnum,density);
        ++i_D2_spring;
        Graph_D2_Temperature_spring->SetPoint(i_D2_T_spring,runnum,tmp);
        ++i_D2_T_spring;
        Graph_D2_Pressure_spring->SetPoint(i_D2_P_spring,runnum,psi);
        ++i_D2_P_spring;
        }
        }
        }//if magnet file exist
        else{std::cout<<"D2 magnet file not exist"<<std::endl;}
        break;
        }
      case 5:
        {
          std::cout<<"Dummy target"<<std::endl;
          break;
        }
    }
  }//end of json iterator
  TFile* root = new TFile("results/currentplot/Target_values.root","RECREATE");
  auto c_density_D2 = new TCanvas();
  Graph_D2->SetMarkerStyle(24);
  Graph_D2->Draw("ap");
  Graph_D2->GetYaxis()->SetTitle("kg/m3");
  Graph_D2->GetXaxis()->SetTitle("run number");
  Graph_D2->SetMarkerSize(1.2);
  c_density_D2->SaveAs("results/currentplot/D2_density.pdf");
  c_density_D2->Write("D2_density");
  auto c_D2_P = new TCanvas();
  Graph_D2_Pressure->SetMarkerStyle(24);
  Graph_D2_Pressure->Draw("ap");
  c_D2_P->Write("D2_P");
  c_D2_P->SaveAs("results/currentplot/D2_P.pdf");
  auto c_D2_T = new TCanvas();
  Graph_D2_Temperature->SetMarkerStyle(24);
  Graph_D2_Temperature->Draw("ap");
  c_D2_T->Write("D2_T");
  c_D2_T->SaveAs("results/currentplot/D2_T.pdf");
  
  auto c_density_H2 = new TCanvas();
  Graph_H2->SetMarkerStyle(24);
  Graph_H2->Draw("ap");
  Graph_H2->GetYaxis()->SetTitle("kg/m3");
  Graph_H2->GetXaxis()->SetTitle("run number");
  Graph_H2->SetMarkerSize(1.2);
  c_density_H2->SaveAs("results/currentplot/H2_density.pdf");
  c_density_H2->Write("H2_density");
  auto c_H2_P = new TCanvas();
  Graph_H2_Pressure->SetMarkerStyle(24);
  Graph_H2_Pressure->Draw("ap");
  c_H2_P->Write("H2_P");
  c_H2_P->SaveAs("results/currentplot/H2_P.pdf");
  auto c_H2_T = new TCanvas();
  Graph_H2_Temperature->SetMarkerStyle(24);
  Graph_H2_Temperature->Draw("ap");
  c_H2_T->Write("H2_T");
  c_H2_T->SaveAs("results/currentplot/H2_T.pdf");
  
  auto c_density_D2_fall = new TCanvas();
  Graph_D2_fall->SetMarkerStyle(24);
  Graph_D2_fall->Draw("ap");
  Graph_D2_fall->GetYaxis()->SetTitle("kg/m3");
  Graph_D2_fall->GetXaxis()->SetTitle("run number");
  Graph_D2_fall->SetMarkerSize(1.2);
  c_density_D2_fall->SaveAs("results/currentplot/D2_density_fall.pdf");
  c_density_D2_fall->Write("D2_density_fall");
  auto c_D2_P_fall = new TCanvas();
  Graph_D2_Pressure_fall->SetMarkerStyle(24);
  Graph_D2_Pressure_fall->Draw("ap");
  c_D2_P_fall->Write("D2_P_fall");
  c_D2_P_fall->SaveAs("results/currentplot/D2_P_fall.pdf");
  auto c_D2_T_fall = new TCanvas();
  Graph_D2_Temperature_fall->SetMarkerStyle(24);
  Graph_D2_Temperature_fall->Draw("ap");
  c_D2_T_fall->Write("D2_T_fall");
  c_D2_T_fall->SaveAs("results/currentplot/D2_T_fall.pdf");

  auto c_density_D2_spring = new TCanvas();
  Graph_D2_spring->SetMarkerStyle(24);
  Graph_D2_spring->Draw("ap");
  Graph_D2_spring->GetYaxis()->SetTitle("kg/m3");
  Graph_D2_spring->GetXaxis()->SetTitle("run number");
  Graph_D2_spring->SetMarkerSize(1.2);
  c_density_D2_spring->SaveAs("results/currentplot/D2_density_spring.pdf");
  c_density_D2_spring->Write("D2_density_spring");
  auto c_D2_P_spring = new TCanvas();
  Graph_D2_Pressure_spring->SetMarkerStyle(24);
  Graph_D2_Pressure_spring->Draw("ap");
  c_D2_P_spring->Write("D2_P_spring");
  c_D2_P_spring->SaveAs("results/currentplot/D2_P_spring.pdf");
  auto c_D2_T_spring = new TCanvas();
  Graph_D2_Temperature_spring->SetMarkerStyle(24);
  Graph_D2_Temperature_spring->Draw("ap");
  c_D2_T_spring->Write("D2_T_spring");
  c_D2_T_spring->SaveAs("results/currentplot/D2_T_spring.pdf");
  
  auto c_density_H2_fall = new TCanvas();
  Graph_H2_fall->SetMarkerStyle(24);
  Graph_H2_fall->Draw("ap");
  Graph_H2_fall->GetYaxis()->SetTitle("kg/m3");
  Graph_H2_fall->GetXaxis()->SetTitle("run number");
  Graph_H2_fall->SetMarkerSize(1.2);
  c_density_H2_fall->SaveAs("results/currentplot/H2_density_fall.pdf");
  c_density_H2_fall->Write("H2_density_fall");
  auto c_H2_P_fall = new TCanvas();
  Graph_H2_Pressure_fall->SetMarkerStyle(24);
  Graph_H2_Pressure_fall->Draw("ap");
  c_H2_P_fall->Write("H2_P_fall");
  c_H2_P_fall->SaveAs("results/currentplot/H2_P_fall.pdf");
  auto c_H2_T_fall = new TCanvas();
  Graph_H2_Temperature_fall->SetMarkerStyle(24);
  Graph_H2_Temperature_fall->Draw("ap");
  c_H2_T_fall->Write("H2_T_fall");
  c_H2_T_fall->SaveAs("results/currentplot/H2_T_fall.pdf");

  auto c_density_H2_spring = new TCanvas();
  Graph_H2_spring->SetMarkerStyle(24);
  Graph_H2_spring->Draw("ap");
  Graph_H2_spring->GetYaxis()->SetTitle("kg/m3");
  Graph_H2_spring->GetXaxis()->SetTitle("run number");
  Graph_H2_spring->SetMarkerSize(1.2);
  c_density_H2_spring->SaveAs("results/currentplot/H2_density_spring.pdf");
  c_density_H2_spring->Write("H2_density_spring");
  auto c_H2_P_spring = new TCanvas();
  Graph_H2_Pressure_spring->SetMarkerStyle(24);
  Graph_H2_Pressure_spring->Draw("ap");
  c_H2_P_spring->Write("H2_P_spring");
  c_H2_P_spring->SaveAs("results/currentplot/H2_P_spring.pdf");
  auto c_H2_T_spring = new TCanvas();
  Graph_H2_Temperature_spring->SetMarkerStyle(24);
  Graph_H2_Temperature_spring->Draw("ap");
  c_H2_T_spring->Write("H2_T_spring");
  c_H2_T_spring->SaveAs("results/currentplot/H2_T_spring.pdf");

  root->Close();
}//end of program
