
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
#include "TFile.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void SHMS_hgcer_compare(){
  json j_xq2z;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j_xq2z;
  }
  std::vector<int> color = {1,2,3,4,6,7,8,9,30,42,35,28,38,46,47,48,49};
  TCanvas* c_p_pos_all = new TCanvas();
  TCanvas* c_xcer_pos_all = new TCanvas();
  TMultiGraph *mg_hgcer_shmsp_all = new TMultiGraph();
  TMultiGraph *mg_hgcer_xcer_all = new TMultiGraph();
  std::vector<int> HGC_cuts = {0,1,2,3};
  for(auto i_hgccut = HGC_cuts.begin();i_hgccut!=HGC_cuts.end();i_hgccut++){
    int i_hgc_cut = *i_hgccut;
  int i_color_all = 0;
    for(auto it = j_xq2z.begin();it!=j_xq2z.end();it++){
      double x = std::stod(it.key());
      //std::string x_2 = it.key().substr(0,4);
      auto runjs = it.value();
      for(auto ik = runjs.begin();ik!=runjs.end();++ik){
        double Q2 = std::stod(ik.key());
        std::cout<<"x_Q2_"<<x<<"_"<<Q2<<std::endl;
        std::string canvas_name = "x_Q2_"+(std::to_string(x)).substr(0,4)+"_"+(std::to_string(Q2)).substr(0,4);
        if(x!=0 && Q2!=0){
          TCanvas* c_p_pos = new TCanvas();
          TCanvas* c_xcer_pos = new TCanvas();
          TMultiGraph *mg_hgcer_shmsp = new TMultiGraph();
          TMultiGraph *mg_hgcer_xcer = new TMultiGraph();
          int i_color = 0;
          auto ikjs = ik.value();
          for(auto ij = ikjs.begin();ij!= ikjs.end();ij++){       
            double z = std::stod(ij.key());
            auto ijjs = ij.value();
            double shms_p = ijjs["shms_p"].get<double>();
            double RunGroup = ijjs["group_num"].get<int>();
            if(shms_p > 3.0){
              std::cout<<"RunGroup "<<RunGroup<<" with shms momentum "<<shms_p<<" is drawn"<<std::endl;
              std::vector<int> neg_D2,pos_D2;
              neg_D2 = ijjs["neg"]["D2"].get<std::vector<int>>();
              pos_D2 = ijjs["pos"]["D2"].get<std::vector<int>>();
              if(!neg_D2.empty() && !pos_D2.empty()){
                for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
                  int RunNumber = *i_pos;
                  std::string rootfile_name = "results/pid/hgcer/hgcer_eff_"+std::to_string(RunNumber)+"_"+std::to_string(i_hgc_cut)+".root";
                  TFile* rootfile = new TFile(rootfile_name.c_str());
                  std::string shms_p_str = std::to_string(shms_p);
                  TH1F* h_hgcer_shmsp = new TH1F("",shms_p_str.c_str(),200,1,6);
                  TH1F* h_hgcer_xcer = new TH1F("",shms_p_str.c_str(),100,-40,40);
                  if(rootfile->IsZombie()){std::cout<<rootfile_name<<" is a zombie."<<std::endl;}
                  if(rootfile->Get("hgcer_xcer")){
                    h_hgcer_shmsp = (TH1F*)rootfile->Get("hgcer_shmsp");
                    h_hgcer_xcer = (TH1F*)rootfile->Get("hgcer_xcer");
                    std::cout<<"succeed "<<rootfile_name<<std::endl;
                  }
                  else{std::cout<<"no histogram in "<<rootfile_name<<std::endl;}
                  //c_p_pos->cd();

                  h_hgcer_shmsp->Rebin(2);
                  h_hgcer_shmsp->Scale(1.0/2);
                  //h_hgcer_shmsp->GetXaxis()->SetRangeUser(2.8,5);
                  //h_hgcer_shmsp->SetMarkerColor(color[i_color]);
                  //h_hgcer_shmsp->SetLineColor(color[i_color]);
                  gStyle->SetOptTitle(0);
                  TGraphErrors *g_hgcer_shmsp = new TGraphErrors(h_hgcer_shmsp);
                  //h_hgcer_shmsp->Draw("p same");
                  g_hgcer_shmsp->SetMarkerStyle(8);
                  g_hgcer_shmsp->SetMarkerColorAlpha(color[i_color_all],0.55);
                  g_hgcer_shmsp->SetLineColor(color[i_color]);
                  mg_hgcer_shmsp->Add(g_hgcer_shmsp,"P");
                  //g_hgcer_shmsp->Draw("p same");

                  //c_p_pos->BuildLegend(0.75,0.05,0.95,0.55);
                  //c_xcer_pos->cd();
                  h_hgcer_xcer->Rebin(2);
                  h_hgcer_xcer->Scale(1.0/2);
                  //h_hgcer_xcer->GetYaxis()->SetRangeUser(0,1.2);
                  //h_hgcer_xcer->SetMarkerColor(color[i_color]);
                  //h_hgcer_xcer->SetLineColor(color[i_color]);
                  //h_hgcer_cer->SetMarkerStyle()
                  gStyle->SetOptTitle(0);
                  //h_hgcer_xcer->Draw("p same");
                  TGraphErrors *g_hgcer_xcer = new TGraphErrors(h_hgcer_xcer);
                  g_hgcer_xcer->SetMarkerStyle(8);
                  g_hgcer_xcer->SetMarkerColorAlpha(color[i_color_all],0.55);
                  g_hgcer_xcer->SetLineColor(color[i_color]);
                  mg_hgcer_xcer->Add(g_hgcer_xcer,"P");
                  //g_hgcer_xcer->Draw("p same");
                  //c_xcer_pos->BuildLegend(0.75,0.05,0.95,0.55);

                  mg_hgcer_shmsp_all->Add(g_hgcer_shmsp,"P");
                  mg_hgcer_xcer_all->Add(g_hgcer_xcer,"P");
                }//loop over pos D2 runs
              }//if D2 runs not empty
              i_color++;
              i_color_all++;
            }//if momentum is higher than threshold 
            else{std::cout<<"RunGroup "<<RunGroup<<" with shms momentum "<<shms_p<<" is below threshold"<<std::endl;}
          }//loop over z
          c_p_pos->cd();
          mg_hgcer_shmsp->GetXaxis()->SetTitle("SHMS P (GeV)");
          mg_hgcer_shmsp->GetYaxis()->SetTitle("HGC eff");
          mg_hgcer_shmsp->SetMinimum(0.1);
          mg_hgcer_shmsp->SetMaximum(1.4);
          mg_hgcer_shmsp->Draw("A");
          mg_hgcer_shmsp->GetXaxis()->SetLimits(2.5,5.5);
          c_p_pos->BuildLegend(0.75,0.15,0.95,0.45);
          std::string c_p_pos_name = "results/pid/hgcer/"+canvas_name+"_"+std::to_string(i_hgc_cut)+"_shmsp.pdf";
          c_p_pos->SaveAs(c_p_pos_name.c_str());

          c_xcer_pos->cd();
          mg_hgcer_xcer->GetXaxis()->SetTitle("Xcer");
          mg_hgcer_xcer->GetYaxis()->SetTitle("HGC eff");
          mg_hgcer_xcer->SetMinimum(0.1);
          mg_hgcer_xcer->SetMaximum(1.4);
          mg_hgcer_xcer->Draw("A");
          c_xcer_pos->BuildLegend(0.75,0.15,0.95,0.45);
          std::string c_xcer_pos_name = "results/pid/hgcer/"+canvas_name+"_"+std::to_string(i_hgc_cut)+"_xcer.pdf";
          c_xcer_pos->SaveAs(c_xcer_pos_name.c_str());
        }//if xQ2 not empty
        //}//if x,q2 not 0
        else
        {std::cout<<"not production runs"<<std::endl;}

    }//loop over Q2
  }//loop over x
  c_p_pos_all->cd();
  mg_hgcer_shmsp_all->GetXaxis()->SetTitle("SHMS P (GeV)");
  mg_hgcer_shmsp_all->GetYaxis()->SetTitle("HGC eff");
  mg_hgcer_shmsp_all->SetMinimum(0.1);
  mg_hgcer_shmsp_all->SetMaximum(1.4);
  mg_hgcer_shmsp_all->Draw("A");
  mg_hgcer_shmsp_all->GetXaxis()->SetLimits(2.5,5.5);
  c_p_pos_all->BuildLegend(0.75,0.15,0.95,0.45);
  std::string c_p_pos_all_name = "results/pid/hgcer/hgcer_"+std::to_string(i_hgc_cut)+"_shmsp.pdf";
  c_p_pos_all->SaveAs(c_p_pos_all_name.c_str());

  c_xcer_pos_all->cd();
  mg_hgcer_xcer_all->GetXaxis()->SetTitle("Xcer");
  mg_hgcer_xcer_all->GetYaxis()->SetTitle("HGC eff");
  mg_hgcer_xcer_all->SetMinimum(0.1);
  mg_hgcer_xcer_all->SetMaximum(1.4);
  mg_hgcer_xcer_all->Draw("A");
  c_xcer_pos_all->BuildLegend(0.75,0.15,0.95,0.45);
  std::string c_xcer_pos_all_name = "results/pid/hgcer/hgcer_"+std::to_string(i_hgc_cut)+"_xcer.pdf";
  c_xcer_pos_all->SaveAs(c_xcer_pos_all_name.c_str());

}//different hgc cuts
}
