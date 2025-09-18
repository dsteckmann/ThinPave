/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef SDCARDHEADER
#define SDCARDHEADER
#include "Globals.h"
  
//#include "prompts.h"
#include "Alfat.h"
#include "device.h"
#include "FS.h"
#define SD_CARD_OUT 1

#define ID_LEN      9         // NAME
#define MAX_NUMBER_STATIONS	99
#define MAX_NUMBER_TUBES      99
#define MAX_NUMBER_READINGS   99
#define NUMBERSTDCNTS         32  
  
#define PROJECT_TYPE_SIMPLE 1
#define PROJECT_TYPE_FULL   2
#define PROJECT_TYPE_DATE   3  
  
#define ENABLE_SD			   0
#define DISABLE_SD   		1
  
enum { SDOUT,SDFAILED,SDERRSEEK,SDERRWRITE,SDCRDERR,CALCON,STATISTICAL,DRIFTPRNT,STANDARDPRINT,JUNITS,JDATE,LOADING};
extern  int8 sdOpened;

void    SD_Diag(void);
void    SDstop(FS_FILE *file);
void    SDstart();
int32   SDreadBuffer(FS_FILE *file,char *buf,int32 len);
FS_FILE* SD_CreateProjectSimpleFile (char* str);
bool     SD_CheckIfProjExists ( char* str );
int32    SD_WriteBuffer(FS_FILE *file,char *buf,int32 len); 
int32    SD_FindTotalFiles ( char* dir, bool directory );
int32    SD_FindFile ( int32 num, char* dir, char* fname, bool directory);
uint8    CreateDir(char * path);
int32    SD_FormatHi(void);
FS_FILE* SDProjOpen ( char* project );
void     DeleteProjectDirectory(void);
void     sd_card_menu();
void     SD_Wake();

uint8 RemoveDir(char * path);

#endif
//[] END OF FILE
