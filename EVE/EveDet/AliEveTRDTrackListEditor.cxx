// Author: Benjamin Hess   25/09/2008

/*************************************************************************
 * Copyright (C) 2008, Alexandru Bercuci, Benjamin Hess.                 *
 * All rights reserved.                                                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AliEveTRDTrackListEditor                                             //
//                                                                      //
// The AliEveTRDTrackListEditor provides the graphical functionality    //
// for the AliEveTRDTrackList. It creates the tabs and canvases, when   //
// they are needed and, as well, frees allocated memory on destruction  //
// (or if new events are loaded and thus some tabs are closed).         //
// The function DrawHistos() accesses the temporary file created by the //
// AliEveTRDTrackList and draws the desired data (the file will be      //
// created within the call of ApplyMacros()). Have a look at this       //
// function to learn more about the structure of the file and how to    //
// access the data.                                                     //
//////////////////////////////////////////////////////////////////////////

#include <EveDet/AliEveTRDData.h>
#include <EveDet/AliEveTRDTrackList.h>
#include "AliEveTRDTrackListEditor.h"

#include <EveBase/AliEveEventManager.h>
#include <AliTRDReconstructor.h>
#include <AliTRDtrackV1.h>
#include <TGButton.h>
#include <TCanvas.h>     
#include <TEveBrowser.h>
#include <TEveGedEditor.h> 
#include <TEveMacro.h>
#include <TEveManager.h>
#include <TFile.h>
#include <TG3DLine.h>
#include <TGButtonGroup.h>
#include <TGFileDialog.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TGMsgBox.h>
#include <TGTab.h>
#include <TMap.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TGTextEntry.h>
#include <TGTextView.h>
#include <TH1.h>
#include <TTreeStream.h>


ClassImp(AliEveTRDTrackListEditor)

///////////////////////////////////////////////////////////
/////////////   AliEveTRDTrackListEditor //////////////////
///////////////////////////////////////////////////////////
AliEveTRDTrackListEditor::AliEveTRDTrackListEditor(const TGWindow* p, Int_t width, Int_t height,
				                   UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options, back),
  fM(0),
  fHistoCanvas(0),
  fHistoCanvasName(0),
  fInheritedMacroList(0),
  fInheritSettings(kFALSE),
  fStyleFrame(0),
  fMainFrame(0),
  fHistoFrame(0),
  fHistoSubFrame(0),
  fBrowseFrame(0),
  fbgStyleColor(0),
  fbgStyleTrack(0),
  frbColor(new TGRadioButton*[3]),
  frbTrack(new TGRadioButton*[3]),
  fbBrowse(0),
  fbApplyMacros(0),
  fbRemoveMacros(0),
  fbDrawHisto(0),
  fteField(0),
  ftlMacroList(0),
  ftlMacroSelList(0),
  fFileInfo(0),
  fFileTypes(0),
  fLabel1(0), fLabel2(0), fLabel3(0), fLabel4(0),
  fLine1(0), fLine2(0), fLine3(0), fLine4(0), fLine5(0),
  fCheckButtons(0)
{
  // Creates the AliEveTRDTrackListEditor.

  // Style stuff
  fLine5 = new TGHorizontal3DLine(this, 194, 8);
  AddFrame(fLine5, new TGLayoutHints(kLHintsLeft  | kLHintsTop, 2, 2, 8, 8));
  fStyleFrame = new TGHorizontalFrame(this);
  AddFrame(fStyleFrame);

  // Style - Track model
  fbgStyleTrack = new TGButtonGroup(fStyleFrame, "Track model");
  fbgStyleTrack->SetMapSubwindows(kTRUE);
  fbgStyleTrack->Resize(194, 200);
  fStyleFrame->AddFrame(fbgStyleTrack);

  frbTrack[0] = new TGRadioButton(fbgStyleTrack, "Rieman", 0);
  frbTrack[0]->SetToolTipText("Set the track model to \"Rieman\" (i.e. the used fit method)");
  fbgStyleTrack->AddFrame(frbTrack[0]);
  frbTrack[1] = new TGRadioButton(fbgStyleTrack, "Kalman", 1);
  frbTrack[1]->SetToolTipText("Set the track model to \"Kalman\" (i.e. the used fit method)");
  fbgStyleTrack->AddFrame(frbTrack[1]);
  frbTrack[2] = new TGRadioButton(fbgStyleTrack, "Line", 2);
  frbTrack[2]->SetToolTipText("Set the track model to \"Line\" (i.e. the used fit method)");
  fbgStyleTrack->AddFrame(frbTrack[2]);  

  // Style - Color model
  fbgStyleColor = new TGButtonGroup(fStyleFrame, "Color model");
  fbgStyleColor->SetMapSubwindows(kTRUE);
  fbgStyleColor->Resize(194, 200);
  fStyleFrame->AddFrame(fbgStyleColor);

  frbColor[0] = new TGRadioButton(fbgStyleColor, "PID LQ", 0);
  frbColor[0]->SetToolTipText("Set color model to \"PID LQ\" -> 2 dimensional likelihood particle identification");
  fbgStyleColor->AddFrame(frbColor[0]);
  frbColor[1] = new TGRadioButton(fbgStyleColor, "PID NN", 1);
  frbColor[1]->SetToolTipText("Set color model to \"PID NN\" -> Neural network particle identification");
  fbgStyleColor->AddFrame(frbColor[1]);
  frbColor[2] = new TGRadioButton(fbgStyleColor, "ESD Source", 2);
  frbColor[2]->SetToolTipText("Set color model to \"ESD Source\" -> By source (TPC track prolongation or TRD stand alone)");
  fbgStyleColor->AddFrame(frbColor[2]);  
  

  // Functionality for adding macros  
  fMainFrame = CreateEditorTabSubFrame("Apply macros");
   
  fLabel1 = new TGLabel(fMainFrame,"Add macro(s):");
  fMainFrame->AddFrame(fLabel1);
  fBrowseFrame = new TGHorizontalFrame(fMainFrame);

  fteField = new TGTextEntry(fBrowseFrame);
  fteField->SetToolTipText("Enter the pathname of the macro you want to add here and press \"Enter\"");
  fteField->Connect("ReturnPressed()","AliEveTRDTrackListEditor", this, "HandleMacroPathSet()"); 
  fBrowseFrame->AddFrame(fteField);
  
  fbBrowse = new TGTextButton(fBrowseFrame, "Browse");
  fbBrowse->SetToolTipText("Browse the macro you want to add");
  fbBrowse->Connect("Clicked()", "AliEveTRDTrackListEditor", this, "BrowseMacros()");
  fBrowseFrame->AddFrame(fbBrowse);
  fMainFrame->AddFrame(fBrowseFrame);

  fLine1 = new TGHorizontal3DLine(fMainFrame, 194, 8);
  fMainFrame->AddFrame(fLine1, new TGLayoutHints(kLHintsLeft  | kLHintsTop, 2, 2, 8, 2));
  fLabel2 = new TGLabel(fMainFrame,"Selection macros:");
  fMainFrame->AddFrame(fLabel2);

  ftlMacroSelList = new TGListBox(fMainFrame);
  ftlMacroSelList->Resize(194, 94);
  ftlMacroSelList->SetMultipleSelections(kTRUE);
  fMainFrame->AddFrame(ftlMacroSelList);

  fLine2 = new TGHorizontal3DLine(fMainFrame, 194, 8);
  fMainFrame->AddFrame(fLine2, new TGLayoutHints(kLHintsLeft  | kLHintsTop, 2, 2, 8, 2));
  fLabel3 = new TGLabel(fMainFrame,"Process macros:");
  fMainFrame->AddFrame(fLabel3);

  ftlMacroList = new TGListBox(fMainFrame);
  ftlMacroList->Resize(194, 94);
  ftlMacroList->SetMultipleSelections(kTRUE);
  fMainFrame->AddFrame(ftlMacroList);

  fLine3 = new TGHorizontal3DLine(fMainFrame, 194, 8);
  fMainFrame->AddFrame(fLine3, new TGLayoutHints(kLHintsLeft  | kLHintsTop, 2, 2, 8, 2));  

  fbApplyMacros = new TGTextButton(fMainFrame, "Apply selected macro(s)");
  fbApplyMacros->SetToolTipText("Apply all selected macros to the tracklist -> A data file will be generated");
  fbApplyMacros->Connect("Clicked()", "AliEveTRDTrackListEditor", this, "ApplyMacros()");
  fbApplyMacros->SetRightMargin(12);
  fMainFrame->AddFrame(fbApplyMacros);

  fbRemoveMacros = new TGTextButton(fMainFrame, "Remove selected macro(s)");
  fbRemoveMacros->SetToolTipText("Remove the selected macro(s) from the list(s)");
  fbRemoveMacros->Connect("Clicked()", "AliEveTRDTrackListEditor", this, "RemoveMacros()");
  fMainFrame->AddFrame(fbRemoveMacros);

  // Stuff for displaying histograms
  fHistoFrame = CreateEditorTabSubFrame("Histograms");  
  fHistoFrame->SetMapSubwindows(kTRUE);
  fLabel4 = new TGLabel(fHistoFrame,"Data from applied macros:");
  fHistoFrame->AddFrame(fLabel4);

  fHistoSubFrame = new TGVerticalFrame(fHistoFrame);
  fHistoSubFrame->SetMapSubwindows(kTRUE);
  fHistoSubFrame->Resize(194, 200);
  fHistoFrame->AddFrame(fHistoSubFrame);

  fLine4 = new TGHorizontal3DLine(fHistoFrame, 194, 8);
  fHistoFrame->AddFrame(fLine4, new TGLayoutHints(kLHintsLeft  | kLHintsTop, 2, 2, 8, 2));  

  fbDrawHisto = new TGTextButton(fHistoFrame, "Draw histogram");
  fbDrawHisto->SetToolTipText("Uses the data file created by the last \"Apply selected macro(s)\".\nClick here to display the data histograms of the selected macros.\nSelect multiple macros to create multi-dimensional plots.\nHisto macros cannot be used for multi-dimensional plots!");
  fbDrawHisto->Connect("Clicked()", "AliEveTRDTrackListEditor", this, "DrawHistos()");
  fHistoFrame->AddFrame(fbDrawHisto);

  // Set up file dialog
  fFileInfo = new TGFileInfo();
  fFileInfo->SetMultipleSelection(kTRUE);

  fFileTypes = new Char_t*[6];
  fFileTypes[0] = (Char_t*)"All files"; fFileTypes[1] = (Char_t*)"*";
  fFileTypes[2] = (Char_t*)"ROOT macros"; fFileTypes[3] = (Char_t*)"*.C";
  fFileTypes[4] = 0; fFileTypes[5] = 0;
  fFileInfo->fFileTypes = (const Char_t**)fFileTypes;
  fFileInfo->fFileTypeIdx = 2;
  fFileInfo->fMultipleSelection = kTRUE;

  fHistoCanvasName = new TGString("");

  // Handle style changed signals:
  fbgStyleTrack->Connect("Clicked(Int_t)", "AliEveTRDTrackListEditor", this, "SetTrackModel(Int_t)");
  fbgStyleColor->Connect("Clicked(Int_t)", "AliEveTRDTrackListEditor", this, "SetTrackColor(Int_t)");

  // Handle the signal "Selected(Int_t ind)"
  ftlMacroList->Connect("Selected(Int_t)", "AliEveTRDTrackListEditor", this, "UpdateMacroListSelection(Int_t)");
  ftlMacroSelList->Connect("Selected(Int_t)", "AliEveTRDTrackListEditor", this, "UpdateMacroListSelection(Int_t)");

  // Handle the signal "NewEventLoaded"
  gAliEveEvent->Connect("NewEventLoaded()", "AliEveTRDTrackListEditor", this, "HandleNewEventLoaded()");

  // Handle the signal "Selected" (another tab has been selected)
  GetGedEditor()->GetTab()->Connect("Selected(Int_t)", "AliEveTRDTrackListEditor", 
                                    this, "HandleTabChangedToIndex(Int_t)");
}

//______________________________________________________
AliEveTRDTrackListEditor::~AliEveTRDTrackListEditor()
{
  // Destructor: Closes all tabs created by this object and
  // frees the corresponding memory.

  if (fFileTypes != 0)
  {
    delete [] fFileTypes;
    fFileTypes = 0;
  }

  if (fFileInfo != 0)
  {
    delete fFileInfo; 
    fFileInfo = 0;
  }
  // Close and delete all tabs that have been created by this class
  CloseTabs();

  if (fHistoCanvasName != 0)
  {
    delete fHistoCanvasName;
    fHistoCanvasName = 0;
  }
  if (fInheritedMacroList != 0)
  {
    fInheritedMacroList->Delete();
    delete fInheritedMacroList;
    fInheritedMacroList = 0;
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::AddMacro(const Char_t* path, const Char_t* name)
{
  // Adds the macro path/name to the macro list. A warning is provided, if there is
  // something wrong, e.g. if the macro does not have the correct signature.

  Int_t result = fM->AddMacro(path, name);

  switch (result)
  {
  case SUCCESS:
    UpdateMacroList();
    break;
  case WARNING:
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Warning", "Macro is already in list (won't be added again)!",
                 kMBIconExclamation, kMBOk);
    break;
  case SIGNATURE_ERROR:
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                 "Macro has not the signature of...\n...a single track selection macro: Bool_t YourMacro(const AliTRDtrackV1*)\n...a correlated tracks selection macro: Bool_t YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*)\n...a single track analyse macro: void YourMacro(const AliTRDtrackV1*, Double_t*&, Int_t&)\n...a correlated tracks analyse macro: void YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*, Double_t*&, Int_t&)\n...a single track histo macro: TH1* YourMacro(const AliTRDtrackV1*)\n...a correlated tracks histo macro: TH1* YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*)", 
                 kMBIconExclamation, kMBOk);
    break;               
  case NOT_EXIST_ERROR:
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                 "File does not exist or you do not have read permission!", kMBIconExclamation, kMBOk);
    break;
  default:
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                 Form("AliEveTRDTrackList::AddMacro exited with unknown return value: %d", result),
                 kMBIconExclamation, kMBOk);
    break;
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::ApplyMacros()
{
  // Applies the selected macros and updates the view.

  Bool_t success = kFALSE;

  // First apply the single track selection macros
  TList* selIterator = new TList();
  ftlMacroSelList->GetSelectedEntries(selIterator);
  fM->ApplySTSelectionMacros(selIterator);
  
  // Update view
  gEve->Redraw3D();

  // Now apply the process macros
  TList* procIterator = new TList();
  ftlMacroList->GetSelectedEntries(procIterator);
  success = fM->ApplyProcessMacros(selIterator, procIterator);

  // Update histogram tab (data has to be reloaded)
  SetModel(fM);
  Update();

  // AlieveTRDTrackList::ApplyProcessMacros() automatically selects a macro -> Draw the histogram for it,
  // if a process macro has been applied
  if (success && procIterator->GetEntries() > 0) 
  {
    // Set focus on "Histograms" tab
    GetGedEditor()->GetTab()->SetTab("Histograms");

    DrawHistos();
  }

  if (selIterator != 0) delete selIterator;
  selIterator = 0;  
  if (procIterator != 0)  delete procIterator;  
  procIterator = 0;  
  
  if (!success)
  {
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                 "AliEveTRDTrackList::ApplyProcessMacros experienced an error (cf. CINT-output)!", 
                 kMBIconExclamation, kMBOk);  
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::BrowseMacros()
{
  // Creates a file-dialog. The selected files will be added to the macro list
  // via AddMacro(...).

  new TGFileDialog(gClient->GetRoot(), GetMainFrame(), kFDOpen, fFileInfo);
  
  if (fFileInfo->fIniDir != 0 && fFileInfo->fFileNamesList != 0)
  {       
    // Extract filenames
    TObject* iter = fFileInfo->fFileNamesList->First();
 
    Char_t* name = 0;

    while (iter != 0)
    {
      // NOTE: fileInfo->fFileNamesList will be changed by that, too!
      name = strrchr(iter->GetName(), '/');
      // Delete '"' at the end
      name[strlen(name)] = '\0';
              
      AddMacro(fFileInfo->fIniDir, name + 1); 
      iter = (TObjString*)fFileInfo->fFileNamesList->After(iter);
    }
  }

  // -> The following problem has been fixed (trunk -> Changes according to 03 September 2008):
  // Some error occurs, when one ends the filedialog with "cancel": fileInfo->fFileNamesList is set to 0x0, but
  // in the next launch no new memory is allocated. So do this manually.
  //if (fileInfo->fFileNamesList == 0)  fileInfo->fFileNamesList = new TList();
}

//______________________________________________________
void AliEveTRDTrackListEditor::CloseTabs()
{
  // Closes + deletes the tabs created by this object

  if (fHistoCanvas != 0)
  {
    // Close the created tab, if it exists
    if (fHistoCanvasName != 0)
    {
      if (gEve->GetBrowser()->GetTab(1)->SetTab(fHistoCanvasName->GetString()))
      {
        // Now the created tab is the current one and can be deleted
        gEve->GetBrowser()->GetTab(1)->RemoveTab();
      }
    }
    // With the tab removal, the canvas will be deleted automatically!
    fHistoCanvas = 0;
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::DrawHistos()
{
  // Accesses the temporary data file created by the last call of ApplyMacros() and draws
  // histograms according to the selection in the "Histograms"-tab.
 
  Int_t nHistograms = GetNSelectedHistograms();
  if (nHistograms <= 0)
  {
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                 "No data selected. Please select the data you want to plot!", kMBIconExclamation, kMBOk);
    return;
  }
  if (nHistograms > 3)
  {
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), 
                 "Error", "Only histograms with up to 3 dimensions supported. Please select 1,2 or 3 data macros!",
                 kMBIconExclamation, kMBOk);
    return;
  }

  // Check, if a histo macro shall be drawn
  Int_t indexOfHistoMacro = -1;
  Int_t selectedChecked = 0;
  for (Int_t j = 0; j < fM->fDataFromMacroList->GetEntries(); j++)
  {
    if (fCheckButtons[j]->TGButton::GetState() == kButtonDown)
    {
      selectedChecked++;

      // Histo macro? -> To check this, look for the substring "(histo macro)"
      if (strstr(fM->fDataFromMacroList->At(j)->GetName(), "(histo macro)") != 0)
      {
        // Is also another macro selected?
        if (nHistograms > 1)
        {
          // Histo macros cannot(!) be correlated!
          new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                       "Histo macros (return value \"TH1*\") cannot be combined with other macros", 
                       kMBIconExclamation, kMBOk);
          return;        
        }

        // Mark this histo macro for drawing
        indexOfHistoMacro = j;

        // Have all selected macros been checked? -> If yes, we are done with this
        if (selectedChecked == nHistograms)  break;
      }
    }
  }

  TFile* file = new TFile(Form("/tmp/TRD.TrackListMacroData_%s.root", gSystem->Getenv("USER")), "READ");
  if (!file)  
  {
    Error("Draw histograms", Form("Cannot open file \"/tmp/TRD.TrackListMacroData_%s.root\"", 
                                  gSystem->Getenv("USER")));
    new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                 Form("Cannot open file \"/tmp/TRD.TrackListMacroData_%s.root\"", gSystem->Getenv("USER")),
                 kMBIconExclamation, kMBOk);
    return;
  }
  
  TTree* t = 0;
  TTree* tFriend1 = 0;
  TTree* tFriend2 = 0;

  Int_t indexOfMacro1 = 0;
  Int_t indexOfMacro2 = 0;
  Int_t indexOfMacro3 = 0;

  // Variable for the loop below -> Will be set to aborting value, if a histo macro is drawn
  Int_t i = 0;
  
  // Draw histo macro?
  if (indexOfHistoMacro >= 0)
  {
    if ((t = (TTree*)file->Get(Form("TrackData%d", indexOfHistoMacro))))
    {
      SetDrawingToHistoCanvasTab();
 
      TH1* myHist = 0;
      t->SetBranchAddress(Form("Macro%d", indexOfHistoMacro), &myHist);
      t->GetEntry(0);
      if (myHist != 0)  myHist->Draw();
      else
      {
        Error("Draw histograms", Form("No histogram for histo macro \"%s\" found!", 
                                      fM->fDataFromMacroList->At(indexOfHistoMacro)->GetName()));
        new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                     Form("No histogram for histo macro \"%s\" found!", 
                          fM->fDataFromMacroList->At(indexOfHistoMacro)->GetName()), kMBIconExclamation, kMBOk);
               
      }

      UpdateHistoCanvasTab();    
    }
    else
    {
      Error("Draw histograms", Form("No data for histo macro \"%s\" found!\nMaybe no tracks have been selected.", 
                                    fM->fDataFromMacroList->At(indexOfHistoMacro)->GetName()));
      new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                   Form("No data for histo macro \"%s\" found!\nMaybe no tracks have been selected.", 
                        fM->fDataFromMacroList->At(indexOfHistoMacro)->GetName()), kMBIconExclamation, kMBOk);
    }

    // Skip the loop below
    i = fM->fDataFromMacroList->GetEntries();
  }

  // Load the trees in succession and remember the entries -> Plot the analyse macros
  for ( ; i < fM->fDataFromMacroList->GetEntries(); i++)
  {
    if (fCheckButtons[i]->TGButton::GetState() == kButtonDown)
    {
      if (t == 0)
      {
        indexOfMacro1 = i;
        if (!(t = (TTree*)file->Get(Form("TrackData%d", i))))
        { 
          Error("Draw histograms", Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                                        fM->fDataFromMacroList->At(i)->GetName()));
          new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                       Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                            fM->fDataFromMacroList->At(i)->GetName()), kMBIconExclamation, kMBOk);
          break;   
        }

        // 1d histogram
        if (nHistograms == 1) 
        {
          SetDrawingToHistoCanvasTab();
      
          t->Draw(Form("Macro%d", indexOfMacro1), "1");
          ((TH1*)gPad->GetPrimitive("htemp"))->SetTitle(Form("%s;%s",
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName()));
          UpdateHistoCanvasTab();        

          break;     
        }
      }
      else if (tFriend1 == 0)
      {
        indexOfMacro2 = i;
        if (!(tFriend1 = (TTree*)file->Get(Form("TrackData%d", i))))
        { 
          Error("Draw histograms", Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                                        fM->fDataFromMacroList->At(i)->GetName()));
          new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                       Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                            fM->fDataFromMacroList->At(i)->GetName()),
                            kMBIconExclamation, kMBOk);
          break;   
        }
        
        // 2d histogram
        if (nHistograms == 2) 
        {
          SetDrawingToHistoCanvasTab();

          t->AddFriend(tFriend1);
          t->Draw(Form("Macro%d:Macro%d", indexOfMacro1, indexOfMacro2), "1");
          ((TH1*)gPad->GetPrimitive("htemp"))->SetTitle(Form("%s - %s;%s;%s",
            fM->fDataFromMacroList->At(indexOfMacro2)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro2)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName()));

          UpdateHistoCanvasTab();
 
          break;     
        }
      }    
      // 3d histogram
      else
      {
        indexOfMacro3 = i;
        if (!(tFriend2 = (TTree*)file->Get(Form("TrackData%d", i))))
        { 
          Error("Draw histograms", Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                                        fM->fDataFromMacroList->At(i)->GetName()));
          new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error - Draw histograms", 
                       Form("No data for macro \"%s\" found!\nMaybe no tracks have been selected.", 
                            fM->fDataFromMacroList->At(i)->GetName()), kMBIconExclamation, kMBOk);
          break;   
        }

        SetDrawingToHistoCanvasTab();

        t->AddFriend(tFriend1);
        t->AddFriend(tFriend2);
        t->Draw(Form("Macro%d:Macro%d:Macro%d", indexOfMacro1, indexOfMacro2, indexOfMacro3), "1");
        ((TH1*)gPad->GetPrimitive("htemp"))->SetTitle(Form("%s - %s - %s;%s;%s;%s",
            fM->fDataFromMacroList->At(indexOfMacro3)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro2)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro3)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro2)->GetName(),
            fM->fDataFromMacroList->At(indexOfMacro1)->GetName()));
        
        UpdateHistoCanvasTab();
 
        break;     
      }
    }
  }

  if (t != 0) delete t;
  t = 0;
  if (tFriend1 != 0)  delete tFriend1;
  tFriend1 = 0;
  if (tFriend2 != 0)  delete tFriend2;
  tFriend2 = 0;

  file->Close("R");
  delete file;
  file = 0;
}

//______________________________________________________
Int_t AliEveTRDTrackListEditor::GetNSelectedHistograms() const
{
  // Returns the number of selected macros (or rather: of their selected data) in the "Histograms"-tab

  Int_t count = 0;
  
  for (Int_t i = 0; i < fM->fDataFromMacroList->GetEntries(); i++)
  {
    if (fCheckButtons[i]->TGButton::GetState() == kButtonDown)  count++;
  }

  return count;
}

//______________________________________________________
void AliEveTRDTrackListEditor::HandleMacroPathSet()
{
  // Takes the input of the text field (adding a macro), checks if the macro can be
  // accessed (and that it exists) and adds the macro to the macro list via AddMacro(...).
  // You can use environment variables in the text field, e.g. "$ALICE_ROOT/Eve/alice-macro/myMacro.C".

  if (strlen(fteField->GetText()) != 0)
  {  
    // Expand the pathname
    Char_t* systemPath = gSystem->ExpandPathName(fteField->GetText());
    fteField->SetText(systemPath);
    delete systemPath;
    systemPath = 0;
       			
    // Check if file exists
    FILE* fp = NULL;

    fp = fopen(fteField->GetText(), "rb");
    if (fp != NULL)
    {
      fclose(fp);

      // Extract filename
      Char_t* name = strrchr(fteField->GetText(), '/');

      // Current path
      if (name == NULL)
      {
        name = new Char_t[AliEveTRDTrackList::fkMaxMacroNameLength];
        memset(name, '\0', sizeof(Char_t) * AliEveTRDTrackList::fkMaxMacroNameLength);
        sprintf(name, "%s", fteField->GetText());

        // Add path to textfield -> Path is "./" -> Use length for the name + 2
        Char_t pathname[AliEveTRDTrackList::fkMaxMacroNameLength + 2];
        memset(pathname, '\0', sizeof(Char_t) * (AliEveTRDTrackList::fkMaxMacroNameLength + 2));
        sprintf(pathname, "./%s", fteField->GetText());
        fteField->SetText(pathname);

        AddMacro(".", name);  
        if (name != 0)  delete name;
        name = 0;
      }
      // Different path
      else
      {
        // Extract path
        Char_t* path = new Char_t[AliEveTRDTrackList::fkMaxMacroPathLength];
        memset(path, '\0', sizeof(Char_t) * AliEveTRDTrackList::fkMaxMacroPathLength);
        strncpy(path, fteField->GetText(), strlen(fteField->GetText()) - strlen(name));
        
        // Ignore the slash "/" in name
        AddMacro(path, name + 1);  
  
        if (path != 0)  delete path;
        path = 0;
      }       
    }
    else
    {
      new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error", 
                   "File does not exist or you do not have read permission!", kMBIconExclamation, kMBOk);
    }
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::HandleNewEventLoaded()
{
  // Closes the tabs created by this object and sets a flag that will
  // cause the function SetModel() to inherit the macro lists + style
  // for the next AliEveTRDTrackList from the current one.

  // Inherit the macro list and track style for the next track list!
  fInheritSettings = kTRUE;

  // Close the tabs
  CloseTabs();
}

//______________________________________________________
void AliEveTRDTrackListEditor::HandleTabChangedToIndex(Int_t index)
{
  // Saves the current tab in the current AliEveTRDTrackList.

  fM->SetSelectedTab(index);
}

//______________________________________________________
void AliEveTRDTrackListEditor::InheritMacroList()
{
  // The old macro list is possibly stored in the corresponding interior map. This function will 
  // use this interior map to move the data from the interior map to the newly loaded AliEveTRDTrackList. 
  // Then the interior map will be cleaned up. With this, the settings will be inherited from the previously 
  // loaded AliEveTRDTrackList.

  if (fInheritedMacroList == 0)  return;

  // Clear list  
  fM->fMacroList->Delete();

  // Store data from interior list in the track list's map
  TMapIter* iter = (TMapIter*)fInheritedMacroList->MakeIterator();
  
  TObject* key = 0;
  TMacroData* macro = 0;
  
  while ((key = iter->Next()) != 0)
  {
    macro = (TMacroData*)fInheritedMacroList->GetValue(key);
    if (macro != 0)  fM->fMacroList->Add(new TObjString(key->GetName()), 
                                         new TMacroData(macro->GetName(), macro->GetPath(), macro->GetType()));
    else
    {
      Error("AliEveTRDTrackListEditor::InheritMacroList", Form("Failed to inherit the macro \"%s\"!", key));
    }
  }
  
  fInheritedMacroList->Delete();
  delete fInheritedMacroList;
  fInheritedMacroList = 0;
}

//______________________________________________________
void AliEveTRDTrackListEditor::InheritStyle()
{
  // The old styles are stored in the corresponding button groups. This function will replace
  // the style settings of the newly loaded AliEveTRDTrackList with the old styles. With this, the settings
  // will be inherited from the previously loaded AliEveTRDTrackList.

  for (Int_t ind = 0; ind < 3; ind++)
  {
    if (fbgStyleTrack->GetButton(ind)->IsOn())
    {
      SetTrackModel(ind);
      break;
    }
  }
  for (Int_t ind = 0; ind < 3; ind++)
  {
    if (fbgStyleColor->GetButton(ind)->IsOn())
    {
      SetTrackColor(ind);
      break;
    }
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::RemoveMacros()
{
  // Removes the selected macros from the corresponding list.

  TList* iterator = new TList();
  
  ftlMacroList->GetSelectedEntries(iterator);
  fM->RemoveSelectedMacros(iterator);

  if (iterator != 0)  delete iterator;

  iterator = new TList();
  ftlMacroSelList->GetSelectedEntries(iterator);
  fM->RemoveSelectedMacros(iterator);

  // Selected macros are deleted from the list -> No selected entries left
  fM->fMacroListSelected = 0;

  UpdateMacroList();

  if (iterator != 0)  delete iterator;
  iterator = 0;
}

//______________________________________________________
void AliEveTRDTrackListEditor::SetDrawingToHistoCanvasTab()
{
  // Sets gPad to the tab with the name of the current AliEveTRDTrackList. If this tab does
  // not exist, it will be created. Otherwise, it is re-used.

  // If the tab with the canvas has been closed, the canvas will be deleted.
  // So, if there is no tab, set the canvas pointer to zero and recreate it in a new tab.
  if (fHistoCanvas != 0) 
  {
    if (gEve->GetBrowser()->GetTab(1)->SetTab(fHistoCanvasName->GetString()) == 0)
    {
      fHistoCanvas = 0;
    }
  }

  if (!fHistoCanvas)
  {
    fHistoCanvas = gEve->AddCanvasTab(fM->GetName());     
  }
                           
  gPad = fHistoCanvas;
}

//______________________________________________________
void AliEveTRDTrackListEditor::SetModel(TObject* obj)
{  
  // Sets the model object, updates the related data in the GUI and
  // inherits settings (cf. Inherit*(...)), if the flag fInheritSettings is set to kTRUE.

  fM = dynamic_cast<AliEveTRDTrackList*>(obj);

  if (fM == 0) 
  {
    Error("SetModel", "Parameter is zero pointer");
    return;
  }

  // Provide a pointer to this editor
  fM->fEditor = this;

  // If macro list + track style shall be inherited from previously loaded track list, do so
  if (fInheritSettings)
  {
    InheritMacroList();
    InheritStyle();

    fInheritSettings = kFALSE;
  }

  // Select the correct styles
  Int_t b = 0;
  UChar_t style = fM->GetSelectedTrackStyle();
  if (TESTBIT(style, AliEveTRDTrack::kSource)) b = 2;
  else 
  {
    if (TESTBIT(style, AliEveTRDTrack::kPID)) b = 1;
    else b = 0;
  } 
  fbgStyleColor->SetButton(b, kTRUE);


  if (TESTBIT(style, AliEveTRDTrack::kTrackCosmics)) b = 2;
  else
  {
    if (TESTBIT(style, AliEveTRDTrack::kTrackModel)) b = 1;
    else b = 0;
  }
  fbgStyleTrack->SetButton(b, kTRUE);
  
  UpdateMacroList();
  UpdateHistoList(); 

  // View correct tab
  GetGedEditor()->GetTab()->SetTab(fM->GetSelectedTab()); 
}

//______________________________________________________
void AliEveTRDTrackListEditor::SaveMacroList(TMap* list)
{
  // Saves the provided macro list in an interior list. This list will be used by
  // InheritMacroList() to restore the data in "list". With this method one is able
  // to inherit the macro list from track list to track list (i.e. from event to event).

  if (fInheritedMacroList != 0)
  {
    fInheritedMacroList->Delete();
    delete fInheritedMacroList;
  }
  fInheritedMacroList = new TMap();
  fInheritedMacroList->SetOwnerKeyValue(kTRUE, kTRUE);

  TMapIter* iter = (TMapIter*)list->MakeIterator();
  TObject* key = 0;
  TMacroData* macro = 0;
  
  while ((key = iter->Next()) != 0)
  {
    macro = (TMacroData*)fM->fMacroList->GetValue(key);
    if (macro != 0) fInheritedMacroList->Add(new TObjString(key->GetName()), 
                                             new TMacroData(macro->GetName(), macro->GetPath(), macro->GetType()));
    else
    {
      Error("AliEveTRDTrackListEditor::SaveMacroList", Form("Failed to inherit the macro \"%s\"!", key));
    }
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::SetTrackColor(Int_t ind)
{
  // Sets the color model for the tracks, updates the tracks with this model and
  // redraws the scene.

  switch(ind)
  { 
    case AliTRDReconstructor::kLQPID:
      fM->UpdateTrackStyle(AliEveTRDTrack::kPID, AliTRDReconstructor::kLQPID);
      break;
    case AliTRDReconstructor::kNNPID:
      fM->UpdateTrackStyle(AliEveTRDTrack::kPID, AliTRDReconstructor::kNNPID);
      break;
    default:
      fM->UpdateTrackStyle(AliEveTRDTrack::kSource);
      break;
  }

  gEve->Redraw3D();
}

//______________________________________________________
void AliEveTRDTrackListEditor::SetTrackModel(Int_t ind)
{
  // Sets the track model for the tracks, updates the tracks with this model and
  // redraws the scene.

  switch(ind)
  { 
    case AliEveTRDTrack::kRieman:
      fM->UpdateTrackStyle(AliEveTRDTrack::kTrackModel, AliEveTRDTrack::kRieman);
      break;
    case AliEveTRDTrack::kKalman:
      fM->UpdateTrackStyle(AliEveTRDTrack::kTrackModel, AliEveTRDTrack::kKalman);
      break;
    default:
      fM->UpdateTrackStyle(AliEveTRDTrack::kTrackCosmics);
      break;
  }

  gEve->Redraw3D();
}

//______________________________________________________
void AliEveTRDTrackListEditor::UpdateDataFromMacroListSelection()
{
  // Saves the current selection in the "Histograms"-tab to the current
  // AliEveTRDTrackList. This means that the selection is updated and won't
  // get lost, if another editor is loaded in Eve.

  for (Int_t i = 0; i < fM->fDataFromMacroList->GetEntries(); i++)
  {
    fM->SetHistoDataSelection(i, fCheckButtons[i]->IsOn());
  }
}

//______________________________________________________
void AliEveTRDTrackListEditor::UpdateHistoCanvasTab()
{
   // Updates the histogram and the corresponding tab (including titles).

  // Update name of the tab (tab has been set to current tab!)
  fHistoCanvasName->SetString(fM->GetName());  

  // Use a copy of fHistoCanvasName!! -> If the user closes a tab manually, the TGString
  // will be deleted -> Error might occur, when accessing the pointer   
  gEve->GetBrowser()->GetTab(1)->GetCurrentTab()->SetText(new TGString(fHistoCanvasName));

  // Switch tabs to force redrawing
  gEve->GetBrowser()->GetTab(1)->SetTab(0);
  gEve->GetBrowser()->GetTab(1)->SetTab(fHistoCanvasName->GetString());
  fHistoCanvas->Update();
}

//______________________________________________________
void AliEveTRDTrackListEditor::UpdateHistoList()
{
  // Reloads (updates) the buttons in the "Histograms"-tab via
  // the current AliEveTRDTrackList (data).

  fHistoSubFrame->TGCompositeFrame::Cleanup();
  
  // Set buttons for histograms
  if (fCheckButtons != 0) delete fCheckButtons;
  fCheckButtons = new TGCheckButton*[fM->fDataFromMacroList->GetEntries()];
  
  TObjString* iter = (TObjString*)fM->fDataFromMacroList->First();
  for (Int_t i = 0; i < fM->fDataFromMacroList->GetEntries() && iter != 0; i++)
  {
    fCheckButtons[i] = new TGCheckButton(fHistoSubFrame, iter->GetName());
    fHistoSubFrame->AddFrame(fCheckButtons[i]);
    
    fCheckButtons[i]->SetState(kButtonUp, kFALSE);
    fCheckButtons[i]->MapRaised();
    fCheckButtons[i]->SetOn(fM->HistoDataIsSelected(i));
    fCheckButtons[i]->Connect("Clicked()", "AliEveTRDTrackListEditor", this, "UpdateDataFromMacroListSelection()");
            
    iter = (TObjString*)fM->fDataFromMacroList->After(iter);
  }  
}

//______________________________________________________
void AliEveTRDTrackListEditor::UpdateMacroList()
{
  // Reloads (updates) the macro list (selection AND process macros) via
  // the current AliEveTRDTrackList (data).

  ftlMacroList->RemoveAll();
  ftlMacroSelList->RemoveAll();
   
  TMapIter* iter = (TMapIter*)fM->fMacroList->MakeIterator();
  TObject* key = 0;
  TMacroData* macro = 0;

  Int_t ind = 0;
  while ((key = iter->Next()) != 0)
  {
    macro = (TMacroData*)fM->fMacroList->GetValue(key);
    if (macro != 0)
    {
      if (macro->IsProcessMacro())
      {
        ftlMacroList->AddEntry(macro->GetName(), ind);
        // Select, what has been selected before
        ftlMacroList->Select(ind, fM->MacroListIsSelected(ind));
        ind++;
      }
      else if (macro->IsSelectionMacro())
      {
        ftlMacroSelList->AddEntry(macro->GetName(), ind);
        // Select, what has been selected before
        ftlMacroSelList->Select(ind, fM->MacroListIsSelected(ind));
        ind++;
      }
      else
      {
        Error("AliEveTRDTrackListEditor::UpdateMacroList()", 
              Form("Macro \"%s/%s.C\" has neither a selection macro nor a process macro!",
                   macro->GetPath(), macro->GetName()));                                        
      }
    }
    else
    {
      Error("AliEveTRDTrackListEditor::UpdateMacroList()", 
              Form("Macro list is corrupted: Macro \"%s\" not found!", key->GetName()));            
    }     
  }

  ftlMacroList->SortByName(); 
  ftlMacroSelList->SortByName(); 
}

//______________________________________________________
void AliEveTRDTrackListEditor::UpdateMacroListSelection(Int_t ind)
{
  // Saves the current selection in the macro listS to the current
  // AliEveTRDTrackList. This means that the selection is updated and won't
  // get lost, if another editor is loaded in Eve.
  // NOTE: The indices in BOTH lists will be unique!

  // Toggle selected item
  fM->SetMacroListSelection(ind, !fM->MacroListIsSelected(ind));
}
