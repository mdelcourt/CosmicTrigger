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

tdc * myTDC;
void send(unsigned int DATA){
    myTDC->WriteOpcode(DATA);
}

unsigned int read(){
    unsigned int DATA;
    myTDC->ReadOpcode(DATA);
    return(DATA);
}

int main(){
  UsbController myCont(NORMAL);
  unsigned int DATA;   
  myTDC = new tdc(&myCont,0x00AA0000);

  myTDC->LoadDefaultConfig(); 
 



}

