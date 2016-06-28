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

void binning() {

  TFile *datafile = TFile::Open("~/Documents/uni/LHCb_CharmSummerProj/Gedcode/baryon-lifetimes-2015/data/run-II-data/datafileLambda_TAUmin200fs_max2200fs.root"); // Open "dataset" .root file (made using mkDataSet.c script).

  RooDataSet* ds = (RooDataSet*)datafile->Get("ds") ;

  // Define x-axis variable, get limits.
  RooRealVar Lambda_cplus_TAU("Lambda_cplus_TAU","Lambda_cplus_TAU",0.0002 ,0.0022 ,"ns") ;  
  double highest;
  double lowest;
  ds->RooAbsData::getRange(Lambda_cplus_TAU, lowest, highest);

  //Lambda_cplus_TAU.setRange("R1",0.00018, 0.0012);
   
   // Fit model
   //model.fitTo(*ds, Range("R1"));
   //expo1.fitTo(*ds, Range("R1"));
   //model.fitTo(*ds) ;
   //RooFitResult* rmodel = model.fitTo(*ds,Save()) ;
   //RooFitResult* rexpo_bkg = expo_bkg.fitTo(*ds,Save());
   

   // Plot
   //RooPlot *Lambda_cplus_TAUframe = Lambda_cplus_TAU.frame(Title("-Title-"));
  int nBins ;
  nBins = 10 ;
  RooPlot *Lambda_cplus_TAUframe = new RooPlot("Lambda_cplus_TAUframe","Lambda_cplus_TAUframe", Lambda_cplus_TAU, 0.00025, 0.002, nBins) ; 

  //ds.plotOn(frame,Binning(10)); //default is 100 bins
  ds->plotOn(Lambda_cplus_TAUframe, Name("data"), MarkerColor(kBlack)) ;

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
  
  // Draw on Canvas   
  TCanvas *c1 = new TCanvas("c1","canvas_name",700,500);
  c1->cd(1) ; 
  //c1->SetLogy() ;
  
   gPad->SetLeftMargin(0.15) ;
   Lambda_cplus_TAUframe->GetYaxis()->SetTitleOffset(1.4) ; 
   Lambda_cplus_TAUframe->Draw() ;
   c1->SaveAs("temp_binned.eps");
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
   cout<< "Lowest independent variable value in dataset = "<<lowest<<endl;
   cout<< "Highest independent variable value in dataset = "<<highest<<endl;
   ds->Print();  //number of events in dataset    
   //rmodel->Print(); //results  
   //cout<<"Chi squared ="<<chi2<<endl ;
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
