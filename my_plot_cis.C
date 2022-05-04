#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "TROOT.h"
#include "TTree.h"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TTimeStamp.h"
#include "TDatime.h"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TFrame.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPad.h"
#include "TBox.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TMap.h"




void plot(int run, int partition, int module, int channel, int gain){
	std::cout << "here lol\n";
	TString filename;
	filename = filename.Format("tileCalibCIS_%d_CIS.0.root",run);
	filename = TString("/afs/cern.ch/user/t/tilecali/public/w0/ntuples/cis/") + filename;
	TFile *file = TFile::Open(filename,"READ");

	file->ls();

	TMap * cisScans = (TMap *)file->FindObjectAny("cisScans");
	if (!cisScans) {
		delete file;
		std::cout << "No CIS scan :(\n";
	} else {
		std::cout << "CIS scan found\n";
		const char* str_part[5] = {"AUX", "EBC", "LBC", "LBA", "EBA"};
		const char* str_gain[2] = {"lowgain", "highgain"};
		TString chan;
		chan=chan.Format("%s%02d_c%02d_%s",str_part[partition],module+1,channel,str_gain[gain]);
		std::cout << chan << '\n';
		TString name;
		name = name.Format("scan%d_%d_%d_%d",partition, module, channel, gain);
		std::cout << name << '\n';
		TPair *obj = ((TPair *)(cisScans->FindObject(name)));
		if (!obj) {
			std::cout << "Ch not found :(\n";
		} else {
			std::cout << "Ch found!\n";
			TString title;
			title=title.Format("Run %d: %s",run,(const char *)chan);
			TCanvas * c1 = 0;
			c1 = new TCanvas(name,title);
			TGraphErrors * gr = ((TGraphErrors *)(obj->Value()));
			gr->SetTitle(title);
			gr->SetMarkerStyle(20);
			gr->SetMarkerSize(1);
			double ymin = 10000;
			double ymax = 0;
			double xmax = 0;
			int nn=gr->GetN();
			std::cout << "nn is " << nn << '\n';
			for (int i=nn-1; i>=0; --i) {
				if (gr->GetPointX(i)>xmax) xmax=gr->GetPointX(i);
				if (gr->GetPointY(i)>ymax) ymax=gr->GetPointY(i);
				if (gr->GetPointY(i)<ymin) ymin=gr->GetPointY(i);
			}
			gr->SetMinimum(0.);
			if (ymax>190. || ymax-ymin>25.) {
				if (ymax<1050.) gr->SetMaximum(1104.84);
				else if (ymax<1300.) gr->SetMaximum(1325.808);
			} else {
				gr->SetMaximum(200.);
			}
			if (xmax>32. || gain==0) {
				xmax=864.;
			} else if (xmax>13.) {
				xmax=27.;
			} else {
				xmax=13.5;
			}
			std::cout << "Xlims " << xmax << '\n';
			std::cout << "Ylims " << ymin << " " << ymax << '\n';
			gr->AddPoint(0.,0.);
			gr->AddPoint(xmax,ymax);
			TVirtualPad *p1=c1->cd(1);
			gr->RemovePoint(nn+1);
			gr->Draw("A");
			gr->GetXaxis()->SetRangeUser(0.,xmax);
			gr->RemovePoint(nn);
			gr->Draw("APL");
			p1->Update();
			c1->Flush();
			c1->ForceUpdate();
			c1->Update();
		}
		delete cisScans;
	}
	delete file;
}



void plot_all_runs(int partition, int module, int channel, int gain){
	int runs[]{416626, 416941, 417164, 417535, 417882, 418169, 418412, 418778, 418994, 419504};
	int num_runs{ static_cast<int>(std::size(runs)) };
	for (int i{0}; i<num_runs; ++i){
		plot(runs[i], partition, module, channel, gain);
	}
}
