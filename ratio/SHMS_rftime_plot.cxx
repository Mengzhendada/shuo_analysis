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
void SHMS_rftime_plot(){
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runsinfo;
  }
  TGraphErrors* g_pos_zhad_fall_HGCeff = new TGraphErrors();
  TGraphErrors* g_pos_zhad_spring_HGCeff = new TGraphErrors();
  TGraphErrors* g_neg_zhad_fall_HGCeff = new TGraphErrors();
  TGraphErrors* g_neg_zhad_spring_HGCeff = new TGraphErrors();
  TGraphErrors* g_pos_shmsp_fall_HGCeff = new TGraphErrors();
  TGraphErrors* g_pos_shmsp_spring_HGCeff = new TGraphErrors();
  TGraphErrors* g_neg_shmsp_fall_HGCeff = new TGraphErrors();
  TGraphErrors* g_neg_shmsp_spring_HGCeff = new TGraphErrors();
  TGraphErrors* g_pos_zhad_fall_Kpiratio = new TGraphErrors();
  TGraphErrors* g_pos_zhad_spring_Kpiratio = new TGraphErrors();
  TGraphErrors* g_neg_zhad_fall_Kpiratio = new TGraphErrors();
  TGraphErrors* g_neg_zhad_spring_Kpiratio = new TGraphErrors();
  TGraphErrors* g_pos_shmsp_fall_Kpiratio = new TGraphErrors();
  TGraphErrors* g_pos_shmsp_spring_Kpiratio = new TGraphErrors();
  TGraphErrors* g_neg_shmsp_fall_Kpiratio = new TGraphErrors();
  TGraphErrors* g_neg_shmsp_spring_Kpiratio = new TGraphErrors();
  TGraphErrors* g_zhad_fall_Kratio = new TGraphErrors();
  TGraphErrors* g_zhad_spring_Kratio = new TGraphErrors();
  TGraphErrors* g_shmsp_fall_Kratio = new TGraphErrors();
  TGraphErrors* g_shmsp_spring_Kratio = new TGraphErrors();
  TGraphErrors* g_zhad_fall_piratio = new TGraphErrors();
  TGraphErrors* g_zhad_spring_piratio = new TGraphErrors();
  TGraphErrors* g_shmsp_fall_piratio = new TGraphErrors();
  TGraphErrors* g_shmsp_spring_piratio = new TGraphErrors();
  int i_fall = 0,i_spring =0;
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!pos_D2.empty() && !neg_D2.empty()){
      double shms_p = it.value()["shms_p"].get<double>();
      double zhad = 1;
      //calculate charge
      double neg_charge = 0;
      for(auto it_neg = neg_D2.begin();it_neg!=neg_D2.end();++it_neg){
        std::string RunNumber_str = std::to_string(*it_neg);
        double charge = j_runsinfo[RunNumber_str.c_str()]["charge"].get<double>();
        std::cout<<" neg charge "<<charge<<std::endl;
        neg_charge = neg_charge+ charge;
      }
      double pos_charge = 0;
      for(auto it_pos = pos_D2.begin();it_pos!=pos_D2.end();++it_pos){
        std::string RunNumber_str = std::to_string(*it_pos);
        double charge = j_runsinfo[RunNumber_str.c_str()]["charge"].get<double>();
        std::cout<<" pos charge "<<charge<<std::endl;
        pos_charge = pos_charge+ charge;
      }
      if(shms_p>2.8){
        //if(1.25*shms_p>2.85){
        json j_rf_HGC;
        {
          std::string name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_4_HGC.json";
          std::ifstream ifstream(name.c_str());
          ifstream>>j_rf_HGC;
        }
        auto point_js = j_rf_HGC[(std::to_string(RunGroup)).c_str()];
        for(auto ik = point_js.begin();ik!=point_js.end();++ik){
          auto point_jsi = ik.value();
          shms_p = point_jsi["shms_dp"].get<double>();
          zhad = point_jsi["z_center"].get<double>();
          double N_pion_hgc_pos = point_jsi["pos"]["pi_HGC_all"].get<double>();
          double N_pion_hgc_neg = point_jsi["neg"]["pi_HGC_all"].get<double>();
          double N_pion_antiHGC_pos = point_jsi["pos"]["pi_antiHGC_all"].get<double>();
          double N_pion_antiHGC_neg = point_jsi["neg"]["pi_antiHGC_all"].get<double>();
          double N_Kaon_antiHGC_pos = point_jsi["pos"]["K_antiHGC_all"].get<double>();
          double N_Kaon_antiHGC_neg = point_jsi["neg"]["K_antiHGC_all"].get<double>();

          double HGC_eff_pos = N_pion_hgc_pos/(N_pion_hgc_pos+N_pion_antiHGC_pos);
          double HGC_eff_neg = N_pion_hgc_neg/(N_pion_hgc_neg+N_pion_antiHGC_neg);
          double R_Kpi_ratio_pos = N_Kaon_antiHGC_pos/(N_pion_hgc_pos+N_pion_antiHGC_pos); 
          double R_Kpi_ratio_neg = N_Kaon_antiHGC_neg/(N_pion_hgc_neg+N_pion_antiHGC_neg); 
          double K_pos_neg_ratio = (N_Kaon_antiHGC_neg/neg_charge)/(N_Kaon_antiHGC_pos/pos_charge); 
          double pi_pos_neg_ratio = (N_pion_hgc_neg/neg_charge)/(N_pion_hgc_pos/pos_charge);

          double HGC_eff_pos_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos- N_pion_hgc_pos)/(N_pion_hgc_pos+N_pion_antiHGC_pos);
          double HGC_eff_neg_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg- N_pion_hgc_neg)/(N_pion_hgc_neg+N_pion_antiHGC_neg);
          double Kpi_ratio_pos_Ks_err = std::sqrt(N_Kaon_antiHGC_pos);        
          double Kpi_ratio_pos_pions_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos);
          double R_Kpi_ratio_pos_err = R_Kpi_ratio_pos*std::sqrt(std::pow(Kpi_ratio_pos_Ks_err/N_Kaon_antiHGC_pos,2)+std::pow(Kpi_ratio_pos_pions_err/(N_pion_hgc_pos+N_pion_antiHGC_pos),2));
          double Kpi_ratio_neg_Ks_err = std::sqrt(N_Kaon_antiHGC_neg);        
          double Kpi_ratio_neg_pions_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg);
          double R_Kpi_ratio_neg_err = R_Kpi_ratio_neg*std::sqrt(std::pow(Kpi_ratio_neg_Ks_err/N_Kaon_antiHGC_neg,2)+std::pow(Kpi_ratio_neg_pions_err/(N_pion_hgc_neg+N_pion_antiHGC_neg),2));
          //double R_Kpi_ratio_pos_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos- N_Kaon_antiHGC_pos)/(N_pion_hgc_pos+N_pion_antiHGC_pos); 
          //double R_Kpi_ratio_neg_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg- N_Kaon_antiHGC_neg)/(N_pion_hgc_neg+N_pion_antiHGC_neg); 
          double K_pos_err = std::sqrt(N_Kaon_antiHGC_pos)/pos_charge;
          double K_neg_err = std::sqrt(N_Kaon_antiHGC_neg)/neg_charge;
          double pi_pos_err = std::sqrt(N_pion_hgc_pos)/pos_charge;
          double pi_neg_err = std::sqrt(N_pion_hgc_neg)/neg_charge;
          double K_pos_neg_ratio_err = K_pos_neg_ratio*std::sqrt(std::pow((K_pos_err/(N_Kaon_antiHGC_pos/pos_charge)),2)+std::pow(K_neg_err/(N_Kaon_antiHGC_neg/neg_charge),2));
          double pi_pos_neg_ratio_err = pi_pos_neg_ratio*std::sqrt(std::pow((pi_pos_err/(N_pion_hgc_pos/pos_charge)),2)+std::pow(pi_neg_err/(N_pion_hgc_neg/neg_charge),2));
          //if(pi_pos_neg_ratio>2) std::cout<<"K+/K- ratio greater than 2 "<<RunGroup<<std::endl;
          if(RunGroup<410 && HGC_eff_pos_err<0.2){
            g_pos_zhad_fall_HGCeff->SetPoint(i_fall,zhad,HGC_eff_pos);
            g_pos_zhad_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
            g_pos_shmsp_fall_HGCeff->SetPoint(i_fall,shms_p,HGC_eff_pos);
            g_pos_shmsp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
            g_neg_zhad_fall_HGCeff->SetPoint(i_fall,zhad,HGC_eff_neg);
            g_neg_zhad_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
            g_neg_shmsp_fall_HGCeff->SetPoint(i_fall,shms_p,HGC_eff_neg);
            g_neg_shmsp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
            g_pos_zhad_fall_Kpiratio->SetPoint(i_fall,zhad,R_Kpi_ratio_pos);
            g_pos_zhad_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
            g_pos_shmsp_fall_Kpiratio->SetPoint(i_fall,shms_p,R_Kpi_ratio_pos);
            g_pos_shmsp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
            g_neg_zhad_fall_Kpiratio->SetPoint(i_fall,zhad,R_Kpi_ratio_neg);
            g_neg_zhad_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
            g_neg_shmsp_fall_Kpiratio->SetPoint(i_fall,shms_p,R_Kpi_ratio_neg);
            g_neg_shmsp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
            g_zhad_fall_Kratio->SetPoint(i_fall,zhad,K_pos_neg_ratio);
            g_zhad_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
            g_shmsp_fall_Kratio->SetPoint(i_fall,shms_p,K_pos_neg_ratio);
            g_shmsp_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
            g_zhad_fall_piratio->SetPoint(i_fall,zhad,pi_pos_neg_ratio);
            g_zhad_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
            g_shmsp_fall_piratio->SetPoint(i_fall,shms_p,pi_pos_neg_ratio);
            g_shmsp_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
            i_fall++;
          }//for fall runs
          else if(HGC_eff_pos<0.2){
            g_pos_zhad_spring_HGCeff->SetPoint(i_spring,zhad,HGC_eff_pos);
            g_pos_zhad_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
            g_pos_shmsp_spring_HGCeff->SetPoint(i_spring,shms_p,HGC_eff_pos);
            g_pos_shmsp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
            g_neg_zhad_spring_HGCeff->SetPoint(i_spring,zhad,HGC_eff_neg);
            g_neg_zhad_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
            g_neg_shmsp_spring_HGCeff->SetPoint(i_spring,shms_p,HGC_eff_neg);
            g_neg_shmsp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
            g_pos_zhad_spring_Kpiratio->SetPoint(i_spring,zhad,R_Kpi_ratio_pos);
            g_pos_zhad_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
            g_pos_shmsp_spring_Kpiratio->SetPoint(i_spring,shms_p,R_Kpi_ratio_pos);
            g_pos_shmsp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
            g_neg_zhad_spring_Kpiratio->SetPoint(i_spring,zhad,R_Kpi_ratio_neg);
            g_neg_zhad_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
            g_neg_shmsp_spring_Kpiratio->SetPoint(i_spring,shms_p,R_Kpi_ratio_neg);
            g_neg_shmsp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
            g_zhad_spring_Kratio->SetPoint(i_spring,zhad,K_pos_neg_ratio);
            g_zhad_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
            g_shmsp_spring_Kratio->SetPoint(i_spring,shms_p,K_pos_neg_ratio);
            g_shmsp_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
            g_zhad_spring_piratio->SetPoint(i_spring,zhad,pi_pos_neg_ratio);
            g_zhad_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
            g_shmsp_spring_piratio->SetPoint(i_spring,shms_p,pi_pos_neg_ratio);
            g_shmsp_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
            i_spring++;
          }//for spring runs
        }//loop over points
      }//if greater than 3
      }//if not empty 
    }//loop over rungroups
    TCanvas *c_HGCeff_zhad = new TCanvas();
    g_pos_zhad_fall_HGCeff->SetMarkerStyle(8);
    g_pos_zhad_fall_HGCeff->SetMarkerColor(kRed);
    g_pos_zhad_fall_HGCeff->SetTitle("pos fall");
    g_pos_zhad_spring_HGCeff->SetMarkerStyle(4);
    g_pos_zhad_spring_HGCeff->SetMarkerColor(kRed);
    g_pos_zhad_spring_HGCeff->SetTitle("pos spring");
    g_neg_zhad_fall_HGCeff->SetMarkerStyle(8);
    g_neg_zhad_fall_HGCeff->SetMarkerColor(kBlack);
    g_neg_zhad_fall_HGCeff->SetTitle("neg fall");
    g_neg_zhad_spring_HGCeff->SetMarkerStyle(4);
    g_neg_zhad_spring_HGCeff->SetMarkerColor(kBlack);
    g_neg_zhad_spring_HGCeff->SetTitle("neg spring");
    TMultiGraph* mg_zhad_HGCeff = new TMultiGraph();
    mg_zhad_HGCeff->Add(g_pos_zhad_fall_HGCeff,"P");
    mg_zhad_HGCeff->Add(g_pos_zhad_spring_HGCeff,"P");
    mg_zhad_HGCeff->Add(g_neg_zhad_fall_HGCeff,"P");
    mg_zhad_HGCeff->Add(g_neg_zhad_spring_HGCeff,"P");
    mg_zhad_HGCeff->Draw("A");
    mg_zhad_HGCeff->GetXaxis()->SetTitle("zhad");
    mg_zhad_HGCeff->GetYaxis()->SetTitle("HGC_eff");
    mg_zhad_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
    c_HGCeff_zhad->BuildLegend();
    c_HGCeff_zhad->SaveAs("results/pid/HGC_pid/HGCeff_zhad.pdf");
    TCanvas *c_HGCeff_shmsp = new TCanvas();
    g_pos_shmsp_fall_HGCeff->SetMarkerStyle(8);
    g_pos_shmsp_fall_HGCeff->SetMarkerColor(kRed);
    g_pos_shmsp_fall_HGCeff->SetTitle("pos fall");
    g_pos_shmsp_spring_HGCeff->SetMarkerStyle(4);
    g_pos_shmsp_spring_HGCeff->SetMarkerColor(kRed);
    g_pos_shmsp_spring_HGCeff->SetTitle("pos spring");
    g_neg_shmsp_fall_HGCeff->SetMarkerStyle(8);
    g_neg_shmsp_fall_HGCeff->SetMarkerColor(kBlack);
    g_neg_shmsp_fall_HGCeff->SetTitle("neg fall");
    g_neg_shmsp_spring_HGCeff->SetMarkerStyle(4);
    g_neg_shmsp_spring_HGCeff->SetMarkerColor(kBlack);
    g_neg_shmsp_spring_HGCeff->SetTitle("neg spring");
    TMultiGraph* mg_shmsp_HGCeff = new TMultiGraph();
    mg_shmsp_HGCeff->Add(g_pos_shmsp_fall_HGCeff,"P");
    mg_shmsp_HGCeff->Add(g_pos_shmsp_spring_HGCeff,"P");
    mg_shmsp_HGCeff->Add(g_neg_shmsp_fall_HGCeff,"P");
    mg_shmsp_HGCeff->Add(g_neg_shmsp_spring_HGCeff,"P");
    mg_shmsp_HGCeff->Draw("A");
    mg_shmsp_HGCeff->GetXaxis()->SetTitle("shmsp");
    mg_shmsp_HGCeff->GetYaxis()->SetTitle("HGC_eff");
    mg_shmsp_HGCeff->GetXaxis()->SetRangeUser(2.8,4.5);
    mg_shmsp_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
    c_HGCeff_shmsp->BuildLegend();
    c_HGCeff_shmsp->SaveAs("results/pid/HGC_pid/HGCeff_shmsp.pdf");
    TCanvas *c_Kpiratio_zhad = new TCanvas();
    g_pos_zhad_fall_Kpiratio->SetMarkerStyle(8);
    g_pos_zhad_fall_Kpiratio->SetMarkerColor(kRed);
    g_pos_zhad_fall_Kpiratio->SetTitle("pos fall");
    g_pos_zhad_spring_Kpiratio->SetMarkerStyle(4);
    g_pos_zhad_spring_Kpiratio->SetMarkerColor(kRed);
    g_pos_zhad_spring_Kpiratio->SetTitle("pos spring");
    g_neg_zhad_fall_Kpiratio->SetMarkerStyle(8);
    g_neg_zhad_fall_Kpiratio->SetMarkerColor(kBlack);
    g_neg_zhad_fall_Kpiratio->SetTitle("neg fall");
    g_neg_zhad_spring_Kpiratio->SetMarkerStyle(4);
    g_neg_zhad_spring_Kpiratio->SetMarkerColor(kBlack);
    g_neg_zhad_spring_Kpiratio->SetTitle("neg spring");
    TMultiGraph* mg_zhad_pos_Kpiratio = new TMultiGraph();
    mg_zhad_pos_Kpiratio->Add(g_pos_zhad_fall_Kpiratio,"P");
    mg_zhad_pos_Kpiratio->Add(g_pos_zhad_spring_Kpiratio,"P");
    mg_zhad_pos_Kpiratio->Fit("pol1");
    gStyle->SetOptFit(0001);
    mg_zhad_pos_Kpiratio->Draw("same");
    TMultiGraph* mg_zhad_Kpiratio = new TMultiGraph();
    mg_zhad_Kpiratio->Add(g_pos_zhad_fall_Kpiratio,"P");
    mg_zhad_Kpiratio->Add(g_pos_zhad_spring_Kpiratio,"P");
    mg_zhad_Kpiratio->Fit("pol1");
    gStyle->SetOptFit(0001);
    //mg_zhad_Kpiratio->Draw("same");
    mg_zhad_Kpiratio->Add(g_neg_zhad_fall_Kpiratio,"P");
    mg_zhad_Kpiratio->Add(g_neg_zhad_spring_Kpiratio,"P");
    mg_zhad_Kpiratio->Draw("A");
    mg_zhad_Kpiratio->GetXaxis()->SetTitle("zhad");
    mg_zhad_Kpiratio->GetYaxis()->SetTitle("K/pi");
    mg_zhad_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
    c_Kpiratio_zhad->BuildLegend(0.15,0.65,0.35,0.9);
    c_Kpiratio_zhad->SaveAs("results/pid/HGC_pid/Kpiratio_zhad.pdf");
    TCanvas *c_Kpiratio_shmsp = new TCanvas();
    g_pos_shmsp_fall_Kpiratio->SetMarkerStyle(8);
    g_pos_shmsp_fall_Kpiratio->SetMarkerColor(kRed);
    g_pos_shmsp_fall_Kpiratio->SetTitle("pos fall");
    g_pos_shmsp_spring_Kpiratio->SetMarkerStyle(4);
    g_pos_shmsp_spring_Kpiratio->SetMarkerColor(kRed);
    g_pos_shmsp_spring_Kpiratio->SetTitle("pos spring");
    g_neg_shmsp_fall_Kpiratio->SetMarkerStyle(8);
    g_neg_shmsp_fall_Kpiratio->SetMarkerColor(kBlack);
    g_neg_shmsp_fall_Kpiratio->SetTitle("neg fall");
    g_neg_shmsp_spring_Kpiratio->SetMarkerStyle(4);
    g_neg_shmsp_spring_Kpiratio->SetMarkerColor(kBlack);
    g_neg_shmsp_spring_Kpiratio->SetTitle("neg spring");
    TMultiGraph* mg_shmsp_pos_Kpiratio = new TMultiGraph();
    mg_shmsp_pos_Kpiratio->Add(g_pos_shmsp_fall_Kpiratio,"P");
    mg_shmsp_pos_Kpiratio->Add(g_pos_shmsp_spring_Kpiratio,"P");
    mg_shmsp_pos_Kpiratio->Fit("pol1");
    gStyle->SetOptFit(0001);
    mg_shmsp_pos_Kpiratio->Draw("same");
    TMultiGraph* mg_shmsp_Kpiratio = new TMultiGraph();
    mg_shmsp_Kpiratio->Add(g_pos_shmsp_fall_Kpiratio,"P");
    mg_shmsp_Kpiratio->Add(g_pos_shmsp_spring_Kpiratio,"P");
    mg_shmsp_Kpiratio->Fit("pol1");
    gStyle->SetOptFit(0001);
    //TF1 *f1_Kpiratio =0wi 
    mg_shmsp_Kpiratio->Add(g_neg_shmsp_fall_Kpiratio,"P");
    mg_shmsp_Kpiratio->Add(g_neg_shmsp_spring_Kpiratio,"P");
    mg_shmsp_Kpiratio->Draw("A");
    mg_shmsp_Kpiratio->GetXaxis()->SetTitle("shmsp");
    mg_shmsp_Kpiratio->GetXaxis()->SetRangeUser(2.8,4.5);
    mg_shmsp_Kpiratio->GetYaxis()->SetTitle("K/pi");
    mg_shmsp_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
    c_Kpiratio_shmsp->BuildLegend(0.15,0.65,0.35,0.9);
    c_Kpiratio_shmsp->SaveAs("results/pid/HGC_pid/Kpiratio_shmsp.pdf");
    TCanvas *c_Kratio_zhad = new TCanvas();
    g_zhad_fall_Kratio->SetMarkerStyle(8);
    g_zhad_fall_Kratio->SetMarkerColor(kRed);
    g_zhad_fall_Kratio->SetTitle("fall");
    g_zhad_spring_Kratio->SetMarkerStyle(4);
    g_zhad_spring_Kratio->SetMarkerColor(kRed);
    g_zhad_spring_Kratio->SetTitle("spring");
    TMultiGraph* mg_zhad_Kratio = new TMultiGraph();
    mg_zhad_Kratio->Add(g_zhad_fall_Kratio,"P");
    mg_zhad_Kratio->Add(g_zhad_spring_Kratio,"P");
    mg_zhad_Kratio->Draw("A");
    mg_zhad_Kratio->GetXaxis()->SetTitle("zhad");
    mg_zhad_Kratio->GetYaxis()->SetTitle("K-/K+");
    //mg_zhad_Kratio->GetYaxis()->SetRangeUser(0,100);
    c_Kratio_zhad->BuildLegend();
    c_Kratio_zhad->SaveAs("results/pid/HGC_pid/Kratio_zhad.pdf");
    TCanvas *c_Kratio_shmsp = new TCanvas();
    g_shmsp_fall_Kratio->SetMarkerStyle(8);
    g_shmsp_fall_Kratio->SetMarkerColor(kRed);
    g_shmsp_fall_Kratio->SetTitle("fall");
    g_shmsp_spring_Kratio->SetMarkerStyle(4);
    g_shmsp_spring_Kratio->SetMarkerColor(kRed);
    g_shmsp_spring_Kratio->SetTitle("spring");
    TMultiGraph* mg_shmsp_Kratio = new TMultiGraph();
    mg_shmsp_Kratio->Add(g_shmsp_fall_Kratio,"P");
    mg_shmsp_Kratio->Add(g_shmsp_spring_Kratio,"P");
    mg_shmsp_Kratio->Draw("A");
    mg_shmsp_Kratio->GetXaxis()->SetTitle("shmsp");
    mg_shmsp_Kratio->GetYaxis()->SetTitle("K-/K+");
    //mg_shmsp_Kratio->GetYaxis()->SetRangeUser(0,1);
    c_Kratio_shmsp->BuildLegend();
    c_Kratio_shmsp->SaveAs("results/pid/HGC_pid/Kratio_shmsp.pdf");
    TCanvas *c_piratio_zhad = new TCanvas();
    g_zhad_fall_piratio->SetMarkerStyle(8);
    g_zhad_fall_piratio->SetMarkerColor(kRed);
    g_zhad_fall_piratio->SetTitle("fall");
    g_zhad_spring_piratio->SetMarkerStyle(4);
    g_zhad_spring_piratio->SetMarkerColor(kRed);
    g_zhad_spring_piratio->SetTitle("spring");
    TMultiGraph* mg_zhad_piratio = new TMultiGraph();
    mg_zhad_piratio->Add(g_zhad_fall_piratio,"P");
    mg_zhad_piratio->Add(g_zhad_spring_piratio,"P");
    mg_zhad_piratio->Draw("A");
    mg_zhad_piratio->GetXaxis()->SetTitle("zhad");
    mg_zhad_piratio->GetYaxis()->SetTitle("pi-/pi+");
    c_piratio_zhad->BuildLegend();
    c_piratio_zhad->SaveAs("results/pid/HGC_pid/piratio_zhad.pdf");
    TCanvas *c_piratio_shmsp = new TCanvas();
    g_shmsp_fall_piratio->SetMarkerStyle(8);
    g_shmsp_fall_piratio->SetMarkerColor(kRed);
    g_shmsp_fall_piratio->SetTitle("fall");
    g_shmsp_spring_piratio->SetMarkerStyle(4);
    g_shmsp_spring_piratio->SetMarkerColor(kRed);
    g_shmsp_spring_piratio->SetTitle("spring");
    TMultiGraph* mg_shmsp_piratio = new TMultiGraph();
    mg_shmsp_piratio->Add(g_shmsp_fall_piratio,"P");
    mg_shmsp_piratio->Add(g_shmsp_spring_piratio,"P");
    mg_shmsp_piratio->Draw("A");
    mg_shmsp_piratio->GetXaxis()->SetTitle("shmsp");
    mg_shmsp_piratio->GetYaxis()->SetTitle("pi-/pi+");
    mg_shmsp_piratio->GetYaxis()->SetRangeUser(0,1);
    c_piratio_shmsp->BuildLegend();
    c_piratio_shmsp->SaveAs("results/pid/HGC_pid/piratio_shmsp.pdf");
  }
