#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex_more(){
  std::ofstream ofs("yield_ratio.tex");
  json j_Q2x;
  {
    std::ifstream ifs("db2/kin_group_xQ2z.json");
    ifs>>j_Q2x;
  }
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
     double xbj = std::stod(it.key());
     auto j_Q2z = it.value();

     for(json::iterator i_Q2 = j_Q2z.begin();i_Q2!=j_Q2z.end();++i_Q2){
       double Q2 = std::stod(i_Q2.key());
       auto j_z = i_Q2.value();

      if(xbj!=0 && Q2!=0){
        std::string q2x_str = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
      std::vector<std::string> counts_z_name;
      std::vector<int> rungroups;
      for(json::iterator i_z = j_z.begin();i_z!=j_z.end();++i_z){
        double z = std::stod(i_z.key());
        std::string q2xz_str = "x_Q2_z_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4)+"_"+std::to_string(100*z).substr(0,2);
        counts_z_name.push_back(q2xz_str);
        double RunGroup = i_z.value()["group_num"].get<int>();
        rungroups.push_back(RunGroup);
      }
      
        ofs<<"\\begin{frame}{corrected yield}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[0]<<"_pos.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[0]<<"_neg.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[1]<<"_pos.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[1]<<"_neg.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      if(counts_z_name.size()>2){
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[2]<<"_pos.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[2]<<"_neg.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      if(counts_z_name.size()>3){
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[3]<<"_pos.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.25\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = \\textwidth]{results/yield/statistics_corr/yield_"<<counts_z_name[3]<<"_neg.png}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      }
      ofs<<"\\end{columns}"<<std::endl;
      }
      ofs<<"\\end{frame}"<<std::endl;
        
      //ofs<<"\\begin{frame}{raw yield ratio}"<<std::endl;
      //ofs<<"\\begin{columns}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics/"<<counts_z_name[0]<<"_ratio.pdf}"<<std::endl;
      //ofs<<"\\end{column}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics/"<<counts_z_name[1]<<"_ratio.pdf}"<<std::endl;
      //ofs<<"\\end{column}"<<std::endl;
      //ofs<<"\\end{columns}"<<std::endl;
      //if(counts_z_name.size()>2){
      //ofs<<"\\begin{columns}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics/"<<counts_z_name[2]<<"_ratio.pdf}"<<std::endl;

      //ofs<<"\\end{column}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      //if(counts_z_name.size()>3){
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics/"<<counts_z_name[3]<<"_ratio.pdf}"<<std::endl;
      //}
      //ofs<<"\\end{column}"<<std::endl;
      //ofs<<"\\end{columns}"<<std::endl;
      //}
      //ofs<<"\\end{frame}"<<std::endl;
      
      //ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield}"<<std::endl;
      //ofs<<"\\begin{columns}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/check/"<<counts_z_name[0]<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[0]<<"_neg.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[0]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[1]<<"_neg.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[1]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      if(counts_z_name.size()>2){
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[2]<<"_neg.pdf}"<<std::endl;

      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[2]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;

      if(counts_z_name.size()>3){
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[3]<<"_neg.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/yield/check/yieldcheck_"<<rungroups[3]<<"_pos.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      }
      ofs<<"\\end{frame}"<<std::endl;
      }
      
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield ratio}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<counts_z_name[0]<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<counts_z_name[1]<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      if(counts_z_name.size()>2){
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<counts_z_name[2]<<"_ratio.pdf}"<<std::endl;

      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      if(counts_z_name.size()>3){
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<counts_z_name[3]<<"_ratio.pdf}"<<std::endl;
      }
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      }
      ofs<<"\\end{frame}"<<std::endl;
      
      //ofs<<"\\begin{frame}{raw yield ratio}"<<std::endl;
      //ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics/"<<q2x_str<<"_ratio.pdf}"<<std::endl;
      //ofs<<"\\end{frame}"<<std::endl;
      ofs<<"\\begin{frame}{TE,pi eff, pi purity corrected yield ratio}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<q2x_str<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      }//if xbj,Q2 not 0
      }//loop over Q2

  }//loop over xbj
}
