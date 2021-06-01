#include "nlohmann/json.hpp"
using json = nlohmann::json;
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

#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

constexpr const double M_P = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
double Eb = 10.214;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

auto p_proton = [](double px, double py, double pz){
  return TLorentzVector{px , py , pz , M_P};
};
auto p_pion = [](double px, double py, double pz) {
  return TLorentzVector{px , py , pz , M_pion};
};
auto p_electron = [](double px, double py, double pz) {
  return TLorentzVector{px , py , pz , M_e};
};
auto p_q = [](TLorentzVector pe ) {
  return TLorentzVector{0.0,0.0,10.214, M_e}-pe;
};
//auto t = [](const double Egamma, Pvec4D& jpsi) {
//  Pvec4D beam{0, 0, Egamma, 0};
//  return (beam - jpsi).M2();
//};
auto z = [](TLorentzVector pq, TLorentzVector ph) {
  return ph.E()/pq.E();
};
auto xbj = [](double Q2,TLorentzVector pq) {
  return Q2/(2.0*0.938*pq.E());
};
auto Q2 = [](TLorentzVector pq) {
  return -1.0*(pq.Dot(pq));
};
auto Wprime2 = [](TLorentzVector pq,TLorentzVector ph) {
  auto Ptot = TLorentzVector{0.0,0.0,0.0, M_P} + pq - ph;
  return Ptot.Dot(Ptot);
};
auto W2 = [](TLorentzVector pq) {
  auto Ptot = TLorentzVector{0.0,0.0,0.0, M_P} + pq;
  return Ptot.Dot(Ptot);
};
//Emiss = Ebeam+Mp - sqrt(P_pion^2 + Mpion^2) - sqrt(P_e+Me^2)
auto Emiss = [](TLorentzVector& p_pion, TLorentzVector p_electron){
  return Eb+M_P-sqrt(p_pion.E()*p_pion.E())-sqrt(p_electron.E()*p_electron.E());
};
auto mmiss = [](TLorentzVector p_pion,TLorentzVector p_electron){
  TLorentzVector missing_mass = TLorentzVector{0.0,0.0,Eb,M_e}+TLorentzVector{0.0,0.0,0.0,M_P}-p_electron-p_pion;
  return std::sqrt(missing_mass.M2());
};
void compare_sim_data(int RunGroup = 0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit) ";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  std::string pos_sim_rootfile = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.root";
  std::string neg_sim_rootfile = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_rad.root";
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  json j_info;
  {
    std::ifstream ifs("db2/rungroup_info.json");
    ifs>>j_info;
  }
  if(!neg_D2.empty() && !pos_D2.empty()){
  std::vector<std::string> files_neg,files_pos;
  double SHMS_P = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};

  //for neg runs
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<"neg"<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_neg.push_back(rootfile_name);
  }
  //for pos runs
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<"pos"<<RunNumber<<std::endl;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    files_pos.push_back(rootfile_name);
  }

  ROOT::RDataFrame d_pos_raw("T",files_pos);
  auto d_pos = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
    .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
    .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    .Filter("H.cal.etottracknorm >0.85")
    .Filter("H.cer.npeSum>10")
    //.Filter("P.cal.etottracknorm > 0.1 && P.cal.etottracknorm < 0.8")
    .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
    .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
   // .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    //.Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_q", p_q, {"p_electron"})
    .Define("z", z, {"p_q","p_pion"})
    .Define("Q2", Q2, {"p_q"})
    .Define("xbj", xbj, {"Q2","p_q"})
    .Define("W2", W2, {"p_q"})
    .Define("Wp2", Wprime2, {"p_q","p_pion"})
    .Define("W", "std::sqrt(W2)")
    .Define("Wp", "std::sqrt(Wp2)")
    .Define("InvMass","p_electron.Dot(p_pion)")
    //.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
    .Define("emiss",Emiss,{"p_pion","p_electron"})
    .Define("mmiss",mmiss,{"p_pion","p_electron"})
    ;
  ROOT::RDataFrame d_neg_raw("T",files_neg);
  auto d_neg = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
    .Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
    .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
    .Filter("H.gtr.dp > -10 && H.gtr.dp < 10")
    .Filter("H.cal.etottracknorm >0.85")
    .Filter("H.cer.npeSum>10")
    //.Filter("P.cal.etottracknorm > 0.1 && P.cal.etottracknorm < 0.8")
    .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
    .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    //.Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    //.Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_q", p_q, {"p_electron"})
    .Define("z", z, {"p_q","p_pion"})
    .Define("Q2", Q2, {"p_q"})
    .Define("xbj", xbj, {"Q2","p_q"})
    .Define("W2", W2, {"p_q"})
    .Define("Wp2", Wprime2, {"p_q","p_pion"})
    .Define("W", "std::sqrt(W2)")
    .Define("Wp", "std::sqrt(Wp2)")
    .Define("InvMass","p_electron.Dot(p_pion)")
    //.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
    .Define("emiss",Emiss,{"p_pion","p_electron"})
    .Define("mmiss",mmiss,{"p_pion","p_electron"})
    ;
//coin time cut for pos runs
  auto h_cointime_pos = d_pos.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
  double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
  std::cout<<"coin time peak "<<coin_peak_center_pos<<std::endl;
  double coin_1stpeak_content = h_cointime_pos->GetBinContent(coin_peak_bin_pos);
  auto d_first = d_pos
                .Filter(
                  [=](double cointime){return std::abs(cointime-coin_peak_center_pos)<2;},{"CTime.ePiCoinTime_ROC2"});
  auto h_cointime_pos_first = d_first.Histo1D({"","first peak",800,0,100},"CTime.ePiCoinTime_ROC2");
  auto h_cointime_pos_2ndpeak = d_pos.Filter([=](double coin_time){return std::abs(coin_time - coin_peak_center_pos)>1.5;},{"CTime.ePiCoinTime_ROC2"}).Histo1D({"","",800,coin_peak_center_pos,100},"CTime.ePiCoinTime_ROC2");
  int coin_2ndpeak_bin_pos = h_cointime_pos_2ndpeak->GetMaximumBin();
  double coin_2ndpeak_center_pos = h_cointime_pos_2ndpeak->GetBinCenter(coin_2ndpeak_bin_pos);
  double coin_2ndpeak_content = h_cointime_pos_2ndpeak->GetBinContent(coin_2ndpeak_bin_pos);
  bool enough;
  if(std::abs(coin_peak_center_pos-coin_2ndpeak_center_pos)>2 && coin_2ndpeak_content>0.2*coin_1stpeak_content){enough = true;}
  else{enough = false;}
  std::cout<<"if we can separate two coin peaks(separation > 2)? "<<enough<<" with 2nd/1st peak height " <<coin_2ndpeak_content/coin_1stpeak_content<<std::endl;
  auto first_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_peak_center_pos)<2 : std::abs(coin_time-coin_peak_center_pos)<2 || std::abs(coin_time-coin_2ndpeak_center_pos)<3;};
  auto second_peak = [=](double coin_time){return enough ? std::abs(coin_time-coin_2ndpeak_center_pos)<3 : std::abs(coin_time-coin_peak_center_pos)<3 || std::abs(coin_time-coin_2ndpeak_center_pos)<3;};
  auto all_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_pos)<2 || std::abs(coin_time-coin_2ndpeak_center_pos)<3;};
    auto d_pos_first = d_pos.Filter(first_peak,{"CTime.ePiCoinTime_ROC2"});
  //rftime cut
    auto h_time_diff_pos = d_pos_first.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
  int time_diff_pos_bin_max = h_time_diff_pos->GetMaximumBin();
  double time_diff_pos_max = h_time_diff_pos->GetBinCenter(time_diff_pos_bin_max);
  double offset_pos = 401.8-time_diff_pos_max;
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_first = d_pos_first.Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
  ;
  auto h_diff_mod_pos = d_mod_first.Histo1D({"mod","mod pos",100,0,4.008},"diff_time_mod");
  json j_rfcut;
  {
    std::ifstream ifs("db2/rftime_cut.json");
    ifs>>j_rfcut;
  }
  double rf_pi_low = j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_low"].get<double>();
  std::cout<<rf_pi_low<<std::endl;
  double rf_pi_high =j_rfcut[(std::to_string(RunGroup)).c_str()]["rf_pi_high"].get<double>();
  std::cout<<rf_pi_high<<std::endl;
  auto d_pos_pi = d_mod_first.Filter(
      [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
    .Filter("P.aero.npeSum > 2")
    ; 
  auto h_2d_pos = d_pos_pi.Histo2D({"2d","2d pos",100,0,4.008,100,0,30},"diff_time_mod","P.aero.npeSum");

  //coin time cut for neg
  auto h_cointime_neg = d_neg.Histo1D({"","coin_time",800,40,55},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
  double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
  std::cout<<"coin time peak "<<coin_peak_center_neg<<std::endl;
  auto neg_peak = [=](double coin_time){return std::abs(coin_time-coin_peak_center_neg)<2;};
  auto d_neg_coin = d_neg.Filter(neg_peak,{"CTime.ePiCoinTime_ROC2"});
  
  //rftime cut for neg
  auto h_time_diff_neg = d_neg_coin.Histo1D({"h_rf_time","type4;rf_time",200,0,4.008},"fptime_minus_rf");
  int time_diff_neg_bin_max = h_time_diff_neg->GetMaximumBin();
  double time_diff_neg_max = h_time_diff_neg->GetBinCenter(time_diff_neg_bin_max);
  double offset_neg = 401.8-time_diff_neg_max;
  std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
  auto d_mod_neg = d_neg_coin.Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
  .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
  ;
  auto h_diff_mod_neg = d_mod_neg.Histo1D({"mod","mod neg",100,0,4.008},"diff_time_mod");
  auto d_neg_pi = d_mod_neg.Filter(
      [=](double difftime){return difftime < rf_pi_high && difftime > rf_pi_low;},{"diff_time_mod"})
    .Filter("P.aero.npeSum > 2")
    ; 
  auto h_2d_neg = d_neg_pi.Histo2D({"2d","2d neg",100,0,4.008,100,0,30},"diff_time_mod","P.aero.npeSum");
  TCanvas *c_monitor = new TCanvas();
  c_monitor->Divide(2,3);
  c_monitor->cd(1);
  h_cointime_neg->DrawCopy("hist");
  c_monitor->cd(2);
  h_cointime_pos->DrawCopy("hist");
  c_monitor->cd(3);
  h_diff_mod_neg->DrawCopy("hist");
  c_monitor->cd(4);
  h_diff_mod_pos->DrawCopy("hist");
  c_monitor->cd(5);
  h_2d_neg->DrawCopy("hist");
  c_monitor->cd(6);
  h_2d_pos->DrawCopy("hist");
  std::string c_monitor_name = "results/simc/monitor_"+std::to_string(RunGroup)+".pdf";
  c_monitor->SaveAs(c_monitor_name.c_str());
  
  


  ROOT::RDataFrame d_sim_pos_raw_1("h10",pos_sim_rootfile);
  ROOT::RDataFrame d_sim_neg_raw_1("h10",neg_sim_rootfile);
  auto d_sim_pos_raw = d_sim_pos_raw_1
    .Filter("hsdelta > -10 && hsdelta < 10")
    .Filter("ssdelta > -10 && ssdelta < 22")
    ;
  auto d_sim_neg_raw = d_sim_neg_raw_1
    .Filter("hsdelta > -10 && hsdelta < 10")
    .Filter("ssdelta > -10 && ssdelta < 22")
    ;

  double dcharge_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["charge"].get<double>();
  //double d_shmsTE_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["TE"].get<double>();
  //double d_hmsTE_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["TEHMS"].get<double>();
  //double d_coinlive_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["TLT"].get<double>();
  double normfac_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["normfac"]["inc"]["rad"]["normfac"].get<double>();
  //double factor_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["factor"].get<double>();
  double nentries_pos = *d_sim_pos_raw_1.Count();
  double nentries_neg = *d_sim_neg_raw_1.Count();
  std::cout<<"neg sim entries "<<nentries_neg<<" pos "<<nentries_pos<<std::endl;
  double wfac_neg = (normfac_neg/nentries_neg)*(dcharge_neg/1);
  auto weightcalculate_neg = [wfac_neg](float weight){return wfac_neg*weight;};
  //auto weightcalculate_neg = [wfac_neg,d_coinlive_neg,d_shmsTE_neg,d_hmsTE_neg,factor_neg](float weight){return wfac_neg*d_coinlive_neg*d_shmsTE_neg*d_hmsTE_neg*weight*factor_neg;};
  double dcharge_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["charge"].get<double>();
  //double d_shmsTE_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["shmsTE"].get<double>();
  //double d_hmsTE_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["hmsTE"].get<double>();
  //double d_coinlive_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["coinlive"].get<double>();
  //double factor_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["factor"].get<double>();
  double normfac_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["normfac"]["inc"]["rad"]["normfac"].get<double>();
  double wfac_pos = (normfac_pos/nentries_pos)*(dcharge_pos/1);
  auto weightcalculate_pos = [wfac_pos](float weight){return wfac_pos*weight;};
  //auto weightcalculate_pos = [wfac_pos,d_coinlive_pos,d_shmsTE_pos,d_hmsTE_pos,factor_pos](float weight){return wfac_pos*d_coinlive_pos*d_shmsTE_pos*d_hmsTE_pos*weight*factor_pos;};
  auto d_sim_pos = d_sim_pos_raw
    //.Define("weight_neg",weightcalculate_neg,{"Weight"})
    .Define("weight_pos",weightcalculate_pos,{"Weight"})
    //.Filter("hsdelta > -10 && hsdelta < 10")
    //.Filter("ssdelta > -10 && ssdelta < 22")
    ;
  auto d_sim_neg = d_sim_neg_raw
    .Define("weight_neg",weightcalculate_neg,{"Weight"})
    //.Define("weight_pos",weightcalculate_pos,{"Weight"})
    //.Filter("hsdelta > -10 && hsdelta < 10")
    //.Filter("ssdelta > -10 && ssdelta < 22")
    ;
  
  //for missing mass
  auto pion_mmiss_neg = d_neg_pi.Histo1D({"mmiss_neg","mmiss neg",100,0,2},"mmiss");
  auto pion_mmiss_pos = d_pos_pi.Histo1D({"mmiss_pos","mmiss pos",100,0,2},"mmiss");
  auto pion_mmiss_sim_neg = d_sim_neg.Histo1D({"mmiss_neg_sim","mmiss neg sim", 100,0,2},"missmass","weight_neg");
  auto pion_mmiss_sim_pos = d_sim_pos.Histo1D({"mmiss_pos_sim","mmiss pos sim", 100,0,2},"missmass","weight_pos");
  TCanvas *c_missmass = new TCanvas();
  c_missmass->Divide(1,2);
  c_missmass->cd(1);
  pion_mmiss_neg->DrawCopy("hist");
  pion_mmiss_pos->SetLineColor(kRed);
  pion_mmiss_pos->DrawCopy("hist same");
  c_missmass->cd(2);
  pion_mmiss_sim_neg->DrawCopy("hist");
  pion_mmiss_sim_pos->SetLineColor(kRed);
  pion_mmiss_sim_pos->DrawCopy("hist same");
  std::string c_missmass_name = "results/simc/missmass_"+std::to_string(RunGroup)+".pdf";
  c_missmass->SaveAs(c_missmass_name.c_str());
  
  //for z
  auto pion_z_neg = d_neg_pi.Histo1D({"z_neg","z neg",100,0,2},"z");
  auto pion_z_pos = d_pos_pi.Histo1D({"z_pos","z pos",100,0,2},"z");
  auto pion_z_sim_neg = d_sim_neg.Histo1D({"z_neg_sim","z neg sim", 100,0,2},"z");
  auto pion_z_sim_pos = d_sim_pos.Histo1D({"z_pos_sim","z pos sim", 100,0,2},"z");
  TCanvas *c_z = new TCanvas();
  c_z->Divide(1,2);
  c_z->cd(1);
  pion_z_neg->DrawCopy("hist");
  pion_z_pos->SetLineColor(kRed);
  pion_z_pos->DrawCopy("hist same");
  c_z->cd(2);
  pion_z_sim_neg->DrawCopy("hist");
  pion_z_sim_pos->SetLineColor(kRed);
  pion_z_sim_pos->DrawCopy("hist same");
  std::string c_z_name = "results/simc/z_"+std::to_string(RunGroup)+".pdf";
  c_z->SaveAs(c_z_name.c_str());
  //for neg runs

  auto p_xfp_neg = d_neg_pi.Histo1D({"pxfp","SHMS x fp data",100,-30,30},"P.dc.x_fp");
  auto p_xfp_sim_neg = d_sim_neg.Histo1D({"pxfpsim","SHMS x fp sim",100,-30,30},"ssxfp","weight_neg");
  auto p_yfp_neg = d_neg_pi.Histo1D({"pyfp","SHMS y fp data",100,-30,30},"P.dc.y_fp");
  auto p_yfp_sim_neg = d_sim_neg.Histo1D({"pyfpsim","SHMS y fp sim",100,-30,30},"ssyfp","weight_neg");
  auto p_xpfp_neg = d_neg_pi.Histo1D({"pxpfp","SHMS xp fp data",100,-0.1,0.1},"P.dc.xp_fp");
  auto p_xpfp_sim_neg = d_sim_neg.Histo1D({"pxpfpsim","SHMS xp fp sim",100,-0.1,0.1},"ssxpfp","weight_neg");
  auto p_ypfp_neg = d_neg_pi.Histo1D({"pypfp","SHMS yp fp data",100,-0.05,0.05},"P.dc.yp_fp");
  auto p_ypfp_sim_neg = d_sim_neg.Histo1D({"pypfpsim","SHMS yp fp sim",100,-0.05,0.05},"ssypfp","weight_neg");
  TCanvas *c_fp = new TCanvas();
  c_fp->Divide(2,2);
  c_fp->cd(1);
  p_xfp_neg->DrawCopy("hist");
  p_xfp_sim_neg->SetLineColor(kRed);
  p_xfp_sim_neg->DrawCopy("hist same");
  //p_xfp_sim_neg->DrawCopy("hist");
  c_fp->cd(2);
  p_yfp_neg->DrawCopy("hist");
  p_yfp_sim_neg->SetLineColor(kRed);
  p_yfp_sim_neg->DrawCopy("hist same");
  c_fp->cd(3);
  p_xpfp_neg->DrawCopy("hist");
  p_xpfp_sim_neg->SetLineColor(kRed);
  p_xpfp_sim_neg->DrawCopy("hist same");
  c_fp->cd(4);
  p_ypfp_neg->DrawCopy("hist");
  p_ypfp_sim_neg->SetLineColor(kRed);
  p_ypfp_sim_neg->DrawCopy("hist same");
  std::string c_fp_name = "results/simc/shms_fp_"+std::to_string(RunGroup)+"_neg.pdf";
  c_fp->SaveAs(c_fp_name.c_str());
  
  auto h_xfp_neg = d_neg_pi.Histo1D({"hxfp","HMS x fp data",100,-30,30},"H.dc.x_fp");
  auto h_xfp_sim_neg = d_sim_neg.Histo1D({"hxfpsim","HMS x fp sim",100,-30,30},"hsxfp","weight_neg");
  auto h_yfp_neg = d_neg_pi.Histo1D({"hyfp","HMS y fp data",100,-30,30},"H.dc.y_fp");
  auto h_yfp_sim_neg = d_sim_neg.Histo1D({"hyfpsim","HMS y fp sim",100,-30,30},"hsyfp","weight_neg");
  auto h_xpfp_neg = d_neg_pi.Histo1D({"hxpfp","HMS xp fp data",100,-0.1,0.1},"H.dc.xp_fp");
  auto h_xpfp_sim_neg = d_sim_neg.Histo1D({"hxpfpsim","HMS xp fp sim",100,-0.1,0.1},"hsxpfp","weight_neg");
  auto h_ypfp_neg = d_neg_pi.Histo1D({"hypfp","HMS yp fp data",100,-0.05,0.05},"H.dc.yp_fp");
  auto h_ypfp_sim_neg = d_sim_neg.Histo1D({"hypfpsim","HMS yp fp sim",100,-0.05,0.05},"hsypfp","weight_neg");
  TCanvas *c_fp_hms = new TCanvas();
  c_fp_hms->Divide(2,2);
  c_fp_hms->cd(1);
  h_xfp_neg->DrawCopy("hist");
  h_xfp_sim_neg->SetLineColor(kRed);
  h_xfp_sim_neg->DrawCopy("hist same");
  c_fp_hms->cd(2);
  h_yfp_neg->DrawCopy("hist");
  h_yfp_sim_neg->SetLineColor(kRed);
  h_yfp_sim_neg->DrawCopy("hist same");
  c_fp_hms->cd(3);
  h_xpfp_neg->DrawCopy("hist");
  h_xpfp_sim_neg->SetLineColor(kRed);
  h_xpfp_sim_neg->DrawCopy("hist same");
  c_fp_hms->cd(4);
  h_ypfp_neg->DrawCopy("hist");
  h_ypfp_sim_neg->SetLineColor(kRed);
  h_ypfp_sim_neg->DrawCopy("hist same");
  std::string c_fp_hms_name = "results/simc/hms_fp_"+std::to_string(RunGroup)+"_neg.pdf";
  c_fp_hms->SaveAs(c_fp_hms_name.c_str());

  auto p_xptar_neg = d_neg_pi.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  auto p_yptar_neg = d_neg_pi.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  auto p_ytar_neg = d_neg_pi.Histo1D({"shms y_tar","shms y_tar",100,-4,4},"P.gtr.y");
  auto p_delta_neg = d_neg_pi.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  auto p_xptar_sim_neg = d_sim_neg.Histo1D({"shms xp_tar","shms xp_tar sim",100,-0.1,0.1},"ssxptar","weight_neg");
  auto p_yptar_sim_neg = d_sim_neg.Histo1D({"shms yp_tar","shms yp_tar sim",100,-0.1,0.1},"ssyptar","weight_neg");
  auto p_ytar_sim_neg = d_sim_neg.Histo1D({"shms y_tar","shms y_tar sim",100,-4,4},"ssytar","weight_neg");
  auto p_delta_sim_neg = d_sim_neg.Histo1D({"shms dp","shms dp sim",100,-10,22},"ssdelta","weight_neg");
  TCanvas *c_tar_p = new TCanvas();
  c_tar_p->Divide(2,2);
  c_tar_p->cd(1);
  p_xptar_neg->DrawCopy("hist");
  p_xptar_sim_neg->SetLineColor(kRed);
  p_xptar_sim_neg->DrawCopy("hist same");
  c_tar_p->cd(2);
  p_yptar_neg->DrawCopy("hist");
  p_yptar_sim_neg->SetLineColor(kRed);
  p_yptar_sim_neg->DrawCopy("hist same");
  c_tar_p->cd(3);
  p_delta_neg->DrawCopy("hist");
  p_delta_sim_neg->SetLineColor(kRed);
  p_delta_sim_neg->DrawCopy("hist same");
  c_tar_p->cd(4);
  p_ytar_neg->DrawCopy("hist");
  p_ytar_sim_neg->SetLineColor(kRed);
  p_ytar_sim_neg->DrawCopy("hist same");
  std::string c_tar_p_name = "results/simc/shms_tar_"+std::to_string(RunGroup)+"_neg.pdf";
  c_tar_p->SaveAs(c_tar_p_name.c_str());

  //for pos runs
  auto p_xfp_pos = d_pos_pi.Histo1D({"pxfp","SHMS x fp data",100,-30,30},"P.dc.x_fp");
  auto p_xfp_sim_pos = d_sim_pos.Histo1D({"pxfpsim","SHMS x fp sim",100,-30,30},"ssxfp","weight_pos");
  auto p_yfp_pos = d_pos_pi.Histo1D({"pyfp","SHMS y fp data",100,-30,30},"P.dc.y_fp");
  auto p_yfp_sim_pos = d_sim_pos.Histo1D({"pyfpsim","SHMS y fp sim",100,-30,30},"ssyfp","weight_pos");
  auto p_xpfp_pos = d_pos_pi.Histo1D({"pxpfp","SHMS xp fp data",100,-0.1,0.1},"P.dc.xp_fp");
  auto p_xpfp_sim_pos = d_sim_pos.Histo1D({"pxpfpsim","SHMS xp fp sim",100,-0.1,0.1},"ssxpfp","weight_pos");
  auto p_ypfp_pos = d_pos_pi.Histo1D({"pypfp","SHMS yp fp data",100,-0.05,0.05},"P.dc.yp_fp");
  auto p_ypfp_sim_pos = d_sim_pos.Histo1D({"pypfpsim","SHMS yp fp sim",100,-0.05,0.05},"ssypfp","weight_pos");
  TCanvas *c_fp_pos = new TCanvas();
  c_fp_pos->Divide(2,2);
  c_fp_pos->cd(1);
  p_xfp_pos->DrawCopy("hist");
  p_xfp_sim_pos->SetLineColor(kRed);
  p_xfp_sim_pos->DrawCopy("hist same");
  c_fp_pos->cd(2);
  p_yfp_pos->DrawCopy("hist");
  p_yfp_sim_pos->SetLineColor(kRed);
  p_yfp_sim_pos->DrawCopy("hist same");
  c_fp_pos->cd(3);
  p_xpfp_pos->DrawCopy("hist");
  p_xpfp_sim_pos->SetLineColor(kRed);
  p_xpfp_sim_pos->DrawCopy("hist same");
  c_fp_pos->cd(4);
  p_ypfp_pos->DrawCopy("hist");
  p_ypfp_sim_pos->SetLineColor(kRed);
  p_ypfp_sim_pos->DrawCopy("hist same");
  std::string c_fp_pos_name = "results/simc/shms_fp_"+std::to_string(RunGroup)+"_pos.pdf";
  c_fp_pos->SaveAs(c_fp_pos_name.c_str());
  
  auto h_xfp_pos = d_pos_pi.Histo1D({"hxfp","HMS x fp data",100,-30,30},"H.dc.x_fp");
  auto h_xfp_sim_pos = d_sim_pos.Histo1D({"hxfpsim","HMS x fp sim",100,-30,30},"hsxfp","weight_pos");
  auto h_yfp_pos = d_pos_pi.Histo1D({"hyfp","HMS y fp data",100,-30,30},"H.dc.y_fp");
  auto h_yfp_sim_pos = d_sim_pos.Histo1D({"hyfpsim","HMS y fp sim",100,-30,30},"hsyfp","weight_pos");
  auto h_xpfp_pos = d_pos_pi.Histo1D({"hxpfp","HMS xp fp data",100,-0.1,0.1},"H.dc.xp_fp");
  auto h_xpfp_sim_pos = d_sim_pos.Histo1D({"hxpfpsim","HMS xp fp sim",100,-0.1,0.1},"hsxpfp","weight_pos");
  auto h_ypfp_pos = d_pos_pi.Histo1D({"hypfp","HMS yp fp data",100,-0.05,0.05},"H.dc.yp_fp");
  auto h_ypfp_sim_pos = d_sim_pos.Histo1D({"hypfpsim","HMS yp fp sim",100,-0.05,0.05},"hsypfp","weight_pos");
  TCanvas *c_fp_pos_hms = new TCanvas();
  c_fp_pos_hms->Divide(2,2);
  c_fp_pos_hms->cd(1);
  h_xfp_pos->DrawCopy("hist");
  h_xfp_sim_pos->SetLineColor(kRed);
  h_xfp_sim_pos->DrawCopy("hist same");
  c_fp_pos_hms->cd(2);
  h_yfp_pos->DrawCopy("hist");
  h_yfp_sim_pos->SetLineColor(kRed);
  h_yfp_sim_pos->DrawCopy("hist same");
  c_fp_pos_hms->cd(3);
  h_xpfp_pos->DrawCopy("hist");
  h_xpfp_sim_pos->SetLineColor(kRed);
  h_xpfp_sim_pos->DrawCopy("hist same");
  c_fp_pos_hms->cd(4);
  h_ypfp_pos->DrawCopy("hist");
  h_ypfp_sim_pos->SetLineColor(kRed);
  h_ypfp_sim_pos->DrawCopy("hist same");
  std::string c_fp_pos_hms_name = "results/simc/hms_fp_"+std::to_string(RunGroup)+"_pos.pdf";
  c_fp_pos_hms->SaveAs(c_fp_pos_hms_name.c_str());

  auto p_xptar_pos = d_pos_pi.Histo1D({"shms xp_tar","shms xp_tar",100,-0.1,0.1},"P.gtr.th");
  auto p_yptar_pos = d_pos_pi.Histo1D({"shms yp_tar","shms yp_tar",100,-0.1,0.1},"P.gtr.ph");
  auto p_ytar_pos = d_pos_pi.Histo1D({"shms yp_tar","shms yp_tar",100,-4,4},"P.gtr.y");
  auto p_delta_pos = d_pos_pi.Histo1D({"shms dp","shms dp",100,-10,22},"P.gtr.dp");
  auto p_xptar_sim_pos = d_sim_pos.Histo1D({"shms xp_tar","shms xp_tar sim",100,-0.1,0.1},"ssxptar","weight_pos");
  auto p_yptar_sim_pos = d_sim_pos.Histo1D({"shms yp_tar","shms yp_tar sim",100,-0.1,0.1},"ssyptar","weight_pos");
  auto p_ytar_sim_pos = d_sim_pos.Histo1D({"shms y_tar","shms y_tar sim",100,-4,4},"ssytar","weight_pos");
  auto p_delta_sim_pos = d_sim_pos.Histo1D({"shms dp","shms dp sim",100,-10,22},"ssdelta","weight_pos");
  TCanvas *c_tar_p_pos = new TCanvas();
  c_tar_p_pos->Divide(2,2);
  c_tar_p_pos->cd(1);
  p_xptar_pos->DrawCopy("hist");
  p_xptar_sim_pos->SetLineColor(kRed);
  p_xptar_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(2);
  p_yptar_pos->DrawCopy("hist");
  p_yptar_sim_pos->SetLineColor(kRed);
  p_yptar_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(3);
  p_delta_pos->DrawCopy("hist");
  p_delta_sim_pos->SetLineColor(kRed);
  p_delta_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(4);
  p_ytar_pos->DrawCopy("hist");
  p_ytar_sim_pos->SetLineColor(kRed);
  p_ytar_sim_pos->DrawCopy("hist same");
  std::string c_tar_p_pos_name = "results/simc/shms_tar_"+std::to_string(RunGroup)+"_pos.pdf";
  c_tar_p_pos->SaveAs(c_tar_p_pos_name.c_str());
  
  //2d histograms
  //shms fp
  auto p_fpy_fpx_neg = d_neg_pi.Histo2D({"p_fpx_fpy_neg","shms focal plane x vs y neg runs;xfp;yfp",100,-30,30,100,-30,30},"P.dc.x_fp","P.dc.y_fp");
  auto p_fpy_fpxp_neg = d_neg_pi.Histo2D({"p_fpy_fpxp_neg","shms focal plane y vs xp neg runs;xpfp;yfp",100,-0.1,0.1,100,-30,30},"P.dc.xp_fp","P.dc.y_fp");
  auto p_fpy_fpyp_neg = d_neg_pi.Histo2D({"p_fpy_fpyp_neg","shms focal plane y vs yp neg runs;ypfp;yfp",100,-0.1,0.1,100,-30,30},"P.dc.yp_fp","P.dc.y_fp");
  auto p_fpx_fpxp_neg = d_neg_pi.Histo2D({"p_fpx_fpxp_neg","shms focal plane x vs xp neg runs;xpfp;xfp",100,-0.1,0.1,100,-30,30},"P.dc.xp_fp","P.dc.x_fp");
  auto p_fpx_fpyp_neg = d_neg_pi.Histo2D({"p_fpx_fpyp_neg","shms focal plane x vs yp neg runs;ypfp;xfp",100,-0.1,0.1,100,-30,30},"P.dc.yp_fp","P.dc.x_fp");
  auto p_fpyp_fpxp_neg = d_neg_pi.Histo2D({"p_fpyp_fpxp_neg","shms focal plane yp vs xp neg runs;xpfp;ypfp",100,-0.1,0.1,100,-0.1,0.1},"P.dc.xp_fp","P.dc.yp_fp");
  TCanvas *c_p_fp_2d_neg = new TCanvas();
  c_p_fp_2d_neg->Divide(6,1);
  c_p_fp_2d_neg->cd(1);
  p_fpy_fpx_neg->DrawCopy("colz");
  c_p_fp_2d_neg->cd(2);
  p_fpy_fpxp_neg->DrawCopy("colz");
  c_p_fp_2d_neg->cd(3);
  p_fpy_fpyp_neg->DrawCopy("colz");
  c_p_fp_2d_neg->cd(4);
  p_fpx_fpxp_neg->DrawCopy("colz");
  c_p_fp_2d_neg->cd(5);
  p_fpx_fpyp_neg->DrawCopy("colz");
  c_p_fp_2d_neg->cd(6);
  p_fpyp_fpxp_neg->DrawCopy("colz");
  std::string c_p_fp_2d_neg_name = "results/simc/shms_fp_2d_"+std::to_string(RunGroup)+"_neg.pdf";
  c_p_fp_2d_neg->SaveAs(c_p_fp_2d_neg_name.c_str());
  
  auto p_fpy_fpx_pos = d_pos_pi.Histo2D({"p_fpx_fpy_pos","shms focal plane x vs y pos runs;xfp;yfp",100,-30,30,100,-30,30},"P.dc.x_fp","P.dc.y_fp");
  auto p_fpy_fpxp_pos = d_pos_pi.Histo2D({"p_fpy_fpxp_pos","shms focal plane y vs xp pos runs;xpfp;yfp",100,-0.1,0.1,100,-30,30},"P.dc.xp_fp","P.dc.y_fp");
  auto p_fpy_fpyp_pos = d_pos_pi.Histo2D({"p_fpy_fpyp_pos","shms focal plane y vs yp pos runs;ypfp;yfp",100,-0.1,0.1,100,-30,30},"P.dc.yp_fp","P.dc.y_fp");
  auto p_fpx_fpxp_pos = d_pos_pi.Histo2D({"p_fpx_fpxp_pos","shms focal plane x vs xp pos runs;xpfp;xfp",100,-0.1,0.1,100,-30,30},"P.dc.xp_fp","P.dc.x_fp");
  auto p_fpx_fpyp_pos = d_pos_pi.Histo2D({"p_fpx_fpyp_pos","shms focal plane x vs yp pos runs;ypfp;xfp",100,-0.1,0.1,100,-30,30},"P.dc.yp_fp","P.dc.x_fp");
  auto p_fpyp_fpxp_pos = d_pos_pi.Histo2D({"p_fpyp_fpxp_pos","shms focal plane yp vs xp pos runs;xpfp;ypfp",100,-0.1,0.1,100,-0.1,0.1},"P.dc.xp_fp","P.dc.yp_fp");
  TCanvas *c_p_fp_2d_pos = new TCanvas();
  c_p_fp_2d_pos->Divide(6,1);
  c_p_fp_2d_pos->cd(1);
  p_fpy_fpx_pos->DrawCopy("colz");
  c_p_fp_2d_pos->cd(2);
  p_fpy_fpxp_pos->DrawCopy("colz");
  c_p_fp_2d_pos->cd(3);
  p_fpy_fpyp_pos->DrawCopy("colz");
  c_p_fp_2d_pos->cd(4);
  p_fpx_fpxp_pos->DrawCopy("colz");
  c_p_fp_2d_pos->cd(5);
  p_fpx_fpyp_pos->DrawCopy("colz");
  c_p_fp_2d_pos->cd(6);
  p_fpyp_fpxp_pos->DrawCopy("colz");
  std::string c_p_fp_2d_pos_name = "results/simc/shms_fp_2d_"+std::to_string(RunGroup)+"_pos.pdf";
  c_p_fp_2d_pos->SaveAs(c_p_fp_2d_pos_name.c_str());
  
  auto p_fpy_fpx_sim = d_sim_neg.Histo2D({"p_fpx_fpy_sim","shms focal plane x vs y sim runs;xfp;yfp",100,-30,30,100,-30,30},"ssxfp","ssyfp");
  auto p_fpy_fpxp_sim = d_sim_neg.Histo2D({"p_fpy_fpxp_sim","shms focal plane y vs xp sim runs;xpfp;yfp",100,-0.1,0.1,100,-30,30},"ssxpfp","ssyfp");
  auto p_fpy_fpyp_sim = d_sim_neg.Histo2D({"p_fpy_fpyp_sim","shms focal plane y vs yp sim runs;ypfp;yfp",100,-0.1,0.1,100,-30,30},"ssypfp","ssyfp");
  auto p_fpx_fpxp_sim = d_sim_neg.Histo2D({"p_fpx_fpxp_sim","shms focal plane x vs xp sim runs;xpfp;xfp",100,-0.1,0.1,100,-30,30},"ssxpfp","ssxfp");
  auto p_fpx_fpyp_sim = d_sim_neg.Histo2D({"p_fpx_fpyp_sim","shms focal plane x vs yp sim runs;ypfp;xfp",100,-0.1,0.1,100,-30,30},"ssypfp","ssxfp");
  auto p_fpyp_fpxp_sim = d_sim_neg.Histo2D({"p_fpyp_fpxp_sim","shms focal plane yp vs xp sim runs;xpfp;ypfp",100,-0.1,0.1,100,-0.1,0.1},"ssxpfp","ssypfp");
  TCanvas *c_p_fp_2d_sim = new TCanvas();
  c_p_fp_2d_sim->Divide(6,1);
  c_p_fp_2d_sim->cd(1);
  p_fpy_fpx_sim->DrawCopy("colz");
  c_p_fp_2d_sim->cd(2);
  p_fpy_fpxp_sim->DrawCopy("colz");
  c_p_fp_2d_sim->cd(3);
  p_fpy_fpyp_sim->DrawCopy("colz");
  c_p_fp_2d_sim->cd(4);
  p_fpx_fpxp_sim->DrawCopy("colz");
  c_p_fp_2d_sim->cd(5);
  p_fpx_fpyp_sim->DrawCopy("colz");
  c_p_fp_2d_sim->cd(6);
  p_fpyp_fpxp_sim->DrawCopy("colz");
  std::string c_p_fp_2d_sim_name = "results/simc/shms_fp_2d_"+std::to_string(RunGroup)+"_sim.pdf";
  c_p_fp_2d_sim->SaveAs(c_p_fp_2d_sim_name.c_str());

 //target variable 2ds 
 auto xptar_delta_neg = d_neg_pi.Histo2D({"xptar vs delta","neg runs;xptar;delta",100,-0.1,0.1,100,-10,22},"P.gtr.th","P.gtr.dp");
 auto yptar_delta_neg = d_neg_pi.Histo2D({"yptar vs delta","neg runs;xptar;delta",100,-0.1,0.1,100,-10,22},"P.gtr.ph","P.gtr.dp");
 auto y_delta_neg = d_neg_pi.Histo2D({"y vs delta","neg runs;ytar;delta",100,-4,4,100,-10,22},"P.gtr.y","P.gtr.dp");
 TCanvas *c_p_tar_neg = new TCanvas();
 c_p_tar_neg->Divide(3,1);
 c_p_tar_neg->cd(1);
 xptar_delta_neg->DrawCopy("colz");
 c_p_tar_neg->cd(2);
 yptar_delta_neg->DrawCopy("colz");
 c_p_tar_neg->cd(3);
 y_delta_neg->DrawCopy("colz");
 std::string c_p_tar_neg_name = "results/simc/shms_tar_2d_"+std::to_string(RunGroup)+"_neg.pdf";
 c_p_tar_neg->SaveAs(c_p_tar_neg_name.c_str());
 auto xptar_delta_pos = d_pos_pi.Histo2D({"xptar vs delta","pos runs;xptar;delta",100,-0.1,0.1,100,-10,22},"P.gtr.th","P.gtr.dp");
 auto yptar_delta_pos = d_pos_pi.Histo2D({"yptar vs delta","pos runs;xptar;delta",100,-0.1,0.1,100,-10,22},"P.gtr.ph","P.gtr.dp");
 auto y_delta_pos = d_pos_pi.Histo2D({"y vs delta","pos runs;ytar;delta",100,-4,4,100,-10,22},"P.gtr.y","P.gtr.dp");
 TCanvas *c_p_tar_pos = new TCanvas();
 c_p_tar_pos->Divide(3,1);
 c_p_tar_pos->cd(1);
 xptar_delta_pos->DrawCopy("colz");
 c_p_tar_pos->cd(2);
 yptar_delta_pos->DrawCopy("colz");
 c_p_tar_pos->cd(3);
 y_delta_pos->DrawCopy("colz");
 std::string c_p_tar_pos_name = "results/simc/shms_tar_2d_"+std::to_string(RunGroup)+"_pos.pdf";
 c_p_tar_pos->SaveAs(c_p_tar_pos_name.c_str());
 auto xptar_delta_sim = d_sim_neg.Histo2D({"xptar vs delta","sim runs;xptar;delta",100,-0.1,0.1,100,-10,22},"ssxptar","ssdelta");
 auto yptar_delta_sim = d_sim_neg.Histo2D({"yptar vs delta","sim runs;xptar;delta",100,-0.1,0.1,100,-10,22},"ssyptar","ssdelta");
 auto y_delta_sim = d_sim_neg.Histo2D({"y vs delta","sim runs;ytar;delta",100,-4,4,100,-10,22},"ssytar","ssdelta");
 TCanvas *c_p_tar_sim = new TCanvas();
 c_p_tar_sim->Divide(3,1);
 c_p_tar_sim->cd(1);
 xptar_delta_sim->DrawCopy("colz");
 c_p_tar_sim->cd(2);
 yptar_delta_sim->DrawCopy("colz");
 c_p_tar_sim->cd(3);
 y_delta_sim->DrawCopy("colz");
 std::string c_p_tar_sim_name = "results/simc/shms_tar_2d_"+std::to_string(RunGroup)+"_sim.pdf";
 c_p_tar_sim->SaveAs(c_p_tar_sim_name.c_str());
  

  }//if it's normal production runs
 
}
