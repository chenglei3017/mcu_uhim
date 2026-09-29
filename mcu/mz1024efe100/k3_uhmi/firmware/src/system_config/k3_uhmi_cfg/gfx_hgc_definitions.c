/*******************************************************************************
  MPLAB Harmony Graphics Composer Generated Implementation File

  File Name:
    gfx_hgc_definitions.c

  Summary:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

  Description:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

    Created with MPLAB Harmony Version 1.08
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#include "gfx_hgc_definitions.h"
#include "mcu_ui.h"

/*** Generated Asset References ***/
extern const GFX_RESOURCE_HDR icon_download_white;
extern const GFX_RESOURCE_HDR icon_upload_white;
extern const GFX_RESOURCE_HDR icon_1jia;
extern const GFX_RESOURCE_HDR icon_360;
extern const GFX_RESOURCE_HDR icon_asus;
extern const GFX_RESOURCE_HDR icon_coolpad;
extern const GFX_RESOURCE_HDR icon_dell;
extern const GFX_RESOURCE_HDR icon_haier;
extern const GFX_RESOURCE_HDR icon_hasee;
extern const GFX_RESOURCE_HDR icon_honor;
extern const GFX_RESOURCE_HDR icon_hp;
extern const GFX_RESOURCE_HDR icon_htc;
extern const GFX_RESOURCE_HDR icon_huawei;
extern const GFX_RESOURCE_HDR icon_iPhone;
extern const GFX_RESOURCE_HDR icon_lenovo;
extern const GFX_RESOURCE_HDR icon_letv;
extern const GFX_RESOURCE_HDR icon_lg;
extern const GFX_RESOURCE_HDR icon_meitu;
extern const GFX_RESOURCE_HDR icon_meizu;
extern const GFX_RESOURCE_HDR icon_oppo;
extern const GFX_RESOURCE_HDR icon_phicomm;
extern const GFX_RESOURCE_HDR icon_samsung;
extern const GFX_RESOURCE_HDR icon_smartisan;
extern const GFX_RESOURCE_HDR icon_sony;
extern const GFX_RESOURCE_HDR icon_tcl;
extern const GFX_RESOURCE_HDR icon_thinkpad;
extern const GFX_RESOURCE_HDR icon_tongfang;
extern const GFX_RESOURCE_HDR icon_vivo;
extern const GFX_RESOURCE_HDR icon_windowsphone;
extern const GFX_RESOURCE_HDR icon_xiaomi;
extern const GFX_RESOURCE_HDR icon_zte;
extern const GFX_RESOURCE_HDR icon_hint_disconnected;
extern const GFX_RESOURCE_HDR icon_information_background_double;
extern const GFX_RESOURCE_HDR icon_information_background_single;
extern const GFX_RESOURCE_HDR icon_recovery;
extern const GFX_RESOURCE_HDR icon_wifi_close;
extern const GFX_RESOURCE_HDR icon_wifi_open;
extern const GFX_RESOURCE_HDR icon_default;
extern const GFX_RESOURCE_HDR icon_two_dimensional_code;
extern const GFX_RESOURCE_HDR phicomm;
extern const GFX_RESOURCE_HDR w_cloudy;
extern const GFX_RESOURCE_HDR w_dust;
extern const GFX_RESOURCE_HDR w_foggy;
extern const GFX_RESOURCE_HDR w_haze;
extern const GFX_RESOURCE_HDR w_overcast;
extern const GFX_RESOURCE_HDR w_rain;
extern const GFX_RESOURCE_HDR w_windy;
extern const GFX_RESOURCE_HDR w_sun;
extern const GFX_RESOURCE_HDR w_snow;
extern const GFX_RESOURCE_HDR w_unknown;
extern const GFX_RESOURCE_HDR icon_update;
extern const GFX_RESOURCE_HDR w_locationfailed;
extern const GFX_RESOURCE_HDR icon_wifi;
extern const GFX_RESOURCE_HDR icon_usb_link;
extern const GFX_RESOURCE_HDR icon_visitor;
extern const GFX_RESOURCE_HDR icon_apmode;
extern const GFX_RESOURCE_HDR font18;
extern const GFX_RESOURCE_HDR font26;
extern const GFX_RESOURCE_HDR font14;
extern const GFX_RESOURCE_HDR font20;
extern const GFX_RESOURCE_HDR font48;
extern const GFX_RESOURCE_HDR font16;
extern const GFX_RESOURCE_HDR font22;
extern const GFX_RESOURCE_HDR alert_font24;
extern const GFX_RESOURCE_HDR city18;
extern const GFX_RESOURCE_HDR weather18;
extern const GFX_RESOURCE_HDR font_pf42;

/*** Generated ASCII Text Labels ***/
/*** HGC Object Global ***/
HGC_OBJECTS hgcObj;
static HGC_STATES hgcState;

/******************************************************************************
  Function:
    HGC_SCREEN_STATES GFX_HGC_GetScreenState ( void )

  Remarks:
    This function returns the screen state
 */
HGC_SCREEN_STATES GFX_HGC_GetScreenState ( void )
{
    return hgcObj.screenState;
}


/******************************************************************************
  Function:
    void GFX_HGC_SetScreenState ( HGC_SCREEN_STATES newState )

  Remarks:
    This function sets the screen state machine to a new state
 */
void GFX_HGC_SetScreenState ( HGC_SCREEN_STATES newState )
{
    hgcObj.prevRefreshState = hgcObj.screenState;
    hgcObj.screenState = newState;
}


/******************************************************************************
  Function:
    void GFX_HGC_Initialize ( void )

  Summary:
    This function initializes the HGC state machine.  
 */
void GFX_HGC_Initialize ( void )
{
    hgcState = HGC_STATE_INIT;
    hgcObj.screenState = HGC_SCREEN_STATE_INIT;
}


/******************************************************************************
  Function:
    void GFX_HGC_Setup ( void )

  Summary:
    This function sets up the GOL message callback and draw callbacks.  
 */
void GFX_HGC_Setup ( void )
{
}

/******************************************************************************
  Function:
    void GFX_HGC_Tasks (SYS_MODULE_OBJ gfxObject);

  Summary:
    This function is called in SYS_Tasks.  The intent wait until the GFX library
    is initialized before supplying items to draw
*/
void GFX_HGC_Tasks  (SYS_MODULE_OBJ gfxObject)
{
    switch ( hgcState )
    {
        case HGC_STATE_INIT:
            if (GFX_Status(gfxObject)==SYS_STATUS_READY)
            {
                GFX_HGC_Setup();
                hgcState = HGC_STATE_RUNNING;
            }
            break;

        case HGC_STATE_RUNNING:
            //Drive screen state machine
            GFX_HGC_DrawScreenTask();
            break;

        default:
            break;
    }
}

/******************************************************************************
  Function: 
    void GFX_HGC_DrawScreenTask( void )

  Remarks: 
    Task loop to drive the HGC generated screen state machine for HGC design
    that has primitives only
 */
void GFX_HGC_DrawScreenTask( void )
{
    switch (hgcObj.screenState)
    {
        case HGC_SCREEN_STATE_INIT:
            //Draw the primary screen as selected in the Composer
            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_SETUP_SCREEN_screen);
            break;
        case HGC_SCREEN_STATE_SETUP_SCREEN_screen:
            GFX_HGC_SetupScreen(screen);

            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_PRE_DRAW_PRIMITIVE_SCREEN_screen);
            break;
        case HGC_SCREEN_STATE_PRE_DRAW_PRIMITIVE_SCREEN_screen:
            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_DRAW_PRIMITIVE_SCREEN_screen);
            break;
        case HGC_SCREEN_STATE_DRAW_PRIMITIVE_SCREEN_screen:
            GFX_HGC_DrawScreen_Primitives(screen);
            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_POST_DRAW_PRIMITIVE_SCREEN_screen);
            break;
        case HGC_SCREEN_STATE_POST_DRAW_PRIMITIVE_SCREEN_screen:
            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_DISPLAY_SCREEN_screen);
            break;
        case HGC_SCREEN_STATE_DISPLAY_SCREEN_screen:
            break;
        default:
            break;
    }
}

/******************************************************************************
  Function: 
    void GFX_HGC_SetupScreen( uint8_t  screenId )

  Remarks: 
    Clears current screen and starts a fresh screen with its background color
 */
void GFX_HGC_SetupScreen(uint8_t screenId)
{
    switch (screenId)
    {
        case screen:
            GFX_ColorSet(GFX_INDEX_0, GFX_RGBConvert(0x00,0x00,0x00));
            GFX_ScreenClear(GFX_INDEX_0);
			
            break;
        default:
            break;
    }
}


/******************************************************************************
  Function: 
    bool GFX_HGC_HasCurrentScreenDrawn( void )

  Output: 
    Returns true if the current screen is completely drawn

  Remarks: 
    Allows application to know if the current screen is finished drawing
 */
bool GFX_HGC_HasCurrentScreenDrawn( void )
{
    switch (hgcObj.screenState)
    {
        case HGC_SCREEN_STATE_DISPLAY_SCREEN_screen:
            return true;
        default:
            return false;
    }

    return false;
}


/******************************************************************************
  Function: 
    bool GFX_HGC_ChangeScreen( uint8_t  screenId )

  Output: 
    Returns true if a screenId matches an existing screen identifier

  Remarks: 
    Can be called to initiate GFX Library to draw a new screen
 */
bool GFX_HGC_ChangeScreen(uint8_t screenId)
{
    switch (screenId)
    {
        case screen:
            GFX_HGC_SetScreenState(HGC_SCREEN_STATE_SETUP_SCREEN_screen);
            break;
        default:
            return false;
    }

    return true; // release drawing control to GOL
}

/******************************************************************************
  Function: 
    bool GFX_HGC_DrawScreen_Primitives( uint8_t  screenId )

  Output:
    Returns true if a screenId matches an existing screen identifier

  Remarks: 
    HGC-specified GFX Primitives are drawn here, grouped by screen.
    GFX Primitives are drawn after GFX GOL Objects to make sure GFX GOL Objects
    do not cover GFX Primitives
 */
bool GFX_HGC_DrawScreen_Primitives(uint8_t screenId)
{
    switch (screenId)
    {
        case screen:
            break;
        default:
            return true;
    } 

    return true; // release drawing control to GOL
}


/******************************************************************************
  Function: 
    bool GFX_HGC_DrawItem(int itemId)

  Output:
    Returns true if a itemId matches an existing item identifier

  Remarks: 
    Every item specified in every screen in HGC is listed in this function
 */
bool GFX_HGC_DrawItem(int itemId)
{
    switch(itemId)
    {
	default:
		return false; // process by default
    }

    return true;
}


