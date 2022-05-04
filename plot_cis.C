//
// 2022-04-11 Sanya.Solodkov@cern.ch

// Macro to plot TGraph with CIS fit results from tileCalibCIS_XXXXXX_CIS.Y.root
// XXXXXX - run number
// Y - version number (0 by default)
// channel argumenr for the macro is channel hardware ID (like in Athena)
// e.g. 1/13/0/0  - LBA14 ch 0 low gain (demonstrator)
//      2/25/10/1 - LBC26 ch 10 high gain
//
// examples:
//
//    .L plot_cis.C++
//    plot_cis(417576,"1/13/0/1");            // version 0 by default, file tileCalibCIS_417576_CIS.0.root will be used
//    plot_cis(417576,"1/13/0/1",0,true);     // "true" option - to save plot to png file
//
//    plot_cis(417576);                       // create plots for all channels which were problematic in March 2022
//    plot_cis(417576,"",0,true,false,true);  // the same, but look at plots one-by-one and save to png file
//    plot_cis(417576,"9");                   // create plots for channel 9 for all modules which were problematic in March 2022
//

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

char getq() {
     std::string s;
     std::getline( cin, s );
     return s.c_str()[0];
}

char plot_cis(int run, const char * channel = NULL, int ver=0, bool saveCanvas=false, bool deleteCanvas=false, bool wait=false, double YMAX=-1.)
{

  //gStyle->SetMarkerStyle(20);
  //gStyle->SetMarkerSize(1);

  TString name;
  std::vector<TString> badch;

  if (channel && strlen(channel)>2) {

    badch.clear();
    if (index(channel,'X')) {
      name = channel;
      name.Replace(name.Index('X'),1,"%d");
      for (int i=0; i<48; ++i) {
        badch.push_back(name.Format((const char *)name,i));
      }
    } else {
      badch.push_back(channel);
    }

  } else {

    // some channels whcih were bad in April 2022

    if (channel and strlen(channel)>0) {

      // problems in whole drawer but plot just one channel

      badch.push_back(name.Format("1/0/%s/1" ,channel));
      badch.push_back(name.Format("1/33/%s/0",channel));
      badch.push_back(name.Format("1/33/%s/1",channel));
      badch.push_back(name.Format("1/50/%s/0",channel));
      badch.push_back(name.Format("1/51/%s/1",channel));
      badch.push_back(name.Format("2/33/%s/0",channel));
      badch.push_back(name.Format("2/33/%s/1",channel));
      badch.push_back(name.Format("2/35/%s/0",channel));
      badch.push_back(name.Format("2/35/%s/1",channel));
      badch.push_back(name.Format("4/0/%s/0" ,channel));

    } else {

      // single channel problems

      // demonstrator
      badch.push_back("1/13/0/0");
      //badch.push_back("1/13/13/0");
      //badch.push_back("1/13/24/0");
      //badch.push_back("1/13/37/0");
      badch.push_back("1/13/1/1");
      //badch.push_back("1/13/14/1");
      //badch.push_back("1/13/25/1");
      //badch.push_back("1/13/38/1");

      badch.push_back("1/3/6/0");
      badch.push_back("1/5/38/0");
      badch.push_back("1/44/6/1");
      badch.push_back("1/51/36/0");
      badch.push_back("1/51/36/1");
      badch.push_back("1/54/24/0");
      badch.push_back("1/54/25/0");
      badch.push_back("1/54/26/0");
      badch.push_back("1/61/26/1");

      badch.push_back("2/22/20/0");
      badch.push_back("2/27/4/0");
      badch.push_back("2/40/25/1");
      badch.push_back("2/42/24/1");
      badch.push_back("2/46/35/0");
      badch.push_back("2/51/18/1");
      badch.push_back("2/51/34/0");

      badch.push_back("4/17/36/1");
      badch.push_back("4/21/16/0");
      badch.push_back("4/33/39/0");
      badch.push_back("4/33/40/0");
      badch.push_back("4/33/41/0");
    }
  }

  TString filename;
  filename = filename.Format("tileCalibCIS_%d_CIS.%d.root",run,ver);
  TFile *file = TFile::Open(filename,"READ");
  if (!file) {
    // no file in current directory, try default location
    filename = TString("/afs/cern.ch/user/t/tilecali/public/w0/ntuples/cis/") + filename;
    std::cout << "Opening " << filename;
    file = TFile::Open(filename,"READ");
    if (!file) {
      return 'X';
    } else {
      std::cout << " OK" << std::endl;
    }
  }

  TMap * cisScans = (TMap *)file->FindObjectAny("cisScans");
  if (!cisScans) {
    delete file;
    return 'Y';
  }

  char ans=0;
  TString chan,title;
  TCanvas * c1 = 0;
  const char * part[5] = {"AUX","LBA","LBC","EBA","EBC"};
  const char * gain[2] = {"LG","HG"};

  for (size_t i=0; i<badch.size(); ++i) {
    chan=badch[i];
    int pa=chan.Atoi();
    int mo=chan.Remove(0,chan.Index('/')+1).Atoi();
    int ch=chan.Remove(0,chan.Index('/')+1).Atoi();
    int gn=chan.Remove(0,chan.Index('/')+1).Atoi();
    chan=chan.Format("%s%02d ch %02d %s",part[pa],mo+1,ch,gain[gn]);
    name=name.Format("scan%s",(const char *)badch[i]);
    name.ReplaceAll("/","_");
    TPair *obj = ((TPair *)(cisScans->FindObject(name)));
    if (!obj) {
      std::cout << "Channel " << badch[i] << " not found in run " << run << std::endl;
      ans='Z';
      continue;
    } else {
      ans=0;
    }
    name=name.Format("channel_%s",(const char *)badch[i]);
    title=title.Format("Run %d %s",run,(const char *)chan);
    c1 = new TCanvas(name,title);
    TGraphErrors * gr = ((TGraphErrors *)(obj->Value()));
    gr->SetTitle(title);
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1);
    double ymin = 10000;
    double ymax = 0;
    double xmax = 0;
    int nn=gr->GetN();
    for (int i=nn-1; i>=0; --i) {
      if (gr->GetPointX(i)>xmax) xmax=gr->GetPointX(i);
      if (gr->GetPointY(i)>ymax) ymax=gr->GetPointY(i);
      if (gr->GetPointY(i)<ymin) ymin=gr->GetPointY(i);
    }
    gr->SetMinimum(0.);
    if (YMAX>0) {
      gr->SetMaximum(YMAX);
    } else {
      if (YMAX==0 || ymax>190. || ymax-ymin>25.) {
        if (ymax<1050.) gr->SetMaximum(1104.84);
        else if (ymax<1300.) gr->SetMaximum(1325.808);
      } else {
        gr->SetMaximum(200.);
      }
    }
    if (xmax>32. || gn==0) {
      xmax=864.;
    } else if (xmax>13.) {
      xmax=27.;
    } else {
      xmax=13.5;
    }
    gr->AddPoint(0.,0.);
    gr->AddPoint(xmax,ymax);
    TVirtualPad *p1=c1->cd(1);
    gr->Draw("A");
    gr->GetXaxis()->SetRangeUser(0.,xmax);
    gr->RemovePoint(nn+1);
    gr->RemovePoint(nn);
    gr->Draw("APL");
    p1->Update();
    c1->Flush();
    c1->ForceUpdate();
    c1->Update();
    if (saveCanvas) {
      filename = name.Format("%s_%d_v%d.png",(const char *)chan,run,ver).ReplaceAll(" ","_");
      c1->SaveAs(filename);
    }
    if (wait) {
      std::cout << "press Enter to continue or q to quit" << std::endl;
      ans = getq();
    }
    if (deleteCanvas) {
      delete gr;
      //delete obj;
      delete c1;
    }
    if (ans=='q' || ans=='Q') break;
  }

  delete cisScans;
  delete file;
  return ans;

}




void check_all_runs(const char * channel = NULL){
  int runs[]{413287, 413579, 413594, 413976, 414257, 414464, 414513, 414763, 415169, 415434, 415531, 415564, 415914, 416099};
  int num_runs{ static_cast<int>(std::size(runs)) };
  for (int i{0}; i<num_runs; ++i){
    plot_cis(runs[i], channel,0,true);
  }
}
