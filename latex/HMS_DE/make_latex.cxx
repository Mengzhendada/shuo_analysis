#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex(){
  std::ofstream ofs("HMS_DE.tex");
  json j_dbase;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_dbase;
  }
  for(int i = 0;i<58;i++){
    std::string RunGroup_str = std::to_string(10*(i+1));
    std::cout<<RunGroup_str<<std::endl;
    double momentum = j_dbase[RunGroup_str.c_str()]["shms_p"].get<double>();
    double x = j_dbase[RunGroup_str.c_str()]["x"].get<double>();
    double Q2 = j_dbase[RunGroup_str.c_str()]["Q2"].get<double>();
    double z = j_dbase[RunGroup_str.c_str()]["z"].get<double>();
    double hms_p = j_dbase[RunGroup_str.c_str()]["hms_p"].get<double>();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_dbase[RunGroup_str.c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_dbase[RunGroup_str.c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;

      ofs<<"\\begin{frame}{"<<RunGroup_str<<", hms momentum "<<hms_p<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/HMS_cal_"<<RunNumber<<".pdf}"<<std::endl;
      ofs<<"\\\\ x = "<<x<<",Q2 = "<<Q2<<",z = "<<z<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", hms momentum "<<hms_p<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/cal_DE_"<<RunNumber<<".pdf}"<<std::endl;
      ofs<<"\\\\x = "<<x<<",Q2 = "<<Q2<<",z = "<<z<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", hms momentum "<<hms_p<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/HMS_cer_"<<RunNumber<<".pdf}"<<std::endl;
      ofs<<"\\\\ x = "<<x<<",Q2 = "<<Q2<<",z = "<<z<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", hms momentum "<<hms_p<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/cer_DE_"<<RunNumber<<".pdf}"<<std::endl;
      ofs<<"\\\\x = "<<x<<",Q2 = "<<Q2<<",z = "<<z<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      }      
    
    }
    else{  
    ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
    ofs<<"singles runs"<<std::endl;
    ofs<<"\\end{frame}"<<std::endl;
    }
  }
}
