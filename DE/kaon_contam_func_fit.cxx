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
#include <math.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"
#include "ROOT/RSnapshotOptions.hxx"
#include "TLine.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"

double c = 299792458;
double shms_length = 20.1;
double t_pi(double p){
  double m = 0.139;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K(double p){
  double m = 0.493;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_pi_fromkaondecay(double p,double position,double pion_peak){
  double m_pi = 0.139;
  double m_K = 0.493;
  double kaon_fly_time = position*std::sqrt(p*p+m_K*m_K)*1e9/(c*p);
  double pi_fly_time = (shms_length-position)*std::sqrt(p*p+m_pi*m_pi)*1e9/(c*p);
  double sum_time = kaon_fly_time+pi_fly_time;
  double real_pi_time = t_pi(p)-pion_peak;
  return sum_time-real_pi_time;
}
double gaus_fun_kaonNodecay(double *x,double *params){
  //param[1] is the momentum, gaus peak mean of the kaon that doesn't decay is calculated by the momentum
  double kaon_nodecay_peak = t_K(params[3])-t_pi(params[3])+params[0];
  //std::cout<<"check kaon slower than pion "<<t_K(params[3])-t_pi(params[3])<<" pion position "<<params[0]<<" kaon no decay position "<<kaon_nodecay_peak<<std::endl;
  //std::cout<<"check kaon "<<kaon_nodecay_peak<<std::endl;
  double gaus_shape = params[2]*exp(-0.5*pow((x[0]-kaon_nodecay_peak)/params[4],2));///(params[2] *sqrt(2*M_PI));
  //std::cout<<"check gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
  
}
double gaus_fun_pion_kaondecay(double *x,double *pa){
//pa[1] is the momentum,
//pa[0] is the probability at the position/78.1%(the probability that kaon doesn't decay at all) of kaon decays multiplied to the fitting kaon peak amplitude(params[0]), pa[2] is the sigma of pion peak, which I use 0.2
//pa[3] is the position where the kaon decay
//gaus peak mean of the pions that kaon decays is calculated by momentum pa[1],and the position where kaon decays pa[3]    
  double pi_fromkaondecay_peak = t_pi_fromkaondecay(pa[1],pa[3],pa[4]);
  double gaus_shape = pa[0]*exp(-0.5*pow((x[0]-pi_fromkaondecay_peak)/pa[2],2));///(pa[2] *sqrt(2*M_PI));
  //std::cout<<"check pion "<<pi_fromkaondecay_peak<<" gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
}
double fit_kaondecay(double *x,double *params){
  //params[0] is the amplitude of nodecay kaon need to be determined by fitting
  //params[1] is the momentum, the mean peak is calculated by the momentum, need to be fixed to be what it is
  //params[2] is the sigma of the nodecay kaon, need to be determined by fitting
  double all_kaon = 0;
  std::ifstream infile("db2/kaoncontam.inp");
  //std::cout<<"type meter prob_left prob_right a b c d e f "<<std::endl;
  double nodecay_kaon = gaus_fun_kaonNodecay(x,params); //This should be already in the last line of the loop (which is deleted, compare kaoncomtam.inp in db2 and in shuo_analysis/dbase), but the sigma is different. This is kaon sigma, which will be a fitting parameter. For the small kaon decay pion peaks, I am using sigma as 0.2, which is typical pion sigma 
  all_kaon = all_kaon+nodecay_kaon;
  string line;
  while(getline(infile,line)){
    double type,meter,prob_left,prob_right,a,b,c,d,e,f;
    std::stringstream ss;
    ss.str(line);
    ss>>type>>meter>>prob_left>>prob_right>>a>>b>>c>>d>>e>>f;
    //std::cout<<type<<" "<<meter<<" "<<prob_left<<" "<<prob_right<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<std::endl;
    double pa[5];
    pa[0] = params[2]*(prob_left+prob_right)/(2*0.8235);//the amplitude of the pion(from kaon decay) peak portion of nodecay kaons. 
    pa[1] = params[3];
    pa[2] = 0.2;
    pa[3] = meter;
    pa[4] = params[0];
    double pi_fromkaondecay =  gaus_fun_pion_kaondecay(x,pa);
    //std::cout<<"check"<<all_kaon<<std::endl;
    all_kaon+=pi_fromkaondecay;
  }
  //std::cout<<"check in fit function "<<all_kaon<<std::endl;
  return all_kaon;
}
double fit_pion(double *x,double *params){
  
  double gaus_shape = params[0]*exp(-0.5*pow((x[0]-params[1])/params[2],2));///(params[2] *sqrt(2*M_PI));
  return gaus_shape;
}
double fit_all(double *x,double *params){
  return fit_pion(x,params)+fit_kaondecay(x,&params[1]);
}
void kaon_contam_func_fit(){
  double parameters[3];
  parameters[0] = 100;
  parameters[1] = 3.9;
  parameters[2] = 0.2;
  //double fit = fit_kaondecay(1.4,parameters);
  //std::cout<<fit<<std::endl;
  json j_cuts;
  {
    std::ifstream ifs("db2/all_cut.json");
    ifs>>j_cuts;
  };
  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs>>j_DE;
  }
  //std::vector<double> rf_cuts = j_DE["SHMS"]["rf_time_cuts"].get<std::vector<double>>();
  std::vector<double> rf_cuts = j_DE["SHMS"]["rf_time_right_cuts"].get<std::vector<double>>();

  double H_dp_low = j_cuts["H_dp_low"].get<double>();
  double H_dp_high = j_cuts["H_dp_high"].get<double>();
  double P_dp_low = j_cuts["P_dp_low"].get<double>();
  double P_dp_high = j_cuts["P_dp_high"].get<double>();
  std::string goodTrackSHMS = "P.gtr.dp>"+std::to_string(P_dp_low)+" && P.gtr.dp<"+std::to_string(P_dp_high);
  std::string goodTrackHMS = "H.gtr.dp>"+std::to_string(H_dp_low)+" && H.gtr.dp<"+std::to_string(H_dp_high);
  std::cout<<goodTrackSHMS<<std::endl;
  std::cout<<goodTrackHMS<<std::endl;
  double SHMS_low = j_cuts["P_cal_pi_low"].get<double>();
  double SHMS_high = j_cuts["P_cal_pi_high"].get<double>();
  std::string piCutSHMS = (" P.cal.etottracknorm > "+std::to_string(SHMS_low)+" && P.cal.etottracknorm < " + std::to_string(SHMS_high)).c_str();
  std::cout<<"picutSHMS "<<piCutSHMS<<std::endl;
  //double P_aero = j_cuts["P_aero"].get<double>();
  double P_aero = j_DE["SHMS"]["aero_n"].get<double>();
  std::string aeroCutSHMS = (" P.aero.npeSum > "+std::to_string(P_aero)).c_str();
  std::cout<<"P_aerocut "<<aeroCutSHMS<<std::endl;
  double H_cal_low = j_cuts["H_cal_low"].get<double>();
  double H_cal_high = j_cuts["H_cal_high"].get<double>();
  double H_cer = j_cuts["H_cer"].get<double>();
  int P_hgcer = j_cuts["P_hgcer"].get<double>();
  std::string hgcCutSHMS = (" P.hgcer.npeSum > "+std::to_string(P_hgcer)).c_str();
  std::string SHMS_hgc_aero = aeroCutSHMS; 
  std::string eCutHMS = ("H.cal.etottracknorm > "+std::to_string(H_cal_low)+" && H.cal.etottracknorm < "+std::to_string(H_cal_high)+" && H.cer.npeSum > "+std::to_string(H_cer)).c_str();
  std::cout<<"cCutHMS"<<eCutHMS<<std::endl;
  double H_xptar_low = j_cuts["H_xptar_low"].get<double>();
  double H_xptar_high = j_cuts["H_xptar_high"].get<double>();
  double P_xptar_low = j_cuts["P_xptar_low"].get<double>();
  double P_xptar_high = j_cuts["P_xptar_high"].get<double>();
  double H_yptar_low = j_cuts["H_yptar_low"].get<double>();
  double H_yptar_high = j_cuts["H_yptar_high"].get<double>();
  double P_yptar_low = j_cuts["P_yptar_low"].get<double>();
  double P_yptar_high = j_cuts["P_yptar_high"].get<double>();
  std::string Normal_HMS = "H.gtr.th > "+std::to_string(H_xptar_low)+" && H.gtr.th < "+std::to_string(H_xptar_high)+" && H.gtr.ph > "+std::to_string(H_yptar_low)+" && H.gtr.ph < "+std::to_string(H_yptar_high); 
  std::string Normal_SHMS = "P.gtr.th > "+std::to_string(P_xptar_low)+" && P.gtr.th < "+std::to_string(P_xptar_high)+" && P.gtr.ph > "+std::to_string(P_yptar_low)+" && P.gtr.ph < "+std::to_string(P_yptar_high); 
  double current_offset = j_cuts["current_diff"].get<double>();
  std::cout<<Normal_HMS<<std::endl;
  std::cout<<Normal_SHMS<<std::endl;

  json j_runsinfo;
  {
    std::ifstream ifs("db2/runs_info.json");
    ifs>>j_runsinfo;
  }
    double cointime_lowcut,cointime_highcut;
  cointime_lowcut = j_cuts["cointime_low_fall"].get<double>();
  cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
  int RunNumber = 6145;
  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d_pos_raw("T",rootfile_name);
  ROOT::RDataFrame d_pos_scaler("TSP",rootfile_name);
  std::cout<<rootfile_name<<std::endl;
  auto pos_scaler_current_list = d_pos_scaler.Take<double>("P.BCM1.scalerCurrent");
  auto pos_scaler_event_list = d_pos_scaler.Take<double>("evNumber");
  auto h_pos_current = d_pos_scaler.Histo1D({"pos current","pos current",100,3,100},"P.BCM1.scalerCurrent");
  double pos_setcurrent = h_pos_current->GetBinCenter(h_pos_current->GetMaximumBin());
  std::cout<<"set current "<<pos_setcurrent<<std::endl;
  //std::cout<<"event size "<<pos_scaler_event_list->size()<<" current size "<<pos_scaler_current_list->size()<<std::endl;
  auto pos_get_current = [&](unsigned int eventNum){
    int i = 0;
    while(eventNum>pos_scaler_event_list->at(i)){
      ++i;
      if(i>pos_scaler_event_list->size()-1)
      {
        i=pos_scaler_event_list->size()-1;
        break;
      }
    }
    return pos_scaler_current_list->at(i);
    //std::cout<< pos_scaler_current_list->at(i)<<std::endl;

  };
  auto d_pos_run = d_pos_raw.Filter("fEvtHdr.fEvtType == 4")
    //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
    .Filter(goodTrackSHMS)
    .Filter(goodTrackHMS)
    .Filter(piCutSHMS)
    .Filter(eCutHMS)
    //.Filter(aeroCutSHMS)
    .Filter(Normal_SHMS)
    .Filter(Normal_HMS)
    .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
    .Define("current",pos_get_current,{"fEvtHdr.fEvtNum"})
    .Filter([&](double current){return current>current_offset;},{"current"})
    ;

  //coin time cut for pos runs
  auto h_cointime_pos = d_pos_run.Histo1D({"","coin_time",800,40,46},"CTime.ePiCoinTime_ROC2");
  int coin_peak_bin_pos = h_cointime_pos->GetMaximumBin();
  double coin_peak_center_pos = h_cointime_pos->GetBinCenter(coin_peak_bin_pos);
  std::cout<<"pos coin time peak "<<coin_peak_center_pos<<std::endl;
  //cointime cut
  double cointime_low_pos = coin_peak_center_pos+cointime_lowcut;
  double cointime_high_pos = coin_peak_center_pos+cointime_highcut;

  auto d_pos_first = d_pos_run
    .Filter([cointime_low_pos,cointime_high_pos](double cointime){return cointime>cointime_low_pos && cointime<cointime_high_pos;},{"CTime.ePiCoinTime_ROC2"})
    ;
  auto h_coin_pos = d_pos_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
  auto h_coin_poscut_rungroup = d_pos_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

  //rftime cut
  //offset
  double offset_pos = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
  std::cout<<"offset for pos runs "<<offset_pos<<std::endl;
  auto d_mod_first = d_pos_first
    .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
    .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
    //    .Filter(aeroCutSHMS)
    ;

  std::string bg_cut = " ";  
  //for bg
  int bg_left_low = j_cuts["random_bg_left_low"].get<int>();
  int bg_left_high = j_cuts["random_bg_left_high"].get<int>();
  int bg_right_low = j_cuts["random_bg_right_low"].get<int>();
  int bg_right_high = j_cuts["random_bg_right_high"].get<int>();
  for(int i = bg_left_low;i<bg_left_high;i=i+2){
    double bg_main = coin_peak_center_pos+i*4.008;
    double bg_left = bg_main+cointime_lowcut;
    double bg_right = bg_main+cointime_highcut;
    bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
  }
  for(int i = bg_right_low;i<bg_right_high;i=i+2){
    double bg_main = coin_peak_center_pos+i*4.008;
    double bg_left = bg_main+cointime_lowcut;
    double bg_right = bg_main+cointime_highcut;
    bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
  }
  bg_cut = bg_cut.substr(0,bg_cut.size()-2);
  std::cout<<bg_cut<<std::endl;

  auto d_pos_bg_norfcut = d_pos_run
    .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
    .Filter(bg_cut)
    .Define("diff_time_shift",[offset_pos](double difftime){return difftime+offset_pos;},{"fptime_minus_rf"})
    .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
    //  .Filter(aeroCutSHMS)
    ;
  auto d_pos_piall  = d_mod_first
    //.Filter(aeroCutSHMS)
    .Filter(SHMS_hgc_aero)
    ;
  auto d_pos_Kall = d_mod_first
    //.Filter("P.aero.npeSum<10")
    .Filter("P.hgcer.npeSum<2")
    .Filter(aeroCutSHMS)

    //.Filter(piCutSHMS)
    ;
  auto d_pos_piall_bg = d_pos_bg_norfcut
    //.Filter(aeroCutSHMS)
    .Filter(SHMS_hgc_aero)
    ;
  auto d_pos_Kall_bg = d_pos_bg_norfcut
    //.Filter("P.aero.npeSum<10")
    .Filter("P.hgcer.npeSum<2")
    //.Filter(piCutSHMS)
    .Filter(aeroCutSHMS)
    ;


  //statistics for DE efficiency
  auto h_rf_pos = d_pos_piall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
  auto h_rf_pos_bg = d_pos_piall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
  h_rf_pos->Add(h_rf_pos_bg.GetPtr(),-1.0/6); 

  auto h_rf_pos_K = d_pos_Kall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
  auto h_rf_pos_K_bg = d_pos_Kall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
  h_rf_pos_K->Add(h_rf_pos_K_bg.GetPtr(),-1.0/6); 

  double par_pos[7];
  TCanvas *c_kaoncon = new TCanvas();
  TF1 *all_pos = new TF1("all fit",fit_all,0.5,2,6);
  all_pos->SetParameter(0,200);
  //all_pos->FixParameter(0,200);
  all_pos->SetParameter(1,1);
  //all_pos->FixParameter(1,3.2);
  all_pos->SetParameter(2,0.2);
  //all_pos->FixParameter(2,0.2);
  all_pos->SetParameter(3,120);
  //all_pos->FixParameter(3,120);
  //double kaon_time = t_K(3.34)-t_pi(3.34); 
  all_pos->SetParameter(4,3.34);
  all_pos->FixParameter(4,3.34);
  all_pos->SetParameter(5,0.4);
  //all_pos->FixParameter(5,0.4);
  h_rf_pos_K->Fit(all_pos);
  all_pos->GetParameters(&par_pos[0]);
  h_rf_pos_K->DrawCopy("hist");
  all_pos->SetLineColor(kBlack);
  all_pos->Draw("same");
  TF1 *pion_fit = new TF1("pion fit",fit_pion,0.5,2,3);
  pion_fit->SetParameters(par_pos[0],par_pos[1],par_pos[2]);
  TF1 *Kaon_fit = new TF1("Kaon fit",fit_kaondecay,0.5,2,5);
  Kaon_fit->SetParameters(par_pos[1],par_pos[2],par_pos[3],par_pos[4],par_pos[5]);
  pion_fit->SetLineColor(kRed);
  pion_fit->Draw("same");
  Kaon_fit->SetLineColor(kOrange);
  Kaon_fit->Draw("same");
  //pi_pos_Kall->SetParameters(par_pos[0],par_pos[1],par_pos[2]);
      //K_pos_Kall->SetParameters(par_pos[3],par_pos[4],par_pos[5]);
  //return fit_pion(x,params)+fit_kaondecay(x,&params[3]);
  c_kaoncon->SaveAs("results/pid/rftime_new/check.pdf");
}
