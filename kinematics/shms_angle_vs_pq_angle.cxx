#include "ROOT/RDataFrame.hxx"
#include "TMath.h"
#include "TVector3.h"
#include "TRotation.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

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

//constexpr const double M_P   = 0.938272;

auto pion_momentum = [](double px,double py,double pz){
  TVector3 v(px,py,pz);
  return v;
};
auto electron_momentum = [](double px, double py, double pz){
  TVector3 v(px, py, pz);
  return v;
};
auto q_momentum = [](TVector3 v_electron_prime){
  TVector3 v_0(0,0,10.6);
  return v_0 - v_electron_prime;
};

void shms_angle_vs_pq_angle(int RunNumber = 0){
  if(RunNumber ==0){
    std::cout<<"Enter a RunNumber (-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<=0)
      return;
  }
  double shms_angle;
  json j;
  std::string RunNumber_str = std::to_string(RunNumber);
  if(RunNumber<7000){
      std::ifstream infile("db2/run_list.json");
      infile>>j;
    if(j.find(RunNumber_str)==j.end()){
      std::cout<<"couldn't find in fall json"<<std::endl;
    }
    else{
    shms_angle = j[RunNumber_str]["spectrometers"]["shms_angle"].get<double>();
    }
  }
  else{
      std::ifstream infile("db2/run_list_update.json");
      infile>>j;
    if(j.find(RunNumber_str)==j.end()){
      std::cout<<"couldn't find in spring json"<<std::endl;
    }
    else{
    shms_angle = j[RunNumber_str]["spectrometers"]["shms_angle"].get<double>();
    }
  }
  TVector3 shms_p(-std::sin(shms_angle),0,std::cos(shms_angle));

  //prescale
  int ps1 = -1;
  int ps2 = -1;
  int ps5 = -1;
  int ps6 = -1;
  bool singles_trigger = true;
  if(j[RunNumber_str].find("daq") != j[RunNumber_str].end()){
    ps1 = j[RunNumber_str]["daq"]["ps1"].get<int>();
    ps2 = j[RunNumber_str]["daq"]["ps2"].get<int>();
    ps5 = j[RunNumber_str]["daq"]["ps5"].get<int>();
    ps6 = j[RunNumber_str]["daq"]["ps6"].get<int>();
  }  
  if(ps1 == -1){
    std::cout<<"No shms singles for run "<<RunNumber_str<<"!, use coin type instead"<<std::endl;
    singles_trigger = false;
  }


  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+RunNumber_str+"_"+RunNumber_str+".root";
  if(!root_file_exists(rootfile_name.c_str())){
    std::cout<<"Rootfile doesn't exists!"<<std::endl;
  }
 
  ROOT::RDataFrame d("T",rootfile_name.c_str());
 
// =================================================================================
// Cuts
// =================================================================================
std::string goodTrack = "P.gtr.dp > -10 && P.gtr.dp < 22 && P.tr.n == 1&&"
                        "-0.05 < P.gtr.th && P.gtr.th < 0.05 && "
                        "-0.035 < P.gtr.ph && P.gtr.ph < 0.025"
                        "&& P.gtr.y > -2.0 && P.gtr.y < 3.0";
std::string eCut = "P.cal.etottracknorm > 0.80 && P.cal.etottracknorm < 2.&&"
                   "P.ngcer.npeSum > 2.";

  std::string piCut = "P.aero.npeSum > 1.0 && P.cal.eprtracknorm < 0.2 && "
                       " P.cal.etottracknorm<1.0";
  std::string hgc_cut = " p_pion.P() < 2.8 || P.hgcer.npeSum > 1.0";

  auto d_SHMS = d.Filter(singles_trigger ? "fEvtHdr.fEvtType == 1" : "fEvtHdr.fEvtType == 4")
                 .Define("electron_p",electron_momentum,{"H.gtr.px","H.gtr.py","H.gtr.pz"})
                 .Define("q_p",q_momentum,{"electron_p"})
                 .Define("angle_diff",[shms_p](TVector3 q_p){return shms_p.Dot(q_p.Unit());},{"q_p"})
                 ;
  auto dGoodTrack = d_SHMS
                    .Filter(goodTrack)
                    ;
  auto dpi = dGoodTrack
  //.Filter(piCut)
  ;
  std::cout<<*dpi.Count()<<std::endl;
  
  auto h_angle_diff_singles = dpi.Histo1D({"","sin(angle) angle between singles q and shmsp",100,-0.1,0.2},"angle_diff");
  TCanvas *c_angle_diff_singles = new TCanvas();
  h_angle_diff_singles->DrawCopy();
  std::string c_singles_name = "results/csv_kin/kin_acceptance/angle_difference_"+RunNumber_str+".pdf";
  c_angle_diff_singles->SaveAs(c_singles_name.c_str());

  int ps4 = -1;
  singles_trigger = false;
  if(j[RunNumber_str].find("daq") != j[RunNumber_str].end()){
    ps4 = j[RunNumber_str]["daq"]["ps4"].get<int>();
  }  
  if(ps4 == -1){
    std::cout<<"No shms coin for run "<<RunNumber_str<<"!, use single type instead"<<std::endl;
    singles_trigger = true;
  }
  auto d_SHMS_coin = d.Filter(singles_trigger ? "fEvtHdr.fEvtType == 1" : "fEvtHdr.fEvtType == 4")
                 .Define("electron_p",electron_momentum,{"H.gtr.px","H.gtr.py","H.gtr.pz"})
                 .Define("q_p",q_momentum,{"electron_p"})
                 .Define("angle_diff",[shms_p](TVector3 q_p){return shms_p.Dot(q_p.Unit());},{"q_p"})
                 ;
  auto dGoodTrack_coin = d_SHMS_coin
                    .Filter(goodTrack)
                    ;
  auto dpi_coin = dGoodTrack_coin
  //.Filter(piCut)
  ;
  std::cout<<*dpi_coin.Count()<<std::endl;
  
  auto h_angle_diff_coin = dpi.Histo1D({"","sin(angle) angle between coin q and shmsp",100,-0.1,0.2},"angle_diff");
  TCanvas *c_angle_diff_coin = new TCanvas();
  h_angle_diff_coin->DrawCopy();
  std::string c_coin_name = "results/csv_kin/kin_acceptance/angle_difference_"+RunNumber_str+".pdf";
  c_angle_diff_coin->SaveAs(c_coin_name.c_str());
  
}
