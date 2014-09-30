
#ifndef TGRSIHISTOMANGER_H
#define TGRSIHISTOMANGER_H

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <deque>


#include <TObject.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCut.h>

class GRSIHistInfo {
   public:
      GRSIHistInfo() : name(""),title(""),dimension(1) { }
      ~GRSIHistInfo() { };
      GRSIHistInfo(const char *n="",const char *t="") : name(n),title(t),dimension(1) { }
      std::string name;
      std::string title;
      std::vector<TCut> cuts;
      int dimension;
      std::deque<Int_t> xmarkers;  
};



class TGRSIHistManager : public TObject {

   public:
      TGRSIHistManager();
      virtual ~TGRSIHistManager();

      TH1 *FindHistByName(const char*);
      TH1 *FindHistByTitle(const char*);

      void InsertHist(TObject*,Option_t *opt="");

      void RemoveHist(TObject*,Option_t *opt="");
      void RemoveAll(Option_t *opt);

      void AddXMarker(const char* name, Int_t bin);

      int Size() { return fGRSIHistMap.size(); }

   private:
      typedef std::map<GRSIHistInfo*,TObject*> GRSIHistMap;
      GRSIHistMap fGRSIHistMap;

      void ReplaceHist(GRSIHistInfo*,TObject*);

      void DrawXMarker(Int_t bin, TH1* hist);
      void UndrawXMarker(Int_t bin, TH1* hist);


      GRSIHistInfo *FindHistInfoByName(const char*);
      GRSIHistInfo *FindHistInfoByTitle(const char*);      

   private:
      static std::map<TH1*,TGRSIHistManager*> MasterHistManagerMap;

   public:
      static TGRSIHistManager* GetHistManagerFromHist(TH1* hist);

   ClassDef(TGRSIHistManager,0)
};

#endif

