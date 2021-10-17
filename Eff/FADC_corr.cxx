#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
double Get_FADC_eff(double SHMS_rate,double HMS_rate){
  return 1- 0.05*(SHMS_rate/1000 - HMS_rate/1000);
  //1- 0.05*(shms_3/4 rate in khz/1000 - hms_elReal_rate in Khz/1000)
}

void FADC_corr(){
  json j_info;
  {
    std::ifstream ifs("results/yield/runs_info.json");
    ifs>>j_info;
  }
  for(auto it = j_info.begin();it!=j_info.end();it++){
    int RunNumber = std::stoi(it.key());
    std::cout<<RunNumber<<std::endl;

    std::string report_file_name = "report/replay_coin_production_"+std::to_string(RunNumber)+"_-1.report";
    std::ifstream report_file;
    report_file.open(report_file_name.c_str());
    std::string SHMS_rate_str = "SHMS 3/4 Trigger Rate";
    std::string HMS_rate_str = "HMS_hEL_REAL";
    double SHMS_rate,HMS_rate;
    std::string line;
    while(std::getline(report_file,line)){
      if(line.find(SHMS_rate_str)<line.length()){
        std::cout<<line<<std::endl;
        size_t i = 0;
        i = line.find(':');
        std::string SHMS_rate_str2 = line.substr(i+1,line.length()-1);
        std::cout<<SHMS_rate_str2<<std::endl;
        i = 0;
        for(;i<SHMS_rate_str2.length();i++){if(!isdigit(SHMS_rate_str2[i])) break;}
        SHMS_rate = std::stod(SHMS_rate_str2.substr(0,i-1));
        std::cout<<"SHMS rate "<<SHMS_rate<<std::endl;    
      }
      if(line.find(HMS_rate_str)<line.length()){
        std::cout<<line<<std::endl;
        size_t i = 0;
        i = line.find('[');
        std::string HMS_rate_str2 = line.substr(i+1,line.length()-1);
        std::cout<<HMS_rate_str2<<std::endl;
        std::cout<<std::stod(HMS_rate_str2)<<std::endl;
        i = 0;
        for(;i<HMS_rate_str2.length();i++){if(!isdigit(HMS_rate_str2[i])) break;}
        HMS_rate = std::stod(HMS_rate_str2.substr(0,i-1));
        std::cout<<"HMS rate "<<HMS_rate<<std::endl;    
      }
    }
    double FADC_eff = Get_FADC_eff(SHMS_rate,HMS_rate);
    j_info[std::to_string(RunNumber)]["FADC_eff"] = FADC_eff;
  }
  std::ofstream ofs("results/yield/runs_info.json");
  ofs<<j_info.dump(4)<<std::endl;
}
