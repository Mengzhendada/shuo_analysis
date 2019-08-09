#include <iostream>

void readtxt(){
  std::ifstream infile;
  infile.open("selected_info.txt");
  int in = 0, a = 0;
  double b = 0;
  while (infile.good()){
    infile>>a>>b;
    std::cout<<a<<" "<<b<<"\n";
  }
}
