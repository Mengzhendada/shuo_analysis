#include "Get_all_eff.h"
#include <iostream>
void Get_eff(int RunNumber = 0){
  if(RunNumber ==0){
    std::cout<<"Enter RunNumber"<<std::endl;
    std::cin>>RunNumber;
  }
  std::cout<<Get_all_eff(RunNumber)<<std::endl;
}
