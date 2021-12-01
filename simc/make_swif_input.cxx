#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
int make_swif_input(){
  json j_input;
  int cpuCores = 1;
  long int diskBytes = 20000000000;
  std::string name = "csv";
  std::string os = "centos77";
  std::string project = "c-csv";
  long int ramBytes = 5000000000;
  std::string shell = "/usr/bin/bash";
  int timeSecs = 28800;
  std::string track = "analysis";
  std::string workflow_name,command,err_out,out_out;
  std::cout<<"Default values: \n"<<"cpuCores: "<<cpuCores<<"\n diskBytes: "<<diskBytes<<"\n name: "<<name<<"\n os: "<<os<<"\n project: "<<project<<"\n ramBytes: "<<ramBytes<<"\n shell: "<<shell<<"\n timeSecs: "<<timeSecs<<"\n track: "<<track<<"\n no changes. if you want to change, go to make_swif_input.cxx script"<<std::endl;
  workflow_name = "csv_simc";
  command = "/home/shuojia/shuo/simc_gfortran/script/";//will add executable in for loop later
  err_out = "/home/shuojia/shuo/simc_gfortran/outfiles/";//will add exact file name later in for loop
  out_out = "/home/shuojia/shuo/simc_gfortran/outfiles/";//will add same as above
  std::cout<<"workflow name: "<<workflow_name<<"\n command: "<<command<<"\nerror_out: "<<err_out<<"\nout_out: "<<out_out<<"\nDefault path, go to make_swif)input.cxx script to change"<<std::endl;
  //std::cout<<"\n Enter workflow name: "<<std::endl;
  //std::cin>>workflow;
  //std::cout<<"\n Enter the executabel file with full path (/path/to/file.sh)"<<std::endl;
  //std::cin>>command;
  //std::cout<<"\n Enter outfile position: (/path/to/file.out), I will add rungroup number"<<std::endl;
  //std::cin>>out_out;
  j_input["name"] = workflow_name;
  std::vector<json> j_jobs;
  for(int i = 1;i<59;i++){
    //  read out 
    //  std::cout<<j_jobs[i]["command"].get<string>()<<std::endl; 
    std::string command_new,errout,outout;
    command_new = command+"run"+std::to_string(10*i)+".sh";
    errout = err_out+workflow_name+"_"+std::to_string(10*i)+".err";
    outout = out_out+workflow_name+"_"+std::to_string(10*i)+".out";
    json j_each;
    j_each["command"] = command_new;
    j_each["cpuCores"] = cpuCores;
    j_each["diskBytes"] = diskBytes;
    j_each["name"] = name;
    j_each["os"] = os;
    j_each["project"] = project;
    j_each["ramBytes"] = ramBytes;
    j_each["shell"] = shell;
    j_each["stderr"] = errout;
    j_each["stdout"] = outout;
    j_each["timeSecs"] = timeSecs;
    j_each["track"] = track;

    j_jobs.push_back(j_each); 
  }
  j_input["jobs"] = j_jobs;
  std::ofstream ofs("shuo_analysis/simc/csv_simc.json");
  ofs<<j_input.dump(4)<<std::endl;
  return 0;
}
