#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TLine.h"
#include "TEllipse.h"
#include "TFile.h"
#include "TBox.h"
#include "TRotation.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TMath.h"
#include "TVector3.h"
#include "TRatioPlot.h"
#include "TChain.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

constexpr const double M_P     = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
//double Eb = 10.6;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

auto p_proton = [](double px, double py, double pz){
  return Pvec4D{px , py , pz , M_P};
};
auto p_pion = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_pion};
};
auto pion_momentum = [](double px,double py,double pz){
  TVector3 v(px,py,pz);
  return v;
};
auto p_electron = [](double px, double py, double pz) {
  return Pvec4D{px , py , pz , M_e};
};
auto p_q = [](Pvec4D& pe ) {
  return Pvec4D{0.0,0.0,10.6, M_e}-pe;
};
auto t = [](const double Egamma, Pvec4D& jpsi) {
  Pvec4D beam{0, 0, Egamma, 0};
  return (beam - jpsi).M2();
};
auto z = [](Pvec4D& pq, Pvec4D& ph) {
  return ph.E()/pq.E();
};
auto xbj = [](double Q2,Pvec4D& pq) {
  return Q2/(2.0*0.938*pq.E());
};
auto Q2 = [](Pvec4D& pq) {
  return -1.0*(pq.Dot(pq));
};
auto Wprime2 = [](Pvec4D& pq,Pvec4D& ph) {
  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq - ph;
  return Ptot.Dot(Ptot);
};
auto W2 = [](Pvec4D& pq) {
  auto Ptot = Pvec4D{0.0,0.0,0.0, M_P} + pq;
  return Ptot.Dot(Ptot);
};

void kin_acceptance(int Rungroup = 0){
  
  if(Rungroup == 0){
    std::cout<<"Enter a Rungroup (-1 to exit):";
    std::cin>>Rungroup;
    if(Rungroup<=0)
      return;
  }
  
  json j_runs;
  {
    std::ifstream runs("db2/ratio_run_group_updated.json");
    runs>>j_runs;
  }
  std::vector neg_runs = j_runs[std::to_string(Rungroup)]["neg"]["D2"].get<std::vector<int>>();
  std::vector pos_runs = j_runs[std::to_string(Rungroup)]["pos"]["D2"].get<std::vector<int>>();
  
  int RunNumber;
  if(!neg_runs.empty()){
    RunNumber = neg_runs[0];
  }
  else{
    RunNumber = pos_runs[0];
  }

  int coolcolor[5] = {4,3,7,38,39};
  int warmcolor[5] = {2,6,46,45,44};

    //for different r 
    double n1 = 1;
    double n2 = 2;
    double n3 = 3;
    double r1square,r2square,r3square;//pxpycuts radias2 for this run group from neg runs

      json j_run_info;
      {
        std::ifstream run_info("db2/run_info_group.json");
        run_info>>j_run_info;
      }

      double x_max,x_min,q2_max,q2_min;
      double x_run_plan = j_run_info[std::to_string(RunNumber)]["x"].get<double>();
      double q2_run_plan = j_run_info[std::to_string(RunNumber)]["Q2"].get<double>();
      if(x_run_plan!=0 && q2_run_plan!=0){
        x_max = x_run_plan+0.07;
        x_min = x_run_plan-0.07;
        q2_max = q2_run_plan+0.5;
        q2_min = q2_run_plan-0.5;
      }

      double shms_angle;
      if(RunNumber <7000){
        json j_fall;
        {
          std::ifstream infile("db2/run_list.json");
          infile>>j_fall;
        }
        shms_angle = j_fall[std::to_string(RunNumber)]["spectrometers"]["shms_angle"].get<double>();
      }
      else{
        json j_spring;
        {
          std::ifstream infile("db2/run_list_update.json");
          infile>>j_spring;
        }
        shms_angle = j_spring[std::to_string(RunNumber)]["spectrometers"]["shms_angle"].get<double>();
      }
      TRotation r;
      r.RotateZ(TMath::Pi()/2);
      r.RotateX(shms_angle*TMath::Pi()/180);
      auto rotate = [r](TVector3 p){return r * p;};
  
      double neg_charge = 0;
      //for all neg runs
      TChain chain("T");
      //loop over all neg runs in this run group 
    for(std::vector<int>::iterator it = neg_runs.begin();it!= neg_runs.end();++it){
      int RunNumber = *it;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_sh("TSP",rootfile_name);
      auto charge = d_sh.Max("P.BCM4B.scalerChargeCut");
      neg_charge += *charge;
      std::cout<<"RunNumber "<<RunNumber<<" charge "<<*charge<<std::endl;
      //TFile* rootfile = new TFile(rootfile_name.c_str());
      chain.Add(rootfile_name.c_str());    
    }//end of neg runs loop
      //dataframe for the chain of this run group 
      ROOT::RDataFrame d(chain);
      //ROOT::RDataFrame d_sh("TSP", chain);
      std::cout<<"charge for pi- runs "<<neg_charge<<std::endl;

      auto d_coin = d.Filter("fEvtHdr.fEvtType == 4");
      auto d_shmssingles = d.Filter("fEvtHdr.fEvtType == 1");
      //  auto d_coin = d;
      //cuts
      std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
      std::string goodTrackHMS = "H.gtr.dp>-8 && H.gtr.dp<8";
      // std::string piCerSHMS = "P.ngcer.npeSum<0.5 && P.aero.npeSum>1.0";
      std::string piCerSHMS = "P.aero.npeSum>1.0";
      std::string piCalSHMS = "P.cal.etottracknorm <0.4 && P.cal.eprtracknorm<0.2";
      std::string piCutSHMS = piCerSHMS +" && "+ piCalSHMS;
      std::string eCutHMS = "H.cal.etottracknorm >0.80 && H.cal.etottracknorm<2 && H.cer.npeSum>1";
      auto dHMSGoodTrack = d_coin.Filter(goodTrackHMS);
      auto dSHMSGoodTrack = d_coin.Filter(goodTrackSHMS);
      auto dCoinGoodTrack = dHMSGoodTrack.Filter(goodTrackSHMS)
      .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
      .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
      .Define("pion_momentum",pion_momentum,{"P.gtr.px","P.gtr.py","P.gtr.pz"})
      .Define("pion_momentum_rotated",rotate,{"pion_momentum"})
      .Define("pion_momentum_rotated_x",[](TVector3 v){return v.X();},{"pion_momentum_rotated"})
      .Define("pion_momentum_rotated_y",[](TVector3 v){return v.Y();},{"pion_momentum_rotated"})
      .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_q", p_q, {"p_electron"})
      .Define("z", z, {"p_q","p_pion"})
      .Define("Q2", Q2, {"p_q"})
      .Define("xbj", xbj, {"Q2","p_q"})
      .Define("W2",W2,{"p_q"})
      .Define("Wp2",Wprime2,{"p_q","p_pion"})
      .Define("W","std::sqrt(W2)")
      .Define("Wp","std::sqrt(Wp2)")
      ;
      auto dHMS_electron = dCoinGoodTrack.Filter(eCutHMS);
      auto dSHMS_pion = dCoinGoodTrack.Filter(piCutSHMS);
      auto dCOIN_sidis = dCoinGoodTrack.Filter(eCutHMS + " && " + piCutSHMS);
      std::cout<<*dCOIN_sidis.Count()<<std::endl; 
      // auto dxq2cut = dCOIN_sidis.Filter(xq2_cut,{"xbj","Q2"});

      auto dxq2cut = dCOIN_sidis;

      //  auto dxq2cut = dCOIN_sidis.Filter([x_min](double x){return x>x_min;},{"xbj"})
      //                            .Filter([x_max](double x){return x<x_max;},{"xbj"})
      //                            .Filter([q2_min](double q2){return q2>q2_min;},{"Q2"})
      //                            .Filter([q2_max](double q2){return q2<q2_max;},{"Q2"})
      //                            ; 
      auto h_xq2 = dCOIN_sidis.Histo2D({"x_Q2","x_Q2",400,0,1,400,0,10},"xbj","Q2");
      //    TBox* xq2_box = new TBox(x_min,q2_min,x_max,q2_max);
      TCanvas* c_xq2 = new TCanvas("x_Q2");
      h_xq2->DrawCopy("colz");
      //    xq2_box->SetFillStyle(0);
      //    xq2_box->SetLineColor(kRed);
      //    xq2_box->Draw("l");
      std::string xq2filename = "results/csv_kin/kin_acceptance/x_Q2_neg_"+std::to_string(Rungroup)+".png";
      c_xq2->SaveAs(xq2filename.c_str());

    // auto dCOIN_sidis_xq2cut = dCOIN_sidis.Filter("xbj>0.4").Filter("Q2>3.5");
 // auto h_x_q2 = dCOIN_sidis_xq2cut.Histo2D({"xQ2","xQ2with cut",400,0,1,400,0,10},"xbj","Q2");
 // TCanvas *c_x_q2 = new TCanvas();
 // h_x_q2->DrawCopy("colz");

    std::cout<<*dxq2cut.Count()<<std::endl;
 // auto h_x = dxq2cut.Histo1D({"x_fp","x",400,-50,50},"P.dc.x_fp");
 // auto h_y = dxq2cut.Histo1D({"y_fp","y",400,-50,50},"P.dc.y_fp");
 // auto h_x_y = dxq2cut.Histo2D({"xy_fp","xy_fp",400,-50,50,400,-50,50},"P.dc.x_fp","P.dc.y_fp");
 // TCanvas* c_shms = new TCanvas();
 // c_shms->Divide(2,2);
 // c_shms->cd(1);
 // h_x->DrawCopy();
 // c_shms->cd(2);
 // h_x->DrawCopy();
 // c_shms->cd(3);
 // h_x_y->DrawCopy();
 // std::string shms_filename = "results/csv_kin/kin_acceptance/shms_"+std::to_string(RunNumber)+".png";
 // c_shms->SaveAs(shms_filename.c_str());
 // 
 // auto h_xp = dxq2cut.Histo1D({"x_fp","x",400,-0.1,0.1},"P.dc.xp_fp");
 // auto h_yp = dxq2cut.Histo1D({"y_fp","y",400,-0.1,0.1},"P.dc.yp_fp");
 // auto h_xp_yp = dxq2cut.Histo2D({"xy_fp","xy_fp",400,-0.1,0.1,400,-0.1,0.1},"P.dc.xp_fp","P.dc.yp_fp");
 // TCanvas* c_shms_prime = new TCanvas();
 // c_shms_prime->Divide(2,2);
 // c_shms_prime->cd(1);
 // h_xp->DrawCopy();
 // c_shms_prime->cd(2);
 // h_xp->DrawCopy();
 // c_shms_prime->cd(3);
 // h_xp_yp->DrawCopy();
 // std::string shms_prime_filename = "results/csv_kin/kin_acceptance/shms_prime_"+std::to_string(RunNumber)+".png";
 // c_shms_prime->SaveAs(shms_prime_filename.c_str());

    //pxpy
    auto h_pion_momentum_x = dxq2cut.Histo1D({"pion_p_x","pion momentum x",500,-2,2},"pion_momentum_rotated_x");
    h_pion_momentum_x->Fit("gaus","0","",-2,2);
    TF1 *Fit_x = h_pion_momentum_x->GetFunction("gaus");
    double x_mean = Fit_x->GetParameter(1);
    double x_sigma = Fit_x->GetParameter(2);
    Fit_x->SetLineColor(2);
    Fit_x->SetLineWidth(1);
    Fit_x->SetLineStyle(1);
    auto h_pion_momentum_y = dxq2cut.Histo1D({"pion_p_y","pion momentum y",500,-2,2},"pion_momentum_rotated_y");
    h_pion_momentum_y->Fit("gaus","0","",-2,2);
    TF1 *Fit_y = h_pion_momentum_y->GetFunction("gaus");
    double y_mean = Fit_y->GetParameter(1);
    double y_sigma = Fit_y->GetParameter(2);
    Fit_y->SetLineColor(2);
    Fit_y->SetLineWidth(1);
    Fit_y->SetLineStyle(1);
    auto h_pion_momentum_xy = dxq2cut.Histo2D({"pion_p_xy","pion momentum x y",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    TCanvas *c_pion_momentum = new TCanvas();
    c_pion_momentum->Divide(2,2);
    c_pion_momentum->cd(1);
    h_pion_momentum_x->DrawCopy();
    Fit_x->DrawCopy("same");
    c_pion_momentum->cd(2);
    h_pion_momentum_y->DrawCopy();
    Fit_y->DrawCopy("same");
    c_pion_momentum->cd(3);
    h_pion_momentum_xy->DrawCopy("colz");
    
    std::string pion_momentum_name = "results/csv_kin/kin_acceptance/kin_neg_acceptance_"+std::to_string(Rungroup)+".png";
    c_pion_momentum->SaveAs(pion_momentum_name.c_str());
  
    
    r1square = n1*n1*x_sigma*x_sigma+n1*n1*y_sigma*y_sigma;
    r2square = n2*n2*x_sigma*x_sigma+n2*n2*y_sigma*y_sigma;
    r3square = n3*n3*x_sigma*x_sigma+n3*n3*y_sigma*y_sigma;
    std::string param_file_name = "results/csv_kin/kin_acceptance/neg_acceptance_param_"+std::to_string(Rungroup)+".txt";
    std::ofstream param_file(param_file_name.c_str());
    param_file<<x_mean<<"    #"<<std::to_string(RunNumber)<<" x mean position"<<std::endl;
    param_file<<y_mean<<"    #"<<std::to_string(RunNumber)<<" y mean position"<<std::endl;
    param_file<<r1square<<"     #"<<std::to_string(RunNumber)<<" r1square for cut 1"<<std::endl;
    param_file<<r2square<<"     #"<<std::to_string(RunNumber)<<" r2square for cut 2"<<std::endl;
    param_file<<r3square<<"     #"<<std::to_string(RunNumber)<<" r3square for cut 3"<<std::endl;
    
    auto cut1 = [x_mean,y_mean,r1square](double p_x, double p_y){return (p_x-x_mean)*(p_x-x_mean)+(p_y-y_mean)*(p_y-y_mean)<r1square;};
    auto cut2 = [x_mean,y_mean,r2square](double p_x, double p_y){return (p_x-x_mean)*(p_x-x_mean)+(p_y-y_mean)*(p_y-y_mean)<r2square;};
    auto cut3 = [x_mean,y_mean,r3square](double p_x, double p_y){return (p_x-x_mean)*(p_x-x_mean)+(p_y-y_mean)*(p_y-y_mean)<r3square;};
  
    auto d_cut1 = dxq2cut.Filter(cut1,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});
    auto d_cut2 = dxq2cut.Filter(cut2,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});
    auto d_cut3 = dxq2cut.Filter(cut3,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});

    auto h_pion_cut1 = d_cut1.Histo2D({"pion_p_xy","pion momentum xy cut1",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    auto h_pion_cut2 = d_cut2.Histo2D({"pion_p_xy","pion momentum xy cut2",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    auto h_pion_cut3 = d_cut3.Histo2D({"pion_p_xy","pion momentum xy cut3",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    TCanvas* c_xy_cuts = new TCanvas();
    h_pion_momentum_xy->SetMarkerStyle(7);
    h_pion_momentum_xy->SetMarkerColor(coolcolor[0]);
    h_pion_momentum_xy->DrawCopy();
    h_pion_cut3->SetMarkerStyle(7);
    h_pion_cut3->SetMarkerColor(coolcolor[3]);
    h_pion_cut3->DrawCopy("p same");
    h_pion_cut2->SetMarkerStyle(7);
    h_pion_cut2->SetMarkerColor(coolcolor[2]);
    h_pion_cut2->DrawCopy("p same");
    h_pion_cut1->SetMarkerStyle(7);
    h_pion_cut1->SetMarkerColor(coolcolor[1]);
    h_pion_cut1->DrawCopy("p same");
    std::string pion_cut_name = "results/csv_kin/kin_acceptance/kin_neg_acceptance_cuts"+std::to_string(Rungroup)+".png";
    c_xy_cuts->BuildLegend();
    c_xy_cuts->SaveAs(pion_cut_name.c_str());

    auto h_xbj_xq2cut = dxq2cut.Histo1D({"xbj_xq2cut","xfor selected x_q2 region",100,0,1},"xbj");
    auto h_q2_xq2cut = dxq2cut.Histo1D({"q2_xq2cut","Q2 for selected x_q2 region",100,0,10},"Q2");
    auto h_Wprime_xq2cut = dxq2cut.Histo1D({"Wprime_xq2cut","W prime for selected x_q2 region",100,-10,10},"Wp2");
    auto h_z_xq2cut = dxq2cut.Histo1D({"z_xq2cut","z for selected x_q2_region",100,0,1},"z");

    auto h_xbj_cut1 = d_cut1.Histo1D({"xbj_cut1","x after cut1",100,0,1},"xbj");
    auto h_q2_cut1 = d_cut1.Histo1D({"q2_cut1","Q2 after cut1",100,0,10},"Q2");
    auto h_Wprime_cut1 = d_cut1.Histo1D({"Wprime_cut1","W prime after cut1",100,-10,10},"Wp2");
    auto h_z_cut1 = d_cut1.Histo1D({"z_cut1","z after cut1",100,0,1},"z");
    
    auto h_xbj_cut2 = d_cut2.Histo1D({"xbj_cut2","x after cut2",100,0,1},"xbj");
    auto h_q2_cut2 = d_cut2.Histo1D({"q2_cut2","Q2 after cut2",100,0,10},"Q2");
    auto h_Wprime_cut2 = d_cut2.Histo1D({"Wprime_cut2","W prime after cut2",100,-10,10},"Wp2");
    auto h_z_cut2 = d_cut2.Histo1D({"z_cut2","z after cut2",100,0,1},"z");

    auto h_xbj_cut3 = d_cut3.Histo1D({"xbj_cut3","x after cut3",100,0,1},"xbj");
    auto h_q2_cut3 = d_cut3.Histo1D({"q2_cut3","Q2 after cut3",100,0,10},"Q2");
    auto h_Wprime_cut3 = d_cut3.Histo1D({"Wprime_cut3","W prime after cut3",100,-10,10},"Wp2");
    auto h_z_cut3 = d_cut3.Histo1D({"z_cut3","z after cut3",100,0,1},"z");

    std::string kin_rootfile_name = "results/csv_kin/kin_acceptance/kin_neg_rootfile_"+std::to_string(Rungroup)+".root";
    TFile *kin_rootfile = new TFile(kin_rootfile_name.c_str(),"RECREATE");
    TCanvas *c_xbj = new TCanvas();
    h_xbj_xq2cut->Scale(1/neg_charge);
    h_xbj_xq2cut->SetLineColor(coolcolor[0]);
    h_xbj_xq2cut->Write();
    h_xbj_xq2cut->DrawCopy();
    h_xbj_cut1->Scale(1/neg_charge);
    h_xbj_cut1->SetLineColor(coolcolor[1]);
    h_xbj_cut1->Write();
    h_xbj_cut1->DrawCopy("same");
    h_xbj_cut2->Scale(1/neg_charge);
    h_xbj_cut2->SetLineColor(coolcolor[2]);
    h_xbj_cut2->Write();
    h_xbj_cut2->DrawCopy("same");
    h_xbj_cut3->Scale(1/neg_charge);
    h_xbj_cut3->SetLineColor(coolcolor[3]);
    h_xbj_cut3->Write();
    h_xbj_cut3->DrawCopy("same");
    c_xbj->BuildLegend();
    std::string c_xbj_name = "results/csv_kin/kin_acceptance/xbj_neg_cuts_"+std::to_string(Rungroup)+".png";
    c_xbj->SaveAs(c_xbj_name.c_str());

    TCanvas *c_q2 = new TCanvas();
    h_q2_xq2cut->Scale(1/neg_charge);
    h_q2_xq2cut->SetLineColor(coolcolor[0]);
    h_q2_xq2cut->Write();
    h_q2_xq2cut->DrawCopy();
    h_q2_cut1->Scale(1/neg_charge);
    h_q2_cut1->SetLineColor(coolcolor[1]);
    h_q2_cut1->Write();
    h_q2_cut1->DrawCopy("same");
    h_q2_cut2->Scale(1/neg_charge);
    h_q2_cut2->SetLineColor(coolcolor[2]);
    h_q2_cut2->Write();
    h_q2_cut2->DrawCopy("same");
    h_q2_cut3->Scale(1/neg_charge);
    h_q2_cut3->SetLineColor(coolcolor[3]);
    h_q2_cut3->Write();
    h_q2_cut3->DrawCopy("same");
    c_q2->BuildLegend();
    std::string c_q2_name = "results/csv_kin/kin_acceptance/q2_neg_cuts_"+std::to_string(Rungroup)+".png";
    c_q2->SaveAs(c_q2_name.c_str());

    TCanvas *c_Wprime = new TCanvas();
    h_Wprime_xq2cut->Scale(1/neg_charge);
    h_Wprime_xq2cut->SetLineColor(coolcolor[0]);
    h_Wprime_xq2cut->Write();
    h_Wprime_xq2cut->DrawCopy();
    h_Wprime_cut1->Scale(1/neg_charge);
    h_Wprime_cut1->SetLineColor(coolcolor[1]);
    h_Wprime_cut1->Write();
    h_Wprime_cut1->DrawCopy("same");
    h_Wprime_cut2->Scale(1/neg_charge);
    h_Wprime_cut2->SetLineColor(coolcolor[2]);
    h_Wprime_cut2->Write();
    h_Wprime_cut2->DrawCopy("same");
    h_Wprime_cut3->Scale(1/neg_charge);
    h_Wprime_cut3->SetLineColor(coolcolor[3]);
    h_Wprime_cut3->Write();
    h_Wprime_cut3->DrawCopy("same");
    c_Wprime->BuildLegend();
    std::string c_Wprime_name = "results/csv_kin/kin_acceptance/Wprime_neg_cuts_"+std::to_string(Rungroup)+".png";
    c_Wprime->SaveAs(c_xbj_name.c_str());

    TCanvas *c_z = new TCanvas();
    h_z_xq2cut->Scale(1/neg_charge);
    h_z_xq2cut->SetLineColor(coolcolor[0]);
    h_z_xq2cut->Write();
    h_z_xq2cut->DrawCopy();
    h_z_cut1->Scale(1/neg_charge);
    h_z_cut1->SetLineColor(coolcolor[1]);
    h_z_cut1->Write();
    h_z_cut1->DrawCopy("same");
    h_z_cut2->Scale(1/neg_charge);
    h_z_cut2->SetLineColor(coolcolor[2]);
    h_z_cut2->Write();
    h_z_cut2->DrawCopy("same");
    h_z_cut3->Scale(1/neg_charge);
    h_z_cut3->SetLineColor(coolcolor[3]);
    h_z_cut3->Write();
    h_z_cut3->DrawCopy("same");
    c_z->BuildLegend();
    std::string c_z_name = "results/csv_kin/kin_acceptance/z_neg_cuts_"+std::to_string(Rungroup)+".png";
    c_z->SaveAs(c_z_name.c_str());

    kin_rootfile->Close();

    double pos_charge = 0;
    //for all pos runs
      TChain chain_pos("T");
      //loop over all neg runs in this run group 
    for(std::vector<int>::iterator it = pos_runs.begin();it!= pos_runs.end();++it){
      int RunNumber = *it;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      ROOT::RDataFrame d_sh("TSP",rootfile_name);
      auto charge = d_sh.Max("P.BCM4B.scalerChargeCut");
      pos_charge += *charge;
      std::cout<<"Run Number "<<RunNumber<<" charge "<<*charge<<std::endl;
      //TFile* rootfile = new TFile(rootfile_name.c_str());
      chain_pos.Add(rootfile_name.c_str());    
    }//end of neg runs loop
      //dataframe for the chain of this run group 
      ROOT::RDataFrame d_pos(chain_pos);
      std::cout<<"charge for all pi+ runs "<<pos_charge<<std::endl;

      auto d_coin_pos = d_pos.Filter("fEvtHdr.fEvtType == 4");
      auto d_shmssingles_pos = d_pos.Filter("fEvtHdr.fEvtType == 1");
      //  auto d_coin = d;
      //cuts same as neg runs
      auto dHMSGoodTrack_pos = d_coin_pos.Filter(goodTrackHMS);
      auto dSHMSGoodTrack_pos = d_coin_pos.Filter(goodTrackSHMS);
      auto dCoinGoodTrack_pos = dHMSGoodTrack_pos.Filter(goodTrackSHMS)
      .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
      .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
      .Define("pion_momentum",pion_momentum,{"P.gtr.px","P.gtr.py","P.gtr.pz"})
      .Define("pion_momentum_rotated",rotate,{"pion_momentum"})
      .Define("pion_momentum_rotated_x",[](TVector3 v){return v.X();},{"pion_momentum_rotated"})
      .Define("pion_momentum_rotated_y",[](TVector3 v){return v.Y();},{"pion_momentum_rotated"})
      .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_q", p_q, {"p_electron"})
      .Define("z", z, {"p_q","p_pion"})
      .Define("Q2", Q2, {"p_q"})
      .Define("xbj", xbj, {"Q2","p_q"})
      .Define("W2",W2,{"p_q"})
      .Define("Wp2",Wprime2,{"p_q","p_pion"})
      .Define("W","std::sqrt(W2)")
      .Define("Wp","std::sqrt(Wp2)")
      ;
      auto dHMS_electron_pos = dCoinGoodTrack_pos.Filter(eCutHMS);
      auto dSHMS_pion_pos = dCoinGoodTrack_pos.Filter(piCutSHMS);
      auto dCOIN_sidis_pos = dCoinGoodTrack_pos.Filter(eCutHMS + " && " + piCutSHMS);
      std::cout<<*dCOIN_sidis_pos.Count()<<std::endl; 

      auto dxq2cut_pos = dCOIN_sidis_pos;

      auto h_xq2_pos = dCOIN_sidis_pos.Histo2D({"x_Q2","x_Q2",400,0,1,400,0,10},"xbj","Q2");
      //    TBox* xq2_box = new TBox(x_min,q2_min,x_max,q2_max);
      TCanvas* c_xq2_pos = new TCanvas("x_Q2");
      h_xq2_pos->DrawCopy("colz");
      //    xq2_box->SetFillStyle(0);
      //    xq2_box->SetLineColor(kRed);
      //    xq2_box->Draw("l");
      std::string xq2filename_pos = "results/csv_kin/kin_acceptance/x_Q2_pos_"+std::to_string(Rungroup)+".png";
      c_xq2_pos->SaveAs(xq2filename_pos.c_str());

    std::cout<<*dxq2cut_pos.Count()<<std::endl;

    //pxpy, use same cut as neg runs
    auto h_pion_momentum_x_pos = dxq2cut_pos.Histo1D({"pion_p_x","pion momentum x",500,-2,2},"pion_momentum_rotated_x");
    auto h_pion_momentum_y_pos = dxq2cut_pos.Histo1D({"pion_p_y","pion momentum y",500,-2,2},"pion_momentum_rotated_y");
    auto h_pion_momentum_xy_pos = dxq2cut_pos.Histo2D({"pion_p_xy","pion momentum x y",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    TCanvas *c_pion_momentum_pos = new TCanvas();
    c_pion_momentum_pos->Divide(2,2);
    c_pion_momentum_pos->cd(1);
    h_pion_momentum_x_pos->DrawCopy();
    c_pion_momentum_pos->cd(2);
    h_pion_momentum_y_pos->DrawCopy();
    c_pion_momentum_pos->cd(3);
    h_pion_momentum_xy_pos->DrawCopy("colz");
    
    std::string pion_momentum_name_pos = "results/csv_kin/kin_acceptance/kin_pos_acceptance_"+std::to_string(Rungroup)+".png";
    c_pion_momentum_pos->SaveAs(pion_momentum_name_pos.c_str());
  
    
    auto d_cut1_pos = dxq2cut_pos.Filter(cut1,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});
    auto d_cut2_pos = dxq2cut_pos.Filter(cut2,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});
    auto d_cut3_pos = dxq2cut_pos.Filter(cut3,{"pion_momentum_rotated_x","pion_momentum_rotated_y"});

    auto h_pion_cut1_pos = d_cut1_pos.Histo2D({"pion_p_xy","pion momentum xy cut1",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    auto h_pion_cut2_pos = d_cut2_pos.Histo2D({"pion_p_xy","pion momentum xy cut2",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    auto h_pion_cut3_pos = d_cut3_pos.Histo2D({"pion_p_xy","pion momentum xy cut3",500,-2,2,500,-2,2},"pion_momentum_rotated_x","pion_momentum_rotated_y");
    TCanvas* c_xy_cuts_pos = new TCanvas();
    h_pion_momentum_xy_pos->SetMarkerStyle(7);
    h_pion_momentum_xy_pos->SetMarkerColor(coolcolor[0]);
    h_pion_momentum_xy_pos->DrawCopy();
    h_pion_cut3_pos->SetMarkerStyle(7);
    h_pion_cut3_pos->SetMarkerColor(coolcolor[3]);
    h_pion_cut3_pos->DrawCopy("p same");
    h_pion_cut2_pos->SetMarkerStyle(7);
    h_pion_cut2_pos->SetMarkerColor(coolcolor[2]);
    h_pion_cut2_pos->DrawCopy("p same");
    h_pion_cut1_pos->SetMarkerStyle(7);
    h_pion_cut1_pos->SetMarkerColor(coolcolor[1]);
    h_pion_cut1_pos->DrawCopy("p same");
    std::string pion_cut_name_pos = "results/csv_kin/kin_acceptance/kin_pos_acceptance_cuts"+std::to_string(Rungroup)+".png";
    c_xy_cuts_pos->BuildLegend();
    c_xy_cuts_pos->SaveAs(pion_cut_name_pos.c_str());

    auto h_xbj_xq2cut_pos = dxq2cut_pos.Histo1D({"xbj_xq2cut","xfor selected x_q2 region",100,0,1},"xbj");
    auto h_q2_xq2cut_pos = dxq2cut_pos.Histo1D({"q2_xq2cut","Q2 for selected x_q2 region",100,0,10},"Q2");
    auto h_Wprime_xq2cut_pos = dxq2cut_pos.Histo1D({"Wprime_xq2cut","W prime for selected x_q2 region",100,-10,10},"Wp2");
    auto h_z_xq2cut_pos = dxq2cut_pos.Histo1D({"z_xq2cut","z for selected x_q2_region",100,0,1},"z");

    auto h_xbj_cut1_pos = d_cut1_pos.Histo1D({"xbj_cut1","x after cut1",100,0,1},"xbj");
    auto h_q2_cut1_pos = d_cut1_pos.Histo1D({"q2_cut1","Q2 after cut1",100,0,10},"Q2");
    auto h_Wprime_cut1_pos = d_cut1_pos.Histo1D({"Wprime_cut1","W prime after cut1",100,-10,10},"Wp2");
    auto h_z_cut1_pos = d_cut1_pos.Histo1D({"z_cut1","z after cut1",100,0,1},"z");
    
    auto h_xbj_cut2_pos = d_cut2_pos.Histo1D({"xbj_cut2","x after cut2",100,0,1},"xbj");
    auto h_q2_cut2_pos = d_cut2_pos.Histo1D({"q2_cut2","Q2 after cut2",100,0,10},"Q2");
    auto h_Wprime_cut2_pos = d_cut2_pos.Histo1D({"Wprime_cut2","W prime after cut2",100,-10,10},"Wp2");
    auto h_z_cut2_pos = d_cut2_pos.Histo1D({"z_cut2","z after cut2",100,0,1},"z");

    auto h_xbj_cut3_pos = d_cut3_pos.Histo1D({"xbj_cut3","x after cut3",100,0,1},"xbj");
    auto h_q2_cut3_pos = d_cut3_pos.Histo1D({"q2_cut3","Q2 after cut3",100,0,10},"Q2");
    auto h_Wprime_cut3_pos = d_cut3_pos.Histo1D({"Wprime_cut3","W prime after cut3",100,-10,10},"Wp2");
    auto h_z_cut3_pos = d_cut3_pos.Histo1D({"z_cut3","z after cut3",100,0,1},"z");

    std::string kin_rootfile_name_pos = "results/csv_kin/kin_acceptance/kin_pos_rootfile_"+std::to_string(Rungroup)+".root";
    TFile *kin_rootfile_pos = new TFile(kin_rootfile_name_pos.c_str(),"RECREATE");
    TCanvas *c_xbj_pos = new TCanvas();
    h_xbj_xq2cut_pos->Scale(1/pos_charge);
    h_xbj_xq2cut_pos->SetLineColor(warmcolor[0]);
    h_xbj_xq2cut_pos->Write();
    h_xbj_xq2cut_pos->DrawCopy();
    h_xbj_cut1_pos->Scale(1/pos_charge);
    h_xbj_cut1_pos->SetLineColor(warmcolor[1]);
    h_xbj_cut1_pos->Write();
    h_xbj_cut1_pos->DrawCopy("same");
    h_xbj_cut2_pos->Scale(1/pos_charge);
    h_xbj_cut2_pos->SetLineColor(warmcolor[2]);
    h_xbj_cut2_pos->Write();
    h_xbj_cut2_pos->DrawCopy("same");
    h_xbj_cut3_pos->Scale(1/pos_charge);
    h_xbj_cut3_pos->SetLineColor(warmcolor[3]);
    h_xbj_cut3_pos->Write();
    h_xbj_cut3_pos->DrawCopy("same");
    c_xbj_pos->BuildLegend();
    std::string c_xbj_name_pos = "results/csv_kin/kin_acceptance/xbj_pos_cuts_"+std::to_string(Rungroup)+".png";
    c_xbj_pos->SaveAs(c_xbj_name_pos.c_str());

    TCanvas *c_q2_pos = new TCanvas();
    h_q2_xq2cut_pos->Scale(1/pos_charge);
    h_q2_xq2cut_pos->SetLineColor(warmcolor[0]);
    h_q2_xq2cut_pos->Write();
    h_q2_xq2cut_pos->DrawCopy();
    h_q2_cut1_pos->Scale(1/pos_charge);
    h_q2_cut1_pos->SetLineColor(warmcolor[1]);
    h_q2_cut1_pos->Write();
    h_q2_cut1_pos->DrawCopy("same");
    h_q2_cut2_pos->Scale(1/pos_charge);
    h_q2_cut2_pos->SetLineColor(warmcolor[2]);
    h_q2_cut2_pos->Write();
    h_q2_cut2_pos->DrawCopy("same");
    h_q2_cut3_pos->Scale(1/pos_charge);
    h_q2_cut3_pos->SetLineColor(warmcolor[3]);
    h_q2_cut3_pos->Write();
    h_q2_cut3_pos->DrawCopy("same");
    c_q2_pos->BuildLegend();
    std::string c_q2_name_pos = "results/csv_kin/kin_acceptance/q2_pos_cuts_"+std::to_string(Rungroup)+".png";
    c_q2_pos->SaveAs(c_q2_name_pos.c_str());

    TCanvas *c_Wprime_pos = new TCanvas();
    h_Wprime_xq2cut_pos->Scale(1/pos_charge);
    h_Wprime_xq2cut_pos->SetLineColor(warmcolor[0]);
    h_Wprime_xq2cut_pos->Write();
    h_Wprime_xq2cut_pos->DrawCopy();
    h_Wprime_cut1_pos->Scale(1/pos_charge);
    h_Wprime_cut1_pos->SetLineColor(warmcolor[1]);
    h_Wprime_cut1_pos->Write();
    h_Wprime_cut1_pos->DrawCopy("same");
    h_Wprime_cut2_pos->Scale(1/pos_charge);
    h_Wprime_cut2_pos->SetLineColor(warmcolor[2]);
    h_Wprime_cut2_pos->Write();
    h_Wprime_cut2_pos->DrawCopy("same");
    h_Wprime_cut3_pos->Scale(1/pos_charge);
    h_Wprime_cut3_pos->SetLineColor(warmcolor[3]);
    h_Wprime_cut3_pos->Write();
    h_Wprime_cut3_pos->DrawCopy("same");
    c_Wprime_pos->BuildLegend();
    std::string c_Wprime_name_pos = "results/csv_kin/kin_acceptance/Wprime_pos_cuts_"+std::to_string(Rungroup)+".png";
    c_Wprime_pos->SaveAs(c_xbj_name_pos.c_str());

    TCanvas *c_z_pos = new TCanvas();
    h_z_xq2cut_pos->Scale(1/pos_charge);
    h_z_xq2cut_pos->SetLineColor(warmcolor[0]);
    h_z_xq2cut_pos->Write();
    h_z_xq2cut_pos->DrawCopy();
    h_z_cut1_pos->Scale(1/pos_charge);
    h_z_cut1_pos->SetLineColor(warmcolor[1]);
    h_z_cut1_pos->Write();
    h_z_cut1_pos->DrawCopy("same");
    h_z_cut2_pos->Scale(1/pos_charge);
    h_z_cut2_pos->SetLineColor(warmcolor[2]);
    h_z_cut2_pos->Write();
    h_z_cut2_pos->DrawCopy("same");
    h_z_cut3_pos->Scale(1/pos_charge);
    h_z_cut3_pos->SetLineColor(warmcolor[3]);
    h_z_cut3_pos->Write();
    h_z_cut3_pos->DrawCopy("same");
    c_z_pos->BuildLegend();
    std::string c_z_name_pos = "results/csv_kin/kin_acceptance/z_pos_cuts_"+std::to_string(Rungroup)+".png";
    c_z->SaveAs(c_z_name_pos.c_str());

    kin_rootfile_pos->Close();
}
