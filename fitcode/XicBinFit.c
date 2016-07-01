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

void XicBinFit()
{
// Load tree file and data set file for Lambda mass entries
TFile *myfile = TFile::Open("/nfs/lhcb/malexander01/charm/baryon-lifetimes-2015/data/run-II-data/turbo_2015_data_wAngles_wBDTGWeights.root");

TTree* mytree = (TTree*) myfile->Get("DecayTree");

RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2320,2520);
RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0,0.006);
RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100,100);
RooRealVar BDTG("BDTG","BDTG",-1,1);


RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_M,Lambda_cplus_IPCHI2_OWNPV,BDTG,Lambda_cplus_TAU),
	      Import (*mytree),Cut("(BDTG>0.091)&&(Lambda_cplus_IPCHI2_OWNPV<3)")); 

// List of parameters 

double mass_lower_bound = 2320;
double mass_upper_bound = 2520;
double mass_peak = 2467;


/// Builds Gaussian signal PDFs 
RooRealVar sigMean("sigMean","sigMean",mass_peak, mass_lower_bound, mass_upper_bound); 
RooRealVar sigWidth("sigWidth", "sigWidth",5,0,50); 
RooGaussian PDF_Mass_Sig("PDF_Mass_Sig", "gaussian PDF", Lambda_cplus_M, sigMean, sigWidth);

RooRealVar x1("x1", "x1", 0, -1., 1);
RooRealVar x2("x2", "x2", 0, -1., 1);
RooChebychev bkg("bkg","Background",Lambda_cplus_M,RooArgSet(x1,x2)) ;

// define event yields
RooRealVar nSig("nSig", "", -100,350000);
RooRealVar nBkg("nBkg", "", -100,350000);

// Construct simple model

RooAddPdf model("model","sig and bkg",RooArgList(PDF_Mass_Sig,bkg), RooArgList(nSig, nBkg));

// Fit model

model.fitTo(ds);

// Plot the mass distribution from 2216 to 2356 MeV

/*
RooPlot* myframe = Lambda_cplus_M.frame();
ds.plotOn(myframe);
model.plotOn(myframe);
model.plotOn(myframe,Components(bkg),LineStyle(kDashed),LineColor(2));
model.plotOn(myframe,Components(PDF_Mass_Sig_Comp),LineColor(3));
myframe->GetYaxis()->SetTitleOffset(1.6);
myframe->Draw();
*/

// Setting shape parameters constant
sigMean.setConstant();
sigWidth.setConstant();
x1.setConstant();
x2.setConstant();

TH1F h_yieldvstime("h_yieldvstime","h_yieldvstime",10,0.2,2.18);
 
// Save values for drawing
double signaly[10];
double signalx[10];
double error[10];

int i;
for (i=0; i<10; i++) {

	double binLowEdge = 0.2+(i*0.18);
	double binHighEdge = 0.38+(i*0.18);

	std::ostringstream binLowEdgeStr;
	binLowEdgeStr << binLowEdge;
	std::ostringstream binHighEdgeStr;
	binHighEdgeStr << binHighEdge;

	RooDataSet* bindata = ds->reduce(RooFit::Cut(TString(binLowEdgeStr.str()) + " < Lambda_cplus_TAU*1000. && Lambda_cplus_TAU*1000. <= " + TString(binHighEdgeStr.str()))) ; 

	model.fitTo(*bindata) ;
	double binSigYield = nSig.getVal() ;
	double binSigError = nSig.getError() ;
	double binCentre = (binHighEdge +binLowEdge)/2.;
	printf("binSigYield: %f", binSigYield);
	puts("");
	printf("binSigError: %f", binSigError);
	puts("");
	printf("binCentre: %f", binCentre);
	puts("");
	std::cout<<i<<std::endl;

	// Save to arrays
	signaly[i] = binSigYield;
	signalx[i] = binCentre;
	error[i] = binSigError;
	h_yieldvstime->SetBinContent(i+1, signaly[i]);
	h_yieldvstime->SetBinError(i+1, binSigError);
	
}

TFile hf("histofxi.root","RECREATE");
h_yieldvstime.Write();

/*
TF1 taufit("taufit","[0]*exp([1]*x)",-1000,1000);
// Draw
gr = new TGraph(20,signalx,signaly);
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->SetTitle("Lambda C lifetime plot");
   gr->GetXaxis()->SetTitle("Time in ps");
   gr->GetYaxis()->SetTitle("Signal Yield");
   gr->Fit(&taufit);
   gr->PaintStats(&taufit);
   gr->Draw();
*/
}
