/*******************************************************************************
  GFX GLCD Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd_local.h

  Summary:
    GFX GLCD Driver Local Data Structures

  Description:
    Driver Local Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_GFX_GLCD_LOCAL_H
#define _DRV_GFX_GLCD_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system/clk/sys_clk.h"
#include "driver/gfx/controller/glcd/drv_gfx_glcd.h"
#include "driver/gfx/controller/glcd/src/drv_gfx_glcd_variant_mapping.h"
#include "osal/osal.h"
#include "system/int/sys_int.h"
#include "system/debug/sys_debug.h"
#include "peripheral/glcd/plib_glcd.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* GFX GLCD Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the GFX GLCD driver.

  Description:
    This object is used to keep track of any data that must be maintained to 
    manage a single instance of the GFX GLCD driver.

  Remarks:
    None.
*/

typedef struct
{
    /* The module index associated with the object*/
    GLCD_MODULE_ID  moduleId;

    /* The status of the driver */
    SYS_STATUS status;
    
    /* Flag to indicate this object is in use  */
    bool inUse;

    /* Flag to indicate that driver has been opened exclusively. */
    bool isExclusive;
    
    /* Hardware instance mutex */
    OSAL_MUTEX_DECLARE(mutexDriverInstance);
    
    /* Keeps track if the driver is in interrupt context
       and if so the nesting levels. */
    uint32_t interruptNestingCount;

    /* Interrupt Source for HSync Interrupt */
    INT_SOURCE hsyncInterruptSource;

    /* Interrupt Source for VSync Interrupt */
    INT_SOURCE vsyncInterruptSource;

    /* Interrupt Source for error Interrupt */
    INT_SOURCE  errorInterruptSource;

    /* Write DMA channel  */
    DMA_CHANNEL dmaChannelWrite;
    
    /* Read DMA channel  */
    DMA_CHANNEL dmaChannelRead;

    /* Write DMA Channel Interrupt Source */
    INT_SOURCE dmaInterruptWrite;

    /* Read DMA Channel Interrupt Source */
    INT_SOURCE dmaInterruptRead;
    
} DRV_GFX_GLCD_OBJ;

// *****************************************************************************
/* GFX GLCD Driver Global Instances Object

  Summary:
    Object used to keep track of data that is common to all instances of the 
    GFX GLCD driver.

  Description:
    This object is used to keep track of any data that is common to all
    instances of the GFX GLCD driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Set to true if all members of this structure
       have been initialized once */
    bool membersAreInitialized;

    /* Mutex to protect buffer queue object pool */
    OSAL_MUTEX_DECLARE(mutexBufferQueueObjects);

} DRV_GFX_GLCD_COMMON_DATA_OBJ;

// *****************************************************************************
/*  Definitions of functions used internally */

void _DRV_GFX_GLCD_HardwareSetup(GLCD_MODULE_ID plibID, DRV_GFX_GLCD_INIT * init);

DRV_GFX_GLCD_CLIENT_OBJ * _DRV_GFX_GLCD_DriverHandleValidate(DRV_HANDLE handle);

#ifdef __cplusplus
    }
#endif
    
#endif //#ifndef _DRV_GFX_GLCD_LOCAL_H

/*******************************************************************************
 End of File
*/
