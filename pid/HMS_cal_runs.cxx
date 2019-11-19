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

void HMS_cal_runs(int RunNumber= 0){
  if(RunNumber == 0){
    std::cout<<"Enter RunNumber(-1 to exit)";
    std::cin>>RunNumber;
    if(RunNumber <0)
      return;
  }
    std::string rootfile = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
    ROOT::RDataFrame d("T",rootfile);
    auto h_hms_cal = d.Histo1D({"","",100,0.1,1.5},"H.cal.etottracknorm");
    h_hms_cal->Fit("gaus","O","",0.5,1.5);
    TF1 *fit = h_hms_cal->GetFunction("gaus");
    double mean = fit->GetParameter(1);
    double sigma = fit->GetParameter(2);
    std::ofstream outfile;
    outfile.open("results/hms_cal.txt",std::ofstream::out | std::ofstream::app);
//    json j;
//    j[std::to_string(RunNumber).c_str()]["mean"] = mean;
//    j[std::to_string(RunNumber).c_str()]["sigma"] = sigma;
//    outfile<<j.dump(4)<<std::endl;
    outfile<<RunNumber <<" "<<mean<<" "<<sigma<<std::endl;
    outfile.close();
}
