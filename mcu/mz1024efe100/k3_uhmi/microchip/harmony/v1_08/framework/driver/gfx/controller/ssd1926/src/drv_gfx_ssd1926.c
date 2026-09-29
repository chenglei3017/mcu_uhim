/*******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    drv_gfx_ssd1926.c
    
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
#include "../drv_gfx_ssd1926.h"
#include "driver/pmp/drv_pmp.h"
#include "system/tmr/sys_tmr.h"
#include "driver/gfx/gfx_common.h"

DRV_GFX_SSD1926_COMMAND  commandBuffer;  //This contains the commands to send to the graphics driver
DRV_GFX_INTERFACE ssdInterface;

uint8_t   dataWidth;     /*PMP Data Bus Width*/



// *****************************************************************************
/* SSD1926 Driver task states

  Summary
    Lists the different states that SSD1926 task routine can have.

  Description
    This enumeration lists the different states that SSD1926 task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    /* Process queue */
    DRV_GFX_SSD1926_INITIALIZE_START,

    DRV_GFX_SSD1926_INITIALIZE_PWR_UP,

    DRV_GFX_SSD1926_INITIALIZE_SET_REGISTERS,

    /* GFX SSD1926 task initialization done */
    DRV_GFX_SSD1926_INITIALIZE_DONE,

} DRV_GFX_SSD1926_OBJECT_TASK;

// *****************************************************************************
/* GFX SSD1926 Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_GFX_SSD1926_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* SSD1926 machine state */
    DRV_GFX_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    DRV_GFX_CLIENT_OBJ *                        pDrvSSD1926ClientObj;

    /* State of the task */
    DRV_GFX_SSD1926_OBJECT_TASK    		task;

    DRV_GFX_INIT *                              initData;

    uint16_t      maxY;
    uint16_t      maxX;
} DRV_GFX_SSD1926_OBJ;

static DRV_GFX_SSD1926_OBJ        drvSSD1926Obj;

static DRV_GFX_CLIENT_OBJ drvSSD1926Clients;


void GFX_TMR_DelayMS ( uint32_t delayMs );


/*********************************************************************
* Macro:  WritePixel(color)
*
* PreCondition: none
*
* Input: color
*
* Output: none
*
* Side Effects: none
*
* Overview: writes pixel at the current address
*
* Note: chip select should be enabled
*
********************************************************************/
#ifdef USE_16BIT_PMP
#define WritePixel(color)	DRV_PMP0_Write(color);
#else
#define WritePixel(color)	DRV_PMP0_Write(color>>8); \
                            DRV_PMP0_Write(color);
#endif


// *****************************************************************************
/*
  Function: DRV_GFX_SSD1926_Open(uint8_t instance)

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
DRV_GFX_HANDLE DRV_GFX_SSD1926_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)DRV_HANDLE_INVALID;

    /* Check if instance object is ready*/
    if(drvSSD1926Obj.status != SYS_STATUS_READY)
    {
        /* The SSD1926 module should be ready */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: Was the driver initialized?");
    }
    else if(intent != DRV_IO_INTENT_EXCLUSIVE)
    {
        /* The driver only supports this mode */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: IO intent mode not supported");
    }
    else if(drvSSD1926Obj.nClients > 0)
    {
        /* Driver supports exclusive open only */
//        SYS_DEBUG(0, "GFX_SSD1926 already opened once. Cannot open again");
    }
    else
    {
        client = &drvSSD1926Clients;

        client->inUse = true;
        client->drvObj = &drvSSD1926Obj;

        /* Increment the client number for the specific driver instance*/
        drvSSD1926Obj.nClients++;
    }

    /* Return invalid handle */
    return ((DRV_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_SSD1926_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the graphics controller

  Description:
    This is closes the instance of the driver specified by handle.
*/
void DRV_GFX_SSD1926_Close( DRV_HANDLE handle )
{
    /* Start of local variable */
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)NULL;
    DRV_GFX_SSD1926_OBJ * drvObj = (DRV_GFX_SSD1926_OBJ *)NULL;
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
            drvObj = (DRV_GFX_SSD1926_OBJ *)client->drvObj;

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
     void DRV_GFX_SSD1926_MaxXGet()

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
uint16_t DRV_GFX_SSD1926_MaxXGet()
{
    return drvSSD1926Obj.maxX;
}

// *****************************************************************************
/*
  Function:
     void GFX_MaxYGet()

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
uint16_t DRV_GFX_SSD1926_MaxYGet()
{
    return drvSSD1926Obj.maxY;
}

/*********************************************************************
  Function:
     DRV_GFX_INTEFACE DRV_GFX_SSD1926_InterfaceGet( DRV_HANDLE handle )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_SSD1926_InterfaceSet( DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )
{
    interface->BarFill = DRV_GFX_SSD1926_BarFill;
    interface->PixelArrayPut = DRV_GFX_SSD1926_PixelArrayPut;
    interface->PixelArrayGet = DRV_GFX_SSD1926_PixelArrayGet;
    interface->PixelPut = DRV_GFX_SSD1926_PixelPut;
    interface->ColorSet = DRV_GFX_SSD1926_SetColor;
    interface->InstanceSet = DRV_GFX_SSD1926_SetInstance;
    interface->MaxXGet = DRV_GFX_SSD1926_MaxXGet;
    interface->MaxYGet = DRV_GFX_SSD1926_MaxYGet;

}
// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_SSD1926_SetAddress(DWORD address)

  Summary:
    sets the pmp address for read/write operations

  Description:
    none

  Input:
    address - address

  Output:
    0 - when call was passed
*/
uint16_t DRV_GFX_SSD1926_SetAddress(uint32_t address)
{
    static uint32_t temp;

    temp = (address << 8) + 1;
    temp |= (0x80000000);

    DisplaySetCommand(); //Set Address Line Low
   
#ifdef USE_8BIT_PMP
    DRV_PMP0_Write(temp>>24);
    DRV_PMP0_Write(temp>>16);
    DRV_PMP0_Write(temp>>8);
#else
    DRV_PMP0_Write(temp>>16);
#endif

    DRV_PMP0_Write(temp);

    DisplaySetData(); //Set Address Line Low

    return(0);

}

// *****************************************************************************
/*
  Function: uint8_t DRV_GFX_SSD1926_SetReg(uint16_t index, uint8_t value)

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
uint16_t DRV_GFX_SSD1926_SetReg(uint16_t index, uint8_t value)
{
    static uint32_t myWriteBuffer;

    DisplaySetCommand(); //Set Address Line Low

#ifdef USE_8BIT_PMP
    DRV_PMP0_Write(0x00);
    DRV_PMP0_Write(index >> 8);
    DRV_PMP0_Write(index);
#else
    DRV_PMP0_Write(index >> 8);
    DRV_PMP0_Write(index << 8);
#endif

    DisplaySetData(); //Set Address Line Low

    myWriteBuffer = (value << (dataWidth*8*(!(index & 0x0001))));

    DRV_PMP0_Write(myWriteBuffer);

    return(0);
}

// *****************************************************************************
/*
  Function: uint8_t DRV_GFX_SSD1926_GetReg(uint16_t index, uint8_t *data)

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
uint8_t  DRV_GFX_SSD1926_GetReg(uint16_t  index, uint8_t *data)
{  
    static uint16_t myReadBuffer = 0xff;

    DisplaySetCommand(); //Set Address Line Low

#ifdef USE_8BIT_PMP
    DRV_PMP0_Write(0x00);
    DRV_PMP0_Write(index >> 8);
    DRV_PMP0_Write(index);
#else
    DRV_PMP0_Write(index >> 8);
    DRV_PMP0_Write(index << 8);
#endif

    DisplaySetData(); //Set Address Line Low

    myReadBuffer = DRV_PMP0_Read();
    DRV_PMP0_Read();

    PMCONbits.PMPEN = 0; // disable PMP
    myReadBuffer = DRV_PMP0_Read();
    PMCONbits.PMPEN = 1; // disable PMP

    myReadBuffer >>= (dataWidth * 8);
    *data = (uint8_t)myReadBuffer;
    return(0);
    
}
// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_SSD1926_Initialize(uint8_t instance)

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
SYS_MODULE_OBJ DRV_GFX_SSD1926_Initialize(const SYS_MODULE_INDEX   index,
                                          const SYS_MODULE_INIT    * const init)
{

    static uint8_t   state =0;
    static uint8_t   dummy = 1;
    static uint16_t  horizontalSize, verticalSize, rotation;
    static uint8_t   panelType  = 0;
    static uint8_t   panelWidth = 0;
    static uint32_t  windowStartAddress, windowOffset;

    /* Validate the driver index */
    if ( index >= GFX_CONFIG_NUMBER_OF_MODULES )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    DRV_GFX_SSD1926_OBJ *dObj = &drvSSD1926Obj;

    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->state = SYS_STATUS_BUSY;
    dObj->initData = (DRV_GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = index;

    uint16_t  horizontalTotal = (drvSSD1926Obj.initData->horizontalPulseWidth + drvSSD1926Obj.initData->horizontalBackPorch + drvSSD1926Obj.initData->horizontalFrontPorch + drvSSD1926Obj.initData->horizontalResolution);
    uint16_t  verticalTotal = (drvSSD1926Obj.initData->verticalPulseWidth + drvSSD1926Obj.initData->verticalBackPorch + drvSSD1926Obj.initData->verticalFrontPorch + drvSSD1926Obj.initData->verticalResolution);


    dObj->task = DRV_GFX_SSD1926_INITIALIZE_START;

   while(state <= 45)
   {
       switch(state)
       {

            case 0:
                DisplayResetEnable();               // hold in reset by default
                DisplayResetConfig();               // enable RESET line
                DisplayCmdDataConfig();
                DisplayConfig();                    // enable chip select line
                
#ifdef USE_16BIT_PMP
                dataWidth = PMP_DATA_SIZE_16_BITS;
#else
                dataWidth = PMP_DATA_SIZE_8_BITS;
#endif

                DisplayResetDisable();
                DisplayEnable();

                /*time for the controller to power up*/
                GFX_TMR_DelayMS(500);

                if((drvSSD1926Obj.initData->orientation == 90) || (drvSSD1926Obj.initData->orientation == 270))
                {
                    horizontalSize = drvSSD1926Obj.initData->verticalResolution;
                    verticalSize   = drvSSD1926Obj.initData->horizontalResolution;
                    drvSSD1926Obj.maxX = horizontalSize - 1;
                    drvSSD1926Obj.maxY = verticalSize - 1;
                }
                else
                {
                    horizontalSize = drvSSD1926Obj.initData->horizontalResolution;
                    verticalSize   = drvSSD1926Obj.initData->verticalResolution;
                    drvSSD1926Obj.maxX = horizontalSize - 1;
                    drvSSD1926Obj.maxY = verticalSize - 1;
                }

                state++;

                panelWidth |= ((drvSSD1926Obj.initData->dataWidth >> 3)<<4);

                if (drvSSD1926Obj.initData->LCDType == LCD_TFT)
                {
                    panelType |= 0x41;
                }
                else if (drvSSD1926Obj.initData->LCDType == LCD_CSTN2)
                {
                    panelType |= 0x40;
                }

                switch(drvSSD1926Obj.initData->orientation)
                {
                    case 90:
                        windowStartAddress = ((((uint32_t) horizontalSize + 1) >> 1) - 1);
                        rotation = 1;
                        break;

                    case 180:
                        windowStartAddress = (((((uint32_t) horizontalSize + 1) * (verticalSize + 1)) >> 1) - 1);
                        rotation = 2;
                        break;

                    case 270:
                        windowStartAddress = ((((uint32_t) horizontalSize + 1) * verticalSize) >> 1);
                        rotation = 3;
                        break;

                    /*case 0:*/
                    default:
                        windowStartAddress = 0ul;
                        rotation = 0;
                        break;

                }

                windowOffset = (horizontalSize + 1) >> 1;
                break;

            case 33:
                while(dummy)
                {
                    while(DRV_GFX_SSD1926_GetReg(SSD1926_REG_POWER_SAVE_CONFIG, &dummy));

                }

                state++;
                state++;

            default:
                break;
        }

        /*SSD1926 Registers to be initialized*/
        uint16_t registers[44][2] = {
                                    { SSD1926_REG_PLL_CONFIG_0,0x0a },
                                    { SSD1926_REG_PLL_CONFIG_1,0xc8  },
                                    { SSD1926_REG_PLL_CONFIG_2,0xae  },
                                    { SSD1926_REG_PLL_CONFIG_0,0x8a  },
                                    { SSD1926_REG_MEMCLK_CONFIG,0  },
                                    { SSD1926_REG_PCLK_FREQ_RATIO_0,0  },
                                    { SSD1926_REG_PCLK_FREQ_RATIO_1,0  },
                                    { SSD1926_REG_PCLK_FREQ_RATIO_2,0x02  },
                                    { SSD1926_REG_PANEL_TYPE,panelType|panelWidth  },
                                    { SSD1926_REG_HORIZ_TOTAL_0,(horizontalTotal >> 3)  },
                                    { SSD1926_REG_HORIZ_TOTAL_1,(horizontalTotal % 8)  },
                                    { SSD1926_REG_HDP,(drvSSD1926Obj.initData->horizontalResolution >>3) - 1  },
                                    { SSD1926_REG_HDP_START_POS0,(drvSSD1926Obj.initData->horizontalPulseWidth + drvSSD1926Obj.initData->horizontalBackPorch)  },
                                    { SSD1926_REG_HDP_START_POS1,(drvSSD1926Obj.initData->horizontalPulseWidth + drvSSD1926Obj.initData->horizontalBackPorch)>>8  },
                                    { SSD1926_REG_HSYNC_PULSE_WIDTH,(drvSSD1926Obj.initData->horizontalPulseWidth - 1)  },
                                    { SSD1926_REG_VERT_TOTAL0,(verticalTotal)  },
                                    { SSD1926_REG_VERT_TOTAL1,(verticalTotal>>8)  },
                                    { SSD1926_REG_VDP0,(drvSSD1926Obj.initData->verticalResolution - 1)  },
                                    { SSD1926_REG_VDP1,((drvSSD1926Obj.initData->verticalResolution - 1)>>8)  },
                                    { SSD1926_REG_VDP_START_POS0,(drvSSD1926Obj.initData->verticalPulseWidth + drvSSD1926Obj.initData->verticalBackPorch)  },
                                    { SSD1926_REG_VDP_START_POS1,(drvSSD1926Obj.initData->verticalPulseWidth + drvSSD1926Obj.initData->verticalBackPorch)>>8  },
                                    { SSD1926_REG_VSYNC_PULSE_WIDTH,(drvSSD1926Obj.initData->verticalPulseWidth - 1)  },
                                    { SSD1926_REG_SPECIAL_EFFECTS, (0x40 | rotation)  },
                                    { SSD1926_REG_MAIN_WIN_DISP_START_ADDR0,(windowStartAddress)  },
                                    { SSD1926_REG_MAIN_WIN_DISP_START_ADDR1,(windowStartAddress>>8)  },
                                    { SSD1926_REG_MAIN_WIN_DISP_START_ADDR2,(windowStartAddress>>16)  },
                                    { SSD1926_REG_MAIN_WIN_ADDR_OFFSET0, windowOffset  },
                                    { SSD1926_REG_MAIN_WIN_ADDR_OFFSET1, (windowOffset >> 8)  },
                                    { SSD1926_REG_DISPLAY_MODE,0x04  },
                                    { SSD1926_REG_RGB_SETTING,0xc0  },
                                    { SSD1926_REG_LSHIFT_POLARITY, drvSSD1926Obj.initData->logicShift  },
                                    { SSD1926_REG_POWER_SAVE_CONFIG,0  },
                                    { 0,0  },
                                    { 0,0  },
                                    { SSD1926_REG_GPIO_STATUS_CONTROL1,0x80  },
                                    { SSD1926_REG_2D_1f8, horizontalSize  },
                                    { SSD1926_REG_2D_1f9,(horizontalSize >> 8)  },
                                    { SSD1926_REG_2D_1d8, horizontalSize  },
                                    { SSD1926_REG_2D_1d9,(horizontalSize >> 8)  },
                                    { SSD1926_REG_2D_214,1  },
                                    { SSD1926_REG_2D_215,0  },
                                    { SSD1926_REG_2D_218,1  },
                                    { SSD1926_REG_2D_219,0  },
                                    { SSD1926_REG_2D_1dd,0 } };

        while(DRV_GFX_SSD1926_SetReg(*registers[state-1], registers[state-1][1]));

        state++;
    }

    if(drvSSD1926Obj.initData->TCON_Init != NULL)
    {
       drvSSD1926Obj.initData->TCON_Init();
    }

   drvSSD1926Obj.initData->horizontalResolution = horizontalSize;
   drvSSD1926Obj.initData->verticalResolution = verticalSize;

    DisplayBacklightOn();

    dObj->nClients = 0;
    dObj->status = SYS_STATUS_READY;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)dObj;

}

// *****************************************************************************
/*
  Function: void DRV_GFX_SSD1926_SetColor(GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:
  
  Output: none

*/

void DRV_GFX_SSD1926_SetColor(GFX_COLOR color)
{


    drvSSD1926Obj.initData->color = color;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_SSD1926_SetInstance(uint8_t instance)

  Summary: Sets the instance for the driver

  Description:
  
  Output: none

*/

void DRV_GFX_SSD1926_SetInstance(uint8_t instance)
{
    instance = instance;
}

uint16_t  DRV_GFX_SSD1926_LineDraw(short x1, short y1, short x2, short y2)
{
    static uint8_t state = 0;
    static uint16_t lineParams[13][2] = {{SSD1926_REG_2D_1e4,0},{SSD1926_REG_2D_1e5, 0},{SSD1926_REG_2D_1e8,0}, //Destination Address
                                       {SSD1926_REG_2D_1e9,0},{SSD1926_REG_2D_1ec,0},{SSD1926_REG_2D_1ed,0},  //Brush Address
                                       {SSD1926_REG_2D_1f0,0},{SSD1926_REG_2D_1f1,0},{SSD1926_REG_2D_1fe,0x09},{SSD1926_REG_2D_1fd,0x01}, //line 2d Commands
                                       {SSD1926_REG_2D_1fc,0xf0},{SSD1926_REG_2D_1d1,0x01},{SSD1926_REG_2D_1d2,0x01}}; //line 2d Commands

    if(state == 0)
    {
        /* Line Boundaries */
        lineParams[0][1] =  (uint8_t)x1;
        lineParams[1][1] =  (uint8_t)(x1 >> 8);
        lineParams[2][1] =  (uint8_t)y1;
        lineParams[3][1] =  (uint8_t)(y1 >> 8);
        lineParams[4][1] =  (uint8_t)x2;
        lineParams[5][1] =  (uint8_t)(x2 >> 8);
        lineParams[6][1] =  (uint8_t)y2;
        lineParams[7][1] =  (uint8_t)(y2 >> 8);

        /* Set Color */
        lineParams[8][1] = GFX_ComponentRedGet(drvSSD1926Obj.initData->color);
        lineParams[9][1] = GFX_ComponentGreenGet(drvSSD1926Obj.initData->color);
        lineParams[10][1] = GFX_ComponentBlueGet(drvSSD1926Obj.initData->color);

        state = 1;

    }

    while(1)
    {
        while(DRV_GFX_SSD1926_SetReg(lineParams[state-1][0], (uint8_t)lineParams[state-1][1]));

        state++;

        if(state == 14)
        {
            state = 0;
            while(DRV_GFX_SSD1926_Busy(0) == 1);
            return(0);
        }
    }
    return (1);
}

// *****************************************************************************
/*
  Function: void  DRV_GFX_SSD1926_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    Hardware accelerated barfill function

  Description:
    see primitive BarDraw
  
  Output:
    1 - call not successful (PMP driver busy)
    0 - call successful
*/
void  DRV_GFX_SSD1926_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
   

   static uint32_t   address;
   static short      width, height;
   static uint8_t    state = 0;

   static uint16_t barParams[13][2] = {{SSD1926_REG_2D_1f4,0},{SSD1926_REG_2D_1f5, 0},{SSD1926_REG_2D_1f6,0}, //Destination Address
                                       {SSD1926_REG_2D_204,0},{SSD1926_REG_2D_205,0},{SSD1926_REG_2D_206,0},  //Brush Address
                                       {SSD1926_REG_2D_1e4,0},{SSD1926_REG_2D_1e5,0},{SSD1926_REG_2D_1e8,0},{SSD1926_REG_2D_1e9,0}, //Width and Height
                                       {SSD1926_REG_2D_1fc,0xf0},{SSD1926_REG_2D_1d1,0x09},{SSD1926_REG_2D_1d2,0x01}}; //Bar 2d Commands

    switch(state)
    {
        case 0:
            width = right - left + 1;
            barParams[6][1] = width;
            barParams[7][1] = width>>8;

            height = bottom - top + 1;
            barParams[8][1] = height;
            barParams[9][1] = (height >> 8);


            address = ((uint32_t) (drvSSD1926Obj.initData->horizontalResolution) * top + left);
            barParams[0][1] = address;
            barParams[1][1] = (address >> 8);
            barParams[2][1] = (address >> 16);
            barParams[3][1] = barParams[0][1];
            barParams[4][1] = barParams[1][1];
            barParams[5][1] = barParams[2][1];
            state++;

        case 1:
            while(DRV_GFX_SSD1926_SetAddress(address<<1));
            state++;

        case 2:

            WritePixel(drvSSD1926Obj.initData->color);
            state++;

        default:
            break;
        }

    while(1)
    {
        while(DRV_GFX_SSD1926_SetReg(barParams[state-3][0], (uint8_t)barParams[state-3][1]));

        if(state++ == 15)
        {
            state = 0;
            while(DRV_GFX_SSD1926_Busy(0) == 1);
            return;
        }
    }

}

// *****************************************************************************
/*
  Function: void DRV_GFX_SSD1926_PixelPut(uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        instance - driver instance
        color - color to output
        x,y - pixel coordinates
  Output:
    1 - call not successful (SSD1926 driver busy)
    0 - call successful
*/
void DRV_GFX_SSD1926_PixelPut(uint16_t x, uint16_t y)
{


    commandBuffer.address = (((uint32_t) (drvSSD1926Obj.initData->horizontalResolution)) * (y) + (x)) << 1;

    while(DRV_GFX_SSD1926_SetAddress(commandBuffer.address));


    WritePixel(drvSSD1926Obj.initData->color);

}  

// *****************************************************************************
/*
  Function: void  DRV_GFX_SSD1926_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count)

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
void  DRV_GFX_SSD1926_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{


    uint16_t temp;


    commandBuffer.address = (((uint32_t) (drvSSD1926Obj.initData->horizontalResolution)) * (y) + (x)) << 1;
    commandBuffer.array = color;   //Starting address of the array

    while(lineCount)
    {
        while(DRV_GFX_SSD1926_SetAddress(commandBuffer.address));

        for(temp = 0; temp < count; temp++)
        {
            WritePixel(*color);
            color++;
        }


        commandBuffer.address += ((drvSSD1926Obj.initData->horizontalResolution)<<1);
        lineCount--;

     }

     commandBuffer.data = 0;

} 

// *****************************************************************************
/*
  Function: uint16_t  DRV_GFX_SSD1926_PixelArrayGet(uint16_t *color, short x, short y, uint16_t count)

  Summary:
    gets an array of pixels of length count starting at *color 

  Description:
    none

  Input:
          instance - driver instance
          *color - start of the array
		  x - x coordinate of the start point.
		  y - y coordinate of the end point.
		  count - number of pixels
  Output:
         1 - call not successful (SSD1926 driver busy)
         0 - call successful
*/ 
uint16_t*  DRV_GFX_SSD1926_PixelArrayGet(uint16_t *color, uint16_t x, uint16_t y, uint16_t count)
{
    /*Not yet supported*/
    return(NULL);
}  

// *****************************************************************************
/*
  Function: uint16_t  DRV_GFX_SSD1926_Busy(uint8_t instance)

  Summary:
    Returns non-zero if LCD controller is busy 
          (previous drawing operation is not completed).

  Description:
    none

  Input:
          instance - driver instance
  Output:
         1 - busy
         0 - not busy
*/ 
uint16_t  DRV_GFX_SSD1926_Busy(uint8_t instance)
{  

   uint8_t status = 0x00;
 
   if(DRV_GFX_SSD1926_GetReg(SSD1926_REG_2D_220,(uint8_t*)&status))
   {
       return(1);
   }


   return((bool)(!status));

}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_GFX_SSD1926_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific module instance of the Driver module.

  Description:
    This function returns the status of the specific module instance disabling its
    operation (and any hardware for driver modules).

  PreCondition:
    The DRV_GFX_SSD1926_Initialize function should have been called before calling
    this function.

  Parameters:
    object          - DRV_GFX_SSD1926 object handle, returned from DRV_GFX_SSD1926_Initialize

  Returns:
    SYS_STATUS_READY    Indicates that any previous module operation for the
                        specified module has completed

    SYS_STATUS_BUSY     Indicates that a previous module operation for the
                        specified module has not yet completed

    SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS DRV_GFX_SSD1926_Status ( SYS_MODULE_OBJ object )
{
    DRV_GFX_SSD1926_OBJ *dObj = (DRV_GFX_SSD1926_OBJ*)object;
    return ( dObj->state );

} /* SYS_TMR_Status */

void GFX_TMR_DelayMS ( unsigned int delayMs )
{
#if (true == SYS_TMR_INTERRUPT_MODE)

#else      //Primitive Blocking Mode
    if(delayMs)
    {
        uint32_t sysClk = SYS_CLK_FREQ;
        uint32_t t0;
        #if   defined (__C32__)
        t0 = _CP0_GET_COUNT();
        while (_CP0_GET_COUNT() - t0 < (sysClk/2000)*delayMs);
        #elif defined (__C30__)
        t0 = ReadTimer23();
        while (ReadTimer23() -  t0 < (sysClk/2000)*mSec);
        #else
        #error "Neither __C32__ nor __C30__ is defined!"
        #endif
    }
#endif
}