
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;


void SHMS_rf_DE_plots_low(){

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_DE;
  }
  std::vector<double> rf_cuts = j_DE["SHMS"]["rf_time_right_cuts"].get<std::vector<double>>();


  //choose rf cut
  int i_rfcut = 7;
  double rf_right = rf_cuts[i_rfcut];
  double rf_left = 2-rf_right;
  std::cout<<"rf cut right "<<rf_right<<", left "<<rf_left<<std::endl;
  TGraphErrors* g_pos_pieff = new TGraphErrors();
  TGraphErrors* g_neg_pieff = new TGraphErrors();
  TGraphErrors* g_pos_pipurity = new TGraphErrors();
  TGraphErrors* g_neg_pipurity = new TGraphErrors();
  int i_graph_pos = 0;
  int i_graph_neg = 0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!pos_D2.empty() && !neg_D2.empty()){
      //std::cout<<"check"<<std::endl;
      json j_rfeff_delta;
      {
        std::string name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_compare_low.json";
        std::ifstream ifstream(name.c_str());
        ifstream>>j_rfeff_delta;
      }
      //std::cout<<"check"<<std::endl;
      if(!j_rfeff_delta.empty() ){
        auto runjs = j_rfeff_delta.begin().value();
        //std::cout<<runjs.begin().key()<<std::endl;
        for(auto it = runjs.begin();it!=runjs.end();++it){
          int i_dp = std::stoi(it.key());
         // std::cout<<i_dp<<std::endl;
          double shms_p = it.value()["shms_p"].get<double>();
          if(shms_p<2.8 && !it.value()["pos"]["pi_eff_all"].empty()){
            std::vector<double> pi_eff_Ns_pos = it.value()["pos"]["pi_eff_Ns"].get<std::vector<double>>();
            double pi_eff_N_pos = pi_eff_Ns_pos[i_rfcut];
            double pi_eff_all_pos = it.value()["pos"]["pi_eff_all"].get<double>();
            std::vector<double> K_con_Ns_pos = it.value()["pos"]["Ks"].get<std::vector<double>>();
            double K_n_pos = K_con_Ns_pos[i_rfcut];
            double pi_eff_pos = pi_eff_N_pos/pi_eff_all_pos;
            double pi_purity_pos = pi_eff_N_pos/(pi_eff_N_pos+K_n_pos);
            //double pi_eff_pos_err = sqrt(pi_eff_all_pos-pi_eff_N_pos)/pi_eff_all_pos;
            double pi_eff_pos_err = sqrt(pi_eff_pos*(1-pi_eff_pos)/pi_eff_all_pos);
            double pi_purity_pos_err = sqrt(K_n_pos)/(pi_eff_N_pos+K_n_pos);

            std::vector<double> pi_eff_Ns_neg = it.value()["neg"]["pi_eff_Ns"].get<std::vector<double>>();
            double pi_eff_N_neg = pi_eff_Ns_neg[i_rfcut];
            double pi_eff_all_neg = it.value()["neg"]["pi_eff_all"].get<double>();
            std::vector<double> K_con_Ns_neg = it.value()["neg"]["Ks"].get<std::vector<double>>();
            double K_n_neg = K_con_Ns_neg[i_rfcut];
            double pi_eff_neg = pi_eff_N_neg/pi_eff_all_neg;
            double pi_purity_neg = pi_eff_N_neg/(pi_eff_N_neg+K_n_neg);
            double pi_eff_neg_err = sqrt(pi_eff_neg*(1-pi_eff_neg)/pi_eff_all_neg);
            //sqrt(pi_eff_all_neg-pi_eff_N_neg)/pi_eff_all_neg;
            double pi_purity_neg_err = sqrt(K_n_neg)/(pi_eff_N_neg+K_n_neg);

            g_pos_pieff->SetPoint(i_graph_pos,shms_p,pi_eff_pos);
            g_pos_pieff->SetPointError(i_graph_pos,0,pi_eff_pos_err);
            g_pos_pipurity->SetPoint(i_graph_pos,shms_p,pi_purity_pos);
            g_pos_pipurity->SetPointError(i_graph_pos,0,pi_purity_pos_err);
            g_neg_pieff->SetPoint(i_graph_neg,shms_p,pi_eff_neg);
            g_neg_pieff->SetPointError(i_graph_neg,0,pi_eff_neg_err);
            g_neg_pipurity->SetPoint(i_graph_neg,shms_p,pi_purity_neg);
            g_neg_pipurity->SetPointError(i_graph_neg,0,pi_purity_neg_err);
            i_graph_pos++;
            i_graph_neg++;
          }//if less than 2.8
        }
      }//if json file exist
    }//if neg and pos not empty
  }//loop over rungroups
  TMultiGraph* mg_pieff = new TMultiGraph();
  g_pos_pieff->SetMarkerStyle(8);
  g_pos_pieff->SetMarkerColor(kRed);
  g_pos_pieff->SetTitle("pos pi eff");
  mg_pieff->Add(g_pos_pieff,"P");
  g_neg_pieff->SetMarkerStyle(8);
  g_neg_pieff->SetTitle("neg pi eff");
  mg_pieff->Add(g_neg_pieff,"P");
  TCanvas* c_pieff = new TCanvas();
  mg_pieff->Draw("A");
  mg_pieff->Fit("pol1");
  gStyle->SetOptFit(0001);
  mg_pieff->SetMaximum(1.01);
  mg_pieff->SetMinimum(0.97);
  //mg_pieff->GetXaxis()->SetLimits(0.99,1.01);
  c_pieff->BuildLegend(0.75,0.25,0.95,0.4);
  c_pieff->SaveAs("results/pid/rftime_new/pi_eff_fixedrf_low.pdf");
  TMultiGraph* mg_pipurity = new TMultiGraph();
  g_pos_pipurity->SetMarkerStyle(8);
  g_pos_pipurity->SetMarkerColor(kRed);
  g_pos_pipurity->SetTitle("pos pi purity");
  //gStyle->SetOptFit(1011);
  mg_pipurity->Add(g_pos_pipurity,"P");
  g_neg_pipurity->SetMarkerStyle(8);
  g_neg_pipurity->SetTitle("neg pi purity");
  mg_pipurity->Add(g_neg_pipurity,"P");
  TCanvas* c_pipurity = new TCanvas();
  mg_pipurity->Draw("A");
  mg_pipurity->SetMaximum(1.01);
  mg_pipurity->SetMinimum(0.7);
  //mg_pipurity->GetXaxis()->SetLimits(0.99,1.01);
  c_pipurity->BuildLegend(0.75,0.75,0.95,0.95);
  c_pipurity->SaveAs("results/pid/rftime_new/pi_purity_fixedrf_low.pdf");

  //TF1 * f = new TF1("f",[](double*x,double*p){return p[0] + p[1]*x[0];},2.8,5,2);
  TCanvas *c_neg_purity = new TCanvas();
  g_neg_pipurity->Draw("AP");
  g_neg_pipurity->Fit("pol2","","",3,5);
  g_neg_pipurity->SetMaximum(1.01);
  g_neg_pipurity->SetMinimum(0.7);
  gStyle->SetOptFit(0001);
  c_neg_purity->SaveAs("results/pid/rftime_new/pi_purity_fixedrf_neg_low.pdf");
  TCanvas *c_pos_purity = new TCanvas();
  c_pos_purity->cd();
  g_pos_pipurity->Draw("AP");
  g_pos_pipurity->Fit("pol2","","",2.9,5);
  g_pos_pipurity->SetMaximum(1.01);
  g_pos_pipurity->SetMinimum(0.7);
  gStyle->SetOptFit(0001);
  c_pos_purity->SaveAs("results/pid/rftime_new/pi_purity_fixedrf_pos_low.pdf");


  //choose pi eff to be 99%
  TGraphErrors* g_pos_pieff_2 = new TGraphErrors();
  TGraphErrors* g_pos_pipurity_2 = new TGraphErrors();
  TGraphErrors* g_pos_rfcut_2 = new TGraphErrors();
  TGraphErrors* g_neg_pieff_2 = new TGraphErrors();
  TGraphErrors* g_neg_pipurity_2 = new TGraphErrors();
  TGraphErrors* g_neg_rfcut_2 = new TGraphErrors();
  int i_pos_2 = 0,i_neg_2 = 0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
      json j_rfeff_delta;
      {
        std::string name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_low_compare.json";
        std::ifstream ifstream(name.c_str());
        ifstream>>j_rfeff_delta;
      }
    if(!pos_D2.empty() && !neg_D2.empty()&& !j_rfeff_delta.empty()){
      auto runjs = j_rfeff_delta.begin().value();
      //std::cout<<runjs.begin().key()<<std::endl;
      for(auto it = runjs.begin();it!=runjs.end();++it){
        int i_dp = std::stoi(it.key());
        //std::cout<<i_dp<<std::endl;
        double shms_p = it.value()["shms_p"].get<double>();
        if(shms_p<2.8 && !it.value()["pos"]["pi_eff_all"].empty()){
        std::vector<double> pi_eff_Ns_pos = it.value()["pos"]["pi_eff_Ns"].get<std::vector<double>>();
        double pi_eff_all_pos = it.value()["pos"]["pi_eff_all"].get<double>();
        std::vector<double> K_con_Ns_pos = it.value()["pos"]["Ks"].get<std::vector<double>>();
        int i_rf_pos = 0;
        for(auto ij = pi_eff_Ns_pos.begin();ij!=pi_eff_Ns_pos.end();++ij){
          double pi_eff_N_pos = *ij;
          double pi_eff_pos = pi_eff_N_pos/pi_eff_all_pos;
          if(pi_eff_pos > 0.99){

            break; 

          }//if >0.99
          i_rf_pos++;
        }//loop over pi eff N to see if it's >0.9
        std::vector<double> rf_cuts_high_pos = it.value()["pos"]["rf_cuts_high"].get<std::vector<double>>();
        double rf_cut_high_pos = rf_cuts_high_pos[i_rf_pos];
        double pi_eff_pos = pi_eff_Ns_pos[i_rf_pos]/pi_eff_all_pos;
        double pi_purity_pos = pi_eff_Ns_pos[i_rf_pos]/(pi_eff_Ns_pos[i_rf_pos]+K_con_Ns_pos[i_rf_pos]);
        double pi_eff_pos_err = sqrt(pi_eff_all_pos-pi_eff_Ns_pos[i_rf_pos])/pi_eff_all_pos;
        double pi_purity_pos_err = sqrt(K_con_Ns_pos[i_rf_pos])/(pi_eff_Ns_pos[i_rf_pos]+K_con_Ns_pos[i_rf_pos]);
        g_pos_rfcut_2->SetPoint(i_pos_2,shms_p,rf_cut_high_pos);
        g_pos_pieff_2->SetPoint(i_pos_2,shms_p,pi_eff_pos);
        g_pos_pieff_2->SetPointError(i_pos_2,0,pi_eff_pos_err);
        g_pos_pipurity_2->SetPoint(i_pos_2,shms_p,pi_purity_pos);
        g_pos_pipurity_2->SetPointError(i_pos_2,0,pi_purity_pos_err);
        i_pos_2++;
        std::vector<double> pi_eff_Ns_neg = it.value()["neg"]["pi_eff_Ns"].get<std::vector<double>>();
        double pi_eff_all_neg = it.value()["neg"]["pi_eff_all"].get<double>();
        std::vector<double> K_con_Ns_neg = it.value()["neg"]["Ks"].get<std::vector<double>>();
        int i_rf_neg = 0;
        for(auto ij = pi_eff_Ns_neg.begin();ij!=pi_eff_Ns_neg.end();++ij){
          double pi_eff_N_neg = *ij;
          double pi_eff_neg = pi_eff_N_neg/pi_eff_all_neg;
          if(pi_eff_neg > 0.99){

            break; 

          }//if >0.99
          i_rf_neg++;
        }//loop over pi eff N to see if it's >0.9
        std::vector<double> rf_cuts_high_neg = it.value()["neg"]["rf_cuts_high"].get<std::vector<double>>();
        double rf_cut_high_neg = rf_cuts_high_neg[i_rf_neg];
        double pi_eff_neg = pi_eff_Ns_neg[i_rf_neg]/pi_eff_all_neg;
        double pi_purity_neg = pi_eff_Ns_neg[i_rf_neg]/(pi_eff_Ns_neg[i_rf_neg]+K_con_Ns_neg[i_rf_neg]);
        double pi_eff_neg_err = sqrt(pi_eff_all_neg-pi_eff_Ns_neg[i_rf_neg])/pi_eff_all_neg;
        double pi_purity_neg_err = sqrt(K_con_Ns_neg[i_rf_neg])/(pi_eff_Ns_neg[i_rf_neg]+K_con_Ns_neg[i_rf_neg]);
        g_neg_rfcut_2->SetPoint(i_neg_2,shms_p,rf_cut_high_neg);
        g_neg_pieff_2->SetPoint(i_neg_2,shms_p,pi_eff_neg);
        g_neg_pieff_2->SetPointError(i_neg_2,0,pi_eff_neg_err);
        g_neg_pipurity_2->SetPoint(i_neg_2,shms_p,pi_purity_neg);
        g_neg_pipurity_2->SetPointError(i_neg_2,0,pi_purity_neg_err);
        i_neg_2++;
        }//if less than 2.8
      }//loop over dp

    }//if not empty
  }//loop over rungroup
  TCanvas* c_rfcut_2 = new TCanvas();
  g_pos_rfcut_2->SetTitle("pos rf right");
  g_neg_rfcut_2->SetTitle("neg rf right");
  g_pos_rfcut_2->SetMarkerColor(kRed);
  g_pos_rfcut_2->SetMarkerStyle(8);
  g_neg_rfcut_2->SetMarkerStyle(8);
  TMultiGraph *mg_rfright = new TMultiGraph();
  mg_rfright->Add(g_pos_rfcut_2,"P");
  mg_rfright->Add(g_neg_rfcut_2,"P");
  mg_rfright->Draw("A");
  mg_rfright->SetMinimum(1.4);
  mg_rfright->Fit("pol1");
  gStyle->SetOptFit(1011);
  c_rfcut_2->BuildLegend(0.75,0.75,0.95,0.95);
  c_rfcut_2->SaveAs("results/pid/rftime_new/rfcut_high_fixed99_low.pdf");
  TMultiGraph* mg_pieff_2 = new TMultiGraph();
  g_pos_pieff_2->SetMarkerStyle(8);
  g_pos_pieff_2->SetMarkerColor(kRed);
  g_pos_pieff_2->SetTitle("pos pi eff");
  mg_pieff_2->Add(g_pos_pieff_2,"P");
  g_neg_pieff_2->SetMarkerStyle(8);
  g_neg_pieff_2->SetTitle("neg pi eff");
  mg_pieff_2->Add(g_neg_pieff_2,"P");
  TCanvas* c_pieff_2 = new TCanvas();
  mg_pieff_2->Draw("A");
  mg_pieff_2->SetMaximum(1);
  mg_pieff_2->SetMinimum(0.97);
  c_pieff_2->BuildLegend(0.75,0.75,0.95,0.95);
  c_pieff_2->SaveAs("results/pid/rftime_new/pi_eff_fixed99_low.pdf");
  TMultiGraph* mg_pipurity_2 = new TMultiGraph();
  g_pos_pipurity_2->SetMarkerStyle(8);
  g_pos_pipurity_2->SetMarkerColor(kRed);
  g_pos_pipurity_2->SetTitle("pos pi purity");
  mg_pipurity_2->Add(g_pos_pipurity_2,"P");
  g_neg_pipurity_2->SetMarkerStyle(8);
  g_neg_pipurity_2->SetTitle("neg pi purity");
  mg_pipurity_2->Add(g_neg_pipurity_2,"P");
  TCanvas* c_pipurity_2 = new TCanvas();
  mg_pipurity_2->Draw("A");
  mg_pipurity_2->SetMaximum(1.01);
  mg_pipurity_2->SetMinimum(0.7);
  c_pipurity_2->BuildLegend(0.75,0.75,0.95,0.95);
  c_pipurity_2->SaveAs("results/pid/rftime_new/pi_purity_fixed99_low.pdf");

}
