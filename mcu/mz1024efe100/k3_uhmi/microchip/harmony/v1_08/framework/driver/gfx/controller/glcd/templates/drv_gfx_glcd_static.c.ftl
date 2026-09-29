/********************************************************************************
  GFX GLCD Driver Functions for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd.c

  Summary:
    Source code for the GFX GLCD driver static implementation.

  Description:
    This file contains the source code for the static implementation of the
    GFX GLCD driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/gfx/controller/glcd/drv_gfx_glcd_static.h"
#include "driver/gfx/gfx_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

//Layer Parameters
//--------------------------------------------------------------------------
typedef struct __display_layer {
    GFX_COLOR  *baseaddr[2];
    int        draw;
    int        frame;
    uint32_t   resx;
    uint32_t   resy;
    uint32_t   buscfg;
    uint32_t   format;
    uint32_t   stride;
    uint32_t   startx;
    uint32_t   starty;
    uint32_t   sizex;
    uint32_t   sizey;
    uint32_t   alpha;
    uint32_t   dblend;
    uint32_t   sblend;
    uint32_t   colorspace;
    GFX_COLOR  color;
} DISPLAY_LAYER;
static DISPLAY_LAYER layer[GLCD_LAYER_ID_MAX];


// *****************************************************************************
/* GFX GLCD Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_GFX_GLCD_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    DRV_GFX_INIT *                         initData;

    DISPLAY_LAYER                               layer[GLCD_LAYER_ID_MAX];

    uint16_t                                    maxY;

    uint16_t                                    maxX;

} DRV_GFX_GLCD_OBJ;

static DRV_GFX_GLCD_OBJ        drvGLCDObj;


// *****************************************************************************
// *****************************************************************************
// Section: GFX GLCD Driver Interface Implementations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                                          const SYS_MODULE_INIT    * const moduleInit)
  Summary:
    Initializes the GLCD driver instance for the specified module index.

*/
    
SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    uint32_t      modeReg;
    uint32_t      xResolution;
    uint32_t      yResolution;
    uint32_t      rightMargin;
    uint32_t      lowerMargin;
    uint32_t      hsyncLength;
    uint32_t      vsyncLength;
    uint32_t      leftMargin;
    uint32_t      upperMargin;

    DRV_GFX_GLCD_OBJ *dObj = &drvGLCDObj;
    
    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->initData = (DRV_GFX_INIT *) init;
    
    /* set temporary information */
    xResolution     = ${CONFIG_DRV_GFX_DISP_HOR_RESOLUTION};
    yResolution     = ${CONFIG_DRV_GFX_DISP_VER_RESOLUTION};
    rightMargin     = ${CONFIG_DRV_GFX_DISP_HOR_FRONT_PORCH};
    leftMargin      = ${CONFIG_DRV_GFX_DISP_HOR_BACK_PORCH};
    hsyncLength     = ${CONFIG_DRV_GFX_DISP_HOR_PULSE_WIDTH};
    vsyncLength     = ${CONFIG_DRV_GFX_DISP_VER_PULSE_WIDTH};
    upperMargin     = ${CONFIG_DRV_GFX_DISP_VER_BACK_PORCH};
    lowerMargin     = ${CONFIG_DRV_GFX_DISP_VER_FRONT_PORCH};
    
    /* glcd initialization */
    PLIB_GLCD_Disable(GLCD_ID_0);
    PLIB_GLCD_BackgroundColorSet(GLCD_ID_0, ${CONFIG_DRV_GFX_GLCD_BACKGROUND_COLOR});
    PLIB_GLCD_VSyncInterruptEnable(GLCD_ID_0);
    PLIB_GLCD_RGBSequentialModeSet(GLCD_ID_0, 1<<31);
 
    PLIB_GLCD_FrontPorchXYSet(GLCD_ID_0, xResolution + rightMargin, yResolution + lowerMargin);
    PLIB_GLCD_BlankingXYSet(GLCD_ID_0, xResolution + rightMargin + hsyncLength, yResolution + lowerMargin + vsyncLength);
    PLIB_GLCD_BackPorchXYSet(GLCD_ID_0, xResolution + rightMargin + hsyncLength + leftMargin, yResolution + lowerMargin + vsyncLength + upperMargin);
    <#if CONFIG_DRV_GFX_GLCD_CURSOR == true>
    PLIB_GLCD_CursorEnable(GLCD_ID_0);
    </#if>
    PLIB_GLCD_ClockDividerSet(GLCD_ID_0, ${CONFIG_DRV_GFX_DISP_PIXEL_CLOCK_FACTOR});
    PLIB_GLCD_ResolutionXYSet(GLCD_ID_0, xResolution, yResolution);

    <#if CONFIG_DRV_GFX_DISP_VSYNC_NEGATIVE_POLARITY == true>
    PLIB_GLCD_SignalPolaritySet(  GLCD_ID_0, GLCD_VSYNC_POLARITY_NEGATIVE );
    </#if>
    <#if CONFIG_DRV_GFX_DISP_HSYNC_NEGATIVE_POLARITY == true>
    PLIB_GLCD_SignalPolaritySet(  GLCD_ID_0, GLCD_HSYNC_POLARITY_NEGATIVE );
    </#if>

    PLIB_GLCD_Enable(GLCD_ID_0);   

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_Open( const SYS_MODULE_INDEX index,
                              const DRV_IO_INTENT intent )

  Summary:
    Opens a GLCD Layer instance for the specified driver index.
    <p><b>Implementation:</b> Static</p>
*/
DRV_GFX_HANDLE DRV_GFX_GLCD_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
    DRV_GFX_GLCD_OBJ *dObj = &drvGLCDObj;

    DISPLAY_LAYER layer;

    switch (index)
    {
        case GLCD_LAYER_ID_0:
            layer.baseaddr[0] = (GFX_COLOR*)GFX_GLCD_LAYER0_BASEADDR;
            layer.baseaddr[1] = (GFX_COLOR*)GFX_GLCD_LAYER0_DBL_BASEADDR;
            layer.resx       = ${CONFIG_DRV_GFX_GLCD_LAYER0_RES_X};
            layer.resy       = ${CONFIG_DRV_GFX_GLCD_LAYER0_RES_Y};
            layer.startx     = ${CONFIG_DRV_GFX_GLCD_LAYER0_START_X};
            layer.starty     = ${CONFIG_DRV_GFX_GLCD_LAYER0_START_Y};
            layer.sizex      = layer.resx;
            layer.sizey      = layer.resy;
            layer.alpha      = ${CONFIG_DRV_GFX_GLCD_LAYER0_ALPHA_VALUE};
            layer.dblend     = ${CONFIG_DRV_GFX_GLCD_LAYER0_DESTINATION_BLEND};
            layer.sblend     = ${CONFIG_DRV_GFX_GLCD_LAYER0_SOURCE_BLEND};
            layer.colorspace = ${CONFIG_DRV_GFX_GLCD_LAYER0_COLORSPACE};
            break;

<#if CONFIG_DRV_GFX_GLCD_LAYER_1>
        case GLCD_LAYER_ID_1:
            layer.baseaddr[0] = (GFX_COLOR*)GFX_GLCD_LAYER1_BASEADDR;
            layer.baseaddr[1] = (GFX_COLOR*)GFX_GLCD_LAYER1_DBL_BASEADDR;
            layer.resx       = ${CONFIG_DRV_GFX_GLCD_LAYER1_RES_X};
            layer.resy       = ${CONFIG_DRV_GFX_GLCD_LAYER1_RES_Y};
            layer.startx     = ${CONFIG_DRV_GFX_GLCD_LAYER1_START_X};
            layer.starty     = ${CONFIG_DRV_GFX_GLCD_LAYER1_START_Y};
            layer.sizex      = layer.resx;
            layer.sizey      = layer.resy;
            layer.alpha      = ${CONFIG_DRV_GFX_GLCD_LAYER1_ALPHA_VALUE};
            layer.dblend     = ${CONFIG_DRV_GFX_GLCD_LAYER1_DESTINATION_BLEND};
            layer.sblend     = ${CONFIG_DRV_GFX_GLCD_LAYER1_SOURCE_BLEND};
            layer.colorspace = ${CONFIG_DRV_GFX_GLCD_LAYER1_COLORSPACE};
            break;
</#if>
<#if CONFIG_DRV_GFX_GLCD_LAYER_2>
        case GLCD_LAYER_ID_2:
            layer.baseaddr[0] = (GFX_COLOR*)GFX_GLCD_LAYER2_BASEADDR;
            layer.baseaddr[1] = (GFX_COLOR*)GFX_GLCD_LAYER2_DBL_BASEADDR;
            layer.resx       = ${CONFIG_DRV_GFX_GLCD_LAYER2_RES_X};
            layer.resy       = ${CONFIG_DRV_GFX_GLCD_LAYER2_RES_Y};
            layer.startx     = ${CONFIG_DRV_GFX_GLCD_LAYER2_START_X};
            layer.starty     = ${CONFIG_DRV_GFX_GLCD_LAYER2_START_Y};
            layer.sizex      = layer.resx;
            layer.sizey      = layer.resy;
            layer.alpha      = ${CONFIG_DRV_GFX_GLCD_LAYER2_ALPHA_VALUE};
            layer.dblend     = ${CONFIG_DRV_GFX_GLCD_LAYER2_DESTINATION_BLEND};
            layer.sblend     = ${CONFIG_DRV_GFX_GLCD_LAYER2_SOURCE_BLEND};
            layer.colorspace = ${CONFIG_DRV_GFX_GLCD_LAYER2_COLORSPACE};
            break;
</#if>

        default:
            break;
    }

    dObj->layer[index].baseaddr[0] = layer.baseaddr[0];
    dObj->layer[index].baseaddr[1] = layer.baseaddr[1];
    dObj->layer[index].resx     = layer.resx;
    dObj->layer[index].resy     = layer.resy;
    dObj->layer[index].startx   = layer.startx;
    dObj->layer[index].starty   = layer.starty;
    dObj->layer[index].sizex    = layer.sizex;
    dObj->layer[index].sizey    = layer.sizey;
    dObj->layer[index].draw     = 0;
    dObj->layer[index].frame    = 0;

    PLIB_GLCD_LayerBaseAddressSet(GLCD_ID_0, index, (uint32_t)dObj->layer[index].baseaddr[0]);
    PLIB_GLCD_LayerStrideSet(GLCD_ID_0, index, dObj->layer[index].resx*4 );
    PLIB_GLCD_LayerResXYSet( GLCD_ID_0, index, dObj->layer[index].resx, dObj->layer[index].resy );
    PLIB_GLCD_LayerStartXYSet( GLCD_ID_0, index, dObj->layer[index].startx, dObj->layer[index].starty );
    PLIB_GLCD_LayerSizeXYSet( GLCD_ID_0, index, dObj->layer[index].sizex, dObj->layer[index].sizey);
    PLIB_GLCD_LayerGlobalAlphaSet( GLCD_ID_0, index, layer.alpha);
    PLIB_GLCD_LayerDestBlendFuncSet(GLCD_ID_0, index, layer.dblend );
    PLIB_GLCD_LayerSrcBlendFuncSet(GLCD_ID_0, index, layer.sblend );
    PLIB_GLCD_LayerColorModeSet(GLCD_ID_0, index, layer.colorspace );
    PLIB_GLCD_LayerEnable(GLCD_ID_0, index);

    /* Return invalid handle */
    return ((DRV_HANDLE)0);
}

// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_Close(const SYS_MODULE_INDEX index)

  Summary:
    Closes the GLCD instance for the specified driver index.
    <p><b>Implementation:</b> Static</p>

*/
void DRV_GFX_GLCD_Close(const SYS_MODULE_INDEX index)
{
    /* not implemented */
}

// *****************************************************************************
/* Function:
     void  DRV_GFX_GLCD_BackgroundColorSet(const SYS_MODULE_INDEX index, uint32_t bgColor)

  Summary:
     Sets the background color to the specified RGBA value.
*/
void  DRV_GFX_GLCD_BackgroundColorSet(const SYS_MODULE_INDEX index, uint32_t bgColor)
{
    PLIB_GLCD_BackgroundColorSet(GLCD_ID_0, bgColor);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerAlphaSet(const SYS_MODULE_INDEX index, uint8_t alpha)

  Summary:
    Sets the alpha value for the specified layer index.
*/
void DRV_GFX_GLCD_LayerAlphaSet(const SYS_MODULE_INDEX index, uint8_t alpha)
{
    PLIB_GLCD_LayerGlobalAlphaSet( GLCD_ID_0, index, alpha);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerDestBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_DEST_BLEND_FUNC blend)

  Summary:
    Sets the destination blend value to be used for the specified layer index.
*/
void DRV_GFX_GLCD_LayerDestBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_DEST_BLEND_FUNC blend)
{
    PLIB_GLCD_LayerDestBlendFuncSet(GLCD_ID_0, index, blend );
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerSrcBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_SRC_BLEND_FUNC blend)

  Summary:
    Sets the source blend value to be used for the specified layer index.
*/
void DRV_GFX_GLCD_LayerSrcBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_SRC_BLEND_FUNC blend)
{
    PLIB_GLCD_LayerSrcBlendFuncSet(GLCD_ID_0, index, blend );
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerColorSpaceSet(const SYS_MODULE_INDEX index, GLCD_LAYER_COLOR_MODE colorSpace)

  Summary:
    Sets the color space to be used for the specified layer index.
*/
void DRV_GFX_GLCD_LayerColorSpaceSet(const SYS_MODULE_INDEX index, GLCD_LAYER_COLOR_MODE colorSpace)
{
    PLIB_GLCD_LayerColorModeSet(GLCD_ID_0, index, colorSpace );
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerEnableSet(const SYS_MODULE_INDEX index, bool enable)

  Summary:
    Enables or disables the specified layer index based on the given enable flag.
*/
void DRV_GFX_GLCD_LayerEnableSet(const SYS_MODULE_INDEX index, bool enable)
{
    if ( enable )
    {
        PLIB_GLCD_LayerEnable(GLCD_ID_0, index);
    } else {
        PLIB_GLCD_LayerDisable(GLCD_ID_0, index );
    }
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerModeSet(const SYS_MODULE_INDEX index, uint32_t layerMode)

  Summary:
    Sets the enable state for the specified layer index.
    <p><b>Implementation:</b> Static</p>
*/
void DRV_GFX_GLCD_LayerModeSet(const SYS_MODULE_INDEX index, uint32_t layerMode)
{
    if ( layerMode & GLCD_LAYER_ENABLE )
    {
        PLIB_GLCD_LayerEnable(GLCD_ID_0, index );
    } else {
        PLIB_GLCD_LayerDisable(GLCD_ID_0, index );
    }
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerFrameBufferSet(const SYS_MODULE_INDEX index, uint32_t * frame)

  Summary:
    Sets the baseaddress of the layer to the specific memory location.

*/
void DRV_GFX_GLCD_LayerFrameBufferSet(const SYS_MODULE_INDEX index, uint32_t * frame)
{
    PLIB_GLCD_LayerBaseAddressSet(GLCD_ID_0, index, (uint32_t)drvGLCDObj.layer[index].baseaddr[0]);
}

/*********************************************************************
  Function:
     void DRV_GFX_GLCD_InterfaceSet( const SYS_MODULE_INDEX index, DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_GLCD_InterfaceSet( const SYS_MODULE_INDEX index, DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )
{
    interface->BarFill = DRV_GFX_GLCD_BarFill;
    interface->PixelArrayPut = DRV_GFX_GLCD_PixelArrayPut;
    interface->PixelPut = DRV_GFX_GLCD_PixelPut;
    interface->ColorSet = DRV_GFX_GLCD_SetColor;
    interface->MaxXGet = DRV_GFX_GLCD_MaxXGet;
    interface->MaxYGet = DRV_GFX_GLCD_MaxYGet;
    interface->FrameBufferSet = DRV_GFX_GLCD_FrameBufferSet;
    interface->FrameBufferGet = DRV_GFX_GLCD_FrameBufferGet;
    interface->DrawBufferSet = DRV_GFX_GLCD_DrawBufferSet;
    interface->DoubleBufferSynchronizeRequest = DRV_GFX_GLCD_DoubleBufferSynchronizeRequest;
    interface->DoubleBufferSynchronize = DRV_GFX_GLCD_DoubleBufferSynchronize;
}

// *****************************************************************************
/*
  Function:
     uint16_t DRV_GFX_GLCD_MaxXGet(const SYS_MODULE_INDEX index)

  Summary:
     Returns x extent of the display.
*/
uint16_t DRV_GFX_GLCD_MaxXGet(const SYS_MODULE_INDEX index)
{
    return drvGLCDObj.layer[index].resx-1;
}

// *****************************************************************************
/*
  Function:
     uint16_t DRV_GFX_GLCD_MaxYGet(const SYS_MODULE_INDEX index)

  Summary:
     Returns y extent of the display.

*/
uint16_t DRV_GFX_GLCD_MaxYGet(const SYS_MODULE_INDEX index)
{
    return drvGLCDObj.layer[index].resy-1;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_SetColor(const SYS_MODULE_INDEX index, GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:
  
  Output: none

*/

void DRV_GFX_GLCD_SetColor(const SYS_MODULE_INDEX index, GFX_COLOR color)
{
    drvGLCDObj.layer[index].color = color;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_PixelPut(const SYS_MODULE_INDEX index, uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        x,y - pixel coordinates
  Output:
    1 - call not successful (lcc driver busy)
    0 - call successful
*/
void DRV_GFX_GLCD_PixelPut(const SYS_MODULE_INDEX index, uint16_t x, uint16_t y)
{
    int draw = drvGLCDObj.layer[index].draw;
    *(GFX_COLOR*)(drvGLCDObj.layer[index].baseaddr[draw]+y*DISP_HOR_RESOLUTION+x) = drvGLCDObj.layer[index].color;
}

// *****************************************************************************
/*
  Function: void  DRV_GFX_GLCD_PixelArrayPut(const SYS_MODULE_INDEX index, GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

  Summary:
    outputs an array of pixels of length count starting at *color 

  Description:
    none

  Input:
          instance - driver instance
          *color - start of the array
		  x - x coordinate of the start point.
		  y - y coordinate of the end point.
		  count - number of pixels
  Output:
         handle to the number of pixels remaining
*/
void  DRV_GFX_GLCD_PixelArrayPut(const SYS_MODULE_INDEX index, GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{
    uint16_t    z = 0;
    int16_t     newLeft, newTop, newRight, newBottom;

    int draw = drvGLCDObj.layer[index].draw;

    newLeft   = x;
    newTop    = y;
    newRight  = x + count - 1;
    newBottom = y;

    while(lineCount)
    {
        for(z = newLeft; z <= newRight; z++)
        {
            *(GFX_COLOR*)(drvGLCDObj.layer[index].baseaddr[draw]+newTop*DISP_HOR_RESOLUTION+z) = *color++;
        }
        newTop++;
        lineCount--;
    }
} 

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_BarFill(const SYS_MODULE_INDEX index, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        left,top - pixel coordinates
        right, bottom - pixel coordinates

  Output:
          1 - call not successful (lcc driver busy)
          0 - call successful
*/
void  DRV_GFX_GLCD_BarFill(const SYS_MODULE_INDEX index, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    uint16_t   x, y;
    int draw = drvGLCDObj.layer[index].draw;

    for(y = top; y < bottom + 1; y++)
        for(x = left; x < right + 1; x++)
            *(GFX_COLOR*)(drvGLCDObj.layer[index].baseaddr[draw]+y*DISP_HOR_RESOLUTION+x) = drvGLCDObj.layer[index].color;
}

// *****************************************************************************
/*
  Function:
     void  DRV_GFX_GLCD_CursorSetPosition(const SYS_MODULE_INDEX index, uint32_t x, uint32_t y, bool enable)

  Summary:
     Renders the cursor at the given x, y position
*/
void  DRV_GFX_GLCD_CursorSetPosition(const SYS_MODULE_INDEX index, uint32_t x, uint32_t y, bool enable)
{
    if ( enable )
    {
        PLIB_GLCD_CursorEnable(GLCD_ID_0);
        PLIB_GLCD_CursorXYSet(GLCD_ID_0, x, y);
    } else {
        PLIB_GLCD_CursorDisable(GLCD_ID_0);
    }
}

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_CursorImageSet(const SYS_MODULE_INDEX index, uint32_t * cursorImage)

  Summary:
     Renders the cursor at the given x,y position
*/
void  DRV_GFX_GLCD_CursorImageSet(const SYS_MODULE_INDEX index, uint32_t * cursorImage)
{
    int addr;
    int mask;
    int shift;
    int x, y;
    int color;
    uint32_t cursorData[128];

    for(x=0;x<32;x++)
        for(y=0;y<32;y++)
        {
            color = cursorImage[y*32+x] & 0xf;
            shift = (x*4);
            addr  = (y*4)+(x>>3);
            mask  = (0xf << shift);
            cursorData[addr] = (cursorData[addr] & ~mask) | (mask & (color << (shift) ));
        }

    PLIB_GLCD_CursorDataSet(GLCD_ID_0, cursorData );
}

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_CursorPaletteSet(const SYS_MODULE_INDEX index, uint32_t * cursorPalette)

  Summary:
    Sets the cursor color lookup table to the given palette.
*/
void  DRV_GFX_GLCD_CursorPaletteSet(const SYS_MODULE_INDEX index, uint32_t * cursorPalette)
{
    PLIB_GLCD_CursorLUTSet( 0, cursorPalette );
}

// *****************************************************************************
/*
  Function: void DRV_GFX_GLCD_GammaPaletteSet(const SYS_MODULE_INDEX index, uint32_t * gammaPalette)

  Summary:
    Sets the gamma lookup table to the given palette.
*/
void  DRV_GFX_GLCD_GammaPaletteSet(const SYS_MODULE_INDEX index, uint32_t * gammaPalette)
{
    PLIB_GLCD_GlobalColorLUTSet( 0, gammaPalette );
}

void  DRV_GFX_GLCD_FrameBufferSet(const SYS_MODULE_INDEX gfxIndex, int frame)
{
    drvGLCDObj.layer[gfxIndex].frame = frame;
    PLIB_GLCD_LayerBaseAddressSet(GLCD_ID_0, gfxIndex, (uint32_t)drvGLCDObj.layer[gfxIndex].baseaddr[frame]);
}

int DRV_GFX_GLCD_FrameBufferGet(const SYS_MODULE_INDEX gfxIndex)
{
    return drvGLCDObj.layer[gfxIndex].frame;
}

void DRV_GFX_GLCD_DrawBufferSet(SYS_MODULE_INDEX gfxIndex, int draw)
{
    drvGLCDObj.layer[gfxIndex].draw = draw;
}


void DRV_GFX_GLCD_DoubleBufferSynchronize(const SYS_MODULE_INDEX gfxIndex)
{
    int draw = drvGLCDObj.layer[gfxIndex].draw;
    int frame = drvGLCDObj.layer[gfxIndex].frame;
    uint16_t   x, y;

    DRV_GFX_GLCD_FrameBufferSet(gfxIndex, draw);
    DRV_GFX_GLCD_DrawBufferSet(gfxIndex, frame);

    for(y = 0; y < DISP_VER_RESOLUTION + 1; y++)
        for(x = 0; x < DISP_HOR_RESOLUTION + 1; x++)
            *(GFX_COLOR*)(drvGLCDObj.layer[gfxIndex].baseaddr[frame]+y*DISP_HOR_RESOLUTION+x) = *(GFX_COLOR*)(drvGLCDObj.layer[gfxIndex].baseaddr[draw]+y*DISP_HOR_RESOLUTION+x);

//        memcpy(drvGLCDObj.layer[gfxIndex].baseaddr[draw], drvGLCDObj.layer[gfxIndex].baseaddr[frame], DISP_VER_RESOLUTION*DISP_HOR_RESOLUTION*4);
//        DRV_GFX_GLCD_FrameBufferSet(gfxIndex, drvGLCDObj.layer[gfxIndex].draw);
//        DRV_GFX_GLCD_DrawBufferSet(gfxIndex, drvGLCDObj.layer[gfxIndex].frame);
//        memcpy(drvGLCDObj.layer[gfxIndex].baseaddr[1], drvGLCDObj.layer[gfxIndex].baseaddr[0], DISP_VER_RESOLUTION*DISP_HOR_RESOLUTION*4);
}

void DRV_GFX_GLCD_DoubleBufferSynchronizeRequest(SYS_MODULE_INDEX gfxIndex)
{
    DRV_GFX_GLCD_DoubleBufferSynchronize(gfxIndex);
}

/*******************************************************************************
 End of File
*/
