#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TMultiGraph.h"


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
void add_newrun(){
  std::cout<<"Which run do you want to add? What is it's shms momentum? eg(7593 3.150)"<<std::endl;
  int runnum;
  double momentum;
  std::cin>>runnum>>momentum;
  
  std::string name = "cal_gain/pcal.param.coin_replay_production_"+std::to_string(runnum)+"+-1+0+-1";
  std::cout<<"Reading in file: "<<name<<std::endl;
  std::ifstream in_file;
  in_file.open(name);
  if(in_file.good()){std::cout<<"Reading in file"<<std::endl;}
  else{
    std::cout<<"Can't read file"<<std::endl;
    exit;
  }
  Read_CALPARAM* rc;
  rc = new Read_CALPARAM(in_file);
 // std::string rootfile = std::string("Cal_gain.root");
 // ROOT::RDataFrame d0("T",rootfile);
 // auto d = d0;
 // TFile *f = new TFile("Cal_gain.root");
 // TTree *t = (TTree*)f->Get("T");
 //   if(f->IsZombie()){
 //   std::cout<<"Error opening file"<<std::endl;
 //   exit(-1);
 // }
 // pos_0 = rc->pos()[0];
 // t->Write();
 // f->Close();

}
