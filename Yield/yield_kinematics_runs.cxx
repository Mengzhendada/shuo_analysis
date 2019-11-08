#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

auto p_proton = [](double px, double py, double pz){
  return Pvec4D{px , py , pz , M_P};
};
auto p_pion = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_pion};
};
auto pion_momentum = [](double px,double py,double pz){
  TVector3 v(px,py,pz);
  return v;
};
auto p_electron = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_e};
};
auto p_q = [](Pvec4D& pe ) {
  return Pvec4D{0.0,0.0,10.6, M_e}-pe;
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

std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
std::string goodTrackHMS = "H.gtr.dp>-10 && H.gtr.dp<10";

std::string piCutSHMS = "P.aero.npeSum >1.0 && P.cal.eprtracknorm <0.2 && P.cal.etottracknorm<1.0";
std::string eCutHMS = "H.cal.etottracknorm >0.8 && H.cal.etottracknorm <2. && "
                       "H.cer.npeSum > 1.";
std::string epiCut = "P.aero.npeSum > 1.0 && P.cal.eprtracknorm <0.2 && "
                     "H.cer.npeSum > 1.0 && H.cal.etottracknorm > 0.6 && "
                     "H.cal.etottracknorm < 2.0 && P.cal.etottracknorm <1.0";

void yield_kinematics_runs(int RunNumber=0){
  
  if(RunNumber ==0){
    std::cout<<"Enter a RunNumber (-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<=0)
      return;
  }
 
  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d("T",rootfile_name);
  ROOT::RDataFrame d_scale("TSP",rootfile_name);
  double charge = *d_scale.Max("P.BCM4B.scalerChargeCut");
  std::cout<<charge<<std::endl;

  auto d_coin = d.Filter("fEvtHdr.fEvtType == 4");
  auto d_CoinGoodTrack = d_coin.Filter(goodTrackHMS)
  .Filter(goodTrackSHMS)
  ;
  auto d_Coin_sidis = d_CoinGoodTrack.Filter(epiCut)
      .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
      .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
      .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_q", p_q, {"p_electron"})
      .Define("z", z, {"p_q","p_pion"})
      .Define("Q2", Q2, {"p_q"})
      .Define("xbj", xbj, {"Q2","p_q"})
      .Define("W2",W2,{"p_q"})
      .Define("Wp2",Wprime2,{"p_q","p_pion"})
      .Define("W","std::sqrt(W2)")
      .Define("Wp","std::sqrt(Wp2)")
      ;
    
  std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
  TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
  auto h_xbj = d_Coin_sidis.Histo1D({"xbj","xbj;xbj;yield",100,0,1},"xbj");
  h_xbj->Scale(1/charge);
  h_xbj->Write();
  auto h_z = d_Coin_sidis.Histo1D({"z","z;z;yield",100,0,1},"z");
  h_z->Scale(1/charge);
  h_z->Write();
  rootfile_out->Close();
}
