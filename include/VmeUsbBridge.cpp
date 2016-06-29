#include "VmeUsbBridge.h"
#include <iostream>
using namespace std;

UsbController::UsbController(int verbose){
  this->verbose=verbose;
  this->board=V1718;
  int32_t *handle = new int32_t;
  int status=CAENVME_Init((CVBoardTypes)(int)board, 0, 0, handle);
  this->BHandle=handle;
  this->AM=A32_S_DATA;
  this->DW=D16;
  if (status==0)cout<<"VME USB Init... ok!"<<endl;
  else {
    cout<<"***FATAL ERROR: "<<status<<" when starting usb board"<<endl;
    exit(-1); 
  }
}




void UsbController::setMode(AddressModifier AM, DataWidth DW){
  this->AM=AM;
  this->DW=DW;
}

int UsbController::reset(){
  return(CAENVME_SystemReset(*this->BHandle));
  }

int UsbController::writeData(long unsigned int address,void* data){
  return(CAENVME_WriteCycle(*this->BHandle,address,data,(CVAddressModifier)(int)this->AM,(CVDataWidth)(int)this->DW));
}

int UsbController::readData(long unsigned int address,void* data){
    return(CAENVME_ReadCycle(*this->BHandle,address,data,(CVAddressModifier)(int)this->AM,(CVDataWidth)(int)this->DW));
}

int UsbController::writeData(long unsigned int address,void* data,AddressModifier AM, DataWidth DW){
  return(CAENVME_WriteCycle(*this->BHandle,address,data,(CVAddressModifier)(int)AM,(CVDataWidth)(int)DW));
}

int UsbController::readData(long unsigned int address,void* data,AddressModifier AM, DataWidth DW){
  return(CAENVME_ReadCycle(*this->BHandle,address,data,(CVAddressModifier)(int)AM,(CVDataWidth)(int)DW));
}

AddressModifier UsbController::getAM(void){
  return(this->AM);
  
}
DataWidth UsbController::getDW(void){
  return(this->DW);
}
UsbController::~UsbController(){
 CAENVME_End(*this->BHandle);
 cout<<"Exiting controller"<<endl; 
}
