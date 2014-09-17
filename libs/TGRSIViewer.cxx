


#include "Globals.h"

#include <TGRSIViewer.h>

#include <TChainElement.h>
#include <TSystemFile.h>
#include <TFile.h>
#include <TList.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TKey.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TObjString.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TApplication.h>

#include <KeySymbols.h>
#include <GuiTypes.h>
#include <WidgetMessageTypes.h>


ClassImp(TGRSIViewer)

GRSICanvasMap TGRSIViewer::fGRSICanvasMap;

TCanvas *TGRSIViewer::fCurrentCanvas = 0;
TChain  *TGRSIViewer::fFragmentChain = 0;
TChain  *TGRSIViewer::fAnalysisChain = 0;

TGRSIViewer::TGRSIViewer(const TGWindow *p,UInt_t w,UInt_t h) 
            :TGMainFrame(p?p:gClient->GetRoot(),w,h)  {

   SetCleanup(kDeepCleanup);

   SetWindowName("Peter's Shoeshine Emporium");              

   LayoutMenuBar();
   LayoutGUI();
   Resize(GetDefaultSize());
   MapSubwindows();
   Layout();
   MapWindow();

}


TGRSIViewer::~TGRSIViewer() { };


void TGRSIViewer::LayoutMenuBar()   { 
   TGPopupMenu *menuFile = new TGPopupMenu(fClient->GetRoot());   
   menuFile->AddEntry("&Open",M_FILE_OPEN);
   menuFile->AddSeparator();
   menuFile->AddEntry("E&xit",M_FILE_EXIT);
   menuFile->Associate(this);
   //
   TGPopupMenu *menuHelp = new TGPopupMenu(fClient->GetRoot());
   menuHelp->AddEntry("&About",M_HELP_ABOUT);
   menuHelp->AddEntry("&HELP!",M_HELP_HELP);
   menuHelp->Associate(this);
   //
   
   TGLayoutHints* menuBarLayout_all   = new TGLayoutHints( kLHintsNormal|kLHintsExpandX, 0, 0, 0, 10 );
   TGLayoutHints* menuBarLayout_left  = new TGLayoutHints( kLHintsTop|kLHintsLeft , 5, 0, 0, 0 );
   TGLayoutHints* menuBarLayout_right = new TGLayoutHints( kLHintsTop|kLHintsRight, 0, 5, 0, 0 );

   TGMenuBar *menuBar = new TGMenuBar( this, 1, 1, kRaisedFrame );
   menuBar->AddPopup("&File",menuFile,menuBarLayout_left);
   menuBar->AddPopup("&Help",menuFile,menuBarLayout_right);

   this->AddFrame(menuBar,menuBarLayout_all);
}

void TGRSIViewer::LayoutGUI()       { 
   
   TGLayoutHints* frameLayout_all = new TGLayoutHints( kLHintsTop|kLHintsExpandX|kLHintsExpandY, 5, 5, 5, 5 );
   TGLayoutHints* frameLayout_x = new TGLayoutHints( kLHintsTop|kLHintsExpandX, 0, 0, 0, 0 );
   TGLayoutHints* frameLayout_y = new TGLayoutHints( kLHintsTop|kLHintsExpandY, 0, 0, 0, 0 );

   TGVerticalFrame   *vFrame      = new TGVerticalFrame(this,10,10);
   TGHorizontalFrame *buttonFrame = new TGHorizontalFrame(vFrame,10,10);
   TGLayoutHints* buttonLayout    = new TGLayoutHints(kLHintsCenterX,2,2,2,2);

   fTextButton_Load   = new TGTextButton(buttonFrame,"Load",B_LOAD); 
   fTextButton_Load->Associate(this);
   fTextButton_Load->SetToolTipText("Button Load Tip",250);
   buttonFrame->AddFrame(fTextButton_Load,buttonLayout);

   fTextButton_Draw  = new TGTextButton(buttonFrame,"Draw",B_DRAW);
   fTextButton_Draw->Associate(this);
   fTextButton_Draw->SetToolTipText("Button Draw Tip",250);
   buttonFrame->AddFrame(fTextButton_Draw,buttonLayout);

   fTextButton_Two   = new TGTextButton(buttonFrame,"Two",B_TWO);
   fTextButton_Two->Associate(this);
   fTextButton_Two->SetToolTipText("Button Two Tip",250);
   buttonFrame->AddFrame(fTextButton_Two,buttonLayout);

   fTextButton_Exit = new TGTextButton(buttonFrame,"Exit",B_EXIT);
   fTextButton_Exit->Associate(this);
   fTextButton_Exit->SetToolTipText("Button Exit Tip",250);
   buttonFrame->AddFrame(fTextButton_Exit,buttonLayout);

   TGCanvas *canvas = new TGCanvas(vFrame,340,300);
   SetupListTree(canvas);

   vFrame->AddFrame(buttonFrame,frameLayout_x);
   vFrame->AddFrame(canvas,frameLayout_all);
   
   this->AddFrame(vFrame,frameLayout_all);   

   TGLayoutHints* statusBarLayout = new TGLayoutHints( kLHintsBottom|kLHintsLeft|kLHintsExpandX,0, 0, 2, 0 );
   fStatusBar = new TGStatusBar( this, 50, 10, kHorizontalFrame );

   this->AddFrame(fStatusBar,statusBarLayout);
}


void TGRSIViewer::SetupListTree(TGCanvas *canvas) {

   fListTree = new TGListTree(canvas,kHorizontalFrame);
   
   fListTree->AddItem(0,"/grsi");
   TGListTreeItem *fTreeItemGRSI = fListTree->GetFirstItem();
   
   fTreeItemFragment = fListTree->AddItem(fTreeItemGRSI,"fragment");
   fTreeItemAnalysis = fListTree->AddItem(fTreeItemGRSI,"analysis");
   fTreeItemCal      = fListTree->AddItem(fTreeItemGRSI,"cal");
   fTreeItemOdb      = fListTree->AddItem(fTreeItemGRSI,"odb");
   fTreeItemCuts     = fListTree->AddItem(fTreeItemGRSI,"cuts");

   fListTree->OpenItem(fTreeItemGRSI);
   fListTree->Associate(this);  //this allows Process Message to work.

   fListTree->Connect("Clicked(TGListTreeItem*,Int_t,Int_t,Int_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeClicked(TGListTreeItem*,Int_t,Int_t,Int_t)");

   fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t,Int_t,Int_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeDoubleClicked(TGListTreeItem*,Int_t,Int_t,Int_t)");

   fListTree->Connect("KeyPressed(TGListTreeItem*,UInt_t,UInt_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeKeyPressed(TGListTreeItem*,UInt_t,UInt_t)"); 

   fListTree->Connect("ReturnPressed(TGListTreeItem*)",
                      "TGRSIViewer",this,
                      "HandleListTreeReturnPressed(TGListTreeItem*)");




   return;
}

bool TGRSIViewer::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {

 // to see the predefined message types look at 
 // $ROOTSYS/include/WidgetMessageTypes.h
 

   printf("Process Message Called:\n");
   printf("\tmsg:   \t%i\n",GET_MSG(msg));   
   printf("\tsubmsg:\t%i\n",GET_SUBMSG(msg));
   printf("\tparm1: \t%li\n",parm1);   
   printf("\tparm2: \t%li\n",parm2);   

   switch(GET_MSG(msg)) {
      case kC_LISTTREE:
         printf("\t\t\tList Tree action.\n");
         

         break;
      case kC_COMMAND:
         printf("\t\t\tCommand action.\n");
         switch(GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               HandleButtonClick(parm1,parm2);
               break;
            case kCM_MENUSELECT:
               printf("\t\t\t\tMenu highlighted.\n");
               break;
            case kCM_MENU:
               printf("\t\t\t\tMenu item selected.\n");
               break;
            default:
               break;
         };
         break;
      default:
         break;
   }


   return true;
};



TCanvas *TGRSIViewer::MakeNewCanvas(const char* title) {
   unsigned int uui = 0;
   while(fGRSICanvasMap.count(uui)!=0)
      uui++;

   std::string name = Form("canvas_%u",uui);
   if(title)
      name.assign(title);

   TCanvas *canvas = new TCanvas(name.c_str(),name.c_str());
   printf("Canvas made with uui of %u\n",uui);
   canvas->SetUniqueID(uui);
   fGRSICanvasMap.insert(std::make_pair(uui,canvas));

   canvas->Connect("Closed()","TGRSIViewer",this,"GRSICanvasClosed()");
   canvas->Connect("Selected(TPad*,TObject*,Int_t)",
                   "TGRSIViewer",this,
                   "GRSICanvasSelected(TPad*,TObject*,Int_t)");
   canvas->Connect("ProcessEvent(Int_t,Int_t,Int_t,TObject*)",
                   "TGRSIViewer",this,
                   "GRSICanvasProcessEvent(Int_t,Int_t,Int_t,TObject*)");


   fCurrentCanvas = canvas;

   return canvas;
}

void TGRSIViewer::GRSICanvasClosed() {

   GRSICanvasMap::iterator cm_it = fGRSICanvasMap.begin();
   while(cm_it!=fGRSICanvasMap.end()) {
      if(cm_it->second->GetCanvasImp()==0) {
         if(fCurrentCanvas == cm_it->second)
            fCurrentCanvas = 0;
         fGRSICanvasMap.erase(cm_it++);
      }
      else
         cm_it++;
   }
   if(fCurrentCanvas==0) {
      if(fGRSICanvasMap.size()>0)
         fCurrentCanvas = fGRSICanvasMap.rbegin()->second;
   }
   printf("fCurrentCanvas: 0x%08x\n",fCurrentCanvas);
   return;
}


void TGRSIViewer::StartLoadFileDialog() {
   TGFileInfo fileinfo;
   fileinfo.fFilename = 0;
   const char *fileTypes[] ={"FragmentTree files","fragment*.root",
                             "AnalysisTree files","analysis*.root",
                             "Calibration files","*.cal",
                             "XML ODB files","*.xml",
                              "All Root files","*.root",
                             "Any","*",0,0};
   fileinfo.fFileTypes= fileTypes;
   fileinfo.SetMultipleSelection(true);
   new TGFileDialog(fClient->GetRoot(),this,kFDOpen,&fileinfo);

   if(fileinfo.fFileNamesList) {
      for(int x=0;x<fileinfo.fFileNamesList->GetSize();x++)
         HandleFile(((TObjString*)fileinfo.fFileNamesList->At(x))->GetName());
   } else 
      HandleFile(fileinfo.fFilename);

   return;
}

void TGRSIViewer::HandleFile(const char *file) {
   TString filename = file;
  
   printf("\tfile selected: %s\n",filename.Data());
   TSystemFile sfile(filename.Data(),0);

   if(!sfile.IsDirectory() && filename.EndsWith("root")) {
      printf("\t\troot file\n");
      OpenRootFile(filename);
   } else if(filename.EndsWith("cal")) {
      printf("\t\tcal file\n");
   } else if(filename.EndsWith("xml")) {
      printf("\t\txml(odb?) file\n");
   }

   return;
}

void TGRSIViewer::OpenRootFile(const char *file) {
   TFile *infile = TFile::Open(file,"read"); 
   if(!infile->IsOpen()) {
      printf("problem opeinng file.\n");
      return;
   }

   TList *keylist = infile->GetListOfKeys();
   TIter iter(keylist);

   std::string name = file;
   size_t end = name.find_last_of('/');
   if(end != std::string::npos) 
      name = name.substr(end+1,name.length()-end-1);

   while(TKey *key = (TKey*)iter()) {
      if(strcmp((key)->GetClassName(),"TTree")==0) {
         TTree *tree = (TTree*)key->ReadObj();
         if(strcmp(tree->GetName(),"FragmentTree")==0)
            AddFragmentTree(infile,tree);
         else if(strcmp(tree->GetName(),"AnalysisTree")==0)
            AddAnalysisTree(infile,tree);
      }

   }

   return;
}


void TGRSIViewer::AddFragmentTree(TFile *infile, TTree *tree) {  
   bool InitChain = false;
   if(!fFragmentChain) {
      fFragmentChain = new TChain("FragmentTree");
      AddToListTree("FragmentChain",fTreeItemFragment,fFragmentChain);
      InitChain = true;
   }


   TObjArray *fileElements=fFragmentChain->GetListOfFiles();
   TIter nextElement(fileElements);
   while (TChainElement *chEl = (TChainElement*)nextElement() ) {
      if(strcmp(chEl->GetTitle(),infile->GetName())==0) {
         return;  // already been added to the chain.  get out!
      }
   }
   fFragmentChain->Add(infile->GetName());



   std::string name = infile->GetName(); 
   size_t found = name.find_last_of('/');
   if(found != std::string::npos) 
      name = name.substr(found+1,name.length()-found-1);
   found = name.find_last_of('.');
   if(found != std::string::npos) 
      name = name.substr(0,name.length()-5);

   TGListTreeItem *item = AddToListTree(name.c_str(),fTreeItemFragment,tree);

   int nbranches = tree->GetNbranches();
   for(int x=0;x<nbranches;x++) {
      AddBranchToListTree((TBranch*)tree->GetListOfBranches()->At(x),
                          item);
      if(InitChain) {
         AddBranchToListTree((TBranch*)tree->GetListOfBranches()->At(x),
                             fListTree->FindChildByName(fTreeItemFragment,"FragmentChain"));
      }   
   }
   


   fListTree->OpenItem(fTreeItemFragment);

}

void TGRSIViewer::AddAnalysisTree(TFile *infile, TTree *tree) {  

   bool InitChain = false;
   if(!fAnalysisChain) {
      fAnalysisChain = new TChain("AnalysisTree");
      AddToListTree("AnalysisChain",fTreeItemAnalysis,fAnalysisChain);
      InitChain = true;
   }

   TObjArray *fileElements=fAnalysisChain->GetListOfFiles();
   TIter nextElement(fileElements);
   while (TChainElement *chEl = (TChainElement*)nextElement() ) {
      if(strcmp(chEl->GetTitle(),infile->GetName())==0) {
         return;  // already been added to the chain.  get out!
      }
   }
   fAnalysisChain->Add(infile->GetName());

   std::string name = infile->GetName(); 
   size_t found = name.find_last_of('/');
   if(found != std::string::npos) 
      name = name.substr(found+1,name.length()-found-1);
   found = name.find_last_of('.');
   if(found != std::string::npos) 
      name = name.substr(0,name.length()-5);

   TGListTreeItem *item = AddToListTree(name.c_str(),fTreeItemAnalysis,tree);

   int nbranches = tree->GetNbranches();
   for(int x=0;x<nbranches;x++) {
      AddBranchToListTree((TBranch*)tree->GetListOfBranches()->At(x),
                          item);
      if(InitChain) {
         AddBranchToListTree((TBranch*)tree->GetListOfBranches()->At(x),
                             fListTree->FindChildByName(fTreeItemAnalysis,"AnalysisChain"));
      }   
   }
   


   fListTree->OpenItem(fTreeItemAnalysis);




}


void *TGRSIViewer::AddBranchToListTree(TBranch *branch,TGListTreeItem *parent) {
   int Nentries = branch->GetListOfBranches()->GetEntries();
   if(Nentries==0) {
      //printf("branch->GetListOfLeaves()->GetEntries() = %i\n",
      //        branch->GetListOfLeaves()->GetEntries());
      TLeaf *leaf = (TLeaf*)branch->GetListOfLeaves()->At(0);//(TLeaf*)branch;
      AddToListTree(leaf->GetName(),parent,leaf);
   } else {
      TGListTreeItem *branchItem = AddToListTree(branch->GetName(),parent,branch);
      for(int x=0;x<Nentries; x++) {
         TBranch *branch2 = (TBranch*)branch->GetListOfBranches()->At(x);
         AddBranchToListTree(branch2,branchItem);
      }
   }
}

void TGRSIViewer::CloseViewer(Option_t *opt) {
   gApplication->Terminate();
   return;
}



TGListTreeItem *TGRSIViewer::AddToListTree(const char *name,TGListTreeItem *parent,TObject *object) {

   std::string pic1 = "folder_t.xpm";
   std::string pic2 = "ofolder_t.xpm";
   bool checked = false;

   if(object->InheritsFrom("TChain")) {
      pic1.assign("chain_t.xpm");
      pic2 = pic1;
   } else if(object->InheritsFrom("TTree")) {
      pic1.assign("tree_t.xpm");
      pic2 = pic1;
   } else if(object->InheritsFrom("TLeaf")) {
      pic1.assign("leaf_t.xpm");
      pic2 = pic1;
   } else if(object->InheritsFrom("TBranch")) {
      pic1.assign("branch_t.xpm");
      pic2 = pic1;
   }

   TGListTreeItem *item = fListTree->AddItem(parent,name,(void*)object,
                                             gClient->GetPicture(pic1.c_str()),
                                             gClient->GetPicture(pic2.c_str()),checked);
   
   return item;
}



bool TGRSIViewer::HandleButtonClick(Long_t parm1, Long_t parm2) {
   printf("\t\t\t\tButton action.\n");
   switch(parm1) {
      case B_LOAD:
         printf("\t\t\t\tLoad!\n");
         StartLoadFileDialog();
         break;
      case B_TWO:
         printf("\t\t\t\tTwo!\n");
         break;
      case B_EXIT:
         printf("\t\t\t\tExit!\n");
         CloseViewer();
         break;
      case B_DRAW:
         printf("\t\t\t\tDraw!\n");
         MakeNewCanvas();
         break;
      deafualt:
         break;
   };
   return true;
}

void TGRSIViewer::HandleListTreeClicked(TGListTreeItem *entry,Int_t btn ,Int_t x ,Int_t y) {
   //printf("entry->GetText(): %s\n",entry->GetText());
   //printf("((TObject*)entry->GetUserData())->GetName(): %s\n",((TObject*)entry->GetUserData())->GetName());
   
   return;
}

void TGRSIViewer::HandleListTreeDoubleClicked(TGListTreeItem *entry,Int_t btn,Int_t x,Int_t y) {
/*

   printf(DBLUE "entry->GetText(): %s" RESET_COLOR "\n",entry->GetText());
   printf(DBLUE "((TObject*)entry->GetUserData())->GetName(): %s" RESET_COLOR "\n",((TObject*)entry->GetUserData())->GetName());
   
   TObject *obj = (TObject*)entry->GetUserData();

   if(!obj)
      return;

   //TCanvas *c;
   //if(!fCurrentCanvas)
   //   c = MakeNewCanvas();
   //else 
   //   c = fCurrentCanvas;

   if(obj->InheritsFrom("TLeaf")) {
      TGListTreeItem *item = entry->GetParent();
      if(!item)
         return;
      while(!((TObject*)item->GetUserData())->InheritsFrom("TTree")) {
         item = item->GetParent();
         if(!item)
            return;   
      }

   printf(DRED "item->GetText(): %s" RESET_COLOR "\n",item->GetText());
   printf(DRED "((TObject*)item->GetUserData())->GetName(): %s" RESET_COLOR "\n",((TObject*)item->GetUserData())->GetName());

      TObject *parent_obj = (TObject*)item->GetUserData();

      TCanvas *c = MakeNewCanvas();
      c->cd();
      if(parent_obj->InheritsFrom("TChain")) {
        TChain *chain = (TChain*)parent_obj;       
        chain->Draw(Form("%s>>hist(16000,0,16000)",obj->GetName()),
                    Form("%s>0",obj->GetName()),
                    Form("goff"));
        TH1F *hist = ((TH1F*)gDirectory->FindObjectAny("hist"));
        hist = (TH1F*) hist->Clone(hist->GetTitle());
        hist->Draw();
      } else if (parent_obj->InheritsFrom("TTree")) {
        TTree *tree = (TTree*)parent_obj;       

      }
   }
*/

   return;
}


void TGRSIViewer::HandleListTreeKeyPressed(TGListTreeItem *entry,UInt_t keysym,UInt_t mask) {
   printf(DYELLOW "keysym = %i  |  mask = 0x%08x" RESET_COLOR "\n",keysym,mask);

   return;
}

void TGRSIViewer::HandleListTreeReturnPressed(TGListTreeItem *entry) {
   printf(DYELLOW "return key pressed" RESET_COLOR "\n");
   return;
}


void TGRSIViewer::GRSICanvasSelected(TPad *selpad, TObject *selected, Int_t event) {

   return;
}


void TGRSIViewer::GRSICanvasProcessEvent(Int_t event,Int_t x,Int_t y,TObject *selected) {

   return;
}



