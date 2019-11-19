#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void statis_kin_plot(){

  json j;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j;
  }
  json j_info;
  {
    std::ifstream runs("db2/run_info_group.json");
    runs>>j_info;
  }
  json j_list;
  {
    std::ifstream runs("")
  }

  int coolcolor[11] = {4,3,7,39,38,37,36,35,34,33,32};
  int warmcolor[11] = {2,6,46,45,44,43,42,41,40,47,48};

  for(json::iterator it = j.begin();it!=j.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    for(json::iterator it = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5);
      TCanvas *c_x_Q2 = new TCanvas("",canvas_name.c_str(),1900,1000);
      int i_color = 0;

      for(json::iterator it = j_z.begin();it!=j_z.end();++it){
        TH1D *h_z_neg_all = new TH1D("",(canvas_name).c_str(),100,0,1);
        h_z_neg_all->GetXaxis()->SetTitle("z");
        h_z_neg_all->GetYaxis()->SetTitle("counts");
        TH1D *h_z_pos_all = new TH1D("",(canvas_name).c_str(),100,0,1);
        h_z_pos_all->GetXaxis()->SetTitle("z");
        h_z_pos_all->GetYaxis()->SetTitle("counts");

        double z = std::stod(it.key());
        std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
        std::vector<int> neg_D2_runs, pos_D2_runs;
        auto runjs = it.value();
        if(xbj!=0 && Q2!=0 && z!=0){
          neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
          pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
          for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
            int RunNumber = *it;
            std::cout<<RunNumber<<std::endl;
            double shms_p = j_info[std::to_string(RunNumber)]["shms_p"].get<double>();
            std::cout<<shms_p<<std::endl;
            TFile *root_file_neg = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            TH1D *h_z_neg = new TH1D("","",100,0,1);
            if(shms_p<2.7){
              if(root_file_neg->Get("z_aero")){
              h_z_neg = (TH1D*)root_file_neg->Get("z_aero");
            h_z_neg_all->Add(h_z_neg); 
              }
              else{
                std::cout<<"can't get histo"<<std::endl;
              }
            }
            else{
            h_z_neg = (TH1D*)root_file_neg->Get("z_hgcer");
            h_z_neg_all->Add(h_z_neg);  
            }
          }//loop over neg runs
          int n_neg = neg_D2_runs.size();
          std::cout<<n_neg<<std::endl;
          for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
            int RunNumber = *it;
            std::cout<<RunNumber<<std::endl;
            double shms_p = j_info[std::to_string(RunNumber)]["shms_p"].get<double>();
            TFile *root_file_pos = new TFile(("results/yield/kinematics_yield_"+std::to_string(RunNumber)+".root").c_str());
            TH1D *h_z_pos = new TH1D("","",100,0,1);
            if(shms_p<2.7){
            h_z_pos = (TH1D*)root_file_pos->Get("z_aero");
            h_z_pos_all->Add(h_z_pos); 
            }
            else{
            h_z_pos = (TH1D*)root_file_pos->Get("z_hgcer");
            h_z_pos_all->Add(h_z_pos);  
            }
          }//loop over pos runs
          c_x_Q2->cd();
          double n_pos = pos_D2_runs.size();
          std::cout<<n_pos<<std::endl;
          h_z_neg_all->SetLineColor(coolcolor[i_color]);
          //h_z_neg_all->SetLineColor(2);
          //std::cout<<"check"<<std::endl;
          h_z_neg_all->SetFillColorAlpha(coolcolor[i_color],0.35);
          std::cout<<"cool color "<<coolcolor[i_color]<<std::endl;
          //h_z_neg_all->SetFillColorAlpha(2,0.35);
          //std::cout<<"check2"<<std::endl;
          //h_z_neg_all->Scale(1.0/double(n_neg));
          h_z_neg_all->Draw("same");
          h_z_neg_all->Draw("hist same");
          h_z_pos_all->SetLineColor(warmcolor[i_color]);
          h_z_pos_all->SetFillColorAlpha(warmcolor[i_color],0.35);
          std::cout<<"warm color "<<warmcolor[i_color]<<std::endl;
          //h_z_pos_all->Scale(1.0/n_pos);
          h_z_pos_all->Draw("same");
          h_z_pos_all->Draw("hist same");
          TLine *z_line = new TLine(z,0,z,10000);  
          z_line->SetLineColor(coolcolor[i_color]);
          z_line->Draw("same");
          ++i_color;
          //std::cout<<i_color<<std::endl;
        }//if not elas runs
      }//loop over json z level
      std::string pdfname = "results/statistics/"+canvas_name+".png";
      c_x_Q2->SaveAs(pdfname.c_str());
    }//loop over json Q2 level 
  }//loop over json x level

}
