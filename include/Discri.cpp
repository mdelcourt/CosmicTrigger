#include "Discri.h"

discri::discri(vmeController *controller,int add):vmeBoard(controller,A32_U_DATA,D16){
  this->add=add;
  this->status=0x0000;//All channels off
  this->setMultiChannel(0);
  int DATA;
  if(readData(add+0xFE,&DATA)!=0){
    if (vLevel(ERROR))std::cerr<<"** ERROR, unable to connect to Discriminator at add="<<add<<std::endl;
  }
  else{if(vLevel(NORMAL))std::cout<<"Connexion to discri... ok!"<<std::endl;}
  this->setMultiChannel(this->status);
}



int discri::setChannel(int num, bool newState){
  if (num==-1){
    if(newState)return(setMultiChannel(0xFFFF));
    else return(setMultiChannel(0x0000));
  }
  
  bool curState=(this->status%(int)pow(2,num+1))/pow(2,num);  
  if(!newState && curState)this->status-=pow(2,num);
  else if (newState && !curState) this->status+=pow(2,num);
  int DATA=status;
  if(TestError(writeData(this->add+0x4A,&DATA),"Discri: Changing channel status")){
    if (vLevel(NORMAL))std::cout<<"New status for channel "<<num<<": "<<(this->status%(int)pow(2,num+1))/pow(2,num)<<std::endl;
    std::cout<<"DEBUG..."<<num<<"-"<<this->status<<"-"<<(this->status%(int)pow(2,num+1))/pow(2,num)<<std::endl; 
    return(1);
  }
  else return(-1);
}


int discri::setMultiChannel(int code){
  this->status=code;
  int DATA=code;
  if(TestError(writeData(this->add+0x4A,&DATA),"Discri: Changing channel status")){
    if (vLevel(NORMAL))    std::cout<<"Channels changed. Code:"<<code<<std::endl;
    return(1);
  }
  return(-1);
}

int discri::setMajority(int num){
  double nRound=(num*50-25.0)/4.0;
  int round;
  if ((nRound+0.5)>int(nRound)){ round=(int)nRound+1;}
  else{ round=(int)nRound;}
  
  if(TestError(writeData(this->add+0x48,&round),"Discri: setting majority")){
    if(vLevel(NORMAL))std::cout<<"Set majority level to "<<num<<"(sent: "<<round<<")"<<std::endl;
    return (1);
  }
  return(-1);
}

int discri::setTh(int value,int num){
  if(value>255 || value<0){
    if(vLevel(WARNING)) std::cerr<<"*  WARNING: illegal value , command ignored"<<std::endl;
    return(-1);
  }
  else{
    if (num==-1){
      int status=1;
      if(vLevel(NORMAL))std::cout<<"Setting all thresholds to "<<value<<std::endl;
      for (int i=0; i<16; i++) if(this->setTh(value,i)<0)status=-1;
      return(status);
    }
    else{
      if(vLevel(DEBUG)) std::cout<<"Setting threshold to "<<value<<" on channel "<<num<<"...";
      if(TestError(writeData(add+2*num,&value),"Discri: Setting threshold")){
	if (vLevel(DEBUG))std::cout<<" ok!"<<std::endl;
	return(1);
      }
      else return(-1);
    }
  }
  return(-1); //Never happens, normally.
}

int discri::setWidth(int value,int num){
  if(value>255 || value<1){
    if(vLevel(WARNING))std::cerr<<"*  WARNING: illegal value , command ignored"<<std::endl;
    return(-1);
  }
  else{
      int DATA=value;
      if(vLevel(NORMAL)) std::cout<<"Setting output width to"<<value<<"...";
      bool state;
      if (num<8)state=TestError(writeData(this->add+0x40,&DATA),"Discri: Setting width");
      if (num<0||num>7)state=TestError(writeData(this->add+0x42,&DATA),"Discri: Setting width");
      if(state && vLevel(NORMAL)){
	std::cout<<" ok!"<<std::endl;
      }
      if(!state) return(-1);
      else return(1);
    }
  return(-1); //Never happens, normally.
}



int discri::viewStatus(void){
  if(vLevel(NORMAL))std::cout<<show_hex(this->status,4)<<std::endl;
  return(this->status);
}


int discri::setDeadTime(int value,int num){
  if(value>255 || value<0){
    if(vLevel(WARNING))std::cerr<<"*  WARNING: illegal value , command ignored"<<std::endl;
    return(-1);
  }
  else{
      int DATA=value;
      if(vLevel(NORMAL))std::cout<<"Setting dead time to "<<value<<"...";
      bool state;
      if (num<8)state=TestError(writeData(this->add+0x44,&DATA),"Discri: setting dead time");
      if (num<0||num>7)state=TestError(writeData(this->add+0x46,&DATA),"Discri: setting dead time");
      
      if(state && vLevel(NORMAL))std::cout<<" ok!"<<std::endl;
      if(!state)return(-1);
      else return(1);
    }
    return(-1); //Never happens, normally.
}

