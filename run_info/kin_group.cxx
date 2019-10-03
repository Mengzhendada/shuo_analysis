#include <vector>
#include <string>
#include <map>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>

struct run_kin {
  double Q2; 
  double x;
  run_kin(double q2,double bjx):Q2(q2),x(bjx){}
  bool operator ==(const run_kin& kinobj)const
  {
    return kinobj.Q2==this->Q2 && kinobj.x==this->x;
  }
  bool operator <(const run_kin& kinobj)const
  {
    return (this->Q2<kinobj.Q2)||(this->Q2==kinobj.Q2 && this->x<kinobj.x);
  }
}

void kin_group(){
   json j;
   {
     std::ifstream json_input_file("db2/ratio_run_group_updated.json");
     json_input_file>>j;
   }
   run_kin runkin(0,0);
   std::map<run_kin,vector<int>> mymap;
   for(json::iterator it = j.begin(); it!=j.end();++it){
   auto runjs = it.value();
   runkin.Q2 = runjs[""]
   }
}
