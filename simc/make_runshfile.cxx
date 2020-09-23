#include <iostream>
#include <string>

void make_runshfile(){
  for(int i = 1;i<59;i++)
  {
    std::string of_name = "run"+std::to_string(10*i)+".sh";
    std::ofstream of;
    of.open(of_name);
    of<<"#/bin/bash"<<std::endl;
    of<<"echo \"simulate RunGroup "<<10*i<<"\""<<std::endl;;
    of<<"cd /home/shuojia/simc_gfortran"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_neg_inc_norad | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_neg_inc_rad | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_neg_exc_norad | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_pos_inc_norad | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_pos_inc_rad | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_pos_exc_norad | ./simc"<<std::endl;
    of.close();
  }
}
