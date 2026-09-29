/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx.h

  Summary:
    The header file joins all header files used in the graphics library
    and contains compile options and defaults.

  Description:
    This header file includes all the header files required to use the
    Microchip Graphics Library. Library features and options defined
    in the Graphics Library configurations will be included in each build.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _GFX_H
// DOM-IGNORE-BEGIN
    #define _GFX_H
// DOM-IGNORE-END


/////////////////////// GRAPHICS_LIBRARY_VERSION /////////////////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION    0x0400

////////////////////////////// INCLUDES //////////////////////////////
#include "system_config.h"
#include "driver/gfx/gfx_common.h"
#include "gfx/gfx_types_macros.h"           
#include "gfx/gfx_primitive.h"
#include "gfx/gfx_colors.h"

#ifndef GFX_CONFIG_GOL_DISABLE
#include "gfx/gfx_gol_scheme.h"
#include "gfx/gfx_gol.h"
#include "gfx/gfx_gol_scan_codes.h"
#include "gfx/gfx_gol_button.h"
#include "gfx/gfx_gol_check_box.h"
#include "gfx/gfx_gol_digital_meter.h"
#include "gfx/gfx_gol_edit_box.h"
#include "gfx/gfx_gol_group_box.h"
#include "gfx/gfx_gol_list_box.h"
#include "gfx/gfx_gol_meter.h"
#include "gfx/gfx_gol_picture.h"
#include "gfx/gfx_gol_custom_control.h"
#include "gfx/gfx_gol_progress_bar.h"
#include "gfx/gfx_gol_radio_button.h"
#include "gfx/gfx_gol_scroll_bar.h"
#include "gfx/gfx_gol_static_text.h"
#include "gfx/gfx_gol_surface.h"
#include "gfx/gfx_gol_text_entry.h"
#include "gfx/gfx_gol_window.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif
        
// *****************************************************************************
/* GFX Machine States

   Summary
    Defines the various states that can be achieved by the GFX operation.

   Description
    This enumeration defines the various states that can be achieved by the
    HAL operation.

   Remarks:
    None.
*/

typedef enum
{
    /*  Module state busy */
    GFX_STATE_BUSY,

    /*  Module state init */
    GFX_STATE_INIT,

    GFX_STATE_INITIALIZED,

} GFX_STATES;

// *****************************************************************************
/*  GFX Client Status

  Summary:
    Enumerated data type that identifies the  GFX Module Client Status.

  Description:
    This enumeration defines the possible status of the  GFX Module Client.
    It is returned by the () function.

  Remarks:
    None.
*/

typedef enum
{
     /* Client is closed or the specified handle is invalid */
    GFX_CLIENT_STATUS_CLOSED
            /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

    /* Client is ready */
    GFX_CLIENT_STATUS_READY
            /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/

} GFX_CLIENT_STATUS;

// *****************************************************************************
/*  GFX client object structure

  Summary:
    GFX client object structure.

  Description:
    GFX client object structure.

  Remarks:
   None.
*/

typedef struct
{
    /* Client status */
    GFX_CLIENT_STATUS         clientState;

    /* Set to true if this object is in use */
    bool                          inUse;

    /* Object from which the client is derived */
    void *                        obj;

//    DRV_GFX_INTERFACE *           drvInterface;

    /* Save the index of the driver */
    SYS_MODULE_INDEX              halIndex;

    GFX_HANDLE                 primitiveHandle;
    GFX_HANDLE                 golHandle;

} GFX_CLIENT_OBJ;

// *****************************************************************************
/* GFX task states

  Summary
    Lists the different states that GFX task routine can have.

  Description
    This enumeration lists the different states that GFX task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    /* open module for client */
    GFX_TASK_STATE_OPEN_MODULE,

    GFX_TASK_STATE_RUNNING,

} GFX_OBJECT_TASK;

// *****************************************************************************
/* GFX Library Instance Object

  Summary:
    Defines the object required for interaction with the GFX Library.

  Description:
    This defines the object required for interaction with the GFX Library.
    This object exists once per application instance of the library.

  Remarks:
    None.
*/

typedef struct _GFX_OBJ
{

   /* Required : The status of the module */
    SYS_STATUS                                              status;

    // /* Object Index */
    SYS_MODULE_INDEX                                        index;

    /* Required : Flag to indicate in use  */
    bool                                                    inUse;

    /* GFX machine state */
    GFX_STATES                                              state;

    /* Flag to indicate that PMP is used in exclusive access mode */
    bool                                                    IsExclusive;

    /* Optional: number of clients possible with the software instance */
    uint8_t                                                 numClients;

    /* State of the task */
    GFX_OBJECT_TASK    		                            task;

    GFX_INIT                                              * initData;

    GFX_HANDLE                                            gfxHandle;

    SYS_MODULE_OBJ                                         primitiveObject;
    SYS_MODULE_OBJ                                         golObject;


} GFX_OBJ;

// *****************************************************************************
/*
    Function:
        void GFX_Initialize (const SYS_MODULE_INDEX   moduleIndex,
                             const SYS_MODULE_INIT    * const moduleInit);

    Summary:
        Initialize the GFX Library.

    Description:
        This function initialize the GFX Library primitive layer and
        Object Layer if enabled. The following default settings
        are set when this function is called.
        1. font - Set to NULL. GFX_FontSet() must be called prior to any text
                 rendering.
        2. line type - Set to GFX_LINE_TYPE_THIN_SOLID (see GFX_LINE_STYLE).
        3. fill type - Set to GFX_FILL_TYPE_COLOR (see GFX_FILL_STYLE).
        4. text anti-alias type - Set to GFX_FONT_ANTIALIAS_OPAQUE
                 (see  GFX_FONT_ANTIALIAS_TYPE). This only
                 affects fonts with anti-aliasing enabled.
        5. Set transparent color feature in image draw functions
           to be disabled.
        6. Set alpha blending value to 100 (or no alpha blending) if
           alpha blending feature is enabled.
        7. Set background information to no background.

        This function does not clear the screen and does not assign
        any color to the currently set color. Application should
        set the color and clear the screen.

    Precondition:
        None.

    Parameters:
        moduleIndex - client instance request.

        moduleInit  - initialization data for the instance.

    Returns:
        If successful, returns a valid handle to a device layer object.
        Otherwise, it returns SYS_MODULE_OBJ_INVALID.

    Example:
        <code>
            GFX_INIT gfxInit;
            SYS_MODULE_OBJ gfxObj;

            // GFX Module initialization
            gfxObj = GFX_Initialize(GFX_INDEX_0, &gfxInit);

            if (SYS_MODULE_OBJ_INVALID == gfxObj)
            {
                // Handle error
            }
            </code>

  Remarks:
    This routine must be called before other GFX library functions.

*/
// *****************************************************************************
SYS_MODULE_OBJ  GFX_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                               const SYS_MODULE_INIT    * const moduleInit);

// *****************************************************************************
/* Function:
    void GFX_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the GFX Module.

  Description:
    Deinitializes the specified instance of the GFX Module. This function
    will also deinitialize the GOL, PRIMITIVE, and driver and conclude all GFX
    related operations. All internal data structures will be reset.

  Precondition:
    Function GFX_Deinitialize should have been called before calling this
    function.

  Parameters:
    object          - GFX layer object handle, returned from the
                      GFX_Initialize routine

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     //  Returned from GFX_Initialize
    SYS_STATUS          status;


    GFX_Deinitialize(object);

    status = GFX_Status(object);
    if (SYS_MODULE_DEINITIALIZED != status)
    {
        // Check again later if you need to know
        // when the driver is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize operation
    must be called before the Initialize operation can be called again. This
    routine will NEVER block waiting for hardware.
*/
void GFX_Deinitialize          (SYS_MODULE_OBJ moduleObject);

// *****************************************************************************
/*
  Function: GFX_HANDLE GFX_Open( SYS_MODULE_OBJ gfxObject )

Summary:
    Opens the specified GFX library instance and returns a handle to it.

  Description:
    This function opens the specified GFX library instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Precondition:
    The GFX_Initialize function must have been called before calling this
    function.

  Parameters:
    object      - Identifier for the object instance to be opened

  Returns:
    If successful, the function returns a valid open instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is GFX_HANDLE_INVALID.

  Example:
    <code>

    GFX_HANDLE  handle;

    handle = GFX_Open ( gfxObject );

    if ( GFX_HANDLE_INVALID == handle )
    {
        // Unable to open the library
    }
    </code>

  Remarks:
    The handle returned is valid until the GFX_Close function is called.

*/
GFX_HANDLE GFX_Open( SYS_MODULE_OBJ gfxObject );

// *****************************************************************************
/* Function:
    void GFX_Tasks (SYS_MODULE_INDEX index);

  Summary:
    Maintains the GFX module state machine. It manages the GFX Module object list
    items and responds to GFX Module primitive events.

  Description:
    Maintains the GFX module state machine. It manages the GFX Module object list
    items and responds to GFX Module primitive events.  This
    function should be called from the SYS_Tasks() function.

  Precondition:
     None.

  Parameters:
    index      - Object index for the specified module instance.

  Returns:
    None.

  Example:
    <code>

    while (true)
    {
        GFX_Tasks (GFX_INDEX_0);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application.
*/
void GFX_Tasks (SYS_MODULE_OBJ gfxObject);

// *****************************************************************************
/* Function:
    SYS_STATUS GFX_Status (SYS_MODULE_OBJ gfxObject )

  Summary:
    Gets the current status of the GFX Module.

  Description:
    This routine provides the current status of the GFX Module.

  Precondition:
    Function GFX_Initialize should have been called before calling this
    function.

  Parameters:
    object          - GFX object index, returned from the
                      GFX_Initialize routine

  Returns:
    SYS_STATUS_READY          - Indicates that the GFX layer is ready for
                                operations.

    SYS_STATUS_DEINITIALIZED  - Indicates that the driver has been deinitialized.

  Example:
    <code>
    SYS_MODULE_INDEX    index;     // Returned from GFX_Initialize
    SYS_STATUS          status;

    status = GFX_Status (object);
    if (SYS_STATUS_READY == status)
    {
        // This means the library can be used, using for example
        // GFX_DrawLine() function.
    }
    </code>

  Remarks:
    None.
*/
SYS_STATUS GFX_Status (SYS_MODULE_OBJ gfxObject );

// *****************************************************************************
/* Function:
    void GFX_Close( GFX_HANDLE handle )

  Summary:
    Closes a client instance of the GFX library.

  Description:
    This routine closes a client instance of the GFX library.

  Precondition:
    The GFX_Open routine must have been called for the specified
    handle.

  Parameters:
    handle       - A valid handle, returned from the open routine

  Returns:
    None

  Example:
    <code>
    GFX_HANDLE handle;  // Returned from GFX_Open

    GFX_Close(handle);
    </code>

  Remarks:

*/

void        GFX_Close              (GFX_HANDLE gfxHandle);

#ifdef __cplusplus
    }
#endif
    
#endif // _GFX_H
