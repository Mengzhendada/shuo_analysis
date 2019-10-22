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
auto electron_momentum = [](double px, double py, double pz){
  TVector3 v(px, py, pz);
  return v;
};
auto q_momentum = [double beam_energy](TVector3 v_electron_prime){
  TVector3 v_0(0,0,beam_energy);
  return v_0 - v_electron_prime;
};

void shms_angle_calib(int RunNumber = 0){
  if(RunNumber ==0){
    std::cout<<"Enter a RunNumber (-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<=0)
      return;
  }
  double shms_angle;
  json j;
  std::string RunNumber_str = std::to_string(RunNumber);
  if(RunNumber<7000){
      std::ifstream infile("db2/run_list.json");
      infile>>j;
    if(j.find(RunNumber_str)==j.end()){
      std::cout<<"couldn't find in fall json"<<std::endl;
    }
    else{
    shms_angle = j[RunNumber_str]["spectrometers"]["shms_angle"].get<double>();
    }
  }
  else{
      std::ifstream infile("db2/run_list_update.json");
      infile>>j;
    if(j.find(RunNumber_str)==j.end()){
      std::cout<<"couldn't find in spring json"<<std::endl;
    }
    else{
    shms_angle = j[RunNumber_str]["spectrometers"]["shms_angle"].get<double>();
    }
  }
  
  //prescale
  int ps1 = -1;
  int ps2 = -1;
  int ps5 = -1;
  int ps6 = -1;
  bool singles_trigger = true;
  if(j[RunNumber_str].find("daq") != j[RunNumber_str].end()){
    ps1 = j[RunNumber_str]["daq"]["ps1"].get<int>();
    ps2 = j[RunNumber_str]["daq"]["ps2"].get<int>();
    ps5 = j[RunNumber_str]["daq"]["ps5"].get<int>();
    ps6 = j[RunNumber_str]["daq"]["ps6"].get<int>();
  }  
  if(ps1 == -1){
    std::cout<<"No shms singles for run "<<RunNumber_str<<"!, use coin type instead"<<std::endl;
    singles_trigger = false;
  }

  std::cout<<"shms angle"<<shms_angle<<std::endl;
  TRotation r;
  r.RotateZ(TMath::Pi()/2);
  r.RotateX(shms_angle*TMath::Pi()/180);
  auto rotate = [r](TVector3 p){return r * p;};

  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+RunNumber_str+"_"+RunNumber_str+".root";
  if(!root_file_exists(rootfile_name.c_str())){
    std::cout<<"Rootfile doesn't exists!"<<std::endl;
  }
 
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

  auto d_SHMS = d.Filter(singles_trigger ? "fEvtHdr.fEvtType == 1" : "fEvtHdr.fEvtType == 4")
                 .Define("pion_momentum",pion_momentum,{"P.gtr.px","P.gtr.py","P.gtr.pz"})
                 .Define("pion_momentum_rotated",rotate,{"pion_momentum"})
                 .Define("pion_momentum_rotated_x",[](TVector3 v){return v.X();},{"pion_momentum_rotated"})
                 .Define("pion_momentum_rotated_y",[](TVector3 v){return v.Y();},{"pion_momentum_rotated"})
                 ;
  auto dGoodTrack = d_SHMS
                    .Filter(goodTrack)
                    ;
  auto dpi = dGoodTrack
  //.Filter(piCut)
  ;
  std::cout<<*dpi.Count()<<std::endl;
    auto h_pion_momentum_x = dpi.Histo1D({"pion_p_x","pion momentum x",500,-1,1},"pion_momentum_rotated_x");
    h_pion_momentum_x->Fit("gaus","0","",-1,1);
    TF1 *Fit_x = h_pion_momentum_x->GetFunction("gaus");
    double x_mean = Fit_x->GetParameter(1);
    double x_sigma = Fit_x->GetParameter(2);
    Fit_x->SetLineColor(2);
    Fit_x->SetLineWidth(1);
    Fit_x->SetLineStyle(1);
    auto h_pion_momentum_y = dpi.Histo1D({"pion_p_y","pion momentum y",500,-1,1},"pion_momentum_rotated_y");
    h_pion_momentum_y->Fit("gaus","0","",-1,1);
    TF1 *Fit_y = h_pion_momentum_y->GetFunction("gaus");
    double y_mean = Fit_y->GetParameter(1);
    double y_sigma = Fit_y->GetParameter(2);
    Fit_y->SetLineColor(2);
    Fit_y->SetLineWidth(1);
    Fit_y->SetLineStyle(1);
    auto h_pion_momentum_xy = dpi.Histo2D({"pion_p_xy","pion momentum x y",500,-1,1,500,-1,1},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    
    auto h_y_angle = dpi.Histo2D({"","pion momentum y vs. angle ;angle;py",100,-0.1,0.1,100,-1,1},"P.gtr.ph","pion_momentum_rotated_y");
    auto h_y_dp = dpi.Histo2D({"","pion momentum y vs. dp ;dp;py",100,-11,23,100,-1,1},"P.gtr.dp","pion_momentum_rotated_y");

    TCanvas *c_xy = new TCanvas();
    c_xy->Divide(1,2);
    c_xy->cd(1);
    h_pion_momentum_x->DrawCopy();
    Fit_x->DrawCopy("same");
    c_xy->cd(2);
    h_pion_momentum_y->DrawCopy();
    Fit_y->DrawCopy("same");
    std::string xy_name = "results/csv_kin/kin_acceptance/shmssingles_xy_"+RunNumber_str+".pdf";
    c_xy->SaveAs(xy_name.c_str());
    
    TCanvas *c_pion_momentum = new TCanvas();
    c_pion_momentum->Divide(2,2);
    c_pion_momentum->cd(1);
    //h_pion_momentum_x->DrawCopy();
    //Fit_x->DrawCopy("same");
    //c_pion_momentum->cd(2);
    h_pion_momentum_y->DrawCopy();
    Fit_y->DrawCopy("same");
    c_pion_momentum->cd(2);
    h_pion_momentum_xy->DrawCopy("colz");
    c_pion_momentum->cd(3);
    h_y_angle->DrawCopy("colz");
    c_pion_momentum->cd(4);
    h_y_dp->DrawCopy("colz");
    
    std::string pion_momentum_name = "results/csv_kin/kin_acceptance/shmssingles_acceptance"+RunNumber_str+".pdf";
    c_pion_momentum->SaveAs(pion_momentum_name.c_str());
    
  int ps4 = -1;
  singles_trigger = false;
  if(j[RunNumber_str].find("daq") != j[RunNumber_str].end()){
    ps4 = j[RunNumber_str]["daq"]["ps4"].get<int>();
  }  
  if(ps4 == -1){
    std::cout<<"No shms coin for run "<<RunNumber_str<<"!, use single type instead"<<std::endl;
    singles_trigger = true;
  }
  auto d_SHMS_coin = d.Filter(singles_trigger ? "fEvtHdr.fEvtType == 1" : "fEvtHdr.fEvtType == 4")
                 .Define("pion_momentum",pion_momentum,{"P.gtr.px","P.gtr.py","P.gtr.pz"})
                 .Define("pion_momentum_rotated",rotate,{"pion_momentum"})
                 .Define("pion_momentum_rotated_x",[](TVector3 v){return v.X();},{"pion_momentum_rotated"})
                 .Define("pion_momentum_rotated_y",[](TVector3 v){return v.Y();},{"pion_momentum_rotated"})
                 ;
  auto dGoodTrack_coin = d_SHMS_coin
                    .Filter(goodTrack)
                    ;
  auto dpi_coin = dGoodTrack_coin
  //.Filter(piCut)
  ;
  std::cout<<*dpi_coin.Count()<<std::endl;
    auto h_pion_momentum_x_coin = dpi_coin.Histo1D({"pion_p_x","pion momentum x coin",500,-1,1},"pion_momentum_rotated_x");
    h_pion_momentum_x_coin->Fit("gaus","0","",-1,1);
    TF1 *Fit_x_coin = h_pion_momentum_x_coin->GetFunction("gaus");
    double x_mean_coin = Fit_x_coin->GetParameter(1);
    double x_sigma_coin = Fit_x_coin->GetParameter(2);
    Fit_x_coin->SetLineColor(2);
    Fit_x_coin->SetLineWidth(1);
    Fit_x_coin->SetLineStyle(1);
    auto h_pion_momentum_y_coin = dpi_coin.Histo1D({"pion_p_y","pion momentum y coin",500,-1,1},"pion_momentum_rotated_y");
    h_pion_momentum_y_coin->Fit("gaus","0","",-1,1);
    TF1 *Fit_y_coin = h_pion_momentum_y_coin->GetFunction("gaus");
    double y_mean_coin = Fit_y_coin->GetParameter(1);
    double y_sigma_coin = Fit_y_coin->GetParameter(2);
    Fit_y_coin->SetLineColor(2);
    Fit_y_coin->SetLineWidth(1);
    Fit_y_coin->SetLineStyle(1);
    auto h_pion_momentum_xy_coin = dpi_coin.Histo2D({"pion_p_xy","pion momentum x y coin",500,-1,1,500,-1,1},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    
    auto h_y_angle_coin = dpi_coin.Histo2D({"","pion momentum y vs. angle coin;angle;py",100,-0.1,0.1,100,-1,1},"P.gtr.ph","pion_momentum_rotated_y");
    auto h_y_dp_coin = dpi_coin.Histo2D({"","pion momentum y vs. dp coin;dp;py",100,-11,23,100,-1,1},"P.gtr.dp","pion_momentum_rotated_y");

    TCanvas *c_xy_coin = new TCanvas();
    c_xy_coin->Divide(1,2);
    c_xy_coin->cd(1);
    h_pion_momentum_x_coin->DrawCopy();
    Fit_x_coin->DrawCopy("same");
    c_xy_coin->cd(2);
    h_pion_momentum_y_coin->DrawCopy();
    Fit_y_coin->DrawCopy("same");
    std::string xy_name_coin = "results/csv_kin/kin_acceptance/shms_coin_xy_"+RunNumber_str+".pdf";
    c_xy_coin->SaveAs(xy_name_coin.c_str());
    
    TCanvas *c_pion_momentum_coin = new TCanvas();
    c_pion_momentum_coin->Divide(2,2);
    c_pion_momentum_coin->cd(1);
    //h_pion_momentum_x->DrawCopy();
    //Fit_x->DrawCopy("same");
    //c_pion_momentum->cd(2);
    h_pion_momentum_y_coin->DrawCopy();
    Fit_y_coin->DrawCopy("same");
    c_pion_momentum_coin->cd(2);
    h_pion_momentum_xy_coin->DrawCopy("colz");
    c_pion_momentum_coin->cd(3);
    h_y_angle_coin->DrawCopy("colz");
    c_pion_momentum_coin->cd(4);
    h_y_dp_coin->DrawCopy("colz");
    
    std::string pion_momentum_name_coin = "results/csv_kin/kin_acceptance/shms_coin_acceptance"+RunNumber_str+".pdf";
    c_pion_momentum_coin->SaveAs(pion_momentum_name_coin.c_str());
  
}
