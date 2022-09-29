#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RSnapshotOptions.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TVector3.h"
#include "Math/IFunction.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include <iostream>

const double GeV         = 1.78266192e-27;
const double c           = 299792458;
const double shms_length = 20.1;

double t_e(double p) {
  const double m = 0.00051;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_pi(double p) {
  const double m = 0.139;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_K(double p) {
  const double m = 0.493;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_proton(double p) {
  const double m = 0.938;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
void SHMS_rf_Kpi(int RunGroup = 0) {
  if (RunGroup == 0) {
    std::cout << "Enter a RunGroup (-1 to exit):";
    std::cin >> RunGroup;
    if (RunGroup <= 0)
      return;
  }

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs >> j_rungroup;
    //std::cout << "derp\n";
  }

  json j_rungroup_info;

  std::vector<int> neg_D2, pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double shms_p_central = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  double shms_p;
  json   j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs >> j_cuts;
  };
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs >> j_DE;
  }
  double      P_hgcer       = j_cuts["P_hgcer"].get<double>();
  double      P_aero      = j_cuts["P_aero"].get<double>();
  std::string aeroCutSHMS = (" P_aero_npeSum > " + std::to_string(P_aero)).c_str();
  std::string SHMS_anti_hgc_aero = ("P_hgcer_npeSum<"+std::to_string(P_hgcer)).c_str();
  auto SHMS_hgc_aero =[=](double shms_dp,double hgc_Npe){

    if((shms_dp+100)*shms_p_central/100>2.85){
      return hgc_Npe>P_hgcer;
    }
    else{
      return true;
    }
  };
  /*
     if (shms_p_central > 3) {
     SHMS_hgc_aero = aeroCutSHMS 
     +" && "+hgcCutSHMS;
     }
     else {
     SHMS_hgc_aero = aeroCutSHMS;
     }
     */

  auto rftime_K = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_K((100+SHMS_dp)*shms_p_central/100)-t_K(shms_p_central);
    return rftime+rf_shift;
  };
  auto rftime_pi = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_pi((100+SHMS_dp)*shms_p_central/100)-t_pi(shms_p_central);
    return rftime+rf_shift;
  };
  auto rftime_proton = [shms_p_central](double SHMS_dp,double rftime){
    double rf_shift = t_proton((100+SHMS_dp)*shms_p_central/100)-t_proton(shms_p_central);
    return rftime+rf_shift;
  };



  json jout;

  std::unique_ptr<TFile> fout(
      TFile::Open(string("results/pid/rf_histograms" + std::to_string(RunGroup) + "_aero" +
          std::to_string(int(P_aero))+ ".root")
        //std::to_string(int(P_aero))+ "_withhgc.root")
      .c_str(),
      "RECREATE"));

  if (!neg_D2.empty() && !pos_D2.empty()) {



    //std::cout << "check" << std::endl;



    //loop over xbin bins in x and zbin bins in z, 
    int i_xz = 0;//count which poin in (x,z), xbin*zbin
    int xbin =2;
    int zbin = 20;
    for(int i_x = 0;i_x<xbin;i_x++){
      double x_left = i_x*1.0/xbin;
      double x_right = (i_x+1)*1.0/xbin;
      double x_center = (x_left+x_right)/2;
      auto x_cut = [=](double xbj){
        return xbj>x_left && xbj<x_right;
      };
      for(int i_z = 0;i_z<zbin;i_z++){
        double z_left = i_z*1.0/zbin;
        double z_right = (i_z+1)*1.0/zbin;
        double z_center = (z_left+z_right)/2;
        auto z_cut = [=](double z){
          return z>z_left && z<z_right;
        };
        TH1D* h_rf_pos_Kall = new TH1D("", ";rftime_K;counts", 150, -1, 5.008);
        TH1D* h_rf_neg_Kall = new TH1D("", ";rftime_K;counts", 150, -1, 5.008);

        TH1D* h_rfpi_pos_Kall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
        TH1D* h_rfpi_neg_Kall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
        TH1D* h_rfproton_pos_Kall = new TH1D("", ";rftime_proton;counts", 150, -1, 5.008);
        TH1D* h_rfproton_neg_Kall = new TH1D("", ";rftime_proton;counts", 150, -1, 5.008);

        TH1D* h_rf_pos_piall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);
        TH1D* h_rf_neg_piall = new TH1D("", ";rftime_pi;counts", 150, -1, 5.008);

        std::string pos_1st_rootfile_name = "results/skim_root/" +
          std::to_string(pos_D2[0])  +
          ".root";
        ROOT::RDataFrame d_mod_1st("T", pos_1st_rootfile_name);

        auto d_pos_piall_1st = d_mod_1st
          .Filter(x_cut,{"xbj"})
          .Filter(z_cut,{"z"})
          .Filter(SHMS_hgc_aero,{"P_gtr_dp","P_hgcer_npeSum"});
        //loop over 20 bins in x and 20 bins in z, 
        double pos_counts = *d_pos_piall_1st.Count(); 

        if(pos_counts>5){
          std::cout<<" good point 1st check "<<pos_counts<<" for "<<RunGroup<<" point "<<i_xz<<" at xbj = "<<x_center<<" at z = "<<z_center<<std::endl;


          for (auto it = pos_D2.begin(); it != pos_D2.end(); ++it) {
            int RunNumber = *it;

            //std::cout << "pos data" << RunNumber << std::endl;

            std::string rootfile_name = "results/skim_root/" +
              std::to_string(RunNumber)  +
              ".root";
            ROOT::RDataFrame d_mod_first("T", rootfile_name);
            ROOT::RDataFrame d_pos_bg_norfcut("T_bg", rootfile_name);

            auto d_pos_piall = d_mod_first
              //.Filter(aeroCutSHMS)
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_hgc_aero,{"P_gtr_dp","P_hgcer_npeSum"});
            auto d_pos_Kall = d_mod_first
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_anti_hgc_aero)
              ;
            auto d_pos_piall_bg = d_pos_bg_norfcut
              //.Filter(aeroCutSHMS)
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_hgc_aero,{"P_gtr_dp","P_hgcer_npeSum"});
            auto d_pos_Kall_bg = d_pos_bg_norfcut
              //.Filter("P.aero.npeSum<10")
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_anti_hgc_aero)
              ;


            // statistics for DE efficiency
            auto d_pos_piall_i = d_pos_piall.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_pos_piall_bg_i = d_pos_piall_bg.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_pos_Kall_i = d_pos_Kall.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_pos_Kall_bg_i = d_pos_Kall_bg.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});

            auto h_rf_pos_pi = d_pos_piall_i.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rf_pos_bg =
              d_pos_piall_bg_i.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rf_pos_K =
              d_pos_Kall_i.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_k");
            auto h_rf_pos_K_bg =
              d_pos_Kall_bg_i.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_k");

            //just for fun
            auto h_rfpi_pos_K =
              d_pos_Kall_i.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rfpi_pos_K_bg =
              d_pos_Kall_bg_i.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rfproton_pos_K =
              d_pos_Kall_i.Histo1D({"", "pos,rftime,norfcut", 150, -1, 5.008}, "rf_proton");
            auto h_rfproton_pos_K_bg =
              d_pos_Kall_bg_i.Histo1D({"", "pos,cal,norfcut", 150, -1, 5.008}, "rf_proton");

            h_rf_pos_pi->Add(h_rf_pos_bg.GetPtr(),-1/6);
            h_rf_pos_piall->Add(h_rf_pos_pi.GetPtr(),1);
            h_rf_pos_K->Add(h_rf_pos_K_bg.GetPtr(),-1/6);
            h_rf_pos_Kall->Add(h_rf_pos_K.GetPtr(),1);
            h_rfpi_pos_K->Add(h_rfpi_pos_K_bg.GetPtr(),-1/6);
            h_rfpi_pos_Kall->Add(h_rfpi_pos_K.GetPtr(),1);
            h_rfproton_pos_K->Add(h_rfproton_pos_K_bg.GetPtr(),-1/6);
            h_rfproton_pos_Kall->Add(h_rfproton_pos_K.GetPtr(),1);

            double pos_piall = *d_pos_piall_i.Count();
            double pos_piall_bg = *d_pos_piall_bg_i.Count();
            double pos_Kall = *d_pos_Kall_i.Count();
            double pos_Kall_bg = *d_pos_Kall_bg_i.Count();

            double shms_p_i = *d_pos_piall_i.Mean("shms_p");
            double shms_dp_i = *d_pos_piall_i.Mean("P_gtr_dp");
            double xbj_i = *d_pos_piall_i.Mean("xbj");
            double z_i = *d_pos_piall_i.Mean("z");
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["pos_piall"] = pos_piall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["piall"] = pos_piall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["pos_piall_bg"] = pos_piall_bg;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["pos_Kall"] = pos_Kall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["pos_Kall_bg"] = pos_Kall_bg;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["x_center"] = x_center;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["z_center"] = z_center;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["shms_p"] = shms_p_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["shms_dp"] = shms_dp_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["xbj"] = xbj_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["z"] = z_i;


          }//for pos runs

          // for neg runs
          for (auto it = neg_D2.begin(); it != neg_D2.end(); ++it) {
            int RunNumber = *it;


            //std::cout << "neg data" << RunNumber << std::endl;

            std::string rootfile_name = "results/skim_root/" +
              std::to_string(RunNumber)  +
              ".root";
            ROOT::RDataFrame d_mod_first("T", rootfile_name);
            ROOT::RDataFrame d_neg_bg_norfcut("T_bg", rootfile_name);

            auto d_neg_piall = d_mod_first
              //.Filter(aeroCutSHMS)
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_hgc_aero,{"P_gtr_dp","P_hgcer_npeSum"});
            auto d_neg_Kall = d_mod_first
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_anti_hgc_aero)
              ;
            auto d_neg_piall_bg = d_neg_bg_norfcut
              //.Filter(aeroCutSHMS)
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_hgc_aero,{"P_gtr_dp","P_hgcer_npeSum"});
            auto d_neg_Kall_bg = d_neg_bg_norfcut
              //.Filter("P.aero.npeSum<10")
              //.Filter(SHMS_p_cut,{"P.gtr.dp"})
              .Define("rf_k",rftime_K,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_proton",rftime_proton,{"P_gtr_dp","diff_time_mod"}) 
              .Define("rf_pi",rftime_pi,{"P_gtr_dp","diff_time_mod"}) 
              .Filter(SHMS_anti_hgc_aero)
              ;


            // statistics for DE efficiency
            auto d_neg_piall_i = d_neg_piall.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_neg_piall_bg_i = d_neg_piall_bg.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_neg_Kall_i = d_neg_Kall.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});
            auto d_neg_Kall_bg_i = d_neg_Kall_bg.Filter(x_cut,{"xbj"}).Filter(z_cut,{"z"});

            auto h_rf_neg_pi = d_neg_piall_i.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rf_neg_bg =
              d_neg_piall_bg_i.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rf_neg_K =
              d_neg_Kall_i.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_k");
            auto h_rf_neg_K_bg =
              d_neg_Kall_bg_i.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_k");

            //just for fun
            auto h_rfpi_neg_K =
              d_neg_Kall_i.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rfpi_neg_K_bg =
              d_neg_Kall_bg_i.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_pi");
            auto h_rfproton_neg_K =
              d_neg_Kall_i.Histo1D({"", "neg,rftime,norfcut", 150, -1, 5.008}, "rf_proton");
            auto h_rfproton_neg_K_bg =
              d_neg_Kall_bg_i.Histo1D({"", "neg,cal,norfcut", 150, -1, 5.008}, "rf_proton");

            h_rf_neg_pi->Add(h_rf_neg_bg.GetPtr(),-1/6);
            h_rf_neg_piall->Add(h_rf_neg_pi.GetPtr(),1);
            h_rf_neg_K->Add(h_rf_neg_K_bg.GetPtr(),-1/6);
            h_rf_neg_Kall->Add(h_rf_neg_K.GetPtr(),1);
            h_rfpi_neg_K->Add(h_rfpi_neg_K_bg.GetPtr(),-1/6);
            h_rfpi_neg_Kall->Add(h_rfpi_neg_K.GetPtr(),1);
            h_rfproton_neg_K->Add(h_rfproton_neg_K_bg.GetPtr(),-1/6);
            h_rfproton_neg_Kall->Add(h_rfproton_neg_K.GetPtr(),1);

            double neg_piall = *d_neg_piall_i.Count();
            double neg_piall_bg = *d_neg_piall_bg_i.Count();
            double neg_Kall = *d_neg_Kall_i.Count();
            double neg_Kall_bg = *d_neg_Kall_bg_i.Count();
            
            double shms_p_i = *d_neg_piall_i.Mean("shms_p");
            double shms_dp_i = *d_neg_piall_i.Mean("P_gtr_dp");
            double xbj_i = *d_neg_piall_i.Mean("xbj");
            double z_i = *d_neg_piall_i.Mean("z");

            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["neg_piall"] = neg_piall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["piall"] = neg_piall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["neg_piall_bg"] = neg_piall_bg;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["neg_Kall"] = neg_Kall;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["neg_Kall_bg"] = neg_Kall_bg;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["x_center"] = x_center;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["z_center"] = z_center;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["shms_p"] = shms_p_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["shms_dp"] = shms_dp_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["xbj"] = xbj_i;
            jout[(std::to_string(RunGroup)).c_str()][(std::to_string(i_xz)).c_str()][(std::to_string(RunNumber)).c_str()]["z"] = z_i;



          }//for neg runs
        }//if counts greater than 5

        fout->cd();
        h_rf_pos_piall->Write(std::string("rftime_pos_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rf_pos_Kall->Write(std::string("rftime_pos_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rfpi_pos_Kall->Write(std::string("rftimepi_pos_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rfproton_pos_Kall->Write(std::string("rftimeproton_pos_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rf_neg_piall->Write(std::string("rftime_neg_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rf_neg_Kall->Write(std::string("rftime_neg_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rfpi_neg_Kall->Write(std::string("rftimepi_neg_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        h_rfproton_neg_Kall->Write(std::string("rftimeproton_neg_K_" + std::to_string(RunGroup) + "_" + std::to_string(i_xz)).c_str());
        i_xz++;
      }//loop over z bins
    }//loop over x bins
  std::cout<<" i_xz is "<<i_xz<<std::endl;
  }   // if normal production runs
        std::string ofs_name = "results/pid/rftime_new/"+std::to_string(RunGroup)+".json";
        std::ofstream ofs(ofs_name.c_str());
        ofs<<jout.dump(4)<<std::endl;
}
