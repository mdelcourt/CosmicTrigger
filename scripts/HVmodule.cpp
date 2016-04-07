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


int HV_NOMINAL=80;
bool switchOff=false;

int main(int argc, char ** argv){
  if(argc>1){
    if (argv[1][0] == ((char*)"s")[0] || argv[1][0] == ((char*)"o")[0]){cout<<"Will stop HV"<<endl; switchOff=true;}
    } 
  UsbController myCont(NORMAL);
  hv * modHv = new hv(&myCont,0xF0000,12);

  if(switchOff){
    modHv->setChV(0,1);
    modHv->setChState(0,1);
    return(0);
  }

  cout<<"#### Warning ####"<<endl;
  cout<<"Will set sensor HV to "<<HV_NOMINAL<<"V"<<flush;

  for(int i=0; i<5; i++){sleep(1);cout<<"."<<flush;}cout<<endl;
  cout<<"Starting HV..."<<endl;


  modHv->setChV(HV_NOMINAL,1);
  modHv->setChState(1,1);

  double **x=0;
  while(true){
    x = modHv->readValues(x);
    for (int i=0; i<4; i++) {
      cout<<"*** Channel "<<i<<" : "<<endl;
      cout<<"Vmon = "<<x[i][0]<<"V. Imon = "<<x[i][1]<<"µA"<<endl;
    }
    cout<<"##########################################"<<endl;

    sleep(1);
  }


  return(0);
  
}

