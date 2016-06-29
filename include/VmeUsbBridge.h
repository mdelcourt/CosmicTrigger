#ifndef __UsbVmeBridge
#define __UsbVmeBridge

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "VmeController.h"

/**
 * \brief Definition of a USB/VME controller.
 * 
 * This class implements the virtual functions of a VmeController object.
 * 
 * It has been built in order to use a V1718 bridge, but should be able to work for other CAEN USB bridges.
 * 
 */

class UsbController: public vmeController {

    public:
        
        UsbController(int verbose = 3);
        /**< 
         * \brief Class constructor.
         * 
         * This constructor will create the object "BHandle" and store it. It also will set a few default modes and check 
         * if the link to the VME is ok.
         * 
         */
        
        ~UsbController();///< Liberates the USB controller and "BHandle    
        void setMode(AddressModifier AM, DataWidth DW);
        int writeData(long unsigned int address, void* data);
        int readData(long unsigned int address, void* data);
        int writeData(long unsigned int address, void* data, AddressModifier AM, DataWidth DW);
        int readData(long unsigned int address, void* data, AddressModifier AM, DataWidth DW);
        int getStatus() { return m_status; }

        AddressModifier getAM(void);
        DataWidth getDW(void);

    private:
        
        AddressModifier AM;
        DataWidth DW;
        int32_t* BHandle;
        int m_status;
        
        /**<
         * 
         * \brief Communication identifier.
         * 
         * This object is primordial in any communication with the VME bridge. It is used every single time DATA has to be transfered from the 
         * host computer to a VME Board.
         * 
         * It is created with the class and will die with the class. Long live BHandle!
         * 
         */
        BoardTypes board;
};

#endif
