#include <iostream>
#include <string>

class Read_CALPARAM{
  
  std::ifstream& ifs;
  std::vector<double> _neg, _pos;
  std::vector<std::vector<double>> _arr;
  int entries;
  
  void skip_first(){
  assert(ifs.good());
  ifs.ignore(5000,',');
  }
  void read_entries(){
    std::string line;
    std::getline(ifs,line);
    std::istringstream ss(line);
    std::string tmp;
    std::getline(ss,tmp,'');
    int found;
    while(ss.good()){
    ss>>tmp;
    if(stringstream(tmp)>>found)
     entries = found;
    std::getline(ss,tmp,',');
    }
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

  //Read_CALPARAM(std::string if_name):ifs(ifstream(if_name)){

  //}
  Read_CALPARAM(std::ifstream& in_ifs):ifs(in_ifs){
    skip_first();
    read_entries();
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
  const int nentries(){return entries;}
};
void readcalparam(){
  std::ifstream infile;
  infile.open("csvspringcalib/shms_cal/pcal.param.coin_replay_production_7593_-1_0_-1");
  Read_CALPARAM rc(infile);
std::cout<<rc.nentries()<<std::endl;

}

