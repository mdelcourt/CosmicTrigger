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

int getDT(struct timeval start,struct timeval stop){
    return(1000*(stop.tv_sec - start.tv_sec) + stop.tv_usec - start.tv_usec);
}

int main(){
  
  
  
  UsbController myCont(NORMAL);
  ttcVi *myTTC = new ttcVi(&myCont);
  scaler *s = new scaler(&myCont,0xCCCC00);
  
  myTTC->changeChannel(-1);
  myTTC->changeRandomFrequency(1);

  myTTC->resetCounter(); 
  cout<<"Starting..."<<endl;
  for (int i=0; i<10; i++){
      int nStart = myTTC->getEventNumber();
      struct timeval start,stop;
      gettimeofday(&start,NULL);
      sleep(2);
      gettimeofday(&stop,NULL);
      int nStop = myTTC->getEventNumber();
      int dT = getDT(start,stop);
      cout<< "nStart = "<<nStart<<"; nStop = "<<nStop<<endl;
      cout<< "Time = "<<dT<<"ms "<<"rate = "<<(nStop-nStart)/(1.*dT)<<"kHz"<<endl;
  }
}

