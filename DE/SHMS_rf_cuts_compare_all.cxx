#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "TGraphErrors.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TStyle.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
double c = 299792458;
double shms_length = 20.1;
double t_e(double p){
  double m = 0.00051;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_pi(double p){
  double m = 0.139;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K(double p){
  double m = 0.493;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}

void SHMS_rf_cuts_compare_all(int RunGroup = 0){
  if(RunGroup ==0){
    std::cout<<"Enter RunGroup (-1 to exit)";
    cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }

  // RunGroup = RunGroup;
  json j_rungroup;
  {
    std::ifstream infile("db2/ratio_run_group_updated.json");
    infile>>j_rungroup;
  }
  double shms_p = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  std::cout<<"check , momentum "<<shms_p<<std::endl;
  double time_diff = t_K(shms_p) - t_pi(shms_p);  
  //I already write cuts into 
  json j_cuts;
  {
    std::string infilename = "db2/PID_test.json";
    std::ifstream infile(infilename.c_str());
    infile>>j_cuts;
  }

      json j_DE;
      {
        std::string infilename = "results/pid/rftime/rf_eff_"+std::to_string(RunGroup)+".json";
        std::cout<<"json file "<<infilename<<std::endl;
        std::ifstream infile(infilename.c_str());
        infile>>j_DE;
      }
      std::vector<double> rf_cuts = j_cuts["SHMS"]["rf_time_cuts"].get<std::vector<double>>();
      std::vector<double> rf_cuts_real;
      TGraphErrors *g_rf_pi = new TGraphErrors();
      g_rf_pi->SetTitle("Pos. pi efficiency;Percentage;Pion efficiency");
      g_rf_pi->SetMarkerStyle(8);
      g_rf_pi->SetMarkerSize(0.75);
      g_rf_pi->SetMarkerColor(1);
      g_rf_pi->SetLineColor(1);
      g_rf_pi->SetFillColor(3);

      TGraphErrors *g_rf_K = new TGraphErrors();
      g_rf_K->SetTitle("Pos. pi purity;Percentage;pion purity");
      g_rf_K->SetMarkerStyle(8);
      g_rf_K->SetMarkerSize(0.75);
      g_rf_K->SetMarkerColor(2);
      g_rf_K->SetLineColor(2);
      g_rf_K->SetFillColor(3);

      TGraphErrors *g_rf_pi_neg = new TGraphErrors();
      g_rf_pi_neg->SetTitle("Neg. pi efficiency;Percentage;pion efficiency");
      g_rf_pi_neg->SetMarkerStyle(5);
      g_rf_pi_neg->SetMarkerSize(0.75);
      g_rf_pi_neg->SetMarkerColor(1);
      g_rf_pi_neg->SetLineColor(1);
      g_rf_pi_neg->SetFillColor(3);

      TGraphErrors *g_rf_K_neg = new TGraphErrors();
      g_rf_K_neg->SetTitle("Neg.pi purity;Percentage;pion purity");
      g_rf_K_neg->SetMarkerStyle(5);
      g_rf_K_neg->SetMarkerSize(0.75);
      g_rf_K_neg->SetMarkerColor(2);
      g_rf_K_neg->SetLineColor(2);
      g_rf_K_neg->SetFillColor(3);
      
      std::cout<<"check"<<std::endl;
      std::vector<double> rf_pion,rf_K;
      double rf_pion_all,rf_K_all;
      rf_pion_all = j_DE[(std::to_string(RunGroup)).c_str()]["pos"]["pi_eff_all"].get<double>();
      rf_K_all = j_DE[(std::to_string(RunGroup)).c_str()]["pos"]["pi"].get<double>();
      rf_pion = j_DE[(std::to_string(RunGroup)).c_str()]["pos"]["pi_eff_Ns"].get<std::vector<double>>();
      rf_K = j_DE[(std::to_string(RunGroup)).c_str()]["pos"]["Ks"].get<std::vector<double>>();
      int n_rf_cuts = (int)rf_cuts.size();
      std::vector<double> rf_pion_neg,rf_K_neg;
      double rf_pion_all_neg,rf_K_all_neg;
      rf_pion_all_neg = j_DE[(std::to_string(RunGroup)).c_str()]["neg"]["pi_eff_all"].get<double>();
      rf_K_all_neg = j_DE[(std::to_string(RunGroup)).c_str()]["neg"]["pi"].get<double>();
      rf_pion_neg = j_DE[(std::to_string(RunGroup)).c_str()]["neg"]["pi_eff_Ns"].get<std::vector<double>>();
      rf_K_neg = j_DE[(std::to_string(RunGroup)).c_str()]["neg"]["Ks"].get<std::vector<double>>();
      for(int i = 0;i<n_rf_cuts;++i){
        //rf_cuts_real[i] = 1+(rf_cuts[i]/100)*time_diff;
        double pi_eff = rf_pion[i]/rf_pion_all;
        double pi_eff_error = 1/rf_pion_all*sqrt(rf_pion[i]*(1-pi_eff));
        g_rf_pi->SetPoint(i,rf_cuts[i],pi_eff);
        g_rf_pi->SetPointError(i,0,pi_eff_error);
        double K_con = 1-rf_K[i]/rf_pion[i];
        double K_eff_error = 1/rf_K_all*sqrt(rf_K[i]*(K_con));
        g_rf_K->SetPoint(i,rf_cuts[i],K_con);
        g_rf_K->SetPointError(i,0,K_eff_error);

        double pi_eff_neg = rf_pion_neg[i]/rf_pion_all_neg;
        double pi_eff_neg_error = 1/rf_pion_all_neg*sqrt(rf_pion_neg[i]*(1-pi_eff_neg));
        g_rf_pi_neg->SetPoint(i,rf_cuts[i],pi_eff_neg);
        g_rf_pi_neg->SetPointError(i,0,pi_eff_neg_error);
        double K_con_neg = 1-rf_K_neg[i]/rf_pion_neg[i];
        double K_con_neg_error = 1/rf_K_all_neg*sqrt(rf_K_neg[i]*(K_con_neg));
        g_rf_K_neg->SetPoint(i,rf_cuts[i],K_con_neg);
        g_rf_K_neg->SetPointError(i,0,K_con_neg_error);
        
      }

      gStyle->Reset("Modern");
      gStyle->SetOptTitle(0);

      TCanvas *c_rf = new TCanvas();
      TPad *p1_rf = new TPad("p1_rf","",0,0,1,1);
      p1_rf->SetGrid();
      TPad *p2_rf = new TPad("p2_rf","",0,0,1,1);
      p2_rf->SetFillStyle(4000);

      p1_rf->Draw();
      p1_rf->cd();
      g_rf_pi->Draw("ALP");
      g_rf_pi->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
      g_rf_pi->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
      g_rf_pi_neg->Draw("LP");
      gPad->Update();

      Style_t tfont_rf = g_rf_pi->GetHistogram()->GetYaxis()->GetTitleFont();
      Float_t tsize_rf = g_rf_pi->GetHistogram()->GetYaxis()->GetTitleSize();
      Style_t lfont_rf = g_rf_pi->GetHistogram()->GetYaxis()->GetLabelFont();
      Float_t lsize_rf = g_rf_pi->GetHistogram()->GetYaxis()->GetLabelSize();

      Double_t xmin_rf = p1_rf->GetUxmin();
      Double_t xmax_rf = p1_rf->GetUxmax();
      Double_t dx_rf = (xmax_rf-xmin_rf)/0.8;
      Double_t ymin_rf = g_rf_K->GetHistogram()->GetMinimum();
      Double_t ymax_rf = g_rf_K->GetHistogram()->GetMaximum();
      Double_t dy_rf =(ymax_rf - ymin_rf)/0.8;
      p2_rf->Range(xmin_rf - 0.1*dx_rf, ymin_rf - 0.1*dy_rf, xmax_rf + 0.1*dx_rf,ymax_rf + 0.1*dy_rf);
      p2_rf->Draw();
      p2_rf->cd();
      g_rf_K->Draw("LP");
      g_rf_K_neg->Draw("LP");
      gPad->Update();

      TGaxis *axis_rf = new TGaxis(xmax_rf, ymin_rf, xmax_rf, ymax_rf, ymin_rf, ymax_rf, 510, "+L");
      axis_rf->SetTitle("Pion purity");
      axis_rf->SetTitleFont(tfont_rf);
      axis_rf->SetTitleSize(tsize_rf);
      axis_rf->SetTitleColor(kRed);
      axis_rf->SetTitleOffset(1.25);
      axis_rf->SetLabelFont(lfont_rf);
      axis_rf->SetLabelSize(lsize_rf);
      axis_rf->SetLabelColor(kRed);
      axis_rf->SetLineColor(kRed);
      axis_rf->Draw();
      gPad->Update();

      TLegend *leg_rf = new TLegend(0.30,0.1,0.60,0.35);
      leg_rf->SetFillColor(gPad->GetFillColor());
      leg_rf->SetTextFont(lfont_rf);
      leg_rf->SetTextSize(lsize_rf);
      std::string rf_title = "#"+std::to_string(RunGroup)+", SHMS p: "+std::to_string(shms_p);
      leg_rf->SetHeader(rf_title.c_str());
      std::cout<<time_diff<<std::endl;
      std::string kaontime = "Kaon time at "+std::to_string((1+time_diff));
      leg_rf->AddEntry((TObject*)0,kaontime.c_str(),"");
      leg_rf->AddEntry(g_rf_pi,"Pos. pi eff","lp");
      leg_rf->AddEntry(g_rf_K,"Pos. pi purity","lp");
      leg_rf->AddEntry(g_rf_pi_neg,"Neg. pi eff","lp");
      leg_rf->AddEntry(g_rf_K_neg,"Neg. pi purity","lp");
      leg_rf->Draw();
      gPad->Update();

      std::string rf_name = "results/pid/SHMS_rf_"+std::to_string(RunGroup)+"_pos.pdf";
      c_rf->SaveAs(rf_name.c_str());


}
