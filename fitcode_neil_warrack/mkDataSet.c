// From Gedeminas work
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TTree.h"
#include "RooAbsData.h"
#include "TFile.h"


using namespace RooFit ;


void mkfunc() {
  // Write .root dataset file from TTree 

  TFile *file_01 = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/data/turbo_2015_data.root");
  TTree* mytree = (TTree*) file_01->Get("Lambda_cToKppiTuple/DecayTree;1");
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",-0.002,0.005);
  RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_TAU),Import(*mytree)) ;
  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/RooFit/firstTry/datafileLambda_TAUminNeg002_max005.root","recreate");
  ds.Write() ;
  datafile->Close(); 
 
   cout<<".root file created"<<endl ;
}

