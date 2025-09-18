/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Interrupts.c
 *  Originating Author:  Dirk Steckmann
 *       Creation Date:  12/21/2016 
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-------------------------[   Include Files   ]------------------------------*/
/*----------------------------------------------------------------------------*/

#include  "Globals.h"
#include  "DataStructs.h"
#include  "LCD_drivers.h"
#include  "Keypad_functions.h"
#include  "Batteries.h"
#include  "prompts.h"
#include  "Utilities.h"
/*----------------------------------------------------------------------------*/
/*------------------------[   Module Global Variables   ]---------------------*/
/*----------------------------------------------------------------------------*/
static uint16_t bat_timer;

/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Functions   ]---------------------------*/
/*----------------------------------------------------------------------------*/

CY_ISR_PROTO ( BlueToothtRx_ISR ) ;

//////////////////////////////////////////INTERRUPTS////////////////////////////
// keypad timer, timer for battery monitor
/*
 *  FUNCTION: T1Interrupt
 *
 *  PARAMETERS: 100ms interrrupt period
 *
 *  DESCRIPTION: Scan the Keypad, control backlight on time, check battery voltages
 *               
 *               
 *  RETURNS: None
 *
 */
 CY_ISR (T1Interrupt )
{    
    
    scan_keys();    
   
    CHARGER_ON_IO_Write ( 0 ); // 0 is on
  
    
    if ( Controls.LCD_light && Flags.button_pressed )  // turn on LCD backlight and reset timer      
    {
       KEY_B_LIGHT_ENABLE(); 		 // turn on keyboard backlight
      LCD_timer = 0;
    }
    
    if( Controls.LCD_light && !Flags.button_pressed)   // disable LCD backlight if keys are not pressed within approx. 25 sec.
    {
      if( ++LCD_timer >= LCD_timeout )                 
      {             
        // turn OFF keyboard backlight
        KEY_B_LIGHT_DISABLE(); 
      }
    }
    
    if (  Spec_flags.auto_turn_off && (shutdown_timer > g_turn_off_cnts) && !Spec_flags.self_test )     // 36000 = 1 hr
    {
      Flags.stand_flag = FALSE;
      Controls.shut_dwn = TRUE;                           // set when "auto" shut off is enabled   
      
      // update status of shutdown, 1 = auto shutdown 
      NV_MEMBER_STORE(OFF_MODE,1);
          
      // write all flag settings to memory      
      NV_MEMBER_STORE( OFFSET_SETTINGS,Offsets);
       
      //NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
         
      NV_MEMBER_STORE( FLAG_SETTINGS, Flags );          
     
      Controls_U.controls_bitfield = &Controls;     
      NV_MEMBER_STORE( CONTROL_SETTINGS, Controls );     
	    Global_ID();                                                 // shutdown all competition.

	    shutdown_inactivity_text_text();
       // flash the keyboard backlight
      KEY_B_LIGHT_ENABLE(); 		                    // turn on keyboard backlight                  
      delay_ms(1000);    
     	KEY_B_LIGHT_DISABLE();                       // turn OFF keyboard backlight
      delay_ms(1000);
      Global_ID();
      
      MICRO_POWER_DISABLE();
           
      while(1) 
      {
       delay_ms(100);  
       MICRO_POWER_DISABLE();
      };    
      
   }
   else	if ( (CHARGER_DETECT_Read()==CHARGER_OFF) && (shutdown_timer <= g_turn_off_cnts) )
   { 
     shutdown_timer+=1;   // condition not satisfied yet, and no charger detected.
	 } 


 if((++bat_timer >= 60) && (Flags.bat_volt == 0) )       // check battery every 6 second
 {
   bat_timer = 0;						              // reset timer even if we go into shutdown.
   checkBatteriesToShutDown();            // check battery voltages.
 }  


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
void pulseBuzzer ( void ) {
  output_high(BUZZER);      
  TIMER_5_Enable();
  COUNTER_5_RESET_Write ( 1 );
  CyDelayUs ( 1 );
  COUNTER_5_RESET_Write ( 0 );
  // isr will run off timer and buzzer.
}  
/*******************************************************************************
 *  PARAMETERS: 
 *  DESCRIPTION:
 *  RETURNS:
 ******************************************************************************/
// timer turns buzzer OFF on interrupt
CY_ISR ( T5Interrupt ) {
  output_low( BUZZER );                                 // turn off buzzer   

  if(!Flags.button_pressed)
   { 
     Controls.buzz_enable = TRUE;
   }  
 
  TIMER_5_Stop();         
}
/*******************************************************************************
 *  PARAMETERS: 
 *  DESCRIPTION:
 *  RETURNS:
 ******************************************************************************/
// timer turns buzzer OFF on interrupt
CY_ISR ( ISR_KILL ) {

 isr_ON_OFF_ClearPending();
 ON_OFF_INT_IO_ClearInterrupt();

 isrTIMER_1_Disable();
 shut_down_text();
       
}
/*******************************************************************************
 *  PARAMETERS: 
 *  DESCRIPTION:
 *  RETURNS:
 ******************************************************************************/
CY_ISR ( MS_TIMER_ISR ) { msTimer++; }
/*******************************************************************************
 *  DESCRIPTION: Receives data packets from the BLE module
 ******************************************************************************/



/*******************************************************************************
 *  PARAMETERS: 
 *  DESCRIPTION:
 *  RETURNS:
 ******************************************************************************/
void initInterrupts  ( void ) {
  TIMER_1_Start();
  isrTIMER_1_StartEx ( T1Interrupt );
  isrTIMER_1_Disable();
  // The buzzer
  TIMER_5_Start();
  isrTIMER_5_StartEx ( T5Interrupt );
  isrTIMER_5_Enable();
  TIMER_5_Stop();
  // The POWER KILL ISR
 isr_ON_OFF_StartEx ( ISR_KILL );

 ADC_DelSig_IRQ_SetPriority ( 6 );
 AlfatRxtInt_SetPriority ( 5 );

 BlueToothtRxInt_SetPriority ( 4 );
 BlueToothtRxInt_StartEx(BlueToothtRx_ISR);

 isr_ms_Timer_StartEx ( MS_TIMER_ISR );

 isr_ms_Timer_SetPriority( 5 );

 isr_ON_OFF_SetPriority ( 3 );
 isrGPS_RX_SetPriority ( 6 );
 isrOneShot_SetPriority (5 );

 isrTIMER_1_SetPriority ( 7 );  // must be lowest priority since it takes a long time to process
 isrTIMER_5_SetPriority ( 5 );
 isrUART2_SetPriority ( 5 );
 // AlfatUart_TXInternalInterrupt_SetPriority ( 5 );
 /* Enable global interrupts */
 CyGlobalIntEnable;
}
/*******************************************************************************
 *  PARAMETERS: CHECK_KEYSCAN_ENABLE()
 *  DESCRIPTION: returns 1 if the keyscan timer is enabled
 *
 *  RETURNS: 1 if isr enabled, 0 is disabled
 *
 ******************************************************************************/
uint8 CHECK_KEYSCAN_ENABLE ( void)
{
  return isrTIMER_1_GetState();
}
