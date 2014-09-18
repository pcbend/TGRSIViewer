


#include <TGRSIHistManager.h>

ClassImp(TGRSIHistManager)

TGRSIHistManager::TGRSIHistManager() { }

TGRSIHistManager::~TGRSIHistManager() { }


void TGRSIHistManager::InsertHist(TObject *obj,Option_t *opt) {
   if(!obj || !obj->InheritsFrom("TH1"))
      return;

   obj = obj->Clone();

   TH1 *hist = (TH1*)obj;
   GRSIHistInfo *info = 0;
   if(info = FindHistInfoByName(hist->GetName())) {
      ReplaceHist(info,hist);
   } else if (info = FindHistInfoByTitle(hist->GetTitle())) {
      ReplaceHist(info,hist);
   }
   
   info = new GRSIHistInfo(hist->GetName(),hist->GetTitle());
   if(hist->InheritsFrom("TH3"))
      info->dimension = 3;
   else if(hist->InheritsFrom("TH2"))
      info->dimension = 2;

   fGRSIHistMap.insert(std::make_pair(info,hist));
   return;
}


void TGRSIHistManager::RemoveHist(TObject *obj, Option_t *opt) {
   if(!obj || !obj->InheritsFrom("TH1"))
      return;
   GRSIHistInfo *info = 0;
   TH1 *hist = (TH1*)obj;
   if(info = FindHistInfoByName(hist->GetName())) {  } 
   else if (info = FindHistInfoByTitle(hist->GetTitle())) {   }
   
   if(info) {
      fGRSIHistMap.erase(info);
      delete info;
      hist->Delete();
   };

   return;
}

void TGRSIHistManager::RemoveAll(Option_t *opt) {
   GRSIHistMap::iterator it;   
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
      TObject *obj = it->second;
      if(obj) obj->Delete();
      if(it->first)  delete it->first;
   }
   fGRSIHistMap.clear();
   return;
}

void TGRSIHistManager::ReplaceHist(GRSIHistInfo *info,TObject *obj) {
   if(!info || !obj)
      return;
   TObject *toremove = fGRSIHistMap.at(info);
   if(toremove)
      toremove->Delete();
   fGRSIHistMap.at(info) = obj;
   return;
}

GRSIHistInfo *TGRSIHistManager::FindHistInfoByName(const char *cc_name) {
   std::string name = cc_name;
   if(name.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   GRSIHistInfo *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(name.compare(it->first->name)==0) {
        found = it->first;  
        break;
     }
   }
   return found;
}

GRSIHistInfo *TGRSIHistManager::FindHistInfoByTitle(const char *cc_title) {
   std::string title = cc_title;
   if(title.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   GRSIHistInfo *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(title.compare(it->first->title)==0) {
        found = it->first;  
        break;
     }
   }
   return found;
}


TH1 *TGRSIHistManager::FindHistByName(const char *cc_name) {
   std::string name = cc_name;
   if(name.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   TObject *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(name.compare(it->first->name)==0) {
        found = it->second;  
        break;
     }
   }
   return (TH1*)found;
}

TH1 *TGRSIHistManager::FindHistByTitle(const char *cc_title) {
   std::string title = cc_title;
   if(title.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   TObject *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(title.compare(it->first->title)==0) {
        found = it->second;  
        break;
     }
   }
   return (TH1*)found;
}








