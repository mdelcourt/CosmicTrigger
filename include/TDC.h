#ifndef __TDC
#define __TDC

#include "VmeBoard.h"
#include "Event.h"
#include <vector>
#include <sstream>
using namespace std;
/**
 * \brief
 *  This class has a few functions encoding the basic functionalities of the TDC.
 * 
 *  You can control a few basic functionalities of the Time to Digital Converter.
 * 
 *  See different functions for detail.
 * 
 * Here is the  <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/V1190_REV13.pdf">data sheet</a> of the V1190 unit.
 */

class tdc:public vmeBoard{
public:
  // CONSTRUCTOR
  tdc(vmeController* controller,int address=0x00120000);
  /**<
  * \brief Constructor
  * 
  * Adress must terminate with 0x0000.
  * 
  * If address not given, will assume it's 0x00120000.
  */

  //FUNCTIONS -- GENERAL
  
  void Reset();
  /**
   * \brief resets the board.
   */
  void WriteOpcode(unsigned int &data);
  /**
   * \brief writes a command line of 16 bit in the Micro Controller register.
   * 
   * This command includes a wait time for micro controllers 'writing ready' bit.
   */
  void ReadOpcode(unsigned int &data);
  /**
   * \brief reads a 16 bit word in the Micro Controller register.
   * 
   * This command includes a wait time for micro controllers 'read ready' bit.
   */
  unsigned int GetStatusWord ();
  /**
   * \brief Returns the status word of the TDC card.
   */
  bool DataReady(unsigned int status = 4);
  /**
   * \brief Returns if data ready in FIFO.
   * \param status is the status word of the TDC. If none is given, will read from TDC.
   */
  bool IsAlmostFull(unsigned int status = 4);
  /**
   * \brief Returns if buffer is almost full.
   * \param status is the status word of the TDC. If none is given, will read from TDC.
   */
  bool IsFull(unsigned int status = 4);
  /**
   * \brief Returns if buffer is full.
   * \param status is the status word of the TDC. If none is given, will read from TDC.
   */
  bool LostTrig(unsigned int status = 4);
  /** \brief Returns if a trigger was lost
   *  \param status is the status word of the TDC. If none is given, will read from TDC.
   */
  int  InError(unsigned int status = 4);
  /**
   * \brief Returns TDC status. One bit per TDC (4bits in total).
   * \param status is the status word of the TDC. If none is given, will read from TDC.
   */

  
  //FUNCTIONS -- DAQ
  int GetNumberOfEvents();
  /**
   * \brief Returns number of events currently in FIFO
   */
  int GetNumberOfWords();
  /**
   * \brief Returns number of words of event in FIFO
   */
  event GetEvent();
  /**
   * \brief Reads an event from the FIFO
   */
  vector <event> ReadFIFO();
  /**
   * \brief Returns all events in the FIFO
   */

  //FUNCTIONS -- CONFIG
  void SetAcqMode(bool Trig = 1);
  /**
   * \brief set the acquisition mode : Trigger (1), Continuous (0).
   */
  bool GetAcqMode();
  /**
   *\brief Gets the acquisition mode. Trigger (1), Continuous(0);
   */

  void LoadDefaultConfig();
  /**
   * \brief Loads the default configuration
   */
  
  void SaveUserConfig();
  /**
   * \brief Saves user config to board
   */

  void LoadUserConfig();
  /**
   * \brief Loads user config from board
   */

  void SetAutoLoadUserConfig();
  /**
   * \brief Automatically load saved config from board
   */
  

  void SetWindowWidth(unsigned int WidthSetting);
  /**
   * \brief sets the window width in multiples of clockcycle.
   */
  
  void SetWindowOffset(int OffsetSetting);
  /**
   * \brief sets the difference between the beginning of the window and the external trigger in multiples of clockcycle.
   */

  void SetExSearchMargin(int ExSearchMrgnSetting );
  /**
   * \brief sets the extra search margin in units of clock cycle.
   */

  void SetRejectMargin(int RejectMrgnSetting);
  /**
   * \brief sets the reject margin in units of clock cycle.
   */

  void PrintTriggerConfiguration();
  /**
   * \brief descibes the current match window settings in the standard output stream.
   * 
   * includes :
   * 
   * -match window width
   * 
   * -Window ofset
   * 
   * -Extra search window width
   * 
   * -Reject margin width
   * 
   * -Trigger time substraction
   */
  vector <unsigned int > GetTriggerConfiguration();
  /**
   * \brief Returns a vector containing the current trigger configuration.
   * Values are :
   * match window width, window offset, extra search window width, reject margin and trigger time substraction
   */

  void SetEdgeDetection(int mode);
  /**
   * \brief Sets the edge detection configuration
   *
   * \param mode can be pair (0), trailing (1), leading (2) and both (3)
   */
  
  int GetEdgeDetection();
  /**
   * \brief Gets the edge detection configuration
   *
   * \return pair(0), trailing(1), leading(2) or both(3)
   */
  void SetEdgeResolution(int res);
  /**
   * \brief Sets edge resolution (not for pair mode)
   *
   * \param resolution : 800ps (0), 200ps (1) or 100ps(2)
   */

  int GetEdgeResolution();
  /**
   * \brief Gets the edge resolution
   *
   * Works only in leading/trailing edge detection mode
   *
   * \return returns resolution : 800ps(0), 200ps(1) or 100ps(2).
   */
    
  void SetDeadTime(int deadTime);
  /**
   * \brief Sets the dead time in all channels
   *
   * \param dead time = 5ns(0), 10ns(1), 30ns(2) or 100ns(3)
  */

  int GetDeadTime();
  /**
   * \brief Reads the dead time
   *
   * \return dead time : 5ns(0), 10ns(1), 30ns(2) or 100ns(3)
   */

  void SetMaxEventsPerHit(int N = 9 );
  /**
   * \brief sets the maximum number of hits the TDC should memorise in a window
   * 
   * Max number of events will be 2**(N-1) if N>0 and N<9
   * 
   * If N == 0, max = 0 and if N == 9, no maximum will be set.
   * 
   */

  int GetMaxEventsPerHit();
  /**
   * \brief Gets the max number of hits per event
   *
   * \return N such that Max = 2**(N-1) if N in [1,8], 0 if max = 0 and 9 if no max is set.
   */

  void EnableFIFO();
  /**
   * \brief enables FIFO.
   * 
   * This in necessary for 'getEvent()'
   */
  
  void DisableTDCHeaderAndTrailer();
  /**
   * \brief disables extra information send from the TDC. 
   * 
   * This information is neglected by 'getEvent()'.
   */




    
  void SetAlmostFull(int nMax);
  /**
   * \brief Set almost full level
   */



private:

  int add;//Default:0x00120000;

  //REGISTER ADRESSES (must depend on add)
  int Opcode;
  int StatusRegister;
  int MicroHandshake;
  int OutputBuffer;
  int EventFIFO;
  int ControlRegister;

  //PRIVATE FUNCTIONS
  int waitWrite(void);
  int waitRead(void);
//  int waitDataReady(void);
  
};


#endif
