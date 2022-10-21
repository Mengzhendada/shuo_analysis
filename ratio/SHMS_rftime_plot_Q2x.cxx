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
#include "TF1.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;
double fit_pol1(double *x,double *pars){
  return pars[0]*x[0]+pars[1];
}
double fit_pol1_neg(double *x,double a,double b,double c){
  return c*(a*x[0]+b);
}
void SHMS_rftime_plot_Q2x(){
  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runsinfo;
  }
  json j_Q2x;
  {
    std::ifstream runs("db2/kin_group_xQ2z.json");
    runs>>j_Q2x;
  }
  json jout;
  {
    std::ifstream ifs("results/pid/Kpi_ratio.json");
    ifs>>jout;
  }

  for(json::iterator it = j_Q2x.begin();it!=j_Q2x.end();++it){
    double xbj_set = std::stod(it.key());
    auto j_Q2z = it.value();
    for(json::iterator ik = j_Q2z.begin();ik!=j_Q2z.end();++ik){
      double Q2_set = std::stod(ik.key());
      std::string file_name;
      auto j_z = ik.value();
      TGraphErrors* g_pos_zhad_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_zhad_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_zhad_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_zhad_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_xbj_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_xbj_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_xbj_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_xbj_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_shmsp_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_shmsp_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_shmsp_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_shmsp_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_shmsdp_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_shmsdp_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_shmsdp_fall_HGCeff = new TGraphErrors();
      TGraphErrors* g_neg_shmsdp_spring_HGCeff = new TGraphErrors();
      TGraphErrors* g_pos_zhad_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_zhad_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_zhad_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_zhad_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_xbj_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_xbj_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_xbj_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_xbj_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_shmsp_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_shmsp_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_shmsp_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_shmsp_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_shmsdp_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_pos_shmsdp_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_shmsdp_fall_Kpiratio = new TGraphErrors();
      TGraphErrors* g_neg_shmsdp_spring_Kpiratio = new TGraphErrors();
      TGraphErrors* g_zhad_fall_Kratio = new TGraphErrors();
      TGraphErrors* g_zhad_spring_Kratio = new TGraphErrors();
      TGraphErrors* g_xbj_fall_Kratio = new TGraphErrors();
      TGraphErrors* g_xbj_spring_Kratio = new TGraphErrors();
      TGraphErrors* g_shmsp_fall_Kratio = new TGraphErrors();
      TGraphErrors* g_shmsp_spring_Kratio = new TGraphErrors();
      TGraphErrors* g_shmsdp_fall_Kratio = new TGraphErrors();
      TGraphErrors* g_shmsdp_spring_Kratio = new TGraphErrors();
      TGraphErrors* g_zhad_fall_piratio = new TGraphErrors();
      TGraphErrors* g_zhad_spring_piratio = new TGraphErrors();
      TGraphErrors* g_xbj_fall_piratio = new TGraphErrors();
      TGraphErrors* g_xbj_spring_piratio = new TGraphErrors();
      TGraphErrors* g_shmsp_fall_piratio = new TGraphErrors();
      TGraphErrors* g_shmsp_spring_piratio = new TGraphErrors();
      TGraphErrors* g_shmsdp_fall_piratio = new TGraphErrors();
      TGraphErrors* g_shmsdp_spring_piratio = new TGraphErrors();
      
      TGraphErrors* g_pos_zhad_Kpiratio = new TGraphErrors();
      int i_fall = 0,i_spring =0,i_all = 0;
      if(xbj_set>0 && Q2_set!=0){
        for(json::iterator iz = j_z.begin();iz!=j_z.end();++iz){
          double z = std::stod(iz.key());
        std::cout<<"xbj set"<<xbj_set<<" Q2 set"<<Q2_set<<"z "<<z<<std::endl;
          auto runjs = iz.value();
          //if(!pos_D2.empty() && !neg_D2.empty()){
          int RunGroup = runjs["group_num"].get<int>();
          std::cout<<"RunGroup "<<RunGroup<<std::endl;
          std::vector<int> neg_D2,pos_D2;
          neg_D2 = runjs["neg"]["D2"].get<std::vector<int>>();
          pos_D2 = runjs["pos"]["D2"].get<std::vector<int>>();
          double shms_p = 3;
          double shms_dp = 0;
          double zhad = 1;
          double xbj = 1;
          //calculate charge
          double neg_charge = 0;
          for(auto it_neg = neg_D2.begin();it_neg!=neg_D2.end();++it_neg){
            std::string RunNumber_str = std::to_string(*it_neg);
            double charge = j_runsinfo[RunNumber_str.c_str()]["charge"].get<double>();
            std::cout<<" neg charge "<<charge<<std::endl;
            neg_charge = neg_charge+ charge;
          }
          double pos_charge = 0;
          for(auto it_pos = pos_D2.begin();it_pos!=pos_D2.end();++it_pos){
            std::string RunNumber_str = std::to_string(*it_pos);
            double charge = j_runsinfo[RunNumber_str.c_str()]["charge"].get<double>();
            std::cout<<" pos charge "<<charge<<std::endl;
            pos_charge = pos_charge+ charge;
          }
          if(shms_p>2.8){
            //if(1.25*shms_p>2.85){
            json j_rf_HGC;
            {
              std::string name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_4_HGC.json";
              std::ifstream ifstream(name.c_str());
              ifstream>>j_rf_HGC;
            }
            std::cout<<"check"<<std::endl;
            auto point_js = j_rf_HGC[(std::to_string(RunGroup)).c_str()];
            for(auto ik = point_js.begin();ik!=point_js.end();++ik){
              auto point_jsi = ik.value();
              shms_p = point_jsi["shms_p"].get<double>();
              shms_dp = point_jsi["shms_dp"].get<double>();
              std::cout<<"check 1 "<<std::endl;
              zhad = point_jsi["z_ave"].get<double>();
              xbj = point_jsi["xbj_ave"].get<double>();
              double N_pion_hgc_pos = point_jsi["pos"]["pi_HGC_all"].get<double>();
              double N_pion_hgc_neg = point_jsi["neg"]["pi_HGC_all"].get<double>();
              double N_pion_antiHGC_pos = point_jsi["pos"]["pi_antiHGC_all"].get<double>();
              double N_pion_antiHGC_neg = point_jsi["neg"]["pi_antiHGC_all"].get<double>();
              double N_Kaon_antiHGC_pos = point_jsi["pos"]["K_antiHGC_all"].get<double>();
              double N_Kaon_antiHGC_neg = point_jsi["neg"]["K_antiHGC_all"].get<double>();

              double chi2_pos = point_jsi["pos"]["chi2"].get<double>();
              double DoF_pos = point_jsi["pos"]["DoF"].get<double>();
              double chi2_neg = point_jsi["neg"]["chi2"].get<double>();
              double DoF_neg = point_jsi["neg"]["DoF"].get<double>();
              double chi2_pos_point = chi2_pos/DoF_pos;
              double chi2_neg_point = chi2_neg/DoF_neg;
              std::cout<<"chi2 pos "<<chi2_pos_point<<"chi2 neg "<<chi2_neg_point<<std::endl;

              double HGC_eff_pos = N_pion_hgc_pos/(N_pion_hgc_pos+N_pion_antiHGC_pos);
              double HGC_eff_neg = N_pion_hgc_neg/(N_pion_hgc_neg+N_pion_antiHGC_neg);
              double R_Kpi_ratio_pos = N_Kaon_antiHGC_pos/(N_pion_hgc_pos+N_pion_antiHGC_pos); 
              double R_Kpi_ratio_neg = N_Kaon_antiHGC_neg/(N_pion_hgc_neg+N_pion_antiHGC_neg); 
              double K_pos_neg_ratio = (N_Kaon_antiHGC_neg/neg_charge)/(N_Kaon_antiHGC_pos/pos_charge); 
              double pi_pos_neg_ratio = (N_pion_hgc_neg/neg_charge)/(N_pion_hgc_pos/pos_charge);

              double HGC_eff_pos_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos- N_pion_hgc_pos)/(N_pion_hgc_pos+N_pion_antiHGC_pos);
              double HGC_eff_neg_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg- N_pion_hgc_neg)/(N_pion_hgc_neg+N_pion_antiHGC_neg);
              double Kpi_ratio_pos_Ks_err = std::sqrt(N_Kaon_antiHGC_pos);        
              double Kpi_ratio_pos_pions_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos);
              double R_Kpi_ratio_pos_err = R_Kpi_ratio_pos*std::sqrt(std::pow(Kpi_ratio_pos_Ks_err/N_Kaon_antiHGC_pos,2)+std::pow(Kpi_ratio_pos_pions_err/(N_pion_hgc_pos+N_pion_antiHGC_pos),2));
              double Kpi_ratio_neg_Ks_err = std::sqrt(N_Kaon_antiHGC_neg);        
              double Kpi_ratio_neg_pions_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg);
              double R_Kpi_ratio_neg_err = R_Kpi_ratio_neg*std::sqrt(std::pow(Kpi_ratio_neg_Ks_err/N_Kaon_antiHGC_neg,2)+std::pow(Kpi_ratio_neg_pions_err/(N_pion_hgc_neg+N_pion_antiHGC_neg),2));
              //double R_Kpi_ratio_pos_err = std::sqrt(N_pion_hgc_pos+N_pion_antiHGC_pos- N_Kaon_antiHGC_pos)/(N_pion_hgc_pos+N_pion_antiHGC_pos); 
              //double R_Kpi_ratio_neg_err = std::sqrt(N_pion_hgc_neg+N_pion_antiHGC_neg- N_Kaon_antiHGC_neg)/(N_pion_hgc_neg+N_pion_antiHGC_neg); 
              double K_pos_err = std::sqrt(N_Kaon_antiHGC_pos)/pos_charge;
              double K_neg_err = std::sqrt(N_Kaon_antiHGC_neg)/neg_charge;
              double pi_pos_err = std::sqrt(N_pion_hgc_pos)/pos_charge;
              double pi_neg_err = std::sqrt(N_pion_hgc_neg)/neg_charge;
              double K_pos_neg_ratio_err = K_pos_neg_ratio*std::sqrt(std::pow((K_pos_err/(N_Kaon_antiHGC_pos/pos_charge)),2)+std::pow(K_neg_err/(N_Kaon_antiHGC_neg/neg_charge),2));
              double pi_pos_neg_ratio_err = pi_pos_neg_ratio*std::sqrt(std::pow((pi_pos_err/(N_pion_hgc_pos/pos_charge)),2)+std::pow(pi_neg_err/(N_pion_hgc_neg/neg_charge),2));
              //if(pi_pos_neg_ratio>2) std::cout<<"K+/K- ratio greater than 2 "<<RunGroup<<std::endl;
              //if(chi2_pos_point<50 && chi2_neg_point<50 && xbj>0.5){
              //if(chi2_pos_point<2 && chi2_neg_point<2 && xbj<0.5 ){
              if(chi2_pos_point<2 && chi2_neg_point<2 && xbj>xbj_set ){
              
                std::cout<< "if less than 0.5"<<std::endl;
                file_name = "Q2_"+(std::to_string(1000*Q2_set)).substr(0,4)+"_xbj_"+(std::to_string(100*xbj_set)).substr(0,2)+"_largex";
                //file_name = "Q2_"+(std::to_string(1000*Q2_set)).substr(0,4)+"_xbj_"+(std::to_string(100*xbj_set)).substr(0,2)+"_smallx";
                if(RunGroup<410 && HGC_eff_pos_err<0.2 && shms_p>2.9){
                  g_pos_zhad_fall_HGCeff->SetPoint(i_fall,zhad,HGC_eff_pos);
                  g_pos_zhad_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
                  g_pos_xbj_fall_HGCeff->SetPoint(i_fall,xbj,HGC_eff_pos);
                  g_pos_xbj_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
                  g_pos_shmsp_fall_HGCeff->SetPoint(i_fall,shms_p,HGC_eff_pos);
                  g_pos_shmsp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
                  g_pos_shmsdp_fall_HGCeff->SetPoint(i_fall,shms_dp,HGC_eff_pos);
                  g_pos_shmsdp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_pos_err);
                  g_neg_zhad_fall_HGCeff->SetPoint(i_fall,zhad,HGC_eff_neg);
                  g_neg_zhad_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
                  g_neg_xbj_fall_HGCeff->SetPoint(i_fall,xbj,HGC_eff_neg);
                  g_neg_xbj_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
                  g_neg_shmsp_fall_HGCeff->SetPoint(i_fall,shms_p,HGC_eff_neg);
                  g_neg_shmsp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
                  g_neg_shmsdp_fall_HGCeff->SetPoint(i_fall,shms_dp,HGC_eff_neg);
                  g_neg_shmsdp_fall_HGCeff->SetPointError(i_fall,0,HGC_eff_neg_err);
                  g_pos_zhad_fall_Kpiratio->SetPoint(i_fall,zhad,R_Kpi_ratio_pos);
                  g_pos_zhad_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
                  g_pos_xbj_fall_Kpiratio->SetPoint(i_fall,xbj,R_Kpi_ratio_pos);
                  g_pos_xbj_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
                  g_pos_shmsp_fall_Kpiratio->SetPoint(i_fall,shms_p,R_Kpi_ratio_pos);
                  g_pos_shmsp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
                  g_pos_shmsdp_fall_Kpiratio->SetPoint(i_fall,shms_dp,R_Kpi_ratio_pos);
                  g_pos_shmsdp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_pos_err);
                  g_neg_zhad_fall_Kpiratio->SetPoint(i_fall,zhad,R_Kpi_ratio_neg);
                  g_neg_zhad_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
                  g_neg_xbj_fall_Kpiratio->SetPoint(i_fall,xbj,R_Kpi_ratio_neg);
                  g_neg_xbj_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
                  g_neg_shmsp_fall_Kpiratio->SetPoint(i_fall,shms_p,R_Kpi_ratio_neg);
                  g_neg_shmsp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
                  g_neg_shmsdp_fall_Kpiratio->SetPoint(i_fall,shms_dp,R_Kpi_ratio_neg);
                  g_neg_shmsdp_fall_Kpiratio->SetPointError(i_fall,0,R_Kpi_ratio_neg_err);
                  g_zhad_fall_Kratio->SetPoint(i_fall,zhad,K_pos_neg_ratio);
                  g_zhad_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
                  g_xbj_fall_Kratio->SetPoint(i_fall,xbj,K_pos_neg_ratio);
                  g_xbj_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
                  g_shmsp_fall_Kratio->SetPoint(i_fall,shms_p,K_pos_neg_ratio);
                  g_shmsp_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
                  g_shmsdp_fall_Kratio->SetPoint(i_fall,shms_dp,K_pos_neg_ratio);
                  g_shmsdp_fall_Kratio->SetPointError(i_fall,0,K_pos_neg_ratio_err);
                  g_zhad_fall_piratio->SetPoint(i_fall,zhad,pi_pos_neg_ratio);
                  g_zhad_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
                  g_xbj_fall_piratio->SetPoint(i_fall,xbj,pi_pos_neg_ratio);
                  g_xbj_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
                  g_shmsp_fall_piratio->SetPoint(i_fall,shms_p,pi_pos_neg_ratio);
                  g_shmsp_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
                  g_shmsdp_fall_piratio->SetPoint(i_fall,shms_dp,pi_pos_neg_ratio);
                  g_shmsdp_fall_piratio->SetPointError(i_fall,0,pi_pos_neg_ratio_err);
                  i_fall++;
                  g_pos_zhad_Kpiratio->SetPoint(i_all,zhad,R_Kpi_ratio_pos);
                  g_pos_zhad_Kpiratio->SetPointError(i_all,0,R_Kpi_ratio_pos_err);
                  i_all++;
                }//for fall runs
                else if(HGC_eff_pos_err<0.2 && shms_p>2.9){
                  g_pos_zhad_spring_HGCeff->SetPoint(i_spring,zhad,HGC_eff_pos);
                  g_pos_zhad_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
                  g_pos_xbj_spring_HGCeff->SetPoint(i_spring,xbj,HGC_eff_pos);
                  g_pos_xbj_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
                  g_pos_shmsp_spring_HGCeff->SetPoint(i_spring,shms_p,HGC_eff_pos);
                  g_pos_shmsp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
                  g_pos_shmsdp_spring_HGCeff->SetPoint(i_spring,shms_dp,HGC_eff_pos);
                  g_pos_shmsdp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_pos_err);
                  g_neg_zhad_spring_HGCeff->SetPoint(i_spring,zhad,HGC_eff_neg);
                  g_neg_zhad_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
                  g_neg_xbj_spring_HGCeff->SetPoint(i_spring,xbj,HGC_eff_neg);
                  g_neg_xbj_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
                  g_neg_shmsp_spring_HGCeff->SetPoint(i_spring,shms_p,HGC_eff_neg);
                  g_neg_shmsp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
                  g_neg_shmsdp_spring_HGCeff->SetPoint(i_spring,shms_dp,HGC_eff_neg);
                  g_neg_shmsdp_spring_HGCeff->SetPointError(i_spring,0,HGC_eff_neg_err);
                  g_pos_zhad_spring_Kpiratio->SetPoint(i_spring,zhad,R_Kpi_ratio_pos);
                  g_pos_zhad_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
                  g_pos_xbj_spring_Kpiratio->SetPoint(i_spring,xbj,R_Kpi_ratio_pos);
                  g_pos_xbj_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
                  g_pos_shmsp_spring_Kpiratio->SetPoint(i_spring,shms_p,R_Kpi_ratio_pos);
                  g_pos_shmsp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
                  g_pos_shmsdp_spring_Kpiratio->SetPoint(i_spring,shms_dp,R_Kpi_ratio_pos);
                  g_pos_shmsdp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_pos_err);
                  g_neg_zhad_spring_Kpiratio->SetPoint(i_spring,zhad,R_Kpi_ratio_neg);
                  g_neg_zhad_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
                  g_neg_xbj_spring_Kpiratio->SetPoint(i_spring,xbj,R_Kpi_ratio_neg);
                  g_neg_xbj_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
                  g_neg_shmsp_spring_Kpiratio->SetPoint(i_spring,shms_p,R_Kpi_ratio_neg);
                  g_neg_shmsp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
                  g_neg_shmsdp_spring_Kpiratio->SetPoint(i_spring,shms_dp,R_Kpi_ratio_neg);
                  g_neg_shmsdp_spring_Kpiratio->SetPointError(i_spring,0,R_Kpi_ratio_neg_err);
                  g_zhad_spring_Kratio->SetPoint(i_spring,zhad,K_pos_neg_ratio);
                  g_zhad_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
                  g_xbj_spring_Kratio->SetPoint(i_spring,xbj,K_pos_neg_ratio);
                  g_xbj_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
                  g_shmsp_spring_Kratio->SetPoint(i_spring,shms_p,K_pos_neg_ratio);
                  g_shmsp_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
                  g_shmsdp_spring_Kratio->SetPoint(i_spring,shms_dp,K_pos_neg_ratio);
                  g_shmsdp_spring_Kratio->SetPointError(i_spring,0,K_pos_neg_ratio_err);
                  g_zhad_spring_piratio->SetPoint(i_spring,zhad,pi_pos_neg_ratio);
                  g_zhad_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
                  g_xbj_spring_piratio->SetPoint(i_spring,xbj,pi_pos_neg_ratio);
                  g_xbj_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
                  g_shmsp_spring_piratio->SetPoint(i_spring,shms_p,pi_pos_neg_ratio);
                  g_shmsp_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
                  g_shmsdp_spring_piratio->SetPoint(i_spring,shms_dp,pi_pos_neg_ratio);
                  g_shmsdp_spring_piratio->SetPointError(i_spring,0,pi_pos_neg_ratio_err);
                  i_spring++;
                  g_pos_zhad_Kpiratio->SetPoint(i_all,zhad,R_Kpi_ratio_pos);
                  g_pos_zhad_Kpiratio->SetPointError(i_all,0,R_Kpi_ratio_pos_err);
                  i_all++;
                }//for spring runs
              }//if xbj >0.5/<0.5
            }//loop over points
          }//if greater than 3
          }//loop over z

          TCanvas *c_HGCeff_zhad = new TCanvas();
          g_pos_zhad_fall_HGCeff->SetMarkerStyle(8);
          g_pos_zhad_fall_HGCeff->SetMarkerColor(kRed);
          g_pos_zhad_fall_HGCeff->SetTitle("pos fall");
          g_pos_zhad_spring_HGCeff->SetMarkerStyle(4);
          g_pos_zhad_spring_HGCeff->SetMarkerColor(kRed);
          g_pos_zhad_spring_HGCeff->SetTitle("pos spring");
          g_neg_zhad_fall_HGCeff->SetMarkerStyle(8);
          g_neg_zhad_fall_HGCeff->SetMarkerColor(kBlack);
          g_neg_zhad_fall_HGCeff->SetTitle("neg fall");
          g_neg_zhad_spring_HGCeff->SetMarkerStyle(4);
          g_neg_zhad_spring_HGCeff->SetMarkerColor(kBlack);
          g_neg_zhad_spring_HGCeff->SetTitle("neg spring");
          TMultiGraph* mg_zhad_HGCeff = new TMultiGraph();
          mg_zhad_HGCeff->Add(g_pos_zhad_fall_HGCeff,"P");
          mg_zhad_HGCeff->Add(g_pos_zhad_spring_HGCeff,"P");
          mg_zhad_HGCeff->Add(g_neg_zhad_fall_HGCeff,"P");
          mg_zhad_HGCeff->Add(g_neg_zhad_spring_HGCeff,"P");
          mg_zhad_HGCeff->Draw("A");
          mg_zhad_HGCeff->GetXaxis()->SetTitle("zhad");
          mg_zhad_HGCeff->GetYaxis()->SetTitle("HGC_eff");
          mg_zhad_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
          c_HGCeff_zhad->BuildLegend();
          std::string c_HGCeff_zhad_name = "results/pid/HGC_pid/HGCeff_zhad_"+file_name+".pdf";
          c_HGCeff_zhad->SaveAs(c_HGCeff_zhad_name.c_str());
          TCanvas *c_HGCeff_xbj = new TCanvas();
          g_pos_xbj_fall_HGCeff->SetMarkerStyle(8);
          g_pos_xbj_fall_HGCeff->SetMarkerColor(kRed);
          g_pos_xbj_fall_HGCeff->SetTitle("pos fall");
          g_pos_xbj_spring_HGCeff->SetMarkerStyle(4);
          g_pos_xbj_spring_HGCeff->SetMarkerColor(kRed);
          g_pos_xbj_spring_HGCeff->SetTitle("pos spring");
          g_neg_xbj_fall_HGCeff->SetMarkerStyle(8);
          g_neg_xbj_fall_HGCeff->SetMarkerColor(kBlack);
          g_neg_xbj_fall_HGCeff->SetTitle("neg fall");
          g_neg_xbj_spring_HGCeff->SetMarkerStyle(4);
          g_neg_xbj_spring_HGCeff->SetMarkerColor(kBlack);
          g_neg_xbj_spring_HGCeff->SetTitle("neg spring");
          TMultiGraph* mg_xbj_HGCeff = new TMultiGraph();
          mg_xbj_HGCeff->Add(g_pos_xbj_fall_HGCeff,"P");
          mg_xbj_HGCeff->Add(g_pos_xbj_spring_HGCeff,"P");
          mg_xbj_HGCeff->Add(g_neg_xbj_fall_HGCeff,"P");
          mg_xbj_HGCeff->Add(g_neg_xbj_spring_HGCeff,"P");
          mg_xbj_HGCeff->Draw("A");
          mg_xbj_HGCeff->GetXaxis()->SetTitle("xbj");
          mg_xbj_HGCeff->GetYaxis()->SetTitle("HGC_eff");
          mg_xbj_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
          c_HGCeff_xbj->BuildLegend();
          std::string c_HGCeff_xbj_name = (("results/pid/HGC_pid/HGCeff_xbj_"+file_name+".pdf").c_str());
          c_HGCeff_xbj->SaveAs(c_HGCeff_xbj_name.c_str());
          TCanvas *c_HGCeff_shmsp = new TCanvas();
          g_pos_shmsp_fall_HGCeff->SetMarkerStyle(8);
          g_pos_shmsp_fall_HGCeff->SetMarkerColor(kRed);
          g_pos_shmsp_fall_HGCeff->SetTitle("pos fall");
          g_pos_shmsp_spring_HGCeff->SetMarkerStyle(4);
          g_pos_shmsp_spring_HGCeff->SetMarkerColor(kRed);
          g_pos_shmsp_spring_HGCeff->SetTitle("pos spring");
          g_neg_shmsp_fall_HGCeff->SetMarkerStyle(8);
          g_neg_shmsp_fall_HGCeff->SetMarkerColor(kBlack);
          g_neg_shmsp_fall_HGCeff->SetTitle("neg fall");
          g_neg_shmsp_spring_HGCeff->SetMarkerStyle(4);
          g_neg_shmsp_spring_HGCeff->SetMarkerColor(kBlack);
          g_neg_shmsp_spring_HGCeff->SetTitle("neg spring");
          TMultiGraph* mg_shmsp_HGCeff = new TMultiGraph();
          mg_shmsp_HGCeff->Add(g_pos_shmsp_fall_HGCeff,"P");
          mg_shmsp_HGCeff->Add(g_pos_shmsp_spring_HGCeff,"P");
          mg_shmsp_HGCeff->Add(g_neg_shmsp_fall_HGCeff,"P");
          mg_shmsp_HGCeff->Add(g_neg_shmsp_spring_HGCeff,"P");
          mg_shmsp_HGCeff->Draw("A");
          mg_shmsp_HGCeff->GetXaxis()->SetTitle("shmsp");
          mg_shmsp_HGCeff->GetYaxis()->SetTitle("HGC_eff");
          //mg_shmsp_HGCeff->GetXaxis()->SetRangeUser(2.8,4.5);
          mg_shmsp_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
          c_HGCeff_shmsp->BuildLegend();
          std::string c_HGCeff_shmsp_name = (("results/pid/HGC_pid/HGCeff_shmsp_"+file_name+".pdf").c_str());
          c_HGCeff_shmsp->SaveAs(c_HGCeff_shmsp_name.c_str());
          TCanvas *c_HGCeff_shmsdp = new TCanvas();
          g_pos_shmsdp_fall_HGCeff->SetMarkerStyle(8);
          g_pos_shmsdp_fall_HGCeff->SetMarkerColor(kRed);
          g_pos_shmsdp_fall_HGCeff->SetTitle("pos fall");
          g_pos_shmsdp_spring_HGCeff->SetMarkerStyle(4);
          g_pos_shmsdp_spring_HGCeff->SetMarkerColor(kRed);
          g_pos_shmsdp_spring_HGCeff->SetTitle("pos spring");
          g_neg_shmsdp_fall_HGCeff->SetMarkerStyle(8);
          g_neg_shmsdp_fall_HGCeff->SetMarkerColor(kBlack);
          g_neg_shmsdp_fall_HGCeff->SetTitle("neg fall");
          g_neg_shmsdp_spring_HGCeff->SetMarkerStyle(4);
          g_neg_shmsdp_spring_HGCeff->SetMarkerColor(kBlack);
          g_neg_shmsdp_spring_HGCeff->SetTitle("neg spring");
          TMultiGraph* mg_shmsdp_HGCeff = new TMultiGraph();
          mg_shmsdp_HGCeff->Add(g_pos_shmsdp_fall_HGCeff,"P");
          mg_shmsdp_HGCeff->Add(g_pos_shmsdp_spring_HGCeff,"P");
          mg_shmsdp_HGCeff->Add(g_neg_shmsdp_fall_HGCeff,"P");
          mg_shmsdp_HGCeff->Add(g_neg_shmsdp_spring_HGCeff,"P");
          mg_shmsdp_HGCeff->Draw("A");
          mg_shmsdp_HGCeff->GetXaxis()->SetTitle("shmsdp");
          mg_shmsdp_HGCeff->GetYaxis()->SetTitle("HGC_eff");
          //mg_shmsdp_HGCeff->GetXaxis()->SetRangeUser(2.8,4.5);
          mg_shmsdp_HGCeff->GetYaxis()->SetRangeUser(0.7,1);
          c_HGCeff_shmsdp->BuildLegend();
          c_HGCeff_shmsdp->SaveAs(("results/pid/HGC_pid/HGCeff_shmsdp_"+file_name+".pdf").c_str());
          
          //Kpi ratio as a function of z
          TCanvas *c_Kpiratio_zhad = new TCanvas();
          gStyle->SetOptFit(0001);
          g_pos_zhad_fall_Kpiratio->SetMarkerStyle(8);
          g_pos_zhad_fall_Kpiratio->SetMarkerColor(kRed);
          g_pos_zhad_fall_Kpiratio->SetTitle("pos fall");
          g_pos_zhad_spring_Kpiratio->SetMarkerStyle(4);
          g_pos_zhad_spring_Kpiratio->SetMarkerColor(kRed);
          g_pos_zhad_spring_Kpiratio->SetTitle("pos spring");
          g_neg_zhad_fall_Kpiratio->SetMarkerStyle(8);
          g_neg_zhad_fall_Kpiratio->SetMarkerColor(kBlack);
          g_neg_zhad_fall_Kpiratio->SetTitle("neg fall");
          g_neg_zhad_spring_Kpiratio->SetMarkerStyle(4);
          g_neg_zhad_spring_Kpiratio->SetMarkerColor(kBlack);
          g_neg_zhad_spring_Kpiratio->SetTitle("neg spring");
          TMultiGraph* mg_zhad_Kpiratio = new TMultiGraph();
          mg_zhad_Kpiratio->Add(g_pos_zhad_fall_Kpiratio,"P");
          mg_zhad_Kpiratio->Add(g_pos_zhad_spring_Kpiratio,"P");
          mg_zhad_Kpiratio->Draw("A");
          
          
          mg_zhad_Kpiratio->Fit("pol1");
          double p0 = 0,p1 = 0;
          if(i_all!=0){
              TF1* fit_kpiratio = mg_zhad_Kpiratio->GetFunction("pol1");
              p0 = fit_kpiratio->GetParameter(0);
              p1 = fit_kpiratio->GetParameter(1);
              }
          //mg_zhad_Kpiratio->Add(g_neg_zhad_fall_Kpiratio,"P");
          //mg_zhad_Kpiratio->Add(g_neg_zhad_spring_Kpiratio,"P");
          mg_zhad_Kpiratio->GetXaxis()->SetTitle("z");
          mg_zhad_Kpiratio->GetYaxis()->SetTitle("K/pi");
          mg_zhad_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
          c_Kpiratio_zhad->BuildLegend(0.15,0.65,0.35,0.9);
          c_Kpiratio_zhad->SaveAs(("results/pid/HGC_pid/Kpiratio_zhad_"+file_name+".pdf").c_str());
         
          TCanvas *c_Kpiratio_zhad_neg = new TCanvas();
          TMultiGraph* mg_zhad_Kpiratio_neg = new TMultiGraph();
          mg_zhad_Kpiratio_neg->Add(g_neg_zhad_fall_Kpiratio,"P");
          mg_zhad_Kpiratio_neg->Add(g_neg_zhad_spring_Kpiratio,"P");
          mg_zhad_Kpiratio_neg->Fit("pol1");
          double p0_neg = 0,p1_neg = 0;
          if(i_all!=0){
              TF1* fit_kpiratio_neg = mg_zhad_Kpiratio_neg->GetFunction("pol1");
              p0_neg = fit_kpiratio_neg->GetParameter(0);
              p1_neg = fit_kpiratio_neg->GetParameter(1);
              }
          //TF1* fit_neg = new TF1("fit_neg","fit_neg",2.8,5,3);
          //fit_neg->SetParameters(p0,p1,0.25);
          //fit_neg->FixParameter(0,p0);
          //fit_neg->FixParameter(1,p1);
          //mg_zhad_Kpiratio_neg->Fit(fit_neg,"r");
          mg_zhad_Kpiratio_neg->Draw("A");
          mg_zhad_Kpiratio_neg->GetXaxis()->SetTitle("z");
          mg_zhad_Kpiratio_neg->GetYaxis()->SetTitle("K/pi");
          mg_zhad_Kpiratio_neg->GetYaxis()->SetRangeUser(0,0.16);
          c_Kpiratio_zhad_neg->BuildLegend(0.15,0.65,0.35,0.9);
          //fit_neg->Draw("same");
          c_Kpiratio_zhad_neg->SaveAs(("results/pid/HGC_pid/Kpiratio_zhad_"+file_name+"_neg.pdf").c_str());


          TCanvas *c_Kpiratio_xbj = new TCanvas();
          g_pos_xbj_fall_Kpiratio->SetMarkerStyle(8);
          g_pos_xbj_fall_Kpiratio->SetMarkerColor(kRed);
          g_pos_xbj_fall_Kpiratio->SetTitle("pos fall");
          g_pos_xbj_spring_Kpiratio->SetMarkerStyle(4);
          g_pos_xbj_spring_Kpiratio->SetMarkerColor(kRed);
          g_pos_xbj_spring_Kpiratio->SetTitle("pos spring");
          g_neg_xbj_fall_Kpiratio->SetMarkerStyle(8);
          g_neg_xbj_fall_Kpiratio->SetMarkerColor(kBlack);
          g_neg_xbj_fall_Kpiratio->SetTitle("neg fall");
          g_neg_xbj_spring_Kpiratio->SetMarkerStyle(4);
          g_neg_xbj_spring_Kpiratio->SetMarkerColor(kBlack);
          g_neg_xbj_spring_Kpiratio->SetTitle("neg spring");
          TMultiGraph* mg_xbj_pos_Kpiratio = new TMultiGraph();
          mg_xbj_pos_Kpiratio->Add(g_pos_xbj_fall_Kpiratio,"P");
          mg_xbj_pos_Kpiratio->Add(g_pos_xbj_spring_Kpiratio,"P");
          mg_xbj_pos_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          mg_xbj_pos_Kpiratio->Draw("same");
          TMultiGraph* mg_xbj_Kpiratio = new TMultiGraph();
          mg_xbj_Kpiratio->Add(g_pos_xbj_fall_Kpiratio,"P");
          mg_xbj_Kpiratio->Add(g_pos_xbj_spring_Kpiratio,"P");
          mg_xbj_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          //mg_xbj_Kpiratio->Draw("same");
          mg_xbj_Kpiratio->Add(g_neg_xbj_fall_Kpiratio,"P");
          mg_xbj_Kpiratio->Add(g_neg_xbj_spring_Kpiratio,"P");
          mg_xbj_Kpiratio->Draw("A");
          mg_xbj_Kpiratio->GetXaxis()->SetTitle("xbj");
          mg_xbj_Kpiratio->GetYaxis()->SetTitle("K/pi");
          mg_xbj_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
          c_Kpiratio_xbj->BuildLegend(0.15,0.65,0.35,0.9);
          c_Kpiratio_xbj->SaveAs(("results/pid/HGC_pid/Kpiratio_xbj_"+file_name+".pdf").c_str());
          TCanvas *c_Kpiratio_shmsp = new TCanvas();
          g_pos_shmsp_fall_Kpiratio->SetMarkerStyle(8);
          g_pos_shmsp_fall_Kpiratio->SetMarkerColor(kRed);
          g_pos_shmsp_fall_Kpiratio->SetTitle("pos fall");
          g_pos_shmsp_spring_Kpiratio->SetMarkerStyle(4);
          g_pos_shmsp_spring_Kpiratio->SetMarkerColor(kRed);
          g_pos_shmsp_spring_Kpiratio->SetTitle("pos spring");
          g_neg_shmsp_fall_Kpiratio->SetMarkerStyle(8);
          g_neg_shmsp_fall_Kpiratio->SetMarkerColor(kBlack);
          g_neg_shmsp_fall_Kpiratio->SetTitle("neg fall");
          g_neg_shmsp_spring_Kpiratio->SetMarkerStyle(4);
          g_neg_shmsp_spring_Kpiratio->SetMarkerColor(kBlack);
          g_neg_shmsp_spring_Kpiratio->SetTitle("neg spring");
          TMultiGraph* mg_shmsp_pos_Kpiratio = new TMultiGraph();
          mg_shmsp_pos_Kpiratio->Add(g_pos_shmsp_fall_Kpiratio,"P");
          mg_shmsp_pos_Kpiratio->Add(g_pos_shmsp_spring_Kpiratio,"P");
          mg_shmsp_pos_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          mg_shmsp_pos_Kpiratio->Draw("same");
          TMultiGraph* mg_shmsp_Kpiratio = new TMultiGraph();
          mg_shmsp_Kpiratio->Add(g_pos_shmsp_fall_Kpiratio,"P");
          mg_shmsp_Kpiratio->Add(g_pos_shmsp_spring_Kpiratio,"P");
          mg_shmsp_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          //TF1 *f1_Kpiratio =0wi 
          mg_shmsp_Kpiratio->Add(g_neg_shmsp_fall_Kpiratio,"P");
          mg_shmsp_Kpiratio->Add(g_neg_shmsp_spring_Kpiratio,"P");
          mg_shmsp_Kpiratio->Draw("A");
          mg_shmsp_Kpiratio->GetXaxis()->SetTitle("shmsp");
          //mg_shmsp_Kpiratio->GetXaxis()->SetRangeUser(2.8,4.5);
          mg_shmsp_Kpiratio->GetYaxis()->SetTitle("K/pi");
          mg_shmsp_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
          c_Kpiratio_shmsp->BuildLegend(0.15,0.65,0.35,0.9);
          c_Kpiratio_shmsp->SaveAs(("results/pid/HGC_pid/Kpiratio_shmsp_"+file_name+".pdf").c_str());
          TCanvas *c_Kpiratio_shmsdp = new TCanvas();
          g_pos_shmsdp_fall_Kpiratio->SetMarkerStyle(8);
          g_pos_shmsdp_fall_Kpiratio->SetMarkerColor(kRed);
          g_pos_shmsdp_fall_Kpiratio->SetTitle("pos fall");
          g_pos_shmsdp_spring_Kpiratio->SetMarkerStyle(4);
          g_pos_shmsdp_spring_Kpiratio->SetMarkerColor(kRed);
          g_pos_shmsdp_spring_Kpiratio->SetTitle("pos spring");
          g_neg_shmsdp_fall_Kpiratio->SetMarkerStyle(8);
          g_neg_shmsdp_fall_Kpiratio->SetMarkerColor(kBlack);
          g_neg_shmsdp_fall_Kpiratio->SetTitle("neg fall");
          g_neg_shmsdp_spring_Kpiratio->SetMarkerStyle(4);
          g_neg_shmsdp_spring_Kpiratio->SetMarkerColor(kBlack);
          g_neg_shmsdp_spring_Kpiratio->SetTitle("neg spring");
          TMultiGraph* mg_shmsdp_pos_Kpiratio = new TMultiGraph();
          mg_shmsdp_pos_Kpiratio->Add(g_pos_shmsdp_fall_Kpiratio,"P");
          mg_shmsdp_pos_Kpiratio->Add(g_pos_shmsdp_spring_Kpiratio,"P");
          mg_shmsdp_pos_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          mg_shmsdp_pos_Kpiratio->Draw("same");
          TMultiGraph* mg_shmsdp_Kpiratio = new TMultiGraph();
          mg_shmsdp_Kpiratio->Add(g_pos_shmsdp_fall_Kpiratio,"P");
          mg_shmsdp_Kpiratio->Add(g_pos_shmsdp_spring_Kpiratio,"P");
          mg_shmsdp_Kpiratio->Fit("pol1");
          gStyle->SetOptFit(0001);
          //TF1 *f1_Kpiratio =0wi 
          mg_shmsdp_Kpiratio->Add(g_neg_shmsdp_fall_Kpiratio,"P");
          mg_shmsdp_Kpiratio->Add(g_neg_shmsdp_spring_Kpiratio,"P");
          mg_shmsdp_Kpiratio->Draw("A");
          mg_shmsdp_Kpiratio->GetXaxis()->SetTitle("shmsdp");
          //mg_shmsdp_Kpiratio->GetXaxis()->SetRangeUser(2.8,4.5);
          mg_shmsdp_Kpiratio->GetYaxis()->SetTitle("K/pi");
          mg_shmsdp_Kpiratio->GetYaxis()->SetRangeUser(0,0.16);
          c_Kpiratio_shmsdp->BuildLegend(0.15,0.65,0.35,0.9);
          c_Kpiratio_shmsdp->SaveAs(("results/pid/HGC_pid/Kpiratio_shmsdp_"+file_name+".pdf").c_str());
          TCanvas *c_Kratio_zhad = new TCanvas();
          g_zhad_fall_Kratio->SetMarkerStyle(8);
          g_zhad_fall_Kratio->SetMarkerColor(kRed);
          g_zhad_fall_Kratio->SetTitle("fall");
          g_zhad_spring_Kratio->SetMarkerStyle(4);
          g_zhad_spring_Kratio->SetMarkerColor(kRed);
          g_zhad_spring_Kratio->SetTitle("spring");
          TMultiGraph* mg_zhad_Kratio = new TMultiGraph();
          mg_zhad_Kratio->Add(g_zhad_fall_Kratio,"P");
          mg_zhad_Kratio->Add(g_zhad_spring_Kratio,"P");
          mg_zhad_Kratio->Draw("A");
          mg_zhad_Kratio->GetXaxis()->SetTitle("zhad");
          mg_zhad_Kratio->GetYaxis()->SetTitle("K-/K+");
          //mg_zhad_Kratio->GetYaxis()->SetRangeUser(0,100);
          c_Kratio_zhad->BuildLegend();
          c_Kratio_zhad->SaveAs(("results/pid/HGC_pid/Kratio_zhad_"+file_name+".pdf").c_str());
          TCanvas *c_Kratio_xbj = new TCanvas();
          g_xbj_fall_Kratio->SetMarkerStyle(8);
          g_xbj_fall_Kratio->SetMarkerColor(kRed);
          g_xbj_fall_Kratio->SetTitle("fall");
          g_xbj_spring_Kratio->SetMarkerStyle(4);
          g_xbj_spring_Kratio->SetMarkerColor(kRed);
          g_xbj_spring_Kratio->SetTitle("spring");
          TMultiGraph* mg_xbj_Kratio = new TMultiGraph();
          mg_xbj_Kratio->Add(g_xbj_fall_Kratio,"P");
          mg_xbj_Kratio->Add(g_xbj_spring_Kratio,"P");
          mg_xbj_Kratio->Draw("A");
          mg_xbj_Kratio->GetXaxis()->SetTitle("xbj");
          mg_xbj_Kratio->GetYaxis()->SetTitle("K-/K+");
          //mg_xbj_Kratio->GetYaxis()->SetRangeUser(0,100);
          c_Kratio_xbj->BuildLegend();
          c_Kratio_xbj->SaveAs(("results/pid/HGC_pid/Kratio_xbj_"+file_name+".pdf").c_str());
          TCanvas *c_Kratio_shmsp = new TCanvas();
          g_shmsp_fall_Kratio->SetMarkerStyle(8);
          g_shmsp_fall_Kratio->SetMarkerColor(kRed);
          g_shmsp_fall_Kratio->SetTitle("fall");
          g_shmsp_spring_Kratio->SetMarkerStyle(4);
          g_shmsp_spring_Kratio->SetMarkerColor(kRed);
          g_shmsp_spring_Kratio->SetTitle("spring");
          TMultiGraph* mg_shmsp_Kratio = new TMultiGraph();
          mg_shmsp_Kratio->Add(g_shmsp_fall_Kratio,"P");
          mg_shmsp_Kratio->Add(g_shmsp_spring_Kratio,"P");
          mg_shmsp_Kratio->Draw("A");
          mg_shmsp_Kratio->GetXaxis()->SetTitle("shmsp");
          mg_shmsp_Kratio->GetYaxis()->SetTitle("K-/K+");
          //mg_shmsp_Kratio->GetYaxis()->SetRangeUser(0,1);
          c_Kratio_shmsp->BuildLegend();
          c_Kratio_shmsp->SaveAs(("results/pid/HGC_pid/Kratio_shmsp_"+file_name+".pdf").c_str());
          TCanvas *c_Kratio_shmsdp = new TCanvas();
          g_shmsdp_fall_Kratio->SetMarkerStyle(8);
          g_shmsdp_fall_Kratio->SetMarkerColor(kRed);
          g_shmsdp_fall_Kratio->SetTitle("fall");
          g_shmsdp_spring_Kratio->SetMarkerStyle(4);
          g_shmsdp_spring_Kratio->SetMarkerColor(kRed);
          g_shmsdp_spring_Kratio->SetTitle("spring");
          TMultiGraph* mg_shmsdp_Kratio = new TMultiGraph();
          mg_shmsdp_Kratio->Add(g_shmsdp_fall_Kratio,"P");
          mg_shmsdp_Kratio->Add(g_shmsdp_spring_Kratio,"P");
          mg_shmsdp_Kratio->Draw("A");
          mg_shmsdp_Kratio->GetXaxis()->SetTitle("shmsdp");
          mg_shmsdp_Kratio->GetYaxis()->SetTitle("K-/K+");
          //mg_shmsdp_Kratio->GetYaxis()->SetRangeUser(0,1);
          c_Kratio_shmsdp->BuildLegend();
          c_Kratio_shmsdp->SaveAs(("results/pid/HGC_pid/Kratio_shmsdp_"+file_name+".pdf").c_str());
          TCanvas *c_piratio_zhad = new TCanvas();
          g_zhad_fall_piratio->SetMarkerStyle(8);
          g_zhad_fall_piratio->SetMarkerColor(kRed);
          g_zhad_fall_piratio->SetTitle("fall");
          g_zhad_spring_piratio->SetMarkerStyle(4);
          g_zhad_spring_piratio->SetMarkerColor(kRed);
          g_zhad_spring_piratio->SetTitle("spring");
          TMultiGraph* mg_zhad_piratio = new TMultiGraph();
          mg_zhad_piratio->Add(g_zhad_fall_piratio,"P");
          mg_zhad_piratio->Add(g_zhad_spring_piratio,"P");
          mg_zhad_piratio->Draw("A");
          mg_zhad_piratio->GetXaxis()->SetTitle("zhad");
          mg_zhad_piratio->GetYaxis()->SetTitle("pi-/pi+");
          c_piratio_zhad->BuildLegend();
          c_piratio_zhad->SaveAs(("results/pid/HGC_pid/piratio_zhad_"+file_name+".pdf").c_str());
          TCanvas *c_piratio_xbj = new TCanvas();
          g_xbj_fall_piratio->SetMarkerStyle(8);
          g_xbj_fall_piratio->SetMarkerColor(kRed);
          g_xbj_fall_piratio->SetTitle("fall");
          g_xbj_spring_piratio->SetMarkerStyle(4);
          g_xbj_spring_piratio->SetMarkerColor(kRed);
          g_xbj_spring_piratio->SetTitle("spring");
          TMultiGraph* mg_xbj_piratio = new TMultiGraph();
          mg_xbj_piratio->Add(g_xbj_fall_piratio,"P");
          mg_xbj_piratio->Add(g_xbj_spring_piratio,"P");
          mg_xbj_piratio->Draw("A");
          mg_xbj_piratio->GetXaxis()->SetTitle("xbj");
          mg_xbj_piratio->GetYaxis()->SetTitle("pi-/pi+");
          c_piratio_xbj->BuildLegend();
          c_piratio_xbj->SaveAs(("results/pid/HGC_pid/piratio_xbj_"+file_name+".pdf").c_str());
          TCanvas *c_piratio_shmsp = new TCanvas();
          g_shmsp_fall_piratio->SetMarkerStyle(8);
          g_shmsp_fall_piratio->SetMarkerColor(kRed);
          g_shmsp_fall_piratio->SetTitle("fall");
          g_shmsp_spring_piratio->SetMarkerStyle(4);
          g_shmsp_spring_piratio->SetMarkerColor(kRed);
          g_shmsp_spring_piratio->SetTitle("spring");
          TMultiGraph* mg_shmsp_piratio = new TMultiGraph();
          mg_shmsp_piratio->Add(g_shmsp_fall_piratio,"P");
          mg_shmsp_piratio->Add(g_shmsp_spring_piratio,"P");
          mg_shmsp_piratio->Draw("A");
          mg_shmsp_piratio->GetXaxis()->SetTitle("shmsp");
          mg_shmsp_piratio->GetYaxis()->SetTitle("pi-/pi+");
          mg_shmsp_piratio->GetYaxis()->SetRangeUser(0,1);
          c_piratio_shmsp->BuildLegend();
          c_piratio_shmsp->SaveAs(("results/pid/HGC_pid/piratio_shmsp_"+file_name+".pdf").c_str());
          TCanvas *c_piratio_shmsdp = new TCanvas();
          g_shmsdp_fall_piratio->SetMarkerStyle(8);
          g_shmsdp_fall_piratio->SetMarkerColor(kRed);
          g_shmsdp_fall_piratio->SetTitle("fall");
          g_shmsdp_spring_piratio->SetMarkerStyle(4);
          g_shmsdp_spring_piratio->SetMarkerColor(kRed);
          g_shmsdp_spring_piratio->SetTitle("spring");
          TMultiGraph* mg_shmsdp_piratio = new TMultiGraph();
          mg_shmsdp_piratio->Add(g_shmsdp_fall_piratio,"P");
          mg_shmsdp_piratio->Add(g_shmsdp_spring_piratio,"P");
          mg_shmsdp_piratio->Draw("A");
          mg_shmsdp_piratio->GetXaxis()->SetTitle("shmsdp");
          mg_shmsdp_piratio->GetYaxis()->SetTitle("pi-/pi+");
          mg_shmsdp_piratio->GetYaxis()->SetRangeUser(0,1);
          c_piratio_shmsdp->BuildLegend();
          c_piratio_shmsdp->SaveAs(("results/pid/HGC_pid/piratio_shmsdp_"+file_name+".pdf").c_str());
          
          jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["largex"]["p0"] = p0;
          jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["largex"]["p1"] = p1;
          jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["largex"]["p0_neg"] = p0_neg;
          jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["largex"]["p1_neg"] = p1_neg;
          //jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["smallx"]["p0"] = p0;
          //jout[(std::to_string(xbj_set)).c_str()][(std::to_string(Q2_set)).c_str()]["smallx"]["p1"] = p1;
        }//if not 0
        }//loop over Q2
      }//loop over xbj
      std::ofstream ofs("results/pid/Kpi_ratio.json");
      ofs<<jout.dump(4)<<std::endl;
    }
