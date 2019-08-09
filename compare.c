#include "ROOT/RDataFrame.hxx"
#include <iostream>
#include "TCanvas.h"

int compare(){
  std::string rootfile1 = std::string("/group/c-csv/shuo/hallc_replay_sidis_fall18/ROOTfiles/coin_replay_production_7822_-1.root");
  std::string rootfile2 = std::string("/group/c-csv/shuo/online_csv/monitoring/good_csv_counter_7822.root");
  ROOT::RDataFrame d1("T",rootfile1);
  ROOT::RDataFrame d2("UPDATE",rootfile2);

  auto h_pcaletottracknorm_1 = d1.Histo1D({"P.cal.etottracknorm","P.cal.etottracknorm",100,0.1,2},"P.cal.etottracknorm");
  auto h_pcaletottracknorm_2 = d2.Histo1D({"P.cal.etottracknorm","P.cal.etottracknorm",100,0.1,2},"P.cal.etottracknorm");
  auto h_hcaletottracknorm_1 = d1.Histo1D({"H.cal.etottracknorm","H.cal.etottracknorm",100,0.1,2},"H.cal.etottracknorm");
  auto h_hcaletottracknorm_2 = d2.Histo1D({"H.cal.etottracknorm","H.cal.etottracknorm",100,0.1,2},"H.cal.etottracknorm");
  auto h_gtr_dp_1 = d1.Histo1D({"H.gtr.dp","delta p",200,-30,40},"H.gtr.dp");
  auto h_gtr_dp_2 = d2.Histo1D({"H.gtr.dp","delta p",200,-30,40},"H.gtr.dp");
  auto h_W_1 = d1.Histo1D({"W","W",200,-30,40},"H.Kin.Primary.W");
  auto h_W_2 = d2.Histo1D({"W","W",200,-30,40},"W");
  TCanvas *c = new TCanvas;
  c->Divide(2,2);
  c->cd(1);
  h_pcaletottracknorm_1->DrawCopy();
  h_pcaletottracknorm_2->DrawCopy("same");
  h_pcaletottracknorm_2->SetLineColor(2);
  c->cd(2);
  h_hcaletottracknorm_1->DrawCopy();
  h_hcaletottracknorm_2->DrawCopy("same");
  h_hcaletottracknorm_2->SetLineColor(2);
  c->cd(3);
  h_gtr_dp_1->DrawCopy();
  h_gtr_dp_2->DrawCopy("same");
  h_gtr_dp_2->SetLineColor(2);
  c->cd(4);
  h_W_1->DrawCopy();
  h_W_2->DrawCopy("same");
  h_W_2->SetLineColor(2);
  c->SaveAs("result/compare.pdf");





  return 0;
}
