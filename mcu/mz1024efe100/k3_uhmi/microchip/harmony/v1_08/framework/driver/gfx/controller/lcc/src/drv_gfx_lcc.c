/*******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    drv_gfx_lcc.c

  Summary:
    Interface for the graphics library where the primitives are renderred and sent to the graphics controller
    either external framebuffer.

  Description:
    None
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

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
#include "driver/gfx/controller/lcc/drv_gfx_lcc.h"
#include <xc.h>
#include <sys/attribs.h>

#include "peripheral/pmp/plib_pmp.h"
#include "peripheral/tmr/plib_tmr.h"
#include "peripheral/ebi/plib_ebi.h"

void __attribute__((always_inline)) LCCRecoverData(void);
void __attribute__((always_inline)) LCCSaveData(void);

#ifdef DRV_GFX_CONFIG_LCC_PALETTE
    #include "gfx/gfx_palette.h"
    GFX_COLOR LUT[256];
#endif

#define PIXEL_DRAW_PER_DMA_TX          20
#define VER_BLANK                 (DISP_VER_PULSE_WIDTH+DISP_VER_BACK_PORCH+DISP_VER_FRONT_PORCH-1)

#define SRAM_ADDR_CS0  0xE0000000
#define EBI_ADDR_CS0   0xC0000000

volatile uint8_t DrawCount = 0;                          /* The current status of how many pixels have been drawn inbetween a DMA IR*/
volatile uint8_t overflowcount;                      /* The count for the amount of overflows that have happened in the PMP Adress*/

static SYS_DMA_CHANNEL_HANDLE dmaHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;

volatile uint8_t driverTaskActive = 0;
#if !defined(ADDR15)
#define PMP_ADDRESS_LINES 0xffff
#else
#define PMP_ADDRESS_LINES 0x7fff
#endif
uint16_t GraphicsFrame[DISP_HOR_RESOLUTION];


#if(GFX_CONFIG_COLOR_DEPTH == 16)
    #define PMP_DATA_LENGTH PMP_DATA_SIZE_16_BITS
#else //(COLOR_DEPTH == 8)
    #ifdef DRV_GFX_CONFIG_LCC_PALETTE
    #define PMP_DATA_LENGTH PMP_DATA_SIZE_16_BITS
    #else
    #define PMP_DATA_LENGTH PMP_DATA_SIZE_8_BITS
    #endif
#endif

uint16_t HBackPorch = (DISP_HOR_PULSE_WIDTH+DISP_HOR_BACK_PORCH);
uint16_t HFrontPorch = 1;

//PIP Variables (NULL at start)
uint16_t PipStartT = 0;
uint16_t PipStartL = 0;
uint16_t PipVLength = 0;
uint16_t PipHLength = 0;
static uint32_t PipX,PipY;
uint8_t GFXPIPPage=0;
uint16_t scroll,scrollLine,scrollPage = 0;
volatile uint16_t* _ebiBaseAddress = (uint16_t*)SRAM_ADDR_CS0;

volatile uint16_t _pixelRendercount;
volatile uint16_t* _framePoint;
volatile uint32_t _frameAddress;


// *****************************************************************************
/* GFX LCC Driver Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _DRV_GFX_LCC_OBJ
{
    /* Flag to indicate in use  */
    bool                                        inUse;

    /* Save the index of the driver */
    SYS_MODULE_INDEX                            drvIndex;

    /* LCC machine state */
    DRV_GFX_STATES                              state;

    /* Status of this driver instance */
    SYS_STATUS                                  status;

    /* Number of clients */
    uint32_t                                    nClients;

    /* Client of this driver */
    DRV_GFX_CLIENT_OBJ *                        pDrvLCCClientObj;

    DRV_GFX_INIT *                              initData;

    uint16_t      maxY;
    uint16_t      maxX;
	
	LCC_VSYNC_Callback_FnPtr                    vsync_cb;

} DRV_GFX_LCC_OBJ;

static DRV_GFX_LCC_OBJ        drvLCCObj;
static DRV_GFX_CLIENT_OBJ drvLCCClients;

// *****************************************************************************
/*
  Function: DRV_GFX_LCC_GetBuffer( void )

  Summary:
    returns address to the framebuffer

  Description:
    none

  Input:
    none

  Output:
    address to the framebuffer
*/

// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_LCC_Initialize(uint8_t instance)

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
SYS_MODULE_OBJ DRV_GFX_LCC_Initialize(const SYS_MODULE_INDEX index,
                                      const SYS_MODULE_INIT* const init)
{

    static uint16_t horizontalSize, verticalSize;

    /* Validate the driver index */
    if ( index >= GFX_CONFIG_NUMBER_OF_MODULES )
        return SYS_MODULE_OBJ_INVALID;

    DRV_GFX_LCC_OBJ *dObj = &drvLCCObj;

    /* Object is valid, set it in use */
    dObj->inUse = true;
    dObj->state = SYS_STATUS_BUSY;
    dObj->initData = (DRV_GFX_INIT *) init;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = index;
    drvLCCObj.initData->TCON_Init = TCON_MODULE;

    if ((drvLCCObj.initData->orientation == 90) || (drvLCCObj.initData->orientation == 270))
	{
        horizontalSize = drvLCCObj.initData->verticalResolution;
        verticalSize = drvLCCObj.initData->horizontalResolution;
        drvLCCObj.maxX = horizontalSize - 1;
        drvLCCObj.maxY = verticalSize - 1;
    }
	else
	{
        horizontalSize = drvLCCObj.initData->horizontalResolution;
        verticalSize = drvLCCObj.initData->verticalResolution;
        drvLCCObj.maxX = horizontalSize - 1;
        drvLCCObj.maxY = verticalSize - 1;
    }

    HSYNC_TRIS = 0;
    LCD_CS_TRIS = 0;
    VSYNC_TRIS = 0;
    LCD_RESET_TRIS = 0;

    BACKLIGHT_TRIS = 0;
    DATA_ENABLE_TRIS = 0;

    LCD_RESET = 1;
    LCD_CS = 1;

    if (drvLCCObj.initData->TCON_Init != NULL)
        drvLCCObj.initData->TCON_Init();

    drvLCCObj.initData->activePage = 0;
    drvLCCObj.initData->visualPage = 0;

    /* Disable the PMP module */
    PLIB_PMP_Disable(0);

    PLIB_PMP_OperationModeSelect(0, PMP_MASTER_READ_WRITE_STROBES_INDEPENDENT);

    /* pins polarity setting */
    PLIB_PMP_ReadWriteStrobePolaritySelect(0, 1 - (DISP_INV_LSHIFT));
    PLIB_PMP_WriteEnableStrobePolaritySelect(0, PMP_POLARITY_ACTIVE_LOW);

    PLIB_PMP_ReadWriteStrobePortEnable(0);
    PLIB_PMP_WriteEnableStrobePortEnable(0);

    PLIB_PMP_DataSizeSelect(0, PMP_DATA_LENGTH);

    /* wait states setting */
    PLIB_PMP_WaitStatesDataHoldSelect(0, PMP_DATA_HOLD_1);
    PLIB_PMP_WaitStatesDataSetUpSelect(0, PMP_DATA_WAIT_ONE);
    PLIB_PMP_WaitStatesStrobeSelect(0, PMP_STROBE_WAIT_3);

    /* setting the hardware for the required interrupt mode */
    PLIB_PMP_InterruptModeSelect(0, PMP_INTERRUPT_EVERY_RW_CYCLE);

    PLIB_PMP_AddressIncrementModeSelect(0, PMP_ADDRESS_AUTO_INCREMENT);

    /* Enable the PMP module */
    PLIB_PMP_Enable(0);
    /* Ports Setting */
    PLIB_PMP_AddressPortEnable(0, PMP_ADDRESS_LINES);
    PLIB_PMP_AddressSet(0, 0);

    SRAM_TRIS = 0;
#ifdef ADDR15_TRIS
    ADDR15_TRIS = 0;
#endif
    ADDR16_TRIS = 0;
    ADDR17_TRIS = 0;
    ADDR18_TRIS = 0;

    SRAM_CS = 0;
    ADDR17 = 0;
    ADDR18 = 0;

#if defined(__PIC32MZ__)
    PLIB_EBI_WriteOutputControlSet(EBI_ID_0, true, false);
    PLIB_EBI_DataEnableSet(EBI_ID_0, true, true);
    PLIB_EBI_BaseAddressSet(EBI_ID_0, 0, 0x20000000);
    PLIB_EBI_MemoryCharacteristicsSet(EBI_ID_0, 0, SRAM, MEMORY_SIZE_8MB, CS_TIMING_0);
    PLIB_EBI_MemoryTimingConfigSet(EBI_ID_0, 0, 0, 0, 0, 0, 0, 1);
    PLIB_EBI_StaticMemoryWidthRegisterSet(EBI_ID_0, 0, MEMORY_WIDTH_16BIT);
    PLIB_EBI_FlashPowerDownModeSet(EBI_ID_0, true);
#endif

    /*Turn Backlight on*/
    BACKLIGHT = BACKLIGHT_ENABLE_LEVEL;

    drvLCCObj.initData->horizontalResolution = horizontalSize;
    drvLCCObj.initData->verticalResolution = verticalSize;
    drvLCCObj.state = SYS_STATUS_READY;

    dObj->nClients = 0;
    dObj->status = SYS_STATUS_READY;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)dObj;
}

static int DRV_GFX_LCC_Start()
{
	/*Suspend DMA Module*/
    SYS_DMA_Suspend();
	
    /* Allocate DMA channel */
    dmaHandle = SYS_DMA_ChannelAllocate(DRV_GFX_LCC_DMA_CHANNEL_INDEX);
    
	if(dmaHandle != SYS_DMA_CHANNEL_HANDLE_INVALID)
        return -1;
		
    // set the transfer parameters: source & destination address, source & destination size, number of bytes per event  
    // set the transfer parameters: source & destination address,
    // source & destination size, number of bytes per event
    SYS_DMA_ChannelTransferAdd(dmaHandle, (void *) &PMDIN, 2, &GraphicsFrame[0],
            HBackPorch, DRV_GFX_LCC_DMA_TRANSFER_LENGTH);

    /* Enable the transfer done interrupt, when all buffer transferred */
    PLIB_DMA_ChannelXINTSourceEnable(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX,
            DMA_INT_BLOCK_TRANSFER_COMPLETE);

    SYS_INT_SourceEnable(INT_SOURCE_DMA_0 + DRV_GFX_LCC_DMA_CHANNEL_INDEX);

    // set the transfer event control: what event is to start the DMA transfer
    SYS_DMA_ChannelSetup(dmaHandle,
            SYS_DMA_CHANNEL_OP_MODE_BASIC,
            DRV_GFX_LCC_DMA_TRIGGER_SOURCE);

    PLIB_TMR_PrescaleSelect(DRV_GFX_LCC_TMR_INDEX, 0);
    PLIB_TMR_Period16BitSet(DRV_GFX_LCC_TMR_INDEX, 1);
    PLIB_TMR_Start(DRV_GFX_LCC_TMR_INDEX);

    // once we configured the DMA channel we can enable it
    SYS_DMA_ChannelEnable(dmaHandle);

    /*Unsuspend DMA Module*/
    SYS_DMA_Resume();
	
	return 0;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_LCC_VSYNC_CallbackSet(LCC_VSYNC_Callback_FnPtr cb)

  Summary:
    sets VSYNC state callback pointer

  Description:
    none

  Input:
        cb - callback pointer
*/
void DRV_GFX_LCC_VSYNC_CallbackSet(LCC_VSYNC_Callback_FnPtr cb)
{
	drvLCCObj.vsync_cb = cb;
}

// *****************************************************************************
/*
  Function: LCC_VSYNC_STATE DRV_GFX_LCC_VSYNC_GetState(void)

  Summary:
    Gets the state of the current VSYNC mode

  Description:
    none

  Output:
        state of VSYNC register
		LCC_VSYNC_TRUE = in VSYNC
		LCC_VSYNC_FALSE = not in VSYNC
*/
LCC_VSYNC_STATE DRV_GFX_LCC_VSYNC_GetState(void)
{
	return VSYNC;
}

// *****************************************************************************
/*
  Function: DRV_GFX_LCC_Open(uint8_t instance)

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
DRV_GFX_HANDLE DRV_GFX_LCC_Open( const SYS_MODULE_INDEX index,
                             const DRV_IO_INTENT intent )
{
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)DRV_HANDLE_INVALID;

    /* Check if the specified driver index is in valid range */
    if(index >= GFX_CONFIG_NUMBER_OF_MODULES)
    {
    }
    /* Check if instance object is ready*/
    else if(drvLCCObj.status != SYS_STATUS_READY)
    {
    }
    else if(intent != DRV_IO_INTENT_EXCLUSIVE)
    {
    }
    else if(drvLCCObj.nClients > 0)
    {
    }
    else
    {
		if(DRV_GFX_LCC_Start() != 0)
            return NULL; // currently single context only
			
        client = &drvLCCClients;

        client->inUse = true;
        client->drvObj = &drvLCCObj;

        /* Increment the client number for the specific driver instance*/
        drvLCCObj.nClients++;
    }

    /* Return invalid handle */
    return ((DRV_HANDLE)client);
}

// *****************************************************************************
/* Function:
    void DRV_GFX_LCC_Close( DRV_HANDLE handle )

  Summary:
    closes an instance of the graphics controller

  Description:
    This is closes the instance of the driver specified by handle.
*/
void DRV_GFX_LCC_Close( DRV_HANDLE handle )
{
    /* Start of local variable */
    DRV_GFX_CLIENT_OBJ * client = (DRV_GFX_CLIENT_OBJ *)NULL;
    DRV_GFX_LCC_OBJ * drvObj = (DRV_GFX_LCC_OBJ *)NULL;
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
            drvObj = (DRV_GFX_LCC_OBJ *)client->drvObj;

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
     void DRV_GFX_LCC_MaxXGet()

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
uint16_t DRV_GFX_LCC_MaxXGet()
{
    return drvLCCObj.maxX;
}

// *****************************************************************************
/*
  Function:
     void DRV_GFX_LCC_MaxYGet()

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
uint16_t DRV_GFX_LCC_MaxYGet()
{
    return drvLCCObj.maxY;
}

/*********************************************************************
  Function:
     void DRV_GFX_LCC_InterfaceSet( DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )

  Summary:
    Returns the API of the graphics controller

  Description:
    none

  Return:

  *********************************************************************/
void DRV_GFX_LCC_InterfaceSet( DRV_HANDLE handle, DRV_GFX_INTERFACE * interface )
{
    interface->BarFill = DRV_GFX_LCC_BarFill;
    interface->PixelArrayPut = DRV_GFX_LCC_PixelArrayPut;
    interface->PixelArrayGet = DRV_GFX_LCC_PixelArrayGet;
    interface->PixelPut = DRV_GFX_LCC_PixelPut;
    interface->ColorSet = DRV_GFX_LCC_SetColor;
    interface->PageSet = DRV_GFX_LCC_SetPage;
    interface->Layer = DRV_GFX_LCC_Layer;
    interface->MaxXGet = DRV_GFX_LCC_MaxXGet;
    interface->MaxYGet = DRV_GFX_LCC_MaxYGet;
}

uint16_t DRV_GFX_LCC_SetPage(uint8_t pageType, uint8_t page)
{

        switch(pageType)
        {
            case ACTIVE_PAGE:
                drvLCCObj.initData->activePage = page;
                _ebiBaseAddress = (uint16_t*)SRAM_ADDR_CS0 + (drvLCCObj.initData->activePage << 17);
                break;
            case VISUAL_PAGE:
                drvLCCObj.initData->visualPage = page;
                ADDR17 = page;
                ADDR18 = page>>1;
                break;
        }

        return(0);
}
// *****************************************************************************
/*
  Function: void DRV_GFX_LCC_SetColor(uint8_t instance, GFX_COLOR color)

  Summary: Sets the color for the driver instance

  Description:
  
  Output: none

*/

void DRV_GFX_LCC_SetColor(GFX_COLOR color)
{
  drvLCCObj.initData->color = color;
}

// *****************************************************************************
/*
  Function: void DRV_GFX_LCC_SetInstance(uint8_t instance)

  Summary: Sets the instance for the driver

  Description:
  
  Output: none

*/
volatile short pixelX;
volatile GFX_COLOR pixelColor;
volatile uint16_t pixelCount = 0;
volatile uint8_t pixelUpdate = 0;

void DRV_GFX_LCC_DisplayRefresh()
{
    static uint8_t GraphicsState = ACTIVE_PERIOD;
    static uint16_t remaining = 0;
    static uint16_t remainingPixels = 0;
    static short line = 0;
    static uint32_t pixelAddress = 0;
    static uint8_t prevGraphicsState = BLANKING_PERIOD;
    static uint16_t pipLine = 0;

    DrawCount = 0;

    #if defined(__32MX460F512L__) || defined (__32MX360F512L__)
        static uint16_t dmatransfersremaining = 0;

        if(dmatransfersremaining != 0)
        {

            if(dmatransfersremaining > 254)
             {
                   //Setup DMA Transfer
                   PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, 254);
                   dmatransfersremaining -= 254;
             }
             else
             {
                   //Setup DMA Transfer
                   PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, dmatransfersremaining);
                   dmatransfersremaining -= dmatransfersremaining;
             }

         PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, DMA_INT_BLOCK_TRANSFER_COMPLETE);
         PLIB_DMA_ChannelXEnable(DMA_ID_0,DRV_GFX_LCC_DMA_CHANNEL_INDEX);
         return;
        }
    #endif

    switch(GraphicsState)
    {

        case ACTIVE_PERIOD:
            remaining = DISP_HOR_RESOLUTION;
            GraphicsState = BLANKING_PERIOD;

            if(line >= 0)
            {
                PMADDR = ((PMP_ADDRESS_LINES) & ((line) * DISP_HOR_RESOLUTION));

                if((line) == (DISP_VER_RESOLUTION))
                {
					// notify registered callback that the display is in vsync mode
					if(drvLCCObj.vsync_cb != NULL)
						drvLCCObj.vsync_cb();
						
                    VSYNC =0;
                    line = (-VER_BLANK);
                    #ifdef ADDR15
                    ADDR15 = 0;          //count holds the additional address line count
                    #endif
                    ADDR16 = 0;
                    overflowcount = 0;
                    pipLine = 0;
               }
               else
               {
                   DATA_ENABLE = 1;

                   if((line >= PipStartT)&&(line <= (PipStartT + PipVLength))&&(PipVLength != 0))
                   {
                       overflowcount = ((line) * DISP_HOR_RESOLUTION)/PMADDR_OVERFLOW;
                       GraphicsState = PIP;

                       if(!PipStartL)
                       {
                           //Draw PIP Line
      case PIP:            ADDR17 = GFXPIPPage;
                           ADDR18 = GFXPIPPage>>1;

                           pixelAddress = (uint32_t)(((PipY+pipLine++)*(DISP_HOR_RESOLUTION))+(PipX));

                           PMADDR = pixelAddress&(PMP_ADDRESS_LINES);

                           overflowcount = pixelAddress/PMADDR_OVERFLOW;
                           remaining = PipHLength;

                           remainingPixels = DISP_HOR_RESOLUTION - remaining - PipStartL;
                           pixelAddress = ((line*DISP_HOR_RESOLUTION) + remaining + PipStartL);
 
                           GraphicsState = FINISH_LINE;
                        }
                       else
                       {
                           remaining = PipStartL;
                       }

                        #ifdef ADDR15_TRIS
                        ADDR15 = overflowcount;          //count holds the additional address line count
                        ADDR16 = (overflowcount>>1);
                        #else
                        ADDR16 = overflowcount;
                        #endif
                 }

                 if(scroll > 0)
                 {

                     switch(scroll)
                     {
                         case 1:             //Up
                         case 2:             //Down
                            if(line < scrollLine)
                            {
                                pixelAddress = ((drvLCCObj.initData->verticalResolution-1)-(scrollLine-line)) * DISP_HOR_RESOLUTION;
                                ADDR17 = drvLCCObj.initData->visualPage;
                                ADDR18 = drvLCCObj.initData->visualPage>>1;
                            }
                            else
                            {
                                pixelAddress = (line-scrollLine) * DISP_HOR_RESOLUTION;
                                ADDR17 = scrollPage;
                                ADDR18 = scrollPage>>1;
                            }

                            PMADDR = (pixelAddress)&(PMP_ADDRESS_LINES);
                            overflowcount = pixelAddress/PMADDR_OVERFLOW;
                            break;

                        case 3://Left
                        case 4://Right
                            pixelAddress = ((line+1) * DISP_HOR_RESOLUTION) + ((drvLCCObj.initData->horizontalResolution-1)-scrollLine);
                   
                            PMADDR = (pixelAddress)&(PMP_ADDRESS_LINES);
                            overflowcount = pixelAddress/PMADDR_OVERFLOW;

                            if(scroll == 3)
                            {
                                ADDR17 = drvLCCObj.initData->visualPage;
                                ADDR18 = drvLCCObj.initData->visualPage>>1;
                            }
                            else
                            {
                                ADDR17 = scrollPage;
                                ADDR18 = scrollPage>>1;
                            }

                            remaining = scrollLine;
                            remainingPixels = DISP_HOR_RESOLUTION - scrollLine;
                            pixelAddress = (line) * DISP_HOR_RESOLUTION;
                            GraphicsState = FINISH_LINE;
                            break;
                    }

                  if((scroll > 2) && (GraphicsState != FINISH_LINE))
                  {
        case FINISH_LINE:            //Finish Line Render
                      remaining = remainingPixels;
                   
                      PMADDR = (pixelAddress)&(PMP_ADDRESS_LINES);
                      overflowcount = pixelAddress/PMADDR_OVERFLOW;

                      if(scroll == 3)
                      {
                          ADDR17 = scrollPage;
                          ADDR18 = scrollPage>>1;
                      }
                      else
                      {
                          ADDR17 = drvLCCObj.initData->visualPage;
                          ADDR18 = drvLCCObj.initData->visualPage>>1;
                      }
 
                      GraphicsState = BLANKING_PERIOD;
                  }

                  #ifdef ADDR15_TRIS
                  ADDR15 = overflowcount;          //count holds the additional address line count
                  ADDR16 = (overflowcount>>1);
                  #else
                  ADDR16 = overflowcount;
                  #endif
              }

              prevGraphicsState = GraphicsState;

              if((PMADDR_OVERFLOW - PMADDR) < (remaining))       
              {   
                  GraphicsState = OVERFLOW;      //Do Overflow routine
                  PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, (uint16_t)(((PMADDR_OVERFLOW)- PMADDR)<<1));
                  PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, DMA_INT_BLOCK_TRANSFER_COMPLETE);
                  PLIB_DMA_ChannelXEnable(DMA_ID_0,DRV_GFX_LCC_DMA_CHANNEL_INDEX);
                  remaining = remaining - ((PMADDR_OVERFLOW)- PMADDR);
                  return;
              }           
          }
      }
        break;

       case OVERFLOW:                    //Adjust the address lines that aren't part of PMP
             GraphicsState = prevGraphicsState;     //goto Front Porch
             #ifdef ADDR15_TRIS
             ADDR15 = ++overflowcount;          //count holds the additional address line count
             ADDR16 = (overflowcount>>1);
             #else
             ADDR16 = ++overflowcount;
             #endif
             break;

        case BLANKING_PERIOD:   //Front Porch then Back Porch Start 
            HSYNC = 0;
            DATA_ENABLE = 0;
            GraphicsState = PMDIN;
            while(PMMODEbits.BUSY == 1);
            HSYNC = 1;
            VSYNC = 1;

            //Setup DMA Back Porch
            remaining = HBackPorch;
            GraphicsState = ACTIVE_PERIOD;   
            line++;      

        default:
            break;
    }


    //Setup DMA Transfer
    #if defined(__32MX460F512L__) || defined (__32MX360F512L__)
    if((remaining) > 254)
    {
        dmatransfersremaining = (remaining ) - 254;
        remaining = (254 >> 1);
    }
    #endif

    PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, (uint16_t) (remaining << 1));
    PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0, DRV_GFX_LCC_DMA_CHANNEL_INDEX, DMA_INT_BLOCK_TRANSFER_COMPLETE);
    SYS_DMA_ChannelEnable(dmaHandle);

}
  
// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_LCC_PixelPut(short x, short y)

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
uint16_t DRV_GFX_LCC_PixelPut(short x, short y)
{

    #if (DISP_ORIENTATION == 270)
    uint16_t tempx=x;
    x = (DISP_HOR_RESOLUTION-1) - y;
    y = tempx; 
    #elif (DISP_ORIENTATION == 90)
    uint16_t tempy=y;
    y = (DISP_VER_RESOLUTION-1) - x;
    x = tempy; 
    #elif (DISP_ORIENTATION == 180)
    x = (DISP_HOR_RESOLUTION-1) - x;
    y = (DISP_VER_RESOLUTION-1) - y;
    #endif

    #if !defined(__PIC32MZ__)
    _frameAddress = (uint32_t)(((y)*(DISP_HOR_RESOLUTION))+(x));
    #else
    _framePoint = (uint16_t*)_ebiBaseAddress + (((y)*(DISP_HOR_RESOLUTION))+(x));
    #endif

    LCCSaveData();

    #if !defined(__PIC32MZ__)
    PMDIN = drvLCCObj.initData->color;
    #else
    *_framePoint++ = drvLCCObj.initData->color;
    #endif

    LCCRecoverData();

    return(0);
}  

// *****************************************************************************
/*
  Function: uint16_t DRV_GFX_LCC_BarFill(short left, short top, short right, short bottom)

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
uint16_t  DRV_GFX_LCC_BarFill(short left, short top, short right, short bottom)
{

#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
    uint16_t lineCount = bottom - top + 1;//vchange;
    uint16_t count = right - left + 1;//hchange;
    #elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
    uint16_t lineCount = right-left + 1;//hchange;
    uint16_t count = bottom - top + 1;//vchange;
    #endif

    #if (DISP_ORIENTATION == 270)
    uint16_t tempx = left;
    left = (DISP_HOR_RESOLUTION) - top - count;
    top = tempx;
    #elif (DISP_ORIENTATION == 90)
    uint16_t tempy = top;
    top = (DISP_VER_RESOLUTION) - left - lineCount;
    left = tempy;
    #elif (DISP_ORIENTATION == 180)
    top = (DISP_VER_RESOLUTION) - top - lineCount;
    left =  (DISP_HOR_RESOLUTION) - left - count;
    #endif

    _frameAddress = (uint32_t)(((top)*(DISP_HOR_RESOLUTION))+(left));

    while(lineCount)
    {

       #if defined(__PIC32MZ__)
       _framePoint = (uint16_t*)_ebiBaseAddress + (_frameAddress);
       #endif

       for(_pixelRendercount = count; _pixelRendercount > 0; _pixelRendercount--)
       {
          /*Suspend DMA Trigger*/
          LCCSaveData();

    #if !defined(__PIC32MZ__)
    if((_pixelRendercount > 8) && (PMADDR < PMP_ADDRESS_LINES-8)) //Write 8 pixels at once
    {
        PMDIN = drvLCCObj.initData->color; while(PMMODEbits.BUSY == 1);
        PMDINSET = 0; while(PMMODEbits.BUSY == 1);
        PMDINSET = 0; while(PMMODEbits.BUSY == 1);
        PMDINSET = 0; while(PMMODEbits.BUSY == 1);
        PMDINSET = 0;
        _frameAddress += 7;
        PMDINSET = 0;
        _pixelRendercount -= 7;
        PMDINSET = 0; while(PMMODEbits.BUSY == 1);
        PMDINSET = 0;
    }
    else
    {
       PMDIN = drvLCCObj.initData->color;
    }
        #else
        if(_pixelRendercount > 8) //Write 8 pixels at once
        {
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            *_framePoint++ = drvLCCObj.initData->color;
            _pixelRendercount -= 7;
            _frameAddress += 7;
        }

        *_framePoint++ = drvLCCObj.initData->color;
        #endif

        LCCRecoverData();
        _frameAddress++;

       }

       _frameAddress += (DISP_HOR_RESOLUTION - count);
       lineCount--;
    }

    return(0);
}  

// *****************************************************************************
/*
  Function: void  DRV_GFX_LCC_PixelArrayPut(uint16_t *color, short x, short y, uint16_t count)

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
void  DRV_GFX_LCC_PixelArrayPut(GFX_COLOR *color, short x, short y, uint16_t count, uint16_t lineCount)
{

       #if (DISP_ORIENTATION == 270)
       #elif (DISP_ORIENTATION == 90)
       uint16_t hchange = count;
       uint16_t tempy = y;
       y = (DISP_HOR_RESOLUTION-2) - (x);
       x = tempy;
       count = lineCount;
       lineCount = hchange;
       #elif (DISP_ORIENTATION == 180)
       #endif

    _frameAddress = (uint32_t)(((y)*(DISP_HOR_RESOLUTION))+(x));

    while(lineCount)
    {
       #if defined(__PIC32MZ__)
       _framePoint = (uint16_t*)_ebiBaseAddress + (_frameAddress);
       #endif

       for(_pixelRendercount = count; _pixelRendercount > 0; _pixelRendercount--)
       {
          /*Suspend DMA Trigger*/
          LCCSaveData();

        #if !defined(__PIC32MZ__)
        if((_pixelRendercount > 4) && (PMADDR < PMP_ADDRESS_LINES-8)) //Write 8 pixels at once
        {
            PMDIN = *color++;
            _pixelRendercount -= 3;
            PMDIN = *color++;
            _frameAddress += 3;
            PMDIN = *color++;
            while(PMMODEbits.BUSY==1);
        }

        PMDIN =  *color++;

        #else
        if(_pixelRendercount > 8) //Write 8 pixels at once
        {
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        *_framePoint++ = *color++;
        _pixelRendercount -= 7;
        _frameAddress += 7;
        }

        *_framePoint++ = *color++;
        #endif

        LCCRecoverData();

        _frameAddress++;

       }

       _frameAddress += (DISP_HOR_RESOLUTION - count);
       lineCount--;
    }
} 

// *****************************************************************************
/*
  Function: uint16_t*  DRV_GFX_LCC_PixelArrayGet(uint16_t *color, short x, short y, uint16_t count)

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
void  DRV_GFX_LCC_PixelArrayGet(GFX_COLOR *color, short x, short y, uint16_t count)
{

    #if (DISP_ORIENTATION == 270)
    uint16_t tempx = x;
    x = (DISP_HOR_RESOLUTION-1) - y;
    y = tempx; 
    #elif (DISP_ORIENTATION == 90)
    uint16_t tempy = y;
    y = (DISP_VER_RESOLUTION-1) - x;
    x = tempy; 
    #elif (DISP_ORIENTATION == 180)
    x = (DISP_HOR_RESOLUTION-1) - x;
    y = (DISP_VER_RESOLUTION-1) - y;
    #endif

     _frameAddress =  (uint32_t)(((y)*(DISP_HOR_RESOLUTION))+(x));

               #if defined(__PIC32MZ__)
              _framePoint = (uint16_t*)_ebiBaseAddress + (_frameAddress);
              #endif

              while(count)
              {
                /*Suspend DMA Trigger*/
                LCCSaveData();

                #if !defined(__PIC32MZ__)
                PMCONCLR = 0x0100;

                *color = PMDIN;
                 while(PMMODEbits.BUSY==1);
                *color++ = PMDIN;

                 if((count > 4) && (PMADDR < PMP_ADDRESS_LINES-8)) //Write 8 pixels at once
                 {
                     *color++ = PMDIN;
                     _frameAddress += 3;
                     *color++ = PMDIN;
                     count -= 3;
                     *color++ = PMDIN;

                  }

                  PMCONSET = 0x0100;
                  #else
                    if(count > 4) // Read 4 pixels at once
                    {
                        *color++ = *_framePoint++;
                        count -= 3;
                        *color++ = *_framePoint++;
                        _frameAddress += 3;
                        *color++ = *_framePoint++;
                    }
                    *color++ = *_framePoint++;
                    #endif

                LCCRecoverData();
                
                _frameAddress++;
                count--;
              }
}

void DRV_GFX_LCC_Layer(uint8_t instance, GFX_LAYER_PARAMS* layer)
{

    GFXPIPPage = layer->page;

   if(layer->on == 0)
   {    
       PipVLength = 0;
       return;
   }

   #if (DISP_ORIENTATION == 90)
     PipStartL = (layer->top);
     PipStartT = (DISP_VER_RESOLUTION-1)-(layer->left + layer->width);
     PipVLength = layer->width;
     PipHLength = layer->height;
     PipY = (DISP_VER_RESOLUTION-1) - (layer->layerLeft+layer->width);
     PipX = (layer->layerTop);
   #elif (DISP_ORIENTATION == 0)
    PipStartL = layer->left;
    PipStartT = layer->top;   
    PipVLength = layer->height;
    PipHLength = layer->width;
    PipX = layer->layerLeft;
    PipY = layer->layerTop;
   #endif
}

 uint16_t prevaddr;

void __attribute__((always_inline)) LCCSaveData(void)
{

    while(DrawCount > PIXEL_DRAW_PER_DMA_TX);

    #if !defined(__PIC32MZ__)
    DMACONbits.SUSPEND = 1;
    while(PMMODEbits.BUSY == 1); //WAIT for DMA transfer to be suspended
    //Perform Write
    #if defined(ADDR15)
    ADDR15 = _frameAddress >> 15; //.bits.b15;
    #endif
    ADDR16 = _frameAddress >> 16; //taskIndex->address.bits.b16;
    ADDR17 = drvLCCObj.initData->activePage;
    ADDR18 = drvLCCObj.initData->activePage>>1;

    //Save previous address value
    prevaddr = PMADDR;
    PMADDR = _frameAddress;
    #else
    T2CONCLR = 0x8000;
    while(PMMODEbits.BUSY == 1); //WAIT for DMA transfer to be suspended
    CFGEBIA = 0x0007ffff;
    CFGEBIC = 0x00002003;
    #endif

}

void __attribute__((always_inline)) LCCRecoverData(void)
{

    #if !defined(__PIC32MZ__)
    //Clean-up Address Lines
    #if defined(ADDR15)
    ADDR15 = overflowcount;          //count holds the additional address line count
    ADDR16 = (overflowcount>>1);
    #else
    ADDR16 = overflowcount;
    #endif
    PMADDR = prevaddr;
    ADDR17 = drvLCCObj.initData->visualPage;
    ADDR18 = drvLCCObj.initData->visualPage>>1;
    DMACONbits.SUSPEND = 0;
    #else
    CFGEBIA = 0;
    CFGEBIC = 0;
    T2CONSET = 0x8000;
    #endif

    DrawCount++;

}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_GFX_LCC_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific module instance of the Driver module.

  Description:
    This function returns the status of the specific module instance disabling its
    operation (and any hardware for driver modules).

  PreCondition:
    The DRV_GFX_LCC_Initialize function should have been called before calling
    this function.

  Parameters:
    object          - DRV_GFX_LCC object handle, returned from DRV_GFX_LCC_Initialize

  Returns:
    SYS_STATUS_READY    Indicates that any previous module operation for the
                        specified module has completed

    SYS_STATUS_BUSY     Indicates that a previous module operation for the
                        specified module has not yet completed

    SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS DRV_GFX_LCC_Status ( SYS_MODULE_OBJ object )
{
    DRV_GFX_LCC_OBJ *dObj = (DRV_GFX_LCC_OBJ*)object;
    return ( dObj->state );

} /* SYS_TMR_Status */

#ifdef DRV_GFX_CONFIG_LCC_PALETTE
uint8_t DRV_GFX_PaletteSet(GFX_COLOR *pPaletteEntry, uint16_t startEntry, uint16_t length)
{
    uint16_t counter;

    if((pPaletteEntry == NULL) || ((startEntry + length) > 256))
    {
        return -1;
    }

    for(counter = 0; counter < length; counter++)
    {

        LUT[counter]  = *pPaletteEntry;

    }

    return 0;
}
#endif
