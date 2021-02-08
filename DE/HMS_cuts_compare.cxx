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

void HMS_cuts_compare(int RunGroup = 0){
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
        std::string infilename = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
        std::cout<<"json file "<<infilename<<std::endl;
        std::ifstream infile(infilename.c_str());
        infile>>j_DE;
      }
      TGraph *g_cal_e = new TGraph();
      g_cal_e->SetTitle("calorimeter;calorimeter cut;efficiency");
      g_cal_e->SetMarkerStyle(8);
      g_cal_e->SetMarkerSize(0.75);
      g_cal_e->SetMarkerColor(1);
      g_cal_e->SetLineColor(1);
      g_cal_e->SetFillColor(3);

      TGraph *g_cal_pion = new TGraph();
      g_cal_pion->SetMarkerStyle(8);
      g_cal_pion->SetMarkerSize(0.75);
      g_cal_pion->SetMarkerColor(2);
      g_cal_pion->SetLineColor(2);
      g_cal_pion->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cal_e,cal_pion;
      double cal_e_all,cal_pion_all;
      cal_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
      cal_pion_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["pi_all"].get<double>();
      cal_e = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
      cal_pion = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["pi_calcuts"].get<std::vector<double>>();
      int n_cal_cuts = (int)cal_e.size();
      std::vector<double> cal_cut = j_cuts["HMS"]["cal_cuts"].get<std::vector<double>>(); 
      //std::vector<double> cal_cut = {0.6,0.65,0.7,0.75,0.8,0.85};
      for(int i = 0;i<n_cal_cuts;++i){
        double e_eff = cal_e[i]/cal_e_all;
        g_cal_e->SetPoint(i,cal_cut[i],e_eff);
        double pion_rej = cal_pion_all/cal_pion[i];
        g_cal_pion->SetPoint(i,cal_cut[i],pion_rej);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cal = new TCanvas();
      TPad *p1_cal = new TPad("p1_cal","",0,0,1,1);
      p1_cal->SetGrid();
      TPad *p2_cal = new TPad("p2_cal","",0,0,1,1);
      p2_cal->SetFillStyle(4000);

      p1_cal->Draw();
      p1_cal->cd();
      g_cal_e->Draw("ALP");
      g_cal_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cal_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cal = g_cal_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal = g_cal_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal = g_cal_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal = g_cal_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cal = p1_cal->GetUxmin();
      Double_t xmax_cal = p1_cal->GetUxmax();
      Double_t dx_cal = (xmax_cal-xmin_cal)/0.8;
      Double_t ymin_cal = g_cal_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cal = g_cal_pion->GetHistogram()->GetMaximum();
      Double_t dy_cal =(ymax_cal - ymin_cal)/0.8;
      p2_cal->Range(xmin_cal - 0.1*dx_cal, ymin_cal - 0.1*dy_cal, xmax_cal + 0.1*dx_cal,ymax_cal + 0.1*dy_cal);
      p2_cal->Draw();
      p2_cal->cd();
      g_cal_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cal = new TGaxis(xmax_cal, ymin_cal, xmax_cal, ymax_cal, ymin_cal, ymax_cal, 510, "+L");
      axis_cal->SetTitle("pion rejection ratio");
      axis_cal->SetTitleFont(tfont_cal);
      axis_cal->SetTitleSize(tsize_cal);
      axis_cal->SetTitleColor(kRed);
      axis_cal->SetTitleOffset(1.25);
      axis_cal->SetLabelFont(lfont_cal);
      axis_cal->SetLabelSize(lsize_cal);
      axis_cal->SetLabelColor(kRed);
      axis_cal->SetLineColor(kRed);
      axis_cal->Draw();
      gPad->Update();

      TLegend *leg_cal = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal->SetFillColor(gPad->GetFillColor());
      leg_cal->SetTextFont(lfont_cal);
      leg_cal->SetTextSize(lsize_cal);
      std::string cal_title = "HMS calorimter for run "+std::to_string(RunNumber)+" runs";
      leg_cal->SetHeader(cal_title.c_str());
      leg_cal->Draw();
      gPad->Update();

      std::string cal_name = "results/pid/HMS_cal_"+std::to_string(RunNumber)+".pdf";
      c_cal->SaveAs(cal_name.c_str());


      //cer pos runs 
      TGraph *g_cer_e = new TGraph();
      g_cer_e->SetTitle("cerenkov;cerenkov cut;efficiency");
      g_cer_e->SetMarkerStyle(8);
      g_cer_e->SetMarkerSize(0.75);
      g_cer_e->SetMarkerColor(1);
      g_cer_e->SetLineColor(1);
      g_cer_e->SetFillColor(3);

      TGraph *g_cer_pion = new TGraph();
      g_cer_pion->SetMarkerStyle(8);
      g_cer_pion->SetMarkerSize(0.75);
      g_cer_pion->SetMarkerColor(2);
      g_cer_pion->SetLineColor(2);
      g_cer_pion->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cer_e,cer_pion;
      double cer_e_all,cer_pion_all;
      cer_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_all"].get<double>();
      cer_pion_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["pi_all"].get<double>();
      cer_e = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_cercuts"].get<std::vector<double>>();
      cer_pion = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["pi_cercuts"].get<std::vector<double>>();
      int n_cer_cuts = (int)cer_e.size();
      std::vector<double> cer_cut = j_cuts["HMS"]["cer_cuts"].get<std::vector<double>>();
      //std::vector<double> cer_cut = {5,6,7,8,9,10};
      for(int i = 0;i<n_cer_cuts;++i){
        double e_eff = cer_e[i]/cer_e_all;
        g_cer_e->SetPoint(i,cer_cut[i],e_eff);
        double pion_rej = cer_pion_all/cer_pion[i];
        g_cer_pion->SetPoint(i,cer_cut[i],pion_rej);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cer = new TCanvas();
      TPad *p1_cer = new TPad("p1_cer","",0,0,1,1);
      p1_cer->SetGrid();
      TPad *p2_cer = new TPad("p2_cer","",0,0,1,1);
      p2_cer->SetFillStyle(4000);

      p1_cer->Draw();
      p1_cer->cd();
      g_cer_e->Draw("ALP");
      g_cer_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cer_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cer = g_cer_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cer = g_cer_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cer = g_cer_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cer = g_cer_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cer = p1_cer->GetUxmin();
      Double_t xmax_cer = p1_cer->GetUxmax();
      Double_t dx_cer = (xmax_cer-xmin_cer)/0.8;
      Double_t ymin_cer = g_cer_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cer = g_cer_pion->GetHistogram()->GetMaximum();
      Double_t dy_cer =(ymax_cer - ymin_cer)/0.8;
      p2_cer->Range(xmin_cer - 0.1*dx_cer, ymin_cer - 0.1*dy_cer, xmax_cer + 0.1*dx_cer,ymax_cer + 0.1*dy_cer);
      p2_cer->Draw();
      p2_cer->cd();
      g_cer_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cer = new TGaxis(xmax_cer, ymin_cer, xmax_cer, ymax_cer, ymin_cer, ymax_cer, 510, "+L");
      axis_cer->SetTitle("pion rejection ratio");
      axis_cer->SetTitleFont(tfont_cer);
      axis_cer->SetTitleSize(tsize_cer);
      axis_cer->SetTitleColor(kRed);
      axis_cer->SetTitleOffset(1.25);
      axis_cer->SetLabelFont(lfont_cer);
      axis_cer->SetLabelSize(lsize_cer);
      axis_cer->SetLabelColor(kRed);
      axis_cer->SetLineColor(kRed);
      axis_cer->Draw();
      gPad->Update();

      TLegend *leg_cer = new TLegend(0.30,0.85,0.80,0.90);
      leg_cer->SetFillColor(gPad->GetFillColor());
      leg_cer->SetTextFont(lfont_cer);
      leg_cer->SetTextSize(lsize_cer);
      std::string cer_title = "HMS cer for run "+std::to_string(RunNumber)+" runs";
      leg_cer->SetHeader(cer_title.c_str());
      leg_cer->Draw();
      gPad->Update();

      std::string cer_name = "results/pid/HMS_cer_"+std::to_string(RunNumber)+".pdf";
      c_cer->SaveAs(cer_name.c_str());
    }//for pos runs

    //for neg runs, I don't have to separate neg or neg here
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      //calorimeter runs part
      int RunNumber = *it;
      json j_DE;
      {
        std::string infilename = "results/pid/HMS_DE_"+std::to_string(RunNumber)+".json";
        std::cout<<"json file "<<infilename<<std::endl;
        std::ifstream infile(infilename.c_str());
        infile>>j_DE;
      }
      TGraph *g_cal_e = new TGraph();
      g_cal_e->SetTitle("calorimeter;calorimeter cut;efficiency");
      g_cal_e->SetMarkerStyle(8);
      g_cal_e->SetMarkerSize(0.75);
      g_cal_e->SetMarkerColor(1);
      g_cal_e->SetLineColor(1);
      g_cal_e->SetFillColor(3);

      TGraph *g_cal_pion = new TGraph();
      g_cal_pion->SetMarkerStyle(8);
      g_cal_pion->SetMarkerSize(0.75);
      g_cal_pion->SetMarkerColor(2);
      g_cal_pion->SetLineColor(2);
      g_cal_pion->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cal_e,cal_pion;
      double cal_e_all,cal_pion_all;
      cal_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_all"].get<double>();
      cal_pion_all = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["pi_all"].get<double>();
      cal_e = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["e_calcuts"].get<std::vector<double>>();
      cal_pion = j_DE[(std::to_string(RunNumber)).c_str()]["cercut"]["pi_calcuts"].get<std::vector<double>>();
      int n_cal_cuts = (int)cal_e.size();
      std::vector<double> cal_cut = j_cuts["HMS"]["cal_cuts"].get<std::vector<double>>(); 
      //std::vector<double> cal_cut = {0.6,0.65,0.7,0.75,0.8,0.85};
      for(int i = 0;i<n_cal_cuts;++i){
        double e_eff = cal_e[i]/cal_e_all;
        g_cal_e->SetPoint(i,cal_cut[i],e_eff);
        double pion_rej = cal_pion_all/cal_pion[i];
        g_cal_pion->SetPoint(i,cal_cut[i],pion_rej);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cal = new TCanvas();
      TPad *p1_cal = new TPad("p1_cal","",0,0,1,1);
      p1_cal->SetGrid();
      TPad *p2_cal = new TPad("p2_cal","",0,0,1,1);
      p2_cal->SetFillStyle(4000);

      p1_cal->Draw();
      p1_cal->cd();
      g_cal_e->Draw("ALP");
      g_cal_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cal_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cal = g_cal_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal = g_cal_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal = g_cal_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal = g_cal_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cal = p1_cal->GetUxmin();
      Double_t xmax_cal = p1_cal->GetUxmax();
      Double_t dx_cal = (xmax_cal-xmin_cal)/0.8;
      Double_t ymin_cal = g_cal_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cal = g_cal_pion->GetHistogram()->GetMaximum();
      Double_t dy_cal =(ymax_cal - ymin_cal)/0.8;
      p2_cal->Range(xmin_cal - 0.1*dx_cal, ymin_cal - 0.1*dy_cal, xmax_cal + 0.1*dx_cal,ymax_cal + 0.1*dy_cal);
      p2_cal->Draw();
      p2_cal->cd();
      g_cal_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cal = new TGaxis(xmax_cal, ymin_cal, xmax_cal, ymax_cal, ymin_cal, ymax_cal, 510, "+L");
      axis_cal->SetTitle("pion rejection ratio");
      axis_cal->SetTitleFont(tfont_cal);
      axis_cal->SetTitleSize(tsize_cal);
      axis_cal->SetTitleColor(kRed);
      axis_cal->SetTitleOffset(1.25);
      axis_cal->SetLabelFont(lfont_cal);
      axis_cal->SetLabelSize(lsize_cal);
      axis_cal->SetLabelColor(kRed);
      axis_cal->SetLineColor(kRed);
      axis_cal->Draw();
      gPad->Update();

      TLegend *leg_cal = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal->SetFillColor(gPad->GetFillColor());
      leg_cal->SetTextFont(lfont_cal);
      leg_cal->SetTextSize(lsize_cal);
      std::string cal_title = "HMS calorimter for run "+std::to_string(RunNumber)+" runs";
      leg_cal->SetHeader(cal_title.c_str());
      leg_cal->Draw();
      gPad->Update();

      std::string cal_name = "results/pid/HMS_cal_"+std::to_string(RunNumber)+".pdf";
      c_cal->SaveAs(cal_name.c_str());


      //cer neg runs 
      TGraph *g_cer_e = new TGraph();
      g_cer_e->SetTitle("cerenkov;cerenkov cut;efficiency");
      g_cer_e->SetMarkerStyle(8);
      g_cer_e->SetMarkerSize(0.75);
      g_cer_e->SetMarkerColor(1);
      g_cer_e->SetLineColor(1);
      g_cer_e->SetFillColor(3);

      TGraph *g_cer_pion = new TGraph();
      g_cer_pion->SetMarkerStyle(8);
      g_cer_pion->SetMarkerSize(0.75);
      g_cer_pion->SetMarkerColor(2);
      g_cer_pion->SetLineColor(2);
      g_cer_pion->SetFillColor(3);

      std::cout<<"check"<<std::endl;
      std::vector<double> cer_e,cer_pion;
      double cer_e_all,cer_pion_all;
      cer_e_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_all"].get<double>();
      cer_pion_all = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["pi_all"].get<double>();
      cer_e = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["e_cercuts"].get<std::vector<double>>();
      cer_pion = j_DE[(std::to_string(RunNumber)).c_str()]["calcut"]["pi_cercuts"].get<std::vector<double>>();
      int n_cer_cuts = (int)cer_e.size();
      std::vector<double> cer_cut = j_cuts["HMS"]["cer_cuts"].get<std::vector<double>>();
      //std::vector<double> cer_cut = {5,6,7,8,9,10};
      for(int i = 0;i<n_cer_cuts;++i){
        double e_eff = cer_e[i]/cer_e_all;
        g_cer_e->SetPoint(i,cer_cut[i],e_eff);
        double pion_rej = cer_pion_all/cer_pion[i];
        g_cer_pion->SetPoint(i,cer_cut[i],pion_rej);
      }

      gStyle->Reset("Modern");

      TCanvas *c_cer = new TCanvas();
      TPad *p1_cer = new TPad("p1_cer","",0,0,1,1);
      p1_cer->SetGrid();
      TPad *p2_cer = new TPad("p2_cer","",0,0,1,1);
      p2_cer->SetFillStyle(4000);

      p1_cer->Draw();
      p1_cer->cd();
      g_cer_e->Draw("ALP");
      g_cer_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_cer_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      gPad->Update();

      Style_t tfont_cer = g_cer_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cer = g_cer_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cer = g_cer_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cer = g_cer_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cer = p1_cer->GetUxmin();
      Double_t xmax_cer = p1_cer->GetUxmax();
      Double_t dx_cer = (xmax_cer-xmin_cer)/0.8;
      Double_t ymin_cer = g_cer_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cer = g_cer_pion->GetHistogram()->GetMaximum();
      Double_t dy_cer =(ymax_cer - ymin_cer)/0.8;
      p2_cer->Range(xmin_cer - 0.1*dx_cer, ymin_cer - 0.1*dy_cer, xmax_cer + 0.1*dx_cer,ymax_cer + 0.1*dy_cer);
      p2_cer->Draw();
      p2_cer->cd();
      g_cer_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cer = new TGaxis(xmax_cer, ymin_cer, xmax_cer, ymax_cer, ymin_cer, ymax_cer, 510, "+L");
      axis_cer->SetTitle("pion rejection ratio");
      axis_cer->SetTitleFont(tfont_cer);
      axis_cer->SetTitleSize(tsize_cer);
      axis_cer->SetTitleColor(kRed);
      axis_cer->SetTitleOffset(1.25);
      axis_cer->SetLabelFont(lfont_cer);
      axis_cer->SetLabelSize(lsize_cer);
      axis_cer->SetLabelColor(kRed);
      axis_cer->SetLineColor(kRed);
      axis_cer->Draw();
      gPad->Update();

      TLegend *leg_cer = new TLegend(0.30,0.85,0.80,0.90);
      leg_cer->SetFillColor(gPad->GetFillColor());
      leg_cer->SetTextFont(lfont_cer);
      leg_cer->SetTextSize(lsize_cer);
      std::string cer_title = "HMS cer for run "+std::to_string(RunNumber)+" runs";
      leg_cer->SetHeader(cer_title.c_str());
      leg_cer->Draw();
      gPad->Update();

      std::string cer_name = "results/pid/HMS_cer_"+std::to_string(RunNumber)+".pdf";
      c_cer->SaveAs(cer_name.c_str());
    }//for neg runs
  }//if normal production runs
}
