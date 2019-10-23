#include <array>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include "TRatioPlot.h"

void kin_acceptance_analysis(){
int group_neg_first[4] = {6111,6115,6124,6136};
int group_pos_first[4] = {6145,6170,6178,6185};

int run1 = 6136;
int run2 = 6185;
std::string root_file_neg_name = "results/csv_kin/kin_acceptance/kin_rootfile_"+std::to_string(run1)+".root";
std::string root_file_pos_name = "results/csv_kin/kin_acceptance/kin_rootfile_"+std::to_string(run2)+".root";
TFile *root_file_neg = new TFile(root_file_neg_name.c_str());
TFile *root_file_pos = new TFile(root_file_pos_name.c_str());
double neg_integral, pos_integral,scale;



TH1F *h_xbj_xq2cut_neg = new TH1F("xbj_nocut_neg","xbj nocut neg rungroup 140",100,0,1);
h_xbj_xq2cut_neg= (TH1F*)root_file_neg->Get("xbj_xq2cut");
h_xbj_xq2cut_neg->SetLineColor(4);
TH1F *h_xbj_xq2cut_pos = new TH1F("xbj_nocut_pos","xbj nocut pos rungroup 140",100,0,1);
h_xbj_xq2cut_pos= (TH1F*)root_file_pos->Get("xbj_xq2cut");
h_xbj_xq2cut_pos->SetLineColor(2);
h_xbj_xq2cut_pos->SetFillColor(2);

neg_integral = h_xbj_xq2cut_neg->Integral();
pos_integral = h_xbj_xq2cut_pos->Integral();
scale = neg_integral/pos_integral;
std::cout<<"1 "<<neg_integral<<" "<<pos_integral<<" "<<scale<<std::endl;

TCanvas *c_xbj_xq2cut = new TCanvas();
h_xbj_xq2cut_pos->Scale(scale);
auto rp_xbj_xq2cut = new TRatioPlot(h_xbj_xq2cut_pos,h_xbj_xq2cut_neg);
c_xbj_xq2cut->SetTicks(0,1);
rp_xbj_xq2cut->Draw();
rp_xbj_xq2cut->GetLowerRefGraph()->SetMinimum(0.8);
rp_xbj_xq2cut->GetLowerRefGraph()->SetMaximum(1.2);
c_xbj_xq2cut->Update();
//h_xbj_xq2cut_pos->DrawCopy();
//h_xbj_xq2cut_neg->DrawCopy("same");
std::string c_xbj_xq2cut_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj_nocut.pdf";
c_xbj_xq2cut->SaveAs(c_xbj_xq2cut_name.c_str());

//TH1F *h_xbj_cut1 = new TH1F("xbj_cut1","xbj cut1 rungroup 140",100,0,1);
TH1F *h_xbj_cut1_neg = new TH1F("xbj_cut1_neg","xbj cut1 neg rungroup 140",100,0,1);
h_xbj_cut1_neg= (TH1F*)root_file_neg->Get("xbj_cut1");
h_xbj_cut1_neg->SetLineColor(3);
TH1F *h_xbj_cut1_pos = new TH1F("xbj_cut1_pos","xbj cut1 pos rungroup 140",100,0,1);
h_xbj_cut1_pos= (TH1F*)root_file_pos->Get("xbj_cut1");
h_xbj_cut1_pos->SetLineColor(6);

TCanvas *c_xbj_cut1 = new TCanvas();
h_xbj_cut1_pos->Scale(scale);
auto rp_xbj_cut1 = new TRatioPlot(h_xbj_cut1_pos,h_xbj_cut1_neg);
c_xbj_cut1->SetTicks(0,1);
rp_xbj_cut1->Draw();
rp_xbj_cut1->GetLowerRefGraph()->SetMinimum(0.8);
rp_xbj_cut1->GetLowerRefGraph()->SetMaximum(1.2);
c_xbj_cut1->Update();
//h_xbj_cut1_pos->DrawCopy();
//h_xbj_cut1_neg->DrawCopy("same");
std::string c_xbj_cut1_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj_cut1.pdf";
c_xbj_cut1->SaveAs(c_xbj_cut1_name.c_str());

TH1F *h_xbj_cut2_neg = new TH1F("xbj_cut2_neg","xbj cut2 neg rungroup 140",100,0,1);
h_xbj_cut2_neg= (TH1F*)root_file_neg->Get("xbj_cut2");
h_xbj_cut2_neg->SetLineColor(7);
TH1F *h_xbj_cut2_pos = new TH1F("xbj_cut2_pos","xbj cut2 pos rungroup 140",100,0,1);
h_xbj_cut2_pos= (TH1F*)root_file_pos->Get("xbj_cut2");
h_xbj_cut2_pos->SetLineColor(46);

TCanvas *c_xbj_cut2 = new TCanvas();
h_xbj_cut2_pos->Scale(scale);
auto rp_xbj_cut2 = new TRatioPlot(h_xbj_cut2_pos,h_xbj_cut2_neg);
c_xbj_cut2->SetTicks(0,1);
rp_xbj_cut2->Draw();
rp_xbj_cut2->GetLowerRefGraph()->SetMinimum(0.8);
rp_xbj_cut2->GetLowerRefGraph()->SetMaximum(1.2);
c_xbj_cut2->Update();
//h_xbj_cut2_pos->DrawCopy();
//h_xbj_cut2_neg->DrawCopy("same");
std::string c_xbj_cut2_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj_cut2.pdf";
c_xbj_cut2->SaveAs(c_xbj_cut2_name.c_str());

TH1F *h_xbj_cut3_neg = new TH1F("xbj_cut3_neg","xbj cut3 neg rungroup 140",100,0,1);
h_xbj_cut3_neg= (TH1F*)root_file_neg->Get("xbj_cut3");
h_xbj_cut3_neg->SetLineColor(38);
TH1F *h_xbj_cut3_pos = new TH1F("xbj_cut3_pos","xbj cut3 pos rungroup 140",100,0,1);
h_xbj_cut3_pos= (TH1F*)root_file_pos->Get("xbj_cut3");
h_xbj_cut3_pos->SetLineColor(45);

TCanvas *c_xbj_cut3 = new TCanvas();
h_xbj_cut3_pos->Scale(scale);
auto rp_xbj_cut3 = new TRatioPlot(h_xbj_cut3_pos,h_xbj_cut3_neg);
c_xbj_cut3->SetTicks(0,1);
rp_xbj_cut3->Draw();
rp_xbj_cut3->GetLowerRefGraph()->SetMinimum(0.8);
rp_xbj_cut3->GetLowerRefGraph()->SetMaximum(1.2);
c_xbj_cut3->Update();
//h_xbj_cut3_pos->DrawCopy();
//h_xbj_cut3_neg->DrawCopy("same");
std::string c_xbj_cut3_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj_cut3.pdf";
c_xbj_cut3->SaveAs(c_xbj_cut3_name.c_str());

    TCanvas *c_xbj_xq2_cut1 = new TCanvas();
    double xq2cut_integral = h_xbj_xq2cut_neg->Integral();
    double cut1_integral = h_xbj_cut1_neg->Integral();
    double scale_neg = xq2cut_integral/cut1_integral;
    //h_xbj_xq2cut->DrawCopy();
    h_xbj_cut1_neg->Scale(scale_neg);
    //h_xbj_cut1->DrawCopy("same");
    auto rp_xbj_xq2_cut1 = new TRatioPlot(h_xbj_xq2cut_neg,h_xbj_cut1_neg);
    c_xbj_xq2_cut1->SetTicks(0,1);
    rp_xbj_xq2_cut1->Draw();
    rp_xbj_xq2_cut1->GetLowerRefGraph()->SetMinimum(0.8);
    rp_xbj_xq2_cut1->GetLowerRefGraph()->SetMaximum(1.2);
    c_xbj_xq2_cut1->SetName("compare nocut and cut1");
    c_xbj_xq2_cut1->Update();
    std::string c_xbj_xq2_cut1_name = "results/csv_kin/kin_acceptance/xbj_xq2cut_cut1_"+std::to_string(run1)+".pdf";
    c_xbj_xq2_cut1->SaveAs(c_xbj_xq2_cut1_name.c_str());
   
    h_xbj_cut1_neg->Scale(1/scale_neg);

    TCanvas *c_xbj_cut3_cut1 = new TCanvas();
    double cut3_integral = h_xbj_cut3_neg->Integral();
    scale_neg = cut3_integral/cut1_integral;
    //h_xbj_cut3->DrawCopy();
    h_xbj_cut1_neg->Scale(scale_neg);
    //h_xbj_cut1->DrawCopy("same");
    auto rp_xbj_cut3_cut1 = new TRatioPlot(h_xbj_cut3_neg,h_xbj_cut1_neg);
    c_xbj_cut3_cut1->SetTicks(0,1);
    rp_xbj_cut3_cut1->Draw();
    rp_xbj_cut3_cut1->GetLowerRefGraph()->SetMinimum(0.8);
    rp_xbj_cut3_cut1->GetLowerRefGraph()->SetMaximum(1.2);
    c_xbj_cut3_cut1->SetName("compare cut3 and cut1");
    c_xbj_cut3_cut1->Update();
    std::string c_xbj_cut3_cut1_name = "results/csv_kin/kin_acceptance/xbj_cut3_cut1_"+std::to_string(run1)+".pdf";
    c_xbj_cut3_cut1->SaveAs(c_xbj_cut3_cut1_name.c_str());

    //TCanvas *c_xbj = new TCanvas("xbj");
//c_xbj->Divide(2,2);
//c_xbj->cd(1);
//h_xbj_xq2cut_pos->Scale(scale);
//auto rp_xbj_nocut = new TRatioPlot(h_xbj_xq2cut_pos,h_xbj_xq2cut_neg);
//rp_xbj_nocut->Draw();
////h_xbj_xq2cut_pos->Draw();
////h_xbj_xq2cut_neg->Draw("same");
//c_xbj->cd(2);
//h_xbj_cut1_pos->Scale(scale);
//auto rp_xbj_cut1 = new TRatioPlot(h_xbj_cut1_pos,h_xbj_cut1_neg);
//rp_xbj_cut1->Draw();
//
////h_xbj_cut1_pos->Draw();
////h_xbj_cut1_neg->Draw("same");
//c_xbj->cd(3);
//h_xbj_cut2_pos->Scale(scale);
//auto rp_xbj_cut2 = new TRatioPlot(h_xbj_cut2_pos,h_xbj_cut2_neg);
//rp_xbj_cut2->Draw();
////h_xbj_cut2_pos->Draw();
////h_xbj_cut2_neg->Draw("same");
//c_xbj->cd(4);
//h_xbj_cut3_pos->Scale(scale);
//auto rp_xbj_cut3 = new TRatioPlot(h_xbj_cut3_pos,h_xbj_cut3_neg);
//rp_xbj_cut3->Draw();
////h_xbj_cut3_pos->Draw();
////h_xbj_cut3_neg->Draw("same");
//c_xbj->Update();
//std::string c_xbj_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj.pdf";
//c_xbj->SaveAs(c_xbj_name.c_str());

TH1F *h_q2_xq2cut = new TH1F("q2_nocut","q2 nocut rungroup 140",100,0,10);
TH1F *h_q2_xq2cut_neg = new TH1F("q2_nocut_neg","q2 nocut neg rungroup 140",100,0,10);
TH1F *h_q2_xq2cut_pos = new TH1F("q2_nocut_pos","q2 nocut pos rungroup 140",100,0,10);
h_q2_xq2cut_neg= (TH1F*)root_file_neg->Get("q2_xq2cut");
h_q2_xq2cut_neg->SetLineColor(4);
h_q2_xq2cut_pos= (TH1F*)root_file_pos->Get("q2_xq2cut");
h_q2_xq2cut_pos->SetLineColor(2);
h_q2_xq2cut->Add(h_q2_xq2cut_neg);
h_q2_xq2cut->Add(h_q2_xq2cut_pos);

TH1F *h_q2_cut1 = new TH1F("q2_cut1","q2 cut1 rungroup 140",100,0,10);
TH1F *h_q2_cut1_neg = new TH1F("q2_cut1_neg","q2 cut1 neg rungroup 140",100,0,10);
TH1F *h_q2_cut1_pos = new TH1F("q2_cut1_pos","q2 cut1 pos rungroup 140",100,0,10);
h_q2_cut1_neg= (TH1F*)root_file_neg->Get("q2_cut1");
h_q2_cut1_neg->SetLineColor(3);
h_q2_cut1_pos= (TH1F*)root_file_pos->Get("q2_cut1");
h_q2_cut1_pos->SetLineColor(6);
h_q2_cut1->Add(h_q2_cut1_neg);
h_q2_cut1->Add(h_q2_cut1_pos);

TH1F *h_q2_cut2 = new TH1F("q2_cut2","q2 cut2 rungroup 140",100,0,10);
TH1F *h_q2_cut2_neg = new TH1F("q2_cut2_neg","q2 cut2 neg rungroup 140",100,0,10);
TH1F *h_q2_cut2_pos = new TH1F("q2_cut2_pos","q2 cut2 pos rungroup 140",100,0,10);
h_q2_cut2_neg= (TH1F*)root_file_neg->Get("q2_cut2");
h_q2_cut2_neg->SetLineColor(7);
h_q2_cut2_pos= (TH1F*)root_file_pos->Get("q2_cut2");
h_q2_cut2_pos->SetLineColor(46);
h_q2_cut2->Add(h_q2_cut2_neg);
h_q2_cut2->Add(h_q2_cut2_pos);

TH1F *h_q2_cut3 = new TH1F("q2_cut3","q2 cut3 rungroup 140",100,0,10);
TH1F *h_q2_cut3_neg = new TH1F("q2_cut3_neg","q2 cut3 neg rungroup 140",100,0,10);
TH1F *h_q2_cut3_pos = new TH1F("q2_cut3_pos","q2 cut3 pos rungroup 140",100,0,10);
h_q2_cut3_neg= (TH1F*)root_file_neg->Get("q2_cut3");
h_q2_cut3_neg->SetLineColor(38);
h_q2_cut3_pos= (TH1F*)root_file_pos->Get("q2_cut3");
h_q2_cut3_pos->SetLineColor(45);
h_q2_cut3->Add(h_q2_cut3_neg);
h_q2_cut3->Add(h_q2_cut3_pos);

TCanvas *c_q2 = new TCanvas("q2");
c_q2->Divide(2,2);
c_q2->cd(1);
h_q2_xq2cut_pos->Scale(scale);
h_q2_xq2cut_pos->Draw();
h_q2_xq2cut_neg->Draw("same");
c_q2->cd(2);
h_q2_cut1_pos->Scale(scale);
h_q2_cut1_pos->Draw();
h_q2_cut1_neg->Draw("same");
c_q2->cd(3);
h_q2_cut2_pos->Scale(scale);
h_q2_cut2_pos->Draw();
h_q2_cut2_neg->Draw("same");
c_q2->cd(4);
h_q2_cut3_pos->Scale(scale);
h_q2_cut3_pos->Draw();
h_q2_cut3_neg->Draw("same");
std::string c_q2_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_q2.pdf";
c_q2->SaveAs(c_q2_name.c_str());

}
