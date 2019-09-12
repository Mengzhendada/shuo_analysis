#include <iostream>
#include <fstream>
#include <string>

void test(int Runnumber){
  std::ofstream out;
  out.open("shuo_analysis/parallel.txt",std::ios_base::app);
  out<<Runnumber<<"1"<<std::endl;

}
