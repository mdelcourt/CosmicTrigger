#pragma once 

#include <vector>
#include "time.h"
/**
 * \brief
 *  This header defines the 'event' structure, necessary to handle some commands of the TDC class.
 */
struct hit
{
  unsigned int channel;
  unsigned int time;
  bool leading; //0 = trailing, 1 = leading
};

class event
{
public:
    event():time(0), eventNumber(0), errorCode(-1){}
    time_t time;
    unsigned int eventNumber;
    std::vector<hit> hits;
    std::vector<int> tdcErrors;
    int errorCode;// -1 = not initialised
};

