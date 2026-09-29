/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Primitive Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_primitive.c

  Summary:
    Primitive Layer of the Microchip Graphics Library.

  Description:
    This module implements the primitive rendering routines of the Microchip
    Graphics Library. The routines are independent of the Display Driver Layer
    and should be compatible with any Display Driver that is compliant with
    the requirements of the Display Driver Layer of the Graphics Library.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "system_config.h"
#include "system_definitions.h"


/*DOM-IGNORE-BEGIN*/
// *****************************************************************************
// The following are internal variables and macros.
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Global Constants
// *****************************************************************************
// *****************************************************************************

#define COSINETABLEENTRIES	90

// Cosine table used to calculate angles when rendering circular objects and  arcs
// Make cosine values * 256 instead of 100 for easier math later
const int16_t   _CosineTable[COSINETABLEENTRIES+1] __attribute__((aligned(2))) =
		{
                    256, 256, 256, 256, 255, 255, 255, 254, 254, 253,
                    252, 251, 250, 249, 248, 247, 246, 245, 243, 242,
                    241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
                    222, 219, 217, 215, 212, 210, 207, 204, 202, 199,
                    196, 193, 190, 187, 184, 181, 178, 175, 171, 168,
                    165, 161, 158, 154, 150, 147, 143, 139, 136, 132,
                    128, 124, 120, 116, 112, 108, 104, 100, 96,  92,
                    88,  83,  79,  75,  71,  66,  62,  58,  53,  49,
                    44,  40,  36,  31,  27,  22,  18,  13,  9,   4,
                    0
                };

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef union
{
    uint8_t  uint328BitValue[4];
    uint16_t uint3216BitValue[2];
    uint32_t uint32Value;
}PRIMITIVE_UINT32_UNION;

DRV_GFX_INTERFACE *           gfxDrv;

// *****************************************************************************
/*  PRIMITIVE HAL Module Client Status

  Summary:
    Enumerated data type that identifies the PRIMITIVE HAL Module Client Status.

  Description:
    This enumeration defines the possible status of the  HAL Module Client.
    It is returned by the () function.

  Remarks:
    None.
*/

typedef enum
{
     /* Client is closed or the specified handle is invalid */
    GFX_PRIMITIVE_STATUS_CLOSED
            /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

    /* Client is ready */
    GFX_PRIMITIVE_STATUS_READY
            /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/

} GFX_PRIMITIVE_CLIENT_STATUS;

// *****************************************************************************
/*  GFX PRIMITIVE HAL client object structure

  Summary:
    GFX PRIMITIVE HAL client object structure.

  Description:
    GFX PRIMITIVE HAL client object structure.

  Remarks:
   None.
*/

typedef struct
{
    /* Client status */
    GFX_PRIMITIVE_CLIENT_STATUS         clientState;

    /* Set to true if this object is in use */
    bool                          inUse;

    /* Object from which the client is derived */
    void *                        obj;

    DRV_GFX_INTERFACE            drvInterface;

    /* Save the index of the driver */
    SYS_MODULE_INDEX              halIndex;

    DRV_GFX_HANDLE                drvHandle;

    GFX_LINE_STYLE       gfxLineStyle;               // line style (see GFX_LINE_STYLE)
    GFX_FILL_STYLE       gfxFillStyle;               // fill style (see GFX_FILL_STYLE)

    GFX_COLOR            gfxGradientStartColor;      // gradient fill start color
    GFX_COLOR            gfxGradientEndColor;        // gradient fill end color
    GFX_COLOR            gfxColor;                   // foreground color for rendering shapes

    int16_t              gfxLineCursorX;
    int16_t              gfxLineCursorY;

    GFX_COLOR            gfxColorTransparent;        // transparent color
    GFX_FEATURE_STATUS   gfxColorTransparentFlag;    // transparent color feature flag

    GFX_RESOURCE_HDR     *pGfxCurrentFont;           // current active font
    int16_t              gfxTextPositionX;           // used only internally to locate where the character will be rendered in the x coordinate
    int16_t              gfxTextPositionY;           // used only internally to locate where the character will be rendered in the y coordinate
    
    GFX_TEXT_CURSOR_TYPE gfxTextCursorType;
    
#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
    // text foreground colors for use with anti-aliased fonts
    GFX_COLOR            gfx_TextForegroundColor100;
    GFX_COLOR            gfx_TextForegroundColor25;
    GFX_COLOR            gfx_TextForegroundColor75;
    // text background colors for use with anti-aliased fonts
    GFX_COLOR            gfx_TextBackgroundColor100;
    GFX_COLOR            gfx_TextBackgroundColor25;
    GFX_COLOR            gfx_TextBackgroundColor75;
    GFX_FONT_ANTIALIAS_TYPE gfxTextAntiAliasType;    // Font anti-aliasing transparency type (see GFX_FONT_ANTIALIAS_TYPE)

#endif

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    uint8_t              gfxAlphaValue;              // alpha value used when rendering with alpha blending
#endif

    // allocate buffer for rendering and/or retrieval of arrays of pixels
    // used primarily for put image and alpha blending functions.
#if (DISP_VER_RESOLUTION > DISP_HOR_RESOLUTION)
    GFX_COLOR            gfxLineBuffer0[DISP_VER_RESOLUTION];
    GFX_COLOR            alphaLine[DISP_VER_RESOLUTION];
#else
    GFX_COLOR            gfxLineBuffer0[DISP_HOR_RESOLUTION];
    GFX_COLOR            alphaLine[DISP_HOR_RESOLUTION];
#endif

    // parameters for the background information used
    GFX_BACKGROUND       gfxBackground;
    uint16_t             gfxImageDrawRenderDisableFlag;

    // bevel drawing type (0 = full bevel, 0xF0 - top bevel only, 0x0F - bottom bevel only
    GFX_BEVEL_RENDER_TYPE gfxBevelDrawType;

    // The following are internally used for the primitive text box
    // function. These defines the window where text rendering will
    // be allowed. Values of these are set by the GFX_TextStringBoxDraw()
    // function. Since GFX_TextStringBoxDraw() calls the GFX_TextCharDraw()
    // to render each character, GFX_TextCharDraw() must know if the character
    // is within the clipping region.
    uint16_t             gfxTextClipLeft;            // used internally to define the left limit for text rendering
    int16_t              gfxTextClipTop;             // used internally to define the top limit for text rendering
    uint16_t             gfxTextClipRight;           // used internally to define the right limit for text rendering
    uint16_t             gfxTextClipBottom;          // used internally to define the bottom limit for text rendering

    // defines the bounding rectangle for all pixels
    GFX_COORD            gfxClipX;
    GFX_COORD            gfxClipY;
    GFX_COORD            gfxClipW;
    GFX_COORD            gfxClipH;

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
    // double buffering feature variable
    GFX_DOUBLE_BUFFERING_MODE   gfxDoubleBufferParam;
#endif

#ifndef GFX_CONFIG_PALETTE_DISABLE
    void    *_palette;
#endif
    uint8_t                       activePage;
    uint8_t                       visualPage;
    uint8_t                       transitionPage;
    uint8_t                       foregroundPage;   //Initialize Alpha Values
    uint8_t                       backgroundPage;
    uint8_t                       destinationPage;
    GFX_ALPHA_PARAMS              alphaArea;
    uint8_t                       InvalidatedRectangleCount;
    uint8_t                       doubleBuffEnabled;

} GFX_PRIMITIVE_CLIENT_OBJ;


// *****************************************************************************
/* LCC Driver task states

  Summary
    Lists the different states that LCC task routine can have.

  Description
    This enumeration lists the different states that LCC task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    GFX_PRIMITIVE_INITIALIZE_DONE

} GFX_PRIMITIVE_OBJECT_TASK;

// *****************************************************************************
/* GFX PRIMITIVE Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the primitive hal instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            halIndex;

    /* LCC machine state */
    GFX_PRIMITIVE_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    GFX_PRIMITIVE_CLIENT_OBJ *                        pDrvLCCClientObj;

    /* State of the task */
    GFX_PRIMITIVE_OBJECT_TASK    		        task;

    GFX_INIT *                              initData;

} GFX_PRIMITIVE_OBJ;

static GFX_PRIMITIVE_OBJ        primitiveObj[GFX_INDEX_COUNT];

static GFX_PRIMITIVE_CLIENT_OBJ primitiveClient[GFX_INDEX_COUNT];

// *****************************************************************************
/* Primitive/Driver Shared Object protection
 */


// *****************************************************************************
// *****************************************************************************
// Section: For Build Erorr Checking
// *****************************************************************************
// *****************************************************************************
#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
#ifndef GFX_CONFIG_PALETTE_DISABLE
    #error "Antialiasing can not be used when Palette is enabled"
#endif
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros and Functions
// *****************************************************************************
// *****************************************************************************

/////////////////////// LOCAL FUNCTIONS PROTOTYPES ////////////////////////////
GFX_STATUS GFX_TextCharRender(  SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam);

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
    void GFX_ImageFlash1BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageFlash4BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageFlash8BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageFlash16BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);

#ifndef GFX_CONFIG_RLE_DECODE_DISABLE
    uint16_t GFX_ImageFlashRLE8BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixelrow,
                                uint16_t size);
    uint16_t GFX_ImageFlashRLE4BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixelrow,
                                uint16_t size);
#endif // end of #ifndef GFX_CONFIG_RLE_DECODE_DISABLE
#endif // end of #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
    void GFX_ImageExternal1BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageExternal4BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageExternal8BPPDraw (
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageExternal16BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);

#ifndef GFX_CONFIG_RLE_DECODE_DISABLE
    uint16_t GFX_ImageFlashExternal8BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size);
    uint16_t GFX_ImageFlashExternal4BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size);
#endif // end of #ifndef GFX_CONFIG_RLE_DECODE_DISABLE
#endif // end of #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

#ifndef GFX_CONFIG_RLE_DECODE_DISABLE
    void GFX_ImageRLE8BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    void GFX_ImageRLE4BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);
    uint32_t GFX_RLEBlockFind(
                                GFX_RESOURCE_HDR *image,
                                uint16_t size,
                                uint16_t height,
                                uint16_t rleType);
#endif // end of #ifndef GFX_CONFIG_RLE_DECODE_DISABLE

    uint16_t GFX_TextStringWidthRamGet(
                                GFX_XCHAR* textString,
                                GFX_RESOURCE_HDR* pFont);
    uint16_t GFX_TextStringWidthFlashGet(
                                GFX_XCHAR* textString,
                                GFX_RESOURCE_HDR *pFont);
    uint16_t GFX_TextStringWidthExternalGet(
                                GFX_XCHAR* textString,
                                GFX_RESOURCE_HDR* pFont);

    GFX_STATUS GFX_BevelFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2,
                                uint16_t rad);


/*DOM-IGNORE-END*/

// *****************************************************************************
/*  Function:
    GFX_COLOR *GFX_RenderingBufferGet(void)

    Summary:
        This is an internal function used to retrieve the common rendering
        buffer address.
 
    Description:
        This function is an internal function and should not be exposed
        or called by the application. This function returns the address of
        the internal rendering buffer used in selected rendering functions.
 
*/
// *****************************************************************************
inline GFX_COLOR *GFX_RenderingBufferGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxLineBuffer0);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RenderStatusCheck(void)

    Summary:
        This is an internal function used to check for rendering status. 

    Description:
        This function is an internal function and should not be exposed
        or called by the application. This function checks for rendering 
        status. Since this function calls GFX_RenderStatusGet() that
        returns GFX_STATUS_BUSY_BIT or GFX_STATUS_READY only, then the
        function will also be returning only these two.

        For non-blocking implementation it returns GFX_STATUS_BUSY_BIT
        when rendering is ongoing or GFX_STATUS_READY when rendering
        is idle.
        For blocking implementation, it will always return GFX_STATUS_READY
        since it will wait for the status to be ready.
 
*/
// *****************************************************************************
inline GFX_STATUS_BIT GFX_RenderStatusCheck(SYS_MODULE_INDEX gfxIndex)
{
#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
    return (GFX_STATUS_READY_BIT);
#else
    return(GFX_RenderStatusGet(gfxIndex));
#endif
}

// *****************************************************************************
/*
    Function:
        void GFX_PRIMITIVE_Initialize (const SYS_MODULE_INDEX   moduleIndex,
                                 const SYS_MODULE_INIT    * const moduleInit);

    Summary:
        Initialize the GFX HAL Library.
*/
// *****************************************************************************
SYS_MODULE_OBJ  GFX_PRIMITIVE_Initialize(const SYS_MODULE_INDEX   index,
                                   const SYS_MODULE_INIT    * const init)
{
    /* Validate the driver index */
    if ( index >= GFX_INDEX_COUNT )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    GFX_PRIMITIVE_OBJ *obj = &primitiveObj[index];

    /* Object is valid, set it in use */
    obj->inUse = true;
    obj->state = SYS_STATUS_BUSY;
    obj->initData = (GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    obj->halIndex = index;

    //obj->initData->drvInitialize(DRV_GFX_INDEX_0, (SYS_MODULE_INIT*)&obj->initData->driverInitData);

    obj->task = GFX_PRIMITIVE_INITIALIZE_DONE;

    obj->nClients = 0;

    obj->status = SYS_STATUS_READY;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)obj;
}

// *****************************************************************************
/* Function:
    void GFX_PRIMITIVE_Deinitialize(SYS_MODULE_OBJ gfxObject)

  Summary:
    Dynamic impementation of GFX_PRIMITIVE_Deinitialize system interface function.

  Description:
    This is the dynamic impementation of GFX_PRIMITIVE_Deinitialize system interface
    function.

  Remarks:
*/

void GFX_PRIMITIVE_Deinitialize ( SYS_MODULE_OBJ gfxObject )
{
    /* Host object */
    GFX_PRIMITIVE_OBJ *obj = (GFX_PRIMITIVE_OBJ *)gfxObject;

    /* Validate the USB Host index */
    if(gfxObject == SYS_MODULE_OBJ_INVALID)
    {
        return;
    }

    /* Validate Host is already assigned */
    if(!obj->inUse)
    {
        return;
    }

    /* Indicate that this object is not is use */
    obj->inUse = false;

}

// *****************************************************************************
/*
  Function: GFX_PRIMITIVE_HANDLE GFX_PRIMITIVE_Open( const SYS_MODULE_INDEX index, DRV_GFX_HANDLE drvHandle )

  Summary:
    opens an instance of the hal
*/
GFX_PRIMITIVE_HANDLE GFX_PRIMITIVE_Open( const SYS_MODULE_INDEX index )
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[index];

    /* Check if the specified index is in valid range */
    if(index >= GFX_INDEX_COUNT)
    {
    }
    /* Check if instance object is ready*/
    else if(primitiveObj[index].status != SYS_STATUS_READY)
    {
    }
    else if(primitiveObj[index].nClients > 0)
    {
    }
    else
    {

        DRV_GFX_HANDLE drvHandle = primitiveObj[index].initData->drvOpen(index, DRV_IO_INTENT_EXCLUSIVE);
        client->drvInterface.BarFill = NULL;
        client->drvInterface.PageSet = NULL;
        client->drvInterface.Layer = NULL;
        client->drvInterface.PixelGet = NULL;
        client->drvInterface.AlphaBlendWindow = NULL;
        client->drvInterface.MaxXGet = NULL;
        client->drvInterface.MaxYGet = NULL;
        primitiveObj[index].initData->drvInterfaceSet(drvHandle, &client->drvInterface );
        gfxDrv = &client->drvInterface;
        client->inUse = true;
        client->obj = &primitiveObj[index];
        client->doubleBuffEnabled = 0;
        
        // Current line type
        GFX_LineStyleSet(index, GFX_LINE_STYLE_THIN_SOLID);
        GFX_FillStyleSet(index, GFX_FILL_STYLE_COLOR);
        GFX_GradientColorSet(index, 0, 0xFF);

        // Current cursor coordinates to 0,0
        GFX_LinePositionSet(index, 0, 0);

        // set the transparent color check to be disabled
    #ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        GFX_TransparentColorDisable(index);
    #endif

    #ifndef GFX_CONFIG_ALPHABLEND_DISABLE
        GFX_AlphaBlendingValueSet(index, 100);
    #endif

        // initialize the background
        GFX_BackgroundSet(index, 0, 0, NULL, 0);

        // initialize the PutImage() render disable flag
        GFX_RenderToDisplayBufferEnable(index);

        // Initialize the text region (used only by GFX_TextStringBoxDraw()
        GFX_TextAreaLeftSet(index, 0);
        GFX_TextAreaTopSet(index, 0);
        GFX_TextAreaRightSet(index, GFX_MaxXGet(index));
        GFX_TextAreaBottomSet(index, GFX_MaxYGet(index));

        // set Bevel drawing
        GFX_BevelDrawTypeSet(index, GFX_DRAW_FULLBEVEL);
  
        /* Increment the client number for the specific driver instance*/
        primitiveObj[index].nClients++;
    }

     /* Return invalid handle */
    return ((GFX_PRIMITIVE_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void GFX_PRIMITIVE_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the hal
*/
void GFX_PRIMITIVE_Close( GFX_PRIMITIVE_HANDLE handle )
{
    /* Start of local variable */
    GFX_PRIMITIVE_CLIENT_OBJ * client = (GFX_PRIMITIVE_CLIENT_OBJ *)NULL;
    GFX_PRIMITIVE_OBJ * obj = (GFX_PRIMITIVE_OBJ *)NULL;

    /* Check if the handle is valid */
    if(handle == GFX_PRIMITIVE_HANDLE_INVALID)
    {
//        SYS_DEBUG(0, "Bad Client Handle");
    }
    else
    {
        client = (GFX_PRIMITIVE_CLIENT_OBJ *)handle;

        if(client->inUse)
        {
            client->inUse = false;
            obj = (GFX_PRIMITIVE_OBJ *)client->obj;

            /* Remove this client from the driver client table */
            obj->nClients--;
        }
        else
        {
//            SYS_DEBUG(0, "Client Handle no inuse");
        }
    }
}

// *****************************************************************************
/*  Function:
    void GFX_ColorSet(GFX_COLOR color)

    Summary:
        This function sets the color to be used in rendering primitive
        shapes and text.

    Description:
        This function sets the color to be used to render primitive shapes
        and text. Any primitive shape that is set to any of the following:
        GFX_LINE_STYLE
        - GFX_LINE_STYLE_THIN_SOLID
        - GFX_LINE_STYLE_THIN_DOTTED
        - GFX_LINE_STYLE_THIN_DASHED
        - GFX_LINE_STYLE_THICK_SOLID
        - GFX_LINE_STYLE_THICK_DOTTED
        - GFX_LINE_STYLE_THICK_DASHED
        GFX_FILL_STYLE
        - GFX_FILL_STYLE_COLOR
        will be rendered using the set color.

        Rendering text using the GFX_TextCharDraw(), GFX_TextStringDraw()
        and GFX_TextStringBoxDraw() will also use the set color.

*/
// *****************************************************************************
inline void GFX_ColorSet(
                                                      SYS_MODULE_INDEX gfxIndex,
                                                      GFX_COLOR newColor)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    gfxDrv->ColorSet(newColor);
    client->gfxColor = newColor;

}

// *****************************************************************************
/*  Function:
    void GFX_ColorGet(GFX_COLOR color)

    Summary:
        This function returns the color currently set to render primitive
        shapes and text.

    Description:
        This function returns the color currently set to render primitive
        shapes and text (See GFX_ColorSet() for more information).

*/
// *****************************************************************************
inline GFX_COLOR GFX_ColorGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxColor);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS TransparentColorEnable(
                                GFX_COLOR color)

    Summary:
        This function sets the transparent color used in GFX_ImageDraw()
        functions and enables the transparent color feature.

    Description:
        This function sets the transparent color used in GFX_ImageDraw()
        functions and enables the transparent color feature.

        When GFX_ImageDraw() or GFX_ImagePartialDraw(gfxIndex, ) is called, any
        pixels in the image that matches the color value will not be
        rendered to the frame buffer.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

*/
// *****************************************************************************
inline GFX_STATUS GFX_TransparentColorEnable(
                                    SYS_MODULE_INDEX gfxIndex,
                                GFX_COLOR color)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxColorTransparent     = color;
    client->gfxColorTransparentFlag = GFX_FEATURE_ENABLED;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_TransparentColorDisable(void)

    Summary:
        This function disables the transparent color feature used 
        in GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

    Description:
        This function disables the transparent color feature used in 
        GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

*/
// *****************************************************************************
inline GFX_STATUS GFX_TransparentColorDisable(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxColorTransparentFlag = GFX_FEATURE_DISABLED;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_TransparentColorGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the current transparent color set for the 
        transparent color feature used in GFX_ImageDraw() and 
        GFX_ImagePartialDraw() functions.

    Description:
        This returns the current transparent color set for the 
        transparent color feature used in GFX_ImageDraw() and 
        GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

*/
// *****************************************************************************
inline GFX_COLOR GFX_TransparentColorGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxColorTransparent);
}

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS GFX_TransparentColorStatusGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the current state of the transparent color feature
        used in GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

    Description:
        This returns the current transparent color set for the
        transparent color feature used in GFX_ImageDraw() and
        GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

*/
// *****************************************************************************
inline GFX_FEATURE_STATUS GFX_TransparentColorStatusGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxColorTransparentFlag);
}

// *****************************************************************************
/*  Function:
    void GFX_TextAreaLeftSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                    uint16_t left)

    Summary:
        This sets the minimum horizontal limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the minimum horizontal limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to left + 1.

*/
// *****************************************************************************
inline void GFX_TextAreaLeftSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                    uint16_t left)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextClipLeft = left;
}

// *****************************************************************************
/*  Function:
    void GFX_TextAreaTopSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                    uint16_t top)

    Summary:
        This sets the minimum vertical limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the minimum vertical limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to top + 1.

*/
// *****************************************************************************
inline void GFX_TextAreaTopSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                    uint16_t top)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextClipTop = top;
}

// *****************************************************************************
/*  Function:
    void GFX_TextAreaRightSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                    uint16_t right)

    Summary:
        This sets the maximum horizontal limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the maximum horizontal limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to right - 1.

*/
// *****************************************************************************
inline void GFX_TextAreaRightSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                     uint16_t right)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextClipRight = right;
}

// *****************************************************************************
/*  Function:
    void GFX_TextAreaBottomSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                uint16_t bottom)

    Summary:
        This sets the maximum vertical limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the maximum vertical limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to bottom - 1.

*/
// *****************************************************************************
inline void GFX_TextAreaBottomSet(
                                    SYS_MODULE_INDEX gfxIndex,
                                uint16_t bottom)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextClipBottom = bottom;
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextAreaLeftGet(
                                    SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the currently set minimum horizontal limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set minimum horizontal limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to left + 1.

*/
// *****************************************************************************
inline uint16_t GFX_TextAreaLeftGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxTextClipLeft);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextAreaTopGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the currently set minimum vertical limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set minimum vertical limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to top + 1.

*/
// *****************************************************************************
inline uint16_t GFX_TextAreaTopGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxTextClipTop);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextAreaRightGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the currently set maximum horizontal limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set maximum horizontal limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to right - 1.

*/
// *****************************************************************************
inline uint16_t GFX_TextAreaRightGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxTextClipRight);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextAreaBottomGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This returns the currently set maximum vertical limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set maximum vertical limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to bottom - 1.

*/
// *****************************************************************************
inline uint16_t GFX_TextAreaBottomGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxTextClipBottom);
}


#ifndef GFX_CONFIG_ALPHABLEND_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_AlphaBlendingValueSet(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t alpha)

    Summary:
        This function sets the alpha value for alpha blending rendering.

    Description:
        This function sets the alpha value for alpha blending rendering.
        Accepted values are dependent on the used alpha blending routines
        at build time. When using the software routines in the Primitive
        Layer, accepted values are 0, 25, 50, 75 and 100.

        If using a specific implementation in the display driver used,
        implementation may support the full range (0-100). When this is
        the case, refer to the driver alpha blending solution for details.

        Function operation will ignore unsupported values of alpha.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_AlphaBlendingValueSet(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t alpha)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxAlphaValue = alpha;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_AlphaBlendingValueGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the current alpha value set for
        alpha blending rendering.

    Description:
        This function returns the current alpha value set for
        alpha blending rendering. See GFX_AlphaBlendingValueSet() for
        details of alpha blending values.

*/
// *****************************************************************************
uint16_t __attribute__ ((weak)) GFX_AlphaBlendingValueGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxAlphaValue);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BarAlphaDraw(gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a alpha blended bar shape using the
        currently set fill style and color.

    Description:
        This function renders an alpha blended bar shape with the
        currently set color and the currently set background type
        (see GFX_BACKGROUND_TYPE).

        The parameters left, top, right bottom will define the
        shape dimension.

        When fill style is set to GFX_FILL_STYLE_ALPHA_COLOR, the bar
        can also be rendered with an option to select the type of
        background.
        GFX_BACKGROUND_NONE - the bar will be rendered with no alpha blending.
        GFX_BACKGROUND_COLOR - the bar will be alpha blended with the
                               currently set background color.
        GFX_BACKGROUND_IMAGE - the bar will be alpha blended with the
                               currently set background image.
        GFX_BACKGROUND_DISPLAY_BUFFER - the bar will be alpha blended
                               with the current contents of the frame buffer.

        The background type is set by the GFX_BackgroundTypeSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_BarAlphaDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{

    int16_t             bgxOffset, bgyOffset;
    uint16_t            x, y, width, fgFromBuffer;
    GFX_COLOR           fgPixel, bgPixel = 0x00;
    GFX_COLOR           *pBuffer = NULL, *pPixel = NULL;
    GFX_RESOURCE_HDR    *pImage = NULL;

    if (GFX_AlphaBlendingValueGet(gfxIndex) == 0)
    {
        // if alpha is 0, then do nothing
        return (GFX_STATUS_SUCCESS);
    }
    else if (GFX_AlphaBlendingValueGet(gfxIndex) != 100)
    {
        bgxOffset = 0;
        bgyOffset = 0;
    
        switch (GFX_BackgroundTypeGet(gfxIndex))
        {
            case GFX_BACKGROUND_COLOR:
                bgPixel = GFX_BackgroundColorGet(gfxIndex);
                pBuffer = GFX_RenderingBufferGet(gfxIndex);
                fgFromBuffer = 0;
                break;
            case GFX_BACKGROUND_IMAGE:
                pImage = GFX_BackgroundImageGet(gfxIndex);
                // note that the assumption here is that the background image is larger and 
                // the bar location is within the image on the screen.
                bgxOffset =  left - GFX_BackgroundImageLeftGet(gfxIndex);
                bgyOffset =  top  - GFX_BackgroundImageTopGet(gfxIndex);
                pBuffer = GFX_RenderingBufferGet(gfxIndex);
                fgFromBuffer = 1;
                break;
            case GFX_BACKGROUND_DISPLAY_BUFFER:
                pBuffer = GFX_RenderingBufferGet(gfxIndex);
                fgFromBuffer = 1;
                break;
            case GFX_BACKGROUND_NONE:
            default: 
                bgPixel = 0x00;
                fgFromBuffer = 0;
                break;
        }
 
        // pre calculate the foreground color since this is faster
        switch(GFX_AlphaBlendingValueGet(gfxIndex))
        {
            case 50:
                fgPixel = GFX_Color50Convert(GFX_ColorGet(gfxIndex));
                break;
            case 25:
                fgPixel = GFX_Color25Convert(GFX_ColorGet(gfxIndex));
                break;
            case 75:
                fgPixel = GFX_Color75Convert(GFX_ColorGet(gfxIndex));
                break;
            default:
                fgPixel = 0;
                break;
        }

        width   = (uint16_t)(right - left) + 1;

        // process one line at a time to perform alpha blending 
        for(y = top; y <= bottom; y++)
        {

            switch (GFX_BackgroundTypeGet(gfxIndex))
            {
                case GFX_BACKGROUND_COLOR:
                    // do nothing, background pixel is already set
                    break;
                case GFX_BACKGROUND_IMAGE:
                    // GFX_ImagePartialDraw() will renders the current line of the image into the buffer (gfxLineBuffer0)
                    GFX_RenderToDisplayBufferDisable(gfxIndex);
                    GFX_ImagePartialDraw(
                            gfxIndex,
                            left, y,
                            bgxOffset, bgyOffset,
                            width, 1,
                            pImage);
                    GFX_RenderToDisplayBufferEnable(gfxIndex);
                    bgyOffset++; 
                    break;
                case GFX_BACKGROUND_DISPLAY_BUFFER:
                    // grab the current line from the display buffer
                    GFX_PixelArrayGet(gfxIndex, left, y, pBuffer, width);
                    break;
                case GFX_BACKGROUND_NONE:
                default: 
                    // do nothing here
                    break;
            }

            pPixel = pBuffer;
            for(x = 0; x < width; x++)
            {
                if (fgFromBuffer)
                {
                    bgPixel = *pPixel;
                }

                switch(GFX_AlphaBlendingValueGet(gfxIndex))
                {
                    case 50: 
                        *pPixel = GFX_Color50Convert(bgPixel);
                        break;
                    case 25: 
                        *pPixel = GFX_Color75Convert(bgPixel);
                        break;
                    case 75: 
                        *pPixel = GFX_Color25Convert(bgPixel);
                        break;
                    default: 
                        // unsupported alpha level don't do anything
                        return 1;
                } 
                *pPixel += fgPixel;
                pPixel++;
            } // end of for loop...

            GFX_PixelArrayPut(
                gfxIndex,
                left,
                y,
                pBuffer,
                width, 1);
        }   
    }
    return (GFX_STATUS_SUCCESS);

}
#endif //#indef GFX_CONFIG_ALPHABLEND_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BarDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a bar shape using the currently set fill
        style and color.

    Description:
        This function renders a bar shape with the currently set
        fill style (See GFX_FillStyleGet() and GFX_FillStyleSet() for
        details of fill style):
        - solid color - when the fill style is set to
                        GFX_FILL_STYLE_COLOR
        - alpha blended fill - when the fill style is set to
                        GFX_FILL_STYLE_ALPHA_COLOR.

        Any other selected fill style will be ignored and will assume
        a solid color fill will be used. The parameters left, top, right
        bottom will define the shape dimension.

        When fill style is set to GFX_FILL_STYLE_ALPHA_COLOR, the bar
        can also be rendered with an option to select the type of
        background.
        GFX_BACKGROUND_NONE - the bar will be rendered with no alpha blending.
        GFX_BACKGROUND_COLOR - the bar will be alpha blended with the
                               currently set background color.
        GFX_BACKGROUND_IMAGE - the bar will be alpha blended with the
                               currently set background image.
        GFX_BACKGROUND_DISPLAY_BUFFER - the bar will be alpha blended
                               with the current contents of the frame buffer.

        The background type is set by the GFX_BackgroundTypeSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_BarDraw(
                               SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{

    if (GFX_RenderStatusCheck(gfxIndex) == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    if (GFX_FillStyleGet(gfxIndex) == GFX_FILL_STYLE_ALPHA_COLOR)
    {
        // NOTE: Alpha is never set to 0 so no need to check
        if(GFX_AlphaBlendingValueGet(gfxIndex) != 100)
        {
            return (GFX_BarAlphaDraw(gfxIndex, left,top,right,bottom));
        }
    }
    // if alpha = 100, then just perform the normal Bar() rendering
    // since pixel colors will be replaced by the GFX_ColorSet()
#endif
    {
        gfxDrv->BarFill(left,top,right,bottom);
    }
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LineStyleSet(
                                GFX_LINE_STYLE style)

    Summary:
        Set the line style to be used when rendering lines.

    Description:
        This function sets the line style to be used (see GFX_LINE_STYLE)
        when rendering lines. All calls to GFX_LineDraw() will use
        the line style set by this function. In addition, all unfilled shapes
        that does specify the line style to be used will use the line
        style specified by this function.

*/
// *****************************************************************************
inline GFX_STATUS GFX_LineStyleSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_LINE_STYLE style)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxLineStyle = style;
    return GFX_STATUS_SUCCESS;
}

// *****************************************************************************
/*  Function:
    GFX_LINE_STYLE GFX_LineStyleGet(void)

    Summary:
        Return the line style used when rendering lines.

    Description:
        This function returns the line style used (see GFX_LINE_STYLE)
        when rendering lines.

*/
// *****************************************************************************
inline GFX_LINE_STYLE GFX_LineStyleGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxLineStyle;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function renders a line from x1,y1 to x2,y2 using the currently
        set line style (see GFX_LineStyleSet()).

    Description:
        This function renders a line from x1,y1 to x2,y2 using the currently
        set line style set by GFX_LineStyleSet(). The color used is the
        color set by the last call to GFX_ColorSet().

        If x1,y1 and/or x2,y2 is not on the frame buffer, then the behavior
        is undefined. If color is not set, before this function
        is called, the output is undefined.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_LineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)
{

    GFX_STATUS status;

    if ((GFX_LineStyleGet(gfxIndex) & GFX_LINE_TYPE_MASK) == 0)
        status = GFX_SolidLineDraw(gfxIndex, x1, y1, x2, y2);
    else
        status = GFX_StyledLineDraw(gfxIndex, x1, y1, x2, y2);

    if (status == GFX_STATUS_SUCCESS)
        GFX_LinePositionSet(gfxIndex, x2, y2);

    return status;

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LineToRelativeDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t dX,
                                int16_t dY)

    Summary:
        This function renders a line from current line cursor position (x,y)  
        to (x+dX,y+dY) using the currently set line style (see GFX_LineStyleSet()).

    Description:
        This function renders a line from current line cursor position (x,y)
        to (x+dX,y+dY) using the currently set line style set by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet(). Note that the parameters dX and dY are signed
        integers. This allows the line to be drawn from the line cursor
        to any direction.

        If (x+dX) and/or (y+dY) results in a position that is not on the
        frame buffer, then the behavior is undefined. If color is not set,
        before this function is called, the output is undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_LineToRelativeDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t dX,
                                int16_t dY)
{
    return (GFX_LineDraw(gfxIndex,    GFX_LinePositionXGet(gfxIndex),
                            GFX_LinePositionYGet(gfxIndex),
                            GFX_LinePositionXGet(gfxIndex) + dX,
                            GFX_LinePositionYGet(gfxIndex) + dY));
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LineToDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t x2,
                                int16_t y2)

    Summary:
        This function renders a line from current line cursor position (x,y)
        to (x2,y2) using the currently set line style (see GFX_LineStyleSet()).

    Description:
        This function renders a line from current line cursor position (x,y)
        to (x2,y2) using the currently set line style set by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet().

        If x2 and/or y2 does not lie on the frame buffer, then the behavior
        is undefined. If color is not set, before this function is called,
        the output is undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_LineToDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t x2,
                                int16_t y2)
{
    return (GFX_LineDraw(gfxIndex,    GFX_LinePositionXGet(gfxIndex),
                            GFX_LinePositionYGet(gfxIndex),
                            x2, y2));
}

// *****************************************************************************
/*  Function:
    int16_t GFX_LinePositionXGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the current x position of the line cursor.

    Description:
        This function returns the current x position of the line cursor.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

*/
// *****************************************************************************
int16_t GFX_LinePositionXGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxLineCursorX;
}

// *****************************************************************************
/*  Function:
    int16_t GFX_LinePositionYGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the current y position of the line cursor.

    Description:
        This function returns the current y position of the line cursor.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

*/
// *****************************************************************************
int16_t GFX_LinePositionYGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxLineCursorY;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LinePositionSet(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function sets the line cursor to a new position.

    Description:
        This function sets the line cursor to a new (x,y) position.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

        If x and/or y does not lie on the frame buffer, then the behavior
        of GFX_LineToDraw() and GFX_LineToRelativeDraw() functions are
        undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_LinePositionSet(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxLineCursorX = x;
    client->gfxLineCursorY = y;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_LinePositionRelativeSet(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t dX,
                                int16_t dY)

    Summary:
        This function sets the line cursor to a new position relative to
        the current position.

    Description:
        This function sets the line cursor to a new (x,y) position
        relative to the current cursor position. The new position is
        calculated by (x+dX, y + dY).

        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.
        Note that the parameters dX and dY are signed integers.
        This allows the new line cursor position to be placed to any
        direction from the current line cursor position.

        If (x+dX) and/or (y+dY) results in a position that is not on the
        frame buffer, then the behavior of GFX_LineToDraw() and
        GFX_LineToRelativeDraw() functions are undefined. If color is not set,
        before this function is called, the output is undefined.
   
*/
// *****************************************************************************
GFX_STATUS GFX_LinePositionRelativeSet(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t dX,
                                int16_t dY)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxLineCursorX += dX;
    client->gfxLineCursorY += dY;
    return (GFX_STATUS_SUCCESS);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_SolidLineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function renders the thin and thick solid line.
        This is an internal function and should not be called by the application.

    Description:
        This function renders the thin and thick solid lines
        (see GFX_LINE_STYLE).

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_SolidLineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)
{
    /*
     * NOTE: This function is an internal function and should not
     *       be called by the application.
     */

    typedef enum
    {
        GFX_SOLID_LINE_IDLE_STATE = 0x0D90,
        GFX_SOLID_LINE_VERHOR_DRAW_STATE,
        GFX_SOLID_LINE_DIAGONAL_INITIALIZE_STATE,
        GFX_SOLID_LINE_UPDATE_DRAW_STATE,
        GFX_SOLID_LINE_DIAGONAL_DRAW_STATE,
        GFX_SOLID_LINE_EXIT_STATE,
    } GFX_SOLID_LINE_DRAW_STATE;

    static GFX_SOLID_LINE_DRAW_STATE state = GFX_SOLID_LINE_IDLE_STATE;

    static int16_t   deltaX, deltaY;
    static int16_t   error, stepErrorLT, stepErrorGE;
    static int16_t   stepX, stepY, steep;
    static uint16_t  left, top, right, bottom, thickAdjustX, thickAdjustY;
    static uint16_t  lineThickness; //unused todo, lineType;
           int16_t   temp;

    while(1)
    {
        switch(state)
        {
            case GFX_SOLID_LINE_IDLE_STATE:

                // unused todo - lineType      = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_TYPE_MASK);
                lineThickness = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_THICKNESS_MASK);

                // line do not support alpha blending
#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
                GFX_AlphaBlendingValueSet(gfxIndex, 100);
#endif

                if((x1 == x2) || (y1 == y2))
                {
                    if(x1 > x2)
                    {
                        left = x2;
                        right = x1;
                    }
                    else
                    {
                        left = x1;
                        right = x2;
                    }

                    if(y1 > y2)
                    {
                        top = y2;
                        bottom = y1;
                    }
                    else
                    {
                        top = y1;
                        bottom = y2;
                    }

                    // adjust the dimension for thicklines
                    if (lineThickness)
                    {
                        if(x1 == x2)
                        {
                            thickAdjustX = 1;
                            thickAdjustY = 0;
                        }
                        else
                        {
                            thickAdjustX = 0;
                            thickAdjustY = 1;
                        }

                        left   -= thickAdjustX;
                        right  += thickAdjustX;
                        top    -= thickAdjustY;
                        bottom += thickAdjustY;
                    }
                    state = GFX_SOLID_LINE_VERHOR_DRAW_STATE;
                }
                else
                {
                    state = GFX_SOLID_LINE_DIAGONAL_INITIALIZE_STATE;
                }
                break;
                
            case GFX_SOLID_LINE_VERHOR_DRAW_STATE:
                // Draw the line
                GFX_BarDraw(gfxIndex, left, top, right, bottom);
                    
                // no break here since the next state is the state
                // that we want.

            case GFX_SOLID_LINE_EXIT_STATE:

                // else it is done exit gracefully
                // Move cursor
                GFX_LinePositionSet(gfxIndex, x2, y2);
                state = GFX_SOLID_LINE_IDLE_STATE;
                return GFX_STATUS_SUCCESS;

            case GFX_SOLID_LINE_DIAGONAL_INITIALIZE_STATE:
                stepX = 0;
                deltaX = x2 - x1;
                if(deltaX < 0)
                {
                    deltaX = -deltaX;
                    --stepX;
                }
                else
                {
                    ++stepX;
                }

                stepY = 0;
                deltaY = y2 - y1;
                if(deltaY < 0)
                {
                    deltaY = -deltaY;
                    --stepY;
                }
                else
                {
                    ++stepY;
                }

                steep = 0;
                if(deltaX < deltaY)
                {
                    ++steep;
                    temp = deltaX;
                    deltaX = deltaY;
                    deltaY = temp;
                    left = y1;
                    top  = x1;

                    temp = stepX;
                    stepX = stepY;
                    stepY = temp;
                    thickAdjustX = 1;
                    thickAdjustY = 0;

                }
                else
                {
                    left = x1;
                    top  = y1;
                    thickAdjustX = 0;
                    thickAdjustY = 1;
                }

                // If the current error greater or equal zero
                stepErrorGE = deltaX << 1;

                // If the current error less than zero
                stepErrorLT = deltaY << 1;

                // Error for the first pixel
                error = stepErrorLT - deltaX;
                state = GFX_SOLID_LINE_UPDATE_DRAW_STATE;
                // no break here since this is the next step anyway
                while(1)
                {
            case GFX_SOLID_LINE_UPDATE_DRAW_STATE:
                if (--deltaX >= 0)
                {
                    if(error >= 0)
                    {
                        top += stepY;
                        error -= stepErrorGE;
                    }

                    left += stepX;
                    error += stepErrorLT;
                    state = GFX_SOLID_LINE_DIAGONAL_DRAW_STATE;
                }
                else
                {
                    // it is done!
                    state = GFX_SOLID_LINE_EXIT_STATE;
                    break;
                }
                // no break here since the next step is the same state where we
                // want to go to

            case GFX_SOLID_LINE_DIAGONAL_DRAW_STATE:
                if (lineThickness)
                {
                    if (steep)
                    {
                        if (GFX_BarDraw(gfxIndex,
                            top - thickAdjustY,
                            left  - thickAdjustX,
                            top + thickAdjustY,
                            left  + thickAdjustX) == GFX_STATUS_FAILURE)
                        {
                            return GFX_STATUS_FAILURE;
                        }
                    }
                    else
                    {
                        if (GFX_BarDraw(gfxIndex,
                            left - thickAdjustX,
                            top  - thickAdjustY,
                            left + thickAdjustX,
                            top  + thickAdjustY) == GFX_STATUS_FAILURE)
                        {
                            return GFX_STATUS_FAILURE;
                        }
                    }

                }
                else
                {
                    // since pixel rendering is blocking no need to check
                    // status here
                    if (steep)
                        DRV_GFX_PixelPut(top, left);
                    else
                        DRV_GFX_PixelPut(left, top);       
                }

                }

            default:
                break;
        } // end of switch()
    } // end of while(1)

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_StyledLineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function renders the thin and thick dotted and dashed lines.
        This is an internal function and should not be called by the application.

    Description:
        This function renders the thin and thick dotted and dashed lines
        (see GFX_LINE_STYLE).

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_StyledLineDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)
{
    // This function implementation is blocking function since this is
    // rendering using the pixel rendering function. This function
    // renders line that is set to dashed or dotted styles.

    /*
     * NOTE: This function is an internal function and should not
     *       be called by the application.
     */


    int16_t   deltaX, deltaY;
    int16_t   error, stepErrorLT, stepErrorGE;
    int16_t   stepX, stepY;
    int16_t   steep;
    int16_t   style, type;
    uint16_t  lineThickness, lineType;
    uint16_t  left, top, right, bottom;
    int16_t   temp;

    lineType      = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_TYPE_MASK);
    lineThickness = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_THICKNESS_MASK);

    if (x1 == x2)
    {
        // &&&&&&&&&&&&&&&&&&&&&&
        // Vertical Line case
        // &&&&&&&&&&&&&&&&&&&&&&

        left = right = x1;
        if (y1 > y2)
        {
            top    = y2;
            bottom = y1;
        }
        else
        {
            top    = y1;
            bottom = y2;
        }
        style = 0;
        type = 1;
        
        for(temp = top; temp < bottom + 1; temp++)
        {
            if((++style) == lineType)
            {
                type ^= 1;
                style = 0;
            }
            if(type)
            {
                DRV_GFX_PixelPut(left, temp);
                if(lineThickness)
                {
                    GFX_BarDraw(gfxIndex, left-1,temp,left+1,temp);
                }        
            }
        } // end of for loop                

    }
    else if (y1 == y2)
    {
        // &&&&&&&&&&&&&&&&&&&&&&
        // Horizontal Line case
        // &&&&&&&&&&&&&&&&&&&&&&

        top = bottom = y1;
        if (x1 > x2)
        {
            left  = x2;
            right = x1;
        }
        else
        {
            left  = x1;
            right = x2;
        }
        style = 0;
        type = 1;
        
        for(temp = left; temp < right + 1; temp++)
        {
            if((++style) == lineType)
            {
                type ^= 1;
                style = 0;
            }
            if(type)
            {
                DRV_GFX_PixelPut(temp, top);
                if(lineThickness)
                {
                    GFX_BarDraw(gfxIndex, temp,top-1,temp,top+1);
                }        
            }
        } // end of for loop               

    }
    else
    {
        // &&&&&&&&&&&&&&&&&&&&&&
        // Diagonal Line case
        // &&&&&&&&&&&&&&&&&&&&&&

        stepX = 0;
        deltaX = x2 - x1;
        if(deltaX < 0)
        {
            deltaX = -deltaX;
            --stepX;
        }
        else
        {
            ++stepX;
        }
        stepY = 0;
        deltaY = y2 - y1;
        if(deltaY < 0)
        {
            deltaY = -deltaY;
            --stepY;
        }
        else
        {
            ++stepY;
        }
        steep = 0;
        if(deltaX < deltaY)
        {
            ++steep;
            temp = deltaX;
            deltaX = deltaY;
            deltaY = temp;

            temp = stepX;
            stepX = stepY;
            stepY = temp;

            left = y1;
            top  = x1;
        }
        else
        {
            left = x1;
            top  = y1;
        }
        right  = x2;
        bottom = y2;

        // If the current error greater or equal zero
        stepErrorGE = deltaX << 1;
        // If the current error less than zero
        stepErrorLT = deltaY << 1;
        // Error for the first pixel
        error = stepErrorLT - deltaX;
        style = 0;
        type = 1;

        while(deltaX >= 0)
        {
            if(type)
            {
                if(steep)
                {
                    DRV_GFX_PixelPut(top, left);
                    if(lineThickness)
                    {
                        GFX_BarDraw(gfxIndex, top-1,left,top+1,left);
                    }
                }
                else
                {
                    DRV_GFX_PixelPut(left, top);
                    if(lineThickness)
                    {
                        GFX_BarDraw(gfxIndex, left,top-1,left,top+1);
                    }
                }
            }

            if(error >= 0)
            {
                top += stepY;
                error -= stepErrorGE;
            }
            left += stepX;
            error += stepErrorLT;
            if((++style) == lineType)
            {
                type ^= 1;
                style = 0;
            }
            // decrement the difference
            deltaX--;
        }   // end of while        

    }

    GFX_LinePositionSet(gfxIndex, right, bottom);
    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_PolygonDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t sides,
                                uint16_t *pPoints)

    Summary:
        This function renders a polygon using the currently set line
        style (see GFX_LineStyleSet()) and color (see GFX_ColorSet()).

    Description:
        This function renders a polygon using the currently set line
        style (see GFX_LineStyleSet()) and color (see GFX_ColorSet()). The
        shape of the polygon is determined by the polygon points (an
        ordered array of x,y pairs) where the pair count is equal to the
        parameter sides.

        If any of the x,y pairs do not lie on the frame buffer, then the
        behavior is undefined. If color is not set, before this function
        is called, the output is undefined.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_PolygonDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t sides,
                                uint16_t *pPoints)
{

    typedef enum
    {
        POLYGON_BEGIN,
        POLYGON_DRAWING,
    } DRAWPOLY_STATES;

    static DRAWPOLY_STATES state = POLYGON_BEGIN;
    static int16_t counter, segmentCount;
    uint16_t sx, sy, ex, ey;

    while(1)
    {
        if(GFX_RenderStatusGet(gfxIndex) == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case POLYGON_BEGIN:
                counter = 0;
                segmentCount = sides << 1;
                state = POLYGON_DRAWING;

            case POLYGON_DRAWING:

                while(counter < segmentCount)
                {
                    sx = pPoints[counter    ];
                    sy = pPoints[counter + 1];
                    ex = pPoints[counter + 2];
                    ey = pPoints[counter + 3];

                    if(GFX_LineDraw(gfxIndex, sx, sy, ex, ey) == GFX_STATUS_FAILURE)
                        return (GFX_STATUS_FAILURE);

                    counter += 2;
                }

                state = POLYGON_BEGIN;
                return (GFX_STATUS_SUCCESS);

            default :
                // this should never happen
                state = POLYGON_BEGIN;
                return (GFX_STATUS_FAILURE);

        } // end of switch...
    } // end of while(1)...

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_FillStyleSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_FILL_STYLE style)

    Summary:
        Set the fill style to be used when rendering filled shapes.

    Description:
        This function sets the fill style to be used (see GFX_FILL_STYLE)
        when rendering filled shapes. All subsequent calls to the following:
        - GFX_RectangleFillDraw()
        - GFX_RectangleRoundFillDraw()
        - GFX_CircleFillDraw()
        will use the fill style set by this function.

*/
// *****************************************************************************
GFX_STATUS GFX_FillStyleSet(
                                            SYS_MODULE_INDEX gfxIndex,
                                            GFX_FILL_STYLE style)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxFillStyle = style;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_FILL_STYLE GFX_FillStyleGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        Return the fill style used when rendering filled shapes.

    Description:
        This function returns the fill style used (see GFX_FILL_STYLE)
        when rendering filled shapes.

*/
// *****************************************************************************
GFX_FILL_STYLE GFX_FillStyleGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxFillStyle);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GradientColorSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)

    Summary:
        Sets the gradient fill start and end color.

    Description:
        This function sets the gradient fill start and end colors
        (see GFX_FILL_STYLE) when rendering gradient filled shapes.
        All subsequent calls to the following using gradient fills:
        - GFX_RectangleFillDraw()
        - GFX_RectangleRoundFillDraw()
        - GFX_CircleFillDraw()
        will use the start and end colors set by this function.

*/
// *****************************************************************************
GFX_STATUS GFX_GradientColorSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxGradientStartColor = startColor;
    client->gfxGradientEndColor = endColor;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_GradientStartColorGet(void)

    Summary:
        Return the gradient start color when rendering gradient filled shapes.

    Description:
        This function returns the start color used when rendering
        gradient filled shapes.

*/
// *****************************************************************************
GFX_COLOR GFX_GradientStartColorGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxGradientStartColor);
}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_GradientEndColorGet(void)

    Summary:
        Return the gradient end color when rendering gradient filled shapes.

    Description:
        This function returns the end color used when rendering
        gradient filled shapes.

*/
// *****************************************************************************
GFX_COLOR GFX_GradientEndColorGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxGradientEndColor);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RectangleFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a filled rectangular shape using the
        currently set fill style and colors.

    Description:
        This function renders a filled rectangular shape with the currently
        set fill style (see GFX_FILL_STYLE) with the given left, top, right,
        and bottom parameters to define the shape dimension. The shape is
        rendered depending on the fill style. If a flat color is used, color
        must be set (see GFX_ColorSet()) before calling this function. If
        gradient color is used, gradient start and end color must be set
        (see GFX_GradientColorSet()) before calling this function.
        After the fill style and colors are set, multiple calls to this
        function can be performed.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{
    GFX_STATUS status = GFX_STATUS_FAILURE;

    GFX_FILL_STYLE style = GFX_FillStyleGet(gfxIndex);

    switch(style)
    {
        case GFX_FILL_STYLE_NONE:
            // call rectangle draw instead since there is no fill
            status = GFX_RectangleDraw(gfxIndex, left, top, right, bottom);
            break;
        case GFX_FILL_STYLE_ALPHA_COLOR:
            status = GFX_BarDraw(gfxIndex, left, top, right, bottom);
            break;
#ifndef GFX_CONFIG_GRADIENT_DISABLE
        case GFX_FILL_STYLE_GRADIENT_DOWN:
        case GFX_FILL_STYLE_GRADIENT_UP:
        case GFX_FILL_STYLE_GRADIENT_RIGHT:
        case GFX_FILL_STYLE_GRADIENT_LEFT:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
            status = GFX_BarGradientDraw(gfxIndex, left, top, right, bottom);
            break;
#endif
        case GFX_FILL_STYLE_COLOR:
        default:
            status = GFX_BarDraw(gfxIndex, left, top, right, bottom);
            break;
    };
    return (status);
    
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RectangleRoundFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius)

    Summary:
        This function renders a filled rectangular shape with round corners
        using the currently set fill style and colors.

    Description:
        This function renders a filled rounded rectangular shape with the
        currently set fill style (see GFX_FILL_STYLE) with the given left,
        top, right, bottom and radius parameters to define the shape dimension.
        The shape is rendered depending on the fill style. If a flat color
        is used, color must be set (see GFX_ColorSet()) before calling
        this function. If gradient color is used, gradient start and end
        color must be set (see GFX_GradientColorSet()) before calling this
        function. After the fill style and colors are set, multiple calls
        to this function can be performed.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleRoundFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius)
{
    GFX_STATUS status = GFX_STATUS_FAILURE;

    GFX_FILL_STYLE style = GFX_FillStyleGet(gfxIndex);

    switch(style)
    {
        case GFX_FILL_STYLE_NONE:
            // call rectangle draw instead since there is no fill
            status = GFX_RectangleRoundDraw(gfxIndex, left, top, right, bottom, radius);
            break;
#ifndef GFX_CONFIG_GRADIENT_DISABLE
        case GFX_FILL_STYLE_GRADIENT_DOWN:
        case GFX_FILL_STYLE_GRADIENT_UP:
        case GFX_FILL_STYLE_GRADIENT_RIGHT:
        case GFX_FILL_STYLE_GRADIENT_LEFT:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
            status = GFX_BevelGradientDraw(gfxIndex, left, top, right, bottom, radius);
            break;
#endif
        case GFX_FILL_STYLE_COLOR:
        case GFX_FILL_STYLE_ALPHA_COLOR:
        default:
            status = GFX_BevelFillDraw(gfxIndex, left, top, right, bottom, radius);
            break;
    };

    return (status);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_CircleFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius)

    Summary:
        This function renders a filled circle shape using the currently
        set fill style and colors.

    Description:
        This function renders a filled circle shape with the currently set
        fill style (see GFX_FILL_STYLE) with the given left, top, right and
        bottom parameters to define the shape dimension.
        The shape is rendered depending on the fill style. If a flat color
        is used, color must be set (see GFX_ColorSet()) before calling
        this function. If gradient color is used, gradient start and end
        color must be set (see GFX_GradientColorSet()) before calling this
        function. After the fill style and colors are set, multiple calls
        to this function can be performed.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When the center defined by x,y is not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_CircleFillDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius)
{
    return GFX_RectangleRoundFillDraw(gfxIndex, x, y, x, y, radius);
}

// *****************************************************************************
/*  Function:
    int16_t GFX_TextCursorPositionXGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the current x position of the text cursor.

    Description:
        This function returns the current x position of the text cursor.
        Text cursor is used as a starting point of the line rendered
        by the GFX_TextCharDraw() function.

*/
// *****************************************************************************
int16_t GFX_TextCursorPositionXGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxTextPositionX;
}

// *****************************************************************************
/*  Function:
    int16_t GFX_TextCursorPositionYGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the current y position of the text cursor.

    Description:
        This function returns the current y position of the text cursor.
        Text cursor is used as a starting point of the line rendered
        by the GFX_TextCharDraw() function.

*/
// *****************************************************************************
int16_t GFX_TextCursorPositionYGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxTextPositionY;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_TextCursorPositionSet(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t x,
                                int16_t y)

    Summary:
        This function sets the text cursor to a new position.

    Description:
        This function sets the text cursor to a new (x,y) position.
        Text cursor is used as a starting point of the character rendered
        by the GFX_TextCharDraw() function.

        If x and/or y does not lie on the frame buffer, then the behavior
        of GFX_TextCharDraw() function is undefined.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_TextCursorPositionSet(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t x,
                                int16_t y)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextPositionX = x;
    client->gfxTextPositionY = y;

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_PixelArrayPut(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels,
                                uint16_t numLines)

    Summary:
        Renders an array of pixels to the frame buffer.

    Description:
        This renders an array of pixels starting from the
        location defined by x and y with the length
        defined by numPixels. The rendering will be performed
        in the increasing x direction. If the length is more
        than 1 line of the screen, the rendering may continue
        to the next line depending on the way the memory of
        the display controller is arranged.
        For example, in some display controller, if the parameters are
           GFX_PixelArrayPut(gfxIndex, 0, 0, ptrToArray, (320*240), 1);
        The array is rendered on all the pixels of a QVGA screen.

        This function also supports transparent color feature.
        When the feature is enabled the pixel with the transparent
        color will not be rendered and will be skipped.
        x and y must define a location within the display
        buffer.
 
*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_PixelArrayPut(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels,
                                uint16_t numLines)
{
    uint16_t    z = 0;
    int16_t     newLeft, newTop, newRight; // unused todo , newBottom;

    newLeft   = x;
    newTop    = y;
    newRight  = x + numPixels - 1;
    // unused todo - newBottom = y;
    GFX_COLOR prevColor = GFX_ColorGet(GFX_INDEX_0);

    if (GFX_TransparentColorStatusGet(gfxIndex) == GFX_FEATURE_ENABLED)
    {
        for(z = newLeft; z <= newRight; z++)
        {
            // Write pixel to screen
            if (GFX_TransparentColorGet(gfxIndex) != *pPixel)
            {
                GFX_ColorSet(gfxIndex, *pPixel);
                DRV_GFX_PixelPut(z, newTop);
            }
            pPixel++;
        }
    }
    else
    {
        gfxDrv->PixelArrayPut(pPixel, x, y, numPixels, 1);
    }
    GFX_ColorSet(gfxIndex, prevColor);
    return z;
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_PixelArrayGet(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)

    Summary:
        Renders an array of pixels to the frame buffer.

    Description:
        This retrieves an array of pixels from the display buffer
        starting from the location defined by x and y with
        the length defined by numPixels. All the retrieved pixels
        must be inside the display buffer.

        The function behavoir will be undefined is one of the
        following is true:
        1. x and y location is outside the frame buffer.
        2. x + numPixels exceeds the frame buffer.
        3. Depending on how the frame buffer memory is arranged
           x + numPixels exceeds the width of the frame buffer.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_PixelArrayGet(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)
{
    uint16_t  z;

    for(z = x; z < (x + numPixels); z++)
    {
        *pPixel = gfxDrv->PixelGet(z, y);
        pPixel++;
    }        

    return (z - x);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ScreenClear(void)

    Summary:
        Clears the screen to the currently set color (GFX_ColorSet()).

    Description:
        This function clears the screen with the current color and sets
        the line cursor position to (0, 0).

        If color is not set, before this function is called, the output
        is undefined.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_ScreenClear(SYS_MODULE_INDEX gfxIndex)
{

    GFX_FILL_STYLE fillType;

    fillType = GFX_FillStyleGet(gfxIndex);
    GFX_FillStyleSet(gfxIndex, GFX_FILL_STYLE_COLOR);

    while(GFX_BarDraw(gfxIndex, 0, 0, GFX_MaxXGet(gfxIndex), GFX_MaxYGet(gfxIndex)) == 0);
    GFX_LinePositionSet(gfxIndex, 0, 0);

    GFX_FillStyleSet(gfxIndex, fillType);

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BevelDraw(
                    SYS_MODULE_INDEX gfxIndex,
                    uint16_t x1,
                    uint16_t y1,
                    uint16_t x2,
                    uint16_t y2,
                    uint16_t rad)

    Summary:
        This function renders an unfilled bevel shape using the currently
        set line style (see GFX_LineStyleSet()).

    Description:
        This is an internal function. Application should not call this
        function directly.

        This function renders an unfilled bevel shape using the given
        x1, y1, x2, y2 and rad parameters to define the shape dimension.
        rad defines the rounded corner shape. The bevel is rendered using the
        currently set line style by GFX_LineStyleSet(). The color used is the
        color set by the last call to GFX_ColorSet().

        Left most pixel location is defined by x1 - rad.
        Top most pixel location is defined by y1 - rad.
        Right most pixel location is defined by x2 + rad.
        Bottom most pixel location is defined by y2 + rad.
        When rad = 0, there are no rounded corners. In this case
        (x1,y1) will define the left, top corner and (x2,y2) will
        define the right, bottom corner of the shape.

        When x1 = x2 and y1 = y2, with radius > 0, a circular object is drawn.
        When x1 < x2 and y1 < y2 and rad (radius) = 0, a rectangular
        object is drawn.

        The rendering of the bevel is undefined when any one of the following
        is true:
        - Any of the following pixel locations x1-rad ,y1 - rad, x2+rad,
          y2 + rad falls outside the frame buffer.
        - Color is not set, before this function is called.
        - x2 < x1
        - y2 < y1

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_BevelDraw(
                                                SYS_MODULE_INDEX gfxIndex,
                                                uint16_t x1, uint16_t y1,
                                                uint16_t x2, uint16_t y2,
                                                uint16_t rad)
{

    /*
     * NOTE: This function is an internal function and should not
     *       be called by the application.
     */

    typedef enum
    {
        GFX_BEVEL_IDLE_STATE = 0x0DA0,
        GFX_BEVEL_RADIUS_EVAL_STATE,
        GFX_BEVEL_RADIUS_ERROR_EVAL_STATE,
        GFX_BEVEL_RADIUS_DRAW_STATE,
        GFX_BEVEL_LINE_TOP_DRAW_STATE,
        GFX_BEVEL_LINE_BOTTOM_DRAW_STATE,
        GFX_BEVEL_LINE_RIGHT_DRAW_STATE,
        GFX_BEVEL_LINE_LEFT_DRAW_STATE,


    } GFX_BEVEL_DRAW_STATE;

    static GFX_BEVEL_DRAW_STATE state = GFX_BEVEL_IDLE_STATE;
    static uint16_t             lineType, lineThickness;
    static int16_t              style, type, xLimit, xPos, yPos, error;
    PRIMITIVE_UINT32_UNION      temp;

    while(1)
    {
        switch(state)
        {
            case GFX_BEVEL_IDLE_STATE:

                if(rad)
                {
                    temp.uint32Value = SIN45 * rad;
                    xLimit = temp.uint3216BitValue[1] + 1;
                    temp.uint32Value = (uint32_t) (ONEP25 - ((int32_t) rad << 16));
                    error = (int16_t) (temp.uint3216BitValue[1]);
                    yPos = rad;
                    style = 0;
                    type = 1;

                    lineType      = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_TYPE_MASK);
                    lineThickness = (GFX_LineStyleGet(gfxIndex) & GFX_LINE_THICKNESS_MASK);
                    state = GFX_BEVEL_RADIUS_EVAL_STATE;
                    xPos = 0;
                    // no break here, to go through the next state
                }
                else
                {
                    state = GFX_BEVEL_LINE_TOP_DRAW_STATE;
                    break;
                }

            case GFX_BEVEL_RADIUS_EVAL_STATE:

                if (xPos <= xLimit)
                {
                    if((++style) == lineType)
                    {
                        type ^= 1;
                        style = 0;
                    }
                    if (type)
                    {
                        state = GFX_BEVEL_RADIUS_DRAW_STATE;
                        // no break here, to go through the next state
                    }
                    else
                    {
                        state = GFX_BEVEL_RADIUS_ERROR_EVAL_STATE;
                        break;

                    }
                }
                else
                {
                    // rounded corner is done
                    // go to rendering the lines
                    state = GFX_BEVEL_LINE_TOP_DRAW_STATE;
                    break;
                }
                // no break here, to go through the next state

            case GFX_BEVEL_RADIUS_DRAW_STATE:
                if(lineThickness)
                {
                    GFX_BarDraw(gfxIndex, x2 + xPos, y1 - yPos - 1,x2 + xPos, y1 - yPos + 1); // 1st quadrant
                    GFX_BarDraw(gfxIndex, x2 + yPos - 1, y1 - xPos,x2 + yPos + 1, y1 - xPos);

                    GFX_BarDraw(gfxIndex, x2 + xPos, y2 + yPos - 1,x2 + xPos, y2 + yPos + 1); // 2nd quadrant
                    GFX_BarDraw(gfxIndex, x2 + yPos - 1, y2 + xPos,x2 + yPos + 1, y2 + xPos);

                    GFX_BarDraw(gfxIndex, x1 - xPos, y2 + yPos - 1,x1 - xPos, y2 + yPos + 1); // 3rd quadrant
                    GFX_BarDraw(gfxIndex, x1 - yPos - 1, y2 + xPos,x1 - yPos + 1, y2 + xPos);

                    GFX_BarDraw(gfxIndex, x1 - yPos - 1, y1 - xPos,x1 - yPos + 1, y1 - xPos); // 4th quadrant
                    GFX_BarDraw(gfxIndex, x1 - xPos, y1 - yPos - 1,x1 - xPos, y1 - yPos + 1);

                }
                else
                {
                    DRV_GFX_PixelPut(x2 + xPos, y1 - yPos);         // 1st quadrant
                    DRV_GFX_PixelPut(x2 + yPos, y1 - xPos);
                    DRV_GFX_PixelPut(x2 + xPos, y2 + yPos);         // 2nd quadrant
                    DRV_GFX_PixelPut(x2 + yPos, y2 + xPos);
                    DRV_GFX_PixelPut(x1 - xPos, y2 + yPos);         // 3rd quadrant
                    DRV_GFX_PixelPut(x1 - yPos, y2 + xPos);
                    DRV_GFX_PixelPut(x1 - yPos, y1 - xPos);         // 4th quadrant
                    DRV_GFX_PixelPut(x1 - xPos, y1 - yPos);
                }

                state = GFX_BEVEL_RADIUS_ERROR_EVAL_STATE;
                // no break here, to go through the next state

            case GFX_BEVEL_RADIUS_ERROR_EVAL_STATE:
                if(error > 0)
                {
                    yPos--;
                    error += 5 + ((xPos - yPos) << 1);
                }
                else
                {
                    error += 3 + (xPos << 1);
                }
                xPos++;
                state = GFX_BEVEL_RADIUS_EVAL_STATE;
                break;

            /* &&&&&&&&&&&&&&&&&&&&&&&&&&&& */
            /*    Render the lines here     */
            /* &&&&&&&&&&&&&&&&&&&&&&&&&&&& */

            case GFX_BEVEL_LINE_TOP_DRAW_STATE:
                // Must use lines here since this can also be used to draw focus of round buttons
                if(x2 - x1 > 0)
                {
                    // draw top
                    if (GFX_LineDraw(gfxIndex, x1, y1 - rad, x2, y1 - rad) == GFX_STATUS_FAILURE)
                        return (GFX_STATUS_FAILURE);
                    state = GFX_BEVEL_LINE_BOTTOM_DRAW_STATE;
                    // no break here, to go through the next state
                }
                else
                {
                    state = GFX_BEVEL_LINE_LEFT_DRAW_STATE;
                    break;
                }

            case GFX_BEVEL_LINE_BOTTOM_DRAW_STATE:
                // draw bottom
                if (GFX_LineDraw(gfxIndex, x1, y2 + rad, x2, y2 + rad) == GFX_STATUS_FAILURE)
                    return (GFX_STATUS_FAILURE);
                state = GFX_BEVEL_LINE_LEFT_DRAW_STATE;
                // no break here, to go through the next state

            case GFX_BEVEL_LINE_LEFT_DRAW_STATE:
                if (y2 - y1 > 0)
                {
                    // draw left
                    if (GFX_LineDraw(gfxIndex, x1 - rad, y1, x1 - rad, y2) == GFX_STATUS_FAILURE)
                        return (GFX_STATUS_FAILURE);
                    state = GFX_BEVEL_LINE_RIGHT_DRAW_STATE;
                    // no break here, to go through the next state
                }

            case GFX_BEVEL_LINE_RIGHT_DRAW_STATE:
                // draw right
                if (GFX_LineDraw(gfxIndex, x2 + rad, y1, x2 + rad, y2) == GFX_STATUS_FAILURE)
                    return (GFX_STATUS_FAILURE);
                state = GFX_BEVEL_IDLE_STATE;
                return (GFX_STATUS_SUCCESS);

            default:
                // this should never happen
                return (GFX_STATUS_FAILURE);

        }; // end of switch

    } // end of while(1)
}

// *****************************************************************************
/*  Function:
    void GFX_BevelDrawTypeSet(SYS_MODULE_INDEX gfxIndex, GFX_BEVEL_RENDER_TYPE type)

    Summary:
        This function sets the bevel rendering type.

    Description:
        This function sets the bevel rendering type. The types
        are described in GFX_BEVEL_RENDER_TYPE type enumeration.
        DRAWFULLBEVEL - draw the full shape
        DRAWTOPBEVEL - draw the upper half portion
        DRAWBOTTOMBEVEL - draw the lower half portion

*/
// *****************************************************************************
void GFX_BevelDrawTypeSet(SYS_MODULE_INDEX gfxIndex, GFX_BEVEL_RENDER_TYPE type)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxBevelDrawType = type;
}

// *****************************************************************************
/*  Function:
    GFX_BEVEL_RENDER_TYPE GFX_BevelDrawTypeGet(void)

    Summary:
        This function gets the bevel rendering type.

    Description:
        This function gets the bevel rendering type. The types
        are described in GFX_BEVEL_RENDER_TYPE type enumeration.

*/
// *****************************************************************************
GFX_BEVEL_RENDER_TYPE GFX_BevelDrawTypeGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBevelDrawType);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RectangleDraw(
                    uint16_t left,
                    uint16_t top,
                    uint16_t right,
                    uint16_t bottom)

    Summary:
        This function renders a rectangular shape using the 
        currently set line style and color.

    Description:
        This function renders a rectangular shape using the given left, top, 
        right and bottom parameters to define the shape dimension. The shape
        is rendered using the currently set line style by GFX_LineStyleSet(). 
        The color used is the color set by the last call to GFX_ColorSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top, right,bottom falls 
          outside the frame buffer.
        - Color is not set, before this function is called.
        - right < left
        - bottom < top

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{
    if (GFX_BevelDraw(gfxIndex, left, top, right, bottom, 0) == GFX_STATUS_FAILURE)
        return (GFX_STATUS_FAILURE);
    else
        return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RectangleRoundDraw(
                    SYS_MODULE_INDEX gfxIndex,
                    uint16_t left,
                    uint16_t top,
                    uint16_t right,
                    uint16_t bottom,
                    uint16_t radius)

    Summary:
        This function renders a rounded corner rectangular shape using the 
        currently set line style and color.

    Description:
        This function renders a rectangular shape with rounded corner using 
        the given left, top, right, bottom and radius parameters to define the
        shape dimension. radius defines the rounded corner shape. The shape
        is rendered using the currently set line style by GFX_LineStyleSet(). 
        The color used is the color set by the last call to GFX_ColorSet().

        Left most pixel location is defined by left - radius.
        Top most pixel location is defined by top - radius.
        Right most pixel location is defined by right + radius.
        Bottom most pixel location is defined by bottom + radius.
        When radius = 0, there are no rounded corners. In this case
        (left,top) will define the left, top corner and (right,bottom) will
        define the right, bottom corner of the shape.

        When left = right and top = bottom, with radius > 0, a circular
        object is drawn. When left < right and top < bottom and radius = 0,
        a rectangular object is drawn.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left-rad , top-rad, right+rad,
          bottom+rad falls outside the frame buffer.
        - Color is not set, before this function is called.
        - right < left
        - bottom < top

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleRoundDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius)
{
    if (GFX_BevelDraw(gfxIndex, left, top, right, bottom, radius) == GFX_STATUS_FAILURE)
        return (GFX_STATUS_FAILURE);
    else
        return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_CircleDraw(
                    SYS_MODULE_INDEX gfxIndex,
                    uint16_t x,
                    uint16_t y,
                    uint16_t radius)

    Summary:
        This function renders a circular shape using the
        currently set line style and color.

    Description:
        This function renders a circular shape using the center (x,y) and
        radius. The shape is rendered using the currently set line style by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet().

        When x,y falls outside the buffer, the behavior is undefined.
        When color is not set before this function is called, the bahavior is
        undefined. When any of the following x+radius, x-radius, y+radius
        and y-radius falls outside the buffer, the behavior is undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_CircleDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius)
{
    if (GFX_BevelDraw(gfxIndex, x, y, x, y, radius) == GFX_STATUS_FAILURE)
        return (GFX_STATUS_FAILURE);
    else
        return (GFX_STATUS_SUCCESS);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BevelFillDraw(
                    uint16_t x1,
                    uint16_t y1,
                    uint16_t x2,
                    uint16_t y2,
                    uint16_t rad)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function renders a filled beveled shape with the currently set
        color set by the last call to GFX_ColorSet(). The dimension of the
        shape is determined by the x1, y1, x2, y2 and radius set by rad.
        The

        When x1,y1,x2,y2 falls outside the buffer, the behavior is undefined.
        When color is not set before this function is called, the bahavior is
        undefined. When any of the following x1+radius, x2-radius, y1+radius
        and y2-radius falls outside the buffer, the behavior is undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_BevelFillDraw(SYS_MODULE_INDEX gfxIndex, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t rad)
{

    typedef enum
    {
        FB_BEGIN,
        FB_INIT_FILL,
        FB_CHECK,
        FB_Q8TOQ1,
        FB_Q7TOQ2,
        FB_Q6TOQ3,
        FB_Q5TOQ4,
        FB_DRAW_MISSING_1,
        FB_DRAW_MISSING_2,
        FB_WAITFORDONE,
    } FB_FILLCIRCLE_STATES;

    static int16_t err;
    static int16_t yLimit, xPos, yPos;
    static int16_t xCur, yCur, yNew;
    static int16_t last54Y, last18Y;
    PRIMITIVE_UINT32_UNION temp;
 

    static FB_FILLCIRCLE_STATES state = FB_BEGIN;

    while(1)
    {
        if(GFX_RenderStatusGet(gfxIndex) == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case FB_BEGIN:

                // draw the face
                if (GFX_BevelDrawTypeGet(gfxIndex) == GFX_DRAW_FULLBEVEL)
                {
                    GFX_BarDraw(gfxIndex, x1 - rad, y1, x2 + rad, y2);
                }
                else if (GFX_BevelDrawTypeGet(gfxIndex) == GFX_DRAW_TOPBEVEL)
                {
                    GFX_BarDraw(gfxIndex, x1 - rad, y1, x2 + rad, y1+((y2-y1)>>1));
                }
                else
                {
                    GFX_BarDraw(gfxIndex, x1 - rad, y1 + ((y2 - y1) >> 1), x2 + rad, y2);
                }
                state = FB_INIT_FILL;
                break;

            case FB_INIT_FILL:

                if(!rad)
                {   // no radius object is a filled rectangle
                    state = FB_WAITFORDONE;
                    break;
                }

                // compute variables
                temp.uint32Value = SIN45 * rad;
                // The + 1 in the limit is added to make the circle look less jagged
                // when rendering with small radius
                yLimit = temp.uint3216BitValue[1] + 1;
                temp.uint32Value = (uint32_t) (ONEP25 - ((int32_t) rad << 16));
                err = (int16_t) (temp.uint3216BitValue[1]);
                xPos = rad;
                yPos = 0;

                // yCur starts at 1 so the center line is not drawn and will be drawn separately
                // this is to avoid rendering twice on the same line
                yCur = 1;
                xCur = xPos;
                yNew = yPos;

                // note initial values are important
                last54Y = GFX_MaxYGet(gfxIndex);
                last18Y = 0;

                state = FB_CHECK;

            case FB_CHECK:

                // check first for limits
                if(yPos > yLimit)
                {
                    if (rad)
                        state = FB_DRAW_MISSING_1;
                    else
                        state = FB_WAITFORDONE;
                    break;
                }

                // yNew records the last y position
                yNew = yPos;

                // calculate the next value of x and y
                if(err > 0)
                {
                    xPos--;
                    err += 5 + ((yPos - xPos) << 1);
                }
                else
                    err += 3 + (yPos << 1);
                yPos++;
                state = FB_Q6TOQ3;

            case FB_Q6TOQ3:

                if (xCur != xPos)
                {
                    // 6th octant to 3rd octant
                    if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_BOTTOMBEVEL)
                    {
                        // check first if there will be an overlap
                        if (y2 + yNew >= last54Y)
                        {
                            if ((y2 + yCur) <= (last54Y - 1))
                            {
                                GFX_BarDraw(gfxIndex, x1 - xCur, y2 + yCur, x2 + xCur, last54Y - 1);
                            }
                        }
                        else
                        {
                                GFX_BarDraw(gfxIndex, x1 - xCur, y2 + yCur, x2 + xCur, y2 + yNew);
                        }
                    }
                    state = FB_Q5TOQ4;
                    break;
                }

                state = FB_CHECK;
                break;

            case FB_Q5TOQ4:

            	if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_BOTTOMBEVEL)
            	{

   	            // 5th octant to 4th octant
                    // check: if overlapping then no need to proceed
                    if ((y2 + xCur) > (y2 + yNew))
                    {
                        GFX_BarDraw(gfxIndex, x1 - yNew, y2 + xCur, x2 + yNew, y2 + xCur);
                        last54Y = (y2 + xCur);
                    }
                }

                state = FB_Q7TOQ2;
                break;

            case FB_Q7TOQ2:

                // 7th octant to 2nd octant
	        if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_TOPBEVEL)
		{
                    // check: if overlapping then no need to proceed
                    if (y1 - yNew <= last18Y)
                    {
                        if ((last18Y + 1) <= (y1 - yCur))
                        {
                            GFX_BarDraw(gfxIndex, x1 - xCur, last18Y + 1, x2 + xCur, y1 - yCur);
                        }
                    }
                    else
                    {
                            GFX_BarDraw(gfxIndex, x1 - xCur, y1 - yNew, x2 + xCur, y1 - yCur);
                    }
                }
                state = FB_Q8TOQ1;
                break;

            case FB_Q8TOQ1:

                // 8th octant to 1st octant
                if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_TOPBEVEL)
		{
                    // check first if there will be an overlap
                    if ((y1 - xCur) < (y1 - yNew))
                    {
    	                GFX_BarDraw(gfxIndex, x1 - yNew, y1 - xCur, x2 + yNew, y1 - xCur);
                        last18Y = y1 - xCur;
                    }

                }
                // update current values
                xCur = xPos;
                yCur = yPos;

                state = FB_CHECK;
                break;

            case FB_DRAW_MISSING_1:
                // check only one side since they are the same
                if ((y1 - yNew) > (y1 - xPos))
                {
                    // there is a missing line, draw that line
                    if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_TOPBEVEL)
                        GFX_BarDraw(gfxIndex, x1 - yNew, y1 - xPos, x2 + yNew, y1 - xPos);

                    state = FB_DRAW_MISSING_2;
                }
                else
                    state = FB_WAITFORDONE;
                break;

            case FB_DRAW_MISSING_2:
                if ((y2 + yNew) < (y2 + xPos))
                {
                    if (GFX_BevelDrawTypeGet(gfxIndex) & GFX_DRAW_BOTTOMBEVEL)
                        GFX_BarDraw(gfxIndex, x1 - yNew, y2 + xPos, x2 + yNew, y2 + xPos);
                }
                state = FB_WAITFORDONE;

            case FB_WAITFORDONE:
                if(GFX_RenderStatusGet(gfxIndex) == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);

                state = FB_BEGIN;
                return (GFX_STATUS_SUCCESS);

        }           // end of switch
    }               // end of while
}

// *****************************************************************************
/*  Function:
    void GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function is intended to allow driver level implementation of
        text rendering. This functions sets the font used when rendering text.

        In some cases, hardware is present to perform rendering and help
        useful Graphics Library routines. When this is present, we take
        advantage by implementing some primitive layer routines in the
        driver code.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont)
{ }

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DRV_TextCharRender(GFX_XCHAR ch)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function is intended to allow driver level implementation of
        text rendering. This functions sets the font used when rendering text.

        In some cases, hardware is present to perform rendering and help
        useful Graphics Library routines. When this is present, we take
        advantage by implementing some primitive layer routines in the
        driver code.

        This function in Primitive Layer is a dummy function. Actual
        implementation is in the driver layer.  If the driver do not
        implement this function, then this function should never be called.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_DRV_TextCharRender(GFX_XCHAR ch)
{
    return (GFX_STATUS_FAILURE);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_FontSet(GFX_RESOURCE_HDR *pFont)

    Summary:
        This function sets the current font used when rendering
        strings and characters.

    Description:
        This function sets the current font used to render strings and
        characters.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_FontSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_RESOURCE_HDR *pFont)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->pGfxCurrentFont = pFont;
    GFX_DRV_FontSet (pFont);        
    

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_RESOURCE_HDR GFX_FontGet()

    Summary:
        This function returns the current font used when rendering
        strings and characters.

    Description:
        This function returns the current font used to render strings and
        characters.

*/
// *****************************************************************************
inline GFX_RESOURCE_HDR* GFX_FontGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->pGfxCurrentFont);
}

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_FontAntiAliasSet(GFX_FONT_ANTIALIAS_TYPE type)

    Summary:
        This function sets the font anti-aliasing mode.

    Description:
        This function sets the font anti-aliasing mode used when
        rendering anti-aliased strings and characters.

*/
// *****************************************************************************
inline GFX_STATUS GFX_FontAntiAliasSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_FONT_ANTIALIAS_TYPE type)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxTextAntiAliasType = type;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_FONT_ANTIALIAS_TYPE GFX_FontAntiAliasGet(void)

    Summary:
        This function returns the current font font anti-aliasing mode.

    Description:
        This function returns the font anti-aliasing mode used when
        rendering anti-aliased strings and characters. Default setting
        at initialization is GFX_FONT_ANTIALIAS_OPAQUE.

*/
// *****************************************************************************
inline GFX_FONT_ANTIALIAS_TYPE GFX_FontAntiAliasGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return client->gfxTextAntiAliasType;
}

/*********************************************************************
* Function: static inline void GFX_CalculateColors(SYS_MODULE_INDEX gfxIndex)
*
* PreCondition: gfx_TextBackgroundColor100 and gfx_TextForegroundColor100 must be set
*
* Input: none
*
* Output: none
*
* Side Effects: gfx_TextBackgroundColor25, gfx_TextBackgroundColor75, gfx_TextForegroundColor25 and gfx_TextForegroundColor75 will be calculated
*
* Overview: calculates mid values of colors
*
* Note: Internal to this file
*
********************************************************************/
static inline void GFX_CalculateColors(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_COLOR   gfx_TextForegroundColor50;
    GFX_COLOR   gfx_TextBackgroundColor50;
    
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)

    gfx_TextForegroundColor50  = GFX_Color50Convert(client->gfx_TextForegroundColor100);
    client->gfx_TextForegroundColor25  = GFX_Color25Convert(client->gfx_TextForegroundColor100);
    client->gfx_TextForegroundColor75  = gfx_TextForegroundColor50 +
                                 client->gfx_TextForegroundColor25;

    gfx_TextBackgroundColor50  = GFX_Color50Convert(client->gfx_TextBackgroundColor100);
    client->gfx_TextBackgroundColor25  = GFX_Color25Convert(client->gfx_TextBackgroundColor100);
    client->gfx_TextBackgroundColor75  = gfx_TextBackgroundColor50 +
                                 client->gfx_TextBackgroundColor25;

#elif ((GFX_CONFIG_COLOR_DEPTH == 8) || (GFX_CONFIG_COLOR_DEPTH == 4))
        
    gfx_TextForegroundColor50  = client->gfx_TextForegroundColor100 >> 1;
    gfx_TextForegroundColor25  = gfx_TextForegroundColor50  >> 1;
    gfx_TextForegroundColor75  = gfx_TextForegroundColor50 +
                                 client->gfx_TextForegroundColor25;

    gfx_TextBackgroundColor50  = client->gfx_TextBackgroundColor100 >> 1;
    client->gfx_TextBackgroundColor25  = gfx_TextBackgroundColor50  >> 1;
    client->gfx_TextBackgroundColor75  = gfx_TextBackgroundColor50 +
                                 client->gfx_TextBackgroundColor25;

    #warning "Antialiasing at 8BPP and 4BPP is supported only for Grayscale mode"
        
#else
    
    #error "Anit-aliasing is currently supported only in 8BPP Grayscale and 16BPP, 24BPP color modes"
    
#endif
    client->gfx_TextForegroundColor25 += client->gfx_TextBackgroundColor75;
    client->gfx_TextForegroundColor75 += client->gfx_TextBackgroundColor25;
}

#endif //#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_TextCharInfoFlashGet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Retrieves the character information from flash memory. This is
        a step performed before the character is rendered.

*/
// *****************************************************************************
void GFX_TextCharInfoFlashGet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)
{
#ifndef GFX_CONFIG_FONT_FLASH_DISABLE  
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY_EXTENDED    *pChTableExtended;
    GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY             *pChTable;

    // set color depth of font,
    // based on 2^bpp where bpp is the color depth setting in the GFX_FONT_HEADER
    pParam->bpp = 1 << client->pGfxCurrentFont->resource.font.header.bpp;

    if(client->pGfxCurrentFont->resource.font.header.extendedGlyphEntry)
    {
        // font used has extended glyph, get info on the character
        pChTableExtended = (GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY_EXTENDED *)
                           (
                                client->pGfxCurrentFont->resource.font.location.progByteAddress +
                                sizeof(GFX_FONT_HEADER)
                           ) 
                           +
                           (
                                (GFX_UXCHAR)ch -
                                (GFX_UXCHAR)client->pGfxCurrentFont->resource.font.header.firstChar
                           );
        pParam->pChImage = (GFX_FONT_SPACE uint8_t *)
                           (
                                client->pGfxCurrentFont->resource.font.location.progByteAddress +
                                pChTableExtended->offset
                           );

        pParam->chGlyphWidth =  pChTableExtended->glyphWidth;
        pParam->xWidthAdjust =  (
                                    pChTableExtended->glyphWidth -
                                    pChTableExtended->cursorAdvance
                                );
        pParam->xAdjust      =  pChTableExtended->xAdjust;
        pParam->yAdjust      =  pChTableExtended->yAdjust;
                    
        if(pParam->yAdjust > 0)
        {
            pParam->heightOvershoot = pParam->yAdjust;
        }
        else
        {
            pParam->heightOvershoot = pParam->yAdjust * -1;
        }

    }
    else
    {
        // font used is normal font, get info on the character
        pChTable = (GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY *) 
                        (
                            client->pGfxCurrentFont->resource.font.location.progByteAddress +
                            sizeof(GFX_FONT_HEADER)
                        ) +
                        (
                            (GFX_UXCHAR)ch -
                            (GFX_UXCHAR)client->pGfxCurrentFont->resource.font.header.firstChar
                        );

        pParam->pChImage = (GFX_FONT_SPACE uint8_t *) 
                        ( client->pGfxCurrentFont->resource.font.location.progByteAddress +
                        (
                            (uint32_t)(pChTable->offsetMSB) << 8) +
                            pChTable->offsetLSB
                        );

        pParam->chGlyphWidth = pChTable->width;
    }

#endif 
    
}

// *****************************************************************************
/*  Function:
    void GFX_TextCharInfoExternalGet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Retrieves the character information from external memory. This is
        a step performed before the character is rendered.

*/
// *****************************************************************************
void GFX_TextCharInfoExternalGet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)
{
#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE
    
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_FONT_GLYPH_ENTRY            chTable;
    GFX_FONT_GLYPH_ENTRY_EXTENDED   chTableExtended;
    uint16_t                        imageSize;
    GFX_UXCHAR                      temp;
    PRIMITIVE_UINT32_UNION          glyphOffset;

    // set color depth of font,
    // based on 2^bpp where bpp is the color depth setting in the GFX_FONT_HEADER
    pParam->bpp = 1 << client->pGfxCurrentFont->resource.font.header.bpp;
       
    if(client->pGfxCurrentFont->resource.font.header.extendedGlyphEntry)
    {
        temp  = (GFX_UXCHAR)ch - (GFX_UXCHAR)client->pGfxCurrentFont->resource.font.header.firstChar;
        temp *= sizeof(GFX_FONT_GLYPH_ENTRY_EXTENDED);
        temp += sizeof(GFX_FONT_HEADER);

        GFX_ExternalResourceCallback
        (
            client->pGfxCurrentFont,
            temp,
//            sizeof(GFX_FONT_HEADER) +
//                    (((GFX_UXCHAR)ch - (GFX_UXCHAR)client->pGfxCurrentFont->resource.font.header.firstChar) * sizeof(GFX_FONT_GLYPH_ENTRY_EXTENDED)),
            sizeof(GFX_FONT_GLYPH_ENTRY_EXTENDED),
            &chTableExtended
        );
    
        pParam->chGlyphWidth = chTableExtended.glyphWidth;
        pParam->xWidthAdjust = chTableExtended.glyphWidth - chTableExtended.cursorAdvance;
        pParam->xAdjust = chTableExtended.xAdjust;
        pParam->yAdjust = chTableExtended.yAdjust;
    
        if(pParam->yAdjust > 0)
        {
            pParam->heightOvershoot = pParam->yAdjust;
        }
        else
        {
            pParam->heightOvershoot = pParam->yAdjust * -1;
        }
                    
    }
    else
    {
        // get glyph entry
        GFX_ExternalResourceCallback
        (
            client->pGfxCurrentFont,
            sizeof(GFX_FONT_HEADER) + ((GFX_UXCHAR)ch - (GFX_UXCHAR)client->pGfxCurrentFont->resource.font.header.firstChar) * sizeof(GFX_FONT_GLYPH_ENTRY),
            sizeof(GFX_FONT_GLYPH_ENTRY),
            &chTable
        );
        pParam->chGlyphWidth = chTable.width;
    }

    // width of glyph in bytes
    imageSize = 0;
    pParam->chEffectiveGlyphWidth = pParam->chGlyphWidth * pParam->bpp;
    if(pParam->chEffectiveGlyphWidth & 0x0007)
        imageSize = 1;
    imageSize += (pParam->chEffectiveGlyphWidth >> 3);

    // glyph image size
    imageSize *= client->pGfxCurrentFont->resource.font.header.height;

    if(client->pGfxCurrentFont->resource.font.header.extendedGlyphEntry)
    {
        glyphOffset.uint32Value = chTableExtended.offset;
    }
    else
    {
        // get glyph image
        glyphOffset.uint3216BitValue[1] = (chTable.offsetMSB >> 8);
        glyphOffset.uint3216BitValue[0] = (chTable.offsetMSB << 8) + (chTable.offsetLSB);
    }
            
    GFX_ExternalResourceCallback(client->pGfxCurrentFont, glyphOffset.uint32Value, imageSize, &(pParam->chImage));
    pParam->pChImage = (uint8_t *) &(pParam->chImage);

#endif //#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE

}

/*********************************************************************
* Function:  void GFX_TextCharRender(GFX_XCHAR ch, GFX_FONT_OUTCHAR *pParam)
*
* PreCondition: none
*
* Input: ch - character code
*        pParam - pointer to character information structure. 
*
* Output: none
*
* Side Effects: none
*
* Overview: Performs the actual rendering of the character using PutPixel().
*
* Note: Application should not call this function. This function is for 
*       versatility of implementing hardware accelerated text rendering
*       only.
*
********************************************************************/
GFX_STATUS GFX_TextCharRender(  SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)
{

    uint8_t     temp = 0;
    uint8_t     mask;
    uint8_t     restoremask;
    int16_t     xCnt, yCnt, x = 0, y, fontHeight;
#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
    uint8_t     val, shift;
    GFX_COLOR   bgcolor;
#endif
    
    fontHeight = GFX_TextStringHeightGet(GFX_FontGet(gfxIndex));

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE   
    
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if(pParam->bpp == 1)
    {
        restoremask = 0x01;
    }
    else
    {
        if(pParam->bpp == 2)
        {
            restoremask = 0x03;
        }
        else
        {
            return (GFX_STATUS_FAILURE); // BPP > 2 are not yet supported
        }
        
        gfxDrv->PixelArrayGet(&bgcolor, GFX_TextCursorPositionXGet(gfxIndex),
                                GFX_TextCursorPositionYGet(gfxIndex) + (fontHeight >> 1),1
                              );
        
        if((client->gfx_TextForegroundColor100 != GFX_ColorGet(gfxIndex)) ||
           (client->gfx_TextBackgroundColor100 != bgcolor))
        {
            client->gfx_TextForegroundColor100 = GFX_ColorGet(gfxIndex);
            client->gfx_TextBackgroundColor100 = bgcolor;
            GFX_CalculateColors(gfxIndex);
        }
    }
#else
    restoremask = 0x01;
#endif
    
    y = GFX_TextCursorPositionYGet(gfxIndex) + pParam->yAdjust;

    for(yCnt = 0; yCnt < fontHeight + pParam->heightOvershoot; yCnt++)
    {

        x = GFX_TextCursorPositionXGet(gfxIndex) + pParam->xAdjust;
        mask = 0;

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
        shift = 0;
#endif

        for(xCnt = 0; xCnt < pParam->chGlyphWidth; xCnt++)
        {

            if(mask == 0)
            {
                temp = *(pParam->pChImage)++;
                mask = restoremask;

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
                shift = 0;
#endif
            }

            // check if the pixel is in the text region
            if (
                    (   (GFX_TextAreaLeftGet(gfxIndex)   < x) &&
                        (GFX_TextAreaRightGet(gfxIndex)  > x)
                    )
                    &&
                    (   (GFX_TextAreaTopGet(gfxIndex)    < y) &&
                        (GFX_TextAreaBottomGet(gfxIndex) > y)
                    )
               )
            {

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
                if (pParam->bpp == 1)
                {
                    if (temp & mask)
                    {
                        DRV_GFX_PixelPut(x, y);
                    }
                }
                else
                {
                    val = (temp & mask) >> shift;
                    if (val)
                    {
                        if(GFX_FontAntiAliasGet(gfxIndex) == GFX_FONT_ANTIALIAS_TRANSLUCENT)
                        {
                            //bgcolor = gfxDrv->PixelArrayGet(&bgcolor, x, y, 1);
                            gfxDrv->PixelArrayGet(&bgcolor, x, y, 1);                            
                            if(client->gfx_TextBackgroundColor100 != bgcolor)
                            {
                                client->gfx_TextBackgroundColor100 = bgcolor;
                                GFX_CalculateColors(gfxIndex);
                            }
                        }

                        switch(val)
                        {
                            case 1:
                                GFX_ColorSet(gfxIndex, client->gfx_TextForegroundColor25);
                                break;

                            case 2:
                                GFX_ColorSet(gfxIndex, client->gfx_TextForegroundColor75);
                                break;

                            case 3:
                                GFX_ColorSet(gfxIndex, client->gfx_TextForegroundColor100);
                                break;
                            default:
                                // should not happen since this switch statement
                                // is within if(val) statement.
                                break;
                        }

                        DRV_GFX_PixelPut(x, y);
                    }
                }
#else
                if(temp & mask)
                {
                    DRV_GFX_PixelPut(x, y);
                }
#endif
            } // end of if statement to check if in rendering area

#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
            mask  <<=  pParam->bpp;
            shift  +=  pParam->bpp;
#else
            mask <<= 1;
#endif
            x++;

        } // for(xCnt = 0; xCnt < pParam->chGlyphWidth; xCnt++)...

        y++;
    } // end of for(yCnt = 0; yCnt...

    // move cursor
    GFX_TextCursorPositionSet(gfxIndex,
                                (x - pParam->xAdjust - pParam->xWidthAdjust),
                                GFX_TextCursorPositionYGet(gfxIndex)
                             );

    // restore color
#ifndef GFX_CONFIG_FONT_ANTIALIASED_DISABLE
    if(pParam->bpp > 1)
    {
        GFX_ColorSet(gfxIndex, client->gfx_TextForegroundColor100);
    }
#endif        

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_TextCharDraw(
                                GFX_XCHAR ch
                               )

    Summary:
        This function renders the given character using the
        currently set color using the currently set font.

    Description:
        This function renders the given character using the currently set font,
        and color to the location defined by the text cursor position. The
        color is set by GFX_ColorSet() while the font is set by GFX_FontSet().
        The text cursor position is set by GFX_TextCursorPositionSet()

        The rendering of the character becomes undefined when any one of the
        following is true:
        - Text cursor position is set to locations outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_TextCharDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_XCHAR ch)
{

    typedef enum
    {
        TC_IDLE,
        TC_GET_INFO,
        TC_CHECK_RENDER_STATUS,
        TC_RENDER_CHAR,
        TC_RENDER_CHAR_DRIVER,
        TC_RENDER_STATUS_CHECK,
        TC_RENDER_NEWLINE_CHAR,
    } TEXTCHAR_RENDER_STATES;

    static TEXTCHAR_RENDER_STATES state = TC_IDLE, renderState;
    static GFX_FONT_OUTCHAR OutCharParam;
    static GFX_STATUS status;
    static GFX_STATUS_BIT statusBit;
    static GFX_RESOURCE_HDR *pFont;

    while(1)
    {
        switch (state)
        {
            case TC_IDLE:

                status = GFX_STATUS_SUCCESS_BIT;
                // check for error conditions
                // since we cannot predict that the font table is always valid we
                // cannot allow the program to hang. So intead of returning
                // GFX_STATUS_FAILURE we will return a GFX_STATUS_SUCCESS.
                // When the character is not in the table, it will just be
                // ignored and not printed.
                pFont = GFX_FontGet(gfxIndex);

                // check if new line character, if yes just go and adjust
                // the text cursor position.
                if (ch == (GFX_UXCHAR)0x0A)
                {
                   state = TC_RENDER_NEWLINE_CHAR;
                   break;
                }

                // make sure characters are printable
                if(pFont == NULL)
                    statusBit = GFX_STATUS_ERROR_BIT;
                else if((GFX_UXCHAR)ch < (GFX_UXCHAR)pFont->resource.font.header.firstChar)
                    statusBit = GFX_STATUS_ERROR_BIT;
                else if((GFX_UXCHAR)ch > (GFX_UXCHAR)pFont->resource.font.header.lastChar)
                    statusBit = GFX_STATUS_ERROR_BIT;
#ifdef  GFX_CONFIG_FONT_ANTIALIASED_DISABLE
                else if(pFont->resource.font.header.bpp > 1)
                    statusBit = GFX_STATUS_ERROR_BIT;
#endif
                if (statusBit == GFX_STATUS_ERROR_BIT)
                    return GFX_STATUS_SUCCESS;
                else
                    state = TC_GET_INFO;

                // no break here since the next state is what we want

            case TC_GET_INFO:

                // initialize variables
#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE
                OutCharParam.pChImage = NULL;
#endif
                OutCharParam.xAdjust = 0;
                OutCharParam.yAdjust = 0;
                OutCharParam.xWidthAdjust = 0;
                OutCharParam.heightOvershoot = 0;
                state = TC_CHECK_RENDER_STATUS;

                // no break here since the next state is what we want

            case TC_CHECK_RENDER_STATUS:

                if (GFX_RenderStatusCheck(gfxIndex) == GFX_STATUS_BUSY_BIT)
                {
                    state = TC_IDLE;
                    return (GFX_STATUS_FAILURE);
                }

                // get the character information
                switch (pFont->type)
                {

                    case GFX_RESOURCE_FONT_FLASH_NONE:
                        GFX_TextCharInfoFlashGet(gfxIndex, ch, &OutCharParam);
                        state = TC_RENDER_CHAR;
                        renderState = TC_RENDER_CHAR;
                        break;

                    case GFX_RESOURCE_FONT_EXTERNAL_NONE:
                        GFX_TextCharInfoExternalGet(gfxIndex, ch, &OutCharParam);
                        state = TC_RENDER_CHAR;
                        renderState = TC_RENDER_CHAR;
                        break;

                    case GFX_RESOURCE_FONT_EDS_NONE:
                        state = TC_RENDER_CHAR_DRIVER;
                        renderState = TC_RENDER_CHAR_DRIVER;
                        break;

                    default:
                        state = TC_IDLE;
                        return (GFX_STATUS_SUCCESS);
                } // end of switch

                break;

            case TC_RENDER_CHAR_DRIVER:

                // this is the case when the driver layer implements
                // code to render the characters

                status = GFX_DRV_TextCharRender(ch);
                state = TC_RENDER_STATUS_CHECK;
                break;

            case TC_RENDER_CHAR:

                status = GFX_TextCharRender(gfxIndex, ch, &OutCharParam);
                state = TC_RENDER_STATUS_CHECK;

                // no break here since the next state is what we want

            case TC_RENDER_STATUS_CHECK:

                // if the rendering of the character did not return with
                // GFX_STATUS_SUCCESS, application should call it again
                // and not re-do the initialization done on the previous
                // states above. So the state only changes to TC_IDLE
                // when it is a success or an error.
                if(status == GFX_STATUS_SUCCESS)
                {
                    state = TC_IDLE;
                }
                else
                {
                    state = renderState;
                }
                return (status);

            case TC_RENDER_NEWLINE_CHAR:
                // since the character is a newline character, just
                // adjust the text cursor position to the new line

                GFX_TextCursorPositionSet(gfxIndex,
                                GFX_TextCursorPositionXGet(gfxIndex),
                                (   
                                    GFX_TextCursorPositionYGet(gfxIndex) +
                                    pFont->resource.font.header.height
                                ));
                status = GFX_STATUS_SUCCESS;
                state = TC_IDLE;
                return (status);

            default:

                // this state should never happen
                state = TC_IDLE;
                return GFX_STATUS_FAILURE;
        } // end of switch ...
    } // end of while(1) ...

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_TextStringDraw(
                                uint16_t x,
                                uint16_t y,
                                GFX_XCHAR *pString,
                                uint16_t length)

    Summary:
        This function renders the given string of character using the
        currently set color using the currently set font.

    Description:
        This function renders the given string of character using the currently
        set font, and color to the location defined by the given x,y position.
        The color is set by GFX_ColorSet() while the font is set by
        GFX_FontSet().

        The text rendered by this function is always left aligned.
        When a newline character is encountered, the next character
        after the newline character will be rendered left aligned
        on the next line. The next line position is determined by the
        height of the font used.

        When the length parameter is set to 0, the string will be rendered
        until the null character is reached.
        When the length parameter is greater than 0, the string will be
        rendered until one of the following cases occurs:
        - null character is reached before the total rendered characters
          is less than the value of length
        - total rendered characters is equal to the value of length before
          the null character is reached.

        The rendering of the character becomes undefined when any one of the
        following is true:
        - x,y position is set to locations outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_TextStringDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                GFX_XCHAR *pString,
                                uint16_t length)
{

    static uint8_t      start = 1;
    static uint16_t     counter = 0;
           GFX_XCHAR    ch;
           GFX_STATUS   status;

    if(start)
    {
        GFX_TextCursorPositionSet(gfxIndex, x, y);
        start = 0;
    }


    while(1)
    {

        ch = *(pString + counter);

        if ( ((GFX_UXCHAR)ch == (GFX_UXCHAR)0x0A) ||
             ((GFX_UXCHAR)ch >  (GFX_UXCHAR)15  )
           )
        {
            status = GFX_TextCharDraw(gfxIndex, ch);

            if (status == GFX_STATUS_FAILURE)
            {
                break;
            }
            // all other status is ignored,
            // continue rendering the characters 

        }
        else if ((GFX_UXCHAR)ch == 0)
        {
            status = GFX_STATUS_SUCCESS;
            break;
        }
        // all other control characters will be ignored and continue until
        // string terminator is encountered

        // if rendering of that character is a success go to
        // the next character
        counter++;

        // check if the total rendered character has reached length
        // when length is 0, ignore length and use the string null
        // terminator instead to exit the function
        if ((length > 0) && (counter == length))
        {
            status = GFX_STATUS_SUCCESS;
            break;
        }

    }

    counter = 0;
    start = 1;

    return (status);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS FX_TextStringBoxDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                GFX_XCHAR *pString,
                                uint16_t length,
                                GFX_ALIGNMENT align)

    Summary:
        This function renders the given string using the currently set color
        and font into a rectangular area.

    Description:
        This function renders the given string using the currently
        color and font into the rectangular area defined by the given
        parameters x,y, width and height. The x,y parameters defines the
        left, top pixel position of the rectangular area. The width and
        height defines the size of the rectangular area. The rectangular
        area will define the area where the text will be rendered. Meaning
        all pixels EXCLUSIVE of the defined rectangle will be the rendering
        area for the text. If the given string exceeds the rectangular area,
        any pixels falling outside and along the defined rectangle (INCLUSIVE
        of the rectangle) will not be drawn.

        The color is set by GFX_ColorSet() while the font is set by
        GFX_FontSet().

        Multiple lines of strings can be rendered. To define string composed
        of multiple lines, end each line with a new line character
        (character ID 0x0A). Each line will be rendered according to the
        text alignment (See GFX_TEXT_ALIGNMENT).

        When the length parameter is set to 0, the string will be rendered
        until the null character is reached.
        When the length parameter is greater than 0, the string will be
        rendered until one of the following cases occurs:
        - null character is reached before the total rendered characters
          is less than the value of length
        - total rendered characters is equal to the value of length before
          the null character is reached.

        The rendering of the character becomes undefined when any one of the
        following is true:
        - x, y, width and height defines an area partially or fully outside
          outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_TextStringBoxDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                GFX_XCHAR *pString,
                                uint16_t length,
                                GFX_ALIGNMENT align)
{
    /*
     1. calculate the rectangular region (determine: left, top, right, bottom)
     2. parse the string to determine the number of lines
        check for number of new line characters on the string
        line count = # newline characters + 1
     3. based on the text alignment and dimension of the text box,
        determine the first line to be rendered.
     4. after knowing which line to start rendering start the rendering
     5. For each line:
        5.1 check the starting position of the line based on the text alignment
            and string width.
        5.2 for each character check if the character will be shown or not.
            This is done to skip processing the character that will not
            show up at all on the text box
        5.3 When a character is deemed to be shown even a pixel of it, process
            that character (for simplicity). Check if the pixel is to
            shown on the screen. If to be shown then render the pixel.
        5.4 continue for each character until we reach a character that will
            not be fully shown on the text box. (meaning beyond the box)
        5.5 If a newline character is sampled go to the next line.
            Then repeat step 5.
            If a string terminator is sampled, rendering is done.

     */

    typedef enum
    {
        TB_IDLE,
        TB_GET_LINE_INFO,
        TB_CHECK_NEWLINE_OR_END,
        TB_RENDER_CHARS,
        TB_EXIT,
    } TEXTSTRINGBOX_RENDER_STATES;

    static TEXTSTRINGBOX_RENDER_STATES state = TB_IDLE;
    static GFX_STATUS status;
    static uint16_t numLines;
    static GFX_XCHAR *pChar;
    static uint16_t  left, top, right, bottom;
    static uint16_t textHeight, textWidth, count;
    static bool ignoreCount;
    
    // note: these two are signed variables
    static int16_t  lineXPos, lineYPos;

    while(1)
    {
        switch (state)
        {
            case TB_IDLE:
                // initialize the dimensions
                left  = x;
                top   = y;
                if (length == 0)
                {
                    ignoreCount = true;
                }
                else
                {
                    ignoreCount = false;
                }

                right    = x + width  - 1;
                if (right > GFX_MaxXGet(gfxIndex))
                    right = GFX_MaxXGet(gfxIndex);

                bottom   = y + height - 1;
                if (bottom > GFX_MaxYGet(gfxIndex))
                        bottom = GFX_MaxYGet(gfxIndex);
                
                lineXPos = 0;
                lineYPos = 0;
                status   = GFX_STATUS_SUCCESS;

                // set the clipping area first
                GFX_TextAreaLeftSet(gfxIndex, left);
                GFX_TextAreaTopSet(gfxIndex, top);
                GFX_TextAreaRightSet(gfxIndex, right);
                GFX_TextAreaBottomSet(gfxIndex, bottom);

                // get the font height, width will be used to get the
                // length of each line
                textHeight = GFX_TextStringHeightGet(GFX_FontGet(gfxIndex));

                // search for new line characters, if none is found
                // the number of lines is 1.
                numLines = 1;
                pChar = pString;
                count = 0;

                while(*(pChar+count) != 0)
                {
                    if (*(pChar+count) == 0x0A)
                        numLines++;
                    count++;
                    if (ignoreCount == false)
                    {
                        if (count == length)
                            break;
                    }
                }

                // Calculate the location of the first line to be rendered.
                // The value is dependent on the vertical alignment of the text and the
                // number of lines
                switch (align & GFX_ALIGN_VERTICAL_MASK)
                {
                    case GFX_ALIGN_TOP:
                        // text is aligned top, render the first line
                        // immediately after the top location
                        lineYPos = top + 1;
                        break;
                    case GFX_ALIGN_BOTTOM:
                        // text is aligned bottom, starting position of the
                        // first line is calculated from the bottom position subtracted by
                        // the (number of lines * height) of the string
                        lineYPos = (int16_t)(bottom - (textHeight * numLines));
                        break;
                    case GFX_ALIGN_VCENTER:
                    default:
                        // use GFX_TEXT_ALIGN_VCENTER as default if not set

                        // text is aligned vertical center, starting position of the
                        // first line is dependent on the number of lines and height
                        // of the font
                        lineYPos = (int16_t)(top + ((height - 1) >> 1)) -  \
                                   (int16_t)((textHeight * numLines) >> 1);
                        break;
                };

                state = TB_GET_LINE_INFO;

                // no break here since the next state is what we want

            case TB_GET_LINE_INFO:

                // get the width of the current string
                textWidth = GFX_TextStringWidthGet(pString, GFX_FontGet(gfxIndex));
                
                if(textWidth == 0)
                {
                    state = TB_CHECK_NEWLINE_OR_END;
                    
                    break;
                }
                
                // calculate the starting position of the current
                // string in the horizntal direction

                switch(align & GFX_ALIGN_HORIZONTAL_MASK)
                {
                    case GFX_ALIGN_LEFT:
                        // text is aligned left, render the first character
                        // immediately after the left location
                        lineXPos = left + 1;
                        break;
                    case GFX_ALIGN_RIGHT:
                        // text is aligned right, starting position of the
                        // first character is calculated from the right position
                        // subtracted by the width of the string
                        lineXPos = (int16_t)(right - textWidth);
                        break;
                    case GFX_ALIGN_HCENTER:
                    default:
                        // use GFX_TEXT_ALIGN_HCENTER as default when not set
                        
                        // text is aligned horizontal center, starting position of
                        // the first character is dependent on the width of the string
                        // and the width of the box.
                        lineXPos = (int16_t)((left + (width >> 1)) - (textWidth >> 1));
                        break;
                }

                GFX_TextCursorPositionSet(gfxIndex, lineXPos, lineYPos);
                pChar = pString;

                count = 0;
                state = TB_CHECK_NEWLINE_OR_END;

                // no break here since the next state is what we want

            case TB_CHECK_NEWLINE_OR_END:

                // since this function needs to know when the current line
                // ends, it implements its own GFX_TextStringDraw() function

                if (ignoreCount == false)
                {
                    if (count == length)
                    {
                    // end of string
                    state = TB_EXIT;
                    status = GFX_STATUS_SUCCESS;
                    break;
                    }
                }

                if (*pChar == 0x0A)
                {
                    pString = ++pChar;
                    count++;
                    lineYPos += textHeight;
                    state = TB_GET_LINE_INFO;
                    break;
                }
                else if ((GFX_UXCHAR)*pChar == 0)
                {
                    // end of string
                    state = TB_EXIT;
                    status = GFX_STATUS_SUCCESS;
                    break;
                }
                else if ((GFX_UXCHAR)*pChar <  (GFX_UXCHAR)15)
                {
                    pChar++;
                    count++;
                    state = TB_CHECK_NEWLINE_OR_END;
                    break;
                }
                
                state = TB_RENDER_CHARS;

                // no break here since the next state is what we want

            case TB_RENDER_CHARS:

                status = GFX_TextCharDraw(gfxIndex, *pChar);
                if (status == GFX_STATUS_FAILURE)
                {
                    return (GFX_STATUS_FAILURE);
                }
                // all other status is ignored,
                // continue rendering the characters

                // if rendering of that character is a success go to
                // the next character
                pChar++;
                count++;
                state = TB_CHECK_NEWLINE_OR_END;
                break;

            case TB_EXIT:

                // set the clipping back to full screen
                GFX_TextAreaLeftSet(gfxIndex, 0);
                GFX_TextAreaTopSet(gfxIndex, 0);
                GFX_TextAreaRightSet(gfxIndex, GFX_MaxXGet(gfxIndex));
                GFX_TextAreaBottomSet(gfxIndex, GFX_MaxYGet(gfxIndex));
                state = TB_IDLE;
                return (status);

            default:
                // this should never happen
                break;

        } // end of switch...
    } // end of while(1)...

}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextStringWidthGet(
                                GFX_XCHAR* pString,
                                GFX_RESOURCE_HDR *pFont);

    Summary:
        This function returns the width of the given string using the
        given font.

    Description:
        This function returns the width of the given string using the
        given font in pixels. The given font must be present in the system.

        The width is calculated by adding the width of each character in
        the string sampled from the first character until control character
        is sampled. The control characters are those characters with ID that
        are less than 15 (0x0F). This includes the new line character
        (9 or 0x0A).

        Therefore, using this function to find the length of a string with
        a new line character included in the string, the length returned
        will be the first line of the string. If the string used do not
        have a new line character and is a terminated string, the length
        returned is the actual length of the string.

        This function return value is undefined if the given pointer does
        not point to a valid font or one or more characters in the given
        string does not exist on the given font. The function will also
        return an undefined value if the string has no new line
        character and not terminated.
    
*/
// *****************************************************************************
uint16_t GFX_TextStringWidthGet(GFX_XCHAR *pString, GFX_RESOURCE_HDR *pFont)
{
    if (pFont == NULL)
        return 1; //If there is no font, there are bigger problems than return 1
                  //We return 1 here to avoid division by 0 crashes
    
    switch(*((int16_t *)pFont))
    {
        case GFX_RESOURCE_FONT_RAM_NONE:
            return GFX_TextStringWidthRamGet(pString, pFont);

        case GFX_RESOURCE_FONT_FLASH_NONE:
            return GFX_TextStringWidthFlashGet(pString, pFont);

        case GFX_RESOURCE_FONT_EXTERNAL_NONE:
            return GFX_TextStringWidthExternalGet(pString, pFont);

        case GFX_RESOURCE_FONT_EDS_NONE:
            return GFX_DRV_TextStringWidthGet(pString, pFont);

        default:
            return (0);
    }
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextStringWidthRamGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Calculates and returns the string width in pixels. The string length
        is based on the given font located in RAM.


*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_TextStringWidthRamGet(
                                GFX_XCHAR* pString,
                                GFX_RESOURCE_HDR* pFont)
{
#ifndef GFX_CONFIG_FONT_RAM_DISABLE
    GFX_FONT_GLYPH_ENTRY *pChTable = NULL;
    GFX_FONT_GLYPH_ENTRY_EXTENDED *pChTableExtended = NULL;
    GFX_FONT_HEADER *pHeader;

    int16_t       textWidth;
    GFX_XCHAR       ch;
    GFX_XCHAR       fontFirstChar;
    GFX_XCHAR       fontLastChar;

    pHeader = (GFX_FONT_HEADER *) ((FONT_RAM *)pFont)->address;
    fontFirstChar = pHeader->firstChar;
    fontLastChar = pHeader->lastChar;
    if(pHeader->extendedGlyphEntry)
    {
        pChTableExtended = (GFX_FONT_GLYPH_ENTRY_EXTENDED *) (pHeader + 1);
    }
    else
    {
        pChTable = (GFX_FONT_GLYPH_ENTRY *) (pHeader + 1);
    }    
    textWidth = 0;
    while((GFX_UXCHAR)15 < (GFX_UXCHAR)(ch = *pString++))
    {
        if((GFX_UXCHAR)ch < (GFX_UXCHAR)fontFirstChar)
            continue;
        if((GFX_UXCHAR)ch > (GFX_UXCHAR)fontLastChar)
            continue;
        if(pHeader->extendedGlyphEntry)
        {
            textWidth += (pChTableExtended + ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar))->cursorAdvance;
        }
        else
        {
            textWidth += (pChTable + ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar))->width;
        }
    }
    return textWidth;

#else

    return 0;
    
#endif //#ifndef GFX_CONFIG_FONT_RAM_DISABLE

}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextStringWidthFlashGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Calculates and returns the string width in pixels. The string length
        is based on the given font located in flash memory.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_TextStringWidthFlashGet(
                                GFX_XCHAR* pString,
                                GFX_RESOURCE_HDR *pFont)
{
#ifndef GFX_CONFIG_FONT_FLASH_DISABLE
    GFX_FONT_SPACE  GFX_FONT_GLYPH_ENTRY *pChTable = NULL;
    GFX_FONT_SPACE  GFX_FONT_GLYPH_ENTRY_EXTENDED *pChTableExtended = NULL;
    GFX_FONT_SPACE  GFX_FONT_HEADER *pHeader;

    uint16_t        textWidth;
    GFX_XCHAR       ch;
    uint16_t       fontFirstChar;
    uint16_t       fontLastChar;

    pHeader = (GFX_FONT_SPACE GFX_FONT_HEADER *)&pFont->resource.font.header.fontID;//resource.font.location.progByteAddress;
    fontFirstChar = pFont->resource.font.header.firstChar;
    fontLastChar = pFont->resource.font.header.lastChar;
    if(pHeader->extendedGlyphEntry)
    {
        pChTableExtended = (GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY_EXTENDED *) (pHeader + 1);
    }
    else
    {
        pChTable = (GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY *) (pFont->resource.font.location.progByteAddress + 8);
    }    
    
    
    
    textWidth = 0;
    while((GFX_UXCHAR)15 < (GFX_UXCHAR)(ch = *pString++))
    {
        if((GFX_UXCHAR)ch < (GFX_UXCHAR)fontFirstChar)
            continue;
        if((GFX_UXCHAR)ch > (GFX_UXCHAR)fontLastChar)
            continue;
        if(pHeader->extendedGlyphEntry)
        {
            textWidth += (pChTableExtended + ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar))->cursorAdvance;
        }
        else
        {
            textWidth += (pChTable + ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar))->width;
        }
    }

    return (textWidth);

#else
    return 0;
#endif 

}

// *****************************************************************************
/*  Function:
    uint16_t GFX_TextStringWidthExternalGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Calculates and returns the string width in pixels. The string length
        is based on the given font located in external memory.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_TextStringWidthExternalGet(
                                GFX_XCHAR* pString,
                                GFX_RESOURCE_HDR* pFont)
{

#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE
    GFX_FONT_GLYPH_ENTRY chTable;
    GFX_FONT_GLYPH_ENTRY_EXTENDED chTableExtended;
    GFX_FONT_HEADER header;

    uint16_t        textWidth;
    GFX_XCHAR       ch;
    GFX_XCHAR       fontFirstChar;
    GFX_XCHAR       fontLastChar;


    GFX_ExternalResourceCallback((void*)pFont, 0, sizeof(GFX_FONT_HEADER), &header);
    fontFirstChar = header.firstChar;
    fontLastChar = header.lastChar;
    textWidth = 0;
    while((GFX_UXCHAR)15 < (GFX_UXCHAR)(ch = *pString++))
    {
        if((GFX_UXCHAR)ch < (GFX_UXCHAR)fontFirstChar)
            continue;
        if((GFX_UXCHAR)ch > (GFX_UXCHAR)fontLastChar)
            continue;
        if(header.extendedGlyphEntry)
        {
            GFX_ExternalResourceCallback
            (
                (void*)pFont,
                sizeof(GFX_FONT_HEADER) + sizeof(GFX_FONT_GLYPH_ENTRY_EXTENDED) * ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar),
                sizeof(GFX_FONT_GLYPH_ENTRY_EXTENDED),
                &chTableExtended
            );
            textWidth += chTableExtended.cursorAdvance;
        }
        else
        {
            GFX_ExternalResourceCallback
            (
                (void*)pFont,
                sizeof(GFX_FONT_HEADER) + sizeof(GFX_FONT_GLYPH_ENTRY) * ((GFX_UXCHAR)ch - (GFX_UXCHAR)fontFirstChar),
                sizeof(GFX_FONT_GLYPH_ENTRY),
                &chTable
            );
            textWidth += chTable.width;
        }
    }

    return (textWidth);
#else
    return 0;
#endif //#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE
    
}

// *****************************************************************************
/*  
    Function:
       GFX_STATUS GFX_TextCursorTypeSet ( SYS_MODULE_INDEX     gfxIndex,
                                          GFX_TEXT_CURSOR_TYPE textCursorType )
  
    Summary:
       Set the type of text cursor to be drawn.

    Description:
       This function sets the type of text cursor to be drawn.

    Conditions:
       Gfx Library to be initialized successfully with instance same as gfxIndex.

    Input:
      gfxIndex - Gfx library instance index.
      textCursorType - Type of text cursor to be drawn.

    Return:
      GFX_STATUS_SUCCESS - Operation successful.
      GFX_STATUS_FAILURE - Operation failed.  

    Example:
      None.

    Remarks:
      This function is an internal function and should not be called by the 
      application.

*/
// *****************************************************************************

GFX_STATUS __attribute__ ((weak)) GFX_TextCursorTypeSet
(
    SYS_MODULE_INDEX    gfxIndex,
    GFX_TEXT_CURSOR_TYPE textCursorType
)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if( textCursorType >= GFX_TEXT_CURSOR_TYPE_NONE && 
        textCursorType < GFX_TEXT_CURSOR_TYPE_MAX )
    {
        client->gfxTextCursorType = textCursorType;
        return (GFX_STATUS_SUCCESS);
    }
    else
    {
        return (GFX_STATUS_FAILURE);
    }
}

// *****************************************************************************
/*  
    Function: 
      GFX_TEXT_CURSOR_TYPE GFX_TextCursorTypeGet ( SYS_MODULE_INDEX gfxIndex )

    Summary:
      Get the type of text cursor to be drawn.        

    Description:
      This function gets the type of text cursor to be drawn.

    Conditions:
      Gfx Library to be initialized successfully with instance same as gfxIndex.

    Input:
      gfxIndex - Gfx library instance index.

    Return:
      GFX_TEXT_CURSOR_TYPE_NONE - Text cursor type none.
      GFX_TEXT_CURSOR_TYPE_BAR - Text cursor type bar.
      GFX_TEXT_CURSOR_TYPE_BLOCK - Text cursor type block.
      GFX_TEXT_CURSOR_TYPE_UNDERSCORE - Text cursor type underscore.
       
    Example:
      None.                                                                 

    Remarks:
      This function is an internal function and should not be called by the 
      application.
*/
// *****************************************************************************

GFX_TEXT_CURSOR_TYPE __attribute__ ((weak)) GFX_TextCursorTypeGet
(
    SYS_MODULE_INDEX    gfxIndex
)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    
    return (client->gfxTextCursorType);
}

// *****************************************************************************
/*  
    Function:
      int16_t GFX_TextCursorWidthGet( SYS_MODULE_INDEX gfxIndex )
  
    Summary:
      Get the text cursor width.

    Description:
      This function gets text cursor width based on font set.

    Conditions:
      Gfx Library to be initialized successfully with instance same as gfxIndex.

    Input:
      gfxIndex - Gfx library instance index.

    Return:
      returns width of the cursor based on font set.  

    Example:
      None.                                                                 

*/
// *****************************************************************************

int16_t __attribute__ ((weak)) GFX_TextCursorWidthGet( SYS_MODULE_INDEX gfxIndex )
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    GFX_FONT_OUTCHAR OutCharParam = {0};
    
    switch (client->pGfxCurrentFont->type)
    {
        case GFX_RESOURCE_FONT_FLASH_NONE:
        {
            GFX_TextCharInfoFlashGet(gfxIndex, '_', &OutCharParam);
            return (OutCharParam.chGlyphWidth);
            break;
        }
            
        case GFX_RESOURCE_FONT_EXTERNAL_NONE:
        {
            GFX_TextCharInfoExternalGet(gfxIndex, '_', &OutCharParam);
            return(OutCharParam.chGlyphWidth);
            break;
        }
        
        default:
        {
            return (GFX_TEXT_CURSOR_WIDTH_DEFAULT);
            break;
        }
        
    } // end of switch
  
}

// *****************************************************************************
/*  
    Function:
      GFX_STATUS __attribute__ ((weak)) GFX_TextCursorDraw ( 
                                                        SYS_MODULE_INDEX gfxIndex,
                                                        uint16_t x,
                                                        uint16_t y,
                                                        uint16_t cursorWidth,
                                                        uint16_t cursorHeight )
  
    Summary:
      Draw the text Cursor.

    Description:
      This function draws the text cursor.

    Conditions:
      Gfx Library to be initialized successfully with instance same as gfxIndex.  

    Input:
      gfxIndex - Gfx library instance index.
      x - x coordinate of cursor start point.
      y - y coordinate of cursor start point.
      cursorWidth - width of cursor.
      cursorHeight - height of cursor.

    Return:
      GFX_STATUS_SUCCESS - Operation successful.
      GFX_STATUS_FAILURE - Operation failed.

    Example:
      None.                                                                 

*/
// *****************************************************************************

GFX_STATUS __attribute__ ((weak)) GFX_TextCursorDraw( SYS_MODULE_INDEX gfxIndex,
                                                      uint16_t x,
                                                      uint16_t y,
                                                      uint16_t cursorWidth,
                                                      uint16_t cursorHeight )
{
    
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    
    switch(client->gfxTextCursorType)
    {
        case GFX_TEXT_CURSOR_TYPE_NONE:
        {
            break;
        }
        
        case GFX_TEXT_CURSOR_TYPE_BAR:
        {
            GFX_LineDraw(gfxIndex, x, y, x, y + cursorHeight );
            break;
        }
    
        case GFX_TEXT_CURSOR_TYPE_BLOCK:
        {
            GFX_BarDraw( gfxIndex, x, y, 
                        x + cursorWidth, y + cursorHeight );
            break;
        }
        
        case GFX_TEXT_CURSOR_TYPE_UNDERSCORE:
        {
            GFX_LineDraw( gfxIndex, x, y + cursorHeight, 
                          x + cursorWidth, y + cursorHeight );
            break;
        }
        
        default:
        {
            break;
        }
    }
    
    return (GFX_STATUS_SUCCESS);
}


// *****************************************************************************
/*  Function:
    uint16_t GFX_DRV_TextStringWidthGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function is intended to allow driver level implementation of
        the string length calculation routine. In some cases, hardware
        is present to perform rendering and help useful Graphics Library
        routines. When this is present, we take advantage by implementing
        some primitive layer routines in the driver code.
        This function is implemented here as a dummy function and is
        declared with weak attributes. When the driver implements this
        function, the one in the driver will be used instead.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_DRV_TextStringWidthGet(GFX_XCHAR* textString, GFX_RESOURCE_HDR* pFont)
{
    return 0;
}

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
// *****************************************************************************
/*  Function:
    void GFX_ImageFlash1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 1bpp image stored in internal flash memory.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageFlash1BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    register uint8_gfx_image_prog   *flashAddress, *pTempFlashAddress;
    int16_t                         x, y;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        sizeX, sizeY;
    uint8_t                         temp = 0, mask;
    uint16_t                        palette[2];
    uint16_t                        addressOffset = 0, adjOffset;
    uint16_t                        OffsetFlag = 0x01;     //Offset from byte color bit0 for the partial image
    GFX_COLOR                       useColor;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer image data
    flashAddress = pImage->resource.image.location.progByteAddress;

    palette[0] = *((uint16_gfx_image_prog *)flashAddress);
    flashAddress += 2;
    palette[1] = *((uint16_gfx_image_prog *)flashAddress);
    flashAddress += 2;

    if (bitmapHdr.width & 0x07)
        adjOffset = 1;
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset needed to jump to the
    // next line
    addressOffset = ((bitmapHdr.width >> 3) + adjOffset);

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        
        // adjust the flashAddress to the starting pixel location
        // adjust one address if the data is not bytes aligned
        flashAddress += ((uint16_gfx_image_prog)pPartialImageData->yoffset*((bitmapHdr.width >> 3) + adjOffset));
        // adjust flashAddress for x offset (if xoffset is zero address stays the same)
        flashAddress += ((pPartialImageData->xoffset) >> 3);

        OffsetFlag <<= ((pPartialImageData->xoffset) & 0x07);;
        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width;
    }

    // store current line data address
    pTempFlashAddress = flashAddress;

    for(y = 0; y < sizeY; )
    {
        // get flash address location of current line being processed
        flashAddress = pTempFlashAddress;

        if (OffsetFlag != 0x01)
        {
            // grab the first set of data then set up the mask to the starting pixel
            temp = *flashAddress++;
        }
        mask = OffsetFlag;


#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            // read 8 pixels from flash
            if(mask == 0x01)
            {
                temp = *flashAddress++;
            }

            // decide which color to use
            if (mask & temp)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = 1;
#else
                useColor = palette[1];
#endif
            }
            else
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = 0;
#else
                useColor = palette[0];
#endif
            }

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

            // adjust the mask
            if(mask == 0x80)
                mask = 0x01;
            else
                // Shift to the next pixel
                mask <<= 1;

        }

        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        }

        y++;

        pTempFlashAddress += (addressOffset);
       
    }
}

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 4bpp image stored in internal flash memory.

*/
// *****************************************************************************
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
void __attribute__((weak)) GFX_ImageFlash4BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    register uint8_gfx_image_prog   *flashAddress, *pTempFlashAddress;
    int16_t                         x, y;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        sizeX, sizeY;
    uint16_t                        temp, mask;
    uint16_t                        addressOffset = 0, adjOffset, nibbleOffset = 0x00;

    uint16_t                        palette[16], counter;

    GFX_COLOR                       useColor;


    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to image data
    flashAddress = pImage->resource.image.location.progByteAddress;

    // Read palette
    for(counter = 0; counter < 16; counter++)
    {
        palette[counter] = *((uint16_gfx_image_prog *)flashAddress);
        flashAddress += 2;
    }

    // this takes care of the padding
    if (bitmapHdr.width & 0x01)
        adjOffset = 1;
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset needed to jump to the
    // next line
    addressOffset = ((bitmapHdr.width >> 1) + adjOffset);

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {

        nibbleOffset = (pPartialImageData->xoffset) & 0x01;
      
        // adjust the flashAddress to the starting pixel location
        // adjust one address if the data is not nibble aligned
        flashAddress += ((uint16_gfx_image_prog)pPartialImageData->yoffset*addressOffset);
        // adjust flashAddress for x offset (if xoffset is zero address stays the same)
        flashAddress += ((pPartialImageData->xoffset - nibbleOffset) >> 1);

        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width + nibbleOffset;
    }

    // store current line data address
    pTempFlashAddress = flashAddress;

    // Note: For speed the code for loops are repeated. A small code size increase for performance
    for(y = 0; y < sizeY; )
    {
        // get flash address location of current line being processed
        flashAddress = pTempFlashAddress;
        mask = nibbleOffset;

        // grab the first set of data then set up the mask to the starting pixel
        temp = *flashAddress;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            // Read 2 pixels from flash
            if(mask & 0x01)
            {
                // second pixel in byte
                // Set color
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = temp >> 4;
#else
                useColor = palette[temp >> 4];
#endif
                flashAddress++;
            }
            else
            {
                temp = *flashAddress;
                // first pixel in byte
                // Set color
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = temp & 0x0f;
#else
                useColor = palette[temp & 0x0f];
#endif
            }

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

            // adjust the mask
            if(mask == 0x01)
                mask = 0x00;
            else
                mask += 1;
        }

        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        }
        y++;

        pTempFlashAddress += (addressOffset);
    }

}
#endif // #if (COLOR_DEPTH >= 4)

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 8bpp image stored in internal flash memory.

*/
// *****************************************************************************
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
void __attribute__((weak)) GFX_ImageFlash8BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    register uint8_gfx_image_prog   *flashAddress;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    int16_t                         x, y;
    uint16_t                        sizeX, sizeY;
    uint8_t                         temp;
    uint16_t                        palette[256];
    uint16_t                        counter;
    uint16_t                        addressOffset = 0;

   GFX_COLOR                        useColor;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to size information
    flashAddress = pImage->resource.image.location.progByteAddress;

    // Read palette
    for(counter = 0; counter < 256; counter++)
    {
        palette[counter] = *((uint16_gfx_image_prog *)flashAddress);
        flashAddress += 2;
    }

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {

         flashAddress += ((uint16_gfx_image_prog)pPartialImageData->yoffset*(bitmapHdr.width));
         flashAddress += pPartialImageData->xoffset;

         addressOffset = bitmapHdr.width - pPartialImageData->width;
         sizeY = pPartialImageData->height;
         sizeX = pPartialImageData->width;
    }

    for(y = 0; y < sizeY; )
    {
#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = *flashAddress++;

            // Set color
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = temp;
            #else
                useColor = palette[temp];
            #endif

            #ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
                client->gfxLineBuffer0[x--] = useColor;
            #else
                client->gfxLineBuffer0[x++] = useColor;
            #endif

        }

        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render the current line
            GFX_PixelArrayPut(
                    gfxIndex,
                    left,
                    top + y,
                    client->gfxLineBuffer0,
                    sizeX, 1);
        }

        y++;

        flashAddress += addressOffset;
    }

}
#endif // #if (COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash16BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 16bpp image stored in internal flash memory.

*/
// *****************************************************************************
#if (GFX_CONFIG_COLOR_DEPTH == 16)
void __attribute__((weak)) GFX_ImageFlash16BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    register uint16_gfx_image_prog  *flashAddress;
    int16_t                         x, y;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        sizeX, sizeY;
    uint16_t                        temp;
    uint16_t                        addressOffset = 0;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to size information
    flashAddress = pImage->resource.image.location.progWordAddress;

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        flashAddress += ((uint16_gfx_image_prog)(pPartialImageData->yoffset)*sizeX);
        flashAddress += pPartialImageData->xoffset;
        
        addressOffset = sizeX - pPartialImageData->width;
        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width;
    }

    for(y = 0; y < sizeY; )
    {
#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = *flashAddress++;
#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = temp;
#else
            client->gfxLineBuffer0[x++] = temp;
#endif
        }
                    
        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render the current line
            GFX_PixelArrayPut(
                                gfxIndex,
                                left,
                                top + y,
                                client->gfxLineBuffer0,
                                sizeX, 1);
        }

        y++;
        flashAddress += addressOffset;
    }

}

#endif //#if (COLOR_DEPTH == 16)
#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_ImageExternal1BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 1bpp image stored in external memory.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageExternal1BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{

    register uint32_t               memOffset;
    int16_t                         x, y;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint8_t                         lineBuffer[((GFX_MaxXGet(gfxIndex) + 1) / 8) + 1];
    uint8_t                         *pData;
    uint16_t                        lineLength;

    uint16_t                        sizeX, sizeY;
    uint8_t                         temp = 0, mask;
    uint16_t                        palette[2];
    uint16_t                        addressOffset = 0, adjOffset;
    //Offset from byte color bit0 for the partial image
    uint16_t                        OffsetFlag = 0x01;
    GFX_COLOR                       useColor;
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);


    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // ignore the palette since system palette is used
            memOffset = 2 * (sizeof(uint16_t));
        }
        else
#endif
        {
            // Get palette (2 entries)
            GFX_ExternalResourceCallback(
                    pImage,
                    0,
                    2 * sizeof(uint16_t),
                    palette);
            // Set offset to the image data (header size + palette
            memOffset = 2 * (sizeof(uint16_t));
        }
    }
    else
    {
        // palette is stripped so skip the ID
        memOffset = sizeof(uint16_t);
    }

    if (bitmapHdr.width & 0x07)
        adjOffset = 1;
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset 
    // needed to jump to the next line
    addressOffset = ((bitmapHdr.width >> 3) + adjOffset);

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        memOffset += ((uint32_t)pPartialImageData->yoffset * addressOffset);
        memOffset += (pPartialImageData->xoffset) >> 3;

        OffsetFlag <<= ((pPartialImageData->xoffset) & 0x07);

        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width;
    }

    // calculate the length of bytes needed per line
    lineLength = (sizeX >> 3) + 1;
    if (sizeX & 0x07)
        lineLength++;

    for(y = 0; y < sizeY; )
    {
        // Get a line
        GFX_ExternalResourceCallback(
                pImage,
                memOffset,
                lineLength,
                lineBuffer);
        memOffset += addressOffset;
        pData = lineBuffer;

        if (OffsetFlag != 0x01)
        {
            // grab the first set of data then set
            // up the mask to the starting pixel
            temp = *pData++;
        }
        mask = OffsetFlag;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            // read 8 pixels from flash
            if(mask == 0x01)
            {
                temp = *pData++;
            }

            // decide which color to use
            if (mask & temp)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = 1;
#else
                useColor = palette[1];
#endif
            }
            else
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = 0;
#else
                useColor = palette[0];
#endif
            }

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

            // adjust the mask
            if(mask == 0x80)
                mask = 0x01;
            else
                // Shift to the next pixel
                mask <<= 1;

        }
        
        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render a copy of the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        }
        y++;

    }

}

// *****************************************************************************
/*  Function:
    void GFX_ImageExternal4BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 4bpp image stored in external memory.

*/
// *****************************************************************************
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
void __attribute__((weak)) GFX_ImageExternal4BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{

    register uint32_t               memOffset;
    int16_t                         x, y;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint8_t                         lineBuffer[((GFX_MaxXGet(gfxIndex) + 1) / 2) + 1];
    uint8_t                         *pData;
    uint16_t                        lineLength;

    uint16_t                        sizeX, sizeY;
    uint16_t                        temp, mask;
    uint16_t                        addressOffset = 0, adjOffset;
    uint16_t                        nibbleOffset = 0x00;
    uint16_t                        palette[16];
    GFX_COLOR                       useColor;
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Read palette
    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // address MUST be skipped since the palette is 
            // embedded in the image but since the system
            // CLUT is enabled, we will ignore the palette
            memOffset = 16 * (sizeof(uint16_t));
        }
        else
#endif
        {
            // Get palette (16 entries)
            GFX_ExternalResourceCallback(
                    pImage,
                    0,
                    16 * sizeof(uint16_t),
                    palette);
            // Set offset to the image data
            memOffset = 16 * (sizeof(uint16_t));
        }

    }
    else
    {
        // palette is stripped so skip the ID
        memOffset = sizeof(uint16_t);
    }

    // this takes care of the padding
    if (bitmapHdr.width & 0x01)
        adjOffset = 1;
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset 
    // needed to jump to the next line
    addressOffset = ((bitmapHdr.width >> 1) + adjOffset);

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {

        nibbleOffset = (pPartialImageData->xoffset) & 0x01;

        memOffset += ((uint32_t)(pPartialImageData->yoffset) * addressOffset);
        memOffset += (pPartialImageData->xoffset) >> 1;

        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width + nibbleOffset;
    }

    // calculate the length of bytes needed per line
    lineLength = (sizeX >> 1) + 1;
    if (sizeX & 0x01)
        lineLength++;
    
    // Note: For speed the code for loops are repeated.
    //       A small code size increase for performance
    for(y = 0; y < sizeY; )
    {

        // Get line
        GFX_ExternalResourceCallback(
                pImage,
                memOffset,
                lineLength,
                lineBuffer);
        memOffset += addressOffset;
        pData = lineBuffer;
        mask = nibbleOffset;

        temp = *pData;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            // Read 2 pixels 
            if(mask & 0x01)
            {
                // second pixel in byte
                // Set color
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = temp >> 4;
#else
                useColor = palette[temp >> 4];
#endif
                pData++;
            }
            else
            {
                temp = *pData;
                // first pixel in byte
                // Set color
#ifndef GFX_CONFIG_PALETTE_DISABLE
                useColor = temp & 0x0f;
#else
                useColor = palette[temp & 0x0f];
#endif
            }

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

            // adjust the mask
            if(mask == 0x01)
                mask = 0x00;
            else
                mask += 1;
        }
        
        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            // render a copy of the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        }
        y++;

    }

}

#endif

#if (GFX_CONFIG_COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    void GFX_ImageExternal8BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 8bpp image stored in external memory.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageExternal8BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{

    register uint32_t               memOffset;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint8_t                         lineBuffer[(GFX_MaxXGet(gfxIndex) + 1)];
    uint8_t                         *pData;
    uint16_t                        lineLength;

    int16_t                         x, y;
    uint16_t                        sizeX, sizeY;
    uint8_t                         temp;
    uint16_t                        palette[256];
    uint16_t                        addressOffset = 0;

   GFX_COLOR                        useColor;
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // address MUST be skipped since the palette 
            // is embedded in the image but since the system
            // CLUT is enabled, we will ignore the palette
            memOffset = sizeof(256 * sizeof(uint16_t));
        }
        else
#endif
        {
            // Get palette (256 entries)
            GFX_ExternalResourceCallback(
                    pImage,
                    0,
                    256 * sizeof(uint16_t),
                    palette);
            // Set offset to the image data
            memOffset = 256 * sizeof(uint16_t);
        }
    }
    else
    {
        // palette is stripped so skip the id
        memOffset = sizeof(uint16_t);
    }

    // compute for addressOffset this is the offset 
    // needed to jump to the next line
    addressOffset = bitmapHdr.width;

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
         memOffset += ((uint32_t)pPartialImageData->yoffset*(GFX_ImageWidthGet(pImage)));
         memOffset += pPartialImageData->xoffset;
         sizeY = pPartialImageData->height;
         sizeX = pPartialImageData->width;
    }

    // calculate the length of bytes needed per line
    // (not equal to image width when redering partial image)
    lineLength = sizeX;

    for(y = 0; y < sizeY; )
    {

        // Get line
        GFX_ExternalResourceCallback(
                pImage,
                memOffset,
                lineLength,
                lineBuffer);
        memOffset += addressOffset;

        pData = lineBuffer;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = *pData++;

            // Set color
#ifndef GFX_CONFIG_PALETTE_DISABLE
            useColor = temp;
#else
            useColor = palette[temp];
#endif

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

        }

        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
            // render a copy of the current line
//            gfxDrv->PixelArrayPut(client->gfxLineBuffer0, left, top + y, sizeX, 1);
        }

        y++;
    }
}
#endif

// *****************************************************************************
/*  Function:
    void GFX_ImageExternal16BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 16bpp image stored in external memory.

*/
// *****************************************************************************
#if (GFX_CONFIG_COLOR_DEPTH == 16)
void __attribute__((weak)) GFX_ImageExternal16BPPDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{

    register uint32_t               memOffset;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        lineBuffer[(GFX_MaxXGet(gfxIndex) + 1)];
    uint16_t                        *pData;
    uint16_t                        lineLength;

    int16_t                         x, y;
    uint16_t                        sizeX, sizeY;
    uint16_t                        temp;
    uint16_t                        addressOffset = 0;
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Set offset to the image data
    memOffset = 0;

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    // compute for addressOffset this is the offset needed to jump to the
    // next line
    addressOffset = bitmapHdr.width << 1;

    if(pPartialImageData->width != 0)
    {
         memOffset += ( (uint32_t)pPartialImageData->xoffset +
                        (uint32_t)pPartialImageData->yoffset *
                                  (GFX_ImageWidthGet(pImage))) << 1;
         sizeY = pPartialImageData->height;
         sizeX = pPartialImageData->width;
    }

    // calculate the length of bytes needed per line
    // (not equal to image width when redering partial image)
    lineLength = sizeX << 1;

    for(y = 0; y < sizeY; )
    {

        // Get line
        GFX_ExternalResourceCallback(
                pImage,
                memOffset,
                lineLength,
                lineBuffer);
        memOffset += addressOffset;

        pData = lineBuffer;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = (sizeX - 1); x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = *pData++;
#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = temp;
#else
            client->gfxLineBuffer0[x++] = temp;
#endif
        }
        
        if (GFX_RenderToDisplayBufferDisableFlagGet(gfxIndex) == 1)
            return;
        else
        {

            // render a copy of the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        }
        y++;

    }

}

#endif
#endif // #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ImagePartialDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t destination_x,
                                uint16_t destination_y,
                                uint16_t source_x_offset,
                                uint16_t source_y_offset,
                                uint16_t source_width,
                                uint16_t source_height,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function renders a portion of an image to the frame buffer.

    Description:
        This function renders an image or a portion of an image to the
        frame buffer with the top, left corner of the image located at
        destination_x, destination_y. To render a full image, source_x_offset,
        source_y_offset, width and height are set to all 0. Using the
        actual image's width and height will also work if these are known.
        If they are not known, avoid the extra extra step to get the image's
        width and height by assigning 0 to the parameters. Another way to
        render a full image is to use the GFX_ImageDraw(). The image to be
        rendered is defined by the pointer pImage.
        <img name="PutImagePartialExample.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - destination_x, destination_y pixel position falls outside
          the frame buffer.
        - source_x_offset, source_y_offset results in a starting position
          beyond the image's dimension.
        - source_width and/or source_height is larger than the actual
          image's width and height.
        - pointer is not properly initialized to a GFX_RESOURCE_HDR object.

*/
// *****************************************************************************
GFX_STATUS GFX_ImagePartialDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t destination_x,
                                uint16_t destination_y,
                                uint16_t source_x_offset,
                                uint16_t source_y_offset,
                                uint16_t source_width,
                                uint16_t source_height,
                                GFX_RESOURCE_HDR *pImage)
{
    uint16_t       colorTemp;
    
#if !defined (GFX_CONFIG_IMAGE_FLASH_DISABLE) ||   \
    !defined (GFX_CONFIG_IMAGE_EXTERNAL_DISABLE)
    GFX_PARTIAL_IMAGE_PARAM partialImage;
    uint8_t        colorDepth;
#endif
    
    if (GFX_RenderStatusCheck(gfxIndex) == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);

    // Save current color
    colorTemp = GFX_ColorGet(gfxIndex);
    
#if !defined (GFX_CONFIG_IMAGE_FLASH_DISABLE) ||   \
    !defined (GFX_CONFIG_IMAGE_EXTERNAL_DISABLE)
    colorDepth = pImage->resource.image.colorDepth;

    // Store the partial image offset
    partialImage.xoffset = source_x_offset;
    partialImage.yoffset = source_y_offset;
    partialImage.width   = source_width;
    partialImage.height  = source_height;
#endif
    
    switch(pImage->type)
    {

#if !defined(GFX_CONFIG_RLE_DECODE_DISABLE) &&             \
        (   !defined (GFX_CONFIG_IMAGE_FLASH_DISABLE) ||   \
            !defined (GFX_CONFIG_IMAGE_EXTERNAL_DISABLE)   \
        )

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
#endif
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
#endif
            
            // Draw the image
            switch(colorDepth)
            {
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
                case 4: 
                    GFX_ImageRLE4BPPDraw(   gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
#endif
  
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
                case 8: 
                    GFX_ImageRLE8BPPDraw(   gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
#endif

                default:    break;
            }
            break;
#endif // #if !defined(GFX_CONFIG_RLE_DECODE_DISABLE) && ...


#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            
#ifdef IMG_SUPPORT_JPEG
    extern IMG_FILE_SYSTEM_API _jpegFileApi; 
            
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        case GFX_RESOURCE_JPEG_EXTERNAL_NONE:        
#endif
#ifdef SYS_FS_MEDIA_NUMBER
        case GFX_RESOURCE_JPEG_FS_NONE:
#endif
        case GFX_RESOURCE_JPEG_FLASH_NONE:

            ImageDecode((IMG_FILE*)pImage,
                IMG_JPEG,
                destination_x,
                destination_y,
                destination_x + source_width,
                destination_y + source_height,
                IMG_ALIGN_CENTER | IMG_DOWN_SCALE,
                &_jpegFileApi,
                ImagePixelIntoArray);            
            break;
#endif
            
#ifdef IMG_SUPPORT_BMP
    extern IMG_FILE_SYSTEM_API _bmpFileApi; 
            
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        case GFX_RESOURCE_BMP_EXTERNAL_NONE:        
#endif
#ifdef SYS_FS_MEDIA_NUMBER
        case GFX_RESOURCE_BMP_FS_NONE:
#endif
        case GFX_RESOURCE_BMP_FLASH_NONE:

            ImageDecode((IMG_FILE*)pImage,
                IMG_BMP,
                destination_x,
                destination_y,
                destination_x + source_width,
                destination_y + source_height,
                IMG_ALIGN_CENTER | IMG_DOWN_SCALE,
                &_bmpFileApi,
                ImagePixelOutput);            
            break;
#endif

#ifdef IMG_SUPPORT_GIF
    extern IMG_FILE_SYSTEM_API _gifFileApi; 
            
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        case GFX_RESOURCE_GIF_EXTERNAL_NONE:        
#endif
#ifdef SYS_FS_MEDIA_NUMBER
        case GFX_RESOURCE_GIF_FS_NONE:
#endif
        case GFX_RESOURCE_GIF_FLASH_NONE:

            ImageDecode((IMG_FILE*)pImage,
                IMG_GIF,
                destination_x,
                destination_y,
                destination_x + source_width,
                destination_y + source_height,
                IMG_ALIGN_CENTER | IMG_DOWN_SCALE,
                &_gifFileApi,
                ImagePixelOutput);            
            break;
#endif
        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_NONE:

            // Draw picture
            switch(colorDepth)
            {
                case 1:     
                    GFX_ImageFlash1BPPDraw( gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
                
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
                case 4:     
                    GFX_ImageFlash4BPPDraw( gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
                case 8:     
                    GFX_ImageFlash8BPPDraw( gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH == 16)
                case 16:
                    GFX_ImageFlash16BPPDraw(gfxIndex,
                                            destination_x,
                                            destination_y,
                                            pImage, &partialImage);
                    break;
#endif
                    
                default:    break;

            }

            break;
#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_NONE:

            // Draw picture
            switch(colorDepth)
            {
                case 1:      
                    GFX_ImageExternal1BPPDraw(  gfxIndex,
                                                destination_x,
                                                destination_y,
                                                pImage, &partialImage);
                    break;
                
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
                case 4:     
                    GFX_ImageExternal4BPPDraw(  gfxIndex,
                                                destination_x,
                                                destination_y,
                                                pImage, &partialImage);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
                case 8:     
                    GFX_ImageExternal8BPPDraw(  gfxIndex,
                                                destination_x,
                                                destination_y,
                                                pImage, &partialImage);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH == 16)
                case 16:    
                    GFX_ImageExternal16BPPDraw( gfxIndex,
                                                destination_x,
                                                destination_y,
                                                pImage, &partialImage);
                    break;
#endif
                    
                default:    break;
            }

            break;
#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

#if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)

#ifndef GFX_CONFIG_IPU_DECODE_DISABLE
        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_IPU:
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_IPU:
#endif 
        case GFX_RESOURCE_MCHP_MBITMAP_EDS_EPMP_NONE:
        
            // this requires special processing of images in Extended Data Space
            // call the driver specific function to perform the processing
            DRV_GFX_ImageDraw(
                            destination_x,
                            destination_y,
                            pImage, &partialImage);
            break;

#endif // #if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)
            
        default:
            break;
    }

    // Restore current color
    GFX_ColorSet(gfxIndex, colorTemp);
    return (GFX_STATUS_SUCCESS);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ImageDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function renders an image to the frame buffer.

    Description:
        This function renders an image to the frame buffer with the
        left-top corner of the image located at given left, top parameters.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - left, top pixel position falls outside
          the frame buffer.
        - pointer is not properly initialized to a GFX_RESOURCE_HDR object.

*/
// *****************************************************************************
GFX_STATUS GFX_ImageDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage)
{
//    return GFX_ImagePartialDraw(gfxIndex, 
//            left, 
//            top, 
//            (left + pImage->resource.image.width - 1), 
//            (top + pImage->resource.image.height - 1), 
//            pImage->resource.image.width, 
//            pImage->resource.image.height, 
//            pImage);
    return GFX_ImagePartialDraw(gfxIndex, left, top, 0, 0, pImage->resource.image.width, pImage->resource.image.height, pImage);
}

// *****************************************************************************
/*  Function:
    int16_t GFX_SineCosineGet(
                               int16_t v,
                               GFX_TRIG_FUNCTION_TYPE type)

    Summary:
        This function returns sine or cosine of an angle.

    Description:
        This function returns sine or cosine of an angle. The angle
        range is -360 to 360 degrees. This function uses the Graphics
        Library look up table of 0-90 degrees. Depending on the
        given angle, the appropriate sign of the result is calculated.

*/
// *****************************************************************************
int16_t GFX_SineCosineGet(int16_t v, GFX_TRIG_FUNCTION_TYPE type)
{
    // if angle is neg, convert to pos equivalent
    if (v < 0) 
    	v += 360;   					
           
    if(v > COSINETABLEENTRIES * 3)
    {
        v -= (COSINETABLEENTRIES * 3);
        if (type == GFX_TRIG_SINE_TYPE)
            return (-_CosineTable[v]);
        else
            return (_CosineTable[COSINETABLEENTRIES - v]);
    }
    else if(v > COSINETABLEENTRIES * 2)
    {
        v -= (COSINETABLEENTRIES * 2);
        if (type == GFX_TRIG_SINE_TYPE)
            return (-_CosineTable[(COSINETABLEENTRIES - v)]);
        else
            return (-_CosineTable[v]);
    }
    else if(v > COSINETABLEENTRIES)
    {
        v -= (COSINETABLEENTRIES);
        if (type == GFX_TRIG_SINE_TYPE)
            return (_CosineTable[v]);
        else
            return (-_CosineTable[COSINETABLEENTRIES - v]);
    }
    else
    {
        if (type == GFX_TRIG_SINE_TYPE)
            return (_CosineTable[COSINETABLEENTRIES - v]);
        else
            return (_CosineTable[v]);
    }

}

#ifndef GFX_CONFIG_GRADIENT_DISABLE

#if ((GFX_CONFIG_COLOR_DEPTH != 16) && (GFX_CONFIG_COLOR_DEPTH != 24))
    #error "Gradient feature can currently support the COLOR_DEPTH of 16 and 24 only."
#endif

#ifndef GFX_CONFIG_PALETTE_DISABLE
    #error "Gradient feature is not currently supported when palette is enabled."
#endif

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BarGradientDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a bar filled gradient colors.

    Description:
        This function renders a bar filled gradient colors. The gradient
        start and end colors are set by GFX_GradientColorSet().
         <img name="BarGradient.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Gradient colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_BarGradientDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{
    static uint16_t  x1,  y1,  x2,  y2;
    static uint16_t doDraw = 0, doDouble;
    static uint16_t px1, py1, px2, py2;
    static GFX_COLOR color1, color2;

    static uint16_t startRed, startBlue, startGreen;
    static uint16_t endRed, endBlue, endGreen;
    static int32_t rdiff=0,gdiff=0,bdiff=0;
    static int16_t steps;

    static uint16_t  i, barSize = 1;

    static GFX_FILL_STYLE direction;

    x1 = px1 = left;
    y1 = py1 = top;
    x2 = px2 = right;
    y2 = py2 = bottom;

    color1    = GFX_GradientStartColorGet(gfxIndex);
    color2    = GFX_GradientEndColorGet(gfxIndex);
    direction = GFX_FillStyleGet(gfxIndex);
    doDouble  = 0;

    switch(direction)
    {
        case GFX_FILL_STYLE_GRADIENT_UP:
        case GFX_FILL_STYLE_GRADIENT_DOWN:
            steps = (bottom - top);
            break;

        case GFX_FILL_STYLE_GRADIENT_LEFT:
        case GFX_FILL_STYLE_GRADIENT_RIGHT:
            steps = (right - left);
            break;

        case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
            steps = (right - left) >> 1;
            doDouble = 1;
            break;

        case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
            steps = (bottom - top) >> 1;
            doDouble = 1;
            break;

        default:
            // this should not happen
            return (GFX_STATUS_FAILURE);
    }
    
    startRed    = GFX_ComponentRedGet(color1);
    startGreen  = GFX_ComponentGreenGet(color1);
    startBlue   = GFX_ComponentBlueGet(color1);

    endRed      = GFX_ComponentRedGet(color2);
    endGreen    = GFX_ComponentGreenGet(color2);
    endBlue     = GFX_ComponentBlueGet(color2);

    ///////////////////////////////////

    //Find the step size for the red portion
    rdiff = ((int32_t)endRed - (int32_t)startRed) << 8;
    rdiff /= steps;

    //Find the step size for the green portion
    gdiff = ((int32_t)endGreen - (int32_t)startGreen) << 8;
    gdiff /= steps;

    //Find the step size for the blue portion
    bdiff = ((int32_t)endBlue - (int32_t)startBlue) << 8;
    bdiff /= steps;

    color1 = GFX_RGBConvert(startRed, startGreen, startBlue);
    i = 0;
    barSize = 1;
    doDraw = 1;

    while(i <= steps)
    {
        if (doDraw)
        {
            GFX_ColorSet(gfxIndex, color2);

            GFX_BarDraw(gfxIndex, x1, y1, x2, y2);

            if (doDouble)
            {
                GFX_BarDraw(gfxIndex, px1, py1, px2, py2);
            }
            barSize = 1;
            doDraw = 0;
        }

        //Calculate the new RGB values
        endRed      = startRed   + ((int32_t)(rdiff*i) >> 8);
        endGreen    = startGreen + ((int32_t)(gdiff*i) >> 8);
        endBlue     = startBlue  + ((int32_t)(bdiff*i) >> 8);

        color2 = GFX_RGBConvert(endRed, endGreen, endBlue);

        // if color matches increase the bar size so we only call one
        // bar call to lines with the same color
        if(color2 == color1)
        {
            barSize++;
        }
        else
        {
            doDraw = 1;
            color1 = color2;

            switch(direction)
            {
                case GFX_FILL_STYLE_GRADIENT_DOWN:
                    y1 = top;
                    y2 = top + barSize;
                    if (y2 > bottom)
                        y2 = bottom;
                    top += (barSize);
                    break;

                case GFX_FILL_STYLE_GRADIENT_UP:
                case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                    y1 = bottom - barSize;
                    y2 = bottom;
                    if (y1 < top)
                        y1 = top;
                    bottom -= (barSize);
                    if (doDouble)
                    {
                        py1 = top;
                        py2 = top + barSize;
                        if (py2 > bottom)
                            py2 = bottom;
                        top += (barSize);
                    }
                    break;

                case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    x1 = left;
                    x2 = left + barSize;
                    if (x2 > right)
                        x2 = right;
                    left += (barSize);
                    break;

                case GFX_FILL_STYLE_GRADIENT_LEFT:
                case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                    x1 = right - barSize;
                    x2 = right;
                    if (x1 < left)
                        x1 = left;
                    right -= (barSize);

                    if (doDouble)
                    {
                        px1 = left;
                        px2 = left + barSize;
                        if (px2 > right)
                            px2 = right;
                        left += (barSize);
                    }
                    break;

                default:
                    break;
            } // end of switch
        } // end of else...

        // update the counter
        i++;

    } // end of while ...

    return (GFX_STATUS_SUCCESS);

}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BevelGradientDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t rad)

    Summary:
        This function renders a bar with rounded corners filled with
        gradient colors.

    Description:
        This function renders a bar with rounded corners  filled with
        gradient colors. The gradient start and end colors are set
        by GFX_GradientColorSet().
         <img name="BevelGradient.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Gradient colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_BevelGradientDraw(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t rad)
{
    typedef enum
    {
        BEVEL_GRADIENT_IDLE,
        BEVEL_GRADIENT_BEGIN,
        BEVEL_GRADIENT_CHECK,
        BEVEL_GRADIENT_CALC_Q8TOQ1,
        BEVEL_GRADIENT_Q8TOQ1,
        BEVEL_GRADIENT_CALC_Q7TOQ2,
        BEVEL_GRADIENT_Q7TOQ2,
        BEVEL_GRADIENT_CALC_Q6TOQ3,
        BEVEL_GRADIENT_Q6TOQ3,
        BEVEL_GRADIENT_CALC_Q5TOQ4,
        BEVEL_GRADIENT_Q5TOQ4,
        BEVEL_GRADIENT_FACE_CHECK,
        BEVEL_GRADIENT_FACE,
    } BEVEL_GRADIENT_FILL_STATES;

    PRIMITIVE_UINT32_UNION temp;
    static int32_t err;
    static int16_t yLimit, xPos, yPos;
    static int16_t xCur, yCur, yNew;

    static int16_t x1, y1, x2, y2, doDouble;

    static BEVEL_GRADIENT_FILL_STATES state = BEVEL_GRADIENT_IDLE;

    static int16_t sred = 0, sblue = 0, sgreen = 0;
    static int16_t ered = 0, eblue = 0, egreen = 0;
    static int32_t rdiff = 0, gdiff = 0, bdiff = 0;
    static int16_t steps;

    static GFX_FILL_STYLE direction = 0;
    static GFX_COLOR      color1, color2;
    static GFX_COLOR      sStart = 0, sEnd = 0;

    uint16_t i;

    while (1)
    {
        if (GFX_RenderStatusCheck(gfxIndex) == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch (state)
        {
            case BEVEL_GRADIENT_IDLE:

                color2    = GFX_GradientEndColorGet(gfxIndex);
                color1    = GFX_GradientStartColorGet(gfxIndex);
                direction = GFX_FillStyleGet(gfxIndex);
                doDouble  = 0;

                //This switch statement calculates the amount of transitions needed
                switch (direction)
                {
                    case GFX_FILL_STYLE_GRADIENT_UP:
                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                        steps = (bottom - top + 1 + (rad << 1));
                        break;

                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                        steps = (right - left + 1 + (rad << 1));
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                        steps = (right - left + 1 + (rad << 1)) >> 1;
                        doDouble = 1;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        steps = (bottom - top + 1 + (rad << 1)) >> 1;
                        doDouble = 1;
                        break;

                    default:
                        return (GFX_STATUS_SUCCESS);
                }

                //Calculate the starting RGB values
                sred    = GFX_ComponentRedGet(color1);
                sgreen  = GFX_ComponentGreenGet(color1);
                sblue   = GFX_ComponentBlueGet(color1);

                ered    = GFX_ComponentRedGet(color2);
                egreen  = GFX_ComponentGreenGet(color2);
                eblue   = GFX_ComponentBlueGet(color2);

                ///////////////////////////////////

                //Find the step size for the red portion//
                rdiff = ((int32_t)ered - (int32_t)sred) << 8;
                rdiff /= steps;

                //Find the step size for the green portion//
                gdiff = ((int32_t)egreen - (int32_t)sgreen) << 8;
                gdiff /= steps;

                //Find the step size for the blue portion//
                bdiff = ((int32_t)eblue - (int32_t)sblue) << 8;
                bdiff /= steps;

                sStart = GFX_GradientStartColorGet(gfxIndex);
                sEnd   = GFX_GradientEndColorGet(gfxIndex);

                state = BEVEL_GRADIENT_BEGIN;
                // no break here since we want the next state anyway

            case BEVEL_GRADIENT_BEGIN:
                if (rad == 0)
                {   // no radius object is a filled rectangle
                    state = BEVEL_GRADIENT_FACE_CHECK;
                    break;
                }

                // compute variables
                temp.uint32Value = SIN45 * rad;
                yLimit = temp.uint3216BitValue[1];
                temp.uint32Value = (uint32_t) (ONEP25 - ((int32_t) rad << 16));
                err = (int16_t) (temp.uint3216BitValue[1]);
                xPos = rad;
                yPos = 0;
                xCur = xPos;
                yCur = yPos;
                yNew = yPos;
                state = BEVEL_GRADIENT_CHECK;
                // no break here since we want the next state anyway

            case BEVEL_GRADIENT_CHECK:

                //bevel_fill_check : if (yPos > yLimit)
                // check if we have reached our limit
                if (yPos > yLimit)
                {
                    state = BEVEL_GRADIENT_FACE_CHECK;
                    break;
                }
                else
                {

                    // New records the last y position
                    yNew = yPos;

                    // calculate the next value of x and y
                    if (err > 0)
                    {
                        xPos--;
                        err += 5 + ((yPos - xPos) << 1);
                    }
                    else
                        err += 3 + (yPos << 1);
                    yPos++;
                    state = BEVEL_GRADIENT_CALC_Q6TOQ3;
                }
                // no break here since we want the next state anyway

            case BEVEL_GRADIENT_CALC_Q6TOQ3:

                // 6th octant to 3rd octant
                if ( doDouble == 1)
                    i = (top - yCur) - top + rad;
                else
                    i = (bottom + yCur) - top + rad;

                ered   = sred   + ((int32_t)(rdiff * i) >> 8);
                egreen = sgreen + ((int32_t)(gdiff * i) >> 8);
                eblue  = sblue  + ((int32_t)(bdiff * i) >> 8);
                color2 = GFX_RGBConvert(ered, egreen, eblue);
                GFX_ColorSet(gfxIndex, color2);

                switch (direction)    //Direction matter because different portions of the circle are drawn
                {
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                        x1 = left - yNew;
                        y1 = top - xCur;
                        x2 = left - yCur;
                        y2 = bottom + xCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                        x1 = right + yCur;
                        y1 = top - xCur;
                        x2 = right + yNew;
                        y2 = bottom + xCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_UP:
                        x1 = left - xCur;
                        y1 = top - yNew;
                        x2 = right + xCur;
                        y2 = top - yCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        x1 = left - xCur;
                        y1 = bottom + yCur;
                        x2 = right + xCur;
                        y2 = bottom + yNew;
                        break;

                    default:
                        // should not happen
                        break;
                } // end of switch...
                
                state = BEVEL_GRADIENT_Q6TOQ3;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_Q6TOQ3:

                GFX_BarDraw(gfxIndex, x1, y1, x2, y2);

                state = BEVEL_GRADIENT_CALC_Q5TOQ4;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_CALC_Q5TOQ4:
                // 5th octant to 4th octant
                if ( doDouble == 1)
                    //i = top + xPos - top + rad;
                    i = (top - xCur) - top + rad;
                else
                    // 5th octant to 4th octant
                    i = (bottom + xPos) - top + rad ;

                //Calculate the starting RGB values
                ered   = sred   + ((int32_t)(rdiff * i) >> 8);
                egreen = sgreen + ((int32_t)(gdiff * i) >> 8);
                eblue   = sblue + ((int32_t)(bdiff * i) >> 8);

                color2 = GFX_RGBConvert(ered, egreen, eblue);
                GFX_ColorSet(gfxIndex, color2);
                
                switch (direction)    //Direction matter because different portions of the circle are drawn
                {
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                        x1 = left - xCur;
                        y1 = top - yNew;
                        x2 = left - xPos;
                        y2 = bottom + yNew;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                        x1 = right + xPos;
                        y1 = top - yNew;
                        x2 = right + xCur;
                        y2 = bottom + yNew;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_UP:
                        x1 = left - yNew;
                        y1 = top - xCur;
                        x2 = right + yNew;
                        y2 = top - xPos;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        x1 = left - yNew;
                        y1 = bottom + xPos;
                        x2 = right + yNew;
                        y2 = bottom + xCur;
                        break;
                        
                    default:
                        // should not happen
                        break;
                }                
                
                state = BEVEL_GRADIENT_Q5TOQ4;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_Q5TOQ4:

                GFX_BarDraw(gfxIndex, x1, y1, x2, y2);

                state = BEVEL_GRADIENT_CALC_Q8TOQ1;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_CALC_Q8TOQ1:
                // 8th octant to 1st octant
                //if (doDouble == 0)
                {
                    i = (top - xCur) - top + rad;

                    //Calculate the starting RGB values
                    ered   = sred   + ((int32_t)(rdiff * i) >> 8);
                    egreen = sgreen + ((int32_t)(gdiff * i) >> 8);
                    eblue  = sblue  + ((int32_t)(bdiff * i) >> 8);
                    
                    color2 = GFX_RGBConvert(ered, egreen, eblue);
                    GFX_ColorSet(gfxIndex, color2);
                }
                
                switch (direction)    //Direction matter because different portions of the circle are drawn
                {
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                        x1 = right + xPos;
                        y1 = top - yNew;
                        x2 = right + xCur;
                        y2 = bottom + yNew;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                        x1 = left - xCur;
                        y1 = top - yNew;
                        x2 = left - xPos;
                        y2 = bottom + yNew;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_UP:

                        x1 = left - yNew;
                        y1 = bottom + xPos;
                        x2 = right + yNew;
                        y2 = bottom + xCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        x1 = left - yNew;
                        y1 = top - xCur;
                        x2 = right + yNew;
                        y2 = top - xPos;
                        break;

                    default:
                        // should not happen
                        break;
                }
                                
                state = BEVEL_GRADIENT_Q8TOQ1;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_Q8TOQ1:

                GFX_BarDraw(gfxIndex, x1, y1, x2, y2);
                        
                state = BEVEL_GRADIENT_CALC_Q7TOQ2;
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_CALC_Q7TOQ2:

                // 7th octant to 2nd octant
                i = (top - yNew) - top + rad;

                //Calculate the starting RGB values
                ered   = sred   + ((int32_t)(rdiff * i) >> 8);
                egreen = sgreen + ((int32_t)(gdiff * i) >> 8);
                eblue  = sblue  + ((int32_t)(bdiff * i) >> 8);

                color2 = GFX_RGBConvert(ered, egreen, eblue);
                GFX_ColorSet(gfxIndex, color2);

                switch (direction)    //Direction matter because different portions of the circle are drawn
                {
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                        x1 = right + yCur;
                        y1 = top - xCur;
                        x2 = right + yNew;
                        y2 = bottom + xCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                        x1 = left - yNew;
                        y1 = top - xCur;
                        x2 = left - yCur;
                        y2 = bottom + xCur;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_UP:
                        x1 = left - xCur;
                        y1 = bottom + yCur;
                        x2 = right + xCur;
                        y2 = bottom + yNew;
                        break;

                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        x1 = left - xCur;
                        y1 = top - yNew;
                        x2 = right + xCur;
                        y2 = top - yCur;
                        break;

                    default:
                        // should not happen
                        break;
                }
                
                state = BEVEL_GRADIENT_Q7TOQ2;
                // no break here since the next state is what we want


            case BEVEL_GRADIENT_Q7TOQ2:

                GFX_BarDraw(gfxIndex, x1, y1, x2, y2);

                // update current values
                xCur = xPos;
                yCur = yPos;
                state = BEVEL_GRADIENT_CHECK;
                break;

            case BEVEL_GRADIENT_FACE_CHECK:
                // this is the face area (the rectangular area in the middle)
                // call the basic rectangle fill for this one.
                if ((right - left) || (bottom - top))
                {
                    i = (top) - top + rad;
                    //Calculate the starting RGB values
                    ered   = sred   + ((rdiff * i) >> 8);
                    egreen = sgreen + ((gdiff * i) >> 8);
                    eblue  = sblue  + ((bdiff * i) >> 8);

                    color1 = GFX_RGBConvert(ered, egreen, eblue);

                    if (doDouble == 1)
                        color2 = GFX_GradientEndColorGet(gfxIndex);
                    else    
                    {
                        i = (bottom) - top + rad;
                        //Calculate the ending RGB values
                        ered   = sred   + ((rdiff * i) >> 8);
                        egreen = sgreen + ((gdiff * i) >> 8);
                        eblue  = sblue  + ((bdiff * i) >> 8);
                        color2 = GFX_RGBConvert(ered, egreen, eblue);
                    }

                    if ( direction == GFX_FILL_STYLE_GRADIENT_UP   ||  \
                         direction == GFX_FILL_STYLE_GRADIENT_DOWN ||  \
                         direction == GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR
                        )
                    {
                        x1 = left - rad;
                        y1 = top;
                        x2 = right + rad;
                        y2 = bottom;
                    }
                    else
                    {
                        x1 = left; 
                        y1 = top - rad;
                        x2 = right;
                        y2 = bottom + rad;
                    }
                    GFX_GradientColorSet(gfxIndex, color1, color2);

                    state = BEVEL_GRADIENT_FACE;
                }
                else
                {
                    // this is more of a check when parameters
                    // do not define a rectangular area
                    state = BEVEL_GRADIENT_IDLE;
                    return (GFX_STATUS_SUCCESS);
                }
                // no break here since the next state is what we want

            case BEVEL_GRADIENT_FACE:

                if (GFX_BarGradientDraw(gfxIndex, x1, y1, x2, y2) != GFX_STATUS_SUCCESS)
                    return GFX_STATUS_FAILURE;

                // all done! return to idle
                state = BEVEL_GRADIENT_IDLE;
                // reset the original gradient colors
                GFX_GradientColorSet(gfxIndex, sStart, sEnd);

                return (GFX_STATUS_SUCCESS);
                
            default:
                // this should not happen
                return (GFX_STATUS_FAILURE);
                
        } // end of switch
    } // end of while
}
#endif


#ifndef GFX_CONFIG_RLE_DECODE_DISABLE

// *****************************************************************************
/*  Function:
    uint32_t GFX_RLEBlockFind(
                                GFX_RESOURCE_HDR *pImage,
                                uint16_t size,
                                uint16_t height,
                                uint16_t rleType)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function searches for an RLE block.

*/
// *****************************************************************************
uint32_t GFX_RLEBlockFind(  GFX_RESOURCE_HDR *image,
                            uint16_t size,
                            uint16_t height,
                            uint16_t rleType)
{
    uint16_t sourceOffset = 0;
    uint16_t decodeSize = 0;
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
    uint8_gfx_image_prog *flashAddress = 0;
#endif
    uint32_t addressOffset = 0;

    if (image->resource.image.type == MCHP_BITMAP_NORMAL)
    {
        // this is the offset to skip the header and the palette portion
        switch(rleType)
        {
            case 4:
                addressOffset = 16 * sizeof(uint16_t);
                break;
            case 8:
                addressOffset = 256 * sizeof(uint16_t);
                break;
            default:
                break;
        }
    }
    else
    {
        // if image is stripped of its palette then skip the id
        addressOffset = sizeof(uint16_t);
    }

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
    if(image->type == GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE)
        flashAddress = (image->resource.image.location.progByteAddress + addressOffset);
#endif

    while(height)
    {
        //pRow = pixelrow;
        decodeSize = 0;
        while(decodeSize < size)
        {
            uint8_t code = 0, value = 0;

            switch(image->type)
            {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
                case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                    code = *flashAddress++;
                    value = *flashAddress++;
                    break;
#endif
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
                case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                    GFX_ExternalResourceCallback(image, addressOffset, sizeof(uint8_t), &code);
                    addressOffset++;
                    GFX_ExternalResourceCallback(image, addressOffset, sizeof(uint8_t), &value);
                    addressOffset++;
                    break;
#endif
                default:
                    // this should not happen, in case it does it will return 0
                    return 0;
            } // switch (image->type)
            sourceOffset += 2;

            if(code > 0)
            {
                decodeSize += code;
            }
            else
            {
                decodeSize += value;
                if (rleType == 4)
                {
                    sourceOffset += ((value + 1) >> 1);
                    switch (image->type)
                    {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
                        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                            flashAddress += (value >> 1);
                            break;
#endif
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
                        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                            addressOffset += (value >> 1);
                            break;
#endif
                        default:
                            break;
                    } // switch (image->type)
                }
                else
                {
                    sourceOffset += value;
                    switch (image->type)
                    {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
                        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                            flashAddress += value;
                            break;
#endif
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
                        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                            addressOffset += value;
                            break;
#endif
                        default:
                            break;
                    } // switch (image->type)
                }
            }
        } // while(decodeSize < size)
        height--;

    } // while(height)

    return (sourceOffset);

}

#if (GFX_CONFIG_COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    void GFX_ImageRLE8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders an 8bpp RLE compressed image. The image can be sourced
        from internal flash or external memory.

*/
// *****************************************************************************
void GFX_ImageRLE8BPPDraw(      SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
    register uint32_t               memOffset = 0;
#endif
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
    register uint8_gfx_image_prog   *flashAddress = 0;
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
    uint16_t                        counter;
#endif
#endif
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    int16_t                         x, y;
    uint16_t                        sizeX, sizeY, xCurr;
    uint8_t                         temp;
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
    uint16_t                        imagePalette[256];
#endif
    uint16_t                        addressOffset = 0;
    uint16_t                        pixelOffset = 0;
    uint8_t                         pixelrow[GFX_MaxXGet(gfxIndex) + 1];

    GFX_COLOR                       useColor;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    switch (pImage->type)
    {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
        // **********************************************
        // for image in flash
        // **********************************************
        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
            // Move pointer to image location
            flashAddress = pImage->resource.image.location.progByteAddress;

            if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    // Adjust the address to skip the palette section.
                    // Pixel data are indices not actual color data.
                    // Palette to be used is assumed to be system level
                    // defined. This supports only 16 bits per palette entry width.
                    // 24 bits palette entry width is not supported.
                    flashAddress = flashAddress + 512;
                }
                else
#endif                
                {
                    // grab the palette of the image
                    // the actual color used are in this palette
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
                    for (counter = 0; counter < 256; counter++)
                    {
#if GFX_CONFIG_COLOR_DEPTH == 16
                        imagePalette[counter] = *((uint16_gfx_image_prog *) flashAddress);
                        flashAddress += 2;
#endif

#if GFX_CONFIG_COLOR_DEPTH == 24
                        imagePalette[counter] = *((uint32_gfx_image_prog *) flashAddress);
                        flashAddress += 4;
#endif
                    }
#endif
                }
            }
            else
            {
                // else if the image is stripped of its palette, then skip the id
                flashAddress += 2;
            }

            break;
#endif //#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        // **********************************************
        // for image in external memory
        // **********************************************
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
        
            if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {        
                    // use system palette, skip the image palette and adjust the offset
                    // assumes palette entry width is 16 bits,
                    // 24 bits palette width is not supported
                    memOffset = 256 * sizeof (uint16_t);
                }
                else
#endif          
                {
                    // Get image palette, since this will be used to render the pixels
                    // when system palette is used, the image palette is assumed to be
                    // the same as the system palette so we do not fetch it
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
                    GFX_ExternalResourceCallback(pImage, 0, 256 * sizeof(GFX_COLOR), imagePalette);
                    memOffset = 256 * sizeof (GFX_COLOR);
#endif
                }
            }

            else 
            {
                // palette is stripped so skip the id
                memOffset = sizeof(uint16_t);
            }

            break;
#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

            default:
                break;
    }

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        switch (pImage->type)
        {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                // adjust the address to the correct starting line
                addressOffset = GFX_RLEBlockFind(pImage, sizeX, pPartialImageData->yoffset, 8);
                flashAddress += addressOffset;
                break;
#endif //#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                addressOffset = (GFX_RLEBlockFind(pImage, sizeX, pPartialImageData->yoffset, 8));
                memOffset += addressOffset;
                break;
#endif // #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            default:
                break;
        }
        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width;

        pixelOffset = pPartialImageData->xoffset;
    }

    for(y = 0; y < sizeY; )
    {

        switch (pImage->type)
        {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                addressOffset = GFX_ImageFlashRLE8BPPDecode(flashAddress, pixelrow, bitmapHdr.width);
                flashAddress += addressOffset;
                break;
#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                addressOffset = (GFX_ImageFlashExternal8BPPDecode(pImage, memOffset, pixelrow, bitmapHdr.width));
                memOffset += addressOffset;
                break;
#endif  // #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            default:
                break;
        }
        xCurr = pixelOffset;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = pixelrow[xCurr];
            xCurr++;

            // Set color
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
            useColor = imagePalette[temp];
#else
            useColor = temp;
#endif

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif

        }

        // render the current line
        GFX_PixelArrayPut(
            gfxIndex,
            left,
            top + y,
            client->gfxLineBuffer0,
            sizeX, 1);

        y++;

    }

}
#endif // #if (GFX_CONFIG_COLOR_DEPTH >= 8)

#if (GFX_CONFIG_COLOR_DEPTH >= 4)

// *****************************************************************************
/*  Function:
    void GFX_ImageRLE4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders an 4bpp RLE compressed image. The image can be sourced
        from internal flash or external memory.

*/
// *****************************************************************************
void GFX_ImageRLE4BPPDraw(      SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
    register uint32_t               memOffset = 0;
#endif
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
    register uint8_gfx_image_prog   *flashAddress = 0;
    uint16_t                        counter;
#endif
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    int16_t                         x, y;
#ifndef GFX_CONFIG_PALETTE_DISABLE
#if (GFX_CONFIG_COLOR_DEPTH == 4)
    int16_t                         p;
    int16_t                         off, adj;
#endif
#endif
    uint16_t                        sizeX, sizeY, xCurr;
    uint16_t                        temp;
    uint16_t                        addressOffset = 0;
    uint16_t                        pixelOffset = 0;

#if (GFX_CONFIG_COLOR_DEPTH > 4)// || (GFX_CONFIG_COLOR_DEPTH == 24)
    uint16_t                        imagePalette[16];
#endif
    uint8_t                         pixelrow[GFX_MaxXGet(gfxIndex) + 1];

#if (GFX_CONFIG_COLOR_DEPTH != 4)
    GFX_COLOR                       useColor;
#endif

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    switch (pImage->type)
    {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
        // **********************************************
        // for image in flash
        // **********************************************
        case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
            // Move pointer to image location
            flashAddress = pImage->resource.image.location.progByteAddress;

            if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    // Adjust the address to skip the palette section.
                    // Pixel data are indices not actual color data.
                    // Palette to be used is assumed to be system level
                    // defined. This supports only 16 bits per palette entry width.
                    // 24 bits palette entry width is not supported.
                    flashAddress = flashAddress + 32;
                }
                else
#endif       
                {
                    // grab the palette of the image
                    // the actual color used are in this palette
                    for (counter = 0; counter < 16; counter++)
                    {
#if GFX_CONFIG_COLOR_DEPTH == 16
                        imagePalette[counter] = *((uint16_gfx_image_prog *) flashAddress);
                        flashAddress += 2;
#endif

#if GFX_CONFIG_COLOR_DEPTH == 24
                        imagePalette[counter] = *((uint32_gfx_image_prog *) flashAddress);
                        flashAddress += 4;
#endif
                    }                
                }
            }
            else
            {
                // else if the image is stripped of its palette, then skip the id
                flashAddress += 2;
            }

            break;
#endif //#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            
#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

        // **********************************************
        // for image in external memory
        // **********************************************
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
        
            if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                if (IsPaletteEnabled())
                {
                    // Adjust the address to skip the palette section.
                    // assumes palette entry width is 16 bits,
                    // 24 bits palette width is not supported
                    memOffset = 16 * sizeof (uint16_t);
                }        
                else
#endif                
                {
#if (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
                    // Get image palette, since this will be used to render the pixels
                    // when system palette is used, the image palette is assumed to be
                    // the same as the system palette so we do not fetch it
                    GFX_ExternalResourceCallback(pImage, 0, 16 * sizeof (GFX_COLOR), imagePalette);
                    memOffset = 16 * sizeof (GFX_COLOR);
#endif
                }
            }
            else 
            {
                //if the image is stripped of its palette, then don't adjust the address.
                memOffset = 2;
            }

            break;
#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

        default:
            break;
    }

    // Read image size
    sizeY = bitmapHdr.height;
    sizeX = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        switch (pImage->type)
        {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                // adjust the address to the correct starting line
                addressOffset = GFX_RLEBlockFind(pImage, bitmapHdr.width, pPartialImageData->yoffset, 4);
                flashAddress += addressOffset;
                break;
#endif //#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                addressOffset = (GFX_RLEBlockFind(pImage, bitmapHdr.width, pPartialImageData->yoffset, 4));
                memOffset += addressOffset;
                break;
#endif // #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

            default:
                break;
        }

        sizeY = pPartialImageData->height;
        sizeX = pPartialImageData->width;

        pixelOffset = pPartialImageData->xoffset;

    }

    // if we are going beyond the screen, truncate the size
    if ((left + sizeX) > GFX_MaxXGet(gfxIndex) + 1)
        sizeX = (GFX_MaxXGet(gfxIndex) + 1) - left;

    // Note: For speed the code for loops are repeated. A small code size increase for performance
    for(y = 0; y < sizeY; )
    {

        switch (pImage->type)
        {
#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE:
                addressOffset = GFX_ImageFlashRLE4BPPDecode(flashAddress, pixelrow, bitmapHdr.width);
                flashAddress += addressOffset;
                break;
#endif //#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
                // Get line
                addressOffset = (GFX_ImageFlashExternal4BPPDecode(pImage, memOffset, pixelrow, bitmapHdr.width));
                memOffset += addressOffset;
                break;
#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

            default:
                break;
        }
        xCurr = pixelOffset;

#if (GFX_CONFIG_COLOR_DEPTH == 4) && !defined (GFX_CONFIG_PALETTE_DISABLE)
        // if system palette is being used,
        // each nibble is an index

        // pack the nibbles to the global line buffer
        x = 0;
        p = 0;

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        off = xCurr + sizeX - 1;
        adj = -1;
#else
        off = xCurr;
        adj = 1;
#endif
        
        // For faster execution: divide the packing into two
        // parts. The first part is always packing even number of nibbles.
        // The last part is the packing of the remaining nibble.
        if (sizeX > 1)
        {
            x = sizeX >> 1;
            while(x)
            {
                // always get two nibbles
                temp = pixelrow[off] & 0x0F;
                off += adj;
                temp = temp | (pixelrow[off] << 4);
                off += adj;
                client->gfxLineBuffer0[p++] = temp;
                x--;
            }
        }
        // now if there is a remaining nibble process it
        if ((sizeX & 0x01) == 0x01)
        {
                temp = pixelrow[off] & 0x0F;
                client->gfxLineBuffer0[p] = temp;
        }
       
#else

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
        for(x = sizeX - 1; x >= 0; )
#else
        for(x = 0; x < sizeX; )
#endif
        {
            temp = pixelrow[xCurr];
            xCurr++;

            // Set color
#if (GFX_CONFIG_COLOR_DEPTH == 4)
            useColor = temp;
#else
            useColor = imagePalette[temp];
#endif

#ifdef GFX_CONFIG_PIXEL_ARRAY_NO_SWAP_DISABLE
            client->gfxLineBuffer0[x--] = useColor;
#else
            client->gfxLineBuffer0[x++] = useColor;
#endif
        }
        
#endif
            // render the current line
            GFX_PixelArrayPut(
                gfxIndex,
                left,
                top + y,
                client->gfxLineBuffer0,
                sizeX, 1);
        y++;

    }

}

#endif // #if (GFX_CONFIG_COLOR_DEPTH >= 4)

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#if (GFX_CONFIG_COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    uint16_t GFX_ImageFlashRLE8BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixel_row,
                                uint16_t size)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Decode 8bpp RLE compressed image stored in internal flash memory.

*/
// *****************************************************************************
uint16_t GFX_ImageFlashRLE8BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixel_row,
                                uint16_t size)
{
    uint16_t sourceOffset = 0;
    uint16_t decodeSize = 0;

    while(decodeSize < size)
    {
        uint8_t code = *flashAddress++;
        uint8_t value = *flashAddress++;
        sourceOffset += 2;
        
        if(code > 0)
        {
            decodeSize += code;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                code -= (decodeSize - size);
            }
            
            while(code)
            {
                *pixel_row++ = value;
                code--;
            }
        }
        else
        {
            decodeSize += value;
            sourceOffset += value;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                value -= (decodeSize - size);
            }
            
            while(value)
            {
                *pixel_row++ = *flashAddress++;
                value--;
            }
        }
    }    
    return (sourceOffset);
}
#endif //#if (COLOR_DEPTH >= 8)

#if (GFX_CONFIG_COLOR_DEPTH >= 4)
        
// *****************************************************************************
/*  Function:
    uint16_t GFX_ImageFlashRLE4BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixel_row,
                                uint16_t size)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Decode 4bpp RLE compressed image stored in internal flash memory.

*/
// *****************************************************************************
uint16_t GFX_ImageFlashRLE4BPPDecode(
                                uint8_gfx_image_prog *flashAddress,
                                uint8_t *pixel_row,
                                uint16_t size)
{
    uint16_t sourceOffset = 0;
    uint16_t decodeSize = 0;

    while(decodeSize < size)
    {
        uint8_t code = *flashAddress++;
        uint8_t value = *flashAddress++;
        uint8_t counter;
        
        sourceOffset += 2;
        
        if(code > 0)
        {
            decodeSize += code;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                code -= (decodeSize - size);
            }
            
            for(counter = 0; counter < code; counter++)
            {
                if(counter & 0x01)
                {
                    *pixel_row++ = (value >> 4) & 0x0F;
                }
                else
                {
                    *pixel_row++ = (value) & 0x0F;
                }
            }
        }
        else
        {
            decodeSize += value;
            sourceOffset += ((value + 1) >> 1);
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                value -= (decodeSize - size);
            }
            
            for(counter = 0; counter < value; counter++)
            {
                if(counter & 0x01)
                {
                    *pixel_row++ = (*flashAddress >> 4) & 0x0F;
                    flashAddress++;
                }
                else
                {
                    *pixel_row++ = (*flashAddress) & 0x0F;
                }
            }
        }
    }

    return (sourceOffset);
}

        #endif //#if (COLOR_DEPTH >= 4)

#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
    
        #if (GFX_CONFIG_COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    uint16_t GFX_ImageFlashExternal8BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Decode 8bpp RLE compressed image stored in external memory.

*/
// *****************************************************************************
uint16_t GFX_ImageFlashExternal8BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size)
{
    uint16_t sourceOffset = 0;
    uint16_t decodeSize = 0;

    struct rleDataPair
    {
        uint8_t code;
        uint8_t value;
    } codePair;


    while(decodeSize < size)
    {
        GFX_ExternalResourceCallback(image, memAddress, sizeof(uint8_t) * 2, &codePair);
        memAddress += 2;
        sourceOffset += 2;
        
        if(codePair.code > 0)
        {
            decodeSize += codePair.code;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                codePair.code -= (decodeSize - size);
            }
            
            while(codePair.code)
            {
                *pixel_row++ = codePair.value;
                codePair.code--;
            }
        }
        else
        {
            decodeSize += codePair.value;
            sourceOffset += codePair.value;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                codePair.value -= (decodeSize - size);
            }
            
            GFX_ExternalResourceCallback(image, memAddress, codePair.value * sizeof(uint8_t), pixel_row);
            pixel_row += codePair.value;
            memAddress += codePair.value;
        }
    }    
    return (sourceOffset);
}
#endif //#if (COLOR_DEPTH >= 8)
        
#if (GFX_CONFIG_COLOR_DEPTH >= 4)

// *****************************************************************************
/*  Function:
    uint16_t GFX_ImageFlashExternal4BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Decode 4bpp RLE compressed image stored in external memory.

*/
// *****************************************************************************
uint16_t GFX_ImageFlashExternal4BPPDecode(
                                void *image,
                                uint32_t memAddress,
                                uint8_t *pixel_row,
                                uint16_t size)
{
    uint16_t sourceOffset = 0;
    uint16_t decodeSize = 0;
    uint8_t *pData, *pRawSource;

    struct rleDataPair
    {
        uint8_t code;
        uint8_t value;
    } codePair;

    // allocate data for temp storage
    pRawSource = (uint8_t *)GFX_malloc(size);

    while(decodeSize < size)
    {
        uint8_t ctr, temp = 0, nibbleCount;

        GFX_ExternalResourceCallback(image, memAddress, sizeof(uint8_t) * 2, &codePair);
        memAddress += 2;
        sourceOffset += 2;
        
        if(codePair.code > 0)
        {
            decodeSize += codePair.code;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                codePair.code -= (decodeSize - size);
            }
            
            for(ctr = 0; ctr < codePair.code; ctr++)
            {
                if(ctr & 0x01)
                {
                    *pixel_row++ = (codePair.value >> 4) & 0x0F;
                }
                else
                {
                    *pixel_row++ = (codePair.value) & 0x0F;
                }
            }
        }
        else
        {
            decodeSize += codePair.value;
            sourceOffset += (codePair.value + 1) >> 1;
            
            if(decodeSize > size) //To avoid writing oustide the array bounds
            {
                codePair.value -= (decodeSize - size);
            }

            // it is possible to end on an upper nibble,
            // therefore, to cover that the memAddress must restart on the proper place

            // calculate how many nibbles are expected
            nibbleCount = (codePair.value >> 1) + (codePair.value & 0x01);

            // get the data
            GFX_ExternalResourceCallback(image, memAddress, sizeof(uint8_t) * nibbleCount, pRawSource);
            // adjust the address correctly, if nibbleCount is odd, then the memAddress
            // should update to the location of the last nibble since the next nibble
            // still needs to be processed.
            memAddress += (codePair.value >> 1);

            // use a pointer to parse the obtained nibbles
            pData = pRawSource;

            for(ctr = 0; ctr < codePair.value; ctr++)
            {
                // there are two nibbles to process
                // get the new nibble after the two are processed.
                if(ctr & 0x01)
                {
                    *pixel_row++ = (temp >> 4) & 0x0F;
                }
                else
                {
                    temp = *pData++;
                    *pixel_row++ = temp & 0x0F;
                }
            }
        }
    }

    // free allocated mem
    GFX_free(pRawSource);

    return (sourceOffset);
}

#endif //#if (COLOR_DEPTH >= 4)

#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
    
#endif //#ifndef GFX_CONFIG_RLE_DECODE_DISABLE

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_DRV_DoubleBufferDisable(void)

    Summary:
        This function is the driver specific double buffering disabling
        steps to initialize buffers to a state where double buffering
        is disabled.

    Description:
        This function is the driver specific double buffering disabling
        steps to initialize buffers to a state where double buffering
        is disabled.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_DRV_DoubleBufferDisable(SYS_MODULE_INDEX gfxIndex)
{ }

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DrawBufferInitialize(
                                int index,
                                uint32_t address)

    Summary:
        This function initializes the address of the draw buffer
        specified by the given index.

    Description:
        For system with multiple buffers, this function is used to
        initialize the array of buffers. The address of the draw buffer
        will be associated with the specified index. Use this function
        to initialize or modify the array of frame buffers in the system
        at run time.
        For systems with single buffer this function will have no effect.
        The size of the buffer is defined by the dimension of the screen
        and the color depth used.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_DrawBufferInitialize(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t index,
                                uint32_t address)
{
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DrawBufferSet(uint16_t index)

    Summary:
        This function sets the draw buffer with the given index number.

    Description:
        This function sets the draw buffer with the given index number.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_DrawBufferSet(SYS_MODULE_INDEX gfxIndex, uint16_t index)
{
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_DrawBufferGet(void)

    Summary:
        This function returns the index of the current draw buffer.

    Description:
        This function returns the index of the current draw buffer.
        Draw buffer is the buffer where rendering is performed.
        For systems with single buffer this function will always
        return 0.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_DrawBufferGet(SYS_MODULE_INDEX gfxIndex)
{
    return (0);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_FrameBufferSet(uint16_t index)

    Summary:
        This function sets the frame buffer with the given index number.

    Description:
        This function sets the frame buffer with the given index number.
        This is the buffer that is displayed on the screen.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_FrameBufferSet(SYS_MODULE_INDEX gfxIndex, uint16_t index)
{
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_FrameBufferGet(void)

    Summary:
        This function returns the index of the current frame buffer.

    Description:
        This function returns the index of the current frame buffer.
        Frame buffer is the buffer that is currently displayed in the screen.
        For systems with single buffer this function will always
        return 0.

*/
// *****************************************************************************
uint16_t __attribute__((weak)) GFX_FrameBufferGet(SYS_MODULE_INDEX gfxIndex)
{
    return (0);
}
GFX_RECTANGULAR_AREA InvalidatedArea[GFX_MAX_INVALIDATE_AREAS];
// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferEnable(void)

    Summary:
        This function enables the double buffering feature of the graphics
        library.

    Description:

        Double buffering is a feature where two buffers are utilized to
        perform rendering on one buffer while displaying the other buffer.
        The frame buffer is the buffer that is being displayed while the
        draw buffer is used for rendering.

        When this function is called, the buffer with index number 0 is
        set as the draw buffer while the buffer with index number 1 is
        set as the frame buffer. Synchronization of the two buffers is
        scheduled and sets the count of unsynchronized areas to zero.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_DoubleBufferEnable(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if (GFX_DoubleBufferStatusGet(gfxIndex) == GFX_FEATURE_DISABLED)
    {
    
        client->InvalidatedRectangleCount = GFX_MAX_INVALIDATE_AREAS;

        // enable the feature
        client->gfxDoubleBufferParam.gfxDoubleBufferFeature = GFX_FEATURE_ENABLED;
        client->gfxDoubleBufferParam.gfxDoubleBufferFeatureRecentStatus = GFX_FEATURE_ENABLED;
    }
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferDisable(void)

    Summary:
        This function disables the double buffering feature of the graphics
        library.

    Description:
        Double buffering is a feature where two buffers are utilized to
        perform rendering on one buffer while displaying the other buffer.
        The frame buffer is the buffer that is being displayed while the
        draw buffer is used for rendering.

        When this function is called, synchronization of the frame buffer
        and the draw buffer is performed then draw buffer is set to the
        frame buffer index. After this function is called, the frame buffer
        and the draw buffer are the same.
   
*/
// *****************************************************************************
inline GFX_STATUS GFX_DoubleBufferDisable(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if (GFX_DoubleBufferStatusGet(gfxIndex) == GFX_FEATURE_ENABLED)
    {
        GFX_DoubleBufferSynchronize(gfxIndex);

        client->activePage = (client->activePage == GFX_BUFFER1)? GFX_BUFFER2: GFX_BUFFER1;

        gfxDrv->PageSet(ACTIVE_PAGE, client->activePage);

        // disable the feature
        client->gfxDoubleBufferParam.gfxDoubleBufferFeature = GFX_FEATURE_DISABLED;
        client->gfxDoubleBufferParam.gfxDoubleBufferFeatureRecentStatus = GFX_FEATURE_DISABLED;
    }
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferPause(void)

    Summary:
        This function pauses the double buffering feature of the graphics
        library.

    Description:

        Double buffering is a feature where two buffers are utilized to
        perform rendering on one buffer while displaying the other buffer.
        The frame buffer is the buffer that is being displayed while the
        draw buffer is used for rendering.

        When this function is called, the feature is paused and no buffer swap
        will occur.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_DoubleBufferPause(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferFeatureRecentStatus 
            = client->gfxDoubleBufferParam.gfxDoubleBufferFeature;
            
    client->gfxDoubleBufferParam.gfxDoubleBufferFeature = GFX_FEATURE_PAUSED;

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferResume(void)

    Summary:
        This function resumes the double buffering feature of the graphics
        library.

    Description:

        Double buffering is a feature where two buffers are utilized to
        perform rendering on one buffer while displaying the other buffer.
        The frame buffer is the buffer that is being displayed while the
        draw buffer is used for rendering.

        When this function is called, the feature is resumes the previous status
        of the double buffering feature.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_DoubleBufferResume(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferFeature 
            = client->gfxDoubleBufferParam.gfxDoubleBufferFeatureRecentStatus;
            
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS GFX_DoubleBufferStatusGet(void)

    Summary:
        This function returns the status of the double buffering feature
        of the graphics library if enabled or not.

    Description:
        This function returns the status of the double buffering feature
        of the graphics library if enabled or not.

*/
// *****************************************************************************
inline GFX_FEATURE_STATUS GFX_DoubleBufferStatusGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxDoubleBufferParam.gfxDoubleBufferFeature);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSynchronize(void)

    Summary:
        This function synchronizes the contents of the draw and
        frame buffer immediately.

    Description:
        This function synchronizes the contents of the draw and
        frame buffer immediately. Contents of both frame and draw
        buffer will be the same after the function exits.

*/
// *****************************************************************************
//GFX_STATUS __attribute__ ((weak)) GFX_DoubleBufferSynchronize(SYS_MODULE_INDEX gfxIndex)
//{
//    // synchronization will be performed immediately.
////    GFX_PageSet(gfxIndex, VISUAL_PAGE, 0);
////    GFX_PageSet(gfxIndex, ACTIVE_PAGE, 0);
//    return (GFX_STATUS_SUCCESS);
//}

GFX_STATUS GFX_DoubleBufferSynchronize(SYS_MODULE_INDEX gfxIndex)
{
    static uint32_t SourceBuffer, DestBuffer;
    static uint8_t state =0;
    static uint16_t width, height;
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    switch(state)
    {

        case 0:
            if(client->activePage == GFX_BUFFER1)
            {
                SourceBuffer = GFX_BUFFER1;
                DestBuffer   = GFX_BUFFER2;
            }
            else
            {
                SourceBuffer = GFX_BUFFER2;
                DestBuffer   = GFX_BUFFER1;
            }

            client->activePage = DestBuffer;
            client->visualPage = SourceBuffer;
            state = 1;

        case 1:
             gfxDrv->PageSet(ACTIVE_PAGE,client->activePage);
            state = 2;

        case 2:
            gfxDrv->PageSet(VISUAL_PAGE,client->visualPage);

            if(client->InvalidatedRectangleCount >= GFX_MAX_INVALIDATE_AREAS)
            {
                GFX_AlphaParamsSet(gfxIndex, SourceBuffer,0,0,SourceBuffer,0,0,DestBuffer,0,0);
        case 3:
                state = 3;
                GFX_CopyPageWindow(gfxIndex, GFX_MaxXGet(gfxIndex)+1, GFX_MaxYGet(gfxIndex)+1);
                client->InvalidatedRectangleCount = 0;
            }
            else
            {
                while(client->InvalidatedRectangleCount)
                {
                    client->InvalidatedRectangleCount--;

                    width =  InvalidatedArea[client->InvalidatedRectangleCount].right - InvalidatedArea[client->InvalidatedRectangleCount].left + 1;
                    height =  InvalidatedArea[client->InvalidatedRectangleCount].bottom - InvalidatedArea[client->InvalidatedRectangleCount].top + 1;

                    GFX_AlphaParamsSet(gfxIndex, SourceBuffer,InvalidatedArea[client->InvalidatedRectangleCount].left,InvalidatedArea[client->InvalidatedRectangleCount].top,
                                    SourceBuffer,InvalidatedArea[client->InvalidatedRectangleCount].left,InvalidatedArea[client->InvalidatedRectangleCount].top,DestBuffer,
                                    InvalidatedArea[client->InvalidatedRectangleCount].left,InvalidatedArea[client->InvalidatedRectangleCount].top);
                    state = 4;
       case 4:
            
                    GFX_CopyPageWindow(gfxIndex,width, height);
                }
        }
    }

    state = 0;
    return(GFX_STATUS_SUCCESS);
}
// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSynchronizeRequest(void)

    Summary:
        This function requests synchronization of the contents
        of the draw and frame buffer.

    Description:
        This function requests synchronization of the contents
        of the draw and frame buffer. The contents will be synchronized
        on the next vertical blanking period.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_DoubleBufferSynchronizeRequest(SYS_MODULE_INDEX gfxIndex)
{
    GFX_STATUS status = GFX_STATUS_SUCCESS;

    if (GFX_DoubleBufferStatusGet(gfxIndex) == GFX_FEATURE_ENABLED)
    {
        // schedule synchronization
        GFX_DoubleBufferSynchronizeSet(gfxIndex);
    }
    return (status);
}

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeStatusGet(void)

    Summary:
        This function returns the status of the synchronization request
        of the draw and frame buffer.

    Description:
        This function returns the status of the synchronization request
        of the draw and frame buffer.

*/
// *****************************************************************************
inline GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeStatusGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxDoubleBufferParam.gfxDoubleBufferRequestSync);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSynchronizeCancel(void)

    Summary:
        This function cancels the request for synchronization of the contents
        of the draw and frame buffer.

    Description:
        This function cancels the requests synchronization of the contents
        of the draw and frame buffer. The contents will be synchronized
        on the next vertical blanking period.

*/
// *****************************************************************************
inline GFX_STATUS GFX_DoubleBufferSynchronizeCancel(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferRequestSync = GFX_FEATURE_DISABLED;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSynchronizeSet(void)

    Summary:
        This function sets the synchronization request flag.

    Description:
        This function sets the synchronization request flag.
        This function is internal to the library and should not be
        called by the application.

*/
// *****************************************************************************
inline GFX_STATUS GFX_DoubleBufferSynchronizeSet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferRequestSync = GFX_FEATURE_ENABLED;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_DoubleBufferSyncAreaCountGet(void)

    Summary:
        This function returns the current count of rectangular areas
        that needs to be synchronized.

    Description:
        This function returns the current count of rectangular areas
        that needs to be synchronized. 

*/
// *****************************************************************************
inline uint16_t GFX_DoubleBufferSyncAreaCountGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxDoubleBufferParam.gfxUnsyncedAreaCount);
}

// *****************************************************************************
/*  Function:
    void GFX_DoubleBufferSyncAreaCountSet(uint16_t count)

    Summary:
        This function sets the current count of rectangular areas
        that needs to be synchronized.

    Description:
        This function sts the current count of rectangular areas
        that needs to be synchronized.

*/
// *****************************************************************************
inline void GFX_DoubleBufferSyncAreaCountSet(SYS_MODULE_INDEX gfxIndex, uint16_t count)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxUnsyncedAreaCount = count;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSyncAllStatusSet(void)

    Summary:
        This function sets the whole draw buffer to be unsynchronized.

    Description:
        This function sets the whole draw buffer to be unsynchronized.
        The next synchronization will copy all pixels of the
        draw buffer to the frame buffer.

*/
// *****************************************************************************
inline GFX_STATUS GFX_DoubleBufferSyncAllStatusSet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferFullSync = GFX_FEATURE_ENABLED;
    GFX_DoubleBufferSynchronizeRequest(gfxIndex);
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSyncAllStatusClear(void)

    Summary:
        This function clears the synchronize all status.

    Description:
        This function clears the synchronize all status.

*/
// *****************************************************************************
inline GFX_STATUS GFX_DoubleBufferSyncAllStatusClear(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxDoubleBufferParam.gfxDoubleBufferFullSync = GFX_FEATURE_DISABLED;
    GFX_DoubleBufferSynchronizeCancel(gfxIndex);
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS GFX_DoubleBufferSyncAllStatusGet(void)

    Summary:
        This function returns the status of the synchronize all flag.

    Description:
        This function returns the status of the synchronize all flag.

*/
// *****************************************************************************
inline GFX_FEATURE_STATUS GFX_DoubleBufferSyncAllStatusGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxDoubleBufferParam.gfxDoubleBufferFullSync);
}

// *****************************************************************************
/*  Function:
    void GFX_DoubleBufferAreaMark(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function adds the given rectangular area as an area that
        will be included in the list of areas for synchronization.

    Description:
        This function adds the given rectangular area into the list of
        rectangular areas for synchronization. When this function is called,
        the given rectangular area is assumed to contain new pixel information
        and is added into the list to be synchronized.

        Synchronization can be scheduled using the
        GFX_DoubleBufferSynchronizeRequest() or immediately performed
        using GFX_DoubleBufferSynchronize().

*/
// *****************************************************************************
void __attribute__ ((weak)) GFX_DoubleBufferAreaMark(
                                SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{
//    uint16_t count;
//
//    if ((GFX_DoubleBufferSyncAllStatusGet(gfxIndex) == GFX_FEATURE_ENABLED) ||
//        (GFX_DoubleBufferStatusGet(gfxIndex)        == GFX_FEATURE_DISABLED)
//       )
//    {
//        // double buffering feature is disabled, or full synchronization
//        // is scheduled already, don't do anything.
//        return;
//    }
//
//    else if((gfxDoubleBufferParam.gfxUnsyncedAreaCount >= GFX_MAX_INVALIDATE_AREAS) ||
//            (gfxDoubleBufferParam.gfxDoubleBufferFullSync == GFX_FEATURE_ENABLED)
//           )
//    {
//        // full frame synchronization is requested or number of unsynchronized
//        // area reached the max size. Set the synchronization request
//        // flag.
//        GFX_DoubleBufferSyncAllStatusSet(gfxIndex);
//        return;
//    }
//    else
//    {
//        // update the count then get the area parameters
//        count = ++(gfxDoubleBufferParam.gfxUnsyncedAreaCount);
//
//        gfxDoubleBufferParam.gfxDoubleBufferAreas[count].left   = left;
//        gfxDoubleBufferParam.gfxDoubleBufferAreas[count].top    = top;
//        gfxDoubleBufferParam.gfxDoubleBufferAreas[count].right  = right;
//        gfxDoubleBufferParam.gfxDoubleBufferAreas[count].bottom = bottom;
//    }

//    if(gGfxObj[gfxIndex].primData.doubleBuffEnabled == 0)
//    {
//        return;    //Do Nothing
//    }
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if(!left && !top && (right == GFX_MaxXGet(gfxIndex)) && (bottom == GFX_MaxYGet(gfxIndex)))
    {
        client->InvalidatedRectangleCount = GFX_MAX_INVALIDATE_AREAS;
    }
    else if(client->InvalidatedRectangleCount < GFX_MAX_INVALIDATE_AREAS)
    {
        InvalidatedArea[client->InvalidatedRectangleCount].left = left;
        InvalidatedArea[client->InvalidatedRectangleCount].top = top;
        InvalidatedArea[client->InvalidatedRectangleCount].right = right;
        InvalidatedArea[client->InvalidatedRectangleCount].bottom = bottom;
        client->InvalidatedRectangleCount++;
    }
}

// *****************************************************************************
/*  Function:
    GFX_RECTANGULAR_AREA GFX_DoubleBufferAreaGet(uint16_t index)

    Summary:
        This function returns rectangular area position on the
        draw buffer.

    Description:
        This function returns rectangular area position on the
        draw buffer.

*/
// *****************************************************************************
GFX_RECTANGULAR_AREA __attribute__ ((weak)) *GFX_DoubleBufferAreaGet(
                                SYS_MODULE_INDEX gfxIndex, uint16_t count)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (&(client->gfxDoubleBufferParam.gfxDoubleBufferAreas[count]));
}

#endif // #ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_BackgroundSet(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_COLOR color);

    Summary:
        This function sets the background information.

    Description:
        This function sets the background information. Note that the width
        and height of the background is only needed when the
        background is an image. When the background is a
        color (image pointer is NULL), the width and height is
        not needed here since the purpose of the background
        information is to record only the color used.

*/
// *****************************************************************************
void GFX_BackgroundSet(
                               SYS_MODULE_INDEX gfxIndex,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_COLOR color)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxBackground.left   = left;
    client->gfxBackground.top    = top;
    client->gfxBackground.color  = color;
    client->gfxBackground.pImage = pImage;
}

// *****************************************************************************
/*  Function:
    void GFX_BackgroundTypeSet(
                                GFX_BACKGROUND_TYPE type)

    Summary:
        This function sets the background type.

    Description:
        This function sets the background type.

*/
// *****************************************************************************
void GFX_BackgroundTypeSet(
                                SYS_MODULE_INDEX gfxIndex,
                                GFX_BACKGROUND_TYPE type)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxBackground.type = type;
}

// *****************************************************************************
/*  Function:
    GFX_BACKGROUND_TYPE GFX_BackgroundTypeGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the type of the current background.

    Description:
        This function returns the type of the current background.

*/
// *****************************************************************************
GFX_BACKGROUND_TYPE GFX_BackgroundTypeGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBackground.type);
}

// *****************************************************************************
/*  Function:
    GFX_RESOURCE_HDR *GFX_BackgroundImageGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the image used in the current background.

    Description:
        This function returns the pointer to the image used in
        the current background.

*/
// *****************************************************************************
GFX_RESOURCE_HDR *GFX_BackgroundImageGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBackground.pImage);
}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_BackGroundColorGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the color used in the current background.

    Description:
        This function returns the color used in the current background.

*/
// *****************************************************************************
GFX_COLOR GFX_BackgroundColorGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBackground.color);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_BackgroundImageLeftGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the horizontal starting position of 
        the current background.

    Description:
        This function returns the horizontal starting position of 
        the current background. This position defines the 
        x position of the upper left corner of the background.

*/
// *****************************************************************************
uint16_t GFX_BackgroundImageLeftGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBackground.left);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_BackgroundImageTopGet(SYS_MODULE_INDEX gfxIndex)

    Summary:
        This function returns the vertical starting position of 
        the current background.

    Description:
        This function returns the vertical starting position of 
        the current background. This position defines the 
        y position of the upper left corner of the background.
        
*/
// *****************************************************************************
uint16_t GFX_BackgroundImageTopGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxBackground.top);
}

/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
//                         INTERNAL FUNCTIONS/MACROS
/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */

// *****************************************************************************
/*  Function:
    void GFX_CirclePointGet(
                                SYS_MODULE_INDEX gfxIndex,
                                int16_t radius,
                                int16_t angle,
                                int16_t *pX,
                                int16_t *pY)

    Summary:
        This function sets the x,y position of the point on a
        circle given the angle and the radius.

    Description:
        This function sets the x,y position of the point on a
        circle given the angle and the radius. This function is
        an internal function for the Graphics Library.
        Application should not call this function.

*/
// *****************************************************************************
void GFX_CirclePointGet(
                                SYS_MODULE_INDEX gfxIndex, int16_t radius, int16_t angle, int16_t *x, int16_t *y)
{
    uint32_t   rad;
    int16_t   ang;
    int16_t   temp;

    ang = angle % 45;
    if((angle / 45) & 0x01)
        ang = 45 - ang;

    rad = radius;
    // there is a shift by 8 bits here since this function assumes 
    // a shift on the calculations for accuracy and to avoid long
    // and complex multiplications.
    rad *= ((uint32_t) GFX_SineCosineGet(ang, GFX_TRIG_COSINE_TYPE) << 8);

    *x = ((PRIMITIVE_UINT32_UNION) rad).uint3216BitValue[1];
    rad = radius;
    rad *= ((uint32_t) GFX_SineCosineGet(ang, GFX_TRIG_SINE_TYPE) << 8);

    *y = ((PRIMITIVE_UINT32_UNION) rad).uint3216BitValue[1];

    if(((angle > 45) && (angle < 135)) || ((angle > 225) && (angle < 315)))
    {
        temp = *x;
        *x = *y;
        *y = temp;
    }

    if((angle > 90) && (angle < 270))
    {
        *x = -*x;
    }

    if((angle > 180) && (angle < 360))
    {
        *y = -*y;
    }
}

// *****************************************************************************
/*  Function:
    void GFX_RenderToDisplayBufferDisable(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function disables the image draw functions from rendering
        images to the frame buffer. Instead, the image is rendered to
        the line buffer.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

*/
// *****************************************************************************
inline void GFX_RenderToDisplayBufferDisable(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxImageDrawRenderDisableFlag = 1;
}

// *****************************************************************************
/*  Function:
    void GFX_RenderToDisplayBufferDisable(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function enbles the image draw functions to render
        images to the frame buffer. Images will be rendered to
        the frame buffer after this function is called.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

*/
// *****************************************************************************
inline void GFX_RenderToDisplayBufferEnable(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->gfxImageDrawRenderDisableFlag = 0;
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_RenderToDisplayBufferDisableFlagGet(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function returns the status of the image draw functions mode
        to render images to the frame buffer.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

*/
// *****************************************************************************
inline uint16_t GFX_RenderToDisplayBufferDisableFlagGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    return (client->gfxImageDrawRenderDisableFlag);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ThickBevelDraw(
                                        uint16_t xL,
                                        uint16_t yT,
                                        uint16_t xR,
                                        uint16_t yB,
                                        uint16_t r1,
                                        uint16_t r2,
                                        uint8_t octant)
 
    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Draws the octant arc of the beveled figure with the given centers,
        radii and octant mask.
        When octant = 0xFF and the following are true:
            1. xL = xR, yT = yB , r1 = 0 and r2 = z, a filled circle is drawn
               with a radius of z.
            2. radii have values (where r1 < r2), a full ring with thickness
               of (r2-r1) is drawn.
            3. xL != xR, yT != yB , r1 = 0 and r2 = 0 (where xR > xL and 
               yB > yT) a rectangle is drawn. xL, yT specifies the left,top
               corner and xR, yB specifies the right,bottom corner.
        When octant != 0xFF the figure drawn is the subsection of the 8 
        section figure where each non-zero bit of the octant value specifies
        the octants that will be drawn.

*/
// *****************************************************************************
GFX_STATUS __attribute__((weak)) GFX_ThickBevelDraw(
                                        SYS_MODULE_INDEX gfxIndex,
                                        uint16_t xL, 
                                        uint16_t yT, 
                                        uint16_t xR, 
                                        uint16_t yB, 
                                        uint16_t r1, 
                                        uint16_t r2, 
                                        uint8_t octant)
{

    // this is using a variant of the Midpoint (Bresenham's) Algorithm
    typedef enum
    {
        BEGIN,
        QUAD11,
        BARRIGHT1,
        QUAD12,
        BARRIGHT2,
        QUAD21,
        BARLEFT1,
        QUAD22,
        BARLEFT2,
        QUAD31,
        BARTOP1,
        QUAD32,
        BARTOP2,
        QUAD41,
        BARBOTTOM1,
        QUAD42,
        BARBOTTOM2,
        CHECK,
    } OCTANTARC_STATES;

    PRIMITIVE_UINT32_UNION temp;

    //	int32_t temp1;
    static uint16_t y1Limit, y2Limit;
    static uint16_t x1, x2, y1, y2, ovrlap;
    static int16_t  err1, err2;
    static uint16_t x1Cur, y1Cur, y1New;
    static uint16_t x2Cur, y2Cur, y2New;
    static int16_t  width, height;
    static OCTANTARC_STATES state = BEGIN;

    while(1)
    {
        if(GFX_RenderStatusGet(gfxIndex) == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case BEGIN:
                temp.uint32Value = SIN45 * r1;
                y1Limit = temp.uint3216BitValue[1];
                temp.uint32Value = SIN45 * r2;
                y2Limit = temp.uint3216BitValue[1];

                temp.uint32Value = (uint32_t) (ONEP25 - ((int32_t) r1 << 16));
                err1 = (int16_t) (temp.uint3216BitValue[1]);

                temp.uint32Value = (uint32_t) (ONEP25 - ((int32_t) r2 << 16));
                err2 = (int16_t) (temp.uint3216BitValue[1]);

                x1 = r1;
                x2 = r2;
                y1 = 0;
                y2 = 0;

                x1Cur = x1;
                y1Cur = y1 + 1;
                y1New = y1;
                x2Cur = x2;
                y2Cur = y2;
                y2New = y2;
                state = CHECK;

            case CHECK:
                arc_check_state : if(y2 > y2Limit)
                {
                    // draw the width and height
                    width  = (int16_t)(xR - xL);
                    height = (int16_t)(yB - yT);

                    state = BARRIGHT1;
                    goto arc_draw_width_height_state;
                }

                // y1New & y2New records the last y positions
                y1New = y1;
                y2New = y2;

                if(y1 <= y1Limit)
                {
                    if(err1 > 0)
                    {
                        x1--;
                        err1 += 5;
                        err1 += (y1 - x1) << 1;
                    }
                    else
                    {
                        err1 += 3;
                        err1 += y1 << 1;
                    }

                    y1++;
                }
                else
                {
                    y1++;
                    if(x1 < y1)
                        x1 = y1;
                }

                if(err2 > 0)
                {
                    x2--;
                    err2 += 5;
                    err2 += (y2 - x2) << 1;
                }
                else
                {
                    err2 += 3;
                    err2 += y2 << 1;
                }

                y2++;

                state = QUAD11;

                //break;

            case QUAD11:
                if((x1Cur != x1) || (x2Cur != x2))
                {

                    // 1st octant
                    if(octant & 0x01)
                    {
                        // check overlaps
                        if (yT - x1Cur < yT - y1New)
                            ovrlap = yT - x1Cur;
                        else
                            ovrlap = yT - y1New;

                        GFX_BarDraw(gfxIndex,
                            xR + y2Cur,
                                        yT - x2Cur,
                                        xR + y1New,
                                        ovrlap);
                    }
                }
                else
                {
                    state = CHECK;
                    goto arc_check_state;
                }

                state = QUAD12;
                break;

            case QUAD12:

                // 2nd octant
                if(octant & 0x02)
                {
                    // check overlaps
                    if (xR + x1Cur > xR + y1New)
                        ovrlap = xR + x1Cur;
                    else
                        ovrlap = xR + y1New;
                    GFX_BarDraw(gfxIndex,
                                ovrlap,
                                yT - y1New,
                                xR + x2Cur,
                                yT - y2Cur);
                }

                state = QUAD21;
                break;

            case QUAD21:

                // 3rd octant
                if(octant & 0x04)
                {
                    // check overlaps
                    if (xR + x1Cur > xR + y2New)
                        ovrlap = xR + x1Cur;
                    else
                        ovrlap = xR + y2New;
                    GFX_BarDraw(gfxIndex,
                            ovrlap,
                            yB + y1Cur,
                            xR + x2Cur,
                            yB + y2New);
                }

                state = QUAD22;
                break;

            case QUAD22:

                // 4th octant
                if(octant & 0x08)
                {

                    // check overlaps
                    if (yB + x1Cur > yB + y2New)
                        ovrlap = yB + x1Cur;
                    else
                        ovrlap = yB + y2New;

                    GFX_BarDraw(gfxIndex,
                            xR + y1Cur,
                            ovrlap,
                            xR + y2New,
                            yB + x2Cur);
                }

                state = QUAD31;
                break;

            case QUAD31:

                // 5th octant
                if(octant & 0x10)
                {
                    // check overlaps
                    if (yB + x1Cur > yB + y1New)
                        ovrlap = yB + x1Cur;
                    else
                        ovrlap = yB + y1New;
                    GFX_BarDraw(gfxIndex,
                            xL - y1New,
                            ovrlap,
                            xL - y2Cur,
                            yB + x2Cur);
                }

                state = QUAD32;
                break;

            case QUAD32:

                // 6th octant
                if(octant & 0x20)
                {
                    // check overlaps
                    if (xL - x1Cur < xL - y1New)
                        ovrlap = xL - x1Cur;
                    else
                        ovrlap = xL - y1New;
                    GFX_BarDraw(gfxIndex,
                            xL - x2Cur,
                            yB + y2Cur,
                            ovrlap,
                            yB + y1New);
                }

                state = QUAD41;
                break;

            case QUAD41:

                // 7th octant
                if(octant & 0x40)
                {
                    // check overlaps
                    if (xL - x1Cur < xL - y2New)
                        ovrlap = xL - x1Cur;
                    else
                        ovrlap = xL - y2New;
                    GFX_BarDraw(gfxIndex,
                            xL - x2Cur,
                            yT - y2New,
                            ovrlap,
                            yT - y1Cur);
                }

                state = QUAD42;
                break;

            case QUAD42:

                // 8th octant
                if(octant & 0x80)
                {
                    // check overlaps
                    if (yT - x1Cur < yT - y2New)
                        ovrlap = yT - x1Cur;
                    else
                        ovrlap = yT - y2New;
                    GFX_BarDraw(gfxIndex,
                            xL - y2New,
                            yT - x2Cur,
                            xL - y1Cur,
                            ovrlap);
                }

                // update current values
                x1Cur = x1;
                y1Cur = y1;
                x2Cur = x2;
                y2Cur = y2;
                state = CHECK;
                break;

            case BARRIGHT1:     // draw upper right
                arc_draw_width_height_state :
                if ((width > 1) || (height > 1))
                {
                    if (height > 1)
                    {
                        // draw right
                        if(octant & 0x02)
                        {
                            GFX_BarDraw(gfxIndex,
                                    xR + r1,
                                    yT,
                                    xR + r2,
                                    (yB + yT) >> 1);
                        }
                    }
                    else
                    {
                        state = BARBOTTOM1;
                        break;
                    }
                }
                else
                {
                    state = BEGIN;
                    return (GFX_STATUS_SUCCESS);
                }

                state = BARRIGHT2;
                break;

            case BARRIGHT2:     // draw lower right
                if(octant & 0x04)
                {
                    GFX_BarDraw(gfxIndex,
                            xR + r1,
                            ((yB + yT) >> 1),
                            xR + r2,
                            yB);
                }

                state = BARLEFT1;
                break;

            case BARLEFT1:              // draw upper left

                // draw left
                if(octant & 0x40)
                {
                    GFX_BarDraw(gfxIndex,
                            xL - r2, yT,
                            xL - r1, ((yB + yT) >> 1));
                }

                state = BARLEFT2;
                break;

            case BARLEFT2:              // draw lower left
                if(octant & 0x20)
                {
                    GFX_BarDraw(gfxIndex,
                            xL - r2, ((yB + yT) >> 1),
                            xL - r1, yB);
                }
                if (width > 1)
                    state = BARBOTTOM1;
                else
                {
                    state = BEGIN;
                    return (GFX_STATUS_SUCCESS);
                }


            case BARBOTTOM1:    // draw left bottom
                // draw bottom
                if(octant & 0x10)
                {
                    GFX_BarDraw(gfxIndex,
                            xL, yB + r1,
                            ((xR + xL) >> 1), yB + r2);
                }

                state = BARBOTTOM2;
                break;

            case BARBOTTOM2:    // draw right bottom
                if(octant & 0x08)
                {
                    GFX_BarDraw(gfxIndex,
                            ((xR + xL) >> 1), yB + r1,
                            xR, yB + r2);
                }

                state = BARTOP1;
                break;

            case BARTOP1:       // draw left top
                // draw top
                if(octant & 0x80)
                {
                    GFX_BarDraw(gfxIndex,
                            xL, yT - r2,
                            ((xR + xL) >> 1), yT - r1);
                }

                state = BARTOP2;

            case BARTOP2:               // draw right top
                if(octant & 0x01)
                {
                    GFX_BarDraw(gfxIndex,
                            ((xR + xL) >> 1),
                            yT - r2, xR, yT - r1);
                }
                state = BEGIN;
                return (GFX_STATUS_SUCCESS);
        }                               // end of switch
    }   // end of while
}

GFX_STATUS_BIT GFX_RenderStatusGet(SYS_MODULE_INDEX gfxIndex)
{
    return GFX_STATUS_READY_BIT;
}

/******************************************************************************
  Function:
    GFX_STATUS GFX_PageSet(uint8_t pageType, uint8_t page)

  Summary:

  Description:

  Remarks:

*/
GFX_STATUS GFX_PageSet(SYS_MODULE_INDEX gfxIndex, uint8_t pageType, uint8_t page)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
  
    switch(pageType)
    {
        case ACTIVE_PAGE:
            client->activePage = page;
            break;
        case VISUAL_PAGE:
             client->visualPage = page;
             break;
        case BACKGROUND_PAGE:  //Used for AlphaBlending
             client->backgroundPage = page;
             break;
        case TRANSITION_PAGE:
             client->transitionPage = page;
             break;
    }

    if(gfxDrv->PageSet(pageType, page))
    {
        return(GFX_STATUS_FAILURE);
    }

    return(GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*


  Function:
    GFX_ActivePageGet()

  Summary:
    Returns the active page.

  Description:

  Precondition:
    None.

  Parameters:

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
*/
uint8_t GFX_PageGet(SYS_MODULE_INDEX gfxIndex, uint8_t page)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    if(page == ACTIVE_PAGE)
    {
        return(client->activePage);
    }
    else if(page == VISUAL_PAGE)
    {
        return(client->visualPage);
    }
    else
    {
        return(client->backgroundPage);
    }
}

GFX_LAYER_PARAMS primitiveLayer[1];
/******************************************************************************
  Function:
    GFX_STATUS GFX_Layer(uint8_t layer, uint8_t action, uint16_t param1, uint16_t param2)

  Summary:

  Description:

  Remarks:
*/
GFX_STATUS GFX_Layer(SYS_MODULE_INDEX gfxIndex, uint8_t layer, GFX_LAYER_ACTIONS action, uint16_t param1, uint16_t param2)
{

    switch(action)
    {
        case ENABLE:
            primitiveLayer[layer].on = 1;
            break;

        case DISABLE:
            primitiveLayer[layer].on = 0;
            break;

        case SET_SIZE:
            primitiveLayer[layer].width = param1;
            primitiveLayer[layer].height = param2;
            return(GFX_STATUS_SUCCESS);

        case SET_PAGE:
            primitiveLayer[layer].page = param1;
            return(GFX_STATUS_SUCCESS);

        case SET_PAGE_START:
            primitiveLayer[layer].left = param1;
            primitiveLayer[layer].top = param2;
            return(GFX_STATUS_SUCCESS);

        case SET_LAYER_START:
            primitiveLayer[layer].layerLeft = param1;
            primitiveLayer[layer].layerTop = param2;
            return(GFX_STATUS_SUCCESS);
        case UPDATE:
            break;
    }

    gfxDrv->Layer(layer, &primitiveLayer[layer]);

    return(GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*

    Function:
        void GFX_ClipSet(SYS_MODULE_INDEX index,
                         GFX_COORD x, GFX_COORD y,
                         GFX_COORD width, GFX_COORD height)

    Summary:
        Sets the global clipping region for this instance..

*/
// *****************************************************************************
void GFX_ClipSet(SYS_MODULE_INDEX gfxIndex, GFX_COORD x, GFX_COORD y, GFX_COORD width, GFX_COORD height )
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    // set max display area if invalid values
    if(  width == 0 || height == 0 )
    {
        client->gfxClipX = 0;
        client->gfxClipY = 0;
        client->gfxClipW = GFX_MaxXGet(gfxIndex);
        client->gfxClipH = GFX_MaxYGet(gfxIndex);
    }
    else
    {
        client->gfxClipX = x;
        client->gfxClipY = y;
        client->gfxClipW = width;
        client->gfxClipH = height;
    }

}

// *****************************************************************************
/*

    Function:
        void GFX_ClipCoordIn(SYS_MODULE_INDEX index,
                         GFX_COORD x, GFX_COORD y)

    Summary:
        Returns GFX_STATUS_SUCCESS if position is in the global clipping
        region, otherwise it returns GFX_STATUS_FAILURE.

*/
// *****************************************************************************
GFX_STATUS GFX_ClipCoordIn(SYS_MODULE_INDEX gfxIndex, GFX_COORD x, GFX_COORD y)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    GFX_STATUS status = GFX_STATUS_FAILURE;

    if( x >= client->gfxClipX && x < client->gfxClipW + client->gfxClipX &&
        y >= client->gfxClipY && y < client->gfxClipH + client->gfxClipY)
    {
        status = GFX_STATUS_SUCCESS;
    }

    return status;
}


GFX_STATUS GFX_CopyPageWindow(SYS_MODULE_INDEX gfxIndex, uint16_t  width, uint16_t height)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    // check for accelerated version
    if ( gfxDrv->AlphaBlendWindow )
    {
        gfxDrv->AlphaBlendWindow(&client->alphaArea, width, height, 100);
    }
    // otherwise call software fallback
    else
    {
        _AlphaBlendWindow(gfxIndex, width, height, 100);
    }
    return GFX_STATUS_SUCCESS;
}

GFX_STATUS GFX_AlphaBlendWindow(SYS_MODULE_INDEX gfxIndex, uint16_t  width, uint16_t height, uint8_t  alphaPercent)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    // check for accelerated version
    if ( gfxDrv->AlphaBlendWindow )
    {
        gfxDrv->AlphaBlendWindow(&client->alphaArea, width, height, alphaPercent);
    }
    // otherwise call software fallback
    else
    {
        _AlphaBlendWindow(gfxIndex, width, height, alphaPercent);
    }
    return GFX_STATUS_SUCCESS;
}
/******************************************************************************
  Function:
    GFX_STATUS GFX_AlphaBlendWindow(GFX_ALPHA_PARAMS *alphaParams,uint16_t  width, uint16_t height, uint8_t  alphaPercent)

  Summary:

  Description:

  Remarks:

*/
GFX_STATUS _AlphaBlendWindow(SYS_MODULE_INDEX gfxIndex, uint16_t  width, uint16_t height, uint8_t  alphaPercent)
{

        GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
        uint16_t x, y;
        GFX_COLOR *bcolor, *fcolor;

                    y = 0;

                    while(y < height)
                    {

                        gfxDrv->PageSet(ACTIVE_PAGE, client->alphaArea.foregroundPage);

                        gfxDrv->PixelArrayGet(&client->gfxLineBuffer0[0],client->alphaArea.foregroundLeft,client->alphaArea.foregroundTop, width);

                        gfxDrv->PageSet(ACTIVE_PAGE, client->alphaArea.backgroundPage);

                        gfxDrv->PixelArrayGet(&client->alphaLine[0],client->alphaArea.backgroundLeft,client->alphaArea.backgroundTop, width);

                        fcolor = &client->gfxLineBuffer0[0];
                        bcolor = &client->alphaLine[0];

                        if(alphaPercent != 100)
                        {
                            for(x=0; x< width; x++)
                            {
                                switch(alphaPercent)
                                {
                                    case 50:
                                        *fcolor = GFX_Color50Convert(*fcolor)+ GFX_Color50Convert(*bcolor);
                                         break;
                                    case 75:
                                        *fcolor = GFX_Color75Convert(*fcolor)+ GFX_Color25Convert(*bcolor);
                                        break;
                                    case 25:
                                        *fcolor = GFX_Color25Convert(*fcolor)+ GFX_Color75Convert(*bcolor);
                                        break;
                                    default:
                                        break;
                                }
                                fcolor++;
                                bcolor++;
                            }
                        }


                        gfxDrv->PageSet(ACTIVE_PAGE, client->alphaArea.destinationPage);

                        gfxDrv->PixelArrayPut(&client->gfxLineBuffer0[0],client->alphaArea.destinationLeft,
                        client->alphaArea.destinationTop, width, 1);

                        client->alphaArea.foregroundTop++;
                        client->alphaArea.backgroundTop++;
                        client->alphaArea.destinationTop++;
                        y++;
                    }

    return (GFX_STATUS_SUCCESS);

}

void GFX_AlphaParamsSet(SYS_MODULE_INDEX gfxIndex, short fArea,short fLeft,short fTop,
                        short bArea,short bLeft,short bTop,
                        short dArea,short dLeft,short dTop)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];

    client->alphaArea.foregroundPage = fArea;
    client->alphaArea.foregroundLeft = fLeft;
    client->alphaArea.foregroundTop = fTop;
    client->alphaArea.backgroundPage = bArea;
    client->alphaArea.backgroundLeft = bLeft;
    client->alphaArea.backgroundTop = bTop;
    client->alphaArea.destinationPage = dArea;
    client->alphaArea.destinationLeft = dLeft;
    client->alphaArea.destinationTop = dTop;
}

uint16_t GFX_MaxXGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    return client->drvInterface.MaxXGet();
}

uint16_t GFX_MaxYGet(SYS_MODULE_INDEX gfxIndex)
{
    GFX_PRIMITIVE_CLIENT_OBJ * client = &primitiveClient[gfxIndex];
    return client->drvInterface.MaxYGet();
}

/*******************************************************************************
 End of File
*/
