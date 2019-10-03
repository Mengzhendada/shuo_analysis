#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>

void tfile_test(){
TH1* h1 = new TH1D("1","11",100,0,1);
h1->FillRandom("gaus",1000);
TH1* h2 = new TH1D("f","ff",100,-3,3);
h2->SetCanExtend(TH1::kAllAxes);
double a[20] = {1,2,3,4.7,5.5,6.5,7,8,9,5.7,5,5.3,5.5,5,5.8,4.6,4.8,4.9,6,6};
for(int i = 0;i<20;i++){
  h2->Fill(a[i]);
}


std::string filename = "something.root";
TFile* root = new TFile(filename.c_str(),"RECREATE");
h1->Write();
h2->Write();
root->Close();
TCanvas *c1 = new TCanvas();
c1->Divide(1,2);
c1->cd(1);
h1->Draw();
c1->cd(2);
h2->Draw();
}
