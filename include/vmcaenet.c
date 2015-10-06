/***************************************************************************/
/*                                                                         */
/*             ------       C . A . E . N .   SpA       ------             */
/*                                                                         */
/*    VMCAENET.C - Caenet Package for V288 Module                          */
/*                                                                         */
/***************************************************************************/

#include "vmcaenet.h"

/***------------------------------------------------------------------------

  Read_data

    --------------------------------------------------------------------***/
int read_data(datovme)
ushort *datovme;
{
ushort q=0;
vme_read(v288addr,datovme,WORD);
vme_read(STATUS,&q,WORD);
return((q == Q) ? TUTTOK : TIMEOUT);
}

/***------------------------------------------------------------------------

  Wait_resp

    --------------------------------------------------------------------***/
int wait_resp(datovme)
ushort *datovme;
{
int i=0;
ushort q=0;
while(i!=TIMEOUT && q!=Q)
     {
      vme_read(v288addr,datovme,WORD);
      vme_read(STATUS,&q,WORD);
      i++;
     }
return((i == TIMEOUT) ? TIMEOUT : TUTTOK);
}

/***------------------------------------------------------------------------

  Send_comm

    --------------------------------------------------------------------***/
int send_comm(vmeaddress,datovme)
unsigned int vmeaddress;
ushort datovme;
{
int i=0;
ushort q=0;
while(i!=TIMEOUT && q!=Q)
     {
      if(!vme_write(vmeaddress,&datovme,WORD))
         return E_BUSERR;
      vme_read(STATUS,&q,WORD);
      i++;   }
return((i == TIMEOUT) ? TIMEOUT : TUTTOK);
}

/***------------------------------------------------------------------------

  Caenet_read: Called by user programs to load "byte_count" bytes from
               CAENET into the buffer pointed by "*dest_buff".

               The VME address of V288, the CAENET crate number and the
               CAENET code are found in global variables.

               Caenet_read returns TUTTOK = 0 if everything has worked;
               It returns one from seven different errors (defined as
               positive constants in Vmcaenet.h) if it has received one
               error which strictly depends from V288 Module;
               It returns a negative error (depending from the CAENET slave
               module) if the CAENET communication has not worked.

     Remember: Module V288 can return three "general" negative errors
               related to the CAENET network that this routine does not
               handle separately from the "slave specific" ones.

    --------------------------------------------------------------------***/
int caenet_read(dest_buff,byte_count)
uchar *dest_buff;
int byte_count;
{
int i,esito;
ushort mstident=V288,datatemp;
short dato;

if((esito=send_comm(v288addr,mstident)) == TIMEOUT)
   return E_NO_Q_IDENT;
else if(esito == E_BUSERR)
   return esito;

/* Transmit Crate Number */
if((esito=send_comm(v288addr,(ushort)craten)) == TIMEOUT)
   return E_NO_Q_CRATE;
else if(esito == E_BUSERR)
   return esito;

/* Transmit Code       */
if((esito=send_comm(v288addr,(ushort)code)) == TIMEOUT)
   return E_NO_Q_CODE;
else if(esito == E_BUSERR)
   return esito;

/* Start Transmission        */
if((esito=send_comm(TXMIT,mstident)) == TIMEOUT)
   return E_NO_Q_TX;
else if(esito == E_BUSERR)
   return esito;

if(wait_resp(&dato) == TIMEOUT)
   return E_NO_Q_RX;

if(dato == TUTTOK)                          /* Test on the operation     */
   for(i=0;i<byte_count;i+=2)
      {
       if(read_data(&datatemp) == TIMEOUT && i<byte_count-1)
          return E_LESSDATA;
       dest_buff[i]   = HIBYTE(datatemp);
       dest_buff[i+1] = LOBYTE(datatemp);
      }
return dato;
}

/***------------------------------------------------------------------------

 Caenet_write: Called by user programs to transfer "byte_count" bytes to
               CAENET from the buffer pointed by "*source_buff".

               The VME address of V288, the CAENET crate number and the
               CAENET code are found in global variables.

               Caenet_write returns TUTTOK = 0 if everything has worked;
               It returns one from seven different errors (defined as
               positive constants in Vmcaenet.h) if it has received one
               error which strictly depends from V288 Module;
               It returns a negative error (depending from the CAENET slave
               module) if the CAENET communication has not worked.

     Remember: Module V288 can return three "general" negative errors
               related to the CAENET network that this routine does not
               handle separately from the "slave specific" ones.

    --------------------------------------------------------------------***/
int caenet_write(source_buff,byte_count)
uchar *source_buff;
int byte_count;
{
int i,esito;
ushort mstident=V288,datatemp;
short dato;

if((esito=send_comm(v288addr,mstident)) == TIMEOUT)
   return E_NO_Q_IDENT;
else if(esito == E_BUSERR)
   return esito;

/* Transmit Crate Number */
if((esito=send_comm(v288addr,(ushort)craten)) == TIMEOUT)
   return E_NO_Q_CRATE;
else if(esito == E_BUSERR)
   return esito;

/* Transmit Code       */
if((esito=send_comm(v288addr,(ushort)code)) == TIMEOUT)
   return E_NO_Q_CODE;
else if(esito == E_BUSERR)
   return esito;

/* Transmit data       */
for(i=0;i<byte_count;i+=2)
   {
    datatemp=(ushort)source_buff[i]<<8 | source_buff[i+1];
    if((esito=send_comm(v288addr,datatemp)) == TIMEOUT)
       return E_NO_Q_DATA;
    else if(esito == E_BUSERR)
       return esito;
   }

/* Start transmission                 */
if((esito=send_comm(TXMIT,mstident)) == TIMEOUT)
   return E_NO_Q_TX;
else if(esito == E_BUSERR)
   return esito;

if(wait_resp(&dato) == TIMEOUT)
   return E_NO_Q_RX;

return dato;
}

/***------------------------------------------------------------------------

  Read_caenet_buffer: Called by user programs to load "byte_count" bytes from
                      CAENET buffer into the buffer pointed by "*dest_buff".

    --------------------------------------------------------------------***/
int read_caenet_buffer(dest_buff,byte_count)
uchar *dest_buff;
int byte_count;
{
int    i;
ushort datatemp;

for(i=0;i<byte_count;i+=2)
   {
    if(read_data(&datatemp) == TIMEOUT && i<byte_count-1)
       return E_LESSDATA;
    dest_buff[i]   = HIBYTE(datatemp);
    dest_buff[i+1] = LOBYTE(datatemp);
   }
return TUTTOK;
}
