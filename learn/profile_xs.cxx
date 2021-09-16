#include "ROOT/RDataFrame.hxx"
#include <vector>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TProfile.h"
#include "TF1.h"
#include "TH1.h"
#include "TLatex.h"
#include "TPaveText.h"
void profile_xs(){
  std::string rootfile_name = "ROOTfiles/kinematics_yield_510_H2_simc.root";
  TFile *rootfile_neg_sim = new TFile(rootfile_name.c_str());
  TH2D *h_xs_z_neg_H2_sim = new TH2D("","pi- sim SIDIS",100,0,1,100,0,1);
  h_xs_z_neg_H2_sim = (TH2D*)rootfile_neg_sim->Get("xs_z_neg_inc_rad");
  auto xs_z_neg = h_xs_z_neg_H2_sim->ProfileX("h1",1,-1,"d");
  double xs = xs_z_neg->GetBinContent(50);
  int bincenter = xs_z_neg->GetXaxis()->FindBin(0.5);
  std::cout<<xs<<" "<<bincenter<<std::endl;
  TCanvas *c_xs = new TCanvas();
  xs_z_neg->Draw();
  c_xs->SaveAs("results/xs.pdf");
}
