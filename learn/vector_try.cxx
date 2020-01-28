#include <vector>

void vector_try(){
  std::vector<int> myvector = {1,2,3,1,32,21,3,1,3,4,1,3,2,1,3,1,3,12,2};
  for(auto it = myvector.begin();it!=myvector.end();++it){
    std::cout<<it-myvector.begin()<<std::endl;
    std::cout<<it<<std::endl;
  }
}
