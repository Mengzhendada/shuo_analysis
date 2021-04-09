#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex_checkyield(){
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

      ofs<<"\\begin{frame}{"<<RunGroup_str<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"+RunGroup_str+"_pos.pdf}";

      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"+RunGroup_str+"_neg.pdf}";
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;


    }
  }
}
