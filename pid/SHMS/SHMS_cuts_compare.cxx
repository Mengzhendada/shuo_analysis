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

  RunGroup = RunGroup;
 // json j_rungroup;
 // {
 //   std::ifstream infile("db2/ratio_run_group_updated.json");
 //   infile>>j_rungroup;
 // }
  json j_cal;
  {
    std::string infilename = "results/pid/SHMS_cal_cuts_"+std::to_string(RunGroup)+".json";
    std::cout<<"json file "<<infilename<<std::endl;
    std::ifstream infile(infilename.c_str());
    infile>>j_cal;
  }
  std::cout<<"check"<<std::endl;
  json j_cer;
  {
    std::string infilename = "results/pid/SHMS_cer_cuts_"+std::to_string(RunGroup)+".json";
    std::cout<<"json file "<<infilename<<std::endl;
    std::ifstream infile(infilename.c_str());
    infile>>j_cer;
  }
  json j_cuts;
  {
    std::string infilename = "db2/PID_test.json";
    std::ifstream infile(infilename.c_str());
    infile>>j_cuts;
  }
  
  //calorimeter neg runs part
  TGraph *g_cal_neg_e = new TGraph();
  g_cal_neg_e->SetTitle("calorimeter;calorimeter cut;efficiency");
  g_cal_neg_e->SetMarkerStyle(8);
  g_cal_neg_e->SetMarkerSize(0.75);
  g_cal_neg_e->SetMarkerColor(1);
  g_cal_neg_e->SetLineColor(1);
  g_cal_neg_e->SetFillColor(3);

  TGraph *g_cal_neg_pion = new TGraph();
  g_cal_neg_pion->SetMarkerStyle(8);
  g_cal_neg_pion->SetMarkerSize(0.75);
  g_cal_neg_pion->SetMarkerColor(2);
  g_cal_neg_pion->SetLineColor(2);
  g_cal_neg_pion->SetFillColor(3);

  std::cout<<"check"<<std::endl;
  std::vector<double> cal_neg_e,cal_neg_pion;
  double cal_neg_e_all,cal_neg_pion_all;
  cal_neg_e_all = j_cal[(std::to_string(RunGroup)).c_str()]["pos_c"]["e_all"].get<double>();
  cal_neg_pion_all = j_cal[(std::to_string(RunGroup)).c_str()]["pos_c"]["pi_all"].get<double>();
  cal_neg_e = j_cal[(std::to_string(RunGroup)).c_str()]["pos_c"]["e"].get<std::vector<double>>();
  cal_neg_pion = j_cal[(std::to_string(RunGroup)).c_str()]["pos_c"]["pi"].get<std::vector<double>>();
  int n_cal_cuts = (int)cal_neg_e.size();
  std::vector<double> cal_cut = j_cuts["SHMS"]["cal_cuts"].get<std::vector<double>>(); 
  //std::vector<double> cal_cut = {0.6,0.65,0.7,0.75,0.8,0.85};
  for(int i = 0;i<n_cal_cuts;++i){
    double e_eff = cal_neg_e[i]/cal_neg_e_all;
    g_cal_neg_e->SetPoint(i,cal_cut[i],e_eff);
    double pion_rej = cal_neg_pion_all/cal_neg_pion[i];
    g_cal_neg_pion->SetPoint(i,cal_cut[i],pion_rej);
  }

  gStyle->Reset("Modern");
  
  TCanvas *c_cal_neg = new TCanvas();
  TPad *p1_cal_neg = new TPad("p1_cal_neg","",0,0,1,1);
  p1_cal_neg->SetGrid();
  TPad *p2_cal_neg = new TPad("p2_cal_neg","",0,0,1,1);
  p2_cal_neg->SetFillStyle(4000);

  p1_cal_neg->Draw();
  p1_cal_neg->cd();
  g_cal_neg_e->Draw("ALP");
  g_cal_neg_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
  g_cal_neg_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
  gPad->Update();

  Style_t tfont_cal_neg = g_cal_neg_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal_neg = g_cal_neg_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal_neg = g_cal_neg_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal_neg = g_cal_neg_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cal_neg = p1_cal_neg->GetUxmin();
      Double_t xmax_cal_neg = p1_cal_neg->GetUxmax();
      Double_t dx_cal_neg = (xmax_cal_neg-xmin_cal_neg)/0.8;
      Double_t ymin_cal_neg = g_cal_neg_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cal_neg = g_cal_neg_pion->GetHistogram()->GetMaximum();
      Double_t dy_cal_neg =(ymax_cal_neg - ymin_cal_neg)/0.8;
      p2_cal_neg->Range(xmin_cal_neg - 0.1*dx_cal_neg, ymin_cal_neg - 0.1*dy_cal_neg, xmax_cal_neg + 0.1*dx_cal_neg,ymax_cal_neg + 0.1*dy_cal_neg);
      p2_cal_neg->Draw();
      p2_cal_neg->cd();
      g_cal_neg_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cal_neg = new TGaxis(xmax_cal_neg, ymin_cal_neg, xmax_cal_neg, ymax_cal_neg, ymin_cal_neg, ymax_cal_neg, 510, "+L");
      axis_cal_neg->SetTitle("pion rejection ratio");
      axis_cal_neg->SetTitleFont(tfont_cal_neg);
      axis_cal_neg->SetTitleSize(tsize_cal_neg);
      axis_cal_neg->SetTitleColor(kRed);
      axis_cal_neg->SetTitleOffset(1.25);
      axis_cal_neg->SetLabelFont(lfont_cal_neg);
      axis_cal_neg->SetLabelSize(lsize_cal_neg);
      axis_cal_neg->SetLabelColor(kRed);
      axis_cal_neg->SetLineColor(kRed);
      axis_cal_neg->Draw();
      gPad->Update();
      
      TLegend *leg_cal_neg = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal_neg->SetFillColor(gPad->GetFillColor());
      leg_cal_neg->SetTextFont(lfont_cal_neg);
      leg_cal_neg->SetTextSize(lsize_cal_neg);
      std::string cal_neg_title = "SHMS calorimter for run group "+std::to_string(RunGroup)+" neg runs";
      leg_cal_neg->SetHeader(cal_neg_title.c_str());
      leg_cal_neg->Draw();
      gPad->Update();

      std::string cal_neg_name = "results/pid/SHMS_cal_neg_"+std::to_string(RunGroup)+".pdf";
      c_cal_neg->SaveAs(cal_neg_name.c_str());

      //calorimeter pos runs part 
  TGraph *g_cal_pos_e = new TGraph();
  g_cal_pos_e->SetTitle("calorimeter;calorimeter cut;efficiency");
  g_cal_pos_e->SetMarkerStyle(8);
  g_cal_pos_e->SetMarkerSize(0.75);
  g_cal_pos_e->SetMarkerColor(1);
  g_cal_pos_e->SetLineColor(1);
  g_cal_pos_e->SetFillColor(3);

  TGraph *g_cal_pos_pion = new TGraph();
  g_cal_pos_pion->SetMarkerStyle(8);
  g_cal_pos_pion->SetMarkerSize(0.75);
  g_cal_pos_pion->SetMarkerColor(2);
  g_cal_pos_pion->SetLineColor(2);
  g_cal_pos_pion->SetFillColor(3);

  std::cout<<"check"<<std::endl;
  std::vector<double> cal_pos_e,cal_pos_pion;
  double cal_pos_e_all,cal_pos_pion_all;
  cal_pos_e_all = j_cal[(std::to_string(RunGroup)).c_str()]["neg_c"]["e_all"].get<double>();
  cal_pos_pion_all = j_cal[(std::to_string(RunGroup)).c_str()]["neg_c"]["pi_all"].get<double>();
  cal_pos_e = j_cal[(std::to_string(RunGroup)).c_str()]["neg_c"]["e"].get<std::vector<double>>();
  cal_pos_pion = j_cal[(std::to_string(RunGroup)).c_str()]["neg_c"]["pi"].get<std::vector<double>>();
  for(int i = 0;i<n_cal_cuts;++i){
    double e_eff = cal_pos_e[i]/cal_pos_e_all;
    g_cal_pos_e->SetPoint(i,cal_cut[i],e_eff);
    double pion_rej = cal_pos_pion_all/cal_pos_pion[i];
    g_cal_pos_pion->SetPoint(i,cal_cut[i],pion_rej);
  }

  gStyle->Reset("Modern");
  
  TCanvas *c_cal_pos = new TCanvas();
  TPad *p1_cal_pos = new TPad("p1_cal_pos","",0,0,1,1);
  p1_cal_pos->SetGrid();
  TPad *p2_cal_pos = new TPad("p2_cal_pos","",0,0,1,1);
  p2_cal_pos->SetFillStyle(4000);

  p1_cal_pos->Draw();
  p1_cal_pos->cd();
  g_cal_pos_e->Draw("ALP");
  g_cal_pos_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
  g_cal_pos_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
  gPad->Update();

  Style_t tfont_cal_pos = g_cal_pos_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cal_pos = g_cal_pos_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cal_pos = g_cal_pos_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cal_pos = g_cal_pos_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cal_pos = p1_cal_pos->GetUxmin();
      Double_t xmax_cal_pos = p1_cal_pos->GetUxmax();
      Double_t dx_cal_pos = (xmax_cal_pos-xmin_cal_pos)/0.8;
      Double_t ymin_cal_pos = g_cal_pos_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cal_pos = g_cal_pos_pion->GetHistogram()->GetMaximum();
      Double_t dy_cal_pos =(ymax_cal_pos - ymin_cal_pos)/0.8;
      p2_cal_pos->Range(xmin_cal_pos - 0.1*dx_cal_pos, ymin_cal_pos - 0.1*dy_cal_pos, xmax_cal_pos + 0.1*dx_cal_pos,ymax_cal_pos + 0.1*dy_cal_pos);
      p2_cal_pos->Draw();
      p2_cal_pos->cd();
      g_cal_pos_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cal_pos = new TGaxis(xmax_cal_pos, ymin_cal_pos, xmax_cal_pos, ymax_cal_pos, ymin_cal_pos, ymax_cal_pos, 510, "+L");
      axis_cal_pos->SetTitle("pion rejection ratio");
      axis_cal_pos->SetTitleFont(tfont_cal_pos);
      axis_cal_pos->SetTitleSize(tsize_cal_pos);
      axis_cal_pos->SetTitleColor(kRed);
      axis_cal_pos->SetTitleOffset(1.25);
      axis_cal_pos->SetLabelFont(lfont_cal_pos);
      axis_cal_pos->SetLabelSize(lsize_cal_pos);
      axis_cal_pos->SetLabelColor(kRed);
      axis_cal_pos->SetLineColor(kRed);
      axis_cal_pos->Draw();
      gPad->Update();
      
      TLegend *leg_cal_pos = new TLegend(0.30,0.85,0.80,0.90);
      leg_cal_pos->SetFillColor(gPad->GetFillColor());
      leg_cal_pos->SetTextFont(lfont_cal_pos);
      leg_cal_pos->SetTextSize(lsize_cal_pos);
      std::string cal_pos_title = "SHMS calorimter for run group "+std::to_string(RunGroup)+" pos runs";
      leg_cal_pos->SetHeader(cal_pos_title.c_str());
      leg_cal_pos->Draw();
      gPad->Update();
      
      std::string cal_pos_name = "results/pid/SHMS_cal_pos_"+std::to_string(RunGroup)+".pdf";
      c_cal_pos->SaveAs(cal_pos_name.c_str());

      //cer neg runs 
  TGraph *g_cer_neg_e = new TGraph();
  g_cer_neg_e->SetTitle("cerenkov;cerenkov cut;efficiency");
  g_cer_neg_e->SetMarkerStyle(8);
  g_cer_neg_e->SetMarkerSize(0.75);
  g_cer_neg_e->SetMarkerColor(1);
  g_cer_neg_e->SetLineColor(1);
  g_cer_neg_e->SetFillColor(3);

  TGraph *g_cer_neg_pion = new TGraph();
  g_cer_neg_pion->SetMarkerStyle(8);
  g_cer_neg_pion->SetMarkerSize(0.75);
  g_cer_neg_pion->SetMarkerColor(2);
  g_cer_neg_pion->SetLineColor(2);
  g_cer_neg_pion->SetFillColor(3);

  std::cout<<"check"<<std::endl;
  std::vector<double> cer_neg_e,cer_neg_pion;
  double cer_neg_e_all,cer_neg_pion_all;
  cer_neg_e_all = j_cer[(std::to_string(RunGroup)).c_str()]["neg_cal"]["e_all"].get<double>();
  cer_neg_pion_all = j_cer[(std::to_string(RunGroup)).c_str()]["neg_cal"]["pi_all"].get<double>();
  cer_neg_e = j_cer[(std::to_string(RunGroup)).c_str()]["neg_cal"]["e"].get<std::vector<double>>();
  cer_neg_pion = j_cer[(std::to_string(RunGroup)).c_str()]["neg_cal"]["pi"].get<std::vector<double>>();
  int n_cer_cuts = (int)cer_neg_e.size();
  std::vector<double> cer_cut = j_cuts["SHMS"]["HGC_cuts"].get<std::vector<double>>();
  //std::vector<double> cer_cut = {5,6,7,8,9,10};
  for(int i = 0;i<n_cer_cuts;++i){
    double e_eff = cer_neg_e[i]/cer_neg_e_all;
    g_cer_neg_e->SetPoint(i,cer_cut[i],e_eff);
    double pion_rej = cer_neg_pion_all/cer_neg_pion[i];
    g_cer_neg_pion->SetPoint(i,cer_cut[i],pion_rej);
  }

  gStyle->Reset("Modern");
  
  TCanvas *c_cer_neg = new TCanvas();
  TPad *p1_cer_neg = new TPad("p1_cer_neg","",0,0,1,1);
  p1_cer_neg->SetGrid();
  TPad *p2_cer_neg = new TPad("p2_cer_neg","",0,0,1,1);
  p2_cer_neg->SetFillStyle(4000);

  p1_cer_neg->Draw();
  p1_cer_neg->cd();
  g_cer_neg_e->Draw("ALP");
  g_cer_neg_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
  g_cer_neg_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
  gPad->Update();

  Style_t tfont_cer_neg = g_cer_neg_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cer_neg = g_cer_neg_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cer_neg = g_cer_neg_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cer_neg = g_cer_neg_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cer_neg = p1_cer_neg->GetUxmin();
      Double_t xmax_cer_neg = p1_cer_neg->GetUxmax();
      Double_t dx_cer_neg = (xmax_cer_neg-xmin_cer_neg)/0.8;
      Double_t ymin_cer_neg = g_cer_neg_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cer_neg = g_cer_neg_pion->GetHistogram()->GetMaximum();
      Double_t dy_cer_neg =(ymax_cer_neg - ymin_cer_neg)/0.8;
      p2_cer_neg->Range(xmin_cer_neg - 0.1*dx_cer_neg, ymin_cer_neg - 0.1*dy_cer_neg, xmax_cer_neg + 0.1*dx_cer_neg,ymax_cer_neg + 0.1*dy_cer_neg);
      p2_cer_neg->Draw();
      p2_cer_neg->cd();
      g_cer_neg_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cer_neg = new TGaxis(xmax_cer_neg, ymin_cer_neg, xmax_cer_neg, ymax_cer_neg, ymin_cer_neg, ymax_cer_neg, 510, "+L");
      axis_cer_neg->SetTitle("pion rejection ratio");
      axis_cer_neg->SetTitleFont(tfont_cer_neg);
      axis_cer_neg->SetTitleSize(tsize_cer_neg);
      axis_cer_neg->SetTitleColor(kRed);
      axis_cer_neg->SetTitleOffset(1.25);
      axis_cer_neg->SetLabelFont(lfont_cer_neg);
      axis_cer_neg->SetLabelSize(lsize_cer_neg);
      axis_cer_neg->SetLabelColor(kRed);
      axis_cer_neg->SetLineColor(kRed);
      axis_cer_neg->Draw();
      gPad->Update();
      
      TLegend *leg_cer_neg = new TLegend(0.30,0.85,0.80,0.90);
      leg_cer_neg->SetFillColor(gPad->GetFillColor());
      leg_cer_neg->SetTextFont(lfont_cer_neg);
      leg_cer_neg->SetTextSize(lsize_cer_neg);
      std::string cer_neg_title = "SHMS cer for run group "+std::to_string(RunGroup)+" neg runs";
      leg_cer_neg->SetHeader(cer_neg_title.c_str());
      leg_cer_neg->Draw();
      gPad->Update();
      
      std::string cer_neg_name = "results/pid/SHMS_cer_neg_"+std::to_string(RunGroup)+".pdf";
      c_cer_neg->SaveAs(cer_neg_name.c_str());

      // SHMS cer for pos runs part 
  TGraph *g_cer_pos_e = new TGraph();
  g_cer_pos_e->SetTitle("cer;cerenkov cut;efficiency");
  g_cer_pos_e->SetMarkerStyle(8);
  g_cer_pos_e->SetMarkerSize(0.75);
  g_cer_pos_e->SetMarkerColor(1);
  g_cer_pos_e->SetLineColor(1);
  g_cer_pos_e->SetFillColor(3);

  TGraph *g_cer_pos_pion = new TGraph();
  g_cer_pos_pion->SetMarkerStyle(8);
  g_cer_pos_pion->SetMarkerSize(0.75);
  g_cer_pos_pion->SetMarkerColor(2);
  g_cer_pos_pion->SetLineColor(2);
  g_cer_pos_pion->SetFillColor(3);

  std::cout<<"check"<<std::endl;
  std::vector<double> cer_pos_e,cer_pos_pion;
  double cer_pos_e_all,cer_pos_pion_all;
  cer_pos_e_all = j_cer[(std::to_string(RunGroup)).c_str()]["pos_cal"]["e_all"].get<double>();
  cer_pos_pion_all = j_cer[(std::to_string(RunGroup)).c_str()]["pos_cal"]["pi_all"].get<double>();
  cer_pos_e = j_cer[(std::to_string(RunGroup)).c_str()]["pos_cal"]["e"].get<std::vector<double>>();
  cer_pos_pion = j_cer[(std::to_string(RunGroup)).c_str()]["pos_cal"]["pi"].get<std::vector<double>>();
  for(int i = 0;i<n_cer_cuts;++i){
    double e_eff = cer_pos_e[i]/cer_pos_e_all;
    g_cer_pos_e->SetPoint(i,cer_cut[i],e_eff);
    double pion_rej = cer_pos_pion_all/cer_pos_pion[i];
    g_cer_pos_pion->SetPoint(i,cer_cut[i],pion_rej);
  }

  gStyle->Reset("Modern");
  
  TCanvas *c_cer_pos = new TCanvas();
  TPad *p1_cer_pos = new TPad("p1_cer_pos","",0,0,1,1);
  p1_cer_pos->SetGrid();
  TPad *p2_cer_pos = new TPad("p2_cer_pos","",0,0,1,1);
  p2_cer_pos->SetFillStyle(4000);

  p1_cer_pos->Draw();
  p1_cer_pos->cd();
  g_cer_pos_e->Draw("ALP");
  g_cer_pos_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
  g_cer_pos_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
  gPad->Update();

  Style_t tfont_cer_pos = g_cer_pos_e->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_cer_pos = g_cer_pos_e->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_cer_pos = g_cer_pos_e->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_cer_pos = g_cer_pos_e->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_cer_pos = p1_cer_pos->GetUxmin();
      Double_t xmax_cer_pos = p1_cer_pos->GetUxmax();
      Double_t dx_cer_pos = (xmax_cer_pos-xmin_cer_pos)/0.8;
      Double_t ymin_cer_pos = g_cer_pos_pion->GetHistogram()->GetMinimum();
      Double_t ymax_cer_pos = g_cer_pos_pion->GetHistogram()->GetMaximum();
      Double_t dy_cer_pos =(ymax_cer_pos - ymin_cer_pos)/0.8;
      p2_cer_pos->Range(xmin_cer_pos - 0.1*dx_cer_pos, ymin_cer_pos - 0.1*dy_cer_pos, xmax_cer_pos + 0.1*dx_cer_pos,ymax_cer_pos + 0.1*dy_cer_pos);
      p2_cer_pos->Draw();
      p2_cer_pos->cd();
      g_cer_pos_pion->Draw("LP");
      gPad->Update();

      TGaxis *axis_cer_pos = new TGaxis(xmax_cer_pos, ymin_cer_pos, xmax_cer_pos, ymax_cer_pos, ymin_cer_pos, ymax_cer_pos, 510, "+L");
      axis_cer_pos->SetTitle("pion rejection ratio");
      axis_cer_pos->SetTitleFont(tfont_cer_pos);
      axis_cer_pos->SetTitleSize(tsize_cer_pos);
      axis_cer_pos->SetTitleColor(kRed);
      axis_cer_pos->SetTitleOffset(1.25);
      axis_cer_pos->SetLabelFont(lfont_cer_pos);
      axis_cer_pos->SetLabelSize(lsize_cer_pos);
      axis_cer_pos->SetLabelColor(kRed);
      axis_cer_pos->SetLineColor(kRed);
      axis_cer_pos->Draw();
      gPad->Update();
      
      TLegend *leg_cer_pos = new TLegend(0.30,0.85,0.80,0.90);
      leg_cer_pos->SetFillColor(gPad->GetFillColor());
      leg_cer_pos->SetTextFont(lfont_cer_pos);
      leg_cer_pos->SetTextSize(lsize_cer_pos);
      std::string cer_pos_title = "SHMS cer for run group "+std::to_string(RunGroup)+" pos runs";
      leg_cer_pos->SetHeader(cer_pos_title.c_str());
      leg_cer_pos->Draw();
      gPad->Update();
      std::string cer_pos_name = "results/pid/SHMS_cer_pos_"+std::to_string(RunGroup)+".pdf";
      c_cer_pos->SaveAs(cer_pos_name.c_str());
}
