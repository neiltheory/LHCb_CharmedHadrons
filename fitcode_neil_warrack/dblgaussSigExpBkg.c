// From Gedeminas work
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

#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExponential.h"
#include "RooPolynomial.h"
#include "RooAbsData.h"
#include "TLatex.h"
#include "TFile.h"
#include "TLegend.h"

// Script plots a double gaussian signal + Exp background fit to Lambda_cplus 
// mass histogram with options of poly background.
// Prints variables to plot (No Chi Square).
// Prints min and max values of mass (GeV), and FCN value to screen.


void fitfunc() {

  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/RooFit/firstTry/datafile.root");
  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;
  

  // DO THIS FIRST!
  /*
  // Write .root dataset file from TTree 
  TFile *file_01 = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/data/turbo_2015_data.root");
  TTree* mytree = (TTree*) file_01->Get("Lambda_cToKppiTuple/DecayTree;1");
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216,2356);
  RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_M),Import(*mytree)) ;
  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/learning_root/RooFit/firstTry/datafile.root","recreate");
  ds.Write() ;
  datafile->Close(); 
  */


  // Define x-axis variable, get limits.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356 ,"GeV") ;  
  double highest;
  double lowest;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowest, highest);
 
   // Limits of data
   double mass_max = highest;
   double mass_min = lowest;
   // Initializing value for fit 
   double mass_peak = 2286;
   
   // Build Gaussian PDF
   RooRealVar gausMean("gausMean", "gausMean",mass_peak, mass_min, mass_max, "GeV");
   RooRealVar sigma1("sigma1","sigma1", 6, 0, 50) ;
   RooGaussian gauss1("gauss1","gauss1",Lambda_cplus_M, gausMean, sigma1);

   RooRealVar sigma2("sigma2","sigma2", 6, 0, 50) ;
   RooGaussian gauss2("gauss2","gauss2",Lambda_cplus_M, gausMean, sigma2);

   RooRealVar nFrac("nFrac", "nFrac", 0.5, 0.0, 1.);
   RooAddPdf gaussCom("gaussCom","gaussCom", RooArgList(gauss1, gauss2), RooArgList(nFrac));

   // Build Polynomial PDF
   // RooPolynomial pol0("pol0","pol0",Lambda_cplus_M,RooArgList());
   // RooRealVar pol0_yield("pol0_yield","yield of polynom",100000,0,656625);
  
   // Build exponential PDF
   RooRealVar expoPar("expoPar","expoPar", -0.00001, -5., 0.);
   RooExponential expo_bkg("expo_bkg", "expo_bkg", Lambda_cplus_M, expoPar);

   // Define signal & background coeffs
   RooRealVar nSignal("nSignal","nSignal", 100000, 0, 656625);
   RooRealVar nBkg("nBkg","nBkg", 500000, 0, 656625);
  
   // Build model PDF
   //RooAddPdf model("model","model",RooArgList(gauss, pol0),RooArgList(nSignal, pol0_yield));
   //RooAddPdf model("model","model",RooArgList(gauss, expo_bkg),RooArgList(nSignal, nBkg));
   RooAddPdf model("model","model",RooArgList(gaussCom, expo_bkg),RooArgList(nSignal, nBkg));

   // Fit model
   //model.fitTo(*ds, Extended());
   RooFitResult* rmodel = model.fitTo(*ds,Save()) ;
   //RooFitResult* rexpo_bkg = expo_bkg.fitTo(*ds,Save());


   // Plot
   RooPlot *Lambda_cplus_Mframe = Lambda_cplus_M.frame(Title("-Title-"));
   //ds.plotOn(frame,Binning(25)); //default is 100 bins
   ds->plotOn(Lambda_cplus_Mframe, Name("data"), MarkerColor(kBlack)) ;
   ds->statOn(Lambda_cplus_Mframe, Name("stats"), Layout(0.65,0.88,0.2), What("N")) ; //NB Layout(xmin,xmax,ymax) 
   model.plotOn(Lambda_cplus_Mframe, Name("Model"), DrawOption("L")) ;
   model.plotOn(Lambda_cplus_Mframe, Components(expo_bkg), LineStyle(kDashed)) ;
   model.paramOn(Lambda_cplus_Mframe,Layout(0.19, 0.45, 0.88)) ; //was 0.4
   Lambda_cplus_Mframe->getAttText()->SetTextSize(0.022) ; 
   
   // Draw on Canvas   
   TCanvas *c1 = new TCanvas("c1","canvas_name",700,500);
   c1->cd(1) ; 
   gPad->SetLeftMargin(0.15) ; 
   Lambda_cplus_Mframe->GetYaxis()->SetTitleOffset(1.4) ; 
   Lambda_cplus_Mframe->Draw() ;
   c1->SaveAs("dblGaussExpMassFit01.eps");

   // create canvas
   //TCanvas *c1 = new TCanvas("c1","canvas_name",10,10,700,500) ;
   //formatCanvas4(c1) ; 
   //c1_1->cd();
   //c1->SetFillColor(33);
   //c1->SetFrameFillColor(41);
   //c1->SetGrid();
   //frame->GetYaxis()->SetTitleOffset(1.4) ; frame->Draw() ;
   


  //TCanvas *c1 = new TCanvas("c1","GausFit01",10,10,700,500);
  //TCanvas* c1 = new TCanvas("rf101_basics","rf101_basics",800,400) ;
  //c1->Divide(2) ;
  //c1->cd(1) ; gPad->SetLeftMargin(0.15) ; xframe->GetYaxis()->SetTitleOffset(1.6) ; xframe->Draw() ;
  //c1->cd(2) ; gPad->SetLeftMargin(0.15) ; xframe2->GetYaxis()->SetTitleOffset(1.6) ; xframe2->Draw() ; 
  //xframe->Draw();
   
   TLatex Tl;
   Tl.SetNDC();
   Tl.SetTextSize(0.025); //was 0.02
   Tl.DrawLatex(0.69,0.75,"#scale[1.1]{#Lambda^{+}_{c}} #rightarrow K^{-} #pi^{+} #it{p}");
   Tl.DrawLatex(0.68,0.8,"Charmed Lambda (udc)");

   // Print info to screen
  
  

   cout<<endl<<"________________INFO__________________"<<endl<<endl;  
   cout<< "lowest independent variable value = "<<lowest<<endl;
   cout<< "highest independent variable value= "<<highest<<endl;
   ds->Print();  //number of events in dataset    
   rmodel->Print(); //results  
 
}


// Canvas formatting (4 plots on landscape A4)
void formatCanvas4(TCanvas* canvas){
  canvas->SetBorderSize(2);
  canvas->SetBorderMode(-1);
  canvas->Clear();
  canvas->Divide(2,2);
  canvas->cd(1);
  gPad->SetLeftMargin(0.2);
  gPad->SetBottomMargin(0.2);
  canvas->cd(2);
  gPad->SetLeftMargin(0.2);
  gPad->SetBottomMargin(0.2);
  canvas->cd(3);
  gPad->SetLeftMargin(0.2);
  gPad->SetBottomMargin(0.2);
  canvas->cd(4);
  gPad->SetLeftMargin(0.2);
  gPad->SetBottomMargin(0.2);
}
