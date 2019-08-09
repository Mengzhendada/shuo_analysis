#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
void readroot1(){

  TFile *f = new TFile("HMS_DC_driftimes.root");
  f->ls();
  TH1F* h1 = new TH1F("h1","h1", 100, 0, 4); 
  h1 = (TH1F*)f->Get("1u1_time");
  TCanvas* c = new TCanvas;
  h1->Draw();
  c->SaveAs("test.pdf");
}
