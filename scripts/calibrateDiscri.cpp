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

bool checkHits (float * t, int * hits, scaler * s){
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

int * getCount(discri * d, scaler * s, float * time, int thresh){
/* This loop returns counts for a given HV value
 */

  //Initialise values
  
  int * hits = new int[nChannel];
  for(int i=0; i<nChannel; i++){hits[i]=0;}
  float t = 0;
  
  //Set HV
  cout<<"Setting Discri thresh at "<<thresh<<endl;
    d->setMultiChannel(0b111);
    d->setTh(thresh);
    d->setWidth(126);
    d->setMajority(3);

  sleep(tSleepInit);
  while (t < *time){
    if(!checkHits(&t,hits,s)){
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
  scaler *myScaler = new scaler(&myCont,0xCCCC00);
  discri * d = new discri(&myCont);
   

  ofstream myFile("calib.dat");
  for(int i=0; i<nChannel; i++){
    myFile<<"Channel"<<i<<"#";
  }
  myFile<<"th#t"<<endl;
  myFile.close();
  for(int th=0; th<30; th+=1){
   float time = 30*60;
   int * hits = getCount(d,myScaler,&time,th);
   ofstream myFile("calib.dat",ios::app);
   for (int i=0; i<nChannel; i++){myFile<<hits[i]<<"#";}
   myFile<<th<<"#"<<time<<endl;
   myFile.close();



  }
/*  for (int channel=0; channel<3; channel++){
    for(int V=500; V<1351; V+=25){
      float time = 60*10;
      int * hits = getCount(myHv,myScaler,&time,V,channel);
      
      ofstream myFile("calib.dat",ios::app);
      for (int i=0; i<nChannel; i++){myFile<<hits[i]<<"#";}
      myFile<<V<<"#"<<time<<"#Channel under test : #"<<channel<<endl;
      myFile.close();
    }
  }
*/  
  return(0);
}

