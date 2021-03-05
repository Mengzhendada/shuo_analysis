#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex(){
  std::ofstream ofs("check.tex");
  json j_dbase;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_dbase;
  }
  for(int i = 0;i<58;i++){
    std::string RunGroup_str = std::to_string(10*(i+1));
    std::cout<<RunGroup_str<<std::endl;
    double momentum = j_dbase[RunGroup_str.c_str()]["shms_p"].get<double>();
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_dbase[RunGroup_str.c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_dbase[RunGroup_str.c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty() && !pos_D2.empty()){
      for(auto it = pos_D2.begin();it!=pos_D2.end();it++){
        int RunNumber = *it;
        std::string RunNumber_str = std::to_string(RunNumber);

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/cointime_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/current_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_cal_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_cer_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_dp_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_aero_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_cal_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_dp_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber pos "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/time_diff_"+RunNumber_str+"_pos.pdf}";
        ofs<<"\\end{frame}"<<std::endl;


      }    
      for(auto it = neg_D2.begin();it!=neg_D2.end();it++){
        int RunNumber = *it;
        std::string RunNumber_str = std::to_string(RunNumber);

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/cointime_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/current_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_cal_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_cer_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/hms_dp_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_aero_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_cal_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/shms_dp_"+RunNumber_str+"_neg.pdf}";
        ofs<<"\\end{frame}"<<std::endl;

        ofs<<"\\begin{frame}{"<<RunGroup_str<<", RunNumber neg "<<RunNumber_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/time_diff_"+RunNumber_str+"_neg.pdf}";
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
