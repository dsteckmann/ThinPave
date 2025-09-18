/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  conversions.c
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 ******************************************************************************/
 
  /*---------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/ 
/*---------------------------------------------------------------------------*/ 
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------ 
 *  10/21/10    CC      Original 
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/ 
/*----------------------------[  Include Files  ]----------------------------*/ 
/*---------------------------------------------------------------------------*/
#include "Globals.h"
#include "conversions.h"

/*---------------------------------------------------------------------------*/
/*-------------------------[   Global Functions   ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*******************************************************************************
 *
 ** atofi
 *
 *  PARAMETERS:Pointer to string, desired floating point precision
 *
 *  DESCRIPTION:Converts a string to an int32 representation of a float with
 *              specified precision.
 *
 *  RETURNS:The converted number
 *
 ******************************************************************************/
int32_t atofi(char *number_ptr, BYTE precision)  // converts a string to an int32 representation of a float with specified precision
{
  uint8_t length, x, y;
  int32_t num_temp, c=1, i;
  
  char temp[9] = "\0\0\0\0\0\0\0\0";  
  
  strcpy(temp,number_ptr);  
  length = strlen(number_ptr);
    
  for ( i=0; i<length; i++ )                           // slide digits over if number is a float
  {                                               // and return an 1nt32 = valueE10
    if ( temp[i] == '.' )
    {           
      for ( c=0;c<precision && temp[c+i+1] != '\0';c++ )
      {
          temp[c+i] = temp[c+i+1];
      }    
      temp[c+i] = '\0';
      x = c;
      c = 1;
      
      for ( y=1;y<=x;y++ )
      {
          c *= 10;
      }    
      
      //  num_temp = atoi32(temp);
      sscanf( temp, "%ld", &num_temp); 
      break;
    }         
  }      
  
 // sscanf( number_ptr, "%ld", & num_temp);
  if (i == length)  // precision is 0
  {
    //num_temp = atoi32(number_ptr);     
    sscanf( number_ptr, "%ld", & num_temp); 
  }  
 
  i = 1;  
  for(y=1;y<=precision;y++)     //scale number to correct precision no matter what
  {
      i *= 10;
  }    
  i = i/c;  
  num_temp *= i;   
  return(num_temp); 
}
/*******************************************************************************
 *
 ** itofa
 *
 *  PARAMETERS:Number to be converted, destination string, floating precision
 *
 *  DESCRIPTION:Converts an int32 representation of a float into a string in
 *              order to be displayed.
 *
 *  RETURNS:None
 *
 ******************************************************************************/
void itofa(int32_t number, char *number_ptr, int16_t precision) // converts an int32 representation of a float into a string in order to be displayed
{
/*  int8_t negative = FALSE;
  int16_t len, prec;  
  int16_t i;
  int32_t num;  
  
  num = number;
  prec = precision;
  if(num == 0)
  {
    number_ptr[0] = '0';
    for(i=1;i<11;i++)
      number_ptr[i] = '\0';
    return;
  } 
  if(num < 0)                                      // check for sign of number
  {
    num *= -1;  // invert sign to make conversion simpler
    negative = TRUE;
  }  
  itoa(number_ptr,num,10);                         // write int value into number_ptr
  
  len = strlen(number_ptr);                    // determine length of new strin  
  if(len > prec)  // insert the decimal place into the string
  {                                                   // to give a precision of 6
    number_ptr[len+1] = '\0';
    i = len-1;
    for(;i>=(len-prec);i--)
    {
      number_ptr[i+1] = number_ptr[i];                // slide digits to the right
    }
    number_ptr[i+1] = '.';                            // insert '.' into string          
  }    
  else  //  add '0.' then zeros in front of number
  {  
    prec += 2;
    number_ptr[prec] = '\0';                           // add EOL to string
    for(i=1;i<=prec;i++)
    {
      if(i<=len)
        number_ptr[prec-i] = number_ptr[len-i];        // move numbers over to the right
      else if((i < prec-1) || (i == prec))
        number_ptr[prec-i] = '0';                      // place leading zeros
      else 
        number_ptr[prec-i] = '.';                            // place dicimal point
    }
  }
  
  if(negative)  //slide numbers to the right and insert minus sign
  {
    len = strlen(number_ptr);
    number_ptr[len+1] = '\0';
    for(i=len;i>0;i--)
      number_ptr[i] = number_ptr[i-1];
    number_ptr[i] = '-';  //insert minus sign 
  } 
  i = strlen(number_ptr);
  for(i -= 1; number_ptr[i] == '0' || number_ptr[i] == '.'; i--)  // remove trailing zeros and '.' if precision of zero
  {
    if(number_ptr[i] == '.')
    {
      number_ptr[i] = '\0';
      break;
    }
    else
      number_ptr[i] = '\0';     
  }// add EOL to replace any trailing zeros and remove '.'
*/

DOUBLE_FLOAT conv = (DOUBLE_FLOAT)number;
uint8_t n;

for ( n = 0; n < precision; n++ )
{
  conv /= 10.0;
}  

sprintf ( number_ptr, "%.6f",(double)conv );

}
/*******************************************************************************
 * 
 ** atoh32
 *
 *  PARAMETERS:String of characters representing a number
 *
 *  DESCTRIPTION:Converts a string of up to 8 characters into an unsigned int32
 *
 *  RETURNS:Converted number
 *
 ******************************************************************************/
uint32_t atoh32(char *s)  //converts a string of up to 8 characters to an int32.
{
  uint8_t i; 
  uint32_t n=0;
    
  for(i=0;i<8;i++)  //convert to hex
  {
    if(s[i] >= '0' && s[i] <= '9')
      n = 16 * n + (s[i] - '0');
    else if(s[i] >= 'A' && s[i] <= 'F')
    {
      n = 16 * n + (s[i] - 55);
    }
    else
      return(n);
  }
  return(n);
}

