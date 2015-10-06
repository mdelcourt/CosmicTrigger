#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;


/**
 * \brief Main controller virtual class.
 * 
 * This virtual class contains the definitions of the basic i/o functions for any VME system.
 * 
 * It has been created in order to be able to control any VME based system and therefore, the functions are pure virtual.
 * 
 */
class vmeController{
public:
  virtual   void setMode(AddressModifier AM, DataWidth DW)=0;///<Sets default modes.
  virtual   int writeData(long unsigned int address,void* data)=0;///<Short write data function using default modes.
  virtual   int readData(long unsigned int address,void* data)=0;///<Short read data function using default modes.
  virtual   int writeData(long unsigned int address,void* data,AddressModifier AM, DataWidth DW)=0;///<Write data function using given mode.
  virtual   int readData(long unsigned int address,void* data,AddressModifier AM, DataWidth DW)=0;///<Read data function using given mode.
  int verbose; ///<Defines verbosity level of any board using this controller
  
  void setVerbose(int verbose){this->verbose=verbose;}///< Sets verbosity level
  
  virtual AddressModifier getAM(void)=0;///<Gets default mode
  virtual DataWidth getDW(void)=0;///<Gets default mode
  
};

#endif
