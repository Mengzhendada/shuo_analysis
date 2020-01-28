#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include <iostream>

void burcu_cerpmtfit(){
  std::string rootfile = "ROOTfiles/coin_replay_production_7593_7593.root";
  ROOT::RDataFrame d("T",rootfile);
  auto d_cuts = d.Filter("P.gtr.dp < 22 && P.gtr.dp>-10 && P.cal.etottracknorm> 0.8")
  .Define("shms_pmt2",[](double npe0){return npe0;},{"P.ngcer.npeSum[0]"});
  auto hcal = d_cuts.Histo1D({"shms ngcer","shms ngcer",100,0.01,30},"shms_pmt2");
  TCanvas *c = new TCanvas("","c",1900,1000);
  hcal->DrawClone();
  c->SaveAs("try.pdf");
}
