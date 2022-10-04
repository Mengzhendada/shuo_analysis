#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"


constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;


bool shms_momentum_high = true;

void statistic_runs_D2(int RunGroup=0){

  if(RunGroup ==0){
    std::cout<<"Enter a RunGroup (-1 to exit):";
    std::cin>>RunGroup;
    if(RunGroup<=0)
      return;
  }

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }

  json jout;
  {
    std::string if_name = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
    std::ifstream ifs(if_name.c_str());
    ifs>>jout;
  }
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  }

  int bins = j_cuts["bins"].get<int>();
 
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();

  auto xprime = [&](double Q2,double xbj){
    return 2*xbj/(1+sqrt(1+4*xbj*xbj*M_P*M_P/(Q2)));
  };
  auto zprime = [&](double zh,double xprime,double xbj,double pt,double Q2){
    return (zh/2)*(xprime/xbj)*(1+sqrt(1-(4*xbj*xbj*M_P*M_P*(M_pion*M_pion-pt*pt))/(zh*zh*Q2*Q2))); 
  };

  auto pt = [](double p,double th){return p*std::sin(th);};
  double pt_cut_num = j_cuts["pt_cut"].get<double>();
  std::string pt_cut = "pt<"+std::to_string(pt_cut_num);
  double Mx2_cut_num = j_cuts["Mx2"].get<double>();
  std::string Mx2_cut = "Mx2>"+std::to_string(Mx2_cut_num);
  std::string Wp2_cut = "Wp2>"+std::to_string(Mx2_cut_num);
  double W2_cut_num = j_cuts["W2"].get<double>();
  std::string W2_cut = "W2 > "+std::to_string(W2_cut_num);
  
  double Q2_low = j_cuts["Q2_low"].get<double>();
  double Q2_high = j_cuts["Q2_high"].get<double>();
  std::string Q2_low_cut = "Q2<"+std::to_string(Q2_low);
  std::string Q2_high_cut = "Q2>"+std::to_string(Q2_high);
  std::string Q2_middle_cut = "Q2>="+std::to_string(Q2_low)+" && Q2<="+std::to_string(Q2_high);

  std::string HGC_bad_center = "(P.hgcer.yAtCer-1.33)*(P.hgcer.yAtCer-1.33) +  (P.hgcer.xAtCer-0.83)*(P.hgcer.xAtCer-0.83) >= 36";
  std::string HGC_bad_strip = "(P.hgcer.xAtCer<0 || P.hgcer.xAtCer>3)";
  std::string HGC_bad = HGC_bad_center + " && "+ HGC_bad_strip;
  double P_hgcer = j_cuts["P_hgcer"].get<double>();
  auto SHMS_hgc_cut =  [=](double shms_p,double hgc_Npe,double xcer,double ycer){
    if(shms_p>2.9){
      if(P_hgcer==-1){
        return true;
      }
      else{
        return hgc_Npe>P_hgcer && (xcer-1.33)*(xcer-1.33)+(ycer-0.83)*(ycer-0.83)>=36 && (xcer<0||xcer>3);
      }
    }
    else{
      return true;
    }
  };
      double rf_pi_low = j_cuts["rf_cut_low"].get<double>();
      double rf_pi_high = j_cuts["rf_cut_high"].get<double>();
      auto rf_cut = [=](double SHMS_rftime){
        return SHMS_rftime>rf_pi_low && SHMS_rftime<rf_pi_high;  
        
      };

  if(!neg_D2.empty() && !pos_D2.empty()){
    //for pos runs
    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);

      auto d_pos_pi = d_pos_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Define("xprime2","xprime*xprime")
        .Define("zprime2","zprime*zprime")
        .Define("xbj2","xbj*xbj")
        .Define("z2","z*z")
        .Define("Q22","Q2*Q2")
        .Define("shms_p2","shms_p*shms_p")
        .Define("shms_dp2","P_gtr_dp*P_gtr_dp")
        .Define("W22","W2*W2")
        .Define("Wp22","Wp2*Wp2")
        .Filter(pt_cut)
        //.Filter(rf_cut,{"diff_time_mod"})
        //.Filter(Mx2_cut)
        .Filter(W2_cut)
        .Filter(Wp2_cut)
        .Filter(SHMS_hgc_cut,{"shms_p","P_hgcer_npeSum","P_hgcer_xAtCer","P_hgcer_yAtCer"})
        ;
      //int pion_n = *d_pos_pi.Count();
      //jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      auto d_pos_pi_1 = d_pos_pi.Filter(Q2_low_cut);
      auto d_pos_pi_2 = d_pos_pi.Filter(Q2_middle_cut);
      auto d_pos_pi_3 = d_pos_pi.Filter(Q2_high_cut);

      // for bg
      ROOT::RDataFrame d_pos_bgraw("T_bg",rootfile_name);
      auto d_pos_bg = d_pos_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Filter(pt_cut)
        //.Filter(Mx2_cut)
        //.Filter(rf_cut,{"diff_time_mod"})
        .Filter(W2_cut)
        .Filter(Wp2_cut)
        .Filter(SHMS_hgc_cut,{"shms_p","P_hgcer_npeSum","P_hgcer_xAtCer","P_hgcer_yAtCer"})
        ;
      auto d_pos_bg_1 = d_pos_bg.Filter(Q2_low_cut);
      auto d_pos_bg_2 = d_pos_bg.Filter(Q2_middle_cut);
      auto d_pos_bg_3 = d_pos_bg.Filter(Q2_high_cut);

      //int bg_n = *d_pos_bg.Count();
      //jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;


      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_pos = d_pos_pi.Histo2D({"Q2_x","Q2_x",bins,0,1,bins,0,10},"xbj","Q2");
      //h_Q2_x_pos->RebinX(Rebin_n);
      //h_Q2_x_pos->RebinY(Rebin_n);
      h_Q2_x_pos->Write();
      auto h_Q2 = d_pos_pi.Histo1D({"Q2","Q2",bins,0,10},"Q2");
      h_Q2->Write();
      auto h_omega = d_pos_pi.Histo1D({"omega","omega",100,0,10},"H_kin_primary_omega");
      h_omega->Write();
      auto h_shmsdp = d_pos_pi.Histo1D({"shmsdp","shmsdp",100,-10,25},"P_gtr_dp");
      h_shmsdp->Write();
      auto h_hmsdp = d_pos_pi.Histo1D({"hmsdp","hmsdp",100,-10,12},"H_gtr_dp");
      h_hmsdp->Write();
      auto h_Q2_1 = d_pos_pi_1.Histo1D({"Q2_1","Q2_1",bins,0,10},"Q2");
      auto h_Q2_2 = d_pos_pi_2.Histo1D({"Q2_2","Q2_2",bins,0,10},"Q2");
      auto h_Q2_3 = d_pos_pi_3.Histo1D({"Q2_3","Q2_3",bins,0,10},"Q2");
      h_Q2_1->Write();
      h_Q2_2->Write();
      h_Q2_3->Write();
      auto h_xbj = d_pos_pi.Histo1D({"xbj","xbj",bins,0,1},"xbj");
      double pion_n_integ = h_xbj->Integral();
      double pion_n = *d_pos_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      jout[(std::to_string(RunNumber)).c_str()]["pion_n_integ"] = pion_n_integ;
      //h_xbj->Rebin(Rebin_n);
      h_xbj->Write();
      auto h_xbj_1 = d_pos_pi_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj");
      auto h_xbj_2 = d_pos_pi_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj");
      auto h_xbj_3 = d_pos_pi_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj");
      h_xbj_1->Write();
      h_xbj_2->Write();
      h_xbj_3->Write();
      auto h_xprime_1 = d_pos_pi_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime");
      auto h_xprime_2 = d_pos_pi_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime");
      auto h_xprime_3 = d_pos_pi_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime");
      h_xprime_1->Write();
      h_xprime_2->Write();
      h_xprime_3->Write();
      auto h_z = d_pos_pi.Histo1D({"z","z",bins,0,1},"z");
      //h_z->Rebin(Rebin_n);
      h_z->Write();
      auto h_z_1 = d_pos_pi_1.Histo1D({"z_1","z_1",bins,0,1},"z");
      auto h_z_2 = d_pos_pi_2.Histo1D({"z_2","z_2",bins,0,1},"z");
      auto h_z_3 = d_pos_pi_3.Histo1D({"z_3","z_3",bins,0,1},"z");
      h_z_1->Write();
      h_z_2->Write();
      h_z_3->Write();
      auto h_zprime_1 = d_pos_pi_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime");
      auto h_zprime_2 = d_pos_pi_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime");
      auto h_zprime_3 = d_pos_pi_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime");
      h_zprime_1->Write();
      h_zprime_2->Write();
      h_zprime_3->Write();
      auto h_Q2_bg = d_pos_bg.Histo1D({"Q2_bg","Q2_bg",bins,0,10},"Q2");
      h_Q2_bg->Write();
      auto h_omega_bg = d_pos_bg.Histo1D({"omega_bg","omega_bg",100,0,10},"H_kin_primary_omega");
      h_omega_bg->Write();
      auto h_shmsdp_bg = d_pos_bg.Histo1D({"shmsdp_bg","shmsdp_bg",100,-10,25},"P_gtr_dp");
      h_shmsdp_bg->Write();
      auto h_hmsdp_bg = d_pos_bg.Histo1D({"hmsdp_bg","hmsdp_bg",100,-10,12},"H_gtr_dp");
      h_hmsdp_bg->Write();
      auto h_Q2_bg_1 = d_pos_bg_1.Histo1D({"Q2_bg_1","Q2_bg_1",bins,0,10},"Q2");
      auto h_Q2_bg_2 = d_pos_bg_2.Histo1D({"Q2_bg_2","Q2_bg_2",bins,0,10},"Q2");
      auto h_Q2_bg_3 = d_pos_bg_3.Histo1D({"Q2_bg_3","Q2_bg_3",bins,0,10},"Q2");
      h_Q2_bg_1->Write();
      h_Q2_bg_2->Write();
      h_Q2_bg_3->Write();
      auto h_xbj_bg = d_pos_bg.Histo1D({"xbj_bg","xbj_bg",bins,0,1},"xbj");
      double bg_n_integ = h_xbj_bg->Integral();
      double bg_n = *d_pos_bg.Count(); 
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;
      jout[(std::to_string(RunNumber)).c_str()]["bg_n_integ"] = bg_n_integ;
      //h_xbj_bg->Rebin(Rebin_n);
      h_xbj_bg->Write();
      auto h_xbj_bg_1 = d_pos_bg_1.Histo1D({"xbj_1_bg","xbj_1_bg",bins,0,1},"xbj");
      auto h_xbj_bg_2 = d_pos_bg_2.Histo1D({"xbj_2_bg","xbj_2_bg",bins,0,1},"xbj");
      auto h_xbj_bg_3 = d_pos_bg_3.Histo1D({"xbj_3_bg","xbj_3_bg",bins,0,1},"xbj");
      h_xbj_bg_1->Write();
      h_xbj_bg_2->Write();
      h_xbj_bg_3->Write();
      auto h_xprime_bg_1 = d_pos_bg_1.Histo1D({"xprime_1_bg","xprime_1_bg",bins,0,1},"xprime");
      auto h_xprime_bg_2 = d_pos_bg_2.Histo1D({"xprime_2_bg","xprime_2_bg",bins,0,1},"xprime");
      auto h_xprime_bg_3 = d_pos_bg_3.Histo1D({"xprime_3_bg","xprime_3_bg",bins,0,1},"xprime");
      h_xprime_bg_1->Write();
      h_xprime_bg_2->Write();
      h_xprime_bg_3->Write();
      auto h_z_bg = d_pos_bg.Histo1D({"z_bg","z_bg",bins,0,1},"z");
      //h_z_bg->Rebin(Rebin_n);
      h_z_bg->Write();
      auto h_z_bg_1 = d_pos_bg_1.Histo1D({"z_1","z_1",bins,0,1},"z");
      auto h_z_bg_2 = d_pos_bg_2.Histo1D({"z_2","z_2",bins,0,1},"z");
      auto h_z_bg_3 = d_pos_bg_3.Histo1D({"z_3","z_3",bins,0,1},"z");
      h_z_bg_1->Write();
      h_z_bg_2->Write();
      h_z_bg_3->Write();
      auto h_zprime_bg_1 = d_pos_bg_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime");
      auto h_zprime_bg_2 = d_pos_bg_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime");
      auto h_zprime_bg_3 = d_pos_bg_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime");
      h_zprime_bg_1->Write();
      h_zprime_bg_2->Write();
      h_zprime_bg_3->Write();
      auto h_x_z_pos = d_pos_pi.Histo2D({"x_z","x_z",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_pos->Write();
      auto h_x_z_pos_raw = d_pos_pi.Histo2D({"x_z_raw","x_z_raw",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_pos_raw->Write();
      auto h_x_z_bg = d_pos_bg.Histo2D({"x_z_bg","x_z_bg",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg->Write();
      auto h_x_z_pos_1 = d_pos_pi_1.Histo2D({"x_z_1","x_z_1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_pos_1->Write();
      auto h_x_z_posraw_1 = d_pos_pi_1.Histo2D({"x_z_raw1","x_z_raw1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_posraw_1->Write();
      auto h_x_z_bg_1 = d_pos_bg_1.Histo2D({"x_z_bg_1","x_z_bg_1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_1->Write();
      auto h_x_z_pos_2 = d_pos_pi_2.Histo2D({"x_z_2","x_z_2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_pos_2->Write();
      auto h_x_z_posraw_2 = d_pos_pi_2.Histo2D({"x_z_raw2","x_z_raw2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_posraw_2->Write();
      auto h_x_z_bg_2 = d_pos_bg_2.Histo2D({"x_z_bg_2","x_z_bg_2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_2->Write();
      auto h_x_z_pos_3 = d_pos_pi_3.Histo2D({"x_z_raw3","x_z_raw3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_pos_3->Write();
      auto h_x_z_posraw_3 = d_pos_pi_3.Histo2D({"x_z_3","x_z_3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_posraw_3->Write();
      auto h_x_z_bg_3 = d_pos_bg_3.Histo2D({"x_z_bg_3","x_z_bg_3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_3->Write();
     
      auto h_xzprime_pos = d_pos_pi.Histo2D({"xzprime","xzprime",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_pos->Write();
      auto h_xzprime_bg = d_pos_bg.Histo2D({"xzprime_bg","xzprime_bg",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg->Write();
      auto h_xzprime_pos_1 = d_pos_pi_1.Histo2D({"xzprime_1","xzprime_1",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_pos_1->Write();
      auto h_xzprime_bg_1 = d_pos_bg_1.Histo2D({"xzprime_bg_1","xzprime_bg_1",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_1->Write();
      auto h_xzprime_pos_2 = d_pos_pi_2.Histo2D({"xzprime_2","xzprime_2",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_pos_1->Write();
      auto h_xzprime_bg_2 = d_pos_bg_2.Histo2D({"xzprime_bg_2","xzprime_bg_2",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_2->Write();
      auto h_xzprime_pos_3 = d_pos_pi_3.Histo2D({"xzprime_3","xzprime_3",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_pos_1->Write();
      auto h_xzprime_bg_3 = d_pos_bg_3.Histo2D({"xzprime_bg_3","xzprime_bg_3",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_3->Write();

      auto h_Q2_z_pos = d_pos_pi.Histo2D({"Q2_z","Q2_z",bins,1,10,bins,0,1},"Q2","z");
      //h_Q2_z_pos->RebinX(Rebin_n);
      //h_Q2_z_pos->RebinY(Rebin_n);
      h_Q2_z_pos->Write();
      
      //auto get_x_weight_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_pos->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_pos->GetBinContent(z_bin_number,x_bin_number);
      //  //-h_x_z_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_pos->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_pos->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_pos->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_pos->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_pos->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_pos->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_pos_pi_after = d_pos_pi
      //  .Define("weighted_xbj",get_x_weight_posxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_posxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_posxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_posxz,{"xprime","zprime"})
      //  ;
      auto h_1dweight_xbj = d_pos_pi.Histo1D({"1dweighted_xbj","1dweighted_xbj",bins,0,1},"z","xbj");
      auto h_1dweight_xbj2 = d_pos_pi.Histo1D({"1dweighted_xbj2","1dweighted_xbj2",bins,0,1},"z","xbj2");
      auto h_1dweight_Q2 = d_pos_pi.Histo1D({"1dweighted_Q2","1dweighted_Q2",bins,0,1},"z","Q2");
      auto h_1dweight_Q22 = d_pos_pi.Histo1D({"1dweighted_Q22","1dweighted_Q22",bins,0,1},"z","Q22");
      auto h_1dweight_W2 = d_pos_pi.Histo1D({"1dweighted_W2","1dweighted_W2",bins,0,1},"z","W2");
      auto h_1dweight_W22 = d_pos_pi.Histo1D({"1dweighted_W22","1dweighted_W22",bins,0,1},"z","W22");
      auto h_1dweight_Wp2 = d_pos_pi.Histo1D({"1dweighted_Wp2","1dweighted_Wp2",bins,0,1},"z","Wp2");
      auto h_1dweight_Wp22 = d_pos_pi.Histo1D({"1dweighted_Wp22","1dweighted_Wp22",bins,0,1},"z","Wp22");
      auto h_1dweight_z = d_pos_pi.Histo1D({"1dweighted_z","1dweighted_z",bins,0,1},"z","z");
      auto h_1dweight_z2 = d_pos_pi.Histo1D({"1dweighted_z2","1dweighted_z2",bins,0,1},"z","z2");
      auto h_1dweight_xprime = d_pos_pi.Histo1D({"1dweighted_xprime","1dweighted_xprime",bins,0,1},"z","xprime");
      auto h_1dweight_zprime = d_pos_pi.Histo1D({"1dweighted_zprime","1dweighted_zprime",bins,0,1},"z","zprime");
      auto h_1dweight_xprime2 = d_pos_pi.Histo1D({"1dweighted_xprime2","1dweighted_xprime2",bins,0,1},"z","xprime2");
      auto h_1dweight_zprime2 = d_pos_pi.Histo1D({"1dweighted_zprime2","1dweighted_zprime2",bins,0,1},"z","zprime2");
      //h_1dweight_xbj->RebinX(Rebin_n);
      //h_1dweight_xbj->RebinY(Rebin_n);
      h_1dweight_xbj->Write();
      h_1dweight_xbj2->Write();
      h_1dweight_Q2->Write();
      h_1dweight_Q22->Write();
      //h_1dweight_z->RebinX(Rebin_n);
      //h_1dweight_z->RebinY(Rebin_n);
      h_1dweight_z->Write();
      h_1dweight_z2->Write();
      h_1dweight_xprime->Write();
      h_1dweight_zprime->Write();
      h_1dweight_xprime2->Write();
      h_1dweight_zprime2->Write();
      h_1dweight_W2->Write();
      h_1dweight_W22->Write();
      h_1dweight_Wp2->Write();
      h_1dweight_Wp22->Write();
      auto h_weight_xbj = d_pos_pi.Histo2D({"weighted_xbj","weighted_xbj",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2 = d_pos_pi.Histo2D({"weighted_xbj2","weighted_xbj2",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2 = d_pos_pi.Histo2D({"weighted_Q2","weighted_Q2",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22 = d_pos_pi.Histo2D({"weighted_Q22","weighted_Q22",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_W2 = d_pos_pi.Histo2D({"weighted_W2","weighted_W2",bins,0,1,bins,0,1},"z","xbj","W2");
      auto h_weight_W22 = d_pos_pi.Histo2D({"weighted_W22","weighted_W22",bins,0,1,bins,0,1},"z","xbj","W22");
      auto h_weight_Wp2 = d_pos_pi.Histo2D({"weighted_Wp2","weighted_Wp2",bins,0,1,bins,0,1},"z","xbj","Wp2");
      auto h_weight_Wp22 = d_pos_pi.Histo2D({"weighted_Wp22","weighted_Wp22",bins,0,1,bins,0,1},"z","xbj","Wp22");
      auto h_weight_z = d_pos_pi.Histo2D({"weighted_z","weighted_z",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2 = d_pos_pi.Histo2D({"weighted_z2","weighted_z2",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime = d_pos_pi.Histo2D({"weighted_xprime","weighted_xprime",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime = d_pos_pi.Histo2D({"weighted_zprime","weighted_zprime",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2 = d_pos_pi.Histo2D({"weighted_xprime2","weighted_xprime2",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2 = d_pos_pi.Histo2D({"weighted_zprime2","weighted_zprime2",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      auto h_weight_shms_p = d_pos_pi.Histo2D({"weighted_shms_p","weighted_shms_p",bins,0,1,bins,0,1},"z","xbj","shms_p");
      auto h_weight_shms_p2 = d_pos_pi.Histo2D({"weighted_shms_p2","weighted_shms_p2",bins,0,1,bins,0,1},"z","xbj","shms_p2");
      auto h_weight_shms_dp = d_pos_pi.Histo2D({"weighted_shms_dp","weighted_shms_dp",bins,0,1,bins,0,1},"z","xbj","P_gtr_dp");
      auto h_weight_shms_dp2 = d_pos_pi.Histo2D({"weighted_shms_dp2","weighted_shms_dp2",bins,0,1,bins,0,1},"z","xbj","shms_dp2");
      //h_weight_xbj->RebinX(Rebin_n);
      //h_weight_xbj->RebinY(Rebin_n);
      h_weight_xbj->Write();
      h_weight_xbj2->Write();
      //h_weight_z->RebinX(Rebin_n);
      //h_weight_z->RebinY(Rebin_n);
      h_weight_z->Write();
      h_weight_z2->Write();
      h_weight_xprime->Write();
      h_weight_zprime->Write();
      h_weight_xprime2->Write();
      h_weight_zprime2->Write();
      h_weight_Q2->Write();
      h_weight_Q22->Write();
      h_weight_shms_p->Write();
      h_weight_shms_p2->Write();
      h_weight_shms_dp->Write();
      h_weight_shms_dp2->Write();
      h_weight_W2->Write();
      h_weight_W22->Write();
      h_weight_Wp2->Write();
      h_weight_Wp22->Write();
      
      //auto get_x_weight_1_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_1->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_pos_1->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_pos_1->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_1_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_1->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_pos_1->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_pos_1->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_1_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_1->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_pos_1->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_pos_1->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_1_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_1->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_pos_1->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_pos_1->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_pos_pi_1_after = d_pos_pi_1
      //  .Define("weighted_xbj",get_x_weight_1_posxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_1_posxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_1_posxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_1_posxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_1 = d_pos_pi_1.Histo2D({"weighted_xbj_1","weighted_xbj_1",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_1 = d_pos_pi_1.Histo2D({"weighted_xbj2_1","weighted_xbj2_1",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_1 = d_pos_pi_1.Histo2D({"weighted_Q2_1","weighted_Q2_1",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_1 = d_pos_pi_1.Histo2D({"weighted_Q22_1","weighted_Q22_1",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_1 = d_pos_pi_1.Histo2D({"weighted_z_1","weighted_z_1",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_1 = d_pos_pi_1.Histo2D({"weighted_z2_1","weighted_z2_1",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_1 = d_pos_pi_1.Histo2D({"weighted_xprime_1","weighted_xprime_1",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_1 = d_pos_pi_1.Histo2D({"weighted_zprime_1","weighted_zprime_1",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_1 = d_pos_pi_1.Histo2D({"weighted_xprime2_1","weighted_xprime2_1",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_1 = d_pos_pi_1.Histo2D({"weighted_zprime2_1","weighted_zprime2_1",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_1->Write();
      h_weight_z_1->Write();
      h_weight_xbj2_1->Write();
      h_weight_z2_1->Write();
      h_weight_Q2_1->Write();
      h_weight_Q22_1->Write();
      h_weight_xprime_1->Write();
      h_weight_zprime_1->Write();
      h_weight_xprime2_1->Write();
      h_weight_zprime2_1->Write();
      
      //auto get_x_weight_2_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_2->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_pos_2->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_pos_2->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_2_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_2->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_pos_2->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_pos_2->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_2_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_2->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_pos_2->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_pos_2->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_2_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_2->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_pos_2->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_pos_2->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_pos_pi_2_after = d_pos_pi_2
      //  .Define("weighted_xbj",get_x_weight_2_posxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_2_posxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_2_posxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_2_posxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_2 = d_pos_pi_2.Histo2D({"weighted_xbj_2","weighted_xbj_2",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_2 = d_pos_pi_2.Histo2D({"weighted_xbj2_2","weighted_xbj2_2",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_2 = d_pos_pi_2.Histo2D({"weighted_Q2_2","weighted_Q2_2",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_2 = d_pos_pi_2.Histo2D({"weighted_Q22_2","weighted_Q22_2",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_2 = d_pos_pi_2.Histo2D({"weighted_z_2","weighted_z_2",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_2 = d_pos_pi_2.Histo2D({"weighted_z2_2","weighted_z2_2",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_2 = d_pos_pi_2.Histo2D({"weighted_xprime_2","weighted_xprime_2",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_2 = d_pos_pi_2.Histo2D({"weighted_zprime_2","weighted_zprime_2",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_2 = d_pos_pi_2.Histo2D({"weighted_xprime2_2","weighted_xprime2_2",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_2 = d_pos_pi_2.Histo2D({"weighted_zprime2_2","weighted_zprime2_2",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_2->Write();
      h_weight_z_2->Write();
      h_weight_xbj2_2->Write();
      h_weight_z2_2->Write();
      h_weight_Q2_2->Write();
      h_weight_Q22_2->Write();
      h_weight_xprime_2->Write();
      h_weight_zprime_2->Write();
      h_weight_xprime2_2->Write();
      h_weight_zprime2_2->Write();
      
      //auto get_x_weight_3_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_3->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_pos_3->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_pos_3->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_3_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_pos_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_pos_3->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_pos_3->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_pos_3->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_3_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_3->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_pos_3->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_pos_3->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_3_posxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_pos_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_pos_3->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_pos_3->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_pos_3->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_pos_pi_3_after = d_pos_pi_3
      //  .Define("weighted_xbj",get_x_weight_3_posxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_3_posxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_3_posxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_3_posxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_3 = d_pos_pi_3.Histo2D({"weighted_xbj_3","weighted_xbj_3",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_3 = d_pos_pi_3.Histo2D({"weighted_xbj2_3","weighted_xbj2_3",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_3 = d_pos_pi_3.Histo2D({"weighted_Q2_3","weighted_Q2_3",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_3 = d_pos_pi_3.Histo2D({"weighted_Q22_3","weighted_Q22_3",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_3 = d_pos_pi_3.Histo2D({"weighted_z_3","weighted_z_3",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_3 = d_pos_pi_3.Histo2D({"weighted_z2_3","weighted_z2_3",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_3 = d_pos_pi_3.Histo2D({"weighted_xprime_3","weighted_xprime_3",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_3 = d_pos_pi_3.Histo2D({"weighted_zprime_3","weighted_zprime_3",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_3 = d_pos_pi_3.Histo2D({"weighted_xprime2_3","weighted_xprime2_3",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_3 = d_pos_pi_3.Histo2D({"weighted_zprime2_3","weighted_zprime2_3",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_3->Write();
      h_weight_xbj2_3->Write();
      h_weight_Q2_3->Write();
      h_weight_Q22_3->Write();
      h_weight_z_3->Write();
      h_weight_z2_3->Write();
      h_weight_xprime_3->Write();
      h_weight_zprime_3->Write();
      h_weight_xprime2_3->Write();
      h_weight_zprime2_3->Write();
      
      rootfile_out->Close();

    }//for pos


    //for neg runs
    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_raw("T",rootfile_name);

      auto d_neg_pi = d_neg_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Define("xprime2","xprime*xprime")
        .Define("zprime2","zprime*zprime")
        .Filter(pt_cut)
        //.Filter(Mx2_cut)
        //.Filter(rf_cut,{"diff_time_mod"})
        .Filter(W2_cut)
        .Filter(Wp2_cut)
        .Filter(SHMS_hgc_cut,{"shms_p","P_hgcer_npeSum","P_hgcer_xAtCer","P_hgcer_yAtCer"})
        .Define("xbj2","xbj*xbj")
        .Define("z2","z*z")
        .Define("Q22","Q2*Q2")
        .Define("shms_p2","shms_p*shms_p")
        .Define("shms_dp2","P_gtr_dp*P_gtr_dp")
        .Define("W22","W2*W2")
        .Define("Wp22","Wp2*Wp2")
        ;
      //int pion_n = *d_neg_pi.Count();
      //jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      auto d_neg_pi_1 = d_neg_pi.Filter(Q2_low_cut);
      auto d_neg_pi_2 = d_neg_pi.Filter(Q2_middle_cut);
      auto d_neg_pi_3 = d_neg_pi.Filter(Q2_high_cut);

      // for bg
      ROOT::RDataFrame d_neg_bgraw("T_bg",rootfile_name);
      auto d_neg_bg = d_neg_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Filter(pt_cut)
        //.Filter(Mx2_cut)
        //.Filter(rf_cut,{"diff_time_mod"})
        .Filter(W2_cut)
        .Filter(Wp2_cut)
        .Filter(SHMS_hgc_cut,{"shms_p","P_hgcer_npeSum","P_hgcer_xAtCer","P_hgcer_yAtCer"})
        ;
      auto d_neg_bg_1 = d_neg_bg.Filter(Q2_low_cut);
      auto d_neg_bg_2 = d_neg_bg.Filter(Q2_middle_cut);
      auto d_neg_bg_3 = d_neg_bg.Filter(Q2_high_cut);

      //int bg_n = *d_neg_bg.Count();
      //jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;


      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_neg = d_neg_pi.Histo2D({"Q2_x","Q2_x",bins,0,1,bins,0,10},"xbj","Q2");
      //h_Q2_x_neg->RebinX(Rebin_n);
      //h_Q2_x_neg->RebinY(Rebin_n);
      h_Q2_x_neg->Write();
      auto h_Q2 = d_neg_pi.Histo1D({"Q2","Q2",bins,0,10},"Q2");
      h_Q2->Write();
      auto h_omega = d_neg_pi.Histo1D({"omega","omega",100,0,10},"H_kin_primary_omega");
      h_omega->Write();
      auto h_shmsdp = d_neg_pi.Histo1D({"shmsdp","shmsdp",100,-10,25},"P_gtr_dp");
      h_shmsdp->Write();
      auto h_hmsdp = d_neg_pi.Histo1D({"hmsdp","hmsdp",100,-10,12},"H_gtr_dp");
      h_hmsdp->Write();
      auto h_Q2_1 = d_neg_pi_1.Histo1D({"Q2_1","Q2_1",bins,0,10},"Q2");
      auto h_Q2_2 = d_neg_pi_2.Histo1D({"Q2_2","Q2_2",bins,0,10},"Q2");
      auto h_Q2_3 = d_neg_pi_3.Histo1D({"Q2_3","Q2_3",bins,0,10},"Q2");
      h_Q2_1->Write();
      h_Q2_2->Write();
      h_Q2_3->Write();
      auto h_xbj = d_neg_pi.Histo1D({"xbj","xbj",bins,0,1},"xbj");
      double pion_n_integ = h_xbj->Integral();
      double pion_n = *d_neg_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      jout[(std::to_string(RunNumber)).c_str()]["pion_n_integ"] = pion_n_integ;
      //h_xbj->Rebin(Rebin_n);
      h_xbj->Write();
      auto h_xbj_1 = d_neg_pi_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj");
      auto h_xbj_2 = d_neg_pi_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj");
      auto h_xbj_3 = d_neg_pi_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj");
      h_xbj_1->Write();
      h_xbj_2->Write();
      h_xbj_3->Write();
      auto h_xprime_1 = d_neg_pi_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime");
      auto h_xprime_2 = d_neg_pi_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime");
      auto h_xprime_3 = d_neg_pi_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime");
      h_xprime_1->Write();
      h_xprime_2->Write();
      h_xprime_3->Write();
      auto h_z = d_neg_pi.Histo1D({"z","z",bins,0,1},"z");
      //h_z->Rebin(Rebin_n);
      h_z->Write();
      auto h_z_1 = d_neg_pi_1.Histo1D({"z_1","z_1",bins,0,1},"z");
      auto h_z_2 = d_neg_pi_2.Histo1D({"z_2","z_2",bins,0,1},"z");
      auto h_z_3 = d_neg_pi_3.Histo1D({"z_3","z_3",bins,0,1},"z");
      h_z_1->Write();
      h_z_2->Write();
      h_z_3->Write();
      auto h_zprime_1 = d_neg_pi_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime");
      auto h_zprime_2 = d_neg_pi_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime");
      auto h_zprime_3 = d_neg_pi_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime");
      h_zprime_1->Write();
      h_zprime_2->Write();
      h_zprime_3->Write();
      auto h_Q2_bg = d_neg_bg.Histo1D({"Q2_bg","Q2_bg",bins,0,10},"Q2");
      h_Q2_bg->Write();
      auto h_omega_bg = d_neg_bg.Histo1D({"omega_bg","omega_bg",100,0,10},"H_kin_primary_omega");
      h_omega_bg->Write();
      auto h_shmsdp_bg = d_neg_bg.Histo1D({"shmsdp_bg","shmsdp_bg",100,-10,25},"P_gtr_dp");
      h_shmsdp_bg->Write();
      auto h_hmsdp_bg = d_neg_bg.Histo1D({"hmsdp_bg","hmsdp_bg",100,-10,12},"H_gtr_dp");
      h_hmsdp_bg->Write();
      auto h_Q2_bg_1 = d_neg_bg_1.Histo1D({"Q2_bg_1","Q2_bg_1",bins,0,10},"Q2");
      auto h_Q2_bg_2 = d_neg_bg_2.Histo1D({"Q2_bg_2","Q2_bg_2",bins,0,10},"Q2");
      auto h_Q2_bg_3 = d_neg_bg_3.Histo1D({"Q2_bg_3","Q2_bg_3",bins,0,10},"Q2");
      h_Q2_bg_1->Write();
      h_Q2_bg_2->Write();
      h_Q2_bg_3->Write();
      auto h_xbj_bg = d_neg_bg.Histo1D({"xbj_bg","xbj_bg",bins,0,1},"xbj");
      double bg_n_integ = h_xbj_bg->Integral();
      double bg_n = *d_neg_bg.Count(); 
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;
      jout[(std::to_string(RunNumber)).c_str()]["bg_n_integ"] = bg_n_integ;
      //h_xbj_bg->Rebin(Rebin_n);
      h_xbj_bg->Write();
      auto h_xbj_bg_1 = d_neg_bg_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj");
      auto h_xbj_bg_2 = d_neg_bg_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj");
      auto h_xbj_bg_3 = d_neg_bg_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj");
      h_xbj_bg_1->Write();
      h_xbj_bg_2->Write();
      h_xbj_bg_3->Write();
      auto h_xprime_bg_1 = d_neg_bg_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime");
      auto h_xprime_bg_2 = d_neg_bg_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime");
      auto h_xprime_bg_3 = d_neg_bg_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime");
      h_xprime_bg_1->Write();
      h_xprime_bg_2->Write();
      h_xprime_bg_3->Write();
      auto h_z_bg = d_neg_bg.Histo1D({"z_bg","z_bg",bins,0,1},"z");
      //h_z_bg->Rebin(Rebin_n);
      h_z_bg->Write();
      auto h_z_bg_1 = d_neg_bg_1.Histo1D({"z_1","z_1",bins,0,1},"z");
      auto h_z_bg_2 = d_neg_bg_2.Histo1D({"z_2","z_2",bins,0,1},"z");
      auto h_z_bg_3 = d_neg_bg_3.Histo1D({"z_3","z_3",bins,0,1},"z");
      h_z_bg_1->Write();
      h_z_bg_2->Write();
      h_z_bg_3->Write();
      auto h_zprime_bg_1 = d_neg_bg_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime");
      auto h_zprime_bg_2 = d_neg_bg_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime");
      auto h_zprime_bg_3 = d_neg_bg_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime");
      h_zprime_bg_1->Write();
      h_zprime_bg_2->Write();
      h_zprime_bg_3->Write();
      auto h_x_z_neg = d_neg_pi.Histo2D({"x_z","x_z",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_neg->Write();
      auto h_x_z_neg_raw = d_neg_pi.Histo2D({"x_z_raw","x_z_raw",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_neg_raw->Write();
      auto h_x_z_bg = d_neg_bg.Histo2D({"x_z_bg","x_z_bg",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg->Write();
      auto h_x_z_neg_1 = d_neg_pi_1.Histo2D({"x_z_1","x_z_1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_neg_1->Write();
      auto h_x_z_negraw_1 = d_neg_pi_1.Histo2D({"x_z_raw1","x_z_raw1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_negraw_1->Write();
      auto h_x_z_bg_1 = d_neg_bg_1.Histo2D({"x_z_bg_1","x_z_bg_1",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_1->Write();
      auto h_x_z_neg_2 = d_neg_pi_2.Histo2D({"x_z_2","x_z_2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_neg_2->Write();
      auto h_x_z_negraw_2 = d_neg_pi_2.Histo2D({"x_z_raw2","x_z_raw2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_negraw_2->Write();
      auto h_x_z_bg_2 = d_neg_bg_2.Histo2D({"x_z_bg_2","x_z_bg_2",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_2->Write();
      auto h_x_z_neg_3 = d_neg_pi_3.Histo2D({"x_z_3","x_z_3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_neg_3->Write();
      auto h_x_z_negraw_3 = d_neg_pi_3.Histo2D({"x_z_raw3","x_z_raw3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_negraw_3->Write();
      auto h_x_z_bg_3 = d_neg_bg_3.Histo2D({"x_z_bg_3","x_z_bg_3",bins,0,1,bins,0,1},"z","xbj");
      h_x_z_bg_3->Write();
     
      auto h_xzprime_neg = d_neg_pi.Histo2D({"xzprime","xzprime",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_neg->Write();
      auto h_xzprime_bg = d_neg_bg.Histo2D({"xzprime_bg","xzprime_bg",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg->Write();
      auto h_xzprime_neg_1 = d_neg_pi_1.Histo2D({"xzprime_1","xzprime_1",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_neg_1->Write();
      auto h_xzprime_bg_1 = d_neg_bg_1.Histo2D({"xzprime_bg_1","xzprime_bg_1",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_1->Write();
      auto h_xzprime_neg_2 = d_neg_pi_2.Histo2D({"xzprime_2","xzprime_2",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_neg_1->Write();
      auto h_xzprime_bg_2 = d_neg_bg_2.Histo2D({"xzprime_bg_2","xzprime_bg_2",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_2->Write();
      auto h_xzprime_neg_3 = d_neg_pi_3.Histo2D({"xzprime_3","xzprime_3",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_neg_1->Write();
      auto h_xzprime_bg_3 = d_neg_bg_3.Histo2D({"xzprime_bg_3","xzprime_bg_3",bins,0,1,bins,0,1},"zprime","xprime");
      h_xzprime_bg_3->Write();

      auto h_Q2_z_neg = d_neg_pi.Histo2D({"Q2_z","Q2_z",bins,1,10,bins,0,1},"Q2","z");
      //h_Q2_z_neg->RebinX(Rebin_n);
      //h_Q2_z_neg->RebinY(Rebin_n);
      h_Q2_z_neg->Write();
      
      //auto get_x_weight_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_neg->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_neg->GetBinContent(z_bin_number,x_bin_number);
      //  //-h_x_z_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_neg->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_neg->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_neg->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_neg->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_neg->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_neg->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_neg_pi_after = d_neg_pi
      //  .Define("weighted_xbj",get_x_weight_negxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_negxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_negxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_negxz,{"xprime","zprime"})
      //  ;
      auto h_1dweight_xbj = d_neg_pi.Histo1D({"1dweighted_xbj","1dweighted_xbj",bins,0,1},"z","xbj");
      auto h_1dweight_xbj2 = d_neg_pi.Histo1D({"1dweighted_xbj2","1dweighted_xbj2",bins,0,1},"z","xbj2");
      auto h_1dweight_Q2 = d_neg_pi.Histo1D({"1dweighted_Q2","1dweighted_Q2",bins,0,1},"z","Q2");
      auto h_1dweight_Q22 = d_neg_pi.Histo1D({"1dweighted_Q22","1dweighted_Q22",bins,0,1},"z","Q22");
      auto h_1dweight_W2 = d_neg_pi.Histo1D({"1dweighted_W2","1dweighted_W2",bins,0,1},"z","W2");
      auto h_1dweight_W22 = d_neg_pi.Histo1D({"1dweighted_W22","1dweighted_W22",bins,0,1},"z","W22");
      auto h_1dweight_Wp2 = d_neg_pi.Histo1D({"1dweighted_Wp2","1dweighted_Wp2",bins,0,1},"z","Wp2");
      auto h_1dweight_Wp22 = d_neg_pi.Histo1D({"1dweighted_Wp22","1dweighted_Wp22",bins,0,1},"z","Wp22");
      auto h_1dweight_z = d_neg_pi.Histo1D({"1dweighted_z","1dweighted_z",bins,0,1},"z","z");
      auto h_1dweight_z2 = d_neg_pi.Histo1D({"1dweighted_z2","1dweighted_z2",bins,0,1},"z","z2");
      auto h_1dweight_xprime = d_neg_pi.Histo1D({"1dweighted_xprime","1dweighted_xprime",bins,0,1},"z","xprime");
      auto h_1dweight_zprime = d_neg_pi.Histo1D({"1dweighted_zprime","1dweighted_zprime",bins,0,1},"z","zprime");
      auto h_1dweight_xprime2 = d_neg_pi.Histo1D({"1dweighted_xprime2","1dweighted_xprime2",bins,0,1},"z","xprime2");
      auto h_1dweight_zprime2 = d_neg_pi.Histo1D({"1dweighted_zprime2","1dweighted_zprime2",bins,0,1},"z","zprime2");
      //h_1dweight_xbj->RebinX(Rebin_n);
      //h_1dweight_xbj->RebinY(Rebin_n);
      h_1dweight_xbj->Write();
      h_1dweight_xbj2->Write();
      h_1dweight_Q2->Write();
      h_1dweight_Q22->Write();
      //h_1dweight_z->RebinX(Rebin_n);
      //h_1dweight_z->RebinY(Rebin_n);
      h_1dweight_z->Write();
      h_1dweight_z2->Write();
      h_1dweight_xprime->Write();
      h_1dweight_zprime->Write();
      h_1dweight_xprime2->Write();
      h_1dweight_zprime2->Write();
      h_1dweight_W2->Write();
      h_1dweight_W22->Write();
      h_1dweight_Wp2->Write();
      h_1dweight_Wp22->Write();
      auto h_weight_xbj = d_neg_pi.Histo2D({"weighted_xbj","weighted_xbj",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2 = d_neg_pi.Histo2D({"weighted_xbj2","weighted_xbj2",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2 = d_neg_pi.Histo2D({"weighted_Q2","weighted_Q2",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22 = d_neg_pi.Histo2D({"weighted_Q22","weighted_Q22",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_W2 = d_neg_pi.Histo2D({"weighted_W2","weighted_W2",bins,0,1,bins,0,1},"z","xbj","W2");
      auto h_weight_W22 = d_neg_pi.Histo2D({"weighted_W22","weighted_W22",bins,0,1,bins,0,1},"z","xbj","W22");
      auto h_weight_Wp2 = d_neg_pi.Histo2D({"weighted_Wp2","weighted_Wp2",bins,0,1,bins,0,1},"z","xbj","Wp2");
      auto h_weight_Wp22 = d_neg_pi.Histo2D({"weighted_Wp22","weighted_Wp22",bins,0,1,bins,0,1},"z","xbj","Wp22");
      auto h_weight_z = d_neg_pi.Histo2D({"weighted_z","weighted_z",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2 = d_neg_pi.Histo2D({"weighted_z2","weighted_z2",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime = d_neg_pi.Histo2D({"weighted_xprime","weighted_xprime",bins,0,1,bins,0,1},"z","xbj","xprime");
      auto h_weight_zprime = d_neg_pi.Histo2D({"weighted_zprime","weighted_zprime",bins,0,1,bins,0,1},"z","xbj","zprime");
      auto h_weight_xprime2 = d_neg_pi.Histo2D({"weighted_xprime2","weighted_xprime2",bins,0,1,bins,0,1},"z","xbj","xprime2");
      auto h_weight_zprime2 = d_neg_pi.Histo2D({"weighted_zprime2","weighted_zprime2",bins,0,1,bins,0,1},"z","xbj","zprime2");
      auto h_weight_shms_p = d_neg_pi.Histo2D({"weighted_shms_p","weighted_shms_p",bins,0,1,bins,0,1},"z","xbj","shms_p");
      auto h_weight_shms_p2 = d_neg_pi.Histo2D({"weighted_shms_p2","weighted_shms_p2",bins,0,1,bins,0,1},"z","xbj","shms_p2");
      auto h_weight_shms_dp = d_neg_pi.Histo2D({"weighted_shms_dp","weighted_shms_dp",bins,0,1,bins,0,1},"z","xbj","P_gtr_dp");
      auto h_weight_shms_dp2 = d_neg_pi.Histo2D({"weighted_shms_dp2","weighted_shms_dp2",bins,0,1,bins,0,1},"z","xbj","shms_dp2");
      //h_weight_xbj->RebinX(Rebin_n);
      //h_weight_xbj->RebinY(Rebin_n);
      h_weight_xbj->Write();
      h_weight_xbj2->Write();
      h_weight_Q2->Write();
      h_weight_Q22->Write();
      h_weight_shms_p->Write();
      h_weight_shms_p2->Write();
      h_weight_shms_dp->Write();
      h_weight_shms_dp2->Write();
      //h_weight_z->RebinX(Rebin_n);
      //h_weight_z->RebinY(Rebin_n);
      h_weight_z->Write();
      h_weight_z2->Write();
      h_weight_xprime->Write();
      h_weight_zprime->Write();
      h_weight_xprime2->Write();
      h_weight_zprime2->Write();
      h_weight_W2->Write();
      h_weight_W22->Write();
      h_weight_Wp2->Write();
      h_weight_Wp22->Write();
      
      //auto get_x_weight_1_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_1->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_neg_1->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_neg_1->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_1_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_1->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_neg_1->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_neg_1->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_1_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_1->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_neg_1->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_neg_1->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_1_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_1->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_1->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_neg_1->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_neg_1->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_1->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_neg_pi_1_after = d_neg_pi_1
      //  .Define("weighted_xbj",get_x_weight_1_negxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_1_negxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_1_negxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_1_negxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_1 = d_neg_pi_1.Histo2D({"weighted_xbj_1","weighted_xbj_1",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_1 = d_neg_pi_1.Histo2D({"weighted_xbj2_1","weighted_xbj2_1",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_1 = d_neg_pi_1.Histo2D({"weighted_Q2_1","weighted_Q2_1",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_1 = d_neg_pi_1.Histo2D({"weighted_Q22_1","weighted_Q22_1",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_1 = d_neg_pi_1.Histo2D({"weighted_z_1","weighted_z_1",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_1 = d_neg_pi_1.Histo2D({"weighted_z2_1","weighted_z2_1",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_1 = d_neg_pi_1.Histo2D({"weighted_xprime_1","weighted_xprime_1",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_1 = d_neg_pi_1.Histo2D({"weighted_zprime_1","weighted_zprime_1",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_1 = d_neg_pi_1.Histo2D({"weighted_xprime2_1","weighted_xprime2_1",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_1 = d_neg_pi_1.Histo2D({"weighted_zprime2_1","weighted_zprime2_1",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_1->Write();
      h_weight_xbj2_1->Write();
      h_weight_Q2_1->Write();
      h_weight_Q22_1->Write();
      h_weight_z_1->Write();
      h_weight_z2_1->Write();
      h_weight_xprime_1->Write();
      h_weight_zprime_1->Write();
      h_weight_xprime2_1->Write();
      h_weight_zprime2_1->Write();
      
      //auto get_x_weight_2_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_2->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_neg_2->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_neg_2->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_2_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_2->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_neg_2->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_neg_2->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_2_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_2->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_neg_2->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_neg_2->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_2_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_2->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_2->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_neg_2->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_neg_2->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_2->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_neg_pi_2_after = d_neg_pi_2
      //  .Define("weighted_xbj",get_x_weight_2_negxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_2_negxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_2_negxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_2_negxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_2 = d_neg_pi_2.Histo2D({"weighted_xbj_2","weighted_xbj_2",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_2 = d_neg_pi_2.Histo2D({"weighted_xbj2_2","weighted_xbj2_2",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_2 = d_neg_pi_2.Histo2D({"weighted_Q2_2","weighted_Q2_2",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_2 = d_neg_pi_2.Histo2D({"weighted_Q22_2","weighted_Q22_2",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_2 = d_neg_pi_2.Histo2D({"weighted_z_2","weighted_z_2",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_2 = d_neg_pi_2.Histo2D({"weighted_z2_2","weighted_z2_2",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_2 = d_neg_pi_2.Histo2D({"weighted_xprime_2","weighted_xprime_2",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_2 = d_neg_pi_2.Histo2D({"weighted_zprime_2","weighted_zprime_2",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_2 = d_neg_pi_2.Histo2D({"weighted_xprime2_2","weighted_xprime2_2",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_2 = d_neg_pi_2.Histo2D({"weighted_zprime2_2","weighted_zprime2_2",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_2->Write();
      h_weight_xbj2_2->Write();
      h_weight_Q2_2->Write();
      h_weight_Q22_2->Write();
      h_weight_z_2->Write();
      h_weight_z2_2->Write();
      h_weight_xprime_2->Write();
      h_weight_zprime_2->Write();
      h_weight_xprime2_2->Write();
      h_weight_zprime2_2->Write();

      //auto get_x_weight_3_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_3->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_x_z_neg_3->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_x_z_neg_3->GetBinContent(z_bin_number,x_bin_number);//-h_x_z_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_z_weight_3_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_x_z_neg_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_x_z_neg_3->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_x_z_neg_3->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_x_z_neg_3->GetBinContent(z_bin_number,x_bin_number)-h_x_z_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto get_xprime_weight_3_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_3->GetXaxis()->FindBin(z);
      //  double x_bincenter = h_xzprime_neg_3->GetYaxis()->GetBinCenter(x_bin_number);
      //  double all = h_xzprime_neg_3->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return x/(all*x_bincenter);
      //  //return x/all;
      //};
      //auto get_zprime_weight_3_negxz = [&](double x,double z){
      //
      //  int x_bin_number = h_xzprime_neg_3->GetYaxis()->FindBin(x);
      //  int z_bin_number = h_xzprime_neg_3->GetXaxis()->FindBin(z);
      //  double z_bincenter = h_xzprime_neg_3->GetXaxis()->GetBinCenter(z_bin_number);
      //  double all = h_xzprime_neg_3->GetBinContent(z_bin_number,x_bin_number)-h_xzprime_bg_3->GetBinContent(z_bin_number,x_bin_number)/6.0;
      //  return z/(all*z_bincenter);
      //  //return z/all;
      //};
      //auto d_neg_pi_3_after = d_neg_pi_3
      //  .Define("weighted_xbj",get_x_weight_3_negxz,{"xbj","z"})
      //  .Define("weighted_z",get_z_weight_3_negxz,{"xbj","z"})
      //  .Define("weighted_xprime",get_xprime_weight_3_negxz,{"xprime","zprime"})
      //  .Define("weighted_zprime",get_zprime_weight_3_negxz,{"xprime","zprime"})
      //  ;
      auto h_weight_xbj_3 = d_neg_pi_3.Histo2D({"weighted_xbj_3","weighted_xbj_3",bins,0,1,bins,0,1},"z","xbj","xbj");
      auto h_weight_xbj2_3 = d_neg_pi_3.Histo2D({"weighted_xbj2_3","weighted_xbj2_3",bins,0,1,bins,0,1},"z","xbj","xbj2");
      auto h_weight_Q2_3 = d_neg_pi_3.Histo2D({"weighted_Q2_3","weighted_Q2_3",bins,0,1,bins,0,1},"z","xbj","Q2");
      auto h_weight_Q22_3 = d_neg_pi_3.Histo2D({"weighted_Q22_3","weighted_Q22_3",bins,0,1,bins,0,1},"z","xbj","Q22");
      auto h_weight_z_3 = d_neg_pi_3.Histo2D({"weighted_z_3","weighted_z_3",bins,0,1,bins,0,1},"z","xbj","z");
      auto h_weight_z2_3 = d_neg_pi_3.Histo2D({"weighted_z2_3","weighted_z2_3",bins,0,1,bins,0,1},"z","xbj","z2");
      auto h_weight_xprime_3 = d_neg_pi_3.Histo2D({"weighted_xprime_3","weighted_xprime_3",bins,0,1,bins,0,1},"zprime","xprime","xprime");
      auto h_weight_zprime_3 = d_neg_pi_3.Histo2D({"weighted_zprime_3","weighted_zprime_3",bins,0,1,bins,0,1},"zprime","xprime","zprime");
      auto h_weight_xprime2_3 = d_neg_pi_3.Histo2D({"weighted_xprime2_3","weighted_xprime2_3",bins,0,1,bins,0,1},"zprime","xprime","xprime2");
      auto h_weight_zprime2_3 = d_neg_pi_3.Histo2D({"weighted_zprime2_3","weighted_zprime2_3",bins,0,1,bins,0,1},"zprime","xprime","zprime2");
      h_weight_xbj_3->Write();
      h_weight_xbj2_3->Write();
      h_weight_Q2_3->Write();
      h_weight_Q22_3->Write();
      h_weight_z_3->Write();
      h_weight_z2_3->Write();
      h_weight_xprime_3->Write();
      h_weight_zprime_3->Write();
      h_weight_xprime2_3->Write();
      h_weight_zprime2_3->Write();
      
      rootfile_out->Close();

    }//for neg



  }//not empty

    std::string of_name = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
  std::ofstream ofs(of_name.c_str());
  ofs<<jout.dump(4)<<std::endl;
}
