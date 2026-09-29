/*******************************************************************************
  Graphics JPEG Image Decoder

  Company:
    Microchip Technology Inc.

  File Name:
    jpeg_image.h

  Summary:
    This implements the jpeg image decoding of the primitive layer.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    JPEG Image Decoder.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _JPEGIMAGE_H
#define _JPEGIMAGE_H

/************************************************************************
 * Section:  Includes
 *******************************************************************************/
#include "system_config.h"
#include "gfx/gfx.h"
#include "gfx/gfx_image_decoder.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
// One function must be implemented in the driver:

/************************************************************************
* Function: void FunctionToReadArrayFromMemory(DWORD address, BYTE* pData, nCount)                                                                     
*
* Overview: this function reads data into buffer specified
*                                                                       
* Input: memory address, pointer to the data buffer, data count
*                                                                       
************************************************************************/ 

#ifdef IMG_SUPPORT_JPEG

    #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
        #define USE_JPEG_FLASH
    #endif
        
    #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        #define USE_JPEG_EXTERNAL
    #endif

    #ifdef SYS_FS_MEDIA_NUMBER
        #define USE_JPEG_FS_EXTERNAL
    #endif

#endif        

// JPEG file image structure for external and internal memories
// See Primitive.h for BITMAP_FLASH and BITMAP_EXTERNAL info
//    typedef IMAGE_FLASH 		JPEG_FLASH;
//    typedef IMAGE_EXTERNAL		JPEG_EXTERNAL;

// Structure with data access functions pointers
//extern IMG_FILE_SYSTEM_API  _jpegFileApi;

/*******************************************************************************
Function:       void* JPEGfopen(void* jpegImage)

Precondition:   None

Overview:       This function sets up the variables to emulate a file
                using constant data in internal flash or external memory

Input:          pointer to JPEG file image

Output:         pointer to JPEG file image
*******************************************************************************/
void                        *JPEGfopen(void *jpegImage);

/*******************************************************************************
Function:       void JPEGInit()

Precondition:   None

Overview:       This function initializes JPEG decoder

Input:          None

Output:         None
*******************************************************************************/
void                        JPEGInit(void);

#ifdef __cplusplus
    }
#endif
    
#endif // _JPEGIMAGE
