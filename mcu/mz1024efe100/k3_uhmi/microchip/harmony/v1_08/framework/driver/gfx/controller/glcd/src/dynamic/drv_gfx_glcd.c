/********************************************************************************
  GFX GLCD Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd.c

  Summary:
    Source code for the GFX GLCD driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
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

#include "driver/gfx/controller/glcd/src/drv_gfx_glcd_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
DRV_GFX_GLCD_OBJ gDrvGfxGLCDObj[DRV_GFX_GLCD_INSTANCES_NUMBER] ;

/* This object maintains data that is required by all GFX GLCD
   driver instances. */
DRV_GFX_GLCD_COMMON_DATA_OBJ gDrvGfxGLCDCommonDataObj;

// *****************************************************************************
// *****************************************************************************
// Section: GFX GLCD Driver Interface Implementations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_Initialize system interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_Initialize system 
    interface function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

SYS_MODULE_OBJ DRV_GFX_GLCD_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    DRV_GFX_GLCD_OBJ  *dObj = (DRV_GFX_GLCD_OBJ*)NULL;
    DRV_GFX_GLCD_INIT *glcdInit = NULL ;
    
    /* Check if the specified driver index is in valid range */
    if(drvIndex >= DRV_GFX_GLCD_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid driver index");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Check if this hardware instance was already initialized */
    if(gDrvGfxGLCDObj[drvIndex].inUse != false)
    {
        SYS_DEBUG(0, "Instance already in use");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Assign to the local pointer the init data passed */
    glcdInit = ( DRV_GFX_GLCD_INIT * ) init ;
    
    /* Allocate the driver object and set the operation flag to be in use */
    dObj = &gDrvGfxGLCDObj[drvIndex];
    dObj->inUse = true;
    
    /* Update the GFX GLCD PLIB Id and other parameters. */
    
    dObj->moduleId              = glcdInit->glcdID;
    dObj->isExclusive           = false;
    dObj->hsyncInterruptSource  = glcdInit->interruptHSync;
    dObj->vsyncInterruptSource  = glcdInit->interruptVSync;
    dObj->errorInterruptSource  = glcdInit->interruptError
    dObj->dmaChannelWrite       = glcdInit->dmaChannelWrite;
    dObj->dmaChannelRead        = glcdInit->dmaChannelRead;
    dObj->dmaInterruptWrite     = glcdInit->dmaInterruptWrite;
    dObj->dmaInterruptRead      = glcdInit->dmaInterruptRead;
    dObj->interruptNestingCount = 0;
    
    /* Setup the Hardware */
    _DRV_GFX_GLCD_HardwareSetup(glcdInit->glcdID, glcdInit ) ;
    
    /* Clear the interrupts */
    SYS_INT_SourceStatusClear(dObj->hsyncInterruptSource);
    SYS_INT_SourceStatusClear(dObj->vsyncInterruptSource);
    SYS_INT_SourceStatusClear(dObj->errorInterruptSource);
    
    /* Enable the interrupt source in case of interrupt mode */
    _DRV_GFX_GLCD_InterruptSourceEnable(dObj->errorInterruptSource);
    
    /* Create the hardware instance mutex. */
    if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    
    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
/* Function:
    void DRV_GFX_GLCD_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_Deinitialize system interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_Deinitialize system interface
    function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

void  DRV_GFX_GLCD_Deinitialize(SYS_MODULE_OBJ object)
{
    DRV_GFX_GLCD_OBJ * dObj;
    bool status;

    /* Check that the object is valid */

    if(object == SYS_MODULE_OBJ_INVALID)
    {
        SYS_DEBUG(0, "Invalid system object handle" );
        return;
    }

    if(object >= DRV_GFX_GLCD_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid system object handle" );
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ*) &gDrvGfxGLCDObj[object];

    if(!dObj->inUse)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return;
    }

    /* The driver will not have clients when it is
       being deinitialized. So the order in which
       we do the following steps is not that important */

    /* Indicate that this object is not is use */
    dObj->inUse = false;

    /* Deinitialize the GFX GLCD status */
    dObj->status =  SYS_STATUS_UNINITIALIZED ;

    /* Disable the interrupt */
    status = _DRV_GFX_GLCD_InterruptSourceDisable(dObj->vsyncInterruptSource) ;
    status = _DRV_GFX_GLCD_InterruptSourceDisable(dObj->hsyncInterruptSource) ;
    status = _DRV_GFX_GLCD_InterruptSourceDisable(dObj->errorInterruptSource);

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Mutex Delete Failed");
        return;
    }

    /* TODO: Disable all DMA interrupts */

}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_GFX_GLCD_Status( SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_Status system interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_Status system interface
    function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

SYS_STATUS DRV_GFX_GLCD_Status( SYS_MODULE_OBJ object)
{
    /* Check if we have a valid object */
    if(object == SYS_MODULE_OBJ_INVALID)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return(SYS_STATUS_UNINITIALIZED);
    }

    if(object > DRV_GFX_GLCD_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return(SYS_STATUS_UNINITIALIZED);
    }

    /* Return the system status of the hardware instance object */
    return (gDrvGfxGLCDObj[object].status);
}

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_GFX_GLCD_Open( const SYS_MODULE_INDEX index,
                               const DRV_IO_INTENT    ioIntent )

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_Open client interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_Open client interface
    function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

DRV_HANDLE DRV_GFX_GLCD_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_GFX_CLIENT_OBJ *clientObj;
    DRV_GFX_GLCD_OBJ *dObj;
    unsigned int iClient;

    if (drvIndex >= DRV_GFX_GLCD_INSTANCES_NUMBER)
    {
        /* Invalid driver index */
        SYS_DEBUG(0, "Invalid Driver Instance");
        return (DRV_HANDLE_INVALID);
    }

    dObj = &gDrvGfxGLCDObj[drvIndex];

    if((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false))
    {
        /* The GFX GLCD module should be ready */

        SYS_DEBUG(0, "Was the driver initialized?");
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive)
    {
        /* This means the another client has opened the driver in exclusive
           mode. The driver cannot be opened again */

        SYS_DEBUG(0, "Driver already opened exclusively");
        return ( DRV_HANDLE_INVALID ) ;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        /* This means the driver was already opened and another driver was
           trying to open it exclusively.  We cannot give exclusive access in
           this case */

        SYS_DEBUG(0, "Driver already opened. Cannot be opened exclusively");
        return(DRV_HANDLE_INVALID);
    }

    clientObj = &drvLCCClients;

    clientObj->inUse = true;
    clientObj->drvObj = &drvGfxGLCDObj;

    /* Increment the client number for the specific driver instance*/
    drvGfxGLCDObj.nClients++;

    /* If we have reached here, it means either we could not find a spare
       client object or the mutex timed out in a RTOS environment. */

    return ((DRV_HANDLE)clientObj);
}

// *****************************************************************************
/* Function:
    DRV_CLIENT_STATUS DRV_GFX_GLCD_Close ( DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_Close client interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_Close client interface
    function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

void DRV_GFX_GLCD_Close ( DRV_HANDLE handle)
{
    /* This function closes the client, The client
       object is deallocated and returned to the
       pool. */

    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    /* Reduce the number of clients */
    dObj->nClients --;

    /* De-allocate the object */
    clientObj->status = DRV_GFX_GLCD_CLIENT_STATUS_CLOSED;
    clientObj->inUse = false;

    return;
}

// *****************************************************************************
/* Function:
    DRV_GFX_GLCD_ERROR DRV_GFX_GLCD_ErrorGet( DRV_HANDLE hClient )

  Summary:
    Dynamic implementation of DRV_GFX_GLCD_ErrorGet client interface function.

  Description:
    This is the dynamic implementation of DRV_GFX_GLCD_ErrorGet client interface
    function.

  Remarks:
    See drv_gfx_glcd.h for usage information.
*/

DRV_GFX_GLCD_ERROR DRV_GFX_GLCD_ErrorGet(DRV_HANDLE hClient)
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_ERROR error;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(hClient);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid. Return error */
        SYS_DEBUG(0, "Invalid client handle");
        return DRV_GFX_GLCD_INVALID_HANDLE;
    }

    /* Return the error. Clear the error before
       returning. */

    error = clientObj->error;
    clientObj->error = DRV_GFX_GLCD_ERROR_NONE;
    return(error);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/

DRV_GFX_CAPABILITIES_RESULT DRV_GFX_GLCD_CapabilitiesGet( DRV_HANDLE handle, 
                                                     DRV_GFX_CAPS capabilities)
{
    DRV_GFX_CAPABILITIES_RESULT result = DRV_GFX_CAPABILITY_NOT_SUPPORTED;
    
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;
    
    if ( dObj->driverCaps & capabilities)
    {
        result = DRV_GFX_CAPABILITY_SUPPORTED;
    }
    
    return (result);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_BG_COLOR_RESULT DRV_GFX_GLCD_BackgroundColorSet( DRV_HANDLE handle,
                                                     uint32_t bgColorABGR8888 )
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    dObj->bgColorABGR8888 = bgColorABGR8888;
    
    return (DRV_GFX_GLCD_BG_COLOR_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
uint32_t DRV_GFX_GLCD_NumberOfLayersGet(DRV_HANDLE handle)
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;
    
    return (dObj->numberOfLayers);
    
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_LAYER_INIT_RESULT DRV_GFX_GLCD_LayerInitialize(DRV_HANDLE handle, 
                                                const DRV_GFX_GLCD_LAYER_ID layerID,
                                                const DRV_GFX_GLCD_LAYER_INIT * const layerInit)
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;
    
    DRV_GFX_GLCD_LAYER_COLOR_MODE colorMode;
    DRV_GFX_GLCD_LAYER_AXI_BEATS axiBeats;
    DRV_GFX_GLCD_LAYER_AXI_FIFO_BURST_THRESHOLD burstThreshold;
    DRV_GFX_GLCD_LAYER_BLEND_FUNCTION srcBlendFunc;
    DRV_GFX_GLCD_LAYER_BLEND_FUNCTION destBlendFunc;
    
    uint8_t  globalAlpha;
    uint32_t startX;
    uint32_t startY;
    uint32_t resolutionX;
    uint32_t resolutionY;
    uint32_t address;
    uint32_t scaleX;
    uint32_t scaleY;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    if(layerID > DRV_GFX_GLCD_LAYERS_NUMBER)
    {
        /* layer ID is not valid */
        SYS_DEBUG(0, "Invalid layer ID");
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;
    
    PLIB_GLCD_LayerEnable( dObj->moduleId, layerID );
            
    if(layerInit->flags & DRV_GFX_GLCD_LAYER_BILINEAR_FILTER_ENABLE)
    {
        PLIB_GLCD_Layer_BilinearFilterEnable( dObj->moduleId, layerID );
    }
    else
    {
        PLIB_GLCD_Layer_BilinearFilterDisable( dObj->moduleId, layerID );
    }
    
    if(layerInit->flags & DRV_GFX_GLCD_LAYER_FORCE_ALPHA_WITH_GLOBAL_ALPHA)
    {
        PLIB_GLCD_LayerForceAlphaWithGlobalAlphaEnable(dObj->moduleId, layerID);
    }
    else
    {
        PLIB_GLCD_LayerForceAlphaWithGlobalAlphaDisable(dObj->moduleId, layerID);
    }
    
    if(layerInit->flags & DRV_GFX_GLCD_LAYER_PREMULTIPLY_IMAGE_ALPHA)
    {
        PLIB_GLCD_LayerPremultiplyImageAlphaEnable(dObj->moduleId, layerID);
    }
    else
    {
        PLIB_GLCD_LayerPremultiplyImageAlphaDisable(dObj->moduleId, layerID);
    }
    
    if(layerInit->flags & DRV_GFX_GLCD_LAYER_HLOCK_ASSERT_ENABLE)
    {
        PLIB_GLCD_LayerHLOCKAssertEnable(dObj->moduleId, layerID);
    }
    else
    {
        PLIB_GLCD_LayerHLOCKAssertDisable(dObj->moduleId, layerID);
    }
    
    if(layerInit->flags & DRV_GFX_GLCD_LAYER_GAMMA_LUT_ENABLE)
    {
        PLIB_GLCD_LayerGammaLUTEnable(dObj->moduleId, layerID);
    }
    else
    {
        PLIB_GLCD_LayerGammaLUTDisable(dObj->moduleId, layerID);
    }
    
    globalAlpha = layerInit->globalAlpha;
    PLIB_GLCD_LayerGlobalAlphaSet(dObj->moduleId, layerID, globalAlpha);
    
    srcBlendFunc = layerInit->srcBlendFunc;
    PLIB_GLCD_LayerSrcBlendFuncSet(dObj->moduleId, layerID, srcBlendFunc);
            
    destBlendFunc = layerInit->destBlendFunc;
    PLIB_GLCD_LayerDestBlendFuncSet(dObj->moduleId, layerID, destBlendFunc);
    
    colorMode = layerInit->colorMode;
    PLIB_GLCD_LayerColorModeSet(colorMode);

    startX = layerInit->startX;
    startY = layerInit->startY;
    PLIB_GLCD_LayerStartXYSet(dObj->moduleId, layerID, startX, startY);    
    
    sizeX = layerInit->sizeX;
    sizeY = layerInit->sizeY;
    PLIB_GLCD_LayerSizeXYSet(dObj->moduleId, layerID, sizeX, sizeY);
    
    address = layerInit->address;
    PLIB_GLCD_LayerAddressSet(dObj->moduleId, layerID, address);
    
    axiBeats = layerInit->axiBeats
    burstThreshold = layerInit->burstThreshold;
    PLIB_GLCD_LayerStrideSet(dObj->moduleId, layerID, axiBeats, burstThreshold);
    
    resolutionX = layerInit->resolutionX;
    resolutionY = layerInit->resolutionY;
    PLIB_GLCD_LayerResXYSet(dObj->moduleId, layerID, resolutionX, resolutionY);
    
    scaleX = layerInit->scaleX;
    scaleY = layerInit->scaleY;
    PLIB_GLCD_LayerScaleXYSet(dObj->moduleId, layerID, scaleX, scaleY);
    
    return (DRV_GFX_GLCD_LAYER_INIT_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
uint32_t DRV_GFX_GLCD_DeviceIdGet( DRV_HANDLE handle )
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;
    
    return (PLIB_GLCD_DeviceIdGet(dObj->moduleId));
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_GLOBAL_LUT_RESULT DRV_GFX_GLCD_GlobalLUTColorXRGB8888Set(
                                                        DRV_HANDLE handle,
                                                        uint32_t * lutColorsXRGB8888 )
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    if(lutColorsXRGB8888 == NULL)
    {
        /* lut color table address invalid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    PLIB_GLCD_GlobalLUTColorXRGB8888Set(dObj->moduleId, lutColorsXRGB8888);
    
    return (DRV_GFX_GLCD_GLOBAL_LUT_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_LAYER_LUT_RESULT DRV_GFX_GLCD_LayerLUTColorARGB8888Set(
                                                        DRV_HANDLE handle,
                                                        DRV_GFX_GLCD_LAYER_ID layerID,
                                                        uint32_t * lutColorsARGB8888 )
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    if(layerID > DRV_GFX_GLCD_LAYERS_NUMBER)
    {
        /* layer ID is not valid */
        SYS_DEBUG(0, "Invalid layer ID");
    }
    
    if(lutColorsARGB8888 == NULL)
    {
        /* lut color table address invalid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    PLIB_GLCD_LayerLUTColorARGB8888Set(dObj->moduleId,layerID, 
                                       lutColorsARGB8888);
    
    return (DRV_GFX_GLCD_LAYER_LUT_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_CURSOR_POSITION_RESULT DRV_GFX_GLCD_CusrsorPositionSet(
                                                        DRV_HANDLE handle,
                                                        uint32_t cursorX,
                                                        uint32_t cursorY )
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;
    
    PLIB_GLCD_CursorXYSet(dObj->moduleId, cursorX, cursorY);
    
    return (DRV_GFX_GLCD_CURSER_POSITION_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_CURSOR_GRAY_COLOR_RESULT DRV_GFX_GLCD_Cusrsor4BitGrayColorLUTSet(
                                                        DRV_HANDLE handle,
                                                        uint32_t * cursorBWG4Data)
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    PLIB_GLCD_CursorBWG4DataSet(dObj->moduleId, cursorBWG4Data);
            
    return (DRV_GFX_GLCD_CURSOR_GRAY_COLOR_RESULT_SUCCESS);
}

// *****************************************************************************
/* Function:
   
   Summary:
  
   Description:
 
   Remarks:
*/
DRV_GFX_GLCD_CURSOR_COLOR_XRGB8888_RESULT DRV_GFX_GLCD_CusrsorColorXRGB8888LUTSet(
                                                        DRV_HANDLE handle,
                                                        uint32_t * cursorLUTColorsXRGB8888)
{
    DRV_GFX_CLIENT_OBJ * clientObj;
    DRV_GFX_GLCD_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_GFX_GLCD_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }
    
    dObj = (DRV_GFX_GLCD_OBJ *)clientObj->hDriver;

    PLIB_GLCD_CursorLUTColorXRGB8888Set(dObj->moduleId, 
                                        cursorLUTColorsXRGB8888);
            
    return (DRV_GFX_GLCD_CURSOR_COLOR_XRGB8888_RESULT_SUCCESS);
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
 // *****************************************************************************

// *****************************************************************************
/* void _DRV_GFX_GLCD_HardwareSetup( GLCD_MODULE_ID  plibID,
                                        DRV_GFX_GLCD_INIT * init)

  Summary:
    Initializes the GFX GLCD module based on the init data structure.

  Description:
    This function initializes the GFX GLCD module based on the init data 
  structure.

  Remarks:
    This is a private function and should not be called directly by the
    application.
*/

void _DRV_GFX_GLCD_HardwareSetup(GLCD_MODULE_ID plibID, DRV_GFX_GLCD_INIT * init)
{
    DRV_GFX_GLCD_POLARITY vsyncPolarity;
    DRV_GFX_GLCD_POLARITY hsyncPolarity;
    DRV_GFX_GLCD_POLARITY dePolarity;
    DRV_GFX_GLCD_POLARITY pixelClockPolarity;
    DRV_GFX_GLCD_RGB_MODE rgbMode;
    DRV_GFX_GLCD_IRQ_TRIGGER_CONTROL triggerControl;
    
    uint32_t clockDivider;
    uint32_t linesPrefetch;
    uint32_t resolutionX;
    uint32_t resolutionY;
    uint32_t frontPorchX;
    uint32_t frontPorchY;
    uint32_t backPorchX;
    uint32_t backPorchY;
    uint32_t blankingX;
    uint32_t blankingY;
    
    PLIB_GLCD_DisplayModeRegisterEnable( plibID );
    
    if (init->flags & DRV_GFX_GLCD_INIT_AHB_LOCKED_TRANSACTIONS)
    {
        PLIB_GLCD_AHBLockedTransactionsEnable( plibID );
    }
    else
    {
        PLIB_GLCD_AHBLockedTransactionsDisable( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_DITHERING_ENABLE)
    {
        PLIB_GLCD_DitheringEnable( plibID );
    }
    else
    {
        PLIB_GLCD_DitheringDisable( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_SINGLE_CYCLE_VSYNC_SET)
    {
        PLIB_GLCD_SingleCycleVsyncSet( plibID );
    }
    else
    {
        PLIB_GLCD_SingleCycleVsyncClear( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_GAMMA_RAMP_ENABLE)
    {
        PLIB_GLCD_PaletteGammaRampEnable( plibID );
    }
    else
    {
        PLIB_GLCD_PaletteGammaRampDisable( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_FORCE_BLANK_OUTPUT_SET)
    {
        PLIB_GLCD_ForceBlankOutputSet( plibID );
    }
    else
    {
        PLIB_GLCD_ForceBlankOutputClear( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_LVDS_PADS_ENABLE)
    {
        PLIB_GLCD_LVDSPadsEnable( plibID );
    }
    else
    {
        PLIB_GLCD_LVDSPadsDisable( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_YUV_OUTPUT_ENABLE)
    {
        PLIB_GLCD_YUVOutputEnable( plibID );
    }
    else
    {
        PLIB_GLCD_YUVOutputDisable( plibID );
    }
    
    if (init->flags & DRV_GFX_GLCD_INIT_CLOCK_DIVIDED_ENABLE)
    {
        PLIB_GLCD_ClockDividedEnable( plibID );
    }
    else
    {
        PLIB_GLCD_ClockDividedDisable( plibID );
    }
            
    vsyncPolarity = init->vsyncPolarity;
    
    switch(vsyncPolarity)
    {
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_VsyncPolaritySet(plibID, GLCD_PIN_POLARITY_POSITIVE);
            
            break;
        }
       
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_VsyncPolaritySet(plibID, GLCD_PIN_POLARITY_NEGATIVE);
            
            break;
        }
    }
    
    hsyncPolarity = init->hsyncPolarity;
    
    switch(hsyncPolarity)
    {
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_HsyncPolaritySet(plibID, GLCD_PIN_POLARITY_POSITIVE);
            
            break;
        }
       
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_HsyncPolaritySet(plibID, GLCD_PIN_POLARITY_NEGATIVE);
            
            break;
        }
    }
    
    dePolarity = init->dePolarity;
    
    switch(dePolarity)
    {
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_DEPolaritySet(plibID, GLCD_PIN_POLARITY_POSITIVE);
            
            break;
        }
       
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_DEPolaritySet(plibID, GLCD_PIN_POLARITY_NEGATIVE);
            
            break;
        }
    }
    
    pixelClockPolarity = init->pixelClockPolarity;
    
    switch(pixelClockPolarity)
    {
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_PixelClockOutPolaritySet(plibID, GLCD_PIN_POLARITY_POSITIVE);
            
            break;
        }
       
        case DRV_GFX_GLCD_POLARITY_POSITIVE:
        {
            PLIB_GLCD_PixelClockOutPolaritySet(plibID, GLCD_PIN_POLARITY_NEGATIVE);
            
            break;
        }
    }
    
    rgbMode = init->rgbMode;
    
    switch(rgbMode)
    {
        case DRV_GFX_GLCD_RGB_MODE_PARALLEL:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_PARALLEL);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_SERIAL_RGB_3:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_SERIAL_RGB_3);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_SERIAL_RGBX_4:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_SERIAL_RGBX_4);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_SERIAL_12BIT:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_SERIAL_12BIT);
            break;
        }
    
        case DRV_GFX_GLCD_RGB_MODE_LVDS_ISP68:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_LVDS_ISP68);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_LVDS_ISP8:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_LVDS_ISP8);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_YUYV_16BIT:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_YUYV_16BIT);
            break;
        }
        
        case DRV_GFX_GLCD_RGB_MODE_BT_656:
        {
            PLIB_GLCD_RGBSequentialModeSet(plibID, GLCD_RGB_MODE_BT_656);
            break;
        }
    }
    
    PLIB_GLCD_DisplayModeRegisterDisable( plibID );
    
    clockDivider  = init->clockDivider;
    linesPrefetch = init->linesPrefetch;
    
    PLIB_GLCD_ClockControlSet(plibID, clockDivider, linesPrefetch);
    
    resolutionX = init->resolutionX;
    resolutionY = init->resolutionY;
    
    PLIB_GLCD_ResolutionXYSet(plibID, resolutionX, resolutionY);
    
    frontPorchX = init->frontPorchX;
    frontPorchY = init->frontPorchY;
    
    PLIB_GLCD_FrontPorchXYSet(plibID, frontPorchX, frontPorchY );
    
    backPorchX = init->backPorchX;
    backPorchY = init->backPorchY;
    
    PLIB_GLCD_BackPorchXYSet(plibID, backPorchX, backPorchY);
    
    blankingX = init->blankingX;
    blankingY = init->blankingY;
    
    PLIB_GLCD_BlankingXYSet(plibID, blankingX, blankingY);
    
    triggerControl = init->triggerControl;
    
    PLIB_GLCD_IRQTriggerControlSet(plibID, triggerControl);
    
    if (init->flags & DRV_GFX_GLCD_INIT_HSYNC_INTERRUPT_ENABLE)
    {
        PLIB_GLCD_HsyncInterruptEnable( plibID );
    }
    else
    {
        PLIB_GLCD_HsyncInterruptDisable( plibID );
    }
    
    if(init->flags & DRV_GFX_GLCD_INIT_VSYNC_INTERRUPT_ENABLE)
    {
        PLIB_GLCD_VsyncInterruptEnable( plibID );
    }
    else
    {
        PLIB_GLCD_VsyncInterruptDisable( plibID );
    }
    
}

// *****************************************************************************
/* Function:
    DRV_GFX_GLCD_CLIENT_OBJ * _DRV_GFX_GLCD_DriverHandleValidate(DRV_HANDLE handle)

  Summary:
    Dynamic implementation of the _DRV_GFX_GLCD_DriverHandleValidate() function.

  Description:
    Dynamic implementation of the _DRV_GFX_GLCD_DriverHandleValidate() function.
    This function return NULL if the handle is invalid else it return a pointer
    to the GFX_GLCD Driver Client Object associated with this handle.

  Remarks:
    This is a private function and should not be called directly by an
    application.
*/

DRV_GFX_GLCD_CLIENT_OBJ * _DRV_GFX_GLCD_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise. */

    DRV_GFX_GLCD_CLIENT_OBJ * client;

    if((DRV_HANDLE_INVALID == handle) ||
            (0 == handle))
    {
        return(NULL);
    }

    client = (DRV_GFX_GLCD_CLIENT_OBJ *)handle;

    if(!client->inUse)
    {
        return(NULL);
    }

    return(client);
}

/*******************************************************************************
 End of File
*/
