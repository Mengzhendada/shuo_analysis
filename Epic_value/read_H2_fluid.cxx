#include <vector>
#include <iostream>
#include <fstream>

void read_H2_fluid(){
  std::ifstream infile("Target_fluid_property/H2_22d41K.txt");
  infile.ignore(5000,'\n');
  double a,b;
  while(infile.good()){
    std::string line = getline()
  }
}
