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
  TH2D* h = new TH2D("h","D2_neg",55,0,550,100,5,80);
  for(auto it = j.begin();it!=j.end();++it){
    int RunGroup = std::stoi(it.key());
    std::vector<double> currents = it.value().get<std::vector<double>>();
    for(auto ik = currents.begin();ik!=currents.end();++ik ){
      double current = *ik;
      h->Fill(RunGroup,current);
    }
  }
  TCanvas *c = new TCanvas();
  c->SetGrid();
  h->SetBarWidth(0.4);
  h->SetBarOffset(-0.1);
  h->SetFillColor(kYellow);
  h->SetFillStyle(1001);
  h->Draw("candle3");
}
