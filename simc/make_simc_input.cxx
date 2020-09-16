#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void make_simc_input(int RunGroup = 0){

  if(RunGroup == 0){
     std::cout<<"Enter Rungroup Number: "<<std::endl;
     std::cin>>RunGroup;
     if(RunGroup < 0)
       return;
    
  }
  
  //RunGroup = 10*RunGroup;

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_fall;
  {
    std::ifstream ifs("db2/run_list.json");
    ifs>>j_fall;
  }
  json j_spring;
  {
    std::ifstream ifs("db2/run_list_update.json");
    ifs>>j_spring;
  }
  
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(!neg_D2.empty() && !pos_D2.empty()){
    int RunNumber = neg_D2[0];
    std::cout<<"Using infos for Run "<<RunNumber<<std::endl;
  //variables to change
  int ngen = 100000; 
  //inclusive rad 100k, inclusive norad 50k, exclusive rad 30k
  double charge = 1.0;
  double Ebeam;
  //= 10214;
  double dEbeam;
  //= 0.00415;

  double e_momentum,e_theta,p_momentum,p_theta;
  if(RunNumber < 7000){
    e_momentum = std::abs(j_fall[std::to_string(RunNumber).c_str()]["spectrometers"]["hms_momentum"].get<double>())*1000;
    e_theta = j_fall[std::to_string(RunNumber).c_str()]["spectrometers"]["hms_angle"].get<double>();
    p_momentum = std::abs(j_fall[std::to_string(RunNumber).c_str()]["spectrometers"]["shms_momentum"].get<double>())*1000;
    p_theta = j_fall[std::to_string(RunNumber).c_str()]["spectrometers"]["shms_angle"].get<double>();
    Ebeam = 10597.825;
    dEbeam = 0.00415;
  }
  else{
    e_momentum = std::abs(j_spring[std::to_string(RunNumber).c_str()]["spectrometers"]["hms_momentum"].get<double>())*1000;
    e_theta = j_spring[std::to_string(RunNumber).c_str()]["spectrometers"]["hms_angle"].get<double>();
    p_momentum = std::abs(j_spring[std::to_string(RunNumber).c_str()]["spectrometers"]["shms_momentum"].get<double>())*1000;
    p_theta = j_spring[std::to_string(RunNumber).c_str()]["spectrometers"]["shms_angle"].get<double>();
    Ebeam = 10212.715;
    dEbeam = 0.00404;
  }
  //take one run to get the kinematic setting

  //D2 input file
  std::string ofs_D2_neg_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_norad.inp";
  std::string ofs_D2_neg_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_neg_inc_rad.inp";
  std::string ofs_D2_neg_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_neg_exc_rad.inp";
 
  std::string ofs_D2_pos_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_norad.inp";
  std::string ofs_D2_pos_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_pos_inc_rad.inp";
  std::string ofs_D2_pos_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_D2_pos_exc_rad.inp";
  //H2,Dummy input file, However, I didn't write them yet
  
  //I didn't write rad yet
  std::ofstream ofs_D2_neg_rad;
  ofs_D2_neg_rad.open(ofs_D2_neg_rad_name.c_str());
  std::ofstream ofs_D2_neg_norad;
  ofs_D2_neg_norad.open(ofs_D2_neg_norad_name.c_str());

  //for norad
  ofs_D2_neg_norad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_D2_neg_norad<<"begin param experiment"<<std::endl;
  ofs_D2_neg_norad<<" ngen = "<<ngen<<std::endl;
  ofs_D2_neg_norad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_D2_neg_norad<<" doing_pion = 1"<<std::endl;
  ofs_D2_neg_norad<<" which_pion = 1"<<std::endl;
  ofs_D2_neg_norad<<" doing_delta = 0"<<std::endl;
  ofs_D2_neg_norad<<" doing_rho = 0"<<std::endl;
  ofs_D2_neg_norad<<" doing_semi = 1"<<std::endl;
  ofs_D2_neg_norad<<" doing_decay = 1"<<std::endl;
  ofs_D2_neg_norad<<" doing_hplus = 0"<<std::endl;
  ofs_D2_neg_norad<<" ctau = 780.4"<<std::endl;
  ofs_D2_neg_norad<<"end parm experiment\n"<<std::endl;

  ofs_D2_neg_norad<<"begin parm kinematics_main"<<std::endl;
  ofs_D2_neg_norad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_D2_neg_norad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_D2_neg_norad<<" electron_arm =1"<<std::endl;
  ofs_D2_neg_norad<<" hadron_arm = 5"<<std::endl;
  ofs_D2_neg_norad<<" pec%e%P = "<<e_momentum<<std::endl;
  ofs_D2_neg_norad<<" pec%e%theta = "<<e_theta<<std::endl;
  ofs_D2_neg_norad<<" pec%p%P = "<<p_momentum<<std::endl;
  ofs_D2_neg_norad<<" pec%p%theta = "<<p_theta<<std::endl;
  ofs_D2_neg_norad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_D2_neg_norad<<"begin parm target"<<std::endl;
  ofs_D2_neg_norad<<" targ%A = 2.0"<<std::endl;
  ofs_D2_neg_norad<<" targ%Z = 1.0"<<std::endl;
  ofs_D2_neg_norad<<" targ%mass_amu = 2.0141017"<<std::endl;
  ofs_D2_neg_norad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_D2_neg_norad<<" targ%rho = 0.1668"<<std::endl;
  ofs_D2_neg_norad<<" targ%thick = 1668"<<std::endl;
  ofs_D2_neg_norad<<" targ%angle = 0"<<std::endl;
  ofs_D2_neg_norad<<" targ%abundancy = 100"<<std::endl;
  ofs_D2_neg_norad<<" targ%can = 1"<<std::endl;
  ofs_D2_neg_norad<<"end parm target\n"<<std::endl;
  
  ofs_D2_neg_norad<<"begin parm debug"<<std::endl;
  ofs_D2_neg_norad<<" debug(1) = 0"<<std::endl;
  ofs_D2_neg_norad<<" debug(2) = 0"<<std::endl;
  ofs_D2_neg_norad<<" debug(3) = 0"<<std::endl;
  ofs_D2_neg_norad<<" debug(4) = 0"<<std::endl;
  ofs_D2_neg_norad<<" debug(5) = 0"<<std::endl;
  ofs_D2_neg_norad<<"end parm debug\n"<<std::endl;
 
  ofs_D2_neg_norad<<"begin parm e_arm_accept"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_D2_neg_norad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_D2_neg_norad<<"begin parm p_arm_accept"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_D2_neg_norad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_D2_neg_norad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_D2_neg_norad<<"begin param beamandtargetinfo"<<std::endl;
  ofs_D2_neg_norad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_D2_neg_norad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_D2_neg_norad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_D2_neg_norad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_D2_neg_norad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_D2_neg_norad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_D2_neg_norad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_D2_neg_norad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_D2_neg_norad<<"end param beamandtargetinfo\n"<<std::endl;
  
  ofs_D2_neg_norad<<"begin param simulate"<<std::endl;
  ofs_D2_neg_norad<<" hard_cuts = 0"<<std::endl;
  ofs_D2_neg_norad<<" using_rad = 0"<<std::endl;
//  ofs_D2_neg_norad<<" using_expon = 0"<<std::endl;
//  ofs_D2_neg_norad<<" using_tail = 0"<<std::endl;
//  ofs_D2_neg_norad<<" intcor_mode = 1"<<std::endl;
//  ofs_D2_neg_norad<<" spect_mode = 0"<<std::endl;
//  ofs_D2_neg_norad<<" cuts%Em%min = 0"<<std::endl;
//  ofs_D2_neg_norad<<" cuts%Em%max = 0"<<std::endl;
//  ofs_D2_neg_norad<<" using_Eloss = 1"<<std::endl;
//  ofs_D2_neg_norad<<" correct_Eloss = 1"<<std::endl;
//  ofs_D2_neg_norad<<" correct_raster = 1"<<std::endl;
//  ofs_D2_neg_norad<<" mc_smear = 1"<<std::endl;
  ofs_D2_neg_norad<<"end param simulate\n"<<std::endl;

  ofs_D2_neg_norad.close();
  }
}
