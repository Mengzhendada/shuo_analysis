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

#include "ROOT/RDataFrame.hxx"
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
double GeV = 1.78266192e-27;
double c = 299792458;
double shms_length = 20.1;
double t_e(double p){
  double m = 0.00051;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_pi(double p){
  double m = 0.139;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K(double p){
  double m = 0.493;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_proton(double p){
  double m = 0.938;
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
  //double nodecay_kaon = gaus_fun_kaonNodecay(x,params); //This should be already in the last line of the loop (which is deleted, compare kaoncomtam.inp in db2 and in shuo_analysis/dbase), but the sigma is different. This is kaon sigma, which will be a fitting parameter. For the small kaon decay pion peaks, I am using sigma as 0.2, which is typical pion sigma 
  //all_kaon = all_kaon+nodecay_kaon;
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
void SHMS_rf_twocomplicatedfit_low(int RunGroup = 0){
  if(RunGroup ==0){
    std::cout<<"Enter a RunGroup (-1 to exit):";
    std::cin>>RunGroup;
    if(RunGroup<=0)
      return;
  }

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  json j_rungroup_info;

  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[(std::to_string(RunGroup)).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double shms_p_central = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();
  double shms_p;
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

  if(!neg_D2.empty() && !pos_D2.empty()){

    double cointime_lowcut,cointime_highcut;
    if(RunGroup<410){
      cointime_lowcut = j_cuts["cointime_low_fall"].get<double>();
      cointime_highcut = j_cuts["cointime_high_fall"].get<double>();
    }
    else{
      cointime_lowcut = j_cuts["cointime_low_spring"].get<double>();
      cointime_highcut = j_cuts["cointime_high_spring"].get<double>();
    }


    TH1D* h_rf_pos_Kall = new TH1D("",";rftime;counts",100,0,4);
    TH1D* h_rf_neg_Kall = new TH1D("",";rftime;counts",100,0,4);

    TH1D* h_rf_pos_piall = new TH1D("",";rftime;counts",100,0,4);
    TH1D* h_rf_neg_piall = new TH1D("",";rftime;counts",100,0,4);

    TH1D* h_delta_pos_all = new TH1D("","",100,-10,20);
    TH1D* h_delta_neg_all = new TH1D("","",100,-10,20);

    //loop over each pos runs data
    std::cout<<"check"<<std::endl;
    std::vector<int> delta_cut_num= j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>(); 
    //{-5,0,5,10,15,20};
    //{"P.gtr.dp < -5","P.gtr.dp>-5 && P.gtr.dp<0","P.gtr.dp>0 && P.gtr.dp < 5","P.gtr.dp>5 && P.gtr.dp<10","P.gtr.dp>10 && P.gtr.dp < 15","P.gtr.dp>15"};
    int i_dpcut = 0;
    double delta_lowend = delta_cut_num[0];
    for(auto it = std::next(delta_cut_num.begin());it<delta_cut_num.end();it++){
      std::string dp_cut = "P.gtr.dp>"+std::to_string(delta_lowend)+" && P.gtr.dp < "+std::to_string(*it);
      std::cout<<"delta cut is "<<dp_cut<<std::endl;
      double shms_p_lowend = shms_p_central*(100+delta_lowend)/100;
      if(shms_p_lowend > 3){
        SHMS_hgc_aero = aeroCutSHMS;
        //+" && "+hgcCutSHMS;
      }
      //SHMS_hgc_aero = aeroCutSHMS;}
      else{SHMS_hgc_aero = aeroCutSHMS;}
      std::cout<<"momentum lowend is "<<shms_p_lowend<<" ,cut is "<<SHMS_hgc_aero<<std::endl;
      for(auto it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        std::cout<<"pos data"<<RunNumber<<std::endl;
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
          .Filter(dp_cut.c_str())
          //.Filter(aeroCutSHMS)
          .Filter(SHMS_hgc_aero)
          ;
        auto d_pos_Kall = d_mod_first
          //.Filter("P.aero.npeSum<10")
          .Filter("P.hgcer.npeSum<2")
          .Filter(dp_cut.c_str())
          .Filter(aeroCutSHMS)

          //.Filter(piCutSHMS)
          ;
        auto d_pos_piall_bg = d_pos_bg_norfcut
          .Filter(dp_cut.c_str())
          //.Filter(aeroCutSHMS)
          .Filter(SHMS_hgc_aero)
          ;
        auto d_pos_Kall_bg = d_pos_bg_norfcut
          //.Filter("P.aero.npeSum<10")
          .Filter("P.hgcer.npeSum<2")
          .Filter(dp_cut.c_str())
          //.Filter(piCutSHMS)
          .Filter(aeroCutSHMS)
          ;



        //not really necessary to check this histogram. 
        auto h_coin_pos_piall = d_pos_piall.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");
        auto h_coin_pos_piall_bg = d_pos_piall_bg.Histo1D({"","pos eall",800,0,100},"CTime.ePiCoinTime_ROC2");

        //statistics for DE efficiency
        auto h_rf_pos = d_pos_piall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
        auto h_rf_pos_bg = d_pos_piall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
        h_rf_pos->Add(h_rf_pos_bg.GetPtr(),-1.0/6); 
        h_rf_pos_piall->Add(h_rf_pos.GetPtr(),1);

        auto h_rf_pos_K = d_pos_Kall.Histo1D({"","pos,rftime,norfcut",100,0,4},"diff_time_mod");
        auto h_rf_pos_K_bg = d_pos_Kall_bg.Histo1D({"","pos,cal,norfcut",100,0,4},"diff_time_mod");
        h_rf_pos_K->Add(h_rf_pos_K_bg.GetPtr(),-1.0/6); 
        h_rf_pos_Kall->Add(h_rf_pos_K.GetPtr(),1);

        //get mean SHMS momentum 
        auto h_delta = d_pos_piall.Histo1D({"","",100,-10,20},"P.gtr.dp");
        auto h_delta_bg = d_pos_piall_bg.Histo1D({"","",100,-10,20},"P.gtr.dp");
        h_delta->Add(h_delta_bg.GetPtr(),-1.0/6);
        h_delta_pos_all->Add(h_delta.GetPtr(),1.0);

      }
        //loop over each neg runs data
        for(auto it = neg_D2.begin();it!=neg_D2.end();++it){
          int RunNumber = *it;
          std::cout<<"neg data"<<RunNumber<<std::endl;
          std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
          ROOT::RDataFrame d_neg_raw("T",rootfile_name);
          ROOT::RDataFrame d_neg_scaler("TSP",rootfile_name);
          std::cout<<rootfile_name<<std::endl;
          auto neg_scaler_current_list = d_neg_scaler.Take<double>("P.BCM1.scalerCurrent");
          auto neg_scaler_event_list = d_neg_scaler.Take<double>("evNumber");
          auto h_neg_current = d_neg_scaler.Histo1D({"neg current","neg current",100,3,100},"P.BCM1.scalerCurrent");
          double neg_setcurrent = h_neg_current->GetBinCenter(h_neg_current->GetMaximumBin());
          std::cout<<"set current "<<neg_setcurrent<<std::endl;
          //std::cout<<"event size "<<neg_scaler_event_list->size()<<" current size "<<neg_scaler_current_list->size()<<std::endl;
          auto neg_get_current = [&](unsigned int eventNum){
            int i = 0;
            while(eventNum>neg_scaler_event_list->at(i)){
              ++i;
              if(i>neg_scaler_event_list->size()-1)
              {
                i=neg_scaler_event_list->size()-1;
                break;
              }
            }
            return neg_scaler_current_list->at(i);
            //std::cout<< neg_scaler_current_list->at(i)<<std::endl;

          };
          auto d_neg_run = d_neg_raw.Filter("fEvtHdr.fEvtType == 4")
            //.Define("shms_p",shms_p_calculate,{"P.gtr.dp"})
            .Filter(goodTrackSHMS)
            .Filter(goodTrackHMS)
            .Filter(piCutSHMS)
            .Filter(eCutHMS)
            //.Filter(aeroCutSHMS)
            .Filter(Normal_SHMS)
            .Filter(Normal_HMS)
            .Define("fptime_minus_rf","P.hod.starttime - T.coin.pRF_tdcTime")
            .Define("current",neg_get_current,{"fEvtHdr.fEvtNum"})
            .Filter([&](double current){return current>current_offset;},{"current"})
            ;


          //coin time cut for neg runs
          auto h_cointime_neg = d_neg_run.Histo1D({"","coin_time",800,30,55},"CTime.ePiCoinTime_ROC2");
          int coin_peak_bin_neg = h_cointime_neg->GetMaximumBin();
          double coin_peak_center_neg = h_cointime_neg->GetBinCenter(coin_peak_bin_neg);
          std::cout<<"neg coin time peak "<<coin_peak_center_neg<<std::endl;
          //cointime cut
          double cointime_low_neg = coin_peak_center_neg+cointime_lowcut;
          double cointime_high_neg = coin_peak_center_neg+cointime_highcut;

          auto d_neg_first = d_neg_run
            .Filter([cointime_low_neg,cointime_high_neg](double cointime){return cointime>cointime_low_neg && cointime<cointime_high_neg;},{"CTime.ePiCoinTime_ROC2"})
            ;
          auto h_coin_neg = d_neg_run.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");
          auto h_coin_negcut_rungroup = d_neg_first.Histo1D({"","",800,0,100},"CTime.ePiCoinTime_ROC2");

          //rftime cut
          //offset
          double offset_neg = j_runsinfo[(std::to_string(RunNumber)).c_str()]["offset"].get<double>();
          std::cout<<"offset for neg runs "<<offset_neg<<std::endl;
          auto d_mod_first = d_neg_first
            .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
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
            double bg_main = coin_peak_center_neg+i*4.008;
            double bg_left = bg_main+cointime_lowcut;
            double bg_right = bg_main+cointime_highcut;
            bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
          }
          for(int i = bg_right_low;i<bg_right_high;i=i+2){
            double bg_main = coin_peak_center_neg+i*4.008;
            double bg_left = bg_main+cointime_lowcut;
            double bg_right = bg_main+cointime_highcut;
            bg_cut = bg_cut + " (bg_cointime > "+std::to_string(bg_left)+" && bg_cointime < "+std::to_string(bg_right)+") ||";
          }
          bg_cut = bg_cut.substr(0,bg_cut.size()-2);
          std::cout<<bg_cut<<std::endl;

          auto d_neg_bg_norfcut = d_neg_run
            .Define("bg_cointime",[](double cointime){return cointime;},{"CTime.ePiCoinTime_ROC2"})
            .Filter(bg_cut)
            .Define("diff_time_shift",[offset_neg](double difftime){return difftime+offset_neg;},{"fptime_minus_rf"})
            .Define("diff_time_mod",[](double difftime){return std::fmod(difftime,4.008);},{"diff_time_shift"})
            //  .Filter(aeroCutSHMS)
            ;

          auto d_neg_piall  = d_mod_first
            .Filter(dp_cut.c_str())
            //.Filter(aeroCutSHMS)
            .Filter(SHMS_hgc_aero)
            ;
          auto d_neg_Kall = d_mod_first
            //.Filter("P.aero.npeSum<10")
            .Filter("P.hgcer.npeSum<2")
            .Filter(dp_cut.c_str())
            //.Filter(piCutSHMS)
            //.Filter(aeroCutSHMS)
            ;
          auto d_neg_piall_bg = d_neg_bg_norfcut
            .Filter(dp_cut.c_str())
            //.Filter(aeroCutSHMS)
            .Filter(SHMS_hgc_aero)
            ;
          auto d_neg_Kall_bg = d_neg_bg_norfcut
            //.Filter("P.aero.npeSum<10")
            .Filter("P.hgcer.npeSum<2")
            .Filter(dp_cut.c_str())
            //.Filter(piCutSHMS)
            //.Filter(aeroCutSHMS)
            ;

          //statistics for DE efficiency
          auto h_rf_neg = d_neg_piall.Histo1D({"","neg,rftime,norfcut",100,0,4},"diff_time_mod");
          auto h_rf_neg_bg = d_neg_piall_bg.Histo1D({"","neg,cal,norfcut",100,0,4},"diff_time_mod");
          h_rf_neg->Add(h_rf_neg_bg.GetPtr(),-1.0/6); 
          h_rf_neg_piall->Add(h_rf_neg.GetPtr(),1);

          auto h_rf_neg_K = d_neg_Kall.Histo1D({"","neg,rftime,norfcut",100,0,4},"diff_time_mod");
          auto h_rf_neg_K_bg = d_neg_Kall_bg.Histo1D({"","neg,cal,norfcut",100,0,4},"diff_time_mod");
          h_rf_neg_K->Add(h_rf_neg_K_bg.GetPtr(),-1.0/6); 
          h_rf_neg_Kall->Add(h_rf_neg_K.GetPtr(),1);

          //get mean SHMS momentum 
          auto h_delta = d_neg_piall.Histo1D({"","",100,-10,20},"P.gtr.dp");
          auto h_delta_bg = d_neg_piall_bg.Histo1D({"","",100,-10,20},"P.gtr.dp");
          h_delta->Add(h_delta_bg.GetPtr(),-1.0/6);
          h_delta_neg_all->Add(h_delta.GetPtr(),1.0);
        }

      double delta_mean_pos = h_delta_pos_all->GetMean();
      std::cout<<"delta mean"<<delta_mean_pos<<std::endl;
      shms_p = shms_p_central*(1+delta_mean_pos/100);
      double time_diff = t_K(shms_p) - t_pi(shms_p);
      double time_diff_proton = t_proton(shms_p) - t_pi(shms_p);
      std::cout<<"time for kaon "<<time_diff<<std::endl;
      if(shms_p<2.8){
        double par_pos[6];
        TCanvas *c_rftime_pos = new TCanvas();
        gStyle->SetOptTitle(0);
        gStyle->SetPalette(kBird);
        h_rf_pos_Kall->DrawCopy("hist");
        TF1 *all_pos = new TF1("all fit",fit_all,0.5,1+2*time_diff,6);
        all_pos->SetLineColor(1);
        all_pos->SetParameter(1,1);
        all_pos->SetParLimits(1,0.9,1.1);
        all_pos->SetParameter(2,0.2);
        all_pos->FixParameter(2,0.2);
        all_pos->SetParameter(4,shms_p);
        all_pos->FixParameter(4,shms_p);
        all_pos->SetParameter(5,0.3);
        h_rf_pos_Kall->Fit(all_pos);
        all_pos->GetParameters(&par_pos[0]);
        TF1* pi_pos_Kall = new TF1("pi",fit_pion,0.5,1.5,3);
        TF1* K_pos_Kall = new TF1("K",fit_kaondecay,0.5,1+2*time_diff,5);
        pi_pos_Kall->SetParameters(par_pos[0],par_pos[1],par_pos[2]);
        K_pos_Kall->SetParameters(par_pos[1],par_pos[2],par_pos[3],par_pos[4],par_pos[5]);
        pi_pos_Kall->SetLineColor(kRed);
        K_pos_Kall->SetLineColor(kOrange);
        pi_pos_Kall->Draw("same");
        K_pos_Kall->Draw("same");
        TPaveText* pt_pos = new TPaveText(0.75,0.5,1,0.95,"brNDC");
        pt_pos->AddText(("RunGroup pos K "+std::to_string(RunGroup)).c_str());
        pt_pos->AddText(("shms p "+std::to_string(shms_p)).c_str());
        pt_pos->AddText(("A_{#pi} = "+std::to_string(par_pos[0])).c_str());
        pt_pos->AddText(("#mu_{#pi} = "+std::to_string(par_pos[1])).c_str());
        pt_pos->AddText(("#sigma_{#pi} = "+std::to_string(par_pos[2])).c_str());
        pt_pos->AddText(("Kaon time "+std::to_string(par_pos[1]+time_diff)).c_str());
        pt_pos->AddText(("A_{K} = "+std::to_string(par_pos[3])).c_str());
        pt_pos->AddText(("#mu_{K} = "+std::to_string(par_pos[1]+time_diff)).c_str());
        pt_pos->AddText(("#sigma_{K} = "+std::to_string(par_pos[5])).c_str());
        pt_pos->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
        pt_pos->Draw();
        c_rftime_pos->Update();
        std::string c_rftime_pos_name = "results/pid/rftime_new/rftime_pos_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_low.pdf";
        c_rftime_pos->SaveAs(c_rftime_pos_name.c_str());

        double par_neg_pi[6];
        TCanvas *c_pi_neg = new TCanvas();
        //c_pi_neg->SetGrid();
        gStyle->SetOptTitle(0);
        gStyle->SetPalette(kBird);
        h_rf_neg_piall->SetMinimum(0);
        h_rf_neg_piall->DrawCopy("hist");
        TF1 *all_neg_pi = new TF1("all fit",fit_all,0.5,1+2*time_diff,6);
        all_neg_pi->SetLineColor(1);
        all_neg_pi->SetParameter(1,1);
        all_neg_pi->SetParLimits(1,0.9,1.1);
        all_neg_pi->SetParameter(2,0.2);
        all_neg_pi->SetParameter(4,shms_p);
        all_neg_pi->FixParameter(4,shms_p);
        all_neg_pi->SetParameter(5,par_pos[5]);
        all_neg_pi->FixParameter(5,par_pos[5]);
        h_rf_neg_piall->Fit(all_neg_pi);
        all_neg_pi->GetParameters(&par_neg_pi[0]);
        TF1* pi_neg_piall = new TF1("pi",fit_pion,0.5,1.5,3);
        pi_neg_piall->SetParameters(par_neg_pi[0],par_neg_pi[1],par_neg_pi[2]);
        TF1* K_neg_piall = new TF1("K",fit_kaondecay,0.5,1+2*time_diff,5);
        K_neg_piall->SetParameters(par_neg_pi[1],par_neg_pi[2],par_neg_pi[3],par_neg_pi[4],par_neg_pi[5]);
        pi_neg_piall->SetLineColor(kRed);
        K_neg_piall->SetLineColor(kOrange);
        pi_neg_piall->Draw("same");
        K_neg_piall->Draw("same");
        double width_neg = h_rf_neg_piall->GetXaxis()->GetBinWidth(1);
        std::cout<<"Bin width "<<width_neg<<std::endl;
        double neg_pi_all_pifit = pi_neg_piall->Integral(0,4,width_neg);
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["neg"]["pi_eff_all"] = neg_pi_all_pifit;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pi_peak"]["neg"] = par_neg_pi[1];
        TPaveText* pt_neg_pi = new TPaveText(0.75,0.5,1,0.95,"brNDC");
        pt_neg_pi->AddText(("RunGroup neg pi "+std::to_string(RunGroup)).c_str());
        pt_neg_pi->AddText(("shms p "+std::to_string(shms_p)).c_str());
        pt_neg_pi->AddText(("A_{#pi} = "+std::to_string(par_neg_pi[0])).c_str());
        pt_neg_pi->AddText(("#mu_{#pi} = "+std::to_string(par_neg_pi[1])).c_str());
        pt_neg_pi->AddText(("#sigma_{#pi} = "+std::to_string(par_neg_pi[2])).c_str());
        pt_neg_pi->AddText(("Kaon time "+std::to_string(par_neg_pi[1]+time_diff)).c_str());
        pt_neg_pi->AddText(("A_{K} = "+std::to_string(par_neg_pi[3])).c_str());
        pt_neg_pi->AddText(("#mu_{K} = "+std::to_string(par_neg_pi[4])).c_str());
        pt_neg_pi->AddText(("#sigma_{K} = "+std::to_string(par_neg_pi[5])).c_str());
        pt_neg_pi->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
        pt_neg_pi->Draw();
        c_pi_neg->Update();
        std::string c_pi_neg_name = "results/pid/rftime_new/rftime_neg_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_pi_low.pdf";
        c_pi_neg->SaveAs(c_pi_neg_name.c_str());
        std::vector<double> n_neg_pi_rf,n_neg_K_rf;
        std::vector<double> rf_neg_cuts,rf_neg_cuts_low;
        for(int i = 0;i<rf_cuts.size();++i){
          //double rf_cut_percent = rf_cuts[i];
          ////double rf_pi_low = 0.5;
          //double rf_pi_low = 1-(rf_cut_percent/100)*time_diff;
          //double rf_pi_high = 1+(rf_cut_percent/100)*time_diff;

          double rf_pi_low = 1-(rf_cuts[i]-1);
          double rf_pi_high = rf_cuts[i];

          rf_neg_cuts.push_back(rf_pi_high);
          rf_neg_cuts_low.push_back(rf_pi_low);
          double neg_pi_N = pi_neg_piall->Integral(rf_pi_low,rf_pi_high,width_neg);
          n_neg_pi_rf.push_back(neg_pi_N);
          double neg_K_N = K_neg_piall->Integral(rf_pi_low,rf_pi_high,width_neg);
          n_neg_K_rf.push_back(neg_K_N); 
          std::cout<<neg_K_N<<" "<<neg_pi_N<<" "<<i_dpcut<<std::endl;
        }
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["neg"]["rf_cuts_high"] = rf_neg_cuts ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["neg"]["rf_cuts_low"] = rf_neg_cuts_low ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["neg"]["pi_eff_Ns"] = n_neg_pi_rf ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["neg"]["Ks"] = n_neg_K_rf;

        double par_pos_pi[6];
        TCanvas *c_pi_pos = new TCanvas();
        ////c_pi_pos->SetGrid();
        gStyle->SetOptTitle(0);
        gStyle->SetPalette(kBird);
        h_rf_pos_piall->SetMinimum(0);
        h_rf_pos_piall->DrawCopy("hist");
        TF1 *all_pos_pi = new TF1("all fit",fit_all,0.5,1+2*time_diff,6);
        all_pos_pi->SetLineColor(1);
        //all_pos_pi->SetParameter(1,1);
        //all_pos_pi->SetParLimits(1,0.9,1.1);
        all_pos_pi->SetParameter(1,par_neg_pi[1]);
        all_pos_pi->FixParameter(1,par_neg_pi[1]);
        all_pos_pi->SetParameter(2,par_pos[2]);
        all_pos_pi->SetParameter(4,shms_p);
        all_pos_pi->FixParameter(4,shms_p);
        all_pos_pi->SetParameter(5,par_pos[5]);
        all_pos_pi->FixParameter(5,par_pos[5]);
        //all_pos_pi->FixParameter(5,par_pos[5]);
        h_rf_pos_piall->Fit(all_pos_pi);
        all_pos_pi->GetParameters(&par_pos_pi[0]);
        TF1 *pi_pos_piall =new TF1("pion fit",fit_pion,0.5,2,3);
        pi_pos_piall->SetParameters(par_pos_pi[0],par_pos_pi[1],par_pos_pi[2]);
        TF1* K_pos_piall = new TF1("K",fit_kaondecay,0.5,1+2*time_diff,5);
        K_pos_piall->SetParameters(par_pos_pi[1],par_pos_pi[2],par_pos_pi[3],par_pos_pi[4],par_pos_pi[5]);
        pi_pos_piall->SetLineColor(kRed);
        K_pos_piall->SetLineColor(kOrange);
        pi_pos_piall->Draw("same");
        K_pos_piall->Draw("same");
        double width_pos = h_rf_pos_piall->GetXaxis()->GetBinWidth(1);
        std::cout<<"Bin width "<<width_pos<<std::endl;
        double pos_pi_all_pifit = pi_pos_piall->Integral(0,4,width_pos);
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pos"]["pi_eff_all"] = pos_pi_all_pifit;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["shms_p"] = shms_p;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pi_peak"]["pos"] = par_pos_pi[1];
        TPaveText* pt_pos_pi = new TPaveText(0.75,0.5,1,0.95,"brNDC");
        pt_pos_pi->AddText(("RunGroup pos pi "+std::to_string(RunGroup)).c_str());
        pt_pos_pi->AddText(("shms p "+std::to_string(shms_p)).c_str());
        pt_pos_pi->AddText(("A_{#pi} = "+std::to_string(par_pos_pi[0])).c_str());
        pt_pos_pi->AddText(("#mu_{#pi} = "+std::to_string(par_pos_pi[1])).c_str());
        pt_pos_pi->AddText(("#sigma_{#pi} = "+std::to_string(par_pos_pi[2])).c_str());
        pt_pos_pi->AddText(("Kaon time "+std::to_string(par_pos_pi[1]+time_diff)).c_str());
        pt_pos_pi->AddText(("A_{K} = "+std::to_string(par_pos_pi[3])).c_str());
        //pt_pos_pi->AddText(("#mu_{K} = "+std::to_string(par_pos_pi[1]+time_diff)).c_str());
        pt_pos_pi->AddText(("#sigma_{K} = "+std::to_string(par_pos_pi[5])).c_str());
        pt_pos_pi->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
        pt_pos_pi->Draw();
        c_pi_pos->Update();
        std::string c_pi_pos_name = "results/pid/rftime_new/rftime_pos_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_pi_low.pdf";
        c_pi_pos->SaveAs(c_pi_pos_name.c_str());

        std::vector<double> n_pos_pi_rf,n_pos_K_rf;
        std::vector<double> rf_pos_cuts,rf_pos_cuts_low;
        for(int i = 0;i<rf_cuts.size();++i){
          //double rf_cut_percent = rf_cuts[i];
          //double rf_pi_low = 0.5;
          //double rf_pi_low = 1-(rf_cut_percent/100)*time_diff;
          //double rf_pi_high = 1+(rf_cut_percent/100)*time_diff;

          double rf_pi_low = 1-(rf_cuts[i]-1);
          double rf_pi_high = rf_cuts[i];

          rf_pos_cuts.push_back(rf_pi_high);
          rf_pos_cuts_low.push_back(rf_pi_low);
          double pos_pi_N = pi_pos_piall->Integral(rf_pi_low,rf_pi_high,width_pos);
          n_pos_pi_rf.push_back(pos_pi_N);
          double pos_K_N = K_pos_piall->Integral(rf_pi_low,rf_pi_high,width_pos);
          n_pos_K_rf.push_back(pos_K_N); 
          std::cout<<pos_K_N<<" "<<pos_pi_N<<" "<<i_dpcut<<std::endl;
        }
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pos"]["rf_cuts_high"] = rf_pos_cuts ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pos"]["rf_cuts_low"] = rf_pos_cuts_low ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pos"]["pi_eff_Ns"] = n_pos_pi_rf ;
        j_rungroup_info[(std::to_string(RunGroup)).c_str()][(std::to_string(i_dpcut)).c_str()]["pos"]["Ks"] = n_pos_K_rf;


        //TCanvas *c_pi_pos_2nd = new TCanvas();
        ////c_pi_pos_2nd->SetGrid();
        //gStyle->SetOptTitle(0);
        //gStyle->SetPalette(kBird);
        //h_rf_pos_piall->DrawCopy("hist");
        //pi_pos_piall->GetParameters(&par_pos_pi[0]);
        //K_pos_piall->GetParameters(&par_pos_pi[3]);
        //double sigma_pi_pos = par_pos_pi[2];
        //TLine *l_rf_lowsigma_pos = new TLine(1-3*sigma_pi_pos,0,1-3*sigma_pi_pos,1000);
        //TLine *l_rf_highsigma_pos = new TLine(1+3*sigma_pi_pos,0,1+3*sigma_pi_pos,1000);
        //l_rf_lowsigma_pos->Draw("same");
        //l_rf_highsigma_pos->Draw("same");
        //double pos_pi_N_sigma = all_pos_pi->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width_pos);
        ////double pos_pi_N_sigma = pi_pos_piall->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width_pos);
        //double pos_K_N_sigma = K_pos_piall->Integral(1-3*sigma_pi_pos,1+3*sigma_pi_pos,width_pos);
        //std::cout<<"Pi number in fitting "<<pos_pi_N_sigma<<"K number in fitting "<<pos_K_N_sigma<<std::endl;
        //TPaveText* pt_pos_pi_2nd = new TPaveText(0.75,0.5,1,0.95,"brNDC");
        //pt_pos_pi_2nd->AddText(("RunGroup pos pi in 3 sigma "+std::to_string(RunGroup)).c_str());
        //pt_pos_pi_2nd->AddText(("shms p "+std::to_string(shms_p)).c_str());
        ////pt_pos_pi_2nd->AddText(("Pi: p0 = "+std::to_string(par_pos_pi[0])).c_str());
        //pt_pos_pi_2nd->AddText(("#mu_{#pi} = "+std::to_string(par_pos_pi[1])).c_str());
        //pt_pos_pi_2nd->AddText(("#sigma_{#pi} = "+std::to_string(par_pos_pi[2])).c_str());
        //pt_pos_pi_2nd->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
        ////pt_pos_pi_2nd->AddText(("K: p0 = "+std::to_string(par_pos_pi[3])).c_str());
        //pt_pos_pi_2nd->AddText(("#mu_{K} = "+std::to_string(par_pos_pi[4])).c_str());
        //pt_pos_pi_2nd->AddText(("#sigma_{K} = "+std::to_string(par_pos_pi[5])).c_str());
        //pt_pos_pi_2nd->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
        //pt_pos_pi_2nd->Draw();
        //c_pi_pos_2nd->Update();
        //std::string c_pi_pos_2nd_name = "results/pid/rftime/rftime_pos_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_pi_2nd.pdf";
        //c_pi_pos_2nd->SaveAs(c_pi_pos_2nd_name.c_str());


        double delta_mean_neg = h_delta_neg_all->GetMean();
        shms_p = shms_p_central*(1+delta_mean_neg/100);
        time_diff = t_K(shms_p) - t_pi(shms_p);
        time_diff_proton = t_proton(shms_p) - t_pi(shms_p);
        std::cout<<"time for kaon "<<time_diff<<std::endl;
        double par_neg[6];
        TCanvas *c_rftime_neg = new TCanvas();
        gStyle->SetOptTitle(0);
        gStyle->SetPalette(kBird);
        //c_rftime_neg->SetGrid();
        h_rf_neg_Kall->DrawCopy("hist");
        TF1 *all_neg = new TF1("all fit",fit_all,0.5,1+2*time_diff,6);
        all_neg->SetLineColor(1);
        all_neg->SetParameter(1,1);
        all_neg->SetParLimits(1,0.9,1.1);
        all_neg->SetParameter(2,0.2);
        all_neg->SetParameter(4,shms_p);
        all_neg->FixParameter(4,shms_p);
        all_neg->SetParameter(5,par_pos[5]);
        all_neg->FixParameter(5,par_pos[5]);
        h_rf_neg_Kall->Fit(all_neg);
        all_neg->GetParameters(&par_neg[0]);
        TF1* pi_neg_Kall = new TF1("pi",fit_pion,0.5,1.5,3);
        TF1* K_neg_Kall = new TF1("K",fit_kaondecay,0.5,1+2*time_diff,5);
        pi_neg_Kall->SetParameters(par_neg[0],par_neg[1],par_neg[2]);
        K_neg_Kall->SetParameters(par_neg[1],par_neg[2],par_neg[3],par_neg[4],par_neg[5]);
        pi_neg_Kall->SetLineColor(kRed);
        K_neg_Kall->SetLineColor(kOrange);
        pi_neg_Kall->Draw("same");
        K_neg_Kall->Draw("same");
        TPaveText* pt_neg = new TPaveText(0.75,0.5,1,0.95,"brNDC");
        pt_neg->AddText(("RunGroup neg K "+std::to_string(RunGroup)).c_str());
        pt_neg->AddText(("shms p "+std::to_string(shms_p)).c_str());
        pt_neg->AddText(("A_{#pi} = "+std::to_string(par_neg[0])).c_str());
        pt_neg->AddText(("#mu_{#pi} = "+std::to_string(par_neg[1])).c_str());
        pt_neg->AddText(("#sigma_{#pi} = "+std::to_string(par_neg[2])).c_str());
        pt_neg->AddText(("Kaon time "+std::to_string(par_neg[1]+time_diff)).c_str());
        pt_neg->AddText(("A_{#pi} "+std::to_string(par_neg[3])).c_str());
        pt_neg->AddText(("#mu_{K} = "+std::to_string(par_neg[4])).c_str());
        pt_neg->AddText(("#sigma_{K} = "+std::to_string(par_neg[5])).c_str());
        pt_neg->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
        pt_neg->Draw();
        c_rftime_neg->Update();
        std::string c_rftime_neg_name = "results/pid/rftime_new/rftime_neg_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_low.pdf";
        c_rftime_neg->SaveAs(c_rftime_neg_name.c_str());



      }//if momentum less than 2.8
      /*
         TCanvas *c_pi_neg_2nd = new TCanvas();
      //c_pi_neg_2nd->SetGrid();
      gStyle->SetOptTitle(0);
      gStyle->SetPalette(kBird);
      h_rf_neg_piall->DrawCopy("hist");
      pi_neg_piall->GetParameters(&par_neg_pi[0]);
      K_neg_piall->GetParameters(&par_neg_pi[3]);
      double sigma_pi_neg = par_neg_pi[2];
      TLine *l_rf_lowsigma_neg = new TLine(1-3*sigma_pi_neg,0,1-3*sigma_pi_neg,1000);
      TLine *l_rf_highsigma_neg = new TLine(1+3*sigma_pi_neg,0,1+3*sigma_pi_neg,1000);
      l_rf_lowsigma_neg->Draw("same");
      l_rf_highsigma_neg->Draw("same");
      double neg_pi_N_sigma = all_neg_pi->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width_neg);
      //double neg_pi_N_sigma = pi_neg_piall->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width_neg);
      double neg_K_N_sigma = K_neg_piall->Integral(1-3*sigma_pi_neg,1+3*sigma_pi_neg,width_neg);
      std::cout<<"Pi number in fitting "<<neg_pi_N_sigma<<"K number in fitting "<<neg_K_N_sigma<<std::endl;
      TPaveText* pt_neg_pi_2nd = new TPaveText(0.75,0.5,1,0.95,"brNDC");
      pt_neg_pi_2nd->AddText(("RunGroup neg pi in 3 sigma "+std::to_string(RunGroup)).c_str());
      pt_neg_pi_2nd->AddText(("shms p "+std::to_string(shms_p)).c_str());
      //pt_neg_pi_2nd->AddText(("Pi: p0 = "+std::to_string(par_neg_pi[0])).c_str());
      pt_neg_pi_2nd->AddText(("#mu_{#pi} = "+std::to_string(par_neg_pi[1])).c_str());
      pt_neg_pi_2nd->AddText(("#sigma_{#pi} = "+std::to_string(par_neg_pi[2])).c_str());
      pt_neg_pi_2nd->AddText(("Kaon time "+std::to_string(1+time_diff)).c_str());
      //pt_neg_pi_2nd->AddText(("K: p0 = "+std::to_string(par_neg_pi[3])).c_str());
      pt_neg_pi_2nd->AddText(("#mu_{K} = "+std::to_string(par_neg_pi[4])).c_str());
      pt_neg_pi_2nd->AddText(("#sigma_{K} = "+std::to_string(par_neg_pi[5])).c_str());
      pt_neg_pi_2nd->AddText(("proton time "+std::to_string(1+time_diff_proton)).c_str());
      pt_neg_pi_2nd->Draw();
      c_pi_neg_2nd->Update();
      std::string c_pi_neg_2nd_name = "results/pid/rftime/rftime_neg_"+std::to_string(RunGroup)+"_"+std::to_string(i_dpcut)+"_pi_2nd.pdf";
      c_pi_neg_2nd->SaveAs(c_pi_neg_2nd_name.c_str());
      */
      i_dpcut = i_dpcut+1;
      delta_lowend = *it;
  }//different delta cut    
}//if normal production runs
std::string of_name = "results/pid/rftime_new/rf_eff_"+std::to_string(RunGroup)+"_low_compare.json";
std::ofstream ofs;
ofs.open(of_name.c_str());
ofs<<j_rungroup_info.dump(4)<<std::endl;
}
