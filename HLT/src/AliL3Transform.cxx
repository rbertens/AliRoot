// $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>, Uli Frankenfeld <mailto:franken@fi.uib.no>
//*-- Copyright &copy ASV
// changes done by Constantin Loizides <mailto:loizides@ikf.physik.uni-frankfurt.de>

#include "AliL3StandardIncludes.h"

#ifdef use_aliroot
#include <AliRun.h>
#include <AliMagF.h>
#include <AliTPCParamSR.h>
#include <AliTPCPRF2D.h>
#include <AliTPCRF1D.h>
#include <TFile.h>
#include <TUnixSystem.h>
#include <TTimeStamp.h>
#endif

#include "AliL3Logging.h"
#include "AliL3Transform.h"

/** \class AliL3Transform 
//<pre>
//_____________________________________________________________
// AliL3Transform
//
// Transformation class for ALICE TPC.
//
// Class which contains all detector specific parameters for the TPC,
// and different useful functions for coordinate transforms.
//
// The class is completely static, which means that no object needs
// to be instantiated. Function calls should then be done like, e.g.:
//
// Double_t eta = AliL3Transform::GetEta(xyz);
//
// IMPORTANT: If used as is, default detector parameters will be used,
//            and you really have to make sure that these correspond to
//            the AliROOT version you are currently working on!!
//            You should therefore always initialize the parameters by
//
//            AliL3Transform::Init(path);
// 
//            where path is a char*, giving the path to where file containing
//            the detector parameter is located. This file should be called
//            "l3transform.config", and can be created with the function MakeInitFile.
//            
//            You can also force reading the parameters from a AliTPCParam object
//            by setting the flag;
//
//            AliL3Transform::Init(path,kTRUE);
//
//            where path is a char* 
//            either providing the rootfile containing the geometry or 
//            the path to the rootfile which  should then be called alirunfile.root. 
//            Note that for both cases you have to
//            compile with USEPACKAGE=ALIROOT set (see level3code/Makefile.conf).
//</pre>
*/

ClassImp(AliL3Transform)

// Defined by HLT group
Int_t AliL3Transform::fRows[6][2] = {{0,29},{30,62},{63,90},{91,116},{117,139},{140,158}}; //Defined by us and GSI
Int_t AliL3Transform::fNRows[6] = {30,33,28,26,23,19}; //Defined by us and GSI
Double_t AliL3Transform::fAnodeWireSpacing = 0.25; //Taken from the TDR

// The following definition is generated by Make_Init macro in exa
const Double_t AliL3Transform::fBFACT = 0.0029980;
Double_t AliL3Transform::fBField = 0.2;
Int_t AliL3Transform::fVersion = 0;
Int_t AliL3Transform::fNPatches = 6;
Int_t AliL3Transform::fBFieldFactor = 1 ;
Int_t AliL3Transform::fNTimeBins = 446 ;
Int_t AliL3Transform::fNRowLow = 63 ;
Int_t AliL3Transform::fNRowUp = 96 ;
Int_t AliL3Transform::fNRowUp1 = 64 ;
Int_t AliL3Transform::fNRowUp2 = 32 ;
Int_t AliL3Transform::fNSectorLow = 36 ;
Int_t AliL3Transform::fNSectorUp = 36 ;
Int_t AliL3Transform::fNSector = 72 ;
Double_t AliL3Transform::fPadPitchWidthLow = 0.400000 ;
Double_t AliL3Transform::fPadPitchWidthUp = 0.600000 ;
Double_t AliL3Transform::fZWidth = 0.56599998474121093750 ;
Double_t AliL3Transform::fZSigma = 0.22880849748219134199 ;
Double_t AliL3Transform::fZLength = 250.00000000000000000000 ;
Double_t AliL3Transform::fZOffset = 0.68642549244657402596 ;
Double_t AliL3Transform::fDiffT = 0.02199999988079071045 ;
Double_t AliL3Transform::fDiffL = 0.02199999988079071045 ;
Double_t AliL3Transform::fInnerPadLength = 0.750000 ;
Double_t AliL3Transform::fOuter1PadLength = 1.000000 ;
Double_t AliL3Transform::fOuter2PadLength = 1.500000 ;
Double_t AliL3Transform::fInnerPRFSigma = 0.20381128787994384766 ;
Double_t AliL3Transform::fOuter1PRFSigma = 0.29932481050491333008 ;
Double_t AliL3Transform::fOuter2PRFSigma = 0.29932320117950439453 ;
Double_t AliL3Transform::fTimeSigma = 0.22880862653255462646 ;
Int_t AliL3Transform::fNSlice = 36 ;
Int_t AliL3Transform::fNRow = 159 ;
Double_t AliL3Transform::fNRotShift = 0.5 ;
Double_t AliL3Transform::fPi = 3.141592653589793 ;
Double_t AliL3Transform::fX[159] = {85.194999694824219,
                                    85.944999694824219,
                                    86.694999694824219,
                                    87.444999694824219,
                                    88.194999694824219,
                                    88.944999694824219,
                                    89.694999694824219,
                                    90.444999694824219,
                                    91.194999694824219,
                                    91.944999694824219,
                                    92.694999694824219,
                                    93.444999694824219,
                                    94.194999694824219,
                                    94.944999694824219,
                                    95.694999694824219,
                                    96.444999694824219,
                                    97.194999694824219,
                                    97.944999694824219,
                                    98.694999694824219,
                                    99.444999694824219,
                                    100.194999694824219,
                                    100.944999694824219,
                                    101.694999694824219,
                                    102.444999694824219,
                                    103.194999694824219,
                                    103.944999694824219,
                                    104.694999694824219,
                                    105.444999694824219,
                                    106.194999694824219,
                                    106.944999694824219,
                                    107.694999694824219,
                                    108.444999694824219,
                                    109.194999694824219,
                                    109.944999694824219,
                                    110.694999694824219,
                                    111.444999694824219,
                                    112.194999694824219,
                                    112.944999694824219,
                                    113.694999694824219,
                                    114.444999694824219,
                                    115.194999694824219,
                                    115.944999694824219,
                                    116.694999694824219,
                                    117.444999694824219,
                                    118.194999694824219,
                                    118.944999694824219,
                                    119.694999694824219,
                                    120.444999694824219,
                                    121.194999694824219,
                                    121.944999694824219,
                                    122.694999694824219,
                                    123.444999694824219,
                                    124.194999694824219,
                                    124.944999694824219,
                                    125.694999694824219,
                                    126.444999694824219,
                                    127.194999694824219,
                                    127.944999694824219,
                                    128.695007324218750,
                                    129.445007324218750,
                                    130.195007324218750,
                                    130.945007324218750,
                                    131.695007324218750,
                                    135.180007934570312,
                                    136.180007934570312,
                                    137.180007934570312,
                                    138.180007934570312,
                                    139.180007934570312,
                                    140.180007934570312,
                                    141.180007934570312,
                                    142.180007934570312,
                                    143.180007934570312,
                                    144.180007934570312,
                                    145.180007934570312,
                                    146.180007934570312,
                                    147.180007934570312,
                                    148.180007934570312,
                                    149.180007934570312,
                                    150.180007934570312,
                                    151.180007934570312,
                                    152.180007934570312,
                                    153.180007934570312,
                                    154.180007934570312,
                                    155.180007934570312,
                                    156.180007934570312,
                                    157.180007934570312,
                                    158.180007934570312,
                                    159.180007934570312,
                                    160.180007934570312,
                                    161.180007934570312,
                                    162.180007934570312,
                                    163.180007934570312,
                                    164.180007934570312,
                                    165.180007934570312,
                                    166.180007934570312,
                                    167.180007934570312,
                                    168.180007934570312,
                                    169.180007934570312,
                                    170.180007934570312,
                                    171.180007934570312,
                                    172.180007934570312,
                                    173.180007934570312,
                                    174.180007934570312,
                                    175.180007934570312,
                                    176.180007934570312,
                                    177.180007934570312,
                                    178.180007934570312,
                                    179.180007934570312,
                                    180.180007934570312,
                                    181.180007934570312,
                                    182.180007934570312,
                                    183.180007934570312,
                                    184.180007934570312,
                                    185.180007934570312,
                                    186.180007934570312,
                                    187.180007934570312,
                                    188.180007934570312,
                                    189.180007934570312,
                                    190.180007934570312,
                                    191.180007934570312,
                                    192.180007934570312,
                                    193.180007934570312,
                                    194.180007934570312,
                                    195.180007934570312,
                                    196.180007934570312,
                                    197.180007934570312,
                                    198.180007934570312,
                                    199.430007934570312,
                                    200.930007934570312,
                                    202.430007934570312,
                                    203.930007934570312,
                                    205.430007934570312,
                                    206.930007934570312,
                                    208.430007934570312,
                                    209.930007934570312,
                                    211.430007934570312,
                                    212.930007934570312,
                                    214.430007934570312,
                                    215.930007934570312,
                                    217.430007934570312,
                                    218.930007934570312,
                                    220.430007934570312,
                                    221.930007934570312,
                                    223.430007934570312,
                                    224.930007934570312,
                                    226.430007934570312,
                                    227.930007934570312,
                                    229.430007934570312,
                                    230.930007934570312,
                                    232.430007934570312,
                                    233.930007934570312,
                                    235.430007934570312,
                                    236.930007934570312,
                                    238.430007934570312,
                                    239.930007934570312,
                                    241.430007934570312,
                                    242.930007934570312,
                                    244.430007934570312,
                                    245.930007934570312
};

Int_t AliL3Transform::fNPads[159] = {67,
                                     67,
                                     69,
                                     69,
                                     69,
                                     71,
                                     71,
                                     71,
                                     73,
                                     73,
                                     73,
                                     75,
                                     75,
                                     75,
                                     77,
                                     77,
                                     77,
                                     79,
                                     79,
                                     79,
                                     81,
                                     81,
                                     81,
                                     83,
                                     83,
                                     83,
                                     85,
                                     85,
                                     85,
                                     87,
                                     87,
                                     87,
                                     89,
                                     89,
                                     89,
                                     91,
                                     91,
                                     91,
                                     93,
                                     93,
                                     93,
                                     95,
                                     95,
                                     95,
                                     97,
                                     97,
                                     97,
                                     99,
                                     99,
                                     99,
                                     99,
                                     101,
                                     101,
                                     101,
                                     103,
                                     103,
                                     103,
                                     105,
                                     105,
                                     105,
                                     107,
                                     107,
                                     107,
                                     73,
                                     75,
                                     75,
                                     75,
                                     75,
                                     77,
                                     77,
                                     77,
                                     79,
                                     79,
                                     79,
                                     81,
                                     81,
                                     81,
                                     81,
                                     83,
                                     83,
                                     83,
                                     85,
                                     85,
                                     85,
                                     85,
                                     87,
                                     87,
                                     87,
                                     89,
                                     89,
                                     89,
                                     91,
                                     91,
                                     91,
                                     91,
                                     93,
                                     93,
                                     93,
                                     95,
                                     95,
                                     95,
                                     95,
                                     97,
                                     97,
                                     97,
                                     99,
                                     99,
                                     99,
                                     101,
                                     101,
                                     101,
                                     101,
                                     103,
                                     103,
                                     103,
                                     105,
                                     105,
                                     105,
                                     105,
                                     107,
                                     107,
                                     107,
                                     109,
                                     109,
                                     109,
                                     111,
                                     111,
                                     111,
                                     113,
                                     113,
                                     113,
                                     115,
                                     115,
                                     117,
                                     117,
                                     119,
                                     119,
                                     121,
                                     121,
                                     121,
                                     123,
                                     123,
                                     125,
                                     125,
                                     127,
                                     127,
                                     127,
                                     129,
                                     129,
                                     131,
                                     131,
                                     133,
                                     133,
                                     135,
                                     135,
                                     135,
                                     137,
                                     137,
                                     139
};

Bool_t AliL3Transform::Init(Char_t* path,Bool_t UseAliTPCParam)
{
  //Overwrite the parameters with values stored in file "l3transform.config" in path.
  //If file does not exist, old default values will be used.
  //If flag UseAliTPCParam is set, the parameters will be read from the the rootfile
  //which then has to be called path/digitfile.root
  
  if(fVersion != 0)
    LOG(AliL3Log::kWarning,"AliL3Transform::Init","Init values")
      <<"You are initializing the parameters more than once; check your code please! "<<fVersion<<ENDLOG;
  
  if(UseAliTPCParam)
    {
      return ReadInit(path);
    }

  Char_t *pathname=new Char_t[1024];
  strcpy(pathname,path);
  strcat(pathname,"/l3transform.config");
  
  FILE *fptr=fopen(pathname,"r");
  if(!fptr){
    LOG(AliL3Log::kWarning,"AliL3Transform::Init","File Open")
      <<"Pointer to Config File \""<<pathname<<"\" 0x0!"<<ENDLOG;
    return kFALSE;
  }

  Char_t d1[250], d2[100], d3[100];
  Int_t dummy=0;
  Double_t ddummy=0.0;

  while(!feof(fptr)) {
    fscanf(fptr,"%s",d1);

    if(strcmp(d1,"fBFieldFactor")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fBFieldFactor=(Int_t)dummy;fBField=fBFieldFactor*0.2;}
    else if(strcmp(d1,"fNTimeBins")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNTimeBins=(Int_t)dummy;}
    else if(strcmp(d1,"fNRowLow")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNRowLow=(Int_t)dummy;}    
    if(fNRowLow != 63)
      LOG(AliL3Log::kError,"AliL3Transform::Init","Overflow")
	<<"Number of inner PadRows should be 63! Check and fgrep the code for 63 to see the consequences of this major change!"<<ENDLOG;
    else if(strcmp(d1,"fNRowUp")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNRowUp=(Int_t)dummy;}
    else if(strcmp(d1,"fNRowUp1")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNRowUp1=(Int_t)dummy;}
    else if(strcmp(d1,"fNRowUp2")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNRowUp2=(Int_t)dummy;}
    else if(strcmp(d1,"fNSectorLow")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNSectorLow=(Int_t)dummy;}
    else if(strcmp(d1,"fNSectorUp")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNSectorUp=(Int_t)dummy;}
    else if(strcmp(d1,"fNSector")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNSector=(Int_t)dummy;}
    else if(strcmp(d1,"fPadPitchWidthLow")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fPadPitchWidthLow=(Double_t)ddummy;}
    else if(strcmp(d1,"fPadPitchWidthUp")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fPadPitchWidthUp=(Double_t)ddummy;}
    else if(strcmp(d1,"fZWidth")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fZWidth=(Double_t)ddummy;}
    else if(strcmp(d1,"fZSigma")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fZSigma=(Double_t)ddummy;}
    else if(strcmp(d1,"fZLength")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fZLength=(Double_t)ddummy;}
    else if(strcmp(d1,"fZOffset")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fZOffset=(Double_t)ddummy;}
    else if(strcmp(d1,"fNSlice")==0){fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNSlice=(Int_t)dummy;}
    else if(strcmp(d1,"fDiffT")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fDiffT=(Double_t)ddummy;}
    else if(strcmp(d1,"fDiffL")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fDiffL=(Double_t)ddummy;}
    else if(strcmp(d1,"fInnerPadLength")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fInnerPadLength=(Double_t)ddummy;}
    else if(strcmp(d1,"fOuter1PadLength")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fOuter1PadLength=(Double_t)ddummy;}
    else if(strcmp(d1,"fOuter2PadLength")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fOuter2PadLength=(Double_t)ddummy;}
    else if(strcmp(d1,"fInnerPRFSigma")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fInnerPRFSigma=(Double_t)ddummy;}
    else if(strcmp(d1,"fOuter1PRFSigma")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fOuter1PRFSigma=(Double_t)ddummy;}
    else if(strcmp(d1,"fOuter2PRFSigma")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fOuter2PRFSigma=(Double_t)ddummy;}
    else if(strcmp(d1,"fTimeSigma")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fTimeSigma=(Double_t)ddummy;}
    else if(strcmp(d1,"fNRow")==0){
      fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNRow=(Int_t)dummy;
      if(fNRow!=159){
	LOG(AliL3Log::kError,"AliL3Transform::Init","Overflow")<<"Number of PadRows should be 159! Check and fgrep the code for 159 to see the consequences of this major change!"<<ENDLOG;
      }
    }
    else if(strcmp(d1,"fNRotShift")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fNRotShift=(Double_t)ddummy;}
    else if(strcmp(d1,"fPi")==0){fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fPi=(Double_t)ddummy;}
    else if(strcmp(d1,"fX[0]")==0){
      fscanf(fptr,"%s %lf %s",d2,&ddummy,d3);fX[0]=(Double_t)ddummy;
      for(Int_t i=1;i<fNRow;i++){fscanf(fptr,"%s %s %lf %s",d1,d2,&ddummy,d3);fX[i]=(Double_t)ddummy;}
    }
    else if(strcmp(d1,"fNPads[0]")==0){
      fscanf(fptr,"%s %d %s",d2,&dummy,d3);fNPads[0]=(Int_t)dummy;
      for(Int_t i=1;i<fNRow;i++){fscanf(fptr,"%s %s %d %s",d1,d2,&dummy,d3);fNPads[i]=(Int_t)dummy;}
    }
  }

  fclose(fptr);
  delete pathname;
  fVersion++; //new version

  return kTRUE;
}

Bool_t AliL3Transform::ReadInit(Char_t *path)
{
  //Read all the parameters from a aliroot file, and store it in a temporary 
  //file which is read by Init. Use this if you want to read the parameters from
  //the rootfile "every" time.
  
#ifndef use_aliroot
  LOG(AliL3Log::kError,"AliL3Transform::ReadInit","Version")
    <<"You have to compile with use_aliroot flag in order to read from AliROOT file"<<ENDLOG;
  return kFALSE;
#else
  Char_t filename[1024];
  //first test whether provided path is the rootfile itself
  TFile *rootfile = TFile::Open(path);
  if(!rootfile) //ok assume its path to file
    {
      sprintf(filename,"%s/alirunfile.root",path); //create rootfile name
    } else {
      rootfile->Close();
      sprintf(filename,"%s",path); //path contains itself the rootfile name
    }
  //finally make dummy init file /tmp/l3transform.config
  if(MakeInitFile(filename,"/tmp/"))
    { 
      Bool_t ret=Init("/tmp/");
      //Move the temp file to /tmp/l3transform.config-"time in seconds"
      TUnixSystem sys;
      TTimeStamp time;
      sprintf(filename,"/tmp/l3transform.config-%ld",(long)time.GetSec()); 
      sys.Rename("/tmp/l3transform.config",filename);
      return ret;
    }

  return kFALSE;
#endif  
}

Bool_t AliL3Transform::MakeInitFile(Char_t *filename,Char_t *path)
{
  //Get the parameters from rootfile, and store it on the file "l3transform.config"
  //which is being read by Init.
  
#ifndef use_aliroot
  LOG(AliL3Log::kError,"AliL3Transform::MakeInitFile","Version")
    <<"You have to compile with use_aliroot flag in order to use this function"<<ENDLOG;
  return kFALSE;
#else
  TFile *rootfile = TFile::Open(filename);
  if(!rootfile)
    {
      LOG(AliL3Log::kError,"AliL3Transform::MakeInitFile","File")
	<<"Could not open file: "<<filename<<ENDLOG;
      return kFALSE;
    }
  AliRun *gAlice = (AliRun*)rootfile->Get("gAlice");
  if(!gAlice)
    {
      LOG(AliL3Log::kError,"AliL3Transform::MakeInitFile","File")
	<<"No gAlice in file: "<<filename<<ENDLOG;
      return kFALSE;
    }  
  AliTPCParamSR *param=(AliTPCParamSR*)rootfile->Get(GetParamName());
  if(!param)
    {
      LOG(AliL3Log::kError,"AliL3Transform::MakeInitFile","File")
	<<"No TPC parameters found"<<ENDLOG;
      return kFALSE;
    }
  
  AliTPCPRF2D    * prfinner   = new AliTPCPRF2D;
  AliTPCPRF2D    * prfouter1   = new AliTPCPRF2D;
  AliTPCPRF2D    * prfouter2   = new AliTPCPRF2D;  
  AliTPCRF1D     * rf    = new AliTPCRF1D(kTRUE);
  rf->SetGauss(param->GetZSigma(),param->GetZWidth(),1.);
  rf->SetOffset(3*param->GetZSigma());
  rf->Update();
  
  TDirectory *savedir=gDirectory;
  TFile *prf_file = TFile::Open("$ALICE_ROOT/TPC/AliTPCprf2d.root");
  if (!prf_file->IsOpen()) 
    { 
      LOG(AliL3Log::kError,"AliL3Transform::MakeInitFile","File")
	<<"Can't open $ALICE_ROOT/TPC/AliTPCprf2d.root !"<<ENDLOG;
      return kFALSE;
    }
  prfinner->Read("prf_07504_Gati_056068_d02");
  prfouter1->Read("prf_10006_Gati_047051_d03");
  prfouter2->Read("prf_15006_Gati_047051_d03");  
  prf_file->Close();
  savedir->cd();
  
  param->SetInnerPRF(prfinner);
  param->SetOuter1PRF(prfouter1); 
  param->SetOuter2PRF(prfouter2);
  param->SetTimeRF(rf);
  
  Int_t nTimeBins = param->GetMaxTBin()+1;
  Int_t nRowLow = param->GetNRowLow();
  Int_t nRowUp  = param->GetNRowUp();
  Int_t nRowUp1 = param->GetNRowUp1();
  Int_t nRowUp2 = param->GetNRowUp2();
  Int_t nRow= fNRowLow + fNRowUp;
  Int_t nSectorLow = param->GetNInnerSector();
  Int_t nSectorUp = param->GetNOuterSector();
  Int_t nSector = fNSectorLow + fNSectorUp;
    
  Char_t tofile[100];
  sprintf(tofile,"%s/l3transform.config",path);
  FILE *f = fopen(tofile,"w");
  fprintf(f,"void AliL3Transform::Init(){\n");

  fprintf(f,"  fBFieldFactor = %d ;\n",(Int_t)gAlice->Field()->Factor());
  fprintf(f,"  //sector:\n");
  fprintf(f,"  fNTimeBins = %d ;\n",nTimeBins);
  fprintf(f,"  fNRowLow = %d ;\n",nRowLow);
  fprintf(f,"  fNRowUp = %d ;\n",nRowUp);
  fprintf(f,"  fNRowUp1 = %d ;\n",nRowUp1);
  fprintf(f,"  fNRowUp2 = %d ;\n",nRowUp2);
  fprintf(f,"  fNSectorLow = %d ;\n",nSectorLow);
  fprintf(f,"  fNSectorUp = %d ;\n",nSectorUp);
  fprintf(f,"  fNSector = %d ;\n",nSector);
  fprintf(f,"  fPadPitchWidthLow = %f ;\n",param->GetInnerPadPitchWidth());
  fprintf(f,"  fPadPitchWidthUp = %f ;\n",param->GetOuterPadPitchWidth());
  fprintf(f,"  fZWidth = %.20f ;\n",param->GetZWidth());
  fprintf(f,"  fZSigma = %.20f ;\n",param->GetZSigma());
  fprintf(f,"  fZLength = %.20f ;\n",param->GetZLength());
  fprintf(f,"  fZOffset = %.20f ;\n",param->GetZOffset());
  fprintf(f,"  fDiffT = %.20f ;\n",param->GetDiffT());
  fprintf(f,"  fDiffL = %.20f ;\n",param->GetDiffL());
  fprintf(f,"  fInnerPadLength = %f ;\n",param->GetInnerPadLength());
  fprintf(f,"  fOuter1PadLength = %f ;\n",param->GetOuter1PadLength());
  fprintf(f,"  fOuter2PadLength = %f ;\n",param->GetOuter2PadLength());
  fprintf(f,"  fInnerPRFSigma = %.20f ;\n",param->GetInnerPRF()->GetSigmaX());
  fprintf(f,"  fOuter1PRFSigma = %.20f ;\n",param->GetOuter1PRF()->GetSigmaX());
  fprintf(f,"  fOuter2PRFSigma = %.20f ;\n",param->GetOuter2PRF()->GetSigmaX());
  
  fprintf(f,"  fTimeSigma = %.20f ;\n",param->GetTimeRF()->GetSigma());
  
  fprintf(f,"\n  //slices:\n");
  fprintf(f,"  fNSlice = %d ;\n",nSectorLow);
  fprintf(f,"  fNRow = %d ;\n",nRow);

  //rotation shift put in by hand -> Constantin 
  fprintf(f,"  fNRotShift = 0.5 ;\n");

  fprintf(f,"  fPi = %.15f ;\n",TMath::Pi());
  
  for(Int_t i=0;i<nRow;i++){
    Int_t sec,row;
    if( i < nRowLow){sec =0;row =i;}
    else{sec = nSectorLow;row =i-nRowLow;}
    fprintf(f,"  fX[%d] = %3.15f ;\n",i,param->GetPadRowRadii(sec,row));
  }
  for(Int_t i=0;i<nRow;i++){
    Int_t sec,row;
    if( i < nRowLow){sec =0;row =i;}
    else{sec = nSectorLow;row =i-nRowLow;}
    fprintf(f,"  fNPads[%d] = %d ;\n",i,param->GetNPads(sec,row));
  }
  
  fprintf(f,"}\n");
  fclose(f);

  return kTRUE;
#endif
}

Double_t AliL3Transform::GetPadLength(Int_t padrow)
{
  if(padrow >= fNRow)
    return 0;
  if(padrow < fNRowLow)
    return fInnerPadLength;
  if(padrow >= fNRowLow && padrow < fNRowLow + fNRowUp1 - 1)
    return fOuter1PadLength;
  if(padrow >= fNRowLow + fNRowUp1 - 1)
    return fOuter2PadLength;

  return -1.0; //should never happen
}

Double_t AliL3Transform::GetPRFSigma(Int_t padrow)
{
  if(padrow >= fNRow)
    return 0;
  if(padrow < fNRowLow)
    return fInnerPRFSigma;
  if(padrow >= fNRowLow && padrow < fNRowLow + fNRowUp1 - 1)
    return fOuter1PRFSigma;
  if(padrow >= fNRowLow + fNRowUp1 - 1)
    return fOuter2PRFSigma;

  return -1.; //should never happen
}

Double_t AliL3Transform::GetEta(Float_t *xyz)
{
  Double_t r3 = sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2]);
  Double_t eta = 0.5 * log((r3+xyz[2])/(r3-xyz[2]));
  return eta;
}

void AliL3Transform::XYZtoRPhiEta(Float_t *rpe, Float_t *xyz)
{
  rpe[0] = sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2]);
  rpe[1] = atan2(xyz[1],xyz[0]);
  rpe[2] = 0.5 * log((rpe[0]+xyz[2])/(rpe[0]-xyz[2]));
}

Double_t AliL3Transform::GetEta(Int_t padrow,Int_t pad,Int_t time)
{
  Float_t xyz[3];
  Int_t sector,row;
  Slice2Sector(0,padrow,sector,row);
  Raw2Local(xyz,sector,row,pad,time);
  
  return GetEta(xyz);
}

Double_t AliL3Transform::GetPhi(Float_t *xyz)
{
  Double_t phi = atan2(xyz[1],xyz[0]);
  //if(phi<0) phi=phi+2*TMath::Pi();
  return phi;
}

Bool_t AliL3Transform::Slice2Sector(Int_t slice, Int_t slicerow, Int_t & sector, Int_t &row)
{
  if(slicerow<0&&slicerow>=fNRow) return kFALSE;
  if(slice<0||slice>=fNSlice) return kFALSE;

  if(slicerow<fNRowLow){
    sector = slice;
    row    = slicerow;
  }
  else {
    sector = slice+fNSlice;
    row    = slicerow-fNRowLow;
  }
  return kTRUE;
}

Bool_t AliL3Transform::Sector2Slice(Int_t & slice, Int_t  sector)
{
  if(sector<0||sector>=fNSector) return kFALSE;
  if(sector<fNSectorLow) slice = sector;
  else          slice = sector - fNSectorLow;
  return kTRUE;
}

Bool_t AliL3Transform::Sector2Slice(Int_t & slice, Int_t & slicerow,Int_t  sector, Int_t row)
{
  if(sector<0||sector>=fNSector||row<0) return kFALSE;
  if(sector<fNSectorLow){
    if(row>=fNRowLow) return kFALSE;
    slice = sector;
    slicerow = row;
  }
  else{
    if(row>=fNRowUp) return kFALSE;
    slice = sector - fNSectorLow;
    slicerow = row + fNRowLow;
  }
  return kTRUE;
}

Double_t AliL3Transform::Row2X(Int_t slicerow){
  if(slicerow<0||slicerow>=fNRow) return 0;
  return fX[slicerow];
}

void AliL3Transform::Local2Global(Float_t *xyz,Int_t slice)
{
  //Transformation to global coordinate system
  Float_t x0 = xyz[0];
  Float_t y0 = xyz[1];
  Float_t cs,sn;
  cs = cos( (2*fPi/18) * (slice+fNRotShift) );
  sn = sin( (2*fPi/18) * (slice+fNRotShift) );
  xyz[0]=x0*cs-y0*sn;
  xyz[1]=x0*sn+y0*cs;
  xyz[2]=xyz[2];//global z=local z
}

void AliL3Transform::Local2GlobalAngle(Float_t *angle,Int_t slice){
  angle[0] = fmod(angle[0]+(slice+fNRotShift)*(2*fPi/18),2*fPi);
}

void AliL3Transform::Global2LocalAngle(Float_t *angle,Int_t slice){
  angle[0] = angle[0]-(slice+fNRotShift)*(2*fPi/18);
  if(angle[0]<0) angle[0]+=2*fPi;
}

void AliL3Transform::Raw2Local(Float_t *xyz,Int_t sector,Int_t row,Float_t pad,Float_t time)
{
  //Transformation from rawdata to local coordinate system
  
  Int_t slice,slicerow;
  Sector2Slice(slice, slicerow, sector, row);  

  //X-Value
  xyz[0]=Row2X(slicerow); 

  //Y-Value
  Int_t npads= fNPads[slicerow];
  if(sector<fNSectorLow)
    xyz[1]=(pad-0.5*(npads-1))*fPadPitchWidthLow;
  else
    xyz[1]=(pad-0.5*(npads-1))*fPadPitchWidthUp;

  //Z-Value (remember PULSA Delay)
  //xyz[2]=fZWidth*time-3.*fZSigma;
  xyz[2]=fZWidth*time-fZOffset;
  if(slice < 18)
    xyz[2]=fZLength-xyz[2];
  else
    xyz[2]=xyz[2]-fZLength;
  
}

void AliL3Transform::Local2Global(Float_t *xyz,Int_t sector,Int_t row)
{
  //Transformation to global coordinate system
  Int_t slice,slicerow;
  Sector2Slice(slice, slicerow, sector, row);  
  Float_t r=Row2X(slicerow);
  Float_t cs = cos( (2*fPi/18) * (slice+fNRotShift) );
  Float_t sn = sin( (2*fPi/18) * (slice+fNRotShift) );

  xyz[0]=r*cs-xyz[1]*sn;
  xyz[1]=r*sn+xyz[1]*cs;
  xyz[2]=xyz[2];//global z=local z
}

Double_t AliL3Transform::GetMaxY(Int_t slicerow)
{

 if(slicerow < fNRowLow)
     return fPadPitchWidthLow*fNPads[slicerow]/2; 
 
 else
     return fPadPitchWidthUp*fNPads[slicerow]/2;

}

void AliL3Transform::Global2Local(Float_t *xyz,Int_t sector,Bool_t isSlice)
{
  
  Int_t slice;
  if(!isSlice)
    Sector2Slice(slice, sector);  
  else
    slice = sector;
  Float_t cs = cos( (2*fPi/18) * (slice+fNRotShift) );
  Float_t sn = sin( (2*fPi/18) * (slice+fNRotShift) );
  Float_t x1 = xyz[0]*cs + xyz[1]*sn;
  Float_t y1 = -xyz[0]*sn + xyz[1]*cs;
  xyz[0] = x1;
  xyz[1] = y1;
}

void AliL3Transform::Raw2Global(Float_t *xyz,Int_t sector,Int_t row,Float_t pad,Float_t time)
{
  //Transformation from raw to global coordinates
  
  Raw2Local(xyz,sector,row,pad,time);
  Local2Global(xyz,sector,row);
}

void AliL3Transform::Local2Raw(Float_t *xyz,Int_t sector,Int_t row)
{
  //Transformation from local coordinates to raw

  Int_t slice,slicerow;
  Sector2Slice(slice, slicerow, sector, row);  
   
  if(sector<fNSectorLow)
    xyz[1]=xyz[1]/fPadPitchWidthLow+0.5*(fNPads[slicerow]-1);
  else
    xyz[1]=xyz[1]/fPadPitchWidthUp+0.5*(fNPads[slicerow]-1);
  Int_t sign=-1;
  Int_t nis=fNSectorLow;
  Int_t nos=fNSectorUp;
 
  if ((sector<nis/2) || ((sector-nis)<nos/2)) sign=1; 
  xyz[2]=fZLength-sign*xyz[2];
  xyz[2]=(xyz[2]+fZOffset)/fZWidth;

}

void AliL3Transform::Global2Raw(Float_t *xyz,Int_t sector,Int_t row)
{
  //Transformation from global coordinates to raw. 

  Global2Local(xyz,sector);
  Local2Raw(xyz,sector,row);

}
