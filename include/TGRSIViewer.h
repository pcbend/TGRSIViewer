#ifndef TGRSIVIEWER_H
#define TGRSIVIEWER_H

#include <map>

#include <TCanvas.h>
#include <TChain.h>
#include <TTree.h>
#include <TFolder.h>

#include "TGFrame.h"
#include "TGButton.h"
#include "TGListTree.h"
#include "TGStatusBar.h"

#include "TGRSIHistManager.h"

typedef std::map<unsigned int,TCanvas *> GRSICanvasMap;

class TGRSIViewer : public TGMainFrame {

   public:
      TGRSIViewer(const TGWindow *p=0,UInt_t w=800,UInt_t h=800);
      virtual ~TGRSIViewer();

      static TGRSIHistManager *GetHistManager() { return fGRSIHistManager; }

   protected:
      enum EMunuCommands { 
         M_FILE_OPEN,
         M_FILE_EXIT,

         M_HELP_ABOUT,
         M_HELP_HELP
      };

      enum EButtonCommands {
         B_LOAD,
         B_TWO,
         B_EXIT,
         B_DRAW
      };



   private:

      bool ProcessMessage(Long_t,Long_t,Long_t);
      bool HandleButtonClick(Long_t,Long_t);

      void LayoutMenuBar();
      void LayoutGUI();
      void SetupListTree(TGCanvas*);

      TGStatusBar *fStatusBar;

      TGTextButton *fTextButton_Load;
      TGTextButton *fTextButton_Two;
      TGTextButton *fTextButton_Exit;
      TGTextButton *fTextButton_Draw;

      TGListTree     *fListTree;

      TGListTreeItem *fTreeItemRoot;
      TGListTreeItem *fTreeItemGRSI;
      TGListTreeItem *fTreeItemFragment;
      TGListTreeItem *fTreeItemAnalysis;
      TGListTreeItem *fTreeItemCal;
      TGListTreeItem *fTreeItemOdb;
      TGListTreeItem *fTreeItemCuts;

      static TCanvas *fCurrentCanvas;
      static TChain  *fFragmentChain;
      static TChain  *fAnalysisChain;

      static TGRSIHistManager *fGRSIHistManager;

   private:
      //static unsigned int gCanvasNumber;
      TCanvas *MakeNewCanvas(const char *title=0);
      static GRSICanvasMap fGRSICanvasMap;

      void StartLoadFileDialog();
      void HandleFile(const char*);
      void OpenRootFile(const char*);
      void AddFragmentTree(TFile*,TTree*);
      void AddAnalysisTree(TFile*,TTree*);
      void AddTFolder(TFolder*,TGListTreeItem*);
      void AddTDirectory(TDirectory*,TGListTreeItem*);

      TGListTreeItem *AddToListTree(const char *name,TGListTreeItem *parent=0, TObject *object=0);
      void AddBranchToListTree(TBranch *branch,TGListTreeItem *parent=0);

      std::map <TGListTreeItem*,TGRSIHistManager*> fHistMap;
      TGRSIHistManager *GetHistManager(TGListTreeItem*);

      void CloseViewer(Option_t *opt ="");

   public:    //slots
      void GRSICanvasClosed();
      void GRSICanvasSelected(TPad*,TObject*,Int_t);
      void GRSICanvasProcessEvent(Int_t,Int_t,Int_t,TObject*);

      void HandleListTreeClicked(TGListTreeItem*,Int_t,Int_t,Int_t);     
      void HandleListTreeDoubleClicked(TGListTreeItem*,Int_t,Int_t,Int_t);     
      void HandleListTreeKeyPressed(TGListTreeItem*,ULong_t,ULong_t);
      void HandleListTreeReturnPressed(TGListTreeItem*);

   ClassDef(TGRSIViewer,0)
};

#endif
