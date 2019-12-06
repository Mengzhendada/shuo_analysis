
     auto h_neg = d_neg_raw.Histo1D({"","all;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     auto h_pos = d_pos_raw.Histo1D({"","all;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     auto h_neg_e = d_neg_e.Histo1D({"","cer>5;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     //int n_e_neg_cer = h_neg_e->Integral(1,100);
     //int n_e_neg_cer = *d_neg_e.Count();
     int n_e_neg_cer = h_neg_e->GetEntries();
     auto h_pos_e = d_pos_e.Histo1D({"","cer>5;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     int n_e_pos_cer = h_pos_e->GetEntries();
     auto h_neg_pion = d_neg_pion.Histo1D({"","cer==0;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     int n_pion_neg_cer = h_neg_pion->GetEntries();
     auto h_pos_pion = d_pos_pion.Histo1D({"","cer==0;E/p;counts",100,0.1,2},"H.cal.etottracknorm");
     int n_pion_pos_cer = h_pos_pion->GetEntries();
     auto h_neg_cer = d_neg.Histo1D({"","cer;npeSum;counts",100,0.1,2},"H.cer.npeSum");
     double e_neg[n_cuts],pion_neg[n_cuts],e_pos[n_cuts],pion_pos[n_cuts];
     for(int i = 0;i<n_cuts;++i){
       TAxis *axis_neg_e = h_neg_e->GetXaxis();
       int min_neg_e = h_neg_e->FindBin(cal_cut_low[i]);
       int max_neg_e = h_neg_e->FindBin(cal_cut_high[i]);
       e_neg[i] = h_neg_e->Integral(min_neg_e,max_neg_e);
       
       TAxis *axis_neg_pion = h_neg_pion->GetXaxis();
       int min_neg_pion = axis_neg_pion->FindBin(cal_cut_low[i]);
       int max_neg_pion = axis_neg_pion->FindBin(cal_cut_high[i]);
       pion_neg[i] = h_neg_pion->Integral(min_neg_pion,max_neg_pion);

       TAxis *axis_pos_e = h_pos_e->GetXaxis();
       int min_pos_e = h_pos_e->FindBin(cal_cut_low[i]);
       int max_pos_e = h_pos_e->FindBin(cal_cut_high[i]);
       e_pos[i] = h_pos_e->Integral(min_pos_e,max_pos_e);
       std::cout<<"pos runs electrons "<<e_pos[i]<<std::endl;
       TAxis *axis_pos_pion = h_pos_pion->GetXaxis();
       int min_pos_pion = axis_pos_pion->FindBin(cal_cut_low[i]);
       int max_pos_pion = axis_pos_pion->FindBin(cal_cut_high[i]);
       pion_pos[i] = h_pos_pion->Integral(min_pos_pion,max_pos_pion);
       double neg_e_eff = e_neg[i]/n_e_neg_cer;
       double pos_e_eff = e_pos[i]/n_e_pos_cer;
       double neg_pion_rej = n_pion_neg_cer/pion_neg[i];
       double pos_pion_rej = n_pion_pos_cer/pion_pos[i];
     }//different cal cut
     TGraphErrors* g_neg_e = new TGraphErrors();
     TGraphErrors* g_pos_e = new TGraphErrors();
     TGraphErrors* g_neg_pion = new TGraphErrors();
     TGraphErrors* g_pos_pion = new TGraphErrors();
     g_neg_e->SetTitle("HMS cal electron efficiency;cal cut;percentage");
     g_neg_e->SetMarkerStyle(8);
     g_neg_e->SetMarkerColor(1);
     g_neg_e->SetLineColor(1);

     g_neg_pion->SetTitle("HMS cal pion rejection factor;cal cut; rejection");
     g_neg_pion->SetMarkerStyle(8);
     g_neg_pion->SetMarkerColor(2);
     g_neg_pion->SetLineColor(2);

     gStyle->Reset("Modern");

     TCanvas *c_neg = new TCanvas();
     TPad *p1_neg = new TPad("p1_neg","",0,0,1,1);
     p1_neg->SetGrid();
     TPad *p2_neg = new TPad("p2_neg","",0,0,1,1);
     p2_neg->SetFillStyle(4000);

     p1_neg->Draw();
     p1_neg->cd();
     g_neg_e->Draw("ALP");
     g_neg_e->GetHistogram()->GetXaxis()->SetTitleOffset(1.25);
     g_neg_e->GetHistogram()->GetYaxis()->SetTitleOffset(1.25);
     gPad->Update();

     p2_neg->Draw();
     p2_neg->cd();
     g_neg_pion->Draw("LP");
     gPad->Update();
     
     c_neg->SaveAs("results/HMS_CAL_PID.pdf");
