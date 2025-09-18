/**************************   ***************************************************************** ***************************/
//
// Title:             
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:    Computer_Interface .c
// Author:      
//
// Processor:    
// Clock:	    z	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"

#include "Globals.h"
#include "DataStructs.h"
#include "conversions.h"
#include "UARTS.h"
#include "LCD_drivers.h"


/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
                        
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
 #define COMM_DELAY()   delay_ms(100) 

void UART2_DOUBLE_FLOAT(DOUBLE_FLOAT value, char* str){
    sprintf( lcdstr, str, value);
    UART2_PutString ( (const char*)lcdstr ) ; //RGS not sure Uart2_PutString is wanted here, perhaps _UART2_PRINT?
    COMM_DELAY();
}
void UART2_UINT16(uint16_t value, char* str){
    sprintf( lcdstr, str, value);
    UART2_PutString ( (const char*)lcdstr ) ; //RGS not sure Uart2_PutString is wanted here, perhaps _UART2_PRINT?
    COMM_DELAY();
}
#if 0
void UART2_PrintThinLayerValues(){
    DOUBLE_FLOAT  value;
        // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.P1, &value ); 
    UART2_DOUBLE_FLOAT(value,"P1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.Q1, &value ); 
    UART2_DOUBLE_FLOAT(value,"Q1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.R1, &value ); 
    UART2_DOUBLE_FLOAT(value,"R1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.P2, &value ); 
    UART2_DOUBLE_FLOAT(value,"P2 = %.5Lf\n\r");
    NV_EE_RD ( Constants.Q2, &value ); 
    UART2_DOUBLE_FLOAT(value,"Q2 = %.5Lf\n\r");
    NV_EE_RD ( Constants.R2, &value ); 
    UART2_DOUBLE_FLOAT(value,"R2 = %.5Lf\n\r");
    NV_EE_RD ( Constants.A1, &value ); 
    UART2_DOUBLE_FLOAT(value,"A1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.B1, &value ); 
    UART2_DOUBLE_FLOAT(value,"B1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.C1, &value ); 
    UART2_DOUBLE_FLOAT(value,"C1 = %.5Lf\n\r");
    NV_EE_RD ( Constants.A2, &value ); 
    UART2_DOUBLE_FLOAT(value,"A2 = %.5Lf\n\r");
    NV_EE_RD ( Constants.B2, &value ); 
    UART2_DOUBLE_FLOAT(value,"B2 = %.5Lf\n\r");
    NV_EE_RD ( Constants.C2, &value ); 
    UART2_DOUBLE_FLOAT(value,"C2 = %.5Lf\n\r");
}
void UART2_PrintMoistValues() {
    DOUBLE_FLOAT  value;
        // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.E_MOIST_CONST, &value );   
    UART2_DOUBLE_FLOAT(value,"E = %.5Lf\n\r");
    NV_EE_RD ( Constants.F_MOIST_CONST, &value ); 
    UART2_DOUBLE_FLOAT(value,"F = %.5Lf\n\r");
}
void UART2_PrintDepth(int depth){
    DOUBLE_FLOAT  value;
    NV_EE_RD ( Constants.DEPTHS[depth].A, &value);
    UART2_DOUBLE_FLOAT(value,"\n\rA = %.5Lf\n\r");
    NV_EE_RD ( Constants.DEPTHS[depth].B, &value );
    UART2_DOUBLE_FLOAT(value,"B = %.5Lf\n\r");
    NV_EE_RD ( Constants.DEPTHS[depth].C, &value );
    UART2_DOUBLE_FLOAT(value,"C = %.5Lf\n\r");
}
void UART2_PrintStds(){
    uint16_t std_value; 
    // get the value from EEPROM (not ram)
    NV_EE_RD (Constants.DENS_1_CAL_STD, &std_value );
    UART2_UINT16(std_value, "DEN 1 = %u\n\r");
    NV_EE_RD (Constants.DENS_2_CAL_STD, &std_value );     
    UART2_UINT16(std_value, "DEN 2 = %u\n\r");
    NV_EE_RD (Constants.MOIST_CAL_STD, &std_value );
    UART2_UINT16(std_value, "MOIST = %u\n\r");
}
void UART2_PrintDate(){
    date_time_t cal_date;
    cal_date = NV_RAM_MEMBER_RD(Constants.CAL_DATE);
    UART2_UINT16(cal_date.iyear, "Year = %u\n\r");
    sprintf( lcdstr, "Month = %u\n\r", cal_date.imonth );
    UART2_PutString ( (const char*)lcdstr ) ; 
    CommDelay();
    sprintf( lcdstr, "Day = %u\n\r", cal_date.iday );
    UART2_PutString ( (const char*)lcdstr ) ; 
    CommDelay();
}
Bool WaitYN(){
    UART2_PutString ("Are these values correct? Y or N\n\n\r"); 
    CommDelay();
    if ( waitc_RX(&s_Computer_RX, 600000) == 'Y')
    {
        return TRUE;
    }
    return FALSE;
}
/////COMPUTER INTERFACE FUNCTIONS ////////////////////////////////////////////////////////
void get_const_computer(void)  // receives constants from computer
{
  uint8_t i, depth, depth_temp = 0;
  date_time_t cal_date;
  char space[] = ",";
  char cal_values[256]; // 50 / 12 floats leaves 3 significant digits for each float, unacceptable, changed it to 256
  UART2_Start();
  isrUART2_Enable();
  UART2_EnableTxInt();
  while(1) {                             // prompt for backscatter constants
    clearComputerRXBuffer();
    UART2_ClearTxBuffer();    
    depth = BS;                        // B_SCAT_A location in memory    
    COMM_DELAY(); 
    UART2_PutString ("Enter the Backscatter values A, B, C and <enter>\n\r") ; // Blocks until all data is placed in TX buffer
    CommDelay();
    gets_comp(cal_values);  // read values from computer
    // Parse the input for A,B,C values and Save BS constants
    NV_MEMBER_STORE(Constants.DEPTHS[depth].A, atof(strtok(cal_values,space)));
    NV_MEMBER_STORE(Constants.DEPTHS[depth].B, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.DEPTHS[depth].C, atof(strtok(0,space)));
    // get the value from EEPROM (not ram)
    UART2_PrintDepth(depth);
    if(WaitYN() == TRUE) { break; }
  }                                                    //end prompt for backscatter constants
  i=1;
  for ( depth = DEPTH_2_INCH; depth < MAX_DEPTHS; depth++) {  // prompt for remaining 12 depth constants
    i++;
    clearComputerRXBuffer();
    sprintf ( lcdstr, "Enter Depth %d values A, B, C, or S(skip) and <enter>\n\r", i );
    UART2_PutString ( (const char *)lcdstr ) ; 
    CommDelay();   
    gets_comp(cal_values);
    if(toupper((int)cal_values[0]) == 'S') {
        depth += 12;
    }    
    else {
      depth_temp = depth;
      while(1) {
        // Parse the input for A,B,C values Save depth constants
        NV_MEMBER_STORE(Constants.DEPTHS[depth].A, atof(strtok(cal_values,space)));
        NV_MEMBER_STORE(Constants.DEPTHS[depth].B, atof(strtok(0,space)));
        NV_MEMBER_STORE(Constants.DEPTHS[depth].C, atof(strtok(0,space)));         
        UART2_PrintDepth(depth);
        if(WaitYN() == TRUE) { break; }
        else
        {
          depth = depth_temp;
          clearComputerRXBuffer();                  
          sprintf ( lcdstr, "Enter Depth %d values A, B, C and <enter>\n\r", i);
          UART2_PutString ( (const char *)lcdstr ) ; 
          CommDelay();        
          gets_comp(cal_values);
        }
      }
    }
  }    //end prompt for remaining 12 depth constants
  while(1) { // "Enter Moisture values E, F  and <enter>\n\r");
    clearComputerRXBuffer();
    UART2_PutString ( "Enter Moisture values E, F  and <enter>\n\r"); 
    CommDelay();
    gets_comp(cal_values);
    NV_MEMBER_STORE(Constants.E_MOIST_CONST, atof(strtok(cal_values,space)));   // Store E value
    NV_MEMBER_STORE(Constants.F_MOIST_CONST, atof(strtok(0,space)));            // Store F value
    UART2_PrintMoistValues();
    if(WaitYN() == TRUE) { break; }
  }                                                    //end "Enter Moisture values E, F  and <enter>\n\r");  
  while(1) { // get thin layer values
    clearComputerRXBuffer();
    UART2_PutString ( "Enter Thin Layer Values P1, Q1, R1 ,P2 ,Q2, R2, A1, B1, C1, A2, B2, C2 <enter>\n\r" ) ; 
    CommDelay();
    gets_comp(cal_values); //rx buffer is 256 bytes, 12 doubles gives 20 chars per double
    // unfortunately cal_values was 50 bytes, not large enough, RGS changed it to 256
    NV_MEMBER_STORE(Constants.P1, atof(strtok(cal_values,space)));             
    NV_MEMBER_STORE(Constants.Q1, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.R1, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.P2, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.Q2, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.R2, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.A1, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.B1, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.C1, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.A2, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.B2, atof(strtok(0,space)));
    NV_MEMBER_STORE(Constants.C2, atof(strtok(0,space)));
    UART2_PrintThinLayerValues();
    if(WaitYN() == TRUE) { break; }
  }                                                    //end get thin layer values
  while(1) { // get standard counts
    clearComputerRXBuffer();
    UART2_PutString ( "Enter Standard Counts Values DEN1, DEN2, MOIST and <enter>\n\r" ) ;
    CommDelay();
    gets_comp(cal_values);
    NV_MEMBER_STORE(Constants.DENS_1_CAL_STD, atoi(strtok(cal_values,space)));   // Store Density 1 std
    NV_MEMBER_STORE(Constants.DENS_2_CAL_STD, atoi(strtok(0,space)));            // Store Density 2 std
    NV_MEMBER_STORE(Constants.MOIST_CAL_STD, atoi(strtok(0,space)));          // Store Moisture Std.
    UART2_PrintStds();
    if(WaitYN() == TRUE) { break; }
  }                                                    //end get standard counts
  while(1) { // get cal date
    clearComputerRXBuffer();
    UART2_PutString ( "Enter Cal. Year, Month, Day and <enter>\n\r" ) ; 
    CommDelay();
    gets_comp(cal_values);
    cal_date.iyear  = atoi(strtok(cal_values,space)); // set the date and time in the date struct
    cal_date.imonth = atoi(strtok(0,space));
    cal_date.iday   = atoi(strtok(0,space));
    cal_date.isecond = 0;
    cal_date.ihour   = 12;
    cal_date.iminute = 0;
    //write the date/time to memory
    NV_MEMBER_STORE(Constants.CAL_DATE, cal_date);
    UART2_PrintDate();    
    if(WaitYN() == TRUE) { break; }
  }                                                    //end get cal date
  // Send the 12 depths A,B,C values and the E and F values, read memory locations and send via serial port
  for ( depth = BS; depth < MAX_DEPTHS - 1; depth++) { //RGS Added - 1 to avoid returning 13 depths
    UART2_PrintDepth(depth);
  }
    UART2_PrintMoistValues();
    UART2_PrintThinLayerValues();
    UART2_PrintStds();
    UART2_PrintDate();
    update_valid_depths();    // update valid depths
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
  
void wait_for_serial(void)  // if computer is connected, wait for input from the computer
{  
    char command;
    Bool updateBle = FALSE;
    initUARTS();      // moved to main.c to support the Plus depth sensor comms.
    UART2_Start();
    isrUART2_Enable();
    clearComputerRXBuffer();
    while ( IS_ACTICE_RS232_DETECTED()  )
    {
        UART2_PutString ( "Enter 'L'(load cal data), 'A'(access serial #) or 'E'(exit)\n\r" ) ; 
        //while ( UART2_GetTxBufferSize() != 0 );  // not necessary
        delay_ms(100);
        while ( ( getRXNumEntries ( &s_Computer_RX) == 0 ) && ( IS_ACTICE_RS232_DETECTED()  ) )
        {
            delay_us(10);
        }
        delay_ms (1);
        if ( getRXNumEntries(&s_Computer_RX) > 0 )  
        { 
            command =  getcRXBuffer(&s_Computer_RX);
            if(command == 'L')                       // load constants Ai, Bi, Ci,...E, F
            {
                updateBle = TRUE;
                get_const_computer();        
            }    
            else if (command == 'A')               //   access serial #
            {
                updateBle = TRUE;
                serial_number(1);     
            }    
            else if (command == 'E')               //   exit            
            {
                break;            
            }    
        } 
    }
    if(updateBle == TRUE) { SendBleConstants(); }
}
#endif
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
  
void gets_comp(char* s)  // reads a string from UART2 XMIT = PIN_F5, RCV = PIN_F4 
{
  #define MAX_RX_TIME_ERROR   0x1122
  uint16_t len = 0, c;
  do
  {
    c = waitc_RX ( &s_Computer_RX , 1000 );
    if ( c != MAX_RX_TIME_ERROR )
    {
     if(c>=' ' && c<='~')
     {
       s[len++]=c;
     }  
    }
  } while(c!=13);
  
  s[len]='\0';
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
  
void puts_printer ( char *s )  //print the string to attached printer
{
  uint8_t c = 0;  
  char data_byte ;
  CLEAR_BUFFER2;
  
  ok_2_print = TRUE;
  
  do
  {
    if ( ok_2_print )
    {
      data_byte  = *(s + c++);   
      UART2_PutChar( data_byte );  //send character to printer up to and including CR
      while ( UART2_GetTxBufferSize() != 0 )   
      {
        CyDelayUs ( 100 );
      }  
    }    
   else
   {
      delay_ms(10);
   }    
  
  }while ( data_byte != '\r' );      
 
 
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
  
void hold_buzzer(void)
{
  while ( BUZZER_Read() )
  {
    delay_us(10);
  }  
}
