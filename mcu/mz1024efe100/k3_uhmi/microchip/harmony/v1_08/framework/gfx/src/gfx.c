/*******************************************************************************
  Company:
    Microchip Technology Incorporated

  File Name:
    gfx.c
    
  Summary:
    Interface for the graphics library.

  Description:
    This file contains the interface definition for the graphics library.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "gfx/gfx.h"
#include "osal/osal.h"


static GFX_OBJ                              gGfxObj[GFX_INDEX_COUNT];
static GFX_CLIENT_OBJ                       gfxClient[GFX_INDEX_COUNT];
//static OSAL_SEM_HANDLE_TYPE                 gfxSemaphore;
//static OSAL_RESULT                          gfxSemaphoreEnabled = OSAL_RESULT_FALSE;
//static bool                                 gfxProtectionInited = false;

/******************************************************************************
  Function:
    SYS_MODULE_OBJ GFX_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                              const SYS_MODULE_INIT    * const moduleInit)

  Summary:
    Initializes primitive data structurse for the general features
    of the primitive layer.

  Description:
    Initializes external and internal data structure for the general
    features of the primitive layer.

    This function must be called at system intialization.

  Remarks:
    None.
*/
SYS_MODULE_OBJ GFX_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                              const SYS_MODULE_INIT    * const moduleInit )
{
//    if (!gfxProtectionInited)
//    {
//        if (OSAL_SEM_Create(&gfxSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE) 
//        {
//           /*Report Error*/
//           gfxSemaphore = NULL;
//           gfxProtectionInited = false;
//        }
//        gfxProtectionInited = true;
//    }

    /* Validate the moduleIndex index */
    if ( moduleIndex >= GFX_INDEX_COUNT )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    GFX_OBJ * obj = &gGfxObj[moduleIndex];

    if ( obj->inUse )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Object is valid, set it in use */
    obj->inUse = true;
    obj->initData = (GFX_INIT *) moduleInit;

#ifndef GFX_CONFIG_GOL_DISABLE
    /* initialize gol portion of GFX */
    SYS_MODULE_OBJ golObj =  (SYS_MODULE_OBJ)GFX_GOL_Initialize(moduleIndex, moduleInit);
    if ( golObj == SYS_MODULE_OBJ_INVALID )
    {
        obj->inUse = false;
        return SYS_MODULE_OBJ_INVALID;
    }

    obj->golObject = golObj;
#endif

    /* initialize primitive (hal) portion of GFX */
    SYS_MODULE_OBJ primObj =  (SYS_MODULE_OBJ)GFX_PRIMITIVE_Initialize(moduleIndex, moduleInit);
    if ( primObj == SYS_MODULE_OBJ_INVALID )
    {
        obj->inUse = false;
        return SYS_MODULE_OBJ_INVALID;
    }
    obj->primitiveObject = primObj;

    obj->index = moduleIndex;
    obj->numClients = 0;
    obj->status = SYS_STATUS_BUSY;
    obj->task = GFX_TASK_STATE_OPEN_MODULE;

    /* Return the module handle */
    return( (SYS_MODULE_OBJ) obj );

}

// *****************************************************************************
/* Function:
    void GFX_Deinitialize(SYS_MODULE_OBJ gfxObject)

  Summary:
    Dynamic impementation of GFX_Deinitialize system interface function.

  Description:
    This is the dynamic impementation of GFX_Deinitialize system interface
    function.

  Remarks:
*/

void GFX_Deinitialize ( SYS_MODULE_OBJ gfxObject )
{
//    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
//    if (gfxProtectionInited != true)
//    {
//            OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
//            return;
//    }

    /* Host object */
    GFX_OBJ *gfxObj = (GFX_OBJ *)gfxObject;

    /* Validate the USB Host index */
    if(gfxObject == SYS_MODULE_OBJ_INVALID)
    {
        return;
    }

    /* Validate Host is already assigned */
    if(!gfxObj->inUse)
    {
        return;
    }

#ifndef GFX_CONFIG_GOL_DISABLE
    /* deinitialize gol portion of GFX */
    GFX_GOL_Deinitialize(gfxObj->golObject);
#endif
    
    /* deinitialize primitive (hal) portion of GFX */
    GFX_PRIMITIVE_Deinitialize(gfxObj->primitiveObject);

    /* Indicate that this object is not is use */
    gfxObj->inUse = false;
    
//    gfxProtectionInited = false;
//    if (OSAL_SEM_Delete(&gfxSemaphore) == OSAL_RESULT_FALSE) {/*Report error*/}
//    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
}

// *****************************************************************************
/*
  Function: GFX_HANDLE GFX_Open( const SYS_MODULE_INDEX index )

  Summary:
    opens an instance of the gfx module
*/
GFX_HANDLE GFX_Open( SYS_MODULE_OBJ gfxObject)
{
    /* Host object */
    GFX_OBJ *obj = (GFX_OBJ *)gfxObject;

    /* Check if instance object is ready*/
    if(obj->status != SYS_STATUS_BUSY)
    {
        return GFX_HANDLE_INVALID;
    }

//    if(obj->numClients > 0)
//    {
//        /* Driver supports exclusive open only */
//        return ((GFX_HANDLE)client);
//    }

    /* initialize the graphics driver */
    GFX_HANDLE primitiveHandle = (GFX_HANDLE)GFX_PRIMITIVE_Open(obj->index );

#ifndef GFX_CONFIG_GOL_DISABLE
    /* initialize the graphics driver */
    GFX_GOL_Open(obj->index );
#endif

    GFX_CLIENT_OBJ * client = &gfxClient[obj->index];

    client->inUse = true;
    client->primitiveHandle = (GFX_HANDLE)primitiveHandle;
    client->obj = obj;
    client->clientState = GFX_CLIENT_STATUS_READY;

    /* Increment the client number for the specific driver instance*/
    obj->numClients++;
    obj->task = GFX_TASK_STATE_RUNNING;
    obj->status = SYS_STATUS_READY;
   
    /* Return invalid handle */
    return ((GFX_HANDLE)client);
}

// *****************************************************************************
/* Function:
    SYS_STATUS GFX_Status (SYS_MODULE_INDEX gfxIndex)

  Summary:
    GFX_Status system interface function.

  Description:
    This is the impementation of GFX_Status system interface
    function.

*/

SYS_STATUS GFX_Status (SYS_MODULE_OBJ gfxObject )
{

    /* Validate the USB Host index */
    if(gfxObject == SYS_MODULE_OBJ_INVALID)
    {
        return(SYS_STATUS_ERROR);
    }

    /* Host object */
    GFX_OBJ *gfxObj = (GFX_OBJ *)gfxObject;

    /* Return the system status of the instance object */
    return ( gfxObj->status );

}

// *****************************************************************************
/* Function:
    void GFX_Close ( GFX_HANDLE handle)

  Summary:
    Closes the client instance.
*/

void GFX_Close (   GFX_HANDLE gfxHandle )
{
   GFX_CLIENT_OBJ * client = (GFX_CLIENT_OBJ*)gfxHandle;
   GFX_OBJ       *gfxObj;

   /* Validate the client address */
    if(client == NULL)
    {
        return;
    }
   /*Find the host object*/
   gfxObj = (GFX_OBJ*)client->obj;

   /* close the handle. */
    client->clientState = GFX_CLIENT_STATUS_CLOSED;

   /* Free the client */
    gfxObj->inUse = false;

}

// *****************************************************************************
/* Function:
    void GFX_Tasks (SYS_MODULE_INDEX index);

  Summary:
    Maintains the GFX module state machine. It manages the GFX Module object list
    items and responds to GFX Module primitive events.

*/
void GFX_Tasks  (SYS_MODULE_OBJ gfxObject)
{
//    if (OSAL_SEM_Pend(&gfxSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
//    {
////        return;
//    }

    /* Host object */
    GFX_OBJ *obj = (GFX_OBJ *)gfxObject;

    if ( ! obj->inUse )
    {
        return;
    }

    switch ( obj->task )
    {
        case GFX_TASK_STATE_OPEN_MODULE:

            /* open the device driver  */
            obj->gfxHandle = GFX_Open( gfxObject );

            if(obj->gfxHandle == GFX_HANDLE_INVALID)
            {
                break;
            }
            obj->task = GFX_TASK_STATE_RUNNING;
            obj->status = SYS_STATUS_READY;
            break;

        case GFX_TASK_STATE_RUNNING:

#ifndef GFX_CONFIG_GOL_DISABLE
            /* process list of objects  */
            GFX_GOL_Tasks(obj->index);
#endif

            break;
    }
//    if (OSAL_SEM_Post(&gfxSemaphore) != OSAL_RESULT_TRUE) {/*report Error*/}
}