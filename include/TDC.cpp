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

void tdc::Reset(){
    unsigned int DATA=0;
    int ADD = this->add +0x1014;
    for(int k = 0; k<3; k++){
        ADD+=2;
        TestError(writeData(ADD, &DATA),"Reset...");
    }
    if(vLevel(NORMAL))cout<< " Module Reset... " << endl << " Software Clear... " << endl <<  " Software Event Reset... " <<endl;
}

void tdc::WriteOpcode(unsigned int &DATA)
{
  waitWrite();
  TestError(writeData(Opcode,&DATA),"TDC: writing OPCODE");
}

void tdc::ReadOpcode(unsigned int &DATA)
{
  waitRead();
  TestError(readData(Opcode,&DATA),"TDC: reading OPCODE");
}

unsigned int tdc::GetStatusWord(){
    unsigned int DATA;
    TestError(readData(StatusRegister,&DATA),"TDC: read Status");
    return(DATA);
}

// Functions to check specific bits in status word.
// Status word = 4 -> Full and no data ready -> get new status word

bool tdc::DataReady(unsigned int status){
    if (status == 4) status = GetStatusWord();
    return(status%2);
}

bool tdc::IsAlmostFull(unsigned int status){
    if (status == 4) status = GetStatusWord();
    return((status>>1)%2);
}

bool tdc::IsFull(unsigned int status){
    if (status == 4) status = GetStatusWord();
    return((status>>2)%2);
}

bool tdc::LostTrig(unsigned int status){
    if (status==4) status = GetStatusWord();
    return((status>>15)%2);
}


int tdc::InError(unsigned int status){
    if (status == 4) status = GetStatusWord();
    return((status>>6)%16);
}

//@@@@@@@@@@@@@@@@@@@ FUNCTIONS DAQ @@@@@@@@@@@@@@@@@@@ 

int tdc::GetNumberOfEvents(){
    unsigned int DATA = 0;
    TestError(readData(this->add+0x103C,&DATA,A32_U_DATA,D16),"TDC: get N events");
    return(DATA);
}

int tdc::GetNumberOfWords(){
    unsigned int DATA = 0;
    TestError(readData(this->add+0x1038,&DATA,A32_U_DATA,D32),"TDC: get N words");
    return(DATA%65536);
}

event tdc::GetEvent(){
    event e;
    int nWords = GetNumberOfWords();
    if (nWords == 0) return (e);
    
    unsigned int DATA=0;
    bool inPayload = false;
    int lastWord = 0;
    for (int i=0; i<nWords; i++){
        lastWord = i;
        TestError(readData(this->add,&DATA,A32_U_DATA,D32),"TDC: read buffer");
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
                if (wordType!=0) {continue;} //Not TDC meas.
                else{
                    hit h;
                    h.channel = (DATA >> 19)%256;
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


vector <event> tdc::ReadFIFO()
{   
    int nEvents = GetNumberOfEvents();
    vector <event> ev;
    cout<<"Getting "<<nEvents<<" evts"<<endl;
    for (int i=0; i<nEvents; i++){
        ev.push_back(GetEvent());
    }
   return(ev); 
}

//@@@@@@@@@@@@@@@@@@@ FUNCTIONS CONFIG @@@@@@@@@@@@@@@@@@@ 

void tdc::SetAcqMode(bool Trig){
    unsigned int DATA=0;
    if(Trig) DATA = 0x0000;
    else DATA =0x0100;
    WriteOpcode(DATA);
    if(vLevel(DEBUG))cout << "Trigger Mode : " << Trig<< endl;
}

bool tdc::GetAcqMode(){
    unsigned int DATA=0x0200;
    WriteOpcode(DATA);
    ReadOpcode(DATA);
    if(vLevel(DEBUG))cout << "Trigger Mode : " << DATA%2<< endl;
    return(DATA%2);
}

void tdc::SetAlmostFull(int nMax){
    unsigned int DATA = nMax;
    TestError(writeData(this->add+=0x1022,&DATA,A32_U_DATA,D16),"TDC: write almost full");
}

void tdc::LoadDefaultConfig(){
    unsigned int DATA = 0x0500;
    WriteOpcode(DATA);
}

void tdc::SaveUserConfig(){
    unsigned int DATA = 0x0600;
    WriteOpcode(DATA);
}

void tdc::LoadUserConfig(){
    unsigned int DATA = 0x0700;
    WriteOpcode(DATA);
}


void tdc::SetAutoLoadUserConfig(){
    unsigned int DATA = 0x0800;
    WriteOpcode(DATA);
}

void tdc::SetWindowWidth(unsigned int WidthSetting)
  {   if (WidthSetting > 4095 ){if(vLevel(WARNING))cout << "Width Setting must be a integer in the range from 1 to 4095" << endl;}
      else
      {unsigned int DATA=0x1000;
      WriteOpcode(DATA);
      DATA = WidthSetting;
      WriteOpcode(DATA);
      cout << "Window Width set to"<< WidthSetting<< endl;
      }
  }


void tdc::SetWindowOffset(int OffsetSetting)
  {   if (OffsetSetting > 40 || OffsetSetting < -2048){if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
      else
      {unsigned int DATA = 0x1100;
      WriteOpcode(DATA);
      DATA = OffsetSetting;
      WriteOpcode(DATA);
      if (vLevel(NORMAL))cout << "Window Width set to"<< OffsetSetting<< endl;
      }
  }


void tdc::SetExSearchMargin(int ExSearchMrgnSetting )
  {
      if (ExSearchMrgnSetting > 50){
          if(vLevel(WARNING)) cout << " 50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50" << endl;
          ExSearchMrgnSetting = 50;
      }
      else{
        unsigned int DATA = 0x1200;
        WriteOpcode(DATA);
        DATA = ExSearchMrgnSetting;
        WriteOpcode(DATA);
        if(vLevel(NORMAL))cout << "Extra Search Margin Width set to"<< ExSearchMrgnSetting<< endl;
      }
  }

void tdc::SetRejectMargin(int RejectMrgnSetting)
  {
      if (RejectMrgnSetting > 4095) {if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
      else
      {
        unsigned int DATA = 0x1300;
        WriteOpcode(DATA);
        DATA = RejectMrgnSetting;
        WriteOpcode(DATA);
        if(vLevel(NORMAL))cout << "Reject Margin set to"<< RejectMrgnSetting<< endl;
      }
  }


void tdc::PrintTriggerConfiguration()
  {
      unsigned int DATA=0x1600;
      WriteOpcode(DATA);
      ReadOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Match window width : "<<digit(DATA,11,0);
      ReadOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Window ofset : "<<digit(DATA,11,0)-4096;
      ReadOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Extra search window width: "<<digit(DATA,11,0);
      ReadOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Reject margin width: "<<digit(DATA,11,0);
      ReadOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Trigger time substraction : "<<digit(DATA,0);
  }
vector<unsigned int> tdc::GetTriggerConfiguration(){
    unsigned int DATA = 0x1600;
    WriteOpcode(DATA);
    vector <unsigned int> trigConfig;
    for(int i = 0; i<5; i++){
        ReadOpcode(DATA);
        trigConfig.push_back(DATA);
    }
    return(trigConfig);
}
      
void tdc::SetEdgeDetection(int mode){
   if(mode>3 || mode<0){
     if(vLevel(WARNING)){cout<<"Bad mode (0=pair, 1=trailing, 2=leading and 3=t&l)"<<endl<<"Mode set to pair"<<endl;}
     mode=0; 
    }
    unsigned int DATA = 0x2200;
    WriteOpcode(DATA);
    DATA = (unsigned int) mode;
    WriteOpcode(DATA);
} 
      
int tdc::GetEdgeDetection(){
    unsigned int DATA = 0x2300;
    WriteOpcode(DATA);
    ReadOpcode(DATA); 
    if(vLevel(DEBUG)){cout<<" Dead time : "<<DATA%4<< "((0=pair, 1=trailing, 2=leading and 3=t&l)"<<endl;} 
    return (DATA%4);
}   

void tdc::SetEdgeResolution(int res){
    if (res>2 || res<0){
        if (vLevel(WARNING)){cout<<"Bad resolution value (has to be 0,1 or 2)"<<endl;}
        return;
    }
    unsigned int DATA=0x2400;
    WriteOpcode(DATA);
    DATA = res;
    WriteOpcode(DATA);
}
    
int tdc::GetEdgeResolution(){
      unsigned int DATA=0x2600;
      WriteOpcode(DATA);
      ReadOpcode(DATA);
      if(vLevel(DEBUG))cout<< "Resolution : "<<(DATA%4)<<endl;;
      return (DATA%4);
}

void tdc::SetDeadTime(int deadTime){
   if(deadTime>3 || deadTime<0){
     if(vLevel(WARNING)){cout<<"Bad dead time (0=5ns, 1=10ns, 2=30ns and 3=100ns"<<endl<<"Dead time set to 5ns"<<endl;}
     deadTime=0; 
    }
    unsigned int DATA = 0x2800;
    WriteOpcode(DATA);
    DATA = (unsigned int) deadTime;
    WriteOpcode(DATA);
}


void tdc::SetMaxEventsPerHit(int N){
    unsigned int DATA=0x3300;
    WriteOpcode(DATA);
    DATA=N;
    if (N>9 or N<0){
        if(vLevel(WARNING)) cout << "Parameter not valid. Removing maximum hit limit."<<endl;
        DATA=9;
    }
    WriteOpcode(DATA);
}

int tdc::GetMaxEventsPerHit(){
    unsigned int DATA=0x3400;
    WriteOpcode(DATA);
    ReadOpcode(DATA);
    if (vLevel(DEBUG))
        cout<<"Max event per hit param = "<<DATA%16<<endl;
    return(DATA%16);
}

void tdc::EnableFIFO()
  {
    unsigned int DATA;  
    TestError(readData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if (digit(DATA,8)==0) {
      DATA+=0x0100;}
    TestError(writeData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if(vLevel(NORMAL))cout<<"FIFO enabled !"<<endl;
}
  
int tdc::GetDeadTime(){
    unsigned int DATA = 0x2900;
    WriteOpcode(DATA);
    ReadOpcode(DATA); 
    if(vLevel(DEBUG)){
      int d=DATA%4;
      cout<<" Dead time : "<<d<< "(="<<5*(d==0)+10*(d==1)+30*(d==2)+100*(d==3)<<"ns)"<<endl;
    } 
    return(DATA%4);
}  

void tdc::DisableTDCHeaderAndTrailer()
  {
    unsigned int DATA = 0x3100;
      WriteOpcode(DATA);
      DATA = 0x3200;
      WriteOpcode(DATA);
      ReadOpcode(DATA);
      if (DATA%2==0)
      if(vLevel(NORMAL))cout << "TDC Header and Trailer disabled"<< endl;
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
        cerr<<"ERROR, device busy after 10000 tries"<<endl;
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
        cerr<<"Error, device busy after 10000 tries"<<endl;
	return success;
}
