#include <TString.h>
//void piplus_hgcer_eff(const char* polarity){//pos = +;neg =neg, posneg = +/- both
void piplus_hgcer_eff(){//pos = +;neg =neg, posneg = +/- both
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(1000011);
  gStyle->SetOptFit(1);
  gStyle->SetTitleOffset(1.,"Y");
  gStyle->SetTitleOffset(.7,"X");
  gStyle->SetLabelSize(0.04,"XY");
  gStyle->SetTitleSize(0.06,"XY");
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetErrorX(0);
  gStyle->SetOptStat(0);

  TChain *tt = new TChain("T");
  
   tt->Add("/home/hdbhatt/ROOTfiles/oct28_hgceff/all_pos_H2D2Al.root");
  //tt->Add(Form("/home/hdbhatt/ROOTfiles/oct28_hgceff/all_%s_H2D2Al.root",polarity));



  //cout<<"data entries = "<<nentriesD<<"  "<<endl;
   double bin_mom = 7;
   double bin_mom_lo = 3.0;
   double bin_mom_hi = 4.5;
   double mom_lo = 3.0;
   double mom_hi = 4.5;
   double pcal_pion = 0.7;
   double pcal_electron = 0.8;
   double rf_lo = 0.5;
   double rf_hi_pip = 1.5;//0.9;
   double rf_hi_pim = 1.2;
   double rf_hi_electron = 1.2;
   double hgcer_eff_Npe = 0.5;
   double accwidth = 24;
   double ctmin = 1.5;
   double ctmax = 1.5;

  
  Double_t hgcer_xAtcer,hgcer_yAtcer,ctime,pcaletottracknorm,pcaletracknorm,mom,hgcernpe,rftime;

  TH1F *pmom = new TH1F("pmom","Momentum (+VE);Momentum (GeV);Entries", 100, bin_mom_lo, bin_mom_hi);
  TH1F *hctime_acc_pre = new TH1F("hctime_acc_pre","Ctime (+VE);Ctime (ns);Entries", 160,20,80);
  TH1F *hctime_acc = new TH1F("hctime_acc","Ctime (+VE);Ctime (ns);Entries", 160,20,80);
  TH1F *hctime_coin = new TH1F("hctime_coin","Ctime (+VE);Ctime (ns);Entries ", 160,20,80);
  TH1F *hctime_clean = new TH1F("hctime_clean","Ctime;Clean Ctime (ns);Entries ", 160,20,80);
  TH1F *pcaleoverp = new TH1F("pcaleoverp","SHMS E/P Blue:Etracknorm:Red;E/P;Entries", 100, 0,2);
  TH1F *hpcaletracknorm = new TH1F("hpcaletracknorm","SHMS EtrackNorm;Etracknorm;Entries", 100, 0,2);
  TH1F *hgcNpe = new TH1F("hgcNpe","Hgcer NPe Sum;NpeSum;Entries", 100,0.2,40);
  TH1F *hgcNpe_acc = new TH1F("hgcNpe_acc","Hgcer NPe Sum;NpeSum;Entries", 100,0.2,40);
  TH1F *hrftime = new TH1F("hrftime","RFTime;RFTime;Entries", 100,0,4);

 
  TProfile2D * xy_vs_npe_grcut_with_hole = new TProfile2D("xy_vs_npe_grcut_with_hole","+VE: X vs Y  vs NPE (hole present, No Cer Cut);P.hgcer.yAtCer;P.hgcer.xAtCer;NPESUM", 300, -50, 50, 300, -50, 50, 0, 35);
  TProfile2D * xy_vs_npe_grcut_hole_removed = new TProfile2D("xy_vs_npe_grcut_hole_removed","X vs Y  vs NPE (hole removed);P.hgcer.yAtCer;P.hgcer.xAtCer;NPESUM", 300, -50, 50, 300, -50, 50, 0, 35);
  TProfile2D * xy_vs_npe_grcut_hole_removed_hgcer = new TProfile2D("xy_vs_npe_grcut_hole_removed_hgcer","X vs Y  vs NPE (hole removed: Npe > 0.5);P.hgcer.yAtCer;P.hgcer.xAtCer;NPESUM", 300, -50, 50, 300, -50, 50, 0, 35);

  TH1F * htotal_mom_before_remove_region_acc  = new TH1F( "htotal_mom_before_remove_region_acc","Total before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_before_remove_region_acc  = new TH1F( "hpass_mom_before_remove_region_acc","Passed before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * htotal_mom_after_remove_region_acc  = new TH1F( "htotal_mom_after_remove_region_acc","Total after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_after_remove_region_acc  = new TH1F( "hpass_mom_after_remove_region_acc","Passed after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 


  TH1F * htotal_mom_before_remove_region_coin  = new TH1F( "htotal_mom_before_remove_region_coin","Total before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_before_remove_region_coin  = new TH1F( "hpass_mom_before_remove_region_coin","Passed before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * htotal_mom_after_remove_region_coin  = new TH1F( "htotal_mom_after_remove_region_coin","Total after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_after_remove_region_coin  = new TH1F( "hpass_mom_after_remove_region_coin","Passed after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 

  TH1F * htotal_mom_before_remove_region_clean  = new TH1F( "htotal_mom_before_remove_region_clean","Total before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_before_remove_region_clean  = new TH1F( "hpass_mom_before_remove_region_clean","Passed before hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * htotal_mom_after_remove_region_clean  = new TH1F( "htotal_mom_after_remove_region_clean","Total after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 
  TH1F * hpass_mom_after_remove_region_clean  = new TH1F( "hpass_mom_after_remove_region_clean","Passed after hole removed (Mom); Momuntum (+VE); Entries", bin_mom, bin_mom_lo, bin_mom_hi); 

  TH1F *hEff = new TH1F("hEff","Hgcer Efficiency;Momentum (+VE) GeV;Efficiency", bin_mom, bin_mom_lo, bin_mom_hi);

 

  tt->SetBranchAddress("P_hgcer_xAtCer", &hgcer_xAtcer);
  tt->SetBranchAddress("P_hgcer_yAtCer", &hgcer_yAtcer);
  tt->SetBranchAddress("CTime_ePiCoinTime_ROC2", &ctime);
  tt->SetBranchAddress("P_cal_etottracknorm", &pcaletottracknorm);
  tt->SetBranchAddress("P_cal_etracknorm", &pcaletracknorm);
  tt->SetBranchAddress("shms_p", &mom);
  tt->SetBranchAddress("P_hgcer_npeSum", &hgcernpe);
  tt->SetBranchAddress("diff_time_mod", &rftime);


  Long64_t nentriesD = tt->GetEntries();
  
 
  Bool_t ctime_peak_finding,data_cut,central_hole_acc, right_or_left_of_hole_acc,central_hole, right_or_left_of_hole,remove_region_acc,remove_region,accL_cut,accR_cut,acc_cutdef,data_cut_coin_withhole,data_cut_acc_withhole, data_cut_coin_hole_removed,data_cut_acc_hole_removed,ctime_cut   ;
  for (int kk=0; kk<nentriesD;  kk++){
    tt->GetEntry(kk);
    central_hole_acc = (pow(hgcer_yAtcer-1.33, 2) +  pow(hgcer_xAtcer-0.83, 2) >= pow(6.0, 2));//radius = 6
    right_or_left_of_hole_acc = ((hgcer_yAtcer > 1.33 && (hgcer_xAtcer < 0. ||  hgcer_xAtcer > 3.0) ) ||  (hgcer_yAtcer < 1.33 && (hgcer_xAtcer < 0.||  hgcer_xAtcer > 3)));
    remove_region_acc = central_hole_acc && right_or_left_of_hole_acc;//ok

    ctime_peak_finding = pcaletottracknorm < pcal_pion  && mom >= mom_lo &&mom   <=  mom_hi  && remove_region_acc;//neg

    if (ctime_peak_finding){

      hctime_acc_pre->Fill(ctime);
    }
   }//entries
  Double_t bin_max =  hctime_acc_pre->GetMaximumBin();
  Double_t max_value =  hctime_acc_pre->GetBinCenter(bin_max);
  cout<<bin_max<<"   "<<max_value<<endl;


  for (int i=0; i<nentriesD;  i++){
  tt->GetEntry(i);
  central_hole = (pow(hgcer_yAtcer-1.33, 2) +  pow(hgcer_xAtcer-0.83, 2) >= pow(6.0, 2));//radius = 6
  right_or_left_of_hole = ((hgcer_yAtcer > 1.33 && (hgcer_xAtcer < 0. ||  hgcer_xAtcer > 3.0) ) ||  (hgcer_yAtcer < 1.33 && (hgcer_xAtcer < 0.||  hgcer_xAtcer > 3)));
  remove_region = central_hole && right_or_left_of_hole;//ok

  
    ctime_cut = ctime>(max_value-ctmin) && ctime<(max_value+ctmax);

     
    accL_cut        =   ctime > (max_value - 22 ) && ctime < (max_value - 10);//for 4 bunch put 22=>18
    accR_cut        =   ctime > (max_value + 14 ) && ctime < (max_value + 26);//for 4 bunch put 26=>22
    acc_cutdef = (accR_cut || accL_cut);


    data_cut =  pcaletottracknorm <pcal_pion    &&  mom >= mom_lo  &&mom <= mom_hi && rftime < rf_hi_pip ;

    
    data_cut_coin_withhole = data_cut   &&  ctime_cut;
    data_cut_acc_withhole  = data_cut   &&  acc_cutdef;

    data_cut_coin_hole_removed =  data_cut_coin_withhole && remove_region;
    data_cut_acc_hole_removed  =  data_cut_acc_withhole  && remove_region;

    //ctime with acc and Acc only
    if(data_cut && remove_region){hctime_coin->Fill(ctime);}//no ctime cut
    if(data_cut && acc_cutdef && remove_region){hctime_acc->Fill(ctime);}
    
    //eff from here
    
    //coin with hole
    if(data_cut_coin_withhole){
      htotal_mom_before_remove_region_coin->Fill(mom);
      if(hgcernpe> hgcer_eff_Npe){
	hpass_mom_before_remove_region_coin->Fill(mom);}
    }
    //acc with hole
    if(data_cut_acc_withhole){
      htotal_mom_before_remove_region_acc->Fill(mom);
      if(hgcernpe> hgcer_eff_Npe){
	hpass_mom_before_remove_region_acc->Fill(mom);}

    }

    //coin after hole removed
    if(data_cut_coin_hole_removed){
      pmom->Fill(mom);
      pcaleoverp->Fill(pcaletottracknorm);
      hpcaletracknorm->Fill(pcaletracknorm);
      hgcNpe->Fill(hgcernpe);
      hrftime->Fill(rftime);
      hctime_clean->Fill(ctime);

      htotal_mom_after_remove_region_coin->Fill(mom);
      if(hgcernpe> hgcer_eff_Npe){
	hpass_mom_after_remove_region_coin->Fill(mom);}
    }
    //acc after hole removed
    if(data_cut_acc_hole_removed){
      hgcNpe_acc->Fill(hgcernpe);

      htotal_mom_after_remove_region_acc->Fill(mom);
      if(hgcernpe> hgcer_eff_Npe){
	hpass_mom_after_remove_region_acc->Fill(mom);}
    }
    //2D plot
    if(data_cut&&ctime_cut){//1
      xy_vs_npe_grcut_with_hole->Fill(hgcer_yAtcer,hgcer_xAtcer,hgcernpe);
    }
    if(remove_region&&data_cut&&ctime_cut){//2
      xy_vs_npe_grcut_hole_removed->Fill(hgcer_yAtcer,hgcer_xAtcer,hgcernpe);
      if(hgcernpe> hgcer_eff_Npe){xy_vs_npe_grcut_hole_removed_hgcer->Fill(hgcer_yAtcer,hgcer_xAtcer,hgcernpe);}//3

    }//remove hole + hgcer cut
 
    
  }//entries

  cout<<"data entries = "<<nentriesD<<"  "<<endl;
  cout<<"good entries = "<<pmom->GetEntries()<<"   or   "<<pmom->Integral()<<endl;


  TH1F *   hpass_mom_hole_acc_1peak = (TH1F*) hpass_mom_before_remove_region_acc->Clone();
  TH1F *   htotal_mom_hole_acc_1peak = (TH1F*) htotal_mom_before_remove_region_acc->Clone();
  TH1F *   hpass_mom_holeremoved_acc_1peak = (TH1F*) hpass_mom_after_remove_region_acc->Clone();
  TH1F *   htotal_mom_holeremoved_acc_1peak = (TH1F*) htotal_mom_after_remove_region_acc->Clone();
  TH1F *   hgcNpe_acc_1peak = (TH1F*) hgcNpe_acc->Clone();


  hpass_mom_hole_acc_1peak->Scale((ctmin+ctmax)/accwidth);
  htotal_mom_hole_acc_1peak->Scale((ctmin+ctmax)/accwidth);
  hpass_mom_holeremoved_acc_1peak->Scale((ctmin+ctmax)/accwidth);
  htotal_mom_holeremoved_acc_1peak->Scale((ctmin+ctmax)/accwidth);
  hgcNpe_acc_1peak->Scale((ctmin+ctmax)/accwidth);

  hgcNpe_acc_1peak->Sumw2();
  hpass_mom_hole_acc_1peak->Sumw2();
  htotal_mom_hole_acc_1peak->Sumw2();
  hpass_mom_holeremoved_acc_1peak->Sumw2();
  htotal_mom_holeremoved_acc_1peak->Sumw2();

  cout<<"Hello ACc HERE"<<endl;
  cout<<"acc passed with hole = "<<hpass_mom_hole_acc_1peak->Integral()<<endl;
  cout<<"acc total with hole = "<<htotal_mom_hole_acc_1peak->Integral()<<endl;
  cout<<"acc passed removing hole = "<<hpass_mom_holeremoved_acc_1peak->Integral()<<endl;
  cout<<"acc total removed hole = "<<htotal_mom_holeremoved_acc_1peak->Integral()<<endl;
  cout<<"Hello ACC DONE"<<endl;
 

  //accidental
  TCanvas* c = new TCanvas("c","c",1000,800);
  c->Divide(2,2);
  c->cd(1);
  hpass_mom_hole_acc_1peak->Draw("hist");
  hpass_mom_hole_acc_1peak->SetTitle("Passed before hole removed (Mom): 4 Acc Plots (+VE)");
  c->cd(2);
  htotal_mom_hole_acc_1peak->Draw("hist");
  c->cd(3);
  hpass_mom_holeremoved_acc_1peak->Draw("hist");
  c->cd(4);
  htotal_mom_holeremoved_acc_1peak->Draw("hist");

  
  htotal_mom_before_remove_region_clean->Add(htotal_mom_before_remove_region_coin,htotal_mom_hole_acc_1peak,1,-1);
  hpass_mom_before_remove_region_clean->Add(hpass_mom_before_remove_region_coin,hpass_mom_hole_acc_1peak,1,-1);
  htotal_mom_after_remove_region_clean->Add(htotal_mom_after_remove_region_coin,htotal_mom_holeremoved_acc_1peak,1,-1);
  hpass_mom_after_remove_region_clean->Add(hpass_mom_after_remove_region_coin,hpass_mom_holeremoved_acc_1peak,1,-1);

  htotal_mom_before_remove_region_clean->Sumw2();
  hpass_mom_before_remove_region_clean->Sumw2();
  htotal_mom_after_remove_region_clean->Sumw2();
  hpass_mom_after_remove_region_clean->Sumw2();

  double total_bef_hole = htotal_mom_before_remove_region_clean->Integral();
  double total_after_hole_rem = htotal_mom_after_remove_region_clean->Integral();
  double passed_bef_hole = hpass_mom_before_remove_region_clean->Integral();
  double passed_after_hole_rem = hpass_mom_after_remove_region_clean->Integral();
  double frac_pion_lost_TOTAL_nohgccut = (total_bef_hole-total_after_hole_rem)/(total_bef_hole/100);
  double frac_pion_lost_PASSED_hgccut = (passed_bef_hole-passed_after_hole_rem)/(passed_bef_hole/100);
  //how many pions lost by removing cenrtral region???
  cout<<"% of Pions lost by removing hole: TOTAL = "<<frac_pion_lost_TOTAL_nohgccut<<"%."<<endl;
  cout<<"% of Pions lost by removing hole: PASSED = "<<frac_pion_lost_PASSED_hgccut<<"%."<<endl;


  cout<<"passed_Int_before_holeRemoved (6 acc peak) = "<<hpass_mom_hole_acc_1peak->Integral()<<endl;
  cout<<"passed_Int_after_holeRemoved (6 acc peak) = "<<hpass_mom_holeremoved_acc_1peak->Integral()<<endl;
  cout<<""<<endl;
  cout<<"passed_Coin_Int_before_holeRemoved (6 acc peak) = "<<hpass_mom_before_remove_region_clean->Integral()<<endl;
  cout<<"passed_Coin_Int_after_holeRemoved  (6 acc peak) = "<<hpass_mom_after_remove_region_clean->Integral()<<endl;


  //////final

  TEfficiency *pEff_mom_before_remove_region = new TEfficiency();
  TEfficiency *pEff_mom_after_remove_region = new TEfficiency();
      
    


  pEff_mom_before_remove_region = new TEfficiency(*hpass_mom_before_remove_region_clean, *htotal_mom_before_remove_region_clean); 
  pEff_mom_after_remove_region  = new TEfficiency(*hpass_mom_after_remove_region_clean,  *htotal_mom_after_remove_region_clean); 

 
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  c1->Divide(3,3);
  c1->cd(1);
  hctime_acc_pre->Draw();
  hctime_acc_pre->SetLineWidth(2);

  c1->cd(2);
  hctime_coin->Draw();
  hctime_coin->SetLineWidth(2);
  hctime_clean->Draw("same");
  hctime_clean->SetLineWidth(2);
  hctime_clean->SetLineColor(kGreen);
  hctime_acc->Draw("same");
  hctime_acc->SetLineWidth(2);
  hctime_acc->SetLineColor(kRed);

  c1->cd(3);
  pmom->Draw();
  pmom->SetLineWidth(2);

  c1->cd(4);
  xy_vs_npe_grcut_with_hole->Draw("colz");

  c1->cd(5);
  xy_vs_npe_grcut_hole_removed->Draw("colz");
 
  c1->cd(6);
  xy_vs_npe_grcut_hole_removed_hgcer->Draw("colz");

  c1->cd(7);
  //pcaleoverp->Draw();
  pcaleoverp->SetLineWidth(2);
  hpcaletracknorm->Draw("");
  hpcaletracknorm->SetLineWidth(2);
  hpcaletracknorm->SetLineColor(kRed);
  pcaleoverp->Draw("same");

  c1->cd(8);
  hgcNpe->Draw("hist");
  hgcNpe->SetLineWidth(2);
  hgcNpe_acc_1peak->Draw("hist sames");
  hgcNpe_acc_1peak->SetLineWidth(2);
  hgcNpe_acc_1peak->SetLineColor(kRed);

  c1->cd(9);
  hrftime->Draw();
  hrftime->SetLineWidth(2);
  



  TCanvas *c2 = new TCanvas("c2","c2",1000,800);
  c2->Divide(3,2);
  c2->cd(1);
  htotal_mom_before_remove_region_clean->Draw("hist");
  htotal_mom_before_remove_region_clean->SetLineWidth(2);
  htotal_mom_hole_acc_1peak->Draw("hist sames");
  htotal_mom_hole_acc_1peak->SetLineWidth(2);
  htotal_mom_hole_acc_1peak->SetLineColor(kRed);

  c2->cd(2);
  hpass_mom_before_remove_region_clean->Draw("hist");
  hpass_mom_before_remove_region_clean->SetLineWidth(2);
  hpass_mom_hole_acc_1peak->Draw("hist sames");
  hpass_mom_hole_acc_1peak->SetLineWidth(2);
  hpass_mom_hole_acc_1peak->SetLineColor(kRed);

  c2->cd(3);
  pEff_mom_before_remove_region->Draw();
  pEff_mom_before_remove_region->SetMarkerStyle(20);
  pEff_mom_before_remove_region->SetLineWidth(2);

  c2->cd(4);
  htotal_mom_after_remove_region_clean->Draw("hist");
  htotal_mom_after_remove_region_clean->SetLineWidth(2);
  htotal_mom_holeremoved_acc_1peak->Draw("hist sames");
  htotal_mom_holeremoved_acc_1peak->SetLineWidth(2);
  htotal_mom_holeremoved_acc_1peak->SetLineColor(kRed);

  c2->cd(5);
  hpass_mom_after_remove_region_clean->Draw("hist");
  hpass_mom_after_remove_region_clean->SetLineWidth(2);
  hpass_mom_holeremoved_acc_1peak->Draw("hist sames");
  hpass_mom_holeremoved_acc_1peak->SetLineWidth(2);
  hpass_mom_holeremoved_acc_1peak->SetLineColor(kRed);

  c2->cd(6);
  pEff_mom_after_remove_region->Draw();
  pEff_mom_after_remove_region->SetMarkerStyle(20);
  pEff_mom_after_remove_region->SetLineWidth(2);



  TCanvas *c3 = new TCanvas("c3","c3",1000,800);
  c3->Divide(1,1);
  c3->cd(1);gPad->SetGrid();
  hEff->Draw();
  hEff->GetYaxis()->SetRangeUser(0.95,1);
  pEff_mom_after_remove_region->Draw("same");
  pEff_mom_after_remove_region->SetMarkerStyle(20);
  pEff_mom_after_remove_region->SetLineWidth(2);

  c1->SaveAs("pdf/base_plots_pos.pdf");
  c2->SaveAs("pdf/eff_plots_pos.pdf");
  c3->SaveAs("pdf/finaleff_pos.pdf");
  c->SaveAs("pdf/accidental_only_pos.pdf");

  
}//void
