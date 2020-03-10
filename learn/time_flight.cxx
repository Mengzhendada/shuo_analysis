#include <iostream>
#include <string>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TPaveText.h"
//convert unit GeV to kg
double GeV = 1.78266192e-27;
double c = 299792458;
double t_e(double p){
  double m = 0.00051;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_pi(double p){
  double m = 0.139;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K(double p){
  double m = 0.493;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_proton(double p){
  double m = 0.938;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
void time_flight(){
  TF1 *fte = new TF1("te","t_e(x)-t_e(x)",1.5,6);
  TF1 *ftpi = new TF1("tpi","t_pi(x)-t_e(x)",1.5,6);
  TF1 *ftk = new TF1("tk","t_K(x)-t_e(x)",1.5,6);
  TF1 *ftproton = new TF1("tproton","t_proton(x)-t_e(x)",1.5,6);
  TCanvas *c = new TCanvas();
  gStyle->SetOptTitle(0);
  ftpi->GetYaxis()->SetRangeUser(0,10);
  ftpi->Draw();
  fte->SetLineColor(kBlack);  
  fte->Draw("same");
  ftk->SetLineColor(kOrange);
  ftk->Draw("same");
  ftproton->SetLineColor(kSpring);
  ftproton->Draw("same");
  //c->SetTitle("t_hardron - t_e");
 //  TPaveText *title = new TPaveText(.11,.95,.35,.99,"brndc");
 //  title->AddText("t_hardron - t_e");
 //  c->cd();
 //  title->Draw();
   TLegend *leg = c->BuildLegend(0.7,0.7,0.95,0.95);
   c->SaveAs("time_flight.pdf");
  double momentum;
  std::cout<<"Enter your momentum :";
  std::cin>>momentum;
  double tpi = t_pi(momentum)-t_e(momentum);
  double tk = t_K(momentum)-t_e(momentum);
  double tproton = t_proton(momentum)-t_e(momentum);
  std::cout<<" time of flight compared to electrons,(pi,K,proton)" <<std::endl;
  std::cout<<tpi<<" "<<tk<<" "<<tproton<<std::endl;
  //std::cout<<" if we set pi peak at 1, then position "<<std::endl;
  //std::cout<<fmod(t)
}
