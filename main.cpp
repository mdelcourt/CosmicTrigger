#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"


int main(){
  UsbController myCont(ERROR);
  discri myDiscri(&myCont);
  ttcVi myTtc(&myCont);
  
  myTtc.changeChannel(1);
    
  myDiscri.setMultiChannel(0x000F);
  myDiscri.setMajority(4);
  myDiscri.setTh(100);
  
  
}
