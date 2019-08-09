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

  const std::vector<double>& neg(){return _neg;}
  const std::vector<double>& pos(){return _pos;}
  const std::vector<std::vector<double>>& arr(){return _arr;}

};

void compare_vs_n_onerun(){
  std::cout << "1"<<std::endl;
  Read_CALPARAM* rc[5]; 
  std::cout << "2"<<std::endl;
  std::ifstream infile_10k;
  std::cout << "3"<< std::endl;
  infile_10k.open("pcal.param.coin_replay_production_6545_-1_0_10000");
  std::cout << "4"<< std::endl;
  rc[0]= new Read_CALPARAM(infile_10k);
  std::cout << "5"<< std::endl;
  
  std::ifstream infile_30k;
  infile_30k.open("pcal.param.coin_replay_production_6545_-1_0_50000");
  rc[1]= new Read_CALPARAM(infile_30k);
  
  std::ifstream infile_50k;
  infile_50k.open("pcal.param.coin_replay_production_6545_-1_0_100000");
  rc[2]= new Read_CALPARAM(infile_50k);
  
  std::ifstream infile_100k;
  infile_100k.open("pcal.param.coin_replay_production_6545_-1_0_500000");
  rc[3]= new Read_CALPARAM(infile_100k);
  
  std::ifstream infile_200k;
  infile_200k.open("pcal.param.coin_replay_production_6545_-1_0_1000000");
  rc[4]= new Read_CALPARAM(infile_200k);
  /*cout << setprecision(4);
  for(const auto& line: rc.arr()){
    for(const auto& num: line){
      std::cout << setw(6) << num << " ";}
   std::cout<<std::endl;
  }*/
  std::cout << "6" << std::endl;
  int i,j,k;
double x_neg=0,x_pos=0,x_arr=0;
TGraphErrors *G_neg = new TGraphErrors();
G_neg->SetTitle("negative gain variation ratio");
TGraphErrors *G_pos = new TGraphErrors();
G_pos->SetTitle("pos gain variation ratio");
TGraphErrors *G_arr = new TGraphErrors();
G_arr->SetTitle("Each pmt gain variation ratio");
for(k=0;k<5;++k){
  for(i=0;i<14;++i){
    std::cout<<"7"<<std::endl;
    x_neg=x_neg+std::pow((rc[k]->neg()[i]-rc[0]->neg()[i]),2);
    x_pos=x_pos+std::pow((rc[k]->pos()[i]-rc[0]->pos()[i]),2);
    std::cout<<"8"<<std::endl;
    for(j=0;j<16;++j){
    x_arr=x_arr+std::pow((rc[k]->arr()[j][i]-rc[0]->arr()[j][i]),2);
    }
  }
  std::cout<<"9"<<std::endl;
    G_neg->SetPoint(k,k,x_neg);
    G_pos->SetPoint(k,k,x_pos);
    G_arr->SetPoint(k,k,x_arr);
}
TCanvas *c = new TCanvas;
c->Divide(2,2);
c->cd(1);
G_neg->SetMarkerStyle(20);
G_neg->Draw();
c->cd(2);
G_pos->SetMarkerStyle(20);
G_pos->Draw();
c->cd(3);
G_arr->SetMarkerStyle(20);
G_arr->Draw();
}
