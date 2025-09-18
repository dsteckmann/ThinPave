/*********************************************************************
 *
 *                  Generic Typedef's
 *
 *********************************************************************
 * FileName:        Generic.h
 * Dependencies:
 * Processor:       dsPIC33F/PIC24H
 * 
 * Complier:        MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.22.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/
#ifndef _DATATYPES_H_
#define _DATATYPES_H_

/*  UnComment the following macro to use AND_OR mask based setting, Comment it to use AND based macros */
//#define USE_AND_OR

#define PUBLIC
#define PROTECTED
#define PRIVATE                 static

typedef unsigned char       BYTE;               // unsigned 8-bit
typedef unsigned short int  WORD;               // unsigned 16-bit
typedef unsigned long       DWORD;              // unsigned 32-bit

typedef unsigned int        UINT16;
typedef unsigned char       UINT8;              // other name for unsigned 8-bit integer
typedef unsigned long       UINT32;             // other name for unsigned 32-bit integer

typedef signed int          INT16;
typedef signed char         INT8;              // other name for signed 8-bit integer
typedef signed long         INT32;             // other name for signed 32-bit integer

typedef long double         DOUBLE_FLOAT;       // this is double 64 bit floating point 

typedef unsigned long       uint32_t;           // other name for unsigned 32-bit integer
typedef float               fp32_t;
typedef long double         fp64_t;
//typedef unsigned int        uint16_t;
typedef unsigned char       uint8_t;                // other name for unsigned 8-bit integer
//typedef signed int          int16_t;
typedef signed char         int8_t;              // other name for signed 8-bit integer
typedef signed long         int32_t;             // other name for signed 32-bit integer


typedef enum _BOOL { FALSE = 0, TRUE } BOOL, Bool;


typedef struct
{
    BYTE    b0:     1;
    BYTE    b1:     1;
    BYTE    b2:     1;
    BYTE    b3:     1;
    BYTE    b4:     1;
    BYTE    b5:     1;
    BYTE    b6:     1;
    BYTE    b7:     1;

}BYTE_BITS;
typedef struct
{
    WORD    b0:     1;
    WORD    b1:     1;
    WORD    b2:     1;
    WORD    b3:     1;
    WORD    b4:     1;
    WORD    b5:     1;
    WORD    b6:     1;
    WORD    b7:     1;
    WORD    b8:     1;
    WORD    b9:     1;
    WORD    b10:    1;
    WORD    b11:    1;
    WORD    b12:    1;
    WORD    b13:    1;
    WORD    b14:    1;
    WORD    b15:    1;
}WORD_BITS;

typedef union _BYTE_VAL
{
    BYTE_BITS bits;
    BYTE Val;
} BYTE_VAL;


typedef union _WORD_VAL
{
    WORD Val;
    WORD_BITS   bits;
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        BYTE_VAL    low;
        BYTE_VAL    high;
    }byteUnion;

    BYTE v[2];
} WORD_VAL;

typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        WORD_VAL    low;
        WORD_VAL    high;
    }wordUnion;
    struct
    {
        BYTE_VAL    lowLSB;
        BYTE_VAL    lowMSB;
        BYTE_VAL    highLSB;
        BYTE_VAL    highMSB;
    }byteUnion;
    BYTE v[4];
    WORD w[2];  
} DWORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])


#endif
