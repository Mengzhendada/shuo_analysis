#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

void make_latex(){
  std::ofstream ofs("yield_ratio.tex");
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
      int RunNumber = pos_D2[0];
      std::string RunNumber_str = std::to_string(RunNumber);
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<".pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
    
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/SHMS_rf_"+RunGroup_str<<".pdf}";
      ofs<<"\\\\percentage: (Kpeak - Pipeak)*percentage + 1"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;

      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum"<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"HGC cut 0 \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/hgcer/SHMS_hgcer_eff_"<<RunNumber_str<<"_0.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"HGC cut 1 \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/hgcer/SHMS_hgcer_eff_"<<RunNumber_str<<"_1.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"HGC cut 2 \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/hgcer/SHMS_hgcer_eff_"<<RunNumber_str<<"_2.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.5\\textwidth}"<<std::endl;
      ofs<<"HGC cut 3 \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/hgcer/SHMS_hgcer_eff_"<<RunNumber_str<<"_3.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
            
      ofs<<"\\end{frame}"<<std::endl;

      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\includegraphics[width = 0.7\\textwidth]{results/pid/hgcer/SHMS_hgcer_eff_"<<RunNumber_str<<"_2_2d.pdf}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      

      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"HGC less than 2, no aero cut\\\\"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"first delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_0.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"second delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_1.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_2.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"forth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_3.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"fifth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_4.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"sixth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_5.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<",SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"first delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_0_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"second delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_1_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"third delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_2_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"forth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_3_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"fifth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_4_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"sixth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_pos_"<<RunGroup_str<<"_5_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<", SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"HGC less than 2, no aero cut\\\\"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"first delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_0.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"second delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_1.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_2.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"forth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_3.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"fifth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_4.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.33\\textwidth}"<<std::endl;
      ofs<<"sixth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_5.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
      
      ofs<<"\\begin{frame}{"<<RunGroup_str<<",SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"first delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_0_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"second delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_1_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"third delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_2_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"forth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_3_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"fifth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_4_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"sixth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/rftime/rftime_neg_"<<RunGroup_str<<"_5_pi.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;
    
      ofs<<"\\begin{frame}{"<<RunGroup_str<<",SHMS momentum "<<momentum<<"}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"first delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_0.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"second delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_1.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"third delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_2.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\begin{columns}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"forth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_3.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"fifth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_4.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\begin{column}[T]{0.3\\textwidth}"<<std::endl;
      ofs<<"sixth delta cut \\\\"<<std::endl;
      ofs<<"\\includegraphics[width = 0.9\\textwidth]{results/pid/SHMS_rf_"<<RunGroup_str<<"_5.pdf}"<<std::endl;
      ofs<<"\\end{column}"<<std::endl;
      ofs<<"\\end{columns}"<<std::endl;
      ofs<<"\\end{frame}"<<std::endl;

    }
    else{  
    ofs<<"\\begin{frame}{"<<RunGroup_str<<", "<<momentum<<"}"<<std::endl;
    ofs<<"singles runs"<<std::endl;
    ofs<<"\\end{frame}"<<std::endl;
    }
  }
}
