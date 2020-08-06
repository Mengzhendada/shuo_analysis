#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

void plot_Q2x_June(){
  json j;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j;
  }
  
  json j_info;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_info;
  }

  for(json::iterator it = j.begin();it!=j.end();++it){
    double xbj_old = std::stod(it.key());
    int xbj_int = 100*xbj_old;
    double xbj = (double)xbj_int/100.;
    auto j_Q2z = it.value();
    for(json::iterator it = j_Q2z.begin();it!= j_Q2z.end();++it){
      double Q2_old = std::stod(it.key());
      int Q2_int = Q2_old*1000;
      double Q2 = (double)Q2_int/1000.;
      auto j_z = it.value();
      std::string canvas_name = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5);
      TCanvas *c_x_Q2 = new TCanvas(canvas_name.c_str(),canvas_name.c_str(),1900,1000);
      auto mg_charge = new TMultiGraph();
      auto mg_counts = new TMultiGraph();
      auto mg_yield = new TMultiGraph();
      TGraphErrors* g_charge_neg = new TGraphErrors();
      TGraphErrors* g_charge_pos = new TGraphErrors();
      TGraphErrors* g_counts_neg = new TGraphErrors();
      TGraphErrors* g_counts_pos = new TGraphErrors();
      TGraphErrors* g_yield_neg = new TGraphErrors();
      TGraphErrors* g_yield_pos = new TGraphErrors();
      TGraphErrors* g_yield_ratio = new TGraphErrors();
    g_charge_neg->GetXaxis()->SetLimits(0.2,0.8); 
    g_charge_pos->GetXaxis()->SetLimits(0.2,0.8); 
    g_counts_neg->GetXaxis()->SetLimits(0.2,0.8);
    g_counts_pos->GetXaxis()->SetLimits(0.2,0.8);
    g_yield_neg->GetXaxis()->SetLimits(0.2,0.8);
    g_yield_pos->GetXaxis()->SetLimits(0.2,0.8);
    g_yield_ratio->GetXaxis()->SetLimits(0.2,0.8);
      int i_z = 0;
      for(json::iterator it = j_z.begin();it!=j_z.end();++it){
        double z = std::stod(it.key());
        std::cout<<"xbj "<<xbj<<" Q2 "<<Q2<<" z "<<z<<std::endl;
        double charge_neg = 0,charge_pos = 0,yield_neg = 0, yield_pos = 0,yield_ratio = 0;
        int counts_neg = 0,counts_pos = 0;
        std::vector<int> neg_D2_runs,pos_D2_runs;
        auto runjs = it.value();
        if(xbj!=0 && Q2!= 0 && z!=0){
        int group_num = runjs["group_num"].get<int>();
        std::cout<<group_num<<std::endl;
          neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
          pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
          for(auto it = neg_D2_runs.begin();it!= neg_D2_runs.end();++it){
          int RunNumber = *it;
          std::cout<<RunNumber<<std::endl;
            if(j_info.find(std::to_string(RunNumber))!=j_info.end()){
          double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
          int counts = j_info[(std::to_string(RunNumber)).c_str()]["pion_n"].get<int>();
                int bg_counts = j_info[(std::to_string(RunNumber)).c_str()]["bg_n"].get<int>();
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
          if(counts == 0){std::cout<<"no info for "<<RunNumber<<std::endl;}
                else{std::cout<<"good "<<RunNumber<<std::endl;}
          charge_neg +=charge;
          counts_neg +=(counts-bg_counts)/TE;
          //counts_neg += counts/TE;
          }
          else{std::cout<<"no "<<RunNumber<<" RunGroup "<<group_num<<std::endl;}
          }
          for(auto it = pos_D2_runs.begin();it!= pos_D2_runs.end();++it){
            int RunNumber = *it;
            if(j_info.find(std::to_string(RunNumber))!=j_info.end()){
             
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                int counts = j_info[(std::to_string(RunNumber)).c_str()]["pion_n"].get<int>();
                int bg_counts = j_info[(std::to_string(RunNumber)).c_str()]["bg_n"].get<int>();
                double TE = j_info[(std::to_string(RunNumber)).c_str()]["TE"].get<double>();
                if(counts == 0){std::cout<<"no info for "<<RunNumber<<std::endl;}
                else{std::cout<<"good "<<RunNumber<<std::endl;}
                  charge_pos +=charge;
                counts_pos +=(counts-bg_counts)/TE;
                //  counts_pos +=counts/TE;    
            }
                else{std::cout<<"no "<<RunNumber<<" RunGroup "<<group_num<<std::endl;}
                }
        }//if not 0
       yield_neg = (double)counts_neg/charge_neg;
       yield_pos = (double)counts_pos/charge_pos;
       yield_ratio = (double)yield_neg/yield_pos;
       g_charge_neg->SetPoint(i_z,z,charge_neg);
       g_charge_pos->SetPoint(i_z,z,charge_pos);
       g_counts_neg->SetPoint(i_z,z,counts_neg);
       g_counts_pos->SetPoint(i_z,z,counts_pos);
       g_yield_neg->SetPoint(i_z,z,yield_neg);
       g_yield_neg->SetPointError(i_z,0,std::sqrt(counts_neg)/charge_neg);
       g_yield_pos->SetPoint(i_z,z,yield_pos);
       g_yield_pos->SetPointError(i_z,0,std::sqrt(counts_pos)/charge_pos);
       g_yield_ratio->SetPoint(i_z,z,yield_ratio);
       g_yield_ratio->SetPointError(i_z,0,yield_ratio/std::sqrt(counts_neg+counts_pos));
       ++i_z;
      }//loop 
      std::string title = "x_"+std::to_string(xbj).substr(0,4)+"_Q2_"+std::to_string(Q2).substr(0,5);
     // c_x_Q2->Divide(2,1);
     // c_x_Q2->cd(1);
     // g_yield_neg->SetMarkerStyle(20);
     // g_yield_neg->SetMarkerColor(2);
     // g_yield_neg->SetTitle("pi-");
     // mg_yield->Add(g_yield_neg,"p");
     // g_yield_pos->SetMarkerStyle(21);
     // g_yield_pos->SetMarkerColor(4);
     // g_yield_pos->SetTitle("pi+");
     // mg_yield->Add(g_yield_pos,"p");
     // //mg_yield->SetTitle(title.c_str());
     // mg_yield->GetXaxis()->SetTitle("z");
     // mg_yield->GetYaxis()->SetTitle("yield");
     // //mg_yield->GetXaxis()->SetLimits(0.2,0.8);
     // mg_yield->Draw("a");
     // auto legend_yield = gPad->BuildLegend(0.7,0.7,0.9,0.9);
     // legend_yield->SetHeader(title.c_str(),"C");
     // gPad->Update();
     // c_x_Q2->cd(2);
      TF1 *fit = new TF1("HERMES","(1.0-x)**0.083583/(1.0+x)**1.9838",0.4,0.7);
      
      g_yield_ratio->GetXaxis()->SetTitle("z");
      g_yield_ratio->GetYaxis()->SetTitle("pi-/pi+");
      g_yield_ratio->SetMarkerStyle(21);
      g_yield_ratio->GetYaxis()->SetRangeUser(0,1);
      g_yield_ratio->Draw("ap");
      fit->Draw("same");
      //gPad->Draw("same");
      //c_x_Q2->cd(4);
      //TPaveText *pt = new TPaveText(0.05,.1,0.95,.2);
      //pt->AddText("Event type 4 (COIN)");
      //pt->AddText(canvas_name.c_str());
      //pt->Draw();
      //TPaveText *pt_HMS = new TPaveText(0.05,.2,.45,.8);
      //pt_HMS->AddText("HMS Cut");
      //pt_HMS->AddText("-10<dp<10");
      //pt_HMS->AddText("0.8<E/p<2.0");
      //pt_HMS->AddText("cer>1.0");
      //pt_HMS->Draw();
      //TPaveText *pt_SHMS = new TPaveText(0.55,.2,.95,.8);
      //pt_SHMS->AddText("SHMS Cut");
      //pt_SHMS->AddText("-10<dp<22");
      //pt_SHMS->AddText("pr<0.2");
      //pt_SHMS->AddText("E/p<0.6");
      //pt_SHMS->AddText("hgcer>1.0 or ");
      //pt_SHMS->AddText("aero>1.0  & shms_p<2.7");
      //pt_SHMS->Draw();
      //c_x_Q2->Update();
      std::string c_name = "results/statistics/x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5)+"_June.png";
     c_x_Q2->SaveAs(c_name.c_str());
      
      TCanvas *c_x_Q2_check = new TCanvas(canvas_name.c_str(),canvas_name.c_str(),1900,1000);
      c_x_Q2_check->Divide(1,2);
      c_x_Q2_check->cd(1);
      g_charge_neg->SetMarkerStyle(20);
      g_charge_neg->SetMarkerColor(2);
      g_charge_neg->SetTitle("pi-");
      mg_charge->Add(g_charge_neg,"p");
      g_charge_pos->SetMarkerStyle(21);
      g_charge_pos->SetMarkerColor(4);
      g_charge_pos->SetTitle("pi+");
      mg_charge->Add(g_charge_pos,"p");
      //mg_charge->SetTitle(title.c_str());
      mg_charge->GetXaxis()->SetTitle("z");
      mg_charge->GetYaxis()->SetTitle("charge");
      mg_charge->GetXaxis()->SetRangeUser(0,1);
      mg_charge->Draw("a");
      c_x_Q2_check->cd(2);
      g_counts_neg->SetMarkerStyle(20);
      g_counts_neg->SetMarkerColor(2);
      g_counts_neg->SetTitle("pi-");
      mg_counts->Add(g_counts_neg,"p");
      g_counts_pos->SetMarkerStyle(21);
      g_counts_pos->SetMarkerColor(4);
      g_counts_pos->SetTitle("pi+");
      mg_counts->Add(g_counts_pos,"p");
      //mg_counts->SetTitle(title.c_str());
      mg_counts->GetXaxis()->SetTitle("z");
      mg_counts->GetYaxis()->SetTitle("counts");
      mg_counts->GetXaxis()->SetRangeUser(0,1);
      mg_counts->Draw("a");
      std::string c_name_check = "results/statistics/x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5)+"_check.png";
     c_x_Q2_check->SaveAs(c_name_check.c_str());
    }//loop Q2

  }//loop x
}
