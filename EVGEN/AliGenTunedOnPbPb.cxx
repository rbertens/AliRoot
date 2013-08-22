/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id: AliGenTunedOnPbPb.cxx 51126 2013-08-19 13:37:49Z fnoferin $ */

// Parameterisation based on 5.5 ATeV PbPb data
// pi,K, p , K0, lambda, phi, Xi, Omega spectra, v2, v3 (no jets!)
// Author: fnoferin@cern.ch

#include <TArrayF.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TF1.h>
#include <TH1.h>
#include <TPDGCode.h>
#include <TParticle.h>
#include <TROOT.h>
#include <TVirtualMC.h>

#include "AliConst.h"
#include "AliDecayer.h"
#include "AliGenEventHeaderTunedPbPb.h"
#include "AliLog.h"
#include "AliRun.h"
#include "AliGenTunedOnPbPb.h"

ClassImp(AliGenTunedOnPbPb)
  
// set default parameters for 10-20% centrality
Int_t AliGenTunedOnPbPb::fgPdgInput[fgNspecies] = {211,-211,111,321,-321,2212,-2212,310,3122,-3122,333,3312,-3312,3334,-3334};
Float_t AliGenTunedOnPbPb::fgMult[fgNspecies] = {450,450,450,70,70,21,21,70,20,20,8,2.4,2.4,0.4,0.4};

Float_t AliGenTunedOnPbPb::fgV3Overv2 = 6.25000000000000000e-01;
Float_t AliGenTunedOnPbPb::fgEventplane=0;

TF1 *AliGenTunedOnPbPb::fgV2 = NULL;

//_____________________________________________________________________________
AliGenTunedOnPbPb::AliGenTunedOnPbPb()
  :AliGenerator(),
   fCmin(0.),
   fCmax(100.),
   fChangeWithCentrality(kFALSE),
   fYMaxValue(2.0)
{
    //
    // Default constructor
    //
    SetCutVertexZ();
    SetPtRange();

    for(Int_t i=0;i < fgNspecies;i++){
      fgHSpectrum[i] = NULL;
      fgHv2[i] = NULL;
    }
}

//_____________________________________________________________________________
AliGenTunedOnPbPb::~AliGenTunedOnPbPb()
{
  //
  // Standard destructor
  //
}

//_____________________________________________________________________________
void AliGenTunedOnPbPb::Init()
{
  //
  // Initialise the generator
  //

  // define histos
}


//_____________________________________________________________________________
void AliGenTunedOnPbPb::Generate()
{
  //
  // Generate one trigger
  //

  Float_t avCentr = (fCmin+fCmax)*0.5;

  Float_t centrality = avCentr;

  if(fChangeWithCentrality) centrality = fCmin + gRandom->Rndm()*(fCmax-fCmin);

  SetParameters(centrality);

  TMCProcess statusPdg[fgNspecies] = {kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary,kPPrimary};

  Float_t parV2scaling[3] = {1,0.202538,-0.00214468};

  Float_t scaleV2 = 1.0;

  TDatabasePDG *pdgD = TDatabasePDG::Instance();

  if(fChangeWithCentrality){
    parV2scaling[0] = 1. / (1 + parV2scaling[1]*avCentr + parV2scaling[2]*avCentr*avCentr); // normalize to average centrality

    scaleV2 = parV2scaling[0]*(1 + parV2scaling[1]*centrality + parV2scaling[2]*centrality*centrality); // apply the trand of v2 w.r.t. centrality
  }

  fgV2->SetParameter(2,fgV3Overv2);

  Float_t psi = gRandom->Rndm()*TMath::Pi();
  fgEventplane = psi;
  Float_t psi3 = gRandom->Rndm()*TMath::Pi()*2/3;
  fgV2->SetParameter(1,psi);
  fgV2->SetParameter(3,psi3);

  Int_t npart = 0;

  Float_t origin0[3];
  for (Int_t j=0;j<3;j++) origin0[j]=fOrigin[j];
  Float_t time;
  time = fTimeOrigin;
  if(fVertexSmear == kPerEvent) {
    Vertex();
    for (Int_t j=0; j < 3; j++) origin0[j] = fVertex[j];
    time = fTime;
  } // if kPerEvent

  printf("Generate event with centrality = %3.1f%c, |y|<%4.1f\n",centrality,'%',fYMaxValue);
  
  for(Int_t isp=0;isp < fgNspecies;isp++){
    if(! fgHSpectrum[isp]) continue;
        
    printf("Total number of %i = %f\n",fgPdgInput[isp],fgMult[isp]*2*fYMaxValue);

    for(Int_t ipart =0; ipart < fgMult[isp]*2*fYMaxValue; ipart++){
      Int_t pdg = fgPdgInput[isp];
      
      Double_t y = gRandom->Rndm()*2*fYMaxValue - fYMaxValue;
      Double_t pt = fgHSpectrum[isp]->GetRandom();
      if(fgHv2[isp]) fgV2->SetParameter(0,fgHv2[isp]->Interpolate(pt) * scaleV2);
      else fgV2->SetParameter(0,0.);
      Double_t mass = pdgD->GetParticle(pdg)->Mass();
      Double_t mt2 = pt*pt + mass*mass;
      Double_t phi = fgV2->GetRandom(-TMath::Pi(),TMath::Pi());
      Double_t px = pt*TMath::Cos(phi);
      Double_t py = pt*TMath::Sin(phi);
      y = TMath::TanH(y);
      Double_t pz = y*TMath::Sqrt(mt2)/TMath::Sqrt(1-y*y);
//       Double_t etot = TMath::Sqrt(mt2 + pz*pz);
      Float_t p[3] = {px,py,pz};
      Float_t polar[3] = {0.,0.,0.};
      
      PushTrack(1, -1, pdg, p, origin0, polar, time, statusPdg[isp], npart, 1., 1);
      KeepTrack(npart);
      npart++;
    }   
  }

  TArrayF eventVertex;
  eventVertex.Set(3);
  eventVertex[0] = origin0[0];
  eventVertex[1] = origin0[1];
  eventVertex[2] = origin0[2];

// Header
    AliGenEventHeaderTunedPbPb* header = new AliGenEventHeaderTunedPbPb("tunedOnPbPb");
// Event Vertex
    header->SetPrimaryVertex(eventVertex);
    header->SetInteractionTime(time);
    header->SetNProduced(npart);
    header->SetCentrality(centrality);
    header->SetPsi2(psi);
    header->SetPsi3(psi3);
    gAlice->SetGenEventHeader(header); 
}

void AliGenTunedOnPbPb::SetPtRange(Float_t ptmin, Float_t ptmax) {
    AliGenerator::SetPtRange(ptmin, ptmax);
}
//_____________________________________________________________________________
TH1F *AliGenTunedOnPbPb::GetMultVsCentrality(Int_t species){
  char title[100];
  snprintf(title,100,"pdg = %i;centrality;dN/dy",fgPdgInput[species]);
  TH1F *h = new TH1F("multVsCentr",title,100,0,100);

  for(Int_t i=1;i<=100;i++){
    Float_t x = i+0.5;
    SetParameters(x);
    h->SetBinContent(i,fgMult[species]);
  }

  return h;
}
//_____________________________________________________________________________
void AliGenTunedOnPbPb::SetParameters(Float_t centrality){

  if(!fgV2) fgV2 = new TF1("fv2Par","(1 + 2*[0]*cos(2*(x-[1])) + 2*[0]*[2]*cos(3*(x-[3])))",-TMath::Pi(),TMath::Pi());

  Float_t fr[9] = {0.,0.,0.,0.,0.,0.,0.,0.,0.};

  if(centrality < 7.5){
    fr[0] = (7.5 - centrality)/5.;
    fr[1] = (centrality-2.5)/5.;
  }
  else if(centrality < 15){
    fr[1] = (15-centrality)/7.5;
    fr[2] = (centrality-7.5)/7.5;
  }
  else if(centrality < 25){
    fr[2] = (25-centrality)/10.;
    fr[3] = (centrality-15)/10.;
  }
  else if(centrality < 35){
    fr[3] = (35-centrality)/10.;
    fr[4] = (centrality-25)/10.;
  }
  else if(centrality < 45){
    fr[4] = (45-centrality)/10.;
    fr[5] = (centrality-35)/10.;
  }
  else if(centrality < 55){
    fr[5] = (55-centrality)/10.;
    fr[6] = (centrality-45)/10.;
  }
  else if(centrality < 65){
    fr[6] = (65-centrality)/10.;
    fr[7] = (centrality-55)/10.;
  }
  else if(centrality < 75){
    fr[7] = (75-centrality)/10.;
    fr[8] = (centrality-65)/10.;
  }
  else{
    fr[8] = 1.0;
  }

  // parameters as a function of centrality
  Float_t multCent[9*fgNspecies] = {680.,680.,680.,110.,110.,34.,34.,110.,28.,28.,11.5,3.1,3.1,0.5,0.5,
				    560.,560.,560.,92.,92.,28.,28.,92.,24.,24.,9.,2.7,2.7,0.45,0.45,
				    450.,450.,450.,70.,70.,21.,21.,70.,20.,20.,8.,2.4,2.4,0.4,0.4,
				    302.,302.,302.,47.,47.,14.5,14.5,47.,14.,14.,5.5,1.5,1.5,0.2,0.2,
				    200.,200.,200.,30.,30.,9.6,9.6,30.,9.,9.,3.5,0.9,0.9,0.08,0.08,
				    120.,120.,120.,18.,18.,6.1,6.1,18.,5.1,5.1,2.2,0.6,0.6,0.055,0.055,
				    70.,70.,70.,10.4,10.4,3.6,3.6,10.4,2.6,2.6,1.4,0.36,0.36,0.035,0.035,
				    36.,36.,36.,5.25,5.25,2.,2.,5.25,1.5,1.5,0.5,0.02,0.02,0.015,0.015,
				    17.,17.,17.,2.3,2.3,0.9,0.9,2.3,0.6,0.6,0.16,0.006,0.006,0.005,0.005};

  Float_t v3Overv2Cent[9] = {1.2,0.82,0.625,0.5,0.45,0.4,0.37,0.3,0.3};

  // set parameters for current centrality
  for(Int_t i=0;i < fgNspecies;i++){
    fgMult[i] = 0;

    for(Int_t j=0;j < 9;j++){
      fgMult[i] += fr[j]*multCent[i+j*fgNspecies];
    }
  }

  fgV3Overv2 = 0;
  for(Int_t j=0;j < 9;j++)
      fgV3Overv2 += fr[j]*v3Overv2Cent[j];
 
  if(centrality > 80){
    for(Int_t i=0;i < fgNspecies;i++)
      fgMult[i] /= TMath::Log(centrality-77.);
  }

}
