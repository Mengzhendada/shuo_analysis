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
  auto diff_legend = new TLegend(0.7,0.7,0.95,0.95);
  auto sum_legend = new TLegend(0.7,0.7,0.95,0.95);
      int i_color = 0;
  int all_color[5] = {1,2,3,4,6}; 
      for(auto it = j_diff_sum.begin();it!=j_diff_sum.end();it++){
    double Q2 = std::stod(it.key());
    auto j_xbjz = it.value();
    std::cout<<"Q2 "<<Q2<<std::endl;    
    for(auto it = j_xbjz.begin();it!=j_xbjz.end();it++){
      int ii_color = 0;
      auto mg_diff_ratios = new TMultiGraph();
      auto mg_sum_ratio = new TMultiGraph();
      TGraphErrors* g_diff_datadeltaexc = new TGraphErrors(); 
      TGraphErrors* g_diff_datadelta = new TGraphErrors(); 
      TGraphErrors* g_diff_dataexc = new TGraphErrors(); 
      double xbj = std::stod(it.key());
      auto j_z = it.value();
      std::cout<<"xbj "<<xbj;    
      //std::string canvas_name = "x:"+std::to_string(xbj).substr(0,4)+",Q2:"+std::to_string(Q2).substr(0,5);
      std::string canvas_name = "#LT x #GT = "+std::to_string(xbj).substr(0,4)+", Q^{2} = "+std::to_string(Q2).substr(0,5)+" [Gev^2]";
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
        std::vector<double> error_diff,error_sum,value_diff,value_sum,value_diff_data,value_diff_datadeltaexc,value_diff_datadelta,value_diff_dataexc;
        if(!j_value["error_diff"].empty()){
          error_diff = j_value["error_diff"].get<std::vector<double>>();
          value_diff = j_value["value_diff"].get<std::vector<double>>();
          value_diff_data = j_value["value_diff_data"].get<std::vector<double>>();
          value_diff_datadeltaexc = j_value["value_diff_datadeltaexc"].get<std::vector<double>>();
          value_diff_datadelta = j_value["value_diff_datadelta"].get<std::vector<double>>();
          value_diff_dataexc = j_value["value_diff_dataexc"].get<std::vector<double>>();
          double weighted_mean_diff = 0,weighted_sigma_diff = 0,weighted_mean_diff_data=0,weighted_mean_diff_datadeltaexc = 0,weighted_mean_diff_datadelta = 0,weighted_mean_diff_dataexc = 0;
          for(int i = 0;i<error_diff.size();++i){
            weighted_mean_diff += value_diff[i]/(error_diff[i]*error_diff[i]);
            weighted_mean_diff_data += value_diff_data[i]/(error_diff[i]*error_diff[i]);
            weighted_mean_diff_datadeltaexc += value_diff_datadeltaexc[i]/(error_diff[i]*error_diff[i]);
            weighted_mean_diff_datadelta += value_diff_datadelta[i]/(error_diff[i]*error_diff[i]);
            weighted_mean_diff_dataexc += value_diff_dataexc[i]/(error_diff[i]*error_diff[i]);
            weighted_sigma_diff += 1/(error_diff[i]*error_diff[i]);
          }
          double mean_diff = weighted_mean_diff/weighted_sigma_diff;
          double mean_diff_data = weighted_mean_diff_data/weighted_sigma_diff;
          double mean_diff_datadeltaexc = weighted_mean_diff_datadeltaexc/weighted_sigma_diff;
          double mean_diff_datadelta = weighted_mean_diff_datadelta/weighted_sigma_diff;
          double mean_diff_dataexc = weighted_mean_diff_dataexc/weighted_sigma_diff;
          double sig_diff = std::sqrt(1/weighted_sigma_diff);
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff"] = mean_diff;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff_data"] = mean_diff_data;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff_datadeltaexc"] = mean_diff_datadeltaexc;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff_datadelta"] = mean_diff_datadelta;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["value_diff_dataexc"] = mean_diff_dataexc;
          jout[(std::to_string(Q2)).c_str()][(std::to_string(xbj)).c_str()][(std::to_string(z)).c_str()]["error_diff"] = sig_diff;
          double ratio_datadeltaexc = mean_diff_datadeltaexc/mean_diff_data;
          double ratio_datadelta = mean_diff_datadelta/mean_diff_data;
          double ratio_dataexc = mean_diff_dataexc/mean_diff_data;
          g_diff_datadeltaexc->SetPoint(i_diff,z,ratio_datadeltaexc);
          g_diff_datadelta->SetPoint(i_diff,z,ratio_datadelta);
          g_diff_dataexc->SetPoint(i_diff,z,ratio_dataexc);
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
      g_diff_ratio->SetTitle(canvas_name.c_str());
      g_diff_ratio->SetName("diff");
      g_diff_ratio->SetMarkerStyle(8);
      g_diff_ratio->SetMarkerColor(all_color[i_color]);
      g_diff_ratio->SetLineColor(all_color[i_color]);
      diff_legend->AddEntry("diff",canvas_name.c_str(),"ep");
      mg_diff->Add(g_diff_ratio,"P");
      g_sum_ratio->SetTitle(canvas_name.c_str());
      g_sum_ratio->SetName("sum");
      g_sum_ratio->SetMarkerStyle(8);
      g_sum_ratio->SetMarkerColor(all_color[i_color]);
      g_sum_ratio->SetLineColor(all_color[i_color]);
      sum_legend->AddEntry("sum",canvas_name.c_str(),"ep");
      mg_sum->Add(g_sum_ratio,"P");
      i_color++; 
      std::cout<<"color "<<i_color<<std::endl;
      g_diff_datadeltaexc->SetName("(data-delta-exc)/data");
      g_diff_datadeltaexc->SetLineColor(1);
      g_diff_datadelta->SetName("(data-delta)/data");
      g_diff_datadelta->SetLineColor(2);
      g_diff_dataexc->SetName("(data-exc)/data");
      g_diff_dataexc->SetLineColor(3);
      mg_diff_ratios->Add(g_diff_datadeltaexc,"L");
      mg_diff_ratios->Add(g_diff_datadelta,"L");
      mg_diff_ratios->Add(g_diff_dataexc,"L");
      TCanvas* c_diff_ratios = new TCanvas();
      mg_diff_ratios->Draw("A");
      mg_diff_ratios->SetMinimum(0);
      mg_diff_ratios->SetMaximum(1.1);
      mg_diff_ratios->GetXaxis()->SetTitle("z");
      mg_diff_ratios->GetYaxis()->SetTitle("ratios");
      std::string c_diff_ratios_name = "results/yield/statistics_H2/diff_ratios_"+canvas_filename+".pdf";
      c_diff_ratios->SetTitle(canvas_name.c_str());
      c_diff_ratios->BuildLegend(0.1,0.1,0.4,0.4,canvas_name.c_str());
      c_diff_ratios->SaveAs(c_diff_ratios_name.c_str());
    }//loop over xbj

  }//loop over Q2
  //std::vector<double> error_diff_raw = jout["4.000000"]["0.350000"]["0.425000"]["error_diff"].get<std::vector<double>>();
  std::ofstream ofs("results/yield/weighted_H2_diff_sum.json");
  ofs<<jout.dump(4)<<std::endl;
  std::string diff_ratio = "#frac{Y^{#pi^{+}}_{p}-Y^{#pi^{-}}_{p}}{Y^{#pi^{+}}_{D}-Y^{#pi^{-}}_{D}}";
  //std::string diff_ratio = "#frac{#sigma^{#pi^{+}}_{p}-#sigma^{#pi^{-}}_{p}}{#sigma^{#pi^{+}}_{D}-#sigma^{#pi^{-}}_{D}}";
  c_diff_ratio->cd();
  mg_diff->GetXaxis()->SetTitle("z");
  mg_diff->GetYaxis()->SetTitle(diff_ratio.c_str());
  mg_diff->Draw("A");
  mg_diff->SetMinimum(0);
  mg_diff->SetMaximum(3);
  mg_diff->GetXaxis()->SetLimits(0.35,0.8);
  mg_diff->Fit("pol0","F");
  TF1 *Fit_1 = mg_diff->GetFunction("pol0");
  double p0 = Fit_1->GetParameter(0);
  double p0_error = Fit_1->GetParError(0);
  double chi2 = Fit_1->GetChisquare();
  int NDF = Fit_1->GetNDF();
  std::cout<<" p0 "<< p0<<" chi2 "<<chi2<<"NDF"<<NDF<<std::endl;
  TF1 *line_1 = new TF1("pol0",std::to_string(p0).c_str(),0,1);
  //line_1->SetParameter(0,p0);
  line_1->SetTitle("pol0");
  line_1->SetLineColor(kBlue);
  line_1->Draw("L same");
  TPaveText *pt_diff = new TPaveText(.5,.05,.95,.45);
  std::string pt_chi2 = "#chi^2/ndf "+std::to_string(chi2)+"/"+std::to_string(NDF);
  std::string pt_p0 = "p0 :"+std::to_string(p0)+"#pm"+std::to_string(p0_error);
  //pt_diff->AddText("SIMC+#Delta+exc");
  //pt_diff->AddText("data-#Delta-exc");
  //pt_diff->AddText("data-#Delta");
  pt_diff->AddText("data");
  TText *t1 = pt_diff->AddText(pt_chi2.c_str());
  TText *t2 = pt_diff->AddText(pt_p0.c_str());
  t1->SetTextColor(kBlue);
  t2->SetTextColor(kBlue);
  pt_diff->Draw("same");
  mg_diff->Fit("pol1","F");
  TF1 *Fit_2 = mg_diff->GetFunction("pol1");
  Fit_2->SetLineColor(kRed);
  Fit_2->Draw("L same");
  gStyle->SetOptFit(0001);
  //diff_legend->Draw();
  c_diff_ratio->BuildLegend(0.3,0.7,0.6,0.95);
  c_diff_ratio->SaveAs("results/yield/statistics_H2/diff_ratio.pdf");
  c_sum_ratio->cd();
  std::string sum_ratio = "#frac{Y^{#pi^{+}}_{p}+Y^{#pi^{-}}_{p}}{Y^{#pi^{+}}_{D}+Y^{#pi^{-}}_{D}}";
  //std::string sum_ratio = "#frac{#sigma^{#pi^{+}}_{p}+#sigma^{#pi^{-}}_{p}}{#sigma^{#pi^{+}}_{D}+#sigma^{#pi^{-}}_{D}}";
  mg_sum->GetXaxis()->SetTitle("z");
  mg_sum->GetYaxis()->SetTitle(sum_ratio.c_str());
  mg_sum->Draw("A");
  mg_sum->SetMinimum(0);
  mg_sum->SetMaximum(2);
  mg_sum->GetXaxis()->SetLimits(0.35,0.8);
  c_sum_ratio->BuildLegend(0.7,0.7,0.95,0.95);
 // sum_legend->Draw();
  c_sum_ratio->SaveAs("results/yield/statistics_H2/sum_ratio.pdf");


  return 0;
}
