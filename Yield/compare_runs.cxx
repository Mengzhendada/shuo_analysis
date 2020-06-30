#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TLine.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TRatioPlot.h"
#include "TChain.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

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

std::string goodTrackSHMS = "P.gtr.dp>-10 && P.gtr.dp<22";
std::string goodTrackHMS = "H.gtr.dp>-10 && H.gtr.dp<10";

std::string piCutSHMS = "P.aero.npeSum >1.0 && P.cal.eprtracknorm <0.2 && P.cal.etottracknorm<1.0";
std::string eCutHMS = "H.cal.etottracknorm >0.8 && H.cal.etottracknorm <2. && "
                       "H.cer.npeSum > 1.";
std::string epiCut = "P.aero.npeSum > 1.0 && P.cal.eprtracknorm <0.2 && "
                     "H.cer.npeSum > 1.0 && H.cal.etottracknorm > 0.6 && "
                     "H.cal.etottracknorm < 2.0 && P.cal.etottracknorm <1.0";

void compare_runs(int Rungroup = 0){
  
  if(Rungroup ==0){
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

  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};
  
  TChain chain_neg("T");
  double neg_charge = 0;
  using Histo1DProxy = decltype(ROOT::RDataFrame{0}.Histo1D(ROOT::RDF::TH1DModel{"","",128u,0.,0.},""));
  int n_neg = neg_runs.size();
  std::vector<Histo1DProxy> h_vec_neg;
  for(std::vector<int>::iterator it = neg_runs.begin();it!=neg_runs.end();++it){
    int RunNumber = *it;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_sh("TSP",rootfile_name);
    double charge = *d_sh.Max("P.BCM4B.scalerChargeCut");
    neg_charge += charge;
    chain_neg.Add(rootfile_name.c_str());
    TFile *rootfile = new TFile(rootfile_name.c_str());
    ROOT::RDataFrame d("T",rootfile);
    auto dCoinGoodTrack = d.Filter(goodTrackHMS)
      .Filter(goodTrackSHMS)
      .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
      .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
      .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_q", p_q, {"p_electron"})
      .Define("z", z, {"p_q","p_pion"})
      .Define("Q2", Q2, {"p_q"})
      .Define("xbj", xbj, {"Q2","p_q"})
      .Define("W2",W2,{"p_q"})
      .Define("Wp2",Wprime2,{"p_q","p_pion"})
      .Define("W","std::sqrt(W2)")
      .Define("Wp","std::sqrt(Wp2)")
      .Filter(epiCut)
      .Filter("fEvtHdr.fEvtType == 4")
      ;
    //++i_neg;
    //std::cout<<i_neg<<std::endl;
    //c_neg->cd(i_neg);  
    auto h = dCoinGoodTrack.Histo1D({(std::to_string(RunNumber)+"_xbj").c_str(),(std::to_string(RunNumber)+"_xbj").c_str(),100,0,1},"xbj");
    //h->DrawCopy();
    h->Scale(1/charge);
    h_vec_neg.push_back(h);
        //dCoinGoodTrack.Histo1D({"","",100,0,1},"xbj").GetPtr());
      std::cout<<"RunNumber "<<RunNumber<<std::endl;
  }
  ROOT::RDataFrame d_neg_all(chain_neg);
  auto dCoinGoodTrack_neg_all = d_neg_all.Filter(goodTrackHMS)
    .Filter(goodTrackSHMS)
    .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
    .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_q", p_q, {"p_electron"})
    .Define("z", z, {"p_q","p_pion"})
    .Define("Q2", Q2, {"p_q"})
    .Define("xbj", xbj, {"Q2","p_q"})
    .Define("W2",W2,{"p_q"})
    .Define("Wp2",Wprime2,{"p_q","p_pion"})
    .Define("W","std::sqrt(W2)")
    .Define("Wp","std::sqrt(Wp2)")
      .Filter(epiCut)
      .Filter("fEvtHdr.fEvtType == 4")
      ;
  auto h_neg_all = dCoinGoodTrack_neg_all.Histo1D({"","",100,0,1},"xbj"); 
  h_neg_all->Scale(1/neg_charge);
  int n_neg_canvas = n_neg+1;
  
  TChain chain_pos("T");
  double pos_charge = 0;
  int n_pos = pos_runs.size();
  std::vector<Histo1DProxy> h_vec_pos;
  for(std::vector<int>::iterator it = pos_runs.begin();it!=pos_runs.end();++it){
    int RunNumber = *it;
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_sh("TSP",rootfile_name);
    double charge = *d_sh.Max("P.BCM4B.scalerChargeCut");
    pos_charge += charge;
    chain_pos.Add(rootfile_name.c_str());
    TFile *rootfile = new TFile(rootfile_name.c_str());
    ROOT::RDataFrame d("T",rootfile);
    auto dCoinGoodTrack = d.Filter(goodTrackHMS)
      .Filter(goodTrackSHMS)
      .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
      .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
      .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
      .Define("p_q", p_q, {"p_electron"})
      .Define("z", z, {"p_q","p_pion"})
      .Define("Q2", Q2, {"p_q"})
      .Define("xbj", xbj, {"Q2","p_q"})
      .Define("W2",W2,{"p_q"})
      .Define("Wp2",Wprime2,{"p_q","p_pion"})
      .Define("W","std::sqrt(W2)")
      .Define("Wp","std::sqrt(Wp2)")
      .Filter(epiCut)
      .Filter("fEvtHdr.fEvtType == 4")
      ;
    //++i_pos;
    //std::cout<<i_pos<<std::endl;
    //c_pos->cd(i_pos);  
    auto h = dCoinGoodTrack.Histo1D({(std::to_string(RunNumber)+"_xbj").c_str(),(std::to_string(RunNumber)+"_xbj").c_str(),100,0,1},"xbj");
    //h->DrawCopy();
    h->Scale(1/charge);
    h_vec_pos.push_back(h);
        //dCoinGoodTrack.Histo1D({"","",100,0,1},"xbj").GetPtr());
      std::cout<<"RunNumber "<<RunNumber<<std::endl;
  }
  ROOT::RDataFrame d_pos_all(chain_pos);
  auto dCoinGoodTrack_pos_all = d_pos_all.Filter(goodTrackHMS)
    .Filter(goodTrackSHMS)
    .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
    .Define("p_pion", p_pion, {"P.gtr.py", "P.gtr.px", "P.gtr.pz"})
    .Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
    .Define("p_q", p_q, {"p_electron"})
    .Define("z", z, {"p_q","p_pion"})
    .Define("Q2", Q2, {"p_q"})
    .Define("xbj", xbj, {"Q2","p_q"})
    .Define("W2",W2,{"p_q"})
    .Define("Wp2",Wprime2,{"p_q","p_pion"})
    .Define("W","std::sqrt(W2)")
    .Define("Wp","std::sqrt(Wp2)")
      .Filter(epiCut)
      .Filter("fEvtHdr.fEvtType == 4")
    ;
  auto h_pos_all = dCoinGoodTrack_pos_all.Histo1D({"","",100,0,1},"xbj"); 
  h_pos_all->Scale(1/pos_charge);
  int n_pos_canvas = n_pos+1;

  int n_canvas;
  if(n_neg_canvas>n_pos_canvas){n_canvas = n_neg_canvas;}
  else{n_canvas = n_pos_canvas;}

  TCanvas *c = new TCanvas();
  c->Divide(n_canvas,2);
  c->cd(1);
  h_neg_all->DrawCopy();
  int secondrow = n_canvas+1;
  c->cd(secondrow);
  h_pos_all->DrawCopy();
  int firstrow = 1;
    c->SetTicks(0,1);
  for(int i = 0;i<n_neg;++i){
    ++firstrow;
    std::cout<<firstrow<<std::endl;
    c->cd(firstrow);
    h_vec_neg.at(i)->SetLineColor(coolcolor[i]);
    h_vec_neg.at(i)->DrawCopy();
    h_neg_all->DrawCopy("same");
    auto rp = new TRatioPlot(h_vec_neg.at(i).GetPtr(),h_neg_all.GetPtr());
    c->cd(firstrow)->SetTicks(0,1);
    rp->Draw();
    rp->GetLowerRefGraph()->SetMinimum(0.7);
    rp->GetLowerRefGraph()->SetMaximum(1.2);
  }
  for(int i = 0;i<n_pos;++i){
    ++secondrow;
    std::cout<<secondrow<<std::endl;
    c->cd(secondrow);
    h_vec_pos.at(i)->SetLineColor(warmcolor[i]);
    h_vec_pos.at(i)->DrawCopy();
    h_pos_all->DrawCopy("same");
    auto rp_pos = new TRatioPlot(h_vec_pos.at(i).GetPtr(),h_pos_all.GetPtr());
    c->cd(secondrow)->SetTicks(0,1);
    rp_pos->Draw();
    rp_pos->GetLowerRefGraph()->SetMinimum(0.7);
    rp_pos->GetLowerRefGraph()->SetMaximum(1.2);
  }
    c->Update();
  c->SaveAs(("results/compare_runs_"+std::to_string(Rungroup)+".pdf").c_str());
  std::cout<<"check"<<std::endl;
  TCanvas *c1 = new TCanvas();
  h_neg_all->DrawCopy();
  h_pos_all->SetLineColor(kRed);
  h_pos_all->DrawCopy("same");
  auto rp_all = new TRatioPlot(h_neg_all.GetPtr(),h_pos_all.GetPtr());
  rp_all->Draw();
  rp_all->GetLowerRefGraph()->SetMinimum(0.7);
  rp_all->GetLowerRefGraph()->SetMaximum(1.2);
  c1->SaveAs(("results/compare_pos_neg_runs_"+std::to_string(Rungroup)+".pdf").c_str());
}
