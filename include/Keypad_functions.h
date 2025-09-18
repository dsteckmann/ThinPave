/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Keypad_functions.h
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *
 *---------------------------------------------------------------------------*/

/*  If we haven't included this file already.... */
#ifndef KEYPAD_FUNCTIONS_H
#define KEYPAD_FUNCTIONS_H
  
#include "Globals.h"

/*----------------------------------------------------------------------------*/
/*-------------------------[   Global MACROS  ]-------------------------------*/
/*----------------------------------------------------------------------------*/

#define     KEYSCAN_DISABLE()    isrTIMER_1_Disable()
#define     KEYSCAN_ENABLE()     isrTIMER_1_Enable() 

    
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Constants   ]---------------------------*/
/*----------------------------------------------------------------------------*/

  
enum buttons { ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,UP,DOWN,PERIOD,CE, 
               ESC,PROJ,MENU,STAND,TIME,MA,STORE,ENTER,YES,NO,THICKNESS,EMPTY };

#define     DFLT    99
#define     deg     0xDF   

#define TIME_DELAY_MAX  0xFFFFFFFF
//REGISTERS*********************************************************************

//******************************************************************************
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Variables   ]---------------------------*/
/*----------------------------------------------------------------------------*/
extern uint8_t key_char;
//extern enum buttons button;

/*----------------------------------------------------------------------------*/
/*---------------------[   Global Function Prototypes   ]---------------------*/
/*----------------------------------------------------------------------------*/
extern void scan_keypad( void );
extern  enum buttons  getKey ( uint32_t time_delay_ms );
extern  enum buttons  getNewKey ( uint32_t time_delay_ms );
extern enum buttons getLastKey( void );
extern int32 enter_number(char *temp_str, int16_t position, int16_t length, int16_t sign);
extern int32 enter_number_std(char *temp_str, int position, int length, int16_t sign);
extern void enter_name(char *file, int position);
extern void discover_secret_characters(void);
extern void wait_for_key_release ( void );
extern double enterNumber ( char *temp_str, int position, int length );
#endif /* endif !KEYPAD_FUNCTIONS_H for "if we haven't included this file already..."    */


