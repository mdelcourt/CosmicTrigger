#include "HV.h"

hv::hv(vmeController *controller, int bridgeAdd, int hvAdd):vmeBoard(controller,A24_S_DATA,D16){
  this->add=bridgeAdd;
  this->hvAdd=hvAdd;
  setAM(A24_S_DATA);
  setDW(D16);
}

int hv::reset(void){
  //std::cout<<show_hex(bridgeAdd)<<std::endl;
  int DATA=0x0000;
  if(vLevel(NORMAL))std::cout<<"Reseting HV...";
  if(TestError(writeData(add+0x06,&DATA),"HV: reset")){
    if (vLevel(NORMAL))std::cout<<" ok!"<<std::endl;
    return(1);
  }
  return(-1);
}

int hv::getStatus(){
    int DATA=0;
    if(vLevel(DEBUG))std::cout<<"Getting HV status...";
    if(TestError(readData(add+0x02,&DATA),"HV: getStatus")){
      if (vLevel(DEBUG))std::cout<<" ok!"<<std::endl;
      return(DATA);
    }
    else return(-1);
}

int hv::comLoop(int data1, int data2) {
    // FIXME
    usleep(100000);
    
    if (getStatus() == 0xFFFF && vLevel(WARNING))
        std::cout << "*  WARNING: Initial status of HV was: error..." << std::endl;
    
    int DATA = 0x0001;
    
    if (!TestError(writeData(add, &DATA), "HV: comLoop, starting communication"))
        return -1;  //Hello
    
    DATA = hvAdd;
    
    if(!TestError(writeData(add, &DATA), "HV: comLoop, setting alim address"))
        return -1;   //Alim address

    DATA = data1;
    
    if(!TestError(writeData(add, &DATA), "HV: comLoop, setting first command"))
        return -1;   //Command
    
    if (data2 > -1){
        DATA = data2;
        if(!TestError(writeData(add, &DATA), "HV: comLoop, setting second command"))
            return -1;  //Value 
    }
    
    DATA = 0x0000;
    
    if (!TestError(writeData(add + 0x04, &DATA), "HV: comLoop, ordering to send a command"))
        std::cout << "Error?" << std::endl; //Send command

    if (getStatus() == 0xFFFF) {
        if (vLevel(ERROR))
            std::cout << "** ERROR while sending " << show_hex(data1, 4) << "&" << show_hex(data2,4) << std::endl;
        return -1;
    }
//     int lbreak=0;
//     
//     while(getStatus()!=0xFFFF && lbreak<100){
//       lbreak++;
//       DATA=0;
//       if(TestError(readData(this->add+0x00,&DATA),"HV: comLoop, reading")){std::cout<<"Word "<<lbreak<<" "<<show_hex(DATA)<<std::endl;}    
//     }
    return 1;
    //DATA=getStatus();
    //if(DATA==0xFFFF){std::cerr<<"ERROR!!!"<<std::endl;}
  
}

int hv::setChState(bool state, int channel) {
    if (channel < 0) {
        
        int status = 1;
    
        for (int i = 0; i < 4; i++) {
            if(setChState(state, i) < 0)
                status = -1;
        }
    
        return status;
  
    } else if (channel > 3) {
    
        if (vLevel(WARNING))
            std::cerr << "*   WARNING: invalid parameter: " << channel << ". Statement ignored" << std::endl;
        return -1;
    
    } else {
        return comLoop(channel * 256 + 0x000B - state);
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
    if(vLevel(WARNING)) std::cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<std::endl;
    return(-1);
  }
  else{
    usleep(10000);
    return(comLoop(channel*256+0x0003,volt));
  }
}

double ** hv::readValues(double ** val){
  if(val==0){
    //std::cout<<"New value vector"<<std::endl;
    val=new double * [4]; 
    for(int i=0; i<4; i++) val[i]=new double[4];
  }
  
  if(comLoop(0x01)==-1)return(0);
//   return(1);
  int DATA=0;
  //int lBreak=0;
  usleep(100000);
  getStatus();
  readData(add,&DATA);
  if(DATA){std::cout<<"No data..."<<std::endl; return(0);}
  
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      readData(add,&DATA);
      //std::cout<<DATA<<std::endl;
      val[i][j]=DATA;
      }
  }
  if(DATA==0xFFFF){this->reset();}

//   if(TestError(readData(add,&DATA),"HV: reading values")==-1){return(-1);}
  
//   if(vLevel(NORMAL)){std::cout<<show_hex(DATA)<<std::endl;}
//   if(vLevel(DEBUG)){std::cout<<"Data useful:"<<show_hex(getStatus())<<std::endl;}
//   if(!(getStatus()==0xFFFF)){return(DATA);}
//   else return(-1);
return(val);
}



