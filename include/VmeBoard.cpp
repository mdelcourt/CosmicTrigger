#include "VmeBoard.h"


vmeBoard::vmeBoard(vmeController* cont,AddressModifier AM,DataWidth DW){
  this->cont=cont;
  this->AM=AM;
  this->DW=DW;
}

bool vmeBoard::vLevel(coutLevel level){
    return(this->cont->verbose>=(int)level);
}


int vmeBoard::writeData(long unsigned int add, void *DATA){
  return(cont->writeData(add,DATA,AM,DW));
}

int vmeBoard::readData(long unsigned int add, void *DATA){
  return(cont->readData(add,DATA,AM,DW)); 
}
int vmeBoard::writeData(long unsigned int add, void *DATA,AddressModifier tAM, DataWidth tDW){
  return(cont->writeData(add,DATA,tAM,tDW));
}
int vmeBoard::readData(long unsigned int add, void *DATA,AddressModifier tAM, DataWidth tDW){
  return(cont->readData(add,DATA,tAM,tDW));
}

void vmeBoard::setAM(AddressModifier AM){
  this->AM=AM;
}
void vmeBoard::setDW(DataWidth DW){
  this->DW=DW;
}
