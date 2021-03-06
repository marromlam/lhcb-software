// $Id: DatabasePagePathDialog.cpp,v 1.4 2010-10-01 12:52:26 frankb Exp $
// STL 
#include <map>

// ROOT
#include <TSystem.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextEdit.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TGMsgBox.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TGResourcePool.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TH1.h>
#include <TH2.h>

// Online
#include "OnlineHistDB/OnlineHistDB.h"

// Local
#include "Presenter/PresenterMainFrame.h"
#include "Presenter/DatabasePagePathDialog.h"

using namespace pres;

ClassImp(DatabasePagePathDialog)

DatabasePagePathDialog::DatabasePagePathDialog(PresenterMainFrame* gui, int width,
  int height, MsgLevel v) :
    TGTransientFrame(gClient->GetRoot(), gui, width, height),
    m_mainFrame(gui),
    m_verbosity(v),
    m_msgBoxReturnCode(0)
{
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "DatabasePagePathDialog", this, "DontCallClose()");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  MapWindow();
}
DatabasePagePathDialog::~DatabasePagePathDialog()
{
  Cleanup();
}
void DatabasePagePathDialog::build()
{
  SetLayoutBroken(true);

  TGLabel *fLabel530 = new TGLabel(this,"Enter (new) or select the parent folder(s):");
  fLabel530->SetTextJustify(36);
  AddFrame(fLabel530, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel530->MoveResize(16, 8, 250, 18);

  m_folderNameTextEntry = new TGTextEntry(this, new TGTextBuffer(15),-1,
                              kSunkenFrame | kDoubleBorder | kOwnBackground);
  m_folderNameTextEntry->SetMaxLength(255);
  m_folderNameTextEntry->SetAlignment(kTextLeft);
  m_folderNameTextEntry->Resize(456, m_folderNameTextEntry->GetDefaultHeight());
  AddFrame(m_folderNameTextEntry,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_folderNameTextEntry->MoveResize(16, 32, 456, 22);
  m_folderNameTextEntry->Connect("ReturnPressed()", "DatabasePagePathDialog",
                                 this, "setOkButton()");

  // canvas widget
  TGCanvas *fCanvas694 = new TGCanvas(this, 456, 192);

  // canvas viewport
  TGViewPort *fViewPort695 = fCanvas694->GetViewPort();

  // list tree
  m_pageFolderListTree = new TGListTree(fCanvas694, kHorizontalFrame);
  m_pageFolderListTree->AddRoot("Pages");
  m_mainFrame->listHistogramsFromHistogramDB(m_pageFolderListTree,
      FoldersAndPages, s_withoutHistograms);
  m_pageFolderListTree->Connect(
    "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "DatabasePagePathDialog",
    this, "updateTextFields(TGListTreeItem*, Int_t, Int_t, Int_t)");

  fViewPort695->AddFrame(m_pageFolderListTree);
  m_pageFolderListTree->SetLayoutManager(
    new TGHorizontalLayout(m_pageFolderListTree));
  m_pageFolderListTree->MapSubwindows();
  fCanvas694->SetContainer(m_pageFolderListTree);
  fCanvas694->MapSubwindows();
  AddFrame(fCanvas694,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fCanvas694->MoveResize(16, 56, 456, 192);

  TGLabel *fLabel717 = new TGLabel(this,"Page name:");
  fLabel717->SetTextJustify(36);
  AddFrame(fLabel717,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel717->MoveResize(16, 256, 72, 18);

  m_pageNameTextEntry = new TGTextEntry(this, new TGTextBuffer(15),-1,
    kSunkenFrame | kDoubleBorder | kOwnBackground);
  m_pageNameTextEntry->SetMaxLength(255);
  m_pageNameTextEntry->SetAlignment(kTextLeft);
  m_pageNameTextEntry->Resize(376, m_pageNameTextEntry->GetDefaultHeight());
  AddFrame(m_pageNameTextEntry,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_pageNameTextEntry->MoveResize(96, 256, 376, 22);
  m_pageNameTextEntry->Connect("ReturnPressed()", "DatabasePagePathDialog",
                               this, "setOkButton()");

                               
  TGLabel *pageDescriptionLabel = new TGLabel(this,"Page Comment:");
  pageDescriptionLabel->SetTextJustify(36);
  AddFrame(pageDescriptionLabel,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  pageDescriptionLabel->MoveResize(16, 286, 100, 18);

  m_pageDescriptionTextEditor = new TGTextEdit(this, 200, 100);
//  m_pageDescriptionTextEditor->SetMaxLength(255);
//  m_pageDescriptionTextEditor->SetAlignment(kTextLeft);
//  m_pageDescriptionTextEditor->Resize(376, m_pageDescriptionTextEditor->GetDefaultHeight());
  AddFrame(m_pageDescriptionTextEditor,
           new TGLayoutHints(kLHintsLeft | kLHintsTop |
                             kLHintsExpandX | kLHintsExpandY,
                             2, 2, 2, 2));
  m_pageDescriptionTextEditor->MoveResize(16, 310, 450, 130);

  TGLabel *patternFileLabel = new TGLabel(this,"Pattern file:");
  patternFileLabel->SetTextJustify(36);
  AddFrame(patternFileLabel,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  patternFileLabel->MoveResize(16, 455, 90, 18);

  m_patternFileTextEntry = new TGTextEntry(this, new TGTextBuffer(15),-1,
    kSunkenFrame | kDoubleBorder | kOwnBackground);
  m_patternFileTextEntry->SetMaxLength(255);
  m_patternFileTextEntry->SetAlignment(kTextLeft);
//  m_patternFileTextEntry->Resize(376, m_pageNameTextEntry->GetDefaultHeight());
  AddFrame(m_patternFileTextEntry,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_patternFileTextEntry->MoveResize(110, 452, 250, 22);
                               
                                                              

  m_okButton = new TGTextButton(this,"OK");
  m_okButton->SetTextJustify(36);
  m_okButton->Resize(80, 24);
  m_okButton->SetState(kButtonDisabled);
  AddFrame(m_okButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_okButton->MoveResize(280, 488, 80, 24);
  m_okButton->Connect("Clicked()", "DatabasePagePathDialog", this, "ok()");

  m_cancelButton = new TGTextButton(this,"Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(80, 24);
  AddFrame(m_cancelButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_cancelButton->MoveResize(376, 488, 80, 24);
  m_cancelButton->Connect("Clicked()", "DatabasePagePathDialog",
                          this, "CloseWindow()");

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(493, 539);
}
void DatabasePagePathDialog::ok()
{
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);

  if (m_mainFrame->isConnectedToHistogramDB()) {

    OnlineHistDB* m_histogramDB = m_mainFrame->histogramDB();
    m_folderName = m_folderNameTextEntry->GetText();
    m_pageName = m_pageNameTextEntry->GetText();
    std::string patternFile = m_patternFileTextEntry->GetText();
    std::string pageDescription = ((dynamic_cast<TGTextView*>(m_pageDescriptionTextEditor)->GetText())->AsString()).Data();
    std::string fullPageName = s_slash+m_folderName+s_slash+m_pageName;
    try {
      OnlineHistPage* page = m_histogramDB->getPage( fullPageName );
      std::map<TPad*,OnlineHistogram*> padOwner;
      bool thereAreOverlaps = false;
      page->removeAllHistograms();
      double xlow, ylow, xup, yup;
      std::cout << "Currently, the page has " << page->nh() << " histograms." << std::endl;
      // first, save owners of pads (not overlaps)

      m_mainFrame->myPage().prepareDisplayHistos();   // Build the final list of all displayhistograms of the page.
      
      DisplayHistograms::iterator itDH;
      for ( itDH  = m_mainFrame->myPage().displayHistos().begin();
            itDH != m_mainFrame->myPage().displayHistos().end();
            itDH++) {
        
        if( (*itDH)->isOverlap()) {
          thereAreOverlaps = true;
        } else {
          if ( (*itDH)->hostingPad() ) {
            (*itDH)->hostingPad()->GetPadPar(xlow, ylow, xup, yup);
            //== Restore the 100% y size dur to the banner
            if ( m_mainFrame->myPage().hasBanner() ) {
              ylow = ylow / 0.95;
              yup  = yup  / 0.95;
            }
            std::cout << "Store histo id '" << (*itDH)->histo()->hid() << "'" << std::endl;
            OnlineHistogram* onlineHistogram = page->addHistogram( (*itDH)->histo(),
                                                                   (float)xlow , (float)ylow, (float)xup, (float)yup);
            if (0 != onlineHistogram) (*itDH)->setOnlineHistogram( onlineHistogram );
            padOwner[(*itDH)->hostingPad()] = onlineHistogram;          
          }
        }
      }
      
      page->setPatternFile(patternFile);

      if ( page->save() ) {
        if (thereAreOverlaps) {
          int iov=0;
          // add definitions of overlapped histograms
          for ( itDH  = m_mainFrame->myPage().displayHistos().begin();
                itDH != m_mainFrame->myPage().displayHistos().end();
                itDH++) {
            if( (*itDH)->isOverlap() ) {
              OnlineHistogram* onlineHistogram = page->addOverlapHistogram( (*itDH)->histo(),
                                                                            padOwner[(*itDH)->hostingPad()],
                                                                            (padOwner[(*itDH)->hostingPad()])->instance(),
                                                                            iov++ );
              if (0 != onlineHistogram) (*itDH)->setOnlineHistogram(onlineHistogram);
            }
          }
          page->save();
        }

        // now save current ROOT display options of histograms on page
        
        for ( itDH  = m_mainFrame->myPage().displayHistos().begin();
              itDH != m_mainFrame->myPage().displayHistos().end();
              itDH++) {
          if( (*itDH)->histo()->page() == page->name() ) {
            (*itDH)->hostingPad()->cd();
            (*itDH)->saveOptionsToDB( (*itDH)->hostingPad() );
          }
        }
      }
      page->setDoc(pageDescription);
      m_histogramDB->commit();
    } catch (std::string sqlException) {
      m_mainFrame->setStatusBarText(sqlException.c_str(), 0);
      std::cout << sqlException << std::endl;
      /*
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
          Form("Could save the page to OnlineHistDB:\n\n%s\n",
               sqlException.c_str()),
               kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      */
    }
    CloseWindow();
  }
}
void DatabasePagePathDialog::updateTextFields(TGListTreeItem* node, int, int, int)
{
  if (0 != node) {
    char path[1024];

    m_pageFolderListTree->GetPathnameFromItem(node, path);
    std::string dbPath = std::string(path);
    // Drop DB url
    dbPath = dbPath.erase(0, std::string(
                          m_pageFolderListTree->GetFirstItem()->GetText()).
                            length() + 2);

    // find pagename in path
    TObjArray* pathSlices = TPRegexp("^((?:(?:[^/]*)/)*)([^/]*)$").
                                      MatchS(dbPath.c_str());

    TString m_folderName = ((TObjString *)pathSlices->At(1))->GetString();
    m_folderName = m_folderName.Strip(TString::kTrailing,'/');
    TString pageName = ((TObjString *)pathSlices->At(2))->GetString();

    pathSlices->Delete();
    delete pathSlices;

    if (NULL != node->GetUserData()) {
       m_pageNameTextEntry->SetText(pageName);
       m_folderNameTextEntry->SetText(m_folderName);
    } else {
       m_folderNameTextEntry->SetText(dbPath.c_str());
    }

    setOkButton();
  }
}
void DatabasePagePathDialog::setOkButton() {
  if (TString(m_folderNameTextEntry->GetText()).Sizeof() > 1  &&
      TString(m_pageNameTextEntry->GetText()).Sizeof() > 1 ) {
    m_okButton->SetEnabled(true);
  } else {
    m_okButton->SetEnabled(false);
  }
}
void DatabasePagePathDialog::CloseWindow() {
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);
  DeleteWindow();
}
