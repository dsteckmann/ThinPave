/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Utilities.h
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
#ifndef BATTERIES_H
#define BATTERIES_H

//#include <project.h> 
#include "Globals.h"  
  
extern void monitorBatteries ( void );
extern void checkAlkalines (void);
extern float readBatteryVoltage ( uint8 battery_type );
extern uint8 checkBatterySelected ( void );
extern void checkBatteriesToShutDown ( void );
extern BOOL checkNICADVoltageWarning (void);
extern void batteryCharging ( void );

#endif




