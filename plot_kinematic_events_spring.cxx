#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


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

void plot_kinematic_events_spring(int RunNumber = 0){
  //std::cout<<"check"<<std::endl;
  //if (RunNumber == 0){
  //   std::cout<<"Enter a Run Number (-1 to exit):";
  //   std::cin>>RunNumber;
  //   if(RunNumber<=0)
  //     return;
  // }
  TFile* root = new TFile("results/csv_kin/kinematics_spring.root","RECREATE");
  std::ifstream infile("db2/spring_good_runlist.txt");
  while(infile>>RunNumber){
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d("T",rootfile_name.c_str());
    auto d_coin = d.Filter("fEvtHdr.fEvtType == 4");
    //auto d_coin = d;
    //cuts
    std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
    std::string goodTrackHMS = "H.gtr.dp>-8 && H.gtr.dp<8";
    // std::string piCerSHMS = "P.ngcer.npeSum<0.5 && P.aero.npeSum>1.0";
    std::string piCerSHMS = "P.aero.npeSum>1.0";
    std::string piCalSHMS = "P.cal.etottracknorm <1.0 && P.cal.eprtracknorm<0.2";
    std::string piCutSHMS = piCerSHMS +" && "+ piCalSHMS;
    std::string eCutHMS = "H.cal.etottracknorm >0.80 && H.cal.etottracknorm<2 && H.cer.npeSum>1";
    auto dHMSGoodTrack = d_coin.Filter(goodTrackHMS);
    auto dCoinGoodTrack = dHMSGoodTrack.Filter(goodTrackSHMS)
    .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_q", p_q, {"p_electron"})
    .Define("z", z, {"p_q","p_pion"})
    .Define("Q2", Q2, {"p_q"})
    .Define("xbj", xbj, {"Q2","p_q"});
    auto dHMS_electron = dCoinGoodTrack.Filter(eCutHMS);
    auto dSHMS_pion = dCoinGoodTrack.Filter(piCutSHMS);
    auto dCOIN_sidis = dCoinGoodTrack.Filter(eCutHMS + " && " + piCutSHMS);

    //   //Timing cuts
    //   //Find the timing peak
    //   //Find the coin peak
    //   double coin_peak_center;
    //   auto h_coin_time = dCOIN_sidis.Histo1D({"coin_time","coin_time",8000,0,1000},"CTime.ePositronCoinTime_ROC2");
    //   h_coin_time->DrawClone();
    //   int coin_peak_bin = h_coin_time->GetMaximumBin();
    //   coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);
    //   std::cout<<"COINCIDENCE time peak found at: "<<coin_peak_center<<std::endl;
    //   //timing cut
    //   auto timing_cut = [=](double coin_time){return std::abs(coin_time - coin_peak_center)<2.;};
    //   auto dCoin_sidis_pion_intime = dCOIN_sidis.Filter(timing_cut);
    auto kin_x_Q2 = dCOIN_sidis.Histo2D({"CSV_KIN","CSV kinematics",400,0,1,400,0,10},"xbj","Q2");
    //auto kin_x_Q2 = dCoin_sidis_pion_intime.Histo2D({"CSV_KIN","CSV kinematics",400,0.3,0.7,400,2,8},"xbj","Q2");
    kin_x_Q2->GetXaxis()->SetTitle("x");
    kin_x_Q2->GetYaxis()->SetTitle("Q2");
    auto c = new TCanvas("CSV kinematics");
    kin_x_Q2->DrawCopy();
    std::string name = "results/csv_kin/kinematics_"+std::to_string(RunNumber)+".pdf";
    c->SaveAs(name.c_str());
    kin_x_Q2->Write(std::to_string(RunNumber).c_str());
  
  //  auto kin_x_Q2_ori = dCOIN_sidis.Histo2D({"csv_kin","csv_kin_root",400,0,1,400,0,10},"H.kin.primary.x_bj","H.kin.primary.Q2");
  //kin_x_Q2_ori->GetXaxis()->SetTitle("x");
  //kin_x_Q2_ori->GetYaxis()->SetTitle("Q2");
  //auto c_xq2ori = new TCanvas("x_Q2_root");
  //kin_x_Q2_ori->DrawCopy();
  //std::string name_xq2ori = "results/csv_kin/kinematics_xq2_"+std::to_string(RunNumber)+".pdf";
  //c->SaveAs(name_xq2ori.c_str());
  //std::string name_xq2ori_root = "xq2root_"+std::to_string(RunNumber);
  //kin_x_Q2_ori->Write(name_xq2ori_root.c_str());

  auto kin_x_z = dCOIN_sidis.Histo2D({"x_z","x_z",400,0,3,400,0,1},"z","xbj");
  kin_x_z->GetXaxis()->SetTitle("z");
  kin_x_z->GetYaxis()->SetTitle("x");
  auto c_x_z = new TCanvas("CSV x vs. z");
  kin_x_z->DrawCopy();
  std::string name_xz = "results/csv_kin/kinematics_xz_"+std::to_string(RunNumber)+".pdf";
  c_x_z->SaveAs(name_xz.c_str());
  std::string name_xz_root = "xz_"+std::to_string(RunNumber);
  kin_x_z->Write(name_xz_root.c_str());

  auto kin_Q2_z = dCOIN_sidis.Histo2D({"Q2_z","Q2_z",400,0,3,400,0,10},"z","Q2");
  kin_Q2_z->GetXaxis()->SetTitle("z");
  kin_Q2_z->GetYaxis()->SetTitle("Q2");
  auto c_Q2_z = new TCanvas("CSV Q2 vs. z");
  kin_Q2_z->DrawCopy();
  std::string name_Q2z = "results/csv_kin/kinematics_q2z_"+std::to_string(RunNumber)+".pdf";
  c_Q2_z->SaveAs(name_Q2z.c_str());
  std::string name_Q2z_root = "q2z_"+std::to_string(RunNumber);
  kin_Q2_z->Write(name_Q2z_root.c_str());
  
  }
  root->Close();

}
