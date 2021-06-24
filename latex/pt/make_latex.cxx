#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex(){
  std::ofstream ofs("pt.tex");
  std::cout<<"check"<<std::endl;
  json j_Q2x;
  {
    std::ifstream ifs("db2/kin_group_xQ2z.json");
    ifs>>j_Q2x;
  }
  
  std::cout<<"check"<<std::endl;
  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj = std::stod(it.key());
    auto j_Q2z = it.value();
    for(json::iterator i_Q2 = j_Q2z.begin();i_Q2!=j_Q2z.end();++i_Q2){
      double Q2 = std::stod(i_Q2.key());
      auto j_z = i_Q2.value();


      if(xbj!=0 && Q2!=0){
        std::string q2x_str = "x_Q2_"+std::to_string(xbj).substr(0,4)+"_"+std::to_string(Q2).substr(0,5);
        std::string q2x_filestr = "x_Q2_"+std::to_string(100*xbj).substr(0,2)+"_"+std::to_string(1000*Q2).substr(0,4);
  std::cout<<"check"<<q2x_str<<std::endl;
        std::vector<std::string> counts_z_name;
        std::vector<int> neg_D2_z,pos_D2_z;
        for(json::iterator i_z = j_z.begin();i_z!=j_z.end();++i_z){
          double z = std::stod(i_z.key());
        std::vector<int> neg_D2,pos_D2;
        neg_D2 = i_z.value()["neg"]["D2"].get<std::vector<int>>();
        pos_D2 = i_z.value()["pos"]["D2"].get<std::vector<int>>();
        neg_D2_z.push_back(neg_D2[0]);
        pos_D2_z.push_back(pos_D2[0]);
        }

        ofs<<"\\begin{frame}{"<<q2x_str<<",pt phi polar neg}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/polar_neg_"<<q2x_filestr<<".pdf}"<<std::endl;
        ofs<<"\\end{frame}"<<std::endl;
        ofs<<"\\begin{frame}{"<<q2x_str<<",pt phi polar pos}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/polar_pos_"<<q2x_str<<".pdf}"<<std::endl;
        ofs<<"\\end{frame}"<<std::endl;
      
        ofs<<"\\begin{frame}{one of the neg run example "<<neg_D2_z[0]<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/phi_neg_"<<std::to_string(neg_D2_z[0])<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/th_neg_"<<std::to_string(neg_D2_z[0])<<".pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/pt_neg_"<<std::to_string(neg_D2_z[0])<<".pdf}"<<std::endl;
        ofs<<"\\end{frame}"<<std::endl;
        
        ofs<<"\\begin{frame}{one of the pos run example "<<pos_D2_z[0]<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/phi_pos_"<<std::to_string(pos_D2_z[0])<<"_ratio.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/th_pos_"<<std::to_string(pos_D2_z[0])<<".pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/pt_pos_"<<std::to_string(pos_D2_z[0])<<".pdf}"<<std::endl;
        ofs<<"\\end{frame}"<<std::endl;
        
        ofs<<"\\begin{frame}{"<<q2x_str<<",pt phi polar pos}"<<std::endl;
        ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pt/pt_ratio_"<<q2x_str<<"_ratio.pdf}"<<std::endl;
        ofs<<"\\end{frame}"<<std::endl;
      
      //ofs<<"\\begin{columns}"<<std::endl;
      //ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
//
//      ofs<<"\\end{column}"<<std::endl;
//      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
//      if(counts_z_name.size()>3){
//      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/yield/statistics_corr/"<<counts_z_name[3]<<"_ratio.pdf}"<<std::endl;
//      }
//      ofs<<"\\end{column}"<<std::endl;
//      ofs<<"\\end{columns}"<<std::endl;
//      }
//      ofs<<"\\end{frame}"<<std::endl;

      }//if xbj,Q2 not 0
    }//loop over Q2

  }//loop over xbj
}
