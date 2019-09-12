#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

void tfile_test(){
TH1* h1 = new TH1F("1","11",100,0.0,4);
h1->FillRandom("gaus",1000);
TH1* h2 = new TH1F("f","ff",100,-3,3);

for(int i = 0;i<10000;i++){
  h2->Fill(h1->GetRandom());
}

std::string filename = "something.root";
TFile* root = new TFile(filename.c_str());
h1->Write();
h2->Write();
root->Close();
TCanvas *c = new TCanvas();
c1->Divide(1,2);
c1->cd(1);
h1->Draw();
c1->cd(2);
h2->Draw();
}
