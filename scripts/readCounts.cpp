#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/Discri.h"
#include "include/HV.h"
#include "include/Scaler.h"

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include<ctime>


int nChannel = 17; //Number of channels on Scaler
float tSleepInit = 5; // Time in sec the program will pause after HV being set
float tStep = 10; //Time between 2 read/reset of scaler
float hvCheckStep = 2; //Time between 2 HV check

float sleepError = 1; //Time system pauses if error occurs
float postResetSleep = 0.1; //Time system pauses after reset

int dv = 7; //Tolerated voltage difference

int get_ms(timeval * t1, timeval * t2){
  long seconds  = -t1->tv_sec  + t2->tv_sec;
  long useconds = -t1->tv_usec + t2->tv_usec;
  return(seconds*1000+useconds/1000+0.5);
}

bool checkHV (int Vmin, int Vmax, hv * h, int mask = 0b1111){
  double ** x = new double * [4];
  for(int i=0; i<4; i++){x[i]=new double[4];}
  if (h ->readValues(x)==0){cout<<"Unable to read HV..."<<endl;delete x; return(0);}
  for (int ch = 0; ch < 4 ; ch++){
    if((mask>>ch)%2){
    int v = x[ch][0];
      if (v>Vmax || v<Vmin){
        cout<<"Error ! Voltage for channel "<<ch<<" is "<<v<<endl;
        delete x;
        return(0);
      }
    }
  }
  delete x;
  return(1);
}

bool checkHits (float * t, int * hits, scaler * s,int mask = 0b1111){
  timeval tStart,tStop;
  s->reset();
  sleep(postResetSleep);
  gettimeofday(&tStart,NULL);  
  int * initCounts = new int [nChannel];
  for(int i=0; i<nChannel; i++){
    initCounts[i]=s->getCount(i);
    if (initCounts[i]<0){cout<<"Unable to read init counts"<<endl;return(0);}
  }
  gettimeofday(&tStop,NULL);  
  while(get_ms(&tStart,&tStop)<1000*tStep){
    sleep(hvCheckStep);
    gettimeofday(&tStop,NULL);
  }
  for(int i=0; i<nChannel; i++){
    int x = s->getCount(i)-initCounts[i]; 
    if(x<0){cout<<"Unable to read final counts"<<endl;return(0);}
    hits[i]+=x;
  }
  gettimeofday(&tStop,NULL);
  *t+=get_ms(&tStart,&tStop)/1000.;

  return(1);
}

int * getCount( scaler * s, float * time, int channel=-1){
/* This loop returns counts for a given HV value
 */

  //Initialise values
  
  int * hits = new int[nChannel];
  for(int i=0; i<nChannel; i++){hits[i]=0;}
  float t = 0;
  

  sleep(tSleepInit);
  int mask=0;
  if (channel<0){mask=0b1111;}
  else mask = pow(2,channel);
  while (t < *time){
    if(!checkHits(&t,hits,s,mask)){
      cerr<<"Unable to get value... sleeping."<<endl;
      sleep(sleepError);
    }
    cout<<"Done "<<t<<"/"<<*time<<endl;
  }
  *time = t;



  return(hits);
}





int main(){
  
  UsbController myCont(NORMAL);
  hv * myHv= new hv(&myCont,0xF0000,2);
  scaler *myScaler = new scaler(&myCont,0xCCCC00);
   
  float time = 60*5;
  int * hits = getCount(myScaler,&time); 
  for (int i=0; i<nChannel; i++){cout<<"Channel "<<i<<":"<<hits[i]<<endl;}
  
  hits = getCount(myScaler,&time); 
  for (int i=0; i<nChannel; i++){cout<<"Channel "<<i<<":"<<hits[i]<<endl;}
  return(0);
}

