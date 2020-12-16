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

void SHMS_cuts_compare(int RunGroup = 0){
  if(RunGroup ==0){
    std::cout<<"Enter RunGroup (-1 to exit)";
    cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }

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
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(!neg_D2.empty() && !pos_D2.empty()){ 

    //for pos runs, I don't have to separate pos or neg here
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      //calorimeter runs part
      json j_DE;
      {
        std::string infilename = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
        std::cout<<"json file "<<infilename<<std::endl;
        std::ifstream infile(infilename.c_str());
        infile>>j_DE;
      }
      TGraph *g_cal_pi = new TGraph();
      g_cal_pi->SetTitle("calorimeter;calorimeter cut;efficiency");
      g_cal_pi->SetMarkerStyle(8);
      g_cal_pi->SetMarkerSize(0.75);
      g_cal_pi->SetMarkerColor(1);
      g_cal_pi->SetLineColor(1);
      g_cal_pi->SetFillColor(3);

      TGraph *g_aero_pi = new TGraph();
      g_aero_pi->SetTitle("aero;aerocut;efficiency");
      g_aero_pi->SetMarkerStyle(8);
      g_aero_pi->SetMarkerSize(0.75);
      g_aero_pi->SetMarkerColor(1);
      g_aero_pi->SetLineColor(1);
      g_aero_pi->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cal_pi;
      double cal_pi_all;
      cal_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
      cal_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_calcuts"].get<std::vector<double>>();
      int n_cal_cuts = (int)cal_pi.size();
      std::vector<double> cal_cut = j_cuts["SHMS"]["cal_cuts"].get<std::vector<double>>(); 
      for(int i = 0;i<n_cal_cuts;++i){
        double e_eff = cal_pi[i]/cal_pi_all;
        g_cal_pi->SetPoint(i,cal_cut[i],e_eff);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cal = new TCanvas();
      TPad *p1_cal = new TPad("p1_cal","",0,0,1,1);
      p1_cal->SetGrid();

      p1_cal->Draw();
      p1_cal->cd();
      g_cal_pi->Draw("ALP");
      g_cal_pi->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cal_pi->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetLabelSize();


      TLegend *leg_cal = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal->SetFillColor(gPad->GetFillColor());
      leg_cal->SetTextFont(lfont_cal);
      leg_cal->SetTextSize(lsize_cal);
      std::string cal_title = "SHMS calorimter for run "+std::to_string(RunNumber)+" runs";
      leg_cal->SetHeader(cal_title.c_str());
      leg_cal->Draw();
      gPad->Update();

      std::string cal_name = "results/pid/SHMS_cal_"+std::to_string(RunNumber)+".pdf";
      c_cal->SaveAs(cal_name.c_str());


      std::vector<double> aero_pi;
      double aero_pi_all;
      aero_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_all"].get<double>();
      aero_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_aerocuts"].get<std::vector<double>>();
      int n_aero_cuts = (int)aero_pi.size();
      std::vector<double> aero_cut = j_cuts["SHMS"]["aero_cuts"].get<std::vector<double>>(); 
      for(int i = 0;i<n_aero_cuts;++i){
        double e_eff = aero_pi[i]/aero_pi_all;
        g_aero_pi->SetPoint(i,aero_cut[i],e_eff);
        std::cout<<e_eff<<std::endl;
      }

      gStyle->Reset("Modern");

      TCanvas *c_aero = new TCanvas();
      TPad *p1_aero = new TPad("p1_aero","",0,0,1,1);
      p1_aero->SetGrid();

      p1_aero->Draw();
      p1_aero->cd();
      g_aero_pi->Draw("ALP");
      g_aero_pi->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_aero_pi->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetLabelSize();


      TLegend *leg_aero = new TLegend(0.30,0.85,0.80,0.90);
      leg_aero->SetFillColor(gPad->GetFillColor());
      leg_aero->SetTextFont(lfont_aero);
      leg_aero->SetTextSize(lsize_aero);
      std::string aero_title = "SHMS aero for run "+std::to_string(RunNumber)+" runs";
      leg_aero->SetHeader(aero_title.c_str());
      leg_aero->Draw();
      gPad->Update();

      std::string aero_name = "results/pid/SHMS_aero_"+std::to_string(RunNumber)+".pdf";
      c_aero->SaveAs(aero_name.c_str());

    }//for pos runs
  

    //for neg runs, I don't have to separate neg or neg here
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      //calorimeter runs part
      json j_DE;
      {
        std::string infilename = "results/pid/SHMS_DE_"+std::to_string(RunNumber)+".json";
        std::cout<<"json file "<<infilename<<std::endl;
        std::ifstream infile(infilename.c_str());
        infile>>j_DE;
      }
      TGraph *g_cal_pi = new TGraph();
      g_cal_pi->SetTitle("calorimeter;calorimeter cut;efficiency");
      g_cal_pi->SetMarkerStyle(8);
      g_cal_pi->SetMarkerSize(0.75);
      g_cal_pi->SetMarkerColor(1);
      g_cal_pi->SetLineColor(1);
      g_cal_pi->SetFillColor(3);

      TGraph *g_aero_pi = new TGraph();
      g_aero_pi->SetTitle("aero;aerocut;efficiency");
      g_aero_pi->SetMarkerStyle(8);
      g_aero_pi->SetMarkerSize(0.75);
      g_aero_pi->SetMarkerColor(1);
      g_aero_pi->SetLineColor(1);
      g_aero_pi->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cal_pi;
      double cal_pi_all;
      cal_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_all"].get<double>();
      cal_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcut"]["pi_calcuts"].get<std::vector<double>>();
      int n_cal_cuts = (int)cal_pi.size();
      std::vector<double> cal_cut = j_cuts["SHMS"]["cal_cuts"].get<std::vector<double>>(); 
      for(int i = 0;i<n_cal_cuts;++i){
        double e_eff = cal_pi[i]/cal_pi_all;
        g_cal_pi->SetPoint(i,cal_cut[i],e_eff);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cal = new TCanvas();
      TPad *p1_cal = new TPad("p1_cal","",0,0,1,1);
      p1_cal->SetGrid();

      p1_cal->Draw();
      p1_cal->cd();
      g_cal_pi->Draw("ALP");
      g_cal_pi->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cal_pi->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal = g_cal_pi->GetHistogram()->GetYaxis()->GetLabelSize();


      TLegend *leg_cal = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal->SetFillColor(gPad->GetFillColor());
      leg_cal->SetTextFont(lfont_cal);
      leg_cal->SetTextSize(lsize_cal);
      std::string cal_title = "SHMS calorimter for run "+std::to_string(RunNumber)+" runs";
      leg_cal->SetHeader(cal_title.c_str());
      leg_cal->Draw();
      gPad->Update();

      std::string cal_name = "results/pid/SHMS_cal_"+std::to_string(RunNumber)+".pdf";
      c_cal->SaveAs(cal_name.c_str());


      std::vector<double> aero_pi;
      double aero_pi_all;
      aero_pi_all = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_all"].get<double>();
      aero_pi = j_DE[(std::to_string(RunNumber)).c_str()]["SHMS"]["rfcutcalcut"]["pi_aerocuts"].get<std::vector<double>>();
      int n_aero_cuts = (int)aero_pi.size();
      std::vector<double> aero_cut = j_cuts["SHMS"]["aero_cuts"].get<std::vector<double>>(); 
      for(int i = 0;i<n_aero_cuts;++i){
        double e_eff = aero_pi[i]/aero_pi_all;
        g_aero_pi->SetPoint(i,aero_cut[i],e_eff);
      }

      gStyle->Reset("Modern");

      TCanvas *c_aero = new TCanvas();
      TPad *p1_aero = new TPad("p1_aero","",0,0,1,1);
      p1_aero->SetGrid();

      p1_aero->Draw();
      p1_aero->cd();
      g_aero_pi->Draw("ALP");
      g_aero_pi->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_aero_pi->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_aero = g_aero_pi->GetHistogram()->GetYaxis()->GetLabelSize();


      TLegend *leg_aero = new TLegend(0.30,0.85,0.80,0.90);
      leg_aero->SetFillColor(gPad->GetFillColor());
      leg_aero->SetTextFont(lfont_aero);
      leg_aero->SetTextSize(lsize_aero);
      std::string aero_title = "SHMS aero for run "+std::to_string(RunNumber)+" runs";
      leg_aero->SetHeader(aero_title.c_str());
      leg_aero->Draw();
      gPad->Update();

      std::string aero_name = "results/pid/SHMS_aero_"+std::to_string(RunNumber)+".pdf";
      c_aero->SaveAs(aero_name.c_str());

    }//for neg runs

  }//if normal production runs
}
