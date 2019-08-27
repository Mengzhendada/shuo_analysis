#include <iostream>
#include <string>

void getline_test(){
  std::ifstream infile;
  infile.open("jlabana/test.txt");
  std::string word,line,temp;
  int n; 
  word = "what";
  int number=0;  
  while(std::getline(infile,line))
  if(line.find(word)<line.length())
  {
    std::cout<<line<<std::endl;
    for(int i = 0; i<line[i];i++){
    if(isdigit(line[i])){
      for(int a = i; a<line.size();a++){
      temp += line[a];
      }
      break;
    }
    }
    std::cout<<temp<<std::endl;
    //temp.clear();
    std::istringstream stream(temp);
    stream>>n;
    std::cout<<n<<std::endl;
    temp.clear();
  }
}
