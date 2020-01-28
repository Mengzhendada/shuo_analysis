#include <iostream>
#include <iomanip>

void double_precision(){
  long double f = 1.2344558382717;
  //std::cout<<f<<std::endl;
  //std::setprecision(3)<<f;
  //double f2 = f;
  //std::cout<<f2<<std::endl;
  int f1 = f*100;
  std::cout<<f1<<std::endl;
  double f2 = (double)f1/100;
  std::cout<<f2<<std::endl;
}
