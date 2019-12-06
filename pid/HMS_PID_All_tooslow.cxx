#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TPaveText.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void HMS_PID(){
  //void HMS_PID(int RunGroup = 0){
  //if(RunGroup == 0){
  //  std::cout<<"Enter RunGroup Number(-1 to exit)";
  //  std::cin>>RunGroup;
  //  if(RunGroup <0)
  //    return;
  //}
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  for(json::iterator it = j_rungroup.begin();it!=j_rungroup.end();++it){
    int RunGroup = std::stoi(it.key());
      std::cout<<"RunGroup "<<RunGroup<<std::endl;
    std::vector<int> neg_D2,pos_D2;
    neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
    pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
    if(!neg_D2.empty()  && !pos_D2.empty()){
      TChain chain_neg("T");
      for(std::vector<int>::iterator it = neg_D2.begin();it!=neg_D2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        chain_neg.Add(rootfile_name.c_str());
      }
      TChain chain_pos("T");
      for(std::vector<int>::iterator it = pos_D2.begin();it!=pos_D2.end();++it){
        int RunNumber = *it;
        std::cout<<RunNumber<<std::endl;
        std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
        chain_pos.Add(rootfile_name.c_str());
      }
      ROOT::RDataFrame d_neg(chain_neg);
      ROOT::RDataFrame d_pos(chain_pos);

      auto d_neg_e = d_neg.Filter("H.cer.npeSum >5");
      auto d_neg_pion = d_neg.Filter("H.cer.npeSum ==0");
      auto d_pos_e = d_pos.Filter("H.cer.npeSum >5");
      auto d_pos_pion = d_pos.Filter("H.cer.npeSum ==0");

      auto h_neg = d_neg.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      int neg_all = h_neg->GetEntries();
      auto h_neg_e = d_neg_e.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      h_neg_e->SetLineColor(kRed);
      int neg_e = h_neg_e->GetEntries();
      auto h_neg_pion = d_neg_pion.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      h_neg_pion->SetLineColor(kRed);
      int neg_pion = h_neg_pion->GetEntries();
      auto h_neg_cer = d_neg.Histo1D({"","Cer;npe;counts",100,0,30},"H.cer.npeSum");

      TCanvas *c_neg = new TCanvas();
      c_neg->Divide(2,2);  
      c_neg->cd(1);
      h_neg->Draw("hist");
      c_neg->cd(2);
      h_neg_cer->Draw("hist");
      TPaveText *pt = new TPaveText(0.65,0.5,0.95,0.95);
      pt->AddText(("all "+std::to_string(neg_all)).c_str());
      pt->AddText(("e "+std::to_string(neg_e)).c_str());
      pt->AddText(("pion "+std::to_string(neg_pion)).c_str());
      c_neg->cd(3);
      h_neg->Draw("hist");
      h_neg_e->Draw("hist same");
      c_neg->cd(4);
      h_neg->Draw("hist");
      h_neg_pion->Draw("hist same");
      std::string canvasname  = "results/HMS_PID_"+std::to_string(RunGroup)+"_neg.pdf";
      c_neg->SaveAs(canvasname.c_str());

      auto h_pos = d_pos.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      int pos_all = h_pos->GetEntries();
      auto h_pos_e = d_pos_e.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      h_pos_e->SetLineColor(kRed);
      int pos_e = h_pos_e->GetEntries();
      auto h_pos_pion = d_pos_pion.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
      h_pos_pion->SetLineColor(kRed);
      int pos_pion = h_pos_pion->GetEntries();
      auto h_pos_cer = d_pos.Histo1D({"","Cer;npe;counts",100,0,30},"H.cer.npeSum");

      TCanvas *c_pos = new TCanvas();
      c_pos->Divide(2,2);  
      c_pos->cd(1);
      h_pos->Draw("hist");
      c_pos->cd(2);
      h_pos_cer->Draw("hist");
      TPaveText *pt_pos = new TPaveText(0.65,0.5,0.95,0.95);
      pt_pos->AddText(("all "+std::to_string(pos_all)).c_str());
      pt_pos->AddText(("e "+std::to_string(pos_e)).c_str());
      pt_pos->AddText(("pion "+std::to_string(pos_pion)).c_str());
      c_pos->cd(3);
      h_pos->Draw("hist");
      h_pos_e->Draw("hist same");
      c_pos->cd(4);
      h_pos->Draw("hist");
      h_pos_pion->Draw("hist same");
      std::string canvasname_pos  = "results/HMS_PID_"+std::to_string(RunGroup)+"_pos.pdf";
      c_pos->SaveAs(canvasname_pos.c_str());
    }
  }
}
