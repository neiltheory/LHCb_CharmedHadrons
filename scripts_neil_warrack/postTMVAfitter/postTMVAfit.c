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
// of the Lambda_cplus particle (OR the Xi_c).


void fitFunc() {

  // Open appropriate "dataset" .root file.
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356.root");
  //TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs_Mmin2216_max2356_CutIPCHI2lt3.root"); 
  TFile *datafile = TFile::Open("/afs/phas.gla.ac.uk/user/n/nwarrack/public_ppe/myLHCb/Gedcode/LHCb_CharmedHadrons/data/TMVA_cut01_firsttry.root") ;


  TTree* mytree = (TTree*) datafile->Get("TrainTree") ;

  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.00025 ,0.002 ,"ns") ;  //real range of interest is [0.00025, 0.002], this is defined later.
  RooRealVar Lambda_cplus_M("Lambda_cplus_M","Lambda_cplus_M",2216 ,2356, "GeV") ; 
  RooRealVar Lambda_cplus_IPCHI2_OWNPV("Lambda_cplus_IPCHI2_OWNPV","Lambda_cplus_IPCHI2_OWNPV",-100 ,100) ; 
  RooRealVar BDTG ("BDTG","BDTG",-1. ,1.) ; 

  // Define dataset
  //RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;
  RooDataSet ds("ds","ds",RooArgSet(Lambda_cplus_M,Lambda_cplus_IPCHI2_OWNPV,BDTG,Lambda_cplus_TAU),Import(*mytree),Cut("(BDTG>0.091)&&(Lambda_cplus_IPCHI2_OWNPV<3)")) ;


  //double highestTAU;
  //double lowestTAU;
  //ds->RooAbsData::getRange(Lambda_cplus_TAU, lowestTAU, highestTAU);
  //cout<<"lowest TAU entry is = "<<lowestTAU<<endl ;

  double highestM;
  double lowestM;
  ds->RooAbsData::getRange(Lambda_cplus_M, lowestM, highestM);
  //cout<<"lowest mass value entry is = "<<lowestM<<endl ;
  

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
  //RooRealVar expoPar("expoPar","expoPar", -0.0001, -1., 0.);
  //RooExponential expo_bkg("expo_bkg", "expo_bkg", Lambda_cplus_M, expoPar);

  // Build Chebychev PDF; "cheb_bkg"
  RooRealVar cheb1("cheb1", "cheb1", 0, -1., 1.) ;
  RooRealVar cheb2("cheb2", "cheb2", 0, -1., 1.) ;
  RooChebychev cheb_bkg("cheb_bkg","cheb_bkg",Lambda_cplus_M,RooArgSet(cheb1,cheb2)) ;


  // Build model PDF
  RooRealVar nSignal("nSignal","nSignal", 23300, 0, 40000);
  RooRealVar nBkg("nBkg","nBkg", 13200, 0, 40000); //36499
  
  //RooAddPdf model("model","model",RooArgList(gauss, pol0),RooArgList(nSignal, pol0_yield));
  //RooAddPdf model("model","model",RooArgList(gauss, expo_bkg),RooArgList(nSignal, nBkg));
  //RooAddPdf model("model","model",RooArgList(gaussComb, expo_bkg),RooArgList(nSignal, nBkg));
  RooAddPdf model("model","model",RooArgList(gaussComb, cheb_bkg),RooArgList(nSignal, nBkg)) ;


  // Fit model
  //model.fitTo(*ds, Extended()) ; 
  model.fitTo(ds, Extended()) ; 
  //model.fitTo(*ds, Range("R1"));
  //expo1.fitTo(*ds, Range("R1"));
  //RooFitResult* rmodel = model.fitTo(*ds,Save()) ;
  //RooFitResult* rexpo_bkg = expo_bkg.fitTo(*ds,Save());


  // Plot
  RooPlot *fullDataFit = Lambda_cplus_M.frame(Title("Lambda_cplus_M with BDTG cut at 0.1506 => (S:sqr(S+B))=164.7712"));
  //ds.plotOn(frame,Binning(25)); //default is 100 bins
  //ds->plotOn(fullDataFit, Name("data"), MarkerColor(kBlack)) ;
  ds.plotOn(fullDataFit, Name("data"), MarkerColor(kBlack)) ;  
  //ds->statOn(fullDataFit, Layout(0.65,0.88,0.2), What("N")) ; //NB Layout(xmin,xmax,ymax)
  ds.statOn(fullDataFit, Layout(0.65,0.88,0.5), What("N")) ; //NB Layout(xmin,xmax,ymax)
  model.plotOn(fullDataFit, Name("Model"), DrawOption("L")) ;
  //model.plotOn(fullDataFit, Components(expo_bkg), LineStyle(kDashed)) ;
  model.plotOn(fullDataFit, Components(cheb_bkg), LineStyle(kDashed)) ;
  model.paramOn(fullDataFit,Layout(0.19, 0.45, 0.88)) ; //was 0.4
  fullDataFit->getAttText()->SetTextSize(0.022) ;
  //fullDataFit->Draw() ;
  


  TCanvas *c1 = new TCanvas("c1","",700,500) ;
  //formatCanvas4(c102) ;

  //c102_1->cd() ;
  //c1->SetFillColor(33) ;
  //c1->SetFrameFillColor(41) ;
  c1->SetGrid() ;
  fullDataFit->GetYaxis()->SetTitleOffset(1.4) ; fullDataFit->Draw() ;
  gPad->SetLeftMargin(0.15) ;
  fullDataFit->GetYaxis()->SetTitleOffset(1.4) ;
  fullDataFit->Draw() ;
  c1->SaveAs("TEST1.eps");




c1->SaveAs("TEST1.png") ;












  //RooDataHist hist4Chi2("hist4Chi2","hist4Chi2", RooArgSet(Lambda_cplus_M), *ds) ;
  //Double_t chi2 = fullDataFit->chiSquare("Model","data",7) ;


  // Set model fit variables to constants (NOT COEFFs!)
  gausMean1.setConstant() ;
  gausMean2.setConstant() ;
  sigma1.setConstant() ;
  sigma2.setConstant() ;
  nFrac.setConstant() ;
  //expoPar.setConstant() ;
  cheb1.setConstant() ;
  cheb2.setConstant() ;



  /*
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

    //model.fitTo(*bindata, gErrorIgnoreLevel = kInfo) ;
    model.fitTo(*bindata, "l") ;
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



  TFile hf("histo_Lambda_cplus_TAU_lifetime_SigOnly.root", "RECREATE") ;
  h_sig.Write() ;





  //__________________TEST1
  TCanvas *c102 = new TCanvas("c102","",600,900) ;
  formatCanvas4(c102) ;

  c102_1->cd() ;
  //c1->SetFillColor(33) ;
  //c1->SetFrameFillColor(41) ;
  c1->SetGrid() ;
  fullDataFit->GetYaxis()->SetTitleOffset(1.4) ; fullDataFit->Draw() ;
  gPad->SetLeftMargin(0.15) ;
  fullDataFit->GetYaxis()->SetTitleOffset(1.4) ;
  fullDataFit->Draw() ;
  c1->SaveAs("TEST1.eps");
  */

  /*
  //__________________TEST2  
  // Plot & Draw
  //TCanvas *c1 = new TCanvas("c1","c1", 800, 800) ;
  //c1->SetLogy() ;
  //RooPlot *signalPlot = Lambda_cplus_TAU.frame(Title("Signal plot")) ;
  h_sig.Draw() ;
  //signalPlot->Draw() ;
  c1->SaveAs("TEST2signalHistoPlot_from_lifetimebinning.png") ;
  */


  cout<<endl<<endl<<"************info************"<<endl;
  //cout<<"signalHistoPlot_from_lifetimebinning.png written"<<endl<<endl ;
  cout<<"double Gaussian fit parameters to full data (signal + background):"<<endl;
  cout<<gausMean1<<endl ;
  cout<<gausMean2<<endl ;
  cout<<sigma1<<endl ;
  cout<<sigma2<<endl ;
  cout<<nFrac<<endl ;
  //cout<<"exponential fit parameters to full data (signal + background):"<<endl;
  //cout<<expoPar<<endl<<endl ;
  cout<<"chebychev fit parameters to full data (signal + background):"<<endl;
  cout<<cheb1<<endl ;
  cout<<cheb2<<endl ;


  // Create arrays to store signal yeald and error for each bin. 
  //double signalYield[10] ;
  //double signalError[10] ;
  //double binCent[10] ; // Records centre of bin, a relic from Gediminas' code

  // Split TAU distribution into bins, make mass fit to bin contents, fill 
  // histogram "h_sig" with signal yield calculated from fit.


   /*
  int k ;
  for (k=0; k<nBins; k++){
    cout<<"Bin number "<<k+1<<" (bin centre = "<<binCent[k]<<"ns):"<<endl;
    cout<<"   "<<"signal yield = "<<signalYield[k]<<endl ;
    cout<<"   "<<"signal error = "<<signalError[k]<<endl<<endl ;
  }
  cout<<"done."<<endl<<endl<<endl ;
   */
    
    /*
        // Chi^2___________________
	Lambda_cplus_M.setBins(100);
	RooDataHist hist("hist","hist", RooArgSet(Lambda_cplus_M), *ds) ;
	RooChi2Var chi2("chi2","chi2", model, hist, true) ;
	RooMinuit minuit(chi2);
	minuit.migrad();
	minuit.hesse();
    */
    
    /*  
	gPad->SetLeftMargin(0.15) ;
	Lambda_cplus_TAUframe->GetYaxis()->SetTitleOffset(1.4) ; 
	Lambda_cplus_TAUframe->Draw() ;
	c1->SaveAs("temp_binned3.eps");
    */  
    
    
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
