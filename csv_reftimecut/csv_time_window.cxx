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


void csv_time_window(){
std::string rootfile = std::string("../rootfiles/coin_replay_production_6009_-1.root");
if(!root_file_exists(rootfile)){std::cout<<rootfile<<" not found!"<<std::endl;}
ROOT::RDataFrame d0("T",rootfile);
auto d = d0;
auto h_multiplicity = d.Histo1D({"Multiplicity","T.coin.pDCREF1_tdcMultiplicity",20,0,20},"T.coin.pDCREF1_tdcMultiplicity");

TCanvas *c_multiplicity = new TCanvas();
h_multiplicity->DrawCopy();
c_multiplicity->SaveAs("csv_time_window/multiplicity.pdf");
//Multiplicity cut
std::string multiplicity_cut = "T.coin.pDCREF1_tdcMultiplicity >1";

auto d_cut = d.Filter(multiplicity_cut);

