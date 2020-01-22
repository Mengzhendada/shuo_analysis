#include <ROOT/RDataFrame.hxx>
#include <array>
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void good_current_shuo(int RunNumber = 0){
  json j_runlist;
  {
    std::ifstream ifs("db2/run_list_csv.json");
    ifs>>j_runlist;
  }
  if(RunNumber == 0){  
    //std::cout<<RunNumber<<std::endl;
    std::cout<<"Enter RunNumber(-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<0)
      return;
      }
  std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
  ROOT::RDataFrame d_scaler{"TSP",rootfile_name.c_str()};
  ROOT::RDataFrame d_events("T",rootfile_name.c_str());

  auto h_current = d_scaler.Histo1D({"current","",10000,10,100},"P.BCM4B.scalerCurrent");
  double target_current = h_current->GetBinCenter(h_current->GetMaximumBin());
  double current_tolerance = 3;
  h_current->DrawClone();
  gPad->Print("current.pdf");
  std::cout << "Most prevalent current in this run is: " << target_current
            << "uA\n";
  std::cout << "Setting tolerance to to +/- " << current_tolerance << "uA\n";
  
  auto h_current_time = d_scaler.Histo2D({"current vs. time", "",1000,0,3000,1000,0,100},"P.1MHz.scalerTime","P.BCM4B.scalerCurrent");
  h_current_time->DrawClone();
  gPad->Print("current_time.pdf");

  //auto h_evNumber = d_scaler.Histo1D({"evNumber","",100,0,})
  
  auto scaler_event_list = d_scaler_good.Take<double>("evNumber");
  //auto 
  //int i_true= 0;
  //for (auto it = has_good_current_list->begin();it!=has_good_current_list->end();++it){
  //  int eventNumber = scaler_event_list->at(it-has_good_current_list->begin());
  //  std::cout<<"for event "<<eventNumber <<" it's "<<*it<<std::endl;
  //  if(*it){++i_true;}
  //}
  //std::cout<<" true events "<<i_true<<std::endl;


}

