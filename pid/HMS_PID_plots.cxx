#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "TCanvas.h"
#include "ROOT/RDataFrame.hxx"
#include "TH1.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TH2.h"

void ReverseXAxis(TH2 *h);
void ReverseYAxis(TH2 *h);
void HMS_PID_plots(int RunGroup = 0){
  json j_cuts;
  {
    std::ifstream ifs("db2/HMS_cuts.json");
    ifs>>j_cuts;
  }
  std::vector<int> coolcolor;
  coolcolor = j_cuts["coolcolor"].get<std::vector<int>>();
  std::vector<int> cercut;
  cercut = j_cuts["cer_cuts"].get<std::vector<int>>();
  int n_cuts = (int)cercut.size();
  std::vector<double> calcut;
  calcut = j_cuts["cal_cuts"].get<std::vector<double>>();
  int n_cal_cuts = (int)calcut.size();

  if(RunGroup == 0){
    std::cout<<"Enter RunGroup Number(-1 to exit)";
    std::cin>>RunGroup;
    if(RunGroup <0)
      return;
  }
  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs>>j_rungroup;
  }
  RunGroup = RunGroup;
  std::vector<int> neg_D2,pos_D2;
  neg_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["neg"]["D2"].get<std::vector<int>>();
  pos_D2 = j_rungroup[std::to_string(RunGroup).c_str()]["pos"]["D2"].get<std::vector<int>>();
  if(!neg_D2.empty()  && !pos_D2.empty()){
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
    auto d_neg_SHMS = d_neg_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_neg = d_neg_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 10");
    auto d_pos_SHMS = d_pos_raw.Filter("-10 < P.gtr.dp && P.gtr.dp < 22");
    auto d_pos = d_pos_SHMS = d_pos_SHMS.Filter("-10 < H.gtr.dp && H.gtr.dp < 22");

    double e_cercut = j_cuts["cer_e"].get<double>();
    double pion_cercut = j_cuts["cer_pi"].get<double>();
    double e_calcut_low = j_cuts["cal_e"].get<double>();
    double pion_calcut_high = j_cuts["cal_pi_high"].get<double>();
    double pion_calcut_low = j_cuts["cal_pi_low"].get<double>();
    std::string e_cer_cut = "H.cer.npeSum > "+std::to_string(e_cercut);
    std::string pion_cer_cut = "H.cer.npeSum < "+std::to_string(pion_cercut);
    std::string e_cal_cut = std::to_string(e_calcut_low)+" < H.cal.etottracknorm";
    std::string pion_cal_cut = "H.cal.etottracknorm > "+std::to_string(pion_calcut_low)+ " && H.cal.etottracknorm < "+std::to_string(pion_calcut_high);
  auto d_neg_e_cercut = d_neg.Filter(e_cer_cut); 
  auto d_neg_pion_cercut = d_neg.Filter(pion_cer_cut);
  auto d_pos_e_cercut = d_pos.Filter(e_cer_cut);
  auto d_pos_pion_cercut = d_pos.Filter(pion_cer_cut);
  auto d_neg_e_calcut = d_neg.Filter(e_cal_cut);
  auto d_neg_pion_calcut = d_neg.Filter(pion_cal_cut);
  auto d_pos_e_calcut = d_pos.Filter(e_cal_cut);
  auto d_pos_pion_calcut = d_pos.Filter(pion_cal_cut);
   // auto d_neg_e_cercut = d_neg.Filter("H.cer.npeSum >10");
   // auto d_neg_pion_cercut = d_neg.Filter("H.cer.npeSum <0.1");
   // auto d_pos_e_cercut = d_pos.Filter("H.cer.npeSum >10");
   // auto d_pos_pion_cercut = d_pos.Filter("H.cer.npeSum <0");
   // auto d_neg_e_calcut = d_neg.Filter("0.85 < H.cal.etottracknorm && H.cal.etottracknorm <1.5");
   // auto d_neg_pion_calcut = d_neg.Filter("H.cal.etottracknorm < 0.6 ");
   // auto d_pos_e_calcut = d_pos.Filter("0.85 < H.cal.etottracknorm && H.cal.etottracknorm <1.5");
   // auto d_pos_pion_calcut = d_pos.Filter("H.cal.etottracknorm < 0.6 ");
    

    auto h_neg = d_neg.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    auto h_neg_e_cercut = d_neg_e_cercut.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    h_neg_e_cercut->SetLineColor(kRed);
    auto h_neg_pion_cercut = d_neg_pion_cercut.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    h_neg_pion_cercut->SetLineColor(kRed);
    auto h_neg_cer = d_neg.Histo1D({"","Cer;npe;counts",100,0,30},"H.cer.npeSum");
    //TLine *l_cercut = 
    TCanvas *c_neg = new TCanvas();
    c_neg->Divide(2,2);  
    c_neg->cd(1);
    TVirtualPad *pad_neg_cal = c_neg->cd(1);
    pad_neg_cal->SetLogy();
    h_neg->Draw("hist");
    c_neg->cd(2);
    h_neg_cer->Draw("hist");
    c_neg->cd(3);
    h_neg->Draw("hist");
    h_neg_e_cercut->Draw("hist same");
    c_neg->cd(4);
    h_neg->Draw("hist");
    h_neg_pion_cercut->Draw("hist same");
    for(int i = 0; i < n_cal_cuts; ++i){
      TLine *line = new TLine(calcut[i],0,calcut[i],40000);
      line->SetLineColor(coolcolor[i]);
      line->Draw("same");
    }
    std::string neg_canvasname  = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"cal_neg.pdf";
    c_neg->SaveAs(neg_canvasname.c_str());

    auto h_pos = d_pos.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    auto h_pos_e_cercut = d_pos_e_cercut.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    h_pos_e_cercut->SetLineColor(kRed);
    auto h_pos_pion_cercut = d_pos_pion_cercut.Histo1D({"","E/p;E/p;counts",100,0.1,1.5},"H.cal.etottracknorm");
    h_pos_pion_cercut->SetLineColor(kRed);
    auto h_pos_cer = d_pos.Histo1D({"","Cer;npe;counts",100,0,30},"H.cer.npeSum");
    //TLine *l_cercut = 
    TCanvas *c_pos = new TCanvas();
    c_pos->Divide(2,2);  
    c_pos->cd(1);
    TVirtualPad *pad_pos_cal = c_pos->cd(1);
    pad_pos_cal->SetLogy();
    h_pos->Draw("hist");
    c_pos->cd(2);
    h_pos_cer->Draw("hist");
    c_pos->cd(3);
    h_pos->Draw("hist");
    h_pos_e_cercut->Draw("hist same");
    c_pos->cd(4);
    h_pos->Draw("hist");
    h_pos_pion_cercut->Draw("hist same");
    for(int i = 0; i < n_cal_cuts; ++i){
      TLine *line = new TLine(calcut[i],0,calcut[i],4000);
      line->SetLineColor(coolcolor[i]);
      line->Draw("same");
    }
    std::string pos_canvasname  = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"cal_pos.pdf";
    c_pos->SaveAs(pos_canvasname.c_str());

  
    
    auto h_neg_e_calcut = d_neg_e_calcut.Histo1D({"","cer;cer;counts",100,0,30},"H.cer.npeSum");
    h_neg_e_calcut->SetLineColor(kRed);
    auto h_neg_pion_calcut = d_neg_pion_calcut.Histo1D({"","cer;cer;counts",100,0,30},"H.cer.npeSum");
    h_neg_pion_calcut->SetLineColor(kRed);
    //TLine *l_cercut = 
    TCanvas *c_neg_cer = new TCanvas();
    c_neg_cer->Divide(2,2);  
    c_neg_cer->cd(1);
    h_neg_cer->Draw("hist");
    c_neg_cer->cd(2);
    TVirtualPad *pad_neg_cer = c_neg_cer->cd(2);
    pad_neg_cer->SetLogy();
    h_neg->Draw("hist");
    c_neg_cer->cd(3);
    h_neg_cer->Draw("hist");
    h_neg_e_calcut->Draw("hist same");
    c_neg_cer->cd(4);
    h_neg_cer->Draw("hist");
    h_neg_pion_calcut->Draw("hist same");
    for(int i = 0; i < n_cuts; ++i){
      TLine *line = new TLine(cercut[i],0,cercut[i],40000);
      line->SetLineColor(coolcolor[i]);
      line->Draw("same");
    }
    
    std::string neg_canvasname_cer  = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"cer_neg.pdf";
    c_neg_cer->SaveAs(neg_canvasname_cer.c_str());

    auto h_pos_e_calcut = d_pos_e_calcut.Histo1D({"","cer;cer;counts",100,0,30},"H.cer.npeSum");
    h_pos_e_calcut->SetLineColor(kRed);
    auto h_pos_pion_calcut = d_pos_pion_calcut.Histo1D({"","cer;cer;counts",100,0,30},"H.cer.npeSum");
    h_pos_pion_calcut->SetLineColor(kRed);
    //TLine *l_cercut = 
    TCanvas *c_pos_cer = new TCanvas();
    c_pos_cer->Divide(2,2);  
    c_pos_cer->cd(1);
    h_pos_cer->Draw("hist");
    c_pos_cer->cd(2);
    //c_pos_cer->SetLogy();
    TVirtualPad *pad_pos_cer = c_pos_cer->cd(2);
    pad_pos_cer->SetLogy();
    h_pos->Draw("hist");
    c_pos_cer->cd(3);
    h_pos_cer->Draw("hist");
    h_pos_e_calcut->Draw("hist same");
    c_pos_cer->cd(4);
    h_pos_cer->Draw("hist");
    h_pos_pion_calcut->Draw("hist same");
    for(int i = 0; i < n_cuts; ++i){
      TLine *line = new TLine(cercut[i],0,cercut[i],40000);
      line->SetLineColor(coolcolor[i]);
      line->Draw("same");
    }
    std::string pos_canvasname_cer  = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"cer_pos.pdf";
    c_pos_cer->SaveAs(pos_canvasname_cer.c_str());
    
    //TCanvas *c_neg_lego = new TCanvas();
    //double npeSum_uplimit = 30;
    //auto d_neg_lego = d_neg.Filter("H.cal.etottracknorm>0.00001")
    //  .Filter("H.cer.npeSum>0.001")
    ////.Define("reverse_npeSum",[npeSum_uplimit](double npeSum){return npeSum_uplimit-npeSum;},{"H.cer.npeSum"})
    //;
    //TH2D* h_neg_cal_cer_lego = new TH2D(*d_neg_lego.Histo2D({"pi-","cal_cer",100,0,1.5,100,0,npeSum_uplimit},"H.cal.etottracknorm","H.cer.npeSum").GetPtr());
    //gStyle->SetPalette(kBird);
    ////TVirtualPad *pad_neg_lego = c_neg_lego->cd();
    ////pad_neg_lego->SetLogz();
    //h_neg_cal_cer_lego->Draw("LEGO2Z");
    ////ReverseYAxis(h_neg_cal_cer_lego);
    //std::string c_neg_lego_name = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"_lego.pdf";
    //c_neg_lego->SaveAs(c_neg_lego_name.c_str());
    //return c_neg_lego;

//   auto h_neg_e_cercut_1 = d_neg.Filter("H.cer.npeSum > 5").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_neg_e_cercut_2 = d_neg.Filter("H.cer.npeSum > 6").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_neg_e_cercut_3 = d_neg.Filter("H.cer.npeSum > 7").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_neg_e_cercut_4 = d_neg.Filter("H.cer.npeSum > 8").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_neg_e_cercut_5 = d_neg.Filter("H.cer.npeSum > 9").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_neg_e_cercut_6 = d_neg.Filter("H.cer.npeSum > 10").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_1 = d_pos.Filter("H.cer.npeSum > 5").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_2 = d_pos.Filter("H.cer.npeSum > 6").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_3 = d_pos.Filter("H.cer.npeSum > 7").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_4 = d_pos.Filter("H.cer.npeSum > 8").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_5 = d_pos.Filter("H.cer.npeSum > 9").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   auto h_pos_e_cercut_6 = d_pos.Filter("H.cer.npeSum > 10").Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm"); 
//   h_neg_e_cercut_1->SetLineColor(coolcolor[0]);
//   h_neg_e_cercut_2->SetLineColor(coolcolor[1]);
//   h_neg_e_cercut_3->SetLineColor(coolcolor[2]);   
//   h_neg_e_cercut_4->SetLineColor(coolcolor[3]);  
//   h_neg_e_cercut_5->SetLineColor(coolcolor[4]); 
//   h_neg_e_cercut_6->SetLineColor(coolcolor[5]);
//   h_pos_e_cercut_1->SetLineColor(coolcolor[0]);
//   h_pos_e_cercut_2->SetLineColor(coolcolor[1]);
//   h_pos_e_cercut_3->SetLineColor(coolcolor[2]);   
//   h_pos_e_cercut_4->SetLineColor(coolcolor[3]);  
//   h_pos_e_cercut_5->SetLineColor(coolcolor[4]); 
//   h_pos_e_cercut_6->SetLineColor(coolcolor[5]);
//   auto c_check = new TCanvas();
//   c_check->Divide(2,2);
//   c_check->cd(1);
//   h_neg_cer->Draw("hist");
//   for(int i = 0; i < n_cuts; ++i){
//     TLine *line = new TLine(cercut[i],0,cercut[i],4000);
//     line->SetLineColor(coolcolor[i]);
//     line->Draw("same");
//   }
//
//   c_check->cd(2);
//   h_neg->Draw("hist");
//   h_neg_e_cercut_1->Draw("hist same");
//   h_neg_e_cercut_2->Draw("hist same");
//   h_neg_e_cercut_3->Draw("hist same");
//   h_neg_e_cercut_4->Draw("hist same");
//   h_neg_e_cercut_5->Draw("hist same");
//   h_neg_e_cercut_6->Draw("hist same");
//
//   //for(int i = 0;i<n_cuts;++i){
//   //  std::string cercuts = "H.cer.npeSum > "+std::to_string(cercut[i]);
//   //  std::cout<<"cercuts "<<cercuts<<std::endl;
//   //  auto h_neg_e_cercut = d_neg.Filter(cercuts)
//   //                             .Histo1D({"","E/P;E/P;counts",100,0.1,1.5},"H.cal.etottracknorm");
//   //  h_neg_e_cercut->SetLineColor(coolcolor[i]);
//   //  c_check->cd(2);
//   //  h_neg_e_cercut->Draw("hist same");
//   //}
//   c_check->cd(3);
//   h_pos_cer->Draw("hist");
//   for(int i = 0; i < n_cuts; ++i){
//     TLine *line = new TLine(cercut[i],0,cercut[i],4000);
//     line->SetLineColor(coolcolor[i]);
//     line->Draw("same");
//   }
//
//   c_check->cd(4);
//   h_pos->Draw("hist");
//   h_pos_e_cercut_1->Draw("hist same");
//   h_pos_e_cercut_2->Draw("hist same");
//   h_pos_e_cercut_3->Draw("hist same");
//   h_pos_e_cercut_4->Draw("hist same");
//   h_pos_e_cercut_5->Draw("hist same");
//   h_pos_e_cercut_6->Draw("hist same");
//   //for(int i = 0;i<n_cuts;++i){
//   //  std::string cercuts = "H.cer.npeSum > "+std::to_string(cercut[i]);
//   //  std::cout<<"cercuts "<<cercuts<<std::endl;
//   //  auto h_pos_e_cercut = d_pos.Filter(cercuts)
//   //                             .Histo1D({"","E/P;E/P;counts",100,0.1,1.5},"H.cal.etottracknorm");
//   //  h_pos_e_cercut->SetLineColor(coolcolor[i]);
//   //  c_check->cd(4);
//   //  h_pos_e_cercut->Draw("hist same");
//   //}
//   c_check->Update();
//   std::string check_name = "results/pid/HMS_PID_"+std::to_string(RunGroup)+"_check.pdf";
//   c_check->SaveAs(check_name.c_str());
  }
}


//void ReverseXAxis (TH2 *h)
//{
//  // Remove the current axis
//  h->GetXaxis()->SetLabelOffset(999);
//  h->GetXaxis()->SetTickLength(0);
//
//  // Redraw the new axis 
//  gPad->Update();
//  TGaxis *newaxis = new TGaxis(gPad->GetUxmax(), 
//      gPad->GetUymin(),
//      gPad->GetUxmin(),
//      gPad->GetUymin(),
//      h->GetXaxis()->GetXmin(),
//      h->GetXaxis()->GetXmax(),
//      510,"-");
//  newaxis->SetLabelOffset(-0.03);
//  newaxis->Draw();
//}
//void ReverseYAxis (TH2 *h)
//{
//  // Remove the current axis
//  h->GetYaxis()->SetLabelOffset(999);
//  h->GetYaxis()->SetTickLength(0);
//
//  // Redraw the new axis
//  gPad->Update();
//  TGaxis *newaxis = new TGaxis(gPad->GetUxmin(),
//      gPad->GetUymax(),
//      gPad->GetUxmin()-0.001,
//      gPad->GetUymin(),
//      h->GetYaxis()->GetXmin(),
//      h->GetYaxis()->GetXmax(),
//      510,"+");
//  newaxis->SetLabelOffset(-0.03);
//  newaxis->Draw();
//}
