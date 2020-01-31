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
#include <bits/stdc++.h>
#include <map>

void HMS_cer_pion(int RunGroup = 0){
  std::vector<int> GroupNum;
  if(RunGroup == 0){
    while(RunGroup >=0){
      std::cout<<"Enter RunGroup Number(-1 to stop,-2 to exit)";
      std::cin>>RunGroup;
      if(RunGroup >0)
        GroupNum.push_back(RunGroup);
    }
    if(RunGroup ==-2)
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
  std::vector<double> all_p;
  double P_pion_threshold = j_cuts["cer_pi_threshold"].get<double>();
  for(auto it = GroupNum.begin();it!=GroupNum.end();++it){
    std::cout<<*it<<std::endl;
    int RunGroup = *it;
    RunGroup = 10*RunGroup;
    double HMS_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
    all_p.push_back(HMS_P);
  }
  double all_p_min = 0.9*(*min_element(all_p.begin(),all_p.end()));
  double all_p_max = 1.1*(*max_element(all_p.begin(),all_p.end())); 
  std::cout<<all_p_min<<" to "<<all_p_max<<std::endl;

  TLine *l_p_pion_threshold = new TLine(P_pion_threshold,0,P_pion_threshold,30);
  int n_dp = 3;
  double p_delta = (all_p_max-P_pion_threshold)/n_dp;
  std::vector<double> all_p_dp;
  //std::vector<TLine*> p_lines;
  //   //    using Histo1D = decltype(ROOT::RDataFrame{0}.Histo1D(ROOT::RDF::TH1DModel{"","",128u,0.,0.},""));
  //   //    std::vector<Histo1D> h_cer_p;
  //   //    std::vector<Histo1D> h_p;
  //   //    std::vector<TLine> l_p;
  //   TH1D *h_histo_neg[n_dp];
  //   TH1D *h_cer_neg[n_dp];
  for(int i = 0;i<n_dp;++i){
    double p = all_p_min+i*p_delta;
    std::cout<<"Range edge of hms momentum "<<p<<std::endl; 
    all_p_dp.push_back(p);
    //TLine *l_p_line = new TLine()
    //     auto d_neg_cuts = d_neg.Filter(("hms_p > "+std::to_string(p_dp[i]) +" && "+"hms_p < "+std::to_string(p_dp[i]+p_delta)).c_str());
    //     auto t = d_neg_cuts.Histo1D({"","",100,all_p_min,all_p_max},"hms_p");
    //     t->SetLineColor(coolcolor[i]);
    //     t->SetFillColorAlpha(coolcolor[i],0.35);
    //     //      std::cout << "type : " << typeid(t).name() << std::endl;
    //     //      std::cout << "type : " << typeid(t.GetPtr()).name() << std::endl;
    //     //      std::cout << "type : " << typeid(*t.GetPtr()).name() << std::endl;
    //     h_histo_neg[i] = new TH1D(*t.GetPtr());
    //     //h_histo[i] = t;
    //     auto h = d_neg_cuts.Histo1D({"","",100,0,30},"H.cer.npeSum");
    //     h->SetLineColor(coolcolor[i]);
    //     h->SetFillColorAlpha(coolcolor[i],0.35);
    //     h_cer_neg[i] = new TH1D(*h.GetPtr());
  }
  std::map<std::pair<int,int>,TH1D*> map_npeSum_neg,map_npeSum_pos;
  std::vector<TH2D*> vector_npeSum_p_neg,vector_npeSum_p_pos;
  std::vector<TH1D*> vector_p_neg,vector_p_pos;
  for(auto it = GroupNum.begin();it!= GroupNum.end();++it){
    RunGroup =  (*it);
    RunGroup = 10* RunGroup;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
    double HMS_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
    std::cout<<"For Rungroup "<<RunGroup<<", the HMS momentum is "<<HMS_P<<std::endl;
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
      auto h_p_neg = d_neg.Histo1D({"","HMS momentum;momentum;counts",100,all_p_min,all_p_max},"hms_p");
      vector_p_neg.push_back(new TH1D(*h_p_neg.GetPtr()));
      auto h_cer_dp_neg = d_neg.Histo2D({"","HMS cer npeSum vs dp;dp;cer npesum",100,-10,10,100,0,30},"H.gtr.dp","H.cer.npeSum");
      auto h_cer_p_neg = d_neg.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,all_p_min,all_p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_p_neg_i = d_neg.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
      vector_npeSum_p_neg.push_back(new TH2D(*h_cer_p_neg.GetPtr()));
      auto h_cer_all_neg = d_neg.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
      for(int i = 0;i<all_p_dp.size();++i){
        double p_i_min = all_p_dp[i];
        double p_i_max = all_p_dp[i]+p_delta;
       // if(p_i_min>=p_min){
       //   p_i_min = p_i_min;
       // }
       // if(p_i_min<p_min && p_i_max>p_min){
       //   p_i_min = p_min;
       // }
       // if(p_i_max<=p_min){
       //   p_i_min = -1;//if the interested momentum range is out of this rungroup range
       // }
       // if(p_max >= p_i_max){
       //   p_i_max = p_i_max;
       // }
       // if(p_max <= p_i_max && p_max >= p_i_min){
       //   p_i_max = p_max;
       // }
       // if(p_max <= p_i_min){
       //   p_i_min = -1;//if the interested momentum range is out of this rungroup range
       // }
       // if(p_i_min != -1 && p_i_max != -1){
          std::cout<<"For RunGroup "<<RunGroup<<", the "<<i<<" hms momentum range,the range edge is "<<p_i_min<<" to "<<p_i_max<<std::endl;
          std::string momentum_cut = "hms_p > "+std::to_string(p_i_min) +" && hms_p < "+std::to_string(p_i_max);
          auto h_npeSum_neg = d_neg.Filter(momentum_cut).Histo1D({"","",100,0,30},"H.cer.npeSum");
          TH1D* h_npeSum_neg_i = new TH1D(*h_npeSum_neg.GetPtr());
          map_npeSum_neg[std::make_pair(RunGroup,i)] = h_npeSum_neg_i;
          auto h_npeSum_pos = d_pos.Filter(momentum_cut).Histo1D({"","",100,0,30},"H.cer.npeSum");
          map_npeSum_pos[std::make_pair(RunGroup,i)] = new TH1D(*h_npeSum_pos.GetPtr());
       // }
       // else{
       //   std::cout<<"For RunGroup "<<RunGroup <<", the "<<i<<"th hms momentum range is out of thie rungroup range."<<std::endl;
       // }
       //  TCanvas *c_try = new TCanvas();
       //  auto h_try = (TH1D*)map_npeSum_neg[std::make_pair(RunGroup,i)];
       //  h_try->Draw("hist");
       //  std::string c_try_name = "results/pid/try_"+std::to_string(RunGroup)+"_"+std::to_string(i)+".pdf";
       //  c_try->SaveAs(c_try_name.c_str());
      }
      TCanvas *c_neg = new TCanvas();
      //c_neg->Divide(2,2);
      //c_neg->cd(1);
      //h_p_neg->Draw("hist");
      //for(int i = 0;i<n_dp;++i){
      //  h_histo_neg[i]->Draw("hist same");
      //}
      //c_neg->cd(2);
      ////    h_cer_dp_neg->Draw("colz");
      //h_cer_all_neg->Draw("hist");
      //c_neg->cd(3);
      TVirtualPad *pad_neg_cer_p = c_neg->cd();
      pad_neg_cer_p->SetLogz();
      h_cer_p_neg_i->Draw("colz");
      //c_neg->cd(4);
      //h_cer_all_neg->Draw("hist");
      //for(int i = 0;i<n_dp;++i){
      //  h_cer_neg[i]->Draw("hist same");
      //}
      std::string c_neg_name = "results/pid/HMS_cer_pion_"+std::to_string(RunGroup)+".pdf";
      c_neg->SaveAs(c_neg_name.c_str());

      auto h_p_pos = d_pos.Histo1D({"","HMS momentum;momentum;counts",100,all_p_min,all_p_max},"hms_p");
      auto h_cer_dp_pos = d_pos.Histo2D({"","HMS cer npeSum vs dp;dp;cer npesum",100,-10,10,100,0,30},"H.gtr.dp","H.cer.npeSum");
      auto h_cer_p_pos = d_pos.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,all_p_min,all_p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_p_pos_i = d_pos.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_all_pos = d_pos.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");

      //    using Histo1D = decltype(ROOT::RDataFrame{0}.Histo1D(ROOT::RDF::TH1DModel{"","",128u,0.,0.},""));
      //    std::vector<Histo1D> h_cer_p;
      //    std::vector<Histo1D> h_p;
      //    std::vector<TLine> l_p;
      //auto h_cer_dpcut_pos = d_pos.Filter("hms_p < 4.6").Histo1D({"","",100,0,30},"H.cer.npeSum");
      TCanvas *c_pos = new TCanvas();
      //c_pos->Divide(2,2);
      //c_pos->cd(1);
      //h_p_pos->Draw("hist");
      //for(int i = 0;i<n_dp;++i){
      //  h_histo_pos[i]->Draw("hist same");
      //}
      //c_pos->cd(2);
      ////    h_cer_dp_pos->Draw("colz");
      //h_cer_all_pos->Draw("hist");
      //h_cer_dpcut_pos->Draw("hist same");
      c_pos->cd(3);
      TVirtualPad *pad_pos_cer_p = c_pos->cd(3);
      pad_pos_cer_p->SetLogz();
      h_cer_p_pos_i->Draw("colz");
      //c_pos->cd(4);
      ////h_cer_all_pos->Draw("hist");
      //for(int i = 0;i<n_dp;++i){
      //  h_cer_pos[i]->Draw("hist same");
      //}
      std::string c_pos_name = "results/pid/HMS_cer_pion_"+std::to_string(RunGroup)+"_pos.pdf";
      c_pos->SaveAs(c_pos_name.c_str());

      auto h_p_neg_withe = d_neg_withe.Histo1D({"","HMS momentum;momentum;counts",100,all_p_min,all_p_max},"hms_p");
      auto h_cer_p_neg_withe = d_neg_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,all_p_min,all_p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_p_neg_withe_i = d_neg_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_all_neg_withe = d_neg_withe.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
      auto h_p_pos_withe = d_pos_withe.Histo1D({"","HMS momentum;momentum;counts",100,all_p_min,all_p_max},"hms_p");
      auto h_cer_p_pos_withe = d_pos_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,all_p_min,all_p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_p_pos_withe_i = d_pos_withe.Histo2D({"","HMS cer npeSum vs p;momentum;cer npeSum",100,p_min,p_max,100,0,30},"hms_p","H.cer.npeSum");
      auto h_cer_all_pos_withe = d_pos_withe.Histo1D({"","HMS cer npeSum;npeSum;counts",100,0,30},"H.cer.npeSum");
      TCanvas *c_neg_withe = new TCanvas;
      //c_neg_withe->Divide(2,2);
      //c_neg_withe->cd(1);
      //h_p_neg_withe->Draw("hist");
      //c_neg_withe->cd(2);
      //h_cer_all_neg_withe->Draw("hist");
      //c_neg_withe->cd(3);
      TVirtualPad *pad_neg_cer_p_withe = c_neg_withe->cd(3);
      pad_neg_cer_p_withe->SetLogz();
      h_cer_p_neg_withe->Draw("colz");
      std::string neg_withe_name = "results/pid/HMS_cer_"+std::to_string(RunGroup)+".pdf";
      c_neg_withe->SaveAs(neg_withe_name.c_str());
      TCanvas *c_pos_withe = new TCanvas;
      // c_pos_withe->Divide(2,2);
      // c_pos_withe->cd(1);
      // h_p_pos_withe->Draw("hist");
      // c_pos_withe->cd(2);
      // h_cer_all_pos_withe->Draw("hist");
      // c_pos_withe->cd(3);
      TVirtualPad *pad_pos_cer_p_withe = c_pos_withe->cd(3);
      pad_pos_cer_p_withe->SetLogz();
      h_cer_p_pos_withe->Draw("colz");
      std::string pos_withe_name = "results/pid/HMS_cer_"+std::to_string(RunGroup)+".pdf";
      c_pos_withe->SaveAs(pos_withe_name.c_str());

    }//if normal rungroup
  }
  //std::map<std::pair<int,int>,TH1D*> map_npeSum_neg,map_npeSum_pos;
  //std::vector<TH2D*> vector_npeSum_p_neg,vector_npeSum_p_pos;
  //std::vector<TH1D*> vector_p_neg,vector_p_pos;
  TH1D* h_npeSum_all_neg = new TH1D("","",100,0,30);
  TH1D* h_p_all_neg = new TH1D("","",100,all_p_min,all_p_max);
  TH2D* h_npeSum_p_all_neg = new TH2D("","",100,all_p_min,all_p_max,100,0,30); 
  std::vector<int> counts_neg;
  TCanvas* c_npeSum_p_method1 = new TCanvas();
  for(auto it = GroupNum.begin();it!=GroupNum.end();++it){
    int RunGroup = *it;
    RunGroup = 10*RunGroup;
    int entries_neg = vector_p_neg[it-GroupNum.begin()]->GetEntries();
    counts_neg.push_back(entries_neg);
    double scale = 10000.0/entries_neg;
    
    double hms_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
    TLine* l_centralp = new TLine(hms_P,0,hms_P,30);
    
    TH2D *h_npeSum_p_neg = (TH2D*)vector_npeSum_p_neg[it-GroupNum.begin()];
    h_npeSum_p_neg->Scale(scale);
    c_npeSum_p_method1->cd();
    h_npeSum_p_neg->Draw("colz same");
    l_centralp->Draw("same");
    h_npeSum_p_all_neg->Add(h_npeSum_p_neg);
  }
  //TF1* f1 = new TF1("","12000*((1-(x*x+0.139*0.139)/(x*x*1.0006349*1.0006349)))",3.9,10);
  std::string npe_pion = "12000*((1-(x*x+0.139*0.139)/(x*x*1.0006349*1.0006349)))";
  std::string npe_e = "12000*((1-(x*x+0.00051*0.00051)/(x*x*1.0006349*1.0006349)))";
  TF1* f_pion = new TF1("",npe_pion.c_str(),3.9,10);
  TF1* f_e = new TF1("",npe_e.c_str(),3.9,10);
  c_npeSum_p_method1->cd();
  f_pion->Draw("same");
  f_e->Draw("same");
  std::string c_npeSum_p_method1_name = "results/pid/all_npeSum_p_"+std::to_string(GroupNum[0])+"_1.pdf";
  c_npeSum_p_method1->SaveAs(c_npeSum_p_method1_name.c_str());
  TCanvas* c_npeSum_p = new TCanvas();
  h_npeSum_p_all_neg->SetMaximum(50);
  h_npeSum_p_all_neg->SetBit(TH1::kNoStats);
  gStyle->SetPalette(kBird);
  h_npeSum_p_all_neg->Draw("colz");
  f_pion->Draw("same");
  f_e->Draw("same");
  for(auto it = GroupNum.begin();it!=GroupNum.end();++it){
    int RunGroup = *it;
    RunGroup = 10*RunGroup;
    double hms_P = std::abs(j_rungroup[std::to_string(RunGroup).c_str()]["hms_p"].get<double>());
    TLine* l_centralp = new TLine(hms_P,0,hms_P,30);
    c_npeSum_p->cd();
    l_centralp->Draw("same");
  }
  std::string c_npeSum_p_name = "results/pid/all_npeSum_p_"+std::to_string(GroupNum[0])+".pdf";
  c_npeSum_p->SaveAs(c_npeSum_p_name.c_str());
  TCanvas* c_p = new TCanvas();
  for(auto it = GroupNum.begin();it!=GroupNum.end();++it){
  TH1D *h_p_neg = (TH1D*)vector_p_neg[it-GroupNum.begin()];
  double scale = 10000.0/counts_neg[it-GroupNum.begin()];  
    h_p_neg->Scale(scale);
    h_p_neg->SetLineColor(coolcolor[it-GroupNum.begin()]);
    h_p_neg->Draw("hist same");
    h_p_all_neg->Add(h_p_neg);
  }
    h_p_all_neg->Draw("hist same");
    for(int i = 0;i<n_dp;++i){
      double p = all_p_dp[i];
      double p_high = all_p_dp[i]+p_delta;
      std::cout<<i<<" "<<p<<" "<<p_high<<std::endl;
      TLine* l = new TLine(p,0,p,300);
      l->SetLineColor(coolcolor[i]);
      c_p->cd();
      l->Draw("same");
      TH1D* h_i = (TH1D*)h_p_all_neg->Clone();
      h_i->SetFillColor(coolcolor[i]);
      h_i->GetXaxis()->SetRangeUser(p,p_high);
//https://root.cern.ch/root/roottalk/roottalk98/0846.html
      c_p->cd();
      h_i->Draw("hist same");
    }
  std::string c_p_name = "results/pid/all_p"+std::to_string(GroupNum[0])+".pdf";
  c_p->SaveAs(c_p_name.c_str());

  //  TH1D *h1 = (TH1D*)map_npeSum_neg[std::make_pair(11,1)];
  //  TCanvas *c_try = new TCanvas();
  //  h1->Draw();
  //  std::string c_try_name = "results/pid/try_"+std::to_string(11)+"_"+std::to_string(1)+".pdf";
  //  c_try->SaveAs(c_try_name.c_str());
  //  //h1->Scale(1/scale);
  //  h_npeSum->Add(h1);
  //std::vector<TH1D*> vector_npeSum_neg,vector_npeSum_pos; 
    TCanvas* c_npeSum = new TCanvas();
  for(int i = 0;i<n_dp;++i){
  TH1D* h_npeSum = new TH1D("","",100,0,30);
    for(auto it = GroupNum.begin();it!=GroupNum.end();++it){
      int RunGroup = *it;
      RunGroup = 10*RunGroup;
      double scale = 10000.0/counts_neg[it-GroupNum.begin()];
      std::cout<<scale<<std::endl;
      std::cout<<RunGroup<<" "<<i<<std::endl;
    auto *h1 = (TH1D*)map_npeSum_neg[std::make_pair(RunGroup,i)];
    TCanvas *c_try = new TCanvas();
    h1->Draw("hist");
    std::string c_try_name = "results/pid/try2_"+std::to_string(RunGroup)+"_"+std::to_string(i)+".pdf";
    c_try->SaveAs(c_try_name.c_str());
    h1->Scale(scale);
    h_npeSum->Add(h1);
    }
    //vector_npeSum_neg.push_back(h_npeSum);
    h_npeSum->SetLineColor(coolcolor[i]);
    int nentries = h_npeSum->GetEntries();
    double scale = 100000.0/nentries;
    std::cout<<nentries<<" scale by "<<scale<<std::endl;
    h_npeSum->Scale(scale);
    std::cout<<h_npeSum->GetEntries()<<std::endl;
    c_npeSum->cd();
    h_npeSum->Draw("hist same");
  }
    std::string c_npeSum_name = "results/pid/npeSum_"+std::to_string(GroupNum[0])+".pdf";
    c_npeSum->SaveAs(c_npeSum_name.c_str());

}//end 

