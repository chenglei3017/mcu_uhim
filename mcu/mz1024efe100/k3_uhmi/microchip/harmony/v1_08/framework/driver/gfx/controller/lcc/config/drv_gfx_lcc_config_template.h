/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_lcc_config_template.h

  Summary:
    This header file template defines the configurations for Graphics Library
    Driver LCC.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DRV_GFX_LCC_CONFIG_TEMPLATE_H
    #define _DRV_GFX_LCC_CONFIG_TEMPLATE_H
// DOM-IGNORE-END

#error "This is a configuration template file.  Do not include it directly."

//////////////////// COMPILE OPTIONS ////////////////////
// *****************************************************************************
/*
     Macro:
        DRV_GFX_LCC_PALETTE_DISABLE

    Summary:
        Macro that disables internal palette memory framebuffer.

    Description:
        This macro disables the use color lookup palette.

        To disable palette internal framebuffer, add this macro in the configuration.

    Remarks:
        Optional definition.

*/
// *****************************************************************************
#define DRV_GFX_CONFIG_LCC_PALETTE

// *****************************************************************************
/*
    Macro:
        DRV_GFX_CONFIG_LCC_EXTERNAL_MEMORY

    Summary:
        Macro that enables internal memory framebuffer.

    Description:
        This macro enables the use of on-chip memory for the graphic framebuffer.

        To enable internal framebuffer, add this macro in the configuration and
        delete DRV_GFX_CONFIG_LCC_EXTERNAL_MEMORY if defined.

    Remarks:
        Mandatory definition if DRV_GFX_CONFIG_LCC_EXTERNAL_MEMORY is not set.
*/
// *****************************************************************************
#define DRV_GFX_CONFIG_LCC_INTERNAL_MEMORY
	
// *****************************************************************************
/*
    Macro:
        DRV_GFX_CONFIG_LCC_EXTERNAL_MEMORY

    Summary:
        Macro that enables external memory framebuffer.

    Description:
        This macro enables the use of off-chip memory for the graphic framebuffer.

        To enable external framebuffer, add this macro in the configuration and
        delete DRV_GFX_CONFIG_LCC_INTERNAL_MEMORY if defined.

    Remarks:
        Mandatory definition if DRV_GFX_CONFIG_LCC_INTERNAL_MEMORY is not set.

*/
// *****************************************************************************
#define DRV_GFX_CONFIG_LCC_EXTERNAL_MEMORY

// *****************************************************************************
/*
     Macro:
        DRV_GFX_LCC_DMA_CHANNEL_INDEX

    Summary:
        Macro that defines the DMA CHANNEL INDEX.

    Description:
        This macro defines the DMA CHANNEL to be used for LCC display controller operations.

    Remarks:
        Mandatory definition.

*/
// *****************************************************************************
#define DRV_GFX_LCC_DMA_CHANNEL_INDEX

#endif // _DRV_GFX_LCC_CONFIG_TEMPLATE_H
