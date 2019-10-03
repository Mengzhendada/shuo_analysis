#ifndef SHMS_CAL_Read_Calparam_hh
#define SHMS_CAL_Read_Calparam_hh

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
class SHMS_CAL_Read_Calparam{
  
  std::ifstream& ifs;
  std::vector<double> _neg, _pos;
  std::vector<std::vector<double>> _arr;
  int entries=0;
  
  
  void read_entries(){
          assert(ifs.good());
          ifs.ignore(5000,',');
          std::string line;
          std::getline(ifs,line);
          std::string temp;
          for(int i = 0;i<line[i];i++){
        	  if(isdigit(line[i])){
        		  for(int a =i; a<line.size();a++){
        			  temp +=line[a];
        		  }
        		  break;
        	  }
          }
          //std::cout<<temp<<std::endl;
          std::istringstream stream(temp);
          stream>>entries;
          temp.clear();
  }
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

  //SHMS_CAL_Read_Calparam(std::string if_name):ifs(ifstream(if_name)){

  //}
  SHMS_CAL_Read_Calparam(std::ifstream& in_ifs):ifs(in_ifs){
    read_entries();
    skip();
    read_neg();
    skip();
    read_pos();
    skip();
    read_arr();
  }
  ~SHMS_CAL_Read_Calparam(){};
  const std::vector<double>& neg(){return _neg;}
  const std::vector<double>& pos(){return _pos;}
  const std::vector<std::vector<double>>& arr(){return _arr;}
  const int& nentries(){return entries;}
};

#endif
