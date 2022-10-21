#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RSnapshotOptions.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TVector3.h"
#include "Math/IFunction.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include <iostream>
const double GeV         = 1.78266192e-27;
const double c           = 299792458;
const double shms_length = 21.2;

double t_e(double p) {
  const double m = 0.00051;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_pi(double p) {
  const double m = 0.139;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_K(double p) {
  const double m = 0.493;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_proton(double p) {
  const double m = 0.938;
  return (shms_length * std::sqrt(p * p + m * m) * 1e9) / (c * p);
}
double t_pi_fromkaondecay(double p, double position, double pion_peak) {
  const double m_pi          = 0.139;
  const double m_K           = 0.493;
  double       kaon_fly_time = position * std::sqrt(p * p + m_K * m_K) * 1e9 / (c * p);
  double pi_fly_time  = (shms_length - position) * std::sqrt(p * p + m_pi * m_pi) * 1e9 / (c * p);
  double sum_time     = kaon_fly_time + pi_fly_time;
  double real_pi_time = t_pi(p) - pion_peak;
  return sum_time - real_pi_time;
}
double gaus_fun_kaonNodecay(double* x, double* params) {
  // param[1] is the momentum, gaus peak mean of the kaon that doesn't decay is calculated by the
  // momentum
  double kaon_nodecay_peak = t_K(params[3]) - t_pi(params[3]) + params[0];
  // std::cout<<"check kaon slower than pion "<<t_K(params[3])-t_pi(params[3])<<" pion position
  // "<<params[0]<<" kaon no decay position "<<kaon_nodecay_peak<<std::endl; std::cout<<"check kaon
  // "<<kaon_nodecay_peak<<std::endl;
  double gaus_shape = params[2] * exp(-0.5 * pow((x[0] - kaon_nodecay_peak) / params[4],2))
    + params[2]/15*exp(-0.5 * pow((x[0] - kaon_nodecay_peak) / (2*params[4]),2));
  // std::cout<<"check gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
}
double gaus_fun_pion_kaondecay(double* x, double* pa) {
  // pa[1] is the momentum,
  // pa[0] is the probability at the position/78.1%(the probability that kaon doesn't decay at all)
  // of kaon decays multiplied to the fitting kaon peak amplitude(params[0]), pa[2] is the sigma of
  // pion peak, which I use 0.2 pa[3] is the position where the kaon decay gaus peak mean of the
  // pions that kaon decays is calculated by momentum pa[1],and the position where kaon decays pa[3]
  double pi_fromkaondecay_peak = t_pi_fromkaondecay(pa[1], pa[3], pa[4]);
  double gaus_shape =
    //Add a background on the pion peak, due to detector inefficiency etc
    pa[0] * exp(-0.5 * pow((x[0] - pi_fromkaondecay_peak) / pa[2], 2))
    +pa[0]/15 * exp(-0.5 * pow((x[0] - pi_fromkaondecay_peak) / (2*pa[2]), 2)); 
  // std::cout<<"check pion "<<pi_fromkaondecay_peak<<" gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
}

//get the kaon decay prob for the input momentum

std::vector<std::pair<double,double>> Get_kaon_decay_prob(double shms_p){
  int i_momentum = (shms_p-1.75)/0.5;
  //std::cout<<"check which momentum"<<i_momentum<<std::endl;
  std::vector<std::vector<double>> kaon_prob;
  /*This table is for kaon decay probability, from left to right: position, prob for momentum = 1.75, 2.25,2.75..... This table is produced by Peter by analyzing SIMC. The numbers are processed in notebook/pandas_test.
    1 , 0.0115 , 0.0088 , 0.0066 , 0.0054 , 0.0038 , 0.0029 , 0.0017 , 0.0011 , 0.0006 , 0.0005,
    2 , 0.0074 , 0.0059 , 0.0049 , 0.0043 , 0.0033 , 0.0026 , 0.0016 , 0.0010 , 0.0006 , 0.0005,
    3 , 0.0056 , 0.0045 , 0.0037 , 0.0033 , 0.0026 , 0.0022 , 0.0015 , 0.0009 , 0.0006 , 0.0005,
    4 , 0.0046 , 0.0038 , 0.0032 , 0.0029 , 0.0023 , 0.0020 , 0.0013 , 0.0009 , 0.0006 , 0.0005,
    5 , 0.0033 , 0.0030 , 0.0026 , 0.0023 , 0.0020 , 0.0016 , 0.0012 , 0.0008 , 0.0006 , 0.0004,
    6 , 0.0023 , 0.0022 , 0.0021 , 0.0019 , 0.0017 , 0.0014 , 0.0011 , 0.0007 , 0.0005 , 0.0004,
    7 , 0.0020 , 0.0019 , 0.0019 , 0.0017 , 0.0016 , 0.0013 , 0.0009 , 0.0007 , 0.0005 , 0.0004,
    8 , 0.0023 , 0.0020 , 0.0018 , 0.0015 , 0.0014 , 0.0011 , 0.0008 , 0.0006 , 0.0004 , 0.0004,
    9 , 0.0026 , 0.0021 , 0.0018 , 0.0016 , 0.0014 , 0.0012 , 0.0009 , 0.0006 , 0.0005 , 0.0004,
    10 , 0.0027 , 0.0022 , 0.0019 , 0.0016 , 0.0014 , 0.0012 , 0.0009 , 0.0006 , 0.0004 , 0.0004,
    11 , 0.0027 , 0.0022 , 0.0019 , 0.0016 , 0.0014 , 0.0012 , 0.0009 , 0.0006 , 0.0004 , 0.0004,
    12 , 0.0029 , 0.0024 , 0.0021 , 0.0018 , 0.0016 , 0.0012 , 0.0009 , 0.0006 , 0.0005 , 0.0004,
    13 , 0.0038 , 0.0032 , 0.0028 , 0.0025 , 0.0021 , 0.0018 , 0.0013 , 0.0009 , 0.0007 , 0.0006,
    14 , 0.0054 , 0.0045 , 0.0040 , 0.0035 , 0.0032 , 0.0028 , 0.0023 , 0.0018 , 0.0016 , 0.0013,
    15 , 0.0064 , 0.0055 , 0.0051 , 0.0044 , 0.0042 , 0.0037 , 0.0035 , 0.0031 , 0.0028 , 0.0026,
    16 , 0.0060 , 0.0053 , 0.0051 , 0.0044 , 0.0042 , 0.0038 , 0.0034 , 0.0031 , 0.0029 , 0.0027,
    17 , 0.0048 , 0.0046 , 0.0046 , 0.0039 , 0.0040 , 0.0035 , 0.0033 , 0.0030 , 0.0028 , 0.0026,
    18 , 0.0036 , 0.0038 , 0.0042 , 0.0034 , 0.0037 , 0.0033 , 0.0031 , 0.0029 , 0.0026 , 0.0024,
    19 , 0.0107 , 0.0097 , 0.0089 , 0.0074 , 0.0070 , 0.0061 , 0.0045 , 0.0038 , 0.0030 , 0.0028,
    20 , 0.0155 , 0.0131 , 0.0112 , 0.0094 , 0.0083 , 0.0071 , 0.0050 , 0.0042 , 0.0030 , 0.0029,
    21 , 0.0149 , 0.0127 , 0.0108 , 0.0092 , 0.0081 , 0.0069 , 0.0049 , 0.0041 , 0.0030 , 0.0028,
    22 , 0.8791 , 0.8964 , 0.9089 , 0.9218 , 0.9308 , 0.9411 , 0.9551 , 0.9642 , 0.9712 , 0.9743,
    */

  kaon_prob = {
    {
      0.0115 , 0.0074 , 0.0056 , 0.0046 , 0.0033 , 0.0023 , 0.002 , 0.0023 , 0.0026 , 0.0027 , 0.0027 , 0.0029 , 0.0038 , 0.0054 , 0.0064 , 0.006 , 0.0048 , 0.0036 , 0.0107 , 0.0155 , 0.0149 , 0.8791 },
    {
      0.0088 , 0.0059 , 0.0045 , 0.0038 , 0.003 , 0.0022 , 0.0019 , 0.002 , 0.0021 , 0.0022 , 0.0022 , 0.0024 , 0.0032 , 0.0045 , 0.0055 , 0.0053 , 0.0046 , 0.0038 , 0.0097 , 0.0131 , 0.0127 , 0.8964 },
    {
      0.0066 , 0.0049 , 0.0037 , 0.0032 , 0.0026 , 0.0021 , 0.0019 , 0.0018 , 0.0018 , 0.0019 , 0.0019 , 0.0021 , 0.0028 , 0.004 , 0.0051 , 0.0051 , 0.0046 , 0.0042 , 0.0089 , 0.0112 , 0.0108 , 0.9089 },
    {
      0.0054 , 0.0043 , 0.0033 , 0.0029 , 0.0023 , 0.0019 , 0.0017 , 0.0015 , 0.0016 , 0.0016 , 0.0016 , 0.0018 , 0.0025 , 0.0035 , 0.0044 , 0.0044 , 0.0039 , 0.0034 , 0.0074 , 0.0094 , 0.0092 , 0.9218 },
    {
      0.0038 , 0.0033 , 0.0026 , 0.0023 , 0.002 , 0.0017 , 0.0016 , 0.0014 , 0.0014 , 0.0014 , 0.0014 , 0.0016 , 0.0021 , 0.0032 , 0.0042 , 0.0042 , 0.004 , 0.0037 , 0.007 , 0.0083 , 0.0081 , 0.9308 },
    {
      0.0029 , 0.0026 , 0.0022 , 0.002 , 0.0016 , 0.0014 , 0.0013 , 0.0011 , 0.0012 , 0.0012 , 0.0012 , 0.0012 , 0.0018 , 0.0028 , 0.0037 , 0.0038 , 0.0035 , 0.0033 , 0.0061 , 0.0071 , 0.0069 , 0.9411 },
    {
      0.0017 , 0.0016 , 0.0015 , 0.0013 , 0.0012 , 0.0011 , 0.0009 , 0.0008 , 0.0009 , 0.0009 , 0.0009 , 0.0009 , 0.0013 , 0.0023 , 0.0035 , 0.0034 , 0.0033 , 0.0031 , 0.0045 , 0.005 , 0.0049 , 0.9551 },
    {
      0.0011 , 0.001 , 0.0009 , 0.0009 , 0.0008 , 0.0007 , 0.0007 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0009 , 0.0018 , 0.0031 , 0.0031 , 0.003 , 0.0029 , 0.0038 , 0.0042 , 0.0041 , 0.9642 },
    {
      0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0005 , 0.0005 , 0.0004 , 0.0005 , 0.0004 , 0.0004 , 0.0005 , 0.0007 , 0.0016 , 0.0028 , 0.0029 , 0.0028 , 0.0026 , 0.003 , 0.003 , 0.003 , 0.9712 },
    {
      0.0005 , 0.0005 , 0.0005 , 0.0005 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0006 , 0.0013 , 0.0026 , 0.0027 , 0.0026 , 0.0024 , 0.0028 , 0.0029 , 0.0028 , 0.9743 }

  };


  std::vector<std::pair<double,double>> kaon_decay_prob;
  //ignore the last not-decay-kaon
  for(int i = 0;i<20;i++){
    kaon_decay_prob.push_back(std::make_pair(i+1,kaon_prob[i_momentum][i]));
    //std::cout<<"check kaon prob "<< kaon_prob[i_momentum][i]<<std::endl;
  }

  return kaon_decay_prob;

}
double Get_kaon_nodecay_prob(double shms_p){
  int i_momentum = (shms_p-1.75)/0.5;
  //std::cout<<"check which momentum"<<i_momentum<<std::endl;
  std::vector<std::vector<double>> kaon_prob;
  kaon_prob = {
    {
      0.0115 , 0.0074 , 0.0056 , 0.0046 , 0.0033 , 0.0023 , 0.002 , 0.0023 , 0.0026 , 0.0027 , 0.0027 , 0.0029 , 0.0038 , 0.0054 , 0.0064 , 0.006 , 0.0048 , 0.0036 , 0.0107 , 0.0155 , 0.0149 , 0.8791 },
    {
      0.0088 , 0.0059 , 0.0045 , 0.0038 , 0.003 , 0.0022 , 0.0019 , 0.002 , 0.0021 , 0.0022 , 0.0022 , 0.0024 , 0.0032 , 0.0045 , 0.0055 , 0.0053 , 0.0046 , 0.0038 , 0.0097 , 0.0131 , 0.0127 , 0.8964 },
    {
      0.0066 , 0.0049 , 0.0037 , 0.0032 , 0.0026 , 0.0021 , 0.0019 , 0.0018 , 0.0018 , 0.0019 , 0.0019 , 0.0021 , 0.0028 , 0.004 , 0.0051 , 0.0051 , 0.0046 , 0.0042 , 0.0089 , 0.0112 , 0.0108 , 0.9089 },
    {
      0.0054 , 0.0043 , 0.0033 , 0.0029 , 0.0023 , 0.0019 , 0.0017 , 0.0015 , 0.0016 , 0.0016 , 0.0016 , 0.0018 , 0.0025 , 0.0035 , 0.0044 , 0.0044 , 0.0039 , 0.0034 , 0.0074 , 0.0094 , 0.0092 , 0.9218 },
    {
      0.0038 , 0.0033 , 0.0026 , 0.0023 , 0.002 , 0.0017 , 0.0016 , 0.0014 , 0.0014 , 0.0014 , 0.0014 , 0.0016 , 0.0021 , 0.0032 , 0.0042 , 0.0042 , 0.004 , 0.0037 , 0.007 , 0.0083 , 0.0081 , 0.9308 },
    {
      0.0029 , 0.0026 , 0.0022 , 0.002 , 0.0016 , 0.0014 , 0.0013 , 0.0011 , 0.0012 , 0.0012 , 0.0012 , 0.0012 , 0.0018 , 0.0028 , 0.0037 , 0.0038 , 0.0035 , 0.0033 , 0.0061 , 0.0071 , 0.0069 , 0.9411 },
    {
      0.0017 , 0.0016 , 0.0015 , 0.0013 , 0.0012 , 0.0011 , 0.0009 , 0.0008 , 0.0009 , 0.0009 , 0.0009 , 0.0009 , 0.0013 , 0.0023 , 0.0035 , 0.0034 , 0.0033 , 0.0031 , 0.0045 , 0.005 , 0.0049 , 0.9551 },
    {
      0.0011 , 0.001 , 0.0009 , 0.0009 , 0.0008 , 0.0007 , 0.0007 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0009 , 0.0018 , 0.0031 , 0.0031 , 0.003 , 0.0029 , 0.0038 , 0.0042 , 0.0041 , 0.9642 },
    {
      0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0006 , 0.0005 , 0.0005 , 0.0004 , 0.0005 , 0.0004 , 0.0004 , 0.0005 , 0.0007 , 0.0016 , 0.0028 , 0.0029 , 0.0028 , 0.0026 , 0.003 , 0.003 , 0.003 , 0.9712 },
    {
      0.0005 , 0.0005 , 0.0005 , 0.0005 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0004 , 0.0006 , 0.0013 , 0.0026 , 0.0027 , 0.0026 , 0.0024 , 0.0028 , 0.0029 , 0.0028 , 0.9743 }

  };

  return kaon_prob[i_momentum][21];
}

double fit_pion(double* x, double* params) {
  double gaus_shape =
    params[0] * exp(-0.5 * pow((x[0] - params[1]) / params[2], 2)); ///(params[2] *sqrt(2*M_PI));
  return gaus_shape;
}

/** Functor class that takes fit parameters as function arguments.
 *
 */
class RFTimeFitFCN : public ROOT::Math::IBaseFunctionMultiDim {
  public:

    double SHMS_momentum = 2.0;
    TH1D * h_positive = nullptr;
    TH1D * h_negative = nullptr;
    // this should be momentum dependent?
    /*
       const std::vector<std::pair<double,double>> kaon_decay_prob =  {
       { 1 , 0.027 + 0.020},
       { 2 , 0.014 + 0.013},
       { 3 , 0.005 + 0.006},
       { 4 , 0.003 + 0.004},
       { 5 , 0.001 + 0.004},
       { 6 , 0.002 + 0.002},
       { 7 , 0.002 + 0.003},
       { 8 , 0.002 + 0.003},
       { 9 , 0.004 + 0.003},
       {10 , 0.003 + 0.004},
       {11 , 0.004 + 0.005},
       {12 , 0.005 + 0.007},
       {13 , 0.005 + 0.009},
       {14 , 0.004 + 0.008},
       {15 , 0.004 + 0.008},
       {16 , 0.002 + 0.005},
       {17 , 0.001 + 0.004},
       {18 , 0.001 + 0.004},
       {19 , 0.033 + 0.030},
       {20 , 0.050 + 0.041}};
       */
    RFTimeFitFCN(TH1D* p, TH1D* n,double mom) :h_positive(p), h_negative(n), SHMS_momentum(mom) { }

    std::vector<std::pair<double,double>> kaon_decay_prob = Get_kaon_decay_prob(SHMS_momentum);
    double pion_part(double x, double A_pi, double mu_piK, double sigma_pi) const {
      //add a background due to ADC inefficiency etc on main pion peak
      double gaus_shape = A_pi * exp(-0.5 * pow((x - mu_piK) / sigma_pi, 2))
      +A_pi/15 * exp(-0.5 * pow((x - mu_piK) / (2*sigma_pi), 2)); 
      //double gaus_shape = A_pi * exp(-0.5 * pow((x - mu_piK) / sigma_pi, 2)); 
      return gaus_shape;
    }
    double kaon_part(double x, double mu_piK,double sigma_pi, double A_K, double sigma_K ) const {
      double params[5] = {mu_piK, sigma_pi/*not used*/, A_K, SHMS_momentum, sigma_K};
      double xx = x;
      double nodecay_kaon = gaus_fun_kaonNodecay( &xx, params); 
      double pa[5];
      double all_kaon = nodecay_kaon;
      for( const auto& [meter,prob] : kaon_decay_prob ) {
        pa[0] = A_K * prob / Get_kaon_nodecay_prob(SHMS_momentum); // the amplitude of the pion(from kaon decay) peak portion of nodecay kaons.
        pa[1] = SHMS_momentum; //
        pa[2] = 0.2;   // sigma fixed
        pa[3] = meter; // distance into spectrometer
        pa[4] = mu_piK; // mu
        double pi_fromkaondecay = gaus_fun_pion_kaondecay(&xx, pa);
        // std::cout<<"check"<<all_kaon<<std::endl;
        all_kaon += pi_fromkaondecay;
      }
      return all_kaon;
    }

    ROOT::Math::IBaseFunctionMultiDim * 	Clone () const { return new  RFTimeFitFCN(*this); }


    double Evaluate_pos (double *x, double *p) {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = kaon_part(x[0], mu_piK, sigma_pi_pos,  A_K_pos, sigma_K_pos ) +
        pion_part(x[0], A_pi_pos, mu_piK, sigma_pi_pos);
      return y_function;
    }
    double Evaluate_neg (double *x, double *p) {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = kaon_part(x[0], mu_piK, sigma_pi,  A_K, sigma_K ) +
        pion_part(x[0], A_pi, mu_piK, sigma_pi);
      return y_function;
    }
    double Evaluate_pions_pos(double *x, double *p)const {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = pion_part(x[0], A_pi_pos, mu_piK, sigma_pi_pos);
      return y_function;
    }
    double Evaluate_kaons_pos(double *x, double *p) const {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = kaon_part(x[0], mu_piK, sigma_pi_pos,  A_K_pos, sigma_K_pos );
      return y_function;
    }
    double Evaluate_pions_neg(double *x, double *p) const {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = pion_part(x[0], A_pi, mu_piK, sigma_pi);
      return y_function;
    }
    double Evaluate_kaons_neg(double *x, double *p) const {
      double A_pi     = p[0];
      double mu_piK   = p[1];
      double sigma_pi = p[2];
      double A_K      = p[3];
      double sigma_K  = p[4];
      double A_pi_pos = p[5];
      double A_K_pos  = p[6];
      double sigma_pi_pos = p[2];//same as negative
      double sigma_K_pos  = p[4];//same as neg
      double y_function = kaon_part(x[0], mu_piK, sigma_pi,  A_K, sigma_K );
      return y_function;
    }

    void PrintPars() const {
      //std::cout <<  "A_pi_neg       = " << A_pi     << "\n";
      //std::cout <<  "mu_piK         = " << mu_piK   << "\n";
      //std::cout <<  "sigma_pi_neg   = " << sigma_pi << "\n";
      //std::cout <<  "A_K_neg        = " << A_K      << "\n";
      //std::cout <<  "sigma_K_neg    = " << sigma_K  << "\n";
      //std::cout <<  "A_pi_pos       = " << A_pi_pos     << "\n";
      ////std::cout <<  "mu_piK     = " << mu_piK   << "\n";
      ////std::cout <<  "sigma_pi_pos   = " << sigma_pi_pos << "\n";
      //std::cout <<  "A_K_pos        = " << A_K_pos      << "\n";
      //std::cout <<  "sigma_K_pos    = " << sigma_K_pos  << "\n";
    }

    // NDim is the number of fit parameters
    unsigned int 	NDim () const { return 7;}

    double DoEval (const double *x) const {
      double A_pi     = x[0];
      double mu_piK   = x[1];
      double sigma_pi = x[2];
      double A_K      = x[3];
      double sigma_K  = x[4];
      double A_pi_pos = x[5];
      double A_K_pos  = x[6];
      //double mu_piK   = x[1];//same as negative
      double sigma_pi_pos = x[2];//same as negative
      double sigma_K_pos  = x[4];//same as neg

      //std::cout<<"A_pi "<<A_pi<<" mu_pi "<<mu_piK<<" sigma_pi "<<sigma_pi<<" A_K "<<A_K<<" sigma_K "<<sigma_K<<" A_pi_pos "<<A_pi_pos<<" A_K_pos "<<A_K_pos<<std::endl; 

      double chi2 = 0; 

      int n_bins  = h_positive->GetNbinsX();
      for(int i_bin = 1; i_bin <= n_bins; i_bin ++ ){
        double x_bin = h_positive->GetBinCenter(i_bin);
        if(x_bin>3 || x_bin<0.5) continue;
        double y_bin = h_positive->GetBinContent(i_bin);
        double dy_bin = h_positive->GetBinError(i_bin);
        if(dy_bin<1e-3) continue;
        double y_function = kaon_part(x_bin, mu_piK, sigma_pi_pos,  A_K_pos, sigma_K_pos ) +
          pion_part(x_bin, A_pi_pos, mu_piK, sigma_pi_pos);
        double chi = (y_bin - y_function)/dy_bin;
        chi2 += chi*chi;
        //std::cout<<i_bin<<" check "<<chi<<std::endl;
        //std::cout<<"y bin "<<y_bin<<" y_function "<<y_function<<" dy "<<dy_bin<<std::endl;
      }
      n_bins  = h_negative->GetNbinsX();
      for(int i_bin = 1; i_bin <= n_bins; i_bin ++ ){
        double x_bin = h_negative->GetBinCenter(i_bin);
        if(x_bin>3 || x_bin<0.5) continue;
        double y_bin = h_negative->GetBinContent(i_bin);
        double dy_bin = h_negative->GetBinError(i_bin);
        if(dy_bin<1e-3) continue;
        double y_function = kaon_part(x_bin, mu_piK, sigma_pi,  A_K, sigma_K ) +
          pion_part(x_bin, A_pi, mu_piK, sigma_pi);
        double chi = (y_bin - y_function)/(dy_bin);
        chi2 += chi*chi;
        //std::cout<<i_bin<<" 2 check "<<chi<<std::endl;
      }
      //std::cout << " chi2 = " << chi2 << "\n";
      return chi2;
    }
};

void Kaon_decay_rftime(double rftime_rightcut = -1 ) {
  if (rftime_rightcut == -1) {
    std::cout << "Enter a rftime_rightcut (-1 to exit):";
    std::cin >> rftime_rightcut;
    if (rftime_right <= 0)
      return;
  }


}
