#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#include <fstream>
#include "nlohmann/json.hpp"

#include "TObject.h"


void print_good_run_number(){
  int runstart, runend;
  int i,n;
  std::ofstream out_file;
  std::ofstream deleted_file;
  deleted_file.open("deleted.txt");
  std::ofstream junk_file;
  junk_file.open("junk.txt");
  std::cout<<"What range do you want? 1.custom 2.spring runs"<<"\n";
  std::cin>>i;
  switch (i)
  {
    case 1:
      std::cout<<"What range do you want? eg. 7593 7693"<<"\n";
      std::cin>>runstart>>runend;
      if (runstart>runend){std::cout<<"wrong input";
        std::quick_exit(3);}
      out_file.open("custom_good_runlist.txt");
      break;
    case 2:
      runstart = 7593;
      runend = 7830;
      out_file.open("spring_good_runlist.txt");
      break;
    default:
      std::cout<<"wrong input"<<"\n";
      std::quick_exit(3);
      break;
  }
  std::cout<<"How strick do you want to choose good runs? \n 1.Trenta(Delete this run if it has comment) \n 2. Venti(Delete this run if it has \" junk \" in comment, ask for permit if it doesn't have \" junk \") \n 3.Grande(Delete this run if it has \" junk \" in comment,save other runs)\n  4.Tall(Ask for permit for all runs if it has comment. Caution: A lot of works)\n";
  std::cin>>n;

  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("db2/run_list_update.json");
    json_input_file >> j;
  }
  json k;
  {
    std::ifstream json_input_file2("db2/run_list_extra.json");
    json_input_file2>>k;
  }
  std::string in;
  std::string str1 ("junk");
  switch (n)
  {
    case 1:
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        if (std::stoi(it.key())>=runstart && std::stoi(it.key()) <=runend){
          if(k.count(it.key())!= 0){deleted_file<<it.key()<<"\n";
            std::cout<<std::type_id(it.key()).name()<<"\n";}
          else{out_file<<it.key()<<"\n";}
        }
      }
      break;

    case 2:
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        if (std::stoi(it.key())>=runstart && std::stoi(it.key()) <=runend){
          if(k.count(it.key())!= 0){
            auto runjs = k[it.key()];
            std::string str2 = runjs["comment"].get<std::string>();
            if(str2.find(str1)<str2.length()){junk_file<<it.key()<<" "<<str2<<"\n";}
            else{
              std::cout<<"Do you want to delete this run?(y or n) "<<"Run "<<it.key()<<"\n";
              std::cout<<"comment: "<<str2<<"\n";
              std::cin>>in;
              if(in.compare("y")==0){std::cout<<"Run "<<it.key()<<" is deleted. "<<"\n";
              deleted_file<<it.key()<<" "<<str2<<"\n";
              }
              else{
                if(in.compare("n")==0){
                  out_file<<it.key()<<"\n";
                  std::cout<<"Run "<<it.key()<<" is saved."<<"\n";
                }
                else{std::cout<<"wrong input"<<"\n";
                  std::quick_exit(3);}
              }

            }
          }
          else{out_file<<it.key()<<"\n";}
        }
      }
      break;

    case 3:
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        if (std::stoi(it.key())>=runstart && std::stoi(it.key()) <=runend){
          if(k.count(it.key())!= 0){
            auto runjs = k[it.key()];
            std::string str2 = runjs["comment"].get<std::string>();
            if(str2.find(str1)<str2.length()){deleted_file<<it.key()<<" "<<str2<<"\n";}
            else{out_file<<it.key()<<"\n";}
          }
          else{out_file<<it.key()<<"\n";}
        }
      }
      break;

    case 4:
      for (json::iterator it = j.begin(); it != j.end();++it){
        if (std::stoi(it.key())>=runstart && std::stoi(it.key())<=runend){
          if (k.count(it.key())!= 0){
            auto runjs = k[it.key()];
            std::string str2 = runjs["comment"].get<std::string>();
            std::cout<<"Do you want to delete this run?(y or n)"<<"\n";
            std::cout<<"comment"<<str2<<"\n";
            std::cin>>in;
            if(in.compare("y")==0){std::cout<<"Run "<<it.key()<<" is deleted."<<"\n";
            deleted_file<<it.key()<<" "<<str2<<"\n";
            }
            else{
              if(in.compare("n")==0){
                out_file<<it.key()<<"\n";
                std::cout<<"Run "<<it.key()<<" is saved."<<"\n";
              }
              else{std::cout<<"wrong input"<<"\n";
                std::quick_exit(3);}
            }
          }
        }
      }
      break;


  }
}
