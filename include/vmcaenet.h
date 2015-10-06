/***************************************************************************/
/*                                                                         */
/*             ------       C . A . E . N .   SpA       ------             */
/*                                                                         */
/*    VMCAENET.H - Declarations for communication with V288 Module         */
/*                                                                         */
/***************************************************************************/


#ifndef   uchar
#define   uchar                        unsigned char
#endif
#ifndef   ushort
#define   ushort                      unsigned short
#endif

/* Constants for vme_cycles routines */
#define   BYTE                                     1
#define   WORD                                     2
#define   LWORD                                    4

/*
  Errors returned by caenet_read and caenet_write; the positive ones
  are depending from V288 Module and not from CAENET network
*/
#define   TUTTOK                                   0
#define   E_NO_Q_IDENT                             1
#define   E_NO_Q_CRATE                             2
#define   E_NO_Q_CODE                              3
#define   E_NO_Q_DATA                              4
#define   E_NO_Q_TX                                5
#define   E_NO_Q_RX                                6
#define   E_LESSDATA                               7
#define   E_BUSERR                                 8

/* Number of iterations before deciding that V288 does not answer */
#define   TIMEOUT                                 -1

#define   Q                           (ushort)0xfffe
#define   V288                                     1

/* Registers of V288 Module */
#define   STATUS                     (v288addr+0x02)
#define   TXMIT                      (v288addr+0x04)

#define   LOBYTE(x)                (uchar)((x)&0xff)
#define   HIBYTE(x)       (uchar)(((x)&0xff00) >> 8)

/*
  Interface between the user program and V288; these functions are defined
  in file Vmcaenet.c
*/
int  caenet_read();
int  caenet_write();
int  read_caenet_buffer();

/* Declarations of Global Variables defined in the user program */
extern  unsigned       v288addr,craten;
extern  ushort                    code;

