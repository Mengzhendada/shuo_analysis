#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "Get_all_eff.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void plot_data_simc_ratio(){

  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j_Q2x;
  }
  json j_info;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_info;
  }
  json j_simc;
  {
    std::ifstream ifs("db2/simc_rungroups_info.json");
    ifs>>j_simc;
  }
  TGraphErrors* g_data_simc_ratio_neg_all = new TGraphErrors();
  TGraphErrors* g_data_simc_ratio_pos_all = new TGraphErrors();
  TGraphErrors* g_data_simc_ratio_H2neg_all = new TGraphErrors();
  TGraphErrors* g_data_simc_ratio_H2pos_all = new TGraphErrors();
  int i_pos_all = 0,i_neg_all = 0,i_H2pos_all=0,i_H2neg_all =0;
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    auto mg_x_all = new TMultiGraph();
    for(json::iterator it  = j_Q2z.begin();it!=j_Q2z.end();++it){
      double Q2 = std::stod(it.key());
      auto j_z = it.value();
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::string q2x_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_datasimc_ratio";
      std::string q2x_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_datasimc_ratio";
      int i_color = 1;
      auto mg_data_simc_ratio = new TMultiGraph();
      //THStack* hs = new THStack("yield_ratio","yield ratio");
      if(xbj !=0 && Q2!=0){  
        for(json::iterator it = j_z.begin();it!=j_z.end();++it){
          double z = std::stod(it.key());
          std::string q2xz_str = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",z:"+std::to_string(z).substr(0,4);
          std::string q2xz_str_filename = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);

          TGraphErrors* g_data_simc_ratio_neg = new TGraphErrors();
          TGraphErrors* g_data_simc_ratio_pos = new TGraphErrors();
          g_data_simc_ratio_neg->SetTitle((q2xz_str+"_D2neg").c_str());
          g_data_simc_ratio_pos->SetTitle((q2xz_str+"_D2pos").c_str());
          TGraphErrors* g_data_simc_ratio_H2neg = new TGraphErrors();
          TGraphErrors* g_data_simc_ratio_H2pos = new TGraphErrors();
          g_data_simc_ratio_H2neg->SetTitle((q2xz_str+"_H2neg").c_str());
          g_data_simc_ratio_H2pos->SetTitle((q2xz_str+"_H2pos").c_str());
          int i_pos = 0,i_neg = 0, i_H2pos = 0, i_H2neg = 0;
          auto runjs = it.value();

          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;

          std::vector<int> neg_D2_runs,pos_D2_runs,neg_H2_runs,pos_H2_runs,neg_Dummy_runs,pos_Dummy_runs;
          bool H2_runs = false;
          if(z!=0){
            neg_D2_runs = runjs["neg"]["D2"].get<std::vector<int>>();
            pos_D2_runs = runjs["pos"]["D2"].get<std::vector<int>>();
            neg_Dummy_runs = runjs["neg"]["Dummy"].get<std::vector<int>>();
            pos_Dummy_runs = runjs["pos"]["Dummy"].get<std::vector<int>>();


            double charge_pos_Dummy=0,charge_neg_Dummy=0,Dummy_pions_neg=0,Dummy_pions_pos=0;
            for(auto it = pos_Dummy_runs.begin();it!=pos_Dummy_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_pos_Dummy += charge;
              //std::cout<<"pos DE check "<<std::endl;

              double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
              double bg_n = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
              double EFF = Get_all_eff(RunNumber);
              double pions_pos = (pion_n-bg_n/6.0)/EFF;
              Dummy_pions_pos+= pions_pos;
            }//loop over Dummy runs
            for(auto it = neg_Dummy_runs.begin();it!=neg_Dummy_runs.end();++it){
              int RunNumber = *it;
              std::cout<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
              charge_neg_Dummy += charge;

              double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
              double bg_n = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
              double EFF = Get_all_eff(RunNumber);
              double pions_neg = (pion_n-bg_n/6.0)/EFF;
              Dummy_pions_neg+= pions_neg;

            }//loop over Dummy runs

            double Dummy_pos_yield = Dummy_pions_pos/charge_pos_Dummy;
            double Dummy_neg_yield = Dummy_pions_neg/charge_neg_Dummy;
            std::cout<<"Dummy Yield"<<Dummy_pos_yield<<" "<<Dummy_neg_yield<<std::endl;
            for(auto it = neg_D2_runs.begin();it!=neg_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<"neg D2"<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();

              double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
              double bg_n = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
              double EFF = Get_all_eff(RunNumber);
              double pions_neg = (pion_n-bg_n/6.0)/EFF;
              double pions_data_yield = pions_neg/charge;
              double pion_yield_inc_norad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["inc_norad"].get<int>();
              double pion_yield_inc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["inc_rad"].get<int>();
              double pion_yield_exc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["exc_rad"].get<int>();
              double pion_yield_delta = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["neg"]["delta"].get<int>();

              double pion_data_simc_ratio = (pions_data_yield-pion_yield_exc_rad-pion_yield_delta-0.245*Dummy_neg_yield)*(1/pion_yield_inc_rad);
              double pion_data_simc_error = std::sqrt(pions_neg)/(charge*pion_yield_inc_rad);
              g_data_simc_ratio_neg->SetPoint(i_neg,RunNumber,pion_data_simc_ratio);          
              g_data_simc_ratio_neg->SetPointError(i_neg,0,pion_data_simc_error);          
              g_data_simc_ratio_neg_all->SetPoint(i_neg_all,RunNumber,pion_data_simc_ratio);
              g_data_simc_ratio_neg_all->SetPointError(i_neg_all,0,pion_data_simc_error);
              i_neg++;
              i_neg_all++;
            }//loop over neg D2 runs
            for(auto it = pos_D2_runs.begin();it!=pos_D2_runs.end();++it){
              int RunNumber = *it;
              std::cout<<"pos D2"<<RunNumber<<std::endl;
              double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();

              double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
              int bg_n_integ = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
              double EFF = Get_all_eff(RunNumber);
              double pions_pos = (pion_n-bg_n/6.0)/EFF;
              double pions_data_yield = pions_pos/charge;
              double pion_yield_inc_norad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["pos"]["inc_norad"].get<int>();
              double pion_yield_inc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["pos"]["inc_rad"].get<int>();
              double pion_yield_exc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["pos"]["exc_rad"].get<int>();
              double pion_yield_delta = j_simc[(std::to_string(RunGroup)).c_str()]["D2"]["pos"]["delta"].get<int>();

              double pion_data_simc_ratio = (pions_data_yield-pion_yield_exc_rad-pion_yield_delta-0.245*Dummy_pos_yield)*(1/pion_yield_inc_rad);
              double pion_data_simc_error = std::sqrt(pions_pos)/(charge*pion_yield_inc_rad);
              g_data_simc_ratio_pos->SetPoint(i_pos,RunNumber,pion_data_simc_ratio);          
              g_data_simc_ratio_pos->SetPointError(i_pos,0,pion_data_simc_error);          
              g_data_simc_ratio_pos_all->SetPoint(i_pos_all,RunNumber,pion_data_simc_ratio);
              g_data_simc_ratio_pos_all->SetPointError(i_pos_all,0,pion_data_simc_error);
              i_pos++;
              i_pos_all++;
            }//loop over pos D2 runs

            if(!runjs["neg"]["H2"].empty() && !runjs["pos"]["H2"].empty()){
              H2_runs = true;
              neg_H2_runs = runjs["neg"]["H2"].get<std::vector<int>>();
              pos_H2_runs = runjs["pos"]["H2"].get<std::vector<int>>();
              for(auto it = neg_H2_runs.begin();it!=neg_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"neg H2"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                // std::cout<<"neg DE check "<<std::endl;

                double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
                double bg_n = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
                double EFF = Get_all_eff(RunNumber);
                double pions_neg = (pion_n-bg_n/6.0)/EFF;
                double pions_data_yield = pions_neg/charge;
                double pion_yield_inc_norad = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["inc_norad"].get<int>();
                double pion_yield_inc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["inc_rad"].get<int>();
                double pion_yield_exc_rad = 0;
                double pion_yield_delta = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["neg"]["delta"].get<int>();

                double pion_data_simc_ratio = (pions_data_yield-pion_yield_exc_rad-pion_yield_delta-0.262*Dummy_neg_yield)*(1/pion_yield_inc_rad);
                double pion_data_simc_error = std::sqrt(pions_neg)/(charge*pion_yield_inc_rad);
                g_data_simc_ratio_H2neg->SetPoint(i_H2neg,RunNumber,pion_data_simc_ratio);          
                g_data_simc_ratio_H2neg->SetPointError(i_H2neg,0,pion_data_simc_error);          
                g_data_simc_ratio_H2neg_all->SetPoint(i_H2neg_all,RunNumber,pion_data_simc_ratio);
                g_data_simc_ratio_H2neg_all->SetPointError(i_H2neg_all,0,pion_data_simc_error);
                i_H2neg++;
                i_H2neg_all++;
              }//loop over neg H2 runs
              for(auto it = pos_H2_runs.begin();it!=pos_H2_runs.end();++it){
                int RunNumber = *it;
                std::cout<<"pos H2"<<RunNumber<<std::endl;
                double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
                //std::cout<<"pos DE check "<<std::endl;

                double pion_n = j_info[(std::to_string(RunNumber)).c_str()]["pion_n_integ"].get<double>();
                double bg_n = j_info[(std::to_string(RunNumber)).c_str()]["bg_n_integ"].get<double>();
                double EFF = Get_all_eff(RunNumber);
                double pions_pos = (pion_n-bg_n/6.0)/EFF;
                double pions_data_yield = pions_pos/charge;
                double pion_yield_inc_norad = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["inc_norad"].get<int>();
                double pion_yield_inc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["inc_rad"].get<int>();
                double pion_yield_exc_rad = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["exc_rad"].get<int>();
                double pion_yield_delta = j_simc[(std::to_string(RunGroup)).c_str()]["H2"]["pos"]["delta"].get<int>();

                double pion_data_simc_ratio = (pions_data_yield-pion_yield_exc_rad-pion_yield_delta-0.262*Dummy_pos_yield)*(1/pion_yield_inc_rad);
                double pion_data_simc_error = std::sqrt(pions_pos)/(charge*pion_yield_inc_rad);
                g_data_simc_ratio_H2pos->SetPoint(i_H2pos,RunNumber,pion_data_simc_ratio);          
                g_data_simc_ratio_H2pos->SetPointError(i_H2pos,0,pion_data_simc_error);          
                g_data_simc_ratio_H2pos_all->SetPoint(i_H2pos_all,RunNumber,pion_data_simc_ratio);
                g_data_simc_ratio_H2pos_all->SetPointError(i_H2pos_all,0,pion_data_simc_error);
                i_H2pos++;
                i_H2pos_all++;
              }//loop over pos H2 runs

            }
          }//if z not 0
          g_data_simc_ratio_neg->SetMarkerStyle(4);
          g_data_simc_ratio_neg->SetMarkerColor(i_color);
          g_data_simc_ratio_pos->SetMarkerStyle(5);
          g_data_simc_ratio_pos->SetMarkerColor(i_color);
          mg_data_simc_ratio->Add(g_data_simc_ratio_neg,"P");
          mg_data_simc_ratio->Add(g_data_simc_ratio_pos,"P");
          if(H2_runs){
            g_data_simc_ratio_H2neg->SetMarkerStyle(20);
            g_data_simc_ratio_H2neg->SetMarkerColor(i_color);
            g_data_simc_ratio_H2pos->SetMarkerStyle(47);
            g_data_simc_ratio_H2pos->SetMarkerColor(i_color);
            mg_data_simc_ratio->Add(g_data_simc_ratio_H2neg,"P");
            mg_data_simc_ratio->Add(g_data_simc_ratio_H2pos,"P");
          }
          i_color++;
        }//loop over z
        TCanvas* c_data_simc_ratio = new TCanvas();
        mg_data_simc_ratio->Draw("AP");
        c_data_simc_ratio->BuildLegend();
        std::string c_data_simc_ratio_name = "results/yield/run_info_pdfs/"+canvas_filename+"_data_simc_ratio.pdf";
        c_data_simc_ratio->SaveAs(c_data_simc_ratio_name.c_str());
      }//if xbjQ2 not 0
    }//loop over Q2
  }//loop over xbj
  g_data_simc_ratio_neg_all->SetMarkerStyle(4);
  g_data_simc_ratio_neg_all->SetMarkerColor(kBlue);
  g_data_simc_ratio_pos_all->SetMarkerStyle(5);
  g_data_simc_ratio_pos_all->SetMarkerColor(kRed);
  g_data_simc_ratio_H2neg_all->SetMarkerStyle(20);
  g_data_simc_ratio_H2neg_all->SetMarkerColor(kBlue);
  g_data_simc_ratio_H2pos_all->SetMarkerStyle(47);
  g_data_simc_ratio_H2pos_all->SetMarkerColor(kRed);
  TCanvas *c_data_simc_ratio_all = new TCanvas();
  g_data_simc_ratio_neg_all->Draw("AP");
  g_data_simc_ratio_pos_all->Draw("P same");
  g_data_simc_ratio_H2neg_all->Draw("P same");
  g_data_simc_ratio_H2pos_all->Draw("P same");
  c_data_simc_ratio_all->SaveAs("results/yield/run_info_pdfs/data_simc_ratio_all.pdf");


}
