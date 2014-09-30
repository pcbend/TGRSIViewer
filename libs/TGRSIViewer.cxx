


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

#include <TQObject.h>

#include <KeySymbols.h>
#include <GuiTypes.h>
#include <WidgetMessageTypes.h>


ClassImp(TGRSIViewer)

GRSICanvasMap TGRSIViewer::fGRSICanvasMap;
TGRSIHistManager *TGRSIViewer::fGRSIHistManager = new TGRSIHistManager;

TCanvas *TGRSIViewer::fCurrentCanvas = 0;
TChain  *TGRSIViewer::fFragmentChain = 0;
TChain  *TGRSIViewer::fAnalysisChain = 0;

TGRSIViewer::TGRSIViewer(const TGWindow *p,UInt_t w,UInt_t h) 
            :TGMainFrame(p?p:gClient->GetRoot(),w,h)  {
   fCurrentPad = 0;

   SetCleanup(kDeepCleanup);

   SetWindowName("TGRSIViewer");              

   this->Connect("CloseWindow()","TGRSIViewer",this,"CallCloseViewer()");
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
   TGPopupMenu *menuTools = new TGPopupMenu(fClient->GetRoot());
   menuTools->AddEntry("&Hammer",M_TOOLS_HAMMER);
   menuTools->AddEntry("&Wrench",M_TOOLS_WRENCH);
   menuTools->Associate(this);
   
   TGLayoutHints* menuBarLayout_all   = new TGLayoutHints( kLHintsNormal|kLHintsExpandX, 0, 0, 0, 10 );
   TGLayoutHints* menuBarLayout_left  = new TGLayoutHints( kLHintsTop|kLHintsLeft , 5, 0, 0, 0 );
   TGLayoutHints* menuBarLayout_right = new TGLayoutHints( kLHintsTop|kLHintsRight, 0, 5, 0, 0 );

   TGMenuBar *menuBar = new TGMenuBar( this, 1, 1, kRaisedFrame );
   menuBar->AddPopup("&File",menuFile,menuBarLayout_left);
   menuBar->AddPopup("&Help",menuFile,menuBarLayout_right);
   menuBar->AddPopup("&Tools",menuTools,menuBarLayout_left);

   this->AddFrame(menuBar,menuBarLayout_all);
}

void TGRSIViewer::LayoutGUI()       { 
   
   TGLayoutHints* frameLayout_all = new TGLayoutHints( kLHintsTop|kLHintsExpandX|kLHintsExpandY, 5, 5, 5, 5 );
   TGLayoutHints* frameLayout_x = new TGLayoutHints( kLHintsTop|kLHintsExpandX, 0, 0, 0, 0 );
   TGLayoutHints* frameLayout_y = new TGLayoutHints( kLHintsTop|kLHintsExpandY, 0, 0, 0, 0 );

   TGHorizontalFrame *hFrame      = new TGHorizontalFrame(this,10,10);

   /*************************************************/
   TGVerticalFrame   *vFrame      = new TGVerticalFrame(hFrame,10,10);
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
   /*************************************************/

  // TGVerticalFrame *vLVFrame   = new TGVerticalFrame(hFrame,10,10,kSunkenFrame,TGFrame::GetWhitePixel());
   //TGCanvas *fLVcanvas = new TGCanvas(vLVFrame,340,300);
   //SetupListView(fLVcanvas);
 //  fListView = new TGListView(vLVFrame,340,300);
 //  vLVFrame->AddFrame(fListView,frameLayout_all);


   hFrame->AddFrame(vFrame,frameLayout_all);
 //  hFrame->AddFrame(vLVFrame,frameLayout_all);

   this->AddFrame(hFrame,frameLayout_all);   

   TGLayoutHints* statusBarLayout = new TGLayoutHints( kLHintsBottom|kLHintsLeft|kLHintsExpandX,0, 0, 2, 0 );
   fStatusBar = new TGStatusBar( this, 50, 10, kHorizontalFrame );



  Int_t parts[] = { 26, 74 };
     fStatusBar->SetParts(parts, 2);


   this->AddFrame(fStatusBar,statusBarLayout);
}


void TGRSIViewer::SetupListTree(TGCanvas *canvas) {

   fListTree = new TGListTree(canvas,kHorizontalFrame);
   
   fListTree->AddItem(0,"/grsi");
   TGListTreeItem *fTreeItemGRSI = fListTree->GetFirstItem();
   fListTree->SetUserControl(true);

   fTreeItemFragment = fListTree->AddItem(fTreeItemGRSI,"fragment");
   fTreeItemAnalysis = fListTree->AddItem(fTreeItemGRSI,"analysis");
   fTreeItemCal      = fListTree->AddItem(fTreeItemGRSI,"cal");
   fTreeItemOdb      = fListTree->AddItem(fTreeItemGRSI,"odb");
   fTreeItemCuts     = fListTree->AddItem(fTreeItemGRSI,"cuts");

   fListTree->OpenItem(fTreeItemGRSI);
   //fListTree->Associate(this);  //this allows Process Message to work.

   fListTree->Connect("Clicked(TGListTreeItem*,Int_t,Int_t,Int_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeClicked(TGListTreeItem*,Int_t,Int_t,Int_t)");

   fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t,Int_t,Int_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeDoubleClicked(TGListTreeItem*,Int_t,Int_t,Int_t)");

   fListTree->Connect("KeyPressed(TGListTreeItem*,ULong_t,ULong_t)",
                      "TGRSIViewer",this,
                      "HandleListTreeKeyPressed(TGListTreeItem*,ULong_t,ULong_t)"); 

   //fListTree->Connect("ReturnPressed(TGListTreeItem*)",
   //                   "TGRSIViewer",this,
   //                   "HandleListTreeReturnPressed(TGListTreeItem*)");
   return;
}



void TGRSIViewer::SetupListView(TGCanvas *canvas) {
   //fListView = new TGListView(canvas,kHorizontalFrame);





   return;
}


TGRSIHistManager *TGRSIViewer::GetHistManager(TGListTreeItem *item) {
   TGRSIHistManager *ghm = 0;
   if(!item)
      return ghm;
   if(fHistMap.count(item) == 0) {
      ghm = new TGRSIHistManager();
      fHistMap.insert(std::make_pair(item,ghm));
   }
   else
      ghm = fHistMap.at(item);
   return ghm;   
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
         switch(GET_SUBMSG(msg)) {
            case kCT_KEY:
               printf(DYELLOW "\t\t\tsubmsg: %i" RESET_COLOR "\n",GET_SUBMSG(msg));
               break;
         };
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
               HandleMenuClick(parm1,parm2);
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
   //canvas->Connect("Picked(TPad*,TObject*,Int_t)",
   //                "TGRSIViewer",this,
   //                "GRSICanvasSelected(TPad*,TObject*,Int_t)");
   canvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
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
      fCurrentPad = 0;
      if(fGRSICanvasMap.size()>0) {
         fCurrentCanvas = fGRSICanvasMap.rbegin()->second;
      }
   }
   printf("fCurrentCanvas: 0x%p\n",fCurrentCanvas);
   return;
}


void TGRSIViewer::StartLoadFileDialog() {
   TGFileInfo fileinfo;
   fileinfo.fFilename = 0;
   const char *fileTypes[] ={
      
                             "All Root files","*.root",
                             "FragmentTree files","fragment*.root",
                             "AnalysisTree files","analysis*.root",
                             "Calibration files","*.cal",
                             "XML ODB files","*.xml",
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

   bool treefile = false;
   while(TKey *key = (TKey*)iter()) {
      if(strcmp((key)->GetClassName(),"TTree")==0) {
         TTree *tree = (TTree*)key->ReadObj();
         if(strcmp(tree->GetName(),"FragmentTree")==0) {
            treefile = true;
            AddFragmentTree(infile,tree);
         }
         else if(strcmp(tree->GetName(),"AnalysisTree")==0) {
            treefile = true;
            AddAnalysisTree(infile,tree);
         }
      }
   }
   
   if(treefile)
      return;
   if(fListTree->FindItemByPathname(Form("/%s",name.c_str()))) {
      printf("file alreadyed added.\n");
      return;
   }
   TGListTreeItem *parent = AddToListTree(name.c_str(),0,infile);
   iter.Reset();
   while(TKey *key = (TKey*)iter()) {
      printf("key->GetClassName() = %s\n\n",key->GetClassName());
      if(strcmp(key->GetClassName(),"TFolder")==0) {
         AddTFolder((TFolder*)key->ReadObj(),parent);
      }
      else if(strcmp(key->GetClassName(),"TDirectoryFile")==0) {
         AddTDirectory((TDirectory*)key->ReadObj(),parent);
      } else {
         TObject *obj = (TObject*)key->ReadObj();
         AddToListTree(obj->GetName(),parent,obj);
      }
   }


   return;
}

void TGRSIViewer::AddTFolder(TFolder *folder, TGListTreeItem *parent) {
   TIter iter(folder->GetListOfFolders());
   while(TObject *obj = (TObject*)iter()) {
      TGListTreeItem *item = AddToListTree(obj->GetName(),parent,obj);
      if(strcmp(obj->IsA()->GetName(),"TFolder")==0) 
         AddTFolder((TFolder*)obj,item);
      else if(obj->InheritsFrom("TDirectory"))
         AddTDirectory((TDirectory*)obj,item);
      } 
}

void TGRSIViewer::AddTDirectory(TDirectory *directory, TGListTreeItem *parent) {
   TGListTreeItem *item = AddToListTree(directory->GetName(),parent,directory);
   TIter iter(directory->GetListOfKeys());
   //printf("directory->GetListOfKeys()->GetSize() = %i\n",directory->GetListOfKeys()->GetSize());
   while(TKey *key = (TKey*)iter()) {
      TObject *obj = (TObject*)key->ReadObj();
      if(obj->InheritsFrom("TDirectory"))
         AddTDirectory((TDirectory*)obj,item);
      else if(strcmp(obj->IsA()->GetName(),"TFolder")==0)
         AddTFolder((TFolder*)obj,item);
      else
         AddToListTree(obj->GetName(),item,obj);
   }
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
   fListTree->HighlightItem(fTreeItemFragment);
   return;
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
   fListTree->HighlightItem(fTreeItemAnalysis);
   return;
}


void TGRSIViewer::AddBranchToListTree(TBranch *branch,TGListTreeItem *parent) {
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

void TGRSIViewer::CallCloseViewer(){
   CloseViewer();
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
   } else if(object->InheritsFrom("TFile")) {
      pic1.assign("rootdb_t.xpm");
      pic2 = pic1;
   //} else if(object->InheritsFrom("TDirectoryFile")) {
   } else if(object->InheritsFrom("TH2")) {
      pic1.assign("h2_t.xpm");
      pic2 = pic1;  
      TGRSIHistManager *ghm = GetHistManager(parent);
      ghm->InsertHist(object);
   } else if(object->InheritsFrom("TH1")) {
      pic1.assign("h1_t.xpm");
      pic2 = pic1;  
      TGRSIHistManager *ghm = GetHistManager(parent);
      ghm->InsertHist(object);
   } 

   


   TGListTreeItem *item = fListTree->AddItem(parent,name,(void*)object,
                                             gClient->GetPicture(pic2.c_str()),
                                             gClient->GetPicture(pic1.c_str()),checked);
   
   return item;
}


bool TGRSIViewer::HandleMenuClick(Long_t parm1, Long_t parm2){
   printf("\t\t\tMenu action.\n");
   switch(parm1) {
      case M_FILE_EXIT:
         printf("\t\t\t\tExit!\n");
         CloseViewer();
         break;
      default:
         break;
   };
   return true;
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
      default:
         break;
   };
   return true;
}

void TGRSIViewer::HandleListTreeClicked(TGListTreeItem *entry,Int_t btn ,Int_t x ,Int_t y) {
   //printf("entry->GetText(): %s\n",entry->GetText());
   //printf("((TObject*)entry->GetUserData())->GetName(): %s\n",((TObject*)entry->GetUserData())->GetName());
   
   fListTree->HighlightItem(entry);
   return;
}

void TGRSIViewer::HandleListTreeDoubleClicked(TGListTreeItem *entry,Int_t btn,Int_t x,Int_t y) {
   //return;
   printf("entry->GetText() = %s\n", entry->GetText());

   TObject *object = (TObject*)entry->GetUserData();
   if(!object )
      return;

   if(object->InheritsFrom("TH1")) {
      DrawHist(entry);
   }

   if(object->InheritsFrom("TLeaf")) {
      printf("TLEAF found.\n");
      
      TGListTreeItem *parent = entry->GetParent();
      if(!parent)
         return;
      TGRSIHistManager *ghm = GetHistManager(entry->GetParent());
      if(!ghm->FindHistByName(object->GetName())) {
         while(!((TObject*)parent->GetUserData())->InheritsFrom("TTree")) {
            parent = parent->GetParent();
            if(!parent)
               return;   
         }
         TObject *parent_obj = (TObject*)parent->GetUserData();
         if(parent_obj->InheritsFrom("TChain")) {
         TChain *chain = (TChain*)parent_obj; 
         chain->Draw(Form("%s>>%s(64000,0,64000)",object->GetName(),object->GetName()),
                     Form("%s>0",object->GetName()),
                     Form("goff"));
                     TObject *newhist = gDirectory->FindObjectAny(Form("%s",object->GetName()));
                     printf("FindObjectAny: 0x%p\n",gDirectory->FindObjectAny(Form("%s",object->GetName())));
         ghm->InsertHist(newhist);
        }
      }
      DrawHist(entry);
   }

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

void TGRSIViewer::DrawHist(TGListTreeItem *entry) {
   if(!entry)
      return;
   TGRSIHistManager *ghm = GetHistManager(entry->GetParent());
   TObject *object = (TObject*)entry->GetUserData();
   TH1 *hist = ghm->FindHistByName(object->GetName()); //entry->GetText()??
   if(!hist)
      return;
   TCanvas *c = MakeNewCanvas();
   c->cd();
   hist->Draw();
   return;
}


void TGRSIViewer::HandleListTreeKeyPressed(TGListTreeItem *entry,ULong_t keysym,ULong_t mask) {
   printf(DYELLOW "keysym = %lu  |  mask = 0x%08lx" RESET_COLOR "\n",keysym,mask);
   switch(keysym) {
      case kKey_Return:
         fListTree->SetEventHandled(true);
         fListTree->HighlightItem(entry);
         if(entry->IsOpen()){printf("opened -> close\n"); fListTree->CloseItem(entry); }
         else               {printf("closed -> open \n"); fListTree->OpenItem(entry);  }
         //HandleListTreeDoubleClicked(entry,0,0,0);
         break;
      case kKey_Space:
         fListTree->SetEventHandled(true);
         fListTree->HighlightItem(entry);
         if(entry->IsOpen()){printf("opened -> close\n"); fListTree->CloseItem(entry); }
         else               {printf("closed -> open \n"); fListTree->OpenItem(entry);  }
         break;
      case kKey_Up:
         fListTree->SetEventHandled(true);
         fListTree->LineUp(true);
         entry = fListTree->GetCurrent();
         fListTree->HighlightItem(entry);
         break;
      case kKey_Down:
         fListTree->SetEventHandled(true);
         fListTree->LineDown(true);
         entry = fListTree->GetCurrent();
         fListTree->HighlightItem(entry);
         break;
   };
   //gClient->NeedRedraw(this);
   return;
}

void TGRSIViewer::HandleListTreeReturnPressed(TGListTreeItem *entry) {
   printf(DYELLOW "return key pressed" RESET_COLOR "\n");
   return;
}


void TGRSIViewer::GRSICanvasSelected(TPad *selpad, TObject *selected, Int_t event) {
   fCurrentPad = selpad;
   printf("\t\tSelecting Pad = 0x%08x\t\t\n",fCurrentPad);
   return;
}

   /*enum EEventType {
    *kNoEvent       =  0,
    *kButton1Down   =  1, kButton2Down   =  2, kButton3Down   =  3, kKeyDown  =  4,
    *kButton1Up     = 11, kButton2Up     = 12, kButton3Up     = 13, kKeyUp    = 14,
    *kButton1Motion = 21, kButton2Motion = 22, kButton3Motion = 23, kKeyPress = 24,
    *kButton1Locate = 41, kButton2Locate = 42, kButton3Locate = 43,
    *kMouseMotion   = 51, kMouseEnter    = 52, kMouseLeave    = 53,
    *kButton1Double = 61, kButton2Double = 62, kButton3Double = 63
    *
    *enum EGEventType {
    *kGKeyPress, kKeyRelease, kButtonPress, kButtonRelease,
    *kMotionNotify, kEnterNotify, kLeaveNotify, kFocusIn, kFocusOut,
    *kExpose, kConfigureNotify, kMapNotify, kUnmapNotify, kDestroyNotify,
    *kClientMessage, kSelectionClear, kSelectionRequest, kSelectionNotify,
    *kColormapNotify, kButtonDoubleClick, kOtherEvent*/


void TGRSIViewer::GRSICanvasProcessEvent(Int_t event,Int_t x,Int_t y,TObject *selected) {

 //  if(!selected)
 //     return;
   //gPad->GetCanvas()->FeedbackMode(kTRUE);
   //printf("gPad->GetListOfPrimitives() = 0x%08x\n",gPad->GetListOfPrimitives());

   //if(selected)
      //printf("selected->GetName() = %s\n",selected->GetName());

   if(!selected)
      return;

   TCanvas *canvas = (TCanvas*)gTQSender;

   //printf("canvas->GetName() = %s\n",canvas->GetName());

   //printf("event = %i %i %i\n",event,x,y);

   TH1 *hist1 = 0;
   switch(event){
      case kButton1Down: {
         fStatusBar->SetText(Form("x = %d , y = %d ",x,y));
         //printf("What you clicked on was a %d
            TIter iter(canvas->GetListOfPrimitives());
         TGRSIHistManager *ghm = 0;
         while(TObject *obj = iter.Next()){
            if(obj->InheritsFrom("TH1")){
               ghm = TGRSIHistManager::GetHistManagerFromHist((TH1*)obj);
               hist1 = ghm->FindHistByName(obj->GetName());
               //printf("The object address: %p\n",hist1);
            }
         }
         if (hist1){
            //printf("You are clicking on histogram: %s\n",hist1->GetName());
            Float_t ux = gPad->AbsPixeltoX(x);
            Float_t binx = gPad->PadtoX(ux);
            ghm->AddXMarker(hist1->GetName(),binx);
         }
         }
         break;
      case kKeyPress:
         HandleKeyPressInHist(hist1,x,y);
         break;
   };


   return;
}


void TGRSIViewer::HandleKeyPressInHist(TH1 *hist,Int_t key,Int_t keysym) {
   switch(keysym) {
      case kKey_Down:
         printf("down key pressed.\n");
         break;
   };

} 

