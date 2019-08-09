#include <string>
#include <iostream>
void creat_txt(){
int i,j;
std::string line;
ofstream out("creat_txt.txt");
ofstream out1("creat_txt1.txt");
ofstream out2("creat_txt2.txt");
//line = ".Define(\"";
line = "t.Branch(\"";
for (i = 0;i<14;i++){
  out<<"{\""<<i<<"\","<<"rc[in]->neg()["<<i<<"]},"<<std::endl;
  out1<<"{\""<<i<<"\","<<"rc[in]->pos()["<<i<<"]},"<<std::endl;
  //out<<"pos_"<<std::to_string(i)<<" = rc[i]->pos()["<<std::to_string(i)<<"]"<<std::endl;
  //out<<"neg_"<<std::to_string(i)<<" = rc[i]->neg()["<<std::to_string(i)<<"]"<<std::endl;
  
  //out<<line<<"pos_"<<std::to_string(i)<<"\",&pos_"<<std::to_string(i)<<");"<<std::endl;;
  //"\",[&b,rc](){return rc[b++]->pos()["<<std::to_string(i)<<"];})"<<std::endl;
//out<<line<<"neg_"<<std::to_string(i)<<"\",&neg_"<<std::to_string(i)<<");"<<std::endl;
//"\",[&b,rc](){return rc[b++]->neg()["<<std::to_string(i)<<"];})"<<std::endl;
  out2<<"{\""<<i<<"\","<<"{"<<std::endl;
for (j = 0;j<16;j++){
  out2<<"{\""<<j<<"\","<<"rc[in]->arr()["<<i<<"]["<<j<<"]},"<<std::endl;
  //out2<<"arr_"<<std::to_string(i)<<"_"<<std::to_string(j)<<" = rc[i]->arr()["<<std::to_string(i)<<"]["<<std::to_string(j)<<"];"<<std::endl;
//out2<<line<<"array_"<<std::to_string(i)<<"_"<<std::to_string(j)<<"\",&arr_"<<std::to_string(i)<<"_"<<std::to_string(j)<<");"<<std::endl;
  //"\",[&b,rc](){return rc[b++]->arr()["<<std::to_string(i)<<"]["<<std::to_string(j)<<"];})"<<std::endl;
}
out2<<"},"<<std::endl;
}
} 
