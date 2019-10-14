#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TLine.h"
#include "TEllipse.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

void kin_acceptance(int RunNumber = 0){

  double line_x_max;
  double line_x_min;
  double line_q2_max;
  double line_q2_min;
  double r1;
  double r2;
  double r3;

  if(RunNumber == 0){
    std::cout<<"Enter a RunNumber (-1 to exit):";
    std::cin>>RunNumber;
    if(RunNumber<=0)
      return;
  }
  std::string

}
