#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TTree.h"
#include "RooAbsData.h"
#include "TFile.h"


using namespace RooFit ;


// Script will write file with type 'DataSet' from a larger ROOT file of type TTree.  

void mkfunc() {

  // Open file
  //TFile *bigFile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/data/turbo_2015_data.root");
  TFile *bigFile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/fit_data_wSWeights.root");  
  //TFile *bigFile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/turbo_2015_data.root") ;


  // Fill TTree
  //TTree* mytree = (TTree*) bigFile->Get("Lambda_cToKppiTuple/DecayTree;1");
  TTree* mytree = (TTree*) bigFile->Get("DecayTree");

  // Define variables to take to new file
    RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",-0.4,121.);
  //RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002,0.0022);
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216,2356) ;
  //RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100,1000) ;

  // make DataSet
  //RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_TAU, Lambda_cplus_M, Lambda_cplus_IPCHI2_OWNPV),Import(*mytree),Cut( "Lambda_cplus_IPCHI2_OWNPV<3")) ;
  //RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_TAU, Lambda_cplus_M, Lambda_cplus_IPCHI2_OWNPV),Import(*mytree)) ;
  RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_TAU, Lambda_cplus_M),Import(*mytree)) ;


  /*
  // Save TTree with sWeights
  TFile treefile("Tree_NoIPCHI2Cut.root", "recreate") ;
  RooTreeDataStore sTree("sTree", "sTree", *ds->get(0), *ds->store()) ;
  TTree& mystree = sTree.tree() ;
  mystree.SetBranchStatus("Lambda_cplus_M", 0) ; // you don't want to save the mass variables again.
  mystree.Write() ;
  treefile.Close() ;
  cout<<endl<<"  TTree file created in current directory..."<<endl ;
  */



  // Choose destination, and name appropriately.
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/RooFit/firstTry/datafileLambda_TAUminNeg002_max005.root","recreate");
  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/DataSetfromGed_fit_data_wSWeight.root","recreate");
  //TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/fitcode_neil_warrack/outputfile.root") ;
  //TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/fitcode_neil_warrack/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3.root","recreate");
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356.root","recreate");
  ds.Write() ;
  datafile->Close(); 
 
   cout<<"ROOT file created"<<endl ;
}
