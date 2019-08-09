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

#include "THStack.h"

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "monitor/DetectorDisplay.h"
#include "monitor/DisplayPlots.h"
#include "monitor/MonitoringDisplay.h"
//#include "monitor/ExperimentMonitor.h"
//#include "scandalizer/PostProcessors.h"
R__LOAD_LIBRARY(libScandalizer.so)

constexpr const double M_P = 0.938272;
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
  return Eb+M_P-sqrt(p_proton.E()*p_proton.E()+M_P2)-sqrt(p_electron.E()*p_electron.E()+M_e*M_e);
};



int simc_data(){
  std::string rootfile1 = std::string("ROOTfiles/coin_replay_production_7593_-1.root");
  //std::string rootfile2 = std::string("/u/home/shuojia/simc_gfortran/worksim/csv7593.root");
  std::string rootfile2 = std::string("/u/home/shuojia/simc_gfortran/worksim/csv7593_1.root");
   
  ROOT::RDataFrame d1_before("T",rootfile1);
  ROOT::RDataFrame d2_before("h666",rootfile2);
  /*double normfac = 60541000;
  double sim_charge = 10;
  */
  double data_charge = 81.665;
  double shmsTE = 0.9769;
  double hmsTE = 0.9859;
  double coinlive = 0.9998; 
  double normfac = 6054100;
  double sim_charge = 1;
  auto nentries = d2_before.Count();
  std::cout<<*nentries<<"\n";
  double wfac = (normfac/10)*(data_charge/sim_charge);
  std::cout<<wfac<<"\n";
  auto weightcalculate = [wfac, coinlive, shmsTE,hmsTE](float weight){return wfac*coinlive*shmsTE*hmsTE*weight;};
  
  std::string goodTrackSHMS = "P.gtr.dp > -10 && P.gtr.dp < 22";
  std::string goodTrackHMS  = "H.gtr.dp > -8 && H.gtr.dp < 8 ";
  ROOT::EnableImplicitMT(24);
  auto d1 = d1_before.Filter("fEvtHdr.fEvtType == 4")
  .Filter(goodTrackHMS)
  .Filter(goodTrackSHMS)
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

  auto d2=d2_before.Define("weight",weightcalculate,{"Weight"});
  
  
  
  
  auto h_pcaletottracknorm_1 = d1.Histo1D({"P.cal.etottracknorm","P.cal.etottracknorm",100,0.1,2},"P.cal.etottracknorm");
  //auto h_pcaletottracknorm_2 = d2.Histo1D({"P.cal.etottracknorm","P.cal.etottracknorm",100,0.1,2},"P.cal.etottracknorm","weight");
  auto h_hcaletottracknorm_1 = d1.Histo1D({"H.cal.etottracknorm","H.cal.etottracknorm",100,0.1,2},"H.cal.etottracknorm");
  //auto h_hcaletottracknorm_2 = d2.Histo1D({"H.cal.etottracknorm","H.cal.etottracknorm",100,0.1,2},"H.cal.etottracknorm","weight");
  auto h_gtr_dp_1 = d1.Histo1D({"H.gtr.dp","delta p",200,-30,40},"H.gtr.dp");
  //auto h_gtr_dp_2 = d2.Histo1D({"H.gtr.dp","delta p",200,-30,40},"H.gtr.dp","weight");
  auto h_W_1 = d1.Histo1D({"W","W",200,0,5},"W");
  auto h_W_2 = d2.Histo1D({"W","W",200,0,5},"W","weight");
  auto h_Q2_1 = d1.Histo1D({"Q2","Q2",200,0,8},"Q2");
  auto h_Q2_2 = d2.Histo1D({"Q2","Q2",200,0,8},"Q2","weight");
  auto h_xbj_1 = d1.Histo1D({"xbj","xbj",200,0,1},"xbj");
  auto h_Weight_2 = d2.Histo1D({"Weight","Weight",100,-0.1,0.1},"weight");
  TCanvas *c = new TCanvas;
  c->Divide(2,2);
  c->cd(1);
  h_Q2_1->DrawCopy();
  h_Q2_2->SetLineColor(2);
  h_Q2_2->DrawCopy("same");
  //h_pcaletottracknorm_1->DrawCopy();
  //h_pcaletottracknorm_2->DrawCopy("same");
  //h_pcaletottracknorm_2->SetLineColor(2);
  c->cd(2);
  h_xbj_1->DrawCopy();
  //h_hcaletottracknorm_1->DrawCopy();
  //h_hcaletottracknorm_2->DrawCopy("same");
  //h_hcaletottracknorm_2->SetLineColor(2);
  c->cd(3);
  h_Weight_2->DrawCopy();
  //h_gtr_dp_1->DrawCopy();
  //h_gtr_dp_2->DrawCopy("same");
  //h_gtr_dp_2->SetLineColor(2);
  c->cd(4);
  h_W_1->DrawCopy();
  h_W_2->SetLineColor(2);
  h_W_2->DrawCopy("same");
  c->SaveAs("result/simc_data.pdf");





  return 0;
}
