#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
  using json = nlohmann::json; 
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

double get_normfac(int RunNumber){
  std::string hist_file_name = "/group/c-csv/shuo/simc_gfortran/outfiles/csv"+std::to_string(RunNumber)+".hist";
  std::ifstream hist_file(hist_file_name.c_str());
  std::string line;
  double normfac_num;
  while(std::getline(hist_file,line)){
    if(line.find("normfac")<line.length()){
      size_t i = 0;
      for(; i< line.length();i++){ if(isdigit(line[i])) break;}
      normfac_num = std::stod(line.substr(i,line.size()));
      std::cout<<normfac_num<<std::endl;;
      return normfac_num;
    }
  }
  std::cout << "No normfac found" << std::endl;
  return 0;
}
void make_simc_json(int RunNumber = 0){

  if(RunNumber == 0){
    std::cout<<"Enter RunNumber Number(-1 to exit) ";
    std::cin>>RunNumber;
    if(RunNumber < 0)
      return;
  }
  json j_spring;
  {
    std::ifstream ifs("db2/run_list_update.json");
    ifs>>j_spring;
  }
  json j_fall;
  {
    std::ifstream ifs("db2/run_list.json");
    ifs>>j_fall;
  }
  json jout;
  {
  std::ifstream simc_json("simc.json");
  simc_json>>jout;
  }
  if(RunNumber < 7000){
    std::cout<< RunNumber<<std::endl;
    jout[(std::to_string(RunNumber)).c_str()]["hpcentral"] = std::abs(j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_momentum"].get<double>());
    std::cout<< std::abs(j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_momentum"].get<double>())<<std::endl;
    jout[(std::to_string(RunNumber)).c_str()]["htheta_lab"] = j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_angle"].get<double>();
    std::cout<< j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_angle"].get<double>()<<std::endl;
    jout[(std::to_string(RunNumber)).c_str()]["ppcentral"] = std::abs(j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_momentum"].get<double>());
    std::cout<< std::abs(j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_momentum"].get<double>())<<std::endl;
    jout[(std::to_string(RunNumber)).c_str()]["ptheta_lab"] = j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_angle"].get<double>();
    std::cout<< j_fall[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_angle"].get<double>()<<std::endl;
    jout[(std::to_string(RunNumber)).c_str()]["charge_simc"] = 1; 
    jout[(std::to_string(RunNumber)).c_str()]["ComputerLT"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["shmsTE"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["hmsTE"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["normfac"] = get_normfac(RunNumber);
    //std::cout<<get_normfac(RunNumber)<<std::endl;
  }
  else{  
    jout[(std::to_string(RunNumber)).c_str()]["hpcentral"] = std::abs(j_spring[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_momentum"].get<double>());
    jout[(std::to_string(RunNumber)).c_str()]["htheta_lab"] = j_spring[(std::to_string(RunNumber)).c_str()]["spectrometers"]["hms_angle"].get<double>();
    jout[(std::to_string(RunNumber)).c_str()]["ppcentral"] = std::abs(j_spring[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_momentum"].get<double>());
    jout[(std::to_string(RunNumber)).c_str()]["ptheta_lab"] = j_spring[(std::to_string(RunNumber)).c_str()]["spectrometers"]["shms_angle"].get<double>();
    jout[(std::to_string(RunNumber)).c_str()]["charge_simc"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["ComputerLT"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["shmsTE"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["hmsTE"] = 1;
    jout[(std::to_string(RunNumber)).c_str()]["normfac"] = get_normfac(RunNumber);
  }
  std::ofstream simc_json("simc.json");
  simc_json<<jout.dump(4)<<std::endl;
}
