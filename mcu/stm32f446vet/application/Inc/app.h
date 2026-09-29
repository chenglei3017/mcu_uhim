#ifndef _APP_H
#define _APP_H

/* included files */
#include "main.h"
//#include "stm32f4xx_hal_uart.h"
//#include "stm32f4xx_hal_tim.h"
#include "uhmi_common.h"
#include "stm32f4xx_hal.h"
#include "mcu_ui.h"
/******************Softwares version***************************/
#define MCU_SW_VER 120
#define MCU_MAJOR MCU_MAJOR_ST446
#define MCU_MINOR MCU_MINOR_EXC
/******************functions called from extern***************************/


/* ******************** common definition ***************************/
#define bool	_Bool
#define true	1
#define false	0

/********************* definition for timer **************************/
#define SYS_CLK_PLL      90000000       //APB1 timer clock 90mHz
#define TIM3_PRESCALER   9000           //prescaler 9000, generate 10kHz timer clock
#define TIM3_PEROID      200            //auto reload register, 200/10kHz = 20ms

extern void Tim3_CallBack(void);
extern void Tim4_CallBack(void);
/********************* definition for key **************************/
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

//pin and channel definition
#define KEY_HOME_CHANNEL            SW1_SET_GPIO_Port      //PA
#define KEY_LEFT_CHANNEL            SW2_UP_GPIO_Port        //PA
#define KEY_RIGHT_CHANNEL           SW3_DOWN_GPIO_Port      //PA

#define KEY_HOME_BIT           SW_HOME_Pin             //PA5, SW1,set
#define KEY_LEFT_BIT           SW_LEFT_Pin              //PA6, SW2,up
#define KEY_RIGHT_BIT          SW_RIGHT_Pin            //PA7, SW3,down

//polling period and long press timeout
#define KEY_POLLING_PERIOD          20      // milliseconds, for timer
#define LONG_TIMEROUT               30      //20 * 30 = 600 ms

//key structure definition
typedef struct
{
    char state;
    char midstate;
    char timeoutCount;
}Key;


/******************** definition for screen ************************/
/* Macro for dimming pin, RPF8 */
#define APP_SCREEN_DIMMING_PORT       BL_EN_GPIO_Port     //GPIOD
#define APP_SCREEN_DIMMING_PIN        BL_EN_Pin           //PD3

#define REFRESH_PERIOD  25  //25 * 20ms = 0.5s,period for refresh page



/*** Screen IDs ***/
enum SCREEN_ID{
   welcome=0,
   softupdate,
   sysinfo,
   sysinterface,
   home,
   weather,
   wifi,
   devlist,
   alert
};


/*** Image IDs ***/
#define welcome_logo                 18000



/******************** definition for uart ***************************/
/* definition for uart buffer */
#define PACKET_SIZE     1024
#define RX_BUF_SIZE     1024
#define TX_BUF_SIZE     1024


extern u8 packet[PACKET_SIZE];
extern u32 pkt_len;
extern u8 Button_IC_Type;
extern u8 Escape;
typedef enum
{
    USART_INIT = 0,
    USART_RUNNING,
    USART_DONE,
    USART_WRONG,
}USART_STATES;
extern void load_rxbyte_to_packet(u8 data, u8* buffer, u32* qlen, u8* escape);
/******************** definition for IWDG************************/
#define IWDG_MAX_COUNT   15      //15 * 40ms = 600ms

extern u8 IWDG_count;
extern u8 IWDG_resumed_state;
/******************** definition for APP state and data************************/
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

typedef struct
{
    /* The application's current state */
    APP_STATES state;
    uint8_t cur_screen;

    /* TODO: Define any additional data used by the application. */

    /* the timer and structure for 3 keys */
    Key Key_Home;
    Key Key_Left;
    Key Key_Right;

    /* USART */
    USART_STATES    usart_state;
    char echo;                              //rx receive byte in each interrupt
    bool cflag;                             //rx buffer receive complete flag

    DATA_INTERFACE_STATUS_T ifstatus;
    DATA_HOME_T             homeinfo;
    DATA_HOSTDEV_T          hostlist;
    DATA_WIFI_T             wificfg;
    DATA_SYSINFO_T          systeminfo;
    DATA_WEATHER_T          weatherinfo;
    EVENT_TYPE_E            eventtype;
    TURN_AROUND home_tr;
    TURN_AROUND alert_tr;
    U8 screen_state;
    U8 count;
    U8 resumed_state;        //0:start from normal state,1:resumed from IWDG

    bool          ScreenOn;       //screen on or off
    bool          changFlag;      //if the new data is different from the last one
    bool          homeInfoChangFlag;
    bool          key_en;           //1: enable key, 0: disable key(default)
    /* global */
    //MSG_TYPE_E msg_type;




} APP_DATA;

extern APP_DATA appData;
/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     application initialization routine.

  Description:
    This function initializes the application.  It places the
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
    application tasks function

  Description:
    This routine is the application's tasks function.  It
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

#endif
