
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

void SHMS_rf_DE_choose(){
  json j_out;
  //  {
  //    std::ifstream ifstream("shuo_analysis/dbase/rf_eff.json");
  //    ifstream>>j_out;
  //  }
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_PID;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_PID;
  }
  std::vector<double> delta_cuts = j_PID["SHMS"]["delta_cuts_forrf"].get<std::vector<double>>();
  for(auto it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
    std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!pos_D2.empty() && !neg_D2.empty()){
      double shms_p_central = it.value()["shms_p"].get<double>();
      json j_rfeff;
      {
        std::string name = "results/pid/rftime/rf_eff_"+std::to_string(RunGroup)+".json";
        std::ifstream ifstream(name.c_str());
        ifstream>>j_rfeff;
      }
      json j_rfeff_delta;
      {
        std::string name = "results/pid/rftime/rf_eff_"+std::to_string(RunGroup)+"_compare.json";
        std::ifstream ifstream(name.c_str());
        ifstream>>j_rfeff_delta;
      }
      int i_dp = 0;
      for(auto ik = delta_cuts.begin();ik!=delta_cuts.end()-1;++ik){    
        double rf_right,rf_left;
        double delta_low = *ik;
        double shms_p = shms_p_central*(100+delta_low)/100;
        std::cout<<"Rungroup"<<RunGroup<<"delta"<<delta_low<<", momentum "<<shms_p<<std::endl;
        if(shms_p < 3){
          double pi_eff_all_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"].get<double>();
          std::cout<<"check"<<std::endl;
          std::vector<double> pi_eff_Ns_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_Ns"].get<std::vector<double>>();
          std::vector<double> rf_cuts_high_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_high"].get<std::vector<double>>();
          std::vector<double> rf_cuts_low_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_low"].get<std::vector<double>>();
          std::cout<<"check"<<std::endl;
          int i_rf_pos = 0;
          for(auto ij = pi_eff_Ns_pos.begin();ij!=pi_eff_Ns_pos.end();++ij){
            double pi_eff_N_pos = *ij;
            double pi_eff_pos = pi_eff_N_pos/pi_eff_all_pos;
            if(pi_eff_pos > 0.99){
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_N"] = pi_eff_N_pos;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"] = pi_eff_all_pos;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff"] = pi_eff_pos;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_N"] = -1;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_all"] = -1;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_purity"] = 1;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_low"] = rf_cuts_low_pos[i_rf_pos]; 
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_high"] = rf_cuts_high_pos[i_rf_pos]; 
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["which_percentage"] = i_rf_pos; 
              break; 

            }
            i_rf_pos++;
          }//loop over pos pi eff N s
          std::vector<double> pi_eff_Ns_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_Ns"].get<std::vector<double>>();
          std::vector<double> rf_cuts_high_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_high"].get<std::vector<double>>();
          std::vector<double> rf_cuts_low_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_low"].get<std::vector<double>>();
          double pi_eff_all_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"].get<double>();
          int i_rf_neg = 0;
          for(auto ij = pi_eff_Ns_neg.begin();ij!=pi_eff_Ns_neg.end();++ij){
            double pi_eff_N_neg = *ij;
            double pi_eff_neg = pi_eff_N_neg/pi_eff_all_neg;
            if(pi_eff_neg > 0.99){
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_N"] = pi_eff_N_neg;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"] = pi_eff_all_neg;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff"] = pi_eff_neg;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_N"] = -1;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_all"] = -1;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_purity"] = 1;
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_low"] = rf_cuts_low_neg[i_rf_neg]; 
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_high"] = rf_cuts_high_neg[i_rf_neg]; 
              j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["which_percentage"] = i_rf_neg; 
              break; 

            }
            i_rf_neg++;
          }//loop over neg
        }//if less than 3
        else{
          if(shms_p < 9){
            double pi_eff_all_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"].get<double>();
            std::cout<<"check"<<std::endl;
            std::vector<double> pi_eff_Ns_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_Ns"].get<std::vector<double>>();
            std::vector<double> K_con_Ns_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["Ks"].get<std::vector<double>>();
            std::vector<double> rf_cuts_high_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_high"].get<std::vector<double>>();
            std::vector<double> rf_cuts_low_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_low"].get<std::vector<double>>();
            std::cout<<"check"<<std::endl;
            int i_rf_pos = 0;
            for(auto ij = pi_eff_Ns_pos.begin();ij!=pi_eff_Ns_pos.end();++ij){
              double pi_eff_N_pos = *ij;
              double pi_eff_pos = pi_eff_N_pos/pi_eff_all_pos;
              if(pi_eff_pos > 0.99){
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_N"] = pi_eff_N_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"] = pi_eff_all_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff"] = pi_eff_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_N"] = K_con_Ns_pos[i_rf_pos];
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_all"] = K_con_Ns_pos[i_rf_pos]+pi_eff_N_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_purity"] = 1-K_con_Ns_pos[i_rf_pos]/(K_con_Ns_pos[i_rf_pos]+pi_eff_N_pos);
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_low"] = rf_cuts_low_pos[i_rf_pos]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_high"] = rf_cuts_high_pos[i_rf_pos]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["which_percentage"] = i_rf_pos; 
                break; 

              }
              i_rf_pos++;
            }//loop over pos pi eff N s
            double pi_eff_all_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"].get<double>();
            std::cout<<"check"<<std::endl;
            std::vector<double> pi_eff_Ns_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_Ns"].get<std::vector<double>>();
            std::vector<double> K_con_Ns_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["Ks"].get<std::vector<double>>();
            std::vector<double> rf_cuts_high_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_high"].get<std::vector<double>>();
            std::vector<double> rf_cuts_low_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_low"].get<std::vector<double>>();
            std::cout<<"check"<<std::endl;
            int i_rf_neg = 0;
            for(auto ij = pi_eff_Ns_neg.begin();ij!=pi_eff_Ns_neg.end();++ij){
              double pi_eff_N_neg = *ij;
              double pi_eff_neg = pi_eff_N_neg/pi_eff_all_neg;
              if(pi_eff_neg > 0.99){
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_N"] = pi_eff_N_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"] = pi_eff_all_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff"] = pi_eff_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_N"] = K_con_Ns_neg[i_rf_neg];
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_all"] = K_con_Ns_neg[i_rf_neg]+pi_eff_N_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_purity"] = 1-K_con_Ns_neg[i_rf_neg]/(K_con_Ns_neg[i_rf_neg]+pi_eff_N_neg);
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_low"] = rf_cuts_low_neg[i_rf_neg]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_high"] = rf_cuts_high_neg[i_rf_neg]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["which_percentage"] = i_rf_neg; 
                break; 

              }
              i_rf_neg++;
            }//loop over neg pi eff N s
          }//if less than 3.9
          else{
            double pi_eff_all_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"].get<double>();
            std::cout<<"check"<<std::endl;
            std::vector<double> pi_eff_Ns_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_Ns"].get<std::vector<double>>();
            std::vector<double> K_con_Ns_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["Ks"].get<std::vector<double>>();
            std::vector<double> rf_cuts_high_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_high"].get<std::vector<double>>();
            std::vector<double> rf_cuts_low_pos = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cuts_low"].get<std::vector<double>>();
            std::cout<<"check"<<std::endl;
            int i_rf_pos = 0;
            for(auto ij = pi_eff_Ns_pos.begin();ij!=pi_eff_Ns_pos.end();++ij){
              double pi_eff_N_pos = *ij;
              double pi_eff_pos = pi_eff_N_pos/pi_eff_all_pos;
              if(pi_eff_pos > 0.99){
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_N"] = pi_eff_N_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff_all"] = pi_eff_all_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_eff"] = pi_eff_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_N"] = K_con_Ns_pos[i_rf_pos];
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["K_con_all"] = K_con_Ns_pos[i_rf_pos]+pi_eff_N_pos;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["pi_purity"] = 1-K_con_Ns_pos[i_rf_pos]/(K_con_Ns_pos[i_rf_pos]+pi_eff_N_pos);
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_low"] = rf_cuts_low_pos[i_rf_pos]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["rf_cut_high"] = rf_cuts_high_pos[i_rf_pos]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["pos"]["which_percentage"] = i_rf_pos; 
                break; 

              }
              i_rf_pos++;
            }//loop over pos pi eff N s
            double pi_eff_all_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"].get<double>();
            std::cout<<"check"<<std::endl;
            std::vector<double> pi_eff_Ns_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_Ns"].get<std::vector<double>>();
            std::vector<double> K_con_Ns_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["Ks"].get<std::vector<double>>();
            std::vector<double> rf_cuts_high_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_high"].get<std::vector<double>>();
            std::vector<double> rf_cuts_low_neg = j_rfeff_delta[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cuts_low"].get<std::vector<double>>();
            std::cout<<"check"<<std::endl;
            int i_rf_neg = 0;
            for(auto ij = pi_eff_Ns_neg.begin();ij!=pi_eff_Ns_neg.end();++ij){
              double pi_eff_N_neg = *ij;
              double pi_eff_neg = pi_eff_N_neg/pi_eff_all_neg;
              if(pi_eff_neg > 0.99){
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_N"] = pi_eff_N_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff_all"] = pi_eff_all_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_eff"] = pi_eff_neg;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_N"] = -1;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["K_con_all"] = -1;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["pi_purity"] = 1;
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_low"] = rf_cuts_low_neg[i_rf_neg]; 
                j_out[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dp)).c_str()]["neg"]["rf_cut_high"] = rf_cuts_high_neg[i_rf_neg]; 
                break; 

              }
              i_rf_neg++;
            }//loop over neg pi eff N s
          }
        }
        i_dp++;
      }//loop over each delta
    }//if pos neg D2 not empty
  }//loop over rungroups
  std::string out_name = "shuo_analysis/dbase/rf_eff.json";
  std::ofstream ofs(out_name.c_str());
  ofs<<j_out.dump(4)<<std::endl;
}
