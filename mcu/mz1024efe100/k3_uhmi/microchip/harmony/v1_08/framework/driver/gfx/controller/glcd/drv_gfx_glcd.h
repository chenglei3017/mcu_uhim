/*******************************************************************************
  GFX GLCD Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd.h

  Summary:
    GFX GLCD Driver Interface Header File

  Description:
    The GFX GLCD device driver provides a simple interface to manage the GFX 
    GLCD modules on Microchip microcontrollers.  This file provides the 
    interface definition for the GFX GLCD driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

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

#ifndef _DRV_GFX_GLCD_H
#define _DRV_GFX_GLCD_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
/* Note:  A file that maps the interface definitions above to appropriate static
          implementations (depending on build mode) is included at the bottom of
          this file.
*/

#include "system_config.h"
#include "driver/driver_common.h"
#include "peripheral/glcd/plib_glcd.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/dma/sys_dma.h"

#ifdef __cplusplus
    extern "C" {
#endif
        
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Driver GFX GLCD Module Index

  Summary:
    GFX GLCD driver index definitions

  Description:
    These constants provide GFX GLCD driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.
    These values should be passed into the DRV_GFX_GLCD_Initialize and
    DRV_GFX_GLCD_Open routines to identify the driver instance in use.
*/

#define DRV_GFX_GLCD_INDEX_0             0
#define DRV_GFX_GLCD_INDEX_1             1
#define DRV_GFX_GLCD_INDEX_2             2

// *****************************************************************************
/* GFX GLCD Driver Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the GFX GLCD driver

  Description:
    This data type defines the data required to initialize or reinitialize the 
    GLCD driver. If the driver is built statically, the members of this data
    structure are statically over-ridden by static override definitions in the 
    system_config.h file.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization data */
    SYS_MODULE_INIT                 moduleInit;

    /* Identifies GFX GLCD hardware module (PLIB-level) ID. For a static build 
       of the driver, this is overridden by DRV_GFX_GLCD_MODULE_ID macro in the
       system_config.h header file. */
    GLCD_MODULE_ID                  glcdID;
    
    /* Interrupt source ID for the hsync interrupt. For a static build of
       the driver, this is overridden by the DRV_GFX_GLCD_HSYNC_INTERRUPT_SOURCE 
       macro in the system_config.h header. */
    INT_SOURCE                      interruptHSync;

    /* Interrupt source ID for the receiver interrupt. For a static build of 
       the driver, this is overridden by the DRV_GFX_GLCD_VSYNC_INTERRUPT_SOURCE 
       macro in the system_config.h header. */
    INT_SOURCE                      interruptVSync;

    /* Interrupt source ID for the error Interrupt. For a static build of the 
       driver, this is overridden by the DRV_GLCD_ERROR_INTERRUPT_SOURCE macro 
       in the system_config.h header. */
    INT_SOURCE                      interruptError;
    
    /* This is the GFX GLCD Write DMA channel. This is applicable
       only if DRV_GFX_GLCD_SUPPORT_WRITE_DMA is defined as true.
       For a static build of the driver, this is overridden by the
       DRV_GFX_GLCD_WRITE_DMA_CHANNEL macro in system_config.h */

    DMA_CHANNEL                     dmaChannelWrite;

    /* This is the GFX GLCD read DMA channel. This is applicable
       only if DRV_GFX_GLCD_SUPPORT_READ_DMA is defined as true.
       For a static build of the driver, this is overridden by the
       DRV_GFX_GLCD_READ_DMA_CHANNEL macro in system_config.h */

    DMA_CHANNEL                     dmaChannelRead;

    /* This is the GFX GLCD write DMA channel interrupt. This is
       applicable only if DRV_GFX_GLCD_SUPPORT_WRITE_DMA is defined
       as true. For a static build of the driver, this is overridden
       by the DRV_GFX_GLCD_INTERRUPT_SOURCE_WRITE_DMA_CHANNEL macro
       in system_config.h */

    INT_SOURCE                      dmaInterruptWrite;

    /* This is the GFX GLCD read DMA channel interrupt. This is
       applicable only if DRV_GFX_GLCD_SUPPORT_READ_DMA is defined
       as true. For a static build of the driver, this is overridden
       by the DRV_GFX_GLCD_INTERRUPT_SOURCE_READ_DMA_CHANNEL macro
       in system_config.h */

    INT_SOURCE                      dmaInterruptRead;

} DRV_GFX_GLCD_INIT;

// ****************************************************************************
// ****************************************************************************
// Section: Included Files (continued)
// ****************************************************************************
// ****************************************************************************
/*  The files included below map the interface definitions above to appropriate
    static implementations, depending on build mode.
*/

#ifdef __cplusplus
    }
#endif
    
#endif // #ifndef _DRV_GFX_GLCD_H
/*******************************************************************************
 End of File
*/
