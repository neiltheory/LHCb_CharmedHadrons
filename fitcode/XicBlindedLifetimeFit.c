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

void XicBlindedLifetimeFit()
{
TFile f("histofxi.root");
TH1F *h_yieldvstime = (TH1F*)f.Get("h_yieldvstime");

// Read the histo into a RooFit dataset. 
RooRealVar binnedTau("binnedTau", "Decay time", 0.2, 2.18) ;
RooDataHist binnedData("binnedData", "Signal yield vs decay time", RooArgSet(binnedTau), h_yieldvstime) ;

// Setup blinding of the decay rate - "someblindingstring" can be anything
// you like really, it's just used to generate a random offset in the decay
// rate between -0.5 and +0.5.
RooRealVar decayConstBlind("decayConstBlind", "Decay rate", -5., -10, 0.) ;
RooUnblindOffset decayConstUnblind("decayConstUnblind", "Unblind decay rate", "xiblindingstring", 0.5, decayConstBlind) ;

// Fit the data with an exponential. The PDF uses the unblind decay rate,
// but only the blinded decay rate will be printed. 
RooExponential decaytimePDF("decaytimePDF", "Decay time PDF", binnedTau, decayConstUnblind) ;
decaytimePDF.fitTo(binnedData, RooFit::SumW2Error(false)) ;

// print the blinded decay rate:
cout << "Decay rate: " << decayConstBlind.getVal() << " +/- " << decayConstBlind.getError() << endl ;

// Plot the data and fit as usual.
TCanvas canv; 
RooPlot* timePlot = binnedTau.frame() ;
binnedData.plotOn(timePlot) ;
decaytimePDF.plotOn(timePlot);
timePlot->Draw();
canv.SaveAs("XiDecayTimeFit.png") ;

}
