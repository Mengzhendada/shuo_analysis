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
    //if(keyobj._hms_p<this->_hms_p){return true;}
    //else{
    //  if(keyobj._hms_th<this->_hms_th){return true;}
    //  else{
    //    if(keyobj._shms_p<this->shms_p){return true;}
    //    else{
    //      if(keyobj._shms_th<this->shms_th){return true;}
    //    }

    //  }
    //}
    //return keyobj._hms_p<this->_hms_p || (keyobj._hms_p==this->_hms_p && (keyobj._hms_th<this->_hms_th));
    return keyobj._hms_p<this->_hms_p || (keyobj._hms_p==this->_hms_p && (keyobj._hms_th<this->_hms_th || (keyobj._hms_th==this->_hms_th && ( keyobj._shms_p<this->_shms_p || ( keyobj._shms_p==this->_shms_p && keyobj._shms_th<this->_shms_th))))); 
  }
  bool operator ==(const run_key& keyobj)const
  {
    //return keyobj._hms_p==this->_hms_p && keyobj._hms_th == this->_hms_th;
    return keyobj._hms_p==this->_hms_p && keyobj._hms_th==this->_hms_th && keyobj._shms_p==this->_shms_p && keyobj._shms_th==this->_shms_th; 
  }

};
std::ostream& operator<<(std::ostream& os,const run_key& k){
 os << "hms_p " <<k._hms_p << " hms_th " << k._hms_th << " shms_p " << k._shms_p << " shms_th " << k._shms_th << " ";
 // os<<"hms_p "<<k._hms_p<<" shms_p "<<k._shms_p;
  return os;
}

void runnumclassify1(){
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("run_list.json");
    json_input_file>>j;
  }
  json k;
  {
    std::ifstream json_input_file2("run_list_update.json");
    json_input_file2>>k;
  }
  std::string runnumber;
  std::ifstream infile_fall;
  infile_fall.open("fall_good_runlist.txt");
  std::ifstream infile_spring;
  infile_spring.open("spring_good_runlist.txt");
  std::ofstream outfile;
  outfile.open("runs_classify.txt");
  run_key runskey(0,0,0,0);
  std::map<run_key,vector<int>> mymap;
  
  //std::ofstream allinfo;
  //allinfo.open("all_run_indo.json");
  while(infile_fall>>runnumber){
  auto it = j.find(runnumber);
  auto runjs = *it;
// std::cout<<"1"<<std::endl;
      runskey.hms_p() = runjs["spectrometers"]["hms_momentum"].get<double>();
      runskey.hms_th() = runjs["spectrometers"]["hms_angle"].get<double>();//just to satisfy run_key class requirments
      runskey.shms_p() = std::abs(runjs["spectrometers"]["shms_momentum"].get<double>()); 
     // runskey.shms_th() = runjs["target"]["target_id"].get<double>();//just to satisfy run_key class requirments
      //runskey.hms_th() = runjs["spectrometers"]["hms_angle"].get<double>();
     // runskey.shms_p() = std::abs(runjs["spectrometers"]["shms_momentum"].get<double>());
      runskey.shms_th() = runjs["spectrometers"]["shms_angle"].get<double>();
      //runskey.shms_th() = runjs["spectrometers"]["shms_angle"].get<double>();
      //mymap.insert(std::make_pair<run_key,int>(runskey,std::stoi(it.key())));
      //std::cout << "pre: " << setw(5) << mymap.size() << std::endl;//<< setw(5) << mymap.at(runskey).size() << endl;
      mymap[runskey].push_back(std::stoi(it.key()));
    //  json all;
    //  all[it.key()]["spectrometers"]["hms_momentum"] = runjs["spectrometers"]["hms_momentum"].get<double>();//get info for each runs
    //  all[it.key()]["spectrometers"]["shms_momentum"] = runjs["spectrometers"]["shms_momentum"].get<double>();
      // = runjs["spectrometers"]["shms_momentum"].get<double>();
   //   all[it.key()]["target"]["target_id"] = runjs["target"]["target_id"].get<double>();
   //   allinfo<<all<<std::endl;
      //std::cout << "pos: " << setw(5) << mymap.size() << std::endl;//flush <<  setw(5) <<  mymap.at(runskey).size() << endl;
      std::cout<<std::stoi(it.key())<<std::endl;
    }
  while(infile_spring>>runnumber){
  auto it = k.find(runnumber);
  auto runjs = *it;
// std::cout<<"1"<<std::endl;
      runskey.hms_p() = runjs["spectrometers"]["hms_momentum"].get<double>();
      runskey.hms_th() = runjs["spectrometers"]["hms_angle"].get<double>();
      //runskey.hms_th() = runjs["spectrometers"]["hms_angle"].get<double>();
      runskey.shms_p() = std::abs(runjs["spectrometers"]["shms_momentum"].get<double>());
     // runskey.shms_th() = runjs["target"]["target_id"].get<double>();
      runskey.shms_th() = runjs["spectrometers"]["shms_angle"].get<double>();
      //mymap.insert(std::make_pair<run_key,int>(runskey,std::stoi(it.key())));
      //std::cout << "pre: " << setw(5) << mymap.size() << std::endl;//<< setw(5) << mymap.at(runskey).size() << endl;
      mymap[runskey].push_back(std::stoi(it.key()));
     // json all;
     // all[it.key()]["spectrometers"]["hms_momentum"] = runjs["spectrometers"]["hms_momentum"].get<double>();//get info for each runs
     // all[it.key()]["spectrometers"]["shms_momentum"] = runjs["spectrometers"]["shms_momentum"].get<double>();
      // = runjs["spectrometers"]["shms_momentum"].get<double>();
     // all[it.key()]["target"]["target_id"] = runjs["target"]["target_id"].get<double>();
     // allinfo<<all<<std::endl;
      //std::cout << "pos: " << setw(5) << mymap.size() << std::endl;//flush <<  setw(5) <<  mymap.at(runskey).size() << endl;
      std::cout<<std::stoi(it.key())<<std::endl;
    }
  std::vector<int> runs;
  int i = 1;
  json jout;
  std::ofstream out("classify_runs.json");
  for(auto it = mymap.begin();it!= mymap.end();++it){
    outfile<< "#kingroup "<<i<<" "<<it->first<<"\n";
    int nn=0;
    std::stringstream ss;
    ss<<it->first;
    std::string string = ss.str();
    run_key runkey = it->first;  
    // std::cout<<runkey.hms_p();
    int targetid = runkey.shms_th();
    int shmsp = runkey.shms_p();
    std::string ii = std::to_string(i);
    //json jout;
    jout[ii]["hms_p"]=runkey.hms_p();
    jout[ii]["shms_p"]=runkey.shms_p();
    //jout[ii]["targetid"]=runkey.shms_th();
    //out<<jout<<std::endl; 
    std::vector<int> runs_pos_2,runs_pos_3,runs_pos_5,runs_neg_2,runs_neg_3,runs_neg_5;
    for(auto ik = it->second.begin();ik!=it->second.end();++ik){
      runs.push_back(*ik);
      std::string runnum = std::to_string(*ik);
    if(*ik<7000){
      json run_info;
      {
        std::ifstream json_input("run_list.json");
        json_input>>run_info;
      }
    if(run_info[runnum]["spectrometers"]["shms_momentum"].get<double>()<0){
     if(run_info[runnum]["target"]["target_id"].get<int>()==2){
     runs_neg_2.push_back(*ik);
     }
     else{
       if(run_info[runnum]["target"]["target_id"].get<int>()==3){
       runs_neg_3.push_back(*ik);
       }
       else{
       runs_neg_5.push_back(*ik);
       }
     }
   }
   else{
     if(run_info[runnum]["target"]["target_id"].get<int>()==2){
     runs_pos_2.push_back(*ik);
     }
     else{
       if(run_info[runnum]["target"]["target_id"].get<int>()==3){
       runs_pos_3.push_back(*ik);
       }
       else{
       runs_pos_5.push_back(*ik);
       }
     }
   }

      //std::cout<<run_info[runnum]["spectrometers"]["hms_momentum"]<<std::endl;
    }
    else{
      json run_info;
      {
        std::ifstream json_input("run_list_update.json");
        json_input>>run_info;
      }
    if(run_info[runnum]["spectrometers"]["shms_momentum"].get<double>()<0){
     if(run_info[runnum]["target"]["target_id"].get<int>()==2){
     runs_neg_2.push_back(*ik);
     }
     else{
       if(run_info[runnum]["target"]["target_id"].get<int>()==3){
       runs_neg_3.push_back(*ik);
       }
       else{
       runs_neg_5.push_back(*ik);
       }
     }
   }
   else{
     if(run_info[runnum]["target"]["target_id"].get<int>()==2){
     runs_pos_2.push_back(*ik);
     }
     else{
       if(run_info[runnum]["target"]["target_id"].get<int>()==3){
       runs_pos_3.push_back(*ik);
       }
       else{
       runs_pos_5.push_back(*ik);
       }
     }
   }
    }
      //auto runinfo = run_info.find(runnum);
    //auto runinfojs=*runinfo;
    //std::cout<<*ik<<" "<<run_info[runnum]["spectrometers"]["hms_momentum"]<<std::endl;
    }
    jout[ii]["neg"]["H2"]=runs_neg_2;
    jout[ii]["neg"]["D2"]=runs_neg_3;
    jout[ii]["neg"]["Dummy"]=runs_neg_5;
    jout[ii]["pos"]["H2"]=runs_pos_2;
    jout[ii]["pos"]["D2"]=runs_pos_3;
    jout[ii]["pos"]["Dummy"]=runs_pos_5;
    //if(shmsp>0){
    //  if(targetid==2)
    //  { 
    //    std::vector<int> runs2;
    //    for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //      outfile<<*ik<<" ";
    //      int runnum = *ik;
    //      runs.push_back(*ik);
    //      nn++;
    //      runs2.push_back(*ik);
    //    }
    //    jout[ii]["pos"]["H2"]["runs"]=runs2;
    //    out<<jout<<std::endl;     
    //  }
    //  else{
    //    if(targetid==3){
    //      std::vector<int> runs3;
    //      for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //        outfile<<*ik<<" ";
    //        int runnum = *ik;
    //        runs.push_back(*ik);
    //        nn++;
    //        runs3.push_back(*ik);
    //      }
    //      jout[ii]["pos"]["D2"]["runs"]=runs3;
    //      out<<jout<<std::endl;
    //    }
    //    else{
    //      std::vector<int> runs5;
    //      for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //        outfile<<*ik<<" ";
    //        int runnum = *ik;
    //        runs.push_back(*ik);
    //        nn++;
    //        runs5.push_back(*ik);
    //      }
    //      jout[ii]["pos"]["Dummy"]["runs"] = runs5;
    //      out<<jout<<std::endl;
    //    }
    //  }
    //}
    //else{
    //  if(targetid==2)
    //  { 
    //    std::vector<int> runs2;
    //    for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //      outfile<<*ik<<" ";
    //      int runnum = *ik;
    //      runs.push_back(*ik);
    //      nn++;
    //      runs2.push_back(*ik);
    //    }
    //    jout[ii]["neg"]["H2"]["runs"]=runs2;
    //    out<<jout<<std::endl;     
    //  }
    //  else{
    //    if(targetid==3){
    //      std::vector<int> runs3;
    //      for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //        outfile<<*ik<<" ";
    //        int runnum = *ik;
    //        runs.push_back(*ik);
    //        nn++;
    //        runs3.push_back(*ik);
    //      }
    //      jout[ii]["neg"]["D2"]["runs"]=runs3;
    //      out<<jout<<std::endl;
    //    }
    //    else{
    //      std::vector<int> runs5;
    //      for(auto ik = it->second.begin();ik!= it->second.end();++ik){
    //        outfile<<*ik<<" ";
    //        int runnum = *ik;
    //        runs.push_back(*ik);
    //        nn++;
    //        runs5.push_back(*ik);
    //      }
    //      jout[ii]["neg"]["Dummy"]["runs"] = runs5;
    //      out<<jout<<std::endl;
    //    }
    //  }
    //}
   i++;
   outfile<<"\n";
   // string _hms_cal_spring = "csvspringcalib/hms_cal/pcal.param.coin_replay_production_";
   // string _shms_cal_spring = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
   // string _shms_cal_fall = "csvfallcalib/shms_cal/pcal.param.coin_replay_production_";
   // for(auto ik = it->second.begin();ik!= it->second.end();++ik){
   //   string name_hms_cal_spring = _hms_cal_spring+std::to_string(*ik)+"_-1_0_-1";
   //  // std::cout<<name_hms_cal_spring<<std::endl;
   //   std::ifstream infile_hms_cal_spring;
   //   infile_hms_cal_spring.open(name_hms_cal_spring);
   //   if(infile_hms_cal_spring.good()){outfile<<"#include \"PARAM/HMS/CAL/pcal.param.coin_replay_production_"<<std::to_string(*ik)<<"_-1_0_-1"<<"\""<<"\n";}
   //   string name_shms_cal_spring = _shms_cal_spring+std::to_string(*ik)+"_-1_0_-1";
   // //  std::cout<<name_shms_cal_spring<<std::endl;
   //   std::ifstream infile_shms_cal_spring;
   //   infile_shms_cal_spring.open(name_shms_cal_spring);
   //   if(infile_shms_cal_spring.good()){
   //     std::cout<<name_shms_cal_spring<<std::endl;
   //     outfile<<"#include \"PARAM/SHMS/CAL/pcal.param.coin_replay_production_"<<std::to_string(*ik)<<"_-1_0_-1"<<"\""<<"\n";}
   //   string name_shms_cal_fall = _shms_cal_fall+std::to_string(*ik)+"_-1_0_-1";
   //   //std::cout<<name_shms_cal_fall<<std::endl;
   //   std::ifstream infile_shms_cal_fall;
   //   infile_shms_cal_fall.open(name_shms_cal_fall);
   //   if(infile_shms_cal_fall.good()){outfile<<"#include \"PARAM/SHMS/CAL/pcal.param.coin_replay_production_"<<std::to_string(*ik)<<"_-1_0_-1"<<"\""<<"\n";}
   //   
   // }  
   // outfile<<"\n";  
  }
    out<<jout.dump(4)<<std::endl;
  
  std::sort(runs.begin(),runs.end());
  std::cout<<"vector size"<<runs.size()<<std::endl;
//  for(auto it = runs.begin();it!= runs.end(); ++it){
//    std::cout<<*it<<std::endl;
//  }
}

