#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"

#include "TSystem.h"

// =================================================================================
// Constants
// =================================================================================
constexpr const double M_P  = .938272;
constexpr const double M_P2 = M_P * M_P;
constexpr const double M_J  = 3.096916;
constexpr const double M_J2 = M_J * M_J;
constexpr const double M_e  = .000511;
constexpr const double M_mu = .1056583745;
// =================================================================================
// Cuts -- (Significantly) looser than the standard analysis cuts
// =================================================================================
std::string shms_good_track = "P.gtr.dp > -15 && P.gtr.dp < 25";
std::string hms_good_track  = "H.gtr.dp > -11 && H.gtr.dp < 11";
std::string shms_good_pid   = "P.cal.etottracknorm > 0.0 && P.cal.etottracknorm < 2.";
std::string hms_good_pid    = "H.cal.etottracknorm > 0.0 && H.cal.etottracknorm < 2.";

// =================================================================================
// Definitions
// =================================================================================
using Pvec3D = ROOT::Math::XYZVector;
using Pvec4D = ROOT::Math::PxPyPzMVector;

// =================================================================================
// J/psi reconstruction --> apply central momentum corrections from Mark
// =================================================================================
auto p_track = [](double px, double py, double pz) { return Pvec4D{px, py, pz, M_e}; };
auto p_jpsi  = [](const Pvec4D& e1, const Pvec4D& e2) { return e1 + e2; };
auto E_gamma = [](const Pvec4D& jpsi) {
  double res = (jpsi.M2() - 2. * jpsi.E() * M_P) /
               (2. * (jpsi.E() - M_P - jpsi.P() * cos(jpsi.Theta())));
  return res;
};
auto abst = [](const double Egamma, Pvec4D& jpsi) {
  Pvec4D beam{0, 0, Egamma, 0};
  return fabs((beam - jpsi).M2());
};

bool root_file_exists(std::string rootfile) {
  bool found_good_file = false;
  if (!gSystem->AccessPathName(rootfile.c_str())) {
    TFile file(rootfile.c_str());
    if (file.IsZombie()) {
      std::cout << rootfile << " is a zombie.\n";
      std::cout << " Did your replay finish?  Check that the it is done before "
                   "running this script.\n";
      return false;
      // return;
    } else {
      std::cout << " using : " << rootfile << "\n";
      return true;
    }
  }
  return false;
}

void skim(int RunNumber = 7146, int nevents = -1) {
  // ===============================================================================================
  // Initialization
  // ===============================================================================================
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("db2/run_list_coin.json");
    try {
      json_input_file >> j;
    } catch (json::parse_error) {
      std::cerr << "error: json file, db2/run_list.json, is incomplete or has "
                   "broken syntax.\n";
      std::quick_exit(-127);
    }
  }

  auto runnum_str = std::to_string(RunNumber);
  if (j.find(runnum_str) == j.end()) {
    std::cout << "Run " << RunNumber << " not found in db2/run_list_coin.json\n";
    std::cout << "Check that run number and replay exists. \n";
    std::cout << "If problem persists please contact Sylvester (217-848-0565)\n";
  }

  std::string coda_type = "COIN";

  bool found_good_file = false;

  std::string rootfile = fmt::format("/lcrc/project/jlab/data/hallc/jpsi-007/replay/full/"
                                     "coin_replay_production_{}_{}.root",
                                     RunNumber, nevents);
  std::cout << "attempting to load file: " << rootfile << std::endl;
  found_good_file = root_file_exists(rootfile.c_str());
  if (!found_good_file) {
    rootfile =
        fmt::format("full_online/coin_replay_production_{}_{}.root", RunNumber, nevents);
    found_good_file = root_file_exists(rootfile.c_str());
  }
  if (!found_good_file) {
    rootfile        = fmt::format("ROOTfiles_volatile/coin_replay_production_{}_{}.root",
                           RunNumber, nevents);
    found_good_file = root_file_exists(rootfile.c_str());
  }
  if (!found_good_file) {
    rootfile = fmt::format("ROOTfiles_jpsi/coin_replay_production_{}_{}.root", RunNumber,
                           nevents);
    found_good_file = root_file_exists(rootfile.c_str());
  }
  if (!found_good_file) {
    rootfile =
        fmt::format("ROOTfiles/coin_replay_production_{}_{}.root", RunNumber, nevents);
    found_good_file = root_file_exists(rootfile.c_str());
  }
  if (!found_good_file) {
    std::cout << " Error: suitable root file not found\n";
    return;
  }
  // ===============================================================================================
  // Dataframe
  // ===============================================================================================

  ROOT::EnableImplicitMT(12);

  //---------------------------------------------------------------------------
  // Detector tree
  ROOT::RDataFrame d("T", rootfile);

  // SHMS Scaler tree
  ROOT::RDataFrame d_sh("TSP", rootfile);

  auto d_coin = d.Filter("fEvtHdr.fEvtType == 4");

  // Good track cuts
  auto d_tracked =
      d_coin.Filter(fmt::format("{} && {}", hms_good_track, shms_good_track))
          .Define("p_shms", p_track, {"P.gtr.px", "P.gtr.py", "P.gtr.pz"})
          .Define("p_hms", p_track, {"H.gtr.px", "H.gtr.py", "H.gtr.pz"})
          .Define("p_jpsi", p_jpsi, {"p_shms", "p_hms"})
          .Define("M_jpsi", "p_jpsi.M()")
          .Define("E_gamma", E_gamma, {"p_jpsi"})
          .Define("abst", abst, {"E_gamma", "p_jpsi"})
          .Define("coin_time", "CTime.ePositronCoinTime_ROC2")
          .Define("run", runnum_str)
          .Define("P_ngcer_npe", [](ROOT::RVec<double> array) { return array; },
                  {"P.ngcer.npe"});

  // PID cuts
  std::string skim_cut = fmt::format("({} && {})", shms_good_pid, hms_good_pid);
  auto        d_skim   = d_tracked.Filter(skim_cut);

  // scalers
  auto   total_charge      = d_sh.Max("P.BCM4B.scalerChargeCut");
  double good_total_charge = *total_charge / 1000.0;  // mC

  // ===============================================================================================
  // Create output
  // ===============================================================================================
  // std::string ofname = "results/skim/skim_coinel_" +
  // std::to_string(RunNumber) + ".root";
  std::string ofname =
      fmt::format("/lcrc/globalscratch/{}/skim_{}.root", getenv("USER"), RunNumber);

  d_skim.Snapshot("Tjpsi", ofname,
                  {"run",
                   "P.gtr.dp",
                   "P.gtr.th",
                   "P.gtr.ph",
                   "P.gtr.y",
                   "P.gtr.x",
                   "P.dc.x_fp",
                   "P.dc.xp_fp",
                   "P.dc.y_fp",
                   "P.dc.yp_fp",
                   "P.cal.eprtrack",
                   "P.cal.eprtracknorm",
                   "P.cal.etot",
                   "P.cal.etotnorm",
                   "P.cal.etottracknorm",
                   "P.cal.etrack",
                   "P.cal.etracknorm",
                   "P.ngcer.npeSum",
                   "P_ngcer_npe",
                   "P.hod.goodscinhit",
                   "P.hod.betanotrack",
                   "P.dc.ntrack",
                   "H.gtr.dp",
                   "H.gtr.th",
                   "H.gtr.ph",
                   "H.gtr.y",
                   "H.gtr.x",
                   "H.cal.eprtrack",
                   "H.cal.eprtracknorm",
                   "H.cal.etot",
                   "H.cal.etotnorm",
                   "H.cal.etottracknorm",
                   "H.cal.etrack",
                   "H.cal.etracknorm",
                   "H.cer.npeSum",
                   "H.hod.goodscinhit",
                   "H.hod.betanotrack",
                   "H.dc.ntrack",
                   "coin_time",
                   "p_shms",
                   "p_hms",
                   "p_jpsi",
                   "M_jpsi",
                   "E_gamma",
                   "abst"});
  // Sylvester: not in replay currently "P.ngcer.npe",
  // these 4 values somehow make (or made...) the skim routine hang on bebop...
  //  --> removed them for now
  // "H.cal.1pr.eplane",
  // "H.cal.2ta.eplane",
  // "H.cal.3ta.eplane",
  // "H.cal.4ta.eplane",

  // also write total charge
  TFile ofile(ofname.c_str(), "update");
  TH1D* ocharge = new TH1D("total_good_charge", "total_good_charge", 1, 0, 1);
  ocharge->SetBinContent(1, good_total_charge);
  ocharge->Write();
  ofile.Close();
}
