/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "uhmi_common.h"
#include "system_definitions.h"

#include "gfx_hgc_definitions.h"
#include "gfx/gfx_types_resource.h"
#include "gfx/gfx_primitive.h"
#include "gfx_resources_int.h"
#include "mcu_ui.h"

#define MCU_SW_VER  116
#define MCU_MAJOR   MCU_MAJOR_PIC32EFE
#define MCU_MINOR   MCU_MINOR_EXC

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

/* 
 * port and structure definition for 3 keys 
 */  
//mid state of key check 
#define KEY_MID_INIT        0
#define KEY_MID_DISSHAKE    1
#define KEY_MID_TIMER       2
#define KEY_MID_WAIT        3
    
//key return state
#define KEY_NONE    0
#define KEY_SINGLE  1
#define KEY_DOUBLE  2    
#define KEY_LONG    3
    
//port and channel definition
#define KEY_RIGHT_CHANNEL               PORT_CHANNEL_A    //RA
#define KEY_LEFT_HOME_CHANNEL           PORT_CHANNEL_C    //RC   
#define KEY_RIGHT_PORT_BIT              PORTS_BIT_POS_5   //RA5, pin 2,SW3
#define KEY_HOME_PORT_BIT               PORTS_BIT_POS_1   //RC1, pin 6,SW2
#define KEY_LEFT_PORT_BIT               PORTS_BIT_POS_2   //RC2, ping 7,SW1   

//polling period and long press timeout
#define KEY_POLLING_PERIOD          40      // milliseconds
#define LONG_TIMEROUT               30      //40 * 30 = 1200 ms

//key structure definition
typedef struct
{
    char state;
    char midstate;
    char timeoutCount;
}Key;

/* Macro for dimming pin, RPF8 */
#define APP_SCREEN_DIMMING_CHNNEL     PORT_CHANNEL_F
#define APP_SCREEN_DIMMING_PIN        PORTS_BIT_POS_8
    
/* interval of boot animation refresh */
#define APP_BOOT_ANIMATION_INTERVAL     120 //ms

/* definition for uart buffer */
#define PACKET_SIZE     1024
#define RX_BUF_SIZE     1024
#define TX_BUF_SIZE     1024

#define REFRESH_PERIOD  25  //25 * 40ms = 1s

extern u8 rx_buff[RX_BUF_SIZE];
extern u32 rxlen;
extern u8 Escape;

extern u8 tx_buff[TX_BUF_SIZE];
extern u32 txlen;

extern u8 refresh_count;
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,
	APP_STATE_SERVICE_TASKS,

	/* TODO: Define states used by the application state machine. */
    APP_STATE_ANIMATION,
    APP_STATE_REFRESH,
    APP_STATE_SPEED_REFRESH,
} APP_STATES;

typedef enum
{
    USART_INIT = 0,
    USART_RUNNING,
    USART_DONE,
    USART_WRONG,
}USART_STATES;
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;
    uint8_t cur_screen;

    /* TODO: Define any additional data used by the application. */

    /* the timer and structure for 3 keys */
    SYS_TMR_HANDLE  tmrServiceHandle;
    Key Key_Home;
    Key Key_Left;
    Key Key_Right;
    
    /* USART */
    USART_STATES    usart_state;
    DRV_HANDLE handleUSART0;
    char echo;                              //rx receive byte in each interrupt
    bool cflag;                             //rx buffer receive complete flag
    
    DATA_INTERFACE_STATUS_T ifstatus;
    DATA_HOME_T             homeinfo;
    DATA_HOSTDEV_T          hostlist;
    DATA_WEATHER_T          weatherinfo;
    DATA_WIFI_T             wificfg;    
    DATA_SYSINFO_T          systeminfo; 
    EVENT_TYPE_E            eventtype;
   
    TURN_AROUND home_tr;
    TURN_AROUND alert_tr;
    unsigned char screen_state;
    unsigned char count;
    
    bool          ScreenOn;       //screen on or off
    bool          changFlag;      //if the new data is different from the last one
    bool          homeInfoChangFlag;
    /* global */
    //MSG_TYPE_E msg_type;
    
} APP_DATA;

extern APP_DATA appData;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/
/* key driver, return key action:single click or long click */
unsigned char key_driver(Key* myKey, int port);

/* function about rx data and signal handle*/
/* load usart data to buffer */
u8 load_rx_to_buffer(u8 data, u32* qlen, u8* buffer, u8* escape);
/* handle rxbuf, check crc, check prot ver, unpackage via MSG_TYPE and load in */
u8 rxbuf_handle(u8* package, u32 plen);

/* function about build and send tx buffer */
u32 build_tx_buffer(u8* packet, u32 plen, u8* txbuf, MSG_TYPE_E msg_type);
u32 send_tx_buffer(u8* packet, u32 plen);

/* send mcu version */
void send_mcu_version(void);
/* send button msg, invalid return */
u8 send_button_signal(u32 button_signal);
/* send event msg, invalid return */
u8 send_event_signal(u32 event_signal);
/* handle event msg signal, invalid return */
u8 msg_event_handle(u32 signal);

/* function for draw */
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the 
    application in its initial state and prepares it to run so that its 
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );

#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */
//#define DEBUG 1
