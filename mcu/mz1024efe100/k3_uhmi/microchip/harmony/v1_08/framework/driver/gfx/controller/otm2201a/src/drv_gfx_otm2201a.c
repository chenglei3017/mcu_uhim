/*
 ******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    drv_gfx_otm2201a.c

  Summary:
    Driver library for device OTM2201A with TFT display connected

  Description:
    Driver library for device OTM2201A includes:
    - Writing to device registers and GRAM using PMP Driver
    - Initialization of the OTM2201A
    - Supports rotation of the display
    - Supports displaying single pixel, n pixels, array of pixels

    PMP Bus width is 8 bit, GRAM address is 16 bit(8 - horizontal, 8 -
    vertical) and color format if 16 bit(5 - Red, 6 - Green, 5 - Blue)
 *******************************************************************************
*/
/*
******************************************************************************
  Software License Agreement
  Copyright � 2008 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller, which is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).

  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.

  SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
*******************************************************************************
*/

#include "../drv_gfx_otm2201a.h"

// *****************************************************************************
/* OTM2201a Driver task states

  Summary
    Lists the different states that OTM2201a task routine can have.

  Description
    This enumeration lists the different states that OTM2201a task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    /* Process queue */
    DRV_GFX_OTM2201A_INITIALIZE_START,

    DRV_GFX_OTM2201A_INITIALIZE_PWR_UP,

    DRV_GFX_OTM2201A_INITIALIZE_SET_REGISTERS,

    /* GFX OTM2201A task initialization done */
    DRV_GFX_OTM2201A_INITIALIZE_DONE,

} DRV_GFX_OTM2201A_OBJECT_TASK;

// *****************************************************************************
/* GFX OTM2201A Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_GFX_OTM2201A_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* OTM2201A machine state */
    DRV_GFX_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    DRV_GFX_CLIENT_OBJ *                        pDrvOTM2201AClientObj;

    /* State of the task */
    DRV_GFX_OTM2201A_OBJECT_TASK    		task;

    DRV_GFX_INIT *                              initData;

    uint16_t      maxY;
    uint16_t      maxX;

} DRV_GFX_OTM2201A_OBJ;

static DRV_GFX_OTM2201A_OBJ        drvOTM2201AObj;

static DRV_GFX_CLIENT_OBJ          drvOTM2201AClients;

/*
 ******************************************************************************
 *                      Gloabal Variables
 ******************************************************************************
 */

DRV_GFX_INTERFACE otmInterface;

//global instance for the driver
uint8_t             _instance;
//PMP Data Width
//uint8_t             _dataWidth;
//pmp driver handle
DRV_HANDLE          pmphandle;
DRV_PMP_MODE_CONFIG config;

//This contains the commands to send to the graphics driver
DRV_GFX_OTM2201A_COMMAND  OTM2201AcommandBuffer[1];

//pointer to where the command Buffer is currently
DRV_GFX_OTM2201A_COMMAND* OTM2201AqueueIndex = &OTM2201AcommandBuffer[0];

//Flag to state driver is busy
volatile uint16_t   OTM2201AdriverBusy   = false;
//Flag to state task is busy
volatile uint8_t    OTM2201AtaskBusy     = false;
//Flag to state task is Ready
volatile uint8_t    OTM2201AtaskReady    = false;

//holds instances of GFX_CONFIG_DRIVER_COUNT number of drivers
uint8_t instances[GFX_CONFIG_DRIVER_COUNT] = {0};

uint8_t xCount = 0;
uint8_t yCount = 0;

#define GFX_PMP_DeviceWrite(data) PLIB_PMP_MasterSend(0, data); \
                                  while(PMMODEbits.BUSY == 1)

// forward declarations
void _DelayMs(unsigned long int msDelay );
uint32_t _PixelStartAddress(uint16_t x, uint16_t y, uint16_t orientation);
uint32_t _PixelAddressUpdate(uint16_t x, uint16_t y,
                            uint16_t lineCount, uint16_t orientation);

// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_OTM2201A_Initialize(uint8_t instance)

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
SYS_MODULE_OBJ DRV_GFX_OTM2201A_Initialize(const SYS_MODULE_INDEX   index,
                                           const SYS_MODULE_INIT    * const init)
{

    static uint8_t  state           = 0;
    static uint16_t horizontalSize;
    static uint16_t verticalSize;
    static uint16_t dummy           = 1;
    static uint16_t entryMode       = 0;
    static uint16_t scanMode        = 0x001C;

    /* Validate the driver index */
    if ( index >= GFX_CONFIG_NUMBER_OF_MODULES )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    DRV_GFX_OTM2201A_OBJ *dObj = &drvOTM2201AObj;

    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->state = SYS_STATUS_BUSY;
    dObj->initData = (DRV_GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = index;

    dObj->task = DRV_GFX_OTM2201A_INITIALIZE_SET_REGISTERS;

    while (state <= 32) {
        switch (state) {
            case 0:

                DisplayResetEnable(); // hold in reset by default
                DisplayResetConfig(); // enable RESET line
                DisplayCmdDataConfig();
                DisplayConfig(); // enable chip select line

               _DelayMs(10);
                               
                // release from reset
                DisplayResetDisable();

                DisplayEnable();

                if ((drvOTM2201AObj.initData->orientation == DEGREE_90) ||
                        (drvOTM2201AObj.initData->orientation == DEGREE_270)) {
                    horizontalSize =
                            drvOTM2201AObj.initData->verticalResolution;
                    verticalSize =
                            drvOTM2201AObj.initData->horizontalResolution;
                    drvOTM2201AObj.maxX = horizontalSize - 1;
                    drvOTM2201AObj.maxY = verticalSize - 1;
                } else {
                    horizontalSize =
                            drvOTM2201AObj.initData->horizontalResolution;
                    verticalSize =
                            drvOTM2201AObj.initData->verticalResolution;
                    drvOTM2201AObj.maxX = verticalSize - 1;
                    drvOTM2201AObj.maxY = horizontalSize - 1;
                }

                switch (drvOTM2201AObj.initData->orientation) {
                    case DEGREE_90:
                        entryMode = 0x1008;
                        scanMode = 0x011C;
                        break;

                    case DEGREE_180:
                        entryMode = 0x1000;
                        scanMode = 0x011C;
                        break;

                    case DEGREE_270:
                        entryMode = 0x1008;
                        scanMode = 0x031C;
                        break;

                    case DEGREE_0:
                    default:
                        entryMode = 0x1030;
                        scanMode = 0x001C;
                        break;
                }

                break;

            case 1:
                while (!dummy)
                    while (DRV_GFX_OTM2201A_RegGet(REG_POWER_CTRL_2, &dummy));

                state = 2;
                break;

            case 3:
                break;

            default:
                break;
        }

        //OTM2201A Registers to be initialized
        uint16_t registers[33][2] = {
            { REG_POWER_CTRL_2, 0x0018 },
            { REG_POWER_CTRL_3, 0x0000 },
            { REG_POWER_CTRL_4, 0x0063 },
            { REG_POWER_CTRL_5, 0x556A },
            { REG_POWER_CTRL_1, 0x0800 },
            { REG_POWER_CTRL_2, 0x0118 },
            { REG_POWER_CTRL_2, 0x0318 },
            { REG_POWER_CTRL_2, 0x0718 },
            { REG_POWER_CTRL_2, 0x0F18 },
            { REG_POWER_CTRL_2, 0x0F38 },
            { REG_DISP_CTRL, 0x001A },
            { REG_DRV_OUT_CTRL, scanMode },
            { REG_ENTRY_MODE, entryMode },
            { REG_DISP_CTRL, 0x0000 },
            { REG_BLANKING_CTRL, 0x0808 },
            { REG_VCI_PERIOD, 0x0020 },
            { REG_HORZ_WND_ADDR_1, 0x00AF },
            { REG_HORZ_WND_ADDR_2, 0x0000 },
            { REG_VERT_WND_ADDR_1, 0x00DB },
            { REG_VERT_WND_ADDR_2, 0x0000 },
            { REG_GAMMA_CTRL_1, 0x0001 },
            { REG_GAMMA_CTRL_2, 0x0208 },
            { REG_GAMMA_CTRL_3, 0x0805 },
            { REG_GAMMA_CTRL_4, 0x0404 },
            { REG_GAMMA_CTRL_5, 0x0C0C },
            { REG_GAMMA_CTRL_6, 0x000C },
            { REG_GAMMA_CTRL_7, 0x0100 },
            { REG_GAMMA_CTRL_8, 0x0400 },
            { REG_GAMMA_CTRL_9, 0x1108 },
            { REG_GAMMA_CTRL_10, 0x050C },
            { REG_OSC_CTRL, 0x1F01 },
            { REG_DISP_CTRL, 0x0012 },
            { REG_DISP_CTRL, 0x0017 }
        };

        while (DRV_GFX_OTM2201A_RegSet(*registers[state],
                registers[state][1]));

        state++;
    }

    drvOTM2201AObj.initData->horizontalResolution = horizontalSize;
    drvOTM2201AObj.initData->verticalResolution = verticalSize;

    dObj->nClients = 0;
    dObj->status = SYS_STATUS_READY;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)dObj;
}

// *****************************************************************************
/*
  Function: DRV_HANDLE DRV_GFX_OTM2201A_Open( const SYS_MODULE_INDEX index,
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
DRV_HANDLE DRV_GFX_OTM2201A_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
   DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)DRV_HANDLE_INVALID;

    /* Check if the specified driver index is in valid range */
    if(index >= GFX_CONFIG_NUMBER_OF_MODULES)
    {
        //SYS_DEBUG(0, "GFX_SSD1926 Driver: Bad Driver Index");
    }
    /* Check if instance object is ready*/
    else if(drvOTM2201AObj.status != SYS_STATUS_READY)
    {
        /* The SSD1926 module should be ready */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: Was the driver initialized?");
    }
    else if(intent != DRV_IO_INTENT_EXCLUSIVE)
    {
        /* The driver only supports this mode */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: IO intent mode not supported");
    }
    else if(drvOTM2201AObj.nClients > 0)
    {
        /* Driver supports exclusive open only */
//        SYS_DEBUG(0, "GFX_SSD1926 already opened once. Cannot open again");
    }
    else
    {
        client = &drvOTM2201AClients;

        client->inUse = true;
        client->drvObj = &drvOTM2201AObj;

        /* Increment the client number for the specific driver instance*/
        drvOTM2201AObj.nClients++;
    }

    /* Return invalid handle */
    return ((DRV_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_OTM2201A_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the graphics controller

  Description:
    This is closes the instance of the driver specified by handle.
*/
void DRV_GFX_OTM2201A_Close( DRV_HANDLE handle )
{
    /* Start of local variable */
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)NULL;
    DRV_GFX_OTM2201A_OBJ * drvObj = ( DRV_GFX_OTM2201A_OBJ *)NULL;
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
            drvObj = ( DRV_GFX_OTM2201A_OBJ *)client->drvObj;

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
     uint16_t DRV_GFX_OTM2201A_MaxXGet(void)

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
uint16_t DRV_GFX_OTM2201A_MaxXGet(void)
{
    return drvOTM2201AObj.maxX;
}

// *****************************************************************************
/*
  Function:
     uint16_t DRV_GFX_OTM2201A_MaxYGet(void)

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
uint16_t DRV_GFX_OTM2201A_MaxYGet(void)
{
    return drvOTM2201AObj.maxY;
}

/*********************************************************************
  Function:
     DRV_GFX_INTEFACE DRV_GFX_OTM2201A_InterfaceGet( DRV_HANDLE handle )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_OTM2201A_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE *interface )
{
    interface->BarFill = DRV_GFX_OTM2201A_BarFill;
    interface->PixelArrayPut = DRV_GFX_OTM2201A_PixelArrayPut;
    interface->PixelArrayGet = DRV_GFX_OTM2201A_PixelArrayGet;
    interface->PixelPut = DRV_GFX_OTM2201A_PixelPut;
    interface->ColorSet = DRV_GFX_OTM2201A_ColorSet;
    interface->MaxXGet = DRV_GFX_OTM2201A_MaxXGet;
    interface->MaxYGet = DRV_GFX_OTM2201A_MaxYGet;
}

/*
*******************************************************************************
  Function: uint16_t DRV_GFX_OTM2201A_AddressSet(uint32_t address)

  Summary:
    Sets the start GRAM address where pixel data to be written

  Description:
    Address consists of: 
        Lower 8 bit at Register REG_RAM_ADDR_LOW
        Higher 8 bit at Register REG_RAM_ADDR_HIGH

  Input:
    address - address

  Output:
    DRV_OTM2201A_ERROR_PMP_WRITE - returns error during PMP Write
    DRV_OTM2201A_ERROR_NO_ERROR - returns success without any error
*******************************************************************************
*/

uint16_t DRV_GFX_OTM2201A_AddressSet(uint32_t address)
{
    static uint32_t data = 0x00000000;
    
    DisplaySetCommand(); // set RS line to low for command
    GFX_PMP_DeviceWrite(0x00);
    GFX_PMP_DeviceWrite(REG_RAM_ADDR_LOW);
    DisplaySetData(); // set RS line to low for command

    data  = address & DRV_OTM2201A_ADDR_LOW_MASK;
    GFX_PMP_DeviceWrite(data>>8);
    GFX_PMP_DeviceWrite(data);
    
    DisplaySetCommand(); // set RS line to low for command
    GFX_PMP_DeviceWrite(0x00);
    GFX_PMP_DeviceWrite(REG_RAM_ADDR_HIGH);
    DisplaySetData(); // set RS line to low for command

    data = (address & DRV_OTM2201A_ADDR_HIGH_MASK)
            >> DRV_OTM2201A_ADDR_HIGH_SHIFT;

    GFX_PMP_DeviceWrite(data>>8);
    GFX_PMP_DeviceWrite(data);

    return(DRV_OTM2201A_ERROR_NO_ERROR);
}

/*
 *****************************************************************************
  Function: uint8_t DRV_GFX_OTM2201A_RegSet(uint16_t index,
                                            uint16_t value,
                                            uint32_t repeatCount)

  Summary:
    updates graphics controller register value (byte access)

  Description:
    This call can set "value" of the register accessed by its "index" and can
 repeat the same by number of times mentioned in "repeatCount"

  Input:
    index       - register number
    value       - value to write to register
    repeatCount - repeatCount number of times value is to be written to the
                    register.

  Output:
    DRV_OTM2201A_ERROR_PMP_WRITE - returns error during PMP Write
    DRV_OTM2201A_ERROR_NO_ERROR - returns success without any error
 
*******************************************************************************
*/
uint16_t DRV_GFX_OTM2201A_RegSet(uint16_t index,
                                uint16_t value)
{

    DisplaySetCommand(); // set RS line to low for command
    GFX_PMP_DeviceWrite(0x00);
    GFX_PMP_DeviceWrite(index);
    DisplaySetData(); // set RS line to low for command

    PLIB_PMP_MasterSend(0, value>>8);
    while(PMMODEbits.BUSY == 1);

    PLIB_PMP_MasterSend(0, value);
    while(PMMODEbits.BUSY == 1);

    return(DRV_OTM2201A_ERROR_NO_ERROR);
}

/*
*******************************************************************************
  Function: uint8_t DRV_GFX_OTM2201A_RegGet(
                                            uint16_t index,
                                            uint8_t *data
                                            )

  Summary:
    returns graphics controller register value (byte access)

  Description:
    returns graphics controller register value (byte access)

  Input:
    index - register number
    *data - array to store register data

  Output:
    DRV_OTM2201A_ERROR_PMP_WRITE - returns error during PMP Write
    DRV_OTM2201A_ERROR_PMP_READ  - returns error during PMP Read
    DRV_OTM2201A_ERROR_NO_ERROR  - returns success without any error
*******************************************************************************
*/
uint8_t  DRV_GFX_OTM2201A_RegGet(uint16_t  index, uint16_t *data)
{

    static uint16_t myReadBuffer = 0x0000;

    DisplaySetCommand(); // set RS line to low for command
    GFX_PMP_DeviceWrite(0x00);
    GFX_PMP_DeviceWrite(index);
    DisplaySetData(); // set RS line to low for command

    myReadBuffer = PMDIN;
    while (PMMODEbits.BUSY);
    PMCONbits.PMPEN = 0; // disable PMP
    myReadBuffer = PMDIN;
    while (PMMODEbits.BUSY);
    PMCONbits.PMPEN = 1; // enable  PMP

    *data = PMDIN;
    while (PMMODEbits.BUSY);
    PMCONbits.PMPEN = 0; // disable PMP
    myReadBuffer = PMDIN;
    while (PMMODEbits.BUSY);
    PMCONbits.PMPEN = 1; // enable  PMP

    *data  = myReadBuffer;
    return(DRV_OTM2201A_ERROR_NO_ERROR);
    
}

/*
*******************************************************************************

  Function: uint8_t DRV_GFX_OTM2201A_BrightnessSet(
    uint8_t instance,
    uint16_t level
    )

  Summary:
    Sets the brightness of the display backlight.

  Description:
    Sets the brightness of the display backlight.

  Input:
        level - Brightness level. Valid values are 0 to 100.
		0   = brightness level is zero or display is turned off.
                100 = brightness level is maximum.

  Output:
    none
 
********************************************************************************
*/
void DRV_GFX_OTM2201A_BrightnessSet(uint8_t instance, uint16_t  level)
{

    // If the brightness can be controlled (for example through PWM)
    // add code that will control the PWM here.

    if (level > 0)
    {
        DisplayBacklightOn();
    }
    else if (level == 0)
    {
        DisplayBacklightOff();
    }

}

/********************************************************************************

  Function: void DRV_GFX_OTM2201A_ColorSet(GFX_COLOR color)

  Summary:
    Sets the color for the driver instance

  Description:
    Sets the color for the driver instance

  Input:
    color - 16 bit 565 format color value

  Output: none

*******************************************************************************
*/

void DRV_GFX_OTM2201A_ColorSet(GFX_COLOR color)
{
     drvOTM2201AObj.initData->color = color;
}

/*
*******************************************************************************

  Function: void DRV_GFX_OTM2201A_PixelPut(uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        x           - pixel coordinate on x axis
        y           - pixel coordinate on y axis
 
  Output:

        DRV_OTM2201A_ERROR_QUEUE_FULL   - OTM2201A command queue is full
        DRV_OTM2201A_ERROR_NO_ERROR     - Success without any error

*******************************************************************************
*/
void DRV_GFX_OTM2201A_PixelPut(uint16_t x, uint16_t y)
{

     OTM2201AqueueIndex->address     = _PixelStartAddress(x, y,
                                    drvOTM2201AObj.initData->orientation);

     while(DRV_GFX_OTM2201A_AddressSet(OTM2201AqueueIndex->address));

     DisplaySetCommand(); // set RS line to low for command
     GFX_PMP_DeviceWrite(0x00);
     GFX_PMP_DeviceWrite(REG_GRAM_DATA);
     DisplaySetData(); // set RS line to low for command

     GFX_PMP_DeviceWrite(drvOTM2201AObj.initData->color >> 8);
     GFX_PMP_DeviceWrite(drvOTM2201AObj.initData->color);

}

/*
*******************************************************************************
  Function: void DRV_GFX_OTM2201A_BarFill(
                                                uint16_t left,
                                                uint16_t top,
                                                uint16_t right,
                                                uint16_t bottom
                                                )

  Summary:
    outputs count number of pixels into the frame buffer from the given x,y
    coordinate.

  Description:
    outputs count number of pixels into the frame buffer from the given x,y
    coordinate.

  Input:
 
        left           - pixel coordinate on x axis
        top           - pixel coordinate on y axis
        right       - pixel coordinate on x axis
        bottom   - pixel coordinate on y axis

  Output:

    DRV_OTM2201A_ERROR_QUEUE_FULL   - OTM2201A command queue is full
    DRV_OTM2201A_ERROR_NO_ERROR     - Success without any error

*******************************************************************************
*/
void  DRV_GFX_OTM2201A_BarFill(uint16_t left,
                                     uint16_t top,
                                     uint16_t right,
                                     uint16_t bottom)
{
    uint16_t count = right - left + 1;
    uint16_t lineCount = bottom - top + 1;

    uint16_t _y = 0;
    uint16_t tempCount = count;
    static uint32_t address                 = 0;

    OTM2201AqueueIndex->address     = _PixelStartAddress(left, top,
                                    drvOTM2201AObj.initData->orientation);

    while (lineCount) {
        address = _PixelAddressUpdate((OTM2201AqueueIndex->address & 0x00FF),
                ((OTM2201AqueueIndex->address & 0xFF00) >> 8),
                _y,
                drvOTM2201AObj.initData->orientation);

        while (DRV_GFX_OTM2201A_AddressSet(address));

        DisplaySetCommand(); // set RS line to low for command
        GFX_PMP_DeviceWrite(0x00);
        GFX_PMP_DeviceWrite(REG_GRAM_DATA);
        DisplaySetData(); // set RS line to low for command

        while (count) {
            GFX_PMP_DeviceWrite(drvOTM2201AObj.initData->color >> 8);
            GFX_PMP_DeviceWrite(drvOTM2201AObj.initData->color);
            count--;
        }
        
        count = tempCount;
        lineCount--;
        _y++;
    }
}

/*
*******************************************************************************
  Function: void  DRV_GFX_OTM2201A_PixelArrayPut( 
                                                      uint16_t *color,
                                                      uint16_t x,
                                                      uint16_t y,
                                                      uint16_t count,
                                                      uint16_t lineCount
                                                    )

  Summary:
    outputs an array of pixels of length count starting at *color

  Description:
    outputs an array of pixels of length count starting at *color

  Input:
        color       - pointer to array of color of pixels
        x           - pixel coordinate on x axis.
        y           - pixel coordinate on y axis.
        count       - count number of pixels
        lineCount   - lineCount number of display lines
 
  Output:
         handle - handle to the number of pixels remaining
         DRV_OTM2201A_ERROR_QUEUE_FULL   - OTM2201A command queue is full

*******************************************************************************
*/
void  DRV_GFX_OTM2201A_PixelArrayPut( 
                                            uint16_t *color,
                                            uint16_t x,
                                            uint16_t y,
                                            uint16_t count,
                                            uint16_t lineCount)
{
    uint16_t _y = 0;
    uint16_t tempCount = count;
    static uint32_t address = 0;


    OTM2201AqueueIndex->address     = _PixelStartAddress(x, y,
                                    drvOTM2201AObj.initData->orientation);

    while (lineCount) {
        address = _PixelAddressUpdate((OTM2201AqueueIndex->address & 0x00FF),
                ((OTM2201AqueueIndex->address & 0xFF00) >> 8),
                _y,
                drvOTM2201AObj.initData->orientation);

        if (DRV_GFX_OTM2201A_AddressSet(address)) {
            return;
        }

        DisplaySetCommand(); // set RS line to low for command
        GFX_PMP_DeviceWrite(0x00);
        GFX_PMP_DeviceWrite(REG_GRAM_DATA);
        DisplaySetData(); // set RS line to low for command

        while(count)
        {
            GFX_PMP_DeviceWrite(*color>>8);
            GFX_PMP_DeviceWrite(*color);
            color++;
            count--;
        }

        count = tempCount;
        lineCount--;
        _y++;
    }

    OTM2201AqueueIndex->count = 0;
    return;

}

/*
*******************************************************************************

  Function: uint16_t  DRV_GFX_OTM2201A_PixelArrayGet( 
 *                                                  uint16_t *color,
                                                    uint16_t x,
                                                    uint16_t y,
                                                    uint16_t count)

  Summary:
    gets an array of pixels of length count into an array starting at *color

  Description:
    gets an array of pixels of length count into an array starting at *color

  Input:
            color   - Pointer to array where color data is to be loaded
            x       - pixel coordinate on x axis
            y       - pixel coordinate on y axis
            count   - count number of pixels
  Output:
         DRV_OTM2201A_ERROR_QUEUE_FULL  - OTM2201A command queue is full
         DRV_OTM2201A_ERROR_NO_ERROR    - Success without any error

*******************************************************************************
*/

uint16_t*  DRV_GFX_OTM2201A_PixelArrayGet(
                                        uint16_t *color,
                                        uint16_t x,
                                        uint16_t y,
                                        uint16_t count)
{
    //Not yet supported
    return(DRV_OTM2201A_ERROR_NO_ERROR);
}

/*
*******************************************************************************

  Function: uint16_t  DRV_GFX_OTM2201A_Busy(uint8_t instance)

  Summary:
    Returns non-zero value if LCD controller is busy
          (previous drawing operation is not completed).

  Description:
    Returns non-zero value if LCD controller is busy
          (previous drawing operation is not completed).

  Input:
          instance - driver instance
 
  Output:
         DRV_OTM2201A_ERROR_DEVICE_BUSY - Device is busy
         DRV_OTM2201A_ERROR_NO_ERROR    - Success, driver is not busy
 
*******************************************************************************
*/

uint16_t  DRV_GFX_OTM2201A_Busy(uint8_t instance)
{
    
   static uint16_t dummy = 0x00;

   if(DRV_GFX_OTM2201A_RegGet(REG_POWER_CTRL_2,(uint16_t*)&dummy))
        return(1);

   if(dummy == 1)
   {
      return(DRV_OTM2201A_ERROR_NO_ERROR);
   }
   else
   {
      return(DRV_OTM2201A_ERROR_DEVICE_BUSY);
   }
   
}

uint32_t _PixelStartAddress(uint16_t x, uint16_t y, uint16_t orientation)
{
    uint32_t pixelStartAddress = 0;
    
    switch(orientation)
    {
        case DEGREE_90:

            pixelStartAddress = (((drvOTM2201AObj.initData->horizontalResolution
                                 - x - 1) << 8) | y);

         break;

         case DEGREE_180:
            pixelStartAddress = (drvOTM2201AObj.initData->horizontalResolution 
                                 - x - 1) | 
                                 ((drvOTM2201AObj.initData->verticalResolution 
                                 - y - 1) << 8);
         break;

         case DEGREE_270:

            pixelStartAddress = ((drvOTM2201AObj.initData->horizontalResolution 
                                 - x - 1) << 8) |
                                 (drvOTM2201AObj.initData->verticalResolution 
                                 - y - 1);

         break;

         case DEGREE_0:
         default:

            pixelStartAddress = x | (y << 8);

         break;
    }
    
    return pixelStartAddress;
}

uint32_t _PixelAddressUpdate(uint16_t x, uint16_t y,
                            uint16_t lineCount, uint16_t orientation)
{
    uint32_t pixelAddressUpdate = 0;
    
    switch(orientation)
    {
        case DEGREE_90:

            pixelAddressUpdate = (x + lineCount) | (y << 8);

            break;

        case DEGREE_180:

            pixelAddressUpdate = x | ((y - lineCount) << 8);

            break;

        case DEGREE_270:

            pixelAddressUpdate = (x - lineCount) | (y << 8);

            break;

        case DEGREE_0:
        default:

            pixelAddressUpdate = x | ((y + lineCount) << 8);

            break;
    }
    
    return pixelAddressUpdate;
}

void _DelayMs(unsigned long int msDelay )
{
    if(msDelay)
    {
        uint32_t sysClk = SYS_CLK_FREQ;
        uint32_t t0;
        t0 = _CP0_GET_COUNT();
        while (_CP0_GET_COUNT() - t0 < (sysClk/2000)*msDelay);
    }
}
