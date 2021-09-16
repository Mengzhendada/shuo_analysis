#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int weighted_average_H2_diff_sum(){
  json j_diff_sum;
  {
    std::ifstream ifs("results/yield/arrange_H2_diff_sum.json");
    ifs>>j_diff_sum;
  }
  json jout;
  json jcheck;
  TCanvas *c_diff_ratio = new TCanvas();
  TCanvas *c_sum_ratio = new TCanvas();
  auto mg_diff = new TMultiGraph();
  auto mg_sum = new TMultiGraph();
      int i_color = 0;
  int all_color[5] = {1,2,3,4,6}; 
      for(auto it = j_diff_sum.begin();it!=j_diff_sum.end();it++){
    double Q2 = std::stod(it.key());
    auto j_xbjz = it.value();
    std::cout<<"Q2 "<<Q2<<std::endl;    
    for(auto it = j_xbjz.begin();it!=j_xbjz.end();it++){
      double xbj = std::stod(it.key());
      auto j_z = it.value();
      std::cout<<"xbj "<<xbj;    
      std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::string diff_canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+",diff";
      std::string sum_canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"sum";
      std::string diff_canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_diff";
      std::string sum_canvas_filename = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_sum";
      std::string q2xdiff_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_diff_Ratio";
      std::string q2xsum_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5)+"_sum_Ratio";
      TGraphErrors* g_diff_ratio = new TGraphErrors();
      TGraphErrors* g_sum_ratio = new TGraphErrors();
      int i_diff = 0;
      int i_sum = 0;
      for(auto it = j_z.begin();it!=j_z.end();it++){
        double z = std::stod(it.key());
        std::cout<<"z "<<z;
        auto j_value = it.value();
        std::vector<double> error_diff,error_sum,value_diff,value_sum;
        if(!j_value["error_diff"].empty()){
          error_diff = j_value["error_diff"].get<std::vector<double>>();
          value_diff = j_value["value_diff"].get<std::vector<double>>();
          double weighted_mean_diff = 0,weighted_sigma_diff = 0;
          for(int i = 0;i<error_diff.size();++i){
            weighted_mean_diff += value_diff[i]/(error_diff[i]*error_diff[i]);
            weighted_sigma_diff += 1/(error_diff[i]*error_diff[i]);
          }
          double mean_diff = weighted_mean_diff/weighted_sigma_diff;
          double sig_diff = std::sqrt(1/weighted_sigma_diff);
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff"] = mean_diff;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_diff"] = sig_diff;
          g_diff_ratio->SetPoint(i_diff,z,mean_diff);
          g_diff_ratio->SetPointError(i_diff,0,sig_diff);
          i_diff++;
        }//if diff exist
        if(!j_value["error_sum"].empty()){
          error_sum = j_value["error_sum"].get<std::vector<double>>();
          value_sum = j_value["value_sum"].get<std::vector<double>>();
          double weighted_mean_sum = 0,weighted_sigma_sum = 0;
          for(int i = 0;i<error_sum.size();++i){
            weighted_mean_sum += value_sum[i]/(error_sum[i]*error_sum[i]);
            weighted_sigma_sum += 1/(error_sum[i]*error_sum[i]);
          }
          double mean_sum = weighted_mean_sum/weighted_sigma_sum;
          double sig_sum = std::sqrt(1/weighted_sigma_sum);
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_sum"] = mean_sum;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_sum"] = sig_sum;
          g_sum_ratio->SetPoint(i_sum,z,mean_sum);
          g_sum_ratio->SetPointError(i_sum,0,sig_sum);
          i_sum++;
        }//if sum exist
      }//loop over z
      std::cout<<std::endl;
      g_diff_ratio->SetTitle(diff_canvas_name.c_str());
      g_diff_ratio->SetMarkerStyle(8);
      g_diff_ratio->SetMarkerColor(all_color[i_color]);
      g_diff_ratio->SetLineColor(all_color[i_color]);
      mg_diff->Add(g_diff_ratio,"P");
      g_sum_ratio->SetTitle(sum_canvas_name.c_str());
      g_sum_ratio->SetMarkerStyle(8);
      g_sum_ratio->SetMarkerColor(all_color[i_color]);
      g_sum_ratio->SetLineColor(all_color[i_color]);
      mg_sum->Add(g_sum_ratio,"P");
      i_color++; 
      std::cout<<"color "<<i_color<<std::endl;
    }//loop over xbj

  }//loop over Q2
  //std::vector<double> error_diff_raw = jout["4.000000"]["0.350000"]["0.425000"]["error_diff"].get<std::vector<double>>();
  std::ofstream ofs("results/yield/weighted_H2_diff_sum.json");
  ofs<<jout.dump(4)<<std::endl;

  c_diff_ratio->cd();
  mg_diff->GetXaxis()->SetTitle("z");
  mg_diff->GetYaxis()->SetTitle("diff ratio");
  mg_diff->Draw("A");
  mg_diff->GetXaxis()->SetLimits(0.35,0.8);
  c_diff_ratio->BuildLegend(0.7,0.7,0.95,0.95);
  c_diff_ratio->SaveAs("results/yield/statistics_H2/diff_ratio.pdf");
  c_sum_ratio->cd();
  mg_sum->GetXaxis()->SetTitle("z");
  mg_sum->GetYaxis()->SetTitle("sum ratio");
  mg_sum->Draw("A");
  mg_sum->GetXaxis()->SetLimits(0.35,0.8);
  c_sum_ratio->BuildLegend(0.7,0.7,0.95,0.95);
  c_sum_ratio->SaveAs("results/yield/statistics_H2/sum_ratio.pdf");


  return 0;
}
