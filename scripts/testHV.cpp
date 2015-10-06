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
string outFolder="outFolder";
string outFile="outFolder/data2.txt";
string scalerFileName="outFolder/scalerMonitor";

int sleepInit=10;//time before data taking after setting values
double contTime=10;
double dataTime=50*60;


int get_ms(timeval * t1, timeval * t2){
  long seconds  = t1->tv_sec  - t2->tv_sec;
  long useconds = t1->tv_usec - t2->tv_usec;
  return(seconds*1000+useconds/1000+0.5);
}

bool testVoltage(int volt, hv * h, scaler * s){

  double * data = new double[16];
  double totalTime=0;

  
  //Create control file
  
  timeval controlTime;
  gettimeofday(&controlTime,NULL);

  stringstream contName;
  contName<<outFolder<<"/hvControl_"<<controlTime.tv_sec<<".txt";
  ofstream cont;
  cont.open(contName.str().c_str());
  if(!cont.is_open()){cerr<<"Can't open file!!!"<<endl;}
  
  stringstream scfn;
  scfn<<scalerFileName<<"_"<<volt<<"_"<<controlTime.tv_sec<<".txt";
  
  ofstream scFile;
  scFile.open(scfn.str().c_str());
  
//   h->setChV(volt,0);
//   h->setChV(volt,3);
  h->setChState(1);
  sleep(sleepInit);
  
  timeval start,t;
  gettimeofday(&start,NULL);

  bool timeOk=true;
  
  double **val= new double *[4];
  for(int i=0; i<4; i++)val[i]=new double[10];
  s->reset();
  double * init = new double[16];
  for(int i=0; i<16; i++){init[i]=s->getCount(i+1);}
  
  int * previousData = new int [16];
  int * currentData = new int [16];
  int * skipped = new int [16]; 
  for(int i=0; i<16; i++){
   previousData[i]=0; 
   currentData[i]=0;
   skipped[i]=0;
  }
  
  int step=0;
  
  while(timeOk){
    step++;
    //sleep
    usleep(1e6*contTime);
    
    //check HV
    h->readValues(val);
    
    //save HV values
    for(int i=0; i<4; i++){
      cont<<val[i][0]<<"#";
    }
    cont<<endl;
    
    //Check Scaler
    
    for(int i=1; i<16; i++){
      currentData[i-1]=s->getCount(i);
      if(currentData[i-1]<previousData[i-1]||currentData[i-1]-previousData[i-1]>261000){skipped[i]++;}
      scFile<<step<<"#"<<currentData[i-1]+(skipped[i]%4)*65536-196608;
      cout<<step<<"#"<<currentData[i-1]+(skipped[i]%4)*65536-196608;
      previousData[i-1]=currentData[i-1];
    }
    scFile<<endl;
    cout<<endl;
    
    
    //check time
    gettimeofday(&t,NULL);
    cout<<"Ran for "<<get_ms(&t,&start)/1000.<<"/"<<dataTime<<"s"<<endl;
    if( 1000*dataTime-get_ms(&t,&start)< 1000*contTime){timeOk=false;}
    
  }
  
  gettimeofday(&t,NULL);
  if((1000*dataTime-get_ms(&t,&start)-4)>0){
  usleep(1e3*(1000*dataTime-get_ms(&t,&start)-4));}
  
  for(int i=0; i<16; i++){
    data[i]=s->getCount(i+1)-init[i]+(skipped[i]%4)*65536;
  }
  
  gettimeofday(&t,NULL);
  totalTime=get_ms(&t,&start)/1000.;  
  
  cont.close();
  scFile.close();
  
  ofstream dataFile;
  dataFile.open(outFile.c_str(),ios::app);
  
  if (!dataFile.is_open()){cerr<<"Can't open data file!!!"<<endl;}
  dataFile<<volt<<"#"<<totalTime;
  cout<<volt<<"#"<<totalTime;
  for (int i=0; i<16; i++){dataFile<<"#"<<data[i];cout<<"#"<<data[i];}
  //Comment
  dataFile<<endl;
  cout<<endl;
  dataFile.close();
  
  return(1);  
  
}


int main(){
  
  
  
  UsbController myCont(NORMAL);
  hv * myHv= new hv(&myCont);
  myHv->reset();

  //myHvsetChState(1);
  
  discri myDiscri(&myCont);
  myDiscri.setMultiChannel(0x000F);
  myDiscri.setMajority(4);
  myDiscri.setTh(0);  
  myHv->setChV(1000);
  myHv->setChState(1);

  
  ttcVi myTtc(&myCont);
  myTtc.changeChannel(1);

//   ofstream cont;
//   cont.open(contName.str().c_str());
//   if(!cont.is_open()){cerr<<"Can't open file!!!"<<endl;}

  scaler *myScaler = new scaler(&myCont,0xECCC00);
    
  myScaler->reset();
   


  myHv->setChState(0);

  return(0);
  
}

