#include <iostream>
int print(int i = 0){
  if(i==0){
    std::cin>>i;
  }
  std::cout<<i<<std::endl;
  return 0;
}
