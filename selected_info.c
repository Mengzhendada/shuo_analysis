#include <iostream>
#include "nlohmann/json.hpp"

void selected_info(){
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file_fall("run_list.json");
    json_input_file_fall>>j;
  }
  json k;
  {
    std::ifstream json_input_file_spring("run_list_update.json");
    json_input_file_spring>>k;
  }
  std::string runnumber;
  std::ifstream infile_fall;
  infile_fall.open("runfallselected.txt");
  std::ifstream infile_spring;
  infile_spring.open("runspringselected.txt");
  std::ofstream outfile_fall,outfile_spring;
  outfile_fall.open("fall_selected_info.txt");
  outfile_spring.open("spring_selected_info.txt");
  int i = 0,ii = 0;
  while(infile_fall>>runnumber){
    auto it = j.find(runnumber);
    auto runjs = *it;
    outfile_fall<<runnumber<<" "<<runjs["spectrometers"]["shms_momentum"].get<double>()<<"\n";
  i++;
  }
    while(infile_spring>>runnumber){
      auto it = k.find(runnumber);
      auto runjs = *it;
      outfile_spring<<runnumber<<" "<<runjs["spectrometers"]["shms_momentum"].get<double>()<<"\n";
    ii++;

    }
    std::cout<<"fall "<<i<<"spring "<<ii<<std::endl;
}
