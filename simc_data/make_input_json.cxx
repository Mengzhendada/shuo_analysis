#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"

#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>

void make_input_json(){
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_info;
  {
    std::ifstream ifs("db2/rungroup_info.json");
    ifs>>j_info;
  }
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();it++){
    double RunGroup = std::stoi(it.key());
    auto runjs = it.value();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
    std::vector<double> charge_neg,charge_pos;
    if(!neg_D2.empty() && !pos_D2.empty()){
      for(auto ineg = neg_D2.begin();ineg!=neg_D2.end();ineg++){
        int runnumber = *ineg;
        std::string report_name = "report/replay_coin_production_"+std::to_string(runnumber)+"_-1.report";
        std::ifstream infile;
        infile.open(report_name.c_str());

        std::string word,line,temp;
        double n; 
        word = "HMS BCM4A Charge";
        int number=0;  
        int ii=0;
        while(std::getline(infile,line))
        { 
          if(line.find(word)<line.length())
          {
            std::cout<<line<<" "<<ii<<std::endl;
            ++ii;
            for(int i = word.size(); i<line[i];i++){
              if(isdigit(line[i]) ){
                for(int a = i; a<line.size();a++){
                  temp += line[a];
                }
                break;  
              }
            }
            std::cout<<"temp"<<temp<<std::endl;
            //temp.clear();
            std::istringstream stream(temp);
            stream>>n;
            std::cout<<"number "<<n<<std::endl;
            temp.clear();
            //ns.push_back(n);
          }
        }
        charge_neg.push_back(n);

      }//go through neg runs

      for(auto ipos = pos_D2.begin();ipos!=pos_D2.end();ipos++){
        int runnumber = *ipos;
        std::string report_name = "report/replay_coin_production_"+std::to_string(runnumber)+"_-1.report";
        std::ifstream infile;
        infile.open(report_name.c_str());

        std::string word,line,temp;
        double n; 
        word = "HMS BCM4A Charge";
        int number=0;  
        int ii=0;
        while(std::getline(infile,line))
        { 
          if(line.find(word)<line.length())
          {
            std::cout<<line<<" "<<ii<<std::endl;
            ++ii;
            for(int i = word.size(); i<line[i];i++){
              if(isdigit(line[i]) ){
                for(int a = i; a<line.size();a++){
                  temp += line[a];
                }
                break;  
              }
            }
            std::cout<<"temp"<<temp<<std::endl;
            //temp.clear();
            std::istringstream stream(temp);
            stream>>n;
            std::cout<<"number "<<n<<std::endl;
            temp.clear();
            //ns.push_back(n);
          }
        }
        charge_pos.push_back(n);

      }//go through pos runs
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["charge"] = std::accumulate(charge_neg.begin(),charge_neg.end(),0.0);
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["shmsTE"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["hmsTE"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["coinlive"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["normfac"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["neg"]["D2"]["factor"] = 1;

      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["charge"] = std::accumulate(charge_pos.begin(),charge_pos.end(),0.0);
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["shmsTE"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["hmsTE"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["coinlive"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["normfac"] = 1;
      j_info[(std::to_string(RunGroup)).c_str()]["pos"]["D2"]["factor"] = 1;
    }//if production runs
  }//go through all run groups
  std::ofstream outfile;
  outfile.open("results/db2/rungroup_info.json");
  outfile<<j_info.dump(4)<<std::endl;

}



