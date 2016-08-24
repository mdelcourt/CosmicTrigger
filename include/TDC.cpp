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


int tdc::waitRead(void)
{
    unsigned int DATA=0;
    int i=0;
    bool success = 0;
	while(i<10000){
			TestError(readData(this->MicroHandshake,&DATA,A32_U_DATA,D16),"TDC: wait_read");
			if(DATA%4==3 || DATA%4==2) {success = true; break;}
            else i++;
            usleep(500);
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
            usleep(500);
	}
	
    if (!success && vLevel(ERROR))
        cerr<<"Error, device busy after 10000 tries"<<endl;
	return success;
}

int tdc::waitDataReady(void)
{
    unsigned int DATA=0;
    int i=0;
    bool success = 0;
	while(i<50){
			TestError(readData(this->StatusRegister,&DATA,A32_U_DATA,D16),"TDC: wait_write");
			if(DATA%2==1){success = true; break;}
            else i++;
            usleep(50000);
	}
    if (!success && vLevel(ERROR))
        cerr<<"Error, no events after 100000 tries"<<endl;
	return success;
}

bool tdc::dataReady(void){
    unsigned  int DATA = 0;
    TestError(readData(StatusRegister,&DATA),"TDC : check data");
    return(DATA%2==1);
}
 
int tdc::getEvent(event &myEvent)
{
    //works only if FIFO enabled !
    
    if ( !dataReady() ){
        if (vLevel(DEBUG))
            cout<<"No data in buffer"<<endl;
        return 0;}
    
    unsigned int DATA=0;
    TestError(readData(this->EventFIFO,&DATA,A32_U_DATA,D32),"TDC: read FIFO");
    unsigned int eventNumberFIFO=digit(DATA,31,16);
    unsigned int numberOfWords=digit(DATA,15,0);
    
    vector<unsigned int> dataOutputBuffer;
    for(unsigned int i=numberOfWords; i>0 ;i--)
    {
    	TestError(readData(this->add,&DATA,A32_U_DATA,D32),"TDC: read buffer");
        dataOutputBuffer.push_back(DATA);
    }
    
    if (dataOutputBuffer.size()==0){
        cerr<<"Empty buffer"<<endl;
        return(-1);
    }

    if (!(eventNumberFIFO==digit(dataOutputBuffer[0],26,5) && digit(dataOutputBuffer[0],31,27)==8)){
        //if (vLevel(ERROR)){cerr<<"Event number mismatch"<<endl;}
        //return (-1);
    }
    
    myEvent.eventNumber=eventNumberFIFO;
    
    hit temporaryHit;
    for(unsigned int i=0; i<numberOfWords-1 ;i++) // "-1" because last event is TRAILER
    {
	if (digit(dataOutputBuffer[i],31,27)==0 )
	{
	  temporaryHit.time=digit(dataOutputBuffer[i],18,0);
	  temporaryHit.channel=digit(dataOutputBuffer[i],25,19);
	  myEvent.hits.push_back(temporaryHit);
	}
    }
    
    time(&myEvent.time);
    return 1;
    
}

void tdc::coutEvent(event myEvent)
{  
  if(vLevel(NORMAL))cout<<"Event number : "<<myEvent.eventNumber<<endl;
  if(vLevel(NORMAL))cout<<"Recorded events :"<<endl;
  for (unsigned int i=0; i<myEvent.hits.size(); i++)
  {
    if(vLevel(NORMAL))cout<<"Channel : "<<myEvent.hits[i].channel<<" - Time : "<<myEvent.hits[i].time<<endl;
  }
  if(vLevel(NORMAL))cout<<endl;
}

// Read the status with the
void tdc::ReadStatus(){
    unsigned int DATA=0;
    waitRead();
    TestError(readData(StatusRegister,&DATA),"TDC: read Status");
    if (DATA%2 > 0){ if(vLevel(NORMAL))cout << "Event Ready"<<endl;}
    else {if(vLevel(NORMAL))cout<< "No data ready"<<endl;}
    if (DATA%8 >3){  if(vLevel(NORMAL))cout<< " Output Buffer is Full"<< endl;}
    else {if(vLevel(NORMAL))cout<< " Output Buffer is not full"<<endl;}
    
    if (DATA%16 >7 ){if(vLevel(NORMAL))cout<< " Operating Mode : Trigger "<<endl;}
    else{ if(vLevel(NORMAL))cout<< "Operating Mode : Continuous"<<endl;}
}

void tdc::Reset(){
    unsigned int DATA=0;
    int ADD = this->add +0x1014;
    for(int k = 0; k<3; k++){
        ADD+=2;
        TestError(writeData(ADD, &DATA),"Reset...");
    }
    if(vLevel(NORMAL))cout<< " Module Reset... " << endl << " Software Clear... " << endl <<  " Software Event Reset... " <<endl;
}

void tdc::setMode(bool Trig){
    unsigned int DATA=0;
    if(Trig) DATA = 0x0000;
    else DATA =0x0100;
    writeOpcode(DATA);
    if(vLevel(DEBUG))cout << "Trigger Mode : " << Trig<< endl;
    }

void tdc::setMaxEvPerHit(int Max_ev_per_hit){
    //FIXME
    return;
    for(int k=0; k<8;k++)
    if (Max_ev_per_hit== (2^(k)))
    {
        if(vLevel(NORMAL))if (k == 8) cout << "No limit on Maximum number of hits per event";
        unsigned int DATA=0x3300; // MEPH = maximum events per hits
        writeOpcode(DATA);
        DATA = k+1;
        writeOpcode(DATA);
    }
    else if (Max_ev_per_hit==0)
    {
	unsigned int DATA=0x3300;
        writeOpcode(DATA);
	DATA=0;
        writeOpcode(DATA);
    }
    else if(vLevel(WARNING))cout<< "Not a valid set  ! value of Max number of hits per event must be 0 or a power of 2 (1 2 4 .. 128) or 256 for NO LIMIT";
   }

void tdc::setWindowWidth(unsigned int WidthSetting)
  {   if (WidthSetting > 4095 ){if(vLevel(WARNING))cout << "Width Setting must be a integer in the range from 1 to 4095" << endl;}
      else
      {unsigned int DATA=0x1000;
      writeOpcode(DATA);
      DATA = WidthSetting;
      writeOpcode(DATA);
      cout << "Window Width set to"<< WidthSetting<< endl;
      }
  }


  void tdc::setWindowOffset(int OffsetSetting)
  {   if (OffsetSetting > 40 || OffsetSetting < -2048){if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
      else
      {unsigned int DATA = 0x1100;
      writeOpcode(DATA);
      DATA = OffsetSetting;
      writeOpcode(DATA);
      if (vLevel(NORMAL))cout << "Window Width set to"<< OffsetSetting<< endl;
      }
  }


  void tdc::setExSearchMargin(int ExSearchMrgnSetting )
  {
      if (ExSearchMrgnSetting > 50){
          if(vLevel(WARNING)) cout << " 50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50" << endl;
          ExSearchMrgnSetting = 50;
      }
      else{
        unsigned int DATA = 0x1200;
        writeOpcode(DATA);
        DATA = ExSearchMrgnSetting;
        writeOpcode(DATA);
        if(vLevel(NORMAL))cout << "Extra Search Margin Width set to"<< ExSearchMrgnSetting<< endl;
      }
  }

  void tdc::setRejectMargin(int RejectMrgnSetting)
  {
      if (RejectMrgnSetting > 4095) {if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
      else
      {
        unsigned int DATA = 0x1300;
        writeOpcode(DATA);
        DATA = RejectMrgnSetting;
        writeOpcode(DATA);
        if(vLevel(NORMAL))cout << "Reject Margin set to"<< RejectMrgnSetting<< endl;
      }
  }

  void tdc::readWindowConfiguration()
  {
      unsigned int DATA=0x1600;
      writeOpcode(DATA);
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Match window width : "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Window ofset : "<<digit(DATA,11,0)-4096;
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Extra search window width: "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Reject margin width: "<<digit(DATA,11,0);
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" Trigger time substraction : "<<digit(DATA,0);
  }
      
      
  void tdc::setChannelNumbers(int clock, int trigger)
  {
      ClockChannelNumber=clock;
      TriggerChannelNumber=trigger;
  }
    
  void tdc::enableFIFO()
  {
    unsigned int DATA;  
    TestError(readData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if (digit(DATA,8)==0) {
      DATA+=0x0100;}
    TestError(writeData(ControlRegister, &DATA),"TDC: Enabling the FIFO");
    if(vLevel(NORMAL))cout<<"FIFO enabled !"<<endl;
  }
  
  void tdc::disableTDCHeaderAndTrailer()
  {
    unsigned int DATA = 0x3100;
      writeOpcode(DATA);
      DATA = 0x3200;
      writeOpcode(DATA);
      readOpcode(DATA);
      if (DATA%2==0)
      if(vLevel(NORMAL))cout << "TDC Header and Trailer disabled"<< endl;
  }
  
  void tdc::readResolution()
  {
      unsigned int DATA=0x2600;
      writeOpcode(DATA);
      readOpcode(DATA);
      if(vLevel(NORMAL))cout<<" resolution : "<<digit(DATA,1,0)<<endl;;
  }


  void tdc::writeDeadTime(int deadTime){
   if(deadTime>3 && deadTime<0){
     if(vLevel(WARNING)){cout<<"Bad dead time (0=5ns, 1=10ns, 2=30ns and 3=100ns"<<endl<<"Dead time set to 5ns"<<endl;}
     deadTime=0; 
    }
    unsigned int DATA = 0x2800;
    writeOpcode(DATA);
    DATA = (unsigned int) deadTime;
    writeOpcode(DATA);
  } 
    
  void tdc::readDeadTime(){
    unsigned int DATA = 0x2900;
    writeOpcode(DATA);
    readOpcode(DATA); 
    if(vLevel(NORMAL)){
      int d=digit(DATA,1,0);
      cout<<" Dead time : "<<d<< "(="<<5*(d==0)+10*(d==1)+30*(d==2)+100*(d==3)<<"ns)"<<endl;
    } 
  }   
    
  
  void tdc::setDetectConf(int mode){
   if(mode>3 || mode<0){
     if(vLevel(WARNING)){cout<<"Bad mode (0=pair, 1=trailing, 2=leading and 3=t&l)"<<endl<<"Mode set to pair"<<endl;}
     mode=0; 
    }
    unsigned int DATA = 0x2200;
    writeOpcode(DATA);
    DATA = (unsigned int) mode;
    writeOpcode(DATA);
  } 
    
  void tdc::getDetectConf(){
    unsigned int DATA = 0x2300;
    writeOpcode(DATA);
    readOpcode(DATA); 
    if(vLevel(NORMAL)){
      int d=digit(DATA,1,0);
      cout<<" Dead time : "<<d<< "((0=pair, 1=trailing, 2=leading and 3=t&l)"<<endl;
    } 
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
