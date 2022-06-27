#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
double Get_FADC_eff(double SHMS_rate,double HMS_rate){
  return 1- 0.05*(SHMS_rate/1000 + HMS_rate/1000);
  //50ns is the slop. 50+-30
  //1- 0.05*(shms_3/4 rate in khz/1000 - hms_elReal_rate in Khz/1000)
}

void FADC_corr(){
    json j_rungroup;
    {
      std::ifstream ifs("db2/ratio_run_group_updated.json");
      ifs>>j_rungroup;
    }
    json j_info;
    {
      std::ifstream ifs("db2/runs_info.json");
      ifs>>j_info;
    }
    double current_uncertainty = 0.1;
    TGraph* g_currentratio_RunGroup = new TGraph();
    double i_currratio=0;
    TGraph* g_currentratio_err_RunGroup = new TGraph();
    double i_currentratio_err = 0;
    TGraph* g_currentratio_err_R_RunGroup = new TGraph();
    double i_currentratio_err_R = 0;

    TH1D* h_uncertainty = new TH1D("","current ratio uncertainty",10,0,0.01);

    int i_rungoup_pos = 0,i_rungroup_neg = 0,i_run_pos = 0,i_run_neg = 0;
    for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
      int RunGroup = std::stoi(it.key());
      std::cout<<RunGroup<<std::endl;
      std::vector<int> neg_D2,pos_D2;
      neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
      pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
      if(!neg_D2.empty() && !pos_D2.empty()){
        std::vector<double> pos_current,pos_charge,neg_current,neg_charge;
        for(auto i_neg = neg_D2.begin();i_neg!=neg_D2.end();i_neg++){
           int RunNumber = *i_neg;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           std::cout<<RunNumber<<", neg "<<current<<std::endl;
           neg_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           neg_charge.push_back(charge);
        }//loop over neg runs
        for(auto i_pos = pos_D2.begin();i_pos!=pos_D2.end();i_pos++){
           int RunNumber = *i_pos;
           double current = j_info[(std::to_string(RunNumber)).c_str()]["current"].get<double>();
           std::cout<<RunNumber<<", pos "<<current<<std::endl;
           pos_current.push_back(current);
           double charge = j_info[(std::to_string(RunNumber)).c_str()]["charge"].get<double>();
           pos_charge.push_back(charge);
        }//loop over pos runs

        double neg_curr = Get_average(neg_current,neg_charge);
        double pos_curr = Get_average(pos_current,pos_charge);
        double curr_ratio = neg_curr/pos_curr;
        g_currentratio_RunGroup ->SetPoint(i_currratio,RunGroup,curr_ratio);
        i_currratio+=1;
        
        double neg_curr_low = neg_curr-current_uncertainty;
        double neg_curr_high = neg_curr+current_uncertainty;
        std::cout<<current_uncertainty<<", neg low "<<neg_curr_low<<" neg high "<<neg_curr_high<<std::endl;
        double pos_curr_low = pos_curr-current_uncertainty;        
        double pos_curr_high = pos_curr+current_uncertainty;
        std::cout<<current_uncertainty<<", pos low "<<pos_curr_low<<" pos high "<<pos_curr_high<<std::endl;
        double currratio_uncertainty = std::abs((neg_curr_high/pos_curr_high)-(neg_curr_low/pos_curr_low))/curr_ratio;
        g_currentratio_err_RunGroup->SetPoint(i_currentratio_err,RunGroup,currratio_uncertainty);
        i_currentratio_err +=1;

        double currratio_uncertainty_R = currratio_uncertainty/curr_ratio;
        g_currentratio_err_R_RunGroup->SetPoint(i_currentratio_err_R,RunGroup,currratio_uncertainty_R);
        i_currentratio_err_R+=1;
  
        h_uncertainty->Fill(currratio_uncertainty);

      }//if normal production runs
    }//loop over rungroups
    TCanvas* c_currratio = new TCanvas();
    g_currentratio_RunGroup->Fit("pol0");
    g_currentratio_RunGroup->SetMarkerColor(kRed);
    g_currentratio_RunGroup->SetMarkerStyle(8);
    g_currentratio_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_RunGroup->GetYaxis()->SetTitle("neg_current/pos_current");
    g_currentratio_RunGroup->Draw("AP");
    std::string c_currratio_name = "results/sys/current_ratio.pdf";
    c_currratio->SaveAs(c_currratio_name.c_str());

    TCanvas* c_currratio_uncertainty = new TCanvas();
    g_currentratio_err_RunGroup->Fit("pol0");
    gStyle->SetOptFit(1);
    //g_currentratio_err_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_currentratio_err_RunGroup->SetMarkerColor(kRed);
    g_currentratio_err_RunGroup->SetMarkerStyle(8);
    g_currentratio_err_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_err_RunGroup->GetYaxis()->SetTitle("current ratio uncertainty");
    g_currentratio_err_RunGroup->Draw("AP");
    std::string c_currratio_uncertainty_name = "results/sys/current_ratio_uncertainty.pdf";
    c_currratio_uncertainty->SaveAs(c_currratio_uncertainty_name.c_str());
    
    TCanvas* c_currratio_uncertainty_R = new TCanvas();
    //g_currentratio_err_R_RunGroup->GetXaxis()->SetRangeUser(0,0.1);
    g_currentratio_err_R_RunGroup->SetMarkerColor(kRed);
    g_currentratio_err_R_RunGroup->SetMarkerStyle(8);
    g_currentratio_err_R_RunGroup->GetXaxis()->SetTitle("RunGroup");
    g_currentratio_err_R_RunGroup->GetYaxis()->SetTitle("#sigma #Delta/#Delta");
    g_currentratio_err_R_RunGroup->Draw("AP");
    std::string c_currratio_uncertainty_R_name = "results/sys/current_ratio_uncertainty_R.pdf";
    c_currratio_uncertainty_R->SaveAs(c_currratio_uncertainty_R_name.c_str());
  
    TCanvas* c_histo = new TCanvas();
    h_uncertainty->Draw("hist");
    h_uncertainty->GetXaxis()->SetTitle("#sigma #Delta/#Delta");
    c_histo->SaveAs("results/sys/current_ratio_histo.pdf");
  for(auto it = j_info.begin();it!=j_info.end();it++){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;

    std::string report_file_name = "report/replay_coin_production_"+std::to_string(RunNumber)+"_-1.report";
    std::ifstream report_file;
    report_file.open(report_file_name.c_str());
    std::string SHMS_rate_str = "SHMS 3/4 Trigger Rate";
    std::string HMS_rate_str = "HMS_hEL_REAL";
    double SHMS_rate,HMS_rate;
    std::string line;
    while(std::getline(report_file,line)){
      if(line.find(SHMS_rate_str)<line.length()){
        std::cout<<line<<std::endl;
        size_t i = 0;
        i = line.find(':');
        std::string SHMS_rate_str2 = line.substr(i+1,line.length()-1);
        std::cout<<SHMS_rate_str2<<std::endl;
        i = 0;
        for(;i<SHMS_rate_str2.length();i++){if(!isdigit(SHMS_rate_str2[i])) break;}
        SHMS_rate = std::stod(SHMS_rate_str2.substr(0,i-1));
        std::cout<<"SHMS rate "<<SHMS_rate<<std::endl;    
      }
      if(line.find(HMS_rate_str)<line.length()){
        std::cout<<line<<std::endl;
        size_t i = 0;
        i = line.find('[');
        std::string HMS_rate_str2 = line.substr(i+1,line.length()-1);
        std::cout<<HMS_rate_str2<<std::endl;
        std::cout<<std::stod(HMS_rate_str2)<<std::endl;
        i = 0;
        for(;i<HMS_rate_str2.length();i++){if(!isdigit(HMS_rate_str2[i])) break;}
        HMS_rate = std::stod(HMS_rate_str2.substr(0,i-1));
        std::cout<<"HMS rate "<<HMS_rate<<std::endl;    
      }
    }
    double FADC_eff = Get_FADC_eff(SHMS_rate,HMS_rate);
    j_info[std::to_string(RunNumber)]["FADC_eff"] = FADC_eff;
  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
