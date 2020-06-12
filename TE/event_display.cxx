#include <fmt/core.h>
#include <fmt/ostream.h>
R__LOAD_LIBRARY(libfmt.so)

#include "nlohmann/json.hpp"
  using json = nlohmann::json;
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

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include <fstream>
#include <vector>
#include <string>

void event_display(int RunNumber){
    if(RunNumber == 0){
      std::cout<<"Enter RunNumber Number(-1 to exit) ";
      std::cin>>RunNumber;
      if(RunNumber < 0)
        return;
    }
    std::string rootfile_name = "ROOTfiles/coin_replay_production_"+std::to_string(RunNumber)+"_"+std::to_string(RunNumber)+".root";
   TFile *f = new TFile(rootfile_name.c_str(),"READ");
   TTree *tdata = (TTree*) f->Get("T");
   //TTree *tscal = (TTree*) f->Get("TSP");

   double x_fp;
   double y_fp;
   double 1x_pos;
   double 1y_pos;
   double 2x_pos;
   double 2y_pos;
   tdata->SetBranchAddress("P.dc.x_fp",&x_fp);
   tdata->SetBranchAddress("P.dc.y_fp",&y_fp);
   tdata->SetBranchAddress("P.hod.1x.Clus.Pos",&1x_pos);//position
   tdata->SetBranchAddress("P.hod.1y.Clus.Pos",&1y_pos);
   tdata->SetBranchAddress("P.hod.2x.Clus.Pos",&2x_pos);
   tdata->SetBranchAddress("P.hod.2y.Clus.Pos",&2y_pos);
  // TH2D* h_fp = new TH2D("fp","fp",100,-30,30,100,-30,30);
  // TH2D* h_hod_1xy = new TH2D("hod1","hod1",100,-50,50,100,-50,50);
   TGraphErrors* g_fp = new TGraphErrors();
   TGraphErrors* g_1xy = new TGraphErrors();
   TGraphErrors* g_2xy = new TGraphErrors();
   int tdata_entries = tdata->GetEntries();

   int n = 1;//continue or not
   int i = 1;//ith event
   while(n == 1){
     std::cout<<"This is "<<i<<"th event"<<std::endl;
     tdata->GetEntry(i);
     g_fp->SetPoint(1,x_fp,y_fp);
     g_1xy->SetPoint(1,1x_pos,1y_pos);
     g_2xy->SetPoint(1,2x_pos,2y_pos);

     TCanvas* c_position = new TCanvas();
     c_position->Divide(1,3);
     c_position->cd(1);
     g_fp->SetMarkerStyle(8);
     g_fp->Draw("ap");
     c_position->cd(2);
     g_1xy->SetMarkerStyle(8);
     g_1xy->Draw("ap");
     c_position->cd(3);
     g_2xy->SetMarkerStyle(8);
     g_2xy->Draw("ap");
    
     std::cout<<"Do you want to continue? 1 to continue, no other options"<<std::endl;
     std::cin>>n;
     i++;
   }
   
}
