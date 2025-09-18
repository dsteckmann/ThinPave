/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  project.h
 *  Originating Author:  
 *       Creation Date:  
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  
 *
 *---------------------------------------------------------------------------*/

/*  If we haven't included this file already.... */
#ifndef ELITE_H
#define elite_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define BATTERY_VOLTAGE_DIVIDER .177    // Resistor Divider Voltage 
#define PROBE_GM1_COUNT 0
#define PROBE_GM2_COUNT 1
  
#define PROJECTS_ENABLED 0  
  
#define delay_us(x)     CyDelayUs(x)
#define delay_ms(x)     CyDelay(x)


#define BIT_SET(value,bity)  (value |= (1 << bity))
#define BIT_CLR(value,bity)  (value &= (~(1 << bity)))

// check if MASK bit is set in value. Return 1 if mask bit is set
#define bit_test(value,bity)      ( ( value & ((UINT32)1<<bity) )? 1: 0 )

#define output_low(x)  ( x ## _Write(0) )

#define output_high(x)  ( x ## _Write(1) )

#define Global_ID()  CyGlobalIntDisable
#define Global_IE()  CyGlobalIntEnable
             

// Returns 1 if CHARGER is plugged into jack
#define IS_CHARGER_DETECTED()         ! CHARGER_DETECT_Read()


// Returns 1 if RS232 cable is connected to serial port
#define IS_ACTICE_RS232_DETECTED()     RS232_CABLE_DETECT_Read()


// Turn on Keypad backlight
#define KEY_B_LIGHT_ENABLE()     LCD_BK_EN_Write(1) 

// Turn off keypad backlight
#define KEY_B_LIGHT_DISABLE()    LCD_BK_EN_Write(0) 


// Enable Power
#define MICRO_POWER_ENABLE()     SHUT_OFF_IO_Write(1)

// Disable Power
#define MICRO_POWER_DISABLE()    SHUT_OFF_IO_Write(0)

#endif



