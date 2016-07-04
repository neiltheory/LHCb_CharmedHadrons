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

using namespace RooFit ;
using namespace RooStats ; //namespace for sWeights 

void fitter()
{
// Load tree file and data set file for Lambda mass entries
TFile *myfile = TFile::Open("/nfs/lhcb/malexander01/charm/baryon-lifetimes-2015/data/run-II-data/turbo_2015_data.root");

TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/g/gsarpis/lambdaC/datafile.root");

TTree* mytree = (TTree*) myfile->Get("Lambda_cToKppiTuple/DecayTree;1");

RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216,2356);

RooDataSet* ds = (RooDataSet*) datafile->Get("ds;1");

// List of parameters 

double mass_lower_bound = 2216;
double mass_upper_bound = 2356;
double mass_peak = 2286;

/*Code for saving dataset
RooDataSet ds("ds","ds with Lambda_cplus_M",mytree,RooArgSet(Lambda_cplus_M));
TFile f("datafile.root","RECREATE") ;
ds->Write();
f.Close();
*/

// Builds Gaussian signal PDFs 
RooRealVar sigMean("sigMean","sigMean",mass_peak, mass_lower_bound, mass_upper_bound); 
RooRealVar sigWidth("sigWidth", "sigWidth",5,0,50); 
RooGaussian PDF_Mass_Sig("PDF_Mass_Sig", "gaussian PDF", Lambda_cplus_M, sigMean, sigWidth);
RooRealVar sigWidth2("sigWidth2", "sigWidth2", 15,0,50);
RooGaussian PDF_Mass_Sig2("PDF_Mass_Sig2", "gaussian 2nd PDF", Lambda_cplus_M, sigMean, sigWidth2);

// Build a double Gaussian
RooRealVar nFrac("nFrac", "Fraction of 2 Gaussians", 0.5, 0,1);
RooAddPdf PDF_Mass_Sig_Comp("PDF_Mass_Sig_Comp", "PDF_Mass_Sig_Comp", RooArgList(PDF_Mass_Sig, PDF_Mass_Sig2), RooArgList(nFrac));

/* Build gaussian signal PDF
RooRealVar mean("mean","mean of gaussian",mass_peak,mass_lower_bound,mass_upper_bound);
RooRealVar sigma("sigma","width of gaussian",6,0,50);
RooGaussian gauss("gauss","gaussian PDF",Lambda_cplus_M,mean,sigma);
*/

// Exponential background
RooRealVar expo("expo", "expo shape parameter", -0.00005, -100., 10); 
RooExponential expo_bkg("expo_bkg", "expo_bkg", Lambda_cplus_M, expo); 

// define event yields
RooRealVar nSig("nSig", "", 1000,700000);
RooRealVar nBkg("nBkg", "", 1000,700000);

// Construct simple model
//
//RooRealVar bkgfrac("bkgfrac","bkg fraction",0.15,0,1);
//RooAddPdf model("model","sig and bkg",RooArgList(gauss,expo_bkg),RooArgList(nSig, nBkg) );
//
RooAddPdf model("model","sig and bkg",RooArgList(PDF_Mass_Sig_Comp, expo_bkg), RooArgList(nSig, nBkg));

// Fit model

model.fitTo(*ds);

// Plot the mass distribution from 2216 to 2356 MeV
RooPlot* myframe = Lambda_cplus_M.frame();
ds.plotOn(myframe);
model.plotOn(myframe);
model.plotOn(myframe,Components(expo_bkg),LineStyle(kDashed));
myframe->Draw();
}

/*
sigma.setConstant();
expo.setConstant();
sigWidth.setConstant();
sigWidth2.setConstant();

RooStats::SPlot* sDataM = new RooStats::SPlot("sData","An SPlot", *ds, &model, RooArgList(nSig,nBkg) );

std::cout << std::endl <<  "Yield of signal is " << nSig.getVal() << ".  From sWeights it is " << sDataM->GetYieldFromSWeight("nSig") << std::endl;
  std::cout << "Yield of background is " << nBkg.getVal() << ".  From sWeights it is " << sDataM->GetYieldFromSWeight("nBkg") << std::endl << std::endl;


// Code for saving dataset with weights
//RooRealVar w("w","w",-5000,5000);
//RooDataSet Wgt("Wgt","dataset with weights",mytree,RooArgSet(Lambda_cplus_M),WeightVar(w));
TFile f("sdatafile.root","RECREATE") ;
sDataM->Write() ;
f.Close() ;

TFile treefile("sTree.root", "recreate") ;
RooTreeDataStore sTree("sTree", "sTree", *ds->get(0), *ds->store()) ;
TTree* tree = sTree.tree() ;
tree->SetBranchStatus("Lambda_cplus_M", 0) ; // you don't want to save the mass variables again.
tree->Write() ;
treefile.Close() ;
}
*/




  



