#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"



// Script plots a gaussian signal + polynomial (max degree: 2) background 
// fit to B0 mass branch from TTree tuple histogram.
// Prints variables to plot (No Chi Square).
// Prints min and max values of mass (GeV), and FCN value to screen.

// DECLARE FUNCTION: quadratic background formula
Double_t bg(Double_t *x, Double_t *par){
  return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
}

// DECLARE FUNCTION: gaussian signal formula
Double_t gaussian(Double_t* x, Double_t *par){
  return par[2]*TMath::Gaus(*x,par[0],par[1],true);
  //NB: 'true' normalises gaussian.
}

// DECLARE FUNCTION: Sum of backgroung and gaussian
Double_t fitFunction(Double_t *x, Double_t *par) {
   return bg(x,par) + gaussian(x,&par[3]);
}


void FittingExamp() {
  TFile *file1 = new TFile("/nfs/lhcb/b2hh01/Bd2PiPi/MC/Bd2PiPi-MC11a-Unbiased-Reformatted.root");

  //create canvas
  TCanvas *c1 = new TCanvas("c1","GausLinFit01",10,10,700,500);
  formatCanvas4(c1); 
  c1_1->cd();
 // c1->SetFillColor(33);
  //c1->SetFrameFillColor(41);
  //c1->SetGrid();

  TH1D *histo = new TH1D("histo","FittedHistogram", 1000, 4820, 5675);

  TTree* B2HHTuple = (TTree*)file1->Get("DecayTreeTuple/B2HHTuple");

  //  B2HHTuple->Draw("B0_M >> histo", "B0_BKGCAT==0"); //ONLY SIGNAL!!
    B2HHTuple->Draw("B0_M >> histo"); //SIGNAL AND BACKGROUND!!

  histo->Draw("E"); //draws histo with error bars

  TF1 *fitfn = new TF1("fitfn",fitFunction, 4820, 5600, 6);
  fitfn->SetLineColor(kMagenta);  

  fitfn->SetParameters(1,-0.3,0,5280,25,histo->GetEntries());
  //fitfn->SetParameters(1,0,0,histo->GetEntries(),5280,25);
  //fitfn->SetParameters(1,0,0,5280,25);
  fitfn->FixParameter(2,0); //Fixes the 2nd parametar at zero so it is not optimized.
    // fitfn->FixParameter(5,histo->Integral("width"));
  cout<<"initial parameters set:"<<endl;
  cout<<"  Mean: 5280"<<endl;
  cout<<"  Width: 25"<<endl;
  cout<<"  norm: "<<histo->Integral("width")<<endl;
  cout<<"  Coefs in quadratic = 1"<<endl;
 
  fitfn->SetParName(0,"y-intercept");
  fitfn->SetParName(1,"gradient");
  fitfn->SetParName(2,"x^2 coef.");
  fitfn->SetParName(3,"Gaus Mean");
  fitfn->SetParName(4,"sigma");
  fitfn->SetParName(5,"normalisation");

  histo->Fit("fitfn", "","",5100,5500);
  // histo->Fit("fitfn");
  gStyle->SetOptFit(1111);
  
  TF1 *backFn = new TF1("backFn", bg, 4820, 5675, 3);
  TF1 *signalFn = new TF1("signalFn", gaussian,  4820, 5675, 3);
  
  backFn->SetLineColor(kRed);
  signalFn->SetLineColor(kBlue);
  
  Double_t par[6];
  fitfn->GetParameters(par);
  backFn->SetParameters(par);
  backFn->Draw("same");
  signalFn->SetParameters(&par[3]);
  //signalFn->SetNpx(500);
  signalFn->Draw("same");


  // draw the legend
  TLegend *legend=new TLegend(0.23,0.65,0.44,0.85);
  legend->SetTextFont(42);
  legend->SetTextSize(0.04);
  legend->AddEntry(histo,"Data","lpe");
  legend->AddEntry(backFn,"Background fit","l");
  legend->AddEntry(signalFn,"Signal fit","l");
  legend->AddEntry(fitfn,"Global Fit","l");
  legend->Draw();

  c1->Print("GausPolyFit_B0_M.pdf");


  //int binmin = histo->GetXaxis()->FindBin(4798);
  //int binmax = histo->GetXaxis()->FindBin(5802);
  //cout<<"min bin number is: "<<binmin<<endl;
  //cout<<"max bin number is: "<<binmax<<endl;
  
  int sigsum = signalFn->Integral(4800,5800);
  int totsum = fitfn->Integral(4800,5800);
  cout<<"signal integral: "<<sigsum<<endl;
  cout<<"global fit integral: "<<totsum<<endl;
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
