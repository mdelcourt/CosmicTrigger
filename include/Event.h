#include <vector>
#include "time.h"
using namespace std;
/**
 * \brief
 *  This header defines the 'event' structure, necessary to handle some commands of the TDC class.
 */
struct hit
{
  unsigned int channel;
  unsigned int time;
};

struct event
{
  unsigned int eventNumber;
  time_t time;
  vector<hit> measurements;
};

