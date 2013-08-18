#include "../include/VmeUsbBridge.h"
#include "../include/TTCvi.h"
#include "../include/Discri.h"
#include "../include/HV.h"
#include "../include/TDC.h"

int main(){
  UsbController myCont;
  ttcVi myTtc(&myCont);
  tdc mytdc(&myCont);
  hv myHv(&myCont);
  discri myDiscri(&myCont);
  
  //myHv.setChV(1250);
  //myHv.setChV(1400,3);
  //myHv.setChState(1);
  
  myDiscri.setMajority(4);
  myDiscri.setMultiChannel(0x008F);
  myDiscri.setTh(100);
  myDiscri.setWidth(255);
  myDiscri.setDeadTime(0);
  
  
  myTtc.changeChannel(2);
  myTtc.changeRandomFrequency(4);
  myTtc.viewMode();
  
  cout<<"----------------------------------------------------"<<endl;
  mytdc.setChannelNumbers(1,23);
  mytdc.setWindowWidth(50);
  mytdc.setWindowOffset(-25);
  mytdc.setExSearchMargin(1);
  mytdc.setRejectMargin(1);
  mytdc.readTriggerConfiguration();
  mytdc.enableFIFO();
  mytdc.disableTDCHeaderAndTrailer();
  mytdc.readResolution();
  
  

  for(int i=0;i<5000;i++)
  {
    vector<unsigned int> data;
    mytdc.getEvent(data);
    mytdc.coutEvent(data);
    mytdc.analyseEvent(data,"coucou");
    
  }
  

  
}
