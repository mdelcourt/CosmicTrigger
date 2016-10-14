#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/TDC.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include<ctime>

tdc * myTDC;
ttcVi * t;
bool trig_enabled = false;

int get_ms(timeval * t1, timeval * t2){
    long seconds  = -t1->tv_sec  + t2->tv_sec;
    long useconds = -t1->tv_usec + t2->tv_usec;
    return(seconds*1000+useconds/1000+0.5);
}



void send(unsigned int DATA){
    myTDC->writeOpcode(DATA);
}

unsigned int read(){
    unsigned int DATA;
    myTDC->readOpcode(DATA);
    return(DATA);
}

void analyseEvent(event e){
    int t0 = 0;
    for (int i=0; i<e.hits.size(); i++){
        if (e.hits.at(i).channel==22){
            t0 = e.hits.at(i).time;
            break;
        }
    }
    if (t0==0) return;
//    t0=0; 
    for (int i=0; i<e.hits.size(); i++){
        if (e.hits.at(i).channel<24 &&e.hits.at(i).channel!=22){
            cout<<"Channel "<<e.hits.at(i).channel<<" : "<<-(int)e.hits.at(i).time+(int)t0<<endl;
            ostringstream ss;
            ss<<"echo '"<<e.hits.at(i).channel<<"-"<<-(int)e.hits.at(i).time+(int)t0<<"'>>log.txt"<<endl;
            system(ss.str().c_str());
        }
    } 
}

void dumpEvent(event e, string * toWrite){
    (*toWrite)+="#\n";
    for (int i=0; i<e.hits.size(); i++){
        ostringstream ss;
        ss<<e.hits.at(i).channel<<"-"<<e.hits.at(i).time<<endl;
        (*toWrite)+=ss.str();
    }
    if (toWrite->size()>5*1024){
       cout<<"Writing to file..."<<endl;
       ofstream out("log.txt",std::ios_base::app);
       out<<*toWrite;
       out.close();
       (*toWrite)="";
    }
}


vector <event> acqLoop(){
    vector <event> evtVec;
    unsigned int status = myTDC->GetStatusWord();
    bool AMF = myTDC->IsAlmostFull(status);
    bool TL  = myTDC->LostTrig(status);
    bool DR  = myTDC->DataReady(status);
    
    if (AMF || TL){ 
        //Stall acquisition
        if (trig_enabled){
            trig_enabled = false;
            t->changeChannel(0);
            cout<<"BACK PRESSURE AMF="<<AMF<<"; TL="<<TL<<endl;
        }
    }
    else{
        //Resume acquisition
        if (!trig_enabled){
            trig_enabled = true;
            t->changeChannel(1);
        }
    }
    if (DR){
        int TTCevtNumber = t->getEventNumber();
        //Read data from FIFO
        int nEventsInTDC = myTDC->GetNumberOfEvents();
        int nToRead=nEventsInTDC;
        if (nToRead >50 || nToRead == 0)
            nToRead = 50;
        for (int i=0; i<nToRead; i++){
            event e = myTDC->GetEvent();
            evtVec.push_back(e);
            if (! e.errorCode==0){
                cout<<e.eventNumber<<" : error num "<<e.errorCode<<endl;
            }
            if (i==0 || true){
                int nEventsInTDC_2 = myTDC->GetNumberOfEvents();
                int TTCevtNumber_2 = t->getEventNumber();
                cout<<"TTC - TDC = "<<TTCevtNumber_2-e.eventNumber-nEventsInTDC_2<<"-"<<trig_enabled<<endl;
                cout<<TTCevtNumber_2<<"-"<<e.eventNumber+nEventsInTDC_2<<endl;
                if (abs(TTCevtNumber_2-e.eventNumber-nEventsInTDC_2) > 3) { 
                    cerr<<"ERROR..."<<endl; 
                    exit(0);
                }
            }
        }
    }
   return(evtVec); 
}


int main(){
 unsigned int DATA;   
  UsbController myCont(NORMAL);
  t = new ttcVi(&myCont);
//  myCont.reset();
  cout<<"Starting trigger..."<<endl;
  myTDC = new tdc(&myCont,0x00AA0000);
  t->changeChannel(0);
  myTDC->IsAlmostFull();
  //myTDC->SetAlmostFull(1000);
  send(0x0500);//Set default
  send(0x0000);//Set trigger matching
  myTDC->setDetectConf(1);
  send(0x4200);//Enable all channels
  myTDC->writeDeadTime(1);
  myTDC->enableFIFO();

  send(0x2300);
  cout<<"Edge det config"<<read()<<endl;
  send(0x2400);
  send(0x2402);// Set LSB to 100ps
  
  send(0x2600);
  cout<<"Resolution config"<<read()<<endl;
  
 // myTDC->readDeadTime();
 // myTDC->SetAlmostFull(4000);
  //return(0);
//  sleep(10);
  t->resetCounter();
  t->changeChannel(7);

//  send(0x0100);
  
  
  myTDC->setWindowOffset(-3);
  myTDC->setWindowWidth(10);
//  myTDC->setExSearchMargin(1);
//  myTDC->setRejectMargin(1);
//  myTDC->readWindowConfiguration();
/*  myTDC->enableFIFO();*/
  myTDC->disableTDCHeaderAndTrailer();
/*  myTDC->readResolution();
  unsigned int DATA=0x2500;
  myTDC->writeOpcode(DATA);
  DATA=0xF000;
  myTDC->writeOpcode(DATA);
*/
  //Setting channels on :
  string * log = new string;
  cout<<"---------------------------------------------------"<<endl;
  int Nmax= 10000;// maximum number of events read
  int Nevents = 0;
    

  float readRate = 20.;
  float dt = 0.2; //time btw two acquisitions
  t->resetCounter();
  t->changeChannel(1);
  timeval tStart,tStop;
  gettimeofday(&tStart,NULL);  
    
  for(int i=0;i<10000;i+=1)
  {
    vector <event> eVec = acqLoop();
    cout<<"Got "<<eVec.size()<<" events."<<endl;
    gettimeofday(&tStop,NULL);
    if (eVec.size()>0){
        cout<<"Rate from start = "<<eVec.at(eVec.size()-1).eventNumber*1000./get_ms(&tStart,&tStop)<<endl;
    }
    if (eVec.size()<5) sleep(1);
  }



  return(0);
  
}

