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

#include "stdio.h"
#include "SDcard.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "ProjectData.h"
#include "DataStructs.h"
char fname[255]; 
int8 sdOpened = OFF;

extern uint16 resetProjectStorage ( void );

char *SD_Str[LOADING + 1] = 
  { 
	"SD NOT Installed","SD FAILED!","SD ERROR SEEK!","SD ERROR WRITE","SD CARD ERROR","Calibration Constants",
   "Statistical Test Results","Drift Test Results","Standard Test Results","Units","Date","Loading"
	};
  
  char *AlfatStr[ALFATMOUNT + 1] =
  { 
	"Opening tst.txt", "Writing:","Reading tst.txt","Read:","USB failed:","Press ESC to Exit","USB NOT INSERTED","Insert USB and","try again.",
  "File Not Found (ESC)","Initializing Drive"	};

  
  
  
/*******************************************************************************
* Function Name: SD_Format Hi
********************************************************************************
* Summary:     
* Parameters:  
*              
*              
*              
*             
* Return:      0 sucess
*******************************************************************************/
int32 SD_FormatHi(void)
{
   char buf[50];
   int32 err;
   FS_FORMAT_INFO fs_info;
   fs_info.NumRootDirEntries = 256;
   fs_info.SectorsPerCluster = 64;
   fs_info.pDevInfo = NULL;
   err = FS_GetVolumeName (0,buf,50);

   CLEAR_DISP;
   DisplayStrCentered(LINE1, "Formatting Card");
   DisplayStrCentered(LINE2, "This will take");      
   DisplayStrCentered(LINE3, "time to do.");      
   err = FS_Format(buf,&fs_info);
   CLEAR_DISP;
   if ( err == 0 )
   {
    DisplayStrCentered(LINE1, "Formatting Done");
   }
   else
   {
    DisplayStrCentered(LINE1, "Formatting Error");
   }
    
    SDstart();
    resetProjectStorage();
    SDstop(0);

  
   CyDelay(2000);
   return err;

}
/*******************************************************************************
* Function Name: CheckEmptyDirectory()
********************************************************************************
* Summary:     checks for empty directory, if not empty, queries user to delet it
*              if yes, deletes all files
* Parameters:  char *path to directory, MUST BE LARGE ENOUGH TO HOLD PAHT AND FILE NAME
*              char *buffer to hold file names
*              int32 len of buffer
* Return: 		0 if empty
*******************************************************************************/
int32 CheckEmptyDirectory(char *path,char *name,int32 nameLen)
{
   int32 res = 0;
   FS_FIND_DATA pfd;
   char fullpath[50];
   if( SD_FindTotalFiles(path,false) != 0 ) 
   { 
  
    FS_FindFirstFile(&pfd,path,name,nameLen);
      do 
      {
        if( ( pfd.Attributes & FS_ATTR_DIRECTORY ) != FS_ATTR_DIRECTORY ) 
        {
          snprintf(fullpath,50,"%s\\%s",path,name);
          FS_Remove(fullpath);
        }
       res = FS_FindNextFile(&pfd);
      } while(res != 0);
    FS_FindClose(&pfd); // Close the dir search
   }

   return res;
}


/*******************************************************************************
* Function Name: RemoveDir
********************************************************************************
* Summary: 
* Parameters:  char *path = directory to remove
* Return: 0 if successful, else 1
*******************************************************************************/
uint8 RemoveDir(char * path)
{
   int32 res = 0, error = 0;
   FS_FIND_DATA pfd;
   char fullpath[50],name[30];
  
   // if there are files in the directory, remove them
   if( SD_FindTotalFiles(path,false) != 0 ) 
   { 
  
    FS_FindFirstFile(&pfd,path,name,sizeof(name)); // find the first file in the directory.
    do 
    {
     // if this is a file and not a sub-directry, delete the file. THERE SHOULD BE NO SUB DIRECTORIES
     if( ( pfd.Attributes & FS_ATTR_DIRECTORY ) != FS_ATTR_DIRECTORY ) 
     {
      snprintf(fullpath,50,"%s\\%s",path,name);
      FS_Remove(fullpath);
     }
       
      // == 1: File found in directory.
      // == 0: In case of any error.
      res = FS_FindNextFile(&pfd); //puts new file name into name array
    } while(res == 1);
    
    FS_FindClose(&pfd); // Close the dir search
   }

   if( SD_FindTotalFiles(path,false) == 0 )  
   {
    if(FS_RmDir(path) != 0)
    {
      CLEAR_DISP;
      LCD_position(LINE1 + 8); 
      _LCD_PRINT("Error");
      LCD_position(LINE2);
      _LCD_PRINT("SD ERROR RMDIR");
      CyDelay ( 2000 );
      error = 1;
    }
    else
    {
     error = 0;
    }
      
   } 
   else
   {
    error = 1;
   }
   return error;
}

/*******************************************************************************
* Function Name: DeleteProjectDirectory()
********************************************************************************
* Summary:     permits user to delete full directories
* Parameters:  none
* Return: 		none
*******************************************************************************/
void DeleteProjectDirectory(void)
{
  int32 error;

  error = RemoveDir("\\Project");
  if ( error == 1 )
  {
  
    CLEAR_DISP ;
    DisplayStrCentered(LINE1,"Error Deleting");
    DisplayStrCentered(LINE2,"Directory");
  }
} 

/*******************************************************************************
* Function Name: createProjectDirectory()
********************************************************************************
* Summary: Get SD card Info and display it
* Parameters:  none
* Return: none
*******************************************************************************/
void createProjectDirectory()
{
  if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
                                                      
    CLEAR_DISP;
    LCD_position(LINE1 + 8); 
    _LCD_PRINT("Error");
    LCD_position(LINE2);
    _LCD_PRINT("SD Card Not Detected");
    CyDelay ( 1000 );
  }
  else
  {
    if ( CreateDir( "Project" ) )
    {
     CLEAR_DISP;
     LCD_position(LINE1); 
     _LCD_PRINT("Project Directory");
     LCD_position(LINE2+7);
     _LCD_PRINT("Created");
     CyDelay ( 1000 );
    }
    else
    {
     CLEAR_DISP;
     LCD_position(LINE1); 
     _LCD_PRINT("Project Directory");
     LCD_position(LINE2+5);
     _LCD_PRINT("Not Created");
     CyDelay ( 1000 );
    }
  }  
}

/*******************************************************************************
* Function Name: SD_FindTotalFiles
********************************************************************************
* Summary:     finds total number of files in a directory
* Parameters:  char* directory to search, FS_FIND_DATA *pfd,
*              char* fname - just so I don't have to put another one on the stack
*              int   flen length of fname buffer
*              bool directory - if true searches for directories, else files
*              bool openProject - looking for an incomplete project
* Return:      number of files in the directory
*******************************************************************************/
int32 SD_FindTotalFiles ( char* dir, bool directory )
{
  uint16 res = 0,err = 0;

  char fname[31];
  res = 0;
  FS_FIND_DATA fd;
  err = FS_FindFirstFile( &fd,dir,fname,sizeof(fname)); 
  if ( err == 0 )
  {
    err = 1;
  }
  else if ( err == 1 )
  {
   err = 0 ;
  }
  else
  {
   return err;
  }
  
  while(err )
  {
      if(directory == true)
      {
         if( ( fd.Attributes & FS_ATTR_DIRECTORY ) == FS_ATTR_DIRECTORY)
        {
            if(fd.sFileName[0] != '.') {
               res++;
            }
         }
      }
      else
      {
         if( ( fd.Attributes & FS_ATTR_DIRECTORY ) != FS_ATTR_DIRECTORY ) 
         {
           res++; 
         }
      }
      err = FS_FindNextFile(&fd);//SD_FindNextFile(pfd,directory,false,null);
  
   }
   FS_FindClose(&fd);
   return res;
}
 

/*******************************************************************************
* Function Name: FindFile()
********************************************************************************
* Summary:     finds the nth number file in directory dir
* Parameters:  number of file to find, FS_FIND_DATA *pfd
*              char* directory to search
*              char* fname - used for result
*              int   flen length of fname buffer
*              bool directory - if true searches for directories, else files
* Return:      0 = success, the name of the file in char* fname
*******************************************************************************/
int32 SD_FindFile ( int32 num, char* dir, char* fname, bool directory)
{
   int32 err = 0;
   FS_FIND_DATA pfd;

   err = FS_FindFirstFile ( &pfd, dir, fname,21) ^ 1;
   while(err == 1)
   {
      if(directory == true) 
      {
        if( ( pfd.Attributes & FS_ATTR_DIRECTORY ) == FS_ATTR_DIRECTORY ) 
        {
            num--;
        }
      }
      else 
      {
         if( ( pfd.Attributes & FS_ATTR_DIRECTORY ) != FS_ATTR_DIRECTORY ) 
         {
          num--;
         }
      }
      if(num==0) {  return (0); }
      err = FS_FindNextFile (&pfd);
   }
   FS_FindClose(&pfd);
   return 1;
}
/*******************************************************************************
* Function Name: SDstart()
********************************************************************************
* Summary: raises sd wake and reset lines, starts uart and interrupt
* Parameters:  none
* Return: none
*******************************************************************************/
void SDstart()
{
  sdOpened = ON;
  CyDelay(50);
  FS_Init();
  FS_FAT_SupportLFN();
}
/*******************************************************************************
* Function Name: SDstop
********************************************************************************
* Summary: Stops sd reset low,wakeup low,pwr ctrl low
* Parameters:  none
* Return: none
*******************************************************************************/
void SDstop(FS_FILE *file)
{
 if ( sdOpened == ON )
 {
  sdOpened = OFF;
   
   if(file != null) { 
      FS_FClose(file); 
   }
  emFile_1_Sleep(); // This also calls emFIle_saveConfig()

  FS_DeInit();
 } 
}

/*******************************************************************************
* Function Name: SD_Wake()
********************************************************************************
* Summary: raises sd wake and reset lines, starts uart and interrupt
* Parameters:  none
* Return: none
*******************************************************************************/
void SD_Wake()
{
  if ( sdOpened == OFF )
  {
    sdOpened = ON;
    FS_Init();
    FS_FAT_SupportLFN();
    emFile_1_Wakeup();
  }
}

/*******************************************************************************
* Function Name: Create Directory
********************************************************************************
* Summary: creates a directory on the SD card
* Parameters:  char *path = directory to create
* Return: 1 if created
*******************************************************************************/
uint8 CreateDir(char * path)
{
	FS_DIR * pDir;
  if( ( pDir = FS_OpenDir(path) ) == 0) 
  { // directory does not exists
		if( 0 != FS_MkDir(path) ) 
    {
		  FS_CloseDir(pDir);
		   return(0);
	  }
    FS_CloseDir(pDir);
	  return (1);
	}
	FS_CloseDir(pDir);
	return (1);
}



/*******************************************************************************
* Function Name: SD_CreateProjectSimpleFile()
********************************************************************************
* Summary:     checks for/creates Simple Directory
* Parameters:  char * file name
* Return:      null if created, else err string
*              
*******************************************************************************/
FS_FILE* SD_CreateProjectSimpleFile (char* proj_str )
{
   FS_FILE *file = null;
   int8 flag;
   char buf[50];
   
	if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
                                                       
    CLEAR_DISP;
    LCD_position(LINE1 + 8); 
    _LCD_PRINT("Error");
    LCD_position(LINE2);
    _LCD_PRINT("SD Card Not Detected");
    LCD_position(LINE4);
    CyDelay ( 2000 );
    return null;
  }
  
  snprintf(buf,50,"Project\\%s",proj_str);
  file = FS_FOpen(buf,"wb+");
  // write dummy values to project data.
  
  // Can't create file
  if(file == null)
  {  
    /* This will create directory "Dir" on SD card */
    // Maybe project didn't exist
    flag = CreateDir( "Project" );
    if ( flag < 0 )
    {
      CLEAR_DISP;
      LCD_position(LINE1); 
      _LCD_PRINT("Error: Can't");
      LCD_position(LINE2);
      _LCD_PRINT("Create Project Dir");
      CyDelay ( 2000 );
      return null;
    }
    else
    {
      snprintf(buf,50,"Project\\%s",proj_str);
      file = FS_FOpen(buf,"wb+");
      if(file == null)
      { 
        // cannot open file emf32noslfn
        CLEAR_DISP;
        LCD_position(LINE1 + 8); 
        _LCD_PRINT("Error");
        LCD_position(LINE2);
        _LCD_PRINT("SD ERR SFOPEN");
        CyDelay ( 2000 );
        return null;
      }
    }
  }

  FS_FClose ( file );
  return file;
}

/*******************************************************************************
* Function Name: SD_CheckIfProjExists()
********************************************************************************
* Summary:     Checks if the project exists
* Parameters:  char * file name
* Return:      null if created, else err string
*******************************************************************************/
bool SD_CheckIfProjExists ( char* str )
{
   bool res = false;
   FS_FILE *file = null;
   char buf[50];

   snprintf(buf,50,"\\Project\\%s",str);
   file = FS_FOpen(buf,"r");
   if(file == null)
   {  // cannot open file emf32noslfn
      res = false;
   }
   else
   {
      res = true;
      FS_FClose(file);
   }
   return res;
}

/*******************************************************************************
* Function Name: SD_Diag()
********************************************************************************
* Summary: Get SD card Info and display it
* Parameters:  none
* Return: none
*******************************************************************************/
void SD_Diag(void)
{
 	FS_FILE *SDfile = null;
	char buf[11],buffer[20];
	uint32 err;
  int8 flag;
  if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
                                                       
    CLEAR_DISP;
    LCD_position(LINE1 + 8); 
    _LCD_PRINT("Error");
    LCD_position(LINE2);
    _LCD_PRINT("SD Card Not Detected");
    CyDelay ( 1000 );
  }

  /* This will create directory "Dir" on SD card */
  flag = CreateDir("ProjTest");
  
  if( flag == 0  )
  {
    /* Display successful directory creation message */
    CLEAR_DISP;  
    DisplayStrCentered(LINE1, "\"ProjTest\" exists");
    CyDelay(1000);
  }
  else if ( flag == 1 )
  {
        /* Display successful directory creation message */
     	CLEAR_DISP;  
      DisplayStrCentered(LINE1, "\"ProjTest\" created");
      CyDelay(1000);
  }
  else
  {
        /* Display failure message */
     	CLEAR_DISP;
      DisplayStrCentered(LINE1,"Failed to create");
      DisplayStrCentered(LINE2,"ProjTest directory");
      CyDelay(1000);
      goto SD_DIAG_EXIT;
  }
    
    CyDelay(1000);
    CLEAR_DISP;
    DisplayStrCentered(LINE1,"Opening tst.txt");
    CyDelay ( 1000 );
    SDfile = FS_FOpen("\\ProjTest\\tst.txt","w");
   	if( SDfile != 0)
   	{ 
         DisplayStrCentered(LINE1,AlfatStr[ALFATWRITING]);
         DisplayStrCentered(LINE2,"0123456789");
         CyDelay(1000);
         err = FS_Write(SDfile, "0123456789", 10);
         FS_FClose(SDfile);
         if(err != 0)  
         {
            CyDelay(1000);
            CLEAR_DISP;
            DisplayStrCentered(LINE1,AlfatStr[ALFATREADING]);
            CyDelay(1000);
            SDfile = FS_FOpen("\\ProjTest\\tst.txt","r");
            if(SDfile != 0)
            {
               if((err = FS_Read(SDfile,buf,10)) == 10) //alfatError[0]
               {
                  buf[10] = '\0';
                  DisplayStrCentered(LINE1,AlfatStr[ALFATREAD]);
                  DisplayStrCentered(LINE2,buf);
                  CyDelay(1000);
                }
               else err = 0;
               FS_FClose(SDfile);
               //err = FS_Remove("\\ProjTest\\tst.txt"); // 0 if successfull
               strncpy (buffer, "\\ProjTest", sizeof(buffer));
               err = FS_RmDir(buffer); // returns 0 is successfully removed
               
               err = FS_Remove("\\ProjTest\\tst.txt"); // 0 if successfull
               err = FS_RmDir(buffer); // returns 0 is successfully removed
              
               if ( err == 0 ) 
                  err = -1;
               else
                 err = 0;
              
            }
            else err = 0;
         }
    }
    else 
    {
      err = 0;
    }  
   
   if(err == 0)
   {
      DisplayStrCentered(LINE4,SD_Str[SDFAILED]);
      CyDelay(1000);
   }
  
  
SD_DIAG_EXIT:
   CyDelay(250);
   CLEAR_DISP;
}
/*******************************************************************************
* Function Name: File WriteBuffer
********************************************************************************
* Summary:     Write a buffer to the file, displays err string if not written
* Parameters:  char * buf data to write, 
*              FS_FILE* file to write to, 
*              int len of buffer
* Return: 0 success -1 error
*******************************************************************************/
int32 SD_WriteBuffer(FS_FILE *file, char *buf,int32 len)
{
   int32 err;
   err = FS_Write(file,buf,len);
   if(err != len)
  {
    DisplayStrCentered (LINE1,SD_Str[SDERRWRITE]);
    DisplayStrCentered( LINE4,AlfatStr[PRESSESC] );
    CyDelay ( 1000 );
    return (-1);
   }
   return (0);
}


/*******************************************************************************
* Function Name: SDreadBuffer
********************************************************************************
* Summary: reads a string for the given file
* Parameters:  char* buf to store the data, len of buffer
* Return: -1 error 0 success
*******************************************************************************/
int32 SDreadBuffer(FS_FILE *file,char *buf,int32 len)
{
   int32 err;
   err = FS_Read(file,buf,len); // read in 1 character at a time
   if(err != len) {            // error reading
      CLEAR_DISP;
      DisplayStrCentered (LINE1,SD_Str[SDCRDERR]);
      DisplayStrCentered( LINE4,"Read Error" );
      CyDelay (1000 );
      return (-1);
   }
   return 0;
}


/*******************************************************************************
* Function Name: SDProjOpen()
********************************************************************************
* Summary: Get SD card Info and display it
* Parameters:  none
* Return: none
*******************************************************************************/
FS_FILE * SDProjOpen ( char* project )
{
	char buf[30];
  FS_FILE * sd;


	if( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  { 
   CLEAR_DISP;  
   DisplayStrCentered(LINE1,SD_Str[SDOUT]);  
   CyDelay(2000);
  }
  snprintf ( buf,30, "\\Project\\%s",project ); 
  sd = FS_FOpen( buf, "r+" );
  return sd;
 
}


/******************************************************************************
 *
 *  Name: testProjectCreation()
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION:  
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 
uint8 SDtestProjectCreation ( void )
{


  FS_FILE *file = null;
  char project_name [50];
  char buf[50],i;
  int error;
  
  SD_Wake(); 
  strcpy ( project_name, "ProjTest" );
  
	if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
                                                       
    CLEAR_DISP;
    LCD_position(LINE1 + 8); 
    _LCD_PRINT("Error");
    LCD_position(LINE2);
    _LCD_PRINT("SD Card Not Detected");
    LCD_position(LINE4);
    _LCD_PRINT("Press <ESC> to Exit");
    while( getKey( TIME_DELAY_MAX ) != ESC ) ;
    return 0;
  }
  
  /* This will create directory "Dir" on SD card */
    if( 1 == CreateDir(project_name) )
    {
        /* Display successful directory creation message */
     	CLEAR_DISP;  
      DisplayStrCentered(LINE1, "\"ProjTest\" created");
      CyDelay(1000);
    }
    else
    {
        /* Display failure message */
     	CLEAR_DISP;
      DisplayStrCentered(LINE1,"Failed to create");
      DisplayStrCentered(LINE2,"ProjTest directory");
      CyDelay(1000);
      return(0);
    }
  
  for ( i=1; i < 20; i++ )
  {
    snprintf(buf,50,"%s\\Test%d.txt",project_name,i);
    file = FS_FOpen(buf,"wb+");
 
    if ( file == NULL )
    {  // cannot open file emf32noslfn
      CLEAR_DISP;
     LCD_position(LINE1 + 8); 
     _LCD_PRINT("Error");
     LCD_position(LINE2);
     _LCD_PRINT("SD ERR SFOPEN");
     LCD_position(LINE4);
     _LCD_PRINT("Press <ESC> to Exit");
     getKey( TIME_DELAY_MAX );
     return 0;
    }
    else
    {
      CLEAR_DISP;  
      snprintf(buf,50,"Test%d.txt",i);
      DisplayStrCentered(LINE1,buf);    
      DisplayStrCentered(LINE2,"Created");
      CyDelay(500);
    }  
   
    FS_FClose ( file );
  }
  
  // now delete each file
  
 for ( i=1; i < 20; i++ )
 {
    snprintf(buf,50,"%s\\Test%c.txt",project_name,i);
    error =  FS_Remove ( buf ) ;
    if ( file == NULL )
    {  // cannot open file emf32noslfn
      CLEAR_DISP;
      DisplayStrCentered(LINE1,"Error Deleting");
      sprintf(buf,"%s",project_name );
      DisplayStrCentered(LINE2,buf);
      DisplayStrCentered(LINE3,(char * )FS_ErrorNo2Text(error));
      DisplayStrCentered(LINE4,"Press <ESC> to Exit");
      getKey( TIME_DELAY_MAX );
      return 0;
    }
    else
    {
      CLEAR_DISP;  
      snprintf(buf,50,"Test%d.txt",i);
      DisplayStrCentered(LINE1,buf);    
      DisplayStrCentered(LINE2,"Deleted");
      CyDelay(500);
    }
   
  }

  RemoveDir(project_name);
  return 1;
}
/******************************************************************************
 *  Name: 
 *  PARAMETERS: 
 *  DESCRIPTION:  
 *  RETURNS: 
 *****************************************************************************/ 
void sd_card_menu(void)
{
  uint8_t menu_track = 1, menu_n = 3, selection;    
  enum buttons button;
  
  in_menu = TRUE;
 
  if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
                                                       
    CLEAR_DISP;
    LCD_position(LINE1 + 8); 
    _LCD_PRINT("Error");
    LCD_position(LINE2);
    _LCD_PRINT("SD Card Not Detected");
    CyDelay ( 1000 );
    SDstop(0); // FS_DeInt if no card detected
  }
  else
  {
    SD_Wake(); 
    while(1)                    // only exit menu when ESC is pressed
    { 
      sd_menu_display(menu_track);
      in_menu = FALSE;
      while(1)
      {
        button = getKey( TIME_DELAY_MAX );
        if((button <= 11) || (button == ESC) || (button == MENU))
        {
            break;
        }    
      }          
      if((button == ESC) || (button == MENU))             // ESC->ready screen, MENU->main menu
      {
          break;  
      }    
      else if(button == UP)                 
      {
          menu_track = ((menu_track + menu_n) - 1)%menu_n;     
      }    
      else if(button == DOWN)                
      {
          menu_track = (menu_track + 1)%menu_n;                  
      }    
      else if(button <= 9)                // selection was made
      { 
        selection = button;    
        if(selection == 1)                  // first button was a 1, wait for second button for 1 sec.
        {
          button = getKey (1000);
          
          if( button <= 9 )
          {
            selection = selection*10 + button;
          }
        }
        switch(selection)
        {
          case  1: SD_Diag() ;  
                    break;  // monitor battery voltage 
          case  2: SD_FormatHi(); 
                    break;
          case  3: 
                  CLEAR_DISP ;
                  DisplayStrCentered(LINE1,"Deleting \"Project\"");
                  DisplayStrCentered(LINE2,"Directory");
                  CyDelay (1000);
                  RemoveDir("\\Project");
                  break;  //
          case  4: createProjectDirectory();
                   break;  // 
          
          case  5: SDtestProjectCreation();
                   break;  // 
          default: break;
        }
        if(button==ESC)
        {
            break;
        }    
        in_menu = TRUE;
      }      
    }  
    SDstop(0);
  }  
}

/* [] END OF FILE */
