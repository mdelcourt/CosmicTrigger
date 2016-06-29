#include <iostream>

#include "VmeUsbBridge.h"

UsbController::UsbController(int verbose):
    vmeController(verbose) {
    
    board = V1718;
    int32_t *handle = new int32_t;
    m_status = CAENVME_Init((CVBoardTypes)(int)board, 0, 0, handle);
    BHandle = handle;
    AM = A32_S_DATA;
    DW = D16;
    
    if (m_status == 0)
        std::cout << "VME USB Init... ok!" << std::endl;
    else
      std::cout << "***FATAL ERROR: " << m_status << " when starting usb board" << std::endl;
}

void UsbController::setMode(AddressModifier AM, DataWidth DW) {
    this->AM = AM;
    this->DW = DW;
}

int UsbController::writeData(long unsigned int address, void* data) {
    return CAENVME_WriteCycle(*BHandle, address, data, (CVAddressModifier)(int)AM, (CVDataWidth)(int)DW);
}

int UsbController::readData(long unsigned int address, void* data) {
    return CAENVME_ReadCycle(*BHandle, address, data, (CVAddressModifier)(int)AM,(CVDataWidth)(int)DW);
}

int UsbController::writeData(long unsigned int address, void* data,AddressModifier AM, DataWidth DW) {
    return CAENVME_WriteCycle(*BHandle, address, data, (CVAddressModifier)(int)AM, (CVDataWidth)(int)DW);
}

int UsbController::readData(long unsigned int address, void* data, AddressModifier AM, DataWidth DW) {
    return CAENVME_ReadCycle(*BHandle,address, data, (CVAddressModifier)(int)AM, (CVDataWidth)(int)DW);
}

AddressModifier UsbController::getAM(void) {
    return AM;
}

DataWidth UsbController::getDW(void) {
    return DW;
}

UsbController::~UsbController() {
    CAENVME_End(*BHandle);
    std::cout << "Exiting controller" << std::endl; 
}
