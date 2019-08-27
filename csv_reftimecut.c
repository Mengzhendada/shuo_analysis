#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <fstream>
#include <vector>
#include <string>

bool root_file_exists(std::string rootfile) {
  bool found_good_file = false;
  if (!gSystem->AccessPathName(rootfile.c_str())) {
    TFile file(rootfile.c_str());
    if (file.IsZombie()) {
      std::cout << rootfile << " is a zombie.\n";
      std::cout
          << " Did your replay finish?  Check that the it is done before running this script.\n";
      return false;
      // return;
    } else {
      std::cout << " using : " << rootfile << "\n";
      return true;
    }
  }
  return false;
}


void csv_reftimecut(){
std::string rootfile = std::string("../rootfiles/coin_replay_production_7822_-1.root");
if(!root_file_exists(rootfile)){std::cout<<rootfile<<" not found!"<<std::endl;}
ROOT::RDataFrame d0("T",rootfile);
auto d = d0;
auto h_multiplicity = d.Histo1D({"Multiplicity","T.coin.pDCREF1_tdcMultiplicity",20,0,20},"T.coin.pDCREF1_tdcMultiplicity");

TCanvas *c_multiplicity = new TCanvas();
h_multiplicity->DrawCopy();
c_multiplicity->SaveAs("csv_reftimecut/multiplicity.pdf");
//Multiplicity cut
std::string multiplicity_cut = "T.coin.pDCREF1_tdcMultiplicity >1";

auto d_cut = d.Filter(multiplicity_cut);

auto h_hodo_adc_nocut = d.Histo1D({"hFADC_TREF_ROC1","h hodo adc",100,5000,7000},"T.coin.hFADC_TREF_ROC1_adcPulseTimeRaw");
auto h_trig_tdc_nocut = d.Histo1D({"hTref1","h trig tdc",100,3500,5000},"T.coin.hT1_tdcTimeRaw");
auto h_hodo_tdc_nocut = d.Histo1D({"hTref2","h hodo tdc",100,3500,5000},"T.coin.hT2_tdcTimeRaw");
auto h_dc_tdc_1_nocut = d.Histo1D({"hDCREF1","h dc tdc",100,15000,18000},"T.coin.hDCREF1_tdcTimeRaw");
auto h_dc_tdc_5_nocut = d.Histo1D({"hDCREF5","h dc tdc",100,15000,18000},"T.coin.hDCREF5_tdcTimeRaw");

auto h_hodo_adc_cut = d_cut.Histo1D({"hFADC_TREF_ROC1","Multipicity Cut",100,5000,7000},"T.coin.hFADC_TREF_ROC1_adcPulseTimeRaw");
auto h_trig_tdc_cut = d_cut.Histo1D({"hTref1","Multipicity Cut",100,3500,5000},"T.coin.hT1_tdcTimeRaw");
auto h_hodo_tdc_cut = d_cut.Histo1D({"hTref2","Multipicity Cut",100,3500,5000},"T.coin.hT2_tdcTimeRaw");
auto h_dc_tdc_1_cut = d_cut.Histo1D({"hDCREF1","Multipicity Cut",100,15000,18000},"T.coin.hDCREF1_tdcTimeRaw");
auto h_dc_tdc_5_cut = d_cut.Histo1D({"hDCREF5","Multipicity Cut",100,15000,18000},"T.coin.hDCREF5_tdcTimeRaw");

auto p_hodo_adc_nocut = d.Histo1D({"pFADC_TREF_ROC2","p hodo adc",100,5000,8000},"T.coin.pFADC_TREF_ROC2_adcPulseTimeRaw");
auto p_trig_tdc_nocut = d.Histo1D({"pTref2","p trig tdc",100,4000,7000},"T.coin.pT2_tdcTimeRaw");
auto p_hodo_tdc_nocut = d.Histo1D({"pTref1","p hodo tdc",100,4000,7000},"T.coin.pT1_tdcTimeRaw");
auto p_dc_tdc_1_nocut = d.Histo1D({"pDCREF1","p dc tdc 1",100,14000,17000},"T.coin.pDCREF1_tdcTimeRaw");
auto p_dc_tdc_2_nocut = d.Histo1D({"pDCREF2","p dc tdc 2",100,14000,17000},"T.coin.pDCREF2_tdcTimeRaw");
auto p_dc_tdc_3_nocut = d.Histo1D({"pDCREF3","p dc tdc 3",100,14000,17000},"T.coin.pDCREF3_tdcTimeRaw");
auto p_dc_tdc_4_nocut = d.Histo1D({"pDCREF4","p dc tdc 4",100,14000,17000},"T.coin.pDCREF4_tdcTimeRaw");
auto p_dc_tdc_5_nocut = d.Histo1D({"pDCREF5","p dc tdc 5",100,14000,17000},"T.coin.pDCREF5_tdcTimeRaw");
auto p_dc_tdc_6_nocut = d.Histo1D({"pDCREF6","p dc tdc 6",100,14000,17000},"T.coin.pDCREF6_tdcTimeRaw");
auto p_dc_tdc_7_nocut = d.Histo1D({"pDCREF7","p dc tdc 7",100,14000,17000},"T.coin.pDCREF7_tdcTimeRaw");
auto p_dc_tdc_8_nocut = d.Histo1D({"pDCREF8","p dc tdc 8",100,14000,17000},"T.coin.pDCREF8_tdcTimeRaw");
auto p_dc_tdc_9_nocut = d.Histo1D({"pDCREF9","p dc tdc 9",100,14000,17000},"T.coin.pDCREF9_tdcTimeRaw");
auto p_dc_tdc_10_nocut = d.Histo1D({"pDCREF10","p dc tdc 10",100,14000,17000},"T.coin.pDCREF10_tdcTimeRaw");

auto p_hodo_adc_cut = d_cut.Histo1D({"pFADC_TREF_ROC2","Multiplcity Cut",100,5000,8000},"T.coin.pFADC_TREF_ROC2_adcPulseTimeRaw");
auto p_trig_tdc_cut = d_cut.Histo1D({"pTref2","Multiplcity Cut",100,4000,7000},"T.coin.pT2_tdcTimeRaw");
auto p_hodo_tdc_cut = d_cut.Histo1D({"pTref1","Multiplcity Cut",100,4000,7000},"T.coin.pT1_tdcTimeRaw");
auto p_dc_tdc_1_cut = d_cut.Histo1D({"pDCREF1","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF1_tdcTimeRaw");
auto p_dc_tdc_2_cut = d_cut.Histo1D({"pDCREF2","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF2_tdcTimeRaw");
auto p_dc_tdc_3_cut = d_cut.Histo1D({"pDCREF3","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF3_tdcTimeRaw");
auto p_dc_tdc_4_cut = d_cut.Histo1D({"pDCREF4","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF4_tdcTimeRaw");
auto p_dc_tdc_5_cut = d_cut.Histo1D({"pDCREF5","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF5_tdcTimeRaw");
auto p_dc_tdc_6_cut = d_cut.Histo1D({"pDCREF6","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF6_tdcTimeRaw");
auto p_dc_tdc_7_cut = d_cut.Histo1D({"pDCREF7","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF7_tdcTimeRaw");
auto p_dc_tdc_8_cut = d_cut.Histo1D({"pDCREF8","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF8_tdcTimeRaw");
auto p_dc_tdc_9_cut = d_cut.Histo1D({"pDCREF9","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF9_tdcTimeRaw");
auto p_dc_tdc_10_cut = d_cut.Histo1D({"pDCREF10","Multiplcity Cut",100,14000,17000},"T.coin.pDCREF10_tdcTimeRaw");

auto *c_h_hodo_adc = new TCanvas();
c_h_hodo_adc->SetLogy();
h_hodo_adc_nocut->SetLineColor(kGreen);
h_hodo_adc_nocut->DrawCopy();
h_hodo_adc_cut->SetLineColor(kBlue);
h_hodo_adc_cut->DrawCopy("same");
c_h_hodo_adc->BuildLegend();
c_h_hodo_adc->SaveAs("csv_reftimecut/h_hodo_adc.pdf");

auto *c_h_trig_tdc = new TCanvas();
c_h_trig_tdc->SetLogy();
h_trig_tdc_nocut->SetLineColor(kGreen);
h_trig_tdc_nocut->DrawCopy();
h_trig_tdc_cut->SetLineColor(kBlue);
h_trig_tdc_cut->DrawCopy("same");
c_h_trig_tdc->BuildLegend();
c_h_trig_tdc->SaveAs("csv_reftimecut/h_trig_tdc.pdf");

auto *c_hodo_tdc = new TCanvas();
c_hodo_tdc->SetLogy();
h_hodo_tdc_nocut->SetLineColor(kGreen);
h_hodo_tdc_nocut->DrawCopy();
h_hodo_tdc_cut->SetLineColor(kBlue);
h_hodo_tdc_cut->DrawCopy("same");
c_hodo_tdc->BuildLegend();
c_hodo_tdc->SaveAs("csv_reftimecut/h_hodo_tdc.pdf");

auto *c_dc_tdc = new TCanvas();
c_dc_tdc->SetLogy();
h_dc_tdc_1_nocut->SetLineColor(kGreen);
h_dc_tdc_1_nocut->DrawCopy();
h_dc_tdc_5_nocut->SetLineColor(kGreen);
h_dc_tdc_5_nocut->DrawCopy("same");
h_dc_tdc_1_cut->SetLineColor(kBlue);
h_dc_tdc_1_cut->DrawCopy("same");
h_dc_tdc_5_cut->SetLineColor(kBlue);
h_dc_tdc_5_cut->DrawCopy("same");
c_dc_tdc->BuildLegend();
c_dc_tdc->SaveAs("csv_reftimecut/h_dc_tdc.pdf");

auto *c_p_hodo_adc = new TCanvas();
c_p_hodo_adc->SetLogy();
p_hodo_adc_nocut->SetLineColor(kGreen);
p_hodo_adc_nocut->DrawCopy();
p_hodo_adc_cut->SetLineColor(kBlue);
p_hodo_adc_cut->DrawCopy("same");
c_p_hodo_adc->BuildLegend();
c_p_hodo_adc->SaveAs("csv_reftimecut/p_hodo_adc.pdf");

auto *c_p_trig_tdc = new TCanvas();
c_p_trig_tdc->SetLogy();
p_trig_tdc_nocut->SetLineColor(kGreen);
p_trig_tdc_nocut->DrawCopy();
p_trig_tdc_cut->SetLineColor(kBlue);
p_trig_tdc_cut->DrawCopy("same");
c_p_trig_tdc->BuildLegend();
c_p_trig_tdc->SaveAs("csv_reftimecut/p_trig_tdc.pdf");

auto *c_p_hodo_tdc = new TCanvas();
c_p_hodo_tdc->SetLogy();
p_hodo_tdc_nocut->SetLineColor(kGreen);
p_hodo_tdc_nocut->DrawCopy();
p_trig_tdc_nocut->SetLineColor(kGreen);
p_trig_tdc_nocut->DrawCopy("same");
p_trig_tdc_cut->SetLineColor(kBlue);
p_trig_tdc_cut->DrawCopy("same");
p_hodo_tdc_cut->SetLineColor(kBlue);
p_hodo_tdc_cut->DrawCopy("same");
c_p_hodo_tdc->BuildLegend();
c_p_hodo_tdc->SaveAs("csv_reftimecut/p_hodo_tdc.pdf");

auto *c_p_dc_tdc = new TCanvas();
c_p_dc_tdc->SetLogy();
p_dc_tdc_1_nocut->SetLineColor(kGreen);
p_dc_tdc_1_nocut->DrawCopy();
p_dc_tdc_2_nocut->SetLineColor(kGreen);
p_dc_tdc_2_nocut->DrawCopy("same");
p_dc_tdc_3_nocut->SetLineColor(kGreen);
p_dc_tdc_3_nocut->DrawCopy("same");
p_dc_tdc_4_nocut->SetLineColor(kGreen);
p_dc_tdc_4_nocut->DrawCopy("same");
p_dc_tdc_5_nocut->SetLineColor(kGreen);
p_dc_tdc_5_nocut->DrawCopy("same");
p_dc_tdc_6_nocut->SetLineColor(kGreen);
p_dc_tdc_6_nocut->DrawCopy("same");
p_dc_tdc_7_nocut->SetLineColor(kGreen);
p_dc_tdc_7_nocut->DrawCopy("same");
p_dc_tdc_8_nocut->SetLineColor(kGreen);
p_dc_tdc_8_nocut->DrawCopy("same");
p_dc_tdc_9_nocut->SetLineColor(kGreen);
p_dc_tdc_9_nocut->DrawCopy("same");
p_dc_tdc_10_nocut->SetLineColor(kGreen);
p_dc_tdc_10_nocut->DrawCopy("same");
p_dc_tdc_1_cut->SetLineColor(kBlue);
p_dc_tdc_1_cut->DrawCopy("same");
p_dc_tdc_2_cut->SetLineColor(kBlue);
p_dc_tdc_2_cut->DrawCopy("same");
p_dc_tdc_3_cut->SetLineColor(kBlue);
p_dc_tdc_3_cut->DrawCopy("same");
p_dc_tdc_4_cut->SetLineColor(kBlue);
p_dc_tdc_4_cut->DrawCopy("same");
p_dc_tdc_5_cut->SetLineColor(kBlue);
p_dc_tdc_5_cut->DrawCopy("same");
p_dc_tdc_6_cut->SetLineColor(kBlue);
p_dc_tdc_6_cut->DrawCopy("same");
p_dc_tdc_7_cut->SetLineColor(kBlue);
p_dc_tdc_7_cut->DrawCopy("same");
p_dc_tdc_8_cut->SetLineColor(kBlue);
p_dc_tdc_8_cut->DrawCopy("same");
p_dc_tdc_9_cut->SetLineColor(kBlue);
p_dc_tdc_9_cut->DrawCopy("same");
p_dc_tdc_10_cut->SetLineColor(kBlue);
p_dc_tdc_10_cut->DrawCopy("same");
c_p_dc_tdc->BuildLegend();
c_p_dc_tdc->SaveAs("csv_reftimecut/p_dc_tdc.pdf");

}
