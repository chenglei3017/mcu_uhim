/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_s1d13517.c

  Summary:
    Interface for the graphics library where the primitives are rendered and sent 
	to the graphics controller either external or internal

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the S1D13517
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
additional informationS1D13517_REGarding your rights and obligations.

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

#ifndef _DRV_GFX_S1D13517_H
    #define _DRV_GFX_S1D13517_H
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
    Number of valid S1D13517 driver indices.

  Description:
    This constant identifies S1D13517 driver index definitions.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is device-specific.
*/

#define DRV_GFX_S1D13517_INDEX_COUNT     DRV_GFX_S1D13517_NUMBER_OF_MODULES


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*

  Function: uint16_t DRV_GFX_S1D13517_Initialize(uint8_t instance)

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
SYS_MODULE_OBJ DRV_GFX_S1D13517_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                                          const SYS_MODULE_INIT    * const moduleInit);

/*********************************************************************

 Function:
     DRV_GFX_S1D13517_Open(uint8_t instance)
    
  Summary:
    opens an instance of the graphics controller
  Description:
    none
  Return:
    1 - driver not initialized 2 - instance doesn't exist 3 - instance
    already open instance to driver when successful                   
  *********************************************************************/
DRV_HANDLE DRV_GFX_S1D13517_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent );

// *****************************************************************************
/*

  Function: DRV_GFX_S1D13517_Close(uint8_t instance)

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
void DRV_GFX_S1D13517_Close( DRV_HANDLE handle );

/*********************************************************************
  Function:
     void DRV_GFX_S1D13517_InterfaceSet( DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_S1D13517_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface );

// *****************************************************************************
/*
  Function:
     void DRV_GFX_S1D13517_MaxXGet()

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
uint16_t DRV_GFX_S1D13517_MaxXGet(void);

// *****************************************************************************
/*
  Function:
     void DRV_GFX_S1D13517_MaxYGet()

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
uint16_t DRV_GFX_S1D13517_MaxYGet(void);

// *****************************************************************************
/*

  Function: void DRV_GFX_S1D13517_SetColor(GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:

  Output: none

*/

void DRV_GFX_S1D13517_SetColor(GFX_COLOR color);

// *****************************************************************************
/*

  Function: void DRV_GFX_S1D13517_SetInstance(uint8_t instance)

  Summary: Sets the instance for the driver

  Description:

  Output: none

*/

void DRV_GFX_S1D13517_SetInstance(uint8_t instance);

// *****************************************************************************
/*

  Function: uint8_t DRV_GFX_S1D13517_SetReg(uint8_t index, uint8_t value)

  Summary:
    updates graphics controllerS1D13517_REGister value (byte access)

  Description:
    none

  Input:
    index -S1D13517_REGister number
    value - value to write toS1D13517_REGister

  Output:
    1 - call was not passed
    0 - call was passed
*/
uint16_t DRV_GFX_S1D13517_SetReg(uint8_t index, uint8_t value);

// *****************************************************************************
/*

  Function: uint8_t DRV_GFX_S1D13517_GetReg(uint8_t index)

  Summary:
    returns graphics controllerS1D13517_REGister value (byte access)

  Description:
    none

  Input:
    index -S1D13517_REGister number

  Output:
    0 - when call was passed
*/
uint8_t  DRV_GFX_S1D13517_GetReg(uint8_t  index);

// *****************************************************************************
/*

  Function: void DRV_GFX_S1D13517_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        left, top - pixel coordinates
        right, bottom - pixel coordinates

  Output:
          NULL - call not successful (PMP driver busy)
         !NULL - address of the display driver queue command
*/
void DRV_GFX_S1D13517_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);

// *****************************************************************************
/*

  Function: uint16_t DRV_GFX_S1D13517_PixelPut(uint16_t x, uint16_t y)

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
void DRV_GFX_S1D13517_PixelPut(uint16_t x, uint16_t y);

// *****************************************************************************
/*

  Function: void  DRV_GFX_S1D13517_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

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
void DRV_GFX_S1D13517_PixelArrayPut(GFX_COLOR *color,uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount);

// *****************************************************************************
/*

  Function: uint8_t DRV_GFX_S1D13517_BrightnessSet(uint16_t level)

  Summary:
    Sets the brightness of the display backlight.

  Description:
    none

  Input:
        level - Brightness level. Valid values are 0 to 100.
			             -   0: brightness level is zero or display is turned off
			             - 100: brightness level is maximum 
  Output:
    none
*/
void DRV_GFX_S1D13517_BrightnessSet(uint16_t  level);

// *****************************************************************************
/*

  Function:
    void DRV_GFX_S1D13517_SetPage(uint8_t pageType, uint8_t page)

  Summary:
    Sets the page of a certain page type

  Description:
    none

  Input:
        instance - driver instance
  Output:
     NULL - call not successful (PMP driver busy)
    !NULL - address of the display driver queue command
*/
uint16_t      DRV_GFX_S1D13517_SetPage(uint8_t pageType, uint8_t page);

// *****************************************************************************
/*

  Function:
    uint16_t* DRV_GFX_S1D13517_Layer(uint8_t type, GFX_LAYER_PARAMS* layer)

  Summary:
    Updates a Layer depending on the layer parameters.

  Description:
    none

  Input:
        type - layer type
        layer - parameters for Layer function call
  Output:
     NULL - call not successful (PMP driver busy)
    !NULL - address of the display driver queue command
*/
uint16_t*     DRV_GFX_S1D13517_Layer(uint8_t type, GFX_LAYER_PARAMS* layer);

// *****************************************************************************
/*

  Function:
	void DRV_GFX_S1D13517_AlphaBlendWindow(uint8_t pageType, uint8_t page)

  Summary:
    SEE primitive layer alphablendWindow definition

*/
uint16_t*      DRV_GFX_S1D13517_AlphaBlendWindow(GFX_ALPHA_PARAMS* alphaParams, uint16_t width, uint16_t height, uint8_t alpha);

/*************************************************************************
  Function:
      void DRV_GFX_SSD1926_Tasks(void)

  Summary:
    Task machine that renders the driver calls for the graphics library it
    must be called periodically to output the contents of its circular
    buffer
  *************************************************************************/
void DRV_GFX_S1D13517_Tasks(SYS_MODULE_OBJ object);

//DOM-IGNORE-BEGIN
/*********************************************************************
 * Overview: S1D13517S1D13517_REGisters definitions.
 *********************************************************************/
#define S1D13517_REG00_PROD_CODE        0x00     // Product CodeS1D13517_REGister [READONLY]
#define S1D13517_REG02_READBACK         0x02     // Configuration ReadbackS1D13517_REGister [READONLY]
#define S1D13517_REG04_PLL_DDIVIDER     0x04     // PLL D-DividerS1D13517_REGister
#define S1D13517_REG06_PLL_0            0x06     // PLL SettingS1D13517_REGister 0
#define S1D13517_REG08_PLL_1            0x08     // PLL SettingS1D13517_REGister 1
#define S1D13517_REG0A_PLL_2            0x0A     // PLL SettingS1D13517_REGister 2
#define S1D13517_REG0C_PLL_NDIVIDER     0x0C     // PLL N-DividerS1D13517_REGister
#define S1D13517_REG0E_SS_CONTROL_0     0x0E     // SS ControlS1D13517_REGister 0
#define S1D13517_REG10_SS_CONTROL_1     0x10     // SS ControlS1D13517_REGister 1
#define S1D13517_REG12_CLK_SRC_SELECT   0x12     // Clock Source SelectS1D13517_REGister
#define S1D13517_REG14_LCD_PANEL_TYPE   0x14     // LCD Panel TypeS1D13517_REGister
#define S1D13517_REG16_HDISP_WIDTH      0x16     // Horizontal Display WidthS1D13517_REGister
#define S1D13517_REG18_HNDP_PERIOD      0x18     // Horizontal Non-Display PeriodS1D13517_REGister
#define S1D13517_REG1A_VDISP_HEIGHT_0   0x1A     // Vertical Display HeightS1D13517_REGister 0
#define S1D13517_REG1C_VDISP_HEIGHT_1   0x1C     // Vertical Display HeightS1D13517_REGister 1
#define S1D13517_REG1E_VNDP_PERIOD      0x1E     // Vertical Non-Display PeriodS1D13517_REGister
#define S1D13517_REG20_PHS_PULSE_WIDTH  0x20     // PHS Pulse Width (HSW)S1D13517_REGister
#define S1D13517_REG22_PHS_PULSE_START  0x22     // PHS Pulse Start Position (HPS)S1D13517_REGister
#define S1D13517_REG24_PVS_PULSE_WIDTH  0x24     // PVS Pulse Width (VSW)S1D13517_REGister
#define S1D13517_REG26_PVS_PULSE_START  0x26     // PVS Pulse Start Position (VPS)S1D13517_REGister
#define S1D13517_REG28_PCLK_POLARITY    0x28     // PCLK PolarityS1D13517_REGister
#define S1D13517_REG2A_DSP_MODE         0x2A     // Display ModeS1D13517_REGister
#define S1D13517_REG2C_PIP1_DSP_SA_0    0x2C     // PIP1 Display Start AddressS1D13517_REGister 0
#define S1D13517_REG2E_PIP1_DSP_SA_1    0x2E     // PIP1 Display Start AddressS1D13517_REGister 1
#define S1D13517_REG30_PIP1_DSP_SA_2    0x30     // PIP1 Display Start AddressS1D13517_REGister 2
#define S1D13517_REG32_PIP1_WIN_X_SP    0x32     // PIP1 Window X Start PositionS1D13517_REGister
#define S1D13517_REG34_PIP1_WIN_Y_SP_0  0x34     // PIP1 Window Y Start PositionS1D13517_REGister 0
#define S1D13517_REG36_PIP1_WIN_Y_SP_1  0x36     // PIP1 Window Y Start PositionS1D13517_REGister 1
#define S1D13517_REG38_PIP1_WIN_X_EP    0x38     // PIP1 Window X End PositionS1D13517_REGister
#define S1D13517_REG3A_PIP1_WIN_Y_EP_0  0x3A     // PIP1 Window Y End PositionS1D13517_REGister 0
#define S1D13517_REG3C_PIP1_WIN_Y_EP_1  0x3C     // PIP1 Window Y End PositionS1D13517_REGister 1
#define S1D13517_REG3E_PIP2_DSP_SA_0    0x3E     // PIP2 Display Start AddressS1D13517_REGister 0
#define S1D13517_REG40_PIP2_DSP_SA_1    0x40     // PIP2 Display Start AddressS1D13517_REGister 1
#define S1D13517_REG42_PIP2_DSP_SA_2    0x42     // PIP2 Display Start AddressS1D13517_REGister 2
#define S1D13517_REG44_PIP2_WIN_X_SP    0x44     // PIP2 Window X Start PositionS1D13517_REGister
#define S1D13517_REG46_PIP2_WIN_Y_SP_0  0x46     // PIP2 Window Y Start PositionS1D13517_REGister 0
#define S1D13517_REG48_PIP2_WIN_Y_SP_1  0x48     // PIP2 Window Y Start PositionS1D13517_REGister 1
#define S1D13517_REG4A_PIP2_WIN_X_EP    0x4A     // PIP2 Window X End PositionS1D13517_REGister
#define S1D13517_REG4C_PIP2_WIN_Y_EP_0  0x4C     // PIP2 Window Y End PositionS1D13517_REGister 0
#define S1D13517_REG4E_PIP2_WIN_Y_EP_1  0x4E     // PIP2 Window Y End PositionS1D13517_REGister 1
#define S1D13517_REG50_DISPLAY_CONTROL  0x50     // Display ControlS1D13517_REGister [WRITEONLY]
#define S1D13517_REG52_INPUT_MODE       0x52     // Input ModeS1D13517_REGister
#define S1D13517_REG54_TRANSP_KEY_RED   0x54     // Transparency Key Color RedS1D13517_REGister
#define S1D13517_REG56_TRANSP_KEY_GREEN 0x56     // Transparency Key Color GreenS1D13517_REGister
#define S1D13517_REG58_TRANSP_KEY_BLUE  0x58     // Transparency Key Color BlueS1D13517_REGister
#define S1D13517_REG5A_WRITE_WIN_X_SP   0x5A     // Write Window X Start PositionS1D13517_REGister
#define S1D13517_REG5C_WRITE_WIN_Y_SP_0 0x5C     // Write Window Start PositionS1D13517_REGister 0
#define S1D13517_REG5E_WRITE_WIN_Y_SP_1 0x5E     // Write Window Start PositionS1D13517_REGister 1
#define S1D13517_REG60_WRITE_WIN_X_EP   0x60     // Write Window X End PositionS1D13517_REGister
#define S1D13517_REG62_WRITE_WIN_Y_EP_0 0x62     // Write Window Y End PositionS1D13517_REGister 0
#define S1D13517_REG64_WRITE_WIN_Y_EP_1 0x64     // Write Window Y End PositionS1D13517_REGister 1
#define S1D13517_REG66_MEM_DATA_PORT_0  0x66     // Memory Data PortS1D13517_REGister 0 [WRITEONLY]
#define S1D13517_REG67_MEM_DATA_PORT_1  0x67     // Memory Data PortS1D13517_REGister 1 [WRITEONLY]
#define S1D13517_REG68_POWER_SAVE       0x68     // Power SaveS1D13517_REGister
#define S1D13517_REG6A_N_DISP_PER_CTRS  0x6A     // Non-Display Period Control/StatusS1D13517_REGister
#define S1D13517_REG6C_GPO_0            0x6C     // General Purpose OutputS1D13517_REGister 0
#define S1D13517_REG6E_GPO_1            0x6E     // General Purpose OutputS1D13517_REGister 1
#define S1D13517_REG70_PWM_CONTROL      0x70     // PWM ControlS1D13517_REGister
#define S1D13517_REG72_PWM_HIGH_DC_0    0x72     // PWM High Duty CycleS1D13517_REGister 0
#define S1D13517_REG74_PWM_HIGH_DC_1    0x74     // PWM High Duty CycleS1D13517_REGister 1
#define S1D13517_REG76_PWM_HIGH_DC_2    0x76     // PWM High Duty CycleS1D13517_REGister 2
#define S1D13517_REG78_PWM_HIGH_DC_3    0x78     // PWM High Duty CycleS1D13517_REGister 3
#define S1D13517_REG7A_PWM_LOW_DC_0     0x7A     // PWM Low Duty CycleS1D13517_REGister 0
#define S1D13517_REG7C_PWM_LOW_DC_1     0x7C     // PWM Low Duty CycleS1D13517_REGister 1
#define S1D13517_REG7E_PWM_LOW_DC_2     0x7E     // PWM Low Duty CycleS1D13517_REGister 2
#define S1D13517_REG80_PWM_LOW_DC_3     0x80     // PWM Low Duty CycleS1D13517_REGister 3
#define S1D13517_REG82_SDRAM_CONTROL_0  0x82     // SDRAM ControlS1D13517_REGister 0
#define S1D13517_REG84_SDRAM_STATUS_0   0x84     // SDRAM StatusS1D13517_REGister 0 [WRITEONLY]
#define S1D13517_REG86_SDRAM_STATUS_1   0x86     // SDRAM StatusS1D13517_REGister 1 [READONLY]
#define S1D13517_REG88_SDRAM_MRS_VAL_0  0x88     // SDRAM MRS ValueS1D13517_REGister 0
#define S1D13517_REG8A_SDRAM_MRS_VAL_1  0x8A     // SDRAM MRS ValueS1D13517_REGister 1
#define S1D13517_REG8C_SDRAM_RFS_CNT_0  0x8C     // SDRAM Refresh CounterS1D13517_REGister 0
#define S1D13517_REG8E_SDRAM_RFS_CNT_1  0x8E     // SDRAM Refresh CounterS1D13517_REGister 1
#define S1D13517_REG90_SDRAM_BUF_SIZE   0x90     // SDRAM Write Buffer Memory SizeS1D13517_REGister
#define S1D13517_REG92_SDRAM_DEBUG      0x92     // SDRAM DebugS1D13517_REGister
#define S1D13517_REG94_ALP_CONTROL      0x94     // Alpha-Blend ControlS1D13517_REGister [WRITEONLY]
#define S1D13517_REG96_ALP_STATUS       0x96     // Alpha-Blend StatusS1D13517_REGister [READONLY]
#define S1D13517_REG98_ALP_HR_SIZE      0x98     // Alpha-Blend Horizontal SizeS1D13517_REGister
#define S1D13517_REG9A_ALP_VR_SIZE_0    0x9A     // Alpha-Blend Vertical SizeS1D13517_REGister 0
#define S1D13517_REG9C_ALP_VR_SIZE_1    0x9C     // Alpha-Blend Vertical SizeS1D13517_REGister 1
#define S1D13517_REG9E_ALP_VALUE        0x9E     // Alpha-Blend ValueS1D13517_REGister
#define S1D13517_REGA0_ALP_IN_IMG1_SA_0 0xA0     // Alpha-Blend Input Image 1 Start AddressS1D13517_REGister 0
#define S1D13517_REGA2_ALP_IN_IMG1_SA_1 0xA2     // Alpha-Blend Input Image 1 Start AddressS1D13517_REGister 1
#define S1D13517_REGA4_ALP_IN_IMG1_SA_2 0xA4     // Alpha-Blend Input Image 1 Start AddressS1D13517_REGister 2
#define S1D13517_REGA6_ALP_IN_IMG2_SA_0 0xA6     // Alpha-Blend Input Image 2 Start AddressS1D13517_REGister 0
#define S1D13517_REGA8_ALP_IN_IMG2_SA_1 0xA8     // Alpha-Blend Input Image 2 Start AddressS1D13517_REGister 1
#define S1D13517_REGAA_ALP_IN_IMG2_SA_2 0xAA     // Alpha-Blend Input Image 2 Start AddressS1D13517_REGister 2
#define S1D13517_REGAC_ALP_OUT_IMG_SA_0 0xAC     // Alpha-Blend Output Image Start AddressS1D13517_REGister 0
#define S1D13517_REGAE_ALP_OUT_IMG_SA_1 0xAE     // Alpha-Blend Output Image Start AddressS1D13517_REGister 1
#define S1D13517_REGB0_ALP_OUT_IMG_SA_2 0xB0     // Alpha-Blend Output Image Start AddressS1D13517_REGister 2
#define S1D13517_REGB2_INTERRUPT_CTRL   0xB2     // Interrupt ControlS1D13517_REGister
#define S1D13517_REGB4_INTERRUPT_STAT   0xB4     // Interrupt StatusS1D13517_REGister [READONLY]
#define S1D13517_REGB6_INTERRUPT_CLEAR  0xB6     // Interrupt ClearS1D13517_REGister [WRITEONLY]
#define S1D13517_REGFLAG_BASE           0xF0     // Special reserved flags beyond this point
#define S1D13517_REGFLAG_DELAY          0xFC     // PLLS1D13517_REGister Programming Delay (in us)
#define S1D13517_REGFLAG_OFF_DELAY      0xFD     // LCD Panel Power Off Delay (in ms)
#define S1D13517_REGFLAG_ON_DELAY       0xFE     // LCD Panel Power On Delay (in ms)
#define S1D13517_REGFLAG_END_OF_TABLE   0xFF     // End ofS1D13517_REGisters Marker
//DOM-IGNORE-END
/*********************************************************************
* Overview: This structure is used to describe layerS1D13517_REGisters.
*
*********************************************************************/
typedef struct
{
    uint8_t XStart;
    uint8_t XEnd;
    uint8_t YStart0;
    uint8_t YStart1;
    uint8_t YEnd0;
    uint8_t YEnd1;
    uint8_t StartAddress0; 
    uint8_t StartAddress1;
    uint8_t StartAddress2;
} LAYER_REGISTERS;

#ifdef __cplusplus
    }
#endif
    
#endif //_DRV_GFX_S1D13517_H
