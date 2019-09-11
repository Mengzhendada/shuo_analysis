#include "ROOT/RDataFrame.hxx"
#include <vector>
#include <iostream>
#include <sstream>
#include "TCanvas.h"

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

void creat_rdataset_begining(){
  Read_CALPARAM* rc[15];
  std::ifstream in_runs;
  in_runs.open("spring_selected_info.txt");
  int runnumber[15];
  double momentum[15];
  int in = 0;
  //std::ifstream infile;
  //std::string name = "csvspringcalib/shms_cal/pcal.param.coin_replay_production_";
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
  //  std::cout<<rc[in]->arr()[1][1]<<std::endl;    
    //infile.open(0); 
  }

 // ROOT::RDataFrame tdf(15);
 // auto b = 0;
 // std::cout<<"second number "<<rc[0]->neg()[0]<<std::endl;
 // auto d = tdf
 //   .Define("pos_0",[&b,rc](){return rc[b++]->pos()[0];})
    //.Define("pos","(double)pos_0*pos_0")
 //    .Define("neg_0",[&b,rc](){return rc[b++]->neg()[0];})
   // .Define("pos_1",[&b,rc](){return rc[b++]->pos()[1];})
   // .Define("neg_1",[&b,rc](){return rc[b++]->neg()[1];})
   // .Define("pos_2",[&b,rc](){return rc[b++]->pos()[2];})
   // .Define("neg_2",[&b,rc](){return rc[b++]->neg()[2];})
   // .Define("pos_3",[&b,rc](){return rc[b++]->pos()[3];})
   // .Define("neg_3",[&b,rc](){return rc[b++]->neg()[3];})
   // .Define("pos_4",[&b,rc](){return rc[b++]->pos()[4];})
   // .Define("neg_4",[&b,rc](){return rc[b++]->neg()[4];})
   // .Define("pos_5",[&b,rc](){return rc[b++]->pos()[5];})
   // .Define("neg_5",[&b,rc](){return rc[b++]->neg()[5];})
   // .Define("pos_6",[&b,rc](){return rc[b++]->pos()[6];})
   // .Define("neg_6",[&b,rc](){return rc[b++]->neg()[6];})
   // .Define("pos_7",[&b,rc](){return rc[b++]->pos()[7];})
   // .Define("neg_7",[&b,rc](){return rc[b++]->neg()[7];})
   // .Define("pos_8",[&b,rc](){return rc[b++]->pos()[8];})
   // .Define("neg_8",[&b,rc](){return rc[b++]->neg()[8];})
   // .Define("pos_9",[&b,rc](){return rc[b++]->pos()[9];})
   // .Define("neg_9",[&b,rc](){return rc[b++]->neg()[9];})
   // .Define("pos_10",[&b,rc](){return rc[b++]->pos()[10];})
   // .Define("neg_10",[&b,rc](){return rc[b++]->neg()[10];})
   // .Define("pos_11",[&b,rc](){return rc[b++]->pos()[11];})
   // .Define("neg_11",[&b,rc](){return rc[b++]->neg()[11];})
   // .Define("pos_12",[&b,rc](){return rc[b++]->pos()[12];})
   // .Define("neg_12",[&b,rc](){return rc[b++]->neg()[12];})
   // .Define("pos_13",[&b,rc](){return rc[b++]->pos()[13];})
   // .Define("neg_13",[&b,rc](){return rc[b++]->neg()[13];})
//    .Define("arr_0_0",[&b,rc](){return rc[b++]->arr()[0][0];})
//    .Define("arr_0_1",[&b,rc](){return rc[b++]->arr()[0][1];})
//    .Define("arr_0_2",[&b,rc](){return rc[b++]->arr()[0][2];})
//    .Define("arr_0_3",[&b,rc](){return rc[b++]->arr()[0][3];})
//    .Define("arr_0_4",[&b,rc](){return rc[b++]->arr()[0][4];})
//    .Define("arr_0_5",[&b,rc](){return rc[b++]->arr()[0][5];})
//    .Define("arr_0_6",[&b,rc](){return rc[b++]->arr()[0][6];})
//    .Define("arr_0_7",[&b,rc](){return rc[b++]->arr()[0][7];})
//    .Define("arr_0_8",[&b,rc](){return rc[b++]->arr()[0][8];})
//    .Define("arr_0_9",[&b,rc](){return rc[b++]->arr()[0][9];})
//    .Define("arr_0_10",[&b,rc](){return rc[b++]->arr()[0][10];})
//    .Define("arr_0_11",[&b,rc](){return rc[b++]->arr()[0][11];})
//    .Define("arr_0_12",[&b,rc](){return rc[b++]->arr()[0][12];})
//    .Define("arr_0_13",[&b,rc](){return rc[b++]->arr()[0][13];})
//    .Define("arr_0_14",[&b,rc](){return rc[b++]->arr()[0][14];})
//    .Define("arr_0_15",[&b,rc](){return rc[b++]->arr()[0][15];})
//    .Define("arr_1_0",[&b,rc](){return rc[b++]->arr()[1][0];})
//    .Define("arr_1_1",[&b,rc](){return rc[b++]->arr()[1][1];})
//    .Define("arr_1_2",[&b,rc](){return rc[b++]->arr()[1][2];})
//    .Define("arr_1_3",[&b,rc](){return rc[b++]->arr()[1][3];})
//    .Define("arr_1_4",[&b,rc](){return rc[b++]->arr()[1][4];})
//    .Define("arr_1_5",[&b,rc](){return rc[b++]->arr()[1][5];})
//    .Define("arr_1_6",[&b,rc](){return rc[b++]->arr()[1][6];})
//    .Define("arr_1_7",[&b,rc](){return rc[b++]->arr()[1][7];})
//    .Define("arr_1_8",[&b,rc](){return rc[b++]->arr()[1][8];})
//    .Define("arr_1_9",[&b,rc](){return rc[b++]->arr()[1][9];})
//    .Define("arr_1_10",[&b,rc](){return rc[b++]->arr()[1][10];})
//    .Define("arr_1_11",[&b,rc](){return rc[b++]->arr()[1][11];})
//    .Define("arr_1_12",[&b,rc](){return rc[b++]->arr()[1][12];})
//    .Define("arr_1_13",[&b,rc](){return rc[b++]->arr()[1][13];})
//    .Define("arr_1_14",[&b,rc](){return rc[b++]->arr()[1][14];})
//    .Define("arr_1_15",[&b,rc](){return rc[b++]->arr()[1][15];})
//    .Define("arr_2_0",[&b,rc](){return rc[b++]->arr()[2][0];})
//    .Define("arr_2_1",[&b,rc](){return rc[b++]->arr()[2][1];})
//    .Define("arr_2_2",[&b,rc](){return rc[b++]->arr()[2][2];})
//    .Define("arr_2_3",[&b,rc](){return rc[b++]->arr()[2][3];})
//    .Define("arr_2_4",[&b,rc](){return rc[b++]->arr()[2][4];})
//    .Define("arr_2_5",[&b,rc](){return rc[b++]->arr()[2][5];})
//    .Define("arr_2_6",[&b,rc](){return rc[b++]->arr()[2][6];})
//    .Define("arr_2_7",[&b,rc](){return rc[b++]->arr()[2][7];})
//    .Define("arr_2_8",[&b,rc](){return rc[b++]->arr()[2][8];})
//    .Define("arr_2_9",[&b,rc](){return rc[b++]->arr()[2][9];})
//    .Define("arr_2_10",[&b,rc](){return rc[b++]->arr()[2][10];})
//    .Define("arr_2_11",[&b,rc](){return rc[b++]->arr()[2][11];})
//    .Define("arr_2_12",[&b,rc](){return rc[b++]->arr()[2][12];})
//    .Define("arr_2_13",[&b,rc](){return rc[b++]->arr()[2][13];})
//    .Define("arr_2_14",[&b,rc](){return rc[b++]->arr()[2][14];})
//    .Define("arr_2_15",[&b,rc](){return rc[b++]->arr()[2][15];})
//    .Define("arr_3_0",[&b,rc](){return rc[b++]->arr()[3][0];})
//    .Define("arr_3_1",[&b,rc](){return rc[b++]->arr()[3][1];})
//    .Define("arr_3_2",[&b,rc](){return rc[b++]->arr()[3][2];})
//    .Define("arr_3_3",[&b,rc](){return rc[b++]->arr()[3][3];})
//    .Define("arr_3_4",[&b,rc](){return rc[b++]->arr()[3][4];})
//    .Define("arr_3_5",[&b,rc](){return rc[b++]->arr()[3][5];})
//    .Define("arr_3_6",[&b,rc](){return rc[b++]->arr()[3][6];})
//    .Define("arr_3_7",[&b,rc](){return rc[b++]->arr()[3][7];})
//    .Define("arr_3_8",[&b,rc](){return rc[b++]->arr()[3][8];})
//    .Define("arr_3_9",[&b,rc](){return rc[b++]->arr()[3][9];})
//    .Define("arr_3_10",[&b,rc](){return rc[b++]->arr()[3][10];})
//    .Define("arr_3_11",[&b,rc](){return rc[b++]->arr()[3][11];})
//    .Define("arr_3_12",[&b,rc](){return rc[b++]->arr()[3][12];})
//    .Define("arr_3_13",[&b,rc](){return rc[b++]->arr()[3][13];})
//    .Define("arr_3_14",[&b,rc](){return rc[b++]->arr()[3][14];})
//    .Define("arr_3_15",[&b,rc](){return rc[b++]->arr()[3][15];})
//    .Define("arr_4_0",[&b,rc](){return rc[b++]->arr()[4][0];})
//    .Define("arr_4_1",[&b,rc](){return rc[b++]->arr()[4][1];})
//    .Define("arr_4_2",[&b,rc](){return rc[b++]->arr()[4][2];})
//    .Define("arr_4_3",[&b,rc](){return rc[b++]->arr()[4][3];})
//    .Define("arr_4_4",[&b,rc](){return rc[b++]->arr()[4][4];})
//    .Define("arr_4_5",[&b,rc](){return rc[b++]->arr()[4][5];})
//    .Define("arr_4_6",[&b,rc](){return rc[b++]->arr()[4][6];})
//    .Define("arr_4_7",[&b,rc](){return rc[b++]->arr()[4][7];})
//    .Define("arr_4_8",[&b,rc](){return rc[b++]->arr()[4][8];})
//    .Define("arr_4_9",[&b,rc](){return rc[b++]->arr()[4][9];})
//    .Define("arr_4_10",[&b,rc](){return rc[b++]->arr()[4][10];})
//    .Define("arr_4_11",[&b,rc](){return rc[b++]->arr()[4][11];})
//    .Define("arr_4_12",[&b,rc](){return rc[b++]->arr()[4][12];})
//    .Define("arr_4_13",[&b,rc](){return rc[b++]->arr()[4][13];})
//    .Define("arr_4_14",[&b,rc](){return rc[b++]->arr()[4][14];})
//    .Define("arr_4_15",[&b,rc](){return rc[b++]->arr()[4][15];})
//    .Define("arr_5_0",[&b,rc](){return rc[b++]->arr()[5][0];})
//    .Define("arr_5_1",[&b,rc](){return rc[b++]->arr()[5][1];})
//    .Define("arr_5_2",[&b,rc](){return rc[b++]->arr()[5][2];})
//    .Define("arr_5_3",[&b,rc](){return rc[b++]->arr()[5][3];})
//    .Define("arr_5_4",[&b,rc](){return rc[b++]->arr()[5][4];})
//    .Define("arr_5_5",[&b,rc](){return rc[b++]->arr()[5][5];})
//    .Define("arr_5_6",[&b,rc](){return rc[b++]->arr()[5][6];})
//    .Define("arr_5_7",[&b,rc](){return rc[b++]->arr()[5][7];})
//    .Define("arr_5_8",[&b,rc](){return rc[b++]->arr()[5][8];})
//    .Define("arr_5_9",[&b,rc](){return rc[b++]->arr()[5][9];})
//    .Define("arr_5_10",[&b,rc](){return rc[b++]->arr()[5][10];})
//    .Define("arr_5_11",[&b,rc](){return rc[b++]->arr()[5][11];})
//    .Define("arr_5_12",[&b,rc](){return rc[b++]->arr()[5][12];})
//    .Define("arr_5_13",[&b,rc](){return rc[b++]->arr()[5][13];})
//    .Define("arr_5_14",[&b,rc](){return rc[b++]->arr()[5][14];})
//    .Define("arr_5_15",[&b,rc](){return rc[b++]->arr()[5][15];})
//    .Define("arr_6_0",[&b,rc](){return rc[b++]->arr()[6][0];})
//    .Define("arr_6_1",[&b,rc](){return rc[b++]->arr()[6][1];})
//    .Define("arr_6_2",[&b,rc](){return rc[b++]->arr()[6][2];})
//    .Define("arr_6_3",[&b,rc](){return rc[b++]->arr()[6][3];})
//    .Define("arr_6_4",[&b,rc](){return rc[b++]->arr()[6][4];})
//    .Define("arr_6_5",[&b,rc](){return rc[b++]->arr()[6][5];})
//    .Define("arr_6_6",[&b,rc](){return rc[b++]->arr()[6][6];})
//    .Define("arr_6_7",[&b,rc](){return rc[b++]->arr()[6][7];})
//    .Define("arr_6_8",[&b,rc](){return rc[b++]->arr()[6][8];})
//    .Define("arr_6_9",[&b,rc](){return rc[b++]->arr()[6][9];})
//    .Define("arr_6_10",[&b,rc](){return rc[b++]->arr()[6][10];})
//    .Define("arr_6_11",[&b,rc](){return rc[b++]->arr()[6][11];})
//    .Define("arr_6_12",[&b,rc](){return rc[b++]->arr()[6][12];})
//    .Define("arr_6_13",[&b,rc](){return rc[b++]->arr()[6][13];})
//    .Define("arr_6_14",[&b,rc](){return rc[b++]->arr()[6][14];})
//    .Define("arr_6_15",[&b,rc](){return rc[b++]->arr()[6][15];})
//    .Define("arr_7_0",[&b,rc](){return rc[b++]->arr()[7][0];})
//    .Define("arr_7_1",[&b,rc](){return rc[b++]->arr()[7][1];})
//    .Define("arr_7_2",[&b,rc](){return rc[b++]->arr()[7][2];})
//    .Define("arr_7_3",[&b,rc](){return rc[b++]->arr()[7][3];})
//    .Define("arr_7_4",[&b,rc](){return rc[b++]->arr()[7][4];})
//    .Define("arr_7_5",[&b,rc](){return rc[b++]->arr()[7][5];})
//    .Define("arr_7_6",[&b,rc](){return rc[b++]->arr()[7][6];})
//    .Define("arr_7_7",[&b,rc](){return rc[b++]->arr()[7][7];})
//    .Define("arr_7_8",[&b,rc](){return rc[b++]->arr()[7][8];})
//    .Define("arr_7_9",[&b,rc](){return rc[b++]->arr()[7][9];})
//    .Define("arr_7_10",[&b,rc](){return rc[b++]->arr()[7][10];})
//    .Define("arr_7_11",[&b,rc](){return rc[b++]->arr()[7][11];})
//    .Define("arr_7_12",[&b,rc](){return rc[b++]->arr()[7][12];})
//    .Define("arr_7_13",[&b,rc](){return rc[b++]->arr()[7][13];})
//    .Define("arr_7_14",[&b,rc](){return rc[b++]->arr()[7][14];})
//    .Define("arr_7_15",[&b,rc](){return rc[b++]->arr()[7][15];})
//    .Define("arr_8_0",[&b,rc](){return rc[b++]->arr()[8][0];})
//    .Define("arr_8_1",[&b,rc](){return rc[b++]->arr()[8][1];})
//    .Define("arr_8_2",[&b,rc](){return rc[b++]->arr()[8][2];})
//    .Define("arr_8_3",[&b,rc](){return rc[b++]->arr()[8][3];})
//    .Define("arr_8_4",[&b,rc](){return rc[b++]->arr()[8][4];})
//    .Define("arr_8_5",[&b,rc](){return rc[b++]->arr()[8][5];})
//    .Define("arr_8_6",[&b,rc](){return rc[b++]->arr()[8][6];})
//    .Define("arr_8_7",[&b,rc](){return rc[b++]->arr()[8][7];})
//    .Define("arr_8_8",[&b,rc](){return rc[b++]->arr()[8][8];})
//    .Define("arr_8_9",[&b,rc](){return rc[b++]->arr()[8][9];})
//    .Define("arr_8_10",[&b,rc](){return rc[b++]->arr()[8][10];})
//    .Define("arr_8_11",[&b,rc](){return rc[b++]->arr()[8][11];})
//    .Define("arr_8_12",[&b,rc](){return rc[b++]->arr()[8][12];})
//    .Define("arr_8_13",[&b,rc](){return rc[b++]->arr()[8][13];})
//    .Define("arr_8_14",[&b,rc](){return rc[b++]->arr()[8][14];})
//    .Define("arr_8_15",[&b,rc](){return rc[b++]->arr()[8][15];})
//    .Define("arr_9_0",[&b,rc](){return rc[b++]->arr()[9][0];})
//    .Define("arr_9_1",[&b,rc](){return rc[b++]->arr()[9][1];})
//    .Define("arr_9_2",[&b,rc](){return rc[b++]->arr()[9][2];})
//    .Define("arr_9_3",[&b,rc](){return rc[b++]->arr()[9][3];})
//    .Define("arr_9_4",[&b,rc](){return rc[b++]->arr()[9][4];})
//    .Define("arr_9_5",[&b,rc](){return rc[b++]->arr()[9][5];})
//    .Define("arr_9_6",[&b,rc](){return rc[b++]->arr()[9][6];})
//    .Define("arr_9_7",[&b,rc](){return rc[b++]->arr()[9][7];})
//    .Define("arr_9_8",[&b,rc](){return rc[b++]->arr()[9][8];})
//    .Define("arr_9_9",[&b,rc](){return rc[b++]->arr()[9][9];})
//    .Define("arr_9_10",[&b,rc](){return rc[b++]->arr()[9][10];})
//    .Define("arr_9_11",[&b,rc](){return rc[b++]->arr()[9][11];})
//    .Define("arr_9_12",[&b,rc](){return rc[b++]->arr()[9][12];})
//    .Define("arr_9_13",[&b,rc](){return rc[b++]->arr()[9][13];})
//    .Define("arr_9_14",[&b,rc](){return rc[b++]->arr()[9][14];})
//    .Define("arr_9_15",[&b,rc](){return rc[b++]->arr()[9][15];})
//    .Define("arr_10_0",[&b,rc](){return rc[b++]->arr()[10][0];})
//    .Define("arr_10_1",[&b,rc](){return rc[b++]->arr()[10][1];})
//    .Define("arr_10_2",[&b,rc](){return rc[b++]->arr()[10][2];})
//    .Define("arr_10_3",[&b,rc](){return rc[b++]->arr()[10][3];})
//    .Define("arr_10_4",[&b,rc](){return rc[b++]->arr()[10][4];})
//    .Define("arr_10_5",[&b,rc](){return rc[b++]->arr()[10][5];})
//    .Define("arr_10_6",[&b,rc](){return rc[b++]->arr()[10][6];})
//    .Define("arr_10_7",[&b,rc](){return rc[b++]->arr()[10][7];})
//    .Define("arr_10_8",[&b,rc](){return rc[b++]->arr()[10][8];})
//    .Define("arr_10_9",[&b,rc](){return rc[b++]->arr()[10][9];})
//    .Define("arr_10_10",[&b,rc](){return rc[b++]->arr()[10][10];})
//    .Define("arr_10_11",[&b,rc](){return rc[b++]->arr()[10][11];})
//    .Define("arr_10_12",[&b,rc](){return rc[b++]->arr()[10][12];})
//    .Define("arr_10_13",[&b,rc](){return rc[b++]->arr()[10][13];})
//    .Define("arr_10_14",[&b,rc](){return rc[b++]->arr()[10][14];})
//    .Define("arr_10_15",[&b,rc](){return rc[b++]->arr()[10][15];})
//    .Define("arr_11_0",[&b,rc](){return rc[b++]->arr()[11][0];})
//    .Define("arr_11_1",[&b,rc](){return rc[b++]->arr()[11][1];})
//    .Define("arr_11_2",[&b,rc](){return rc[b++]->arr()[11][2];})
//    .Define("arr_11_3",[&b,rc](){return rc[b++]->arr()[11][3];})
//    .Define("arr_11_4",[&b,rc](){return rc[b++]->arr()[11][4];})
//    .Define("arr_11_5",[&b,rc](){return rc[b++]->arr()[11][5];})
//    .Define("arr_11_6",[&b,rc](){return rc[b++]->arr()[11][6];})
//    .Define("arr_11_7",[&b,rc](){return rc[b++]->arr()[11][7];})
//    .Define("arr_11_8",[&b,rc](){return rc[b++]->arr()[11][8];})
//    .Define("arr_11_9",[&b,rc](){return rc[b++]->arr()[11][9];})
//    .Define("arr_11_10",[&b,rc](){return rc[b++]->arr()[11][10];})
//    .Define("arr_11_11",[&b,rc](){return rc[b++]->arr()[11][11];})
//    .Define("arr_11_12",[&b,rc](){return rc[b++]->arr()[11][12];})
//    .Define("arr_11_13",[&b,rc](){return rc[b++]->arr()[11][13];})
//    .Define("arr_11_14",[&b,rc](){return rc[b++]->arr()[11][14];})
//    .Define("arr_11_15",[&b,rc](){return rc[b++]->arr()[11][15];})
//    .Define("arr_12_0",[&b,rc](){return rc[b++]->arr()[12][0];})
//    .Define("arr_12_1",[&b,rc](){return rc[b++]->arr()[12][1];})
//    .Define("arr_12_2",[&b,rc](){return rc[b++]->arr()[12][2];})
//    .Define("arr_12_3",[&b,rc](){return rc[b++]->arr()[12][3];})
//    .Define("arr_12_4",[&b,rc](){return rc[b++]->arr()[12][4];})
//    .Define("arr_12_5",[&b,rc](){return rc[b++]->arr()[12][5];})
//    .Define("arr_12_6",[&b,rc](){return rc[b++]->arr()[12][6];})
//    .Define("arr_12_7",[&b,rc](){return rc[b++]->arr()[12][7];})
//    .Define("arr_12_8",[&b,rc](){return rc[b++]->arr()[12][8];})
//    .Define("arr_12_9",[&b,rc](){return rc[b++]->arr()[12][9];})
//    .Define("arr_12_10",[&b,rc](){return rc[b++]->arr()[12][10];})
//    .Define("arr_12_11",[&b,rc](){return rc[b++]->arr()[12][11];})
//    .Define("arr_12_12",[&b,rc](){return rc[b++]->arr()[12][12];})
//    .Define("arr_12_13",[&b,rc](){return rc[b++]->arr()[12][13];})
//    .Define("arr_12_14",[&b,rc](){return rc[b++]->arr()[12][14];})
//    .Define("arr_12_15",[&b,rc](){return rc[b++]->arr()[12][15];})
//    .Define("arr_13_0",[&b,rc](){return rc[b++]->arr()[13][0];})
//    .Define("arr_13_1",[&b,rc](){return rc[b++]->arr()[13][1];})
//    .Define("arr_13_2",[&b,rc](){return rc[b++]->arr()[13][2];})
//    .Define("arr_13_3",[&b,rc](){return rc[b++]->arr()[13][3];})
//    .Define("arr_13_4",[&b,rc](){return rc[b++]->arr()[13][4];})
//    .Define("arr_13_5",[&b,rc](){return rc[b++]->arr()[13][5];})
//    .Define("arr_13_6",[&b,rc](){return rc[b++]->arr()[13][6];})
//    .Define("arr_13_7",[&b,rc](){return rc[b++]->arr()[13][7];})
//    .Define("arr_13_8",[&b,rc](){return rc[b++]->arr()[13][8];})
//    .Define("arr_13_9",[&b,rc](){return rc[b++]->arr()[13][9];})
//    .Define("arr_13_10",[&b,rc](){return rc[b++]->arr()[13][10];})
//    .Define("arr_13_11",[&b,rc](){return rc[b++]->arr()[13][11];})
//    .Define("arr_13_12",[&b,rc](){return rc[b++]->arr()[13][12];})
//    .Define("arr_13_13",[&b,rc](){return rc[b++]->arr()[13][13];})
//    .Define("arr_13_14",[&b,rc](){return rc[b++]->arr()[13][14];})
//    .Define("arr_13_15",[&b,rc](){return rc[b++]->arr()[13][15];})
//    .Snapshot("cal_gain","Cal_gain.root");
//  d = d->Define("arr_13_15",[&b,rc](){return rc[b++]->arr()[13][15];});
  
//classic way
  TFile f("Cal_gain.root","NEW");
  TTree t("T","T");
  double pos_0;
  double neg_0;
  double pos_1;
  double neg_1;
  double pos_2;
  double neg_2;
  double pos_3;
  double neg_3;
  double pos_4;
  double neg_4;
  double pos_5;
  double neg_5;
  double pos_6;
  double neg_6;
  double pos_7;
  double neg_7;
  double pos_8;
  double neg_8;
  double pos_9;
  double neg_9;
  double pos_10;
  double neg_10;
  double pos_11;
  double neg_11;
  double pos_12;
  double neg_12;
  double pos_13;
  double neg_13;
  double arr_0_0;
  double arr_0_1;
  double arr_0_2;
  double arr_0_3;
  double arr_0_4;
  double arr_0_5;
  double arr_0_6;
  double arr_0_7;
  double arr_0_8;
  double arr_0_9;
  double arr_0_10;
  double arr_0_11;
  double arr_0_12;
  double arr_0_13;
  double arr_0_14;
  double arr_0_15;
  double arr_1_0;
  double arr_1_1;
  double arr_1_2;
  double arr_1_3;
  double arr_1_4;
  double arr_1_5;
  double arr_1_6;
  double arr_1_7;
  double arr_1_8;
  double arr_1_9;
  double arr_1_10;
  double arr_1_11;
  double arr_1_12;
  double arr_1_13;
  double arr_1_14;
  double arr_1_15;
  double arr_2_0;
  double arr_2_1;
  double arr_2_2;
  double arr_2_3;
  double arr_2_4;
  double arr_2_5;
  double arr_2_6;
  double arr_2_7;
  double arr_2_8;
  double arr_2_9;
  double arr_2_10;
  double arr_2_11;
  double arr_2_12;
  double arr_2_13;
  double arr_2_14;
  double arr_2_15;
  double arr_3_0;
  double arr_3_1;
  double arr_3_2;
  double arr_3_3;
  double arr_3_4;
  double arr_3_5;
  double arr_3_6;
  double arr_3_7;
  double arr_3_8;
  double arr_3_9;
  double arr_3_10;
  double arr_3_11;
  double arr_3_12;
  double arr_3_13;
  double arr_3_14;
  double arr_3_15;
  double arr_4_0;
  double arr_4_1;
  double arr_4_2;
  double arr_4_3;
  double arr_4_4;
  double arr_4_5;
  double arr_4_6;
  double arr_4_7;
  double arr_4_8;
  double arr_4_9;
  double arr_4_10;
  double arr_4_11;
  double arr_4_12;
  double arr_4_13;
  double arr_4_14;
  double arr_4_15;
  double arr_5_0;
  double arr_5_1;
  double arr_5_2;
  double arr_5_3;
  double arr_5_4;
  double arr_5_5;
  double arr_5_6;
  double arr_5_7;
  double arr_5_8;
  double arr_5_9;
  double arr_5_10;
  double arr_5_11;
  double arr_5_12;
  double arr_5_13;
  double arr_5_14;
  double arr_5_15;
  double arr_6_0;
  double arr_6_1;
  double arr_6_2;
  double arr_6_3;
  double arr_6_4;
  double arr_6_5;
  double arr_6_6;
  double arr_6_7;
  double arr_6_8;
  double arr_6_9;
  double arr_6_10;
  double arr_6_11;
  double arr_6_12;
  double arr_6_13;
  double arr_6_14;
  double arr_6_15;
  double arr_7_0;
  double arr_7_1;
  double arr_7_2;
  double arr_7_3;
  double arr_7_4;
  double arr_7_5;
  double arr_7_6;
  double arr_7_7;
  double arr_7_8;
  double arr_7_9;
  double arr_7_10;
  double arr_7_11;
  double arr_7_12;
  double arr_7_13;
  double arr_7_14;
  double arr_7_15;
  double arr_8_0;
  double arr_8_1;
  double arr_8_2;
  double arr_8_3;
  double arr_8_4;
  double arr_8_5;
  double arr_8_6;
  double arr_8_7;
  double arr_8_8;
  double arr_8_9;
  double arr_8_10;
  double arr_8_11;
  double arr_8_12;
  double arr_8_13;
  double arr_8_14;
  double arr_8_15;
  double arr_9_0;
  double arr_9_1;
  double arr_9_2;
  double arr_9_3;
  double arr_9_4;
  double arr_9_5;
  double arr_9_6;
  double arr_9_7;
  double arr_9_8;
  double arr_9_9;
  double arr_9_10;
  double arr_9_11;
  double arr_9_12;
  double arr_9_13;
  double arr_9_14;
  double arr_9_15;
  double arr_10_0;
  double arr_10_1;
  double arr_10_2;
  double arr_10_3;
  double arr_10_4;
  double arr_10_5;
  double arr_10_6;
  double arr_10_7;
  double arr_10_8;
  double arr_10_9;
  double arr_10_10;
  double arr_10_11;
  double arr_10_12;
  double arr_10_13;
  double arr_10_14;
  double arr_10_15;
  double arr_11_0;
  double arr_11_1;
  double arr_11_2;
  double arr_11_3;
  double arr_11_4;
  double arr_11_5;
  double arr_11_6;
  double arr_11_7;
  double arr_11_8;
  double arr_11_9;
  double arr_11_10;
  double arr_11_11;
  double arr_11_12;
  double arr_11_13;
  double arr_11_14;
  double arr_11_15;
  double arr_12_0;
  double arr_12_1;
  double arr_12_2;
  double arr_12_3;
  double arr_12_4;
  double arr_12_5;
  double arr_12_6;
  double arr_12_7;
  double arr_12_8;
  double arr_12_9;
  double arr_12_10;
  double arr_12_11;
  double arr_12_12;
  double arr_12_13;
  double arr_12_14;
  double arr_12_15;
  double arr_13_0;
  double arr_13_1;
  double arr_13_2;
  double arr_13_3;
  double arr_13_4;
  double arr_13_5;
  double arr_13_6;
  double arr_13_7;
  double arr_13_8;
  double arr_13_9;
  double arr_13_10;
  double arr_13_11;
  double arr_13_12;
  double arr_13_13;
  double arr_13_14;
  double arr_13_15;

  t.Branch("pos_0",&pos_0);
  t.Branch("neg_0",&neg_0);
  t.Branch("pos_1",&pos_1);
  t.Branch("neg_1",&neg_1);
  t.Branch("pos_2",&pos_2);
  t.Branch("neg_2",&neg_2);
  t.Branch("pos_3",&pos_3);
  t.Branch("neg_3",&neg_3);
  t.Branch("pos_4",&pos_4);
  t.Branch("neg_4",&neg_4);
  t.Branch("pos_5",&pos_5);
  t.Branch("neg_5",&neg_5);
  t.Branch("pos_6",&pos_6);
  t.Branch("neg_6",&neg_6);
  t.Branch("pos_7",&pos_7);
  t.Branch("neg_7",&neg_7);
  t.Branch("pos_8",&pos_8);
  t.Branch("neg_8",&neg_8);
  t.Branch("pos_9",&pos_9);
  t.Branch("neg_9",&neg_9);
  t.Branch("pos_10",&pos_10);
  t.Branch("neg_10",&neg_10);
  t.Branch("pos_11",&pos_11);
  t.Branch("neg_11",&neg_11);
  t.Branch("pos_12",&pos_12);
  t.Branch("neg_12",&neg_12);
  t.Branch("pos_13",&pos_13);
  t.Branch("neg_13",&neg_13);
  t.Branch("array_0_0",&arr_0_0);
  t.Branch("array_0_1",&arr_0_1);
  t.Branch("array_0_2",&arr_0_2);
  t.Branch("array_0_3",&arr_0_3);
  t.Branch("array_0_4",&arr_0_4);
  t.Branch("array_0_5",&arr_0_5);
  t.Branch("array_0_6",&arr_0_6);
  t.Branch("array_0_7",&arr_0_7);
  t.Branch("array_0_8",&arr_0_8);
  t.Branch("array_0_9",&arr_0_9);
  t.Branch("array_0_10",&arr_0_10);
  t.Branch("array_0_11",&arr_0_11);
  t.Branch("array_0_12",&arr_0_12);
  t.Branch("array_0_13",&arr_0_13);
  t.Branch("array_0_14",&arr_0_14);
  t.Branch("array_0_15",&arr_0_15);
  t.Branch("array_1_0",&arr_1_0);
  t.Branch("array_1_1",&arr_1_1);
  t.Branch("array_1_2",&arr_1_2);
  t.Branch("array_1_3",&arr_1_3);
  t.Branch("array_1_4",&arr_1_4);
  t.Branch("array_1_5",&arr_1_5);
  t.Branch("array_1_6",&arr_1_6);
  t.Branch("array_1_7",&arr_1_7);
  t.Branch("array_1_8",&arr_1_8);
  t.Branch("array_1_9",&arr_1_9);
  t.Branch("array_1_10",&arr_1_10);
  t.Branch("array_1_11",&arr_1_11);
  t.Branch("array_1_12",&arr_1_12);
  t.Branch("array_1_13",&arr_1_13);
  t.Branch("array_1_14",&arr_1_14);
  t.Branch("array_1_15",&arr_1_15);
  t.Branch("array_2_0",&arr_2_0);
  t.Branch("array_2_1",&arr_2_1);
  t.Branch("array_2_2",&arr_2_2);
  t.Branch("array_2_3",&arr_2_3);
  t.Branch("array_2_4",&arr_2_4);
  t.Branch("array_2_5",&arr_2_5);
  t.Branch("array_2_6",&arr_2_6);
  t.Branch("array_2_7",&arr_2_7);
  t.Branch("array_2_8",&arr_2_8);
  t.Branch("array_2_9",&arr_2_9);
  t.Branch("array_2_10",&arr_2_10);
  t.Branch("array_2_11",&arr_2_11);
  t.Branch("array_2_12",&arr_2_12);
  t.Branch("array_2_13",&arr_2_13);
  t.Branch("array_2_14",&arr_2_14);
  t.Branch("array_2_15",&arr_2_15);
  t.Branch("array_3_0",&arr_3_0);
  t.Branch("array_3_1",&arr_3_1);
  t.Branch("array_3_2",&arr_3_2);
  t.Branch("array_3_3",&arr_3_3);
  t.Branch("array_3_4",&arr_3_4);
  t.Branch("array_3_5",&arr_3_5);
  t.Branch("array_3_6",&arr_3_6);
  t.Branch("array_3_7",&arr_3_7);
  t.Branch("array_3_8",&arr_3_8);
  t.Branch("array_3_9",&arr_3_9);
  t.Branch("array_3_10",&arr_3_10);
  t.Branch("array_3_11",&arr_3_11);
  t.Branch("array_3_12",&arr_3_12);
  t.Branch("array_3_13",&arr_3_13);
  t.Branch("array_3_14",&arr_3_14);
  t.Branch("array_3_15",&arr_3_15);
  t.Branch("array_4_0",&arr_4_0);
  t.Branch("array_4_1",&arr_4_1);
  t.Branch("array_4_2",&arr_4_2);
  t.Branch("array_4_3",&arr_4_3);
  t.Branch("array_4_4",&arr_4_4);
  t.Branch("array_4_5",&arr_4_5);
  t.Branch("array_4_6",&arr_4_6);
  t.Branch("array_4_7",&arr_4_7);
  t.Branch("array_4_8",&arr_4_8);
  t.Branch("array_4_9",&arr_4_9);
  t.Branch("array_4_10",&arr_4_10);
  t.Branch("array_4_11",&arr_4_11);
  t.Branch("array_4_12",&arr_4_12);
  t.Branch("array_4_13",&arr_4_13);
  t.Branch("array_4_14",&arr_4_14);
  t.Branch("array_4_15",&arr_4_15);
  t.Branch("array_5_0",&arr_5_0);
  t.Branch("array_5_1",&arr_5_1);
  t.Branch("array_5_2",&arr_5_2);
  t.Branch("array_5_3",&arr_5_3);
  t.Branch("array_5_4",&arr_5_4);
  t.Branch("array_5_5",&arr_5_5);
  t.Branch("array_5_6",&arr_5_6);
  t.Branch("array_5_7",&arr_5_7);
  t.Branch("array_5_8",&arr_5_8);
  t.Branch("array_5_9",&arr_5_9);
  t.Branch("array_5_10",&arr_5_10);
  t.Branch("array_5_11",&arr_5_11);
  t.Branch("array_5_12",&arr_5_12);
  t.Branch("array_5_13",&arr_5_13);
  t.Branch("array_5_14",&arr_5_14);
  t.Branch("array_5_15",&arr_5_15);
  t.Branch("array_6_0",&arr_6_0);
  t.Branch("array_6_1",&arr_6_1);
  t.Branch("array_6_2",&arr_6_2);
  t.Branch("array_6_3",&arr_6_3);
  t.Branch("array_6_4",&arr_6_4);
  t.Branch("array_6_5",&arr_6_5);
  t.Branch("array_6_6",&arr_6_6);
  t.Branch("array_6_7",&arr_6_7);
  t.Branch("array_6_8",&arr_6_8);
  t.Branch("array_6_9",&arr_6_9);
  t.Branch("array_6_10",&arr_6_10);
  t.Branch("array_6_11",&arr_6_11);
  t.Branch("array_6_12",&arr_6_12);
  t.Branch("array_6_13",&arr_6_13);
  t.Branch("array_6_14",&arr_6_14);
  t.Branch("array_6_15",&arr_6_15);
  t.Branch("array_7_0",&arr_7_0);
  t.Branch("array_7_1",&arr_7_1);
  t.Branch("array_7_2",&arr_7_2);
  t.Branch("array_7_3",&arr_7_3);
  t.Branch("array_7_4",&arr_7_4);
  t.Branch("array_7_5",&arr_7_5);
  t.Branch("array_7_6",&arr_7_6);
  t.Branch("array_7_7",&arr_7_7);
  t.Branch("array_7_8",&arr_7_8);
  t.Branch("array_7_9",&arr_7_9);
  t.Branch("array_7_10",&arr_7_10);
  t.Branch("array_7_11",&arr_7_11);
  t.Branch("array_7_12",&arr_7_12);
  t.Branch("array_7_13",&arr_7_13);
  t.Branch("array_7_14",&arr_7_14);
  t.Branch("array_7_15",&arr_7_15);
  t.Branch("array_8_0",&arr_8_0);
  t.Branch("array_8_1",&arr_8_1);
  t.Branch("array_8_2",&arr_8_2);
  t.Branch("array_8_3",&arr_8_3);
  t.Branch("array_8_4",&arr_8_4);
  t.Branch("array_8_5",&arr_8_5);
  t.Branch("array_8_6",&arr_8_6);
  t.Branch("array_8_7",&arr_8_7);
  t.Branch("array_8_8",&arr_8_8);
  t.Branch("array_8_9",&arr_8_9);
  t.Branch("array_8_10",&arr_8_10);
  t.Branch("array_8_11",&arr_8_11);
  t.Branch("array_8_12",&arr_8_12);
  t.Branch("array_8_13",&arr_8_13);
  t.Branch("array_8_14",&arr_8_14);
  t.Branch("array_8_15",&arr_8_15);
  t.Branch("array_9_0",&arr_9_0);
  t.Branch("array_9_1",&arr_9_1);
  t.Branch("array_9_2",&arr_9_2);
  t.Branch("array_9_3",&arr_9_3);
  t.Branch("array_9_4",&arr_9_4);
  t.Branch("array_9_5",&arr_9_5);
  t.Branch("array_9_6",&arr_9_6);
  t.Branch("array_9_7",&arr_9_7);
  t.Branch("array_9_8",&arr_9_8);
  t.Branch("array_9_9",&arr_9_9);
  t.Branch("array_9_10",&arr_9_10);
  t.Branch("array_9_11",&arr_9_11);
  t.Branch("array_9_12",&arr_9_12);
  t.Branch("array_9_13",&arr_9_13);
  t.Branch("array_9_14",&arr_9_14);
  t.Branch("array_9_15",&arr_9_15);
  t.Branch("array_10_0",&arr_10_0);
  t.Branch("array_10_1",&arr_10_1);
  t.Branch("array_10_2",&arr_10_2);
  t.Branch("array_10_3",&arr_10_3);
  t.Branch("array_10_4",&arr_10_4);
  t.Branch("array_10_5",&arr_10_5);
  t.Branch("array_10_6",&arr_10_6);
  t.Branch("array_10_7",&arr_10_7);
  t.Branch("array_10_8",&arr_10_8);
  t.Branch("array_10_9",&arr_10_9);
  t.Branch("array_10_10",&arr_10_10);
  t.Branch("array_10_11",&arr_10_11);
  t.Branch("array_10_12",&arr_10_12);
  t.Branch("array_10_13",&arr_10_13);
  t.Branch("array_10_14",&arr_10_14);
  t.Branch("array_10_15",&arr_10_15);
  t.Branch("array_11_0",&arr_11_0);
  t.Branch("array_11_1",&arr_11_1);
  t.Branch("array_11_2",&arr_11_2);
  t.Branch("array_11_3",&arr_11_3);
  t.Branch("array_11_4",&arr_11_4);
  t.Branch("array_11_5",&arr_11_5);
  t.Branch("array_11_6",&arr_11_6);
  t.Branch("array_11_7",&arr_11_7);
  t.Branch("array_11_8",&arr_11_8);
  t.Branch("array_11_9",&arr_11_9);
  t.Branch("array_11_10",&arr_11_10);
  t.Branch("array_11_11",&arr_11_11);
  t.Branch("array_11_12",&arr_11_12);
  t.Branch("array_11_13",&arr_11_13);
  t.Branch("array_11_14",&arr_11_14);
  t.Branch("array_11_15",&arr_11_15);
  t.Branch("array_12_0",&arr_12_0);
  t.Branch("array_12_1",&arr_12_1);
  t.Branch("array_12_2",&arr_12_2);
  t.Branch("array_12_3",&arr_12_3);
  t.Branch("array_12_4",&arr_12_4);
  t.Branch("array_12_5",&arr_12_5);
  t.Branch("array_12_6",&arr_12_6);
  t.Branch("array_12_7",&arr_12_7);
  t.Branch("array_12_8",&arr_12_8);
  t.Branch("array_12_9",&arr_12_9);
  t.Branch("array_12_10",&arr_12_10);
  t.Branch("array_12_11",&arr_12_11);
  t.Branch("array_12_12",&arr_12_12);
  t.Branch("array_12_13",&arr_12_13);
  t.Branch("array_12_14",&arr_12_14);
  t.Branch("array_12_15",&arr_12_15);
  t.Branch("array_13_0",&arr_13_0);
  t.Branch("array_13_1",&arr_13_1);
  t.Branch("array_13_2",&arr_13_2);
  t.Branch("array_13_3",&arr_13_3);
  t.Branch("array_13_4",&arr_13_4);
  t.Branch("array_13_5",&arr_13_5);
  t.Branch("array_13_6",&arr_13_6);
  t.Branch("array_13_7",&arr_13_7);
  t.Branch("array_13_8",&arr_13_8);
  t.Branch("array_13_9",&arr_13_9);
  t.Branch("array_13_10",&arr_13_10);
  t.Branch("array_13_11",&arr_13_11);
  t.Branch("array_13_12",&arr_13_12);
  t.Branch("array_13_13",&arr_13_13);
  t.Branch("array_13_14",&arr_13_14);
  t.Branch("array_13_15",&arr_13_15);
  for(int i = 0; i<15;i++){

    pos_0 = rc[i]->pos()[0];
      neg_0 = rc[i]->neg()[0];
      pos_1 = rc[i]->pos()[1];
      neg_1 = rc[i]->neg()[1];
      pos_2 = rc[i]->pos()[2];
      neg_2 = rc[i]->neg()[2];
      pos_3 = rc[i]->pos()[3];
      neg_3 = rc[i]->neg()[3];
      pos_4 = rc[i]->pos()[4];
      neg_4 = rc[i]->neg()[4];
      pos_5 = rc[i]->pos()[5];
      neg_5 = rc[i]->neg()[5];
      pos_6 = rc[i]->pos()[6];
      neg_6 = rc[i]->neg()[6];
      pos_7 = rc[i]->pos()[7];
      neg_7 = rc[i]->neg()[7];
      pos_8 = rc[i]->pos()[8];
      neg_8 = rc[i]->neg()[8];
      pos_9 = rc[i]->pos()[9];
      neg_9 = rc[i]->neg()[9];
      pos_10 = rc[i]->pos()[10];
      neg_10 = rc[i]->neg()[10];
      pos_11 = rc[i]->pos()[11];
      neg_11 = rc[i]->neg()[11];
      pos_12 = rc[i]->pos()[12];
      neg_12 = rc[i]->neg()[12];
      pos_13 = rc[i]->pos()[13];
      neg_13 = rc[i]->neg()[13];
      arr_0_0 = rc[i]->arr()[0][0];
    arr_0_1 = rc[i]->arr()[0][1];
    arr_0_2 = rc[i]->arr()[0][2];
    arr_0_3 = rc[i]->arr()[0][3];
    arr_0_4 = rc[i]->arr()[0][4];
    arr_0_5 = rc[i]->arr()[0][5];
    arr_0_6 = rc[i]->arr()[0][6];
    arr_0_7 = rc[i]->arr()[0][7];
    arr_0_8 = rc[i]->arr()[0][8];
    arr_0_9 = rc[i]->arr()[0][9];
    arr_0_10 = rc[i]->arr()[0][10];
    arr_0_11 = rc[i]->arr()[0][11];
    arr_0_12 = rc[i]->arr()[0][12];
    arr_0_13 = rc[i]->arr()[0][13];
    arr_0_14 = rc[i]->arr()[0][14];
    arr_0_15 = rc[i]->arr()[0][15];
    arr_1_0 = rc[i]->arr()[1][0];
    arr_1_1 = rc[i]->arr()[1][1];
    arr_1_2 = rc[i]->arr()[1][2];
    arr_1_3 = rc[i]->arr()[1][3];
    arr_1_4 = rc[i]->arr()[1][4];
    arr_1_5 = rc[i]->arr()[1][5];
    arr_1_6 = rc[i]->arr()[1][6];
    arr_1_7 = rc[i]->arr()[1][7];
    arr_1_8 = rc[i]->arr()[1][8];
    arr_1_9 = rc[i]->arr()[1][9];
    arr_1_10 = rc[i]->arr()[1][10];
    arr_1_11 = rc[i]->arr()[1][11];
    arr_1_12 = rc[i]->arr()[1][12];
    arr_1_13 = rc[i]->arr()[1][13];
    arr_1_14 = rc[i]->arr()[1][14];
    arr_1_15 = rc[i]->arr()[1][15];
    arr_2_0 = rc[i]->arr()[2][0];
    arr_2_1 = rc[i]->arr()[2][1];
    arr_2_2 = rc[i]->arr()[2][2];
    arr_2_3 = rc[i]->arr()[2][3];
    arr_2_4 = rc[i]->arr()[2][4];
    arr_2_5 = rc[i]->arr()[2][5];
    arr_2_6 = rc[i]->arr()[2][6];
    arr_2_7 = rc[i]->arr()[2][7];
    arr_2_8 = rc[i]->arr()[2][8];
    arr_2_9 = rc[i]->arr()[2][9];
    arr_2_10 = rc[i]->arr()[2][10];
    arr_2_11 = rc[i]->arr()[2][11];
    arr_2_12 = rc[i]->arr()[2][12];
    arr_2_13 = rc[i]->arr()[2][13];
    arr_2_14 = rc[i]->arr()[2][14];
    arr_2_15 = rc[i]->arr()[2][15];
    arr_3_0 = rc[i]->arr()[3][0];
    arr_3_1 = rc[i]->arr()[3][1];
    arr_3_2 = rc[i]->arr()[3][2];
    arr_3_3 = rc[i]->arr()[3][3];
    arr_3_4 = rc[i]->arr()[3][4];
    arr_3_5 = rc[i]->arr()[3][5];
    arr_3_6 = rc[i]->arr()[3][6];
    arr_3_7 = rc[i]->arr()[3][7];
    arr_3_8 = rc[i]->arr()[3][8];
    arr_3_9 = rc[i]->arr()[3][9];
    arr_3_10 = rc[i]->arr()[3][10];
    arr_3_11 = rc[i]->arr()[3][11];
    arr_3_12 = rc[i]->arr()[3][12];
    arr_3_13 = rc[i]->arr()[3][13];
    arr_3_14 = rc[i]->arr()[3][14];
    arr_3_15 = rc[i]->arr()[3][15];
    arr_4_0 = rc[i]->arr()[4][0];
    arr_4_1 = rc[i]->arr()[4][1];
    arr_4_2 = rc[i]->arr()[4][2];
    arr_4_3 = rc[i]->arr()[4][3];
    arr_4_4 = rc[i]->arr()[4][4];
    arr_4_5 = rc[i]->arr()[4][5];
    arr_4_6 = rc[i]->arr()[4][6];
    arr_4_7 = rc[i]->arr()[4][7];
    arr_4_8 = rc[i]->arr()[4][8];
    arr_4_9 = rc[i]->arr()[4][9];
    arr_4_10 = rc[i]->arr()[4][10];
    arr_4_11 = rc[i]->arr()[4][11];
    arr_4_12 = rc[i]->arr()[4][12];
    arr_4_13 = rc[i]->arr()[4][13];
    arr_4_14 = rc[i]->arr()[4][14];
    arr_4_15 = rc[i]->arr()[4][15];
    arr_5_0 = rc[i]->arr()[5][0];
    arr_5_1 = rc[i]->arr()[5][1];
    arr_5_2 = rc[i]->arr()[5][2];
    arr_5_3 = rc[i]->arr()[5][3];
    arr_5_4 = rc[i]->arr()[5][4];
    arr_5_5 = rc[i]->arr()[5][5];
    arr_5_6 = rc[i]->arr()[5][6];
    arr_5_7 = rc[i]->arr()[5][7];
    arr_5_8 = rc[i]->arr()[5][8];
    arr_5_9 = rc[i]->arr()[5][9];
    arr_5_10 = rc[i]->arr()[5][10];
    arr_5_11 = rc[i]->arr()[5][11];
    arr_5_12 = rc[i]->arr()[5][12];
    arr_5_13 = rc[i]->arr()[5][13];
    arr_5_14 = rc[i]->arr()[5][14];
    arr_5_15 = rc[i]->arr()[5][15];
    arr_6_0 = rc[i]->arr()[6][0];
    arr_6_1 = rc[i]->arr()[6][1];
    arr_6_2 = rc[i]->arr()[6][2];
    arr_6_3 = rc[i]->arr()[6][3];
    arr_6_4 = rc[i]->arr()[6][4];
    arr_6_5 = rc[i]->arr()[6][5];
    arr_6_6 = rc[i]->arr()[6][6];
    arr_6_7 = rc[i]->arr()[6][7];
    arr_6_8 = rc[i]->arr()[6][8];
    arr_6_9 = rc[i]->arr()[6][9];
    arr_6_10 = rc[i]->arr()[6][10];
    arr_6_11 = rc[i]->arr()[6][11];
    arr_6_12 = rc[i]->arr()[6][12];
    arr_6_13 = rc[i]->arr()[6][13];
    arr_6_14 = rc[i]->arr()[6][14];
    arr_6_15 = rc[i]->arr()[6][15];
    arr_7_0 = rc[i]->arr()[7][0];
    arr_7_1 = rc[i]->arr()[7][1];
    arr_7_2 = rc[i]->arr()[7][2];
    arr_7_3 = rc[i]->arr()[7][3];
    arr_7_4 = rc[i]->arr()[7][4];
    arr_7_5 = rc[i]->arr()[7][5];
    arr_7_6 = rc[i]->arr()[7][6];
    arr_7_7 = rc[i]->arr()[7][7];
    arr_7_8 = rc[i]->arr()[7][8];
    arr_7_9 = rc[i]->arr()[7][9];
    arr_7_10 = rc[i]->arr()[7][10];
    arr_7_11 = rc[i]->arr()[7][11];
    arr_7_12 = rc[i]->arr()[7][12];
    arr_7_13 = rc[i]->arr()[7][13];
    arr_7_14 = rc[i]->arr()[7][14];
    arr_7_15 = rc[i]->arr()[7][15];
    arr_8_0 = rc[i]->arr()[8][0];
    arr_8_1 = rc[i]->arr()[8][1];
    arr_8_2 = rc[i]->arr()[8][2];
    arr_8_3 = rc[i]->arr()[8][3];
    arr_8_4 = rc[i]->arr()[8][4];
    arr_8_5 = rc[i]->arr()[8][5];
    arr_8_6 = rc[i]->arr()[8][6];
    arr_8_7 = rc[i]->arr()[8][7];
    arr_8_8 = rc[i]->arr()[8][8];
    arr_8_9 = rc[i]->arr()[8][9];
    arr_8_10 = rc[i]->arr()[8][10];
    arr_8_11 = rc[i]->arr()[8][11];
    arr_8_12 = rc[i]->arr()[8][12];
    arr_8_13 = rc[i]->arr()[8][13];
    arr_8_14 = rc[i]->arr()[8][14];
    arr_8_15 = rc[i]->arr()[8][15];
    arr_9_0 = rc[i]->arr()[9][0];
    arr_9_1 = rc[i]->arr()[9][1];
    arr_9_2 = rc[i]->arr()[9][2];
    arr_9_3 = rc[i]->arr()[9][3];
    arr_9_4 = rc[i]->arr()[9][4];
    arr_9_5 = rc[i]->arr()[9][5];
    arr_9_6 = rc[i]->arr()[9][6];
    arr_9_7 = rc[i]->arr()[9][7];
    arr_9_8 = rc[i]->arr()[9][8];
    arr_9_9 = rc[i]->arr()[9][9];
    arr_9_10 = rc[i]->arr()[9][10];
    arr_9_11 = rc[i]->arr()[9][11];
    arr_9_12 = rc[i]->arr()[9][12];
    arr_9_13 = rc[i]->arr()[9][13];
    arr_9_14 = rc[i]->arr()[9][14];
    arr_9_15 = rc[i]->arr()[9][15];
    arr_10_0 = rc[i]->arr()[10][0];
    arr_10_1 = rc[i]->arr()[10][1];
    arr_10_2 = rc[i]->arr()[10][2];
    arr_10_3 = rc[i]->arr()[10][3];
    arr_10_4 = rc[i]->arr()[10][4];
    arr_10_5 = rc[i]->arr()[10][5];
    arr_10_6 = rc[i]->arr()[10][6];
    arr_10_7 = rc[i]->arr()[10][7];
    arr_10_8 = rc[i]->arr()[10][8];
    arr_10_9 = rc[i]->arr()[10][9];
    arr_10_10 = rc[i]->arr()[10][10];
    arr_10_11 = rc[i]->arr()[10][11];
    arr_10_12 = rc[i]->arr()[10][12];
    arr_10_13 = rc[i]->arr()[10][13];
    arr_10_14 = rc[i]->arr()[10][14];
    arr_10_15 = rc[i]->arr()[10][15];
    arr_11_0 = rc[i]->arr()[11][0];
    arr_11_1 = rc[i]->arr()[11][1];
    arr_11_2 = rc[i]->arr()[11][2];
    arr_11_3 = rc[i]->arr()[11][3];
    arr_11_4 = rc[i]->arr()[11][4];
    arr_11_5 = rc[i]->arr()[11][5];
    arr_11_6 = rc[i]->arr()[11][6];
    arr_11_7 = rc[i]->arr()[11][7];
    arr_11_8 = rc[i]->arr()[11][8];
    arr_11_9 = rc[i]->arr()[11][9];
    arr_11_10 = rc[i]->arr()[11][10];
    arr_11_11 = rc[i]->arr()[11][11];
    arr_11_12 = rc[i]->arr()[11][12];
    arr_11_13 = rc[i]->arr()[11][13];
    arr_11_14 = rc[i]->arr()[11][14];
    arr_11_15 = rc[i]->arr()[11][15];
    arr_12_0 = rc[i]->arr()[12][0];
    arr_12_1 = rc[i]->arr()[12][1];
    arr_12_2 = rc[i]->arr()[12][2];
    arr_12_3 = rc[i]->arr()[12][3];
    arr_12_4 = rc[i]->arr()[12][4];
    arr_12_5 = rc[i]->arr()[12][5];
    arr_12_6 = rc[i]->arr()[12][6];
    arr_12_7 = rc[i]->arr()[12][7];
    arr_12_8 = rc[i]->arr()[12][8];
    arr_12_9 = rc[i]->arr()[12][9];
    arr_12_10 = rc[i]->arr()[12][10];
    arr_12_11 = rc[i]->arr()[12][11];
    arr_12_12 = rc[i]->arr()[12][12];
    arr_12_13 = rc[i]->arr()[12][13];
    arr_12_14 = rc[i]->arr()[12][14];
    arr_12_15 = rc[i]->arr()[12][15];
    arr_13_0 = rc[i]->arr()[13][0];
    arr_13_1 = rc[i]->arr()[13][1];
    arr_13_2 = rc[i]->arr()[13][2];
    arr_13_3 = rc[i]->arr()[13][3];
    arr_13_4 = rc[i]->arr()[13][4];
    arr_13_5 = rc[i]->arr()[13][5];
    arr_13_6 = rc[i]->arr()[13][6];
    arr_13_7 = rc[i]->arr()[13][7];
    arr_13_8 = rc[i]->arr()[13][8];
    arr_13_9 = rc[i]->arr()[13][9];
    arr_13_10 = rc[i]->arr()[13][10];
    arr_13_11 = rc[i]->arr()[13][11];
    arr_13_12 = rc[i]->arr()[13][12];
    arr_13_13 = rc[i]->arr()[13][13];
    arr_13_14 = rc[i]->arr()[13][14];
    arr_13_15 = rc[i]->arr()[13][15];
    t.Fill();
  }
  t.Write();
  f.Close();
  //auto t1 = f.Get<TTree>("T");
  //ROOT::RDataFrame d(t,f);  
  // std::cout<<"check1 "<<std::endl;
  // auto cut_zero = [](double gain){return gain>0;};
  //tdf.Filter(cut_zero,"pos_0");
  // auto pos0 = d.Histo1D({"pos_0"});
  // auto neg0 = d.Histo1D({"neg_0"});
  //auto pos0 = d.Fill<double>(TH1D("th1d","",100,0,100),{"pos"})
  // auto c1 = new TCanvas();
  //   c1->Divide(2,1);
  //   c1->cd(1);
  //   pos0->DrawCopy();
  //   c1->cd(2);
  //   neg0->DrawCopy();
}

