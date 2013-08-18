#ifndef __TYPEDEF
#define __TYPEDEF

#include <iostream>
#include <cmath>
#include <sstream>
#include <stdlib.h>

using namespace std;

bool TestError(int erreur,string endroit="",bool fatal=false);
/**<
 * \brief Handles errors.
 * 
 * \param erreur Return value of a readData or writeData function. This will be shown in an error message if useful.
 * \param endroit1 String indicating the place of the error. Will be shown in error message.
 * \param fatal If set to true, the program exists.
 * 
 * This function is the same as the vmeBoard member function but doesn't handle verbosity.
 * 
 */
string show_hex(int number, int size=-1);
/**<
 * \brief Converts an hexadecimal number to a string.
 * 
 * \param number Number to convert.
 * \param size Number of "digits" to show. Autoset if not specified.
 * 
 */

unsigned int digit(unsigned int data, int begin, int end);

unsigned int digit(unsigned int data, int digit);


/// Defines coutLevels to manage library output.
typedef enum coutLevel{
      SILENT = 0,	///<Shows nearly nothing, except fatal errors
      ERROR  =  1,	///<Shows errors
      WARNING = 2,	///<Shows warning (usually bad parameters)
      NORMAL = 3,	///<Normal level, explains what it's doing
      DEBUG = 4		///<Debug, it talks... A LOT
  
} coutLevel;


/**
        CAEN VME bridges.
*/
typedef enum BoardTypes {
        V1718 = 0,                    ///< CAEN V1718 USB-VME bridge                    
        V2718 = 1,                    ///< V2718 PCI-VME bridge with optical link       
        A2818 = 2,                    ///< PCI board with optical link                  
        A2719 = 3,                    ///< Optical link piggy-back                      
        A3818 = 4		      ///< PCIe board with up to 4 optical links		
} BoardTypes;

/**
        VME cycles data width.
*/
typedef enum DataWidth {
        D8  = 0x01,                   ///<  8 bit                                       
        D16 = 0x02,                   ///< 16 bit                                       
        D32 = 0x04,                   ///< 32 bit                                       
        D64 = 0x08,                   ///< 64 bit                                       
        D16_swapped = 0x12,           ///< 16 bit swapped                               
        D32_swapped = 0x14,           ///< 32 bit swapped                               
        D64_swapped = 0x18            ///< 64 bit swapped                               
} DataWidth;

/**
        VME cycles address modifiers
*/
typedef enum AddressModifier {
        A16_S         = 0x2D,         ///< A16 supervisory access                       
        A16_U         = 0x29,         ///< A16 non-privileged                           
        A16_LCK       = 0x2C,         ///< A16 lock command                             

        A24_S_BLT     = 0x3F,         ///< A24 supervisory block transfer               
        A24_S_PGM     = 0x3E,         ///< A24 supervisory program access               
        A24_S_DATA    = 0x3D,         ///< A24 supervisory data access                  
        A24_S_MBLT    = 0x3C,         ///< A24 supervisory 64-bit block trnsfer         
        A24_U_BLT     = 0x3B,         ///< A24 non-privileged block transfer            
        A24_U_PGM     = 0x3A,         ///< A24 non-privileged program access            
        A24_U_DATA    = 0x39,         ///< A24 non-privileged data access               
        A24_U_MBLT    = 0x38,         ///< A24 non-privileged 64-bit block trnsfer      
        A24_LCK       = 0x32,         ///< A24 lock command                             

        A32_S_BLT     = 0x0F,         ///< A32 supervisory block transfer               
        A32_S_PGM     = 0x0E,         ///< A32 supervisory program access               
        A32_S_DATA    = 0x0D,         ///< A32 supervisory data access                  
        A32_S_MBLT    = 0x0C,         ///< A32 supervisory 64-bit block trnsfer         
        A32_U_BLT     = 0x0B,         ///< A32 non-privileged block transfer            
        A32_U_PGM     = 0x0A,         ///< A32 non-privileged program access            
        A32_U_DATA    = 0x09,         ///< A32 non-privileged data access               
        A32_U_MBLT    = 0x08,         ///< A32 non-privileged 64-bit block trnsfer      
        A32_LCK       = 0x05,         ///< A32 lock command                             

        CR_CSR        = 0x2F,         ///< CR/CSR space                                 

} AddressModifier;

/**
        Error codes returned by the exported functions.
*/
typedef enum ErrorCodes {
        Success       =  0,           ///< Operation completed successfully             
        BusError      = -1,           ///< VME bus error during the cycle               
        CommError     = -2,           ///< Communication error                          
        GenericError  = -3,           ///< Unspecified error                            
        InvalidParam  = -4,           ///< Invalid parameter                            
        TimeoutError  = -5,           ///< Timeout error                                
} ErrorCodes;


#endif
