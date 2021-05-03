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

void compare_sim_data_new(int RunGroup = 0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit) ";
    std::cin>>RunGroup;
    if(RunGroup < 0)
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
  json j_info;
  {
    std::ifstream ifs("db2/rungroup_info.json");
    ifs>>j_info;
  }
  json j_runs_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runs_info;
  }
  if(!neg_D2.empty() && !pos_D2.empty()){
  std::vector<std::string> files_neg,files_pos;

  TH1D* h_neg_Q2_all = new TH1D("","neg Q2",100,0,10);
  TH1D* h_neg_W_all = new TH1D("","neg W",100,0,4);
  TH1D* h_neg_Wp_all = new TH1D("","neg Wp",100,0,10);
  TH1D* h_neg_xbj_all = new TH1D("","neg xbj",100,0,1);
  TH1D* h_neg_emiss_all = new TH1D("","neg emiss",100,0,4);
  TH1D* h_neg_mmiss_all = new TH1D("","neg mmiss",100,0,2);
  TH1D* h_neg_z_all = new TH1D("","neg z",100,0,1);
  TH1D* h_pos_Q2_all = new TH1D("","pos Q2",100,0,10);
  TH1D* h_pos_W_all = new TH1D("","pos W",100,0,4);
  TH1D* h_pos_Wp_all = new TH1D("","pos Wp",100,0,10);
  TH1D* h_pos_xbj_all = new TH1D("","pos xbj",100,0,1);
  TH1D* h_pos_emiss_all = new TH1D("","pos emiss",100,0,4);
  TH1D* h_pos_pmiss_all = new TH1D("","pos pmiss",100,0,4);
  TH1D* h_pos_mmiss_all = new TH1D("","pos mmiss",100,0,2);
  TH1D* h_pos_z_all = new TH1D("","pos z",100,0,1);
  TH2D* h_pos_Em_Pm_data_all = new TH2D("","",100,0,4,100,0,4);

  TH1D* h_neg_xptar_all = new TH1D("","neg SHMS xptar",100,-0.1,0.1);
  TH1D* h_neg_yptar_all = new TH1D("","neg SHMS yptar",100,-0.1,0.1);
  TH1D* h_neg_dp_all = new TH1D("","neg delta",100,-10,22);
  TH1D* h_neg_ytar_all = new TH1D("","neg ytar",100,-4,4);
  
  TH1D* h_pos_xptar_all = new TH1D("","pos SHMS xptar",100,-0.1,0.1);
  TH1D* h_pos_yptar_all = new TH1D("","pos SHMS yptar",100,-0.1,0.1);
  TH1D* h_pos_dp_all = new TH1D("","pos delta",100,-10,22);
  TH1D* h_pos_ytar_all = new TH1D("","pos ytar",100,-4,4);


  TH1D* sim_neg_Q2_all = new TH1D("","neg Q2",100,0,10);
  TH1D* sim_neg_W_all = new TH1D("","neg W",100,0,4);
  TH1D* sim_neg_Wp_all = new TH1D("","neg Wp",100,0,10);
  TH1D* sim_neg_xbj_all = new TH1D("","neg xbj",100,0,1);
  TH1D* sim_neg_emiss_all = new TH1D("","neg emiss",100,0,4);
  TH1D* sim_neg_mmiss_all = new TH1D("","neg mmiss",100,0,2);
  TH1D* sim_neg_z_all = new TH1D("","neg z",100,0,1);
  TH1D* sim_pos_Q2_all = new TH1D("","pos Q2",100,0,10);
  TH1D* sim_pos_W_all = new TH1D("","pos W",100,0,4);
  TH1D* sim_pos_Wp_all = new TH1D("","pos Wp",100,0,10);
  TH1D* sim_pos_xbj_all = new TH1D("","pos xbj",100,0,1);
  TH1D* sim_pos_emiss_all = new TH1D("","pos emiss",100,0,4);
  TH1D* sim_pos_mmiss_all = new TH1D("","pos mmiss",100,0,2);
  TH1D* sim_pos_z_all = new TH1D("","pos z",100,0,1);
  
  TH1D* sim_neg_xptar_all = new TH1D("","neg SHMS xptar",100,-0.1,0.1);
  TH1D* sim_neg_yptar_all = new TH1D("","neg SHMS yptar",100,-0.1,0.1);
  TH1D* sim_neg_dp_all = new TH1D("","neg delta",100,-10,22);
  TH1D* sim_neg_ytar_all = new TH1D("","neg ytar",100,-4,4);
  
  TH1D* sim_pos_xptar_all = new TH1D("","pos SHMS xptar",100,-0.1,0.1);
  TH1D* sim_pos_yptar_all = new TH1D("","pos SHMS yptar",100,-0.1,0.1);
  TH1D* sim_pos_dp_all = new TH1D("","pos delta",100,-10,22);
  TH1D* sim_pos_ytar_all = new TH1D("","pos ytar",100,-4,4);
  
  std::string rootfile_sim = "results/skim_root/"+std::to_string(RunGroup)+"_sim.root";

  //for neg runs
  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_neg_pi("T",rootfile_name);
    ROOT::RDataFrame d_neg_bg("T_bg",rootfile_name);
  double d_charge_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
  double d_shmsTE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
  double d_hmsTE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
  double d_hmscalDE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
  double d_hmscerDE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
  double d_shmscalDE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
  double d_shmsaeroDE_neg = j_runs_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
  double d_eff_neg = d_shmsTE_neg*d_hmscalDE_neg*d_hmscerDE_neg*d_hmsTE_neg*d_shmscalDE_neg*d_shmsaeroDE_neg; 
    auto h_neg_Q2_pi = d_neg_pi.Histo1D({"","",100,0,10},"Q2");
    auto h_neg_xbj_pi = d_neg_pi.Histo1D({"","",100,0,1},"xbj");
    auto h_neg_W_pi = d_neg_pi.Histo1D({"","",100,0,4},"W");
    auto h_neg_Wp_pi = d_neg_pi.Histo1D({"","",100,0,10},"Wp");
    auto h_neg_z_pi = d_neg_pi.Histo1D({"","",100,0,1},"z");
    auto h_neg_emiss_pi = d_neg_pi.Histo1D({"","",100,0,4},"emiss");
    auto h_neg_mmiss_pi = d_neg_pi.Histo1D({"","",100,0,2},"mmiss");
    auto h_neg_xptar = d_neg_pi.Histo1D({"","",100,-0.1,0.1},"P_gtr_th");
    auto h_neg_yptar = d_neg_pi.Histo1D({"","",100,-0.1,0.1},"P_gtr_ph");
    auto h_neg_ytar = d_neg_pi.Histo1D({"","",100,-4,4},"P_gtr_y");
    auto h_neg_dp = d_neg_pi.Histo1D({"","",100,-10,22},"P_gtr_dp");
    auto h_neg_Q2_bg = d_neg_bg.Histo1D({"","",100,0,10},"Q2");
    auto h_neg_xbj_bg = d_neg_bg.Histo1D({"","",100,0,1},"xbj");
    auto h_neg_W_bg = d_neg_bg.Histo1D({"","",100,0,4},"W");
    auto h_neg_Wp_bg = d_neg_bg.Histo1D({"","",100,0,10},"Wp");
    auto h_neg_z_bg = d_neg_bg.Histo1D({"","",100,0,1},"z");
    auto h_neg_emiss_bg = d_neg_bg.Histo1D({"","",100,0,4},"emiss");
    auto h_neg_mmiss_bg = d_neg_bg.Histo1D({"","",100,0,2},"mmiss");
    auto h_neg_xptar_bg = d_neg_bg.Histo1D({"","",100,-0.1,0.1},"P_gtr_th");
    auto h_neg_yptar_bg = d_neg_bg.Histo1D({"","",100,-0.1,0.1},"P_gtr_ph");
    auto h_neg_ytar_bg = d_neg_bg.Histo1D({"","",100,-4,4},"P_gtr_y");
    auto h_neg_dp_bg = d_neg_bg.Histo1D({"","",100,-10,22},"P_gtr_dp");
    h_neg_Q2_all->Add(h_neg_Q2_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_xbj_all->Add(h_neg_xbj_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_W_all->Add(h_neg_W_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_z_all->Add(h_neg_z_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_emiss_all->Add(h_neg_emiss_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_mmiss_all->Add(h_neg_mmiss_pi.GetPtr(),1/(1*d_eff_neg));
    h_neg_xptar_all->Add(h_neg_xptar.GetPtr(),1/(1*d_eff_neg));
    h_neg_yptar_all->Add(h_neg_yptar.GetPtr(),1/(1*d_eff_neg));
    h_neg_ytar_all->Add(h_neg_ytar.GetPtr(),1/(1*d_eff_neg));
    h_neg_dp_all->Add(h_neg_dp.GetPtr(),1/(1*d_eff_neg));
    h_neg_Q2_all->Add(h_neg_Q2_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_xbj_all->Add(h_neg_xbj_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_W_all->Add(h_neg_W_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_z_all->Add(h_neg_z_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_emiss_all->Add(h_neg_emiss_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_mmiss_all->Add(h_neg_mmiss_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_xptar_all->Add(h_neg_xptar_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_yptar_all->Add(h_neg_yptar_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_ytar_all->Add(h_neg_ytar_bg.GetPtr(),-1/(6*d_eff_neg));
    h_neg_dp_all->Add(h_neg_dp_bg.GetPtr(),-1/(6*d_eff_neg));
    //h_neg_Q2_all->Add(h_neg_Q2_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_xbj_all->Add(h_neg_xbj_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_W_all->Add(h_neg_W_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_Wp_all->Add(h_neg_Wp_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_z_all->Add(h_neg_z_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_emiss_all->Add(h_neg_emiss_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_mmiss_all->Add(h_neg_mmiss_pi.GetPtr(),1/(d_charge_neg*d_eff_neg));
    //h_neg_Q2_all->Add(h_neg_Q2_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_xbj_all->Add(h_neg_xbj_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_W_all->Add(h_neg_W_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_Wp_all->Add(h_neg_Wp_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_z_all->Add(h_neg_z_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_emiss_all->Add(h_neg_emiss_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
    //h_neg_mmiss_all->Add(h_neg_mmiss_bg.GetPtr(),-1/(6*d_charge_neg*d_eff_neg));
  }
  //for pos runs
  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
    int RunNumber = *it;
    std::cout<<RunNumber<<std::endl;
    std::string rootfile_name = "results/skim_root/"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_pos_pi("T",rootfile_name);
    ROOT::RDataFrame d_pos_bg("T_bg",rootfile_name);
  double d_charge_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
  double d_shmsTE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
  double d_hmsTE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["TEHMS"].get<double>();
  double d_hmscalDE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["HMS_cal_eff"].get<double>();
  double d_hmscerDE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["HMS_cer_eff"].get<double>();
  double d_shmscalDE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["SHMS_cal_eff"].get<double>();
  double d_shmsaeroDE_pos = j_runs_info[(std::to_string(RunNumber)).c_str()]["SHMS_aero_eff"].get<double>();
  double d_eff_pos = d_shmsTE_pos*d_hmscalDE_pos*d_hmscerDE_pos*d_hmsTE_pos*d_shmscalDE_pos*d_shmsaeroDE_pos; 
    auto h_pos_Q2_pi = d_pos_pi.Histo1D({"","",100,0,10},"Q2");
    auto h_pos_xbj_pi = d_pos_pi.Histo1D({"","",100,0,1},"xbj");
    auto h_pos_W_pi = d_pos_pi.Histo1D({"","",100,0,4},"W");
    auto h_pos_Wp_pi = d_pos_pi.Histo1D({"","",100,0,10},"Wp");
    auto h_pos_z_pi = d_pos_pi.Histo1D({"","",100,0,1},"z");
    auto h_pos_emiss_pi = d_pos_pi.Histo1D({"","",100,0,4},"emiss");
    auto h_pos_mmiss_pi = d_pos_pi.Histo1D({"","",100,0,2},"mmiss");
    auto h_pos_xptar = d_pos_pi.Histo1D({"","",100,-0.1,0.1},"P_gtr_th");
    auto h_pos_yptar = d_pos_pi.Histo1D({"","",100,-0.1,0.1},"P_gtr_ph");
    auto h_pos_ytar = d_pos_pi.Histo1D({"","",100,-4,4},"P_gtr_y");
    auto h_pos_dp = d_pos_pi.Histo1D({"","",100,-10,22},"P_gtr_dp");
    auto h_pos_Q2_bg = d_pos_bg.Histo1D({"","",100,0,10},"Q2");
    auto h_pos_xbj_bg = d_pos_bg.Histo1D({"","",100,0,1},"xbj");
    auto h_pos_W_bg = d_pos_bg.Histo1D({"","",100,0,4},"W");
    auto h_pos_Wp_bg = d_pos_bg.Histo1D({"","",100,0,10},"Wp");
    auto h_pos_z_bg = d_pos_bg.Histo1D({"","",100,0,1},"z");
    auto h_pos_emiss_bg = d_pos_bg.Histo1D({"","",100,0,4},"emiss");
    auto h_pos_mmiss_bg = d_pos_bg.Histo1D({"","",100,0,2},"mmiss");
    auto h_pos_xptar_bg = d_pos_bg.Histo1D({"","",100,-0.1,0.1},"P_gtr_th");
    auto h_pos_yptar_bg = d_pos_bg.Histo1D({"","",100,-0.1,0.1},"P_gtr_ph");
    auto h_pos_ytar_bg = d_pos_bg.Histo1D({"","",100,-4,4},"P_gtr_y");
    auto h_pos_dp_bg = d_pos_bg.Histo1D({"","",100,-10,22},"P_gtr_dp");
    h_pos_Q2_all->Add(h_pos_Q2_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_xbj_all->Add(h_pos_xbj_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_W_all->Add(h_pos_W_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_z_all->Add(h_pos_z_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_emiss_all->Add(h_pos_emiss_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_mmiss_all->Add(h_pos_mmiss_pi.GetPtr(),1/(1*d_eff_pos));
    h_pos_xptar_all->Add(h_pos_xptar.GetPtr(),1/(1*d_eff_pos));
    h_pos_yptar_all->Add(h_pos_yptar.GetPtr(),1/(1*d_eff_pos));
    h_pos_ytar_all->Add(h_pos_ytar.GetPtr(),1/(1*d_eff_pos));
    h_pos_dp_all->Add(h_pos_dp.GetPtr(),1/(1*d_eff_pos));
    h_pos_Q2_all->Add(h_pos_Q2_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_xbj_all->Add(h_pos_xbj_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_W_all->Add(h_pos_W_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_z_all->Add(h_pos_z_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_emiss_all->Add(h_pos_emiss_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_mmiss_all->Add(h_pos_mmiss_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_xptar_all->Add(h_pos_xptar_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_yptar_all->Add(h_pos_yptar_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_ytar_all->Add(h_pos_ytar_bg.GetPtr(),-1/(6*d_eff_pos));
    h_pos_dp_all->Add(h_pos_dp_bg.GetPtr(),-1/(6*d_eff_pos));
    //h_pos_Q2_all->Add(h_pos_Q2_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_xbj_all->Add(h_pos_xbj_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_W_all->Add(h_pos_W_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_Wp_all->Add(h_pos_Wp_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_z_all->Add(h_pos_z_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_emiss_all->Add(h_pos_emiss_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_mmiss_all->Add(h_pos_mmiss_pi.GetPtr(),1/(d_charge_pos*d_eff_pos));
    //h_pos_Q2_all->Add(h_pos_Q2_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_xbj_all->Add(h_pos_xbj_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_W_all->Add(h_pos_W_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_Wp_all->Add(h_pos_Wp_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_z_all->Add(h_pos_z_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_emiss_all->Add(h_pos_emiss_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
    //h_pos_mmiss_all->Add(h_pos_mmiss_bg.GetPtr(),-1/(6*d_charge_pos*d_eff_pos));
  }
  double dcharge_neg = j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["charge"].get<double>();
  double dcharge_pos = j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["charge"].get<double>();
    h_neg_Q2_all   ->Scale(1.0/dcharge_neg); 
    h_neg_W_all   ->Scale(1.0/dcharge_neg); 
    h_neg_Wp_all  ->Scale(1.0/dcharge_neg);  
    h_neg_xbj_all ->Scale(1.0/dcharge_neg);  
    h_neg_emiss_all ->Scale(1.0/dcharge_neg); 
    h_neg_mmiss_all ->Scale(1.0/dcharge_neg); 
    h_neg_z_all ->Scale(1.0/dcharge_neg); 
    h_pos_Q2_all  ->Scale(1.0/dcharge_pos); 
    h_pos_W_all ->Scale(1.0/dcharge_pos); 
    h_pos_Wp_all ->Scale(1.0/dcharge_pos);  
    h_pos_xbj_all ->Scale(1.0/dcharge_pos); 
    h_pos_emiss_all->Scale(1.0/dcharge_pos); 
    h_pos_pmiss_all->Scale(1.0/dcharge_pos); 
    h_pos_mmiss_all->Scale(1.0/dcharge_pos); 
    h_pos_z_all ->Scale(1.0/dcharge_pos); 

    h_neg_xptar_all ->Scale(1.0/dcharge_neg); 
    h_neg_yptar_all ->Scale(1.0/dcharge_neg); 
    h_neg_dp_all   ->Scale(1.0/dcharge_neg); 
    h_neg_ytar_all  ->Scale(1.0/dcharge_neg); 

    h_pos_xptar_all->Scale(1.0/dcharge_pos); 
    h_pos_yptar_all->Scale(1.0/dcharge_pos); 
    h_pos_dp_all->Scale(1.0/dcharge_pos);   
    h_pos_ytar_all->Scale(1.0/dcharge_pos);  
  
  
  
  
  
  ROOT::RDataFrame d_sim_pos_inc_norad("T_pos_inc_norad",rootfile_sim);
  ROOT::RDataFrame d_sim_pos_exc_rad("T_pos_exc_rad",rootfile_sim);
  ROOT::RDataFrame d_sim_pos_inc_rad("T_pos_inc_rad",rootfile_sim);
  ROOT::RDataFrame d_sim_pos_rho("T_pos_rho",rootfile_sim);
  ROOT::RDataFrame d_sim_pos_delat("T_pos_delta",rootfile_sim);
  ROOT::RDataFrame d_sim_neg_inc_norad("T_neg_inc_norad",rootfile_sim);
  ROOT::RDataFrame d_sim_neg_exc_rad("T_neg_exc_rad",rootfile_sim);
  ROOT::RDataFrame d_sim_neg_inc_rad("T_neg_inc_rad",rootfile_sim);
  ROOT::RDataFrame d_sim_neg_rho("T_neg_rho",rootfile_sim);
  ROOT::RDataFrame d_sim_neg_delat("T_neg_delta",rootfile_sim);
  
  //for missing mass
  //h_pos_mmiss_all
  auto mmiss_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"mmiss_neg_sim","inc norad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"mmiss_neg_sim","exc rad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"mmiss_neg_sim","inc rad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_neg_rho = d_sim_neg_rho.Histo1D({"mmiss_neg_sim","rho", 100,0,2},"missmass","weight_new");
  sim_neg_mmiss_all->Add(mmiss_sim_neg_inc_norad.GetPtr()); 
  sim_neg_mmiss_all->Add(mmiss_sim_neg_inc_rad.GetPtr()); 
  sim_neg_mmiss_all->Add(mmiss_sim_neg_exc_rad.GetPtr()); 
  auto mmiss_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"mmiss_pos_sim","inc norad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"mmiss_pos_sim","exc rad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"mmiss_pos_sim","inc rad", 100,0,2},"missmass","weight_new");
  auto mmiss_sim_pos_rho = d_sim_pos_rho.Histo1D({"mmiss_pos_sim","rho", 100,0,2},"missmass","weight_new");
  sim_pos_mmiss_all->Add(mmiss_sim_pos_inc_norad.GetPtr()); 
  sim_pos_mmiss_all->Add(mmiss_sim_pos_inc_rad.GetPtr()); 
  sim_pos_mmiss_all->Add(mmiss_sim_pos_exc_rad.GetPtr()); 
  
  TCanvas *c_missmass = new TCanvas();
  c_missmass->Divide(2,1);
  c_missmass->cd(1);
  //sim_pos_mmiss_all->SetMarkerStyle(8);
  //sim_pos_mmiss_all->SetMarkerColor(9);
  //sim_pos_mmiss_all->DrawCopy();
  h_pos_mmiss_all->SetBit(TH1::kNoStats);
  h_pos_mmiss_all->DrawCopy("hist same");
  mmiss_sim_pos_inc_norad->SetMarkerStyle(8);
  mmiss_sim_pos_inc_norad->SetMarkerColor(1);
  mmiss_sim_pos_inc_norad->DrawCopy();
  //mmiss_sim_pos_inc_norad->DrawCopy("same");
  mmiss_sim_pos_inc_rad->SetMarkerStyle(8);
  mmiss_sim_pos_inc_rad->SetMarkerColor(2);
  mmiss_sim_pos_inc_rad->DrawCopy("same");
  mmiss_sim_pos_exc_rad->SetMarkerStyle(8);
  mmiss_sim_pos_exc_rad->SetMarkerColor(4);
  mmiss_sim_pos_exc_rad->DrawCopy("same");
  mmiss_sim_pos_rho->SetMarkerStyle(8);
  mmiss_sim_pos_rho->SetMarkerColor(6);
  mmiss_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_missmass->cd(2);
  h_neg_mmiss_all->SetBit(TH1::kNoStats);
  h_neg_mmiss_all->DrawCopy("hist");
  mmiss_sim_neg_inc_norad->SetMarkerStyle(8);
  mmiss_sim_neg_inc_norad->SetMarkerColor(1);
  mmiss_sim_neg_inc_norad->DrawCopy();
  //mmiss_sim_neg_inc_norad->DrawCopy("same");
  mmiss_sim_neg_inc_rad->SetMarkerStyle(8);
  mmiss_sim_neg_inc_rad->SetMarkerColor(2);
  mmiss_sim_neg_inc_rad->DrawCopy("same");
  mmiss_sim_neg_exc_rad->SetMarkerStyle(8);
  mmiss_sim_neg_exc_rad->SetMarkerColor(4);
  mmiss_sim_neg_exc_rad->DrawCopy("same");
  mmiss_sim_neg_rho->SetMarkerStyle(8);
  mmiss_sim_neg_rho->SetMarkerColor(6);
  mmiss_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_missmass_name = "results/simc/missmass_"+std::to_string(RunGroup)+".pdf";
  c_missmass->SaveAs(c_missmass_name.c_str());
  std::cout<<"Done with miss mass"<<std::endl;
  
   //for Q2
  //h_pos_Q2_all
  auto Q2_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"Q2_neg_sim","inc norad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"Q2_neg_sim","exc rad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"Q2_neg_sim","inc rad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_neg_rho = d_sim_neg_rho.Histo1D({"Q2_neg_sim","rho", 100,0,10},"Q2","weight_new");
  auto Q2_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"Q2_pos_sim","inc norad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"Q2_pos_sim","exc rad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"Q2_pos_sim","inc rad", 100,0,10},"Q2","weight_new");
  auto Q2_sim_pos_rho = d_sim_pos_rho.Histo1D({"Q2_pos_sim","rho", 100,0,10},"Q2","weight_new");
  std::cout<<"neg rho "<<Q2_sim_neg_rho->GetEntries()<<" pos rho"<<Q2_sim_pos_rho->GetEntries()<<std::endl;
  std::cout<<"neg rho "<<Q2_sim_neg_rho->Integral(1,100)<<" pos rho"<<Q2_sim_pos_rho->Integral(1,100)<<std::endl;
  TCanvas *c_Q2 = new TCanvas();
  c_Q2->Divide(2,1);
  c_Q2->cd(1);
  h_pos_Q2_all->SetBit(TH1::kNoStats);
  h_pos_Q2_all->DrawCopy("hist");
  Q2_sim_pos_inc_norad->SetMarkerStyle(8);
  Q2_sim_pos_inc_norad->SetMarkerColor(1);
  Q2_sim_pos_inc_norad->DrawCopy("same");
  Q2_sim_pos_inc_rad->SetMarkerStyle(8);
  Q2_sim_pos_inc_rad->SetMarkerColor(2);
  Q2_sim_pos_inc_rad->DrawCopy("same");
  Q2_sim_pos_exc_rad->SetMarkerStyle(8);
  Q2_sim_pos_exc_rad->SetMarkerColor(4);
  Q2_sim_pos_exc_rad->DrawCopy("same");
  Q2_sim_pos_rho->SetMarkerStyle(8);
  Q2_sim_pos_rho->SetMarkerColor(6);
  Q2_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_Q2->cd(2);
  h_neg_Q2_all->SetBit(TH1::kNoStats);
  h_neg_Q2_all->DrawCopy("hist");
  Q2_sim_neg_inc_norad->SetMarkerStyle(8);
  Q2_sim_neg_inc_norad->SetMarkerColor(1);
  Q2_sim_neg_inc_norad->DrawCopy("same");
  Q2_sim_neg_inc_rad->SetMarkerStyle(8);
  Q2_sim_neg_inc_rad->SetMarkerColor(2);
  Q2_sim_neg_inc_rad->DrawCopy("same");
  Q2_sim_neg_exc_rad->SetMarkerStyle(8);
  Q2_sim_neg_exc_rad->SetMarkerColor(4);
  Q2_sim_neg_exc_rad->DrawCopy("same");
  Q2_sim_neg_rho->SetMarkerStyle(8);
  Q2_sim_neg_rho->SetMarkerColor(6);
  Q2_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_Q2_name = "results/simc/Q2_"+std::to_string(RunGroup)+".pdf";
  c_Q2->SaveAs(c_Q2_name.c_str());
  
  std::cout<<"xbj check"<<std::endl;
  auto xbj_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"xbj_neg_sim","inc norad", 100,0,1},"xbj","weight_new");
  std::cout<<"xbj check"<<std::endl;
  auto xbj_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"xbj_neg_sim","exc rad", 100,0,1},"xbj","weight_new");
  std::cout<<"xbj check"<<std::endl;
  auto xbj_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"xbj_neg_sim","inc rad", 100,0,1},"xbj","weight_new");
  auto xbj_sim_neg_rho = d_sim_neg_rho.Histo1D({"xbj_neg_sim","rho", 100,0,1},"xbj","weight_new");
  auto xbj_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"xbj_pos_sim","inc norad", 100,0,1},"xbj","weight_new");
  auto xbj_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"xbj_pos_sim","exc rad", 100,0,1},"xbj","weight_new");
  auto xbj_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"xbj_pos_sim","inc rad", 100,0,1},"xbj","weight_new");
  auto xbj_sim_pos_rho = d_sim_pos_rho.Histo1D({"xbj_pos_sim","rho", 100,0,1},"xbj","weight_new");
  TCanvas *c_xbj = new TCanvas();
  c_xbj->Divide(2,1);
  c_xbj->cd(1);
  h_pos_xbj_all->SetBit(TH1::kNoStats);
  h_pos_xbj_all->DrawCopy("hist");
  xbj_sim_pos_inc_norad->SetMarkerStyle(8);
  xbj_sim_pos_inc_norad->SetMarkerColor(1);
  xbj_sim_pos_inc_norad->DrawCopy("same");
  xbj_sim_pos_inc_rad->SetMarkerStyle(8);
  xbj_sim_pos_inc_rad->SetMarkerColor(2);
  xbj_sim_pos_inc_rad->DrawCopy("same");
  xbj_sim_pos_exc_rad->SetMarkerStyle(8);
  xbj_sim_pos_exc_rad->SetMarkerColor(kGreen);
  xbj_sim_pos_exc_rad->DrawCopy("same");
  xbj_sim_pos_rho->SetMarkerStyle(8);
  xbj_sim_pos_rho->SetMarkerColor(6);
  xbj_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_xbj->cd(2);
  h_neg_xbj_all->SetBit(TH1::kNoStats);
  h_neg_xbj_all->DrawCopy("hist");
  xbj_sim_neg_inc_norad->SetMarkerStyle(8);
  xbj_sim_neg_inc_norad->SetMarkerColor(1);
  xbj_sim_neg_inc_norad->DrawCopy("same");
  xbj_sim_neg_inc_rad->SetMarkerStyle(8);
  xbj_sim_neg_inc_rad->SetMarkerColor(2);
  xbj_sim_neg_inc_rad->DrawCopy("same");
  xbj_sim_neg_exc_rad->SetMarkerStyle(8);
  xbj_sim_neg_exc_rad->SetMarkerColor(4);
  xbj_sim_neg_exc_rad->DrawCopy("same");
  xbj_sim_neg_rho->SetMarkerStyle(8);
  xbj_sim_neg_rho->SetMarkerColor(6);
  xbj_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_xbj_name = "results/simc/xbj_"+std::to_string(RunGroup)+".pdf";
  c_xbj->SaveAs(c_xbj_name.c_str());
  
  auto W_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"W_neg_sim","inc norad", 100,0,4},"W","weight_new");
  auto W_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"W_neg_sim","exc rad", 100,0,4},"W","weight_new");
  auto W_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"W_neg_sim","inc rad", 100,0,4},"W","weight_new");
  auto W_sim_neg_rho = d_sim_neg_rho.Histo1D({"W_neg_sim","rho", 100,0,4},"W","weight_new");
  auto W_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"W_pos_sim","inc norad", 100,0,4},"W","weight_new");
  auto W_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"W_pos_sim","exc rad", 100,0,4},"W","weight_new");
  auto W_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"W_pos_sim","inc rad", 100,0,4},"W","weight_new");
  auto W_sim_pos_rho = d_sim_pos_rho.Histo1D({"W_pos_sim","rho", 100,0,4},"W","weight_new");
  TCanvas *c_W = new TCanvas();
  c_W->Divide(2,1);
  c_W->cd(1);
  h_pos_W_all->SetBit(TH1::kNoStats);
  h_pos_W_all->DrawCopy("hist");
  W_sim_pos_inc_norad->SetMarkerStyle(8);
  W_sim_pos_inc_norad->SetMarkerColor(1);
  W_sim_pos_inc_norad->DrawCopy("same");
  W_sim_pos_inc_rad->SetMarkerStyle(8);
  W_sim_pos_inc_rad->SetMarkerColor(2);
  W_sim_pos_inc_rad->DrawCopy("same");
  W_sim_pos_exc_rad->SetMarkerStyle(8);
  W_sim_pos_exc_rad->SetMarkerColor(4);
  W_sim_pos_exc_rad->DrawCopy("same");
  W_sim_pos_rho->SetMarkerStyle(8);
  W_sim_pos_rho->SetMarkerColor(6);
  W_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_W->cd(2);
  h_neg_W_all->SetBit(TH1::kNoStats);
  h_neg_W_all->DrawCopy("hist");
  W_sim_neg_inc_norad->SetMarkerStyle(8);
  W_sim_neg_inc_norad->SetMarkerColor(1);
  W_sim_neg_inc_norad->DrawCopy("same");
  W_sim_neg_inc_rad->SetMarkerStyle(8);
  W_sim_neg_inc_rad->SetMarkerColor(2);
  W_sim_neg_inc_rad->DrawCopy("same");
  W_sim_neg_exc_rad->SetMarkerStyle(8);
  W_sim_neg_exc_rad->SetMarkerColor(4);
  W_sim_neg_exc_rad->DrawCopy("same");
  W_sim_neg_rho->SetMarkerStyle(8);
  W_sim_neg_rho->SetMarkerColor(6);
  W_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_W_name = "results/simc/W_"+std::to_string(RunGroup)+".pdf";
  c_W->SaveAs(c_W_name.c_str());
  
 // auto Wp_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"Wp_neg_sim","inc norad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"Wp_neg_sim","exc rad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"Wp_neg_sim","inc rad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_neg_rho = d_sim_neg_rho.Histo1D({"Wp_neg_sim","rho", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"Wp_pos_sim","inc norad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"Wp_pos_sim","exc rad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"Wp_pos_sim","inc rad", 100,0,4},"Wp","weight_new");
 // auto Wp_sim_pos_rho = d_sim_pos_rho.Histo1D({"Wp_pos_sim","rho", 100,0,4},"Wp","weight_new");
  TCanvas *c_Wp = new TCanvas();
  c_Wp->Divide(2,1);
  c_Wp->cd(1);
  h_pos_Wp_all->SetBit(TH1::kNoStats);
  h_pos_Wp_all->DrawCopy("hist");
 // Wp_sim_pos_inc_norad->SetMarkerStyle(8);
 // Wp_sim_pos_inc_norad->SetMarkerColor(1);
 // Wp_sim_pos_inc_norad->DrawCopy("same");
 // Wp_sim_pos_inc_rad->SetMarkerStyle(8);
 // Wp_sim_pos_inc_rad->SetMarkerColor(2);
 // Wp_sim_pos_inc_rad->DrawCopy("same");
 // Wp_sim_pos_exc_rad->SetMarkerStyle(8);
 // Wp_sim_pos_exc_rad->SetMarkerColor(4);
 // Wp_sim_pos_exc_rad->DrawCopy("same");
 // Wp_sim_pos_rho->SetMarkerStyle(8);
 // Wp_sim_pos_rho->SetMarkerColor(6);
 // Wp_sim_pos_rho->DrawCopy("same");
 // gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_Wp->cd(2);
  h_neg_Wp_all->SetBit(TH1::kNoStats);
  h_neg_Wp_all->DrawCopy("hist");
 // Wp_sim_neg_inc_norad->SetMarkerStyle(8);
 // Wp_sim_neg_inc_norad->SetMarkerColor(1);
 // Wp_sim_neg_inc_norad->DrawCopy("same");
 // Wp_sim_neg_inc_rad->SetMarkerStyle(8);
 // Wp_sim_neg_inc_rad->SetMarkerColor(2);
 // Wp_sim_neg_inc_rad->DrawCopy("same");
 // Wp_sim_neg_exc_rad->SetMarkerStyle(8);
 // Wp_sim_neg_exc_rad->SetMarkerColor(4);
 // Wp_sim_neg_exc_rad->DrawCopy("same");
 // Wp_sim_neg_rho->SetMarkerStyle(8);
 // Wp_sim_neg_rho->SetMarkerColor(6);
 // Wp_sim_neg_rho->DrawCopy("same");
 // gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_Wp_name = "results/simc/Wp_"+std::to_string(RunGroup)+".pdf";
  c_Wp->SaveAs(c_Wp_name.c_str());

  auto emiss_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"emiss_neg_sim","inc norad", 100,0,4},"Em","weight_new");
  auto emiss_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"emiss_neg_sim","exc rad", 100,0,4},"Em","weight_new");
  auto emiss_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"emiss_neg_sim","inc rad", 100,0,4},"Em","weight_new");
  auto emiss_sim_neg_rho = d_sim_neg_rho.Histo1D({"emiss_neg_sim","rho", 100,0,4},"Em","weight_new");
  auto emiss_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"emiss_pos_sim","inc norad", 100,0,4},"Em","weight_new");
  auto emiss_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"emiss_pos_sim","exc rad", 100,0,4},"Em","weight_new");
  auto emiss_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"emiss_pos_sim","inc rad", 100,0,4},"Em","weight_new");
  auto emiss_sim_pos_rho = d_sim_pos_rho.Histo1D({"emiss_pos_sim","rho", 100,0,4},"Em","weight_new");
  TCanvas *c_emiss = new TCanvas();
  c_emiss->Divide(2,1);
  c_emiss->cd(1);
  h_pos_emiss_all->SetBit(TH1::kNoStats);
  h_pos_emiss_all->DrawCopy("hist");
  emiss_sim_pos_inc_norad->SetMarkerStyle(8);
  emiss_sim_pos_inc_norad->SetMarkerColor(1);
  emiss_sim_pos_inc_norad->DrawCopy("same");
  emiss_sim_pos_inc_rad->SetMarkerStyle(8);
  emiss_sim_pos_inc_rad->SetMarkerColor(2);
  emiss_sim_pos_inc_rad->DrawCopy("same");
  emiss_sim_pos_exc_rad->SetMarkerStyle(8);
  emiss_sim_pos_exc_rad->SetMarkerColor(4);
  emiss_sim_pos_exc_rad->DrawCopy("same");
  emiss_sim_pos_rho->SetMarkerStyle(8);
  emiss_sim_pos_rho->SetMarkerColor(6);
  emiss_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_emiss->cd(2);
  h_neg_emiss_all->SetBit(TH1::kNoStats);
  h_neg_emiss_all->DrawCopy("hist");
  emiss_sim_neg_inc_norad->SetMarkerStyle(8);
  emiss_sim_neg_inc_norad->SetMarkerColor(1);
  emiss_sim_neg_inc_norad->DrawCopy("same");
  emiss_sim_neg_inc_rad->SetMarkerStyle(8);
  emiss_sim_neg_inc_rad->SetMarkerColor(2);
  emiss_sim_neg_inc_rad->DrawCopy("same");
  emiss_sim_neg_exc_rad->SetMarkerStyle(8);
  emiss_sim_neg_exc_rad->SetMarkerColor(4);
  emiss_sim_neg_exc_rad->DrawCopy("same");
  emiss_sim_neg_rho->SetMarkerStyle(8);
  emiss_sim_neg_rho->SetMarkerColor(6);
  emiss_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  std::string c_emiss_name = "results/simc/emiss_"+std::to_string(RunGroup)+".pdf";
  c_emiss->SaveAs(c_emiss_name.c_str());

  auto z_sim_neg_inc_norad = d_sim_neg_inc_norad.Histo1D({"z_neg_sim","inc norad", 100,0,1},"z","weight_new");
  auto z_sim_neg_exc_rad = d_sim_neg_exc_rad.Histo1D({"z_neg_sim","exc rad", 100,0,1},"z","weight_new");
  auto z_sim_neg_inc_rad = d_sim_neg_inc_rad.Histo1D({"z_neg_sim","inc rad", 100,0,1},"z","weight_new");
  auto z_sim_neg_rho = d_sim_neg_rho.Histo1D({"z_neg_sim","rho", 100,0,1},"z","weight_new");
  auto z_sim_pos_inc_norad = d_sim_pos_inc_norad.Histo1D({"z_pos_sim","inc norad", 100,0,1},"z","weight_new");
  auto z_sim_pos_exc_rad = d_sim_pos_exc_rad.Histo1D({"z_pos_sim","exc rad", 100,0,1},"z","weight_new");
  auto z_sim_pos_inc_rad = d_sim_pos_inc_rad.Histo1D({"z_pos_sim","inc rad", 100,0,1},"z","weight_new");
  auto z_sim_pos_rho = d_sim_pos_rho.Histo1D({"z_pos_sim","rho", 100,0,1},"z","weight_new");
  TCanvas *c_z = new TCanvas();
  c_z->Divide(2,1);
  c_z->cd(1);
  h_pos_z_all->SetBit(TH1::kNoStats);
  h_pos_z_all->DrawCopy("hist");
  z_sim_pos_inc_norad->SetMarkerStyle(8);
  z_sim_pos_inc_norad->SetMarkerColor(1);
  z_sim_pos_inc_norad->DrawCopy("same");
  z_sim_pos_inc_rad->SetMarkerStyle(8);
  z_sim_pos_inc_rad->SetMarkerColor(2);
  z_sim_pos_inc_rad->DrawCopy("same");
  z_sim_pos_exc_rad->SetMarkerColor(kGreen);
  z_sim_pos_exc_rad->DrawCopy("same");
  z_sim_pos_rho->SetMarkerStyle(8);
  z_sim_pos_rho->SetMarkerColor(6);
  z_sim_pos_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  c_z->cd(2);
  h_neg_z_all->SetBit(TH1::kNoStats);
  h_neg_z_all->DrawCopy("hist");
  z_sim_neg_inc_norad->SetMarkerStyle(8);
  z_sim_neg_inc_norad->SetMarkerColor(1);
  z_sim_neg_inc_norad->DrawCopy("same");
  z_sim_neg_inc_rad->SetMarkerStyle(8);
  z_sim_neg_inc_rad->SetMarkerColor(2);
  z_sim_neg_inc_rad->DrawCopy("same");
  z_sim_neg_rho->SetMarkerStyle(8);
  z_sim_neg_rho->SetMarkerColor(6);
  z_sim_neg_rho->DrawCopy("same");
  gPad->BuildLegend(0.75,0.75,0.95,0.95);
  z_sim_neg_exc_rad->SetMarkerColor(kGreen);
  z_sim_neg_exc_rad->DrawCopy("same");
  std::string c_z_name = "results/simc/z_"+std::to_string(RunGroup)+".pdf";
  c_z->SaveAs(c_z_name.c_str());
  
  //auto h_Em_Pm_sim_inc_norad = d_sim_pos_inc_norad.Histo2D({"","",100,0,4,100,0,4},"Em","Pm");
  //auto h_Em_Pm_sim_inc_rad = d_sim_pos_inc_rad.Histo2D({"","",100,0,4,100,0,4},"Em","Pm");
  //TCanvas *c_radia= new TCanvas();
  //c_radia->Divide(2,1);
  //c_radia->cd(1);
  ////h_pos_pmiss_all->DrawCopy("hist"); 
  //TVirtualPad* Pad_data = c_radia->cd(1);
  //Pad_data->SetLogz();
  //h_pos_Em_Pm_data_all->SetBit(TH1::kNoStats);
  //h_pos_Em_Pm_data_all->DrawCopy("LEGO");
  //c_radia->cd(2);
  //TVirtualPad* Pad_sim = c_radia->cd(2);
  //Pad_sim->SetLogz();
  //h_Em_Pm_sim_inc_norad->SetBit(TH1::kNoStats);
  //h_Em_Pm_sim_inc_norad->DrawCopy("LEGO");
  //std::string c_radia_name = "results/simc/radia_"+std::to_string(RunGroup)+".pdf";
  //c_radia->SaveAs(c_radia_name.c_str());



  auto p_xptar_sim_pos = d_sim_pos_inc_rad.Histo1D({"shms xp_tar","shms xp_tar sim",100,-0.1,0.1},"ssxptar","weight_new");
  auto p_yptar_sim_pos = d_sim_pos_inc_rad.Histo1D({"shms yp_tar","shms yp_tar sim",100,-0.1,0.1},"ssyptar","weight_new");
  auto p_ytar_sim_pos = d_sim_pos_inc_rad.Histo1D({"shms y_tar","shms y_tar sim",100,-4,4},"ssytar","weight_new");
  auto p_delta_sim_pos = d_sim_pos_inc_rad.Histo1D({"shms dp","shms dp sim",100,-10,22},"ssdelta","weight_new");
  TCanvas *c_tar_p_pos = new TCanvas();
  c_tar_p_pos->Divide(2,2);
  c_tar_p_pos->cd(1);
  h_pos_xptar_all->DrawCopy("hist");
  p_xptar_sim_pos->SetLineColor(kRed);
  p_xptar_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(2);
  h_pos_yptar_all->DrawCopy("hist");
  p_yptar_sim_pos->SetLineColor(kRed);
  p_yptar_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(3);
  h_pos_dp_all->DrawCopy("hist");
  p_delta_sim_pos->SetLineColor(kRed);
  p_delta_sim_pos->DrawCopy("hist same");
  c_tar_p_pos->cd(4);
  h_pos_ytar_all->DrawCopy("hist");
  p_ytar_sim_pos->SetLineColor(kRed);
  p_ytar_sim_pos->DrawCopy("hist same");
  std::string c_tar_p_pos_name = "results/simc/shms_tar_"+std::to_string(RunGroup)+"_pos.pdf";
  c_tar_p_pos->SaveAs(c_tar_p_pos_name.c_str());

  auto p_xptar_sim_neg = d_sim_neg_inc_rad.Histo1D({"shms xp_tar","shms xp_tar sim",100,-0.1,0.1},"ssxptar","weight_new");
  auto p_yptar_sim_neg = d_sim_neg_inc_rad.Histo1D({"shms yp_tar","shms yp_tar sim",100,-0.1,0.1},"ssyptar","weight_new");
  auto p_ytar_sim_neg = d_sim_neg_inc_rad.Histo1D({"shms y_tar","shms y_tar sim",100,-4,4},"ssytar","weight_new");
  auto p_delta_sim_neg = d_sim_neg_inc_rad.Histo1D({"shms dp","shms dp sim",100,-10,22},"ssdelta","weight_new");
  TCanvas *c_tar_p_neg = new TCanvas();
  c_tar_p_neg->Divide(2,2);
  c_tar_p_neg->cd(1);
  h_neg_xptar_all->DrawCopy("hist");
  p_xptar_sim_neg->SetLineColor(kRed);
  p_xptar_sim_neg->DrawCopy("hist same");
  c_tar_p_neg->cd(2);
  h_neg_yptar_all->DrawCopy("hist");
  p_yptar_sim_neg->SetLineColor(kRed);
  p_yptar_sim_neg->DrawCopy("hist same");
  c_tar_p_neg->cd(3);
  h_neg_dp_all->DrawCopy("hist");
  p_delta_sim_neg->SetLineColor(kRed);
  p_delta_sim_neg->DrawCopy("hist same");
  c_tar_p_neg->cd(4);
  h_neg_ytar_all->DrawCopy("hist");
  p_ytar_sim_neg->SetLineColor(kRed);
  p_ytar_sim_neg->DrawCopy("hist same");
  std::string c_tar_p_neg_name = "results/simc/shms_tar_"+std::to_string(RunGroup)+"_neg.pdf";
  c_tar_p_neg->SaveAs(c_tar_p_neg_name.c_str());
  }//if it's normal production runs
 
}
