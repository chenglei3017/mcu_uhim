/*******************************************************************************
  GFX GLCD Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd_static.h

  Summary:
    GFX GLCD driver interface declarations for the static single instance driver.

  Description:
    The GLCD device driver provides a simple interface to manage the GLCD
    module on Microchip microcontrollers. This file defines the interface
    Declarations for the GLCD driver.

  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.
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

#ifndef _DRV_GFX_GLCD_H
#define _DRV_GFX_GLCD_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
/* Note:  A file that maps the interface definitions above to appropriate static
          implementations (depending on build mode) is included at the bottom of
          this file.
*/

#include "system_config.h"
#include "driver/driver_common.h"
#include "peripheral/glcd/plib_glcd.h"
#include "driver/gfx/controller/drv_gfx_controller.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
// *****************************************************************************
/* GLCD Layer Control Definitions

  Summary:
    Structure holding the layer control definitions

  Description:
    This structure contains the layer control definitions.

  Remarks:
*/

typedef enum {
        GLCD_LAYER_DISABLE   = 0,
        GLCD_LAYER_ENABLE    = 1 << 31,
} GLCD_LAYER_CONTROL;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Headers for the Static Driver
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                                          const SYS_MODULE_INIT    * const moduleInit)
  Summary:
    Initializes the GLCD driver instance for the specified module index.

  Description:
    This routine initializes the GLCD Driver instance for the specified driver
    instance, making it ready for clients to use it. The initialization
    routine is specified by the MHC parameters.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    This routine must be called before any other GLCD routine is called.
    This routine should only be called once during system initialization.
*/
SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                                       const SYS_MODULE_INIT    * const moduleInit);


// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_Open( const SYS_MODULE_INDEX index,
                              const DRV_IO_INTENT intent )

  Summary:
    Opens a GLCD Layer instance for the specified driver index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine opens the GLCD Driver instance for the specified driver
    instance, making it ready for clients to use it.

  Precondition:
    DRV_GFX_GLCD_Initialize has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
*/
DRV_HANDLE DRV_GFX_GLCD_Open( const SYS_MODULE_INDEX index,
                              const DRV_IO_INTENT intent );

// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_Close(const SYS_MODULE_INDEX index)

  Summary:
    Closes the GLCD instance for the specified driver index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine closes the GLCD driver instance for the specified driver
    instance, making it ready for clients to use it.

  Precondition:
    DRV_GFX_GLCD_Initialize has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_Close(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
     void  void  DRV_GFX_GLCD_BackgroundColorSet(const SYS_MODULE_INDEX index, uint32_t bgColor)

  Summary:
     Sets the background color to the specified RGBA value

     <p><b>Implementation:</b> Static</p>

  Description:
     Sets the background color to the specified RGBA value


  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    bgColor                        - RGBA color

  Returns:
    * none

  Remarks:
*/
void  DRV_GFX_GLCD_BackgroundColorSet(const SYS_MODULE_INDEX index, uint32_t bgColor);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerAlphaSet(const SYS_MODULE_INDEX index, uint8_t alpha)

  Summary:
    Sets the alpha value for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the alpha value of the layer ranging from 0 to 0xFF.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_LayerAlphaSet(const SYS_MODULE_INDEX index, uint8_t alpha);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerDestBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_DEST_BLEND_FUNC blend)

  Summary:
    Sets the destination blend value to be used for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the destination blend value to be used for the specified layer index.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_LayerDestBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_DEST_BLEND_FUNC blend);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerSrcBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_SRC_BLEND_FUNC blend)

  Summary:
    Sets the source blend value to be used for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the source blend value to be used for the specified layer index.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_LayerSrcBlendSet(const SYS_MODULE_INDEX index, GLCD_LAYER_SRC_BLEND_FUNC blend);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerColorSpaceSet(const SYS_MODULE_INDEX index, GLCD_LAYER_COLOR_MODE colorSpace)

  Summary:
    Sets the color space to be used for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the color space to be used for the specified layer index.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_LayerColorSpaceSet(const SYS_MODULE_INDEX index, GLCD_LAYER_COLOR_MODE colorSpace);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerEnableSet(const SYS_MODULE_INDEX index, bool enable)

  Summary:
    Enables or disables the specified layer index based on the given enable flag.
    <p><b>Implementation:</b> Static</p>

  Description:
    Enables or disables the specified layer index based on the given enable flag.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
*/
void DRV_GFX_GLCD_LayerEnableSet(const SYS_MODULE_INDEX index, bool enable);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerModeSet(const SYS_MODULE_INDEX index, uint32_t layerMode)

  Summary:
    Sets the mode state for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the mode state of the layer to either show or not show the layer.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Deprecated. Will not be available next release.

*/
void DRV_GFX_GLCD_LayerModeSet(const SYS_MODULE_INDEX index, uint32_t layerMode);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_LayerFrameBufferSet(const SYS_MODULE_INDEX index, uint32_t * frame)

  Summary:
    Sets the baseaddress of the layer to the specific memory location.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets the baseaddress of the layer to the specific memory location.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

*/
void DRV_GFX_GLCD_LayerFrameBufferSet(const SYS_MODULE_INDEX index, uint32_t * frame);


// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_InterfaceSet( const SYS_MODULE_INDEX index,
                                     DRV_HANDLE handle,
                                     DRV_GFX_INTERFACE * interface )

  Summary:
    Sets the driver runtime functions for the specified driver index.
    <p><b>Implementation:</b> Static</p>

  Description:
    This routine sets implementation function function applicable to the gfx driver
    interface.

  Precondition:
    DRV_GFX_GLCD_Initialize has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use
    DRV_HANDLE handle              - driver handle
    DRV_GFX_INTERFACE * interface  - pointer to interface structure

  Returns:
    None.

  Remarks:
*/
void DRV_GFX_GLCD_InterfaceSet( const SYS_MODULE_INDEX index,
                                DRV_HANDLE handle,
                                DRV_GFX_INTERFACE * interface );

// *****************************************************************************
/* Function:
    uint16_t DRV_GFX_GLCD_MaxXGet(const SYS_MODULE_INDEX index)


  Summary:
    Returns the maximum X coordinate value for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
   This routine returns the maximum X coordinate value for the specified layer index.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use

  Returns:
    * x position - When a message has been received

  Remarks:
*/
uint16_t DRV_GFX_GLCD_MaxXGet(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    uint16_t DRV_GFX_GLCD_MaxYGet(const SYS_MODULE_INDEX index)


  Summary:
    Returns the maximum Y coordinate value for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
   This routine returns the maximum Y coordinate value for the specified layer index.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use

  Returns:
    * y position - When a message has been received

  Remarks:
*/
uint16_t DRV_GFX_GLCD_MaxYGet(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_SetColor(const SYS_MODULE_INDEX index, GFX_COLOR color)

  Summary:
    Sets the global pixel color used for PixelPut operations for the specified layer index.
    <p><b>Implementation:</b> Static</p>

  Description:
   Sets the global pixel color used for PixelPut operations using the given color.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use
    GFX_COLOR  color               - color to use

  Returns:
    * none

  Remarks:
*/
void DRV_GFX_GLCD_SetColor(const SYS_MODULE_INDEX index, GFX_COLOR color);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_BarFill(const SYS_MODULE_INDEX index, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    Outputs a rectangle filled with the global color.
    <p><b>Implementation:</b> Static</p>

  Description:
    Outputs a rectangle of pixels into the framebuffer at the given position, width and height.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use
    left, top                      - pixel coordinates
    right, bottom                  - pixel coordinates

  Returns:
    * none

  Remarks:
*/
void DRV_GFX_GLCD_BarFill(const SYS_MODULE_INDEX index, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_PixelPut(const SYS_MODULE_INDEX index, uint16_t x, uint16_t y)

  Summary:
    Outputs one pixel into the frame buffer at the given position.
    <p><b>Implementation:</b> Static</p>

  Description:
    Outputs one pixel into the frame buffer at the x,y coordinate given

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use
    uint16_t x,y                      - pixel coordinates

  Returns:
    * none

  Remarks:
*/
void DRV_GFX_GLCD_PixelPut(const SYS_MODULE_INDEX index, uint16_t x, uint16_t y);

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_PixelArrayPut(const SYS_MODULE_INDEX index, GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

  Summary:
    Outputs an array of pixels of length count starting at *color

    <p><b>Implementation:</b> Static</p>

  Description:
    Outputs an array of pixels of length count starting at *color

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    SYS_MODULE_INDEX index         - layer to use
    *color                         - start of the array
    x                              - x coordinate of the start point.
    y                              - y coordinate of the end point.
    count                          - number of pixels
    lineCount                      - number of lines

  Returns:
    * none

  Remarks:
*/
void DRV_GFX_GLCD_PixelArrayPut(const SYS_MODULE_INDEX index, GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount);

// *****************************************************************************
/* Function:
     void  DRV_GFX_GLCD_CursorSetPosition(const SYS_MODULE_INDEX index, uint32_t x, uint32_t y, bool enable)

  Summary:
     Sets the position and enable state of the cursor.

     <p><b>Implementation:</b> Static</p>

  Description:
     Sets the x, y position of the cursor and its enable state.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    x                              - x coordinate of the start point.
    y                              - y coordinate of the end point.
    enable                         - enable cursor on or off

  Returns:
    * none

  Remarks:
*/
void  DRV_GFX_GLCD_CursorSetPosition(const SYS_MODULE_INDEX index, uint32_t x, uint32_t y, bool enable);

// *****************************************************************************
/* Function:
     void  DRV_GFX_GLCD_CursorImageSet(const SYS_MODULE_INDEX index, uint32_t * cursorImage)

  Summary:
     Sets Cursor image to the image specified.
     <p><b>Implementation:</b> Static</p>

  Description:
     Sets Cursor image to the image specified.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    uint32_t * cursorImage         - 32x32 palatte image

  Returns:
    * none

  Remarks:
*/
void  DRV_GFX_GLCD_CursorImageSet(const SYS_MODULE_INDEX index, uint32_t * cursorImage);


// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_CursorPaletteSet(uint32_t * cursorPalette)

  Summary:
     Sets the Cursor Pallette to the given cursor palette.
     <p><b>Implementation:</b> Static</p>

  Description:
     Sets Cursor image to the image specified.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    uint32_t * cursorImage         - 32x32 palatte image

  Returns:
    * none

  Remarks:
*/
void DRV_GFX_GLCD_CursorPaletteSet(const SYS_MODULE_INDEX index, uint32_t * gammaPalette);


// *****************************************************************************
/* Function:
     void DRV_GFX_GLCD_GammaPaletteSet(const SYS_MODULE_INDEX index, uint32_t * cursorPalette)

  Summary:
     Setsthe global gamma/palette to the given cursor palette.
     <p><b>Implementation:</b> Static</p>

  Description:
     Sets the global gamma/platform image.

  Precondition:
    DRV_GFX_GLCD_Open has been called.

  Parameters:
    uint32_t * cursorImage         - 32x32 palatte image

  Returns:
    * none

  Remarks:
*/
void  DRV_GFX_GLCD_GammaPaletteSet(const SYS_MODULE_INDEX index, uint32_t * gammaPalette);



void  DRV_GFX_GLCD_FrameBufferSet(const SYS_MODULE_INDEX gfxIndex, int page);

int DRV_GFX_GLCD_FrameBufferGet(const SYS_MODULE_INDEX gfxIndex);

void DRV_GFX_GLCD_DrawBufferSet(SYS_MODULE_INDEX gfxIndex, int page);

void DRV_GFX_GLCD_DoubleBufferSynchronize(const SYS_MODULE_INDEX gfxIndex);

void DRV_GFX_GLCD_DoubleBufferSynchronizeRequest(const SYS_MODULE_INDEX gfxIndex);

// ****************************************************************************
// ****************************************************************************
// Section: Included Files (continued)
// ****************************************************************************
// ****************************************************************************
/*  The files included below map the interface definitions above to appropriate
    static implementations, depending on build mode.
*/

#ifdef __cplusplus
    }
#endif
    
#endif // #ifndef _DRV_GFX_GLCD_H
/*******************************************************************************
 End of File
*/
