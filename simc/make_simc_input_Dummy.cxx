#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void make_simc_input_Dummy(int RunGroup = 0){

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
  
  std::vector<int> neg_Dummy,pos_Dummy;
  neg_Dummy = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["Dummy"].get<std::vector<int>>();
  pos_Dummy = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["Dummy"].get<std::vector<int>>();
  if(!neg_Dummy.empty() && !pos_Dummy.empty()){
    int RunNumber = neg_Dummy[0];
    std::cout<<"Using infos for Run "<<RunNumber<<std::endl;
  //variables to change
  int ngen = 10000; 
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

  //Dummy input file
  std::string ofs_Dummy_neg_inc_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_neg_inc_norad.inp";
  std::string ofs_Dummy_neg_inc_rad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_neg_inc_rad.inp";
  std::string ofs_Dummy_neg_exc_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_neg_exc_norad.inp";
 
  std::string ofs_Dummy_pos_inc_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_pos_inc_norad.inp";
  std::string ofs_Dummy_pos_inc_rad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_pos_inc_rad.inp";
  std::string ofs_Dummy_pos_exc_norad_name = "shuo_analysis/simc/input/csv_"+std::to_string(RunGroup)+"_Dummy_pos_exc_norad.inp";
  //H2,Dummy input file, However, I didn't write them yet
  
  std::ofstream ofs_Dummy_neg_inc_rad;
  ofs_Dummy_neg_inc_rad.open(ofs_Dummy_neg_inc_rad_name.c_str());
  std::ofstream ofs_Dummy_neg_inc_norad;
  ofs_Dummy_neg_inc_norad.open(ofs_Dummy_neg_inc_norad_name.c_str());
  std::ofstream ofs_Dummy_neg_exc_norad;
  ofs_Dummy_neg_exc_norad.open(ofs_Dummy_neg_exc_norad_name.c_str());
  std::ofstream ofs_Dummy_pos_inc_rad;
  ofs_Dummy_pos_inc_rad.open(ofs_Dummy_pos_inc_rad_name.c_str());
  std::ofstream ofs_Dummy_pos_inc_norad;
  ofs_Dummy_pos_inc_norad.open(ofs_Dummy_pos_inc_norad_name.c_str());
  std::ofstream ofs_Dummy_pos_exc_norad;
  ofs_Dummy_pos_exc_norad.open(ofs_Dummy_pos_exc_norad_name.c_str());

  int neg = 1,pos = 0;
  int norad = 0,rad =1;
  int inc = 1,exc = 0;
  //for norad
  ofs_Dummy_neg_inc_norad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_neg_inc_norad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" which_pion = "<<neg<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_semi = "<<inc<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_neg_inc_norad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_neg_inc_norad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_neg_inc_norad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_neg_inc_norad<<"begin parm target"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_neg_inc_norad<<"begin parm debug"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_neg_inc_norad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_inc_norad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_inc_norad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_neg_inc_norad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_norad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_neg_inc_norad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_rad = "<<norad<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_neg_inc_norad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_neg_inc_norad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_neg_inc_norad.close();
  
  //for rad
  ofs_Dummy_neg_inc_rad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_neg_inc_rad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" which_pion = "<<neg<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_semi = "<<inc<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_neg_inc_rad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_neg_inc_rad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_neg_inc_rad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_neg_inc_rad<<"begin parm target"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_neg_inc_rad<<"begin parm debug"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_neg_inc_rad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_inc_rad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_inc_rad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_neg_inc_rad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_inc_rad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_neg_inc_rad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_rad = "<<rad<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_neg_inc_rad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_neg_inc_rad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_neg_inc_rad.close();
  
  //for exclusive
  ofs_Dummy_neg_exc_norad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_neg_exc_norad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" which_pion = "<<neg<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_semi = "<<exc<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_neg_exc_norad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_neg_exc_norad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_neg_exc_norad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_neg_exc_norad<<"begin parm target"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_neg_exc_norad<<"begin parm debug"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_neg_exc_norad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_exc_norad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_neg_exc_norad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_neg_exc_norad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_neg_exc_norad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_neg_exc_norad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_rad = "<<norad<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_neg_exc_norad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_neg_exc_norad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_neg_exc_norad.close();
  
  //for pi+ norad
  ofs_Dummy_pos_inc_norad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_pos_inc_norad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" which_pion = "<<pos<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_semi = "<<inc<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_pos_inc_norad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_pos_inc_norad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_pos_inc_norad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_pos_inc_norad<<"begin parm target"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_pos_inc_norad<<"begin parm debug"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_pos_inc_norad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_inc_norad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_inc_norad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_pos_inc_norad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_norad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_pos_inc_norad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_rad = "<<norad<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_pos_inc_norad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_pos_inc_norad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_pos_inc_norad.close();
  
  //for rad
  ofs_Dummy_pos_inc_rad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_pos_inc_rad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" which_pion = "<<pos<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_semi = "<<inc<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_pos_inc_rad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_pos_inc_rad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_pos_inc_rad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_pos_inc_rad<<"begin parm target"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_pos_inc_rad<<"begin parm debug"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_pos_inc_rad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_inc_rad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_inc_rad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_pos_inc_rad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_inc_rad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_pos_inc_rad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_rad = "<<rad<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_pos_inc_rad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_pos_inc_rad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_pos_inc_rad.close();
  
  //for exclusive
  ofs_Dummy_pos_exc_norad<<";This is a CTP file, using info from Run "<<RunNumber<<"\n"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"begin parm experiment"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" ngen = "<<ngen<<std::endl;
  ofs_Dummy_pos_exc_norad<<" EXPER%charge = "<<charge<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_phsp = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_kaon = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_pion = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" which_pion = "<<pos<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_delta = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_rho = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_semi = "<<exc<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_hplus = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" doing_decay = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" ctau = 780.4"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm experiment\n"<<std::endl;

  ofs_Dummy_pos_exc_norad<<"begin parm kinematics_main"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" Ebeam = "<<Ebeam<<std::endl;
  ofs_Dummy_pos_exc_norad<<" dEbeam = "<<dEbeam<<std::endl;
  ofs_Dummy_pos_exc_norad<<" electron_arm =1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" hadron_arm = 5"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_first_cer = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%P = "<<e_momentum<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%theta = "<<e_theta<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%P = "<<p_momentum<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%theta = "<<p_theta<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm kinematics_main\n"<<std::endl;
  
  ofs_Dummy_pos_exc_norad<<"begin parm target"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%A = 27.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%Z = 13.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%mass_amu = 26.981541"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%mrec_amu = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%rho = 2.7"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%thick = 181.000"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%angle = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%abundancy = 100"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%can = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm target\n"<<std::endl;
  
  ofs_Dummy_pos_exc_norad<<"begin parm debug"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" debug(1) = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" debug(2) = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" debug(3) = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" debug(4) = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" debug(5) = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm debug\n"<<std::endl;
 
  ofs_Dummy_pos_exc_norad<<"begin parm e_arm_accept"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%delta%max = 15.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%e%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm e_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_exc_norad<<"begin parm p_arm_accept"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%delta%min = -15.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%delta%max = 30.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%yptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%yptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%xptar%min = -100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" SPedge%p%xptar%max = 100.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm p_arm_accept\n"<<std::endl;

  ofs_Dummy_pos_exc_norad<<"begin parm beamandtargetinfo"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" gen%xwid = 0.008868"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" gen%ywid = 0.004235"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%fr_pattern = 3"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%fr1 = 0.1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%fr2 = 0.1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%xoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%yoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%zoffset = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%Bangle = 80.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%Bphi = 270"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" targ%pol = 1.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm beamandtargetinfo\n"<<std::endl;
  
  ofs_Dummy_pos_exc_norad<<"begin parm spect_offset"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%e%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%offset%x = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%offset%y = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%offset%z = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%offset%xptar = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spec%p%offset%yptar = 0."<<std::endl;
  ofs_Dummy_pos_exc_norad<<"end parm spect_offset"<<std::endl;


  ofs_Dummy_pos_exc_norad<<"begin parm simulate"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" hard_cuts = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_rad = "<<norad<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_expon = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" one_tail = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" intcor_mode = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" spect_mode = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" cuts%Em%min = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" cuts%Em%max = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" correct_Eloss = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" correct_raster = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" mc_smear = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" deForest_flag = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" rad_flag = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" extrad_flag = 2"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" lambda(1) = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" lambda(2) = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" lambda(3) = 0.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" Nntu = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_Coulomb = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" dE_edge_test = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" use_offshell_rad = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" Egamma_gen_max = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" using_tgt_field= 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" tgt_field_file = trg_field_map.dat"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" drift_to_cal = 320.0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" do_fermi = 0"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" pt_b_param = 3.8"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" sigc_flag = 1"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" sigc_nbin = 10"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" sigc_kin_min = 0.035"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" sigc_kin_max = 0.150"<<std::endl;
  ofs_Dummy_pos_exc_norad<<" sigc_kin_ind = 0.550"<<std::endl;
  
  ofs_Dummy_pos_exc_norad<<"end parm simulate\n"<<std::endl;

  ofs_Dummy_pos_exc_norad.close();
  }
}
