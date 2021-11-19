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
  
  double Q2_low = j_cuts["Q2_low"].get<double>();
  double Q2_high = j_cuts["Q2_high"].get<double>();
  std::string Q2_low_cut = "Q2<"+std::to_string(Q2_low);
  std::string Q2_high_cut = "Q2>"+std::to_string(Q2_high);
  std::string Q2_middle_cut = "Q2>="+std::to_string(Q2_low)+" && Q2<="+std::to_string(Q2_high);


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
        .Filter(pt_cut)
        ;
      int pion_n = *d_pos_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      auto d_pos_pi_1 = d_pos_pi.Filter(Q2_low_cut);
      auto d_pos_pi_2 = d_pos_pi.Filter(Q2_middle_cut);
      auto d_pos_pi_3 = d_pos_pi.Filter(Q2_high_cut);

      // for bg
      ROOT::RDataFrame d_pos_bgraw("T_bg",rootfile_name);
      auto d_pos_bg = d_pos_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Filter(pt_cut)
        ;
      auto d_pos_bg_1 = d_pos_bg.Filter(Q2_low_cut);
      auto d_pos_bg_2 = d_pos_bg.Filter(Q2_middle_cut);
      auto d_pos_bg_3 = d_pos_bg.Filter(Q2_high_cut);

      int bg_n = *d_pos_bg.Count();
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;


      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_pos = d_pos_pi.Histo2D({"Q2_x","Q2_x",bins,0,1,bins,0,10},"xbj","Q2");
      //h_Q2_x_pos->RebinX(Rebin_n);
      //h_Q2_x_pos->RebinY(Rebin_n);
      h_Q2_x_pos->Write();
      auto h_Q2 = d_pos_pi.Histo1D({"Q2","Q2",bins,0,10},"Q2","weight");
      h_Q2->Write();
      auto h_Q2_1 = d_pos_pi_1.Histo1D({"Q2_1","Q2_1",bins,0,10},"Q2","weight");
      auto h_Q2_2 = d_pos_pi_2.Histo1D({"Q2_2","Q2_2",bins,0,10},"Q2","weight");
      auto h_Q2_3 = d_pos_pi_3.Histo1D({"Q2_3","Q2_3",bins,0,10},"Q2","weight");
      h_Q2_1->Write();
      h_Q2_2->Write();
      h_Q2_3->Write();
      auto h_xbj = d_pos_pi.Histo1D({"xbj","xbj",bins,0,1},"xbj","weight");
      //h_xbj->Rebin(Rebin_n);
      h_xbj->Write();
      auto h_xbj_1 = d_pos_pi_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj","weight");
      auto h_xbj_2 = d_pos_pi_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj","weight");
      auto h_xbj_3 = d_pos_pi_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj","weight");
      h_xbj_1->Write();
      h_xbj_2->Write();
      h_xbj_3->Write();
      auto h_xprime_1 = d_pos_pi_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime","weight");
      auto h_xprime_2 = d_pos_pi_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime","weight");
      auto h_xprime_3 = d_pos_pi_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime","weight");
      h_xprime_1->Write();
      h_xprime_2->Write();
      h_xprime_3->Write();
      auto h_z = d_pos_pi.Histo1D({"z","z",bins,0,1},"z","weight");
      //h_z->Rebin(Rebin_n);
      h_z->Write();
      auto h_z_1 = d_pos_pi_1.Histo1D({"z_1","z_1",bins,0,1},"z","weight");
      auto h_z_2 = d_pos_pi_2.Histo1D({"z_2","z_2",bins,0,1},"z","weight");
      auto h_z_3 = d_pos_pi_3.Histo1D({"z_3","z_3",bins,0,1},"z","weight");
      h_z_1->Write();
      h_z_2->Write();
      h_z_3->Write();
      auto h_zprime_1 = d_pos_pi_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime","weight");
      auto h_zprime_2 = d_pos_pi_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime","weight");
      auto h_zprime_3 = d_pos_pi_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime","weight");
      h_zprime_1->Write();
      h_zprime_2->Write();
      h_zprime_3->Write();
      auto h_Q2_bg = d_pos_bg.Histo1D({"Q2_bg","Q2_bg",bins,0,10},"Q2","weight");
      h_Q2_bg->Write();
      auto h_Q2_bg_1 = d_pos_bg_1.Histo1D({"Q2_bg_1","Q2_bg_1",bins,0,10},"Q2","weight");
      auto h_Q2_bg_2 = d_pos_bg_2.Histo1D({"Q2_bg_2","Q2_bg_2",bins,0,10},"Q2","weight");
      auto h_Q2_bg_3 = d_pos_bg_3.Histo1D({"Q2_bg_3","Q2_bg_3",bins,0,10},"Q2","weight");
      h_Q2_bg_1->Write();
      h_Q2_bg_2->Write();
      h_Q2_bg_3->Write();
      auto h_xbj_bg = d_pos_bg.Histo1D({"xbj_bg","xbj_bg",bins,0,1},"xbj","weight");
      //h_xbj_bg->Rebin(Rebin_n);
      h_xbj_bg->Write();
      auto h_xbj_bg_1 = d_pos_bg_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj","weight");
      auto h_xbj_bg_2 = d_pos_bg_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj","weight");
      auto h_xbj_bg_3 = d_pos_bg_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj","weight");
      h_xbj_bg_1->Write();
      h_xbj_bg_2->Write();
      h_xbj_bg_3->Write();
      auto h_xprime_bg_1 = d_pos_bg_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime","weight");
      auto h_xprime_bg_2 = d_pos_bg_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime","weight");
      auto h_xprime_bg_3 = d_pos_bg_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime","weight");
      h_xprime_bg_1->Write();
      h_xprime_bg_2->Write();
      h_xprime_bg_3->Write();
      auto h_z_bg = d_pos_bg.Histo1D({"z_bg","z_bg",bins,0,1},"z","weight");
      //h_z_bg->Rebin(Rebin_n);
      h_z_bg->Write();
      auto h_z_bg_1 = d_pos_bg_1.Histo1D({"z_1","z_1",bins,0,1},"z","weight");
      auto h_z_bg_2 = d_pos_bg_2.Histo1D({"z_2","z_2",bins,0,1},"z","weight");
      auto h_z_bg_3 = d_pos_bg_3.Histo1D({"z_3","z_3",bins,0,1},"z","weight");
      h_z_bg_1->Write();
      h_z_bg_2->Write();
      h_z_bg_3->Write();
      auto h_zprime_bg_1 = d_pos_bg_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime","weight");
      auto h_zprime_bg_2 = d_pos_bg_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime","weight");
      auto h_zprime_bg_3 = d_pos_bg_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime","weight");
      h_zprime_bg_1->Write();
      h_zprime_bg_2->Write();
      h_zprime_bg_3->Write();
      auto h_x_z_pos = d_pos_pi.Histo2D({"x_z","x_z",bins,0,1,bins,0,1},"z","xbj","weight");
      //h_x_z_pos->RebinX(Rebin_n);
      //h_x_z_pos->RebinY(Rebin_n);
      h_x_z_pos->Write();
      auto h_x_z_bg = d_pos_bg.Histo2D({"x_z_bg","x_z_bg",bins,0,1,bins,0,1},"z","xbj","weight");
      //h_x_z_bg->RebinX(Rebin_n);
      //h_x_z_bg->RebinY(Rebin_n);
      h_x_z_bg->Write();

      auto h_Q2_z_pos = d_pos_pi.Histo2D({"Q2_z","Q2_z",bins,1,10,bins,0,1},"Q2","z");
      //h_Q2_z_pos->RebinX(Rebin_n);
      //h_Q2_z_pos->RebinY(Rebin_n);
      h_Q2_z_pos->Write();
      
      auto get_x_weight_posxz = [&](double x,double z){
      
        int x_bin_number = h_x_z_pos->GetYaxis()->FindBin(x);
        int z_bin_number = h_x_z_pos->GetXaxis()->FindBin(z);
        double x_bincenter = h_x_z_pos->GetYaxis()->GetBinCenter(x_bin_number);
        double all = h_x_z_pos->GetBinContent(z_bin_number,x_bin_number);
        return x/(all*x_bincenter);
        //return x/all;
      };
      auto get_z_weight_posxz = [&](double x,double z){
      
        int x_bin_number = h_x_z_pos->GetYaxis()->FindBin(x);
        int z_bin_number = h_x_z_pos->GetXaxis()->FindBin(z);
        double z_bincenter = h_x_z_pos->GetXaxis()->GetBinCenter(z_bin_number);
        double all = h_x_z_pos->GetBinContent(z_bin_number,x_bin_number);
        return z/(all*z_bincenter);
        //return z/all;
      };
      auto d_pos_pi_after = d_pos_pi
        .Define("weighted_xbj",get_x_weight_posxz,{"xbj","z"})
        .Define("weighted_z",get_z_weight_posxz,{"xbj","z"})
        ;
      auto h_weight_xbj = d_pos_pi_after.Histo2D({"weighted_xbj","weighted_xbj",bins,0,1,bins,0,1},"z","xbj","weighted_xbj");
      auto h_weight_z = d_pos_pi_after.Histo2D({"weighted_z","weighted_z",bins,0,1,bins,0,1},"z","xbj","weighted_z");
      
      //h_weight_xbj->RebinX(Rebin_n);
      //h_weight_xbj->RebinY(Rebin_n);
      h_weight_xbj->Write();
      //h_weight_z->RebinX(Rebin_n);
      //h_weight_z->RebinY(Rebin_n);
      h_weight_z->Write();
      
      rootfile_out->Close();

    }

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
        .Filter(pt_cut)
        ;

      int pion_n = *d_neg_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;
      auto d_neg_pi_1 = d_neg_pi.Filter(Q2_low_cut);
      auto d_neg_pi_2 = d_neg_pi.Filter(Q2_middle_cut);
      auto d_neg_pi_3 = d_neg_pi.Filter(Q2_high_cut);

      // for bg
      ROOT::RDataFrame d_neg_bgraw("T_bg",rootfile_name);
      auto d_neg_bg = d_neg_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Define("xprime",xprime,{"Q2","xbj"})
        .Define("zprime",zprime,{"z","xprime","xbj","pt","Q2"})
        .Filter(pt_cut)
        ;
      auto d_neg_bg_1 = d_neg_bg.Filter(Q2_low_cut);
      auto d_neg_bg_2 = d_neg_bg.Filter(Q2_middle_cut);
      auto d_neg_bg_3 = d_neg_bg.Filter(Q2_high_cut);


      int bg_n = *d_neg_bg.Count();
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;

      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_neg = d_neg_pi.Histo2D({"Q2_x","Q2_x",bins,0,1,bins,0,10},"xbj","Q2");
      //h_Q2_x_neg->RebinX(Rebin_n);
      //h_Q2_x_neg->RebinY(Rebin_n);
      h_Q2_x_neg->Write();
      auto h_Q2 = d_neg_pi.Histo1D({"Q2","Q2",bins,0,10},"Q2","weight");
      h_Q2->Write();
      auto h_Q2_1 = d_neg_pi_1.Histo1D({"Q2_1","Q2_1",bins,0,10},"Q2","weight");
      auto h_Q2_2 = d_neg_pi_2.Histo1D({"Q2_2","Q2_2",bins,0,10},"Q2","weight");
      auto h_Q2_3 = d_neg_pi_3.Histo1D({"Q2_3","Q2_3",bins,0,10},"Q2","weight");
      h_Q2_1->Write();
      h_Q2_2->Write();
      h_Q2_3->Write();
      auto h_xbj = d_neg_pi.Histo1D({"xbj","xbj",bins,0,1},"xbj","weight");
      //h_xbj->Rebin(Rebin_n);
      h_xbj->Write();
      auto h_xbj_1 = d_neg_pi_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj","weight");
      auto h_xbj_2 = d_neg_pi_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj","weight");
      auto h_xbj_3 = d_neg_pi_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj","weight");
      h_xbj_1->Write();
      h_xbj_2->Write();
      h_xbj_3->Write();
      auto h_xprime_1 = d_neg_pi_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime","weight");
      auto h_xprime_2 = d_neg_pi_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime","weight");
      auto h_xprime_3 = d_neg_pi_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime","weight");
      h_xprime_1->Write();
      h_xprime_2->Write();
      h_xprime_3->Write();
      auto h_z = d_neg_pi.Histo1D({"z","z",bins,0,1},"z","weight");
      //h_z->Rebin(Rebin_n);
      h_z->Write();
      auto h_z_1 = d_neg_pi_1.Histo1D({"z_1","z_1",bins,0,1},"z","weight");
      auto h_z_2 = d_neg_pi_2.Histo1D({"z_2","z_2",bins,0,1},"z","weight");
      auto h_z_3 = d_neg_pi_3.Histo1D({"z_3","z_3",bins,0,1},"z","weight");
      h_z_1->Write();
      h_z_2->Write();
      h_z_3->Write();
      auto h_zprime_1 = d_neg_pi_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime","weight");
      auto h_zprime_2 = d_neg_pi_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime","weight");
      auto h_zprime_3 = d_neg_pi_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime","weight");
      h_zprime_1->Write();
      h_zprime_2->Write();
      h_zprime_3->Write();
      auto h_Q2_bg = d_neg_bg.Histo1D({"Q2_bg","Q2_bg",bins,0,10},"Q2","weight");
      h_Q2_bg->Write();
      auto h_Q2_bg_1 = d_neg_bg_1.Histo1D({"Q2_bg_1","Q2_bg_1",bins,0,10},"Q2","weight");
      auto h_Q2_bg_2 = d_neg_bg_2.Histo1D({"Q2_bg_2","Q2_bg_2",bins,0,10},"Q2","weight");
      auto h_Q2_bg_3 = d_neg_bg_3.Histo1D({"Q2_bg_3","Q2_bg_3",bins,0,10},"Q2","weight");
      h_Q2_bg_1->Write();
      h_Q2_bg_2->Write();
      h_Q2_bg_3->Write();
      auto h_xbj_bg = d_neg_bg.Histo1D({"xbj_bg","xbj_bg",bins,0,1},"xbj","weight");
      //h_xbj_bg->Rebin(Rebin_n);
      h_xbj_bg->Write();
      auto h_xbj_bg_1 = d_neg_bg_1.Histo1D({"xbj_1","xbj_1",bins,0,1},"xbj","weight");
      auto h_xbj_bg_2 = d_neg_bg_2.Histo1D({"xbj_2","xbj_2",bins,0,1},"xbj","weight");
      auto h_xbj_bg_3 = d_neg_bg_3.Histo1D({"xbj_3","xbj_3",bins,0,1},"xbj","weight");
      h_xbj_bg_1->Write();
      h_xbj_bg_2->Write();
      h_xbj_bg_3->Write();
      auto h_xprime_bg_1 = d_neg_bg_1.Histo1D({"xprime_1","xprime_1",bins,0,1},"xprime","weight");
      auto h_xprime_bg_2 = d_neg_bg_2.Histo1D({"xprime_2","xprime_2",bins,0,1},"xprime","weight");
      auto h_xprime_bg_3 = d_neg_bg_3.Histo1D({"xprime_3","xprime_3",bins,0,1},"xprime","weight");
      h_xprime_bg_1->Write();
      h_xprime_bg_2->Write();
      h_xprime_bg_3->Write();
      auto h_z_bg = d_neg_bg.Histo1D({"z_bg","z_bg",bins,0,1},"z","weight");
      //h_z_bg->Rebin(Rebin_n);
      h_z_bg->Write();
      auto h_z_bg_1 = d_neg_bg_1.Histo1D({"z_1","z_1",bins,0,1},"z","weight");
      auto h_z_bg_2 = d_neg_bg_2.Histo1D({"z_2","z_2",bins,0,1},"z","weight");
      auto h_z_bg_3 = d_neg_bg_3.Histo1D({"z_3","z_3",bins,0,1},"z","weight");
      h_z_bg_1->Write();
      h_z_bg_2->Write();
      h_z_bg_3->Write();
      auto h_zprime_bg_1 = d_neg_bg_1.Histo1D({"zprime_1","zprime_1",bins,0,1},"zprime","weight");
      auto h_zprime_bg_2 = d_neg_bg_2.Histo1D({"zprime_2","zprime_2",bins,0,1},"zprime","weight");
      auto h_zprime_bg_3 = d_neg_bg_3.Histo1D({"zprime_3","zprime_3",bins,0,1},"zprime","weight");
      h_zprime_bg_1->Write();
      h_zprime_bg_2->Write();
      h_zprime_bg_3->Write();
      auto h_x_z_neg = d_neg_pi.Histo2D({"x_z","x_z",bins,0,1,bins,0,1},"z","xbj","weight");
      //h_x_z_neg->RebinX(Rebin_n);
      //h_x_z_neg->RebinY(Rebin_n);
      h_x_z_neg->Write();
      auto h_x_z_bg = d_neg_bg.Histo2D({"x_z_bg","x_z_bg",bins,0,1,bins,0,1},"z","xbj","weight");
      //h_x_z_bg->RebinX(Rebin_n);
      //h_x_z_bg->RebinY(Rebin_n);
      h_x_z_bg->Write();

      auto h_Q2_z_neg = d_neg_pi.Histo2D({"Q2_z","Q2_z",bins,1,10,bins,0,1},"Q2","z");
      //h_Q2_z_neg->RebinX(Rebin_n);
      //h_Q2_z_neg->RebinY(Rebin_n);
      h_Q2_z_neg->Write();
      
      auto get_x_weight_negxz = [&](double x,double z){
      
        int x_bin_number = h_x_z_neg->GetYaxis()->FindBin(x);
        int z_bin_number = h_x_z_neg->GetXaxis()->FindBin(z);
        double x_bincenter = h_x_z_neg->GetYaxis()->GetBinCenter(x_bin_number);
        double all = h_x_z_neg->GetBinContent(z_bin_number,x_bin_number);
        return x/(all*x_bincenter);
        //return x/all;
      };
      auto get_z_weight_negxz = [&](double x,double z){
      
        int x_bin_number = h_x_z_neg->GetYaxis()->FindBin(x);
        int z_bin_number = h_x_z_neg->GetXaxis()->FindBin(z);
        double z_bincenter = h_x_z_neg->GetXaxis()->GetBinCenter(z_bin_number);
        double all = h_x_z_neg->GetBinContent(z_bin_number,x_bin_number);
        return z/(all*z_bincenter);
        //return z/all;
      };
      auto d_neg_pi_after = d_neg_pi
        .Define("weighted_xbj",get_x_weight_negxz,{"xbj","z"})
        .Define("weighted_z",get_z_weight_negxz,{"xbj","z"})
        ;
      auto h_weight_xbj = d_neg_pi_after.Histo2D({"weighted_xbj","weighted_xbj",bins,0,1,bins,0,1},"z","xbj","weighted_xbj");
      auto h_weight_z = d_neg_pi_after.Histo2D({"weighted_z","weighted_z",bins,0,1,bins,0,1},"z","xbj","weighted_z");
      //h_weight_xbj->RebinX(Rebin_n);
      //h_weight_xbj->RebinY(Rebin_n);
      h_weight_xbj->Write();
      //h_weight_z->RebinX(Rebin_n);
      //h_weight_z->RebinY(Rebin_n);
      h_weight_z->Write();
      
      rootfile_out->Close();

    }//neg runs




  }//not empty

    std::string of_name = "results/yield/run_info/"+std::to_string(RunGroup)+".json";
  std::ofstream ofs(of_name.c_str());
  ofs<<jout.dump(4)<<std::endl;
}
