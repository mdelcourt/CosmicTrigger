#ifndef __TDC
#define __TDC

#include "VmeBoard.h"
#include <vector>
#include <sstream>


using namespace std;

class tdc:public vmeBoard{
public:
  // CONSTRUCTOR
  tdc(vmeController* controller,int address=0x00120000);

  //FUNCTIONS
  void getEvent(vector<unsigned int> &event);
  void analyseEvent(vector<unsigned int> &event, string filename);
  void coutEvent(vector<unsigned int> &event);
  void ReadStatus();
  void Reset();
  void setMode(bool Trig = 1);
  void setMaxEvPerHit(int Max_ev_per_hit =1 );
  void Initialize();
  
  void setWindowWidth(unsigned int WidthSetting);
  void setWindowOffset(int OffsetSetting);
  void setExSearchMargin(int ExSearchMrgnSetting );
  void setRejectMargin(int RejectMrgnSetting);
  void readTriggerConfiguration();
  void readResolution();
  
  void setChannelNumbers(int clock, int trigger);
  void enableFIFO();
  void disableTDCHeaderAndTrailer();

private:

  //REGISTER ADRESSES (must depend on add)
  int Opcode;
  int StatusRegister;
  int MicroHandshake;
  int OutputBuffer;
  int EventFIFO;
  int ControlRegister;

  //INTERNAL VARIABLES
  unsigned int ClockChannelNumber;
  unsigned int TriggerChannelNumber;


  //PRIVATE FUNCTIONS
  int waitWrite(void);
  int waitRead(void);
  int waitDataReady(void);


};


#endif
