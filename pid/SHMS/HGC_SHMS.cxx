#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include <math.h>
#include <iostream>

void HGC_SHMS(){
  auto costh = [](double *x, double *par){
   return par[0]*std::sqrt(x[0]*x[0]+par[1]*par[1])/(x[0]*1.00137);
  };
  auto npe = [](double *x, double *par){
   //return par[0]-par[0]*(x[0]*x[0]+par[1]*par[1])/(par[1]*par[1]*1.0122*1.0122);
    return 1-par[0]*par[0]*(x[0]*x[0]+par[1]*par[1])/(x[0]*x[0]*1.00137*1.00137);
  };
  TF1 *f11 = new TF1("pi;cos(th);momentum",costh,0,10,2);
  f11->SetParameters(1,0.139);
  f11->GetXaxis()->SetTitle("momentum");
  f11->GetYaxis()->SetTitle("cos(th)");
  TCanvas *c_pion_p_cos = new TCanvas();
  f11->Draw();
  f11->SetTitle("pi");
  c_pion_p_cos->SaveAs("results/hgc_costh_pi_p.pdf");

  TF1 *f_e_1 = new TF1("e;cos(th);momentum",costh,0,10,2);
  f_e_1->SetParameters(1,0.00051);
  f_e_1->GetXaxis()->SetTitle("momentum");
  f_e_1->GetYaxis()->SetTitle("cos(th)");
  TCanvas *c_electron_p = new TCanvas();
  f_e_1->Draw();
  f_e_1->SetTitle("e");
  c_electron_p->SaveAs("results/hgc_costh_e_p.pdf");
  
  TF1 *f_K_1 = new TF1("e;cos(th);momentum",costh,0,10,2);
  f_K_1->SetParameters(1,0.493);
  f_K_1->GetXaxis()->SetTitle("momentum");
  f_K_1->GetYaxis()->SetTitle("cos(th)");
  TCanvas *c_K_p = new TCanvas();
  f_K_1->Draw();
  f_K_1->SetTitle("K");
  c_K_p->SaveAs("results/hgc_costh_K_p.pdf");
  TF1 *f_proton_1= new TF1("e;cos(th);momentum",costh,0,10,2);
  f_proton_1->SetParameters(1,0.938);
  f_proton_1->GetXaxis()->SetTitle("momentum");
  f_proton_1->GetYaxis()->SetTitle("cos(th)");
  TCanvas *c_proton_p = new TCanvas();
  f_proton_1->Draw();
  f_proton_1->SetTitle("proton");
  c_proton_p->SaveAs("results/hgc_costh_proton_p.pdf");
  
  TF1 *f_pi_2 = new TF1("pi npe",npe,0,10,2);
  f_pi_2->SetParameters(1,0.139);
  TF1 *f_K_2 = new TF1("K npe",npe,2.8,10,2);
  f_K_2->SetParameters(1,0.493);
  TF1 *f_proton_2 = new TF1("proton npe",npe,2.8,10,2);
  f_proton_2->SetParameters(1,0.938);
  TF1 *f_e_2 = new TF1("e npe",npe,0,10,2);
  f_e_2->SetParameters(1,0.00051);
  TCanvas *c_pion_thisp = new TCanvas();
  f_pi_2->Draw();
  f_pi_2->GetYaxis()->SetRangeUser(0,0.005);
  f_pi_2->GetYaxis()->SetTitle("Aero npe/N0");
  f_e_2->SetLineColor(kBlack);
  f_e_2->Draw("same");
  f_K_2->SetLineColor(kOrange);
  f_K_2->Draw("same");
  f_proton_2->SetLineColor(kBlue);
  f_proton_2->Draw("same");
  c_pion_thisp->BuildLegend(0.75,0.75,0.95,0.95);
  c_pion_thisp->SaveAs("results/hgc_npe_p.pdf");
}
