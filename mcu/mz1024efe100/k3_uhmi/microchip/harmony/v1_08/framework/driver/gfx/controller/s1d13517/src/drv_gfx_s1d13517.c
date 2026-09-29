/*******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    drv_gfx_s1d13517.c

  Summary:
    Interface for the graphics library where the primitives are renderred and sent to the graphics controller
    either external or internal

  Description:
    None
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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
#include "../drv_gfx_s1d13517.h"
#include "framework/driver/pmp/drv_pmp_static.h"
#include "driver/gfx/gfx_common.h"

DRV_GFX_INTERFACE s1dInterface;
GFX_COLOR  color;                 /*global color for the driver*/
uint8_t    instance = 0;          /*global instance for the driver*/
uint16_t   queueCount = 0;
//volatile uint16_t driverBusy = 0;
//uint8_t    instances[GFX_CONFIG_DRIVER_COUNT] = {0};
uint8_t    dataWidth = 1; /*PMP Data Width*/
uint8_t    displayReg = 0x01;

#ifdef USE_16BIT_PMP
#if (GFX_CONFIG_COLOR_DEPTH <= 16)
inline void WritePixel(GFX_COLOR color)
{
    DRV_PMP0_Write(color);
}
#elif (GFX_CONFIG_COLOR_DEPTH == 24)
inline void WritePixel(GFX_COLOR color)
{
    DRV_PMP0_Write(GFX_ComponentRedGet(color));
    DRV_PMP0_Write((GFX_ComponentGreenGet(color)) << 8 | GFX_ComponentBlueGet(color));
}
#endif

#else

#if (GFX_CONFIG_COLOR_DEPTH <= 8)
inline void WritePixel(GFX_COLOR color)
{
    DRV_PMP0_Write((uint8_t)color);
}
#elif (GFX_CONFIG_COLOR_DEPTH == 16)
inline void WritePixel(GFX_COLOR color)
{
    DRV_PMP0_Write((uint8_t)(color >> 8));
    DRV_PMP0_Write((uint8_t)color);
}
#elif (GFX_CONFIG_COLOR_DEPTH == 24)
inline void WritePixel(GFX_COLOR color)
{
    DRV_PMP0_Write(GFX_ComponentRedGet(color));
    DRV_PMP0_Write(GFX_ComponentGreenGet(color));
    DRV_PMP0_Write(GFX_ComponentBlueGet(color));
}
#endif
#endif

inline void WindowSetup(uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount);
inline void _PutArray(GFX_COLOR* array, uint16_t count, uint16_t lineCount);
inline void _PutPixels(GFX_COLOR color, uint16_t count, uint16_t lineCount);
inline void _CopyPixels(GFX_COLOR color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount);

//GFX_COLOR transparentColor;
//transparentColor = GFX_RGBConvert(0xFF, 0x00, 0xCC); /*Transparent Color chosen for the application*/
//////uint32_t transparentColor = transparentColor;

uint16_t* BarAlpha(GFX_ALPHA_PARAMS* alphaParams, uint16_t height);

#define GetSystemClock()    (40000000ULL)
 static void TMR_DelayMS( uint32_t delay_in_ms )
 {
     uint32_t tWait = ( GetSystemClock() / 2000 ) * delay_in_ms;
     uint32_t tStart = _CP0_GET_COUNT();
     while( ( _CP0_GET_COUNT() - tStart ) < tWait );
 }

    const LAYER_REGISTERS PIPRegister[2] = 
    {
        { 
        S1D13517_REG32_PIP1_WIN_X_SP,S1D13517_REG38_PIP1_WIN_X_EP,S1D13517_REG34_PIP1_WIN_Y_SP_0,
        S1D13517_REG36_PIP1_WIN_Y_SP_1,S1D13517_REG3A_PIP1_WIN_Y_EP_0,S1D13517_REG3C_PIP1_WIN_Y_EP_1,
        S1D13517_REG2C_PIP1_DSP_SA_0,S1D13517_REG2E_PIP1_DSP_SA_1,S1D13517_REG30_PIP1_DSP_SA_2
        },
        { 
        S1D13517_REG44_PIP2_WIN_X_SP,S1D13517_REG4A_PIP2_WIN_X_EP,S1D13517_REG46_PIP2_WIN_Y_SP_0,
        S1D13517_REG48_PIP2_WIN_Y_SP_1,S1D13517_REG4C_PIP2_WIN_Y_EP_0,S1D13517_REG4E_PIP2_WIN_Y_EP_1,
        S1D13517_REG3E_PIP2_DSP_SA_0,S1D13517_REG40_PIP2_DSP_SA_1,S1D13517_REG42_PIP2_DSP_SA_2
        }
    };

    #define BUFFER_SIZE 0x140000l  /*WVGA used for ALL LCDs*/

    // *****************************************************************************
/* S1D13517 Driver task states

  Summary
    Lists the different states that S1D13517 task routine can have.

  Description
    This enumeration lists the different states that S1D13517 task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    /* Process queue */
    DRV_GFX_S1D13517_INITIALIZE_START,

    DRV_GFX_S1D13517_INITIALIZE_PWR_UP,

    DRV_GFX_S1D13517_INITIALIZE_SET_REGISTERS,

    /* GFX S1D13517 task initialization done */
    DRV_GFX_S1D13517_INITIALIZE_DONE,

} DRV_GFX_S1D13517_OBJECT_TASK;

// *****************************************************************************
/* GFX S1D13517 Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_GFX_S1D13517_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* S1D13517 machine state */
    DRV_GFX_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    DRV_GFX_CLIENT_OBJ *                        pDrvS1D13517ClientObj;

    /* State of the task */
    DRV_GFX_S1D13517_OBJECT_TASK    		task;

    DRV_GFX_INIT *                              initData;

    uint16_t      maxY;
    uint16_t      maxX;

} DRV_GFX_S1D13517_OBJ;

static DRV_GFX_S1D13517_OBJ        drvS1D13517Obj;

static DRV_GFX_CLIENT_OBJ          drvS1D13517Clients;

//Local Protoypes
void InitializeHardware();


// *****************************************************************************
/*
  Function: DRV_GFX_HANDLE DRV_GFX_S1D13517_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )

  Summary:
    opens an instance of the graphics controller

  Description:
    none

  Input:
    instance of the driver

  Output:
    1 - driver not initialied
    2 - instance doesn't exist
    3 - instance already open
    instance to driver when successful
*/
DRV_GFX_HANDLE DRV_GFX_S1D13517_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
   DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)DRV_HANDLE_INVALID;

    /* Check if instance object is ready*/
    if(drvS1D13517Obj.status != SYS_STATUS_READY)
    {
        /* The SSD1926 module should be ready */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: Was the driver initialized?");
    }
    else if(intent != DRV_IO_INTENT_EXCLUSIVE)
    {
        /* The driver only supports this mode */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: IO intent mode not supported");
    }
    else if(drvS1D13517Obj.nClients > 0)
    {
        /* Driver supports exclusive open only */
//        SYS_DEBUG(0, "GFX_SSD1926 already opened once. Cannot open again");
    }
    else
    {
        client = &drvS1D13517Clients;

        client->inUse = true;
        client->drvObj = &drvS1D13517Obj;

        /* Increment the client number for the specific driver instance*/
        drvS1D13517Obj.nClients++;
    }

    /* Return invalid handle */
    return ((DRV_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_S1D13517_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the graphics controller

  Description:
    This is closes the instance of the driver specified by handle.
*/
void DRV_GFX_S1D13517_Close( DRV_HANDLE handle )
{
    /* Start of local variable */
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)NULL;
    DRV_GFX_S1D13517_OBJ * drvObj = ( DRV_GFX_S1D13517_OBJ *)NULL;
    /* End of local variable */

    /* Check if the handle is valid */
    if(handle == DRV_HANDLE_INVALID)
    {
//        SYS_DEBUG(0, "Bad Client Handle");
    }
    else
    {
        client = (DRV_GFX_CLIENT_OBJ *)handle;

        if(client->inUse)
        {
            client->inUse = false;
            drvObj = ( DRV_GFX_S1D13517_OBJ *)client->drvObj;

            /* Remove this client from the driver client table */
            drvObj->nClients--;
        }
        else
        {
//            SYS_DEBUG(0, "Client Handle no inuse");
        }
    }
    return;
}


// *****************************************************************************
/*
  Function:
     void DRV_GFX_S1D13517_MaxXGet(void)

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
uint16_t DRV_GFX_S1D13517_MaxXGet(void)
{
    return drvS1D13517Obj.maxX;
}

// *****************************************************************************
/*
  Function:
     void DRV_GFX_S1D13517_MaxYGet(void)

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
uint16_t DRV_GFX_S1D13517_MaxYGet(void)
{
    return drvS1D13517Obj.maxY;
}

/*********************************************************************
  Function:
     void DRV_GFX_S1D13517_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_S1D13517_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )
{
    interface->BarFill = DRV_GFX_S1D13517_BarFill;
    interface->PixelArrayPut = DRV_GFX_S1D13517_PixelArrayPut;
    interface->PixelPut = DRV_GFX_S1D13517_PixelPut;
    interface->ColorSet = DRV_GFX_S1D13517_SetColor;
    interface->PageSet = DRV_GFX_S1D13517_SetPage;
    interface->Layer = DRV_GFX_S1D13517_Layer;
    interface->AlphaBlendWindow = DRV_GFX_S1D13517_AlphaBlendWindow;
    interface->MaxXGet = DRV_GFX_S1D13517_MaxXGet;
    interface->MaxYGet = DRV_GFX_S1D13517_MaxYGet;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_S1D13517_SetColor(GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:
  
  Output: none

*/

void DRV_GFX_S1D13517_SetColor(GFX_COLOR color)
{
     drvS1D13517Obj.initData->color = color;
}

// *****************************************************************************
/*
  Function: uint8_t DRV_GFX_S1D13517_SetReg(uint8_t index, uint8_t value)

  Summary:
    updates graphics controller register value (byte access)

  Description:
    none

  Input:
    index - register number 
    value - value to write to register

  Output:
    1 - call was not passed
    0 - call was passed
*/
uint16_t DRV_GFX_S1D13517_SetReg(uint8_t index, uint8_t value)
{
    DisplaySetCommand(); // set RS line to low for command

    DRV_PMP0_Write(index);

    DisplaySetData();    // set RS line to high for data

    DRV_PMP0_Write(value);

    return(0);

}

// *****************************************************************************
/*
  Function: uint8_t DRV_GFX_S1D13517_GetReg(uint8_t index, uint8_t *data)

  Summary:
    returns graphics controller register value (byte access)

  Description:
    none

  Input:
    index - register number 
    *data - array to store data

  Output:
    0 - when call was passed
*/
uint8_t  DRV_GFX_S1D13517_GetReg(uint8_t  index)
{
     uint16_t myReadBuffer = 0xff;

     DisplaySetCommand(); // set RS line to low for command

     DRV_PMP0_Write(index);

     DisplaySetData();    // set RS line to high for data

     DRV_PMP0_Read(); //Perform the fake read

     PMCONbits.PMPEN = 0; // disable PMP
     myReadBuffer = DRV_PMP0_Read();
     PMCONbits.PMPEN = 1; // disable PMP

    return((uint8_t)myReadBuffer);
}

// *****************************************************************************
/*
  Function: uint8_t DRV_GFX_S1D13517_BrightnessSet(const SYS_MODULE_INDEX index, uint8_t instance, uint16_t level)

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
void DRV_GFX_S1D13517_BrightnessSet(uint16_t  level)
{
   if(level == 100)
   {
       while(DRV_GFX_S1D13517_SetReg(S1D13517_REG70_PWM_CONTROL,0x85));          //Turn on Backlight
   }
   else if (level == 0)
   {    
       DRV_GFX_S1D13517_SetReg(S1D13517_REG70_PWM_CONTROL,0x84);       //Turn off Backlight
   }
   else if (level <= 50)
   {
       level >>= 1;
       level *=  5;  /*Sets the value from (0-250)*/

       DRV_GFX_S1D13517_SetReg(S1D13517_REG72_PWM_HIGH_DC_0,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG74_PWM_HIGH_DC_1,level);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG7A_PWM_LOW_DC_0,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG7C_PWM_LOW_DC_1,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG70_PWM_CONTROL,0x86);   //Turn off Backlight PWM
   }
   else
   {
       level >>= 1;
       level *=  5;  /*Sets the value from (0-250)*/

       DRV_GFX_S1D13517_SetReg(S1D13517_REG72_PWM_HIGH_DC_0,level);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG74_PWM_HIGH_DC_1,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG7A_PWM_LOW_DC_0,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG7C_PWM_LOW_DC_1,0xff);
       DRV_GFX_S1D13517_SetReg(S1D13517_REG70_PWM_CONTROL,0x86);   //Turn off Backlight PWM

    }
}    

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
    1 - call not successful (PMP driver busy)
    0 - call successful
*/
SYS_MODULE_OBJ DRV_GFX_S1D13517_Initialize(const SYS_MODULE_INDEX   index,
                                           const SYS_MODULE_INIT    * const init)
{

    static uint8_t   state =0;
    static uint16_t  horizontalSize, verticalSize;

    /* Validate the driver index */
    if ( index >= GFX_CONFIG_NUMBER_OF_MODULES )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    DRV_GFX_S1D13517_OBJ *dObj = &drvS1D13517Obj;

    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->state = SYS_STATUS_BUSY;
    dObj->initData = (DRV_GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = index;

    uint16_t horizontalTiming = drvS1D13517Obj.initData->horizontalPulseWidth+drvS1D13517Obj.initData->horizontalFrontPorch+drvS1D13517Obj.initData->horizontalBackPorch;
    uint16_t verticalTiming = drvS1D13517Obj.initData->verticalPulseWidth+drvS1D13517Obj.initData->verticalFrontPorch+drvS1D13517Obj.initData->verticalBackPorch;

    dObj->task = DRV_GFX_S1D13517_INITIALIZE_SET_REGISTERS;

    DisplayResetEnable();               // hold in reset by default

    DisplayResetConfig();               // enable RESET line

    DisplayCmdDataConfig();

    DisplayConfig();                    // enable chip select line

    DisplayBacklightConfig();

    while(state <= 40)
    {
        switch(state)
        {
            case 0:
#ifdef USE_16BIT_PMP
                dataWidth = PMP_DATA_SIZE_16_BITS;
#else
                dataWidth = PMP_DATA_SIZE_8_BITS;
#endif

                DisplayResetDisable();              // release from reset
                DisplayEnable();

                if((drvS1D13517Obj.initData->orientation == 90) || (drvS1D13517Obj.initData->orientation == 270))
                {
                    horizontalSize = drvS1D13517Obj.initData->verticalResolution;
                    verticalSize   = drvS1D13517Obj.initData->horizontalResolution;
                    drvS1D13517Obj.maxX = verticalSize - 1;
                    drvS1D13517Obj.maxY = horizontalSize - 1;
                }
                else
                {
                    horizontalSize = drvS1D13517Obj.initData->horizontalResolution;
                    verticalSize   = drvS1D13517Obj.initData->verticalResolution;
                    drvS1D13517Obj.maxX = horizontalSize - 1;
                    drvS1D13517Obj.maxY = verticalSize - 1;
                }

                state++;
                break;

            case 11:
                TMR_DelayMS(100);
                break;

        }

        /*S1D13517 Registers to be initialized*/
        uint8_t registers[40][2] = {
                                { S1D13517_REG2A_DSP_MODE,0x00 },
                                { S1D13517_REG68_POWER_SAVE,0x00 },
                                { S1D13517_REG04_PLL_DDIVIDER,0x17 },
                                { S1D13517_REG06_PLL_0,0x29 },
                                { S1D13517_REG08_PLL_1,0x01 },
                                { S1D13517_REG0A_PLL_2,0x08 },
                                { S1D13517_REG0C_PLL_NDIVIDER,0x59 },
#if (DISP_HOR_RESOLUTION < 481)
                                { S1D13517_REG12_CLK_SRC_SELECT,0x02 },
#else
                                { S1D13517_REG12_CLK_SRC_SELECT,0x92 },
#endif
                                { S1D13517_REG04_PLL_DDIVIDER,0x97 },
                                { S1D13517_REG0E_SS_CONTROL_0,0x8F },
#if (GFX_CONFIG_COLOR_DEPTH == 24)
                                { S1D13517_REG14_LCD_PANEL_TYPE,0x02 },
#else
                                { S1D13517_REG14_LCD_PANEL_TYPE,0x05 },
#endif
                                { S1D13517_REG16_HDISP_WIDTH,(drvS1D13517Obj.initData->horizontalResolution>>3)-1 },
                                { S1D13517_REG18_HNDP_PERIOD,((horizontalTiming)>>1)-1 },
                                { S1D13517_REG1A_VDISP_HEIGHT_0,drvS1D13517Obj.initData->verticalResolution-1 },
                                { S1D13517_REG1C_VDISP_HEIGHT_1,(drvS1D13517Obj.initData->verticalResolution-1)>>8 },
                                { S1D13517_REG1E_VNDP_PERIOD,((verticalTiming)>>1)-1 },
                                { S1D13517_REG20_PHS_PULSE_WIDTH,drvS1D13517Obj.initData->horizontalPulseWidth -1 },
                                { S1D13517_REG22_PHS_PULSE_START,drvS1D13517Obj.initData->horizontalFrontPorch },
                                { S1D13517_REG24_PVS_PULSE_WIDTH,drvS1D13517Obj.initData->verticalPulseWidth -1 },
                                { S1D13517_REG26_PVS_PULSE_START,drvS1D13517Obj.initData->verticalFrontPorch },
                                { S1D13517_REG28_PCLK_POLARITY,(drvS1D13517Obj.initData->logicShift << 7) },
                                { S1D13517_REG82_SDRAM_CONTROL_0,0x03 },
                                { S1D13517_REG8C_SDRAM_RFS_CNT_0,0xFF },
                                { S1D13517_REG8E_SDRAM_RFS_CNT_1,0x03 },
                                { S1D13517_REG90_SDRAM_BUF_SIZE,0x50 },
                                { S1D13517_REG68_POWER_SAVE,0xE8 },
                                { S1D13517_REG68_POWER_SAVE,0x00 },
                                { S1D13517_REG68_POWER_SAVE,0x01 },
                                { S1D13517_REG84_SDRAM_STATUS_0,0x86 },
                                { S1D13517_REG52_INPUT_MODE,0x08 },
                                { S1D13517_REG54_TRANSP_KEY_RED, GFX_ComponentRedGet(GFX_RGBConvert(0xFF, 0x00, 0xCC)) },
                                { S1D13517_REG56_TRANSP_KEY_GREEN, GFX_ComponentGreenGet(GFX_RGBConvert(0xFF, 0x00, 0xCC)) },
                                { S1D13517_REG58_TRANSP_KEY_BLUE, GFX_ComponentBlueGet(GFX_RGBConvert(0xFF, 0x00, 0xCC)) },
                                { S1D13517_REG6E_GPO_1,0x07 },
                                { S1D13517_REG2A_DSP_MODE, displayReg },
                                { S1D13517_REG50_DISPLAY_CONTROL,0x80 },
                                { S1D13517_REGB2_INTERRUPT_CTRL,0x01 },
                                { S1D13517_REG9E_ALP_VALUE,0x80 },
                                { S1D13517_REG94_ALP_CONTROL,0x01 },
                                { S1D13517_REG94_ALP_CONTROL,0x00 }
                                };

        while(DRV_GFX_S1D13517_SetReg(*registers[state-1], registers[state-1][1]));

        state++;

    }

    if(drvS1D13517Obj.initData->TCON_Init != NULL)
    {
       drvS1D13517Obj.initData->TCON_Init();
    }

    TMR_DelayMS(500);

    DRV_GFX_S1D13517_BrightnessSet(100);
//    drvS1D13517Obj.initData->horizontalResolution = horizontalSize;
//    drvS1D13517Obj.initData->verticalResolution = verticalSize;
    drvS1D13517Obj.state = SYS_STATUS_READY;

    dObj->nClients = 0;
    dObj->status = SYS_STATUS_READY;

    DRV_GFX_S1D13517_SetPage(ACTIVE_PAGE, 0);
    DRV_GFX_S1D13517_SetPage(VISUAL_PAGE, 0);

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)dObj;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_S1D13517_PixelPut(uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        instance - driver instance
        color - color to output
        x,y - pixel coordinates
  Output:
    1 - call not successful (S1D13517 driver busy)
    0 - call successful
*/
void DRV_GFX_S1D13517_PixelPut(uint16_t x, uint16_t y)
{

    #if (DISP_ORIENTATION == 90)
    uint16_t tempx = x;
    x = (DISP_HOR_RESOLUTION-1) - y;
    y = tempx;
    #elif (DISP_ORIENTATION == 270)
    uint16_t tempy = y;
    y = (DISP_VER_RESOLUTION-1) - x;
    x = tempy;
    #elif (DISP_ORIENTATION == 180)
    x = (DISP_HOR_RESOLUTION-1) - x;
    y = (DISP_VER_RESOLUTION-1) - y;
    #endif

    WindowSetup(x, y, 1, 1);

    _PutPixels(drvS1D13517Obj.initData->color, 1, 1);
}  

// *****************************************************************************
/*
  Function: void DRV_GFX_S1D13517_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        left,top - pixel coordinates
        right, bottom - pixel coordinates

  Output:
          1 - call not successful (S1D13517 driver busy)
          0 - call successful
*/
void  DRV_GFX_S1D13517_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{

    uint16_t count = right - left + 1;
    uint16_t lineCount = bottom - top + 1;

    #if (DISP_ORIENTATION == 90)
    uint16_t tempx = left;
    uint16_t tempCount = count;
    left = (DISP_HOR_RESOLUTION-1) - top - (lineCount - 1);
    top = tempx;
    count = lineCount;
    lineCount = tempCount;
    #elif (DISP_ORIENTATION == 180)
    left = (DISP_HOR_RESOLUTION) - left - (count);
    top = (DISP_VER_RESOLUTION) - top - (lineCount);
    #elif (DISP_ORIENTATION == 270)
    uint16_t tempy = top;
    uint16_t tempCount = count;
    top = (DISP_VER_RESOLUTION - 1) - left - (count - 1);
    left = tempy;
    count = lineCount;
    lineCount = tempCount;
    #endif

    WindowSetup(left, top, count, lineCount);

    if(((count-(8-(left&7)))>7) && (lineCount > 1))
    {
        _CopyPixels(drvS1D13517Obj.initData->color, left, top, count, lineCount);
    }
    else
    {
        _PutPixels(drvS1D13517Obj.initData->color, count, lineCount);
    }

}  

// *****************************************************************************
/*
  Function: uint16_t*  DRV_GFX_S1D13517_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

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
          lineCount - number of lines
  Output:
         handle to the number of pixels remaining
*/
#if (DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 0)
void  DRV_GFX_S1D13517_PixelArrayPut(GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{

    #if (DISP_ORIENTATION == 180)
    x = (DISP_HOR_RESOLUTION) - x - (count);
    y = (DISP_VER_RESOLUTION) - y - (lineCount);
    color += lineCount * count;
    #endif

    WindowSetup(x, y, count, lineCount);

    _PutArray(color, count, lineCount);

}
#else
void  DRV_GFX_S1D13517_PixelArrayPut(GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{
    uint16_t tempx = x;
    uint16_t tempCount = count;

    while(lineCount--)
    {
        while(tempCount--)
        {
            drvS1D13517Obj.initData->color = *color++;
            while(DRV_GFX_S1D13517_PixelPut(x++, y) == 1);
        }

        x = tempx;
        tempCount = count;
        y++;
    }
    
}
#endif
// *****************************************************************************
/*
  Function:
	void DRV_GFX_S1D13517_SetPage(uint8_t pageType, uint8_t page)

  Summary:
	Task machine that renders the driver calls for the graphics library
    it must be called peridically to output the contents of its circular buffer
*/
//
uint16_t DRV_GFX_S1D13517_SetPage(uint8_t pageType, uint8_t page)
{

    switch(pageType)
    {
        case ACTIVE_PAGE:
            while(DRV_GFX_S1D13517_SetReg(S1D13517_REG52_INPUT_MODE, 0x08 | (page<<4)));
            drvS1D13517Obj.initData->activePage = page;
            break;

        case VISUAL_PAGE:

            displayReg &= 0x0f;

            while(DRV_GFX_S1D13517_SetReg(S1D13517_REG2A_DSP_MODE, ((displayReg) | (page << 4))));
            while(DRV_GFX_S1D13517_SetReg(S1D13517_REG50_DISPLAY_CONTROL,0x80));

            drvS1D13517Obj.initData->visualPage = page;

        default:
            break;
    }

    return(0);
}

// *****************************************************************************
/*
  Function:
	void _CopyPixels
*/
//
uint16_t beforeCount,afterCount;

inline void _CopyPixels(GFX_COLOR color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{
   static GFX_ALPHA_PARAMS alphaParams;
   static uint16_t _lineCount, finalWidth;
   static uint16_t width,start;
   uint8_t _count = 4;
   static uint32_t color1,color2;
   static uint8_t myWriteBuffer[4]; //Write buffer for the PMP
   uint8_t lineCopySize = 2;
   short temp;

   while(_count)
   {

       _lineCount = lineCount;

       if(_count == 4)
       {
           width = 8;
           start = x;
           color1 = GFX_RGBConvert(0xFF, 0x00, 0xCC);
           color2 = color;
       }
       else if(_count == 3)
       {
           width = count;

           if(beforeCount)
           {
               start += (8 - beforeCount);
               width -= (8 - beforeCount);
           }

           width &= 0xFFFFFFF8;
           finalWidth = width;
           _lineCount = 1;

           /*Setup the Alpha Window Parameters for later*/
           alphaParams.foregroundPage = drvS1D13517Obj.initData->activePage;
           alphaParams.foregroundLeft = start;
           alphaParams.foregroundTop = (y);
           alphaParams.destinationPage = alphaParams.foregroundPage;
           alphaParams.destinationLeft = start;
           alphaParams.destinationTop = (y + 1);
       }
       else if(_count == 2)
       {
          start = (x + count);
          width = 8;
          color2 = GFX_RGBConvert(0xFF, 0x00, 0xCC);
          color1 = color;
       }
       else if(_count == 1)
       {
           while(DRV_GFX_S1D13517_AlphaBlendWindow(&alphaParams, finalWidth, 1, 100) == NULL);
           _lineCount = 2;

           while((_lineCount) < (lineCount-1))
           {
               while(BarAlpha(&alphaParams, lineCopySize) == NULL);
               alphaParams.destinationTop = (y + _lineCount);

               if(_lineCount < ((lineCount-1) - (lineCopySize<<1)))
                   lineCopySize <<= 1;
               else
                   lineCopySize = 2;
               _lineCount += lineCopySize;
           }

           alphaParams.destinationTop = (y + lineCount - 2);
           while(BarAlpha(&alphaParams, 1) == NULL);
           return;
       }
          beforeCount = (start & 7);
          while(DRV_GFX_S1D13517_SetReg(S1D13517_REG5A_WRITE_WIN_X_SP,(start >> 2)));

          if(!beforeCount)
          {
              myWriteBuffer[2]  = (start + width - 1) >> 2;
          }
          else
          {
              myWriteBuffer[2]  = (start) >> 2;
          }

          while(DRV_GFX_S1D13517_SetReg(S1D13517_REG60_WRITE_WIN_X_EP, myWriteBuffer[2]));

          while(DRV_GFX_S1D13517_SetReg(S1D13517_REG64_WRITE_WIN_Y_EP_1,y + lineCount));

          while(_lineCount)
          {
               for(temp = 0; temp < beforeCount; temp++)
              {
                  WritePixel(color1);
              }

              for(temp = 0; temp < (width - beforeCount); temp++)
              {
                  WritePixel(color2);
              }
               
              _lineCount--;
           }
           _count--;
    }
 }

/*********************************************************************
* Function: uint43_t GFXGetPageOriginAddress(short pageNumber)
********************************************************************/
uint32_t GFXGetPageOriginAddress(short pageNumber)
{
	return(BUFFER_SIZE * (uint32_t)pageNumber);
}

/*********************************************************************
* Function: uint32_t GFXGetPageXYAddress(uint8_t pageNumber, uint16_t x, uint16_t y)
********************************************************************/
uint32_t GFXGetPageXYAddress(uint8_t pageNumber, uint16_t x, uint16_t y)
{
    uint32_t address;

    address = (uint32_t)(y * DISP_HOR_RESOLUTION);
    address += (uint32_t)x;
    address *= 3;
    address += GFXGetPageOriginAddress(pageNumber);

    return(address);
}

uint16_t* BarAlpha(GFX_ALPHA_PARAMS* alphaParams, uint16_t height)
{
    static uint8_t state = 0;
    static uint32_t destinationWindowAddr;

    destinationWindowAddr = GFXGetPageXYAddress(alphaParams->destinationPage, alphaParams->destinationLeft, alphaParams->destinationTop);

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG9A_ALP_VR_SIZE_0,height));
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGAC_ALP_OUT_IMG_SA_0,destinationWindowAddr));

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGAE_ALP_OUT_IMG_SA_1,destinationWindowAddr>>8));

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB0_ALP_OUT_IMG_SA_2,destinationWindowAddr>>16));

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG94_ALP_CONTROL,0x01));
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG94_ALP_CONTROL,0x00));

    while(DRV_GFX_S1D13517_GetReg((S1D13517_REGB4_INTERRUPT_STAT)) != 1);
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB6_INTERRUPT_CLEAR,0x01));
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB6_INTERRUPT_CLEAR,0x00));

    state = 0;
    return((uint16_t*)&state);
}

uint16_t* DRV_GFX_S1D13517_AlphaBlendWindow(GFX_ALPHA_PARAMS* alphaParams, uint16_t width, uint16_t height, uint8_t alpha)
{

    static uint32_t foregroundWindowAddr, backgroundWindowAddr, destinationWindowAddr;
    static uint8_t alphaReg;

    #if (DISP_ORIENTATION == 180)
    alphaParams->foregroundLeft =  (DISP_HOR_RESOLUTION-1) - alphaParams->foregroundLeft - (width - 1);
    alphaParams->destinationLeft = (DISP_HOR_RESOLUTION-1) - alphaParams->destinationLeft - (width - 1);
    alphaParams->backgroundLeft = (DISP_HOR_RESOLUTION-1) - alphaParams->backgroundLeft - (width - 1);
    alphaParams->foregroundTop = (DISP_VER_RESOLUTION-1) - alphaParams->foregroundTop - (height - 1);
    alphaParams->destinationTop = (DISP_VER_RESOLUTION-1) - alphaParams->destinationTop - (height - 1);
    alphaParams->backgroundTop = (DISP_VER_RESOLUTION-1) - alphaParams->backgroundTop - (height - 1);
    #endif
    
   foregroundWindowAddr = GFXGetPageXYAddress(alphaParams->foregroundPage, alphaParams->foregroundLeft, alphaParams->foregroundTop);
   destinationWindowAddr = GFXGetPageXYAddress(alphaParams->destinationPage, alphaParams->destinationLeft, alphaParams->destinationTop);

   while(DRV_GFX_S1D13517_SetReg(S1D13517_REG98_ALP_HR_SIZE,((width-1)>> 3)));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REG9A_ALP_VR_SIZE_0,height));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REG9C_ALP_VR_SIZE_1,(height >> 8)));

   foregroundWindowAddr  += (foregroundWindowAddr & 7);
   destinationWindowAddr += (destinationWindowAddr & 7);

   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGA0_ALP_IN_IMG1_SA_0,foregroundWindowAddr));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGA2_ALP_IN_IMG1_SA_1,foregroundWindowAddr>>8));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGA4_ALP_IN_IMG1_SA_2,foregroundWindowAddr>>16));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGAC_ALP_OUT_IMG_SA_0,destinationWindowAddr));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGAE_ALP_OUT_IMG_SA_1,destinationWindowAddr>>8));
   while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB0_ALP_OUT_IMG_SA_2,destinationWindowAddr>>16));

   if(alpha == 100)
   {
       alphaReg = (0xC0 | 0x20);
   }
   else
   {
       backgroundWindowAddr = GFXGetPageXYAddress(alphaParams->backgroundPage, alphaParams->backgroundLeft, alphaParams->backgroundTop);
       backgroundWindowAddr  += (backgroundWindowAddr & 7);
       alphaReg = (0x80 | Percentage2Alpha(alpha));

       while(DRV_GFX_S1D13517_SetReg(S1D13517_REGA6_ALP_IN_IMG2_SA_0,backgroundWindowAddr));
       while(DRV_GFX_S1D13517_SetReg(S1D13517_REGA8_ALP_IN_IMG2_SA_1,backgroundWindowAddr>>8));
       while(DRV_GFX_S1D13517_SetReg(S1D13517_REGAA_ALP_IN_IMG2_SA_2,backgroundWindowAddr>>16));

    }

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG9E_ALP_VALUE, alphaReg));
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG94_ALP_CONTROL, 0x01)); 				    //Enable Alpha Blend
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG94_ALP_CONTROL, 0x00));

    while(DRV_GFX_S1D13517_GetReg((S1D13517_REGB4_INTERRUPT_STAT)) != 1);
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB6_INTERRUPT_CLEAR,0x01));
    while(DRV_GFX_S1D13517_SetReg(S1D13517_REGB6_INTERRUPT_CLEAR,0x00));

    return((uint16_t*)&alphaReg);
}

uint16_t* DRV_GFX_S1D13517_Layer(uint8_t type, GFX_LAYER_PARAMS* layer)
{
    static uint32_t PIPStartAddress;
            
    while((displayReg = DRV_GFX_S1D13517_GetReg(S1D13517_REG2A_DSP_MODE))==0);

    layer->left &= 0xf8; //X must be a multiple of 8
    PIPStartAddress = GFXGetPageXYAddress(layer->page,layer->left,layer->top);

    if(layer->on == 0)
    {
        displayReg &= 0xf1;
        return(NULL);
    }

    displayReg |= 0x04;
            
    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].XStart, (layer->layerLeft)>>2));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].XEnd, (layer->layerLeft +(layer->width)) >> 2));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].YStart0, (layer->layerTop) >> 2));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].YStart1, layer->layerTop));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].YEnd0, (layer->layerTop+layer->height) >> 2));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].YEnd1, layer->layerTop + layer->height ));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].StartAddress0,PIPStartAddress));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].StartAddress1,PIPStartAddress >> 8));

    while(DRV_GFX_S1D13517_SetReg(PIPRegister[type].StartAddress2,PIPStartAddress >> 16));

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG2A_DSP_MODE, displayReg));

    while(DRV_GFX_S1D13517_SetReg(S1D13517_REG50_DISPLAY_CONTROL,0x80));

    return(&queueCount);
}

inline void WindowSetup(uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{

   while(DRV_GFX_S1D13517_SetReg(S1D13517_REG5A_WRITE_WIN_X_SP,(x >> 2)));

   DRV_PMP0_Write(y>>2);
   DRV_PMP0_Write(y);
   DRV_PMP0_Write((x + (count-1))>>2);
   DRV_PMP0_Write((y + lineCount-1)>>2);
   DRV_PMP0_Write((y + lineCount-1));

   beforeCount = ((x) & 7);
   afterCount = (beforeCount + count);  //Get Pixel Count
   afterCount &= 7;
   afterCount = (8 - afterCount) & 7;
}

inline void _PutArray(GFX_COLOR* array, uint16_t count, uint16_t lineCount)
{
    short temp;
    
     #if (DISP_ORIENTATION == 180)
     static int8_t change = -1;
     #else
     static int8_t change = 1;
     #endif

            while(lineCount)
            {

                for(temp = 0;temp < beforeCount; temp++)
                {
                     WritePixel(GFX_RGBConvert(0xFF, 0x00, 0xCC));
                }

                for(temp = 0;temp < count; temp++)
                {
                    WritePixel(*array);
                    array += change;
                }

                for(temp = 0;temp < afterCount; temp++)
                {
                    WritePixel(GFX_RGBConvert(0xFF, 0x00, 0xCC));
                }

                array += (count * change);
                lineCount--;
           }

}
inline void _PutPixels(GFX_COLOR color, uint16_t count, uint16_t lineCount)
{
    short temp;

    while(lineCount)
    {

       for(temp = 0;temp < beforeCount; temp++)
       {
           WritePixel(GFX_RGBConvert(0xFF, 0x00, 0xCC));
       }

       for(temp = 0;temp < count; temp++)
       {
           WritePixel(color);
      }

       for(temp = 0;temp < afterCount; temp++)
       {
           WritePixel(GFX_RGBConvert(0xFF, 0x00, 0xCC));
      }

           lineCount--;
   }

}
