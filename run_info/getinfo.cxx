#include "shuo_analysis/run_info/CSV_Runinfo.h"

void getinfo(){
  CSV_Runinfo info;
  std::cout<<info.Get("shms_p",6068)<<std::endl;
}
