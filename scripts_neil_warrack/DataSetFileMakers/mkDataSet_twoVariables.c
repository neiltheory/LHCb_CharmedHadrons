#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TTree.h"
#include "RooAbsData.h"
#include "TFile.h"


using namespace RooFit ;


// Script will write .root dataset (with two variables) file from TTree 

void mkfunc() {


  //TFile *file_01 = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/data/turbo_2015_data.root");
  TFile *file_01 = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/turbo_2015_data.root") ;

  TTree* mytree = (TTree*) file_01->Get("Lambda_cToKppiTuple/DecayTree;1");
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002,0.0022);
 
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216,2356) ;
  RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100,100) ;
  RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_TAU, Lambda_cplus_M, Lambda_cplus_IPCHI2_OWNPV),Import(*mytree),Cut( "Lambda_cplus_IPCHI2_OWNPV<3")) ;

  // Choose destination, and name appropriately.
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/RooFit/firstTry/datafileLambda_TAUminNeg002_max005.root","recreate");
  //TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/fitcode_neil_warrack/outputfile.root") ;
  TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/fitcode_neil_warrack/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3.root","recreate");
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356.root","recreate");
  ds.Write() ;
  datafile->Close(); 
 
   cout<<"ROOT file created"<<endl ;
}

