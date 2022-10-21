#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <iomanip>
#include <streambuf>
#include <math.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RVec.hxx"
#include "TVector3.h"
#include "ROOT/RSnapshotOptions.hxx"
#include "TLine.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"

double c = 299792458;
double shms_length = 20.1;
double t_pi(double p){
  double m = 0.139;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p);
}
double t_K_peak_position(double p){
  double m = 0.493;
  double real_pi_time = t_pi(p)-1;
  return (shms_length*std::sqrt(p*p+m*m)*1e9)/(c*p)-real_pi_time;
}
double t_pi_fromkaondecay(double p,double position){
  double m_pi = 0.139;
  double m_K = 0.493;
  double kaon_fly_time = position*std::sqrt(p*p+m_K*m_K)*1e9/(c*p);
  double pi_fly_time = (shms_length-position)*std::sqrt(p*p+m_pi*m_pi)*1e9/(c*p);
  double sum_time = kaon_fly_time+pi_fly_time;
  double real_pi_time = t_pi(p)-1;
  return sum_time-real_pi_time;
}
double gaus_fun_kaonNodecay(double x,double *params){
  //param[1] is the momentum, gaus peak mean of the kaon that doesn't decay is calculated by the momentum
  double kaon_nodecay_peak = t_K_peak_position(params[1]);
  //std::cout<<"check kaon "<<kaon_nodecay_peak<<std::endl;
  double gaus_shape = params[0]*exp(-0.5*pow((x-kaon_nodecay_peak)/params[2],2));///(params[2] *sqrt(2*M_PI));
  //std::cout<<"check gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
  
}
double gaus_fun_pion_kaondecay(double x,double *pa){
//pa[1] is the momentum,
//pa[0] is the probability at the position/78.1%(the probability that kaon doesn't decay at all) of kaon decays multiplied to the fitting kaon peak amplitude(params[0]), pa[2] is the sigma of pion peak, which I use 0.2
//pa[3] is the position where the kaon decay
//gaus peak mean of the pions that kaon decays is calculated by momentum pa[1],and the position where kaon decays pa[3]    
  double pi_fromkaondecay_peak = t_pi_fromkaondecay(pa[1],pa[3]);
  double gaus_shape = pa[0]*exp(-0.5*pow((x-pi_fromkaondecay_peak)/pa[2],2));///(pa[2] *sqrt(2*M_PI));
  //std::cout<<"check pion "<<pi_fromkaondecay_peak<<" gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
}
double fit_kaondecay(double x,double *params){
  //params[0] is the amplitude of nodecay kaon need to be determined by fitting
  //params[1] is the momentum, the mean peak is calculated by the momentum, need to be fixed to be what it is
  //params[2] is the sigma of the nodecay kaon, need to be determined by fitting
  double all_kaon = 0;
  std::ifstream infile("db2/kaoncontam.inp");
  //std::cout<<"type meter prob_left prob_right a b c d e f "<<std::endl;
  //double nodecay_kaon = gaus_fun_kaonNodecay(x,params); This should be already in the last line of the loop
  //all_kaon = all_kaon+nodecay_kaon;
  string line;
  while(getline(infile,line)){
    double type,meter,prob_left,prob_right,a,b,c,d,e,f;
    std::stringstream ss;
    ss.str(line);
    ss>>type>>meter>>prob_left>>prob_right>>a>>b>>c>>d>>e>>f;
    //std::cout<<type<<" "<<meter<<" "<<prob_left<<" "<<prob_right<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<std::endl;
    double pa[4];
    pa[0] = params[0]*(prob_left+prob_right)/(2*0.8235);//the amplitude of the pion(from kaon decay) peak portion of nodecay kaons. 
    pa[1] = params[1];
    pa[2] = 0.2;
    pa[3] = meter;
    double pi_fromkaondecay =  gaus_fun_pion_kaondecay(x,pa);
    //std::cout<<"check"<<all_kaon<<std::endl;
    all_kaon+=pi_fromkaondecay;
  }
  //std::cout<<"check in fit function "<<all_kaon<<std::endl;
  return all_kaon;
}
void kaon_contam_func(){
  double parameters[3];
  parameters[0] = 100;
  parameters[1] = 3.9;
  parameters[2] = 0.2;
  double fit = fit_kaondecay(1.4,parameters);
  std::cout<<fit<<std::endl;
  TGraph *g_kaondecay = new TGraph();
  TGraph *g_gaus = new TGraph();
  double rftime = 0;
  for(int i = 0;i<400;++i){
    g_kaondecay->SetPoint(i,rftime,fit_kaondecay(rftime,parameters));
    
    double kaon_gaus = gaus_fun_kaonNodecay(rftime,parameters);
    g_gaus->SetPoint(i,rftime,kaon_gaus);
    rftime+=0.01;
  }
  std::ifstream infile("db2/kaoncontam.inp");
  //std::cout<<"type meter prob_left prob_right a b c d e f "<<std::endl;
  string line;
  TMultiGraph* mg_pion_fromkaondecay = new TMultiGraph();
  while(getline(infile,line)){
    double type,meter,prob_left,prob_right,a,b,c,d,e,f;
    std::stringstream ss;
    ss.str(line);
    ss>>type>>meter>>prob_left>>prob_right>>a>>b>>c>>d>>e>>f;
    //std::cout<<type<<" "<<meter<<" "<<prob_left<<" "<<prob_right<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<std::endl;
    double pa[4];
    pa[0] = parameters[0]*(prob_left+prob_right)/(2*0.8235);//the amplitude of the pion(from kaon decay) peak portion of nodecay kaons. 
    pa[1] = parameters[1];
    pa[2] = 0.2;
    pa[3] = meter;
    //std::cout<<"check"<<all_kaon<<std::endl;
    double rftime = 0;
    TGraph *g_pion = new TGraph();
    for(int i = 0;i<400;++i){
      double pi_fromkaondecay =  gaus_fun_pion_kaondecay(rftime,pa);
      g_pion->SetPoint(i,rftime,pi_fromkaondecay);

      rftime+=0.01;
    }
    g_pion->SetMarkerColor(kBlue);
    mg_pion_fromkaondecay->Add(g_pion,"P");
  }
  mg_pion_fromkaondecay->SetTitle("kaon decay");
  g_kaondecay->SetTitle("Sum");
  mg_pion_fromkaondecay->Add(g_kaondecay,"P");
  TCanvas *c_kaondecay = new TCanvas();
  //g_kaondecay->SetMarkerStyle(8);
  c_kaondecay->SetGrid();
  //g_gaus->SetMarkerStyle(8);
  //g_gaus->SetMarkerColor(kRed);
  //g_kaondecay->Draw("AL");
  //g_gaus->Draw("P same");
  //mg_pion_fromkaondecay->SetMarkerColor(kBlue);
  //c_kaondecay->SetLogy(true);
  //mg_pion_fromkaondecay->GetYaxis()->SetRangeUser(0.1,15);
  //mg_pion_fromkaondecay->GetXaxis()->SetRangeUser(0.1,3);
  mg_pion_fromkaondecay->Draw("A");
  mg_pion_fromkaondecay->GetXaxis()->SetTitle("rftime");
  mg_pion_fromkaondecay->GetYaxis()->SetTitle("counts");
  //c_kaondecay->BuildLegend();
  c_kaondecay->SaveAs("results/kaon_contam.pdf");
}
