#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include<ctime>

long int getDT(struct timeval start,struct timeval stop){
//    cout<<"Start = "<<start.tv_sec<<"."<<start.tv_usec<<endl;
//    cout<<"Stop  = "<<stop.tv_sec<<"."<<stop.tv_usec<<endl;
    return(1000*(stop.tv_sec - start.tv_sec) + (stop.tv_usec - start.tv_usec)/1000);
}

int main(){
  
  
  
  UsbController myCont(NORMAL);
  ttcVi *myTTC = new ttcVi(&myCont);
  
  cout<<"Starting..."<<endl;

  for (int i=0; i<50; i++){
      struct timeval start,stop;
      int nStart = myTTC->getEventNumber();
      gettimeofday(&start,NULL);

      sleep(5);
      int nStop = myTTC->getEventNumber();
      gettimeofday(&stop,NULL);
      long int dT = getDT(start,stop);
      cout<<"Ntrig ="<<nStop-nStart<<"    Time = "<<dT<<"ms "<<"    -> rate = "<<1000*(nStop-nStart)/(1.*dT)<<"Hz"<<endl;
  }
}

