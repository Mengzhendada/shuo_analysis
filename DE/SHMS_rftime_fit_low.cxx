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
const double shms_length = 20.1;

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
  double gaus_shape = params[2] * exp(-0.5 * pow((x[0] - kaon_nodecay_peak) / params[4],
                                                 2)); ///(params[2] *sqrt(2*M_PI));
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
      pa[0] * exp(-0.5 * pow((x[0] - pi_fromkaondecay_peak) / pa[2], 2)); ///(pa[2] *sqrt(2*M_PI));
  // std::cout<<"check pion "<<pi_fromkaondecay_peak<<" gaus "<<gaus_shape<<std::endl;
  return gaus_shape;
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

  RFTimeFitFCN(TH1D* p, TH1D* n,double mom) :h_positive(p), h_negative(n), SHMS_momentum(mom) { }

  double pion_part(double x, double A_pi, double mu_piK, double sigma_pi) const {
    double gaus_shape = A_pi * exp(-0.5 * pow((x - mu_piK) / sigma_pi, 2)); ///(params[2] *sqrt(2*M_PI));
    return gaus_shape;
  }
  double kaon_part(double x, double mu_piK,double sigma_pi, double A_K, double sigma_K ) const {
    double params[5] = {mu_piK, sigma_pi/*not used*/, A_K, SHMS_momentum, sigma_K};
    double xx = x;
    //double nodecay_kaon = gaus_fun_kaonNodecay( &xx, params); 
    double pa[5];
    double all_kaon = 0;//nodecay_kaon;
    for( const auto& [meter,prob] : kaon_decay_prob ) {
      pa[0] = A_K * prob / (2 * 0.8235); // the amplitude of the pion(from kaon decay) peak portion of nodecay kaons.
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

    double chi2 = 0; 

    int n_bins  = h_positive->GetNbinsX();
    for(int i_bin = 1; i_bin <= n_bins; i_bin ++ ){
      double x_bin = h_positive->GetBinCenter(i_bin);
      double y_bin = h_positive->GetBinContent(i_bin);
      double dy_bin = h_positive->GetBinError(i_bin);
      double y_function = kaon_part(x_bin, mu_piK, sigma_pi_pos,  A_K_pos, sigma_K_pos ) +
                          pion_part(x_bin, A_pi_pos, mu_piK, sigma_pi_pos);
      double chi = (y_bin - y_function)/dy_bin;
      chi2 += chi*chi;
    }
    n_bins  = h_negative->GetNbinsX();
    for(int i_bin = 1; i_bin <= n_bins; i_bin ++ ){
      double x_bin = h_negative->GetBinCenter(i_bin);
      double y_bin = h_negative->GetBinContent(i_bin);
      double dy_bin = h_negative->GetBinError(i_bin);
      double y_function = kaon_part(x_bin, mu_piK, sigma_pi,  A_K, sigma_K ) +
                          pion_part(x_bin, A_pi, mu_piK, sigma_pi);
      double chi = (y_bin - y_function)/dy_bin;
      chi2 += chi*chi;
    }
    //std::cout << " chi2 = " << chi2 << "\n";
    return chi2;
  }
};

void SHMS_rftime_fit_high(int RunGroup = 0, int n_aero=4 ) {
  if (RunGroup == 0) {
    std::cout << "Enter a RunGroup (-1 to exit):";
    std::cin >> RunGroup;
    if (RunGroup <= 0)
      return;
  }
  std::unique_ptr<TFile> fin(TFile::Open(string("results/rf_hitsograms" + to_string(RunGroup) +
                                                "_aero" + std::string(n_aero) + ".root")
                                             .c_str(),
                                         "READ"));

  json j_DE;
  {
    std::ifstream ifs("db2/PID_test.json");
    ifs >> j_DE;
  }
  std::cout << "derp\n";

  json j_rungroup;
  {
    std::ifstream ifs("db2/ratio_run_group_updated.json");
    ifs >> j_rungroup;
  }
  double shms_p_central = j_rungroup[(std::to_string(RunGroup)).c_str()]["shms_p"].get<double>();



  std::vector<int> delta_cut = j_DE["SHMS"]["delta_cuts_forrf"].get<std::vector<int>>();
  double delta_lowend = delta_cut[0];
  for (int i_dpcut = 0;i_dpcut< std::size(delta_cut)-1 ;i_dpcut++) {

    std::string dp_cut = "P.gtr.dp>" + std::to_string(delta_cut[i_dpcut]) + " && P.gtr.dp < " + std::to_string(delta_cut[i_dpcut+1]);
    std::cout << "delta cut is " << dp_cut << std::endl;
    double shms_p = shms_p_central * (100 + (delta_cut[i_dpcut]+delta_cut[i_dpcut+1])/2) / 100;

    auto h_rf_pos_piall = fin->Get<TH1D>(std::string("rftime_pos_" + std::to_string(RunGroup) + "_" + std::to_string(i_dpcut)).c_str());
    auto h_rf_neg_piall = fin->Get<TH1D>(std::string("rftime_neg_" + std::to_string(RunGroup) + "_" + std::to_string(i_dpcut)).c_str());

    ROOT::Math::Minimizer* minimum =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(100000);  // for GSL
    minimum->SetTolerance(0.001);
    minimum->SetPrintLevel(2);

    RFTimeFitFCN f_pi(h_rf_pos_piall,h_rf_neg_piall,shms_p);
    minimum->SetFunction(f_pi);

    minimum->SetVariable(       0,"A_{#pi,neg} ", 100.0,  1 );
    minimum->SetLimitedVariable(1,"#mu         ", 1.0, 0.01, 0.8, 1.2);
    minimum->SetLimitedVariable(2,"#sigma_{#pi}", 0.2, 0.001,0.18,0.22);
    minimum->SetVariable(       3,"A_{K,neg}   ", 2.0,  1.0 );
    minimum->SetFixedVariable(  4,"#sigma_{K}  ", 0.25);
    minimum->SetVariable(       5,"A_{#pi,pos} ", 100.0,  1 );
    minimum->SetVariable(       6,"A_{K,pos}   ", 2.0,  1.0 );
    minimum->Minimize();

    const double *min_pi_pars = minimum->X();

    TCanvas* c = new TCanvas("c1","c1",1200,900);
    c->Divide(1,3);
    c->cd(1);
    TF1 * fpos = new TF1("rftime_pos",&f_pi,&RFTimeFitFCN::Evaluate_pos,0.5,3.0,7,"RFTimeFitFCN","Evaluate_pos");   // create TF1 class.
    fpos->SetParameters(min_pi_pars);
    TF1 * fpos_pp = new TF1("rftime_pp",&f_pi,&RFTimeFitFCN::Evaluate_pions_pos,0.5,3.0,7,"RFTimeFitFCN","Evaluate_pions_pos");   // create TF1 class.
    fpos_pp->SetParameters(min_pi_pars);
    fpos_pp->SetLineColor(4);
    TF1 * fpos_kp = new TF1("rftime_kp",&f_pi,&RFTimeFitFCN::Evaluate_kaons_pos,0.5,3.0,7,"RFTimeFitFCN","Evaluate_kaons_pos");   // create TF1 class.
    fpos_kp->SetParameters(min_pi_pars);
    fpos_kp->SetLineColor(2);
    auto h1 = h_rf_pos_piall->DrawCopy();
    h1->GetYaxis()->SetRangeUser(0,h1->GetMaximum()*1.1);
    fpos->DrawCopy("lsame");
    fpos_pp->DrawCopy("lsame");
    fpos_kp->DrawCopy("lsame");

    TLatex lt;
    lt.DrawLatexNDC(0.8,0.7, std::string("P_{SHMS} = " +  std::to_string(shms_p) + " GeV").c_str());

    c->cd(2);
    TF1 * fneg = new TF1("rftime_neg",&f_pi,&RFTimeFitFCN::Evaluate_neg,0.5,3.0,7,"RFTimeFitFCN","Evaluate_neg");   // create TF1 class.
    fneg->SetParameters(min_pi_pars);
    h_rf_neg_piall->Draw();
    h_rf_neg_piall->GetYaxis()->SetRangeUser(0,h_rf_neg_piall->GetMaximum()*1.1);
    fneg->DrawCopy("lsame");

    c->cd(3);
    h_rf_pos_piall->Divide(h_rf_neg_piall);
    h_rf_pos_piall->Draw();
    h_rf_pos_piall->GetYaxis()->SetRangeUser(0,3.5);

    c->SaveAs(std::string("results/pid/rftime_" + std::to_string(RunGroup) + "_" + std::to_string(i_dpcut)+".png").c_str());

  }


}
