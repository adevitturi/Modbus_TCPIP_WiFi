/*
  ModbusTCPIP_Master.cpp - Library for reading and writing HRegisters as Modbus Master-Client.
  Created by Agustin A. Devitturi, January 14, 2019.
*/
#include "Arduino.h"
#include "MB_TCP_IP_Master_Wifi_ESP.h"

MB_TCPIP_Master::MB_TCPIP_Master()
{
  int TransactionID = 0;
  byte byteFN = MB_FC_NONE;
  int i=0;
}

bool MB_TCPIP_Master::MB_ReadHreg(byte Slave_ID, word Adress, word Count, word* ReadHregBuff)
{    
    bool response = false;
    
    // Pido leer un Hreg:
    //Transaction ID
    ByteArray[0] = (TransactionID >> 8);
    ByteArray[1] = TransactionID;
    //Protocol ID
    ByteArray[2] = 0;
    ByteArray[3] = 0;
    //Byte Lenght remaining
    ByteArray[4] = 0;
    ByteArray[5] = 6;
    //Slave ID
    ByteArray[6] = Slave_ID;
    //FUNC
    ByteArray[7] = MB_FC_READ_REGISTERS;
    //ADDR
    ByteArray[8] = (Adress >> 8);
    ByteArray[9] = Adress;
    //COUNT
    ByteArray[10] = (Count >> 8);
    ByteArray[11] = Count;

    MB_Wifi_client.write(ByteArray,12);

    TransactionID++;
      
    delay(DELAY_FOR_RESPONSE);
    response = MB_ManageReadResponse(ReadHregBuff);
    return(response);
}

bool MB_TCPIP_Master::MB_Read_DWORD_Hreg(byte Slave_ID, word Adress,long* DWordResp)
{    
    word ReadWords[2];
    bool Status = false;
    Status = MB_ReadHreg(Slave_ID, Adress + 16384,2,ReadWords);
    if(Status == false)
      return false;
    else
    {
      *DWordResp = ((long)ReadWords[0]<<16) | ReadWords[1];
      return true;
    }    
}

bool MB_TCPIP_Master::MB_WriteSingleHreg(byte Slave_ID, word Adress, word Value)
{    
    bool response = false;        
    
    // Pido leer un Hreg:
    //Transaction ID
    ByteArray[0] = (TransactionID >> 8);
    ByteArray[1] = TransactionID;
    //Protocol ID
    ByteArray[2] = 0;
    ByteArray[3] = 0;
    //Byte Lenght remaining
    ByteArray[4] = 0;
    ByteArray[5] = 6;
    //Slave ID
    ByteArray[6] = Slave_ID;
    //FUNC
    ByteArray[7] = MB_FC_WRITE_REGISTER;
    //ADDR
    ByteArray[8] = (Adress >> 8);
    ByteArray[9] = Adress;
    //COUNT
    ByteArray[10] = (Value >> 8);
    ByteArray[11] = Value;

    MB_Wifi_client.write(ByteArray,12);

    TransactionID++;
      
    delay(DELAY_FOR_RESPONSE);
    response = MB_ManageWriteResponse();
    return(response);
}

/*
bool MB_TCPIP_Master::MB_Write_DWORD_Hreg(byte Slave_ID, word Adress, long Value)
{    
    bool WriteStatus1 = false;
    bool WriteStatus2 = false;

    WriteStatus1 = MB_WriteSingleHreg(Slave_ID, Adress + 16384, (word) (Value >> 16));

    WriteStatus2 = MB_WriteSingleHreg(Slave_ID, Adress + 16384 + 1, (word) Value);
    
    return WriteStatus1 & WriteStatus2;
}*/

bool MB_TCPIP_Master::MB_ManageReadResponse(word* ReadHregBuff)
{  
  // if there are incoming bytes available
  // from the server, read them and print them:
   
  int len = MB_Wifi_client.available();
  
  if (len > 0) 
  {
      if (len > 80) len = 80;
      MB_Wifi_client.read(ByteArray, len);

      #if DEBUG_SERIAL_MB_RESP
        Serial.println("Modbus Slave Response:"); 
        for(i=0; i<len ; i++)
          Serial.println(ByteArray[i], HEX); 
        Serial.println(""); 
      #endif           
   }   
   MB_Wifi_client.flush();

  //// rutine Modbus TCP
  byteFN = ByteArray[MB_TCP_FUNC];  

  switch(byteFN) 
  { 
     case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
     {          
        ByteDataLength = ByteArray[MB_TCP_BYTECOUNT];   //Bytes leidos de datos
        WordDataLength = ByteDataLength/2;              //Words leidas de datos
        if(MAX_READ_PAR >= WordDataLength)              //Si puedo leer tantos parametros como MAX_READ_PAR, los asigno
        {
          for(int i=0; i<WordDataLength; i++)
          {
            ReadHregBuff[i] = ((word)ByteArray[9+i*2])<<8 | ByteArray[10+i*2];
            #if DEBUG_SERIAL_MB_PAR_READ
              Serial.print("Hreg #");
              Serial.print(i);
              Serial.print(" read: ");
              Serial.println(ReadHregBuff[i]);
            #endif
          }              
        }
        else
        {
            #if DEBUG_SERIAL_ERRORS
              Serial.println("Amount of Hreg read greater then MAX defined");
            #endif         
        }
        byteFN = MB_FC_NONE;
        return true;          
        break;
     }      

     case 0x86:
     {
        #if DEBUG_SERIAL_ERRORS
          Serial.println("Write Error");
        #endif
        byteFN = MB_FC_NONE;  
        return false;     
        break;
     }

    default:
    {
        #if DEBUG_SERIAL_ERRORS
          Serial.println("Operation Error");
        #endif
        byteFN = MB_FC_NONE;   
        return false;    
        break;
    }
  }    
}

bool MB_TCPIP_Master::MB_ManageWriteResponse()
{  
  // if there are incoming bytes available
  // from the server, read them and print them:
   
  int len = MB_Wifi_client.available();
  
  if (len > 0) 
  {
      if (len > 80) len = 80;
      MB_Wifi_client.read(ByteArray, len);

      #if DEBUG_SERIAL_MB_RESP
        Serial.println("Modbus Slave Response:"); 
        for(i=0; i<len ; i++)
          Serial.println(ByteArray[i], HEX); 
        Serial.println(""); 
      #endif           
   }   
   MB_Wifi_client.flush();

  //// rutine Modbus TCP
  byteFN = ByteArray[MB_TCP_FUNC];  

  switch(byteFN) 
  {      
       case MB_FC_WRITE_REGISTER: // 06 Write Holding Register
       {  
          #if DEBUG_SERIAL_MB_PAR_WRITE
            Serial.println("Write Success");
          #endif      
          byteFN = MB_FC_NONE;    
          return true;
          break;
       }   

       case 0x86:
       {
          #if DEBUG_SERIAL_ERRORS
            Serial.println("Write Error");
          #endif
          byteFN = MB_FC_NONE;  
          return false;     
          break;
       }

      default:
      {
          #if DEBUG_SERIAL_ERRORS
            Serial.println("Operation Error");
          #endif
          byteFN = MB_FC_NONE;   
          return false;    
          break;
       }
  }    
}

word MB_TCPIP_Master::GetStandardAdress(word menu, byte par)
{
  if(menu > 162 || par > 99)
    return 0;
  word RespAddr = menu * 100 + par - 1;
  return RespAddr;
}

word MB_TCPIP_Master::GetModifiedAdress(word menu, byte par)
{
  if(menu > 63)
    return 0;
  word RespAddr = menu * 256 + par - 1;
  return RespAddr;
}