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
#include "TH2.h"
R__LOAD_LIBRARY(libfmt.so)

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

void calorimeter_gain_analysis_statistic(){
  Read_CALPARAM* rc[15];
  std::ifstream in_runs;
  in_runs.open("spring_selected_info.txt");
  int runnumber[15];
  double momentum[15];
  int in,i,j;
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
    std::cout<<rc[in]->arr()[1][1]<<std::endl;    
    //infile.open(0); 
  }

  int neg_gain = 14;
  int pos_gain = 14;
  int array_gain_i = 14,array_gain_j=16;
  TH2* h = new TH2D("h2","effective gain for each blocks(for 15 runs)",16,0,15,14,0,13);
  for(int i = 0;i<array_gain_i;++i){
    for(int j = 0; j<array_gain_j;++j){
      for(in = 0;in<15;++in){
        if(rc[in]->arr()[i][j]!=0){
        h->Fill(j,i);
        }
      }
    }
  }
  TCanvas *c = new TCanvas;
  //c->SetGrid(2,2);
  h->Draw("TEXT");
  c->SaveAs("cal_gain/statistic.pdf");
}
