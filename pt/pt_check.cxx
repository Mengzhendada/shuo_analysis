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
#include "ROOT/RSnapshotOptions.hxx"
#include "TGraphPolar.h"

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;



void pt_check(int RunGroup=0){

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

  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();

  auto pt = [](double p,double th){return p*std::sin(th);};
  //loop over each pos runs data
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<"pos data"<<RunNumber<<std::endl;
    std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_pos_raw("T",rootfile_name);
    ROOT::RDataFrame d_pos_bgraw("T_bg",rootfile_name);
    auto d_pos_pi = d_pos_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"});
    auto d_pos_bg = d_pos_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"});
    auto h_pt_pos_pi = d_pos_pi.Histo1D({"","",100,0,0.5},"pt");
    TCanvas* c_pt = new TCanvas();
    h_pt_pos_pi->DrawCopy("hist");
    std::string c_pi_name = "results/pt/pt_pos_"+std::to_string(RunNumber)+".pdf";
    c_pt->SaveAs(c_pi_name.c_str());
    auto h_th_xq = d_pos_pi.Histo1D({"","th_xq",100,-0.1,0.1},"P_kin_secondary_th_xq");
    auto h_gtr_th = d_pos_pi.Histo1D({"","gtr_th",100,-0.1,0.1},"P_gtr_th");
    TCanvas* c_th = new TCanvas();
    h_th_xq ->DrawCopy("hist");
    h_gtr_th->SetLineColor(kRed);
    h_gtr_th->DrawCopy("hist same");
    c_th->BuildLegend();
    std::string c_th_name = "results/pt/th_pos_"+std::to_string(RunNumber)+".pdf";
    c_th->SaveAs(c_th_name.c_str());
    auto h_phi_xq = d_pos_pi.Histo1D({"","phi_xq",100,-4,4},"P_kin_secondary_ph_xq");
    TCanvas* c_phi = new TCanvas();
    h_phi_xq->SetLineColor(kRed);
    h_phi_xq->DrawCopy("hist");
    std::string c_phi_name = "results/pt/phi_pos_"+std::to_string(RunNumber)+".pdf";
    c_phi->SaveAs(c_phi_name.c_str());

    //auto pt_sinphi = [&](double pt, double phi){return pt*std::sin(phi);};
    //auto pt_cosphi = [&](double pt, double phi){return pt*std::cos(phi);};
    //auto d_pos_pt = d_pos_pi
    //  .Define("ptsinphi",pt_sinphi,{"pt","P.kin.secondary.ph_xq"})
    //  .Define("ptcosphi",pt_cosphi,{"pt","P.kin.secondary.ph_xq"})
    //  ;
    auto pos_pt_list = d_pos_pi.Take<double>("pt");
    auto pos_phi_list = d_pos_pi.Take<double>("P_kin_secondary_ph_xq");
    int n_pos = (int)pos_pt_list->size();
    std::cout<<"check pt size "<<n_pos<<std::endl;
    double error[n_pos],phi[n_pos],p_t[n_pos];
    for(int i = 0;i<n_pos;i++){
      error[i] = 0;
      phi[i] = pos_phi_list->at(i);
      p_t[i] = pos_pt_list->at(i);
      //std::cout<<"check"<<phi[i]<<" "<<radius[i]<<std::endl;
    } 
    TGraphPolar * pt_phi_polar = new TGraphPolar(n_pos,phi,p_t,error,error);
    TCanvas *c_ptpolar = new TCanvas();
    pt_phi_polar->SetMarkerStyle(8);
    pt_phi_polar->SetMarkerColor(4);
    pt_phi_polar->Draw("P");
    std::string c_ptpolar_name = "results/pt/polar_pos_"+std::to_string(RunNumber)+".pdf";
    c_ptpolar->SaveAs(c_ptpolar_name.c_str());

    auto h_pt_pos = d_pos_pi.Histo1D({"pt","pt",100,0,0.5},"pt");
    auto h_pt_bg_pos = d_pos_bg.Histo1D({"pt_bg","bg",100,0,0.5},"pt");
    std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+"_pt.root";
    TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"UPDATE");
    pt_phi_polar->Write("pt_phi_polar");
    h_pt_pos->Write();
    h_pt_bg_pos->Write();
    rootfile_out->Close();

  }

  //loop over each neg runs data
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<"neg data"<<RunNumber<<std::endl;
    std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_neg_raw("T",rootfile_name);
    ROOT::RDataFrame d_neg_bgraw("T_bg",rootfile_name);
    auto d_neg_pi = d_neg_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"});
    auto d_neg_bg = d_neg_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"});
    auto h_pt_neg_pi = d_neg_pi.Histo1D({"","",100,0,0.5},"pt");
    TCanvas* c_pt = new TCanvas();
    h_pt_neg_pi->DrawCopy("hist");
    std::string c_pi_name = "results/pt/pt_neg_"+std::to_string(RunNumber)+".pdf";
    c_pt->SaveAs(c_pi_name.c_str());
    auto h_th_xq = d_neg_pi.Histo1D({"","th_xq",100,-0.1,0.1},"P_kin_secondary_th_xq");
    auto h_gtr_th = d_neg_pi.Histo1D({"","gtr_th",100,-0.1,0.1},"P_gtr_th");
    TCanvas* c_th = new TCanvas();
    h_th_xq ->DrawCopy("hist");
    h_gtr_th->SetLineColor(kRed);
    h_gtr_th->DrawCopy("hist same");
    c_th->BuildLegend();
    std::string c_th_name = "results/pt/th_neg_"+std::to_string(RunNumber)+".pdf";
    c_th->SaveAs(c_th_name.c_str());
    auto h_phi_xq = d_neg_pi.Histo1D({"","phi_xq",100,-4,4},"P_kin_secondary_ph_xq");
    TCanvas* c_phi = new TCanvas();
    h_phi_xq->SetLineColor(kRed);
    h_phi_xq->DrawCopy("hist");
    std::string c_phi_name = "results/pt/phi_neg_"+std::to_string(RunNumber)+".pdf";
    c_phi->SaveAs(c_phi_name.c_str());

    //auto pt_sinphi = [&](double pt, double phi){return pt*std::sin(phi);};
    //auto pt_cosphi = [&](double pt, double phi){return pt*std::cos(phi);};
    //auto d_neg_pt = d_neg_pi
    //  .Define("ptsinphi",pt_sinphi,{"pt","P.kin.secondary.ph_xq"})
    //  .Define("ptcosphi",pt_cosphi,{"pt","P.kin.secondary.ph_xq"})
    //  ;
    auto neg_pt_list = d_neg_pi.Take<double>("pt");
    auto neg_phi_list = d_neg_pi.Take<double>("P_kin_secondary_ph_xq");
    int n_neg = (int)neg_pt_list->size();
    std::cout<<"check pt size "<<n_neg<<std::endl;
    double error[n_neg],phi[n_neg],p_t[n_neg];
    for(int i = 0;i<n_neg;i++){
      error[i] = 0;
      phi[i] = neg_phi_list->at(i);
      p_t[i] = neg_pt_list->at(i);
      //std::cout<<"check"<<phi[i]<<" "<<radius[i]<<std::endl;
    } 
    TGraphPolar * pt_phi_polar = new TGraphPolar(n_neg,phi,p_t,error,error);
    TCanvas *c_ptpolar = new TCanvas();
    pt_phi_polar->SetMarkerStyle(8);
    pt_phi_polar->SetMarkerColor(4);
    pt_phi_polar->Draw("P");
    std::string c_ptpolar_name = "results/pt/polar_neg_"+std::to_string(RunNumber)+".pdf";
    c_ptpolar->SaveAs(c_ptpolar_name.c_str());
    
    auto h_pt_neg = d_neg_pi.Histo1D({"pt","pt",100,0,0.5},"pt");
    auto h_pt_bg_neg = d_neg_bg.Histo1D({"pt_bg","pt_bg",100,0,0.5},"pt");
    std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+"_pt.root";
    TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"UPDATE");
    pt_phi_polar->Write("pt_phi_polar");
    h_pt_neg->Write();
    h_pt_bg_neg->Write();
    rootfile_out->Close();
  }

}
