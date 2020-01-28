#include <vector>
#include <bits/stdc++.h>
#include <numeric>

void find_most_common(){
int A[4] ={0,2,3,1};
const int N = sizeof(A)/sizeof(int);
int n = std::distance(A,max_element(A,A+N));
int max = *max_element(A,A+N);
std::cout<<n<<" "<<max<<std::endl;
return 0;
}
