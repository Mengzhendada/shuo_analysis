#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void make_json(){
  json j;
  j["pi"] = 3.141;
  j["happy"] = true;
  std::ofstream o("pretty.json");
  o<<std::setw(4)<<j<<std::endl;
  json j2 = {
    {"answer",{{"everything",42}
              }},
    {"object",{
                {"currency","USD"},
                {"value",42.99}
   
              } },
  };
  std::ofstream o2("j2.json");
  o2<<std::setw(4)<<j2<<std::endl;
}
