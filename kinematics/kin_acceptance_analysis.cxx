#include <array>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include "TRatioPlot.h"
#include "TPaveText.h"

void kin_acceptance_analysis(int Rungroup=0){

if(Rungroup ==0){
  std::cout<<"Enter a Rungroup (-1 to exit):";
  std::cin>>Rungroup;
  if(Rungroup<=0)
    return;
}

std::string root_file_neg_name = "results/csv_kin/kin_acceptance/kin_neg_rootfile_"+std::to_string(Rungroup)+".root";
std::string root_file_pos_name = "results/csv_kin/kin_acceptance/kin_pos_rootfile_"+std::to_string(Rungroup)+".root";
TFile *root_file_neg = new TFile(root_file_neg_name.c_str());
TFile *root_file_pos = new TFile(root_file_pos_name.c_str());
double neg_xbj_integral, pos_xbj_integral,scale_xbj;
double xq2cut_integral,cut1_integral,cut3_integral,scale_neg;

//..........................compare xbj under different condition
//compare neg and pos xbj without cuts
TH1F *h_xbj_xq2cut_neg = new TH1F("xbj_nocut_neg","xbj nocut neg rungroup 140",100,0,1);
h_xbj_xq2cut_neg= (TH1F*)root_file_neg->Get("xbj_xq2cut");
h_xbj_xq2cut_neg->SetLineColor(4);
TH1F *h_xbj_xq2cut_pos = new TH1F("xbj_nocut_pos","xbj nocut pos rungroup 140",100,0,1);
h_xbj_xq2cut_pos= (TH1F*)root_file_pos->Get("xbj_xq2cut");
h_xbj_xq2cut_pos->SetLineColor(2);
h_xbj_xq2cut_pos->SetFillColor(2);

neg_xbj_integral = h_xbj_xq2cut_neg->Integral();
pos_xbj_integral = h_xbj_xq2cut_pos->Integral();
scale_xbj = neg_xbj_integral/pos_xbj_integral;
std::cout<<"1 "<<neg_xbj_integral<<" "<<pos_xbj_integral<<" "<<scale_xbj<<std::endl;

TCanvas *c_xbj_xq2cut = new TCanvas();
//h_xbj_xq2cut_pos->Scale(scale_xbj);
auto rp_xbj_xq2cut = new TRatioPlot(h_xbj_xq2cut_pos,h_xbj_xq2cut_neg);
c_xbj_xq2cut->SetTicks(0,1);
rp_xbj_xq2cut->Draw();
rp_xbj_xq2cut->GetLowerRefGraph()->SetMinimum(0.5);
rp_xbj_xq2cut->GetLowerRefGraph()->SetMaximum(1.5);
//rp_xbj_xq2cut->Fit("pol 1","","",0,1);
//TF1 *Fit_xbj_xq2cut = rp_xbj_xq2cut->GetFunction("pol 1");
//double xbj_xq2cut_offset = Fit_xbj_xq2cut->GetParameter(1);
//double xbj_xq2cut_slop = Fit_xbj_xq2cut->GetParameter(2);
//TPaveText *pt_xbj_xq2cut = new TPaveText(.05,.1,.95,.8);
//std::string string_xbj_xq2cut = "The linear fit function has slop " +xbj_xq2cut_slop +" and offset "+xbj_xq2cut_offset;
//pt_xbj_xq2cut->AddText(string_xbj_xq2cut.c_str());
//pt_xbj_xq2cut->Draw();
c_xbj_xq2cut->Update();
//h_xbj_xq2cut_pos->DrawCopy();
//h_xbj_xq2cut_neg->DrawCopy("same");
std::string c_xbj_xq2cut_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_xbj_nocut.png";
c_xbj_xq2cut->SaveAs(c_xbj_xq2cut_name.c_str());

//compare neg and pos xbj with cut1
//TH1F *h_xbj_cut1 = new TH1F("xbj_cut1","xbj cut1 rungroup 140",100,0,1);
TH1F *h_xbj_cut1_neg = new TH1F("xbj_cut1_neg","xbj cut1 neg rungroup 140",100,0,1);
h_xbj_cut1_neg= (TH1F*)root_file_neg->Get("xbj_cut1");
h_xbj_cut1_neg->SetLineColor(3);
TH1F *h_xbj_cut1_pos = new TH1F("xbj_cut1_pos","xbj cut1 pos rungroup 140",100,0,1);
h_xbj_cut1_pos= (TH1F*)root_file_pos->Get("xbj_cut1");
h_xbj_cut1_pos->SetLineColor(6);

TCanvas *c_xbj_cut1 = new TCanvas();
//h_xbj_cut1_pos->Scale(scale_xbj);
auto rp_xbj_cut1 = new TRatioPlot(h_xbj_cut1_pos,h_xbj_cut1_neg);
c_xbj_cut1->SetTicks(0,1);
rp_xbj_cut1->Draw();
rp_xbj_cut1->GetLowerRefGraph()->SetMinimum(0.5);
rp_xbj_cut1->GetLowerRefGraph()->SetMaximum(1.5);
c_xbj_cut1->Update();
//h_xbj_cut1_pos->DrawCopy();
//h_xbj_cut1_neg->DrawCopy("same");
std::string c_xbj_cut1_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_xbj_cut1.png";
c_xbj_cut1->SaveAs(c_xbj_cut1_name.c_str());

//compare neg and pos xbj with cut2
TH1F *h_xbj_cut2_neg = new TH1F("xbj_cut2_neg","xbj cut2 neg rungroup 140",100,0,1);
h_xbj_cut2_neg= (TH1F*)root_file_neg->Get("xbj_cut2");
h_xbj_cut2_neg->SetLineColor(7);
TH1F *h_xbj_cut2_pos = new TH1F("xbj_cut2_pos","xbj cut2 pos rungroup 140",100,0,1);
h_xbj_cut2_pos= (TH1F*)root_file_pos->Get("xbj_cut2");
h_xbj_cut2_pos->SetLineColor(46);

TCanvas *c_xbj_cut2 = new TCanvas();
//h_xbj_cut2_pos->Scale(scale_xbj);
auto rp_xbj_cut2 = new TRatioPlot(h_xbj_cut2_pos,h_xbj_cut2_neg);
c_xbj_cut2->SetTicks(0,1);
rp_xbj_cut2->Draw();
rp_xbj_cut2->GetLowerRefGraph()->SetMinimum(0.5);
rp_xbj_cut2->GetLowerRefGraph()->SetMaximum(1.5);
c_xbj_cut2->Update();
//h_xbj_cut2_pos->DrawCopy();
//h_xbj_cut2_neg->DrawCopy("same");
std::string c_xbj_cut2_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_xbj_cut2.png";
c_xbj_cut2->SaveAs(c_xbj_cut2_name.c_str());

//compare neg and pos with cut3
TH1F *h_xbj_cut3_neg = new TH1F("xbj_cut3_neg","xbj cut3 neg rungroup 140",100,0,1);
h_xbj_cut3_neg= (TH1F*)root_file_neg->Get("xbj_cut3");
h_xbj_cut3_neg->SetLineColor(38);
TH1F *h_xbj_cut3_pos = new TH1F("xbj_cut3_pos","xbj cut3 pos rungroup 140",100,0,1);
h_xbj_cut3_pos= (TH1F*)root_file_pos->Get("xbj_cut3");
h_xbj_cut3_pos->SetLineColor(45);

TCanvas *c_xbj_cut3 = new TCanvas();
//h_xbj_cut3_pos->Scale(scale_xbj);
auto rp_xbj_cut3 = new TRatioPlot(h_xbj_cut3_pos,h_xbj_cut3_neg);
c_xbj_cut3->SetTicks(0,1);
rp_xbj_cut3->Draw();
rp_xbj_cut3->GetLowerRefGraph()->SetMinimum(0.5);
rp_xbj_cut3->GetLowerRefGraph()->SetMaximum(1.5);
c_xbj_cut3->Update();
//h_xbj_cut3_pos->DrawCopy();
//h_xbj_cut3_neg->DrawCopy("same");
std::string c_xbj_cut3_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_xbj_cut3.png";
c_xbj_cut3->SaveAs(c_xbj_cut3_name.c_str());

//compare neg runs xbj with different cut, no cut and cut1
    TCanvas *c_xbj_xq2_cut1 = new TCanvas();
    xq2cut_integral = h_xbj_xq2cut_neg->Integral();
    cut1_integral = h_xbj_cut1_neg->Integral();
    scale_neg = xq2cut_integral/cut1_integral;
    h_xbj_xq2cut_neg->DrawCopy();
    h_xbj_cut1_neg->Scale(scale_neg);
    h_xbj_cut1_neg->DrawCopy("same");
    auto rp_xbj_xq2_cut1 = new TRatioPlot(h_xbj_xq2cut_neg,h_xbj_cut1_neg);
    c_xbj_xq2_cut1->SetTicks(0,1);
    rp_xbj_xq2_cut1->Draw();
    rp_xbj_xq2_cut1->GetLowerRefGraph()->SetMinimum(0.5);
    rp_xbj_xq2_cut1->GetLowerRefGraph()->SetMaximum(1.5);
    c_xbj_xq2_cut1->SetName("compare nocut and cut1");
    c_xbj_xq2_cut1->Update();
    std::string c_xbj_xq2_cut1_name = "results/csv_kin/kin_acceptance/xbj_neg_xq2cut_cut1_"+std::to_string(Rungroup)+".png";
    c_xbj_xq2_cut1->SaveAs(c_xbj_xq2_cut1_name.c_str());
   
    //scale back to origin xbj
    h_xbj_cut1_neg->Scale(1/scale_neg);

    //compare neg runs xbj for different cuts, cut3 and cut 1
    TCanvas *c_xbj_cut3_cut1 = new TCanvas();
    cut3_integral = h_xbj_cut3_neg->Integral();
    scale_neg = cut3_integral/cut1_integral;
    h_xbj_cut3_neg->DrawCopy();
    h_xbj_cut1_neg->Scale(scale_neg);
    h_xbj_cut1_neg->DrawCopy("same");
    auto rp_xbj_cut3_cut1 = new TRatioPlot(h_xbj_cut3_neg,h_xbj_cut1_neg);
    c_xbj_cut3_cut1->SetTicks(0,1);
    rp_xbj_cut3_cut1->Draw();
    rp_xbj_cut3_cut1->GetLowerRefGraph()->SetMinimum(0.5);
    rp_xbj_cut3_cut1->GetLowerRefGraph()->SetMaximum(1.5);
    c_xbj_cut3_cut1->SetName("compare cut3 and cut1");
    c_xbj_cut3_cut1->Update();
    std::string c_xbj_cut3_cut1_name = "results/csv_kin/kin_acceptance/xbj_neg_cut3_cut1_"+std::to_string(Rungroup)+".png";
    c_xbj_cut3_cut1->SaveAs(c_xbj_cut3_cut1_name.c_str());

    //.......................xbj done...............

double neg_z_integral, pos_z_integral,scale_z;

TH1F *h_z_xq2cut_neg = new TH1F("z_nocut_neg","z nocut neg ",100,0,3);
h_z_xq2cut_neg= (TH1F*)root_file_neg->Get("z_xq2cut");
h_z_xq2cut_neg->SetLineColor(4);
TH1F *h_z_xq2cut_pos = new TH1F("z_nocut_pos","z nocut pos ",100,0,3);
h_z_xq2cut_pos= (TH1F*)root_file_pos->Get("z_xq2cut");
h_z_xq2cut_pos->SetLineColor(2);
h_z_xq2cut_pos->SetFillColor(2);

neg_z_integral = h_z_xq2cut_neg->Integral();
pos_z_integral = h_z_xq2cut_pos->Integral();
scale_z = neg_z_integral/pos_z_integral;
std::cout<<"1 "<<neg_z_integral<<" "<<pos_z_integral<<" "<<scale_z<<std::endl;

TCanvas *c_z_xq2cut = new TCanvas();
//h_z_xq2cut_pos->Scale(scale_z);
auto rp_z_xq2cut = new TRatioPlot(h_z_xq2cut_pos,h_z_xq2cut_neg);
c_z_xq2cut->SetTicks(0,1);
rp_z_xq2cut->Draw();
rp_z_xq2cut->GetLowerRefGraph()->SetMinimum(0.5);
rp_z_xq2cut->GetLowerRefGraph()->SetMaximum(1.5);
c_z_xq2cut->Update();
//h_z_xq2cut_pos->DrawCopy();
//h_z_xq2cut_neg->DrawCopy("same");
std::string c_z_xq2cut_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_z_nocut.png";
c_z_xq2cut->SaveAs(c_z_xq2cut_name.c_str());

//TH1F *h_z_cut1 = new TH1F("z_cut1","z cut1 rungroup 140",100,0,3);
TH1F *h_z_cut1_neg = new TH1F("z_cut1_neg","z cut1 neg ",100,0,3);
h_z_cut1_neg= (TH1F*)root_file_neg->Get("z_cut1");
h_z_cut1_neg->SetLineColor(3);
TH1F *h_z_cut1_pos = new TH1F("z_cut1_pos","z cut1 pos ",100,0,3);
h_z_cut1_pos= (TH1F*)root_file_pos->Get("z_cut1");
h_z_cut1_pos->SetLineColor(6);

TCanvas *c_z_cut1 = new TCanvas();
//h_z_cut1_pos->Scale(scale_z);
auto rp_z_cut1 = new TRatioPlot(h_z_cut1_pos,h_z_cut1_neg);
c_z_cut1->SetTicks(0,1);
rp_z_cut1->Draw();
rp_z_cut1->GetLowerRefGraph()->SetMinimum(0.5);
rp_z_cut1->GetLowerRefGraph()->SetMaximum(1.5);
c_z_cut1->Update();
//h_z_cut1_pos->DrawCopy();
//h_z_cut1_neg->DrawCopy("same");
std::string c_z_cut1_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_z_cut1.png";
c_z_cut1->SaveAs(c_z_cut1_name.c_str());

TH1F *h_z_cut2_neg = new TH1F("z_cut2_neg","z cut2 neg ",100,0,3);
h_z_cut2_neg= (TH1F*)root_file_neg->Get("z_cut2");
h_z_cut2_neg->SetLineColor(7);
TH1F *h_z_cut2_pos = new TH1F("z_cut2_pos","z cut2 pos ",100,0,3);
h_z_cut2_pos= (TH1F*)root_file_pos->Get("z_cut2");
h_z_cut2_pos->SetLineColor(46);

TCanvas *c_z_cut2 = new TCanvas();
//h_z_cut2_pos->Scale(scale_z);
auto rp_z_cut2 = new TRatioPlot(h_z_cut2_pos,h_z_cut2_neg);
c_z_cut2->SetTicks(0,1);
rp_z_cut2->Draw();
rp_z_cut2->GetLowerRefGraph()->SetMinimum(0.5);
rp_z_cut2->GetLowerRefGraph()->SetMaximum(1.5);
c_z_cut2->Update();
//h_z_cut2_pos->DrawCopy();
//h_z_cut2_neg->DrawCopy("same");
std::string c_z_cut2_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_z_cut2.png";
c_z_cut2->SaveAs(c_z_cut2_name.c_str());

TH1F *h_z_cut3_neg = new TH1F("z_cut3_neg","z cut3 neg ",100,0,3);
h_z_cut3_neg= (TH1F*)root_file_neg->Get("z_cut3");
h_z_cut3_neg->SetLineColor(38);
TH1F *h_z_cut3_pos = new TH1F("z_cut3_pos","z cut3 pos ",100,0,3);
h_z_cut3_pos= (TH1F*)root_file_pos->Get("z_cut3");
h_z_cut3_pos->SetLineColor(45);

TCanvas *c_z_cut3 = new TCanvas();
//h_z_cut3_pos->Scale(scale_z);
auto rp_z_cut3 = new TRatioPlot(h_z_cut3_pos,h_z_cut3_neg);
c_z_cut3->SetTicks(0,1);
rp_z_cut3->Draw();
rp_z_cut3->GetLowerRefGraph()->SetMinimum(0.5);
rp_z_cut3->GetLowerRefGraph()->SetMaximum(1.5);
c_z_cut3->Update();
//h_z_cut3_pos->DrawCopy();
//h_z_cut3_neg->DrawCopy("same");
std::string c_z_cut3_name = "results/csv_kin/kin_acceptance/compare_"+std::to_string(Rungroup)+"_z_cut3.png";
c_z_cut3->SaveAs(c_z_cut3_name.c_str());

    TCanvas *c_z_xq2_cut1 = new TCanvas();
    xq2cut_integral = h_z_xq2cut_neg->Integral();
    cut1_integral = h_z_cut1_neg->Integral();
    scale_neg = xq2cut_integral/cut1_integral;
    //h_z_xq2cut->DrawCopy();
    h_z_cut1_neg->Scale(scale_neg);
    //h_z_cut1->DrawCopy("same");
    auto rp_z_xq2_cut1 = new TRatioPlot(h_z_xq2cut_neg,h_z_cut1_neg);
    c_z_xq2_cut1->SetTicks(0,1);
    rp_z_xq2_cut1->Draw();
    rp_z_xq2_cut1->GetLowerRefGraph()->SetMinimum(0.5);
    rp_z_xq2_cut1->GetLowerRefGraph()->SetMaximum(1.5);
    c_z_xq2_cut1->SetName("compare nocut and cut1");
    c_z_xq2_cut1->Update();
    std::string c_z_xq2_cut1_name = "results/csv_kin/kin_acceptance/z_xq2cut_cut1_"+std::to_string(Rungroup)+".png";
    c_z_xq2_cut1->SaveAs(c_z_xq2_cut1_name.c_str());
   
    h_z_cut1_neg->Scale(1/scale_neg);

    TCanvas *c_z_cut3_cut1 = new TCanvas();
    cut3_integral = h_z_cut3_neg->Integral();
    scale_neg = cut3_integral/cut1_integral;
    //h_z_cut3->DrawCopy();
    h_z_cut1_neg->Scale(scale_neg);
    //h_z_cut1->DrawCopy("same");
    auto rp_z_cut3_cut1 = new TRatioPlot(h_z_cut3_neg,h_z_cut1_neg);
    c_z_cut3_cut1->SetTicks(0,1);
    rp_z_cut3_cut1->Draw();
    rp_z_cut3_cut1->GetLowerRefGraph()->SetMinimum(0.5);
    rp_z_cut3_cut1->GetLowerRefGraph()->SetMaximum(1.5);
    c_z_cut3_cut1->SetName("compare cut3 and cut1");
    c_z_cut3_cut1->Update();
    std::string c_z_cut3_cut1_name = "results/csv_kin/kin_acceptance/z_cut3_cut1_"+std::to_string(Rungroup)+".png";
    c_z_cut3_cut1->SaveAs(c_z_cut3_cut1_name.c_str());

}
