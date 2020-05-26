#include "nlohmann/json.hpp"
void trackingeff(int RunNumber=0){
   if (RunNumber == 0){
     cout<<"enter a run number:";
     cin >> RunNumber;
     if(RunNumber <= 0)
     return;
     }
//TString filename = Form("ROOTfiles/coin_replay_production_%d_%d.root", RunNumber);
   std::string filename = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
   TFile *f = new TFile(filename.c_str(), "READ");
TTree *tdata = (TTree*) f->Get("T");
TTree *tscal = (TTree*) f->Get("TSP");

double evNum;
double bcm1_q;
double bcm1_i;
double s1x_scal;
double s1y_scal;
double s2x_scal;
double s2y_scal;
double elclean;
double pTRIG1;
double pTRIG2;
double pTRIG3;
double pTRIG4;
double pTRIG6;
double scal_time;
double l1accept;
//double setCur= 63;
double setCur ;
double th_current = 3.; //3 uA
double prev_time=0.;
double prev_char=0.;
double prev_elclean=0.;
double prev_s1x=0.;
double prev_s1y=0.;
double prev_s2x=0.;
double prev_s2y=0.;
double prev_trig1=0.;
double prev_trig2=0.;
double prev_trig3=0.;
double prev_trig4=0.;
double prev_trig6=0.;
double prev_pedtm_scaler=0.;
double prev_l1accept=0.;
double tot_time=0.;
double tot_char=0.;
double tot_s1x=0.;
double tot_s2x=0.;
double tot_s1y=0.;
double tot_s2y=0.;
double tot_elclean=0.;
double tot_trig1=0.;
double tot_trig2=0.;
double tot_trig3=0.;
double tot_trig4=0;
double tot_trig6=0.;
double total_l1accept=0.;
double total_elclean_cut=0.;
double total_scal_q_cut=0.;
double total_time_cut=0.;
double total_s1x_cut=0.;
double total_s2x_cut=0.;
double total_s1y_cut=0.;
double total_s2y_cut=0.;
double total_ptrig1_cut=0.;
double total_ptrig2_cut=0.;
double total_ptrig3_cut=0.;
double total_ptrig4_cut;
double total_ptrig6_cut=0.;
double total_pedtm_scaler=0.;
double total_pedtm_scaler_cut=0.;
double total_l1accept_cut=0.;
double xptar;
double ytar;
double png_npeSum;
double petottracknorm;
double petotnorm;
double pdelta;
double ntrack;
double evtyp;
double evNum_data;
double pEDTM_scaler;
double pBetanotrk;
double pGoodScinHit;
double shms_singles=0.;
double shms_coin=0.;
//int ps2_factor = 129.;
double peprtracknorm;
double tnhit;
double ptrig6_data;
double ptrig2_data;
double edtm;
double pelclean;
using nlohmann::json;
json j;
{
std::ifstream json_input_file("db2/run_list_coin.json");
json_input_file >> j;
}

std::string run = std::to_string(RunNumber);
int ps2 = j[run.c_str()]["daq"]["ps2"].get<int>();
int ps2_factor = (ps2 == 0) ? 1. :(std::pow(2, ps2 - 1) + 1);
tscal->SetBranchAddress("evNumber", &evNum);
tscal->SetBranchAddress("P.BCM1.scalerCharge", &bcm1_q);
tscal->SetBranchAddress("P.BCM1.scalerCurrent", &bcm1_i);
tscal->SetBranchAddress("P.S1X.scaler", &s1x_scal);
tscal->SetBranchAddress("P.S1Y.scaler", &s1y_scal);
tscal->SetBranchAddress("P.S2X.scaler", &s2x_scal);
tscal->SetBranchAddress("P.S2Y.scaler", &s2y_scal);
tscal->SetBranchAddress("P.pTRIG1.scaler", &pTRIG1);
tscal->SetBranchAddress("P.pTRIG2.scaler", &pTRIG2);
tscal->SetBranchAddress("P.pTRIG3.scaler", &pTRIG3);
tscal->SetBranchAddress("P.pTRIG4.scaler", &pTRIG4);
tscal->SetBranchAddress("P.pTRIG6.scaler", &pTRIG6);
tscal->SetBranchAddress("P.1MHz.scalerTime", &scal_time);
tscal->SetBranchAddress("P.EDTM.scaler",&pEDTM_scaler);
tscal->SetBranchAddress("P.pEL_CLEAN.scaler", &elclean); 
tscal->SetBranchAddress("P.pL1ACCP.scaler",&l1accept);
TH1F *hcurrent = new TH1F("current","",1000, 10, 100);
const int  tscal_entries =tscal->GetEntries();
int ev_flag[tscal_entries];
int scal_evt_num[tscal_entries];
 
 for (int j = 0; j < tscal_entries; j++){ 
   tscal->GetEntry(j);
   hcurrent->Fill(bcm1_i);
 }

 for (int i = 0; i < tscal_entries; i++){ 
   tscal->GetEntry(i);
   ev_flag[i] = 0;
   scal_evt_num[i] = evNum;
   setCur = hcurrent->GetBinCenter(hcurrent->GetMaximumBin());
   tot_time=scal_time;
   tot_char=bcm1_q; 
   tot_s1x=s1x_scal;
   tot_s1y=s1y_scal;
   tot_s2x=s2x_scal;
   tot_s2y=s2y_scal;
   tot_elclean = elclean;
   tot_trig1=pTRIG1;
   tot_trig2=pTRIG2;
   tot_trig3=pTRIG3;
   tot_trig4=pTRIG4;
   tot_trig6=pTRIG6;
   total_pedtm_scaler=pEDTM_scaler;
   total_l1accept = l1accept;

   if(abs(s1x_scal-prev_s1x) < 100000000.){
   if(abs(setCur - bcm1_i) < th_current){
     ev_flag[i] = 1; // turn the flag on
   
     total_time_cut = total_time_cut + (scal_time-prev_time);
     total_scal_q_cut = total_scal_q_cut + (bcm1_q-prev_char);
     total_s1x_cut = total_s1x_cut + (s1x_scal-prev_s1x);
     total_s1y_cut = total_s1y_cut + (s1y_scal-prev_s1y);
     total_s2x_cut = total_s2x_cut + (s2x_scal-prev_s2x);
     total_s2y_cut = total_s2y_cut + (s2y_scal-prev_s2y);
     total_ptrig1_cut = total_ptrig1_cut + (pTRIG1-prev_trig1);
     total_ptrig2_cut = total_ptrig2_cut + (pTRIG2-prev_trig2);
     total_ptrig3_cut = total_ptrig3_cut + (pTRIG3-prev_trig3);
     total_ptrig4_cut = total_ptrig4_cut + (pTRIG4-prev_trig4);
     total_ptrig6_cut = total_ptrig6_cut + (pTRIG6-prev_trig6);
     total_pedtm_scaler_cut = total_pedtm_scaler_cut + (pEDTM_scaler-prev_pedtm_scaler); 
     total_elclean_cut = total_elclean_cut + (elclean - prev_elclean);
     total_l1accept_cut = total_l1accept_cut + (l1accept - prev_l1accept);
   }
   
   prev_time = scal_time;
   prev_char = bcm1_q;
   prev_s1x = s1x_scal;
   prev_s1y = s1y_scal;
   prev_s2x = s2x_scal;
   prev_s2y = s2y_scal;
   prev_trig1 = pTRIG1;
   prev_trig2 = pTRIG2;
   prev_trig3 = pTRIG3;
   prev_trig4 = pTRIG4;
   prev_trig6 = pTRIG6;
   prev_elclean = elclean;
   prev_pedtm_scaler = pEDTM_scaler;
   prev_l1accept = l1accept;
   }  
 }

 double pS1X_rate = total_s1x_cut/total_time_cut/1000.;
 double pS1Y_rate = total_s1y_cut/total_time_cut/1000.;
 double pS2X_rate = total_s2x_cut/total_time_cut/1000.;
 double pS2Y_rate = total_s2y_cut/total_time_cut/1000.;
 double pTRIG1_rate = total_ptrig1_cut/total_time_cut/1000.;
 double pTRIG2_rate = total_ptrig2_cut/total_time_cut/1000.;
 double pTRIG3_rate = total_ptrig3_cut/total_time_cut/1000.;
 double pTRIG4_rate = total_ptrig4_cut/total_time_cut/1000.;
 double pTRIG6_rate = total_ptrig6_cut/total_time_cut/1000.;
 double pEDTM_rate = total_pedtm_scaler_cut/total_time_cut/1000.;
 double elclean_rate = total_elclean_cut/total_time_cut/1000.;
 double l1accept_rate = total_l1accept_cut/total_time_cut/1000.;
 pS1X_rate = pS1X_rate - pEDTM_rate;
 pS1Y_rate = pS1Y_rate - pEDTM_rate;
 pS2X_rate = pS2X_rate - pEDTM_rate;
 pS2Y_rate = pS2Y_rate - pEDTM_rate;
 pTRIG1_rate = pTRIG1_rate - pEDTM_rate;
 pTRIG2_rate = pTRIG2_rate - pEDTM_rate;
 pTRIG3_rate = pTRIG3_rate - pEDTM_rate;
 pTRIG4_rate = pTRIG4_rate - pEDTM_rate;
 pTRIG6_rate = pTRIG6_rate - pEDTM_rate;

 /*cout<<"set current: "<<setCur<<" "<<"charge: "<<total_scal_q_cut<<" "<<"time: "<<scal_time<<" "<<"S1X rate: "<<" "<<pS1X_rate<<" "<<"S1Y rate: "<<pS1Y_rate<<" "<<"S2X rate: "<<pS2X_rate<<" "<<"S2Y rate: "<<pS2Y_rate<<" "<<"TRIG1 rate: "<<pTRIG1_rate<<" "<<"el clean rate " <<elclean_rate<<endl;
*/
cout<<l1accept_rate<<endl; 
 //=====DATA=====
 tdata->SetBranchAddress("P.ngcer.npeSum",&png_npeSum); 
 tdata->SetBranchAddress("P.cal.etottracknorm",&petottracknorm);
 tdata->SetBranchAddress("P.cal.eprtracknorm",&peprtracknorm);
 tdata->SetBranchAddress("P.cal.etotnorm",&petotnorm);
 tdata->SetBranchAddress("P.gtr.dp",&pdelta);
 tdata->SetBranchAddress("P.gtr.th",&xptar);
 tdata->SetBranchAddress("P.gtr.y",&ytar);
 tdata->SetBranchAddress("P.dc.ntrack", &ntrack);
 tdata->SetBranchAddress("g.evtyp", &evtyp);
 tdata->SetBranchAddress("g.evnum", &evNum_data);
 tdata->SetBranchAddress("P.hod.betanotrack",&pBetanotrk);    
 tdata->SetBranchAddress("P.hod.goodscinhit",&pGoodScinHit);
 tdata->SetBranchAddress("P.dc.tnhit",&tnhit);
 tdata->SetBranchAddress("T.coin.pEDTM_tdcTimeRaw",&edtm);
 tdata->SetBranchAddress("T.coin.pTRIG6_ROC2_tdcTimeRaw",&ptrig6_data);
 tdata->SetBranchAddress("T.coin.pTRIG2_ROC2_tdcTimeRaw",&ptrig2_data);
 tdata->SetBranchAddress("T.coin.pEL_CLEAN_ROC2_tdcTimeRaw",&pelclean);
 TH1F *hod = new TH1F("hod", "", 100, 0.01, 3.);
 TH1F *hodcut = new TH1F("hodcut", "", 100, 0.01, 3.);
 TH1F *cal = new TH1F("etotnorm", "", 100, 0.01, 2.0);
 TH1F *calcut = new TH1F("etotnormcut", "", 100, 0.01, 2.0);
 TH1F *delta = new TH1F("delta", "", 100, -30., 30.);
 TH1F *deltacut = new TH1F("deltacut", "", 100, -30., 30.);
 TH1F *ntrack0 = new TH1F("track", "", 100, -5, 10);
 TH1F *ntrack_non0 = new TH1F("tracknon0", "", 100, -5, 10); 
 TH1F *nhit0 = new TH1F("nhit0","",100, -5, 100);
 TH1F *nhitnon0 = new TH1F("nhitnon0","",100, -5, 100);
 double eshould = 0.;
 double edid = 0.;
 double acc_trig6_cut = 0.;
 double edtm_singles = 0.;
 double edtm_coin = 0.;
 int scal = 0;   //scaler read counter
 const int  tdata_entries =tdata->GetEntries();

 
 for (int j = 0; j < tdata_entries; j++){
   tdata->GetEntry(j);
    if(ev_flag[scal]==1){
     if(pGoodScinHit==1. && pBetanotrk > 0.5 && pBetanotrk < 1.4 && petotnorm > 0.6 && petotnorm < 1.6){
       eshould++;
       if(ntrack > 0.){
	     edid++;
       } //end e_did
     }//end possible track cuts
      //count the accepted ptrig6 events
      if(ptrig6_data > 0.){
      acc_trig6_cut++;
      }
       //count accepted edtm events
      if(edtm > 0.){
        if(evtyp==1){
         edtm_singles++;
        };
        if(evtyp==4){
          if(pelclean>0.){
          edtm_coin++;
        };
    }
}
      if(edtm == 0.){
        if(petottracknorm > 0.65 && pdelta > -10. && pdelta < 22. && abs(xptar) < 0.06 && abs(ytar) < 4.&& peprtracknorm > 0.05){
          if(evtyp==1){
           shms_singles++; 
       } // end event type 1    
       if(evtyp==4 && pelclean>0.){
       shms_coin++;
       } // end event type 4
   } // end cuts on yield
  } // end no edtm cut
}//end bcm flag

  if(evNum_data >  scal_evt_num[scal]){
     scal++;    
  }
     //cout<<scal<<endl;

 } //end event entry loop
 
 
 double  el_trackeff = edid/eshould;
 double err_el_trackeff = sqrt(el_trackeff)*(sqrt(eshould-edid)/eshould);
 double comp_lt = acc_trig6_cut / total_ptrig6_cut;
 double comp_lt_error = TMath::Sqrt(acc_trig6_cut) / total_ptrig6_cut;
 double tot_lt = (edtm_singles*ps2_factor +  edtm_coin) / total_pedtm_scaler_cut;
 //double tot_lt_error = TMath::Sqrt(((ps2_factor*ps2_factor*edtm_singles*(total_pedtm_scaler_cut-edtm_singles))+(edtm_coin*(total_pedtm_scaler_cut-edtm_coin)))/(total_pedtm_scaler_cut*total_pedtm_scaler_cut*total_pedtm_scaler_cut)); 
 double tot_lt_error = TMath::Sqrt(((edtm_coin*(total_pedtm_scaler_cut-edtm_coin)))/(total_pedtm_scaler_cut*total_pedtm_scaler_cut*total_pedtm_scaler_cut));
 double yield = (shms_singles*ps2_factor + shms_coin)/(total_scal_q_cut*el_trackeff*tot_lt);
 double err_yield = TMath::Sqrt( (shms_singles*ps2_factor*ps2_factor) + (shms_coin))/(el_trackeff*total_scal_q_cut*tot_lt);

 //cout<<" "<<ps2_factor*ps2_factor*edtm_singles*(total_pedtm_scaler_cut-edtm_singles)<<" "<<""<<edtm_coin*(total_pedtm_scaler_cut-edtm_coin)<<" "<<total_pedtm_scaler_cut*total_pedtm_scaler_cut*total_pedtm_scaler_cut<<endl;
 cout<<RunNumber<<" "<<eshould<<" "<<edid<<" "<<el_trackeff<< " "<<err_el_trackeff<<" "<<" "<<edtm_singles<<" "<<edtm_coin<<" "<<total_pedtm_scaler_cut<<" "<<tot_lt<<" "<<tot_lt_error<<" "<<shms_singles<<" "<<shms_coin<<" "<<yield<<" "<<err_yield<<endl;
 
 using nlohmann::json;
 json jmake;
 std::string run_str  = std::to_string(RunNumber);
 jmake[run_str]["coin trigger events"]  = shms_coin;
 jmake[run_str]["shms singles trigger events"]  = shms_singles;
 jmake[run_str]["tracking eff"]  = el_trackeff;
 jmake[run_str]["tracking eff error"]  = err_el_trackeff;
 jmake[run_str]["yield"]  = yield;
 jmake[run_str]["yield error"]  = err_yield;
 jmake[run_str]["total live time"]  = tot_lt;
 jmake[run_str]["total live time error"] = tot_lt_error;
 jmake[run_str]["comp live time"]  = comp_lt;
 jmake[run_str]["comp live time error"]  = comp_lt_error;
 jmake[run_str]["set current"]  = setCur;
 jmake[run_str]["charge"] = total_scal_q_cut;
 jmake[run_str]["s1x rate"] = pS1X_rate;
 jmake[run_str]["s1y rate"] = pS1Y_rate;
 jmake[run_str]["s2x rate"] = pS2X_rate;
 jmake[run_str]["s2y rate"] = pS2Y_rate;
 jmake[run_str]["s1x rate"] = pS1X_rate;
 jmake[run_str]["total time"] = scal_time;
 jmake[run_str]["bcm1 current"] = bcm1_i;
 jmake[run_str]["SHMS 3/4 rates"] = pTRIG1_rate;
 jmake[run_str]["HMS 3/4 rates"] = pTRIG4_rate;
 jmake[run_str]["SHMS ELCLEAN"] = pTRIG2_rate;
 jmake[run_str]["HMS ELCLEAN"] = pTRIG3_rate;
 jmake[run_str]["SHMS el clean scaler rate"] = elclean_rate;
 jmake[run_str]["ps value"] = ps2;
 jmake[run_str]["ps factor"] = ps2_factor;
 jmake[run_str]["edtm singles"] = edtm_singles;
 jmake[run_str]["edtm coin"] = edtm_coin;
 jmake[run_str]["accepted coin events"] = acc_trig6_cut;
 jmake[run_str]["scaler coin triggers"] = total_ptrig6_cut;
 jmake[run_str]["total scaler edtm events"] = total_pedtm_scaler_cut;
 jmake[run_str]["l1 accept rate"] = l1accept_rate;
 std::string jsonname = "phase4/yield"+std::to_string(RunNumber)+".json";
 std::ofstream o2(jsonname);
 o2<< std::setw(4) << jmake << "\n";
 
 }

