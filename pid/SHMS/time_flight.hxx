#include <cmath>

class time_flight{
  
double c = 299792458;
double period = 4.008;

  public:
    time_flight();
    ~time_flight();
    double t_e(double p);
    double t_pi(double p);
    double t_K(double p);
    double t_proton(double p);

    double Get_t_pi(double p){return t_pi(p)-t_e(p);}
    double Get_t_K(double p){return t_K(p)-t_e(p);}
    double Get_t_proton(double p){return t_proton(p)-t_e(p);}

    double t_pi_mod(double p){return std::fmod(Get_t_pi(p),period);}
    double t_K_mod(double p){return std::fmod(Get_t_K(p),period);}
    double t_proton_mod(double p){return std::fmod(Get_t_proton(p),period);}
};

double time_flight::t_e(double p){
  double m = 0.00051;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
};
double time_flight::t_pi(double p){
  double m = 0.139;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
};
double time_flight::t_K(double p){
  double m = 0.493;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
};
double time_flight::t_proton(double p){
  double m = 0.938;
  return (18.1*std::sqrt(p*p+m*m)*1e9)/(c*p);
};
