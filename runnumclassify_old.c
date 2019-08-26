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
    return keyobj._hms_p<this->_hms_p || (keyobj._hms_p==this->_hms_p && (keyobj._hms_th<this->_hms_th || (keyobj._hms_th==this->_hms_th && ( keyobj._shms_p<this->_shms_p || ( keyobj._shms_p==this->_shms_p && keyobj._shms_th<this->_shms_th))))); 
  }
  bool operator ==(const run_key& keyobj)const
  {
    return keyobj._hms_p==this->_hms_p && keyobj._hms_th==this->_hms_th && keyobj._shms_p==this->_shms_p && keyobj._shms_th==this->_shms_th; 
  }
};
std::ostream& operator<<(std::ostream& os,const run_key& k){
  os << "hms_p " <<k._hms_p << " hms_th " << k._hms_th << " shms_p " << k._shms_p << " shms_th " << k._shms_th << " ";
  return os;
}

void runnumclassify(){
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
  std::ofstream outfile;
  outfile.open("runs_classify.txt");
  run_key runskey(0,0,0,0);
  std::map<run_key,vector<int>> mymap;
  for(json::iterator it = j.begin(); it!= j.end(); ++it){
    if(std::stoi(it.key())>=6009 && std::stoi(it.key())<= 6532){
      runskey.hms_p() = it.value()["spectrometers"]["hms_momentum"].get<double>();
      runskey.hms_th() = it.value()["spectrometers"]["hms_angle"].get<double>();
      runskey.shms_p() = it.value()["spectrometers"]["shms_momentum"].get<double>();
      runskey.shms_th() = it.value()["spectrometers"]["shms_angle"].get<double>();
      //mymap.insert(std::make_pair<run_key,int>(runskey,std::stoi(it.key())));
      //std::cout << "pre: " << setw(5) << mymap.size() << std::endl;//<< setw(5) << mymap.at(runskey).size() << endl;
      mymap[runskey].push_back(std::stoi(it.key()));
      //std::cout << "pos: " << setw(5) << mymap.size() << std::endl;//flush <<  setw(5) <<  mymap.at(runskey).size() << endl;
      //std::cout<<std::stoi(it.key())<<std::endl;
    }}
  std::vector<int> runs;
  for(auto it = mymap.begin();it!= mymap.end();++it){
    outfile<< "#"<<it->first<<"\n";
    for(auto ik = it->second.begin();ik!= it->second.end();++ik){
      outfile<<*ik<<" ";
      runs.push_back(*ik);
    }
    outfile<<"\n";  
  }
  std::sort(runs.begin(),runs.end());
  std::cout<<"vector size"<<runs.size()<<std::endl;
  for(auto it = runs.begin();it!= runs.end(); ++it){
    std::cout<<*it<<std::endl;
  }
}
