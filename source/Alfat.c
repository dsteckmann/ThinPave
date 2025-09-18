/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/* ========================================
 *
 * Copyright InstroTek, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek.
 *
 * ========================================
*/

#include "Globals.h"
#include "Keypad_functions.h"
#include "Alfat.h"
#include "LCD_drivers.h"

// added to check github

#define TIME_OUT_MS 10000       // TIME TO WAIT FOR CHAR FROM ALFAT
#define alfatDBufSize 23

ALFAT_DATE_TIME_UNION time_date;

volatile uint8_t alfatRxBuf[alfatRxBufSize + 1];
volatile uint16 alfatRxPtr = 0, alfatError[2];
volatile BOOL   alfatData = false;
volatile char alfatString[alfatRxBufSize + 1];     // keep this as small as possible
volatile char alfatStringD[4][alfatDBufSize + 1];  // keep this as small as possible
volatile uint8 alfatDwhich = 0, alfatEwhich = 0;

uint16 alfatRdPtr = 0;

#define DEGREE_CHAR 0xDF
uint8 alfat_rx_debug = 0;

FILE_PARAMETERS g_myfile;

/*******************************************************************************
* Function Name: CY _ISR(AlfatRxISR)
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
      alfatRxPtr &= 0xff;
     
      if( ( ch == '\n' || alfatRxPtr == alfatRxBufSize ) && alfatData == false && alfat_rx_debug == 0 )
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
/*******************************************************************************
* Function Name: AlfatWaitError(uint64 time)
********************************************************************************
* Summary: Waits for errorcode
* Parameters:  time to wait, string to send alfat
* Return: error code
*******************************************************************************/
uint16 AlfatWaitError(uint64 time, char* str)
{
   uint64 timer = 0;
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
   uint64 timer = 0;
   
   while(timer < 100000) // first 4 bytes are !00\n
   {
     CyDelayUs(100); timer++;
		//CyWdtClear();
      if(alfatRdPtr != alfatRxPtr)
      {     
         buf[length++] = alfatRxBuf[alfatRdPtr++];
         timer = 90000;
         if(length >= numBytes) { break; }
      } 
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
   ALFAT_PutChar('\r');
   len  = AlfatRead(err,4); // first 4 bytes are !00\n
   if(len < 4) return 0;
   memcpy((char*)&alfatError[0],(char*)&err[1],2);
   if(alfatError[0] != ALFAT_ERR_SUCCESS) return (0); // did not accept read command
   len = AlfatRead(buf,numBytes[0]); // read data bytes
   if(len < numBytes[0]) return len; // return what was read
   len2  = AlfatRead(err,10); // next 10 bytes are length sent
   if(len2 < 10 ) return len; // return what was read
   numBytes[0] = strtol(&err[1],null,16);
   len2 = AlfatRead(err,4); // last 4 bytes are !00\n
   if(len2 == 4) { memcpy((char*)&alfatError[0],(char*)&err[1],2); }
   return alfatError[0];
}
/*******************************************************************************
* Function Name: AlfatEnableEcho
********************************************************************************
* Summary: Enables/Disables Echo
*          #<SP>n<LF>    // sent          #define ALFAT_DISABLE_ECHO    0
*          !00<LF>      // ok response    #define ALFAT_ENABLE_ECHO     1
* Parameters:  uint8 state (state = 0 disable echo else enable echo)
* Return: Error code
*******************************************************************************/
uint16 AlfatEnableEcho(uint8 State)
{
   if(State == ALFAT_DISABLE_ECHO)
   {
      return AlfatWaitError(10000,"# 0\r");  //return SndStrAlfatRes("# 0\r",&aq);
   }
   return AlfatWaitError(10000,"# 1\r");     //return SndStrAlfatRes("# 1\r",&aq);
}

/*******************************************************************************
* Function Name: AlfatInitialize RealTimeClock
********************************************************************************
* Summary: Initialize Real Time Clock
*          Z<SP>n<LF>    // sent          #define ALFAT_RTC_SHARED_MODE 0
*          !00<LF>       // ok response   #define ALFAT_RTC_BACKUP_MODE 1
* Parameters:  uint8 Mode
* Return: Error code
*******************************************************************************/
uint16 AlfatInitializeRealTimeClock(uint8 Mode)
{
   if(Mode == ALFAT_RTC_SHARED_MODE)
   {
      return AlfatWaitError(10000,"T S\r");
   }
   return AlfatWaitError(10000,"T B\r");
}
/*******************************************************************************
* Function Name: AlfatInitMn tDevice
********************************************************************************
* Summary: Initialize and Mount Drive by Name
*          I<SP>X:<LF>        // sent
*          !00<LF>            // ok response
* Parameters:  char *drive (drive name - defined in header)
* Return: Error code
*******************************************************************************/
uint16 AlfatInitMntDevice(char *drive)
{
   ALFAT_PutString("I ");
   ALFAT_PutString(drive);
   return AlfatWaitError(10000,":\r");  
}

/*******************************************************************************
* Function Name: AlfatInitFilesFoldersList
********************************************************************************
* Summary: Initialize Files and Folders List
*          @<SP>full path<LF>    // sent
*          !00<LF>               // ok response
* Parameters: char *path         // full path to file or folder (Initialize folder M:\TEST\TMP)
* Return: Error code
*******************************************************************************/
uint16 AlfatInitFilesFoldersList(char *path)
{
   ALFAT_PutString("@ ");
   ALFAT_PutString(path);
   return AlfatWaitError(10000,"\r");
}
/*******************************************************************************
* Function Name: AlfatFileOpen
********************************************************************************
* Summary: Open File for Read, Write, or Append ('R','W','A')
*          Does not check for file already opened
*          O nM>path<LF>            // n=file handle, M = mode, path = full path to file
*          !00<LF>                  // ok response  Finished Calculations
* Parameters: FILE_PARAMETERS *fp   // struct to hold the information
*             uint32 timeout        // time to wait for response
* Return: Error code
*******************************************************************************/
uint16 AlfatFileOpen(FILE_PARAMETERS *fp)
{   
   char buf[5];
   snprintf(buf,5,"O %X",fp->fileHandle);
   ALFAT_PutString(buf);
   ALFAT_PutChar(fp->mode);
   ALFAT_PutChar('>');
   ALFAT_PutString(fp->fileAttr.fname);
   return AlfatWaitError(10000,"\r");
}
/*******************************************************************************
* Function Name: AlfatFlushData
********************************************************************************
* Summary: Flush data to drive
*          F n<LF>              // n = fileHandle
*          response: !00
* Parameters: uint8 File Handle
* Return: Error code
*******************************************************************************/
uint16 AlfatFlushData(uint8 FileHandle)
{
   char buf[5];
   snprintf(buf,5,"F %X\r",FileHandle);
   return AlfatWaitError(10000,buf);
}
/*******************************************************************************
* Function Name: AlfatCloseFile
********************************************************************************
* Summary: close file
*          C n<LF>              // n = fileHandle
*          response !00
* Parameters: uint8 File Handle
* Return: Error code
*******************************************************************************/
uint16 AlfatCloseFile(uint8 FileHandle)
{
   char buf[5];
   snprintf(buf,5,"C %X\r",FileHandle);
   return AlfatWaitError(10000,buf);
}
/*******************************************************************************
* Function Name: AlfatDeleteFile
********************************************************************************
* Summary: Delete file from drive
*          D<SP>FULL PATH<LF>
*          response !00
* Parameters: uint8* path         // full path to file to delete
* Return: Error code
*******************************************************************************/
uint16 AlfatDeleteFile(uint8 *path)
{
   ALFAT_PutString("D ");
   ALFAT_PutString((char*)path);
   return AlfatWaitError(10000,"\r");
}
/*******************************************************************************
* Function Name: AlfatRenameFile
********************************************************************************
* Summary: Rename file
*          A<SP>FULL PATH>NEW FILE NAME
*          response: !00
* Parameters: uint8* path          // complete path to file
*             uint8* new name      // just the new name not the path
* Return: Error code
*******************************************************************************/
uint16 AlfatRenameFile(uint8 *path,uint8 *newName)
{
   ALFAT_PutString("A ");
   ALFAT_PutString((char*)path);
   ALFAT_PutChar('>');
   ALFAT_PutString((char*)newName);
   return AlfatWaitError(10000,"\r");
}
/*******************************************************************************
* Function Name: AlfatFileSeek
********************************************************************************
* Summary: Change file pointer position in a file
*          P n>ssssssss<LF>              // n = fileHandle
*          response !00
* Parameters: uint8 File Handle
*             uint32 Seek Position
* Return: Error code
*******************************************************************************/
uint16 AlfatFileSeek(uint8 FileHandle,uint32 seek)
{
   char buf[15];
   snprintf(buf,15,"P %X>%lX\r",FileHandle,seek);
   return AlfatWaitError(10000,buf);
}
/*******************************************************************************
* Function Name: AlfatSetBaudRate
********************************************************************************
* Summary: Set UART Baud Rate
*          B<SP>ssssssss<LF>  // sent
*          !00<LF>            // ok response
*          !00<LF>            // ok response  This sent after baud changes - we don't catch it
* Parameters:  uint32 baud rate
* Return: Error code
*******************************************************************************/
uint16 AlfatSetBaudRate(uint32 baud)
{
   char buf[12];
   snprintf(buf,12,"B %lX\r",baud);
   return AlfatWaitError(10000,buf);  
}
/*******************************************************************************
* Function Name: AlfatDeleteFolder
********************************************************************************
* Summary: Delete folder from drive
*          Flushes RX buffer
*          D<SP>FULL PATH[\]<LF>
*
* Parameters: uint8* path          // complete path to folder
*
* Return: Error code
*******************************************************************************/
uint16 AlfatDeleteFolder(uint8 *path)
{
   ALFAT_PutString("D ");
   ALFAT_PutString((char*)path);
   return AlfatWaitError(10000,"\\\r");
}
/*******************************************************************************
* Function Name: AlfatSetCurrentTimeDate
********************************************************************************
* Summary: Set Current Time Date
*          S<SP>ddddtttt<LF>  // sent
*          !00<LF>            // ok response
* Parameters:  ALFAT_DATE_TIME *dateTime (pointer to date time structure)
* Return: Error code
*******************************************************************************/
uint16 AlfatSetCurrentTimeDate( uint32 time )
{
   char buf[20];
   AlfatInitializeRealTimeClock ( ALFAT_RTC_SHARED_MODE );
   snprintf(buf,20,"S %lX\r",time);
   return AlfatWaitError(2000,buf);
}
/*******************************************************************************
* Function Name: AlfatGetCurrentTime
********************************************************************************
* Summary: Get Current Time
*          G<SP>T<LF>  // sent
*          HH:MM:SS<LF>
*          !00<LF>            // ok response
* Parameters: ALFAT_DATE_TIME_UNION au pointer
* Return: Error code
*******************************************************************************/
uint16 AlfatGetCurrentTime(ALFAT_DATE_TIME_UNION *au)
{
   uint16 res;
   res = AlfatWaitError(4000,"G T\r");   
   au[0].dateTimeUint32 &= 0xFFFF0000; // clear time bits
   au[0].dateTimeUint32 |= AlfatConvertTime((char*)alfatString);
   return res;
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
uint16 AlfatGetVersion(char *version)
{
   uint16 res;
   res = AlfatWaitError(500,"V\r");
   strcpy(version,(char*)alfatString);
   return res;
}
/*******************************************************************************
* Function Name: AlfatFormat
********************************************************************************
* Summary: Alfat confirm format
*          Q CONFIRM FORMAT X        // sent 
*          !00   
*          !00<LF> // MAY TAKE A LONG TIME
* Parameters: char * drive (M:,U0:,U1:)
* Return: Error code
*******************************************************************************/
uint16 AlfatFormat(char *drive)
{
   ALFAT_PutString("Q CONFIRM FORMAT ");
   ALFAT_PutString(drive);
   return AlfatWaitError2(60000,"\r");
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
   uint32 res= 0 ;
   uint32 status;
   res = AlfatWaitError2(1000,"J\r");
   status = strtol((char*)&alfatStringD[0][1],null,16);
   res = (res << 16) | status;
   return res;
}
/*******************************************************************************
* Function Name: AlfatGetFreeSize
********************************************************************************
* Summary: Get Free Size
*          K<SP>X<LF>            // sent
*          !00<LF>               // ok response  Signifies starting Calculations
*          $ssssssssssssssss<LF> // ok response  
*          !00<LF>               // ok response  Finished Calculations
* Parameters: uint64* size pointer to store result char * drive
* Return: Error Code
*******************************************************************************/
uint16 AlfatGetFreeSize(uint64 *freeSize, char* drive )
{
   uint16 error;
   ALFAT_PutString("K ");
   ALFAT_PutString(drive);
   error = AlfatWaitError2(10000,"\r");
   freeSize[0] = strtoll((char*)&alfatStringD[0][1],null,16);
   return error;
}
/*******************************************************************************
* Function Name: AlfatFileTell
********************************************************************************
* Summary: Change file pointer position in a file
*          Y n<LF>              // n = fileHandle
*          response: !00
*                    $ssssssss
*                    !00
* Parameters: uint8 File Handle  uint32* position     pointer to hold position
* Return: Error code
*******************************************************************************/
uint16 AlfatFileTell(uint8 FileHandle,uint32 *pos)
{
   char buf[5];
   uint16 error;
   snprintf(buf,5,"Y %c\r",FileHandle);
   error = AlfatWaitError2(10000,buf);
   pos[0] = strtol((char*)&alfatStringD[0][1],null,16);
   return error;
}
/*******************************************************************************
* Function Name: AlfatCopyFile
********************************************************************************
* Summary:  Copy Source File to Destination File
*           M<SP>HANDLE_SRC<SP>INDEX<SP>HANDLE_DES<SP>LENGTH<LF>
*           !00<LF>
*           $xxxx<LF> // actual number of bytes copied
*           !00<LF>
* Parameters: char fpSource,fpDest file handles (0-15) indx-start index, len to copy
* Return: Error code
*******************************************************************************/
uint16 AlfatCopyFile(char fpSource,char fpDest,int32 index, int32 len)
{
   uint16 error;
   char buf[26];
   snprintf(buf,26,"M %X %lX %X %lX\r",fpSource,index,fpDest,len);
   error = AlfatWaitError2(10000,buf);
   len = strtol((char*)&alfatStringD[0][1],null,16);
   return error;
}
/*******************************************************************************
* Function Name: AlfatTestSpeed
********************************************************************************
* Summary:  Test the speed of medium  
*           e X ssssssss
*           !00<LF>
*           $aaaaaaaa<LF> // ms for write
*           $bbbbbbbb<LF> // ms for read
*           !00<LF>
* Parameters: int32 *number bytes to write int32 *read time
* Return: numBytes = write time readTime = read time, error code
*******************************************************************************/
uint16 AlfatTestSpeed(int32* numBytes, int32* readTime)
{
   char buf[12];
   uint16 error;
   snprintf(buf,12,"E %lX\r",numBytes[0]);
   error = AlfatWaitError2(60000,buf);
   numBytes[0] = strtol((char*)&alfatStringD[0][1],null,16);
   readTime[0] = strtol((char*)&alfatStringD[1][1],null,16);
   return error;
}
/*******************************************************************************
* Function Name: AlfatFindFile
********************************************************************************
* Summary:  Find file
*           ?<SP>FULL PATH<LF>
*           !00<LF>
*           $ssssssss<LF>
*           $AA<LF>
*           $hh:mm:ss<SP>mm-dd-yyyy<LF>
*           !00<LF>
* Parameters: FILE_ENTRY *fe  // containing full path to file
* Return: Error code
*******************************************************************************/
uint16 AlfatFindFile(FILE_ENTRY *fe)
{
   uint16 error;
   ALFAT_PutString("? ");
   ALFAT_PutString(fe->fname);
   error = AlfatWaitError2(10000,"\r");
   fe->fileSize = strtol((char*)&alfatStringD[0][1],null,16);
   fe->attributes = (uint8)strtol((char*)&alfatStringD[1][1],null,16);
   strcpy(fe->dateTimeMod,(char*)&alfatStringD[2][1]);
   return error;
}
/*******************************************************************************
* Function Name: AlfatGetNextDirectoryEntry
********************************************************************************
* Summary: Get Next Directory Entry
*          N<LF>                 // sent
*          !00<LF>               // ok response  Finished Calculations
*          NNNNN.EEE<LF>         // Filename and extension
*          $AA<LF>               // file attributes
*          $ssssssss<LF>         // filesize (4 bytes)
*          !00<LF>               // ok response  Finished Calculations
* Parameters: FILE_ENTRY *fe     // struct to hold the information
* Return: Error code*******************************************************************************/
uint16 AlfatGetNextDirectoryEntry(FILE_ENTRY *fe)
{
   uint16 error = AlfatWaitError2(60000,"N\r");
   memcpy(fe->fname,(char*)&alfatString[1],FAT32_MAX_FILENAME_LENGTH - 1);
   fe->fname[FAT32_MAX_FILENAME_LENGTH] = '\0';
   fe->attributes = (uint8)strtol((char*)&alfatStringD[0][1],null,16);
   fe->fileSize = strtol((char*)&alfatStringD[2][1],null,16);
   return error;
}
/*******************************************************************************
* Function Name: AlfatReadFromFile
********************************************************************************
* Summary: Read Data From File
*          Opens it if it has not already been opened
*          R nM>ssssssss<LF>        // n = fileHandle
*          !00<LF>                  // first error code
*          ssssssss                 // requested bytes returned
*          $aaaaaaaa<LF>            // actual number of bytes returned
*          !00<LF>                  // error code
* Parameters: FILE_PARAMETERS *fp   // struct with file parameters filled in
* Return: Error code
*         fp->numBytes will be updated to the actual number bytes read
*******************************************************************************/
uint16 AlfatReadFromFile(FILE_PARAMETERS *fp)
{
   char buf[12];   // buffer to hold the number of bytes to read in string form
   uint16 error = 0;
   uint32  numBytes;
   
   snprintf(buf,11,"R %X",fp->fileHandle);
   ALFAT_PutString(buf);
   ALFAT_PutChar(fp->fillerChar);      
   snprintf(buf,11,">%lX",fp->numBytes);
   ALFAT_PutString(buf);
   numBytes = fp->numBytes;
   fp->numBytes = 0;
   error = AlfatReadBytes((char*)fp->dataBuffer,&numBytes); // buffer has data, numbytes = bytes read
   fp->numBytes = numBytes;
   return error;
}
/*******************************************************************************
* Function Name: AlfatWrit eToFile
********************************************************************************
* Summary: Write Data to File
*          Opens it if it has not already been opened
*          Flushes RX buffer
*          W<SP>n>ssssssss<LF>      // n = fileHandle
*          !00<LF>                  // first error code
*          ssssssss                 // requested bytes returned
*          $aaaaaaaa<LF>            // actual number of bytes returned
*          !00<LF>                  // error code
*
* Parameters: FILE_PARAMETERS *fp   // struct with file parameters filled in
*
* Return: Error code
*         fp->numBytes will be updated to the actual number bytes written
*
*******************************************************************************/
uint16 AlfatWriteToFile(FILE_PARAMETERS *fp)
{
   char buf[15];   // buffer to hold the number of bytes to read in string form
   uint8 bytes2send = 0;
   int32 txIndex = 0;
   uint16 error;
   uint64 time;
   snprintf(buf,12,"W %X>%lX\r",fp->fileHandle,(uint32)fp->numBytes);
   error = AlfatWaitError(5000,buf);
   if(error == ALFAT_ERR_SUCCESS)                            // if no error numBytes should come next
   {
      AlfatUart_ClearRxBuffer();
      alfatRxPtr = 0;
      alfatError[0] = alfatError[1] = 0;
      alfatEwhich = 0;
      alfatDwhich = 0;
      
      while(txIndex < fp->numBytes)
      {
         if( (fp->numBytes - txIndex) > 255)  // put_array is uint8
         {
            bytes2send = 255;
         }
         else
         {
            bytes2send = (uint8)((fp->numBytes - txIndex));
         }
      //  alfat_rx_debug = 1;
        
        ALFAT_PutArray(&fp->dataBuffer[txIndex], bytes2send);
        txIndex += bytes2send;
      }
   
    //  alfat_rx_debug = 0;
      time = 5000;
      while(time > 0)
      {
         CyDelay(1); time--;
         if(alfatError[0] != 0) { break; }
      }
      error = alfatError[0]; // gets response and error code (12 bytes)
      fp->numBytes = strtol((char*)&alfatStringD[0][1], NULL, 16);
   }
   return error;
}
/*******************************************************************************
* Function Name: AlfatConvertDTunion()
********************************************************************************
* Summary: converts ALFAT_DATE_TIME_UNION to a string using AM PM
* Parameters:  none
* Return: none
*******************************************************************************/
void AlfatConvertDTunion(ALFAT_DATE_TIME_UNION au,char *str)
{
	int8 pm = 0;
	int16 y;
   uint8 hour;
   hour = au.dateTimeBits.Hour;
	if(hour >= 12) { // 0- 23 0 = 12 am, 12 = 12 pm, 23 is 11 pm
		if(hour > 12) { hour -= 12; }
		pm = 1;
	}
   if(hour == 0) hour = 12;
	y = au.dateTimeBits.Years + 1980;
	snprintf(str,18,"%02d/%02d/%02d %02d:%02d ",au.dateTimeBits.Month,au.dateTimeBits.Day,y % 100
														,hour,au.dateTimeBits.Minute);
	if(pm ==1) strcat(str,"PM");
	else strcat(str,"AM");
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
 if ( usb_start == 0 )
 {
   ALFAT_EN_Write ( 1 );
   AlfatReset_Write(0);   // Keep in reset
   CyDelay(10);
   AlfatReset_Write(1);
   CyDelay(1000);
   AlfatUart_Start();
   AlfatRxtInt_Enable();
   usb_start = 1;
 }
}
/*******************************************************************************
* Function Name: initAlfat()
********************************************************************************
* Summary: raises alfat wake and reset lines, starts uart and interrupt
* Parameters:  none
* Return: none
*******************************************************************************/
void initAlfat()
{
 AlfatUart_Start();  
 AlfatRxtInt_StartEx(AlfatRxISR);
 AlfatStop();
 usb_start = 0;
 
}


/*******************************************************************************
* Function Name: AlfatStop
********************************************************************************
* Summary: Stops Alfat reset low,wakeup low,pwr ctrl low
* Parameters:  none
* Return: none
*******************************************************************************/
void AlfatStop()
{
// AlfatUart_Stop();
// AlfatRxtInt_Disable();
// ALFAT_EN_Write ( 0 );
// AlfatReset_Write(0);   // Keep in reset
 
}
/*******************************************************************************
* Function Name: AlfatConvertTime
********************************************************************************
* Summary: Convert Time String to ALFAT_DATE_TIME_UNION
* Parameters: ALFAT_QUERY aq  // Struct with timeout and response buffer(must be at least 20 bytes)
*
* Return: Error code
*******************************************************************************/
uint32 AlfatConvertTime(char* time)
{// HH:MM:SS
	char *h,*m,*s;
	ALFAT_DATE_TIME_UNION u;
	u.dateTimeUint32 = 0;
	h=time; m=s=null;
	m=strchr(h,':');
	if(m==null) return(u.dateTimeUint32);
   m++;
	s=strchr(m,':');
	u.dateTimeBits.Hour=strtol(h,null,10);
	u.dateTimeBits.Minute=strtol(m,null,10);
	if(s==null) { s = 0; }
	u.dateTimeBits.Second2=strtol(s,null,10)/2;
	return (u.dateTimeUint32);
}
/*******************************************************************************
* Function Name: AlfatConvertDate
********************************************************************************
* Summary: Convert date String to ALFAT_DATE_TIME_UNION
* Parameters: ALFAT_QUERY aq  // Struct with timeout and response buffer(must be at least 20 bytes)
* Return: Error code
*******************************************************************************/
uint32 AlfatConvertDate(char* date)
{ //MM-DD-YYYY<LF>   // 2006-12-01T04:00:00.000< yyyy-mm-dd
	char *m,*d,*y,*t;
   int32 years;
	ALFAT_DATE_TIME_UNION u;
	u.dateTimeUint32 = 0;
	m=date; d=y=null;
	d=strchr(m,'-');
	if(d==null) { // some have dashes some have slashes
      d=strchr(m,'/');
       if(d==null) { return(u.dateTimeUint32); } // return 0 if neither is found
   }
   d++;
	y=strchr(d,'-');
	if(y==null) {
	   y=strchr(d,'/');
   	if(y==null) { return(u.dateTimeUint32); } // return 0 if neither is found, must have both
   }
	y++;
   t = strchr(date,'T');
   if(t != null) { // excell style
      
      t[0] = ' '; t = y; y = m; m = d; d = t;
   }
	u.dateTimeBits.Day=strtol(d,null,10);
	u.dateTimeBits.Month=strtol(m,null,10);
   years=strtol(y,null,10); // since 1980
   if(years > 1979) {
      years -= 1980;
   }
   else {
      years += 20;
   }
   u.dateTimeBits.Years = years;
	return (u.dateTimeUint32);
}
///*******************************************************************************
//* Function Name: AlfatDiag
//********************************************************************************
//* Summary: Diagnostics for Alfat Opens test file, writes to file, reads from file
//* Parameters: none
//* Return: none
//*******************************************************************************/
void AlfatDiag(void)
{
   uint32 err;
   char buf[11];
   FILE_PARAMETERS fp;
 
   DisplayStrCentered(LINE1, "ALFATMOUNT");
   err = AlfatInitMntDevice("U0");
   if(err == ALFAT_ERR_SUCCESS)
   {
      DisplayStrCentered(LINE2,"ALFATOPENFILE");
      fp.fileAttr.fname = "U0:\\tst.txt";
      strcpy(buf,"0123456789");
      fp.mode = ALFAT_FILE_OPEN_WRITE;
      fp.fileHandle = 0;
      fp.dataBuffer = (uint8*)buf;   
      fp.numBytes = 10;
      err = AlfatFileOpen(&fp);
      if(err == ALFAT_ERR_SUCCESS)
      {
         DisplayStrCentered(LINE3, "ALFATWRITING" );   
         DisplayStrCentered(LINE4,"0123456789");
         err = AlfatWriteToFile(&fp);
         AlfatCloseFile(0);
         CyDelay(1000);
         if(err == ALFAT_ERR_SUCCESS)
         {
            clearlcd();
            DisplayStrCentered(LINE1, "ALFATREADING ");
            fp.mode = ALFAT_FILE_OPEN_READ;
            err = AlfatFileOpen(&fp);
            if(err == ALFAT_ERR_SUCCESS)
            {
               fp.dataBuffer = (uint8*)buf;
               fp.numBytes = 10;
               err = AlfatReadFromFile(&fp); // err is last error received, fp.numbytes has number of bytes read, fp.dataBuffer has data read
               if(err == ALFAT_ERR_SUCCESS) //alfatError[0]
               {
                  DisplayStrCentered(LINE2, "ALFATREAD" );
                  buf[10] = '\0';
                  DisplayStrCentered(LINE3,buf);
               }
               AlfatCloseFile(0);
            }
         }
      }
   }
   if(err != ALFAT_ERR_SUCCESS)
   {
      printAtPosLCD(LINE4, "ALFATFAILED" );
      sprintf(buf,"%04X",(uint16)err);
      printAtPosLCD(LINE4 + strlen("ALFATFAILED"),buf);
   }
   else
   {
      printAtPosLCD(LINE4,"Press <ESC>" );
   }
	while(  getKey( TIME_DELAY_MAX )  != ESC ) { };

}

void ReplaceIllegalChars(char *str)
{
   uint32 i;
   char c;
   for(i = 0; i < strlen(str); i++) {
      c = str[i];
      if(c < 'A' && (c > '9' || c < '0') ) {
         if(c == '/') c = '_';
         else if(c == ':') c = '_';
         else c = ' ';
         str[i] = c;
      }
   }
}
/*******************************************************************************
* Function Name: Alfat WriteStr
********************************************************************************
* Summary: Write a string to the file
* Parameters:  char * str = string to write FS_FILE * file to write to
* Return: none
*******************************************************************************/
void AlfatWriteStr(FILE_PARAMETERS *fp,char *str)
{
   fp->dataBuffer = (uint8*)str;
   fp->numBytes = strlen(str); 
   AlfatWriteToFile(fp);
}

/*******************************************************************************
* Function Name: AlfatOpenUSB
********************************************************************************
* Summary:     Checks for and mounts the usb, opens the file, leaves the alfat on
* Parameters:  
* Return: 0 success
*******************************************************************************/
int32 AlfatOpenUSB(FILE_PARAMETERS *fp)
{
   static int32 err;
   fp->opened = false;
   fp->fillerChar = 0x1F;
   err = AlfatReadStatusReg();
   if( (err & ALFAT_USB0_ATTCHD) != ALFAT_USB0_ATTCHD) // usb is in
   {
    
      clearlcd();
      DisplayStrCentered(LINE1,"USB not Inserted");      
      CyDelay(2000);
      return (-1);
   }
   if((err & ALFAT_USB0_MOUNT) != ALFAT_USB0_MOUNT)
   {
      err = AlfatInitMntDevice("U0");
      if(err != ALFAT_ERR_SUCCESS)
      {
     
         clearlcd();
         DisplayStrCentered(LINE2,"USB ERROR MOUNT");
         CyDelay(2000);
         return (-2);
      }
   }
   err = AlfatReadStatusReg ( );
  
  // set the rtc date
  read_RTC( &date_time_g ); 
  convertRTCtoAlfatTime ( date_time_g );
  AlfatSetCurrentTimeDate (time_date.dateTimeUint32);
  
   err = AlfatFileOpen ( fp );
   if ( err != ALFAT_ERR_SUCCESS )
   {
   
      clearlcd ( ) ;
      DisplayStrCentered ( LINE2,"USB ERROR OPEN" );
      CyDelay ( 2000 );
      return-3;
   }
   return 0;
}
/*******************************************************************************
* Function Name: AlfatOpenUSBquiet
********************************************************************************
* Summary:     Checks for and mounts the usb, opens the file, leaves the alfat on
* Parameters:  
* Return: 0 success
*******************************************************************************/
int32 AlfatOpenUSBquiet(FILE_PARAMETERS *fp)
{
   int32 err;
   fp->opened = false;
   fp->fillerChar = 0x1F;
   err = AlfatReadStatusReg();
   if( (err & ALFAT_USB0_ATTCHD) != ALFAT_USB0_ATTCHD) // usb is in
   { AlfatStop();  return (-1); }
   if((err & ALFAT_USB0_MOUNT) != ALFAT_USB0_MOUNT)
   {
      err = AlfatInitMntDevice("U0");
      if(err != ALFAT_ERR_SUCCESS) { AlfatStop(); return (-2); }
   }
   err = AlfatFileOpen(fp);
   if(err != ALFAT_ERR_SUCCESS) { AlfatStop(); return-3; }
   return 0;
}



/*******************************************************************************
 *
 * initialize_USB()
 *
 *  PARAMETERS:  TRUE- display error if it occurs
 *
 *  DESCRIPTION: Mounts attached USB storage device. USB should be connected 
 *               before this function is called.
 *
 *  RETURNS:TRUE if USB is mouted successfully.
 *
 ******************************************************************************/
Bool initialize_USB( Bool display_error )
{   
  Bool USB_mounted = 1;
  
  uint32 err;
   
  
  err = AlfatInitMntDevice("U0");    

  if (err != ALFAT_ERR_SUCCESS && display_error == TRUE ) 
  { 
    CLEAR_DISP;
    LCD_position (LINE1);
    _LCD_PRINT ( "  Failed to Mount");  
    LCD_position (LINE2);
    _LCD_PRINT ( "    USB Device.");
    delay_ms(500);
    USB_mounted = FALSE;
  }  
  

  return(USB_mounted);
}


/*******************************************************************************
 *
 *  viod USB_open_file ( void )
 *
 *  PARAMETERS:The name of the file to be opened for write
 *
 *  DESCRIPTION:Opens the specified file for write operations.
 *
 *  RETURNS: TRUE if file successfully opened.
 *
 ******************************************************************************/
Bool USB_open_file ( char *filename , FILE_PARAMETERS * file)    
{
  char fname[70];
  //date[21];
  Bool open = TRUE;
  
   
  sprintf (fname,"U0:%c%s.csv", 0x5C, filename);
  file->fileAttr.fname = fname;
  
  file->mode = ALFAT_FILE_OPEN_WRITE;
  file->fileHandle = 0;
     
  if (AlfatOpenUSB ( file ) != 0) 
  {
   DisplayStrCentered(LINE2,"Failed Writing File");
   open = FALSE;
  }
  

  
  return ( open );
}


/*******************************************************************************
 *
 *  viod USB_open_file_for read ( void )
 *
 *  PARAMETERS:The name of the file to be opened for read
 *
 *  DESCRIPTION:Opens the specified file for read operations.
 *
 *  RETURNS: TRUE if file successfully opened.
 *
 ******************************************************************************/
bool USB_open_file_for_read (  )    
{
  uint16 err;
  bool open = TRUE;
  
  g_myfile.mode = ALFAT_FILE_OPEN_READ;  // change to append mode, once file already exists
  
  err = AlfatFileOpen ( &g_myfile );
  
  if(err != ALFAT_ERR_SUCCESS)
  {
   open = FALSE;
  }

  return ( open );
}

/*******************************************************************************
 *
 *  viod USB_open_file ( void )
 *
 *  PARAMETERS:The name of the file to be opened for write
 *
 *  DESCRIPTION:Opens the specified file for write operations.
 *
 *  RETURNS: TRUE if file successfully opened.
 *
 ******************************************************************************/
Bool USB_open_file_for_append ( FILE_PARAMETERS * file )    
{
  uint16 err;
  Bool open = TRUE;
  
  file->mode = ALFAT_FILE_OPEN_APPEND;  // change to append mode, sonce file already exists
  
  err = AlfatFileOpen ( file );
  
  if(err != ALFAT_ERR_SUCCESS)
  {
   open = FALSE;
  }

  return ( open );
}
/*******************************************************************************
 *
 ** check_for_USB
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION:Checks to see if USB memory device is connected.
 *
 *  RETURNS:TRUE if a valid device is detected.
 *
 ******************************************************************************/
Bool check_for_USB(void)
{
  Bool detect = TRUE;  
  uint8 err = AlfatReadStatusReg();
  
  if( (err & ALFAT_USB0_ATTCHD) != ALFAT_USB0_ATTCHD) // usb is in
  {  
    CyDelay ( 100 );
    err = AlfatReadStatusReg();
  }
  else
  {
    return detect;
  } 
 
  if( (err & ALFAT_USB0_ATTCHD) != ALFAT_USB0_ATTCHD) // usb is in
  {
      
    clearlcd();
      DisplayStrCentered(LINE1,"USB not Inserted");      
      CyDelay(2000);
      detect = FALSE;
  }
  
  return detect;
}

/*******************************************************************************
* Function Name: AlfatMntDevice
********************************************************************************
* Summary:     Checks for and mounts the usb
* Parameters:  NONE
* Return: 0 success, -1 NO USB, -2 USB ERROR MOUNT
*******************************************************************************/
int32 AlfatMntDevice()
{
   static int32 err;
   err = AlfatReadStatusReg();
   if( (err & ALFAT_USB0_ATTCHD) != ALFAT_USB0_ATTCHD) // usb is in
   {
      return (-1);
   }
   if((err & ALFAT_USB0_MOUNT) != ALFAT_USB0_MOUNT)
   {
      err = AlfatInitMntDevice("U0");
      if(err != ALFAT_ERR_SUCCESS)
      {
         return (-2);
      }
   }
   return 0;
}

/*******************************************************************************
* Function Name: Firmware Menu
********************************************************************************
* Summary: Displays the Firmware Update menu
* Parameters:  none
* Return: 		none
*******************************************************************************/
void FirmwareMenu ( )
{
   FILE_ENTRY fe;
   char buf[41];   
   int key;
   char t[DISPLAY_LENGTH];
   alfat_rx_debug = 0;
   
    AlfatStart();
 
    if  ( AlfatGetVersion( t ) == 0x3030 )
    {
      alfat_error = NO_ERROR;
      t[6] = 0;
    }  
    else
    {
      alfat_error = ERROR;
    }
   CLEAR_DISP;
   if ( NO_ERROR == alfat_error )
   {
    printOnLCDLineLocString ( 1, centerStart( strlen(t)), t );
   }  
   else
   {
     printOnLCDLineLocString ( 1,centerStart(11), "USB Error");
   }
   CyDelay ( 1500 );
  
   CLEAR_DISP;
   DisplayStrCentered(LINE1,"Insert Drive with");
   DisplayStrCentered(LINE2,"ThinPAVEUpdate.cyacd");
   DisplayStrCentered(LINE3,"and press <YES>");
   DisplayStrCentered(LINE4,"<ESC> to Exit"); 
  
   key = getKey(TIME_DELAY_MAX);
   if ( YES == key  )
   {
    CLEAR_DISP;
    DisplayStrCentered(LINE1,"Starting Update"); 
    CyDelay(1500);  
   }
    
  
   snprintf(buf,40,"U0:\\ThinPAVEUpdate.cyacd");
   fe.fname = buf;
   if ( AlfatMntDevice() == 0)
    {
      if ( AlfatFindFile(&fe) == ALFAT_ERR_SUCCESS )
      {
        CLEAR_DISP;
        DisplayStrCentered(LINE1,"Do not remove");
        DisplayStrCentered(LINE2,"Drive while loading.");
        CyDelay(2500);
        EEPROM_ByteWrite(0x49, (CYDEV_EE_SIZE - 1) / CYDEV_EEPROM_ROW_SIZE, 15);
        AlfatStop();
        CySoftwareReset();
      }
      else
      {
        CLEAR_DISP;
        DisplayStrCentered(LINE1,"Unable to find");
        DisplayStrCentered(LINE2,"ThinPAVEUpdate.cyacd");
        DisplayStrCentered(LINE3,"on USB Drive");
        CyDelay(3000);
      }  
    }
    else
    {
        CLEAR_DISP;
        DisplayStrCentered(LINE1,"Unable to find");
        DisplayStrCentered(LINE2,"Mount USB Drive");
        CyDelay(3000);
    }
   
  AlfatStop();

}


/* [] END OF FILE */
