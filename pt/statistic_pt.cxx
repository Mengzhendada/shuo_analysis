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

void statistic_pt(int RunGroup=0){

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


  if(!neg_D2.empty() && !pos_D2.empty()){
    //for pos runs
    //loop over each pos runs data
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_raw("T",rootfile_name);

      auto d_pos_pi = d_pos_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Filter("pt<0.12");

      // for bg
      ROOT::RDataFrame d_pos_bgraw("T_bg",rootfile_name);
      auto d_pos_bg = d_pos_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Filter("pt<0.12");


      auto h_pt_pos = d_pos_pi.Histo1D({"pt","pt",100,0,0.5},"pt","weight");
      auto h_phi_pos = d_pos_pi.Histo1D({"phi","phi",100,-4,4},"P_kin_secondary_ph_xq","weight");
      auto h_pt_pos_bg = d_pos_bg.Histo1D({"pt","pt",100,0,0.5},"pt","weight");
      auto h_phi_pos_bg = d_pos_bg.Histo1D({"phi","phi",100,-4,4},"P_kin_secondary_ph_xq","weight");
   
      h_phi_pos->Add(h_phi_pos_bg.GetPtr(),-1.0/6);
      TCanvas* c_phi = new TCanvas();
      h_phi_pos->DrawCopy("hist");
      std::string c_phi_name = "results/pt/phi_afterpt_"+std::to_string(RunNumber)+".pdf";
      c_phi->SaveAs(c_phi_name.c_str());

    }

    //for neg runs
    //loop over each neg runs data
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";

      ROOT::RDataFrame d_neg_raw("T",rootfile_name);

      auto d_neg_pi = d_neg_raw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Filter("pt<0.12");

      // for bg
      ROOT::RDataFrame d_neg_bgraw("T_bg",rootfile_name);
      auto d_neg_bg = d_neg_bgraw.Define("pt",pt,{"P_gtr_p","P_kin_secondary_th_xq"})
        .Filter("pt<0.12");

      auto h_pt_neg = d_neg_pi.Histo1D({"pt","pt",100,0,0.5},"pt","weight");
      auto h_phi_neg = d_neg_pi.Histo1D({"phi","phi",100,-4,4},"P_kin_secondary_ph_xq","weight");
      auto h_pt_neg_bg = d_neg_bg.Histo1D({"pt","pt",100,0,0.5},"pt","weight");
      auto h_phi_neg_bg = d_neg_bg.Histo1D({"phi","phi",100,-4,4},"P_kin_secondary_ph_xq","weight");
   
      h_phi_neg->Add(h_phi_neg_bg.GetPtr(),-1.0/6);
      TCanvas* c_phi = new TCanvas();
      h_phi_neg->DrawCopy("hist");
      std::string c_phi_name = "results/pt/phi_afterpt_"+std::to_string(RunNumber)+".pdf";
      c_phi->SaveAs(c_phi_name.c_str());
    
    }




  }
}
