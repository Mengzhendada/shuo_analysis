#include <iostream>
#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#include <fstream>
#include "nlohmann/json.hpp"

void txt_json(){
ifstream infile;
infile.open("runfallselected.txt");
ofstream outfile;
outfile.open("");
using nlohmann::json;
json j;
j.open("runfallselected.json");
int runnumber;
while(infile>>runnumber){
    j.push_back(runnumber);
}

}
