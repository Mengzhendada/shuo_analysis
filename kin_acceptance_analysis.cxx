#include <array>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

void kin_acceptance_analysis(){
int group_neg_first[4] = {6111,6115,6124,6136};
int group_pos_first[4] = {6145,6170,6178,6185};

int run1 = 6136;
int run2 = 6185;
std::string root_file_neg_name = "results/csv_kin/kin_acceptance/kin_rootfile_"+std::to_string(run1)+".root";
std::string root_file_pos_name = "results/csv_kin/kin_acceptance/kin_rootfile_"+std::to_string(run2)+".root";
TFile *root_file_neg = new TFile(root_file_neg_name.c_str());
TFile *root_file_pos = new TFile(root_file_pos_name.c_str());

TH1F *h_xbj_xq2cut = new TH1F("xbj_xq2cut","xbj xq2cut rungroup 140",100,0,1);
TH1F *h_xbj_xq2cut_neg = new TH1F("xbj_xq2cut_neg","xbj xq2cut neg rungroup 140",100,0,1);
TH1F *h_xbj_xq2cut_pos = new TH1F("xbj_xq2cut_pos","xbj xq2cut pos rungroup 140",100,0,1);
h_xbj_xq2cut_neg= (TH1F*)root_file_neg->Get("xbj_xq2cut");
h_xbj_xq2cut_neg->SetLineColor(4);
h_xbj_xq2cut_pos= (TH1F*)root_file_pos->Get("xbj_xq2cut");
h_xbj_xq2cut_pos->SetLineColor(2);
h_xbj_xq2cut_pos->SetFillColor(2);
h_xbj_xq2cut->Add(h_xbj_xq2cut_neg);
h_xbj_xq2cut->Add(h_xbj_xq2cut_pos);

double neg_integral = h_xbj_xq2cut_neg->Integral();
double pos_integral = h_xbj_xq2cut_pos->Integral();
double scale = neg_integral/pos_integral;

TH1F *h_xbj_cut1 = new TH1F("xbj_cut1","xbj cut1 rungroup 140",100,0,1);
TH1F *h_xbj_cut1_neg = new TH1F("xbj_cut1_neg","xbj cut1 neg rungroup 140",100,0,1);
TH1F *h_xbj_cut1_pos = new TH1F("xbj_cut1_pos","xbj cut1 pos rungroup 140",100,0,1);
h_xbj_cut1_neg= (TH1F*)root_file_neg->Get("xbj_cut1");
h_xbj_cut1_neg->SetLineColor(3);
h_xbj_cut1_pos= (TH1F*)root_file_pos->Get("xbj_cut1");
h_xbj_cut1_pos->SetLineColor(6);
h_xbj_cut1_pos->SetFillColorAlpha(6,0.3);
h_xbj_cut1->Add(h_xbj_cut1_neg);
h_xbj_cut1->Add(h_xbj_cut1_pos);

TH1F *h_xbj_cut2 = new TH1F("xbj_cut2","xbj cut2 rungroup 140",100,0,1);
TH1F *h_xbj_cut2_neg = new TH1F("xbj_cut2_neg","xbj cut2 neg rungroup 140",100,0,1);
TH1F *h_xbj_cut2_pos = new TH1F("xbj_cut2_pos","xbj cut2 pos rungroup 140",100,0,1);
h_xbj_cut2_neg= (TH1F*)root_file_neg->Get("xbj_cut2");
h_xbj_cut2_neg->SetLineColor(7);
h_xbj_cut2_pos= (TH1F*)root_file_pos->Get("xbj_cut2");
h_xbj_cut2_pos->SetLineColor(46);
h_xbj_cut2->Add(h_xbj_cut2_neg);
h_xbj_cut2->Add(h_xbj_cut2_pos);

TH1F *h_xbj_cut3 = new TH1F("xbj_cut3","xbj cut3 rungroup 140",100,0,1);
TH1F *h_xbj_cut3_neg = new TH1F("xbj_cut3_neg","xbj cut3 neg rungroup 140",100,0,1);
TH1F *h_xbj_cut3_pos = new TH1F("xbj_cut3_pos","xbj cut3 pos rungroup 140",100,0,1);
h_xbj_cut3_neg= (TH1F*)root_file_neg->Get("xbj_cut3");
h_xbj_cut3_neg->SetLineColor(38);
h_xbj_cut3_pos= (TH1F*)root_file_pos->Get("xbj_cut3");
h_xbj_cut3_pos->SetLineColor(45);
h_xbj_cut3->Add(h_xbj_cut3_neg);
h_xbj_cut3->Add(h_xbj_cut3_pos);

TCanvas *c_xbj = new TCanvas("xbj");
c_xbj->Divide(2,2);
c_xbj->cd(1);
h_xbj_xq2cut_pos->Scale(scale);
h_xbj_xq2cut_pos->Draw();
h_xbj_xq2cut_neg->Draw("same");
c_xbj->cd(2);
h_xbj_cut1_pos->Scale(scale);
h_xbj_cut1_pos->Draw();
h_xbj_cut1_neg->Draw("same");
c_xbj->cd(3);
h_xbj_cut2_pos->Scale(scale);
h_xbj_cut2_pos->Draw();
h_xbj_cut2_neg->Draw("same");
c_xbj->cd(4);
h_xbj_cut3_pos->Scale(scale);
h_xbj_cut3_pos->Draw();
h_xbj_cut3_neg->Draw("same");
std::string c_xbj_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(run1)+"_"+std::to_string(run2)+"_xbj.pdf";
c_xbj->SaveAs(c_xbj_name.c_str());

TH1F *h_q2_xq2cut = new TH1F("q2_xq2cut","q2 xq2cut rungroup 140",100,0,10);
TH1F *h_q2_xq2cut_neg = new TH1F("q2_xq2cut_neg","q2 xq2cut neg rungroup 140",100,0,10);
TH1F *h_q2_xq2cut_pos = new TH1F("q2_xq2cut_pos","q2 xq2cut pos rungroup 140",100,0,10);
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
