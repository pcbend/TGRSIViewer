
#include <cstdio>
#include <string>



#include <TApplication.h>

#include <TGRSIViewer.h>

int main(int argc,char **argv) {

   TApplication *grsi = new TApplication("TGRSIViewer",0,0);
   TGRSIViewer *view   = new TGRSIViewer();
   grsi->Run("true");
   return 0;

}
