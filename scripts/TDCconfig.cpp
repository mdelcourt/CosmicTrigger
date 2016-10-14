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

using namespace std;
tdc * myTDC;
void send(unsigned int DATA){
    myTDC->writeOpcode(DATA);
}

unsigned int read(){
    unsigned int DATA;
    myTDC->readOpcode(DATA);
    return(DATA);
}

void testAcq(UsbController * cont){
    discri * myDiscri = new discri(cont);
    myDiscri->setMultiChannel(0x000F);
    myDiscri->setTh(100);
    ttcVi * myTTC = new ttcVi(cont);
    myTTC->changeChannel(1);
    for (int i=0; i<3; i++){
        if (tdc::dataReady(myTDC->getStatusWord())){
            vector<event> ev = myTDC->readFIFO();
            for (vector<event>::iterator e =  ev.begin(); e!=ev.end(); e++){
                cout<<"Event read :"<<endl;
                cout<<e->hits.size()<<" hits."<<endl;
                cout<<"Number of TDC errors : "<<e->tdcErrors.size()<<endl;
                int nClk = 0;
                int firstClk = -1;
                for (vector <hit>::iterator h = e->hits.begin(); h!=e->hits.end(); h++){
                    if (h->channel == 17){ 
                        nClk++;
                        if (h->time<firstClk || firstClk<0) firstClk=h->time;
                    }
                }
                for (vector <hit>::iterator h = e->hits.begin(); h!=e->hits.end(); h++){
                        cout<<"Hit in channel"<<h->channel<<", "<<h->time-firstClk<<" after first clk"<<endl;
                }
                cout<<"Seen "<<nClk<<" clock edges."<<endl;
            }
            cout<<"FIFO read."<<endl;
        }
        else{
            cout<<"No data... sleeping 1s..."<<endl;
            sleep(1);
            i--;
        }
    }
    myTTC->changeChannel(7);
}


int main(){
  UsbController myCont(NORMAL);
  unsigned int DATA;   
  myTDC = new tdc(&myCont,0x00AA0000);
  myTDC->reset();
  myTDC->loadUserConfig();
  myTDC->setAcqMode(1);         // Set trigger matching mode
  myTDC->setWindowWidth(13);    // Set Window width to XX clock cycles. TODO
  myTDC->enableFIFO();          // Enables FIFO
  myTDC->setWindowOffset(0);    // Set offset to 0 clk cycles. TODO
  myTDC->setExSearchMargin(3);  // ??? TODO
  myTDC->setRejectMargin(0);    // ??? TODO
  myTDC->setEdgeDetection(2);   // Set to leading
  myTDC->setDeadTime(3);        // Set Dead time btw hits to 5ns
  myTDC->setEdgeResolution(2);  // Edge resolution set to 100ps
  myTDC->setMaxEventsPerHit(9); // No maximum events

  send(0x3100);                 //Disable TDC header/trailer
  send(0x3500);                 //Enable TDC error mark
  send(0x3900);                 //Enable all errors (1/2)
  send(0x00FF);                 //                  (2/2)
  send(0x3B00);                 //Set FIFO size to 256 (1/2)
  send(0x0003);                 //                     (2/2)
//  send(0x3000);                 //Enable TDC header/trailer
  
  myTDC->setStatusAllChannels(0);
  for (int i=0; i<20; i++) myTDC->setStatusChannel(i,1);
  myTDC->setAutoLoadUserConfig();
  myTDC->saveUserConfig();
 
  testAcq(&myCont); 
}

