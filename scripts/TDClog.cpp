#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/TDC.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include<ctime>

int main(){
  
  
  
  UsbController myCont(DEBUG);
  myCont.reset();
  tdc * myTDC = new tdc(&myCont,0x00AA0000);
  
  cout<<"----------------------------------------------------"<<endl;

  myTDC->Reset();
  myTDC->Reset();
  myTDC->Reset();
  myTDC->Reset();
  myCont.reset();
  myTDC->ReadStatus();
  cout<<"Wut?"<<endl;
  return(0);
  myTDC->setChannelNumbers(1,23);
  myTDC->setWindowWidth(50);
  myTDC->setWindowOffset(-25);
  myTDC->setExSearchMargin(1);
  myTDC->setRejectMargin(1);
  myTDC->readWindowConfiguration();
  myTDC->enableFIFO();
  myTDC->disableTDCHeaderAndTrailer();
  myTDC->readResolution();
  myTDC->getDetectConf();
  myTDC->setDetectConf(3);
  myTDC->getDetectConf();

  for(int i=0;i<5;i++)
  {
    cout<<"Loooopin' "<<i<<endl;
    event e;
    myTDC->getEvent(e);
    cout<<"Got event!"<<endl;
    myTDC->coutEvent(e);
    myTDC->analyseEvent(e,"coucou");
    sleep(1); 
  }
  
 
  
  return(0);
  
}

