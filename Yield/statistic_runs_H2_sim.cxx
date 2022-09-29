
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
void statistic_runs_H2_sim(int RunGroup = 0){

  if(RunGroup ==0){
    std::cout<<"Enter a RunGroup (-1 to exit):";
    std::cin>>RunGroup;
    if(RunGroup<=0)
      return;
  }
  //auto p_pion = [](double px, double py, double pz) {
  //  return TLorentzVector{px , py , pz , M_pion};
  //};
  //auto p_electron = [](double px, double py, double pz) {
  //  return TLorentzVector{px , py , pz , M_e};
  //};
  //auto p_q = [](TLorentzVector pe ) {
  //  return TLorentzVector{0.0,0.0,10.214, M_e}-pe;
  //};
    auto z = [](float pq, float ph) {
      return ph/pq;
    };
    auto Mx2 = [](float nu,float z,float pmiss){
      return (M_P+nu - z*nu)*(M_P+nu - z*nu) -abs(pmiss)*abs(pmiss);
    };
  //  auto W2 = [](TLorentzVector pq) {
  //    auto Ptot = TLorentzVector{0.0,0.0,0.0, M_P} + pq;
  //    return Ptot.Dot(Ptot);
  //  };
  //  auto Wprime2 = [](TLorentzVector pq,TLorentzVector ph){
  //    auto Ptot = TLorentzVector{0.0,0.0,0.0,M_P}+pq-ph;
  //    return Ptot.Dot(Ptot);
  //  };
    auto xbj = [=](float Q2,float pq){
      return Q2/(2.0*M_P*pq);
    };
    auto phi_2pi = [](float phi){
      int high_than_pi = 0;
      if(phi>M_PI){
        high_than_pi =2;
      }
     return phi-high_than_pi*M_PI;
    };
    auto pt = [](float p,float th){return p*std::sin(th);};
    auto Dipole_Exit_HMS = [](float x_fp,float xp_fp,float y_fp,float yp_fp){

      float DipoleExitWindowZpos = -147.48;
      float xdip = x_fp+xp_fp*DipoleExitWindowZpos;
      float ydip = y_fp+yp_fp*DipoleExitWindowZpos;
      float xpipe_offset = 2.8;
      float ypipe_offset = 0.0;
      float pipe_rad = 46.507;
      if( ((xdip-xpipe_offset)*(xdip-xpipe_offset)+(ydip-ypipe_offset)*(ydip-ypipe_offset)) > pipe_rad*pipe_rad) return 0;
      else return 1;
    };
    auto Dipole_Exit_SHMS = [](float x_fp,float xp_fp,float y_fp,float yp_fp){
      float DipoleExitWindowZpos = -307.;
      float xdip = x_fp+xp_fp*DipoleExitWindowZpos;
      float ydip = y_fp+yp_fp*DipoleExitWindowZpos;
      float crad = 23.81;
      float voffset = crad-24.035;
      float hwid = 11.549/2;
      if(abs(ydip)<hwid){
        if(abs(xdip)>(crad+voffset)) return 0;
      } else {
        if (ydip >=hwid){
          if ( ((xdip-voffset)*(xdip-voffset)+(ydip-hwid)*(ydip-hwid))>crad*crad) return 0;
        }
        if ( ydip <=-hwid){
          if( ((xdip-voffset)*(xdip-voffset)+(ydip+hwid)*(ydip+hwid)) > crad*crad) return 0;
        }
      }
      return 1;
    };
    json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }

  std::vector<int> neg_H2,pos_H2;
  neg_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["H2"].get<std::vector<int>>();
  pos_H2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["H2"].get<std::vector<int>>();
  std::cout<<" H2 runs check "<<std::endl;
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
  double P_hgcer = j_cuts["P_hgcer"].get<double>();
  auto HGC_bad = [=](float shms_p, float x_fp,float xp_fp,float y_fp,float yp_fp){
    double xcer = x_fp+156.27*xp_fp;
    double ycer = y_fp+156.27*yp_fp;

    if(shms_p>2.9){
      if(P_hgcer==-1){
        return true;
      }
      else{
        return (xcer-1.33)*(xcer-1.33)+(ycer-0.83)*(ycer-0.83)>=36 && (xcer<0||xcer>3);
      }
    }
    else{
      return true;
    }
  };

  int bins = j_cuts["bins"].get<int>();
  json jout;
  {
    std::string if_name = "results/yield/run_info/simc_"+std::to_string(RunGroup)+"_info.json";
    std::ifstream ifs(if_name.c_str());
    ifs>>jout;
  }
  double Q2_low = j_cuts["Q2_low"].get<double>();
  double Q2_high = j_cuts["Q2_high"].get<double>();
  std::string Q2_low_cut = "Q2<"+std::to_string(Q2_low);
  std::string Q2_high_cut = "Q2>"+std::to_string(Q2_high);
  std::string Q2_middle_cut = "Q2>="+std::to_string(Q2_low)+" && Q2<="+std::to_string(Q2_high);

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
  std::string Wp2_cut = "Wp2>"+std::to_string(Mx2_cut_num);
  double pt_cut_num = j_cuts["pt_cut"].get<double>();
  std::string pt_cut = "pt<"+std::to_string(pt_cut_num);
  std::string H2_neg_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_neg_inc_norad.root";
  std::string H2_neg_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_neg_inc_rad.root";
  std::string H2_neg_exc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_neg_exc_rad.root";
  std::string H2_neg_rho_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_neg_rho.root";
  std::string H2_neg_delta_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_neg_delta.root";
  std::string Dummy_neg_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_Dummy_neg_inc_rad.root";
  std::string H2_pos_inc_norad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_pos_inc_norad.root";
  std::string H2_pos_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_pos_inc_rad.root";
  std::string H2_pos_exc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_pos_exc_rad.root";
  std::string H2_pos_rho_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_pos_rho.root";
  std::string H2_pos_delta_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_H2_pos_delta.root";
  std::string Dummy_pos_inc_rad_rootfile_name = "sim/csv_"+std::to_string(RunGroup)+"_Dummy_pos_inc_rad.root";

  std::string skim_name = "results/skim_root/"+std::to_string(RunGroup)+"_sim.root";
  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "UPDATE";

  //For inc norad
  ROOT::RDataFrame d_H2_neg_inc_norad_raw("h10",H2_neg_inc_norad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_H2_pos_inc_norad_raw("h10",H2_pos_inc_norad_rootfile_name.c_str()); 
  double nentries_H2_neg_inc_norad = *d_H2_neg_inc_norad_raw.Count();
  double nentries_H2_pos_inc_norad = *d_H2_pos_inc_norad_raw.Count();
  double normfac_H2_neg_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["neg"]["inc"]["norad"]["normfac"].get<double>();
  double normfac_H2_pos_inc_norad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["pos"]["inc"]["norad"]["normfac"].get<double>();
  auto d_H2_neg_inc_norad_1 = d_H2_neg_inc_norad_raw
    .Filter(Good_Track_SHMS_sim)
    .Filter(Good_Track_HMS_sim)
    .Filter(Normal_xptar_SHMS_sim)
    .Filter(Normal_xptar_HMS_sim)
    .Filter(Normal_yptar_SHMS_sim)
    .Filter(Normal_yptar_HMS_sim)
    .Define("Mx2",Mx2,{"nu","z","Pm"})
    .Filter(Mx2_cut)
    //.Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    //.Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    //.Define("p_q", p_q, {"p_electron"})
    //.Define("z", z, {"p_q","p_pion"})
    //.Define("Q2", Q2, {"p_q"})
    //.Define("xbj", xbj, {"Q2","p_q"})
    //.Define("W2", W2, {"p_q"})
    //.Define("Wp2", Wprime2, {"p_q","p_pion"})
    .Define("W2",W2,{"W"})
    .Filter(W2_cut)
    .Define("phi_2pi",phi_2pi,{"phipq"})
    .Define("pt",pt,{"ppi","thetapq"})
    .Filter(pt_cut)
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    .Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double wfac_H2_neg_inc_norad = (normfac_H2_neg_inc_norad/nentries_H2_neg_inc_norad);
  //auto weight_calculate = [&](float weight){return wfac_H2_neg_inc_norad*weight;}
  //neg inc norad
  auto d_H2_neg_inc_norad = d_H2_neg_inc_norad_1
    .Define("weight_new",[wfac_H2_neg_inc_norad](float weight){return wfac_H2_neg_inc_norad*weight;},{"Weight"})
    .Define("weight_sig",[wfac_H2_neg_inc_norad](float weight,float sighad){return wfac_H2_neg_inc_norad*weight*sighad;},{"Weight","sighad"});
  d_H2_neg_inc_norad.Snapshot("T_neg_inc_norad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta","phipq","thetapq","ppi"});

  auto d_H2_pos_inc_norad_1 = d_H2_pos_inc_norad_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    .Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  std::cout<<"sim counts "<<nentries_H2_pos_inc_norad<<std::endl;
  double wfac_H2_pos_inc_norad = (normfac_H2_pos_inc_norad/nentries_H2_pos_inc_norad);
  //pos inc norad
  auto d_H2_pos_inc_norad = d_H2_pos_inc_norad_1
    .Define("weight_new",[wfac_H2_pos_inc_norad](float weight){return wfac_H2_pos_inc_norad*weight;},{"Weight"})
    .Define("weight_sig",[wfac_H2_neg_inc_norad](float weight,float sighad){return wfac_H2_neg_inc_norad*weight*sighad;},{"Weight","sighad"});
    ;
  d_H2_pos_inc_norad.Snapshot("T_pos_inc_norad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta","phipq","thetapq","ppi"},opts);


  std::cout<<"normfac check "<<std::endl;
  
  //For exc rad
  //ROOT::RDataFrame d_H2_neg_exc_rad_raw("h10",H2_neg_exc_rad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_H2_pos_exc_rad_raw("h10",H2_pos_exc_rad_rootfile_name.c_str()); 
  //auto d_H2_neg_exc_rad_1 = d_H2_neg_exc_rad_raw
  //  .Filter(Good_Track_SHMS_sim)
  //  .Filter(Good_Track_HMS_sim)
  //  .Filter(Normal_xptar_SHMS_sim)
  //  .Filter(Normal_xptar_HMS_sim)
  //  .Filter(Normal_yptar_SHMS_sim)
  //  .Filter(Normal_yptar_HMS_sim)
  //  .Define("xbj",xbj,{"Q2","nu"})
  //  .Define("z",z,{"nu","phad"})
  //  .Define("Mx2",Mx2,{"nu","z","Pm"})
  //  .Filter(Mx2_cut)
  //  .Define("W2",W2,{"W"})
  //  .Filter(W2_cut)
  //  //.Define("phi_2pi",phi_2pi,{"phipq"})
  //  //.Define("pt",pt,{"ppi","thetapq"})
  //  //.Filter(pt_cut)
  //  ;
  //double nentries_H2_neg_exc_rad = *d_H2_neg_exc_rad_raw.Count();

  //double normfac_H2_neg_exc_rad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["neg"]["exc"]["rad"]["normfac"].get<double>();
  double normfac_H2_pos_exc_rad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["pos"]["exc"]["rad"]["normfac"].get<double>();

  //double wfac_H2_neg_exc_rad = (normfac_H2_neg_exc_rad/nentries_H2_neg_exc_rad);
  ////auto weight_calculate = [&](float weight){return wfac_H2_neg_exc_rad*weight;}
  ////neg exc rad
  //auto d_H2_neg_exc_rad = d_H2_neg_exc_rad_1.Define("weight_new",[wfac_H2_neg_exc_rad](float weight){return wfac_H2_neg_exc_rad*weight;},{"Weight"});
  //d_H2_neg_exc_rad.Snapshot("T_neg_exc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  auto d_H2_pos_exc_rad_1 = d_H2_pos_exc_rad_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    //.Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_pos_exc_rad = *d_H2_pos_exc_rad_raw.Count();
  std::cout<<"sim counts "<<nentries_H2_pos_exc_rad<<std::endl;
  double wfac_H2_pos_exc_rad = (normfac_H2_pos_exc_rad/nentries_H2_pos_exc_rad);
  //pos exc rad
  auto d_H2_pos_exc_rad = d_H2_pos_exc_rad_1.Define("weight_new",[wfac_H2_pos_exc_rad](float weight){return wfac_H2_pos_exc_rad*weight;},{"Weight"});
  d_H2_pos_exc_rad.Snapshot("T_pos_exc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For inc rad
  ROOT::RDataFrame d_H2_neg_inc_rad_raw("h10",H2_neg_inc_rad_rootfile_name.c_str()); 
  ROOT::RDataFrame d_H2_pos_inc_rad_raw("h10",H2_pos_inc_rad_rootfile_name.c_str()); 
  auto d_H2_neg_inc_rad_1 = d_H2_neg_inc_rad_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    .Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_neg_inc_rad = *d_H2_neg_inc_rad_raw.Count();

  double normfac_H2_neg_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["neg"]["inc"]["rad"]["normfac"].get<double>();
  double normfac_H2_pos_inc_rad = j_simc[std::to_string(RunGroup).c_str()]["H2"]["pos"]["inc"]["rad"]["normfac"].get<double>();

  double wfac_H2_neg_inc_rad = (normfac_H2_neg_inc_rad/nentries_H2_neg_inc_rad);
  //auto weight_calculate = [&](float weight){return wfac_H2_neg_inc_rad*weight;}
  //neg inc rad
  auto d_H2_neg_inc_rad = d_H2_neg_inc_rad_1
    .Define("weight_new",[wfac_H2_neg_inc_rad](float weight){return wfac_H2_neg_inc_rad*weight;},{"Weight"})
    .Define("weight_sig",[wfac_H2_neg_inc_rad](float weight,float sighad){return wfac_H2_neg_inc_rad*weight*sighad;},{"Weight","sighad"});
  d_H2_neg_inc_rad.Snapshot("T_neg_inc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);

  auto d_H2_pos_inc_rad_1 = d_H2_pos_inc_rad_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    .Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_pos_inc_rad = *d_H2_pos_inc_rad_raw.Count();
  std::cout<<"sim counts "<<nentries_H2_pos_inc_rad<<std::endl;
  double wfac_H2_pos_inc_rad = (normfac_H2_pos_inc_rad/nentries_H2_pos_inc_rad);
  //pos inc rad
  auto d_H2_pos_inc_rad = d_H2_pos_inc_rad_1
    .Define("weight_new",[wfac_H2_pos_inc_rad](float weight){return wfac_H2_pos_inc_rad*weight;},{"Weight"})
    .Define("weight_sig",[wfac_H2_neg_inc_rad](float weight,float sighad){return wfac_H2_neg_inc_rad*weight*sighad;},{"Weight","sighad"});
  d_H2_pos_inc_rad.Snapshot("T_pos_inc_rad",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For rho 
  ROOT::RDataFrame d_H2_neg_rho_raw("h10",H2_neg_rho_rootfile_name.c_str()); 
  auto d_H2_neg_rho_1 = d_H2_neg_rho_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    //.Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_neg_rho = *d_H2_neg_rho_raw.Count();

  double normfac_H2_neg_rho = j_simc[std::to_string(RunGroup).c_str()]["H2"]["neg"]["rho"]["normfac"].get<double>();

  double wfac_H2_neg_rho = (normfac_H2_neg_rho/nentries_H2_neg_rho);
  auto d_H2_neg_rho = d_H2_neg_rho_1.Define("weight_new",[wfac_H2_neg_rho](float weight){return wfac_H2_neg_rho*weight;},{"Weight"});
  d_H2_neg_rho.Snapshot("T_neg_rho",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  //pos rho 
  ROOT::RDataFrame d_H2_pos_rho_raw("h10",H2_pos_rho_rootfile_name.c_str()); 
  double normfac_H2_pos_rho = j_simc[std::to_string(RunGroup).c_str()]["H2"]["pos"]["rho"]["normfac"].get<double>();
  auto d_H2_pos_rho_1 = d_H2_pos_rho_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    //.Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_pos_rho = *d_H2_pos_rho_raw.Count();
  double wfac_H2_pos_rho = (normfac_H2_pos_rho/nentries_H2_pos_rho);
  auto d_H2_pos_rho = d_H2_pos_rho_1.Define("weight_new",[wfac_H2_pos_rho](float weight){return wfac_H2_pos_rho*weight;},{"Weight"});
  d_H2_pos_rho.Snapshot("T_pos_rho",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  
  //For delta 
  ROOT::RDataFrame d_H2_neg_delta_raw("h10",H2_neg_delta_rootfile_name.c_str()); 
  auto d_H2_neg_delta_1 = d_H2_neg_delta_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    //.Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_neg_delta = *d_H2_neg_delta_raw.Count();
  std::cout<<"sim counts neg delta "<<nentries_H2_neg_delta<<std::endl;
  double normfac_H2_neg_delta = j_simc[std::to_string(RunGroup).c_str()]["H2"]["neg"]["delta"]["normfac"].get<double>();

  double wfac_H2_neg_delta = (normfac_H2_neg_delta/nentries_H2_neg_delta);
  auto d_H2_neg_delta = d_H2_neg_delta_1.Define("weight_new",[wfac_H2_neg_delta](float weight){return wfac_H2_neg_delta*weight;},{"Weight"});
  d_H2_neg_delta.Snapshot("T_neg_delta",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);
  //pos delta 
  ROOT::RDataFrame d_H2_pos_delta_raw("h10",H2_pos_delta_rootfile_name.c_str()); 
  auto d_H2_pos_delta_1 = d_H2_pos_delta_raw
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
    .Define("HMS_Dipole_Exit",Dipole_Exit_HMS,{"hsxfp","hsxpfp","hsyfp","hsypfp"})
    .Filter("HMS_Dipole_Exit>0")
    .Define("SHMS_Dipole_Exit",Dipole_Exit_SHMS,{"ssxfp","ssxpfp","ssyfp","ssypfp"})
    .Filter("SHMS_Dipole_Exit>0")
    //.Filter(HGC_bad,{"ppi","ssxfp","ssxpfp","ssyfp","ssypfp"})
    ;
  double nentries_H2_pos_delta = *d_H2_pos_delta_raw.Count();
  std::cout<<"sim counts pos delta "<<nentries_H2_pos_delta<<std::endl;
  double normfac_H2_pos_delta = j_simc[std::to_string(RunGroup).c_str()]["H2"]["pos"]["delta"]["normfac"].get<double>();
  std::cout<<"sim normfac"<<normfac_H2_pos_delta<<"neg"<<normfac_H2_neg_delta<<std::endl;
  double wfac_H2_pos_delta = (normfac_H2_pos_delta/nentries_H2_pos_delta);
  auto d_H2_pos_delta = d_H2_pos_delta_1.Define("weight_new",[wfac_H2_pos_delta](float weight){return wfac_H2_pos_delta*weight;},{"Weight"});
  d_H2_pos_delta.Snapshot("T_pos_delta",skim_name.c_str(),{"xbj","z","Q2","W2","W","Em","missmass","Mx2","Pm","weight_new","ssxptar","ssyptar","ssytar","ssdelta","ssxpfp","ssypfp","hsxptar","hsyptar","hsdelta"},opts);

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

  //double charge_H2_neg_inc_norad = 0,charge_H2_pos_inc_norad = 0;
  //
  //if(!neg_H2.empty() && !pos_H2.empty()){
  //  for(auto it = neg_H2.begin();it!=neg_H2.end();++it){
  //    int RunNumber = *it;
  //    double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
  //    charge_H2_neg_inc_norad+=charge;
  //  }
  //  for(auto it = pos_H2.begin();it!=pos_H2.end();++it){
  //    int RunNumber = *it;
  //    double charge = j_runs[std::to_string(RunNumber).c_str()]["charge"].get<double>();
  //    charge_H2_pos_inc_norad+=charge;
  //  }
  //}

  //std::cout<<"neg charge "<<charge_H2_neg_inc_norad<<" pos charge "<<charge_H2_pos_inc_norad<<std::endl;
  
  auto h_Q2_H2_neg_inc_norad = d_H2_neg_inc_norad.Histo1D({"Q2_neg_inc_norad","Q2_neg_inc_norad",bins,0,10},"Q2","weight_new");
  auto h_Q2_H2_pos_inc_norad = d_H2_pos_inc_norad.Histo1D({"Q2_pos_inc_norad","Q2_pos_inc_norad",bins,0,10},"Q2","weight_new");
  auto h_xbj_H2_neg_inc_norad = d_H2_neg_inc_norad.Histo1D({"xbj_neg_inc_norad","xbj_neg_inc_norad",bins,0,1},"xbj","weight_new");
  auto h_xbj_H2_pos_inc_norad = d_H2_pos_inc_norad.Histo1D({"xbj_pos_inc_norad","xbj_pos_inc_norad",bins,0,1},"xbj","weight_new");
  auto h_z_H2_neg_inc_norad = d_H2_neg_inc_norad.Histo1D({"z_neg_inc_norad","z_neg_inc_norad",bins,0,1},"z","weight_new");
  auto h_z_H2_pos_inc_norad = d_H2_pos_inc_norad.Histo1D({"z_pos_inc_norad","z_pos_inc_norad",bins,0,1},"z","weight_new");
  auto h_x_z_neg_inc_norad = d_H2_neg_inc_norad.Histo2D({"x_z_neg_inc_norad","x_z_neg_inc_norad",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_inc_norad = d_H2_pos_inc_norad.Histo2D({"x_z_pos_inc_norad","x_z_pos_inc_norad",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_neg_inc_norad_1 = d_H2_neg_inc_norad
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_neg_inc_norad_1","x_z_neg_inc_norad_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_inc_norad_2 = d_H2_neg_inc_norad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_inc_norad_2","x_z_neg_inc_norad_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_inc_norad_3 = d_H2_neg_inc_norad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_inc_norad_3","x_z_neg_inc_norad_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_norad_1 = d_H2_pos_inc_norad
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_pos_inc_norad_1","x_z_pos_inc_norad_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_norad_2 = d_H2_pos_inc_norad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_inc_norad_2","x_z_pos_inc_norad_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_norad_3 = d_H2_pos_inc_norad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_inc_norad_3","x_z_pos_inc_norad_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_xs_z_neg_inc_norad = d_H2_neg_inc_norad.Histo1D({"xs_z_neg_inc_norad","xs_z_neg_inc_norad",bins,0,1},"z","weight_sig");
  auto h_xs_z_pos_inc_norad = d_H2_pos_inc_norad.Histo1D({"xs_z_pos_inc_norad","xs_z_pos_inc_norad",bins,0,1},"z","weight_sig");
  auto h_xs_xbj_neg_inc_norad = d_H2_neg_inc_norad.Histo1D({"xs_xbj_neg_inc_norad","xs_xbj_neg_inc_norad",bins,0,1},"xbj","weight_sig");
  auto h_xs_xbj_pos_inc_norad = d_H2_pos_inc_norad.Histo1D({"xs_xbj_pos_inc_norad","xs_xbj_pos_inc_norad",bins,0,1},"xbj","weight_sig");
  auto h_xs_z_neg_inc_norad_raw = d_H2_neg_inc_norad.Histo1D({"xs_z_neg_inc_norad_raw","xs_z_neg_inc_norad_raw",bins,0,1},"z","weight_new");
  auto h_xs_z_pos_inc_norad_raw = d_H2_pos_inc_norad.Histo1D({"xs_z_pos_inc_norad_raw","xs_z_pos_inc_norad_raw",bins,0,1},"z","weight_new");
  auto h_xs_xbj_neg_inc_norad_raw = d_H2_neg_inc_norad.Histo1D({"xs_xbj_neg_inc_norad_raw","xs_xbj_neg_inc_norad_raw",bins,0,1},"xbj","weight_new");
  auto h_xs_xbj_pos_inc_norad_raw = d_H2_pos_inc_norad.Histo1D({"xs_xbj_pos_inc_norad_raw","xs_xbj_pos_inc_norad_raw",bins,0,1},"xbj","weight_new");
  std::cout<<"inc norad check"<<std::endl;  
  auto h_Q2_H2_pos_exc_rad = d_H2_pos_exc_rad.Histo1D({"Q2_pos_exc_rad","Q2_pos_exc_rad",bins,0,10},"Q2","weight_new");
  auto h_xbj_H2_pos_exc_rad = d_H2_pos_exc_rad.Histo1D({"xbj_pos_exc_rad","xbj_pos_exc_rad",bins,0,1},"xbj","weight_new");
  auto h_z_H2_pos_exc_rad = d_H2_pos_exc_rad.Histo1D({"z_pos_exc_rad","z_pos_exc_rad",bins,0,1},"z","weight_new");
  auto h_x_z_pos_exc_rad = d_H2_pos_exc_rad.Histo2D({"x_z_pos_exc_rad","x_z_pos_exc_rad",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_exc_rad_1 = d_H2_pos_exc_rad
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_pos_exc_rad_1","x_z_pos_exc_rad_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_exc_rad_2 = d_H2_pos_exc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_exc_rad_2","x_z_pos_exc_rad_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_exc_rad_3 = d_H2_pos_exc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_exc_rad_3","x_z_pos_exc_rad_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  std::cout<<"exc norad check"<<std::endl;  
  auto h_Q2_H2_neg_inc_rad = d_H2_neg_inc_rad.Histo1D({"Q2_neg_inc_rad","Q2_neg_inc_rad",bins,0,10},"Q2","weight_new");
  auto h_Q2_H2_pos_inc_rad = d_H2_pos_inc_rad.Histo1D({"Q2_pos_inc_rad","Q2_pos_inc_rad",bins,0,10},"Q2","weight_new");
  auto h_xbj_H2_neg_inc_rad = d_H2_neg_inc_rad.Histo1D({"xbj_neg_inc_rad","xbj_neg_inc_rad",bins,0,1},"xbj","weight_new");
  auto h_xbj_H2_pos_inc_rad = d_H2_pos_inc_rad.Histo1D({"xbj_pos_inc_rad","xbj_pos_inc_rad",bins,0,1},"xbj","weight_new");
  auto h_z_H2_neg_inc_rad = d_H2_neg_inc_rad.Histo1D({"z_neg_inc_rad","z_neg_inc_rad",bins,0,1},"z","weight_new");
  auto h_z_H2_pos_inc_rad = d_H2_pos_inc_rad.Histo1D({"z_pos_inc_rad","z_pos_inc_rad",bins,0,1},"z","weight_new");
  auto h_x_z_neg_inc_rad = d_H2_neg_inc_rad.Histo2D({"x_z_neg_inc_rad","x_z_neg_inc_rad",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_inc_rad = d_H2_pos_inc_rad.Histo2D({"x_z_pos_inc_rad","x_z_pos_inc_rad",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_neg_inc_rad_1 = d_H2_neg_inc_rad
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_neg_inc_rad_1","x_z_neg_inc_rad_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_inc_rad_2 = d_H2_neg_inc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_inc_rad_2","x_z_neg_inc_rad_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_inc_rad_3 = d_H2_neg_inc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_inc_rad_3","x_z_neg_inc_rad_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_rad_1 = d_H2_pos_inc_rad
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_pos_inc_rad_1","x_z_pos_inc_rad_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_rad_2 = d_H2_pos_inc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_inc_rad_2","x_z_pos_inc_rad_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_inc_rad_3 = d_H2_pos_inc_rad
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_inc_rad_3","x_z_pos_inc_rad_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  //auto xs_neg_inc_rad_list = d_H2_neg_inc_rad.Take<float>("siglab");
  //auto z_neg_inc_rad_list = d_H2_neg_inc_rad.Take<float>("z");
  //auto xbj_neg_inc_rad_list = d_H2_neg_inc_rad.Take<float>("xbj");
  //int nentries_neg = *d_H2_neg_inc_rad.Count();
  //TGraph* g_xs_z_neg_inc_rad = new TGraph(); 
  //TGraph* g_xs_xbj_neg_inc_rad = new TGraph(); 
  //for(int i = 0;i<nentries_neg;i++){
  //g_xs_z_neg_inc_rad->SetPoint(i,z_neg_inc_rad_list->at(i),xs_neg_inc_rad_list->at(i));
  //g_xs_xbj_neg_inc_rad->SetPoint(i,xbj_neg_inc_rad_list->at(i),xs_neg_inc_rad_list->at(i));
  //}
  //auto xs_pos_inc_rad_list = d_H2_pos_inc_rad.Take<float>("siglab");
  //auto z_pos_inc_rad_list = d_H2_pos_inc_rad.Take<float>("z");
  //auto xbj_pos_inc_rad_list = d_H2_pos_inc_rad.Take<float>("xbj");
  //int nentries_pos = *d_H2_pos_inc_rad.Count();
////  std::cout<<nentries_pos<<std::endl;
  //TGraph* g_xs_z_pos_inc_rad = new TGraph(); 
  //TGraph* g_xs_xbj_pos_inc_rad = new TGraph(); 
  //for(int i = 0;i<nentries_pos;i++){
  //g_xs_z_pos_inc_rad->SetPoint(i,z_pos_inc_rad_list->at(i),xs_pos_inc_rad_list->at(i));
  //g_xs_xbj_pos_inc_rad->SetPoint(i,xbj_pos_inc_rad_list->at(i),xs_pos_inc_rad_list->at(i));
  //}
  std::cout<<"inc rad check"<<std::endl;  

  auto h_Q2_H2_neg_rho = d_H2_neg_rho.Histo1D({"Q2_neg_rho","Q2_neg_rho",bins,0,10},"Q2","weight_new");
  auto h_Q2_H2_pos_rho = d_H2_pos_rho.Histo1D({"Q2_pos_rho","Q2_pos_rho",bins,0,10},"Q2","weight_new");
  auto h_xbj_H2_neg_rho = d_H2_neg_rho.Histo1D({"xbj_neg_rho","xbj_neg_rho",bins,0,1},"xbj","weight_new");
  auto h_xbj_H2_pos_rho = d_H2_pos_rho.Histo1D({"xbj_pos_rho","xbj_pos_rho",bins,0,1},"xbj","weight_new");
  auto h_z_H2_neg_rho = d_H2_neg_rho.Histo1D({"z_neg_rho","z_neg_rho",bins,0,1},"z","weight_new");
  auto h_z_H2_pos_rho = d_H2_pos_rho.Histo1D({"z_pos_rho","z_pos_rho",bins,0,1},"z","weight_new");
  auto h_x_z_neg_rho = d_H2_neg_rho.Histo2D({"x_z_neg_rho","x_z_neg_rho",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_rho = d_H2_pos_rho.Histo2D({"x_z_pos_rho","x_z_pos_rho",bins,0,1,bins,0,1},"z","xbj","weight_new");
  std::cout<<"rho check"<<std::endl;  
  
  auto h_Q2_H2_neg_delta = d_H2_neg_delta.Histo1D({"Q2_neg_delta","Q2_neg_delta",bins,0,10},"Q2","weight_new");
  auto h_Q2_H2_pos_delta = d_H2_pos_delta.Histo1D({"Q2_pos_delta","Q2_pos_delta",bins,0,10},"Q2","weight_new");
  auto h_xbj_H2_neg_delta = d_H2_neg_delta.Histo1D({"xbj_neg_delta","xbj_neg_delta",bins,0,1},"xbj","weight_new");
  auto h_xbj_H2_pos_delta = d_H2_pos_delta.Histo1D({"xbj_pos_delta","xbj_pos_delta",bins,0,1},"xbj","weight_new");
  auto h_z_H2_neg_delta = d_H2_neg_delta.Histo1D({"z_neg_delta","z_neg_delta",bins,0,1},"z","weight_new");
  auto h_z_H2_pos_delta = d_H2_pos_delta.Histo1D({"z_pos_delta","z_pos_delta",bins,0,1},"z","weight_new");
  auto h_x_z_neg_delta = d_H2_neg_delta.Histo2D({"x_z_neg_delta","x_z_neg_delta",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_pos_delta = d_H2_pos_delta.Histo2D({"x_z_pos_delta","x_z_pos_delta",bins,0,1,bins,0,1},"z","xbj","weight_new");
  auto h_x_z_neg_delta_1 = d_H2_neg_delta
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_neg_delta_1","x_z_neg_delta_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_delta_2 = d_H2_neg_delta
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_delta_2","x_z_neg_delta_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_neg_delta_3 = d_H2_neg_delta
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_neg_delta_3","x_z_neg_delta_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_delta_1 = d_H2_pos_delta
    .Filter(Q2_low_cut)
    .Histo2D({"x_z_pos_delta_1","x_z_pos_delta_1",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_delta_2 = d_H2_pos_delta
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_delta_2","x_z_pos_delta_2",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  auto h_x_z_pos_delta_3 = d_H2_pos_delta
    .Filter(Q2_middle_cut)
    .Histo2D({"x_z_pos_delta_3","x_z_pos_delta_3",bins,0,1,bins,0,1},"z","xbj","weight_new")
    ;
  std::cout<<"delta check"<<std::endl;  
  //std::cout<<"delta check neg "<<*d_H2_neg_delta.Count()<<"pos "<<*d_H2_pos_delta.Count()<<std::endl;  
  
  std::string rootfile_out_name = "results/yield/kinematics_yield_"+std::to_string(RunGroup)+"_H2_simc.root";
  TFile *rootfile_out = new TFile(rootfile_out_name.c_str(),"RECREATE");
  h_Q2_H2_neg_inc_norad->Write();
  h_Q2_H2_pos_inc_norad->Write();
  h_xbj_H2_neg_inc_norad->Write();
  h_xbj_H2_pos_inc_norad->Write();
  h_z_H2_neg_inc_norad->Write();
  h_z_H2_pos_inc_norad->Write();
  h_x_z_neg_inc_norad->Write();
  h_x_z_pos_inc_norad->Write();
  h_x_z_neg_inc_norad_1->Write();
  h_x_z_pos_inc_norad_1->Write();
  h_x_z_neg_inc_norad_2->Write();
  h_x_z_pos_inc_norad_2->Write();
  h_x_z_neg_inc_norad_3->Write();
  h_x_z_pos_inc_norad_3->Write();
  h_Q2_H2_pos_exc_rad->Write();
  h_xbj_H2_pos_exc_rad->Write();
  h_z_H2_pos_exc_rad->Write();
  //h_x_z_neg_exc_rad->Write();
  h_x_z_pos_exc_rad->Write();
  //h_x_z_neg_exc_rad_1->Write();
  h_x_z_pos_exc_rad_1->Write();
  //h_x_z_neg_exc_rad_2->Write();
  h_x_z_pos_exc_rad_2->Write();
  //h_x_z_neg_exc_rad_3->Write();
  h_x_z_pos_exc_rad_3->Write();
  h_Q2_H2_neg_inc_rad->Write();
  h_Q2_H2_pos_inc_rad->Write();
  h_xbj_H2_neg_inc_rad->Write();
  h_xbj_H2_pos_inc_rad->Write();
  h_z_H2_neg_inc_rad->Write();
  h_z_H2_pos_inc_rad->Write();
  h_x_z_neg_inc_rad->Write();
  h_x_z_pos_inc_rad->Write();
  h_x_z_neg_inc_rad_1->Write();
  h_x_z_pos_inc_rad_1->Write();
  h_x_z_neg_inc_rad_2->Write();
  h_x_z_pos_inc_rad_2->Write();
  h_x_z_neg_inc_rad_3->Write();
  h_x_z_pos_inc_rad_3->Write();
  h_xs_z_neg_inc_norad->Write("xs_z_neg_inc_norad");
  h_xs_z_pos_inc_norad->Write("xs_z_pos_inc_norad");
  h_xs_xbj_neg_inc_norad->Write("xs_xbj_neg_inc_norad");
  h_xs_xbj_pos_inc_norad->Write("xs_xbj_pos_inc_norad");
  h_xs_z_neg_inc_norad_raw->Write();
  h_xs_z_pos_inc_norad_raw->Write();
  h_xs_xbj_neg_inc_norad_raw->Write();
  h_xs_xbj_pos_inc_norad_raw->Write();
  h_Q2_H2_neg_rho->Write();
  h_Q2_H2_pos_rho->Write();
  h_xbj_H2_neg_rho->Write();
  h_xbj_H2_pos_rho->Write();
  h_z_H2_neg_rho->Write();
  h_z_H2_pos_rho->Write();
  h_x_z_neg_rho->Write();
  h_x_z_pos_rho->Write();
  h_Q2_H2_neg_delta->Write();
  h_Q2_H2_pos_delta->Write();
  h_xbj_H2_neg_delta->Write();
  h_xbj_H2_pos_delta->Write();
  h_z_H2_neg_delta->Write();
  h_z_H2_pos_delta->Write();
  h_x_z_neg_delta->Write();
  h_x_z_pos_delta->Write();
  h_x_z_neg_delta_1->Write();
  h_x_z_pos_delta_1->Write();
  h_x_z_neg_delta_2->Write();
  h_x_z_pos_delta_2->Write();
  h_x_z_neg_delta_3->Write();
  h_x_z_pos_delta_3->Write();

  rootfile_out->Close();

  double pos_inc_norad = h_z_H2_pos_inc_norad->Integral();
  double neg_inc_norad = h_z_H2_neg_inc_norad->Integral();
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["inc_norad"] = pos_inc_norad;
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["inc_norad"] = neg_inc_norad;
  double pos_inc_rad = h_z_H2_pos_inc_rad->Integral();
  double neg_inc_rad = h_z_H2_neg_inc_rad->Integral();
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["inc_rad"] = pos_inc_rad;
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["inc_rad"] = neg_inc_rad;
  double pos_exc_rad = h_z_H2_pos_exc_rad->Integral();
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["exc_rad"] = pos_exc_rad;
//  jout[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["exc_rad"] = neg_exc_rad;
  double pos_delta = h_z_H2_pos_delta->Integral();
  double neg_delta = h_z_H2_neg_delta->Integral();
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["delta"] = pos_delta;
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["delta"] = neg_delta;
  double pos_rho = h_z_H2_pos_rho->Integral();
  double neg_rho = h_z_H2_neg_rho->Integral();
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["rho"] = pos_rho;
  jout[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["rho"] = neg_rho;
    std::string of_name = "results/yield/run_info/simc_"+std::to_string(RunGroup)+"_info.json";
  std::ofstream ofs(of_name.c_str());
  ofs<<jout.dump(4)<<std::endl;

}
