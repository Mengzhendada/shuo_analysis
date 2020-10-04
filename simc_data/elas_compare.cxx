#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"

#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void elas_compare(){
  std::vector<int> fall_elas = {6009,6010,6012,6013,6016,6017}; 
  std::vector<int> spring_elas = {7822,7823,7826,7827,7830};

  json j_simc;
  {
    std::ifstream ifs("db2/simc.json");
    ifs>>j_simc;
  }

  TGraphErrors *G_yield_fall_data = new TGraphErrors();
  G_yield_fall_data->SetTitle("data");
  TGraphErrors *G_yield_spring_data = new TGraphErrors();
  G_yield_spring_data->SetTitle("data");
  TGraphErrors *G_yield_fall_sim = new TGraphErrors();
  G_yield_fall_sim->SetTitle("sim");
  TGraphErrors *G_yield_spring_sim = new TGraphErrors();
  G_yield_spring_sim->SetTitle("sim");

  for(auto it = j_simc.begin();it!=j_simc.end();++it){
    std::string rootfile_data_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+ 
  }

}
