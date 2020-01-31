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
  double current_offset = 0.8; 
  TCanvas* c_current = new TCanvas();
  h_current->DrawClone();
  c_current->SaveAs("current.pdf");
  std::cout << "Most prevalent current in this run is: " << target_current
            << "uA\n";
  std::cout << "Setting current offset at  " << current_offset*target_current << "uA\n";
  
  auto h_current_time = d_scaler.Histo2D({"current vs. time", "",1000,0,3000,1000,0,100},"P.1MHz.scalerTime","P.BCM4B.scalerCurrent");
  TCanvas *c_current_time = new TCanvas;
  h_current_time->SetMarkerStyle(8);
  h_current_time->DrawClone();
  c_current_time->SaveAs("current_time.pdf");

  //auto h_evNumber = d_scaler.Histo1D({"evNumber","",100,0,})
  auto scaler_current_list = d_scaler.Take<double>("P.BCM4B.scalerCurrent"); 
  auto scaler_event_list = d_scaler.Take<double>("evNumber");
  
  //for (auto it = scaler_current_list->begin();it!=scaler_current_list->end();++it){
  //  int eventNumber = scaler_event_list->at(it-scaler_current_list->begin());
  //  std::cout<<"for event "<<eventNumber <<" it's "<<*it<<std::endl;
  //}

  auto get_current = [&](unsigned int eventNum){
    int i=0;
    while(eventNum>scaler_event_list->at(i)){
    ++i;
    }
    //std::cout<<scaler_current_list->at(i)<<std::endl;
    return scaler_current_list->at(i);
  };

  auto d_events_current = d_events.Define("current",get_current,{"fEvtHdr.fEvtNum"});

  auto h_event_current = d_events_current.Histo1D({"","",1000,0,100},"current");
  TCanvas* c_event_current = new TCanvas();
  h_event_current->SetMarkerStyle(8);
  h_event_current->DrawClone();
  c_event_current->SaveAs("current.pdf");

  auto event_current = d_events_current.Graph("fEvtHdr.fEvtNum","current");
  std::string good_current = "current >= "+std::to_string(current_offset*target_current);
  std::cout<<good_current<<std::endl;
  auto event_current_good = d_events_current.Filter(good_current).Graph("fEvtHdr.fEvtNum","current");
  TCanvas* c_graph = new TCanvas();
  event_current->SetMarkerStyle(8);
  event_current->Draw("ap");
  event_current_good->SetMarkerStyle(8);
  event_current_good->SetMarkerColor(2);
  event_current_good->Draw("p same");
  c_graph->SaveAs("event_current.pdf");

}

