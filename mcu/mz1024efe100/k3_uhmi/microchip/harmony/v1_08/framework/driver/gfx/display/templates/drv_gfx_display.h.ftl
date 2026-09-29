<#--
/*******************************************************************************
  Graphics Display Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_display.h.ftl

  Summary:
    GFX Freemarker Template File

  Description:

*******************************************************************************/

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
-->

/*** Graphics Display Configuration ***/
<#if CONFIG_USE_GFX_TRULY_7_800X480 == true || CONFIG_USE_GFX_TRULY_7_800X480 == true 
    || CONFIG_USE_GFX_TRULY_57_640X480 == true || CONFIG_USE_GFX_TRULY_32_240X320 == true 
    || CONFIG_USE_GFX_POWERTIP_43_480X272 == true || CONFIG_USE_GFX_NEWHAVEN_50_800X480_PCAP == true
    || CONFIG_USE_GFX_NEWHAVEN_43_480X272_PCAP == true || CONFIG_USE_GFX_CRYSTAL_2_172x220 == true
    || CONFIG_USE_GFX_LTS_43_480X272_RTP == true>
#define DISP_ORIENTATION                        ${CONFIG_DRV_GFX_DISP_ORIENTATION}
#define DISP_HOR_RESOLUTION                     ${CONFIG_DRV_GFX_DISP_HOR_RESOLUTION}
#define DISP_VER_RESOLUTION                     ${CONFIG_DRV_GFX_DISP_VER_RESOLUTION}
#define DISP_DATA_WIDTH			                ${CONFIG_DRV_GFX_DISP_DATA_WIDTH}
#define DISP_HOR_PULSE_WIDTH                    ${CONFIG_DRV_GFX_DISP_HOR_PULSE_WIDTH}
#define DISP_HOR_BACK_PORCH                     ${CONFIG_DRV_GFX_DISP_HOR_BACK_PORCH}
#define DISP_HOR_FRONT_PORCH                    ${CONFIG_DRV_GFX_DISP_HOR_FRONT_PORCH}
#define DISP_VER_PULSE_WIDTH                    ${CONFIG_DRV_GFX_DISP_VER_PULSE_WIDTH}
#define DISP_VER_BACK_PORCH                     ${CONFIG_DRV_GFX_DISP_VER_BACK_PORCH}
#define DISP_VER_FRONT_PORCH                    ${CONFIG_DRV_GFX_DISP_VER_FRONT_PORCH}
#define DISP_INV_LSHIFT                         ${CONFIG_DRV_GFX_DISP_INV_LSHIFT}
#define GFX_LCD_TYPE                            ${CONFIG_DRV_GFX_DISP_LCD_TYPE}
#define BACKLIGHT_ENABLE_LEVEL                  ${CONFIG_DRV_GFX_DISP_BACKLIGHT_ENABLE_LEVEL}
#define BACKLIGHT_DISABLE_LEVEL                 ${CONFIG_DRV_GFX_DISP_BACKLIGHT_DISABLE_LEVEL}
<#if CONFIG_USE_GFX_TRULY_32_240X320 == true>
#define USE_TCON_SSD1289
</#if>
#define TCON_MODULE                             ${CONFIG_DRV_GFX_DISP_TCON_MODULE}
<#if CONFIG_DRV_GFX_DISP_USE_TCON_MODULE == true>
#define USE_TCON_MODULE                         0
</#if>
<#if CONFIG_DRV_GFX_DISP_USE_CUSTOM_TCON_MODULE == true>
#define GFX_USE_TCON_CUSTOM
</#if>
<#if CONFIG_DRV_GFX_DISP_TOUCHSCREEN_RESISTIVE_SWAP_XY?has_content 
&& CONFIG_DRV_GFX_DISP_TOUCHSCREEN_RESISTIVE_SWAP_XY == true>
#define TOUCHSCREEN_RESISTIVE_SWAP_XY
</#if>
<#if CONFIG_DRV_GFX_DISP_REQUIRE_CALIBRATION_POINTS == true>
<#if CONFIG_USE_GFX_TRULY_7_800X480 == true>
#define TOUCHCAL_ULX                             0x00ad
#define TOUCHCAL_ULY                             0x033f
#define TOUCHCAL_URX                             0x03ac
#define TOUCHCAL_URY                             0x0363
#define TOUCHCAL_LLX                             0x0081
#define TOUCHCAL_LLY                             0x00e2
#define TOUCHCAL_LRX                             0x0378
#define TOUCHCAL_LRY                             0x00d4
<#elseif CONFIG_USE_GFX_TRULY_57_640X480 == true>
#define TOUCHCAL_ULX                             0x00aa
#define TOUCHCAL_ULY                             0x0349
#define TOUCHCAL_URX                             0x038a
#define TOUCHCAL_URY                             0x0349
#define TOUCHCAL_LLX                             0x00a8
#define TOUCHCAL_LLY                             0x00ce
#define TOUCHCAL_LRX                             0x0374
#define TOUCHCAL_LRY                             0x00ce
<#elseif CONFIG_USE_GFX_TRULY_32_240X320 == true>
#define TOUCHCAL_ULX                             0x00C0
#define TOUCHCAL_ULY                             0x00D4
#define TOUCHCAL_URX                             0x037C
#define TOUCHCAL_URY                             0x00D7
#define TOUCHCAL_LLX                             0x00B9
#define TOUCHCAL_LLY                             0x0350
#define TOUCHCAL_LRX                             0x035A
#define TOUCHCAL_LRY                             0x0333
<#elseif CONFIG_USE_GFX_POWERTIP_43_480X272 == true>
#define TOUCHCAL_ULX                             0x0099
#define TOUCHCAL_ULY                             0x00FF
#define TOUCHCAL_URX                             0x0369
#define TOUCHCAL_URY                             0x00F1
#define TOUCHCAL_LLX                             0x00AC
#define TOUCHCAL_LLY                             0x032A
#define TOUCHCAL_LRX                             0x0368
#define TOUCHCAL_LRY                             0x033C
</#if>

</#if>
// DOM-IGNORE-END
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->

