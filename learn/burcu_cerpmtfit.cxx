#include "ROOT/RDataFrame.hxx"

#include <iostream>

void burcu_cerpmtfit(){
  std::string rootfile = "ROOTfiles/coin_replay_production_7593_7593.root";
  ROOT::RDataFrame d("T",rootfile);
  auto d_cuts = d.Filter("P.gtr.dp < 22 && P.gtr.dp>-10 && P.cal.etottracknorm> 0.8");
}
