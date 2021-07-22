
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
#include <math.h>

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

//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;
constexpr const double M_P = 0.938272;
void statistic_runs_D2_sim(int RunGroup = 0){

  if(RunGroup ==0){
    std::cout<<"Enter a RunGroup (-1 to exit):";
    std::cin>>RunGroup;
    if(RunGroup<=0)
      return;
  }
    auto z = [](float pq, float ph) {
      return ph/pq;
    };
    auto Mx2 = [](float nu,float z,float pmiss){
      return (M_P+nu - z*nu)*(M_P+nu - z*nu) -abs(pmiss)*abs(pmiss);
    };
    auto xbj = [=](float Q2,float pq){
      return Q2/(2.0*M_P*pq);
    };
    auto phi_2pi = [](double phi){
     if(phi>M_PI){
       return phi-2*M_PI;
     }
     return phi;
    };
    auto pt = [](double p,double th){return p*std::sin(th);};
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

  auto W2 = [](float W){return W*W;};
  double W2_cut_num = j_cuts["W2"].get<double>();
  std::string W2_cut = "W2 > "+std::to_string(W2_cut_num);
  double Mx2_cut_num = j_cuts["Mx2"].get<double>();
  std::string Mx2_cut = "Mx2>"+std::to_string(Mx2_cut_num);
  double pt_cut_num = j_cuts["pt_cut"].get<double>();
  std::string pt_cut = "pt<"+std::to_string(pt_cut_num);
  std::string D2_neg_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_norad.root";
  std::string D2_neg_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_rad.root";
  std::string D2_neg_exc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_exc_rad.root";
  std::string D2_neg_rho_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_rho.root";
  std::string D2_neg_delta_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_neg_delta.root";
  std::string Dummy_neg_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_Dummy_neg_inc_rad.root";
  std::string D2_pos_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_norad.root";
  std::string D2_pos_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.root";
  std::string D2_pos_exc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_exc_rad.root";
  std::string D2_pos_rho_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_rho.root";
  std::string D2_pos_delta_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_D2_pos_delta.root";
  std::string Dummy_pos_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_Dummy_pos_inc_rad.root";

  std::string skim_name = "results/skim_root/"+std::to_string(RunGroup)+"_sim.root";
  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "UPDATE";

  //For inc norad
  ROOT::RDataFrame d_D2_neg_inc_norad_raw("h10",D2_neg_inc_norad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_D2_pos_inc_norad_raw("h10",D2_pos_inc_norad_rootfile_name.c_str()); 
  double nentries_D2_neg_inc_norad = *d_D2_neg_inc_norad_raw.Count();
  double nentries_D2_pos_inc_norad = *d_D2_pos_inc_norad_raw.Count();
  double normfac_D2_neg_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["inc"]["norad"]["normfac"].get<double>();
  double normfac_D2_pos_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["inc"]["norad"]["normfac"].get<double>();
  auto d_D2_neg_inc_norad_1 = d_D2_neg_inc_norad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    .Define("phi_2pi",phi_2pi,{"phipq"})
    .Define("pt",pt,{"ppi","thetapq"})
    .Filter(pt_cut)
    ;
  double wfac_D2_neg_inc_norad = (normfac_D2_neg_inc_norad/nentries_D2_neg_inc_norad);
  //auto weight_calculate = [&](float weight){return wfac_D2_neg_inc_norad*weight;}
  //neg inc norad
  auto d_D2_neg_inc_norad = d_D2_neg_inc_norad_1.Define("weight_new",[wfac_D2_neg_inc_norad](float weight){return wfac_D2_neg_inc_norad*weight;},{"Weight"});
  d_D2_neg_inc_norad.Snapshot("T_neg_inc_norad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta","phipq","thetapq","ppi","phi_2pi"});
  auto d_D2_pos_inc_norad_1 = d_D2_pos_inc_norad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    .Define("phi_2pi",phi_2pi,{"phipq"})
    .Define("pt",pt,{"ppi","thetapq"})
    .Filter(pt_cut)
    ;
  std::cout<<"sim counts "<<nentries_D2_pos_inc_norad<<std::endl;
  double wfac_D2_pos_inc_norad = (normfac_D2_pos_inc_norad/nentries_D2_pos_inc_norad);
  //pos inc norad
  auto d_D2_pos_inc_norad = d_D2_pos_inc_norad_1.Define("weight_new",[wfac_D2_pos_inc_norad](float weight){return wfac_D2_pos_inc_norad*weight;},{"Weight"})
    ;
  d_D2_pos_inc_norad.Snapshot("T_pos_inc_norad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta","phipq","thetapq","ppi","phi_2pi"},opts);

  std::cout<<"normfac check "<<std::endl;
  
  //For exc rad
  ROOT::RDataFrame d_D2_neg_exc_rad_raw("h10",D2_neg_exc_rad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_D2_pos_exc_rad_raw("h10",D2_pos_exc_rad_rootfile_name.c_str()); 
  auto d_D2_neg_exc_rad_1 = d_D2_neg_exc_rad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("xbj",xbj,{"Q2","nu"})
    .Define("z",z,{"nu","phad"})
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_neg_exc_rad = *d_D2_neg_exc_rad_raw.Count();

  double normfac_D2_neg_exc_rad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["exc"]["rad"]["normfac"].get<double>();
  double normfac_D2_pos_exc_rad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["exc"]["rad"]["normfac"].get<double>();

  double wfac_D2_neg_exc_rad = (normfac_D2_neg_exc_rad/nentries_D2_neg_exc_rad);
  //auto weight_calculate = [&](float weight){return wfac_D2_neg_exc_rad*weight;}
  //neg exc rad
  auto d_D2_neg_exc_rad = d_D2_neg_exc_rad_1.Define("weight_new",[wfac_D2_neg_exc_rad](float weight){return wfac_D2_neg_exc_rad*weight;},{"Weight"});
  d_D2_neg_exc_rad.Snapshot("T_neg_exc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  auto d_D2_pos_exc_rad_1 = d_D2_pos_exc_rad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("xbj",xbj,{"Q2","nu"})
    .Define("z",z,{"nu","phad"})
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_pos_exc_rad = *d_D2_pos_exc_rad_raw.Count();
  std::cout<<"sim counts "<<nentries_D2_pos_exc_rad<<std::endl;
  double wfac_D2_pos_exc_rad = (normfac_D2_pos_exc_rad/nentries_D2_pos_exc_rad);
  //pos exc rad
  auto d_D2_pos_exc_rad = d_D2_pos_exc_rad_1.Define("weight_new",[wfac_D2_pos_exc_rad](float weight){return wfac_D2_pos_exc_rad*weight;},{"Weight"});
  d_D2_pos_exc_rad.Snapshot("T_pos_exc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For inc rad
  ROOT::RDataFrame d_D2_neg_inc_rad_raw("h10",D2_neg_inc_rad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_D2_pos_inc_rad_raw("h10",D2_pos_inc_rad_rootfile_name.c_str()); 
  auto d_D2_neg_inc_rad_1 = d_D2_neg_inc_rad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    .Define("phi_2pi",phi_2pi,{"phipq"})
    .Define("pt",pt,{"ppi","thetapq"})
    .Filter(pt_cut)
    ;
  double nentries_D2_neg_inc_rad = *d_D2_neg_inc_rad_raw.Count();

  double normfac_D2_neg_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["inc"]["rad"]["normfac"].get<double>();
  double normfac_D2_pos_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["inc"]["rad"]["normfac"].get<double>();

  double wfac_D2_neg_inc_rad = (normfac_D2_neg_inc_rad/nentries_D2_neg_inc_rad);
  //auto weight_calculate = [&](float weight){return wfac_D2_neg_inc_rad*weight;}
  //neg inc rad
  auto d_D2_neg_inc_rad = d_D2_neg_inc_rad_1.Define("weight_new",[wfac_D2_neg_inc_rad](float weight){return wfac_D2_neg_inc_rad*weight;},{"Weight"});
  d_D2_neg_inc_rad.Snapshot("T_neg_inc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);

  auto d_D2_pos_inc_rad_1 = d_D2_pos_inc_rad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    .Define("phi_2pi",phi_2pi,{"phipq"})
    .Define("pt",pt,{"ppi","thetapq"})
    .Filter(pt_cut)
    ;
  double nentries_D2_pos_inc_rad = *d_D2_pos_inc_rad_raw.Count();
  std::cout<<"sim counts "<<nentries_D2_pos_inc_rad<<std::endl;
  double wfac_D2_pos_inc_rad = (normfac_D2_pos_inc_rad/nentries_D2_pos_inc_rad);
  //pos inc rad
  auto d_D2_pos_inc_rad = d_D2_pos_inc_rad_1.Define("weight_new",[wfac_D2_pos_inc_rad](float weight){return wfac_D2_pos_inc_rad*weight;},{"Weight"});
  d_D2_pos_inc_rad.Snapshot("T_pos_inc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For rho 
  ROOT::RDataFrame d_D2_neg_rho_raw("h10",D2_neg_rho_rootfile_name.c_str()); 
  auto d_D2_neg_rho_1 = d_D2_neg_rho_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_neg_rho = *d_D2_neg_rho_raw.Count();

  double normfac_D2_neg_rho = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["rho"]["normfac"].get<double>();

  double wfac_D2_neg_rho = (normfac_D2_neg_rho/nentries_D2_neg_rho);
  auto d_D2_neg_rho = d_D2_neg_rho_1.Define("weight_new",[wfac_D2_neg_rho](float weight){return wfac_D2_neg_rho*weight;},{"Weight"});
  d_D2_neg_rho.Snapshot("T_neg_rho",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  //pos rho 
  ROOT::RDataFrame d_D2_pos_rho_raw("h10",D2_pos_rho_rootfile_name.c_str()); 
  double normfac_D2_pos_rho = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["rho"]["normfac"].get<double>();
  auto d_D2_pos_rho_1 = d_D2_pos_rho_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_pos_rho = *d_D2_pos_rho_raw.Count();
  std::cout<<"sim counts "<<nentries_D2_pos_rho<<std::endl;
  double wfac_D2_pos_rho = (normfac_D2_pos_rho/nentries_D2_pos_rho);
  auto d_D2_pos_rho = d_D2_pos_rho_1.Define("weight_new",[wfac_D2_pos_rho](float weight){return wfac_D2_pos_rho*weight;},{"Weight"});
  d_D2_pos_rho.Snapshot("T_pos_rho",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For delta 
  ROOT::RDataFrame d_D2_neg_delta_raw("h10",D2_neg_delta_rootfile_name.c_str()); 
  auto d_D2_neg_delta_1 = d_D2_neg_delta_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("xbj",xbj,{"Q2","nu"})
    .Define("z",z,{"nu","phad"})
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_neg_delta = *d_D2_neg_delta_raw.Count();

  double normfac_D2_neg_delta = j_simc[std::to_string(RunGroup).c_str()]["D2"]["neg"]["delta"]["normfac"].get<double>();

  double wfac_D2_neg_delta = (normfac_D2_neg_delta/nentries_D2_neg_delta);
  auto d_D2_neg_delta = d_D2_neg_delta_1.Define("weight_new",[wfac_D2_neg_delta](float weight){return wfac_D2_neg_delta*weight;},{"Weight"});
  d_D2_neg_delta.Snapshot("T_neg_delta",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  //pos delta 
  ROOT::RDataFrame d_D2_pos_delta_raw("h10",D2_pos_delta_rootfile_name.c_str()); 
  double normfac_D2_pos_delta = j_simc[std::to_string(RunGroup).c_str()]["D2"]["pos"]["delta"]["normfac"].get<double>();
  auto d_D2_pos_delta_1 = d_D2_pos_delta_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("xbj",xbj,{"Q2","nu"})
    .Define("z",z,{"nu","phad"})
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    //.Define("phi_2pi",phi_2pi,{"phipq"})
    //.Define("pt",pt,{"ppi","thetapq"})
    //.Filter(pt_cut)
    ;
  double nentries_D2_pos_delta = *d_D2_pos_delta_raw.Count();
  std::cout<<"sim counts "<<nentries_D2_pos_delta<<std::endl;
  double wfac_D2_pos_delta = (normfac_D2_pos_delta/nentries_D2_pos_delta);
  auto d_D2_pos_delta = d_D2_pos_delta_1.Define("weight_new",[wfac_D2_pos_delta](float weight){return wfac_D2_pos_delta*weight;},{"Weight"});
  d_D2_pos_delta.Snapshot("T_pos_delta",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);

  ////For inc rad
  //ROOT::RDataFrame d_Dummy_neg_inc_rad_raw("h10",Dummy_neg_inc_rad_rootfile_name.c_str()); 
  //ROOT::RDataFrame d_Dummy_pos_inc_rad_raw("h10",Dummy_pos_inc_rad_rootfile_name.c_str()); 
  //auto d_Dummy_neg_inc_rad_1 = d_Dummy_neg_inc_rad_raw
  //  .Filter(Good_Track_SHMS_sim)
  //  .Filter(Good_Track_HMS_sim)
  //  .Filter(Normal_xptar_SHMS_sim)
  //  .Filter(Normal_xptar_HMS_sim)
  //  .Filter(Normal_yptar_SHMS_sim)
  //  .Filter(Normal_yptar_HMS_sim)
  //  ;
  //double nentries_Dummy_neg_inc_rad = *d_Dummy_neg_inc_rad_raw.Count();

  //double normfac_Dummy_neg_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["Dummy"]["neg"]["inc"]["rad"]["normfac"].get<double>();
  //double normfac_Dummy_pos_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["Dummy"]["pos"]["inc"]["rad"]["normfac"].get<double>();

  //double wfac_Dummy_neg_inc_rad = (normfac_Dummy_neg_inc_rad/nentries_Dummy_neg_inc_rad);
  ////auto weight_calculate = [&](float weight){return wfac_Dummy_neg_inc_rad*weight;}
  ////neg inc rad
  //auto d_Dummy_neg_inc_rad = d_Dummy_neg_inc_rad_1.Define("weight_new",[wfac_Dummy_neg_inc_rad](float weight){return wfac_Dummy_neg_inc_rad*weight;},{"Weight"});
  //auto d_Dummy_pos_inc_rad_1 = d_Dummy_pos_inc_rad_raw
  //  .Filter(Good_Track_SHMS_sim)
  //  .Filter(Good_Track_HMS_sim)
  //  .Filter(Normal_xptar_SHMS_sim)
  //  .Filter(Normal_xptar_HMS_sim)
  //  .Filter(Normal_yptar_SHMS_sim)
  //  .Filter(Normal_yptar_HMS_sim)
  //  ;
  //double nentries_Dummy_pos_inc_rad = *d_Dummy_pos_inc_rad_raw.Count();
  //std::cout<<"sim counts "<<nentries_Dummy_pos_inc_rad<<std::endl;
  //double wfac_Dummy_pos_inc_rad = (normfac_Dummy_pos_inc_rad/nentries_Dummy_pos_inc_rad);
  ////pos inc rad
  //auto d_Dummy_pos_inc_rad = d_Dummy_pos_inc_rad_1.Define("weight_new",[wfac_Dummy_pos_inc_rad](float weight){return wfac_Dummy_pos_inc_rad*weight;},{"Weight"});

  //double charge_D2_neg_inc_norad = 0,charge_D2_pos_inc_norad = 0;
  //
  //if(!neg_D2.empty() && !pos_D2.empty()){
  //  for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
  //    int RunNumber = *it;
  //    double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
  //    charge_D2_neg_inc_norad+=charge;
  //  }
  //  for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
  //    int RunNumber = *it;
  //    double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
  //    charge_D2_pos_inc_norad+=charge;
  //  }
  //}

  //std::cout<<"neg charge "<<charge_D2_neg_inc_norad<<" pos charge "<<charge_D2_pos_inc_norad<<std::endl;
  
  auto h_Q2_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"Q2_neg_inc_norad","Q2_neg_inc_norad",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"Q2_pos_inc_norad","Q2_pos_inc_norad",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"xbj_neg_inc_norad","xbj_neg_inc_norad",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"xbj_pos_inc_norad","xbj_pos_inc_norad",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_inc_norad = d_D2_neg_inc_norad.Histo1D({"z_neg_inc_norad","z_neg_inc_norad",100,0,1},"z","weight_new");
  auto h_z_D2_pos_inc_norad = d_D2_pos_inc_norad.Histo1D({"z_pos_inc_norad","z_pos_inc_norad",100,0,1},"z","weight_new");
  auto h_x_z_neg_inc_norad = d_D2_neg_inc_norad.Histo2D({"x_z_neg_inc_norad","x_z_neg_inc_norad",100,0,1,100,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_inc_norad = d_D2_pos_inc_norad.Histo2D({"x_z_pos_inc_norad","x_z_pos_inc_norad",100,0,1,100,0,1},"z","xbj","weight_new");
  std::cout<<"inc norad check"<<std::endl;  
  auto h_Q2_D2_neg_exc_rad = d_D2_neg_exc_rad.Histo1D({"Q2_neg_exc_rad","Q2_neg_exc_rad",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_exc_rad = d_D2_pos_exc_rad.Histo1D({"Q2_pos_exc_rad","Q2_pos_exc_rad",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_exc_rad = d_D2_neg_exc_rad.Histo1D({"xbj_neg_exc_rad","xbj_neg_exc_rad",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_exc_rad = d_D2_pos_exc_rad.Histo1D({"xbj_pos_exc_rad","xbj_pos_exc_rad",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_exc_rad = d_D2_neg_exc_rad.Histo1D({"z_neg_exc_rad","z_neg_exc_rad",100,0,1},"z","weight_new");
  auto h_z_D2_pos_exc_rad = d_D2_pos_exc_rad.Histo1D({"z_pos_exc_rad","z_pos_exc_rad",100,0,1},"z","weight_new");
  auto h_x_z_neg_exc_rad = d_D2_neg_exc_rad.Histo2D({"x_z_neg_exc_rad","x_z_neg_exc_rad",100,0,1,100,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_exc_rad = d_D2_pos_exc_rad.Histo2D({"x_z_pos_exc_rad","x_z_pos_exc_rad",100,0,1,100,0,1},"z","xbj","weight_new");
  std::cout<<"exc norad check"<<std::endl;  
  auto h_Q2_D2_neg_inc_rad = d_D2_neg_inc_rad.Histo1D({"Q2_neg_inc_rad","Q2_neg_inc_rad",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_inc_rad = d_D2_pos_inc_rad.Histo1D({"Q2_pos_inc_rad","Q2_pos_inc_rad",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_inc_rad = d_D2_neg_inc_rad.Histo1D({"xbj_neg_inc_rad","xbj_neg_inc_rad",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_inc_rad = d_D2_pos_inc_rad.Histo1D({"xbj_pos_inc_rad","xbj_pos_inc_rad",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_inc_rad = d_D2_neg_inc_rad.Histo1D({"z_neg_inc_rad","z_neg_inc_rad",100,0,1},"z","weight_new");
  auto h_z_D2_pos_inc_rad = d_D2_pos_inc_rad.Histo1D({"z_pos_inc_rad","z_pos_inc_rad",100,0,1},"z","weight_new");
  auto h_x_z_neg_inc_rad = d_D2_neg_inc_rad.Histo2D({"x_z_neg_inc_rad","x_z_neg_inc_rad",100,0,1,100,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_inc_rad = d_D2_pos_inc_rad.Histo2D({"x_z_pos_inc_rad","x_z_pos_inc_rad",100,0,1,100,0,1},"z","xbj","weight_new");
  std::cout<<"inc rad check"<<std::endl;  

  auto h_Q2_D2_neg_rho = d_D2_neg_rho.Histo1D({"Q2_neg_rho","Q2_neg_rho",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_rho = d_D2_pos_rho.Histo1D({"Q2_pos_rho","Q2_pos_rho",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_rho = d_D2_neg_rho.Histo1D({"xbj_neg_rho","xbj_neg_rho",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_rho = d_D2_pos_rho.Histo1D({"xbj_pos_rho","xbj_pos_rho",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_rho = d_D2_neg_rho.Histo1D({"z_neg_rho","z_neg_rho",100,0,1},"z","weight_new");
  auto h_z_D2_pos_rho = d_D2_pos_rho.Histo1D({"z_pos_rho","z_pos_rho",100,0,1},"z","weight_new");
  auto h_x_z_neg_rho = d_D2_neg_rho.Histo2D({"x_z_neg_rho","x_z_neg_rho",100,0,1,100,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_rho = d_D2_pos_rho.Histo2D({"x_z_pos_rho","x_z_pos_rho",100,0,1,100,0,1},"z","xbj","weight_new");
  std::cout<<"rho check"<<std::endl;  
  
  auto h_Q2_D2_neg_delta = d_D2_neg_delta.Histo1D({"Q2_neg_delta","Q2_neg_delta",100,0,10},"Q2","weight_new");
  auto h_Q2_D2_pos_delta = d_D2_pos_delta.Histo1D({"Q2_pos_delta","Q2_pos_delta",100,0,10},"Q2","weight_new");
  auto h_xbj_D2_neg_delta = d_D2_neg_delta.Histo1D({"xbj_neg_delta","xbj_neg_delta",100,0,1},"xbj","weight_new");
  auto h_xbj_D2_pos_delta = d_D2_pos_delta.Histo1D({"xbj_pos_delta","xbj_pos_delta",100,0,1},"xbj","weight_new");
  auto h_z_D2_neg_delta = d_D2_neg_delta.Histo1D({"z_neg_delta","z_neg_delta",100,0,1},"z","weight_new");
  auto h_z_D2_pos_delta = d_D2_pos_delta.Histo1D({"z_pos_delta","z_pos_delta",100,0,1},"z","weight_new");
  auto h_x_z_neg_delta = d_D2_neg_delta.Histo2D({"x_z_neg_delta","x_z_neg_delta",100,0,1,100,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_delta = d_D2_pos_delta.Histo2D({"x_z_pos_delta","x_z_pos_delta",100,0,1,100,0,1},"z","xbj","weight_new");
  std::cout<<"delta check"<<std::endl;  
  
  std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_simc.root";
  TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
  h_Q2_D2_neg_inc_norad->Write();
  h_Q2_D2_pos_inc_norad->Write();
  h_xbj_D2_neg_inc_norad->Write();
  h_xbj_D2_pos_inc_norad->Write();
  h_z_D2_neg_inc_norad->Write();
  h_z_D2_pos_inc_norad->Write();
  h_x_z_neg_inc_norad->Write();
  h_x_z_pos_inc_norad->Write();
  h_Q2_D2_neg_exc_rad->Write();
  h_Q2_D2_pos_exc_rad->Write();
  h_xbj_D2_neg_exc_rad->Write();
  h_xbj_D2_pos_exc_rad->Write();
  h_z_D2_neg_exc_rad->Write();
  h_z_D2_pos_exc_rad->Write();
  h_x_z_neg_exc_rad->Write();
  h_x_z_pos_exc_rad->Write();
  h_Q2_D2_neg_inc_rad->Write();
  h_Q2_D2_pos_inc_rad->Write();
  h_xbj_D2_neg_inc_rad->Write();
  h_xbj_D2_pos_inc_rad->Write();
  h_z_D2_neg_inc_rad->Write();
  h_z_D2_pos_inc_rad->Write();
  h_x_z_neg_inc_rad->Write();
  h_x_z_pos_inc_rad->Write();
  h_Q2_D2_neg_rho->Write();
  h_Q2_D2_pos_rho->Write();
  h_xbj_D2_neg_rho->Write();
  h_xbj_D2_pos_rho->Write();
  h_z_D2_neg_rho->Write();
  h_z_D2_pos_rho->Write();
  h_x_z_neg_rho->Write();
  h_x_z_pos_rho->Write();
  h_Q2_D2_neg_delta->Write();
  h_Q2_D2_pos_delta->Write();
  h_xbj_D2_neg_delta->Write();
  h_xbj_D2_pos_delta->Write();
  h_z_D2_neg_delta->Write();
  h_z_D2_pos_delta->Write();
  h_x_z_neg_delta->Write();
  h_x_z_pos_delta->Write();

  rootfile_out->Close();

}
