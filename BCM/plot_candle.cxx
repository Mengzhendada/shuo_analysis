#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "TCanvas.h"
#include "TH2.h"

void plot_candle(){
  json j;
  {
    std::ifstream ifs("results/bcm/current.json");
    ifs>>j;
  }
  TH2D* h = new TH2D("h","D2_neg;RunGroup;current",55,0,550,100,5,80);
  TH2D* h_pos = new TH2D("h","D2_pos",55,0,550,100,5,80);
  for(auto it = j.begin();it!=j.end();++it){
    int RunGroup = std::stoi(it.key());
    std::vector<double> currents = it.value()["neg"].get<std::vector<double>>();
    for(auto ik = currents.begin();ik!=currents.end();++ik ){
      double current = *ik;
      h->Fill(RunGroup,current);
    }
    std::vector<double> currents_pos = it.value()["pos"].get<std::vector<double>>();
    for(auto ik = currents_pos.begin();ik!=currents_pos.end();++ik ){
      double current = *ik;
      h_pos->Fill(RunGroup,current);
    }
  }
  TCanvas *c = new TCanvas();
  c->SetGrid();
  h->SetBarWidth(1);
  h->SetBarOffset(-0.5);
  h->SetFillColorAlpha(kBlue,0.8);
  h->SetFillStyle(1001);
  h->Draw("candle3");
  h_pos->SetBarWidth(1);
  h_pos->SetBarOffset(-0.5);
  h_pos->SetFillColorAlpha(kRed,0.8);
  h_pos->SetLineColor(kRed);
  h_pos->Draw("candle3 same");
  gPad->BuildLegend(0.78,0.695,0.980,0.980,"","f");
  c->SaveAs("results/bcm/current_D2.pdf");
}
