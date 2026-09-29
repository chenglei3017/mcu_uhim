/*******************************************************************************
  Driver Common Header Definitions

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_common.h

  Summary:
    This file defines the common macros and definitions used by the gfx
    definition and implementation headers.

  Description:
    This file defines the common macros and definitions used by the gfx
    definition and the implementation header.

  Remarks:
    The directory in which this file resides should be added to the compiler's
    search path for header files.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2010-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS-IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _GFX_COMMON_H
#define _GFX_COMMON_H
//DOM-IGNORE-END

#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "driver/driver_common.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
#if !defined (GFX_CONFIG_NUMBER_OF_MODULES)
    #define GFX_CONFIG_NUMBER_OF_MODULES GFX_INSTANCES_NUMBER
#endif

// *****************************************************************************
/*
  GFX Module Index Numbers

  Summary:
    Identifies the GFX module index definitions.

  Description:
    These constants provide the GFX Library index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

    These values should be passed into the GFX_Initialize and
    function to identify the driver instance in use.
*/
typedef enum
{
    /* First GFX library instance */
    GFX_INDEX_0,
            
    /* Second GFX library instance */
    GFX_INDEX_1,
            
    /* Third GFX library instance */
    GFX_INDEX_2

} GFX_INDEX;

// *****************************************************************************
/* GFX Module Index Count

  Summary:
    Identifies the number of valid GFX module indices.

  Description:
    This constant identifies the number of valid GFX module indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from device-specific header files defined as part of 
    the peripheral libraries.
*/

#define GFX_INDEX_COUNT     GFX_CONFIG_NUMBER_OF_MODULES

// *****************************************************************************
/*  Data type for GFX handle.

  Summary:
    Data type for GFX handle.

  Description:
    The data type of the handle that is returned from GFX_Open
    function.

  Remarks:
    None.
*/

typedef uintptr_t GFX_HANDLE;

// *****************************************************************************
/* GFX Preemption Levels

   Summary
    Defines the level of preemption of the GFX library.

   Description
    This enumeration defines the various levels that the library can be
   preempted in between an operation.

   Remarks:
    None.
*/

typedef enum
{
    /* blocking - no cooperation */
    GFX_PREEMPTION_LEVEL_0 = 0,

    /* preempts within GOL after each object draw */
    GFX_PREEMPTION_LEVEL_1,

    /* preempts within each Object after each primitive draw */
    GFX_PREEMPTION_LEVEL_2,

    /* preempts within each Primitive (not-available) */
    GFX_PREEMPTION_LEVEL_3,

} GFX_PREEMPTION_LEVEL;

// *****************************************************************************
/* GFX Invalid Handle

  Summary:
    Constant that defines the value of an Invalid Device Handle.

  Description:
    This constant is returned by the GFX_Open() function when the
    function fails.

  Remarks:
    None.
*/

#define GFX_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((GFX_HANDLE)(-1))/*DOM-IGNORE-END*/

#ifndef GFX_CONFIG_COLOR_DEPTH
    #error "Please define GFX_CONFIG_COLOR_DEPTH in gfx_config.h (via system_config.h); valid values (dependent on the driver used) are: 1, 4, 8, 16, 24)"
#endif

#if ((GFX_CONFIG_COLOR_DEPTH != 1) && (GFX_CONFIG_COLOR_DEPTH != 4)  && \
     (GFX_CONFIG_COLOR_DEPTH != 8) && (GFX_CONFIG_COLOR_DEPTH != 16) && \
     (GFX_CONFIG_COLOR_DEPTH != 24 && GFX_CONFIG_COLOR_DEPTH != 32))
    #error "Defined GFX_CONFIG_COLOR_DEPTH value is not suppported. Valid values (dependent on the driver used) are: 1, 4, 8, 16, 24)"
#endif
// DOM-IGNORE-END

/*********************************************************************
 Overview: Data type that defines the color data.
           This type is dependent on the COLOR_DEPTH setting.
           See COLOR_DEPTH.
           - GFX_COLOR is type uint8_t  if COLOR_DEPTH <= 8
           - GFX_COLOR is type uint16_t if COLOR_DEPTH = 16
           - GFX_COLOR is type uint32_t if COLOR_DEPTH = 24

*********************************************************************/
#if (GFX_CONFIG_COLOR_DEPTH <= 8)
    typedef uint8_t GFX_COLOR;
#elif (GFX_CONFIG_COLOR_DEPTH == 16)
    typedef uint16_t GFX_COLOR;
#elif (GFX_CONFIG_COLOR_DEPTH == 24)
    typedef uint32_t GFX_COLOR;
#elif (GFX_CONFIG_COLOR_DEPTH == 32)
    typedef uint32_t GFX_COLOR;
#endif

#if (GFX_CONFIG_COLOR_DEPTH == 16)

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_RGBConvert(
                                uint8_t red,
                                uint8_t blue,
                                uint8_t green)

    Summary:
        Function to convert 24 bpp color data to X bpp color data.

    Description:
        RGB Conversion macro. 24 BPP color to x BPP color conversion.
        Color depths of 1, 2 or 4 are usually for monochrome/grayscale
        format. These are not supported in RGB conversion.

        <TABLE>
		   	COLOR_DEPTH            Conversion
		   	##################     ############
		   	8                      8-8-8 to 3-3-2 conversion
		   	16                     8-8-8 to to 5-6-5 conversion
		   	24                     8-8-8 to 8-8-8 conversion or no conversion
		</TABLE>

    Precondition:
        None.

    Parameters:
        red - red component of the color.
        green - green component of the color.
        blue - blue component of the color.

    Returns:
        The converted color data. Size is dependent on the COLOR_DEPTH.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
#ifndef GFX_RGBConvert
extern inline GFX_COLOR __attribute__((always_inline)) 
                                            GFX_RGBConvert( GFX_COLOR red,
                                                            GFX_COLOR green,
                                                            GFX_COLOR blue )
{
    return  (GFX_COLOR) (
                         ( ( (red  ) & 0xF8) << 8 )  |
                         ( ( (green) & 0xFC) << 3 )  |
                           ( (blue )         >> 3 )
                        );
}
#endif

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_Color50Convert(
                                GFX_COLOR color)

    Summary:
        Function to convert the 50% equivalent of the given color.

    Description:
        Function to convert the 50% equivalent of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be converted.

    Returns:
        The converted color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) 
                                        GFX_Color50Convert( GFX_COLOR color )
{
    return (GFX_COLOR) (GFX_COLOR)((color & (0b1111011111011110))>>1);
}

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_Color25Convert(
                                GFX_COLOR color)

    Summary:
        Function to convert the 25% equivalent of the given color.

    Description:
        Function to convert the 25% equivalent of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be converted.

    Returns:
        The converted color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) 
                                          GFX_Color25Convert( GFX_COLOR color )
{
    return (GFX_COLOR) (GFX_COLOR)((color & (0b1110011110011100))>>2);
}

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_Color75Convert(
                                GFX_COLOR color)

    Summary:
        Function to convert the 75% equivalent of the given color.

    Description:
        Function to convert the 75% equivalent of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be converted.

    Returns:
        The converted color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) 
                                           GFX_Color75Convert( GFX_COLOR color )
{
    return (GFX_COLOR) (GFX_COLOR)( GFX_Color50Convert(color) +
                                    GFX_Color25Convert(color));
}
#elif (GFX_CONFIG_COLOR_DEPTH == 32)
#ifndef GFX_RGBConvert
    #define GFX_RGBAConvert(red, green, blue, alpha)    (GFX_COLOR) (((GFX_COLOR)(red) << 24) | ((GFX_COLOR)(green) << 16) | (GFX_COLOR)(blue) << 8 | (GFX_COLOR)(alpha) )
    #define GFX_RGBConvert(red, green, blue)    (GFX_COLOR) (((GFX_COLOR)(red) << 24) | ((GFX_COLOR)(green) << 16) | (GFX_COLOR)(blue) << 8 | 0)
    #define GFX_ComponentRedGet(color)       (((color) & 0xFF000000) >> 24)
    #define GFX_ComponentGreenGet(color)     (((color) & 0x00FF0000) >> 16)
    #define GFX_ComponentBlueGet(color)      (((color) & 0x0000FF00) >> 8)
    #define GFX_ComponentAlphaGet(color)      ((color) & 0x000000FF)
#endif

   #define ConvertColor50(color)  (GFX_COLOR)((color & (0x00FEFEFEul))>>1)
   #define ConvertColor25(color)  (GFX_COLOR)((color & (0x00FCFCFCul))>>2)
   #define ConvertColor75(color)  (GFX_COLOR)(ConvertColor50(color) + ConvertColor25(color))

#elif (GFX_CONFIG_COLOR_DEPTH == 24)
#ifndef GFX_RGBConvert
    #define GFX_RGBConvert(red, green, blue)    (GFX_COLOR) (((GFX_COLOR)(red) << 16) | ((GFX_COLOR)(green) << 8) | (GFX_COLOR)(blue))
#endif

   #define ConvertColor50(color)  (GFX_COLOR)((color & (0x00FEFEFEul))>>1)
   #define ConvertColor25(color)  (GFX_COLOR)((color & (0x00FCFCFCul))>>2)
   #define ConvertColor75(color)  (GFX_COLOR)(ConvertColor50(color) + ConvertColor25(color))

   #define GFX_Color50Convert ConvertColor50
   #define GFX_Color25Convert ConvertColor25
   #define GFX_Color75Convert ConvertColor75

#elif (GFX_CONFIG_COLOR_DEPTH == 8)
#ifndef GFX_RGBConvert
    #define GFX_RGBConvert(red, green, blue)    (GFX_COLOR) (((GFX_COLOR)(red) & 0xE0) | (((GFX_COLOR)(green) & 0xE0) >> 3) | (((GFX_COLOR)(blue)) >> 6))
#endif

#endif

// Note: Gradient feature does not support
//       GFX_CONFIG_COLOR_DEPTH values of 8, 4 and 1.
//       Gradient feature is not enabled when palette is enabled

#if (GFX_CONFIG_COLOR_DEPTH == 16)
// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_ComponentRedGet(
                                GFX_COLOR color)

    Summary:
        Function to extract the red component of the given color.

    Description:
        Function to extract the red component of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be used to extract the
                red component.

    Returns:
        The red component of the given color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_ComponentRedGet(GFX_COLOR color)
{
    return (GFX_COLOR) (GFX_COLOR)(((color) & 0xF800) >> 8);
}

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_ComponentGreenGet(
                                GFX_COLOR color)

    Summary:
        Function to extract the green component of the given color.

    Description:
        Function to extract the green component of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be used to extract the
                green component.

    Returns:
        The green component of the given color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_ComponentGreenGet(GFX_COLOR color)
{
    return (GFX_COLOR) (((color) & 0x07E0) >> 3);
}

// *****************************************************************************
/*
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_ComponentBlueGet(
                                GFX_COLOR color)

    Summary:
        Function to extract the blue component of the given color.

    Description:
        Function to extract the blue component of the given color.

    Precondition:
        None.

    Parameters:
        color - the color value that will be used to extract the
                blue component.

    Returns:
        The blue component of the given color.

    Example:
        None.

    Remarks:
        None.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_ComponentBlueGet(GFX_COLOR color)
{
    return (GFX_COLOR) (((color) & 0x001F) << 3);
}


#elif (GFX_CONFIG_COLOR_DEPTH == 24)
    #define GFX_ComponentRedGet(color)       (((color) & 0xFF0000) >> 16)
    #define GFX_ComponentGreenGet(color)     (((color) & 0x00FF00) >> 8)
    #define GFX_ComponentBlueGet(color)      ((color) & 0x0000FF)
#endif


// DOM-IGNORE-BEGIN
typedef struct
{
    uint8_t type;
    uint8_t on;
    uint8_t page;
    short   left;
    short   top;
    uint16_t width;
    uint16_t height;
    short   layerLeft;
    short   layerTop;
} GFX_LAYER_PARAMS;

// *****************************************************************************
/* GFX Alphablend Parameters

  Summary:
    GFX Alphablend parameters

  Description:
    Defines the parameters associated with alpha blend area.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t foregroundPage;
    short   foregroundLeft;
    short   foregroundTop;
    uint8_t backgroundPage;
    short   backgroundLeft;
    short   backgroundTop;
    uint8_t destinationPage;
    short   destinationLeft;
    short   destinationTop;
    uint8_t alpha;
    GFX_COLOR prevAlphaColor;
} GFX_ALPHA_PARAMS;

// *********************************************************************
/*
  Structure: GFX_DRV_FUNCTIONS

  Summary: Structure containing the driver functions used by the primitive layer.

  Description: This structure needs to be defined in the driver file to inform the primitive layer
  as to what functions are available.
*/
typedef struct
{
    /* */
    void      (*BarFill)(uint16_t,uint16_t,uint16_t,uint16_t);

    /* */
    void          (*PixelArrayPut)(GFX_COLOR*,uint16_t,uint16_t,uint16_t, uint16_t);

    /* */
    uint16_t*     (*PixelArrayGet)(GFX_COLOR*,uint16_t,uint16_t,uint16_t);

    /* */
    void      (*PixelPut)(uint16_t,uint16_t);

    /* */
    void          (*ColorSet)(GFX_COLOR);

    /* */
    void          (*InstanceSet)(uint8_t);

    /* */
    uint16_t      (*PageSet)(uint8_t, uint8_t);

    /* */
    uint16_t*     (*Layer)(uint8_t, GFX_LAYER_PARAMS*);

    // DOM-IGNORE-BEGIN
    /* */
    uint16_t      (*PixelGet)(uint16_t,uint16_t);
    // DOM-IGNORE-END

    /* */
    uint16_t*     (*AlphaBlendWindow)(GFX_ALPHA_PARAMS*, uint16_t, uint16_t,
                                      uint8_t);

    /* */
    uint16_t      (*MaxXGet)(void);

    /* */
    uint16_t      (*MaxYGet)(void);

    void (*FrameBufferSet)(const SYS_MODULE_INDEX, int);

    int (*FrameBufferGet)(const SYS_MODULE_INDEX);

    void (*DrawBufferSet)(SYS_MODULE_INDEX, int);

    void (*DoubleBufferSynchronize)(const SYS_MODULE_INDEX);

    void (*DoubleBufferSynchronizeRequest)(SYS_MODULE_INDEX gfxIndex);


} DRV_GFX_INTERFACE;

// *****************************************************************************
/* Driver State Machine States

   Summary
    Defines the various states that can be achieved by the driver operation.

   Description
    This enumeration defines the various states that can be achieved by the
    driver operation.

   Remarks:
    None.
*/

typedef enum
{
    /* thin-film transistor */
    LCD_TFT = 1,

    /* monochrome super-twisted */
    LCD_MSTN2,

    /* color super-twisted */
    LCD_CSTN2,

} DRV_GFX_LCD;

// *****************************************************************************
/*
  GFX Driver Initialize Data

  Summary:
    Defines the GFX driver initialization data.

  Description:
    This data type defines data required to initialize or reinitialize the GFX
    driver.

  Remarks:
    Not all initialization features are available on all devices.
*/

typedef struct _GFX_GFX_INIT
{
    /* System module initialization */
    SYS_MODULE_INIT         moduleInit;

    /* Orientation of the display (given in degrees of 0,90,180,270) */
    uint16_t                orientation;

    /* Horizontal Resolution of the displayed orientation in Pixels */
    uint16_t                horizontalResolution;

    /* Vertical resolution of the displayed orientation in pixels */
    uint16_t                verticalResolution;

    /* */
    uint16_t                dataWidth;

    /* Horizontal Pulse Width of the LCD */
    uint16_t                horizontalPulseWidth;

    /* Horizontal BackPorch of the LCD */
    uint16_t                horizontalBackPorch;

    /* Horizontal FrontPorch of the LCD */
    uint16_t                horizontalFrontPorch;

    /*  Vertical Pulse width of the LCD */
    uint16_t                verticalPulseWidth;

    /* Vertical BackPorch of the LCD */
    uint16_t                verticalBackPorch;

    /* Vertical FrontPorch of the LCD */
    uint16_t                verticalFrontPorch;

    /* Rising/Falling edge indicator of the LCD pixel clock */
    uint8_t                 logicShift;

    /* LCD type */
    DRV_GFX_LCD             LCDType;

    /* color depth (18BPP, 24BPP, 8BPP, palette) */
    uint8_t                 colorType;

    /* Display signals timing control module initialization */
    void                    (*TCON_Init)(void);

    /* current Color set for the display driver */
    GFX_COLOR               color;

    /*  current activepage set for the display driver */
    volatile uint8_t        activePage;

    /* current visualPage set for the display driver */
    volatile uint8_t        visualPage;

} DRV_GFX_INIT;

// *****************************************************************************
/*
  GFX PRIMITIVE HAL Initialize Data

  Summary:
    Defines the GFX PRIMITIVE HAL initialization data.

  Description:
    This data type defines data required to initialize or reinitialize the GFX
    HAL.

  Remarks:
    Not all initialization features are available on all devices.
*/

typedef struct
{
    /* System module initialization */
//    SYS_MODULE_INIT                 		moduleInit;

} GFX_PRIMITIVE_INIT;

// *****************************************************************************
/*
  GFX GOL GUI Initialize Data

  Summary:
    Defines the GFX GOL GUI layer initialization data.

  Description:
    This data type defines data required to initialize or reinitialize the GFX
    GOL GUI layer.

  Remarks:
    Not all initialization features are available on all devices.
*/

typedef struct
{
    /* System module initialization */
//    SYS_MODULE_INIT                 		moduleInit;

} GFX_GOL_INIT;

// *****************************************************************************
/*
  GFX Module Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the GFX Module

  Description:
    This data type defines the data required to initialize or reinitialize the
    GFX Module.

  Remarks:
    None.
*/

typedef struct _GFX_INIT
{
    /* System module initialization */
    SYS_MODULE_INIT                         moduleInit;

    SYS_MODULE_OBJ                          (*drvInitialize) (const SYS_MODULE_INDEX   index,
                                                const SYS_MODULE_INIT    * const init);

    GFX_HANDLE                              (*drvOpen) ( const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent );

    void                                    (*drvInterfaceSet)(GFX_HANDLE handle, DRV_GFX_INTERFACE * );

    DRV_GFX_INIT                            driverInitData;

    GFX_PRIMITIVE_INIT                      primitiveInitData;

    GFX_GOL_INIT                            golInitData;

    GFX_PREEMPTION_LEVEL                    preemptionLevel;

} GFX_INIT;

#ifdef __cplusplus
    }
#endif
    
#endif //_GFX_COMMON_H

/*******************************************************************************
 End of File
*/
