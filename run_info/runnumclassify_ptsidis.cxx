#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#include <iostream>
#include "nlohmann/json.hpp"
#include "TObject.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include <numeric>

class run_key{
  friend std::ostream& operator<<(std::ostream& os,const run_key&);
  double _hms_p;
  double _hms_th;
  double _shms_p;
  double _shms_th;

  public:
  run_key(double hmsp,double hmsth, double shmsp,double shmsth):_shms_p(shmsp),_shms_th(shmsth),_hms_p(hmsp),_hms_th(hmsth)
  {}
  double& hms_p()  {return _hms_p;}
  double& hms_th()  {return _hms_th;}
  double& shms_p()  {return _shms_p;}
  double& shms_th()  {return _shms_th;}
  bool operator <(const run_key& keyobj)const
  {
    //    return keyobj._hms_p<this->_hms_p || (keyobj._hms_p==this->_hms_p && (keyobj._hms_th<this->_hms_th || (keyobj._hms_th==this->_hms_th && ( keyobj._shms_p<this->_shms_p || ( keyobj._shms_p==this->_shms_p && keyobj._shms_th<this->_shms_th))))); 
    double epsilon = 0.02;
    return (this->_hms_p-keyobj._hms_p>epsilon) || ((std::abs(this->_hms_p-keyobj._hms_p)<epsilon) && ((this->_hms_th-keyobj._hms_th>epsilon) || ((std::abs(this->_hms_th-keyobj._hms_th)<epsilon) && ( (this->_shms_p-keyobj._shms_p>epsilon) || (( std::abs(this->_shms_p-keyobj._shms_p)<epsilon) && (this->_shms_th-keyobj._shms_th>epsilon)))))); 
  }
  bool operator ==(const run_key& keyobj)const
  {
    double epsilon=0.02;
    return (std::abs(keyobj._hms_p-this->_hms_p)<epsilon) && (std::abs(keyobj._hms_th-this->_hms_th)<epsilon) && (std::abs(keyobj._shms_p-this->_shms_p)<epsilon) && (std::abs(keyobj._shms_th-this->_shms_th)<epsilon); 
    //return keyobj._hms_p==this->_hms_p && keyobj._hms_th == this->_hms_th;
    //  return keyobj._hms_p==this->_hms_p && keyobj._hms_th==this->_hms_th && keyobj._shms_p==this->_shms_p && keyobj._shms_th==this->_shms_th; 
  }

};
std::ostream& operator<<(std::ostream& os,const run_key& k){
  os << "hms_p " <<k._hms_p << " hms_th " << k._hms_th << " shms_p " << k._shms_p << " shms_th " << k._shms_th << " ";
  // os<<"hms_p "<<k._hms_p<<" shms_p "<<k._shms_p;
  return os;
}

void runnumclassify_ptsidis(){
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("shuo_analysis/dbase/ptsidis_runlist.json");
    json_input_file>>j;
  }

  std::ofstream outfile;
  outfile.open("runs_classify_ptsidis.txt");
  run_key runskey(0,0,0,0);
  std::map<run_key,vector<int>> mymap;

  for(auto it = j.begin();it!=j.end();++it){
    //while(infile_fall>>runnumber){
    std::string runnumber = it.key();
    auto runjs = it.value();
    // std::cout<<"1"<<std::endl;
    runskey.hms_p() = runjs["hms_p"].get<double>();
    runskey.hms_th() = runjs["hms_theta"].get<double>();//just to satisfy run_key class requirments
    runskey.shms_p() = std::abs(runjs["shms_p"].get<double>()); 
    runskey.shms_th() = runjs["shms_theta"].get<double>();
    mymap[runskey].push_back(std::stoi(it.key()));
    std::cout<<std::stoi(it.key())<<std::endl;
  }
  std::vector<int> runs;
  int i = 1;
  json jout;
  std::ofstream out("classify_runs_ptsidis.json");
  for(auto it = mymap.begin();it!= mymap.end();++it){
    std::cout<< "#kingroup "<<i<<" "<<it->first<<std::endl;;
    outfile<< "#kingroup "<<i<<" "<<it->first<<"\n";
    int nn=0;
    std::stringstream ss;
    ss<<it->first;
    std::string string = ss.str();
    run_key runkey = it->first;  
     std::cout<<runkey.hms_p()<<std::endl;;
    int shms_theta = runkey.shms_th();
    int shmsp = runkey.shms_p();
    std::string ii = std::to_string(i);
    //json jout;
    jout[ii]["hms_p"]=runkey.hms_p();
    jout[ii]["shms_p"]=runkey.shms_p();
    jout[ii]["shms_theta"]=runkey.shms_th();
    jout[ii]["hms_theta"]=runkey.hms_th();
    //out<<jout<<std::endl; 
    std::vector<int> runs_pos_2,runs_pos_3,runs_pos_5,runs_neg_2,runs_neg_3,runs_neg_5;
    for(auto ik = it->second.begin();ik!=it->second.end();++ik){
      runs.push_back(*ik);
      std::string runnum = std::to_string(*ik);
        json run_info;
        {
          std::ifstream json_input("shuo_analysis/dbase/ptsidis_runlist.json");
          json_input>>run_info;
        }
        if(run_info[runnum]["shms_p"].get<double>()<0){
          if(run_info[runnum]["target"].get<int>()==2){
            runs_neg_2.push_back(*ik);
          }
          else{
            if(run_info[runnum]["target"].get<int>()==3){
              runs_neg_3.push_back(*ik);
            }
            else{
              runs_neg_5.push_back(*ik);
            }
          }
        }
        else{
          if(run_info[runnum]["target"].get<int>()==2){
            runs_pos_2.push_back(*ik);
          }
          else{
            if(run_info[runnum]["target"].get<int>()==3){
              runs_pos_3.push_back(*ik);
            }
            else{
              runs_pos_5.push_back(*ik);
            }
          }
        }

        //std::cout<<run_info[runnum]["spectrometers"]["hms_momentum"]<<std::endl;
      //auto runinfo = run_info.find(runnum);
      //auto runinfojs=*runinfo;
      //std::cout<<*ik<<" "<<run_info[runnum]["spectrometers"]["hms_momentum"]<<std::endl;
    }
    std::vector<int> onerungroup;
    if(!runs_neg_2.empty()){
      onerungroup.push_back(*min_element(runs_neg_2.begin(),runs_neg_2.end()));
    }
    if(!runs_neg_3.empty()){
      onerungroup.push_back(*min_element(runs_neg_3.begin(),runs_neg_3.end()));}
    if(!runs_neg_5.empty()){
      onerungroup.push_back(*min_element(runs_neg_5.begin(),runs_neg_5.end()));}
    if(!runs_pos_2.empty()){
      onerungroup.push_back(*min_element(runs_pos_2.begin(),runs_pos_2.end()));}
    if(!runs_pos_3.empty()){
      onerungroup.push_back(*min_element(runs_pos_3.begin(),runs_pos_3.end()));}
    if(!runs_pos_5.empty()){
      onerungroup.push_back(*min_element(runs_pos_5.begin(),runs_pos_5.end()));}
    double min = *min_element(onerungroup.begin(),onerungroup.end());
    //double min = *min_element(runs.begin(),runs.end());
    jout[ii]["kin_group"]=min;
    jout[ii]["neg"]["H2"]=runs_neg_2;
    jout[ii]["neg"]["D2"]=runs_neg_3;
    jout[ii]["neg"]["Dummy"]=runs_neg_5;
    jout[ii]["pos"]["H2"]=runs_pos_2;
    jout[ii]["pos"]["D2"]=runs_pos_3;
    jout[ii]["pos"]["Dummy"]=runs_pos_5;
    i++;
    outfile<<"\n";
  }
  out<<jout.dump(4)<<std::endl;

  std::sort(runs.begin(),runs.end());
  std::cout<<"vector size"<<runs.size()<<std::endl;
  //  for(auto it = runs.begin();it!= runs.end(); ++it){
  //    std::cout<<*it<<std::endl;
  //  }
  }

