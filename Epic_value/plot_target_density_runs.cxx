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
  //TGraphErrors* Graph_D2_Temperature = new TGraphErrors();
  //Graph_D2_Temperature->SetTitle("D2_T");
  //TGraphErrors* Graph_D2_Pressure = new TGraphErrors();
  //Graph_D2_Pressure->SetTitle("D2_P");
  //TGraphErrors* Graph_H2_Temperature = new TGraphErrors();
  //Graph_H2_Temperature->SetTitle("H2_T");
  //TGraphErrors* Graph_H2_Pressure = new TGraphErrors();
  //Graph_H2_Pressure->SetTitle("H2_P");

  //int i_D2=0,i_H2=0,i_D2_psi=0,i_H2_psi=0,i_D2_Density = 0,i_H2_Density = 0;
  int i_D2 = 0,i_H2 = 0;
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
  //auto c = new TCanvas();
  //Graph_D2_Temperature->SetMarkerStyle(23);
  //Graph_D2_Temperature->Draw("ap");
  //Graph_D2_Temperature->Write();

  //c->SaveAs("results/currentplot/D2_T.pdf");
  auto c_density = new TCanvas();
  Graph_D2->SetMarkerStyle(24);
  Graph_D2->Draw("ap");
  //auto mg_D2 = new TMultiGraph();
  //mg_D2->Add(Graph_D2);
  //mg_D2->Draw("a");
  //mg_D2->Write("D2_density");
  Graph_D2->Write("D2_density");
  Graph_D2->GetYaxis()->SetTitle("kg/m3");
  Graph_D2->GetXaxis()->SetTitle("run number");
  //Graph_D2->SetMarkerStyle(23);
  Graph_D2->SetMarkerSize(1.2);
  c_density->SaveAs("results/currentplot/D2_density.pdf");

  //auto c_D2_P = new TCanvas();
  //Graph_D2_Pressure->SetMarkerStyle(24);
  //Graph_D2_Pressure->Draw("ap");
  //Graph_D2_Pressure->Write();
  //c_D2_P->SaveAs("results/currentplot/D2_P.pdf");
  //auto c1 = new TCanvas();
  //Graph_H2_Temperature->SetMarkerStyle(23);
  //Graph_H2_Temperature->Draw("ap");
  //Graph_H2_Temperature->GetXaxis()->SetTitle("run_group");
  //Graph_H2_Temperature->Write();

  //c1->SaveAs("results/currentplot/H2_T.pdf");
  auto c_H2_density = new TCanvas();
  Graph_H2->SetMarkerStyle(24);
  Graph_H2->Draw("ap");
  Graph_H2->GetYaxis()->SetTitle("kg/m3");
  Graph_H2->GetXaxis()->SetTitle("run number");
  Graph_H2->Write("H2_density");
  //auto mg_H2 = new TMultiGraph();
  //mg_H2->Add(Graph_H2);
  //mg_H2->Draw("a");
  //mg_H2->Write("H2_density");
  //auto legend = gPad->BuildLegend();
  //legend->SetHeader("H2_density","C");
  //gPad->Update();
  //gPad->Write("H2_density");
  c_H2_density->SaveAs("results/currentplot/H2_Density.pdf");
  //mg_D2->BuildLegend();
  //mg_H2->BuildLegend();
  //mg_D2->Write("D2 density");
  //mg_H2->Write("H2 density");
  //auto c1_H2_P = new TCanvas;
  //Graph_H2_Pressure->SetMarkerStyle(24);
  //Graph_H2_Pressure->Draw("ap");
  //Graph_H2_Pressure->Write();
  //c1_H2_P->SaveAs("results/currentplot/H2_P.pdf");
  root->Close();
}//end of program
