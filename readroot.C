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


int readroot(){
 std::string rootfile = std::string("ROOTfiles/coin_replay_production_7822_-1.root");
 ROOT::RDataFrame d0("T",rootfile);
 auto d = d0.Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
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
 .Define("Emiss",Emiss,{"p_proton","p_electron"})
 .Define("protonEnergy","p_proton.E()")
 .Define("electronEnergy","p_electron.E()")
 ;

  auto h_H_etottracknorm = d.Histo1D({"hmsetottracknorm","hmsEoverP",100,0.1,2},"H.cal.etottracknorm");
  auto h_H_index = d.Histo1D({"hindex","hindex",100,-1,1},"H.gtr.index");
  auto h_P_index = d.Histo1D({"pindex","pindex",100,-1,1},"P.gtr.index");
  auto h_Emiss = d.Histo1D({"MissingE","Emiss",100,-0.2,0.2},"Emiss");
  auto h_W = d.Histo1D({"W","W",100,0,5},"W");
  //auto all = h_P_etottracknorm->Integral();
  auto h_h_px = d.Histo1D({"hmspx","hms momentum x",100,-5,5},"H.gtr.px");
  auto h_s_E = d.Histo1D({"protonEnergy","proton energy",100,0,20},"protonEnergy");
  auto h_h_E = d.Histo1D({"electron Energy","electron energy",100,0,20},"electronEnergy");
  auto *c = new TCanvas;
  c->Divide(2,2);
  c->cd(1);
  h_h_E->DrawCopy();
  //h_H_etottracknorm->DrawCopy();
  c->cd(2);
  h_W->DrawCopy();
  c->cd(3);
  h_s_E->DrawCopy();
//  h_h_px->DrawCopy();
 // h_P_index->DrawCopy();
  c->cd(4);
  h_Emiss->DrawCopy();
  return 0;
}
