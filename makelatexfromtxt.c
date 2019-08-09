#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>

void makelatexfromtxt(){
ifstream infile;
std::string runnumber;
infile.open("runspringselected.txt");
ofstream outfile;
outfile.open("latexselected.txt");
using nlohmann::json;
json j;
{std::ifstream json_input_file("run_list.json");
json_input_file>>j;
}
//std::cout<<j<<std::endl;

  while(infile>>runnumber){
  
  std::cout<<runnumber<<std::endl;;
  outfile<<"\\begin{frame}[fragile]{run "<<runnumber<<"}"<<"\n";
  outfile<<" \\begin{figure} [h]"<<"\n";
  outfile<<"  \\includegraphics[scale = 0.45]{coin_replay_production_"<<runnumber<<"_-1_0_-1.pdf}"<<"\n";
  outfile<<" \\caption{run"<<runnumber<<"}"<<"\n";
  outfile<<" \\label{fig:fig"<<runnumber<<"}"<<"\n";
  outfile<<"\\end{figure}"<<"\n";
//  auto it = j.find(runnumber);
//  if(it == j.end()){
//    std::cerr << "no runnumber: " << runnumber << " was found!" << std::endl;
//    continue;
//  }
  //std::cout<<*(j.find(runnumber))<<std::endl;
//  auto runjs = *it;
//  auto momentum = runjs["spectrometers"]["shms_momentum"].get<double>();
  //std::cout<<momentum<<"\n";
//  outfile<<"Figure \\ref{fig:fig"<<runnumber<<"} shows run "<<runnumber<<"with shms momentum "<<momentum<<".\n";
  outfile<<"\\end{frame}"<<"\n";
}
}
