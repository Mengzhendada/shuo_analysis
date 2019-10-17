#include "ROOT/RDataFrame.hxx"
#include "TMath.h"
#include "TVector3.h"
#include "TRotation.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

bool root_file_exists(std::string rootfile) {
  bool found_good_file = false;
  if (!gSystem->AccessPathName(rootfile.c_str())) {
    TFile file(rootfile.c_str());
    if (file.IsZombie()) {
      std::cout << rootfile << " is a zombie.\n";
      std::cout
          << " Did your replay finish?  Check that the it is done before running this script.\n";
      return false;
      // return;
    } else {
      std::cout << " using : " << rootfile << "\n";
      return true;
    }
  }
  return false;
}

//constexpr const double M_P   = 0.938272;

auto pion_momentum = [](double px,double py,double pz){
  TVector3 v(px,py,pz);
  return v;
};

void shms_angle_calib(int RunNumber = 0){
  if(RunNumber ==0){
    std::cout<<"Enter a RunNumber (-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<=0)
      return;
  }
  double shms_angle;
  if(RunNumber<7000){
    json j_fall;
    {
      std::ifstream infile("db2/run_list.json");
      infile>>j_fall;
    }
    //if(j_fall.find(std::to_string(RunNumber)){
    shms_angle = j_fall[std::to_string(RunNumber)]["spectrometers"]["shms_angle"].get<double>();
    //}
    //else{
    //  std::cout<<"couldn't find in fall json"<<std::endl;
   // }
  }
  else{
    json j_spring;
    {
      std::ifstream infile("db2/run_list_update.json");
      infile>>j_spring;
    }
    //if(j_spring.find(std::to_string(RunNumber)){
    shms_angle = j_spring[std::to_string(RunNumber)]["spectrometers"]["shms_angle"].get<double>();
    //}
    //else{
    //  std::cout<<"couldn't find in spring json"<<std::endl;
   // }
  }
  TRotation r;
  r.RotateX(shms_angle*TMath::Pi()/180);
  auto rotate = [r](TVector3 p){return r * p;};

  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d("T",rootfile_name.c_str());

// =================================================================================
// Cuts
// =================================================================================
std::string goodTrack = "P.gtr.dp > -10 && P.gtr.dp < 22 && P.tr.n == 1&&"
                        "-0.05 < P.gtr.th && P.gtr.th < 0.05 && "
                        "-0.035 < P.gtr.ph && P.gtr.ph < 0.025"
                        "&& P.gtr.y > -2.0 && P.gtr.y < 3.0";
std::string eCut = "P.cal.etottracknorm > 0.80 && P.cal.etottracknorm < 2.&&"
                   "P.ngcer.npeSum > 2.";

  std::string piCut = "P.aero.npeSum > 1.0 && P.cal.eprtracknorm < 0.2 && "
                       " P.cal.etottracknorm<1.0";
  std::string hgc_cut = " p_pion.P() < 2.8 || P.hgcer.npeSum > 1.0";

  auto d_SHMS = d.Filter("fEvtHdr.fEvtType == 2")
                 .Define("pion_momentum",pion_momentum,{"P.gtr.py","P.gtr.px","P.gtr.pz"})
                 .Define("pion_momentum_rotated",rotate,{"pion_momentum"})
                 .Define("pion_momentum_rotated_x",[](TVector3 v){return v.X();},{"pion_momentum_rotated"})
                 .Define("pion_momentum_rotated_y",[](TVector3 v){return v.Y();},{"pion_momentum_rotated"})
                 ;
  auto dGoodTrack = d_SHMS.Filter(goodTrack);
  auto dpi = dGoodTrack
  //.Filter(piCut)
  ;
  std::cout<<*dpi.Count()<<std::endl;
    auto h_pion_momentum_x = dpi.Histo1D({"pion_p_x","pion momentum x",500,-5,5},"pion_momentum_rotated_x");
    h_pion_momentum_x->Fit("gaus","0","",-5,5);
    TF1 *Fit_x = h_pion_momentum_x->GetFunction("gaus");
    double x_mean = Fit_x->GetParameter(1);
    double x_sigma = Fit_x->GetParameter(2);
    Fit_x->SetLineColor(2);
    Fit_x->SetLineWidth(1);
    Fit_x->SetLineStyle(1);
    auto h_pion_momentum_y = dpi.Histo1D({"pion_p_y","pion momentum y",500,-5,5},"pion_momentum_rotated_y");
    h_pion_momentum_y->Fit("gaus","0","",-5,5);
    TF1 *Fit_y = h_pion_momentum_y->GetFunction("gaus");
    double y_mean = Fit_y->GetParameter(1);
    double y_sigma = Fit_y->GetParameter(2);
    Fit_y->SetLineColor(2);
    Fit_y->SetLineWidth(1);
    Fit_y->SetLineStyle(1);
    auto h_pion_momentum_xy = dpi.Histo2D({"pion_p_xy","pion momentum x y",500,-5,5,500,-5,5},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    TCanvas *c_pion_momentum = new TCanvas();
    c_pion_momentum->Divide(2,2);
    c_pion_momentum->cd(1);
    h_pion_momentum_x->DrawCopy();
    Fit_x->DrawCopy("same");
    c_pion_momentum->cd(2);
    h_pion_momentum_y->DrawCopy();
    Fit_y->DrawCopy("same");
    c_pion_momentum->cd(3);
    h_pion_momentum_xy->DrawCopy();
    
    std::string pion_momentum_name = "results/csv_kin/kin_acceptance/shmssingles_acceptance"+std::to_string(RunNumber)+".pdf";
    c_pion_momentum->SaveAs(pion_momentum_name.c_str());
    
  
}
