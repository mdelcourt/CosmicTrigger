#ifndef __HVControl
#define __HVControl

#include "VmeBoard.h"

using namespace std;

/**<
 * 
 * \brief High voltage controll class.
 * 
 * This class allows to communicate with a HS CAENET device and was created to control a High voltage unit.
 * 
 * Unlike other vmeBoard objects, it uses 2 different addresses. The main address is the bridge's address and can be set using the switches on the board (see datasheets for details). The second address is the module's address.
 * 
 * In the case of the High Voltage unit, that address can be set by typing F99# Add # on the unit. 
 * 
 * 
 *   
 * Here are the data sheets for the <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/n470_rev3.pdf">CAENET controller</a> and for the <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/v288_rev0.pdf">High voltage source</a>.

 * 
 */


class hv:public vmeBoard{
public:
  hv(vmeController *cont,int bridgeAdd=0x900000, int hvAdd=0x09);
  /**<
   * 
   * \brief Constructor
   * 
   * This constructor sets up the basic Board address and the High voltage unit's address.
   * 
   * 
   */
    
  int comLoop(int data1,int data2=-1);
  /**
   * 
   * \brief Implements H.S. CAENET communication protocol.
   * 
   * This function will send to the LEMO cable:
   * 
   * -A hello signal
   *
   * -The address of the slave device (hvAdd implemented in the constructor and can be changed with setHvAdd() )
   * 
   * -The first data byte (data1)
   * 
   * -If necessary (not -1), the second data byte
   * 
   * Then, after writing in the "send register" of the bridge, the "status register" will be read.
   * 
   * This function returns 1 if everything went as expected, 0 if not.
   * 
   * 
   */
  
  int setChState(bool state, int channel=-1);
  /**
   * \brief Sets channel on/off.
   * 
   * This function will send a command to the High Voltage to set the given channel on (state=0) or off(state=1).
   * 
   * If the channel is not specified or set to -1, the function will set all 4 channels on/off.
   * 
   * This function returns 1 if everything worked and -1 if not.
   * 
   */
  
  int setChV(int volt,int channel=-1);
   /**
   * \brief Sets channel's voltage.
   * 
   * This function will send a command to the High Voltage to set the given channel to the given voltage.
   * 
   * If the channel is not specified or set to -1, the function will set all 4 channels to that voltage.
   * 
   * This function returns 1 if everything worked and -1 if not.
   * 
   */

  int readValues(void);
  /**
   * \brief Reads bridge i/o register
   * 
   * This function will read the value stored on the bridge's i/o register.
   * 
   * If the slave wrote on it correctly, the function will return 1. 
   * 
   * If there is a communication problem, or if the slave did not write on the bridge, the function returns -1
   * 
   */
  
  int reset(void);///<Resets the bridge.
  
  int getStatus(void);
  /**
   * \brief Reads the status register.
   * 
   * When writing data in the "i/o register" or in the "send register", the bridge will set this status register to tell if the action was valid or not.
   * 
   * If the action was valid, it returns 0xFFFE. If not, it returns 0xFFFF.
   * 
   * 
   */
  
  void setHvAdd(int add){hvAdd=add;}///<Allows to change the HV's address.
  
  
private:
  int hvAdd;
  
};



#endif
