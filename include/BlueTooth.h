/* ========================================
 * Copyright InstroTek Inc., 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek Inc..
 * ========================================*/

#ifndef BLUETOOTHEADER
#define BLUETOOTHEADER
   
 #include <project.h>
 #include <stdbool.h>
 #include "ProjectData.h"
  
   
 int8 BluetoothEnable();
 void BT_Disable(bool verbal);
 void doBluetooth();
 void sendPcStr(char *str);
 void sendBlueResults( station_data_t station_d ) ;
 
#endif

/* [] END OF FILE */
