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
  scaler *s = new scaler(&myCont,0xCCCC00);
  
  myTTC->changeChannel(7);
  cout<<"Resting counters...";
  myTTC->resetCounter(); 
  cout<<"done !"<<endl;
  cout<<"Nevents = "<<myTTC->getEventNumber()<<endl;
  cout<<"Starting trigger..."<<endl;
  myTTC->changeChannel(1);
  for (int i=0; i<2e6; i++){
      cout<<"Nevents = "<<myTTC->getEventNumber()<<endl;
  }
}

