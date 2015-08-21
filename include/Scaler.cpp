#include "Scaler.h"

scaler::scaler(vmeController* controller,int address):vmeBoard(controller,A24_U_DATA,D16){
  this->add=address;
  if(vLevel(DEBUG))cout<<"Address"<<show_hex(add)<<endl;
  getInfo();
}

 
int scaler::getCount(int channel){
  int DATA=0;
  int completeAdd=add+0x80+4*(channel-1);
  if(TestError(readData(completeAdd,&DATA,A24_U_DATA,D32),"Scaler: getting count")){
    if(vLevel(DEBUG))cout<<"Count="<<DATA<<"("<<show_hex(DATA)<<") at add:"<<show_hex(completeAdd)<<endl;
    return(DATA);
  }
  return(-1);
}


int scaler::getInfo(){ 
  int DATA=0;
  if(vLevel(NORMAL))cout<<"Getting scaler information...";
  if(TestError(readData(add+0xFE,&DATA),"Scaler: testing communication")){
    if(vLevel(NORMAL)){
    cout<<" ok!"<<endl;
    }
   return(DATA);
  }
  return(-1);
}

int scaler::reset(){
  int DATA=0;
  if(vLevel(NORMAL))cout<<"Reseting scaler...";
  if(TestError(writeData(add,&DATA),"Scaler: resetting")){
    if(vLevel(NORMAL)){
    cout<<" ok!"<<endl;
    }
    return(1);
  }
  return(-1);
}


int scaler::readPresets(int channel){
  int DATA=0;
  if(TestError(readData(add+0x40+4*(channel-1),&DATA),"Scaler: reading presets")){;
    if(vLevel(NORMAL))cout<<"Preset: "<<DATA<<endl;
    return(DATA);
  }
  return(-1);
}


int scaler::setPresets(int channel,int value){
  int DATA=value;
  if(vLevel(NORMAL))cout<<"Setting presets to "<<value<<"...";
  if(TestError(writeData(add+0x40+4*(channel-1),&DATA),"Scaler: setting presets")){
    if(vLevel(NORMAL)){
      cout<<" ok!"<<endl;
    }
    return(1);
  }
  else{
    return(-1);
  }
}