#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <numeric>

auto Average(std::vector<int> v)
{
  return std::accumulate(v.begin(),v.end(),0)/v.size();
}
auto Standard_Derivation(std::vector<int> v, double mean){
  double stupid=0;
  for(int i = 0;i<v.size();i++){stupid+=(v[i]-mean)*(v[i]-mean);}
  return std::sqrt(stupid/v.size());
}
auto Most_Common(std::vector<int> v){
  sort(v.begin(),v.end());
  auto most_value=*v.begin();
  auto test = *v.begin();
  int max_cnt = 0;
  int cnt =0;
  for(auto item : v){
    if(item == test){
      cnt++;
    }
    else{
      if(cnt>max_cnt){
        most_value = test;
        max_cnt = cnt;
      }
      test = item;
      cnt = 1;
    }
  }
  return most_value;
}


void getline_test(){
  std::ifstream infile;
  infile.open("test.txt");
  std::string word,line,temp;
  int n; 
  word = "what";
  int number=0;  
  std::vector<int> ns;
  int ii=0;
  while(std::getline(infile,line))
  { 
    if(line.find(word)<line.length())
    {
      std::cout<<line<<" "<<ii<<std::endl;
      ++ii;
      for(int i = word.size(); i<line[i];i++){
        if(isdigit(line[i])){
          for(int a = i; a<line.size();a++){
            temp += line[a];
          }
          break;  
        }
      }
      std::cout<<"temp"<<temp<<std::endl;
      //temp.clear();
      std::istringstream stream(temp);
      stream>>n;
      std::cout<<"number "<<n<<std::endl;
      temp.clear();
      ns.push_back(n);
    }
  }
  auto max = *max_element(ns.begin(),ns.end());
  auto min = *min_element(ns.begin(),ns.end());
  std::cout<<"max "<<max<<" min "<<min<<std::endl;
  double average = Average(ns);
  double stdder = Standard_Derivation(ns,average);
  double most = Most_Common(ns);
  std::cout<<"average "<<average<<"Standard derviation "<<stdder<<"most "<<most<<std::endl;
  std::cout<<ns.size()<<std::endl;
}
