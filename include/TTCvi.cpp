#include "TTCvi.h"

ttcVi::ttcVi(vmeController* controller,int address):vmeBoard(controller,A32_U_DATA,D16){
  this->add=address;
  this->channel=1;
  this->channelFrequency=0;
  if(vLevel(NORMAL))std::cout<<"New TTCvi... ok!"<<std::endl;
}

void ttcVi::sendTrig(){
    int DATA(0);
    if(TestError(writeData(this->add+0x86,&DATA),"TTCvi: sending trigger") && vLevel(DEBUG)) std::cout<<"Sent VME trigger"<<std::endl;
}

void ttcVi::resetCounter(){
    int DATA(0);
    if(TestError(writeData(this->add+0x8C,&DATA),"TTCvi: reset counter") && vLevel(DEBUG)) std::cout<<"ResetCounter"<<std::endl;
}
long int ttcVi::getEventNumber(){
    int DATA0(0),DATA1(0);
    if(TestError(readData(this->add+0x88,&DATA0),"TTCvi: sending trigger") 
        && TestError(readData(this->add+0x8A,&DATA1),"TTCvi: sending trigger") 
        && vLevel(DEBUG))        std::cout<<"Read event Number"<<std::endl;

    return(0x10000*(DATA0%256)+DATA1);
}

void ttcVi::changeChannel(int channel){
  this->channel=channel;
  int DATA=-1;
  switch (channel){
    case 0:
      if (vLevel(NORMAL))std::cout<<"Set mode to L1A(0)...";
      DATA=0x0000;
      break;
    case 1:
      if (vLevel(NORMAL))std::cout<<"Set mode to L1A(1)...";
      DATA=0x0001;
      break;
    case 2:
      if (vLevel(NORMAL))std::cout<<"Set mode to L1A(2)...";
      DATA=0x0002;
      break;
    case 3:
      if (vLevel(NORMAL))std::cout<<"Set mode to L1A(3)...";
      DATA=0x0003;
      break;
    case -1:
      if (vLevel(NORMAL))std::cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 4:
      if (vLevel(NORMAL))std::cout<<"Set mode to VME function...";
      DATA=0x0004;
      break;
    case 5:
      if (vLevel(NORMAL))std::cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 6:
      if (vLevel(NORMAL))std::cout<<"Set mode to calibration...";
      DATA=0x0006;
      break;
    case 7:
      if (vLevel(NORMAL))std::cout<<"Set mode Disabled...";
      DATA=0x0007;
      break;

    default:
      if (vLevel(WARNING))std::cout<<"*   WARNING: wrong code to change channel. Expected -1(random),0,1,2,3. Statement ignored"<<std::endl;
    }
  if (DATA>-1){
    DATA+=this->channelFrequency*16*16*16;
    if(TestError(writeData(this->add+0x80,&DATA),"TTCvi: Writing new mode")&&vLevel(NORMAL))std::cout<<" ok!"<<std::endl;
    if(vLevel(DEBUG))std::cout<<"Sent: "<<show_hex(DATA,4)<<" to TTCvi (add:"<<show_hex(this->add,6)<<")"<<std::endl;
  }
}


void ttcVi::changeRandomFrequency(int frequencyId){
  this->channelFrequency=frequencyId;
  if (channel==-1){
    if(verbose>2)std::cout<<"Sending new frequency to TTCvi"<<std::endl;
    this->changeChannel(-1);
  }
}


int ttcVi::viewMode(void){  
  int DATA=0;
  if(TestError(readData(this->add+0x80,&DATA),"TTCvi: viewMode")){
  switch(DATA%16){
    case 0:
      if (vLevel(NORMAL))std::cout<<"L1A(0)"<<std::endl;
      break;
    case 1:
      if (vLevel(NORMAL))std::cout<<"L1A(1)"<<std::endl;
      break;
    case 2:
      if (vLevel(NORMAL))std::cout<<"L1A(2)"<<std::endl;
      break;
    case 3:
      if (vLevel(NORMAL))std::cout<<"L1A(3)"<<std::endl;
      break;
    case 5:
      if (vLevel(NORMAL))std::cout<<"Random, frequency="<<DATA/(16*16*16)<<std::endl;
      break;
    case 6:
      if(vLevel(NORMAL))std::cout<<"Calibration"<<std::endl;
      break;
    case 7:
      if(vLevel(NORMAL))std::cout<<"disabled"<<std::endl;
      break;
    default:
      if(vLevel(WARNING))std::cerr<<"*   WARNING: unknown TTCvi mode."<<std::endl;
    
  }
    return(16*16*16*(DATA/(16*16*16))+DATA%16);
  }
  else return(-1);
}
