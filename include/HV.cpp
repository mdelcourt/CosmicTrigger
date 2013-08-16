#include "HV.h"

hv::hv(vmeController *controller, int bridgeAdd, int hvAdd):vmeBoard(controller,A24_S_DATA,D16){
  this->add=bridgeAdd;
  this->hvAdd=hvAdd;
  setAM(A24_S_DATA);
  setDW(D16);
}

int hv::reset(void){
  //cout<<show_hex(bridgeAdd)<<endl;
  int DATA=0x0000;
  if(vLevel(NORMAL))cout<<"Reseting HV...";
  if(TestError(writeData(add+0x06,&DATA))){
    if (vLevel(NORMAL))cout<<" ok!"<<endl;
    return(1);
  }
  return(-1);
}

int hv::getStatus(){
    int DATA=0;
    if(vLevel(NORMAL))cout<<"Getting HV status...";
    if(TestError(readData(add+0x02,&DATA))){
      if (vLevel(NORMAL))cout<<" ok!"<<endl;
      return(DATA);
    }
    else return(-1);
}

int hv::comLoop(int data1, int data2){
    usleep(100000);
    if(getStatus()==0xFFFF&&vLevel(WARNING))cout<<"*  WARNING: Initial status of HV was: error..."<<endl;
    int DATA=0x0001;
    if(!TestError(writeData(this->add,&DATA)))return(-1);  //Hello  
    DATA=this->hvAdd;
    if(!TestError(writeData(this->add,&DATA)))return(-1);  //Alim add

    DATA=data1;
    if(!TestError(writeData(this->add,&DATA)))return(-1);  //Command
    if (data2>-1){
      DATA=data2;
      if(!TestError(writeData(this->add,&DATA)))return(-1); //Value 
    }
    
    DATA=0x0000;
    if(!TestError(writeData(this->add+0x04,&DATA)))return(-1); //Send command
    if(getStatus()==0xFFFF){
      if(vLevel(ERROR))cout<<"** ERROR while sending "<<show_hex(data1,4)<<"&"<<show_hex(data2,4)<<endl;
      return(-1);
    }
    return(1);
    //DATA=getStatus();
    //if(DATA==0xFFFF){cerr<<"ERROR!!!"<<endl;}
  
}
int hv::setChState(bool state, int channel){
  if (channel<0){
    int status=1;
    for (int i=0; i<4; i++){
      if(setChState(state,i)<0)status=-1;
    }
    return(status);
  }
  else if(channel>3){
    if(vLevel(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
    return(-1);
  }
  else{
    return(comLoop(channel*256+0x000B-state));
  }
}
int hv::setChV(int volt, int channel){
  if (channel<0){
    int status=1;
    for(int i=0; i<4; i++){
      if(setChV(volt,i)<0) status=-1;
    }
    return(status);
  }
  else if(channel>3){
    if(vLevel(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
    return(-1);
  }
  else{
    usleep(10000);
    return(comLoop(channel*256+0x0003,volt));
  }
}

int hv::readValues(void){
  if(comLoop(0x00)==-1)return(-1);
  int DATA=0;
  if(TestError(readData(add,&DATA))==-1)return(-1);
  if(vLevel(NORMAL))cout<<show_hex(DATA)<<endl;
  if(vLevel(DEBUG))cout<<"Data useful:"<<show_hex(getStatus())<<endl;
  if(!(getStatus()==0xFFFF)) return(DATA);
  else return(-1);
}



