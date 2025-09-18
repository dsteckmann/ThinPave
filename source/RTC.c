

/**************************   ***************************************************************** ***************************/
//
// Title:       Elite Explorer
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  Driveers for the EPSON RX 8035
//
// Filename:   
// Author:      
//
// Processor:  
// Clock:	    12MHz	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/
#include "Globals.h"
#include <device.h>
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "Utilities.h"
#include "DataStructs.h"
#include "Alfat.h"

/************************************************************************************/
#define RTC_ADDRESS 0x32
#define I2C_WRITE 0
#define I2C_READ  1

#define RTC_SEC   0<<4
#define RTC_MIN   1<<4
#define RTC_HOUR  2<<4
#define RTC_DAY   4<<4
#define RTC_MONTH 5<<4
#define RTC_YEAR  6<<4

extern ALFAT_DATE_TIME_UNION time_date;



/*
Function:    HexToBCD();

Description: Converts a 2 digit decimal to BCD format.
             Value Byte to be converted

Return:  Converted byte
*/ 
uint8_t HexToBCD (uint8_t Value)
{
  uint32_t bcdhigh = 0U;

  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }

  return ((uint8_t)(bcdhigh << 4U) | Value);
}

/*******************************************************************************
* Function Name: I2C_MasterSendStart
********************************************************************************
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
* Return:
*  Status error - Zero means no errors.
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
* Reentrant:
*  No.
*******************************************************************************/
uint8 I2C_MasterSendStart(uint8 slaveAddress, uint8 R_nW) {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;
    if(I2C_1_SM_IDLE == I2C_1_state) /* If IDLE, check if bus is free */
    {
        /* If bus is free, generate Start condition */
        if(I2C_1_CHECK_BUS_FREE(I2C_1_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            I2C_1_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << I2C_1_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= I2C_1_READ_FLAG;
                I2C_1_state = I2C_1_SM_MSTR_RD_ADDR;
            }
            else
            {
                I2C_1_state = I2C_1_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            I2C_1_DATA_REG = slaveAddress;
            I2C_1_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
            {
              CyDelayUs(1);
              count++;
              if ( count > 100 )
              {
                break; 
               }
            }

            if(I2C_1_CHECK_ADDR_NAK(I2C_1_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                I2C_1_state = I2C_1_SM_IDLE;
                errStatus = I2C_1_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = I2C_1_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = I2C_1_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterSendRestart
********************************************************************************
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
* Return:
*  Status error - Zero means no errors.
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
* Reentrant:
*  No.
*******************************************************************************/
uint8 I2C_MasterSendRestart(uint8 slaveAddress, uint8 R_nW) {
    uint8 errStatus;
    uint8 count = 0;

    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << I2C_1_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= I2C_1_READ_FLAG;
            I2C_1_state = I2C_1_SM_MSTR_RD_ADDR;
        }
        else
        {
            I2C_1_state = I2C_1_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        I2C_1_DATA_REG = slaveAddress;
        I2C_1_GENERATE_RESTART_MANUAL;
        count = 0;
        /* Wait until address has been transferred */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 200 )
         {
          break; 
         }
        }
  
        if(I2C_1_CHECK_ADDR_NAK(I2C_1_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            I2C_1_state = I2C_1_SM_IDLE;
            errStatus = I2C_1_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = I2C_1_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_1_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 I2C_MasterSendStop(void)  {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        I2C_1_GENERATE_STOP_MANUAL;
        I2C_1_state = I2C_1_SM_IDLE;

        /* Wait until stop has been generated */
        while(I2C_1_WAIT_STOP_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
        }

        errStatus = I2C_1_MSTR_NO_ERROR;
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 I2C_MasterWriteByte(uint8 theByte) {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        I2C_1_DATA_REG = theByte;   /* Write DATA register */
        I2C_1_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        I2C_1_state = I2C_1_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
        }

        if(I2C_1_CHECK_DATA_ACK(I2C_1_CSR_REG))
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
            errStatus = I2C_1_MSTR_NO_ERROR;
        }
        else
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
            errStatus = I2C_1_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 I2C_MasterReadByte(uint8 acknNak) {
    uint8 theByte;
    uint8 count = 0;
    theByte = 0u;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(I2C_1_SM_MSTR_RD_ADDR == I2C_1_state)
        {
            I2C_1_READY_TO_READ_MANUAL;
            I2C_1_state = I2C_1_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
         
        }

        if ( count > 100 )
        {
         theByte = 0xAA;
        }
        else
        {
         theByte = I2C_1_DATA_REG;
        }  

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            I2C_1_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
        }
    }

    return(theByte);
}
/*******************************************************************************
* Function Name: writePointerValue
********************************************************************************
* Summary: Write a 8 bit register value to the EPSON RX-8035 RTC
*  
*
* Parameters: pointer to the register
*  None
*
* Return: the 8 bit register value
*  None
*
*******************************************************************************/
uint8 writePointerValue ( uint8 ptr_rtc, uint8 value ) {
 uint8 i2c_error;
 uint8 intState;
 intState = CyEnterCriticalSection(); 

  // write address and pointer 
  i2c_error = I2C_MasterSendStart( RTC_ADDRESS, I2C_1_WRITE_XFER_MODE);  
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte ( ptr_rtc );            // write the pointer address to Device ID ( 0x1004 )
  }  
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte( value );   //read byte, send ACK MSB
      
  }  
  i2c_error =I2C_MasterSendStop(); // send stop, since we want to read from device
  CyDelayUs ( 100 );
  
  CyExitCriticalSection(intState);
  return i2c_error;
   

}
/*******************************************************************************
* Function Name: readPointerValue
********************************************************************************
* Summary: Read a 7, 8 bit register values from the RX-8035
*  
*
* Parameters: pointer to the register
*  None
*
* Return: the 16 bit register value
*  None
*
*******************************************************************************/
uint8 readPointerValue ( uint8* rtc_data ) {

 uint8 i2c_error, n;
 uint8 intState;
 intState = CyEnterCriticalSection(); 
  
  // write address and pointer 
  i2c_error = I2C_MasterSendStart( RTC_ADDRESS, I2C_WRITE);
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte ( 0 );      // write the pointer address 
  }  
  
  // read byte from device 
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterSendRestart( RTC_ADDRESS, I2C_READ);    // set for read mode
  }  
    
  if ( i2c_error == 0 )
  {
    for ( n = 0; n < 6; n++ )
    { 
      rtc_data[n] = I2C_MasterReadByte( I2C_1_ACK_DATA );   //read byte, send ACK MSB
    }  
    rtc_data[6] = I2C_MasterReadByte( I2C_1_NAK_DATA );   //read byte, send ACK LSB
     
  }  
  i2c_error = I2C_1_MasterSendStop();   // send stop, since we want to read from device
  CyDelayUs ( 100 );
  
  CyExitCriticalSection(intState);
  return i2c_error; 
}
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
uint32_t decode_date ( date_time_t date ) {
  
  float counter;
  
  counter  =  ( (float)date.imonth * 30.42 ) + (float)date.iday  + ( (float)date.iyear * 365.25 );  
                   
 
  return( (uint32_t) counter );  
}
/*******************************************************************************
* Function Name: bcdToDecimal
********************************************************************************
* Summary: Convert a BCD number to its decimal equivalent
*  
*
* Parameters: 8 bit BCD value
*  None
*
* Return: the 8 bit decimal
*  None
*
*******************************************************************************/
uint8 bcdToDecimal ( uint8 value ) {
 uint8 valuem = (((value & 0xF0)/16) * 10) + ( value & 0x0F );
 return valuem; 
}
/******************************************************************************
 *
 *  Name: read_I2C_RTC
 *
 *  PARAMETERS: pointer to date/time struct
 *
 *  DESCRIPTION: read the RX-8035 RTC and return the time/date
 *               
 *            
 *  RETURNS: error and data in pointer 
 *
 *****************************************************************************/ 
void read_RTC ( date_time_t* d_time ) {  //reads time and date from RTC
  uint8 error;
  uint8 rtc[16];
  // get the time and date into a 32 bit union
  error = readPointerValue ( rtc );
  if ( error == 0 )
  {
    d_time->ihour = (bcdToDecimal (rtc[2]& 0x3F)) ; // mask off 24/12 flag
    d_time->iyear = bcdToDecimal (rtc[6]) + 2000;
    d_time->imonth =  bcdToDecimal (rtc[5]);
    d_time->iday = bcdToDecimal (rtc[4]);
    d_time->iminute =  bcdToDecimal(rtc[1]);
    d_time->isecond = bcdToDecimal (rtc[0]);
  }
  else
    {
    d_time->ihour = 12;
    d_time->iyear = 00;
    d_time->imonth =  1;
    d_time->iday = 1;
    d_time->iminute =  0;
    d_time->isecond = 0;
  }
} 
/******************************************************************************
 *
 *  Name:  ( date_time_t date )
 *
 *  PARAMETERS: date/time struct and dest. address of RAM
 *
 *  DESCRIPTION: Takes the integer values from the struct and makes a string
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void getTimeDateStr ( date_time_t date, char * d_t_str ) {

 char AM_PM[] = "AM";

 // If in AM PM mode convert to this mode
 if ( OFF == Features.flag_24HourMode )
 {
  if ( date.ihour > 12 )
   {
      date.ihour -= 12;
      AM_PM[0] = 'P';    
   }
   else if ( date.ihour == 12 )   // Noon
   {
     AM_PM[0] = 'P';    
   }
   else if ( date.ihour == 0 )   // Midnight
   {
     date.ihour = 12;
   }
 }
 else
 {
   AM_PM[0] = 0; // no AM or PM displayed
   if ( date.ihour == 0 )   // Midnight
   {
     date.ihour = 24;
   } 
 }

 if ( Features.day_month_year_flag == OFF )
 {
   sprintf( d_t_str,"%02u/%02u/%04u %02u:%02u%s", date.imonth, date.iday,date.iyear, 
                                      date.ihour,date.iminute, AM_PM );
 }
 else // dd/mm/yy format
 {
  sprintf( d_t_str,"%02u/%02u/%04u %02u:%02u%s", date.iday,date.imonth, date.iyear, 
                                      date.ihour,date.iminute, AM_PM );
 }
 
}
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 
void  convertRTCtoAlfatTime ( date_time_t date ) {
 if ( date.iyear > 1979 ) {
    time_date.dateTimeBits.Years =  date.iyear - 1980;
 }
 else {
   time_date.dateTimeBits.Years = 0;
 }    
 time_date.dateTimeBits.Day     = date.iday;
 time_date.dateTimeBits.Month   = date.imonth;
 time_date.dateTimeBits.Hour    = date.ihour;
 time_date.dateTimeBits.Minute  = date.iminute; 
 time_date.dateTimeBits.Second2 = date.isecond/2;
}
/******************************************************************************
 *
 *  Name: printTimeDate ( date_time_t date )
 *
 *  PARAMETERS: date/time struct
 *
 *  DESCRIPTION: Prints the MONTH:DAY:YEAR HOUR:MINUTE to LCD
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void printTimeDate ( date_time_t date )
{
 char lcd[21];
 
 getTimeDateStr ( date, lcd );
      
 LCD_print ( lcd );
}
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

Bool date_check(uint8_t month_temp, uint8_t day_temp)  // verifies that the entered date is valid
{
  Bool date_correct = 1;


  if ( ( ( (month_temp == 1 ) || (month_temp == 3) || (month_temp == 5) || (month_temp == 7) || (month_temp == 8) 
          || (month_temp == 10) || (month_temp == 12)) && day_temp > 31) )
  {
    date_correct = 0;                                                               //not feb, apr, jun, sep, or nov and date not valid  
  }  
  else if ( ( (month_temp == 4) || (month_temp == 6) || (month_temp == 9) || (month_temp == 11)) && (day_temp > 30))
  {
    date_correct = 0;                                                               //feb, apr, jun, sep, or nov and date not valid  
  }
  //february and date not valid
  else if ( (month_temp == 2) && (day_temp > 29 ) )
  {
    date_correct = 0;
  }
  else if ( month_temp > 12)                       
  {
    date_correct = 0;
  }
  
  if(!date_correct)
  {
    enter_valid_text(0);  //TEXT// display "  Please Enter"\n   Valid Date" LINE2,3    
    return(0);
  }  
return(1);  
}

Bool time_check(uint8_t hr)   // verifies that the entered date is valid
{      
   if(hr > 12)
   {
     enter_valid_text(1); //TEXT// display "    Please Enter\n     Valie Time" LINE2,3
     return (0);
   }
   return (1);
}
uint8 DecimalToBCD ( uint8 value )
{
 uint8 valuem = value/10 ;
 
 valuem *= 16; 
 valuem += (value % 10 ); 
  
 return valuem; 
}

void RTC_WriteTime ( date_time_t* d_time )  //write time and date from RTC
{
  uint8 error;
  uint8 sec, min, hr, day, month;
  uint16 yr;
  uint8 lsb_yr;
  
  // Put info into BCD format needed for RTC
  sec = DecimalToBCD (d_time->isecond);
  min = DecimalToBCD (d_time->iminute);
  hr =  DecimalToBCD ( d_time->ihour );
  hr |= 0x80;                              // 24 hour mode
  day = DecimalToBCD ( d_time->iday );
  month = DecimalToBCD ( d_time->imonth );
  
//  if ( d_time->Year < 2000 ) // date time sent from PC cannot be < 2022
//  {
//    yr = 0x2000;
//  }
//  else
    if ( d_time->iyear > 2099 ) { yr = 0x2100; }
    else { yr = 0x2000; }
    lsb_yr = d_time->iyear - 2000;
    yr += DecimalToBCD ( lsb_yr );
  
  error = writePointerValue (0x00, sec);    //sec
  if(error == 0) {error = writePointerValue(0x10, min); }
  if(error == 0) {error = writePointerValue(0x20, hr); }  
  if(error == 0) {error = writePointerValue(0x40, day); } 
  if(error == 0) {error = writePointerValue(0x50, month); }
  if(error == 0) {error = writePointerValue(0x60, yr); }
}


 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void enterTime (void)  // leads user through process to set the time and date
{
  float month, day, year, hour, minute;  
  enum buttons  button;
  uint16_t min, hr, da, mon, yr;
  char number_ptr[20] =  NULL_NAME_STRING ;
  CURSOR_OFF;
  CLEAR_DISP;
  // Display the  Date
  read_RTC( &date_time_g ); 
  gotoloc ( 0,0 );
  printTimeDate ( date_time_g ) ;
  
  CyDelay ( 1000 );

  
  //"Change Value?");
  //"Press YES or NO"); 
  //"ESC to Exit"); 
  change_val_yesno_text();
  while(1)
  {
   button = getKey( TIME_DELAY_MAX);
   if((button == ESC) || (button == YES) || (button == NO))
   {
     break;
   }  
  }    
  if(button != YES)   // NO or ESC was pressed, exit to menu
  {
   return;
  }  

   CURSOR_ON;
   // Enter Value for
   // Month:       
   // <Enter> to Accept
   // <ESC> to Exit
   while ( 1 )
   {
     s_EnterValueMonth ();
     month = 1;
     sprintf( number_ptr, "%u", (uint8)month);
     month = enterNumber ( number_ptr, LINE2 + 7, 2 );
    
    
     if (  999999 == (uint32)month )
    {
      return; 
    } 
    if ( ( month >= 1 ) &&  (month <= 12 ))
      break;
   } 
  
   while ( 1 )
   {
    // Enter Value for
    // Day:       
    // <Enter> to Accept
    // <ESC> to Exit
    s_EnterValueDay ();
    day = 1;
    sprintf( number_ptr, "%u", (uint8)day);
    day = enterNumber ( number_ptr, LINE2 + 7, 2 );
    if ( 999999 == (uint32)day )
    {
     return; 
    }
      
    if ( ( day >= 1 ) && ( day  <= 31 ) )
    {
      if ( ( month == 2 ) && ( day <= 29 ) )
      {
        break;
      }  
      else if ( ( month == 1 ) || ( month == 3 )  || ( month == 5 )  || ( month == 7 )  || ( month == 9 )  || ( month == 10 )  || ( month == 12 )  )
      {
        if ( day <= 31 )
        { 
          break;
        }  
      }
      else if ( day <= 30 )
      {
        break;
      }  
    }
  } // end day while loop    
  
  // Get Year
  while ( 1 )
  {
    // Enter Value for
    // Year:       
    // <Enter> to Accept
    // <ESC> to Exit
    s_EnterValueYear();
    year = 2025;
    //year  = getFloatInput ( 1, 6, 4, year, 0, 0 );          // line,loc, max_len, init value, decimal places
    sprintf( number_ptr, "%u", (uint16)year);
    year = enterNumber ( number_ptr, LINE2 + 6, 4 );
    
    
    if ( 999999 == (uint32)year )
    {
     return; 
    }
    if ( ( year >= 2025) && ( year < 2100 ) )
    {
      break;
    }
  }  

    // Get Hour
  while ( 1 )
  {
    // Enter Value for
    // Hour:       
    // <Enter> to Accept
    // <ESC> to Exit
    s_EnterValueHour ();
    hour = 12;
   // hour  = getFloatInput ( 1, 6, 2, hour, 0, 0 );          // line,loc, max_len, init value, decimal places
     sprintf( number_ptr, "%u", (uint8)hour);
     hour = enterNumber ( number_ptr, LINE2 + 6, 2 );
    
    if ( 999999 == (uint32)hour )
    {
     return; 
    }
    
    if ( OFF == Features.flag_24HourMode )
    {
      if ( ( hour >= 1) && ( hour <= 12 ) )
      {
        break;
      }
    }
    else
    {
      if ( ( hour >= 1) && ( hour <= 24 ) )
      {
        break;
      }
    }
  }  
  
  
  // Get minute
  while ( 1 )
  {
    // Enter Value for
    // minute:       
    // <Enter> to Accept
    // <ESC> to Exit
    s_EnterValueMinutes();
    minute = 0;
   // minute  = getFloatInput ( 1, 9, 2, minute, 0, 0 );          // line,loc, max_len, init value, decimal places
    sprintf( number_ptr, "%u", (uint8)minute);
    minute = enterNumber ( number_ptr, LINE2 + 9, 2 );
    if ( 999999 == (uint32)minute )
    {
     return; 
    }
    if ( ( minute >= 0) && ( minute < 60 ) )
    {
      break;
    }
  }  

  // If in AM/PM mode change to 24 hour mode to set RTC
  if ( OFF == Features.flag_24HourMode )
  {
    
    //("Select"); 
    //("1. AM");       
    //("2. PM");
    selectAM_PM();
        
    while(1)
    {
      button = getKey( TIME_DELAY_MAX);
     
      if((button == ONE) || (button == TWO) || (button == ESC))
      {
        break;
      }    
    }  

    if ( button == TWO ) // 
    {
      if ( hour == 12 ) // This is noon, 12 PM
      { 
        hour = 12; 
      }
      else {
        hour += 12 ; 
      }   
    } 
    else 
    {
      if ( hour == 12 ) // This is 12 AM
      { 
        hour = 0; 
      }
    }
  }
  else
  {
    if ( hour == 24 )
    {
      hour = 0;
    }
  }
  
  hr =  (uint16) hour;
  hr = HexToBCD ( hr ) | 0x80; // 24 hour mode
  
  // Put info into BCD format needed for RTC
  min = (uint16)minute;
  min = HexToBCD ( min );

  da = (uint16) day;
  da = HexToBCD ( da );
  year = year - 2000;
  yr = (uint16)year;
  yr = HexToBCD ( yr );
  mon = (uint16)month;
  mon = HexToBCD ( mon );
 

  uint8   error =  writePointerValue ( 0x00, 0 );    //sec
    
  if ( error == 0 )
  {
    error =  writePointerValue ( 0x10, min );  // minute
  }
  
  if ( error == 0 )
  {        
    error =  writePointerValue ( 0x20, hr );  // hour
  }  
  if ( error == 0 )
  {
    error =  writePointerValue  ( 0x40, da );  // day
  }  
  
  if ( error == 0 )
  {
    error =  writePointerValue  ( 0x50, mon );  // month
  }  
  
  if ( error == 0 )
  {
   error =  writePointerValue  ( 0x60, yr  );  // year
  }

  if ( error == 0 )
  {       
    CLEAR_DISP;
    read_RTC( &date_time_g ); 
    gotoloc(2,0);
    printTimeDate ( date_time_g );
    CyDelay ( 1500 );
  }  
  else
  {
   sClockSetError ( );
  }

  
}


void enterTimeDate ( void )
{
  enum buttons key;
  cursor_off();
  // 1. Change Time/Date
  // 2. Change M/D Order
  // 3. AM-PM or 24 Hour
  // <ESC> to Exit

  while ( 1 )
  {
    sSelectTimeItem();
     key = getNewKey ( TIME_DELAY_MAX );
   if ( key == ESC )    return;
   else if ( key == ONE ) 
   {
     //enterTimeRTCI2C();
     enterTime();
     clearlcd();
     
   }
   else if ( key == TWO )
   {
    changeMonthDayOrder ();

   }
   else if ( key == THREE )
   {
     changeAMPM_24Hour ( );
   }
  }

}

/* [] END OF FILE */
