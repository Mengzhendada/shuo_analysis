#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <cmath>
#include <cassert>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include "TF1.h"
#include "TLegend.h"
#include "TPaveStats.h"

R__LOAD_LIBRARY(libfmt.so)

//Read calorimeter calibrate numbers
void calorimeter_gain_modify(){
  std::cout<<"Which calorimeter calibration file do you want to add?"<<std::endl;
  std::string in;
  std::cin>>in;
  Read_CALPARAM* rc; 
  rc = new Read_CALPARAM(in);

  int neg_gain = 14;
  int pos_gain = 14;
  int array_gain_i = 14,array_gain_j=16;
  std::ofstream outfile;
  outfile.open("cal_gain/calorimeter_gain_analysis.txt");
  std::ofstream outfile_1;
  outfile_1.open("cal_gain/calorimeter_gain_analysis_param.txt");
  TFile *f = TFile::Open("Cal_gain.root");
  
  std::cout<<"check 1"<<std::endl;
  gStyle->SetOptFit();
  TCanvas c1("c1");
  c1.Print("c1.ps(");
  c1.SetGrid();
  for(int i = 0;i<array_gain_i;++i){
    for(int j = 0; j<array_gain_j;++j){
      outfile<<"aray "<<i<<" "<<j<<"\n";
      outfile<<"runnumber "<<"momentum" <<"gain"<<"\n";
      //TGraphErrors *G = new TGraphErrors();
      auto mg = new TMultiGraph();
      TGraph *G = new TGraph();
      //G->GetXaxis()->SetLimits(1,6);
      //G->GetYaxis()->SetLimits(0,100);
      //G->SetMaximum(100);
      //G->SetMinimum(0);
      std::cout<<"check 2"<<std::endl;
      int n=0;
     // for(in = 0;in<15;++in){
     //   outfile<<runnumber[in]<<" "<<std::abs(momentum[in])<<" "<<rc[in]->arr()[i][j]<<"\n";
        //if(rc[in]->arr()[i][j]>0 && rc[in]->arr()[i][j]<100 && std::abs(momentum[in])>1 && std::abs(momentum[in])<6){n++;
     //   G->SetPoint(in,std::abs(momentum[in]),rc[in]->arr()[i][j]);
       // }
      }
      //outfile<<"array "<<i<<" "<<j<<" "<<"effective calibration "<<n<<"\n";
      //outfile_1<<"array "<<i<<" "<<j<<" "<<"effective calibration "<<n<<"\n";
      //TCanvas *c = new TCanvas();
      G->SetMarkerStyle(20);
      G->SetLineColor(4);
     // TF1 *Pol = new TF1("pol","pol",-5,-1);
      //if(n>0){G->Fit("pol1","FQ","",1,5);}
      //G->SetTitle(fmt::format("cal_gain_{}_{}",i,j).c_str());
      //TF1 *G_F = G->GetFunction("pol1");
      //double a;
      //double b;
      //double chi2;
      //if(G_F){
      //  a = G_F->GetParameter(1);
      //  b = G_F->GetParameter(2);
      //  chi2 = G_F->GetChisquare();
      //  outfile<<"Fitting parameters "<<a<<" "<<b<<" "<<chi2<<std::endl;
      // outfile_1<<"Fitting parameters "<<a<<" "<<b<<" "<<chi2<<std::endl;
      //}i
      mg->SetTitle(fmt::format("cal_gain_{}_{}",i,j).c_str());
      mg->GetXaxis()->SetTitle("shms momentum(GeV)");
      mg->GetYaxis()->SetTitle("calorimeter gain");
      mg->GetXaxis()->SetLimits(1,6);
      mg->GetYaxis()->SetLimits(0,100);
      mg->GetXaxis()->SetRangeUser(1,6);
      mg->GetYaxis()->SetRangeUser(0,100);
      mg->Add(G);
      // mg->Print(fmt::format("y={}x+{},chi2={},{}out of 15 data points",a,b,chi2,n));
      mg->Draw("AP");
      //TF1 *f1 = new TF1("f1","pol1",)
      //mg->Fit("pol1","FQ");
      //c->SaveAs(fmt::format("cal_gain/cal_gain_{}_{}.ps",i,j).c_str());       
      TF1 *f1 = new TF1("f1",fline,0,6,2);
      f1->SetParameters(30,0.1);
      reject = kTRUE;
      mg->Fit(f1,"0");
      reject = kFALSE;
      c1.Update();
//      auto stats = (TPaveStats*)G->GetListOfFunctions()->FindObject("stats");
//      stats->SetTextColor(kBlue);
//      stats->SetX1NDC(0.12);
//      stats->SetX2NDC(0.32);
//      stats->SetY1NDC(0.75);
      c1.Modified();
      c1.Print("c1.ps");
    }
  }
c1.Print("c1.ps)");

}
