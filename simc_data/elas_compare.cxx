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
using PVec3D = ROOT::Math::XYZVector();
using PVec4D = ROOT::Math::PxPyPzMVector;

constexpr const double M_P = 0.938272;
constexpr const double M_P2    = M_P * M_P;
constexpr const double M_pion  = 0.139;
constexpr const double M_pion2 = M_pion * M_pion;
constexpr const double M_e     = 0.000511;
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;


void elas_compare(){
  std::vector<int> fall_elas = {6009,6010,6012,6013,6016,6017}; 
  std::vector<int> spring_elas = {7822,7823,7826,7827,7830};

  json j_simc;
  {
    std::ifstream ifs("db2/simc.json");
    ifs>>j_simc;
  }

  TGraphErrors *G_yield_fall_data = new TGraphErrors();
  G_yield_fall_data->SetTitle("data");
  TGraphErrors *G_yield_spring_data = new TGraphErrors();
  G_yield_spring_data->SetTitle("data");
  TGraphErrors *G_yield_fall_sim = new TGraphErrors();
  G_yield_fall_sim->SetTitle("sim");
  TGraphErrors *G_yield_spring_sim = new TGraphErrors();
  G_yield_spring_sim->SetTitle("sim");
  TGraphErrors *G_Epdiff_fall = new TGraphErrors();
  G_Epdiff_fall->SetTitle("fall");
  G_Epdiff_fall->GetXaxis()->SetTitle("momentum");
  G_Epdiff_fall->GetYaxis()->SetTitle("Eprime diff");
  TGraphErrors *G_Epdiff_spring = new TGraphErrors();
  G_Epdiff_spring->SetTitle("spring");
  G_Epdiff_spring->GetXaxis()->SetTitle("momentum");
  G_Epdiff_spring->GetYaxis()->SetTitle("Eprime diff");
  TGraphErrors *G_Epdiff_angle_fall = new TGraphErrors();
  G_Epdiff_angle_fall->SetTitle("fall");
  G_Epdiff_angle_fall->GetXaxis()->SetTitle("angle");
  G_Epdiff_angle_fall->GetYaxis()->SetTitle("Epdiff diff");
  TGraphErrors *G_Epdiff_angle_spring = new TGraphErrors();
  G_Epdiff_angle_spring->SetTitle("spring");
  G_Epdiff_angle_spring->GetXaxis()->SetTitle("angle");
  G_Epdiff_angle_spring->GetYaxis()->SetTitle("Epdiff diff");
  TGraphErrors *G_W_momentum_fall = new TGraphErrors();
  G_W_momentum_fall->SetTitle("fall");
  G_W_momentum_fall->GetXaxis()->SetTitle("momentum");
  G_W_momentum_fall->GetYaxis()->SetTitle("W diff");
  TGraphErrors *G_W_momentum_spring = new TGraphErrors();
  G_W_momentum_spring->SetTitle("spring");
  G_W_momentum_spring->GetXaxis()->SetTitle("momentum");
  G_W_momentum_spring->GetYaxis()->SetTitle("W diff");
  TGraphErrors *G_W_angle_fall = new TGraphErrors();
  G_W_angle_fall->SetTitle("fall");
  G_W_angle_fall->GetXaxis()->SetTitle("angle");
  G_W_angle_fall->GetYaxis()->SetTitle("W diff");
  TGraphErrors *G_W_angle_spring = new TGraphErrors();
  G_W_angle_spring->SetTitle("spring");
  G_W_angle_spring->GetXaxis()->SetTitle("angle");
  G_W_angle_spring->GetYaxis()->SetTitle("W diff");
  TGraphErrors *G_W_Epdiff_fall = new TGraphErrors();
  G_W_Epdiff_fall->SetTitle("fall");
  TGraphErrors *G_W_Epdiff_spring = new TGraphErrors();
  G_W_Epdiff_spring->SetTitle("spring");
  int i_fall = 0, i_spring = 0;
  for(auto it = j_simc.begin();it!=j_simc.end();++it){
    int RunNumber = std::stod(it.key());
    std::string rootfile_data_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    std::string rootfile_sim_name = "sim/csv"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d_data_raw("T",rootfile_data_name.c_str());
    ROOT::RDataFrame d_data_scaler("TSP",rootfile_data_name.c_str());
    ROOT::RDataFrame d_sim_raw("h10",rootfile_sim_name.c_str());
    //if ( !root_file_exists(rootfile_data_name)){
    //  std::cout <<  rootfile_data_name << " not found !\n";
    //}
    //if ( !root_file_exists(rootfile_sim_name)){
    //  std::cout <<  rootfile_sim_name << " not found !\n";
    //}
      auto scaler_current_list = d_data_scaler.Take<double>("P.BCM4B.scalerCurrent");
      auto scaler_charge_list = d_data_scaler.Take<double>("P.BCM4B.scalerChargeCut");
      std::cout<<"charge in scaler "<<*d_data_scaler.Max("P.BCM4B.scalerChargeCut")<<std::endl;
      auto scaler_event_list = d_data_scaler.Take<double>("evNumber");
      auto h_current = d_data_scaler.Histo1D({"","",100,3,100},"P.BCM4B.scalerCurrent");
      double set_cur = h_current->GetBinCenter(h_current->GetMaximumBin());
      std::cout<<"set current "<<set_cur<<"for Run "<<RunNumber<<std::endl;
      double tot_charge,pre_charge = 0,tot_charge_cut = 0;
      double tot_time,pre_time = 0, tot_time_cut = 0;
      int scaler_entries = scaler_event_list->size();
      for(int i = 0; i<scaler_entries;i++){
          tot_charge = scaler_charge_list->at(i);
        if(abs(scaler_current_list->at(i)-set_cur)<3){
          tot_charge_cut = tot_charge_cut+(tot_charge-pre_charge);
          
        }
        pre_charge = tot_charge;
      }
      std::cout<<"charge "<<tot_charge<<" after cut "<<tot_charge_cut<<std::endl;
      TCanvas* c_current = new TCanvas();
      h_current->DrawCopy("hist");
      std::string current_name = "results/simc_data/"+std::to_string(RunNumber)+"_current.pdf";
      c_current->SaveAs(current_name.c_str());
      //std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size "<<pos_scaler_current_list->size()<<std::endl;
      auto get_current = [&](unsigned int eventNum){
        int i = 0;
        while(eventNum>scaler_event_list->at(i)){
          ++i;
          if(i>scaler_event_list->size()-1)
          {
            i=scaler_event_list->size()-1;
            break;
          }
        }
        return scaler_current_list->at(i);
        //std::cout<< pos_scaler_current_list->at(i)<<std::endl;

      };
      double Eb;
      if(RunNumber < 7000){Eb = 10.597;}
      else{Eb = 10.214;}
      auto p_proton = [&](double px, double py, double pz){
        return PVec4D{px , py , pz , M_P};
      };
      auto p_pion = [&](double px, double py, double pz) {
        return PVec4D{px , py , pz , M_pion};
      };
      auto p_electron = [&](double px, double py, double pz) {
        return PVec4D{px , py , pz , M_e};
      };
      auto p_q = [&](PVec4D pe) {
        return PVec4D{0.0,0.0,Eb, M_e}-pe;
      };
      //auto t = [](const double Egamma, Pvec4D& jpsi) {
      //  Pvec4D beam{0, 0, Egamma, 0};
      //  return (beam - jpsi).M2();
      //};
      auto z = [&](PVec4D pq, PVec4D ph) {
        return ph.E()/pq.E();
      };
      auto xbj = [&](double Q2,PVec4D pq) {
        return Q2/(2.0*M_P*pq.E());
      };
      auto Q2 = [&](PVec4D pq) {
        return -1.0*pq.M2();
      };
      auto Wprime2 = [&](PVec4D pq,PVec4D ph) {
        auto Ptot = PVec4D{0.0,0.0,0.0, M_P} + pq - ph;
        return Ptot.Dot(Ptot);
      };
      auto W2 = [&](PVec4D pq) {
        auto Ptot = PVec4D{0.0,0.0,0.0, M_P} + pq;
        return Ptot.Dot(Ptot);
      };
      //Emiss = Ebeam+Mp - sqrt(P_pion^2 + Mpion^2) - sqrt(P_e+Me^2)
      auto Emiss = [&](PVec4D& p_pion, PVec4D p_electron){
        return Eb+M_P-sqrt(p_pion.E()*p_pion.E()+M_P2)-sqrt(p_electron.E()*p_electron.E()+M_e*M_e);
      };
      auto mmiss = [&](PVec4D p_pion,PVec4D p_electron){
        PVec4D missing_mass = PVec4D{0.0,0.0,Eb,M_e}+PVec4D{0.0,0.0,0.0,M_P}-p_electron-p_pion;
        return std::sqrt(missing_mass.M2());
      };
      double hms_momentum = j_simc[std::to_string(RunNumber)]["hpcentral"].get<double>();
      double hms_angle = j_simc[std::to_string(RunNumber)]["htheta_lab"].get<double>()*M_PI/180;  
      double Eprime = Eb/(1.0+((2.0*Eb/M_P)*(sin(hms_angle/2.0)*sin(hms_angle/2.0))));
      std::cout<<"Eprime "<<Eprime<<std::endl;
      //std::cout<<sin(30*3.14/180)<<std::endl;
      auto d_data = d_data_raw
        .Filter([](double etottracknorm){return etottracknorm > 0.8;},{"H.cal.etottracknorm"})
        //.Filter([](double emiss){return emiss<0.1 && emiss>-0.1;},{"P.kin.secondary.emiss"})
        .Define("current",get_current,{"fEvtHdr.fEvtNum"})
        .Filter([&](double current){return std::abs(current-set_cur)<3;},{"current"})
        .Define("p_electron", p_electron, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("E_prime",[](PVec4D p_e){return p_e.E();},{"p_electron"})
        .Define("E_diff",[&](double eprime){return eprime-Eprime;},{"E_prime"})
        .Define("p_proton",p_proton, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
        //.Define("p_pion", p_pion, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
        //.Define("p_pion_HMS", p_pion, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
        .Define("p_q", p_q, {"p_electron"})
        .Define("z", z, {"p_q","p_proton"})
        .Define("Q2", Q2, {"p_q"})
        .Define("xbj", xbj, {"Q2","p_q"})
        .Define("W2", W2, {"p_q"})
        .Define("Wp2", Wprime2, {"p_q","p_proton"})
        .Define("W", "std::sqrt(W2)")
        .Define("Wp", "std::sqrt(Wp2)")
        .Define("InvMass","p_electron.Dot(p_proton)")
        //.Define("InvMass_pions","p_pion_HMS.Dot(p_pion)")
        .Define("emiss",Emiss,{"p_proton","p_electron"})
        .Define("mmiss",mmiss,{"p_proton","p_electron"})
        //.Define()   
        .Filter("H.gtr.dp > -8 && H.gtr.dp < 8")
        .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
        .Filter("P.gtr.th >-0.045 && P.gtr.th < 0.045")
        .Filter("H.gtr.th >-0.060 && H.gtr.th < 0.060")
        .Filter("P.gtr.ph >-0.028 && P.gtr.ph < 0.028")
        .Filter("H.gtr.ph > -0.022 && H.gtr.ph < 0.022")
        ;
      std::cout<<" data counts "<<*d_data.Count()<<std::endl;
      //  auto d_data_nocurrentcut = d_data_raw
      //    .Filter([](double etottracknorm){return etottracknorm > 0.8;},{"H.cal.etottracknorm"})
      //    .Filter([](double emiss){return emiss<0.1 && emiss>-0.1;},{"P.kin.secondary.emiss"})
      //    .Filter("H.gtr.dp > -8 && H.gtr.dp < 8")
      //    .Filter("P.gtr.dp > -10 && P.gtr.dp < 22")
      //    ;
      auto d_sim_1 = d_sim_raw
        .Filter("hsdelta > -8 && hsdelta < 8")
        .Filter("ssdelta > -10 && ssdelta < 22")
        .Filter("hsxptar > -0.06 && hsxptar < 0.06")
        .Filter("ssxptar > -0.045 && ssxptar < 0.045")
        .Filter("hsyptar > -0.022 && hsyptar < 0.022")
        .Filter("ssyptar > -0.028 && ssyptar < 0.028")
        ;
      double nentries = *d_sim_1.Count();
      std::cout<<"sim nentries "<<nentries<<std::endl;
      double normfac = j_simc[std::to_string(RunNumber).c_str()]["normfac"].get<double>();
      double hmsTE = j_simc[std::to_string(RunNumber).c_str()]["hmsTE"].get<double>();
      double shmsTE = j_simc[std::to_string(RunNumber).c_str()]["shmsTE"].get<double>();
      std::cout<<"normfac "<<normfac<<" hmsTE "<<hmsTE<<" shmsTE "<<shmsTE<<std::endl;
      double wfac = (normfac/nentries)*(tot_charge_cut/1000);
      auto weightcalculate = [&](float weight){return wfac*shmsTE*hmsTE*weight;};
      auto d_sim = d_sim_1.Define("weight_new",weightcalculate,{"Weight"});

      auto Q2_sim = d_sim.Histo1D({"Q2 sim","Q2 sim",100,7,12},"Q2","weight_new");
      auto W_sim = d_sim.Histo1D({"W sim","W sim",100,0.4,1.8},"W","weight_new");
      W_sim->Fit("gaus","0","",0.4,1.2);
      TF1 *W_sim_fit = W_sim->GetFunction("gaus");
      //auto Q2_data = d_data.Histo1D({"Q2 data","Q2 data",100,7,12},"H.kin.primary.Q2");
      //auto W_data = d_data.Histo1D({"W data","W data",100,0.4,1.8},"H.kin.primary.W");
      auto xbj_data = d_data.Histo1D({"xbj data","xbj data",100,0,2},"xbj");
      auto Eprime_diff = d_data.Histo1D({"Eprime diff","Eprime diff",100,-1,1},"E_diff");
      std::cout<<"Eprime diff counts "<<Eprime_diff->Integral()<<std::endl;
      std::cout<<"Mean "<<Eprime_diff->GetMean(1)<<std::endl;
      double Eprimediff_mean = Eprime_diff->GetMean(1);
      //auto xbj_sim = d_sim.Histo1D({"xbj sim","xbj sim",100,0,2},"xbj");
      auto Q2_data = d_data.Histo1D({"Q2 data","Q2 data",100,7,12},"Q2");
      auto W_data = d_data.Histo1D({"W data","W data",100,0.4,1.8},"W");
      W_data->Fit("gaus","0","",0.4,1.2);
      TF1 *W_data_fit = W_data->GetFunction("gaus");
      double W_data_mean = W_data_fit->GetParameter(1);
      double W_sim_mean = W_sim_fit->GetParameter(1);
      double W_mean_diff = W_data_mean-W_sim_mean;
      std::cout<<"W mean data "<<W_data_mean<<" W mean sim "<<W_sim_mean<<"W_diff "<<W_mean_diff<<std::endl;
      //auto Q2_data = d_data_nocurrentcut.Histo1D({"Q2 data","Q2 data",100,7,12},"H.kin.primary.Q2");
      //auto W_data = d_data_nocurrentcut.Histo1D({"W data","W data",100,0.4,1.8},"H.kin.primary.W");

      std::cout<<" counts "<<xbj_data->Integral()<<std::endl;
      
      auto W_xfp_data = d_data.Histo2D({"","W vs xfp data",100,-30,30,100,0.4,1.8},"P.dc.x_fp","W");
      auto W_xpfp_data = d_data.Histo2D({"","W vs xpfp data",100,-0.1,0.1,100,0.4,1.8},"P.dc.xp_fp","W");
      auto W_xfp_sim = d_sim.Histo2D({"","W vs xfp sim",100,-30,30,100,0.4,1.8},"hsxfp","W","weight_new");
      auto W_xpfp_sim = d_sim.Histo2D({"","W vs xpfp sim",100,-0.1,0.1,100,0.4,1.8},"hsxpfp","W","weight_new");
      //auto Emiss_data = d_data.Histo1D({"","Emiss data",100,-0.04,0.2},"P.kin.secondary.emiss");
      auto Emiss_data = d_data.Histo1D({"","Emiss data",100,-0.2,0.4},"emiss");
      auto Emiss_sim = d_sim.Histo1D({"","Emiss sim",100,-0.2,0.4},"Em","weight_new");

      double sim_counts = Q2_sim->Integral();
      double data_counts = Q2_data->Integral();
      double data_counts_1 = *d_data.Count();
      double sim_counts_1 = *d_sim.Count();
      std::cout<<sim_counts<<" "<<sim_counts_1<<std::endl;
      std::cout<<data_counts<<" "<<data_counts_1<<std::endl;;
      if(RunNumber < 7000){
        std::cout<<"RunNumber for graphs"<<RunNumber<<std::endl;
        //double yield_sim = sim_counts/tot_charge;
        //double yield_data = data_counts/tot_charge;
        //G_yield_fall_data->SetPoint(i_fall,RunNumber,yield_data);
        //G_yield_fall_data->SetPointError(i_fall,0,std::sqrt(data_counts)/tot_charge);
        //G_yield_fall_sim->SetPoint(i_fall,RunNumber,yield_sim);
        //G_yield_fall_sim->SetPointError(i_fall,0,std::sqrt(sim_counts)/tot_charge);
        double yield_sim = sim_counts/tot_charge_cut;
        double yield_data = data_counts/tot_charge_cut;
        G_yield_fall_data->SetPoint(i_fall,RunNumber,yield_data);
        G_yield_fall_data->SetPointError(i_fall,0,std::sqrt(data_counts)/tot_charge_cut);
        G_yield_fall_sim->SetPoint(i_fall,RunNumber,yield_sim);
        G_yield_fall_sim->SetPointError(i_fall,0,std::sqrt(sim_counts)/tot_charge_cut);
        G_Epdiff_fall->SetPoint(i_fall,hms_momentum,Eprimediff_mean);
        G_Epdiff_angle_fall->SetPoint(i_fall,hms_angle*180/M_PI,Eprimediff_mean);
        G_W_momentum_fall->SetPoint(i_fall,hms_momentum,W_mean_diff);
        G_W_angle_fall->SetPoint(i_fall,hms_angle*180/M_PI,W_mean_diff);
        G_W_Epdiff_fall->SetPoint(i_fall,Eprimediff_mean,W_mean_diff);
        i_fall++;
      }
      else{
        //double yield_sim = sim_counts/tot_charge;
        //double yield_data = data_counts/tot_charge;
        //G_yield_spring_data->SetPoint(i_spring,RunNumber,yield_data);
        //G_yield_spring_data->SetPointError(i_spring,0,std::sqrt(data_counts)/tot_charge);
        //G_yield_spring_sim->SetPoint(i_spring,RunNumber,yield_sim);
        //G_yield_spring_sim->SetPointError(i_spring,0,std::sqrt(sim_counts)/tot_charge);
        double yield_sim = sim_counts/tot_charge_cut;
        double yield_data = data_counts/tot_charge_cut;
        G_yield_spring_data->SetPoint(i_spring,RunNumber,yield_data);
        G_yield_spring_data->SetPointError(i_spring,0,std::sqrt(data_counts)/tot_charge_cut);
        G_yield_spring_sim->SetPoint(i_spring,RunNumber,yield_sim);
        G_yield_spring_sim->SetPointError(i_spring,0,std::sqrt(sim_counts)/tot_charge_cut);
        G_Epdiff_spring->SetPoint(i_spring,hms_momentum,Eprimediff_mean);
        G_Epdiff_angle_spring->SetPoint(i_spring,hms_angle*180/M_PI,Eprimediff_mean);
        G_W_momentum_spring->SetPoint(i_spring,hms_momentum,W_mean_diff);
        G_W_angle_spring->SetPoint(i_spring,hms_angle*180/M_PI,W_mean_diff);
        G_W_Epdiff_spring->SetPoint(i_spring,Eprimediff_mean,W_mean_diff);
        i_spring++;

      }
      TCanvas *c_Q2 = new TCanvas();
      c_Q2->Divide(1,2);
      c_Q2->cd(1);
      Q2_sim->SetLineColor(kRed);
      Q2_data->GetXaxis()->SetTitle("Q2");
      Q2_sim->DrawCopy("hist");
      Q2_data->DrawCopy("hist same");
      c_Q2->cd(2);
      TVirtualPad *pad_Q2 = c_Q2->cd(2);
      pad_Q2->SetLogy();
      Q2_sim->SetLineColor(kRed);
      Q2_sim->DrawCopy("hist");
      Q2_data->GetXaxis()->SetTitle("Q2");
      Q2_data->DrawCopy("hist same");
      std::string c_Q2_name = "results/simc_data/Q2_"+std::to_string(RunNumber)+".pdf";
      //std::string c_kin_name = "results/simc_data/W_Q2_"+std::to_string(RunNumber)+"_nocurrentcut.pdf";
      c_Q2->SaveAs(c_Q2_name.c_str());

      TCanvas *c_W = new TCanvas();
      c_W->Divide(1,2);
      c_W->cd(1);
      W_sim->SetLineColor(kRed);
      W_sim->DrawCopy("hist");
      W_data->DrawCopy("hist same");
      W_sim->GetXaxis()->SetTitle("W");
      W_data_fit->DrawCopy("same");
      W_sim_fit->SetLineColor(kRed);
      W_sim_fit->DrawCopy("same");
      c_W->cd(2);
      TVirtualPad *pad_W = c_W->cd(2);
      pad_W->SetLogy();
      W_sim->SetLineColor(kRed);
      W_data->GetXaxis()->SetTitle("W");
      W_sim->DrawCopy("hist");
      W_data->DrawCopy("hist same");
      std::string c_W_name = "results/simc_data/W_"+std::to_string(RunNumber)+".pdf";
      //std::string c_kin_name = "results/simc_data/W_W_"+std::to_string(RunNumber)+"_nocurrentcut.pdf";
      c_W->SaveAs(c_W_name.c_str());

      TCanvas* c_Ediff = new TCanvas();
      Eprime_diff->GetXaxis()->SetTitle("Eprime diff");
      Eprime_diff->DrawCopy("hist");
      std::string c_Ediff_name = "results/simc_data/Ediff_"+std::to_string(RunNumber)+".pdf";
      c_Ediff->SaveAs(c_Ediff_name.c_str());
      
      TCanvas* c_W_xfp_data = new TCanvas();
      W_xfp_data->GetXaxis()->SetTitle("xfp");
      W_xfp_data->GetYaxis()->SetTitle("W");
      W_xfp_data->DrawCopy("colz");
      std::string c_W_xfp_data_name = "results/simc_data/W_xfp_"+std::to_string(RunNumber)+"_data.pdf";
      c_W_xfp_data->SaveAs(c_W_xfp_data_name.c_str());
      TCanvas* c_W_xfp_sim = new TCanvas();
      W_xfp_sim->GetXaxis()->SetTitle("xfp");
      W_xfp_sim->GetYaxis()->SetTitle("W");
      W_xfp_sim->DrawCopy("colz");
      std::string c_W_xfp_sim_name = "results/simc_data/W_xfp_"+std::to_string(RunNumber)+"_sim.pdf";
      c_W_xfp_sim->SaveAs(c_W_xfp_sim_name.c_str());
      TCanvas* c_W_xpfp_data = new TCanvas();
      W_xpfp_data->GetXaxis()->SetTitle("xpfp");
      W_xpfp_data->GetYaxis()->SetTitle("W");
      W_xpfp_data->DrawCopy("colz");
      std::string c_W_xpfp_data_name = "results/simc_data/W_xpfp_"+std::to_string(RunNumber)+"_data.pdf";
      c_W_xpfp_data->SaveAs(c_W_xpfp_data_name.c_str());
      TCanvas* c_W_xpfp_sim = new TCanvas();
      W_xpfp_sim->DrawCopy("colz");
      std::string c_W_xpfp_sim_name = "results/simc_data/W_xpfp_"+std::to_string(RunNumber)+"_sim.pdf";
      c_W_xpfp_sim->SaveAs(c_W_xpfp_sim_name.c_str());
      
      TCanvas* c_Emiss = new TCanvas();
      Emiss_data->GetXaxis()->SetTitle("Emiss");
      Emiss_data->DrawCopy("hist");
      Emiss_sim->SetLineColor(kRed);
      Emiss_sim->DrawCopy("hist same");
      std::string c_Emiss_name = "results/simc_data/Emiss_"+std::to_string(RunNumber)+".pdf";
      c_Emiss->SaveAs(c_Emiss_name.c_str());

  }
  TCanvas* c_fall = new TCanvas();
  auto mg_fall = new TMultiGraph(); 
  G_yield_fall_data->SetMarkerStyle(20);
  G_yield_fall_data->SetLineColor(4);
  G_yield_fall_sim->GetYaxis()->SetTitle("yield");
  G_yield_fall_sim->SetMarkerStyle(21);
  G_yield_fall_sim->SetMarkerColor(2);
  G_yield_fall_sim->SetLineColor(2);
  mg_fall->Add(G_yield_fall_data);
  mg_fall->Add(G_yield_fall_sim);
  mg_fall->Draw("ap");
  c_fall->BuildLegend(0.7,0.7,0.95,0.95);
  //c_fall->SaveAs("results/simc_data/fall_yield_nocurrentcut.pdf");
  c_fall->SaveAs("results/simc_data/fall_yield.pdf");
  TCanvas* c_spring = new TCanvas();
  auto mg_spring = new TMultiGraph(); 
  G_yield_spring_data->SetMarkerStyle(20);
  G_yield_spring_data->SetLineColor(4);
  G_yield_spring_sim->SetMarkerStyle(21);
  G_yield_spring_sim->SetMarkerColor(2);
  G_yield_spring_sim->SetLineColor(2);
  mg_spring->Add(G_yield_spring_data);
  mg_spring->Add(G_yield_spring_sim);
  mg_spring->Draw("ap");
  c_spring->BuildLegend(0.0,0.8,0.15,0.95);
  //c_spring->SaveAs("results/simc_data/spring_yield_nocurrentcut.pdf");
  c_spring->SaveAs("results/simc_data/spring_yield.pdf");

  TCanvas* c_Epdiff = new TCanvas();
  auto mg_Epdiff = new TMultiGraph();
  G_Epdiff_fall->SetMarkerStyle(8);
  G_Epdiff_fall->SetMarkerColor(4);
  G_Epdiff_spring->SetMarkerStyle(8);
  G_Epdiff_spring->SetMarkerColor(2);
  G_Epdiff_spring->GetXaxis()->SetTitle("momentum");
  G_Epdiff_spring->GetYaxis()->SetTitle("Eprime difference");
  mg_Epdiff->Add(G_Epdiff_fall);
  mg_Epdiff->Add(G_Epdiff_spring);
  mg_Epdiff->Draw("ap");
  c_Epdiff->SaveAs("results/simc_data/epdiff.pdf");

  TCanvas* c_Epdiff_angle = new TCanvas();
  auto mg_Epdiff_angle = new TMultiGraph();
  G_Epdiff_angle_fall->SetMarkerStyle(8);
  G_Epdiff_angle_fall->SetMarkerColor(4);
  G_Epdiff_angle_spring->SetMarkerStyle(8);
  G_Epdiff_angle_spring->SetMarkerColor(2);
  G_Epdiff_angle_spring->GetXaxis()->SetTitle("angle");
  G_Epdiff_angle_spring->GetYaxis()->SetTitle("Eprime difference");
  mg_Epdiff_angle->Add(G_Epdiff_angle_fall);
  mg_Epdiff_angle->Add(G_Epdiff_angle_spring);
  mg_Epdiff_angle->Draw("ap");
  c_Epdiff_angle->SaveAs("results/simc_data/epdiff_angle.pdf");
  
  TCanvas* c_W_momentum = new TCanvas();
  auto mg_W_momentum = new TMultiGraph();
  G_W_momentum_fall->SetMarkerStyle(8);
  G_W_momentum_fall->SetMarkerColor(4);
  G_W_momentum_spring->SetMarkerStyle(8);
  G_W_momentum_spring->SetMarkerColor(2);
  G_W_momentum_spring->GetXaxis()->SetTitle("momentum");
  G_W_momentum_spring->GetYaxis()->SetTitle("W difference");
  mg_W_momentum->Add(G_W_momentum_fall);
  mg_W_momentum->Add(G_W_momentum_spring);
  mg_W_momentum->GetXaxis()->SetTitle("momentum");
  mg_W_momentum->GetYaxis()->SetTitle("W difference");
  mg_W_momentum->Draw("ap");
  c_W_momentum->SaveAs("results/simc_data/W_momentum.pdf");

  TCanvas* c_W_angle = new TCanvas();
  auto mg_W_angle = new TMultiGraph();
  G_W_angle_fall->SetMarkerStyle(8);
  G_W_angle_fall->SetMarkerColor(4);
  G_W_angle_spring->SetMarkerStyle(8);
  G_W_angle_spring->SetMarkerColor(2);
  G_W_angle_spring->GetXaxis()->SetTitle("angle");
  G_W_angle_spring->GetYaxis()->SetTitle("W difference");
  mg_W_angle->Add(G_W_angle_fall);
  mg_W_angle->Add(G_W_angle_spring);
  mg_W_angle->GetXaxis()->SetTitle("angle");
  mg_W_angle->GetYaxis()->SetTitle("W difference");
  mg_W_angle->Draw("ap");
  c_W_angle->SaveAs("results/simc_data/W_angle.pdf");
  
  TCanvas* c_W_Epdiff = new TCanvas();
  auto mg_W_Epdiff = new TMultiGraph();
  G_W_Epdiff_fall->SetMarkerStyle(8);
  G_W_Epdiff_fall->SetMarkerColor(4);
  G_W_Epdiff_spring->SetMarkerStyle(8);
  G_W_Epdiff_spring->SetMarkerColor(2);
  G_W_Epdiff_spring->GetXaxis()->SetTitle("Epdiff");
  G_W_Epdiff_spring->GetYaxis()->SetTitle("W difference");
  mg_W_Epdiff->Add(G_W_Epdiff_fall);
  mg_W_Epdiff->Add(G_W_Epdiff_spring);
  mg_W_Epdiff->GetXaxis()->SetTitle("Epdiff");
  mg_W_Epdiff->GetYaxis()->SetTitle("W difference");
  mg_W_Epdiff->Draw("ap");
  c_W_Epdiff->SaveAs("results/simc_data/W_Epdiff.pdf");
}
