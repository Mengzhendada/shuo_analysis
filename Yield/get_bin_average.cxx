#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "Get_all_eff.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void get_bin_average(){
  
  int RunNumber = 6194; 
  //std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
  std::string rootfile_name = "ROOTfiles/"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d_pos_raw("T",rootfile_name);
  ROOT::RDataFrame d_pos_bg_raw("T",rootfile_name);
  double all = *d_pos_raw.Count();
  std::cout<<"all "<<all<<std::endl;
  auto divide_by_all = [&](double value){return value/all;};
  auto d_pos_pi = d_pos_raw.Define("xbjmean",divide_by_all,{"xbj"})
    .Define("zmean",divide_by_all,{"z"})
    ;
  auto h_x_pos = d_pos_pi.Histo1D({"","",20,0,1},"xbj");
  auto h_x_pos_bins = d_pos_pi.Histo1D({"","",100,0,1},"xbj");
  auto h_weight_xbj_1 = d_pos_pi.Histo1D({"","",20,0,1},"xbj","xbj");
  h_weight_xbj_1->Divide(h_x_pos.GetPtr());
  std::cout<<h_x_pos->FindBin(0.36)<<" "<<h_x_pos->GetBinContent(h_x_pos->FindBin(0.36))<<" "<<h_x_pos->GetBinCenter(h_x_pos->FindBin(0.36))<<std::endl;
  auto h_x_pos_bg = d_pos_bg_raw.Histo1D({"","",20,0,1},"xbj");
  //h_x_pos->Rebin(5);
  auto get_weight = [&](double x){
     int binNumber = h_x_pos->FindBin(x);
     double binCenter = h_x_pos->GetBinCenter(binNumber);
     double pi_bincontent = h_x_pos->GetBinContent(binNumber);
     double bg_bincontent = h_x_pos_bg->GetBinContent(binNumber);
     double bincontent = h_x_pos->GetBinContent(binNumber)-h_x_pos_bg->GetBinContent(binNumber)/6.0;
     double all = h_x_pos->GetEntries()-h_x_pos_bg->GetEntries()/6.0;
     //return x*(bincontent/pi_bincontent)/all;
     //return x/(bincontent*pi_bincontent*all);
     //return x*bincontent/(pi_bincontent*pi_bincontent);//*all);
     //return bincontent*x/(all);
     //*pi_bincontent);
     //return x*bincontent/(all*pi_bincontent);
     //return x/(bincontent*pi_bincontent);
     return x/pi_bincontent;
  };
  auto get_ratio_weight = [&](double x){
     int binNumber = h_x_pos->FindBin(x);
     double binCenter = h_x_pos->GetBinCenter(binNumber);
     double pi_bincontent = h_x_pos->GetBinContent(binNumber);
     double bg_bincontent = h_x_pos_bg->GetBinContent(binNumber);
     double bincontent = h_x_pos->GetBinContent(binNumber)-h_x_pos_bg->GetBinContent(binNumber)/6.0;
     double all = h_x_pos->GetEntries()-h_x_pos_bg->GetEntries()/6.0;
     //return x*bincontent/(all*pi_bincontent*binCenter);
     //return x/(bincontent*pi_bincontent*binCenter);
     //return x/(bincontent*pi_bincontent*binCenter*all);
     //return x*bincontent/(pi_bincontent*pi_bincontent*binCenter);
     //return x*bincontent/(pi_bincontent*pi_bincontent*all*binCenter);
     return x/(pi_bincontent*binCenter);
  
  };
  auto d_pos_pi_after = d_pos_pi
    .Define("weight_xbj",get_weight,{"xbj"})
    .Define("weight_xbj_ratio",get_ratio_weight,{"xbj"})
    ;
  auto h_weight_xbj = d_pos_pi_after.Histo1D({"","",20,0,1},"xbj","weight_xbj");
  auto h_weight_xbj_ratio = d_pos_pi_after.Histo1D({"","",20,0,1},"xbj","weight_xbj_ratio");
  std::cout<<h_weight_xbj->FindBin(0.33)<<" "<<h_weight_xbj->GetBinContent(h_weight_xbj->FindBin(0.33))<<" "<<h_weight_xbj->GetBinCenter(h_weight_xbj->FindBin(0.33))<<" "<<h_weight_xbj_ratio->GetBinContent(h_weight_xbj_ratio->FindBin(0.33))<<std::endl;
  //h_weight_xbj->GetYaxis()->SetRangeUser(0.9,1.1);
  std::cout<<"check"<<std::endl;
  //h_x_pos->Rebin(5);
  //h_weight_xbj->Rebin(5);
  //h_weight_xbj->Scale(1.0/5);
  //h_weight_xbj_ratio->Rebin(5);
  //h_weight_xbj_ratio->Scale(1.0/5);
  //auto h_x_pos_mean = d_pos_pi_after.Histo1D({"","",100,0,1},"xbj","weight_xbj");
  TCanvas *c_check_1d = new TCanvas();
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleFontSize(.08);
  gStyle->SetLabelSize(.05, "XY");

  h_x_pos->SetBit(TH1::kNoStats);
  h_x_pos->GetXaxis()->SetRangeUser(0.3,0.6);
  h_x_pos->GetXaxis()->SetTitle("x_{bj}");
  h_x_pos->GetXaxis()->SetTitleSize(0.05);
  h_x_pos->Draw();
  h_x_pos_bins->Scale(5);
  TGraph* g_x_pos_bins = new TGraph(h_x_pos_bins.GetPtr());
  g_x_pos_bins->SetLineColor(kRed);
  g_x_pos_bins->Draw("same");
  std::cout<<h_x_pos->GetBinContent(10)<<std::endl;
  TLine* line_2 = new TLine(0.475,0,0.475,h_x_pos->GetBinContent(10));
  line_2->SetLineStyle(3);
  line_2->Draw("same");
  double xave = h_weight_xbj->GetBinContent(10);
  TLine* line_ave = new TLine(xave,0,xave,h_x_pos->GetBinContent(10));
  line_ave->SetLineStyle(2);
  line_ave->SetLineColor(kRed);
  line_ave->Draw("same");
  //h_x_pos_bg->SetLineColor(kRed);
  //h_x_pos_bg->Scale(1/6.0);
  //h_x_pos_bg->Draw("same");
  c_check_1d->SaveAs("6194_xbj.pdf");
  TCanvas *c_check_1d_2 = new TCanvas();
  h_weight_xbj->SetBit(TH1::kNoStats);
  h_weight_xbj->GetXaxis()->SetRangeUser(0.3,0.6);
  h_weight_xbj->GetXaxis()->SetTitle("x_{bj,center}");
  h_weight_xbj->GetYaxis()->SetTitle("x_{bj,average}");
  h_weight_xbj->GetXaxis()->SetTitleSize(0.05);
  h_weight_xbj->GetYaxis()->SetTitleSize(0.05);
  h_weight_xbj->Draw();
  h_weight_xbj_1->SetLineColor(kRed);
  h_weight_xbj_1->Draw("hist same");
  //h_x_pos_mean->Draw();
  c_check_1d_2->SaveAs("6194_xbjmean_xbj.pdf");
  TCanvas *c_check_1d_3 = new TCanvas();
  h_weight_xbj_1->Draw();
  //h_weight_xbj_ratio->SetBit(TH1::kNoStats);
  //h_weight_xbj_ratio->GetXaxis()->SetRangeUser(0.3,0.6);
  //h_weight_xbj_ratio->GetXaxis()->SetTitle("x_{bj,center}");
  //h_weight_xbj_ratio->GetYaxis()->SetTitle("ratio");
  //h_weight_xbj_ratio->GetXaxis()->SetTitleSize(0.05);
  //h_weight_xbj_ratio->GetYaxis()->SetTitleSize(0.05);
  //h_weight_xbj_ratio->Draw();
  //h_x_pos_mean->Draw();
  c_check_1d_3->SaveAs("6194_xbjmean_xbj_ratio.pdf");
  

  auto h_x_z_pos = d_pos_pi.Histo2D({"x_z","x_z",100,0,1,100,0,1},"z","xbj","weight");
  //auto h_x_z_mean = d_pos_pi.Histo2D({"x_z_mean","x_z_mean",100,0,1,100,0,1},"zmean","xbjmean","weight");
  std::cout<<"check"<<std::endl;
  double xbincenter = h_x_z_pos->GetXaxis()->GetBinCenter(75);
  double xbincontent = h_x_z_pos->GetBinContent(75,46);
  //double xbincontent_mean = h_x_z_mean->GetBinContent(75,46);
  std::cout<<xbincenter<<std::endl;
  //std::cout<<xbincontent<<" "<<xbincontent_mean<<std::endl;
  auto h_x_z_prof = d_pos_pi.Profile2D({"","",100,0,1,100,0,1},"z","xbj","weight");
  double xbincenter_prof = h_x_z_prof->GetXaxis()->GetBinCenter(75);
  std::cout<<xbincenter_prof<<std::endl;
  TCanvas *c_check = new TCanvas();
  c_check->Divide(2,1);
  c_check->cd(1);
  h_x_z_pos->Draw();
  c_check->cd(2);
  //h_x_z_mean->Draw("colz");
  c_check->SaveAs("check.pdf");  
  
}
