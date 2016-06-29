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

int main(){
  
  
  // Start USB controller 
  UsbController myCont(NORMAL);

  //Set correct discriminator settings
  discri * d = new discri(&myCont);
  d->setMultiChannel(0b111);
  d->setTh(5);
  //d->setWidth(126);
  d->setWidth(200);
  d->setMajority(3);

  //Set correct TTC channel
  ttcVi *myTTC = new ttcVi(&myCont);
  myTTC->changeChannel(2);//1=VME 2=NIM





  return(0);
  
}

