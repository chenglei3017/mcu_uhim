/*******************************************************************************
  GFX GLCD Driver Variant Mapping

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_glcd_variant_mapping.h

  Summary:
    GFX GLCD Driver Variant Mapping

  Description:
    This file provides feature and build variant mapping macros allowing the
    driver to easily be built with different implementation variations based
    on static build-time configuration selections.
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

#ifndef _DRV_GFX_GLCD_FEATURE_MAPPING_H
#define _DRV_GFX_GLCD_FEATURE_MAPPING_H

#ifdef __cplusplus
    extern "C" {
#endif
        

// *****************************************************************************
// *****************************************************************************
// Section: Interrrupt Variations
// *****************************************************************************
// *****************************************************************************
/* Mapping of the interrupt mode variations
*/

#if defined (DRV_GFX_GLCD_INTERRUPT_MODE) && \
            (DRV_GFX_GLCD_INTERRUPT_MODE == true)

    #define _DRV_GFX_GLCD_InterruptSourceIsEnabled(source)     SYS_INT_SourceIsEnabled( source )
    #define _DRV_GFX_GLCD_InterruptSourceEnable(source)        SYS_INT_SourceEnable( source )
    #define _DRV_GFX_GLCD_InterruptSourceDisable(source)       SYS_INT_SourceDisable( source )
    #define _DRV_GFX_GLCD_InterruptSourceStatusClear(source)   SYS_INT_SourceStatusClear( source )

#elif defined (DRV_GFX_GLCD_INTERRUPT_MODE) && \
            (DRV_GFX_GLCD_INTERRUPT_MODE == false)

    /* Driver is configured for polled mode */
    #define _DRV_GFX_GLCD_InterruptSourceIsEnabled(source)       false
    #define _DRV_GFX_GLCD_InterruptSourceEnable(source)
    #define _DRV_GFX_GLCD_InterruptSourceDisable(source)         false
    #define _DRV_GFX_GLCD_InterruptSourceStatusClear(source)     SYS_INT_SourceStatusClear( source )    

#else

    #error "No Task mode chosen at build, interrupt or polling needs to be selected."

#endif

#ifdef __cplusplus
    }
#endif
    
#endif