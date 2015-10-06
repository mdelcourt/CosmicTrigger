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
  
  
  
  UsbController myCont(DEBUG);

  hv * hvPMT = new hv(&myCont,0xF0000,2);
  hvPMT->setChV(1025,0);
  hvPMT->setChV(925,1);
  hvPMT->setChV(1225,2);

  hvPMT->setChState(1,0);
  hvPMT->setChState(1,1);
  hvPMT->setChState(1,2);
  hvPMT->setChState(0,3);
  return(0);
  
}

