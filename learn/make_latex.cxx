#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex(){
  std::ofstream ofs("all_rftime.tex");
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
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/coin_time_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/rf_shmsp_pos_1st_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/hgcer_rfcut_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/hgcer_npe_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/hgcer_rfcut_2d_"<<RunGroup_str<<"_picut.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/hgcer_rfcut_2d_"<<RunGroup_str<<"_Kcut.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_npe_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_rftime_pirfcut_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns} \n"<< "\\begin{column}[T]{0.5\\textwidth} \n";
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_rfcut_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{column} \n"<<"\\begin{column}[T]{0.5\\textwidth} \n";
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_rfcut_2nd_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{column} \n"<<"\\end{columns} \n";
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_rfcut_2d_"<<RunGroup_str<<"_picut.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{rftime/aero_rfcut_2d_"<<RunGroup_str<<"_Kcut.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
    }
    else{  
    ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
    ofs<<"singles runs"<<std::endl;
    ofs<<"\\end{frame}"<<std::endl;
    }
  }
}
