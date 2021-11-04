
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>

void read_root_histo(){
  std::string rootfile_name = "ROOTfiles/kinematics_yield_6145.root";
  TFile *rootfile = new TFile(rootfile_name.c_str());
  TH2D *h_xz = new TH2D("","",100,0,1,100,0,1);
  h_xz = (TH2D*)rootfile->Get("weighted_xbj");
  //h_xz->RebinX(5);
  //h_xz->RebinY(5);
  //h_xz->Scale(1.0/25);
  TCanvas *c_check = new TCanvas();
  h_xz->Draw("LEGO");
  h_xz->GetZaxis()->SetRangeUser(0.96,1.04);
  h_xz->GetXaxis()->SetRangeUser(0.54,0.74);
  h_xz->GetXaxis()->SetTitle("z");
  h_xz->GetYaxis()->SetRangeUser(0.25,0.5);
  h_xz->GetYaxis()->SetTitle("x_{bj}");
  c_check->SaveAs("checkweighted_xbj.pdf");
}
