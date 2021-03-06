#include "../include/VmeUsbBridge.h" // UsbController object
#include "../include/TTCvi.h"	// TTCvi object
#include "../include/Discri.h"	// Discriminator object
#include "../include/Scaler.h"	// Scaler object


int main(){
 
  UsbController myCont(NORMAL);// Creates a USB controller with NORMAL verbosity
  discri myDiscri(&myCont); // Creates a discriminator unit with that controller
  ttcVi myTtc(&myCont);	// Same thing with a TTCvi
  
  myTtc.changeChannel(1); //Chooses the trigger on channel 1 
  
  myDiscri.setMultiChannel(0x000F); //Enables 4 first channels on discriminator
  myDiscri.setMajority(4);	// Asks for a coincidence of 4 units
  myDiscri.setTh(100);		// Sets the threshold to 100mV
  
}
