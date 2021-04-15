#include <iostream>
#include <string>

int make_runshfile(){
  for(int i = 1;i<59;i++)
  {
    std::string of_name = "shuo_analysis/simc/script/run"+std::to_string(10*i)+".sh";
    std::ofstream of;
    of.open(of_name,std::ofstream::out);
    of<<"#/bin/bash"<<std::endl;
    of<<"echo \"simulate RunGroup "<<10*i<<"\""<<std::endl;;
    of<<"cd /group/c-csv/shuo/simc_gfortran"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_neg_inc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_neg_inc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_neg_exc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_neg_rho | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_neg_delta | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_pos_inc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_pos_inc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_pos_exc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_D2_pos_rho | ./simc"<<std::endl;
    of<<"echo csv_"<<10*i<<"_D2_pos_delta | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_neg_inc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_neg_inc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_neg_exc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_neg_rho | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_pos_inc_norad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_pos_inc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_pos_exc_rad | ./simc"<<std::endl;
    //of<<"echo csv_"<<10*i<<"_Dummy_pos_rho | ./simc"<<std::endl;
    of.close();
  }
  return 0;
}
