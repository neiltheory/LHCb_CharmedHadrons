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


// Script opens a previously made .root file which contains a dataset type object which 
// holds all events in a preselected and appropriate range for the TAU lifetime variable 
// of the Lambda_cplus particel (OR the Xi_c).


void sWeigher() {

  TFile *fullFile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/turbo_2015_data.root") ;

  TTree* mytree = (TTree*) fullFile->Get("Lambda_cToKppiTuple/DecayTree;1") ;  

  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3.root") ; 

  // Define dataset
  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;

  // Define TAU variable, get limits.
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002 ,0.0022 ,"ns") ;  //real range of interest is [0.00025, 0.002], this is defined later.
  double highestTAU;
  double lowestTAU;
  ds->RooAbsData::getRange(Lambda_cplus_TAU, lowestTAU, highestTAU);

  // Define Mass variable, get limits.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356, "GeV") ; 
  double highestM;
  double lowestM;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowestM, highestM) ;
  
  // Define IPCHI2 variable
  RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100 ,100) ; 

  // Build combined double Gaussian PDF; "gaussComb".
 
  double mass_peak = 2286 ;  // initialize gaussian mean value for fit

  RooRealVar gausMean1("gausMean1", "gausMean1",mass_peak, lowestM, highestM, "GeV") ;
  RooRealVar sigma1("sigma1","sigma1", 6, 0, 50) ;
  RooGaussian gauss1("gauss1","gauss1",Lambda_cplus_M, gausMean1, sigma1) ;

  RooRealVar gausMean2("gausMean2", "gausMean2",mass_peak, lowestM, highestM, "GeV") ;
  RooRealVar sigma2("sigma2","sigma2", 4, 0, 100) ;
  RooGaussian gauss2("gauss2","gauss2",Lambda_cplus_M, gausMean2, sigma2) ;

  RooRealVar nFrac("nFrac", "nFrac", 0.5, 0.0, 1.) ;
  RooAddPdf gaussComb("gaussComb","gaussComb", RooArgList(gauss1, gauss2), RooArgList(nFrac)) ;

  //Lambda_cplus_TAU.setRange("R1",0.00018, 0.0012);
   
  // Build exponential PDF; "expo_bkg"
  RooRealVar expoPar("expoPar","expoPar", -0.0001, -1., 0.);
  RooExponential expo_bkg("expo_bkg", "expo_bkg", Lambda_cplus_M, expoPar);

  // Build model PDF
  RooRealVar nSignal("nSignal","nSignal", 200000, 0, 409570);
  RooRealVar nBkg("nBkg","nBkg", 200000, 0, 409570);
  
  //RooAddPdf model("model","model",RooArgList(gauss, pol0),RooArgList(nSignal, pol0_yield));
  //RooAddPdf model("model","model",RooArgList(gauss, expo_bkg),RooArgList(nSignal, nBkg));
  RooAddPdf model("model","model",RooArgList(gaussComb, expo_bkg),RooArgList(nSignal, nBkg));

  // Fit model
  model.fitTo(*ds, Extended()) ;

  // Set model fit variables to constants (NOT COEFFs!)
  gausMean1.setConstant() ;
  gausMean2.setConstant() ;
  sigma1.setConstant() ;
  sigma2.setConstant() ;
  nFrac.setConstant() ;
  expoPar.setConstant() ;
  
  // Create and record sWeights
  RooStats::SPlot* sDataM = new RooStats::SPlot("sData", "An SPlot", *ds, &model, RooArgList(nSignal, nBkg)) ;

  std::cout << std::endl <<  "Yield of signal is " << nSignal.getVal() << ".  From sWeights it is " << sDataM->GetYieldFromSWeight("nSignal") << std::endl;
  std::cout << "Yield of background is " << nBkg.getVal() << ".  From sWeights it is " << sDataM->GetYieldFromSWeight("nBkg") << std::endl << std::endl;


  // Code for saving dataset with weights
  //RooRealVar w("w","w",-5000,5000);
  //RooDataSet Wgt("Wgt","Wgt", mytree, RooArgSet(Lambda_cplus_M), WeightVar(w));
  TFile f("sWeightsDatafile.root","RECREATE") ;
  sDataM->Write() ;
  f.Close() ;
  cout<<endl<<"sWeightsDatafile.root created in current directory..."<<endl ;
 

  //TFile treefile("sTree.root", "recreate") ;
  //RooTreeDataStore sTree("sTree", "sTree", *ds->get(0), *ds->store()) ;
  //TTree* tree = sTree.tree() ;
  //tree->SetBranchStatus("Lambda_cplus_M", 0) ; // you don't want to save the mass variables again.
  //tree->Write() ;
  //treefile.Close() ;

}
