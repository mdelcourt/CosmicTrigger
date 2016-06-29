#include "TTCvi.h"

ttcVi::ttcVi(vmeController* controller,int address):vmeBoard(controller,A32_U_DATA,D16){
  this->add=address;
  this->channel=1;
  this->channelFrequency=0;
  if(vLevel(NORMAL))cout<<"New TTCvi... ok!"<<endl;
}

void ttcVi::sendTrig(){
    int DATA(0);
    if(TestError(writeData(this->add+0x86,&DATA),"TTCvi: sending trigger") && vLevel(DEBUG)) cout<<"Sent VME trigger"<<endl;
}

void ttcVi::resetCounter(){
    int DATA(0);
    if(TestError(writeData(this->add+0x8C,&DATA),"TTCvi: reset counter") && vLevel(DEBUG)) cout<<"ResetCounter"<<endl;
}
long int ttcVi::getEventNumber(){
    int DATA0(0),DATA1(0);
    if(TestError(readData(this->add+0x88,&DATA0),"TTCvi: sending trigger") 
        && TestError(readData(this->add+0x8A,&DATA1),"TTCvi: sending trigger") 
        && vLevel(DEBUG))        cout<<"Read event Number"<<endl;

    return(0x10000*(DATA0%256)+DATA1);
}

void ttcVi::changeChannel(int channel){
  this->channel=channel;
  int DATA=-1;
  switch (channel){
    case 0:
      if (vLevel(NORMAL))cout<<"Set mode to L1A(0)...";
      DATA=0x0000;
      break;
    case 1:
      if (vLevel(NORMAL))cout<<"Set mode to L1A(1)...";
      DATA=0x0001;
      break;
    case 2:
      if (vLevel(NORMAL))cout<<"Set mode to L1A(2)...";
      DATA=0x0002;
      break;
    case 3:
      if (vLevel(NORMAL))cout<<"Set mode to L1A(3)...";
      DATA=0x0003;
      break;
    case -1:
      if (vLevel(NORMAL))cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 4:
      if (vLevel(NORMAL))cout<<"Set mode to VME function...";
      DATA=0x0004;
      break;
    case 5:
      if (vLevel(NORMAL))cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 6:
      if (vLevel(NORMAL))cout<<"Set mode to calibration...";
      DATA=0x0006;
      break;
    case 7:
      if (vLevel(NORMAL))cout<<"Set mode Disabled...";
      DATA=0x0007;
      break;

    default:
      if (vLevel(WARNING))cout<<"*   WARNING: wrong code to change channel. Expected -1(random),0,1,2,3. Statement ignored"<<endl;
    }
  if (DATA>-1){
    DATA+=this->channelFrequency*16*16*16;
    if(TestError(writeData(this->add+0x80,&DATA),"TTCvi: Writing new mode")&&vLevel(NORMAL))cout<<" ok!"<<endl;
    if(vLevel(DEBUG))cout<<"Sent: "<<show_hex(DATA,4)<<" to TTCvi (add:"<<show_hex(this->add,6)<<")"<<endl;
  }
}


void ttcVi::changeRandomFrequency(int frequencyId){
  this->channelFrequency=frequencyId;
  if (channel==-1){
    if(verbose>2)cout<<"Sending new frequency to TTCvi"<<endl;
    this->changeChannel(-1);
  }
}


int ttcVi::viewMode(void){  
  int DATA=0;
  if(TestError(readData(this->add+0x80,&DATA),"TTCvi: viewMode")){
  switch(DATA%16){
    case 0:
      if (vLevel(NORMAL))cout<<"L1A(0)"<<endl;
      break;
    case 1:
      if (vLevel(NORMAL))cout<<"L1A(1)"<<endl;
      break;
    case 2:
      if (vLevel(NORMAL))cout<<"L1A(2)"<<endl;
      break;
    case 3:
      if (vLevel(NORMAL))cout<<"L1A(3)"<<endl;
      break;
    case 5:
      if (vLevel(NORMAL))cout<<"Random, frequency="<<DATA/(16*16*16)<<endl;
      break;
    case 6:
      if(vLevel(NORMAL))cout<<"Calibration"<<endl;
      break;
    case 7:
      if(vLevel(NORMAL))cout<<"disabled"<<endl;
      break;
    default:
      if(vLevel(WARNING))cerr<<"*   WARNING: unknown TTCvi mode."<<endl;
    
  }
    return(16*16*16*(DATA/(16*16*16))+DATA%16);
  }
  else return(-1);
}
