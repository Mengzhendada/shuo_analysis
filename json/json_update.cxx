#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "nlohmann/json.hpp"

#include "TObject.h"


void json_update(){

  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("/group/c-csv/shuo/online_csv/db2/run_list_updated.json");
    json_input_file >> j;
  }

  //std::cout << j.dump(2);

  //std::cout << " runs : ";
  //std::vector<int> runs;
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
  //auto runjs = it.value();
    if(std::stoi(it.key())>7593)
    { //std::cout<<it.value()<<std::endl;
      it.value()["beam_energy"]=10.214;
  }
  }
  std::ofstream o("runlist_updated.json");
  o<<j<<std::endl;
}
