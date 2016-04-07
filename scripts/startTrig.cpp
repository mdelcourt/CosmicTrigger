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
  ttcVi *myTTC = new ttcVi(&myCont);

  myTTC->changeChannel(-1);
  myTTC->changeRandomFrequency(1);
}

