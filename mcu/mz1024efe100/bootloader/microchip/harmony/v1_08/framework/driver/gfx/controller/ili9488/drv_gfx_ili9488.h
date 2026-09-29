/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_ili9488.c

  Summary:
    Interface for the graphics library where the primitives are rendered and sent 
	to the graphics controller either external or internal

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the ILI9488
	Graphics Controller.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional informationILI9488_REGarding your rights and obligations.

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

#ifndef _DRV_GFX_ILI9488_H
    #define _DRV_GFX_ILI9488_H
// DOM-IGNORE-END

#include "driver/gfx/controller/drv_gfx_controller.h"

//DOM-IGNORE-BEGIN

#ifdef __cplusplus
    extern "C" {
#endif
        
	typedef enum {
	GFX_PAGE0 = 0,
	GFX_PAGE1,
	GFX_PAGE2,
	GFX_PAGE3,
	GFX_PAGE4,
	GFX_PAGE5,
	GFX_PAGE6,
	GFX_PAGE7,
	GFX_PAGE8,
	GFX_PAGE9,
	GFX_PAGE10,
	GFX_PAGE11,
	GFX_PAGE12,
	GFX_PAGE13,
	GFX_PAGE14,
	GFX_PAGE15,
	GFX_NUM_OF_PAGES
	} GFX_PAGE;

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* SSD1926 Driver Module Index Count

  Summary:
    Number of valid ILI9488 driver indices.

  Description:
    This constant identifies ILI9488 driver index definitions.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is device-specific.
*/

#define DRV_GFX_ILI9488_INDEX_COUNT     DRV_GFX_ILI9488_NUMBER_OF_MODULES


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*

  Function: uint16_t DRV_GFX_ILI9488_Initialize(uint8_t instance)

  Summary:
    resets LCD, initializes PMP

  Description:
    none

  Input:
        instance - driver instance
  Output:
     NULL - call not successful (PMP driver busy)
    !NULL - address of the display driver queue command
*/
SYS_MODULE_OBJ DRV_GFX_ILI9488_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                                          const SYS_MODULE_INIT    * const moduleInit);

/*********************************************************************

 Function:
     DRV_GFX_ILI9488_Open(uint8_t instance)
    
  Summary:
    opens an instance of the graphics controller
  Description:
    none
  Return:
    1 - driver not initialized 2 - instance doesn't exist 3 - instance
    already open instance to driver when successful                   
  *********************************************************************/
DRV_HANDLE DRV_GFX_ILI9488_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent );

// *****************************************************************************
/*

  Function: DRV_GFX_ILI9488_Close(uint8_t instance)

  Summary:
    closes an instance of the graphics controller

  Description:
    none

  Input:
    instance of the driver

  Output:
    0 - instance closed
    2 - instance doesn't exist
    3 - instance already closed
*/
void DRV_GFX_ILI9488_Close( DRV_HANDLE handle );

/*********************************************************************
  Function:
     void DRV_GFX_ILI9488_InterfaceSet( DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_ILI9488_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface );

// *****************************************************************************
/*
  Function:
     void DRV_GFX_ILI9488_MaxXGet()

  Summary:
     Returns x extent of the display.

  Description:

  Precondition:

  Parameters:

  Returns:

  Example:
    <code>
    <code>

  Remarks:
*/
uint16_t DRV_GFX_ILI9488_MaxXGet(void);

// *****************************************************************************
/*
  Function:
     void DRV_GFX_ILI9488_MaxYGet()

  Summary:
     Returns y extent of the display.

  Description:

  Precondition:

  Parameters:

  Returns:

  Example:
    <code>
    <code>

  Remarks:
*/
uint16_t DRV_GFX_ILI9488_MaxYGet(void);

// *****************************************************************************
/*

  Function: void DRV_GFX_ILI9488_SetColor(GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:

  Output: none

*/
void DRV_GFX_ILI9488_SetColor(GFX_COLOR color);

// *****************************************************************************
/*

  Function: void DRV_GFX_ILI9488_SetInstance(uint8_t instance)

  Summary: Sets the instance for the driver

  Description:

  Output: none

*/
void DRV_GFX_ILI9488_SetInstance(uint8_t instance);

// *****************************************************************************
/*

  Function: uint16_t DRV_GFX_ILI9488_PixelPut(uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        instance - driver instance
        color - color to output
        x,y - pixel coordinates
  Output:
    NULL - call not successful (PMP driver busy)
    !NULL - address of the display driver queue command
*/
void DRV_GFX_ILI9488_PixelPut(uint16_t x, uint16_t y);

// *****************************************************************************
/*

  Function: void  DRV_GFX_ILI9488_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

  Summary:
    outputs an array of pixels of length count starting at *color 

  Description:
    none

  Input:
          *color - start of the array
		  x - x coordinate of the start point.
		  y - y coordinate of the end point.
		  count - number of pixels
              lineCount - number of lines
  Output:
         NULL - call not successful (PMP driver busy)
        !NULL - address to the number of pixels yet to be serviced
*/
void DRV_GFX_ILI9488_PixelArrayPut(GFX_COLOR *color,uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount);

void  DRV_GFX_ILI9488_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);

uint16_t*  DRV_GFX_ILI9488_PixelArrayGet(GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count);
/*************************************************************************
  Function:
      void DRV_GFX_SSD1926_Tasks(void)

  Summary:
    Task machine that renders the driver calls for the graphics library it
    must be called periodically to output the contents of its circular
    buffer
  *************************************************************************/
void DRV_GFX_ILI9488_Tasks(SYS_MODULE_OBJ object);


//#define ILI9488_BACKLIGHT_SET SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F,PORTS_BIT_POS_8);
//#define ILI9488_BACKLIGHT_CLEAR SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F,PORTS_BIT_POS_8);

// on = 1 display_on, on = 0 display_off
void ILI9488Display(uint8_t on);

#ifdef __cplusplus
    }
#endif
    
#endif //_DRV_GFX_ILI9488_H
