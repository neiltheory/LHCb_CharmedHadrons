#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooDataHist.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "RooPlot.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"
#include <stdio.h>
#include "TMath.h"

using namespace RooFit ;

// Script uses a previously made .root file containing a histogram with signale only exponential decay in TAU variable for Lambda_cplus hadron and makes an exponential fit to it, then plots and saves the data and fit.

void lifetimeFit()
{
  TFile f("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/scripts_neil_warrack/lifetime_removeBkg/histo_Lambda_cplus_TAU_lifetime_SigOnly.root") ;
  TH1F *h_signal = (TH1F*)f.Get("h_sig");

  // Read histogram into RooFit dataset
  RooRealVar tau("tau", "tau (ns)", 0.00025, 0.002) ;
  RooDataHist binnedData("binnedData", "binnedData", RooArgSet(tau), h_signal) ;
  // Build exponential PDF,
  RooRealVar expoParam("expoParam", "expoParam", -5000, -5000, 0.) ;
  //RooRealVar blindConst("blindConst", "blindConst", -50000., -10, 0.) ;
  //RooUnblindOffset decayConstUnblind("decayConstUnblind", "Unblind decay rate", "someblindingstring", 0.0005, blindConst) ;

  RooExponential lifetimePDF("lifetimePDF", "lifetimePDF", tau, expoParam) ; 
  //RooExponential lifetimePDF("lifetimePDF", "lifetimePDF", tau, decayConstUnblind) ;

  // Fit to data
  lifetimePDF.fitTo(binnedData, RooFit::SumW2Error(false)) ;

  // print the blinded decay rate:
  //cout << "Decay rate: " << blindConst.getVal() << " +/- " << blindConst.getError() << endl ;

  // Plot
  TCanvas c1;
  RooPlot* lifetimePlot = tau.frame() ;
  binnedData.plotOn(lifetimePlot) ;
  lifetimePDF.plotOn(lifetimePlot);
  lifetimePlot->Draw();
  lifetimePlot->GetYaxis()->SetTitleOffset(1.4);
  
  // Save file as .png
c1.SaveAs("histo_Lambda_cplus_TAU_lifetime_SigOnly_FITTED.png") ;

}



