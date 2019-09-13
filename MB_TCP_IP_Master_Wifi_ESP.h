/*
  ModbusTCPIP_Master.h - Library for reading and writing HRegisters as Modbus Master-Client.
  Created by Agustin A. Devitturi, January 14, 2019.
*/
#ifndef MODBUS_TCPIP_M_h
#define MODBUS_TCPIP_M_h

#include "Arduino.h"
#include <WiFi.h>

//////// DEBUG SERIAL //////////
//#define DEBUG_SERIAL_MB_RESP 1
#define DEBUG_SERIAL_MB_RESP 0

//#define DEBUG_SERIAL_MB_PAR_READ 1
#define DEBUG_SERIAL_MB_PAR_READ 0

//#define DEBUG_SERIAL_MB_PAR_WRITE 1
#define DEBUG_SERIAL_MB_PAR_WRITE 0

//#define DEBUG_SERIAL_ERRORS 1
#define DEBUG_SERIAL_ERRORS 0

//////// Required for Modbus TCP / IP /// Requerido para Modbus TCP/IP /////////
#define MB_TCP_PORT 502

#define DELAY_FOR_RESPONSE 50 //ms antes de esperar respuesta

#define MAX_READ_PAR 10 //maximos de registros que se pueden leer en lectura multiple

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_BYTECOUNT 8

class MB_TCPIP_Master
{
  public:
  	MB_TCPIP_Master();
   	bool MB_ReadHreg(byte Slave_ID, word Adress, word Count, word* ReadHregBuff);
    bool MB_Read_DWORD_Hreg(byte Slave_ID, word Adress,long* DWordResp);
    bool MB_WriteSingleHreg(byte Slave_ID, word Adress, word Value);
    //bool MB_Write_DWORD_Hreg(byte Slave_ID, word Adress, long Value);
    word GetStandardAdress(word menu, byte par);
    word GetModifiedAdress(word menu, byte par);

    WiFiClient MB_Wifi_client;

  private:
    bool MB_ManageReadResponse(word* ReadHregBuff);
    bool MB_ManageWriteResponse();

    int TransactionID;
  	byte byteFN;
  	int WordDataLength;
  	int ByteDataLength;
  	int i;
  	byte ByteArray[16+2*MAX_READ_PAR];  	
};

#endif