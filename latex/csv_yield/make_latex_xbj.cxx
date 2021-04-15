#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex_xbj(){
  std::ofstream ofs("yield_ratio_xbj.tex");
  json j_Q2z;
  {
    std::ifstream ifs("db2/kin_group_Q2zx.json");
    ifs>>j_Q2z;
  }
  for(json::iterator it = j_Q2z.begin();it!=j_Q2z.end();++it){
     double Q2 = std::stod(it.key());
     auto j_zx = it.value();

     for(json::iterator i_z = j_zx.begin();i_z!=j_zx.end();++i_z){
       double z = std::stod(i_z.key());
       auto j_x = i_z.value();

      if(z!=0 && Q2!=0){
        std::string q2z_str = "Q2_z_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
      std::vector<std::string> counts_x_name;
      std::vector<int> rungroups;
      for(json::iterator i_x = j_x.begin();i_x!=j_x.end();++i_x){
        double xbj = std::stod(i_x.key());
        if(xbj!=0){
        std::string q2xz_str = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
        counts_x_name.push_back(q2xz_str);
        double RunGroup = i_x.value()["group_num"].get<int>();
        rungroups.push_back(RunGroup);
        std::cout<<q2xz_str<<std::endl;
        }
      }
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check_xbj/yieldcheck_"<<rungroups[0]<<"_neg.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check_xbj/yieldcheck_"<<rungroups[0]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      if(counts_x_name.size()>1){
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check_xbj/yieldcheck_"<<rungroups[1]<<"_neg.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check_xbj/yieldcheck_"<<rungroups[1]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      }
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield ratio}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_q2zcorr/"<<counts_x_name[0]<<"_ratio.pdf}"<<std::endl;
      if(counts_x_name.size()>1){
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_q2zcorr/"<<counts_x_name[1]<<"_ratio.pdf}"<<std::endl;
      }
      ofs<<"\\end{columns}"<<std::endl;
      
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield ratio}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_q2zcorr/"<<q2z_str<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      }//if xbj,Q2 not 0
      }//loop over Q2

  }//loop over xbj
}
