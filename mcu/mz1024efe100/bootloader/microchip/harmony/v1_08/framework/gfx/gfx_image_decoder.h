/*******************************************************************************
  Graphics Image Decoder

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_image_decoder.h

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
#ifndef __IMAGEDECODER_H__
    #define __IMAGEDECODER_H__

#include "system_config.h"
#include "system_definitions.h"
#include "gfx/gfx_types_resource.h"
#include "gfx/gfx.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
/*********************************************************************
* Overview: Typedefs of the used File System APIs
*********************************************************************/
typedef size_t (*FileRead)(void *ptr, size_t size, size_t n, void *stream);
typedef bool    (*FileSeek)(void *stream, long offset, int whence);
typedef uint32_t  (*FileTell)(void *fo);
typedef bool    (*FileFeof)(void *stream);

/*********************************************************************
* Overview: IMG_FileSystemAPI holds function pointers to the used
*           File System APIs
*********************************************************************/
typedef struct _IMG_FILE_SYSTEM_API
{
    FileRead pFread;
    FileSeek pFseek;
    FileTell pFtell;
    FileFeof pFeof;
} IMG_FILE_SYSTEM_API;

/*********************************************************************
* Overview: IMG_ImageFileFormat specifies all the supported
*           image file formats
*********************************************************************/
typedef enum _IMG_FILE_FORMAT
{
    IMG_NONE = 0,
    IMG_BMP,                /* Bitmap image */
    IMG_JPEG,               /* JPEG image */
    IMG_GIF                 /* GIF image */
} IMG_FILE_FORMAT;

/*********************************************************************
* Overview: IMG_PixelRgb holds the RGB information of a pixel in BYTES
*********************************************************************/
typedef struct _IMG_PIXEL_XY_RGB_888
{
    uint16_t X;
    uint16_t Y;
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint16_t POS;
} IMG_PIXEL_XY_RGB_888;

/*********************************************************************
* Overview: IMG_PixelOutput is a callback function which receives the
*           color information of the output pixel
*********************************************************************/
typedef void (*IMG_PIXEL_OUTPUT)(IMG_PIXEL_XY_RGB_888 *pPix);

/*************************************************************************
  Description:
    IMG_LoopCallback is a callback function which is called in every loop
    of the decoding cycle so that user application can do some maintenance
    activities                                                            
  *************************************************************************/
typedef void (*IMG_LOOP_CALLBACK)(void);

/*************************************************************************
  Description:
    IMG_FrameCompleteCallback is a callback function which is called at
    completion decoding current frame so that user application can do some 
    activities                                                            
  *************************************************************************/
typedef bool (*IMG_LOOP_FRAMECOMPLETE)(uint16_t frame);

/*************************************************************************
  Description:
    IMG_FrameCompleteCallback is a callback function which is called at
    completion decoding current frame so that user application can do some 
    activities                                                            
  *************************************************************************/
typedef void (*IMG_LOOP_IMAGECOMPLETE)(void);

/*************************************************************************
  Description:
    IMG_FrameCompleteCallback is a callback function which is called at
    completion decoding current frame so that user application can do some 
    activities                                                            
  *************************************************************************/
typedef void (*IMG_LOOP_IMAGEFAIL)(void);

/* The global variables which define the image position and size */
#ifndef __IMAGEDECODER_C__
  extern uint8_t IMG_blAbortImageDecoding;
  extern uint16_t IMG_wStartX;
  extern uint16_t IMG_wStartY;
  extern uint16_t IMG_wWidth;
  extern uint16_t IMG_wHeight;
  extern uint16_t IMG_wImageWidth;
  extern uint16_t IMG_wImageHeight;
  extern uint16_t IMG_bDownScalingFactor;
  extern uint8_t IMG_bAlignCenter;
 
 #ifndef IMG_USE_ONLY_565_GRAPHICS_DRIVER_FOR_OUTPUT
  extern IMG_PIXEL_OUTPUT IMG_pPixelOutput;
  extern IMG_PIXEL_XY_RGB_888 IMG_PixelXYColor;
 #endif

 #ifndef IMG_USE_ONLY_MDD_FILE_SYSTEM_FOR_INPUT
  extern IMG_FILE_SYSTEM_API *IMG_pFileAPIs;
 #endif
 
 #ifdef IMG_SUPPORT_IMAGE_DECODER_LOOP_CALLBACK
  extern IMG_LOOP_CALLBACK  IMG_pLoopCallbackFn;
 #endif

  extern bool IMG_pImageExitRequested;
  extern IMG_LOOP_FRAMECOMPLETE IMG_pFrameCompleteCallbackFn;
  extern IMG_LOOP_IMAGECOMPLETE IMG_pImageCompleteCallbackFn;
  extern IMG_LOOP_IMAGEFAIL IMG_pImageFailCallbackFn;
  extern uint16_t IMG_pCurrentFrame;     
  
 #define IMG_FILE         void
 #define IMG_FREAD        IMG_pFileAPIs->pFread
 #define IMG_FSEEK        IMG_pFileAPIs->pFseek
 #define IMG_FTELL        IMG_pFileAPIs->pFtell
 #define IMG_FEOF         IMG_pFileAPIs->pFeof

#ifndef __IMAGEDECODER_C__

  extern IMG_FILE *IMG_pCurrentFile;

#endif

/* The individual image decoders use these defines instead of directly using those provided in the Display driver file */
#define IMG_SCREEN_WIDTH         (GetMaxX()+1)
#define IMG_SCREEN_HEIGHT        (GetMaxY()+1)
#define _PutPixel(x, y)          if(IMG_pPixelOutput != NULL) { IMG_PixelXYColor.X = x; IMG_PixelXYColor.Y = y; IMG_pPixelOutput(&IMG_PixelXYColor); }

#endif

#define IMG_vSetColor(r, g, b)   IMG_PixelXYColor.R = r; IMG_PixelXYColor.G = g; IMG_PixelXYColor.B = b; 
  
#define IMG_vPixelArray(pos)   IMG_PixelXYColor.POS = pos; 

#define IMG_vPutPixel(x, y)      if(IMG_bDownScalingFactor <= 1)\
                                 {                          \
                                     _PutPixel(IMG_wStartX + (x), IMG_wStartY + (y));\
                                 }                          \
                                 else if((x)%IMG_bDownScalingFactor == 0 && (y)%IMG_bDownScalingFactor == 0)\
                                 {                          \
                                     _PutPixel(IMG_wStartX + ((x)/IMG_bDownScalingFactor), IMG_wStartY + ((y)/IMG_bDownScalingFactor)); \
                                 }


#ifdef IMG_SUPPORT_IMAGE_DECODER_LOOP_CALLBACK
       #define IMG_vLoopCallback() if(IMG_pLoopCallbackFn != NULL) { IMG_pLoopCallbackFn(); }
#else
       #define IMG_vLoopCallback()
#endif

#define IMG_vFrameComplete() if (IMG_pFrameCompleteCallbackFn != NULL) { IMG_pImageExitRequested = IMG_pFrameCompleteCallbackFn(++IMG_pCurrentFrame); }
#define IMG_vImageComplete() if (IMG_pImageCompleteCallbackFn != NULL) { IMG_pImageCompleteCallbackFn(); }
#define IMG_vImageFail() if (IMG_pImageFailCallbackFn != NULL) { IMG_pImageFailCallbackFn(); }
  
#define IMG_DECODE_ABORTED 0xFF
#define IMG_vCheckAndAbort()                \
        if(IMG_blAbortImageDecoding == 1)   \
        {                                   \
              IMG_blAbortImageDecoding = 0; \
              return IMG_DECODE_ABORTED;    \
        }


#ifdef IMG_SUPPORT_BMP
 #include "decoder/bmp/BmpDecoder.h"
#endif

#ifdef IMG_SUPPORT_JPEG
 #include "decoder/jpeg/JpegDecoder.h"
#endif

#ifdef IMG_SUPPORT_GIF
 #include "decoder/gif/GifDecoder.h"
#endif

/* Function prototypes */
void ImageRegisterImageCompleteCallback(void* callbackFn);
void ImageRegisterImageFailCallback(void* callbackFn);
void ImageRegisterFrameCompleteCallback(void* callbackFn);
int8_t ImageDecoderParceHeader(IMG_FILE *pImageFile, GFX_RESOURCE_HDR* header, GFX_RESOURCE source);

/********************************************************************
  Function:
     void ImageDecoderInit(void)
    
  Description:
    This function initializes the global variables to 0 and then
    initializes the driver. This must be called once before any other
    function of the library is called
  Return:
    None
  Example:
    <code lang="c++">
    void main(void)
    {
        ImageInit();
        ...
    }
    </code>
  Side Effects:
    The graphics driver will be reset                                
  ********************************************************************/
void ImageDecoderInit(void);

/*************************************************************************
  Function:
     void ImageLoopCallbackRegister(IMG_LOOP_CALLBACK pLoopCallbackFn)
    
  Description:
    This function registers the loop callback function so that the decoder
    calls this function in every decoding loop. This can be used by the
    application program to do maintenance activities such as fetching
    data, updating the display, etc...
  Return:
    None
  Example:
    <code lang="c++">
    void Mainantance(void)
    {
        ...
    }
    
    void main(void)
    {
        ImageInit();
        ImageLoopCallbackRegister(Mainantance);
        ...
    }
    </code>
  Side Effects:
    The graphics driver will be reset                                     
  *************************************************************************/
void ImageLoopCallbackRegister(IMG_LOOP_CALLBACK pFn);

/*********************************************************************
* Function: uint8_t ImageDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat, uint16_t wStartx, uint16_t wStarty, uint16_t wWidth, uint16_t wHeight, uint16_t wFlags, IMG_FILE_SYSTEM_API *pFileAPIs, IMG_PIXEL_OUTPUT pPixelOutput)
*
* Overview: This function decodes and displays the image on the screen
*
* Input: pImageFile -> The image file pointer
*        eImgFormat -> Type of image to be decoded
*        wStartx  -> Starting x position for the image to be displayed
*        wStarty  -> Starting y position for the image to be displayed
*        wWidth   -> The width into which the image to be displayed/compressed
*                    (This is not the width of the image)
*        wHeight  -> The height into which the image to be displayed/compressed
*                    (This is not the height of the image)
*        wFlags   -> If bit 0 is set, the image would be center aligned into the area
*                         specified by wStartx, wStarty, wWidth and wHeight
*                 -> If bit 1 is set, the image would be downscaled if required to fit
*                         into the area specified by wStartx, wStarty, wWidth
*                         and wHeight
*        pFileAPIs     -> The pointer to a structure which has function pointers
*                         to the File System APIs
*        pPixelOutput  -> The function to output (x, y) coordinates and the color
*
* Output: Error code -> 0 means no error
*
* Example:
*   <PRE> 
*	void main(void)
*	{
*		IMG_FILE pImageFile;
*       IMG_vInitialize();
*       pImageFile = IMG_FOPEN("Image.jpg", "r");
*       if(pImageFile == NULL)
*       {
*                 <- Error handling ->
*       }
*       IMG_bDecode(pImageFile, IMG_JPEG, 0, 0, 320, 240, 0, NULL, NULL);
*       IMG_FCLOSE(pImageFile);
*       while(1);
*	}
*	</PRE> 
*
* Side Effects: None
*
********************************************************************/
uint8_t ImageDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat,
                 uint16_t wStartx, uint16_t wStarty, uint16_t wWidth, uint16_t wHeight,
                 uint16_t wFlags, IMG_FILE_SYSTEM_API *pFileAPIs,
                 IMG_PIXEL_OUTPUT pPixelOutput);

/* Flags */
#define IMG_ALIGN_CENTER 0x0001
#define IMG_DOWN_SCALE   0x0002

void ImagePixelOutput(IMG_PIXEL_XY_RGB_888 *pPix);
void ImagePixelIntoArray(IMG_PIXEL_XY_RGB_888 *pPix);

/*************************************************************************************************************************************************
  Function:
     uint8_t ImageFullScreenDecode(IMG_FILE *pImageFile, IMG_FILE_FORMAT eImgFormat, IMG_FILE_SYSTEM_API pFileAPIs, IMG_PIXEL_OUTPUT pPixelOutput)
    
  Description:
    This function decodes and displays the image on the screen in
    fullscreen mode with center aligned and downscaled if required
  Return:
    Error code -> 0 means no error
  Example:
    <code lang="c++">
    void main(void)
    {
        IMG_FILE pImageFile;
        IMG_vInitialize();
        pImageFile = IMG_FOPEN("Image.jpg", "r");
        if(pImageFile == NULL)
        {
                  <- Error handling ->
        }
        IMG_bFullScreenDecode(pImageFile, IMG_JPEG, NULL, NULL);
        IMG_FCLOSE(pImageFile);
        while(1);
    }
    </code>
  Side Effects:
    None                                                                                                                                          
  *************************************************************************************************************************************************/
#define ImageFullScreenDecode(pImageFile, eImgFormat, pFileAPIs, pPixelOutput) \
        ImageDecode(pImageFile, eImgFormat, 0, 0, IMG_SCREEN_WIDTH, IMG_SCREEN_HEIGHT, (IMG_ALIGN_CENTER | IMG_DOWN_SCALE), pFileAPIs, pPixelOutput)

/************************************************************************
  Function:
     uint8_t ImageDecodeTask(void)
    
  Description:
    This function completes one small part of the image decode function
  Return:
    Status code - '1' means decoding is completed
      * '0' means decoding is not yet completed, call this function again
  Example:
    <code lang="c++">
        IMG_bFullScreenDecode(pImageFile, IMG_JPEG, NULL, NULL);
        while(!ImageDecodeTask());
    </code>
  Side Effects:
    None                                                                 
  ************************************************************************/
uint8_t ImageDecodeTask(void);

/*********************************************************************
  Function:
     void ImageAbort(void)
    
  Description:
    This function sets the Image Decoder's Abort flag so that decoding
    aborts in the next decoding loop.
  Return:
    None
  Example:
    <code lang="c++">
    
    void callback(void);
    void main(void)
    {
        IMG_FILE pImageFile;
        IMG_vInitialize();
        ImageLoopCallbackRegister(callback);
        pImageFile = IMG_FOPEN("Image.jpg", "r");
        if(pImageFile == NULL)
        {
                  <- Error handling ->
        }
        IMG_bFullScreenDecode(pImageFile, IMG_JPEG, NULL, NULL);
        IMG_FCLOSE(pImageFile);
        while(1);
    }
    
    void callback(void)
    {
        if(<- check for abort condition ->)
        {
            ImageAbort();
        }
    }
    
    </code>
  Side Effects:
    None                                                              
  *********************************************************************/
#define ImageAbort() IMG_blAbortImageDecoding = 1;

/********* This is not for the user *********/
/* This is used by the individual decoders */
void IMG_vSetboundaries(void);
/********* This is not for the user *********/

#ifdef __cplusplus
    }
#endif
    
#endif
