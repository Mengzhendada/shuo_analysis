#include "ROOT/RDataFrame.hxx"
#include <vector>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TProfile.h"
#include "TF1.h"
#include "TH1.h"
#include "TLatex.h"
#include "TPaveText.h"
bool reject;
void profile_practice(){
  std::string rootfile_name = "ROOTfiles/coin_replay_production_7593_7593.root";
  ROOT::RDataFrame d("T",rootfile_name.c_str());
  double SHMS_P = 3.15;
  auto shms_p_calculate = [SHMS_P](double shms_dp){return SHMS_P*(1+shms_dp/100);};
  auto d_SHMS = d.Filter("-10 < P.gtr.dp && P.gtr.dp< 22")
                    .Filter("-10 < H.gtr.dp && H.gtr.dp < 10")
                    .Filter("H.cer.npeSum > 10")
                    .Filter("H.cal.etottracknorm > 0.85")
                    .Filter("fEvtHdr.fEvtType == 4")
                    .Define("shms_p",shms_p_calculate,{"P.gtr.dp"});

  auto h_coin_time = d_SHMS.Histo1D({"coin_time","coin_time;cointime;counts",800,0,100},"CTime.ePositronCoinTime_ROC2");
  int coin_peak_bin = h_coin_time->GetMaximumBin();
  double coin_peak_center = h_coin_time->GetBinCenter(coin_peak_bin);
  auto d_SHMS_coin = d_SHMS.Filter(
      [=](double coin_time){return std::abs(coin_time - coin_peak_center) < 3.0;},{"CTime.ePositronCoinTime_ROC2"}
    )
                            .Filter("P.cal.etottracknorm > 0.015 && P.cal.etottracknorm < 0.85");
    
    //TF1 *f1 = new TF1("f1","[0]*((1-(x*x+0.139*0.139)/(x*x*1.00137*1.00137)))",0.9*SHMS_P,0.95*SHMS_P);

  auto h_npe_shmsp = d_SHMS_coin.Histo2D({"","",100,0.9*3.15,1.22*3.15,100,0,30},"shms_p","P.hgcer.npeSum")->ProfileX("h1",1,-1,"d");
     auto fline = [SHMS_P](double* x,double *par){
      //double shms_p = 3.15;
      if(reject && x[0] > 0.93*SHMS_P && x[0] < 1.05*SHMS_P){
        TF1::RejectPoint();
        return 0.0;
      }
      return par[0]*((1-par[1]*(x[0]*x[0]+0.139*0.139)/(x[0]*x[0]*1.00137*1.00137)));
    };
    TF1 *f1 = new TF1("f1",fline,0.9*SHMS_P,1.22*SHMS_P,2);
    //f1->SetParameter(0,SHMS_P);
    reject = true;
    h_npe_shmsp->Fit(f1,"0");
    reject = false;
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);
    std::cout<<" parameters "<<p0<<" "<<p1<<std::endl;
    std::string parameter_1 = "p0 = "+std::to_string(p0);
    std::string parameter_2 = "p1 = "+std::to_string(p1);

    //c->cd();
    //return c;
    TCanvas *c = new TCanvas();
    //c->Divide(1,2);
    //c->cd(1);
    TF1 *fleft = new TF1("fleft",fline,0.9*3.15,0.93*3.15,2);
    fleft->SetParameters(f1->GetParameters());
    h_npe_shmsp->GetListOfFunctions()->Add(fleft);
    gROOT->GetListOfFunctions()->Remove(fleft);
    TF1 *fright = new TF1("fright",fline,1.05*3.15,1.22*3.15,2);
    fright->SetParameters(f1->GetParameters());
    h_npe_shmsp->GetListOfFunctions()->Add(fright);
    gROOT->GetListOfFunctions()->Remove(fright);
    TPaveText *pt  = new  TPaveText(0.6,0.15,0.9,0.25,"brNDC");
    pt->AddText(parameter_1.c_str());
    pt->AddText(parameter_2.c_str());
    h_npe_shmsp->SetBit(TH1::kNoStats);
    h_npe_shmsp->Draw();
    c->cd();
    pt->Draw();
    c->SaveAs("results/pid/shms_2d.pdf");
}
