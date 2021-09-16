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

void statistic_runs_Dummy(int RunGroup=0){

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
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>jout;
  }

  std::vector<int> neg_Dummy,pos_Dummy;
  neg_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["Dummy"].get<std::vector<int>>();
  pos_Dummy = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["Dummy"].get<std::vector<int>>();


  if(!neg_Dummy.empty() && !pos_Dummy.empty()){
    //for pos runs
    //loop over each pos runs data
    for(auto it = pos_Dummy.begin();it!=pos_Dummy.end();++it){
      int RunNumber = *it;
      std::cout<<"pos data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_pos_pi("T",rootfile_name);

      int pion_n = *d_pos_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;

      // for bg
      ROOT::RDataFrame d_pos_bg("T_bg",rootfile_name);

      int bg_n = *d_pos_bg.Count();
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;

      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_pos = d_pos_pi.Histo2D({"Q2_x","Q2_x",100,0,1,100,0,10},"xbj","Q2");
      h_Q2_x_pos->Write();
      auto h_xbj = d_pos_pi.Histo1D({"xbj","xbj",100,0,1},"xbj","weight");
      h_xbj->Write();
      auto h_z = d_pos_pi.Histo1D({"z","z",100,0,1},"z","weight");
      h_z->Write();
      auto h_xbj_bg = d_pos_bg.Histo1D({"xbj_bg","xbj_bg",100,0,1},"xbj","weight");
      h_xbj_bg->Write();
      auto h_z_bg = d_pos_bg.Histo1D({"z_bg","z_bg",100,0,1},"z","weight");
      h_z_bg->Write();
      auto h_x_z_pos = d_pos_pi.Histo2D({"x_z","x_z",100,0,1,100,0,1},"z","xbj");
      h_x_z_pos->Write();
      auto h_x_z_bg = d_pos_bg.Histo2D({"x_z_bg","x_z_bg",100,0,1,100,0,1},"z","xbj","weight");
      h_x_z_bg->Write();
      
      auto h_Q2_z_pos = d_pos_pi.Histo2D({"Q2_z","Q2_z",100,1,10,100,0,1},"Q2","z");
      h_Q2_z_pos->Write();
      rootfile_out->Close();
    
    }

    //for neg runs
    //loop over each neg runs data
    for(auto it = neg_Dummy.begin();it!=neg_Dummy.end();++it){
      int RunNumber = *it;
      std::cout<<"neg data"<<RunNumber<<std::endl;
      std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_neg_pi("T",rootfile_name);

      int pion_n = *d_neg_pi.Count();
      jout[(std::to_string(RunNumber)).c_str()]["pion_n"] = pion_n;


      // for bg
      ROOT::RDataFrame d_neg_bg("T_bg",rootfile_name);
      
      int bg_n = *d_neg_bg.Count();
      jout[(std::to_string(RunNumber)).c_str()]["bg_n"] = bg_n;


      std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root";
      TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
      auto h_Q2_x_neg = d_neg_pi.Histo2D({"Q2_x","Q2_x",100,0,1,100,0,10},"xbj","Q2");
      h_Q2_x_neg->Write();
      auto h_xbj = d_neg_pi.Histo1D({"xbj","xbj",100,0,1},"xbj","weight");
      h_xbj->Write();
      auto h_z = d_neg_pi.Histo1D({"z","z",100,0,1},"z","weight");
      h_z->Write();
      auto h_xbj_bg = d_neg_bg.Histo1D({"xbj_bg","xbj_bg",100,0,1},"xbj","weight");
      h_xbj_bg->Write();
      auto h_z_bg = d_neg_bg.Histo1D({"z_bg","z_bg",100,0,1},"z","weight");
      h_z_bg->Write();
      auto h_x_z_neg = d_neg_pi.Histo2D({"x_z","x_z",100,0,1,100,0,1},"z","xbj","weight");
      h_x_z_neg->Write();
      auto h_x_z_bg = d_neg_bg.Histo2D({"x_z_bg","x_z_bg",100,0,1,100,0,1},"z","xbj","weight");
      h_x_z_bg->Write();
      
      auto h_Q2_z_neg = d_neg_pi.Histo2D({"Q2_z","Q2_z",100,1,10,100,0,1},"Q2","z");
      h_Q2_z_neg->Write();
      rootfile_out->Close();
    
    }




  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<jout.dump(4)<<std::endl;
}
