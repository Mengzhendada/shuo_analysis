#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
  using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <fstream>
#include <vector>
#include <string>

void shms_rate_D2(int RunGroup){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit) ";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  
  if(!neg_D2.empty() && !pos_D2.empty()){
      double shmsp = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
      json jout;
      {
      std::ifstream ifs("results/TE/trackingeff_info.json");
      ifs>>jout;
      }
      jout[(std::to_string(RunGroup)).c_str()]["shms_p"] = shmsp;
    //for neg runs
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        ROOT::RDataFrame d_neg_scale("TSP",rootfile_name);
        std::string start_time;
        json j_runs;
        if(RunNumber < 7000)
        {
          std::ifstream ifs("db2/run_list.json");//no daq info here, need to fix this
          ifs>>j_runs;
        start_time = j_runs[(std::to_string(RunNumber)).c_str()]["start_time"].get<std::string>();
        std::cout<<start_time<<" "<<start_time.size()<<std::endl;
        }
        else{
          std::ifstream ifs("db2/run_list_coin.json");
          ifs>>j_runs;
        start_time = j_runs[(std::to_string(RunNumber)).c_str()]["run_info"]["start_time"].get<std::string>();
        std::cout<<start_time<<" "<<start_time.size()<<std::endl;
        }
        std::cout<< "start_time " <<start_time<<std::endl;
        std::string starttime,startdate;
        starttime = start_time.substr(0,start_time.size()-13);
        std::cout<<"only time "<<starttime<<" start_time "<<start_time<<std::endl;
        startdate = start_time.substr(8,start_time.size()-4);
        std::cout<<starttime<<"at"<<startdate<<std::endl;
        char time_char[10];
        char date_char[10];
        int hour;
        int minute;
        int second;
        int month;
        int date;
        int year;
        std::istringstream time_ss(starttime.c_str());
        time_ss>>time_char;
        std::sscanf(time_char,"%d:%d:%d",&hour,&minute,&second);
        std::istringstream date_ss(startdate.c_str());
        date_ss>>date_char;
        std::sscanf(date_char,"%d/%d/%d",&month,&date,&year);
        std::cout<<hour<<":"<<minute<<":"<<second<<" "<<month<<"/"<<date<<"/"<<year<<std::endl;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["hour"] = hour;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["minute"] = minute;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["second"] = second;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["month"] = month;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["date"] = date;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["start_time"]["year"] = year;
        //std::tm t = {};
        //std::istringstream ss(start_time.c_str());
        //ss.imbue(std::locale("de_DE.utf-8"));
        //ss>>get_time(&t,"%H:%M:%S %m-%d-%y");
        //std::cout<<t.tm_hour<<":"<<t.tm_min<<":"<<t.tm_sec<<std::endl;
        //int ps1 = -1;
        //int ps2 = -1;
        //int ps5 = -1;
        //int ps6 = -1;
        //if(j_runs[(std::to_string(RunNumber)).c_str()].find("daq")!= j_runs[(std::to_string(RunNumber)).c_str()].end()){
        //ps1 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps1"].get<int>();
        //ps2 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps2"].get<int>();
        //ps5 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps5"].get<int>();
        //ps6 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps6"].get<int>();
        //}
        //else{
        //  std::cout<< "Error: pre-scaler unspecified for "<<RunNumber<<std::endl;
        //  std::quick_exit(-127);
        //}
        //double ps_factor = (ps6 == 0 ) ? 1. : (std::pow(2,ps6 - 1) + 1);

       // auto h_ptrig1 = d_neg_scale.Histo1D({"","",}) 
        double total_charge = *d_neg_scale.Max("P.BCM4B.scalerChargeCut")/1000.0;
        double time_1MHz_cut = *d_neg_scale.Max("P.1MHz.scalerTimeCut");
        int scaler_events = *d_neg_scale.Count();
        long int datacounts = *d_neg_scale.Max("P.pTRIG1.scaler"); 
        std::cout<<"data counts "<<datacounts<<std::endl;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["charge"] = total_charge;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["time"] = time_1MHz_cut;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["scaler_n"] = scaler_events;
        jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["data_n"] = datacounts;
      jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["target"] = "D2"; 
        //jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["daq"]["ps6"] = ps6;
        //jout[(std::to_string(RunGroup)).c_str()]["neg"][(std::to_string(RunNumber)).c_str()]["ps_factor"] = ps_factor;


      }
      //for pos runs
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        ROOT::RDataFrame d_pos_scale("TSP",rootfile_name);
        std::string start_time;
        json j_runs;
        if(RunNumber < 7000)
        {
          std::ifstream ifs("db2/run_list.json");//no daq info here, need to fix this
          ifs>>j_runs;
        start_time = j_runs[(std::to_string(RunNumber)).c_str()]["start_time"].get<std::string>();

        }
        else{
          std::ifstream ifs("db2/run_list_coin.json");
          ifs>>j_runs;
        start_time = j_runs[(std::to_string(RunNumber)).c_str()]["run_info"]["start_time"].get<std::string>();
        }
        std::cout<<start_time<<std::endl;
        std::string starttime,startdate;
        starttime = start_time.substr(0,start_time.size()-13);
        startdate = start_time.substr(8,start_time.size()-4);
        std::cout<<starttime<<"at"<<startdate<<std::endl;
        char time_char[10];
        char date_char[10];
        int hour;
        int minute;
        int second;
        int month;
        int date;
        int year;
        std::istringstream time_ss(starttime.c_str());
        time_ss>>time_char;
        std::sscanf(time_char,"%d:%d:%d",&hour,&minute,&second);
        std::istringstream date_ss(startdate.c_str());
        date_ss>>date_char;
        std::sscanf(date_char,"%d/%d/%d",&month,&date,&year);
        std::cout<<hour<<":"<<minute<<":"<<second<<" "<<month<<"/"<<date<<"/"<<year<<std::endl;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["hour"] = hour;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["minute"] = minute;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["second"] = second;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["month"] = month;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["date"] = date;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["start_time"]["year"] = year;
        //int ps1 = -1;
        //int ps2 = -1;
        //int ps5 = -1;
        //int ps6 = -1;
        //if(j_runs[(std::to_string(RunNumber)).c_str()].find("daq")!= j_runs[(std::to_string(RunNumber)).c_str()].end()){
        //ps1 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps1"].get<int>();
        //ps2 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps2"].get<int>();
        //ps5 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps5"].get<int>();
        //ps6 = j_runs[(std::to_string(RunNumber)).c_str()]["daq"]["ps6"].get<int>();
        //}
        //else{
        //  std::cout<< "Error: pre-scaler unspecified for "<<RunNumber<<std::endl;
        //  std::quick_exit(-127);
        //}
        //double ps_factor = (ps6 == 0 ) ? 1. : (std::pow(2,ps6 - 1) + 1);


        double total_charge = *d_pos_scale.Max("P.BCM4B.scalerChargeCut")/1000.0;
        double time_1MHz_cut = *d_pos_scale.Max("P.1MHz.scalerTimeCut");
        int scaler_events = *d_pos_scale.Count();
        long int datacounts = *d_pos_scale.Max("P.pTRIG1.scaler"); 
        std::cout<<"data counts "<<datacounts<<std::endl;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["charge"] = total_charge;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["time"] = time_1MHz_cut;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["scaler_n"] = scaler_events;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["data_n"] = datacounts;
        jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["target"] = "D2"; 
        
        //jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["daq"]["ps6"] = ps6;
        //jout[(std::to_string(RunGroup)).c_str()]["pos"][(std::to_string(RunNumber)).c_str()]["ps_factor"] = ps_factor;
      }

      std::ofstream ofs("results/TE/trackingeff_info.json");
      ofs<<jout.dump(4)<<std::endl;

  }
}
