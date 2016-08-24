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


int main(){
 unsigned int DATA; 

 

  
  UsbController myCont(NORMAL);
//  myCont.reset();
  cout<<"Starting trigger..."<<endl;
  ttcVi * t = new ttcVi(&myCont);
  t->changeRandomFrequency(0);
  discri * d = new discri(&myCont);
  d->setWidth(1);
  d->setMultiChannel(0xFFFF);
  d->setMajority(3);
  d->setTh(100);



  myTDC = new tdc(&myCont,0x00AA0000);
  t->changeChannel(0);
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
  
  myTDC->readDeadTime();

  sleep(2);

  t->changeChannel(1);

//  send(0x0100);
  
  
  myTDC->setWindowOffset(-3);
  myTDC->setWindowWidth(10);
//  myTDC->setExSearchMargin(1);
//  myTDC->setRejectMargin(1);
//  myTDC->readWindowConfiguration();
/*  myTDC->enableFIFO();*/
//  myTDC->disableTDCHeaderAndTrailer();
/*  myTDC->readResolution();
  unsigned int DATA=0x2500;
  myTDC->writeOpcode(DATA);
  DATA=0xF000;
  myTDC->writeOpcode(DATA);
*/
  //Setting channels on :
  string * log = new string;
  cout<<"---------------------------------------------------"<<endl;
  int Nmax= 2e6;
  for(int i=0;i<Nmax;i++)
  {
    //cout<<"Loooopin' "<<i<<endl;
    event e;
    int status = myTDC->getEvent(e);
    if (status==1){
        if (i<10 || (i<100 && i%10==0) || (i%100==0)){
            cout<<"Got evt ("<<i+1<<"/"<<Nmax<<")"<<endl;
            ifstream killer("kill.txt");
            if (killer.is_open()){break;}
        }
//        myTDC->coutEvent(e);
        dumpEvent(e, log);
    }
    else if (status==-1){
  //      cout<<"Empty evt..."<<endl;
    }
    else{
        i--;
    //    cout<<"No event present... Sleeping 1s"<<endl;
        sleep(1); 
        continue;
    }
    //    myTDC->analyseEvent(e,"coucou");

  }
  
    ofstream out("log.txt",std::ios_base::app);
    out<<*log;
    out.close();

  
  return(0);
  
}

