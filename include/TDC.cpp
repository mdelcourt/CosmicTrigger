#include "TDC.h"
#include<vector>
#include "time.h"

tdc::tdc(vmeController* controller,int address):vmeBoard(controller,A32_U_DATA,D16){
    this->add=address;
    Opcode=add+0x102E;
    StatusRegister=add+0x1002;
    MicroHandshake=add+0x1030;
    OutputBuffer=add+0x0000;
    EventFIFO=add+0x1038;
    ControlRegister=add+0x1000;
}

//@@@@@@@@@@@@@@@@@@@ FUNCTIONS GENERAL @@@@@@@@@@@@@@@@@@@ 

void tdc::reset(){
    unsigned int DATA=0;
    int ADD = this->add +0x1014;
    for(int k = 0; k<3; k++){
        ADD+=2;
        TestError(writeData(ADD, &DATA),"Reset...");
    }
    if(vLevel(NORMAL))
        std::cout<< " Module Reset, Software clear and Software event reset." << std::endl;
}

void tdc::writeOpcode(unsigned int &DATA)
{
  waitWrite();
  TestError(writeData(Opcode,&DATA),"TDC: writing OPCODE");
}

void tdc::readOpcode(unsigned int &DATA)
{
  waitRead();
  TestError(readData(Opcode,&DATA),"TDC: reading OPCODE");
}

unsigned int tdc::getStatusWord(){
    unsigned int DATA;
    TestError(readData(StatusRegister,&DATA),"TDC: read Status");
    return(DATA);
}

// Functions to check specific bits in status word.
// Status word = 4 -> Full and no data ready -> get new status word

bool tdc::dataReady(unsigned int status){
    return(status%2);
}

bool tdc::isAlmostFull(unsigned int status){
    return((status>>1)%2);
}

bool tdc::isFull(unsigned int status){
    return((status>>2)%2);
}

bool tdc::lostTrig(unsigned int status){
    return((status>>15)%2);
}


int tdc::inError(unsigned int status){
    return((status>>6)%16);
}

//@@@@@@@@@@@@@@@@@@@ FUNCTIONS DAQ @@@@@@@@@@@@@@@@@@@ 

int tdc::getNumberOfEvents(){
    unsigned int DATA = 0;
    TestError(readData(this->add+0x103C,&DATA,A32_U_DATA,D16),"TDC: get N events");
    return(DATA);
}

int tdc::getNumberOfWords(){
    unsigned int DATA = 0;
    TestError(readData(this->add+0x1038,&DATA,A32_U_DATA,D32),"TDC: get N words");
    return(DATA%65536);
}

event tdc::getEvent(){
    event e;
    int nWords = getNumberOfWords();
    if (nWords == 0) return (e);
    
    unsigned int DATA=0;
    bool inPayload = false;
    int lastWord = 0;
    for (int i=0; i<nWords; i++){
        lastWord = i;
        TestError(readData(this->add,&DATA,A32_U_DATA,D32),"TDC: read buffer");
        if (vLevel(DEBUG))
            std::cout<<"WORD "<<i<<(i<10?"  ":" ")<<": "<<show_hex(DATA,8)<<std::endl;
        short int wordType = DATA>>27;
        if (!inPayload){ //We are not in the payload yet (expecting header)
            if (wordType == 8){// Global header
                inPayload = true;
                e.eventNumber = (DATA>>5)%4194304;
            }
            else{
                e.errorCode = -2; // Sync loss error
            }
        }
        else{ // We are in the payload
            if (wordType < 8 ){ // TDC Data
                if (wordType==4) { //TDC error
                    e.tdcErrors.push_back(DATA%65536);
                } 
                else if (wordType==0){ //TDC meas
                    hit h;
                    h.channel = (DATA >> 19)%128;
                    h.leading = !((DATA >> 26)%2);
                    h.time    = (DATA)%524288;
                    e.hits.push_back(h);
                }
            }
            else if (wordType == 17){continue;} // Ext. time trigger tag
            else if (wordType == 16){// Trailer
                inPayload = false;
                e.errorCode = (DATA>>24)%8;
                break;
            }
        }
    }
    if (lastWord!=nWords-1) e.errorCode = -3;
    if (inPayload)  e.errorCode = -4;
    time(&e.time);
    return(e);
}


std::vector <event> tdc::readFIFO()
{   
    int nEvents = getNumberOfEvents();
    std::vector <event> ev;
    std::cout<<"Getting "<<nEvents<<" evts"<<std::endl;
    for (int i=0; i<nEvents; i++){
        ev.push_back(getEvent());
    }
   return(ev); 
}

//@@@@@@@@@@@@@@@@@@@ FUNCTIONS CONFIG @@@@@@@@@@@@@@@@@@@ 

void tdc::setAcqMode(bool Trig){
    unsigned int DATA=0;
    if(Trig) DATA = 0x0000;
    else DATA =0x0100;
    writeOpcode(DATA);
    if(vLevel(DEBUG))std::cout << "Trigger Mode : " << Trig<< std::endl;
}

bool tdc::getAcqMode(){
    unsigned int DATA=0x0200;
    writeOpcode(DATA);
    readOpcode(DATA);
    if(vLevel(DEBUG))std::cout << "Trigger Mode : " << DATA%2<< std::endl;
    return(DATA%2);
}

void tdc::setAlmostFull(int nMax){
    unsigned int DATA = nMax;
    TestError(writeData(this->add+=0x1022,&DATA,A32_U_DATA,D16),"TDC: write almost full");
}

void tdc::loadDefaultConfig(){
    unsigned int DATA = 0x0500;
    writeOpcode(DATA);
}

void tdc::saveUserConfig(){
    unsigned int DATA = 0x0600;
    writeOpcode(DATA);
}

void tdc::loadUserConfig(){
    unsigned int DATA = 0x0700;
    writeOpcode(DATA);
}


void tdc::setAutoLoadUserConfig(){
    unsigned int DATA = 0x0800;
    writeOpcode(DATA);
}

void tdc::setWindowWidth(unsigned int WidthSetting)
  {   if (WidthSetting > 4095 ){if(vLevel(WARNING))std::cout << "Width Setting must be a integer in the range from 1 to 4095" << std::endl;}
      else
      {unsigned int DATA=0x1000;
      writeOpcode(DATA);
      DATA = WidthSetting;
      writeOpcode(DATA);
      std::cout << "Window Width set to"<< WidthSetting<< std::endl;
      }
  }


void tdc::setWindowOffset(int OffsetSetting)
  {   if (OffsetSetting > 40 || OffsetSetting < -2048){if(vLevel(WARNING))std::cout << "Offset Setting must be a integer in the range from -2048 to +40" << std::endl;}
      else
      {unsigned int DATA = 0x1100;
      writeOpcode(DATA);
      DATA = OffsetSetting;
      writeOpcode(DATA);
      if (vLevel(NORMAL))std::cout << "Window Width set to"<< OffsetSetting<< std::endl;
      }
  }


void tdc::setExSearchMargin(int ExSearchMrgnSetting )
  {
      if (ExSearchMrgnSetting > 50){
          if(vLevel(WARNING)) std::cout << " 50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50" << std::endl;
          ExSearchMrgnSetting = 50;
      }
      else{
        unsigned int DATA = 0x1200;
        writeOpcode(DATA);
        DATA = ExSearchMrgnSetting;
        writeOpcode(DATA);
        if(vLevel(NORMAL))std::cout << "Extra Search Margin Width set to"<< ExSearchMrgnSetting<< std::endl;
      }
  }

void tdc::setRejectMargin(int RejectMrgnSetting)
  {
      if (RejectMrgnSetting > 4095) {if(vLevel(WARNING))std::cout << "Offset Setting must be a integer in the range from -2048 to +40" << std::endl;}
      else
      {
        unsigned int DATA = 0x1300;
        writeOpcode(DATA);
        DATA = RejectMrgnSetting;
        writeOpcode(DATA);
        if(vLevel(NORMAL))std::cout << "Reject Margin set to"<< RejectMrgnSetting<< std::endl;
      }
  }


void tdc::printTriggerConfiguration()
  {
      unsigned int DATA=0x1600;
      writeOpcode(DATA);
      readOpcode(DATA);
      if(vLevel(NORMAL))std::cout<<" Match window width : "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))std::cout<<" Window ofset : "<<digit(DATA,11,0)-4096;
      readOpcode(DATA);
      if(vLevel(NORMAL))std::cout<<" Extra search window width: "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))std::cout<<" Reject margin width: "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))std::cout<<" Trigger time substraction : "<<digit(DATA,0);
  }
std::vector<unsigned int> tdc::getTriggerConfiguration(){
    unsigned int DATA = 0x1600;
    writeOpcode(DATA);
    std::vector <unsigned int> trigConfig;
    for(int i = 0; i<5; i++){
        readOpcode(DATA);
        trigConfig.push_back(DATA);
    }
    return(trigConfig);
}
      
void tdc::setEdgeDetection(int mode){
   if(mode>3 || mode<0){
     if(vLevel(WARNING)){std::cout<<"Bad mode (0=pair, 1=trailing, 2=leading and 3=t&l)"<<std::endl<<"Mode set to pair"<<std::endl;}
     mode=0; 
    }
    unsigned int DATA = 0x2200;
    writeOpcode(DATA);
    DATA = (unsigned int) mode;
    writeOpcode(DATA);
} 
      
int tdc::getEdgeDetection(){
    unsigned int DATA = 0x2300;
    writeOpcode(DATA);
    readOpcode(DATA); 
    if(vLevel(DEBUG)){std::cout<<" Dead time : "<<DATA%4<< "((0=pair, 1=trailing, 2=leading and 3=t&l)"<<std::endl;} 
    return (DATA%4);
}   

void tdc::setEdgeResolution(int res){
    if (res>2 || res<0){
        if (vLevel(WARNING)){std::cout<<"Bad resolution value (has to be 0,1 or 2)"<<std::endl;}
        return;
    }
    unsigned int DATA=0x2400;
    writeOpcode(DATA);
    DATA = res;
    writeOpcode(DATA);
}
    
int tdc::getEdgeResolution(){
      unsigned int DATA=0x2600;
      writeOpcode(DATA);
      readOpcode(DATA);
      if(vLevel(DEBUG))std::cout<< "Resolution : "<<(DATA%4)<<std::endl;;
      return (DATA%4);
}

void tdc::setDeadTime(int deadTime){
   if(deadTime>3 || deadTime<0){
     if(vLevel(WARNING)){std::cout<<"Bad dead time (0=5ns, 1=10ns, 2=30ns and 3=100ns"<<std::endl<<"Dead time set to 5ns"<<std::endl;}
     deadTime=0; 
    }
    unsigned int DATA = 0x2800;
    writeOpcode(DATA);
    DATA = (unsigned int) deadTime;
    writeOpcode(DATA);
}


void tdc::setMaxEventsPerHit(int N){
    unsigned int DATA=0x3300;
    writeOpcode(DATA);
    DATA=N;
    if (N>9 or N<0){
        if(vLevel(WARNING)) std::cout << "Parameter not valid. Removing maximum hit limit."<<std::endl;
        DATA=9;
    }
    writeOpcode(DATA);
}

int tdc::getMaxEventsPerHit(){
    unsigned int DATA=0x3400;
    writeOpcode(DATA);
    readOpcode(DATA);
    if (vLevel(DEBUG))
        std::cout<<"Max event per hit param = "<<DATA%16<<std::endl;
    return(DATA%16);
}

void tdc::enableFIFO()
  {
    unsigned int DATA;  
    TestError(readData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if (digit(DATA,8)==0) {
      DATA+=0x0100;}
    TestError(writeData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if(vLevel(NORMAL))std::cout<<"FIFO enabled !"<<std::endl;
}
  
int tdc::getDeadTime(){
    unsigned int DATA = 0x2900;
    writeOpcode(DATA);
    readOpcode(DATA); 
    if(vLevel(DEBUG)){
      int d=DATA%4;
      std::cout<<" Dead time : "<<d<< "(="<<5*(d==0)+10*(d==1)+30*(d==2)+100*(d==3)<<"ns)"<<std::endl;
    } 
    return(DATA%4);
}  

void tdc::disableTDCHeaderAndTrailer()
  {
    unsigned int DATA = 0x3100;
      writeOpcode(DATA);
      DATA = 0x3200;
      writeOpcode(DATA);
      readOpcode(DATA);
      if (DATA%2==0)
      if(vLevel(NORMAL))
          std::cout << "TDC Header and Trailer disabled"<< std::endl;
  }
  
void tdc::setStatusAllChannels(bool status){
    unsigned int DATA=0x4200;
    if (status==0) DATA+=0x0100;
    writeOpcode(DATA);
    if (vLevel(DEBUG))
        std::cout<<"Set all channels to "<<(status?"ON":"OFF")<<std::endl;
}

void tdc::setStatusChannel(int channel, bool status){
    unsigned int DATA=0x4000;
    if (status==0)DATA+=0x0100;
    DATA+=channel;
    writeOpcode(DATA);
    if(vLevel(DEBUG))
        std::cout<<"Set channel "<<channel<<" to "<<(status?"ON":"OFF")<<std::endl;
}

int tdc::waitRead(void)
{
    unsigned int DATA=0;
    int i=0;
    bool success = 0;
	while(i<10000){
			TestError(readData(this->MicroHandshake,&DATA,A32_U_DATA,D16),"TDC: wait_read");
			if(DATA%4==3 || DATA%4==2) {success = true; break;}
            else i++;
            usleep(100); //TODO Optimise sleeping time
	}
    if (!success && vLevel(ERROR))
        std::cerr<<"ERROR, device busy after 10000 tries"<<std::endl;
	return success;
}

int tdc::waitWrite(void)
{
    unsigned int DATA=0;
    int i=0;
    bool success = 0;
	while(i<10000){
			TestError(readData(this->MicroHandshake,&DATA,A32_U_DATA,D16),"TDC: wait_write");
			if(DATA%2==1){success = true; break;}
            else i++;
            usleep(100); //TODO Optimise sleeping time
	}
	
    if (!success && vLevel(ERROR))
        std::cerr<<"Error, device busy after 10000 tries"<<std::endl;
	return success;
}
