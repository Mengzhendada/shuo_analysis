#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "TGraphErrors.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TStyle.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void SHMS_rf_compare(){

  // RunGroup = RunGroup;
  json j_rungroup;
  {
    std::ifstream infile("db2/ratio_run_group_updated.json");
    infile>>j_rungroup;
  }
  std::cout<<"check"<<std::endl;

  //I already write cuts into 
  json j_cuts;
  {
    std::string infilename = "db2/PID_test.json";
    std::ifstream infile(infilename.c_str());
    infile>>j_cuts;
  }

  json j_rungroup_info;
  TGraphErrors *pos_rf_momentum = new TGraphErrors();
  pos_rf_momentum->SetTitle("pos,rfcut efficiency;momentum;eff");
  TGraphErrors *neg_rf_momentum = new TGraphErrors();
  neg_rf_momentum->SetTitle("neg,rfcut efficiency;momentum;eff");
  TGraphErrors *pos_rf_rungroup = new TGraphErrors();
  pos_rf_rungroup->SetTitle("pos,rfcut efficiency;rungroup;eff");
  TGraphErrors *neg_rf_rungroup = new TGraphErrors();
  neg_rf_rungroup->SetTitle("neg,rfcut efficiency;rungroup;eff");
  TGraphErrors *pos_rfsigma_momentum = new TGraphErrors();
  pos_rfsigma_momentum->SetTitle("pos,2ndrfcut efficiency;momentum;eff");
  TGraphErrors *neg_rfsigma_momentum = new TGraphErrors();
  neg_rfsigma_momentum->SetTitle("neg,2ndrfcut efficiency;momentum;eff");
  TGraphErrors *pos_rfsigma_rungroup = new TGraphErrors();
  pos_rfsigma_rungroup->SetTitle("pos,2ndrfcut efficiency;rungroup;eff");
  TGraphErrors *neg_rfsigma_rungroup = new TGraphErrors();
  neg_rfsigma_rungroup->SetTitle("neg,2ndrfcut efficiency;rungroup;eff");
  int i_g = 0;
  for(int i = 10;i<580;i=i+10){
    int RunGroup = i;
    std::cout<<RunGroup<<std::endl;
    std::string infile_name = "results/pid/rftime/rf_eff_"+std::to_string(RunGroup)+".json";
    std::ifstream ifs(infile_name.c_str());
    if(ifs.good()){
      json j_each;
      ifs>>j_each;
      double shms_p = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
      std::cout<<shms_p<<std::endl;
      if(j_each.find(std::to_string(RunGroup))!=j_each.end()){
        j_rungroup_info[(std::to_string(RunGroup)).c_str()] = j_each[(std::to_string(RunGroup)).c_str()];
        double pos_pi = j_each[(std::to_string(RunGroup)).c_str()]["pos"]["pi"].get<double>();
        double pos_K = j_each[(std::to_string(RunGroup)).c_str()]["pos"]["K"].get<double>();
        double neg_pi = j_each[(std::to_string(RunGroup)).c_str()]["neg"]["pi"].get<double>();
        double neg_K = j_each[(std::to_string(RunGroup)).c_str()]["neg"]["K"].get<double>();
        
        double pos_pi_2nd = j_each[(std::to_string(RunGroup)).c_str()]["pos"]["pi_sigma"].get<double>();
        double pos_K_2nd = j_each[(std::to_string(RunGroup)).c_str()]["pos"]["K_sigma"].get<double>();
        double neg_pi_2nd = j_each[(std::to_string(RunGroup)).c_str()]["neg"]["pi_sigma"].get<double>();
        double neg_K_2nd = j_each[(std::to_string(RunGroup)).c_str()]["neg"]["K_sigma"].get<double>();

        if(pos_pi>100&neg_pi>100){
          double eff_pos = 1-pos_K/pos_pi;
          double eff_pos_error = 1/pos_pi*sqrt(pos_K*(1-eff_pos));
            //wrong!!!std::sqrt(pos_pi+pos_K)*eff_pos;
          double eff_neg = 1-neg_K/neg_pi;
          double eff_neg_error = 1/neg_pi*sqrt(neg_K*(1-eff_neg));
          std::cout<<RunGroup<<" "<<shms_p<<" "<<eff_pos<<" "<<eff_neg<<std::endl;
          
          double eff_pos_2nd = 1-pos_K_2nd/pos_pi_2nd;
          double eff_pos_2nd_error = 1/pos_pi_2nd*sqrt(pos_K_2nd*(1-eff_pos_2nd));
          double eff_neg_2nd = 1-neg_K_2nd/neg_pi_2nd;
          double eff_neg_2nd_error = 1/neg_pi_2nd*sqrt(neg_K_2nd*(1-eff_neg_2nd));

          pos_rf_momentum->SetPoint(i_g,shms_p,eff_pos);
          pos_rf_momentum->SetPointError(i_g,0,eff_pos_error);
          neg_rf_momentum->SetPoint(i_g,shms_p,eff_neg);
          neg_rf_momentum->SetPointError(i_g,0,eff_neg_error);
          pos_rf_rungroup->SetPoint(i_g,RunGroup,eff_pos);
          pos_rf_rungroup->SetPointError(i_g,0,eff_pos_error);
          neg_rf_rungroup->SetPoint(i_g,RunGroup,eff_neg);
          neg_rf_rungroup->SetPointError(i_g,0,eff_neg_error);
          
          pos_rfsigma_momentum->SetPoint(i_g,shms_p,eff_pos_2nd);
          pos_rfsigma_momentum->SetPointError(i_g,0,eff_pos_2nd_error);
          neg_rfsigma_momentum->SetPoint(i_g,shms_p,eff_neg_2nd);
          neg_rfsigma_momentum->SetPointError(i_g,0,eff_neg_2nd_error);
          pos_rfsigma_rungroup->SetPoint(i_g,RunGroup,eff_pos_2nd);
          pos_rfsigma_rungroup->SetPointError(i_g,0,eff_pos_2nd_error);
          neg_rfsigma_rungroup->SetPoint(i_g,RunGroup,eff_neg_2nd);
          neg_rfsigma_rungroup->SetPointError(i_g,0,eff_neg_2nd_error);
          i_g++;
        }
        else{
          std::cout<<"Not enough pions "<<RunGroup<<std::endl;
        }
      }
    }
  }
  TCanvas *c_rf_momentum = new TCanvas();
  
  pos_rf_momentum->SetMarkerStyle(4);
  pos_rf_momentum->SetMarkerColor(kRed);
  pos_rf_momentum->Draw("AP");
  neg_rf_momentum->SetMarkerStyle(4);
  neg_rf_momentum->Draw("P same");
  c_rf_momentum->SaveAs("results/pid/SHMS_rf_momentum.pdf");
  TCanvas *c_rf_rungroup = new TCanvas();
  pos_rf_rungroup->SetMarkerStyle(4);
  pos_rf_rungroup->SetMarkerColor(kRed);
  pos_rf_rungroup->Draw("AP");
  neg_rf_rungroup->SetMarkerStyle(4);
  neg_rf_rungroup->Draw("P same");
  c_rf_rungroup->SaveAs("results/pid/SHMS_rf_rungroup.pdf");
  
  TCanvas *c_rf_momentum_2nd = new TCanvas();
  
  pos_rfsigma_momentum->SetMarkerStyle(4);
  pos_rfsigma_momentum->SetMarkerColor(kRed);
  pos_rfsigma_momentum->Draw("AP");
  neg_rfsigma_momentum->SetMarkerStyle(4);
  neg_rfsigma_momentum->Draw("P same");
  c_rf_momentum_2nd->SaveAs("results/pid/SHMS_rf_momentum_2nd.pdf");
  TCanvas *c_rf_rungroup_2nd = new TCanvas();
  pos_rfsigma_rungroup->SetMarkerStyle(4);
  pos_rfsigma_rungroup->SetMarkerColor(kRed);
  pos_rfsigma_rungroup->Draw("AP");
  neg_rfsigma_rungroup->SetMarkerStyle(4);
  neg_rfsigma_rungroup->Draw("P same");
  c_rf_rungroup_2nd->SaveAs("results/pid/SHMS_rf_rungroup_2nd.pdf");
}
