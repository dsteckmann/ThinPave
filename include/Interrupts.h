
#ifndef INTERRUPTS_H

#define INTERRUPTS_H
  
#include "Globals.h"  
  
#define CHARGING_STATE_UNPLUGGED     0
#define CHARGING_STATE_STARTED       0  
#define CHARGING_STATE_CHARGING      0    
  
  
  
extern uint8 CHECK_KEYSCAN_ENABLE ( void);
void initInterrupts  ( void );

#endif

