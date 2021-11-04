#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

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
  R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
  namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TDatime.h"

#include <fstream>
#include <vector>
#include <string>
#include <locale>
  void plot_te_spring(){
    json j_te;
    { 
      std::ifstream ifs("results/TE/trackingeff_info.json");
      ifs>>j_te;
    }
    int i = 0;
    int i_neg = 0;
    int i_pos = 0;
    TGraphErrors* G_te_all_pos = new TGraphErrors();
    TGraphErrors* G_te_all_neg = new TGraphErrors();
    G_te_all_pos->SetTitle("SHMS TE;RunGroup;TE");
    TGraphErrors* G_te_all_pos_vs_shmsp = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_shmsp = new TGraphErrors();
    G_te_all_pos_vs_shmsp->SetTitle("SHMS TE; shmsp; TE");
    TGraphErrors* G_te_all_pos_vs_rate = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_rate = new TGraphErrors();
    G_te_all_pos_vs_rate->SetTitle("SHMS TE; rate; TE");
    TGraphErrors* G_te_all_pos_vs_yield = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_yield = new TGraphErrors();
    G_te_all_pos_vs_yield->SetTitle("SHMS TE; rate;raw yield");
    G_te_all_pos_vs_yield->GetXaxis()->SetRangeUser(0,600);
    G_te_all_neg_vs_yield->GetXaxis()->SetRangeUser(0,600);
    TGraphErrors* G_te_all_pos_yieldte = new TGraphErrors();
    TGraphErrors* G_te_all_neg_yieldte = new TGraphErrors();
    G_te_all_pos_yieldte->SetTitle("SHMS TE; rate;TE corrected yield");
    G_te_all_pos_yieldte->GetXaxis()->SetRangeUser(0,600);
    G_te_all_neg_yieldte->GetXaxis()->SetRangeUser(0,600);
    TGraphErrors* G_te_all_pos_yieldteratecorr = new TGraphErrors();
    TGraphErrors* G_te_all_neg_yieldteratecorr = new TGraphErrors();
    G_te_all_pos_yieldteratecorr->GetXaxis()->SetRangeUser(0,600);
    G_te_all_neg_yieldteratecorr->GetXaxis()->SetRangeUser(0,600);
    G_te_all_pos_yieldteratecorr->SetTitle("SHMS TE; rate;rate corrected TE corrected yield");
    TGraphErrors* G_te_all_pos_vs_yieldrate = new TGraphErrors();
    TGraphErrors* G_te_all_neg_vs_yieldrate = new TGraphErrors();
    G_te_all_pos_vs_yieldrate->SetTitle("SHMS TE; yieldrate;TE");
    TGraphErrors* G_te_all_pos_runs = new TGraphErrors();
    TGraphErrors* G_te_all_neg_runs = new TGraphErrors();
    G_te_all_neg_runs->SetTitle("SHMS TE; runs;TE");
    
    TDatime T0(2018,11,7,12,0,0);
    int x0 = T0.Convert();
    TGraph* G_te_all_pos_realtime = new TGraph();
    G_te_all_pos_realtime->GetXaxis()->SetTimeDisplay(1);
    G_te_all_pos_realtime->GetXaxis()->SetNdivisions(503);
    G_te_all_pos_realtime->GetXaxis()->SetTimeFormat("%H");
    //G_te_all_pos_realtime->GetXaxis()->SetTimeFormat("%Y-%m-%d %H");
    G_te_all_pos_realtime->GetXaxis()->SetTimeOffset(x0,"gmt");
    G_te_all_pos_realtime->SetTitle("SHMS TE; 24hours;TE");
    TGraph* G_te_all_neg_realtime = new TGraph();
    G_te_all_neg_realtime->GetXaxis()->SetTimeDisplay(1);
    G_te_all_neg_realtime->GetXaxis()->SetNdivisions(503);
    G_te_all_neg_realtime->GetXaxis()->SetTimeFormat("%H");
    //G_te_all_neg_realtime->GetXaxis()->SetTimeFormat("%Y-%m-%d %H");
    G_te_all_neg_realtime->GetXaxis()->SetTimeOffset(x0,"gmt");
    G_te_all_neg_realtime->SetTitle("SHMS TE; time;TE");
    
    TGraph* G_te_all_neg_ratecorr_vs_rate = new TGraph();
    G_te_all_neg_ratecorr_vs_rate->SetTitle("SHMS TE rate corrected; rate;TE_corr");
    TGraph* G_te_all_pos_ratecorr_vs_rate = new TGraph();

    TGraph* G_te_all_pos_realtime_ratecorr = new TGraph();
    G_te_all_pos_realtime_ratecorr->GetXaxis()->SetTimeDisplay(1);
    G_te_all_pos_realtime_ratecorr->GetXaxis()->SetNdivisions(503);
    G_te_all_pos_realtime_ratecorr->GetXaxis()->SetTimeFormat("%H");
    G_te_all_pos_realtime_ratecorr->GetXaxis()->SetTimeOffset(x0,"gmt");
    G_te_all_pos_realtime_ratecorr->SetTitle("SHMS TE; 24hours;TE");
    gStyle->SetTimeOffset(x0);
    TGraph* G_te_all_neg_realtime_ratecorr = new TGraph();
    G_te_all_neg_realtime_ratecorr->GetXaxis()->SetTimeDisplay(1);
    G_te_all_neg_realtime_ratecorr->GetXaxis()->SetNdivisions(503);
    G_te_all_neg_realtime_ratecorr->GetXaxis()->SetTimeFormat("%H");
    G_te_all_neg_realtime_ratecorr->GetXaxis()->SetTimeOffset(x0,"gmt");
    G_te_all_neg_realtime_ratecorr->SetTitle("SHMS TE; 24hours;TE");
    
    TGraph* G_te_pos_index = new TGraph();
    G_te_pos_index->SetTitle("SHMS TE; #in kin group;TE");
    TGraph* G_te_pos_index_ratecorr = new TGraph();
    G_te_pos_index_ratecorr->SetTitle("SHMS TE; #in kin group;TE_corr");
    
    TGraph* G_te_neg_index = new TGraph();
    G_te_neg_index->SetTitle("SHMS TE; in kin group;TE");
    TGraph* G_te_neg_index_ratecorr = new TGraph();
    G_te_neg_index_ratecorr->SetTitle("SHMS TE; in kin group;TE_corr");
    
    TGraph* G_te_pos_vs_momentum = new TGraph();
    G_te_pos_vs_momentum->SetTitle("SHMS TE;momentum;TE");
    TGraph* G_te_neg_vs_momentum = new TGraph();
    TGraph* G_te_pos_vs_momentum_ratecorr = new TGraph();
    G_te_pos_vs_momentum_ratecorr->SetTitle("SHMS TE;momentum;TE");
    TGraph* G_te_neg_vs_momentum_ratecorr = new TGraph();
    
    for(auto it = j_te.begin();it!=j_te.end();it++){
      int RunGroup;
      RunGroup = std::stoi(it.key());
    int i_index = 0;
    int i_index_neg = 0;
      if(RunGroup>410){
        auto runjs = it.value();
        double momentum = runjs["shms_p"].get<double>();
       // double pi_expected_pos = runjs["pos"]["SHMS_pi_expected"].get<double>();
       // double pi_found_pos_1 = runjs["pos"]["SHMS_pi_found_1"].get<double>();
       // double pi_found_pos_2 = runjs["pos"]["SHMS_pi_found_2"].get<double>();
       // double pi_found_pos_3 = runjs["pos"]["SHMS_pi_found_3"].get<double>();
       // double pi_expected_neg = runjs["neg"]["SHMS_pi_expected"].get<double>();
       // double pi_found_neg_1 = runjs["neg"]["SHMS_pi_found_1"].get<double>();
       // double pi_found_neg_2 = runjs["neg"]["SHMS_pi_found_2"].get<double>();
       // double pi_found_neg_3 = runjs["neg"]["SHMS_pi_found_3"].get<double>();
       // double te_pos_1 = pi_found_pos_1/pi_expected_pos;
       // double te_neg_1 = pi_found_neg_1/pi_expected_neg;
       // G_te_all_pos->SetPoint(i,RunGroup,te_pos_1);
       // G_te_all_neg->SetPoint(i,RunGroup,te_neg_1);
       // i++;
        std::cout<<"check rungroup "<<it.key()<<std::endl;
        for(auto it_neg = runjs["neg"].begin();it_neg!=runjs["neg"].end();it_neg++){
          std::string neg_key = it_neg.key();
          if(std::isdigit(neg_key[0])){
            int RunNumber = std::stoi(it_neg.key());
            auto ik_neg = it_neg.value();
            if(ik_neg.find("data_n") != ik_neg.end() && ik_neg.find("SHMS_pi_expected") != ik_neg.end() && ik_neg.find("time") != ik_neg.end() && ik_neg.find("start_time")!= ik_neg.end()){
              double pi_expected = ik_neg["SHMS_pi_expected"].get<double>();
              double pi_found = ik_neg["SHMS_pi_found_1"].get<double>();
              double charge = ik_neg["charge"].get<double>();
              double time = ik_neg["time"].get<double>();
              double counts = ik_neg["data_n"].get<double>();
              double te = pi_found/pi_expected;
              double yield = pi_found/charge;
              double rate = counts/(1000*time);
              double neg_fit_value;
              double te_corr;
              //if(RunNumber<7000)
              //{ 
              //  neg_fit_value = -2.396e-5*rate+0.98;
              //  te_corr = te*(0.98/neg_fit_value);
              //}
              //else{
              //  neg_fit_value = -2.134e-5*rate+0.974;
              //  te_corr = te*(0.974/neg_fit_value);
              //}
   
              if(rate < 0){std::cout<< "Alert "<<it_neg.key()<<std::endl;}
              double yieldrate = pi_expected/(charge*time);
              double yield_te = yield/te;
              double yield_teratecorr = yield/te_corr;
              G_te_all_neg_vs_rate->SetPoint(i_neg,rate,te);
              G_te_all_neg_vs_yield->SetPoint(i_neg,rate,yield);
              G_te_all_neg_yieldte->SetPoint(i_neg,rate,yield_te);
              G_te_all_neg_yieldteratecorr->SetPoint(i_neg,rate,yield_teratecorr);

              G_te_all_neg_vs_yieldrate->SetPoint(i_neg,yieldrate,te);
              G_te_all_neg_runs->SetPoint(i_neg,RunNumber,te);
              G_te_all_neg_ratecorr_vs_rate->SetPoint(i_neg,rate,te_corr);
              
              std::cout<<"check neg "<<it_neg.key()<<std::endl;
              int start_hour = ik_neg["start_time"]["hour"].get<int>();
              int start_minute = ik_neg["start_time"]["minute"].get<int>();
              int start_second = ik_neg["start_time"]["second"].get<int>();
              int start_month = ik_neg["start_time"]["month"].get<int>();
              int start_date = ik_neg["start_time"]["date"].get<int>();
              int start_year = ik_neg["start_time"]["year"].get<int>();
              start_year = start_year+2000;
              TDatime datime_neg(2018,11,8,start_hour,start_minute,start_second);
              //TDatime datime_neg(start_year,start_month,start_date,start_hour,start_minute,start_second);
              double realtime = datime_neg.Convert();
              G_te_all_neg_realtime->SetPoint(i_neg,realtime,te);
              G_te_all_neg_realtime_ratecorr->SetPoint(i_neg,realtime,te_corr);
              G_te_neg_vs_momentum->SetPoint(i_neg,momentum,te);
              G_te_neg_vs_momentum_ratecorr->SetPoint(i_neg,momentum,te_corr);
            G_te_neg_index->SetPoint(i_neg,i_index_neg,te);
            G_te_neg_index_ratecorr->SetPoint(i_neg,i_index_neg,te_corr);
            i_index_neg++;
              i_neg++;
              //std::cout<<"end of "<<it_neg.key()<<" neg"<<std::endl;
            }
            else{std::cout<<"missing info for "<<it_neg.key()<<std::endl;;}
          }
        }//neg runs
        //for pos runs
        runjs = it.value();
        for(auto it_pos = runjs["pos"].begin();it_pos!=runjs["pos"].end();it_pos++){
          std::string pos_key = it_pos.key();
          if(std::isdigit(pos_key[0])){
            int RunNumber = std::stoi(it_pos.key());
            auto ik_pos = it_pos.value();
            if(ik_pos.find("data_n") != ik_pos.end() && ik_pos.find("SHMS_pi_expected")!=ik_pos.end() && ik_pos.find("time") != ik_pos.end() && ik_pos.find("start_time")!= ik_pos.end()){
              double pi_expected = ik_pos["SHMS_pi_expected"].get<double>();
              double pi_found = ik_pos["SHMS_pi_found_1"].get<double>();
              double charge = ik_pos["charge"].get<double>();
              double time = ik_pos["time"].get<double>();
              double counts = ik_pos["data_n"].get<double>();
              double te = pi_found/pi_expected;
              double yield = pi_found/charge;
              if(yield>2000){std::cout<<"yield greater than 2000 "<<RunNumber<<std::endl;}
              else{
                double rate = counts/(1000*time);
                double pos_fit_value;
                double te_corr;
                //if(RunNumber < 7000){
                //  pos_fit_value = -5.219e-5*rate+0.997;
                //  te_corr = te*(0.997/pos_fit_value);
                //}
                //else{
                //  pos_fit_value = -2.601e-5*rate+0.977;
                //  te_corr = te*(0.977/pos_fit_value);
                //}
                if(rate<0){std::cout<<"Alert "<<it_pos.key()<<std::endl;}
                double yieldrate = pi_expected/(charge*time);
              double yield_te = yield/te;
              double yield_teratecorr = yield/te_corr;
                G_te_all_pos_vs_rate->SetPoint(i_pos,rate,te);
                G_te_all_pos_vs_yield->SetPoint(i_pos,rate,yield);
                G_te_all_pos_yieldte->SetPoint(i_pos,rate,yield_te);
                G_te_all_pos_yieldteratecorr->SetPoint(i_pos,rate,yield_teratecorr);
                G_te_all_pos_vs_yieldrate->SetPoint(i_pos,yieldrate,te);
                G_te_all_pos_runs->SetPoint(i_pos,RunNumber,te);
                G_te_all_pos_ratecorr_vs_rate->SetPoint(i_pos,rate,te_corr);
                std::cout<<"check pos"<<it_pos.key()<<std::endl;
                int start_hour = ik_pos["start_time"]["hour"].get<int>();
                int start_minute = ik_pos["start_time"]["minute"].get<int>();
                int start_second = ik_pos["start_time"]["second"].get<int>();
                int start_month = ik_pos["start_time"]["month"].get<int>();
                int start_date = ik_pos["start_time"]["date"].get<int>();
                int start_year = ik_pos["start_time"]["year"].get<int>();
                start_year = start_year+2000;
                TDatime datime_pos(2018,11,8,start_hour,start_minute,start_second);
                //TDatime datime_pos(start_year,start_month,start_date,start_hour,start_minute,start_second);
                double realtime = datime_pos.Convert();
                G_te_all_pos_realtime->SetPoint(i_pos,realtime,te);
                G_te_all_pos_realtime_ratecorr->SetPoint(i_pos,realtime,te_corr);
                // std::cout<<"end of "<<it_pos.key()<<" pos"<<std::endl;
                G_te_pos_vs_momentum->SetPoint(i_pos,momentum,te);
                G_te_pos_vs_momentum_ratecorr->SetPoint(i_pos,momentum,te_corr);
                G_te_pos_index->SetPoint(i_pos,i_index,te);
                G_te_pos_index_ratecorr->SetPoint(i_pos,i_index,te_corr);
                i_index++;
                i_pos++;
              }
            }
            else{std::cout<<"missing info for "<<it_pos.key()<<std::endl;;}
          }
        }//pos runs
      }//if rungroup greater than 10 
    }//rungroups
   // TCanvas* c_te_all = new TCanvas();
   // c_te_all->SetGrid();
   // G_te_all_pos->SetMarkerColor(kRed);
   // G_te_all_pos->SetMarkerStyle(8);
   // G_te_all_pos->Draw("ap");
   // G_te_all_neg->SetMarkerColor(kBlack);
   // G_te_all_neg->SetMarkerStyle(8);
   // G_te_all_neg->Draw("p");
   // std::string c_te_all_name = "results/TE/pi_te_all.pdf";
   // c_te_all->SaveAs(c_te_all_name.c_str());
    TCanvas* c_te_all_yield = new TCanvas();
    c_te_all_yield->SetGrid();
    G_te_all_pos_vs_yield->SetMarkerColor(kRed);
    G_te_all_pos_vs_yield->SetMarkerStyle(8);
    G_te_all_pos_vs_yield->Draw("ap");
    G_te_all_neg_vs_yield->SetMarkerColor(kBlack);
    G_te_all_neg_vs_yield->SetMarkerStyle(8);
    G_te_all_neg_vs_yield->Draw("p");
    std::string c_te_all_yield_name = "results/TE/pi_te_spring_vs_yield.pdf";
    c_te_all_yield->SaveAs(c_te_all_yield_name.c_str());

    TCanvas* c_te_all_rate = new TCanvas();
    c_te_all_rate->SetGrid();
    G_te_all_pos_vs_rate->SetMarkerColor(kRed);
    G_te_all_pos_vs_rate->SetMarkerStyle(8);
    G_te_all_pos_vs_rate->GetYaxis()->SetRangeUser(0.9,0.96);
    TF1 *pos_fit = new TF1("pol1","pol1",0,500);
  //  pos_fit->FixParameter(0,1);
    G_te_all_pos_vs_rate->Fit(pos_fit);
    pos_fit = G_te_all_pos_vs_rate->GetFunction("pol1");
    pos_fit->SetLineColor(kRed);
    pos_fit->Draw("same");
    G_te_all_pos_vs_rate->Draw("ap");
    gStyle->SetOptFit(1);
    G_te_all_neg_vs_rate->SetMarkerColor(kBlack);
    G_te_all_neg_vs_rate->SetMarkerStyle(8);
    G_te_all_neg_vs_rate->GetYaxis()->SetRangeUser(0.9,0.96);
    TF1 *neg_fit = new TF1("pol1","pol1",0,500);
  //  neg_fit->FixParameter(0,1);
    G_te_all_neg_vs_rate->Fit(neg_fit);
    neg_fit = G_te_all_neg_vs_rate->GetFunction("pol1");
    neg_fit->Draw("same");
    gStyle->SetOptFit(1);
    G_te_all_neg_vs_rate->Draw("p");
    std::string c_te_all_rate_name = "results/TE/pi_te_spring_vs_rate.pdf";
    c_te_all_rate->SaveAs(c_te_all_rate_name.c_str());

    double pos_param_1 = pos_fit->GetParameter(0);
    double pos_param_2 = pos_fit->GetParameter(1);
    double neg_param_1 = neg_fit->GetParameter(0);
    double neg_param_2 = neg_fit->GetParameter(1);
    std::cout<<" pos fitting "<<pos_param_1<<" "<<pos_param_2<<std::endl;
    std::cout<<" neg fitting "<<neg_param_1<<" "<<neg_param_2<<std::endl;

    TCanvas* c_te_all_ratecorr = new TCanvas();
    c_te_all_ratecorr->SetGrid();
    G_te_all_pos_ratecorr_vs_rate->SetMarkerColor(kRed);
    G_te_all_pos_ratecorr_vs_rate->SetMarkerStyle(8);
    G_te_all_pos_ratecorr_vs_rate->Draw("ap");
    G_te_all_neg_ratecorr_vs_rate->SetMarkerColor(kBlack);
    G_te_all_neg_ratecorr_vs_rate->SetMarkerStyle(8);
    G_te_all_neg_ratecorr_vs_rate->Draw("p");
    std::string c_te_all_ratecorr_name = "results/TE/pi_te_spring_vs_rate_ratecorr.pdf";
    c_te_all_ratecorr->SaveAs(c_te_all_ratecorr_name.c_str());

    TCanvas* c_te_all_yieldte = new TCanvas();
    c_te_all_yieldte->SetGrid();
    G_te_all_pos_yieldte->SetMarkerColor(kRed);
    G_te_all_pos_yieldte->SetMarkerStyle(8);
    G_te_all_pos_yieldte->Draw("ap");
    G_te_all_neg_yieldte->SetMarkerColor(kBlack);
    G_te_all_neg_yieldte->SetMarkerStyle(8);
    G_te_all_neg_yieldte->Draw("p");
    std::string c_te_all_yieldte_name = "results/TE/pi_te_spring_yieldte.pdf";
    c_te_all_yieldte->SaveAs(c_te_all_yieldte_name.c_str());

    TCanvas* c_te_all_yieldteratecorr = new TCanvas();
    c_te_all_yieldteratecorr->SetGrid();
    G_te_all_pos_yieldteratecorr->SetMarkerColor(kRed);
    G_te_all_pos_yieldteratecorr->SetMarkerStyle(8);
    G_te_all_pos_yieldteratecorr->Draw("ap");
    G_te_all_neg_yieldteratecorr->SetMarkerColor(kBlack);
    G_te_all_neg_yieldteratecorr->SetMarkerStyle(8);
    G_te_all_neg_yieldteratecorr->Draw("p");
    std::string c_te_all_yieldteratecorr_name = "results/TE/pi_te_spring_yieldteratecorr.pdf";
    c_te_all_yieldteratecorr->SaveAs(c_te_all_yieldteratecorr_name.c_str());
    
    TCanvas* c_te_all_yieldrate = new TCanvas();
    c_te_all_yieldrate->SetGrid();
    G_te_all_pos_vs_yieldrate->SetMarkerColor(kRed);
    G_te_all_pos_vs_yieldrate->SetMarkerStyle(8);
    G_te_all_pos_vs_yieldrate->Draw("ap");
    G_te_all_neg_vs_yieldrate->SetMarkerColor(kBlack);
    G_te_all_neg_vs_yieldrate->SetMarkerStyle(8);
    G_te_all_neg_vs_yieldrate->Draw("p");
    std::string c_te_all_yieldrate_name = "results/TE/pi_te_spring_vs_yieldrate.pdf";
    c_te_all_yieldrate->SaveAs(c_te_all_yieldrate_name.c_str());
    
    TCanvas* c_te_all_runs = new TCanvas();
    c_te_all_runs->SetGrid();
    G_te_all_pos_runs->SetMarkerColor(kRed);
    G_te_all_pos_runs->SetMarkerStyle(8);
    G_te_all_pos_runs->Draw("ap");
    G_te_all_neg_runs->SetMarkerColor(kBlack);
    G_te_all_neg_runs->SetMarkerStyle(8);
    G_te_all_neg_runs->Draw("p");
    std::string c_te_all_runs_name = "results/TE/pi_te_spring_runs.pdf";
    c_te_all_runs->SaveAs(c_te_all_runs_name.c_str());

    TCanvas* c_te_all_realtime = new TCanvas();
    c_te_all_realtime->SetGrid();
    G_te_all_pos_realtime->SetMarkerColor(kRed);
    G_te_all_pos_realtime->SetMarkerStyle(8);
    G_te_all_pos_realtime->Draw("ap");

    G_te_all_neg_realtime->SetMarkerColor(kBlack);
    G_te_all_neg_realtime->SetMarkerStyle(8);
    gStyle->SetTimeOffset(x0);
    G_te_all_neg_realtime->Draw("p");
    std::string c_te_all_realtime_name = "results/TE/pi_te_spring_realtime.pdf";
    c_te_all_realtime->SaveAs(c_te_all_realtime_name.c_str());
    
    TCanvas* c_te_all_realtime_ratecorr = new TCanvas();
    c_te_all_realtime_ratecorr->SetGrid();
    G_te_all_pos_realtime_ratecorr->SetMarkerColor(kRed);
    G_te_all_pos_realtime_ratecorr->SetMarkerStyle(8);
    gStyle->SetTimeOffset(x0);
    G_te_all_pos_realtime_ratecorr->Draw("ap");

    G_te_all_neg_realtime_ratecorr->SetMarkerColor(kBlack);
    G_te_all_neg_realtime_ratecorr->SetMarkerStyle(8);
    gStyle->SetTimeOffset(x0);
    G_te_all_neg_realtime_ratecorr->Draw("p");
    std::string c_te_all_realtime_ratecorr_name = "results/TE/pi_te_spring_realtime_ratecorr.pdf";
    c_te_all_realtime_ratecorr->SaveAs(c_te_all_realtime_ratecorr_name.c_str());

    TCanvas* c_te_momentum = new TCanvas();
    c_te_momentum->SetGrid();
    G_te_pos_vs_momentum->SetMarkerColor(kRed);
    G_te_pos_vs_momentum->SetMarkerStyle(8);
    G_te_pos_vs_momentum->Draw("ap");
    G_te_neg_vs_momentum->SetMarkerStyle(8);
    G_te_neg_vs_momentum->Draw("p");
    std::string c_te_momentum_name = "results/TE/pi_te_spring_momentum.pdf";
    c_te_momentum->SaveAs(c_te_momentum_name.c_str());
    
    TCanvas* c_te_momentum_ratecorr = new TCanvas();
    c_te_momentum_ratecorr->SetGrid();
    G_te_pos_vs_momentum_ratecorr->SetMarkerColor(kRed);
    G_te_pos_vs_momentum_ratecorr->SetMarkerStyle(8);
    G_te_pos_vs_momentum_ratecorr->Draw("ap");
    G_te_neg_vs_momentum_ratecorr->SetMarkerStyle(8);
    G_te_neg_vs_momentum_ratecorr->Draw("p");
    std::string c_te_momentum_ratecorr_name = "results/TE/pi_te_momentum_spring_ratecorr.pdf";
    c_te_momentum_ratecorr->SaveAs(c_te_momentum_ratecorr_name.c_str());

    TCanvas* c_te_index = new TCanvas();
    c_te_index->SetGrid();
    c_te_index->Divide(1,2);
    c_te_index->cd(1);
    gPad->SetGrid();
    G_te_pos_index->SetMarkerColor(kRed);
    G_te_pos_index->SetMarkerStyle(8);
    G_te_pos_index->Draw("ap");
    c_te_index->cd(2);
    gPad->SetGrid();
    G_te_neg_index->SetMarkerStyle(8);
    G_te_neg_index->Draw("ap");
    std::string c_te_index_name =  "results/TE/pi_te_index_spring.pdf";
    c_te_index->SaveAs(c_te_index_name.c_str());
    //TCanvas* c_te_index_neg = new TCanvas();
    //c_te_index_neg->SetGrid();
    //G_te_neg_index->SetMarkerStyle(8);
    //G_te_neg_index->Draw("p");
    //std::string c_te_index_neg_name =  "results/TE/pi_te_index_neg.pdf";
    //c_te_index_neg->SaveAs(c_te_index_neg_name.c_str());
    
    TCanvas* c_te_index_ratecorr = new TCanvas();
    c_te_index_ratecorr->SetGrid();
    c_te_index_ratecorr->Divide(1,2);
    c_te_index_ratecorr->cd(1);
    gPad->SetGrid();
    G_te_pos_index_ratecorr->SetMarkerColor(kRed);
    G_te_pos_index_ratecorr->SetMarkerStyle(8);
    G_te_pos_index_ratecorr->Draw("ap");
    c_te_index_ratecorr->cd(2);
    gPad->SetGrid();
    G_te_neg_index_ratecorr->SetMarkerStyle(8);
    G_te_neg_index_ratecorr->Draw("ap");
    std::string c_te_index_ratecorr_name =  "results/TE/pi_te_index_spring_ratecorr.pdf";
    c_te_index_ratecorr->SaveAs(c_te_index_ratecorr_name.c_str());
  }
