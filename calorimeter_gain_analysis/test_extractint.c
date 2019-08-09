#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
void test_extractint(){
	std::ifstream ifs("pcal.param.coin_replay_production_6248_-1_0_-1");
	std::string temp;
        ifs.ignore(5000,',');
        std::string str;
        std::getline(ifs,str);
	int number = 0;
	for(int i = 0;i<str.size();i++){
		if(isdigit(str[i])){
			for (int a = i;a<str.size();a++){
				temp +=str[a];
			}
			break;		
		}
	}
	std::istringstream stream(temp);
	stream>>number;

	std::cout<<number<<std::endl;
}
