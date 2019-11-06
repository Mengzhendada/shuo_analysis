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

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <iostream>
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
constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
double Eb = 10.214;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

auto p_proton = [](double px, double py, double pz){
  return Pvec4D{px , py , pz , M_P};
};
auto p_pion = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_pion};
};
auto p_electron = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_e};
};
auto p_q = [](Pvec4D& pe ) {
  return Pvec4D{0.0,0.0,10.214, M_e}-pe;
};
auto t = [](const double Egamma, Pvec4D& jpsi) {
  Pvec4D beam{0, 0, Egamma, 0};
  return (beam - jpsi).M2();
};
auto z = [](Pvec4D& pq, Pvec4D& ph) {
  return ph.E()/pq.E();
};
auto xbj = [](double Q2,Pvec4D& pq) {
  return Q2/(2.0*0.938*pq.E());
};
auto Q2 = [](Pvec4D& pq) {
  return -1.0*(pq.Dot(pq));
};
auto Wprime2 = [](Pvec4D& pq,Pvec4D& ph) {
  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq - ph;
  return Ptot.Dot(Ptot);
};
auto W2 = [](Pvec4D& pq) {
  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq;
  return Ptot.Dot(Ptot);
};
//Emiss = Ebeam+Mp - sqrt(P_p^2 + Mp^2) - sqrt(P_e+Me^2)
auto Emiss = [](Pvec4D& p_proton, Pvec4D& p_electron){
  return Eb+M_P-p_proton.E()-p_electron.E();
};

void yield_vs_z(){
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("db2/kin33.json");
    json_input_file >> j;
  }
  
  TGraphErrors *G_yield_data = new TGraphErrors();
  G_yield_data->SetTitle("data");

  TGraphErrors *G_yield_run = new TGraphErrors();
  G_yield_run->SetTitle("runs");

  int i = 0; 
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    auto runjs = it.value();
    double charge = runjs["charge"].get<double>();
    int runnumber = std::stoi(it.key());
    double z_json = runjs["z"].get<double>();
    std::string rootfile_data = fmt::format("ROOTfiles/coin_replay_production_{}_-1.root",runnumber);
//    std::cout << rootfile_data << std::endl;


    if ( !root_file_exists(rootfile_data)){
      std::cout <<  rootfile_data << " not found !\n";
    }

    //modify data 
  std::string goodTrackSHMS = "P.gtr.dp > -10 && P.gtr.dp < 22";
  std::string goodTrackHMS  = "H.gtr.dp > -8 && H.gtr.dp < 8 ";
  ROOT::EnableImplicitMT(24);

  // Detector tree
  ROOT::RDataFrame d("T", rootfile_data);


  auto d_coin = d.Filter("fEvtHdr.fEvtType == 4");

  // Good "track" cuts
  auto dHMSGoodTrack  = d_coin.Filter(goodTrackHMS);
  auto dSHMSGoodTrack = d_coin.Filter(goodTrackSHMS);
  auto dCOINGoodTrack = dHMSGoodTrack.Filter(goodTrackSHMS)
 .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
 .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
 .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
 .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
 .Define("p_q", p_q, {"p_electron"})
 .Define("z", z, {"p_q","p_pion"})
 .Define("Q2", Q2, {"p_q"})
 .Define("xbj", xbj, {"Q2","p_q"})
 .Define("W2", W2, {"p_q"})
 .Define("Wp2", Wprime2, {"p_q","p_pion"})
 .Define("W", "std::sqrt(W2)")
 .Define("Wp", "std::sqrt(Wp2)")
 .Define("InvMass","p_electron.Dot(p_pion)")
 .Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
 .Define("emiss",Emiss,{"p_proton","p_electron"})
 ;
    auto yield_data_before = *dCOINGoodTrack.Count()/charge;
    std::cout<<"yield data before"<<yield_data_before<<std::endl;
    //building dataframe for data
    auto d1data = dCOINGoodTrack.Filter([](double etottracknorm){return etottracknorm>0.8;},{"H.cal.etottracknorm"})
    //.Filter([](double emiss){return  emiss<0.1 && emiss>-0.1;},{"emiss"})
    .Filter([](double hindex){return hindex>-1;},{"H.gtr.index"})
    .Filter([](double pindex){return pindex>-1;},{"P.gtr.index"})
    ;
    auto h_z = d1data.Histo1D({"z","z",100,0,1},"z");
    auto yield_data_after = *d1data.Count()/(charge);
    std::cout<<"yield data after"<<yield_data_after<<std::endl;
    G_yield_run->SetPoint(i,  runnumber, yield_data_after);
    G_yield_run->SetPointError(i, 0, std::sqrt(*d1data.Count())/charge);
    G_yield_data->SetPoint(i, z_json, yield_data_after);
    G_yield_data->SetPointError(i,0,std::sqrt(*d1data.Count())/charge);
    i++;
    TCanvas *c_kin = new TCanvas;
    h_z->DrawCopy();
    c_kin->SaveAs(fmt::format("csvresult/yieldvsz_z_{}.pdf",runnumber).c_str());
  }
TCanvas *c_yield = new TCanvas;
c_yield->Divide(2,1);
c_yield->cd(1);
G_yield_run->SetMarkerStyle(20);
G_yield_run->SetLineColor(2);
G_yield_run->Draw();
c_yield->cd(2);
G_yield_data->SetMarkerStyle(20);
G_yield_data->SetLineColor(2);
G_yield_data->Draw();

c_yield->SaveAs("csvresult/yieldvsz.pdf");
}
