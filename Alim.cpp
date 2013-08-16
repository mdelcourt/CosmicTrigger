#include "include/VmeUsbBridge.h"
#include "include/HV.h"

int main(){
  UsbController myCont;
  hv myHv(&myCont);
  
  cout<<"Setting all channels to 1250V...";
  myHv.setChV(1250);
  cout<<" Ok!"<<endl<<"Setting channel 3 to 1400V...";
  myHv.setChV(1400,3);
  cout<<" Ok!"<<endl<<"Setting all channels on...";
  myHv.setChState(1);
  cout<<" Ok!"<<endl;
  
  cout<<"Waiting for Ctrl-C"<<endl;
  system("sleep 100h");
  
  cout<<"Shutting down..."<<endl;
  myHv.setChState(0);
  cout<<" Ok!"<<endl;
  /*
  myDiscri.setMajority(2);
  myDiscri.setMultiChannel(0x00FF);
  myDiscri.setTh(100);
  myDiscri.setWidth(100);
  myDiscri.setDeadTime(0);
  */
  
  //myTtc.changeChannel(1);
  //myTtc.changeRandomFrequency();
  //myTtc.viewMode();



  
}