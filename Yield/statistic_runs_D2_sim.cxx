
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

void statistic_runs_D2_sim(int RunGroup = 0){

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
  std::cout<<" D2 runs check "<<std::endl;
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  };

  json j_runs;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runs;
  };

  json j_simc;
  {
    std::ifstream ifs("db2/simc_rungroup.json");
    ifs>>j_simc;
  };

  double H_dp_low = j_cuts["H_dp_low"].get<double>();
  double H_dp_high = j_cuts["H_dp_high"].get<double>();
  double P_dp_low = j_cuts["P_dp_low"].get<double>();
  double P_dp_high = j_cuts["P_dp_high"].get<double>();
  double H_xptar_low = j_cuts["H_xptar_low"].get<double>();
  double H_xptar_high = j_cuts["H_xptar_high"].get<double>();
  double H_yptar_low = j_cuts["H_yptar_low"].get<double>();
  double H_yptar_high = j_cuts["H_yptar_high"].get<double>();
  double P_xptar_low = j_cuts["P_xptar_low"].get<double>();
  double P_xptar_high = j_cuts["P_xptar_high"].get<double>();
  double P_yptar_low = j_cuts["P_yptar_low"].get<double>();
  double P_yptar_high = j_cuts["P_yptar_high"].get<double>();
  std::cout<<" cuts check"<<std::endl;

  std::string Good_Track_SHMS_sim = "ssdelta>"+std::to_string(P_dp_low)+"&& ssdelta < "+std::to_string(P_dp_high);
  std::string Good_Track_HMS_sim = "hsdelta>"+std::to_string(H_dp_low)+"&& hsdelta < "+std::to_string(H_dp_high);
  std::string Normal_xptar_SHMS_sim = "ssxptar>"+std::to_string(P_xptar_low)+"&& ssxptar < "+std::to_string(P_xptar_high);
  std::string Normal_xptar_HMS_sim = "hsxptar>"+std::to_string(H_xptar_low)+"&& hsxptar < "+std::to_string(H_xptar_high);
  std::string Normal_yptar_SHMS_sim = "ssyptar>"+std::to_string(P_yptar_low)+"&& ssyptar < "+std::to_string(P_yptar_high);
  std::string Normal_yptar_HMS_sim = "hsyptar>"+std::to_string(H_yptar_low)+"&& hsyptar < "+std::to_string(H_yptar_high);


  std::string D2_neg_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_norad.root";
  std::string D2_neg_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_rad.root";
  std::string D2_neg_exc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_exc_norad.root";
  std::string D2_pos_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_norad.root";
  std::string D2_pos_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.root";
  std::string D2_pos_exc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_exc_norad.root";
  ROOT::RDataFrame d_D2_neg_inc_norad_raw("h10",D2_neg_inc_norad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_D2_pos_inc_norad_raw("h10",D2_pos_inc_norad_rootfile_name.c_str()); 
  auto d_D2_neg_inc_norad_1 = d_D2_neg_inc_norad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    ;
  double nentries_D2_neg_inc_norad = *d_D2_neg_inc_norad_1.Count();

  double normfac_D2_neg_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["inc"]["norad"]["normfac"].get<double>();
  double normfac_D2_pos_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["inc"]["norad"]["normfac"].get<double>();
  std::cout<<"normfac check "<<std::endl;
  double charge_D2_neg_inc_norad = 0,charge_D2_pos_inc_norad = 0;
  
  if(!neg_D2.empty() && !pos_D2.empty()){
    for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
      charge_D2_neg_inc_norad+=charge;
    }
    for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
      charge_D2_pos_inc_norad+=charge;
    }
  }

  std::cout<<"neg charge "<<charge_D2_neg_inc_norad<<" pos charge "<<charge_D2_pos_inc_norad<<std::endl;

  double wfac_D2_neg_inc_norad = (normfac_D2_neg_inc_norad/nentries_D2_neg_inc_norad)*(charge_D2_neg_inc_norad/1);
  //auto weight_calculate = [&](float weight){return wfac_D2_neg_inc_norad*weight;}
  auto d_D2_neg_inc_norad = d_D2_neg_inc_norad_1.Define("weight_new",[wfac_D2_neg_inc_norad](float weight){return wfac_D2_neg_inc_norad*weight;},{"Weight"});
  auto d_D2_pos_inc_norad_1 = d_D2_pos_inc_norad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    ;
  double nentries_D2_pos_inc_norad = *d_D2_pos_inc_norad_1.Count();
  double wfac_D2_pos_inc_norad = (normfac_D2_pos_inc_norad/nentries_D2_pos_inc_norad)*(charge_D2_pos_inc_norad/1);
  auto d_D2_pos_inc_norad = d_D2_pos_inc_norad_1.Define("weight_new",[wfac_D2_pos_inc_norad](float weight){return wfac_D2_pos_inc_norad*weight;},{"Weight"});

  auto h_Q2_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"Q2_neg","Q2_neg",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"Q2_pos","Q2_pos",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"xbj_neg","xbj_neg",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"xbj_pos","xbj_pos",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"z_neg","z_neg",100,0,1},"z","weight_new");
  auto h_z_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"z_pos","z_pos",100,0,1},"z","weight_new");
  std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root";
  TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
  h_Q2_D2_neg_inc_norad->Write();
  h_Q2_D2_pos_inc_norad->Write();
  h_xbj_D2_neg_inc_norad->Write();
  h_xbj_D2_pos_inc_norad->Write();
  h_z_D2_neg_inc_norad->Write();
  h_z_D2_pos_inc_norad->Write();
  rootfile_out->Close();

}
