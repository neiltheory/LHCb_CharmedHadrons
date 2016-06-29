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


void binFit() {

  // Open appropriate "dataset" .root file (made using mkDataSet.c script)
  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356.root");

  // Define dataset
  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;

  // Define TAU variable, get limits.
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002 ,0.0022 ,"ns") ;  //real range of interest is [0.00025, 0.002], this is defined later.
  double highestTAU;
  double lowestTAU;
  ds->RooAbsData::getRange(Lambda_cplus_TAU, lowestTAU, highestTAU);
  ds->Print();  //NB: number of events in dataset ( = 861373 )


  // Define Mass variable, get limits.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356, "GeV") ; 
  double highestM;
  double lowestM;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowestM, highestM) ;
  
    // 'Initializing' value for fit
  double mass_peak = 2286;

  // Build combined double Gaussian PDF; "gaussComb".
  RooRealVar gausMean("gausMean", "gausMean",mass_peak, lowestM, highestM, "GeV") ;
  RooRealVar sigma1("sigma1","sigma1", 6, 0, 50) ;
  RooGaussian gauss1("gauss1","gauss1",Lambda_cplus_M, gausMean, sigma1) ;

  RooRealVar sigma2("sigma2","sigma2", 6, 0, 50) ;
  RooGaussian gauss2("gauss2","gauss2",Lambda_cplus_M, gausMean, sigma2) ;

  RooRealVar nFrac("nFrac", "nFrac", 0.5, 0.0, 1.) ;
  RooAddPdf gaussComb("gaussComb","gaussComb", RooArgList(gauss1, gauss2), RooArgList(nFrac)) ;

  //Lambda_cplus_TAU.setRange("R1",0.00018, 0.0012);
   
  // Build exponential PDF; "expo_bkg"
  RooRealVar expoPar("expoPar","expoPar", -0.00001, -5., 0.);
  RooExponential expo_bkg("expo_bkg", "expo_bkg", Lambda_cplus_M, expoPar);

  // Build model PDF
  RooRealVar nSignal("nSignal","nSignal", 200000, 0, 409570);
  RooRealVar nBkg("nBkg","nBkg", 200000, 0, 409570);
  
  //RooAddPdf model("model","model",RooArgList(gauss, pol0),RooArgList(nSignal, pol0_yield));
  //RooAddPdf model("model","model",RooArgList(gauss, expo_bkg),RooArgList(nSignal, nBkg));
  RooAddPdf model("model","model",RooArgList(gaussComb, expo_bkg),RooArgList(nSignal, nBkg));

  // Fit model
  //model.fitTo(*ds, Range("R1"));
  //expo1.fitTo(*ds, Range("R1"));
  model.fitTo(*ds) ;
  //RooFitResult* rmodel = model.fitTo(*ds,Save()) ;
  //RooFitResult* rexpo_bkg = expo_bkg.fitTo(*ds,Save());
  
  // Set model fit variables to constants (NOT COEFFs!)
  gausMean.setConstant() ;
  sigma1.setConstant() ;
  sigma2.setConstant() ;
  nFrac.setConstant() ;
  expoPar.setConstant() ;
 

  int nBins ; // define number of bins
  nBins = 10 ;
  
  // Create lifertime histogram of signal candidates
  TH1D h_sig("h_sig","h_sig",10 ,0.0002 ,0.0022) ;

  // Create arrays to store signal yeald and error for each bin. 
  double signalYield[10] ;
  double signalError[10] ;
  double binCent[10] ; // Records centre of bin, a relic from Gediminas' code

  // Split TAU distribution into bins, make mass fit to bin contents, fill 
  // histogram "h_sig" with signal yield calculated from fit.

  int i ;
  for (i=0; i<nBins; i++){
 
    double binBoundLo = 0.00025 + (i * ((0.002-0.00025)/nBins)) ;
    double binBoundHi = 0.00025 + ((i+1) * ((0.002-0.00025)/nBins)) ;

    std::ostringstream binLowEdgeStr;
    binLowEdgeStr << binBoundLo;
    std::ostringstream binHighEdgeStr;
    binHighEdgeStr << binBoundHi;

    // Creat dataset for bin i only
    RooDataSet* bindata = ds->reduce(RooFit::Cut(TString(binLowEdgeStr.str()) + " < Lambda_cplus_TAU && Lambda_cplus_TAU <= " + TString(binHighEdgeStr.str()))) ;

    model.fitTo(*bindata, gErrorIgnoreLevel = kInfo) ;
    double binSignalYield = nSignal.getVal() ;
    double binError = nSignal.getError() ;
    double binCentre = (binBoundLo + binBoundHi)/2. ;

    printf("binSignalYield = %f", binSignalYield) ; 
    puts("") ;
    printf("binError = %f", binError) ; 
    puts("") ;
    printf("binCentre = %f", binCentre) ; 
    puts("") ;
    cout<<i<<endl ;

    // Fill arrays
    signalYield[i] = binSignalYield ;
    signalError[i] = binError ;
    binCent[i] = binCentre ;

    // Fill histogram
    h_sig->SetBinContent(i+1, signalYield[i]) ;
    h_sig->SetBinError(i+1, signalError[i]) ;
}

  TFile hf("histotest.root", "RECREATE") ;
  h_sig.Write() ;
  /*
  // Plot
  //RooPlot *Lambda_cplus_TAUframe = Lambda_cplus_TAU.frame(Title("-Title-"));
  int nBins ;
  nBins = 10 ;
  RooPlot *Lambda_cplus_TAUframe = new RooPlot("Lambda_cplus_TAUframe","Lambda_cplus_TAUframe", Lambda_cplus_TAU, 0.00025, 0.002, nBins) ; 

  //ds.plotOn(frame,Binning(10)); //default is 100 bins
  ds->plotOn(Lambda_cplus_TAUframe, Name("data"), MarkerColor(kBlack)) ;
*/


  //ds->statOn(Lambda_cplus_TAUframe, Layout(0.65,0.88,0.2), What("N")) ; //NB Layout(xmin,xmax,ymax) 
  //model.plotOn(Lambda_cplus_TAUframe, Name("Model"), DrawOption("L")) ;
  //model.plotOn(Lambda_cplus_TAUframe, Components(expo2), LineStyle(kDashed)) ;
  //model.paramOn(Lambda_cplus_TAUframe,Layout(0.19, 0.45, 0.88)) ; //was 0.4
  //Lambda_cplus_TAUframe->getAttText()->SetTextSize(0.022) ; 
  
  //RooDataHist hist("hist","hist", RooArgSet(Lambda_cplus_TAU), *ds) ;
  //Double_t chi2 = Lambda_cplus_TAUframe->chiSquare("Model","data",2) ;
  //t1->Draw();
  
   

  // Chi^2___________________
  //Lambda_cplus_M.setBins(100);
  //RooDataHist hist("hist","hist", RooArgSet(Lambda_cplus_M), *ds) ;
  //RooChi2Var chi2("chi2","chi2", model, hist, true) ;
  //RooMinuit minuit(chi2);
  //minuit.migrad();
  //minuit.hesse();
  /*  
  // Draw on Canvas   
  TCanvas *c1 = new TCanvas("c1","canvas_name",700,500);
  c1->cd(1) ; 
  //c1->SetLogy() ;
  
   gPad->SetLeftMargin(0.15) ;
   Lambda_cplus_TAUframe->GetYaxis()->SetTitleOffset(1.4) ; 
   Lambda_cplus_TAUframe->Draw() ;
   c1->SaveAs("temp_binned3.eps");
  */  

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
   /*
   TLatex Tl;
   Tl.SetNDC();
   Tl.SetTextSize(0.025); //was 0.02
   Tl.DrawLatex(0.69,0.75,"#scale[1.1]{#Lambda^{+}_{c}} #rightarrow K^{-} #pi^{+} #it{p}");
   Tl.DrawLatex(0.68,0.8,"Charmed Lambda (udc)");
   
   // Print info to screen
   cout<<endl<<"________________INFO__________________"<<endl<<endl;  
   cout<< "Lowest lifetime value in dataset (should be:) = "<<lowestTAU<<endl;
   cout<< "Highest lifetime value in dataset = "<<highestTAU<<endl;
   cout<< "Lowest mass value in dataset (should be: 2216)= "<<lowestM<<endl;
   cout<< "Highest mass value in dataset (should be: 2356)= "<<highestM<<endl;
   
   cout<<"number of events should be: 861373"<<endl ; 
   ds->Print();  //number of events in dataset    
   //rmodel->Print(); //results  
   //cout<<"Chi squared ="<<chi2<<endl ;
   */
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
