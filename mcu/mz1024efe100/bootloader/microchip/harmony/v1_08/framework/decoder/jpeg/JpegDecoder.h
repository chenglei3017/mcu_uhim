#ifndef __JPEGDECODER_H__
#define __JPEGDECODER_H__

/******************************************************************************
* File Name:        JpegDecoder.h
* Dependencies:    Image decoding library; project requires File System library
* Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology Inc.

 * Software License Agreement
 *
 * Copyright ?2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.

Author                 Date           Comments
--------------------------------------------------------------------------------
Pradeep Budagutta    03-Mar-2008    First release
*******************************************************************************/

#include "gfx/gfx_types_resource.h"

#define JPEG_SendError(x)  pJpegDecoder->bError = x; return(x); /* Point to proper error handler routine */

#define MAX_CHANNELS      3 /* This supports only Grayscale and YCbCr images - DONT CHANGE THIS */
#define MAX_BLOCKS        6 /* To decode one logical block, we have to decode 1 to 6 blocks depending on channels and subsampling - DONT REDUCE THIS */
#define MAX_HUFF_TABLES   2 /* Each causes 2 tables -> One for AC and another for DC - DONT REDUCE THIS */
#define MAX_DATA_BUF_LEN  JPEG_DATA_BUF_LEN /* Increase if you have more data memory */

#define EXIF_2_2_ORIENTATION_OFFSET 0x2E

/* Error list */
enum Errors
{
     IMAGE_FILE_NOT_AVAILABLE
};

/* JPEG Markers list */
enum Markers
{
     SOF0 = 0xC0,
     DHT  = 0xC4,
     SOI  = 0xD8,
     EOI  = 0xD9,
     SOS  = 0xDA,
     DQT  = 0xDB,
     DRI  = 0xDD,
     APP0 = 0xE0,
     APP1 = 0xE1, // EXIF
     COM  = 0xFE,
/* The below markers doesn't have parameters */
     TEM  = 0x01,
     RST0 = 0xD0,
     RST1 = 0xD1,
     RST2 = 0xD2,
     RST3 = 0xD3,
     RST4 = 0xD4,
     RST5 = 0xD5,
     RST6 = 0xD6,
     RST7 = 0xD7
};


/* Function prototype */
/* This function must be called after setting proper values in the global variables of ImageDecoder.c */
uint8_t JPEG_bDecode(IMG_FILE *pfile);
int8_t JPEG_bInterpretResourceHeader(GFX_RESOURCE_HDR* header);

#endif
