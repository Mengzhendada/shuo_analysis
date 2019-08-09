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
  class Read_CALPARAM{

    std::ifstream& ifs;
    std::vector<double> _neg, _pos;
    std::vector<std::vector<double>> _arr;

    void skip(){
      assert(ifs.good());
      ifs.ignore(5000,'=');
    }
    void read_neg(){
      std::string line;
      std::getline(ifs,line);
      std::istringstream ss(line);
      std::string tmp;
      std::getline(ss,tmp,',');
      while(ss.good()){
        _neg.push_back(std::stod(tmp));
        std::getline(ss,tmp,',');
      }
    }
    void read_pos(){
      std::string line;
      std::getline(ifs,line);
      std::istringstream ss(line);
      std::string tmp;
      std::getline(ss,tmp,',');
      while(ss.good()){
        _pos.push_back(std::stod(tmp));
        std::getline(ss,tmp,',');
      }   
    }

    void read_arr(){
      std::string line;
      std::getline(ifs,line);
      while(ifs.good()){
        std::istringstream ss(line);
        std::string tmp;
        std::vector<double> tmpv;
        std::getline(ss,tmp,',');
        while(ss.good()){
          tmpv.push_back(std::stod(tmp));
          std::getline(ss,tmp,',');
        }   
        _arr.push_back(tmpv);
        std::getline(ifs,line);
      }
    }

    public:

    //Read_CALPARAM(std::string if_name):ifs(ifstream(if_name)){

    //}
    Read_CALPARAM(std::ifstream& in_ifs):ifs(in_ifs){
      skip();
      read_neg();
      skip();
      read_pos();
      skip();
      read_arr();
    }
    ~Read_CALPARAM();
    const std::vector<double>& neg(){return _neg;}
    const std::vector<double>& pos(){return _pos;}
    const std::vector<std::vector<double>>& arr(){return _arr;}

  };

//select fit datapoint
Bool_t reject;
Double_t fline(Double_t *x, Double_t *par)
{
  if(reject && x[1]!=0){
    TF1::RejectPoint();
    return 0;
  }
  return par[0]+par[1]*x[0];
}
void calorimeter_gain_analysis(){
  Read_CALPARAM* rc[15];
  std::ifstream in_runs;
  in_runs.open("spring_selected_info.txt");
  int runnumber[15];
  double momentum[15];
  int in = 0;
  //std::ifstream infile;
  //std::string name = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
  for(in = 0;in<15;++in){
    in_runs>>runnumber[in]>>momentum[in];
    std::cout<<runnumber[in]<<" "<<momentum[in]<<std::endl;

    std::string  name = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
    name = name+std::to_string(runnumber[in])+"_-1_0_-1";
    std::cout<<name<<std::endl;
    std::ifstream infile;
    infile.open(name);
    if(infile.good()){
      std::cout<<"good "<<in<<std::endl;
      rc[in]= new Read_CALPARAM(infile);}
    else{exit;}
  //  std::cout<<rc[in]->arr()[1][1]<<std::endl;    
    //infile.open(0); 
  }

  int neg_gain = 14;
  int pos_gain = 14;
  int array_gain_i = 14,array_gain_j=16;
  std::ofstream outfile;
  outfile.open("cal_gain/calorimeter_gain_analysis.txt");
  std::ofstream outfile_1;
  outfile_1.open("cal_gain/calorimeter_gain_analysis_param.txt");
  // for(i = 0;i<neg_gain;++i){

  // }
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
      for(in = 0;in<15;++in){
        outfile<<runnumber[in]<<" "<<std::abs(momentum[in])<<" "<<rc[in]->arr()[i][j]<<"\n";
        //if(rc[in]->arr()[i][j]>0 && rc[in]->arr()[i][j]<100 && std::abs(momentum[in])>1 && std::abs(momentum[in])<6){n++;
        G->SetPoint(in,std::abs(momentum[in]),rc[in]->arr()[i][j]);
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
