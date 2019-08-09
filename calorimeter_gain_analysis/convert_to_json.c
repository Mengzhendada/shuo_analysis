#include "nlohmann/json.hpp"
#include "ROOT/RDataFrame.hxx"
#include <vector>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
using json = nlohmann::json;
//Read calorimeter calibrate numbers
class Read_CALPARAM{
  
  std::ifstream& ifs;
  std::vector<double> _neg, _pos;
  std::vector<std::vector<double>> _arr;
  int entries;
  
  void skip_first(){
  assert(ifs.good());
  ifs.ignore(5000,'=');
  }
  void read_entries{
    std::string line;
    std::getline(ifs,line);
    std::istringstream ss(line);
    std::string tmp;
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

void to_json(){
  Read_CALPARAM* rc[15];
  std::ifstream in_runs;
  in_runs.open("spring_selected_info.txt");
  int runnumber[15];
  double momentum[15];

  //std::ifstream infile;
  std::string name = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
  // for(in = 0;in<15;++in){
  //in_runs>>runnumber[in]>>momentum[in];
  // }
  json j,tmp;
  // j = {{"one","eins"}};
  for(int in = 0;in<15;++in){
    in_runs>>runnumber[in]>>momentum[in];
    std::string  name = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
    name = name+std::to_string(runnumber[in])+"_-1_0_-1";
    std::cout<<name<<std::endl;
    std::ifstream infile;
    infile.open(name);
    if(infile.good()){
      std::cout<<"good "<<in<<std::endl;
      rc[in]= new Read_CALPARAM(infile);}
    else{exit;}

    std::string runnum = std::to_string(runnumber[in]);
    std::cout<<rc[in]->neg()[0]<<std::endl;
    tmp = {
      {runnum,{
                {"momentum",momentum[in]},
                {"neg",{
                         {"0",rc[in]->neg()[0]},
                         {"1",rc[in]->neg()[1]},
                         {"2",rc[in]->neg()[2]},
                         {"3",rc[in]->neg()[3]},
                         {"4",rc[in]->neg()[4]},
                         {"5",rc[in]->neg()[5]},
                         {"6",rc[in]->neg()[6]},
                         {"7",rc[in]->neg()[7]},
                         {"8",rc[in]->neg()[8]},
                         {"9",rc[in]->neg()[9]},
                         {"10",rc[in]->neg()[10]},
                         {"11",rc[in]->neg()[11]},
                         {"12",rc[in]->neg()[12]},
                         {"13",rc[in]->neg()[13]}
                       }
                },
                {"pos",{
                         {"0",rc[in]->pos()[0]},
                         {"1",rc[in]->pos()[1]},
                         {"2",rc[in]->pos()[2]},
                         {"3",rc[in]->pos()[3]},
                         {"4",rc[in]->pos()[4]},
                         {"5",rc[in]->pos()[5]},
                         {"6",rc[in]->pos()[6]},
                         {"7",rc[in]->pos()[7]},
                         {"8",rc[in]->pos()[8]},
                         {"9",rc[in]->pos()[9]},
                         {"10",rc[in]->pos()[10]},
                         {"11",rc[in]->pos()[11]},
                         {"12",rc[in]->pos()[12]},
                         {"13",rc[in]->pos()[13]}
                       }
                },
                {"arr",{
                         {"0",{
                                {"0",rc[in]->arr()[0][0]},
                                {"1",rc[in]->arr()[0][1]},
                                {"2",rc[in]->arr()[0][2]},
                                {"3",rc[in]->arr()[0][3]},
                                {"4",rc[in]->arr()[0][4]},
                                {"5",rc[in]->arr()[0][5]},
                                {"6",rc[in]->arr()[0][6]},
                                {"7",rc[in]->arr()[0][7]},
                                {"8",rc[in]->arr()[0][8]},
                                {"9",rc[in]->arr()[0][9]},
                                {"10",rc[in]->arr()[0][10]},
                                {"11",rc[in]->arr()[0][11]},
                                {"12",rc[in]->arr()[0][12]},
                                {"13",rc[in]->arr()[0][13]},
                                {"14",rc[in]->arr()[0][14]},
                                {"15",rc[in]->arr()[0][15]}
                              }},
                         {"1",{
                                {"0",rc[in]->arr()[1][0]},
                                {"1",rc[in]->arr()[1][1]},
                                {"2",rc[in]->arr()[1][2]},
                                {"3",rc[in]->arr()[1][3]},
                                {"4",rc[in]->arr()[1][4]},
                                {"5",rc[in]->arr()[1][5]},
                                {"6",rc[in]->arr()[1][6]},
                                {"7",rc[in]->arr()[1][7]},
                                {"8",rc[in]->arr()[1][8]},
                                {"9",rc[in]->arr()[1][9]},
                                {"10",rc[in]->arr()[1][10]},
                                {"11",rc[in]->arr()[1][11]},
                                {"12",rc[in]->arr()[1][12]},
                                {"13",rc[in]->arr()[1][13]},
                                {"14",rc[in]->arr()[1][14]},
                                {"15",rc[in]->arr()[1][15]}
                              }},
                         {"2",{
                                {"0",rc[in]->arr()[2][0]},
                                {"1",rc[in]->arr()[2][1]},
                                {"2",rc[in]->arr()[2][2]},
                                {"3",rc[in]->arr()[2][3]},
                                {"4",rc[in]->arr()[2][4]},
                                {"5",rc[in]->arr()[2][5]},
                                {"6",rc[in]->arr()[2][6]},
                                {"7",rc[in]->arr()[2][7]},
                                {"8",rc[in]->arr()[2][8]},
                                {"9",rc[in]->arr()[2][9]},
                                {"10",rc[in]->arr()[2][10]},
                                {"11",rc[in]->arr()[2][11]},
                                {"12",rc[in]->arr()[2][12]},
                                {"13",rc[in]->arr()[2][13]},
                                {"14",rc[in]->arr()[2][14]},
                                {"15",rc[in]->arr()[2][15]}
                              }},
                         {"3",{
                                {"0",rc[in]->arr()[3][0]},
                                {"1",rc[in]->arr()[3][1]},
                                {"2",rc[in]->arr()[3][2]},
                                {"3",rc[in]->arr()[3][3]},
                                {"4",rc[in]->arr()[3][4]},
                                {"5",rc[in]->arr()[3][5]},
                                {"6",rc[in]->arr()[3][6]},
                                {"7",rc[in]->arr()[3][7]},
                                {"8",rc[in]->arr()[3][8]},
                                {"9",rc[in]->arr()[3][9]},
                                {"10",rc[in]->arr()[3][10]},
                                {"11",rc[in]->arr()[3][11]},
                                {"12",rc[in]->arr()[3][12]},
                                {"13",rc[in]->arr()[3][13]},
                                {"14",rc[in]->arr()[3][14]},
                                {"15",rc[in]->arr()[3][15]}
                              }},
                         {"4",{
                                {"0",rc[in]->arr()[4][0]},
                                {"1",rc[in]->arr()[4][1]},
                                {"2",rc[in]->arr()[4][2]},
                                {"3",rc[in]->arr()[4][3]},
                                {"4",rc[in]->arr()[4][4]},
                                {"5",rc[in]->arr()[4][5]},
                                {"6",rc[in]->arr()[4][6]},
                                {"7",rc[in]->arr()[4][7]},
                                {"8",rc[in]->arr()[4][8]},
                                {"9",rc[in]->arr()[4][9]},
                                {"10",rc[in]->arr()[4][10]},
                                {"11",rc[in]->arr()[4][11]},
                                {"12",rc[in]->arr()[4][12]},
                                {"13",rc[in]->arr()[4][13]},
                                {"14",rc[in]->arr()[4][14]},
                                {"15",rc[in]->arr()[4][15]}
                              }},
                         {"5",{
                                {"0",rc[in]->arr()[5][0]},
                                {"1",rc[in]->arr()[5][1]},
                                {"2",rc[in]->arr()[5][2]},
                                {"3",rc[in]->arr()[5][3]},
                                {"4",rc[in]->arr()[5][4]},
                                {"5",rc[in]->arr()[5][5]},
                                {"6",rc[in]->arr()[5][6]},
                                {"7",rc[in]->arr()[5][7]},
                                {"8",rc[in]->arr()[5][8]},
                                {"9",rc[in]->arr()[5][9]},
                                {"10",rc[in]->arr()[5][10]},
                                {"11",rc[in]->arr()[5][11]},
                                {"12",rc[in]->arr()[5][12]},
                                {"13",rc[in]->arr()[5][13]},
                                {"14",rc[in]->arr()[5][14]},
                                {"15",rc[in]->arr()[5][15]}
                              }},
                         {"6",{
                                {"0",rc[in]->arr()[6][0]},
                                {"1",rc[in]->arr()[6][1]},
                                {"2",rc[in]->arr()[6][2]},
                                {"3",rc[in]->arr()[6][3]},
                                {"4",rc[in]->arr()[6][4]},
                                {"5",rc[in]->arr()[6][5]},
                                {"6",rc[in]->arr()[6][6]},
                                {"7",rc[in]->arr()[6][7]},
                                {"8",rc[in]->arr()[6][8]},
                                {"9",rc[in]->arr()[6][9]},
                                {"10",rc[in]->arr()[6][10]},
                                {"11",rc[in]->arr()[6][11]},
                                {"12",rc[in]->arr()[6][12]},
                                {"13",rc[in]->arr()[6][13]},
                                {"14",rc[in]->arr()[6][14]},
                                {"15",rc[in]->arr()[6][15]}
                              }},
                         {"7",{
                                {"0",rc[in]->arr()[7][0]},
                                {"1",rc[in]->arr()[7][1]},
                                {"2",rc[in]->arr()[7][2]},
                                {"3",rc[in]->arr()[7][3]},
                                {"4",rc[in]->arr()[7][4]},
                                {"5",rc[in]->arr()[7][5]},
                                {"6",rc[in]->arr()[7][6]},
                                {"7",rc[in]->arr()[7][7]},
                                {"8",rc[in]->arr()[7][8]},
                                {"9",rc[in]->arr()[7][9]},
                                {"10",rc[in]->arr()[7][10]},
                                {"11",rc[in]->arr()[7][11]},
                                {"12",rc[in]->arr()[7][12]},
                                {"13",rc[in]->arr()[7][13]},
                                {"14",rc[in]->arr()[7][14]},
                                {"15",rc[in]->arr()[7][15]}
                              }},
                         {"8",{
                                {"0",rc[in]->arr()[8][0]},
                                {"1",rc[in]->arr()[8][1]},
                                {"2",rc[in]->arr()[8][2]},
                                {"3",rc[in]->arr()[8][3]},
                                {"4",rc[in]->arr()[8][4]},
                                {"5",rc[in]->arr()[8][5]},
                                {"6",rc[in]->arr()[8][6]},
                                {"7",rc[in]->arr()[8][7]},
                                {"8",rc[in]->arr()[8][8]},
                                {"9",rc[in]->arr()[8][9]},
                                {"10",rc[in]->arr()[8][10]},
                                {"11",rc[in]->arr()[8][11]},
                                {"12",rc[in]->arr()[8][12]},
                                {"13",rc[in]->arr()[8][13]},
                                {"14",rc[in]->arr()[8][14]},
                                {"15",rc[in]->arr()[8][15]}
                              }},
                         {"9",{
                                {"0",rc[in]->arr()[9][0]},
                                {"1",rc[in]->arr()[9][1]},
                                {"2",rc[in]->arr()[9][2]},
                                {"3",rc[in]->arr()[9][3]},
                                {"4",rc[in]->arr()[9][4]},
                                {"5",rc[in]->arr()[9][5]},
                                {"6",rc[in]->arr()[9][6]},
                                {"7",rc[in]->arr()[9][7]},
                                {"8",rc[in]->arr()[9][8]},
                                {"9",rc[in]->arr()[9][9]},
                                {"10",rc[in]->arr()[9][10]},
                                {"11",rc[in]->arr()[9][11]},
                                {"12",rc[in]->arr()[9][12]},
                                {"13",rc[in]->arr()[9][13]},
                                {"14",rc[in]->arr()[9][14]},
                                {"15",rc[in]->arr()[9][15]}
                              }},
                         {"10",{
                                 {"0",rc[in]->arr()[10][0]},
                                 {"1",rc[in]->arr()[10][1]},
                                 {"2",rc[in]->arr()[10][2]},
                                 {"3",rc[in]->arr()[10][3]},
                                 {"4",rc[in]->arr()[10][4]},
                                 {"5",rc[in]->arr()[10][5]},
                                 {"6",rc[in]->arr()[10][6]},
                                 {"7",rc[in]->arr()[10][7]},
                                 {"8",rc[in]->arr()[10][8]},
                                 {"9",rc[in]->arr()[10][9]},
                                 {"10",rc[in]->arr()[10][10]},
                                 {"11",rc[in]->arr()[10][11]},
                                 {"12",rc[in]->arr()[10][12]},
                                 {"13",rc[in]->arr()[10][13]},
                                 {"14",rc[in]->arr()[10][14]},
                                 {"15",rc[in]->arr()[10][15]}
                               }},
                         {"11",{
                                 {"0",rc[in]->arr()[11][0]},
                                 {"1",rc[in]->arr()[11][1]},
                                 {"2",rc[in]->arr()[11][2]},
                                 {"3",rc[in]->arr()[11][3]},
                                 {"4",rc[in]->arr()[11][4]},
                                 {"5",rc[in]->arr()[11][5]},
                                 {"6",rc[in]->arr()[11][6]},
                                 {"7",rc[in]->arr()[11][7]},
                                 {"8",rc[in]->arr()[11][8]},
                                 {"9",rc[in]->arr()[11][9]},
                                 {"10",rc[in]->arr()[11][10]},
                                 {"11",rc[in]->arr()[11][11]},
                                 {"12",rc[in]->arr()[11][12]},
                                 {"13",rc[in]->arr()[11][13]},
                                 {"14",rc[in]->arr()[11][14]},
                                 {"15",rc[in]->arr()[11][15]}
                               }},
                         {"12",{
                                 {"0",rc[in]->arr()[12][0]},
                                 {"1",rc[in]->arr()[12][1]},
                                 {"2",rc[in]->arr()[12][2]},
                                 {"3",rc[in]->arr()[12][3]},
                                 {"4",rc[in]->arr()[12][4]},
                                 {"5",rc[in]->arr()[12][5]},
                                 {"6",rc[in]->arr()[12][6]},
                                 {"7",rc[in]->arr()[12][7]},
                                 {"8",rc[in]->arr()[12][8]},
                                 {"9",rc[in]->arr()[12][9]},
                                 {"10",rc[in]->arr()[12][10]},
                                 {"11",rc[in]->arr()[12][11]},
                                 {"12",rc[in]->arr()[12][12]},
                                 {"13",rc[in]->arr()[12][13]},
                                 {"14",rc[in]->arr()[12][14]},
                                 {"15",rc[in]->arr()[12][15]}
                               }},
                         {"13",{
                                 {"0",rc[in]->arr()[13][0]},
                                 {"1",rc[in]->arr()[13][1]},
                                 {"2",rc[in]->arr()[13][2]},
                                 {"3",rc[in]->arr()[13][3]},
                                 {"4",rc[in]->arr()[13][4]},
                                 {"5",rc[in]->arr()[13][5]},
                                 {"6",rc[in]->arr()[13][6]},
                                 {"7",rc[in]->arr()[13][7]},
                                 {"8",rc[in]->arr()[13][8]},
                                 {"9",rc[in]->arr()[13][9]},
                                 {"10",rc[in]->arr()[13][10]},
                                 {"11",rc[in]->arr()[13][11]},
                                 {"12",rc[in]->arr()[13][12]},
                                 {"13",rc[in]->arr()[13][13]},
                                 {"14",rc[in]->arr()[13][14]},
                                 {"15",rc[in]->arr()[13][15]}
                               }
                          }
                         
                       }
                }
              }
      }
//tmp 
    };
//    if(in == 0){j=tmp;}
//    else{
//      j.insert(tmp.begin(),tmp.end());
//    }
//for loop
//  }
  std::string jsonname = fmt::format("cal_gain_json/shms_cal_gain_{}.json",runnumber[in]).c_str(); 
  std::ofstream o2(jsonname);
  o2<<std::setw(4)<<tmp<<std::endl;
}
  //all
}
