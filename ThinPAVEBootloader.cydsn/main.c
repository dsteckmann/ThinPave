
/* ========================================
 *
 * Copyright InstroTek Inc., 2013
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek Inc..
 *
 * ========================================
*/



#include <device.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"

#define alfatRxBufSize 256
#define alfatDBufSize 23

int state = 0;
uint16 alfatRdPtr = 0;

volatile uint8 alfatRxBuf[alfatRxBufSize + 1];
volatile uint16 alfatRxPtr = 0, alfatError[2];
volatile bool   alfatData = false;
volatile char alfatString[alfatRxBufSize + 1];     // keep this as small as possible
volatile char alfatStringD[4][alfatDBufSize + 1];  // keep this as small as possible
volatile uint8 alfatDwhich = 0, alfatEwhich = 0;

void CyBtldrCommStart(void);
void CyBtldrCommStop (void);
void CyBtldrCommReset(void);

cystatus CyBtldrCommWrite(uint8* buffer, uint16 size, uint16* count, uint8 timeOut);
cystatus CyBtldrCommRead (uint8* buffer, uint16 size, uint16* count, uint8 timeOut);
uint16 AlfatWaitError(uint32 time, char* str);
uint32 AlfatReadBytes(char* buf,uint32 *numBytes);
uint32 AlfatRead(char* buf,uint32 numBytes);
uint16 AlfatWaitError2(uint32 time, char* str);
void UINT8_2HEX(char *buf,uint8 u);
uint32 AlfatReadStatusReg(void);
uint8 HEX2DEC(uint8 c);
uint8 HEX2UINT8(uint8* buf);
uint16 HEX2UINT16(uint8* buf);
uint32 HEX2UINT32(uint8* buf);
void LoadErr(char c);
void AlfatStart ( void );
uint16 AlfatGetVersion(  char *version );
void LCD_NChar_Start(void) ;

#pragma pack(1)
typedef struct
{
    uint32 id;
    uint8  rev;
    uint8  cs;

} Bootloader_1_ENTER_t;

enum buttons { NONE,ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,UP,DOWN,PERIOD,CE, 
               ESC,PROJ,MENU,STAND,TIME,MAPR,STORE,ENTER,YES,NO,PRINT,EMPTY };

enum buttons button;

// check if MASK bit is set in value. Return 1 if mask bit is set
#define bit_test(value,bity)      ( ( value & ((uint32)1<<bity) )? 1: 0 )

/*******************************************************************************
*
*  FUNCTION:  readBothColumns()
*
*  PARAMETERS:  None
*
*  DESCRIPTION: Reads the 11 columns and the YES and NO keys.
*
*
*  RETURNS: enum
*
******************************************************************************/
void  scan_keys(void)
{
	// function scans keypad, if button is pressed, button variable is set and remains set until another button is pressed

	uint16 column = 0, column1 = 0;

  button = NONE;

	// enable row 1
	ROW_1_Write(0);
	ROW_2_Write(1);
	CyDelay(50);

	// check if key pressed
	column = Status_KEY_COLUMN_0_Read();
	column1 = Status_KEY_COLUMN_1_Read();
	column |= (column1 << 8);
	column &= 0x1FFF;

	if (bit_test(column, 0) == 0)
	{
		button = CE;
	}

	ROW_1_Write(1);
	ROW_2_Write(1);
}

/*******************************************************************************
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 ******************************************************************************/
// timer turns buzzer OFF on interrupt
CY_ISR ( ISR_KILL )
{

 BUZZER_Write(1);  
 CyDelay(250);
 BUZZER_Write(0);  
 CyDelay(250);
 BUZZER_Write(1);  
 CyDelay(250);
 BUZZER_Write(0);  

 isr_ON_OFF_ClearPending();
 ON_OFF_INT_IO_ClearInterrupt();
      
}



/*******************************************************************************
* Function Name: Bootloader_1_CalcPacketChecksum
********************************************************************************
* Summary:
*  This computes the 16 bit checksum for the provided number of bytes contained
*  in the provided buffer
* Parameters:
*  buffer: The buffer containing the data to compute the checksum for
*  size:   The number of bytes in buffer to compute the checksum for
* Returns: 16 bit checksum for the provided data
*******************************************************************************/
uint16 Bootloader_CalcPacketChecksum(const uint8 buffer[], uint16 size) \
                    CYSMALL 
{
    #if(0u != Bootloader_1_PACKET_CHECKSUM_CRC)

        uint16 CYDATA crc = Bootloader_1_CRC_CCITT_INITIAL_VALUE;
        uint16 CYDATA tmp;
        uint8  CYDATA i;
        uint16 CYDATA tmpIndex = size;

        if(0u == size) { crc = ~crc; }
        else
        {
            do
            {
                tmp = buffer[tmpIndex - size];
                for (i = 0u; i < 8u; i++)
                {
                    if (0u != ((crc & 0x0001u) ^ (tmp & 0x0001u))) { crc = (crc >> 1u) ^ Bootloader_1_CRC_CCITT_POLYNOMIAL; }
                    else { crc >>= 1u; }
                    tmp >>= 1u;
                }
                size--;
            }
            while(0u != size);
            crc = ~crc;
            tmp = crc;
            crc = ( uint16 )(crc << 8u) | (tmp >> 8u);
        }
        return(crc);
    #else
        uint16 CYDATA sum = 0u;
        while (size > 0u)
        {
            sum += buffer[size - 1u];
            size--;
        }
        return(( uint16 )1u + ( uint16 )(~sum));
    #endif /* (0u != Bootloader_1_PACKET_CHECKSUM_CRC) */
}
/*******************************************************************************
* Function Name: CY_ISR(AlfatRxISR)
********************************************************************************
* Summary: Alfat uart rx interrupt
* Parameters:  none
* Return: decimal value
*******************************************************************************/
CY_ISR(AlfatRxISR)
{
   char ch;

   while( (ch = AlfatUart_GetChar()) != 0)
   {
      alfatRxBuf[alfatRxPtr++] = ch;
      if( ( ch == '\n' || alfatRxPtr >= alfatRxBufSize ) && alfatData == false)
      {
         switch(alfatRxBuf[0])
         {
            case '!': // error code some results have 2 - don't like this protocol
               memcpy((char*)&alfatError[alfatEwhich++],(char*)&alfatRxBuf[1],2);
               alfatEwhich &= 1;
            break;
            case '$': // data response - some results have 3
               memcpy((char*)alfatStringD[alfatDwhich],(char*)alfatRxBuf,alfatRxPtr);
               alfatStringD[alfatDwhich][alfatRxPtr++] = '\0';
               alfatRxPtr &= 0x03;
            break;
            default: // string response - don't know how long they can be
               memcpy((char*)alfatString,(char*)alfatRxBuf,alfatDBufSize);
               alfatString[alfatDBufSize] = '\0';
            break;
         }
         alfatRxPtr = 0;
      }
   }
}

//#define Bootloader_1_VERSION        {10, 1, 0x01u}

/******************************************************************************
 *
 *  Name: initIOLines ( void )
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Initialize the GPIO
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void initIOLines ( void )
{
 
 DIS_YES_ON_Write ( 1 ); // Disable the YES/ON key from toggling the LTC2951 power controller.
 SHUT_OFF_IO_Write(1) ;


LCD_BK_EN_Write(0) ;


BUZZER_Write(1);  
CyDelay(250);
BUZZER_Write(0);  

// The POWER KILL ISR
isr_ON_OFF_StartEx ( ISR_KILL );
isr_ON_OFF_SetPriority( 3);

}


int main()
{   
  char t[21];

 DIS_YES_ON_Write ( 1 ); // Disable the YES/ON key from toggling the LTC2951 power controller.
 SHUT_OFF_IO_Write(1) ;
 LCD_BK_EN_Write(0) ;
  AlfatStart();
  BUZZER_Write(1);  
  CyDelay(500);
  BUZZER_Write(0);  

  // The POWER KILL ISR
  isr_ON_OFF_StartEx ( ISR_KILL );
 
  CyGlobalIntEnable;
  LCD_NChar_Init();
  LCD_NChar_Start();
  
  
  LCD_NChar_Position(0,0);
  printStringOnLCD("     InstroTek      ");
 
  EEPROM_Start();
  uint8 InstroTek = (*(reg8*)(CYDEV_EE_BASE + CYDEV_EE_SIZE - 1));
  
  state = 0;
  scan_keys();
  
  if ( button != CE )
  {
    if(InstroTek == 0x54) //== 0x49) // app sets it to 54 at start, and 49 from menu
    {
     state = 9; 
     Bootloader_1_Start();
    }  
  }

  // reset eerpom so that bootloader does not run next reset
  EEPROM_ByteWrite(0x54, (CYDEV_EE_SIZE - 1) / CYDEV_EEPROM_ROW_SIZE, 15);  
  
  if ( AlfatGetVersion( t ) != 0x3030 )
  {
     state = 9; 
     Bootloader_1_Start();
  }  
  
  LCD_NChar_Position(1,6);
  //center the string
  t[6] = 0;
  printStringOnLCD (  t );
  CyDelay(1500);
    
  LCD_NChar_Position(1,0);

  printStringOnLCD("      LOADING       ");
   
 if( ( AlfatReadStatusReg() & 0x20 ) == 0x20)  // drive in
 {
   if(AlfatWaitError(6000,"I U0:\r") == 0x3030) // mounted
   { 
     CyDelay ( 2000 );
     if( AlfatWaitError(1000,"O 0R>U0:\\ThinPAVEupdate.cyacd\r") == 0x3030 )
         {   // success
        LCD_NChar_Position(1,0);  
        printStringOnLCD("     LOADING CYACD  ");
        Bootloader_1_Start(); // should reset and start app               
     }
     else
     {
      LCD_NChar_Position(1,0);
      printStringOnLCD("   NO CYACD    ");
     }            
   
   }
   else
   {
    LCD_NChar_Position(0,0);
    printStringOnLCD("DRIVE NOT MOUNTED");
   }
  }
  else
  {
    LCD_NChar_Position(0,0);
    printStringOnLCD("DRIVE NOT PRESENT");
  }

 state = 9;
 Bootloader_1_Start();
 for(;;) 
 {
 }
 return 1;
}
void CyBtldrCommStart(void) { AlfatUart_Start(); }
void CyBtldrCommStop (void) { AlfatUart_Stop(); }
void CyBtldrCommReset(void)  {  // reset the buffers
   AlfatUart_ClearRxBuffer();
   AlfatUart_ClearTxBuffer();
}
void LoadErr(char c)
{
   clearlcd();
   LCD_NChar_Position(1,0);
   printStringOnLCD("  LOAD ERROR! ");
   printCharOnLCD(c);
   for(;;);
}
cystatus CyBtldrCommWrite(uint8* buffer, uint16 size, uint16* count, uint8 timeOut)
{
   char buf[20];
   uint32 bread;
   uint16 err;   
   Bootloader_1_ENTER_t* h;
   Bootloader_1_ENTER_t header;

   switch (state) {
      case 0:
         AlfatUart_PutString("R 0^>E");
         bread = 14;
         err = AlfatReadBytes(buf,&bread); // buf has data, numbytes = bytes read
         if(err == 0x3030) {
            memset(&header,0,sizeof(Bootloader_1_ENTER_t));
            header.id = HEX2UINT32((uint8*)&buf[0]);
            header.rev = HEX2UINT8((uint8*)&buf[8]); 
            header.cs = HEX2UINT8((uint8*)&buf[10]);
            h = (Bootloader_1_ENTER_t*)&buffer[4];
            if( (h[0].id == header.id) && (h[0].rev == header.rev) ) { 
               state = 1; 
            }
         }
         else { 
            LoadErr(0x30);
         }
      case 1:
         if(buffer[1] != 0x00) {
            LoadErr(0x32);
         }
      break;
   }
   return 0;
}
cystatus CyBtldrCommRead (uint8* buffer, uint16 size, uint16* count, uint8 timeOut)
{
#define bufSize 600
   uint32 bread,i;
   uint16 err;
   int16 len;
   char buf[bufSize]; // max expected is 590 
   
   buffer[0] = 0x01;    // SOP [0]
   switch(state)
   {
      case 0:
         buffer[1] = 0x38;             // enter bootloader
         buffer[2] = buffer[3] = 0x00; // length
         ((uint16*)&buffer[4])[0] = Bootloader_CalcPacketChecksum(buffer,4);
         buffer[6] = 0x17;             // EOP
         count[0] = 7;                 // total count of bytes in buffer
         return CYRET_SUCCESS;
      case 1:
         AlfatUart_PutString("R 0^>B");
         bread = 11;
         err = AlfatReadBytes(buf,&bread); // buf has data, numbytes = bytes read
         if(err == 0x3030 && bread == 11 && buf[0] == ':') {
            buffer[1] = 0x39;    // program row [1]
            ((uint16*)&buffer[2])[0] = bread = len = HEX2UINT16((uint8*)&buf[7]); // len of data [3:2]
            ((uint16*)&buffer[2])[0] += 3;
            buffer[4] = HEX2UINT8((uint8*)&buf[1]);   // flash array id       [4]
            ((uint16*)&buffer[5])[0] = HEX2UINT16((uint8*)&buf[3]);  // flash row number [6:5]
            bread = bread * 2 + 4;              // read in all + cs + \r\n
            if(bread >= bufSize || (len + 7) > size) {
               LoadErr(0x31);
            }
            else
            {
               LCD_NChar_Position(2,0);
               buf[11] = 0;
               printStringOnLCD("    ");
               printStringOnLCD(buf);
               AlfatUart_PutString("R 0^>");
               UINT8_2HEX(buf,bread >> 8);
               UINT8_2HEX(&buf[2],bread);
               buf[4] = 0;
               AlfatUart_PutString(buf);
               i = bread;
               err = AlfatReadBytes(buf,&bread); // buf has data, numbytes = bytes read
               if(err == 0x3030 && bread == i) { 
                  for(count[0] = 7, bread = 0; len > 0; count[0]++, len--, bread += 2 ) {    // save two for cs
                     buffer[count[0]] = HEX2UINT8((uint8*)&buf[bread]);
                  }
                  ((uint16*)&buffer[count[0]])[0] = Bootloader_CalcPacketChecksum(buffer,count[0]);
                  count[0] += 2;
                  buffer[count[0]++] = 0x17;
                  return CYRET_SUCCESS;
               }
               else {
                  LoadErr(0x32);
               }
            }
         } //end    if(err == 0x3030 && bread == 11 && buf[0] == ':') {
         else
         {
          Bootloader_1_SET_RUN_TYPE(Bootloader_1_START_APP);
          CySoftwareReset();
          //  Bootloader_1_LaunchApplication();
           // LaunchApp(Bootloader_1_BL_APP_ENTRY_ADDR);
         }
         break;
      case 9:
         buffer[1] = 0x3B;             // enter bootloader
         buffer[2] = buffer[3] = 0x00; // length
         ((uint16*)&buffer[4])[0] = Bootloader_CalcPacketChecksum(buffer,4);
         buffer[6] = 0x17;             // EOP
         count[0] = 7;                 // total count of bytes in buffer
         return CYRET_SUCCESS;
      break;
   }
   return CYRET_EMPTY;
}
/*******************************************************************************
* Function Name: NIB2HEX()
********************************************************************************
* Summary:     converts nibble uint8 to hex char
* Parameters:  uint8 n nibble to convert
* Return:     uint8 result
*******************************************************************************/
uint8 NIB2HEX(uint8 n)
{
   n &= 0x0f;
   if(n > 9) { // a-f
      n += 0x07;      
   }
   n += 0x30;
   return n;
}
/*******************************************************************************
* Function Name: UINT8_2HEX()
********************************************************************************
* Summary:     converts uint8 to hex chars
* Parameters:  uint8* c to store the two hex codes, uint8 u to convert
* Return:     none
*******************************************************************************/
void UINT8_2HEX(char *buf,uint8 u)
{
   buf[0] = NIB2HEX(u >> 4);
   buf[1] = NIB2HEX(u);
}
/*******************************************************************************
* Function Name: HEX2DEC(char c)
********************************************************************************
* Summary:     converts char to dec
* Parameters:  char to convert
* Return:      dec value ('2' = 2) (0x32 = 2)
*******************************************************************************/
uint8 HEX2DEC(uint8 c)
{
   if(c > 0x39) {
      c = c & 0xDF; // MAKE CAP
      c = c - 0x37;
   }
   c = c & 0x0f;
   return c;
}
/*******************************************************************************
* Function Name: HEX2UINT8(char c)
********************************************************************************
* Summary:     converts char[] to uint8
* Parameters:  char to convert
* Return:      uint8 result
*******************************************************************************/
uint8  HEX2UINT8(uint8* buf)
{
   uint8 u = HEX2DEC((uint8)buf[0]);
   u = (u << 4) + HEX2DEC((uint8)buf[1]);
   return u;
}
/*******************************************************************************
* Function Name: HEX2UINT16(char c)
********************************************************************************
* Summary:     converts char[] to uint16
* Parameters:  char to convert
* Return:      uint16 result
*******************************************************************************/
uint16 HEX2UINT16(uint8* buf)
{
   uint16 u = HEX2UINT8(buf);
   u = (u << 8) | HEX2UINT8(&buf[2]);
   return u;
}
/*******************************************************************************
* Function Name: HEX2UINT8(char c)
********************************************************************************
* Summary:     converts char[] to uint8
* Parameters:  char to convert
* Return:      uint32 result
*******************************************************************************/
uint32 HEX2UINT32(uint8* buf)
{
   uint32 u = HEX2UINT16(buf);
   u = (u << 16) | HEX2UINT16(&buf[4]);
   return u;
}
/*******************************************************************************
* Function Name: AlfatWaitError(uint64 time)
********************************************************************************
* Summary:     Waits for errorcode
* Parameters:  time to wait, string to send alfat
* Return:      error code
*******************************************************************************/
uint16 AlfatWaitError(uint32 time, char* str)
{
   uint32 timer = 0;
   alfatError[0] = 0;
   alfatString[0] = '\0';
   alfatData = false;
   alfatRxPtr = 0;
   alfatEwhich = 0;
   if(str[0] != '\0') { AlfatUart_PutString(str); }
   while(timer < time)
   {
      CyDelay(1);
      timer++;
      if(alfatError[0] != 0) 
      { 
        break; 
      }
   }
   return alfatError[0];
}  
/*******************************************************************************
* Function Name: AlfatReadStatusReg
********************************************************************************
* Summary: Read Status Register
*          J<LF>        // sent
*          !00<LF>            // ok response  Signifies starting Calculations
*          $ss<LF>            // ok response  
*          !00<LF>            // ok response  Finished Calculations
* Parameters: none
* Return: (error << 16) | status
*******************************************************************************/
uint32 AlfatReadStatusReg(void)
{
   uint32 res;
   uint32 status;
   res = AlfatWaitError2(10000,"J\r");
   status = strtol((char*)&alfatStringD[0][1],0,16);
   res = (res << 16) | status;
   return res;
}
/*******************************************************************************
* Function Name: AlfatWaitError2(uint64 time)
********************************************************************************
* Summary: Waits for second errorcode
* Parameters:  time to wait, string to send alfat
* Return: error code
*******************************************************************************/
uint16 AlfatWaitError2(uint32 time, char* str)
{
   uint32 timer = 0;
   alfatError[0] = alfatError[1] = 0;
   alfatString[0] = '\0';
   alfatStringD[0][0] = alfatStringD[1][0] = alfatStringD[2][0] = '\0';
   alfatData = false;
   alfatRxPtr = 0;
   alfatEwhich = alfatDwhich = 0;
   AlfatUart_PutString(str);
  
   while(timer < time)
   {
      CyDelay(1);
      timer++;
      if ( ( alfatError[0] != 0 ) && ( alfatError[0] != 0x3030 ))
      {
       return alfatError[0];
      }
      
      if ( alfatError[1] != 0 )
      { 
        break;
      }
   }
   return alfatError[1];
}
/*******************************************************************************
* Function Name: AlfatRead
********************************************************************************
* Summary: Reads numBytes of data from uart
*          Time out is reset on reception of each byte.
* Parameters:  char* buf         (buffer to store data)
*              uint8 bufLen      (len of buffer)
* Return: number of bytes read
*******************************************************************************/
uint32 AlfatRead(char* buf,uint32 numBytes)
{
   uint32 length = 0;
   uint32 timer;
   
   timer = 10000;   
   while(timer-- > 0) // first 4 bytes are !00\n
   {
      if(alfatRdPtr != alfatRxPtr)
      {     
         buf[length++] = alfatRxBuf[alfatRdPtr++];
         if(length >= numBytes) { break; }
      }
      else {  CyDelay(1); }
   }   
   return length;
}
/*******************************************************************************
* Function Name: AlfatReadBytes
********************************************************************************
* Summary: Reads numBytes of data from uart
*          Time out is reset on reception of each byte.
* Parameters:  char* buf         (buffer to store data)
*              uint8 bufLen      (len of buffer)
* Return: number of bytes received
*******************************************************************************/
uint32 AlfatReadBytes(char* buf,uint32 *numBytes)
{
   char err[11];
   uint32 len;
   uint32 len2;

   alfatData = true;
   alfatRxPtr = alfatRdPtr = 0;
   AlfatUart_PutChar('\r');
   len  = AlfatRead(err,4); // first 4 bytes are !00\n
   if(len < 4) {
      return 0;
   }
   memcpy((char*)&alfatError[0],(char*)&err[1],2);
   if(alfatError[0] != 0x3030) {
      return (0); // did not accept read command
   }
   len = AlfatRead(buf,numBytes[0]); // read data bytes
   if(len < numBytes[0]) {
      return len; // return what was read
   }
   len2  = AlfatRead(err,10); // next 10 bytes are length sent
   if(len2 < 10 ) {
      return len; // return what was read
   }
   numBytes[0] = strtol(&err[1],0,16);
   len2 = AlfatRead(err,4); // last 4 bytes are !00\n
   if(len2 == 4) { 
      memcpy((char*)&alfatError[0],(char*)&err[1],2); 
   }
   return alfatError[0];
}



/*******************************************************************************
* Function Name: Alfat Start()
********************************************************************************
* Summary: raises alfat wake and reset lines, starts uart and interrupt
* Parameters:  none
* Return: none
*******************************************************************************/
void AlfatStart()
{
 
  ALFAT_EN_Write ( 1 ); // turn on power
  AlfatReset_Write(0);   
  CyDelay(100);
  AlfatReset_Write(1);
  
  AlfatRxtInt_StartEx(AlfatRxISR);
  AlfatUart_Start();
  AlfatWaitError(500,"");
  CyDelay(30);
}


/*******************************************************************************
* Function Name: AlfatGetVersion
********************************************************************************
* Summary: Queries Alfat for version, blocks for timeOutMs or result obtained
*          V<LF>        // sent 
*          vx.x.x<LF>   // result - buffer should be at least 7 bytes long
*          !00<LF>      // ok response
* Parameters: ALFAT_QUERY aq  // Struct with timeout and response buffer(must be at least 20 bytes)
* Return: Error code
*******************************************************************************/
uint16 AlfatGetVersion(  char *version )
{
   uint16 res;
   res = AlfatWaitError(250,"V\r");
   strcpy(version,(char*)alfatString);
   return res;
}
/* [] END OF FILE */
