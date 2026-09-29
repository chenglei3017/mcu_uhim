/*******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    drv_gfx_ili9488.c

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
#include "../drv_gfx_ili9488.h"
#include "framework/driver/pmp/drv_pmp_static.h"
#include "driver/gfx/gfx_common.h"

DRV_GFX_INTERFACE   s1dInterface;
GFX_COLOR           color;                 /*global color for the driver*/
uint8_t             instance = 0;          /*global instance for the driver*/

#define SET_RS_LOW SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A,PORTS_BIT_POS_3)
#define SET_RS_HIGH SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A,PORTS_BIT_POS_3)

#define SET_RESET_LOW SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G,PORTS_BIT_POS_15)
#define SET_RESET_HIGH SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G,PORTS_BIT_POS_15)

static void PanelWriteCommand(uint16_t command)
{
    SET_RS_LOW;
    DRV_PMP0_Write(command);
    SET_RS_HIGH;
}

static uint16_t PanelReadData(void)
{
    return DRV_PMP0_Read();
}

static void PanelWriteData(uint16_t data)
{
    DRV_PMP0_Write(data);
}

// on = 1 display_on, on = 0 display_off
void ILI9488Display(uint8_t on)
{
    if (1 == on)
        PanelWriteCommand(0x29); 
    else
        PanelWriteCommand(0x28);    
}

static void ILI9488PanelRowArea (uint16_t start, uint16_t end)
{
    PanelWriteCommand(0x2b);
    PanelWriteData((start>>8) & 0xff);
    PanelWriteData(start & 0xff);
    PanelWriteData((end>>8) & 0xff);
    PanelWriteData(end & 0xff);
}

void ILI9488PanelColumnArea (uint16_t start, uint16_t end)
{
    PanelWriteCommand(0x2a);
    PanelWriteData((start>>8) & 0xff);
    PanelWriteData(start & 0xff);
    PanelWriteData((end>>8) & 0xff);
    PanelWriteData(end & 0xff);
}

//GFX_COLOR transparentColor;
//transparentColor = GFX_RGBConvert(0xFF, 0x00, 0xCC); /*Transparent Color chosen for the application*/
//////uint32_t transparentColor = transparentColor;
#define GetSystemClock()    (SYS_CLK_FREQ)
 static void TMR_DelayMS( uint32_t TMR_DelayMS_in_ms )
 {
    uint32_t tWait = ( GetSystemClock() / 2000 ) * TMR_DelayMS_in_ms;
    uint32_t tStart = _CP0_GET_COUNT();
    while( ( _CP0_GET_COUNT() - tStart ) < tWait );
 }
 
// *****************************************************************************
/* ILI9488 Driver task states

  Summary
    Lists the different states that ILI9488 task routine can have.

  Description
    This enumeration lists the different states that ILI9488 task routine can have.

  Remarks:
    None.
*/
typedef enum
{
    /* Process queue */
    DRV_GFX_ILI9488_INITIALIZE_START,

    DRV_GFX_ILI9488_INITIALIZE_PWR_UP,

    DRV_GFX_ILI9488_INITIALIZE_SET_REGISTERS,

    /* GFX ILI9488 task initialization done */
    DRV_GFX_ILI9488_INITIALIZE_DONE,

} DRV_GFX_ILI9488_OBJECT_TASK;

// *****************************************************************************
/* GFX ILI9488 Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/
typedef struct _DRV_GFX_ILI9488_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* ILI9488 machine state */
    DRV_GFX_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    DRV_GFX_CLIENT_OBJ *                        pDrvILI9488ClientObj;

    /* State of the task */
    DRV_GFX_ILI9488_OBJECT_TASK                 task;
    
    DRV_GFX_INIT *                              initData;

    uint16_t      maxY;
    uint16_t      maxX;

} DRV_GFX_ILI9488_OBJ;

static DRV_GFX_ILI9488_OBJ        drvILI9488Obj;

static DRV_GFX_CLIENT_OBJ          drvILI9488Clients;

//Local Protoypes
void InitializeHardware();


// *****************************************************************************
/*
  Function: DRV_GFX_HANDLE DRV_GFX_ILI9488_Open( const SYS_MODULE_INDEX index,
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
DRV_GFX_HANDLE DRV_GFX_ILI9488_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)DRV_HANDLE_INVALID;

    /* Check if instance object is ready*/
    if(drvILI9488Obj.status != SYS_STATUS_READY)
    {
        /* The SSD1926 module should be ready */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: Was the driver initialized?");
    }
    else if(intent != DRV_IO_INTENT_EXCLUSIVE)
    {
        /* The driver only supports this mode */
//        SYS_DEBUG(0, "GFX_SSD1926 Driver: IO intent mode not supported");
    }
    else if(drvILI9488Obj.nClients > 0)
    {
        /* Driver supports exclusive open only */
//        SYS_DEBUG(0, "GFX_SSD1926 already opened once. Cannot open again");
    }
    else
    {
        client = &drvILI9488Clients;

        client->inUse = true;
        client->drvObj = &drvILI9488Obj;

        /* Increment the client number for the specific driver instance*/
        drvILI9488Obj.nClients++;
    }

    /* Return invalid handle */
    return ((DRV_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_ILI9488_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the graphics controller

  Description:
    This is closes the instance of the driver specified by handle.
*/
void DRV_GFX_ILI9488_Close( DRV_HANDLE handle )
{
    /* Start of local variable */
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)NULL;
    DRV_GFX_ILI9488_OBJ * drvObj = ( DRV_GFX_ILI9488_OBJ *)NULL;
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
            drvObj = ( DRV_GFX_ILI9488_OBJ *)client->drvObj;

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
     void DRV_GFX_ILI9488_MaxXGet(void)

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
uint16_t DRV_GFX_ILI9488_MaxXGet(void)
{
    return drvILI9488Obj.maxX;
}

// *****************************************************************************
/*
  Function:
     void DRV_GFX_ILI9488_MaxYGet(void)

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
uint16_t DRV_GFX_ILI9488_MaxYGet(void)
{
    return drvILI9488Obj.maxY;
}

/*********************************************************************
  Function:
     void DRV_GFX_ILI9488_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_ILI9488_InterfaceSet(DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )
{
    interface->BarFill = DRV_GFX_ILI9488_BarFill;
    interface->PixelArrayPut = DRV_GFX_ILI9488_PixelArrayPut;
    interface->PixelArrayGet = DRV_GFX_ILI9488_PixelArrayGet;
    interface->PixelPut = DRV_GFX_ILI9488_PixelPut;
    interface->ColorSet = DRV_GFX_ILI9488_SetColor;
    interface->InstanceSet = DRV_GFX_ILI9488_SetInstance;
    interface->MaxXGet = DRV_GFX_ILI9488_MaxXGet;
    interface->MaxYGet = DRV_GFX_ILI9488_MaxYGet;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_ILI9488_SetColor(GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:
  
  Output: none

*/

void DRV_GFX_ILI9488_SetColor(GFX_COLOR color)
{
    drvILI9488Obj.initData->color = color;
}


void DRV_GFX_ILI9488_SetInstance(uint8_t instance)
{
    instance = instance;
}

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
    1 - call not successful (PMP driver busy)
    0 - call successful
*/
SYS_MODULE_OBJ DRV_GFX_ILI9488_Initialize(const SYS_MODULE_INDEX   index,
                                           const SYS_MODULE_INIT    * const init)
{
    static uint8_t   state =0;
    static uint16_t  horizontalSize, verticalSize;

    /* Validate the driver index */
    if ( index >= GFX_CONFIG_NUMBER_OF_MODULES )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    DRV_GFX_ILI9488_OBJ *dObj = &drvILI9488Obj;

    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->state = SYS_STATUS_BUSY;
    dObj->initData = (DRV_GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = index;

    //uint16_t horizontalTiming = drvILI9488Obj.initData->horizontalPulseWidth+drvILI9488Obj.initData->horizontalFrontPorch+drvILI9488Obj.initData->horizontalBackPorch;
    //uint16_t verticalTiming = drvILI9488Obj.initData->verticalPulseWidth+drvILI9488Obj.initData->verticalFrontPorch+drvILI9488Obj.initData->verticalBackPorch;

    dObj->task = DRV_GFX_ILI9488_INITIALIZE_SET_REGISTERS;
    
    drvILI9488Obj.maxX = DISP_HOR_RESOLUTION-1;
    drvILI9488Obj.maxY = DISP_VER_RESOLUTION-1;    
    
    //-----------ILI9488 reset sequence-----------//
    SET_RESET_HIGH;
    TMR_DelayMS(1);
    SET_RESET_LOW;
    TMR_DelayMS(10);
    SET_RESET_HIGH;
    TMR_DelayMS(120);

    // 5.3.33. PGAMCTRL (Positive Gamma Control) (E0h)
    PanelWriteCommand(0xE0);
    PanelWriteData(0x00);
    PanelWriteData(0x04);
    PanelWriteData(0x0E);
    PanelWriteData(0x08);
    PanelWriteData(0x17);
    PanelWriteData(0x0A);
    PanelWriteData(0x40);
    PanelWriteData(0x79);
    PanelWriteData(0x4D);
    PanelWriteData(0x07);
    PanelWriteData(0x0E);
    PanelWriteData(0x0A);
    PanelWriteData(0x1A);
    PanelWriteData(0x1D);
    PanelWriteData(0x0F);

    PanelWriteCommand(0xE1);
    PanelWriteData(0x00);
    PanelWriteData(0x1B);
    PanelWriteData(0x1F);
    PanelWriteData(0x02);
    PanelWriteData(0x10);
    PanelWriteData(0x05);
    PanelWriteData(0x32);
    PanelWriteData(0x34);
    PanelWriteData(0x43);
    PanelWriteData(0x02);
    PanelWriteData(0x0A);
    PanelWriteData(0x09);
    PanelWriteData(0x33);
    PanelWriteData(0x37);
    PanelWriteData(0x0F);

    PanelWriteCommand(0xC0);
    PanelWriteData(0x18);
    PanelWriteData(0x16);

    PanelWriteCommand(0xC1);
    PanelWriteData(0x41);

    PanelWriteCommand(0xC5);
    PanelWriteData(0x00);
    PanelWriteData(0x22);
    PanelWriteData(0x80);

    PanelWriteCommand(0x36);
    PanelWriteData(0x08);

    PanelWriteCommand(0x3A);// Interface Mode Control
    PanelWriteData(0x55);


    PanelWriteCommand(0xB0);  //Interface Mode Control
    PanelWriteData(0x00);
    PanelWriteCommand(0xB1);   //Frame rate 70HZ
    PanelWriteData(0xB0);

    PanelWriteCommand(0xB4);
    PanelWriteData(0x02);

    PanelWriteCommand(0xB6); //RGB/MCU Interface Control
    PanelWriteData(0x02);
    PanelWriteData(0x22);
    PanelWriteData(0x3B);

    PanelWriteCommand(0xE9);
    PanelWriteData(0x00);

    PanelWriteCommand(0XF7);
    PanelWriteData(0xA9);
    PanelWriteData(0x51);
    PanelWriteData(0x2C);
    PanelWriteData(0x82);      

    PanelWriteCommand(0x11);
    TMR_DelayMS(120); 
    ILI9488Display(1);    
    
    //SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F,PORTS_BIT_POS_8); 
    
    if(drvILI9488Obj.initData->TCON_Init != NULL)
    {
       drvILI9488Obj.initData->TCON_Init();
    }

    TMR_DelayMS(200);

//    DRV_GFX_ILI9488_BrightnessSet(100);
    
//    drvILI9488Obj.initData->horizontalResolution = horizontalSize;
//    drvILI9488Obj.initData->verticalResolution = verticalSize;
    drvILI9488Obj.state = SYS_STATUS_READY;

    dObj->nClients = 0;
    dObj->status = SYS_STATUS_READY;

//    DRV_GFX_ILI9488_SetPage(ACTIVE_PAGE, 0);
//    DRV_GFX_ILI9488_SetPage(VISUAL_PAGE, 0);

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)dObj;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_ILI9488_PixelPut(uint16_t x, uint16_t y)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        instance - driver instance
        color - color to output
        x,y - pixel coordinates
  Output:
    1 - call not successful (ILI9488 driver busy)
    0 - call successful
*/
void DRV_GFX_ILI9488_PixelPut(uint16_t x, uint16_t y)
{
    ILI9488PanelRowArea(y,y+1);
    ILI9488PanelColumnArea(x,x+1);
    PanelWriteCommand(0x2c); 
    PanelWriteData(drvILI9488Obj.initData->color);
}  

// *****************************************************************************
/*
  Function: void DRV_GFX_ILI9488_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)

  Summary:
    outputs one pixel into the frame buffer at the x,y coordinate given

  Description:
    none

  Input:
        left,top - pixel coordinates
        right, bottom - pixel coordinates

  Output:
          1 - call not successful (ILI9488 driver busy)
          0 - call successful
*/
void  DRV_GFX_ILI9488_BarFill(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    uint32_t i;

    ILI9488PanelRowArea(top,bottom);
    ILI9488PanelColumnArea(left,right);
    PanelWriteCommand(0x2c);
    for (i=0;i<(right-left+1)*(bottom-top+1);i++){
        PanelWriteData(drvILI9488Obj.initData->color);
    }
}  

// *****************************************************************************
/*
  Function: uint16_t*  DRV_GFX_ILI9488_PixelArrayPut(uint16_t *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)

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
void  DRV_GFX_ILI9488_PixelArrayPut(GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count, uint16_t lineCount)
{
    uint32_t i;
    
    ILI9488PanelRowArea(y,y+lineCount-1);
    ILI9488PanelColumnArea(x,x+count-1);
//    ILI9488PanelRowArea(y,y+lineCount);
//    ILI9488PanelColumnArea(x,x+count);
    PanelWriteCommand(0x2c);
    for (i=0; i<count*lineCount; i++)
    {
        PanelWriteData(color[i]);
    }
}

uint16_t* DRV_GFX_ILI9488_PixelArrayGet(GFX_COLOR *color, uint16_t x, uint16_t y, uint16_t count)
{
    ILI9488PanelRowArea(y,y+1);
    ILI9488PanelColumnArea(x,x+1);
    PanelWriteCommand(0x2e); 
    color[0] = PanelReadData();
    
    return color;
}
