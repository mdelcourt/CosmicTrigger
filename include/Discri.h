#ifndef __DISCRI
#define __DISCRI

#include "VmeBoard.h"

/**
 * 
 * \brief Discriminator/Coincidence unit controller.
 * 
 * This class allows the user to controll a CAEN V812 discri/coincidence unit.
 * 
 * 
 */
class discri:public vmeBoard{
public:
  discri(vmeController *controller,int add=0x070000);///<Constructor.
  
  int setChannel(int num=-1, bool newState=true);
  /**<
   * \brief Sets a channel on/off
   * 
   * This function will switch a given channel on/off. If the channel is not specified, it will set all the channels on/off. If the state is not specified, it will set it on.
   * 
   * 
   */
  int setMultiChannel(int code);
    /**<
   * \brief Sets all the channel at once.
   * 
   * Sends the given code to the discri unit.
   * 
   * The code should be a 16 bits long number, the LSB setting channel 0 and the MSB setting channel 15.
   * 
   * 
   */

  int setMajority(int num);
    /**<
   * 
   * 
   * 
   */

  int setTh(int value,int num=-1);
    /**<
   * 
   * 
   * 
   */

  int setWidth(int count,int num=-1);
    /**<
   * 
   * 
   * 
   */

  int setDeadTime(int value,int num=-1);
    /**<
   * 
   * 
   * 
   */

  
  
  int viewStatus(void);
  
  
private:  
  int status;
};



#endif