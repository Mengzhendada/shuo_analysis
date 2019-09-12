#include "TGraph.h"

void testrange(){
  double x[10] = {1,2,3,4,5,6,7,8,9,10};
  double y[10] = {1.2,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2};
  TGraph *gr = new TGraph(10,x,y);
  gr->SetMarkerStyle(20);
  gr->GetXaxis()->SetLimits(2,4);
  gr->GetYaxis()->SetLimits(1,6);
  gr->GetXaxis()->SetRangeUser(2,4);
  gr->GetYaxis()->SetRangeUser(1,6);
  gr->Draw("APL");
}
