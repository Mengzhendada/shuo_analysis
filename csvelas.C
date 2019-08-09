#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
R__LOAD_LIBRARY(libMathMore.so)
R__LOAD_LIBRARY(libGenVector.so)

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include "TObject.h"
#include <iostream>
#include <fstream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

bool root_file_exists(std::string rootfile) {
  bool found_good_file = false;
  if (!gSystem->AccessPathName(rootfile.c_str())) {
    TFile file(rootfile.c_str());
    if (file.IsZombie()) {
      std::cout << rootfile << " is a zombie.\n";
      std::cout
          << " Did your replay finish?  Check that the it is done before running this script.\n";
      return false;
      // return;
    } else {
      std::cout << " using : " << rootfile << "\n";
      return true;
    }
  }
  return false;
}

//main function
void csvelas(){
  double Eb = 10.5965;
  double Mp = 0.93825;
  using nlohmann::json;
  json j;
  {
    std::ifstream json_input_file("db2/heep40separate.json");
    json_input_file >> j;
  }


  TH2F *h_Ep_diff = new TH2F(
      "Ee_diff", "difference of hms momentum and hms calculated momentum", 10,
      6008, 6018, 100, -0.5, 0.7);


  TGraphErrors *G_yield_data = new TGraphErrors();
  G_yield_data->SetTitle("data");

  TGraphErrors *G_yield_sim = new TGraphErrors();
  G_yield_sim->SetTitle("sim");

  TGraphErrors *G_yield_data_momentum = new TGraphErrors();
  G_yield_data_momentum->SetTitle("data");
  TGraphErrors *G_yield_sim_momentum = new TGraphErrors();
  G_yield_sim_momentum->SetTitle("sim");

  TGraphErrors *G_yield_data_angle = new TGraphErrors();
  G_yield_data_angle->SetTitle("data");
  TGraphErrors *G_yield_sim_angle = new TGraphErrors();
  G_yield_sim_angle->SetTitle("sim");

  //loop over different runs. 
  int i = 0; 
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    
    auto runjs = it.value();


    double hms_angle    = runjs["htheta_lab"].get<double>() * 3.1415926 / 180.;
    double hms_momentum = runjs["hpcentral"].get<double>();
    double charge       = runjs["charge"].get<double>();
    double normfac      = runjs["normfac"].get<double>();
    std::cout<<"hms angle "<<hms_angle<<std::endl;
    double Ep = Eb/(1.0+((2.0*Eb/Mp)*(sin(hms_angle/2.0)*sin(hms_angle/2.0))));
    double Ep_diff = Ep-hms_momentum;
    int runnumber = std::stoi(it.key());

    h_Ep_diff->Fill(runnumber,Ep_diff);
    //find correct root file with correct runnumber. 
    fmt::print("{} data run check {}", runnumber, hms_momentum);

    std::string rootfile_sim =
        fmt::format("/u/home/shuojia/simc_gfortran/worksim/csvelas{}.root", runnumber);
    std::string rootfile_data = fmt::format("/group/c-csv/shuo/hallc_replay_sidis_fall18/ROOTfiles/coin_replay_production_{}_-1.root", runnumber);

    std::cout << rootfile_data << std::endl;
    std::cout << rootfile_sim << std::endl;

    ROOT::RDataFrame ddata("T",  rootfile_data.c_str());
    ROOT::RDataFrame dsim("h666", rootfile_sim.c_str());

    if ( !root_file_exists(rootfile_data)){
      std::cout <<  rootfile_data << " not found !\n";
    }
    if ( !root_file_exists(rootfile_sim)){
      std::cout <<  rootfile_sim << " not found !\n";
    }

    auto yield_data_before = *ddata.Count()/charge;

    std::cout<<yield_data_before<<std::endl;



    //building dataframe for data
    auto d1data = ddata.Filter([](double etottracknorm){return etottracknorm>0.8;},{"H.cal.etottracknorm"})
    .Filter([](double emiss){return  emiss<0.6 && emiss>0.4;},{"P.kin.secondary.emiss"})
    .Filter([](double hindex){return hindex>-2;},{"H.gtr.index"})
    .Filter([](double pindex){return pindex>-2;},{"P.gtr.index"})
    .Define("Ep_cal",[&Eb,&Mp](double th){return  Eb/(1.0+((2.0*Eb/Mp)*(sin(th*3.1415926/360))*(sin(th* 3.1415926/360))));},{"H.kin.primary.scat_ang_deg"})
    .Define("Ep_diff",[](double Ee,double hmom){return Ee-hmom;},{"Ep_cal","H.gtr.p"});

    auto h_W_data = d1data.Histo1D({"W data","W d",100,0.4,1.6},"H.kin.primary.W");
    h_W_data->Fit("gaus","O","",0.4,1.6);

    TF1 *h_W_data_fit= h_W_data->GetFunction("gaus");
    double h_W_data_mean = 0.0;
    double h_W_data_sigm = 0.0;
    if( h_W_data_fit ){
      h_W_data_fit->SetLineColor(3);
      h_W_data_mean = h_W_data_fit->GetParameter(1);
      h_W_data_sigm = h_W_data_fit->GetParameter(2);
    }
   /* double Wmin = h_W_data_mean-1.5*h_W_data_sigm;
    double Wmax = h_W_data_mean+1.5*h_W_data_sigm;
    TLine *L_Wmin = new TLine(Wmin,350,Wmin,0);
    L_Wmin->SetLineColor(3);
    TLine *L_Wmax = new TLine(Wmax,350,Wmax,0);
    L_Wmax->SetLineColor(3);
*/
    
    //double dpmax_data = *d1data.Max("H.gtr.dp");
    //double dpmin_data = *d1data.Min("H.gtr.dp");
    //double dpcut_data = 0.75*dpmax_data+0.25*dpmin_data;

    // d0 is data with W Cut
    auto d0data = d1data;//.Filter([&Wmax,&Wmin](double w){return w<Wmax &&w>Wmin;},{"H.kin.primary.W"});
    //.Filter([dpcut_data](double dp){return dp<dpcut_data;},{"H.gtr.dp"});
    auto yield_data_after = *d0data.Count()/(charge);
    std::cout<<yield_data_after<<std::endl;

    G_yield_data->SetPoint(i,runnumber+0.1,yield_data_after);
    G_yield_data->SetPointError(i,0,std::sqrt(*d0data.Count())/charge);

    G_yield_data_momentum->SetPoint(i,hms_momentum+0.01,yield_data_after);
    G_yield_data_momentum->SetPointError(i,0,std::sqrt(*d0data.Count())/charge);

    G_yield_data_angle->SetPoint(i,hms_angle*180/3.14+0.05,yield_data_after);
    G_yield_data_angle->SetPointError(i,0,std::sqrt(*d0data.Count())/charge);

    //building dataframe for simulation
    double sim_charge = 100.0; //mC
    double dcharge = charge;//data[5][i]; //mC read from /REPORT_OUTPUT/COIN/PRODUCTION/replay_coin_production_num_-1.root
    //std::cout<<"dcharge check"<<dcharge<<std::endl;
    double shmsTE = runjs["shmsTE"].get<double>();//data[7][i];//as last
    //std::cout<<"shmsTE check"<<shmsTE<<std::endl;
    double hmsTE =  runjs["hmsTE"].get<double>();//data[8][i];//as last
    //std::cout<<"hmsTE check"<<hmsTE<<std::endl;
    double coinlive =  runjs["ComputerLT"].get<double>();// data[6][i];//as last
    //std::cout<<"coinlive check "<<coinlive<<std::endl;
    //double normfac = data[9][i];//from /simc/outfiles/csvnum.hist
    auto nentries = dsim.Count();
    double wfac = (normfac / *nentries) *(dcharge /sim_charge);
    //double wfac = dcharge/charge;
    std::cout<<"wfac "<<wfac<<std::endl;
    auto weightcalculate = [wfac,coinlive,shmsTE,hmsTE](float weight){return wfac*coinlive*shmsTE*hmsTE*weight;};
    auto d1sim = dsim.Define("weight",weightcalculate,{"Weight"})
    .Filter([](float emiss){return emiss<0.1 && emiss>-0.1;},{"Em"});
    
    //float dpmax = *d1sim.Max("hsdelta");
    //float dpmin = *d1sim.Min("hsdelta");
    //float dpcut = 0.75*dpmax+0.25*dpmin;
    auto d0sim = d1sim;//.Filter(        [Wmax, Wmin](float w) { return w < Wmax && w > Wmin; }, {"W"});
    //.Filter([dpcut](float dp){return dp<dpcut;},{"hsdelta"});
    //.Filter([](float EOVerP){return EOverP>0.8},{""});

    std::cout<<*d1sim.Count()<<std::endl;

    auto Q2_sim_sum = d0sim.Histo1D({"Q2 sim","Q2 sim",100,7,12},"Q2","weight")->Integral();
    std::cout<<"runnumber "<<runnumber<<" sim "<<Q2_sim_sum<<" data "<<*d0data.Count()<<std::endl;
    auto yield_sim = Q2_sim_sum/charge;
    //auto yield_beforeweight = *d1sim.Count()/data[5][i];

    G_yield_sim->SetPoint(i,runnumber,yield_sim);
    G_yield_sim->SetPointError(i,0,std::sqrt(Q2_sim_sum)/charge);

    G_yield_sim_momentum->SetPoint(i,hms_momentum,yield_sim);
    G_yield_sim_momentum->SetPointError(i,0,std::sqrt(Q2_sim_sum)/charge);

    G_yield_sim_angle->SetPoint(i,hms_angle*180/3.14,yield_sim);
    G_yield_sim_angle->SetPointError(i,0,std::sqrt(Q2_sim_sum)/charge);
    //auto c = new TCanvas;
    //h_yield_sim->Draw();
    //h_Q2_sim->DrawCopy();
    //std::cout<<h_Q2_sim<<std::endl;

    //building histo for data kin
    auto h_Q2_data = d0data.Histo1D({"Q2 data","Q2 data",100,7,12},"H.kin.primary.Q2");

    /*
       auto h_W_data = d0data.Histo1D({"W data","W d",100,0.4,1.6},"H.kin.primary.W");
       h_W_data->Fit("gaus","O","",0.4,1.6);
       TF1 *h_W_data_fit= h_W_data->GetFunction("gaus");
       h_W_data_fit->SetLineColor(3);
       double h_W_data_mean = h_W_data_fit->GetParameter(1);
       double h_W_data_sigm = h_W_data_fit->GetParameter(2);
       std::cout<<"data mean "<<h_W_data_mean<<std::endl;
       std::cout<<"data sigma"<<h_W_data_sigm<<std::endl;
       */
    auto h_Emiss_data = d0data.Histo1D({"Missing Energy data","Missing E data",100,-0.2,0.2},"P.kin.secondary.emiss");
    auto h_Pmiss_data = d0data.Histo1D({"Missing Momentum data","Missing P data",100,-0.2,0.2},"P.kin.secondary.pmiss");
    auto h_om_data = d0data.Histo1D({"Omega data","Om d",100,4.0,6.4},"H.kin.primary.omega");

    //building histo for simulation
    auto h_Q2_sim = d1sim.Histo1D({"Q2 simulation","Q2 sim",100,7,12},"Q2","weight");
    auto h_W_sim = d1sim.Histo1D({"W simulation","W sim",100,0.4,1.6},"W","weight");
    h_W_sim->Fit("gaus","O","",0.4,1.6);
    TF1 *h_W_sim_fit = h_W_sim->GetFunction("gaus");
    double h_W_sim_mean = h_W_sim_fit->GetParameter(1);
    double h_W_sim_sigm = h_W_sim_fit->GetParameter(2);
    std::cout<<"sim mean "<<h_W_sim_mean<< std::endl;
    std::cout<<"sim sigma "<<h_W_sim_sigm<< std::endl;
    //auto h_Q2_sim_without = d1sim_withoutweight.Histo1D({"Q2 simulation","Q2 sim",100,7,12},"Q2");
    //auto h_Q2_sim_without = d1sim_withoutweight.Histo1D({"Q2 simulation","Q2 sim",100,7,12},"Q2");
    //auto h_weight = d1sim_withoutweight.Histo1D({"Weight","Weight",100,0,0.00001},"Weight");
    auto h_Emiss_sim = d1sim.Histo1D({"Missing energy sim","Missing energy sim",100,-0.2,0.2},"Em","weight");
    auto h_Pmiss_sim = d1sim.Histo1D({"Missing momentum sim","Missing P sim",100,-0.2,0.2},"Pm","weight");
    auto h_om_sim = d1sim.Histo1D({"Omega","om d",100,4.0,6.4},"nu","weight");

    // -------------------------------------------------------
    //
    THStack*     hs = nullptr;
    TCanvas*     c  = nullptr;
    TMultiGraph* mg = nullptr;

    c = new TCanvas("c_kine","ckin");
    c->Divide(2, 3);
    c->cd(1);
    hs = new THStack("hs0","Q2; Q2 ");
    hs->Add((TH1D*)h_Q2_data->Clone());
    hs->Add((TH1D*)h_Q2_sim->Clone() );
    hs->Draw("nostack");

    c->cd(2);
    hs = new THStack("hsW","W; W [GeV] ");
    hs->Add((TH1D*)h_W_data->Clone() );
    hs->Add((TH1D*)h_W_sim->Clone()  );
    hs->Draw("nostack");
    //L_Wmin->Draw("same");
    //L_Wmax->Draw("same");

    c->cd(3);
    h_Emiss_data->DrawCopy();
    h_Emiss_sim ->DrawCopy("same");

    c->cd(4);
    h_Pmiss_data->DrawCopy();
    h_Pmiss_sim->DrawCopy("same");

    c->cd(5);
    h_om_data->DrawCopy();
    h_om_sim->DrawCopy("same");

    c->SaveAs(fmt::format("csvresult/c_kin_{}.pdf",runnumber).c_str());

    /*
    //target
    auto h_htarx_data = d0data.Histo1D({"hms x tar","hms x tar",100,-5,5},"H.gtr.x");
    auto h_htary_data = d0data.Histo1D({"hms y tar","hms y tar",100,-5,5},"H.gtr.y");
    auto h_ptarx_data = d0data.Histo1D({"shms x tar","shms x tar",100,-5,5},"P.gtr.x");
    auto h_ptary_data = d0data.Histo1D({"shms y tar","shms y tar",100,-5,5},"P.gtr.y");

    TCanvas *c_tarpos = new TCanvas;
    c_tarpos->Divide(2,2);
    c_tarpos->cd(1);
    h_htarx_data->DrawCopy();
    c_tarpos->cd(2);
    h_htary_data->DrawCopy();
    c_tarpos->cd(3);
    h_ptarx_data->DrawCopy();
    c_tarpos->cd(4);
    h_ptary_data->DrawCopy();

    c_tarpos->SaveAs((std::string("csvresult/c_tarpos_")+std::to_string((int)data[0][i])+".pdf").c_str());

*/


    //good track
    auto h_htarxpd   = d0data.Histo1D({"hms xp_tar", "htarxpd", 100, -0.1, 0.1}, "H.gtr.th");
    auto h_htarypd   = d0data.Histo1D({"hms yp_tar", "htarypd", 100, -0.1, 0.1}, "H.gtr.ph");
    auto h_hdeltad   = d0data.Histo1D({"hms dp", "hms dp", 100, -14, 14.0}, "H.gtr.dp");
    auto h_hdeltad_2 = d1data.Histo1D(
        {"hms dp", "hms dp without W cut", 100, -14, 14.0}, "H.gtr.dp");
    //auto h_ptarxpd = d0data.Histo1D({"shms xp_tar","ptarxpd",100,-0.1,0.1},"P.gtr.th");
    //auto h_ptarypd = d0data.Histo1D({"shms yp_tar","ptarxpd",100,-0.1,0.1},"P.gtr.ph");
    //auto h_pdeltad = d0data.Histo1D({"shms dp","ptarxpd",100,-14,4},"P.gtr.dp");

    auto h_htarxps = d0sim.Histo1D({"hms xp_tar","htarxpd",100,-0.1,0.1},"hsxptar","weight");
    auto h_htaryps = d0sim.Histo1D({"hms yp_tar","htarypd",100,-0.1,0.1},"hsyptar","weight");
    auto h_hdeltas = d0sim.Histo1D({"hms dp","hdeltad",100,-14,14.0},"hsdelta","weight");
    auto h_hdeltas_2 = d1sim.Histo1D({"hdeltad","hdeltad",100,-14,14.0},"hsdelta","weight");
    //auto h_ptarxps = d0sim.Histo1D({"shms xp_tar","ptarxpd",100,-0.1,0.1},"ssxptar","weight");
    //auto h_ptaryps = d0sim.Histo1D({"shms yp_tar","ptarxpd",100,-0.1,0.1},"ssyptar","weight");
    //auto h_pdeltas = d0sim.Histo1D({"shms dp","ptarxpd",100,-14,4},"ssdelta","weight");

    TCanvas *c_gtr = new TCanvas; 
    c_gtr->Divide(1,3);
    c_gtr->cd(1);
    h_htarxpd->DrawCopy();
    h_htarxps->DrawCopy("same");
    c_gtr->cd(2);
    h_htarypd->DrawCopy();
    h_htaryps->DrawCopy("same");
    c_gtr->cd(3);
    h_hdeltad->DrawCopy();
    h_hdeltas->DrawCopy("same");

    h_hdeltad_2->SetLineColor(2);
    h_hdeltad_2->DrawCopy("same");

    h_hdeltas_2->SetLineColor(8);
    h_hdeltas_2->DrawCopy("same");
    //c_gtr->cd(4);
    //h_ptarxpd->DrawCopy();
    //h_ptarxps->DrawCopy("same");
    //c_gtr->cd(5);
    //h_ptarxpd->DrawCopy();
    //h_ptarxps->DrawCopy("same");
    //c_gtr->cd(6);
    //h_ptarypd->DrawCopy();
    //h_ptaryps->DrawCopy("same");
    c_gtr->SaveAs(fmt::format("csvresult/c_gtr_{}.pdf",runnumber).c_str());
    /*
    //focal plane data
    auto h_sx_fp_data = d0data.Histo1D({"shms fp x data","shms fp x data",100,-30,15},"P.dc.x_fp");
    auto h_sxp_fp_data = d0data.Histo1D({"shms fp xp data","shms fp xp data",100,-0.08,0.06},"P.dc.xp_fp");
    auto h_sy_fp_data = d0data.Histo1D({"shms fp y data","shms fp y data",100,-20,10},"P.dc.y_fp");
    auto h_syp_fp_data = d0data.Histo1D({"shms fp yp data","shms fp yp data",100,-0.04,0.03},"P.dc.yp_fp");
    auto h_hx_fp_data = d0data.Histo1D({"hms fp x data","hms fp x data",100,-30,30},"H.dc.x_fp");
    auto h_hxp_fp_data = d0data.Histo1D({"hms fp xp data","hms fp xp data",100,-0.04,0.04},"H.dc.xp_fp");
    auto h_hy_fp_data = d0data.Histo1D({"hms fp y data","hms fp y data",100,-20,30},"H.dc.y_fp");
    auto h_hyp_fp_data = d0data.Histo1D({"hms fp yp data","hms fp yp data",100,-0.03,0.03},"H.dc.yp_fp");



    auto h_sx_fp_sim = d1sim.Histo1D({"shms fp x sim","shms fp x sim",100,-30,15},"ssxfp","weight");
    auto h_sxp_fp_sim = d1sim.Histo1D({"shms fp xp sim","shms fp xp sim",100,-0.08,0.06},"ssxpfp","weight");
    auto h_sy_fp_sim = d1sim.Histo1D({"shms fp y sim","shms fp y sim",100,-20,10},"ssyfp","weight");
    auto h_syp_fp_sim = d1sim.Histo1D({"shms fp yp sim","shms fp yp sim",100,-0.04,0.03},"ssypfp","weight");
    auto h_hx_fp_sim = d1sim.Histo1D({"hms fp x sim","hms fp x sim",100,-30,30},"hsxfp","weight");
    auto h_hxp_fp_sim = d1sim.Histo1D({"hms fp xp sim","hms fp xp sim",100,-0.04,0.04},"hsxpfp","weight");
    auto h_hy_fp_sim = d1sim.Histo1D({"hms fp y sim","hms fp y sim",100,-20,30},"hsyfp","weight");
    auto h_hyp_fp_sim = d1sim.Histo1D({"hms fp yp sim","hms fp yp sim",100,-0.03,0.03},"hsypfp","weight");

    TCanvas *c_fp = new TCanvas;
    c_fp->Divide(2,4);
    c_fp->cd(1);
    h_sx_fp_data->DrawCopy();
    h_sx_fp_sim->DrawCopy("same");
    c_fp->cd(2);
    h_sxp_fp_data->DrawCopy();
    h_sxp_fp_sim->DrawCopy("same");
    c_fp->cd(3);
    h_sy_fp_data->DrawCopy();
    h_sy_fp_sim->DrawCopy("same");
    c_fp->cd(4);
    h_syp_fp_data->DrawCopy();
    h_syp_fp_sim->DrawCopy("same");
    c_fp->cd(5);
    h_hx_fp_data->DrawCopy();
    h_hx_fp_sim->DrawCopy("same");
    c_fp->cd(6);
    h_hxp_fp_data->DrawCopy();
    h_hxp_fp_sim->DrawCopy("same");
    c_fp->cd(7);
    h_hy_fp_data->DrawCopy();
    h_hy_fp_sim->DrawCopy("same");
    c_fp->cd(8);
    h_hyp_fp_data->DrawCopy();
    h_hyp_fp_sim->DrawCopy("same");
    c_fp->SaveAs((std::string("csvresult/c_fp_")+std::to_string((int)data[0][i])+".pdf").c_str());


    //
    auto h_Wvsxfp_data = d0data.Histo2D({"W vs. x focal plane","W:x",100,-2,2,100,0.4,1.6},"H.dc.x_fp","H.kin.primary.W");
    auto h_Wvsptardp_data = d0data.Histo2D({"W vs. ptardp","W:ptardp",100,-0.2,0.2,100,0.4,1.6},"P.gtr.dp","H.kin.primary.W");
    auto h_Wvsptarth_data = d0data.Histo2D({"W vs. ptarth","W:ptarth",100,-0.2,0.2,100,0.4,1.6},"P.gtr.th","H.kin.primary.W");
    auto h_Wvsptarph_data = d0data.Histo2D({"W vs. ptarph","W:ptarph",100,-0.2,0.2,100,0.4,1.6},"P.gtr.ph","H.kin.primary.W");
    auto h_Ep_diff = d0data.Histo1D({"hms momentum difference","hms mom diff",100,-1,1},"Ep_diff");
    auto h_Wvshmsmomdiff_data = d0data.Histo2D({"W vs. hms momentum difference","W vs. hms_mom_diff",100,-1,1,100,0.4,1.6},"H.kin.primary.W","Ep_diff");
    auto diff_mom_vs_theta = d0data.Histo2D({"difference momentum vs. hms theta","Mom_diff vs. hms th",100,-0.5,0.7,100,17,22},"Ep_diff","H.kin.primary.scat_ang_deg");
    auto h_fpxvsmomdiff_data = d0data.Histo2D({"fp x vs. momentum difference","fp x vs. mom_diff",100,-1,1,100,-30,30},"Ep_diff","H.dc.x_fp");
    auto h_fpxpvsmomdiff_data = d0data.Histo2D({"fp xp vs. momentum difference","fp xp vs. mom_diff",100,-1,1,100,-0.06,0.06},"Ep_diff","H.dc.xp_fp");
    auto h_fpyvsmomdiff_data = d0data.Histo2D({"fp y vs. momentum difference","fp y vs. mom_diff",100,-1,1,100,20,40},"Ep_diff","H.dc.y_fp");
    auto h_fpypvsmomdiff_data = d0data.Histo2D({"fp yp vs. momentum difference","fp yp vs. mom_diff",100,-1,1,100,-0.02,0.03},"Ep_diff","H.dc.yp_fp");

    TCanvas *c_hist = new TCanvas;
    c_hist->Divide(2,6);
    c_hist->cd(1);
    h_Wvsxfp_data->DrawCopy();
    c_hist->cd(2);
    h_Wvsptardp_data->DrawCopy();
    c_hist->cd(3);
    h_Wvsptarth_data->DrawCopy();
    c_hist->cd(4);
    h_Wvsptarph_data->DrawCopy();
    c_hist->cd(5);
    h_Ep_diff->DrawCopy();
    c_hist->cd(6);
    h_Wvshmsmomdiff_data->DrawCopy();
    c_hist->cd(7);
    diff_mom_vs_theta->DrawCopy();
    c_hist->cd(8);
    h_fpxvsmomdiff_data->DrawCopy();
    c_hist->cd(9);
    h_fpxpvsmomdiff_data->DrawCopy();
    c_hist->cd(10);
    h_fpyvsmomdiff_data->DrawCopy();
    c_hist->cd(11);
    h_fpypvsmomdiff_data->DrawCopy();
    c_hist->SaveAs((std::string("csvresult/c_hist_")+std::to_string((int)data[0][i])+".pdf").c_str());

    */
    i++;
  }

  TCanvas *c_yield = new TCanvas;
  /*c_yield->Divide(2,2);
    c_yield->cd(1);
    h_yield_beforecut->SetMarkerStyle(10);
    h_yield_beforecut->Draw();
    c_yield->cd(2);
    h_yield_aftercut->SetMarkerStyle(20);
    h_yield_aftercut->Draw();
    c_yield->cd(3);
    */

  /*h_yield_beforecut->SetMarkerStyle(22);
    h_yield_beforecut->Draw("e1");
    h_yield_aftercut->SetMarkerStyle(20);
    h_yield_aftercut->Draw("same e1");
    h_yield_sim->SetMarkerStyle(21);
    h_yield_sim->Draw("same e1");
    gPad->BuildLegend();
    c_yield->SaveAs("csvresult/c_yield.pdf");
    */
  auto mg = new TMultiGraph();
  G_yield_data->SetMarkerStyle(20);
  G_yield_data->SetLineColor(4);
  G_yield_sim->SetMarkerStyle(21);
  G_yield_sim->SetLineColor(2);
  mg->Add(G_yield_data);
  mg->Add(G_yield_sim);

  /*G_yield_data->Draw("ap");//a: axie, p:point
    G_yield_data->SetMarkerStyle(20);
    G_yield_sim->Draw("ap");
    G_yield_sim->SetMarkerStyle(21);
    */
  mg->Draw("ap");
  c_yield->BuildLegend(0.7,0.7,0.9,0.9);
  c_yield->SaveAs("csvresult/c_yield.pdf");
  TCanvas *c_yield_momentum = new TCanvas;
  c_yield_momentum->SetTitle("yield vs. momentum");
  auto mg_momentum = new TMultiGraph();
  G_yield_data_momentum->SetMarkerStyle(20);     
  G_yield_data_momentum->SetLineColor(4);        
  G_yield_sim_momentum->SetMarkerStyle(21);
  G_yield_sim_momentum->SetLineColor(2);
  mg_momentum->Add(G_yield_data_momentum);
  mg_momentum->Add(G_yield_sim_momentum);

  /*G_yield_data->Draw("ap");//a: axie, p
    G_yield_data->SetMarkerStyle(20);
    G_yield_sim->Draw("ap");
    G_yield_sim->SetMarkerStyle(21);
    */
  mg_momentum->Draw("ap");
  c_yield_momentum->BuildLegend(0,0.7,0.2,0.9);
  c_yield_momentum->SaveAs("csvresult/c_yield_momentum.pdf");
  TCanvas *c_yield_angle = new TCanvas;
  c_yield_angle->SetTitle("yield vs. angle");
  auto mg_angle = new TMultiGraph();
  G_yield_data_angle->SetMarkerStyle(20);     
  G_yield_data_angle->SetLineColor(4);        
  G_yield_sim_angle->SetMarkerStyle(21);
  G_yield_sim_angle->SetLineColor(2);
  mg_angle->Add(G_yield_data_angle);
  mg_angle->Add(G_yield_sim_angle);
  mg_angle->Draw("ap");
  c_yield_angle->BuildLegend(0.7,0.7,0.9,0.9);
  c_yield_angle->SaveAs("csvresult/c_yield_angle.pdf");
}
