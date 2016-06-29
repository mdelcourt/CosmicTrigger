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
  hv * hvMod = new hv(&myCont,0xF0000,12);
  double **x=0;
  while(true){
    x = hvPMT->readValues(x);
    for (int i=0; i<4; i++) {
      cout<<"*** Channel "<<i<<" : "<<endl;
      cout<<"Vmon = "<<x[i][0]<<"V. Imon = "<<x[i][1]<<"µA"<<endl;
    }
    x=hvMod->readValues(x);
    cout<<"Module : ";
    cout<<"Vmon = "<< x[1][0]<<"V"<<endl;
    cout<<"##########################################"<<endl;

    sleep(1);
  }


  return(0);
  
}

