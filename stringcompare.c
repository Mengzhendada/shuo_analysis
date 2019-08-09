#include <iostream>
#include <string>

int main(){

  std::string str1 ("junk");
  std::string str2 ("this is jnk");
  if (str2.find(str1)<str2.length())
  {std::cout<<str2<<" contains "<<str1<<"\n";}
  else{std::cout<<"no"<<"\n";}
  return 0;
}
