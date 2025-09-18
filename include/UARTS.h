/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  UARTS.h
 *  Originating Author:  DMS
 *       Creation Date:  8/2012 
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
#ifndef UARTS_H
#define UARTS_H
#include "Globals.h"
/*---------------------------------------------------------------------------*/
/*---------------------------[  Global Constants  ]--------------------------*/
/*---------------------------------------------------------------------------*/

#define BUFF_SIZE 256
  
/*---------------------------------------------------------------------------*/
/*----------------------------[  Global Variables  ]-------------------------*/
/*---------------------------------------------------------------------------*/
typedef struct 
{
  uint8_t rx_buffer[BUFF_SIZE];
  uint16_t buffer_start;
  uint16_t buffer_end;
} RX_BUFFER;

extern RX_BUFFER  s_Alfat_RX;
extern RX_BUFFER  s_Computer_RX;


/*---------------------------------------------------------------------------*/
/*----------------------[  Global Function Prototypes  ]---------------------*/
/*---------------------------------------------------------------------------*/
extern void initUARTS ( void );


extern void clearAlfatRXBuffer ( void );

extern void clearComputerRXBuffer ( void );

extern uint16_t getcRXBuffer ( RX_BUFFER * );

extern uint16_t getRXNumEntries ( RX_BUFFER  *  );

extern uint16_t waitc_RX ( RX_BUFFER * buffer , int32_t max_msec_delay );

#endif 


