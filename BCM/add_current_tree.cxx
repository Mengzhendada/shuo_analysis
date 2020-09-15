#include <ROOT/RDataFrame.hxx>
#include <array>
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void add_current_tree(int RunNumber = 0){
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

  auto scaler_current_list = d_scaler.Take<double>("P.BCM4B.scalerCurrent"); 
  auto scaler_event_list = d_scaler.Take<double>("evNumber");
  std::cout<<"event size "<<scaler_event_list->size()<<" current size "<<scaler_current_list->size()<<std::endl;
  int scaler_size = scaler_event_list->size();
  auto get_current = [&](unsigned int eventNum){
    int i=0;
    while( eventNum>scaler_event_list->at(i) ){
      ++i;
      if(i>scaler_current_list->size()-1)
      {
        i=scaler_current_list->size()-1;
        break;
      }
    }
    //std::cout<<scaler_current_list->at(i)<<std::endl;
    return scaler_current_list->at(i);
  };

  auto d_skim = d_events.Define("current",get_current,{"fEvtHdr.fEvtNum"});
  double all_events = *d_events.Max("fEvtHdr.fEvtNum");
  std::cout<<" all events "<<all_events<<std::endl;
  std::string of_name = "skim/skim_"+std::to_string(RunNumber)+".root";
  d_skim.Snapshot("Tcsv",of_name,{
      "fEvtHdr.fEvtNum",
      "P.cal.etottracknorm",
      "current",
      "H.cal.etottracknorm"
      });
  TFile of(of_name.c_str(),"update");
  of.Close();
  
  auto h_current = d_scaler.Histo1D({"","",100,10,100},"P.BCM4B.scalerCurrent");
  TCanvas *c = new TCanvas();
  h_current->DrawCopy("hist");
  std::string c_name = "results/yield/check/"+std::to_string(RunNumber)+"_current_check.pdf";
  c->SaveAs(c_name.c_str());
}
