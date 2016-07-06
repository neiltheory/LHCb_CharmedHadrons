#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExponential.h"
#include "RooPolynomial.h"
#include "RooAbsData.h"
#include "TLatex.h"
#include "TFile.h"
#include "TLegend.h"

using namespace RooFit ;

// Script reads info from ROOT files to screen


void read() {

  // Open File
  
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356.root");
 
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3.root"); 

  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/DataSetLambda_TAUmin0002_max0022.root"); 

  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/DataSetfromGed_fit_data_wSWeight.root"); 

  // Define dataset
  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;

  // Define TAU variable, get limits.
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002 ,0.0022 ,"ns") ;  //real range of interest is [0.00025, 0.002], this is defined later.
  double highestTAU;
  double lowestTAU;
  ds->getRange(Lambda_cplus_TAU, lowestTAU, highestTAU);

  // Define Mass variable, get limits.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356, "GeV") ; 
  double highestM;
  double lowestM;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowestM, highestM) ;
  
  // Define IPCHI2 variable
  RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100 ,100) ; 
  double highestIPCHI2_OWNPV;
  double lowestIPCHI2_OWNPV;
  ds->RooAbsData::getRange(Lambda_cplus_IPCHI2_OWNPV, lowestIPCHI2_OWNPV, highestIPCHI2_OWNPV) ;

  //Lambda_cplus_TAU.setRange("R1",0.00018, 0.0012);




  // Print to Screen
  cout<<endl<<endl<<"************info************"<<endl;

  cout<< "Lowest lifetime value in dataset = "<<lowestTAU<<endl;
  cout<< "Highest lifetime value in dataset = "<<highestTAU<<endl;  
  cout<< "Lowest mass value in dataset (should be: 2216)= "<<lowestM<<endl;
  cout<< "Highest mass value in dataset (should be: 2356)= "<<highestM<<endl; 
  cout<< "Lowest IPCHI2_OWNPV value in dataset = "<<lowestIPCHI2_OWNPV<<endl;
  cout<< "Highest IPCHI2_OWNPV value in dataset = "<<highestIPCHI2_OWNPV<<endl;
 
  cout<<"number of events: "<<endl ; 
  ds->Print();  //number of events in dataset    
  //rmodel->Print(); //results  
  //cout<<"Chi squared ="<<chi2<<endl ;
  
}
