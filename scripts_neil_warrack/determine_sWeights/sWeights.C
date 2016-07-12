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
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExponential.h"
#include "RooPolynomial.h"

using namespace std ;
using namespace RooFit ;


// Script opens a previously made ROOT file which contains a dataset type object (which is a reduced version 
// of the full data, reduced but applying specific cuts in specfic variables) and calculates 'sWeights'.
// sWeights are then saved to a 'dataset' .root file AND/OR to a TTree type ROOT file, along with their
// original info.


void sWeigher() {

  //TFile *fullFile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/turbo_2015_data.root") ; // Opens full data.
  TFile *fullFile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/run-II-data/turbo_2015_data.root") ; 

  TTree* mytree = (TTree*) fullFile->Get("Lambda_cToKppiTuple/DecayTree;1") ;  

  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/turbo_M_TAU_cut01.root") ; // opens 'reduced' data.
  TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/turbo_M_TAU_cut01.root") ;  


  // Define dataset
  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;


  // Define TAU variable, get limits.
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.00025 ,0.002 ,"ns") ; 
  double highestTAU;
  double lowestTAU;
  ds->RooAbsData::getRange(Lambda_cplus_TAU, lowestTAU, highestTAU);

  // Define Mass variable, get limits.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356, "GeV") ; 
  double highestM;
  double lowestM;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowestM, highestM) ;
  
  // Define IPCHI2 variable
  //RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100 ,100) ; 


  // Build combined double Gaussian PDF; "gaussComb". 
  double mass_peak = 2286 ;  // initialize gaussian mean value for fit for Lambda_cplus

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
  RooRealVar nSignal("nSignal","nSignal", 200000, 0, 409570); // For CHI2 < 3 cut, the max yield is 147000
  RooRealVar nBkg("nBkg","nBkg", 200000, 0, 409570);          // For CHI2 < 3 cut, the max yield is 147000
  
  //RooAddPdf model("model","model",RooArgList(gauss, pol0),RooArgList(nSignal, pol0_yield));
  //RooAddPdf model("model","model",RooArgList(gauss, expo_bkg),RooArgList(nSignal, nBkg));
  RooAddPdf model("model","model",RooArgList(gaussComb, expo_bkg),RooArgList(nSignal, nBkg));


  // Fit model
  model.fitTo(*ds, Extended()) ;


  // Set model fit variables to constants (NOT COEFFs!) ; this is important for the sPlot function below.
  gausMean1.setConstant() ;
  gausMean2.setConstant() ;
  sigma1.setConstant() ;
  sigma2.setConstant() ;
  nFrac.setConstant() ;
  expoPar.setConstant() ;
  
  //RooMsgService::instance().setSilentMode(true);

  // Create and record sWeights
  RooStats::SPlot* sDataM = new RooStats::SPlot("sData", "An SPlot", *ds, &model, RooArgList(nSignal, nBkg)) ;

 /*
//To Check...
  for (Int_t i=90; i < 100; i++){
    cout << "nSignal: "<<sDataM->GetSWeight(i,"nSignal")
	 << ", nBkg: "<<sDataM->GetSWeight(i,"nBkg")
	 << ", Total Weight: "<<sDataM->GetSumOfEventSWeight(i) // Should be total = 1 (always!)
	 << endl;
  }
  */

  /*
  // Save dataset with sWeights
   TFile *dsWithWeights = TFile::Open("DataSet_Lambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3_withWeights.root","RECREATE") ;
  ds->Write() ;
  dsWithWeights->Close() ;
  cout<<endl<<"  Dataset type file (datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3_withWeights.root) created in current directory..."<<endl ;
  */

  // Save OUTPUT TTree with sWeights
  //TFile treefile("sTree.root", "recreate") ;
  //TFile treefile("~/Documents/uni/LHCb_CharmSummerProj/learning_root/sWeightsTree_M_TAU_cut01.root", "recreate") ;
  TFile treefile("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/sWeightsTree_M_TAU_cut01.root", "recreate") ;
  RooTreeDataStore sTree("sTree", "sTree", *ds->get(0), *ds->store()) ;
  TTree& mystree = sTree.tree() ;
  mystree.SetBranchStatus("Lambda_cplus_M", 0) ; // you don't want to save the mass variables again.
  mystree.Write() ;
  treefile.Close() ;
  cout<<endl<<"TTree file created..."<<endl ;

  // Check the values of the yields and the 'yields' from the sWeight are the same
  cout<<endl<<"Yield of signal is: "<<nSignal.getVal()<<"; from sWeights it is: "
      <<sDataM->GetYieldFromSWeight("nSignal")<<endl ;
  cout<<"Yield of background is: "<<nBkg.getVal()<<"; from sWeights it is: "
      <<sDataM->GetYieldFromSWeight("nBkg")<<endl ;
  cout<<"^ These should match..."<<endl<<endl ;
  cout<<"nSignal + nBkg = "<<nSignal.getVal()<<" + "<<nBkg.getVal()<<" = "<<nSignal.getVal()+nBkg.getVal()<<endl;
  cout<<"^ This should match the number of events (below): "<<endl ;
  ds->Print() ;

  cout<<"========== FIT INFO =========="<<endl ;
  cout<<"  Double Gaussian fit parameters to full data (signal + background):"<<endl;
  cout<<gausMean1<<endl ;
  cout<<gausMean2<<endl ;
  cout<<sigma1<<endl ;
  cout<<sigma2<<endl ;
  cout<<nFrac<<endl ;
  cout<<"  Exponential fit parameters to full data (signal + background):"<<endl;
  cout<<expoPar<<endl<<endl ;


  cout<<"highestM = "<<highestM<<endl;
  cout<<"lowestM = "<<lowestM<<endl;
  cout<<"highestTAU = "<<highestTAU<<endl;
  cout<<"lowestTAU = "<<lowestTAU<<endl;
  // VISUAL CHECK: check the fit is correct, plot the fitted model on the data
  /*
  // Plot =================================================
  RooPlot *fullDataFit = Lambda_cplus_M.frame(Title("SignalAndBkg"));
   ds->plotOn(fullDataFit, Name("data"), MarkerColor(kBlack)) ;
  ds->statOn(fullDataFit, Layout(0.65,0.88,0.2), What("N")) ; //NB Layout(xmin,xmax,ymax)
  model.plotOn(fullDataFit, Name("Model"), DrawOption("L")) ;
  model.plotOn(fullDataFit, Components(expo_bkg), LineStyle(kDashed)) ;
  model.paramOn(fullDataFit,Layout(0.19, 0.45, 0.88)) ; //was 0.4
  fullDataFit->getAttText()->SetTextSize(0.022) ;

  // Draw =================================================
  TCanvas *c102 = new TCanvas("c102","",600,900) ;
  //formatCanvas4(c102) ;

  gStyle->SetOptStat("") ;
  
  c102->cd() ;
  fullDataFit->Draw() ;
  c102->Update() ;  
  c102->Print("SignalAndBkg01.pdf");
  // ======================================================
  */
}

