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
	while(1){
			TestError(readData(this->MicroHandshake,&DATA,A32_U_DATA,D16),"wait_read");
			if(DATA%4==3 || DATA%4==2){
			break;
			i++;
			}
	}
	return 0;
}

int tdc::waitWrite(void)
{
    unsigned int DATA=0;
    int i=0;
	while(1){
			TestError(readData(this->MicroHandshake,&DATA,A32_U_DATA,D16),"wait_write");
			if(DATA%2==1){
			break;
			i++;
			}
	}
	return 0;
}

int tdc::waitDataReady(void)
{
    unsigned int DATA=0;
    int i=0;
	while(1){
			TestError(readData(this->StatusRegister,&DATA,A32_U_DATA,D16),"wait_write");
			if(DATA%2==1){
			break;
			i++;
			}
	}
	return 0;
}

 
void tdc::getEvent(vector<unsigned int> &event)
{
    //works only if FIFO enabled
    unsigned int DATA=0;
    this->waitDataReady(); 
    cout<<endl<<"data ready!"<<endl;
    //unsigned int eventNumber=0;
    unsigned int numberOfWords=0;
    TestError(readData(this->EventFIFO,&DATA,A32_U_DATA,D32),"read FIFO");
    //eventNumber=digit(DATA,31,16);
    numberOfWords=digit(DATA,15,0);
    cout<<"number of words from FIFO : "<<numberOfWords<<endl;
    for(unsigned int i=numberOfWords; i>0 ;i--)
    {
	TestError(readData(this->add,&DATA,A32_U_DATA,D32),"read buffer");
        event.vector::push_back(DATA);

    }
    //possible error check by comparing 'eventNumber' with event number in global header of 'event'
}

void tdc::analyseEvent(vector<unsigned int> &event, string filename)
{
    int triggerTime=0;
    int firstClockTime=0;
    for(unsigned int i=0; i<event.vector::size();i++)
    {
        if (digit(event[i],31,27)==0 && digit(event[i],25,19)==this->ClockChannelNumber)
        {
            firstClockTime=digit(event[i],18,0);
	    break;
	}
    }
    for(unsigned int i=0; i<event.vector::size();i++)
    {
        if (digit(event[i],31,27)==0 && digit(event[i],25,19)==this->TriggerChannelNumber)
        {
            triggerTime=digit(event[i],18,0);
	    break;
	}
    }
    
    //Getting closest clock:
    
    int nextClock=0;
    for(unsigned int i=0; i<event.vector::size();i++)
    {
        if (digit(event[i],31,27)==0 && digit(event[i],25,19)==this->ClockChannelNumber)
        {
	    if (digit(event[i],18,0)>triggerTime){nextClock=digit(event[i],18,0);break;}
	}
    }
    
    cout<<"Trigger Time: "<<triggerTime<<" Next clock time: "<<firstClockTime<<endl;
    
    float phase=nextClock-triggerTime;
    int eventNumber=digit(event[0],26,5);
    
    //completer ICI !
    
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    stringstream mystream;
    mystream<<"echo '"<<asctime(localtime(&rawtime))<<" - "<<phase<<"'>> eventsLong.txt"<<endl;
    system(mystream.str().c_str());
    mystream.str("");
    mystream<<"echo '"<<phase<<"'>> eventsShort.txt"<<endl;
    system(mystream.str().c_str());
    
    
    //Getting all clock ticks
    
    unsigned int previousClock=0;
    for(unsigned int i=0; i<event.vector::size();i++)
    {
        if (digit(event[i],31,27)==0 && digit(event[i],25,19)==this->ClockChannelNumber)
        {
	  if(previousClock!=0){
	        mystream.str("");
		mystream<<"echo '"<<digit(event[i],18,0)-previousClock<<" - Event "<<i<<"'>> clocksLong.txt"<<endl;
		system(mystream.str().c_str());
	        mystream.str("");
		mystream<<"echo '"<<digit(event[i],18,0)-previousClock<<"'>> clocksShort.txt"<<endl;
		system(mystream.str().c_str());
	  }
	  previousClock=digit(event[i],18,0);
	  
	}
    }

    
    
}

void tdc::coutEvent(vector<unsigned int> &event)
{  
  for(unsigned int i=0; i<event.vector::size();i++)
    {
      switch(digit(event[i],31,27))
	{
	  case 0:
	    //time=digit(event[i],18,0);
	    cout<<"channel:"<<digit(event[i],25,19)<<"   time:"<<digit(event[i],18,0)<<endl;
	    break;
	  case 8:
	    cout<<"Global Header of event "<<digit(event[i],26,5)<<endl;
	    break;
	  case 16:
	    cout<<"Global Trailer"<<endl;
	    break;
	  default :
	    cout<<"Other"<<endl;
	    break;
	}
	  
	
    }

}

// Read the status with the
void tdc::ReadStatus(){
    int DATA=0;
    waitRead();
    TestError(readData(StatusRegister,&DATA));
    if (DATA%2 > 0) cout << "Event Ready"<<endl;
    else cout<< "No data ready"<<endl;
    if (DATA%8 >3)  cout<< " Output Buffer is Full"<< endl;
    else cout<< " Output Buffer is not full"<<endl;
    if (DATA%16 >7 ) cout<< " Operating Mode : Trigger "<<endl;
    else cout<< "Operating Mode : Continuous"<<endl;
}

void tdc::Reset(){
    unsigned int DATA=0;
    int ADD = this->add +0x1014;
    for(int k = 0; k<3; k++){
        ADD+=2;
        TestError(writeData(ADD, &DATA),"Reset...");
    }
    cout<< " Module Reset... " << endl << " Software Clear... " << endl <<  " Software Event Reset... " <<endl;
}

void tdc::setMode(bool Trig){
    unsigned int DATA=0;
    if(Trig) DATA = 0x0000;
    else DATA =0x0100;
    waitWrite();
    TestError(writeData(Opcode,&DATA,A32_U_DATA,D16));
    cout << "Trigger Mode : " << Trig<< endl;
    }

void tdc::setMaxEvPerHit(int Max_ev_per_hit){
    for(int k=0; k<8;k++)
    if (Max_ev_per_hit== (2^(k)))
    {
        if (k == 8) cout << "No limit on Maximum number of hits per event";
        unsigned int DATA=0x3300; // MEPH = maximum events per hits
        waitWrite();
        TestError(writeData(Opcode, &DATA));
        DATA = k+1;

        TestError(writeData(Opcode, &DATA)," Maximum events per hits : set ! ");
    }
    else cout<< "Not a valid set  ! value of Max number of hits per event must be a power of 2 (1 2 4 .. 128) or 256 for NO LIMIT";
   }

void tdc::setWindowWidth(unsigned int WidthSetting)
  {   if (WidthSetting > 4095 ) cout << "Width Setting must be a integer in the range from 1 to 4095" << endl;
      else
      {unsigned int DATA=0x1000;
        waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitWrite();
      DATA = WidthSetting;
      TestError(writeData(this->Opcode,&DATA), "Window Width : set !");
      cout << "Window Width set to"<< WidthSetting<< endl;
      }
  }


  void tdc::setWindowOffset(int OffsetSetting)
  {   if (OffsetSetting > 40 || OffsetSetting < -2048) cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;
      else
      {int DATA = 0x1100;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitWrite();
      DATA = OffsetSetting;
      TestError(writeData(this->Opcode,&DATA), "Window Width : set !");
      cout << "Window Width set to"<< OffsetSetting<< endl;
      }
  }


  void tdc::setExSearchMargin(int ExSearchMrgnSetting )
  {
      if (ExSearchMrgnSetting > 50) cout << " 50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50" << endl;
      else
      {int DATA = 0x1200;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitWrite();
      DATA = ExSearchMrgnSetting;
      TestError(writeData(this->Opcode,&DATA), "Extra Search Margin Width : set !");
      cout << "Extra Search Margin Width set to"<< ExSearchMrgnSetting<< endl;
  }
  }

  void tdc::setRejectMargin(int RejectMrgnSetting)
  {
      if (RejectMrgnSetting > 4095) cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;
      else
      {int DATA = 0x1300;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitWrite();
      DATA = RejectMrgnSetting;
      TestError(writeData(this->Opcode,&DATA), "Reject Margin Width : set !");
      cout << "Reject Margin set to"<< RejectMrgnSetting<< endl;
  }
  }

  void tdc::readTriggerConfiguration()
  {
      int DATA=0x1600;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" Match WIndow Width : "<<digit(DATA,11,0);
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" WindowOfset : "<<digit(DATA,11,0)-4096;
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" ExtraSearchWindow : "<<digit(DATA,11,0);
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" Reject Margin : "<<digit(DATA,11,0);
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" Trigger Time Substraction : "<<digit(DATA,0);
      
  }
      
      
  void tdc::setChannelNumbers(int clock, int trigger)
  {
      ClockChannelNumber=clock;
      TriggerChannelNumber=trigger;
  }
    
  void tdc::enableFIFO()
  {
    unsigned int DATA;  
    TestError(readData(ControlRegister, &DATA));
    if (digit(DATA,8)==0) {
      DATA+=0x0100;}
    TestError(writeData(ControlRegister, &DATA));
    cout<<"FIFO enabled !"<<endl;
  }
  
  void tdc::disableTDCHeaderAndTrailer()
  {
    int DATA = 0x3100;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitWrite();
      DATA = 0x3200;
      TestError(writeData(this->Opcode,&DATA));
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      if (DATA%2==0)
      cout << "TDC Header and Trailer disabled"<< endl;
  }
  
  void tdc::readResolution()
  {
      int DATA=0x2600;
      waitWrite();
      TestError(writeData(this->Opcode,&DATA));
      waitRead();
      TestError(readData(this->Opcode,&DATA));
      cout<<" resolution : "<<digit(DATA,1,0)<<endl;;
  }
      