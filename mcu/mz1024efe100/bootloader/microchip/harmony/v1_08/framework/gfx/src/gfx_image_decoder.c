/*******************************************************************************
  Graphics Image Decoder

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_image_decoder.c

  Summary:
    This implements the image decoding of the primitive layer.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Image Decoder.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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
#include "system_config.h"
#include "system_definitions.h"
#include "gfx/gfx_image_decoder.h"

/**************************/
/**** GLOBAL VARIABLES ****/
/**************************/
extern DRV_GFX_INTERFACE *  gfxDrv;

IMG_FILE *IMG_pCurrentFile;

uint16_t IMG_wStartX;
uint16_t IMG_wStartY;
uint16_t IMG_wWidth;
uint16_t IMG_wHeight;
uint16_t IMG_wImageWidth;
uint16_t IMG_wImageHeight;
uint16_t IMG_bDownScalingFactor;
uint8_t IMG_bAlignCenter;
uint8_t IMG_blAbortImageDecoding;

#ifndef IMG_USE_ONLY_565_GRAPHICS_DRIVER_FOR_OUTPUT
IMG_PIXEL_OUTPUT IMG_pPixelOutput;
IMG_PIXEL_XY_RGB_888 IMG_PixelXYColor;
#endif

#ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
IMG_FILE_SYSTEM_API *IMG_pFileAPIs;
#endif

 #ifdef IMG_SUPPORT_IMAGE_DECODER_LOOP_CALLBACK
IMG_LOOP_CALLBACK  IMG_pLoopCallbackFn;
 #endif

bool IMG_pImageExitRequested;
IMG_LOOP_FRAMECOMPLETE  IMG_pFrameCompleteCallbackFn;
IMG_LOOP_IMAGECOMPLETE IMG_pImageCompleteCallbackFn;
IMG_LOOP_IMAGEFAIL IMG_pImageFailCallbackFn;
uint16_t IMG_pCurrentFrame;     

  int8_t ImageDecoderParceHeader(IMG_FILE* pImageFile, GFX_RESOURCE_HDR* header, GFX_RESOURCE source)
{
#ifdef IMG_SUPPORT_JPEG
    extern IMG_FILE_SYSTEM_API _jpegFileApi;
#endif
#ifdef IMG_SUPPORT_BMP
    extern IMG_FILE_SYSTEM_API _bmpFileApi;
#endif
#ifdef IMG_SUPPORT_GIF
	extern IMG_FILE_SYSTEM_API _gifFileApi;
#endif

    if (header == NULL)
        return -1;

    header->resource.image.location.extAddress = (uint32_t)pImageFile;
    header->type = source;
    
#ifdef IMG_SUPPORT_JPEG
    
#ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
    IMG_pFileAPIs = &_jpegFileApi;
#endif
    if (JPEG_bInterpretResourceHeader(header) == 0)
    {
        header->type = (GFX_RESOURCE)(source | GFX_RESOURCE_TYPE_JPEG | GFX_RESOURCE_COMP_NONE);        
        return 0;
    }
#endif
#ifdef IMG_SUPPORT_BMP
    
#ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
    IMG_pFileAPIs = &_bmpFileApi;
#endif
    if (BDEC_bInterpretResourceHeader(header) == 0)
    {
        header->type = (GFX_RESOURCE)(source | GFX_RESOURCE_TYPE_BMP | GFX_RESOURCE_COMP_NONE);        
        return 0;
    }
#endif
#ifdef IMG_SUPPORT_GIF

#ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
	IMG_pFileAPIs = &_gifFileApi;
#endif
	if (GIF_bInterpretResourceHeader(header) == 0)
	{
		header->type = (GFX_RESOURCE)(source | GFX_RESOURCE_TYPE_GIF | GFX_RESOURCE_COMP_NONE);
		return 0;
	}
#endif
	return -1;
}

void ImageRegisterImageCompleteCallback(void* callbackFn)
{
    IMG_pImageCompleteCallbackFn = callbackFn;
}
  
void ImageRegisterImageFailCallback(void* callbackFn)
{
    IMG_pImageFailCallbackFn = callbackFn;
}

void ImageRegisterFrameCompleteCallback(void* callbackFn)
{
    IMG_pFrameCompleteCallbackFn = callbackFn;
}

/**************************/
/*******************************************************************************
Function:       void ImageDecoderInit(void)

Precondition:   None

Overview:       This function resets the variables and initializes the display

Input:          None

Output:         None
*******************************************************************************/
void ImageDecoderInit(void)
{
     IMG_wStartX = 0;
     IMG_wStartY = 0;
     IMG_wWidth  = 0;
     IMG_wHeight = 0;
     IMG_wImageWidth = 0;
     IMG_wImageHeight = 0;

   #ifndef IMG_USE_ONLY_565_GRAPHICS_DRIVER_FOR_OUTPUT
     IMG_pPixelOutput = NULL;
     IMG_PixelXYColor.X = 0;
     IMG_PixelXYColor.Y = 0;
     IMG_PixelXYColor.R = 0;
     IMG_PixelXYColor.G = 0;
     IMG_PixelXYColor.B = 0;
   #endif

   #ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
     IMG_pFileAPIs = NULL;
   #endif

    IMG_blAbortImageDecoding = 0;

#ifdef IMG_USE_NON_BLOCKING_DECODING
    IMG_pCurrentFile = NULL;
#endif
}

/*******************************************************************************
Function:       BYTE ImageDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat, WORD wStartx, WORD wStarty, WORD wWidth, WORD wHeight, WORD wFlags, IMG_FILE_SYSTEM_API *pFileAPIs, IMG_PIXEL_OUTPUT pPixelOutput)

Precondition:   None

Overview:       This function uses the approperiate image decoding function to
                decode and display the image

Input:          File pointer, Kind of image, Image position and boundaries, If center alignment and downscaling to fit into the display is required, File System API pointer and function to output the decoded pixels

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t ImageDecodeToBuffer(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat,
                 uint16_t wStartx, uint16_t wStarty, uint16_t wWidth, uint16_t wHeight,
                 uint16_t wFlags, IMG_FILE_SYSTEM_API *pFileAPIs,
                 IMG_PIXEL_OUTPUT pPixelOutput,
                 GFX_COLOR* pixelBuffer)
{

    static uint8_t state = 0;
    static uint8_t bRetVal = 0;

    switch (state)
    {
        case 0:
            IMG_pCurrentFrame = 0;
            IMG_pImageExitRequested = false;
                    
            IMG_wStartX = wStartx;
            IMG_wStartY = wStarty;
            IMG_wWidth = wWidth;
            IMG_wHeight = wHeight;
            IMG_wImageWidth = 0;
            IMG_wImageHeight = 0;

            IMG_bDownScalingFactor = (wFlags & IMG_DOWN_SCALE) ? 1 : 0;
            IMG_bAlignCenter = (wFlags & IMG_ALIGN_CENTER) ? 1 : 0;

            #ifndef IMG_USE_ONLY_565_GRAPHICS_DRIVER_FOR_OUTPUT
            IMG_pPixelOutput = pPixelOutput;
            #endif

            #ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
            IMG_pFileAPIs = pFileAPIs;
            #endif
            state = 1;

        case 1:
            switch (eImgFormat)
            {
                #ifdef IMG_SUPPORT_BMP
                case IMG_BMP:
                    bRetVal = BMP_bDecode(pImageFile);
                    break;
                #endif
                #ifdef IMG_SUPPORT_JPEG
                case IMG_JPEG:
                    bRetVal = JPEG_bDecode(pImageFile);
                    break;
                #endif
                #ifdef IMG_SUPPORT_GIF
                case IMG_GIF:
                    bRetVal = GIF_bDecode(pImageFile);
                    break;
                #endif
                default: bRetVal = 0xFF;
            }
    }
    if (bRetVal != 0xff)
    {
        state = 0; //Reset State Machine
    }
    return (bRetVal);

}

/*******************************************************************************
Function:       BYTE ImageDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat, WORD wStartx, WORD wStarty, WORD wWidth, WORD wHeight, WORD wFlags, IMG_FILE_SYSTEM_API *pFileAPIs, IMG_PIXEL_OUTPUT pPixelOutput)

Precondition:   None

Overview:       This function uses the approperiate image decoding function to
                decode and display the image

Input:          File pointer, Kind of image, Image position and boundaries, If center alignment and downscaling to fit into the display is required, File System API pointer and function to output the decoded pixels

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t ImageDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat,
                 uint16_t wStartx, uint16_t wStarty, uint16_t wWidth, uint16_t wHeight,
                 uint16_t wFlags, IMG_FILE_SYSTEM_API *pFileAPIs,
                 IMG_PIXEL_OUTPUT pPixelOutput)
{

    static uint8_t state = 0;
    static uint8_t bRetVal = 0;

    switch (state)
    {
        case 0:
            IMG_pCurrentFrame = 0;
            IMG_wStartX = wStartx;
            IMG_wStartY = wStarty;
            IMG_wWidth = wWidth;
            IMG_wHeight = wHeight;
            IMG_wImageWidth = 0;
            IMG_wImageHeight = 0;

            IMG_bDownScalingFactor = (wFlags & IMG_DOWN_SCALE) ? 1 : 0;
            IMG_bAlignCenter = (wFlags & IMG_ALIGN_CENTER) ? 1 : 0;

            #ifndef IMG_USE_ONLY_565_GRAPHICS_DRIVER_FOR_OUTPUT
            IMG_pPixelOutput = pPixelOutput;
            #endif

            #ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
            IMG_pFileAPIs = pFileAPIs;
            #endif
            state = 1;

        case 1:
            switch (eImgFormat)
            {
                #ifdef IMG_SUPPORT_BMP
                case IMG_BMP:
                    bRetVal = BMP_bDecode(pImageFile);
                    break;
                #endif
                #ifdef IMG_SUPPORT_JPEG
                case IMG_JPEG:
                    bRetVal = JPEG_bDecode(pImageFile);
                    break;
                #endif
                #ifdef IMG_SUPPORT_GIF
                case IMG_GIF:
                    bRetVal = GIF_bDecode(pImageFile);
                    break;
                #endif
                default: bRetVal = 0xFF;
            }
    }
    if (bRetVal != 0xff)
    {
        state = 0; //Reset State Machine
    }
    return (bRetVal);

}

/*******************************************************************************
Function:       void IMG_vSetboundaries(void)

Precondition:   None

Overview:       This function sets the boundaries and scaling factor. THIS IS
                NOT FOR THE USER.

Input:          None

Output:         None
*******************************************************************************/
void IMG_vSetboundaries(void)
{
    uint16_t wXScalingFactor = 1;
    uint16_t wYScalingFactor = 1;

    if(IMG_bDownScalingFactor > 0 && IMG_wWidth > 0 && IMG_wHeight > 0)
    {
        wXScalingFactor = IMG_wImageWidth / IMG_wWidth;
        wYScalingFactor = IMG_wImageHeight / IMG_wHeight;

        if(wXScalingFactor * IMG_wWidth < IMG_wImageWidth)
        {
            if (IMG_wImageWidth - DISP_HOR_RESOLUTION > (DISP_HOR_RESOLUTION / 10) )
            {
                wXScalingFactor++;                
            }
        }
        if(wYScalingFactor * IMG_wHeight < IMG_wImageHeight)
        {
            if (IMG_wImageHeight - DISP_VER_RESOLUTION > (DISP_VER_RESOLUTION / 10) )
            {
                wYScalingFactor++;
            }
        }

        IMG_bDownScalingFactor = (wXScalingFactor > wYScalingFactor)? wXScalingFactor: wYScalingFactor;

        if(IMG_bDownScalingFactor <= 1)
        {
            IMG_bDownScalingFactor = 0;
        }
    }

    if(IMG_bAlignCenter > 0 && IMG_bDownScalingFactor > 1)
    {
        uint8_t bDownScalingFactor = (IMG_bDownScalingFactor <= 1)? 1: IMG_bDownScalingFactor;
        if(IMG_wWidth > (IMG_wImageWidth / bDownScalingFactor))
        {
            if (bDownScalingFactor > 3)
            {
                IMG_wStartX += (IMG_wWidth - (IMG_wImageWidth / bDownScalingFactor)) >> 1;                
            }
            else
            {
                IMG_wStartX += (IMG_wWidth - (IMG_wImageWidth / bDownScalingFactor)) / 3;                                
            }
        }
        if(IMG_wHeight > (IMG_wImageHeight / bDownScalingFactor))
        {
            if (bDownScalingFactor > 3)
            {
                IMG_wStartY += (IMG_wHeight - (IMG_wImageHeight / bDownScalingFactor)) >> 1;
            }
            else
            {
                IMG_wStartY += (IMG_wHeight - (IMG_wImageHeight / bDownScalingFactor)) / 3;                
            }
        }
    }
}

void ImagePixelOutput(IMG_PIXEL_XY_RGB_888 *pPix)
{
    if (pPix->X < GFX_MaxXGet(GFX_INDEX_0) && pPix->Y < GFX_MaxYGet(GFX_INDEX_0))
    {
        GFX_ColorSet(GFX_INDEX_0, GFX_RGBConvert(pPix->R, pPix->G, pPix->B));
        DRV_GFX_PixelPut(pPix->X,pPix->Y);
    }    
}

void ImagePixelIntoArray(IMG_PIXEL_XY_RGB_888 *pPix)
{
    GFX_COLOR* pixelArray = GFX_RenderingBufferGet(GFX_INDEX_0);
 
    if (IMG_bDownScalingFactor <= 1)
    {
        if (pPix->X < GFX_MaxXGet(GFX_INDEX_0) && pPix->Y < GFX_MaxYGet(GFX_INDEX_0))
        {
            pixelArray[pPix->POS] = GFX_RGBConvert(pPix->R, pPix->G, pPix->B);
        }        
    }
    else
    {
        ImagePixelOutput(pPix);
    }
}