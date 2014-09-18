
#ifndef TGRSIHISTOMANGER_H
#define TGRSIHISTOMANGER_H

#include <cstdio>
#include <string>
#include <vector>
#include <map>



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

      int Size() { return fGRSIHistMap.size(); }

   private:
      typedef std::map<GRSIHistInfo*,TObject*> GRSIHistMap;
      GRSIHistMap fGRSIHistMap;

      void ReplaceHist(GRSIHistInfo*,TObject*);

      GRSIHistInfo *FindHistInfoByName(const char*);
      GRSIHistInfo *FindHistInfoByTitle(const char*);

   ClassDef(TGRSIHistManager,0)
};

#endif

