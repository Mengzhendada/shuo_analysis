#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
using json = nlohmann::json;
#include "TGraphErrors.h"
#include "TFile.h"
#include "TLine.h"

const int events_limit = 150;
double weighted_average(std::vector<double> v1,std::vector<int> v2){
  double sum1,sum2;
  for(int i = 0; i<v1.size();i++){
    if(v1[i]<0){v2[i] = 0;}
    if(v2[i]<=events_limit){v2[i] = 0;}
    sum1 += v1[i]*v2[i];
    sum2 +=v2[i];
    //     std::cout<<"v1 "<<v1[i]<<" v2 "<<v2[i]<<std::endl;
  }
  //std::cout<<"in function "<<sum1/sum2<<std::endl;
  //return sum1/sum2;
  if(sum2>0.000001){
  int a = 100*sum1/sum2;
  double out = (double)a/100;
  return out;
    //std::cout<<sum2<<" "<<sum1/sum2<<std::endl;
  //  return sum1/sum2;
  }
  else{
    return 0;
  }
}

void block_gain(int RunStart =0,int RunEnd = 0){
  if(RunStart ==0 || RunEnd == 0){
    std::cout<<"Enter Start number(-1 to exit):"<<std::endl;
    std::cin>>RunStart;
    std::cout<<"Enter End number:"<<std::endl;
    std::cin>>RunEnd;
    if(RunStart <0 ||RunEnd <0 || RunEnd<RunStart){
      return;
    }
  }
  int neg_gain = 14;
  int pos_gain = 14;
  int array_gain_i = 14,array_gain_j =16;
  json out;
  TFile* root = new TFile("results/cal_gain_json/cal_gain.root","RECREATE");
  //  std::fstream fs; 
  // fs.open("results/cal_gain_json/shms_cal_gain_6068to6559.txt",std::fstream::app);
  //otfiletxt<<";Calibration constants for runs from "+std::to_string(RunStart)<<" to "<<std::to_string(RunEnd)<<std::endl;
  //outfiletxt<<"pcal_neg_gain_cor = ";
  for(int i =0;i<neg_gain;++i){
    std::vector<int> events;
    std::vector<double> gain_neg,gain_pos;
    std::cout<<i<<"th number for neg, pos and arr"<<std::endl;
    std::string name_g_neg = "neg_gain_"+std::to_string(i);
    TGraph* g_neg = new TGraph();
    g_neg->GetXaxis()->SetRange((double)RunStart,(double)RunEnd);
    std::string name_g_pos = "pos_gain_"+std::to_string(i);
    TGraph* g_pos = new TGraph();
    g_pos->GetXaxis()->SetRange((double)RunStart,(double)RunEnd);
    int g_neg_i=0,g_pos_i=0;
    std::string name_g_neg_failed = "neg_gain_"+std::to_string(i);
    TGraph* g_neg_failed = new TGraph();
    g_neg_failed->GetXaxis()->SetRange((double)RunStart,(double)RunEnd);
    std::string name_g_pos_failed = "pos_gain_"+std::to_string(i);
    TGraph* g_pos_failed = new TGraph();
    g_pos_failed->GetXaxis()->SetRange((double)RunStart,(double)RunEnd);
    int g_neg_failed_i=0,g_pos_failed_i=0;
    for(int run = RunStart;run<=RunEnd;++run){
      std::string name = "results/cal_gain_json/shms_cal_gain_"+std::to_string(run)+".json";
      std::ifstream infile(name);
      if(infile.good()){
        std::cout<<"Find run "<<run<<std::endl;
        json jfile;
        infile>>jfile;
        int nentries = jfile["events"].get<int>();
        double cor_neg = jfile["neg"][std::to_string(i)].get<double>();
        double cor_pos = jfile["pos"][std::to_string(i)].get<double>();
        std::cout<<nentries<<std::endl;
        if(nentries > events_limit && cor_neg > 0 && cor_pos > 0){
          events.push_back(jfile["events"].get<int>());
          gain_neg.push_back(jfile["neg"][std::to_string(i)].get<double>());
          gain_pos.push_back(jfile["pos"][std::to_string(i)].get<double>());
          g_neg->SetPoint(g_neg_i,run,jfile["neg"][std::to_string(i)].get<double>());
          ++g_neg_i;
          g_pos->SetPoint(g_pos_i,run,jfile["pos"][std::to_string(i)].get<double>());
          ++g_pos_i;
        }
        else{
          events.push_back(0);
          gain_neg.push_back(0);
          gain_pos.push_back(0);
          //g_neg->SetPoint(g_neg_i,run,jfile["neg"][std::to_string(i)].get<double>());
          //++g_neg_i;
          //g_pos->SetPoint(g_pos_i,run,jfile["pos"][std::to_string(i)].get<double>());
          //++g_pos_i;
          g_neg_failed->SetPoint(g_neg_failed_i,run,cor_neg);
          g_pos_failed->SetPoint(g_pos_failed_i,run,cor_pos);
          ++g_neg_failed_i;
          ++g_pos_failed_i;
        }
      }
    }//loop over runs
    double gain_neg_average,gain_pos_average;
    gain_neg_average = weighted_average(gain_neg,events);
    gain_pos_average = weighted_average(gain_pos,events);
    out["neg"][std::to_string(i)]=gain_neg_average;
    out["pos"][std::to_string(i)]=gain_pos_average;
    if(g_neg_i>0){
    auto c_neg = new TCanvas(name_g_neg.c_str()); 
    g_neg->SetMarkerStyle(21);
    g_neg->Draw("ap");
    g_neg_failed->SetMarkerStyle(22);
    g_neg_failed->SetMarkerColor(kRed);
    g_neg_failed->Draw("p same");
    TLine* l1_neg = new TLine(RunStart,gain_neg_average,RunEnd,gain_neg_average);
    l1_neg->Draw("same");
    c_neg->Write(name_g_neg.c_str());
    }
    else{
    auto c_neg = new TCanvas(name_g_neg.c_str()); 
    g_neg_failed->SetMarkerStyle(22);
    g_neg_failed->SetMarkerColor(kRed);
    //g_neg_failed->GetYaxis()->SetRange()
    g_neg_failed->Draw("ap");
    TLine* l1_neg = new TLine(RunStart,gain_neg_average,RunEnd,gain_neg_average);
    l1_neg->Draw("same");
    c_neg->Write(name_g_neg.c_str());
    }
    if(g_pos_i>0){
    auto c_pos = new TCanvas(name_g_pos.c_str());
    g_pos->SetMarkerStyle(21);
    g_pos->Draw("ap");
    g_pos_failed->SetMarkerStyle(22);
    g_pos_failed->SetMarkerColor(kRed);
    g_pos_failed->Draw("p same");
    TLine* l1_pos = new TLine(RunStart,gain_pos_average,RunEnd,gain_pos_average);
    l1_pos->Draw("same");
    c_pos->Write(name_g_pos.c_str());
    }
    else{
    auto c_pos = new TCanvas(name_g_pos.c_str());
    g_pos_failed->SetMarkerStyle(22);
    g_pos_failed->SetMarkerColor(kRed);
    g_pos_failed->Draw("ap");
    TLine* l1_pos = new TLine(RunStart,gain_pos_average,RunEnd,gain_pos_average);
    l1_pos->Draw("same");
    c_pos->Write(name_g_pos.c_str());
     
    }
  }//loop over i, for preshower
  std::cout<<"Preshower done"<<std::endl;
  for(int i = 0;i<array_gain_i;++i){
    for(int j = 0;j<array_gain_j;++j){
      std::vector<int> events;
      std::vector<double> gain_arr;
      std::string g_arr_name = "cal_gain_"+std::to_string(i)+"_"+std::to_string(j);
      TGraph* g_arr = new TGraph();
      int g_arr_i = 0;
      TGraph* g_arr_failed = new TGraph();
      int g_arr_failed_i = 0;
      for(int run = RunStart;run<RunEnd;++run){
        std::string name = "results/cal_gain_json/shms_cal_gain_"+std::to_string(run)+".json";
        std::ifstream infile(name);
        if(infile.good()){
          std::cout<<"Find run "<<run<<std::endl;
          json jfile;
          infile>>jfile;
          int nentries = jfile["events"].get<int>();
          double cor_arr = jfile["arr"][std::to_string(i)][std::to_string(j)].get<double>();
          std::cout<<nentries<<" "<<cor_arr<<std::endl;
          if(nentries > events_limit && cor_arr >0){
            events.push_back(jfile["events"].get<int>());
            gain_arr.push_back(jfile["arr"][std::to_string(i)][std::to_string(j)].get<double>());
            g_arr->SetPoint(g_arr_i,run,jfile["arr"][std::to_string(i)][std::to_string(j)].get<double>());
            ++g_arr_i;
        std::cout<<"check1"<<std::endl;
          }
          else{
          //  events.push_back(jfile["events"].get<int>());
          //  gain_arr.push_back(jfile["arr"][std::to_string(i)][std::to_string(j)].get<double>());
          //  g_arr->SetPoint(g_arr_i,run,jfile["arr"][std::to_string(i)][std::to_string(j)].get<double>());
          //  
          //  ++g_arr_i;
            events.push_back(0);
            gain_arr.push_back(0);
            g_arr_failed->SetPoint(g_arr_failed_i,run,cor_arr);
            ++g_arr_failed_i;
        std::cout<<"check2"<<std::endl;
          }
        }//if calib file exist
      }//loop over runs
      double gain_arr_average = weighted_average(gain_arr,events);
      out["arr"][std::to_string(i)][std::to_string(j)] = gain_arr_average;
      if(g_arr_i>0){
      auto c = new TCanvas(g_arr_name.c_str());
      g_arr->SetMarkerStyle(21);
      g_arr->Draw("ap");
      g_arr_failed->SetMarkerStyle(22);
      g_arr_failed->SetMarkerColor(kRed);
      g_arr_failed->Draw("p same");
    TLine* l1_arr = new TLine(RunStart,gain_arr_average,RunEnd,gain_arr_average);
    l1_arr->Draw("same");
      c->Write(g_arr_name.c_str());
      }
      else{
      auto c = new TCanvas(g_arr_name.c_str());
      g_arr_failed->SetMarkerStyle(22);
      g_arr_failed->SetMarkerColor(kRed);
      g_arr_failed->Draw("ap");
    TLine* l1_arr = new TLine(RunStart,gain_arr_average,RunEnd,gain_arr_average);
    l1_arr->Draw("same");
      c->Write(g_arr_name.c_str());
      }
      //std::cout<<"check"<<std::endl;
    }//loop over j for shower
  }//loop over i for shower
  root->Close();
  std::string outfilename = "results/cal_gain_json/shms_cal_gain_"+std::to_string(RunStart)+"to"+std::to_string(RunEnd)+".json";
  std::ofstream outfile(outfilename.c_str());
  outfile<<out.dump(4)<<std::endl;
}
