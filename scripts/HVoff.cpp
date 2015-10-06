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
  
  
  
  UsbController myCont(NORMAL);

  hv * hvPMT = new hv(&myCont,0xF0000,2);
  hvPMT->setChV(0);
  hvPMT->setChState(0);
  return(0);
  
}

