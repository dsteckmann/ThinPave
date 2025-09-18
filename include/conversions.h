/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  conversions.h
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
#ifndef CONVERSIONS_H
#define CONVERSIONS_H
#include "Globals.h"
/*---------------------------------------------------------------------------*/
/*--------------------[   Global Functions Prototypes   ]--------------------*/
/*---------------------------------------------------------------------------*/
extern int32_t atofi(char *number_ptr, BYTE precision);
extern void itofa(int32_t number, char *number_ptr, int16_t precision);
extern uint32_t atoh32(char *s);

#endif /* endif !CONVERSIONS_H for "if we haven't included this file already..."    */

