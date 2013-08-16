#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"


int main(){
  UsbController myCont(4);
  discri myDiscri(&myCont);
  ttcVi myTtc(&myCont);
  
  myTtc.changeChannel(1);
//   myTtc.changeRandomFrequency();
    
  myDiscri.setMultiChannel(0x000F);
  myDiscri.setMajority(4);
  myDiscri.setTh(100);
  
  
}
