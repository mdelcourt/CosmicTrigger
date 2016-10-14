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
    myTDC->WriteOpcode(DATA);
}

unsigned int read(){
    unsigned int DATA;
    myTDC->ReadOpcode(DATA);
    return(DATA);
}

void testAcq(UsbController * cont){
    discri * myDiscri = new discri(cont);
    myDiscri->setMultiChannel(0x000F);
    myDiscri->setTh(100);
    ttcVi * myTTC = new ttcVi(cont);
    myTTC->changeChannel(1);
    for (int i=0; i<3; i++){
        if (myTDC->DataReady()){
            vector<event> ev = myTDC->ReadFIFO();
            for (vector<event>::iterator e =  ev.begin(); e!=ev.end(); e++){
                cout<<"Event read :"<<endl;
                cout<<e->hits.size()<<" hits."<<endl;
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
  UsbController myCont(DEBUG);
  unsigned int DATA;   
  myTDC = new tdc(&myCont,0x00AA0000);
  myTDC->Reset();
  myTDC->LoadUserConfig();
 // myTDC->LoadDefaultConfig(); 
/* 
  myTDC->SetAcqMode(1);         // Set trigger matching mode
  myTDC->SetWindowWidth(13);    // Set Window width to XX clock cycles. TODO
  myTDC->EnableFIFO();          // Enables FIFO
  myTDC->SetWindowOffset(0);    // Set offset to 0 clk cycles. TODO
  myTDC->SetExSearchMargin(3);  // ??? TODO
  myTDC->SetRejectMargin(0);    // ??? TODO
  myTDC->SetEdgeDetection(2);   // Set to leading
  myTDC->SetDeadTime(0);        // Set Dead time btw hits to 5ns
  myTDC->SetEdgeResolution(2);  // Edge resolution set to 100ps
  myTDC->SetMaxEventsPerHit(9); // No maximum events

  send(0x3100);                 //Disable TDC header/trailer
  send(0x3500);                 //Enable TDC error mark
  send(0x3900);                 //Enable all errors (1/2)
  send(0x00FF);                 //                  (2/2)
  send(0x3B00);                 //Set FIFO size to 256 (1/2)
  send(0x0003);                 //                     (2/2)


  myTDC->SetStatusAllChannels(0);
  for (int i=0; i<20; i++) myTDC->SetStatusChannel(i,1);
  myTDC->SetAutoLoadUserConfig();
  myTDC->SaveUserConfig();
*/
 testAcq(&myCont); 
}

