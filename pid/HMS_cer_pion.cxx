#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TLine.h"
#include "TH2.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void HMS_cer_pion(int RunGroup = 0){
  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup < 0)
      return;
  }
  json j_cuts;
  {
    std::ifstream ifs("db2/HMS_cuts.json");
    ifs>>j_cuts;
  }
  std::vector<int> coolcolor;
  coolcolor = j_cuts["coolcolor"].get<std::vector<int>>();
  double cal_pi_low = j_cuts["cal_pi_low"].get<double>();
  double cal_pi_high = j_cuts["cal_pi_high"].get<double>();
  std::string cal_pion_cut = "H.cal.etottracknorm > "+std::to_string(cal_pi_low)+" && H.cal.etottracknorm < "+std::to_string(cal_pi_high);
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  RunGroup = 10*RunGroup;
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  double HMS_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
  std::cout<<HMS_P<<std::endl;
  double p_min = HMS_P*0.9;
  double p_max = HMS_P*1.1;
  auto hms_p_calculate = [HMS_P](double hms_dp){return  HMS_P*(1+hms_dp/100);};
  if(!neg_D2.empty() && !pos_D2.empty()){
    std::vector<std::string> files_neg,files_pos;
    for(std::vector<int>::iterator it = neg_D2.begin();it!=neg_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_neg.push_back(rootfile_name);
    }
    for(std::vector<int>::iterator it = pos_D2.begin();it!=pos_D2.end();++it){
      int RunNumber = *it;
      std::cout<<RunNumber<<std::endl;
      std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
      files_pos.push_back(rootfile_name);
    }
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d_neg_raw("T",files_neg);
    ROOT::RDataFrame d_pos_raw("T",files_pos);
    auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22").Filter("fEvtHdr.fEvtType == 4");
    auto d_neg_withe = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10").Define("hms_p",hms_p_calculate,{"H.gtr.dp"});
    auto d_neg = d_neg_withe.Filter(cal_pion_cut.c_str());
    auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22").Filter("fEvtHdr.fEvtType == 4");
    auto d_pos_withe = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10").Define("hms_p",hms_p_calculate,{"H.gtr.dp"});
    auto d_pos = d_pos_withe.Filter(cal_pion_cut.c_str());
        
  //  auto h_p = d_neg.Histo1D({"","HMS momentum;momentum;counts",100,-10,10},"H.gtr.dp");
    auto h_p_neg = d_neg.Histo1D({"","HMS momentum;momentum;counts",100,p_min,p_max},"hms_p");
    auto h_cer_dp_neg = d_neg.Histo2D({"","HMS cer npeSum vs dp;dp;cer npesum",100,-10,10,100,0,30},"H.gtr.dp","H.cer.npeSum");
    auto h_cer_p_neg = d_neg.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
    auto h_cer_all_neg = d_neg.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");

    double P_pion_threshold = 4.8;
      if(P_pion_threshold<p_min)
        P_pion_threshold = p_min;
    TLine *l_p_pion_threshold = new TLine(4.6,0,4.6,30);
    int n_dp = 10;
    double p_delta = (p_max-P_pion_threshold)/n_dp;
    std::vector<double> p_dp;
//    using Histo1D = decltype(ROOT::RDataFrame{0}.Histo1D(ROOT::RDF::TH1DModel{"","",128u,0.,0.},""));
//    std::vector<Histo1D> h_cer_p;
//    std::vector<Histo1D> h_p;
//    std::vector<TLine> l_p;
    TH1D *h_histo_neg[n_dp];
    TH1D *h_cer_neg[n_dp];
    for(int i = 0;i<n_dp;++i){
      double p = P_pion_threshold+i*p_delta;
      p_dp.push_back(p);
      auto d_neg_cuts = d_neg.Filter(("hms_p > "+std::to_string(p_dp[i]) +" && "+"hms_p < "+std::to_string(p_dp[i]+p_delta)).c_str());
      auto t = d_neg_cuts.Histo1D({"","",100,p_min,p_max},"hms_p");
      t->SetLineColor(coolcolor[i]);
      t->SetFillColorAlpha(coolcolor[i],0.35);
//      std::cout << "type : " << typeid(t).name() << std::endl;
//      std::cout << "type : " << typeid(t.GetPtr()).name() << std::endl;
//      std::cout << "type : " << typeid(*t.GetPtr()).name() << std::endl;
      h_histo_neg[i] = new TH1D(*t.GetPtr());
      //h_histo[i] = t;
      auto h = d_neg_cuts.Histo1D({"","",100,0,30},"H.cer.npeSum");
      h->SetLineColor(coolcolor[i]);
      h->SetFillColorAlpha(coolcolor[i],0.35);
      h_cer_neg[i] = new TH1D(*h.GetPtr());
    }
    auto h_cer_dpcut_neg = d_neg.Filter("hms_p < 4.6").Histo1D({"","",100,0,30},"H.cer.npeSum");
    TCanvas *c_neg = new TCanvas();
    c_neg->Divide(2,2);
    c_neg->cd(1);
    h_p_neg->Draw("hist");
    for(int i = 0;i<n_dp;++i){
    h_histo_neg[i]->Draw("hist same");
    }
    c_neg->cd(2);
//    h_cer_dp_neg->Draw("colz");
    h_cer_all_neg->Draw("hist");
    h_cer_dpcut_neg->Draw("hist same");
    c_neg->cd(3);
    h_cer_p_neg->Draw("colz");
    c_neg->cd(4);
    //h_cer_all_neg->Draw("hist");
    for(int i = 0;i<n_dp;++i){
      h_cer_neg[i]->Draw("hist same");
    }
    std::string c_neg_name = "results/pid/HMS_cer_pion_"+std::to_string(RunGroup)+".pdf";
    c_neg->SaveAs(c_neg_name.c_str());

    auto h_p_pos = d_pos.Histo1D({"","HMS momentum;momentum;counts",100,p_min,p_max},"hms_p");
    auto h_cer_dp_pos = d_pos.Histo2D({"","HMS cer npeSum vs dp;dp;cer npesum",100,-10,10,100,0,30},"H.gtr.dp","H.cer.npeSum");
    auto h_cer_p_pos = d_pos.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
    auto h_cer_all_pos = d_pos.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");

//    using Histo1D = decltype(ROOT::RDataFrame{0}.Histo1D(ROOT::RDF::TH1DModel{"","",128u,0.,0.},""));
//    std::vector<Histo1D> h_cer_p;
//    std::vector<Histo1D> h_p;
//    std::vector<TLine> l_p;
    TH1D *h_histo_pos[n_dp];
    TH1D *h_cer_pos[n_dp];
    for(int i = 0;i<n_dp;++i){
      double p = P_pion_threshold+i*p_delta;
      p_dp.push_back(p);
      auto d_pos_cuts = d_pos.Filter(("hms_p > "+std::to_string(p_dp[i]) +" && "+"hms_p < "+std::to_string(p_dp[i]+p_delta)).c_str());
      auto t = d_pos_cuts.Histo1D({"","",100,p_min,p_max},"hms_p");
      t->SetLineColor(coolcolor[i]);
      t->SetFillColorAlpha(coolcolor[i],0.35);
//      std::cout << "type : " << typeid(t).name() << std::endl;
//      std::cout << "type : " << typeid(t.GetPtr()).name() << std::endl;
//      std::cout << "type : " << typeid(*t.GetPtr()).name() << std::endl;
      h_histo_pos[i] = new TH1D(*t.GetPtr());
      //h_histo[i] = t;
      auto h = d_pos_cuts.Histo1D({"","",100,0,30},"H.cer.npeSum");
      h->SetLineColor(coolcolor[i]);
      h->SetFillColorAlpha(coolcolor[i],0.35);
      h_cer_pos[i] = new TH1D(*h.GetPtr());
    }
    auto h_cer_dpcut_pos = d_pos.Filter("hms_p < 4.6").Histo1D({"","",100,0,30},"H.cer.npeSum");
    TCanvas *c_pos = new TCanvas();
    c_pos->Divide(2,2);
    c_pos->cd(1);
    h_p_pos->Draw("hist");
    for(int i = 0;i<n_dp;++i){
    h_histo_pos[i]->Draw("hist same");
    }
    c_pos->cd(2);
//    h_cer_dp_pos->Draw("colz");
    h_cer_all_pos->Draw("hist");
    h_cer_dpcut_pos->Draw("hist same");
    c_pos->cd(3);
    h_cer_p_pos->Draw("colz");
    c_pos->cd(4);
    //h_cer_all_pos->Draw("hist");
    for(int i = 0;i<n_dp;++i){
      h_cer_pos[i]->Draw("hist same");
    }
    std::string c_pos_name = "results/pid/HMS_cer_pion_"+std::to_string(RunGroup)+"_pos.pdf";
    c_pos->SaveAs(c_pos_name.c_str());
  
    auto h_p_neg_withe = d_neg_withe.Histo1D({"","HMS momentum;momentum;counts",100,p_min,p_max},"hms_p");
    auto h_cer_p_neg_withe = d_neg_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
    auto h_cer_all_neg_withe = d_neg_withe.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
    auto h_p_pos_withe = d_pos_withe.Histo1D({"","HMS momentum;momentum;counts",100,p_min,p_max},"hms_p");
    auto h_cer_p_pos_withe = d_pos_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
    auto h_cer_all_pos_withe = d_pos_withe.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
    TCanvas *c_neg_withe = new TCanvas;
    c_neg_withe->Divide(2,2);
    c_neg_withe->cd(1);
    h_p_neg_withe->Draw("hist");
    c_neg_withe->cd(2);
    h_cer_all_neg_withe->Draw("hist");
    c_neg_withe->cd(3);
    h_cer_p_neg_withe->Draw("colz");
    std::string neg_withe_name = "results/pid/HMS_cer_"+std::to_string(RunGroup)+".pdf";
    c_neg_withe->SaveAs(neg_withe_name.c_str());
    TCanvas *c_pos_withe = new TCanvas;
    c_pos_withe->Divide(2,2);
    c_pos_withe->cd(1);
    h_p_pos_withe->Draw("hist");
    c_pos_withe->cd(2);
    h_cer_all_pos_withe->Draw("hist");
    c_pos_withe->cd(3);
    h_cer_p_pos_withe->Draw("colz");
    std::string pos_withe_name = "results/pid/HMS_cer_"+std::to_string(RunGroup)+".pdf";
    c_pos_withe->SaveAs(pos_withe_name.c_str());
  }//if normal rungroup
}//end 

