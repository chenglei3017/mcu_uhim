/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "gfx_hgc_definitions.h"
#include "math.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

/* rx and tx buffer */
u8 rx_buff[RX_BUF_SIZE];
u32 rxlen;
u8 Escape;

u8 tx_buff[TX_BUF_SIZE];
u32 txlen;
u8 refresh_count;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
u8 key_driver(Key* myKey, int port)
{
    unsigned char key_press, key_return;
    if (port == KEY_RIGHT_PORT_BIT)
    {
         key_press = PLIB_PORTS_PinGet(PORTS_ID_0, KEY_RIGHT_CHANNEL, port);
    }
    else
    {
         key_press = PLIB_PORTS_PinGet(PORTS_ID_0, KEY_LEFT_HOME_CHANNEL, port);
    }
   
    key_return = KEY_NONE;

    switch (myKey->midstate)

    {
        case KEY_MID_INIT: 
            if (!key_press) myKey->midstate = KEY_MID_DISSHAKE;
        break;

        case KEY_MID_DISSHAKE: 
            if (!key_press)
            {
                myKey->timeoutCount = 0; 
                myKey->midstate = KEY_MID_TIMER; 
            }
            else myKey->midstate = KEY_MID_INIT; 
            break;

        case KEY_MID_TIMER:
            if(key_press)
            {   
                key_return = KEY_SINGLE;
                myKey->midstate = KEY_MID_INIT; 
            }

            else if (++(myKey->timeoutCount) >= LONG_TIMEROUT) 
            {
                key_return = KEY_LONG; 
                myKey->midstate = KEY_MID_WAIT; 
            }
            break;

        case KEY_MID_WAIT: 
            if (key_press) myKey->midstate = KEY_MID_INIT; 
        break;
    }

    return key_return;

}

u8 key_event();

void TimerCallBack(uintptr_t context, uint32_t tickCount)
{
    
    /* refresh page content every 1s */
     if(hgcObj.screenState == HGC_SCREEN_STATE_DISPLAY_SCREEN_screen){
        ChangeScreen();
    }
    
    refresh_count++;
    if (refresh_count > REFRESH_PERIOD)
    {
        if((appData.cur_screen == home) || (appData.cur_screen == sysinterface) \
             || (appData.cur_screen == wifi) || (appData.cur_screen == devlist) \
             || (appData.cur_screen == weather) || (appData.cur_screen == softupdate)\
             || (appData.cur_screen == sysinfo) )
        {
                switch(appData.cur_screen)
                {
                    case home:
                    {
                        if (appData.changFlag == true)
                        {
                            appData.state = APP_STATE_REFRESH;
                        }
                        else if (appData.homeInfoChangFlag == true)
                        {
                            appData.state = APP_STATE_SPEED_REFRESH;
                        }
                    }
                        break;
                    default:
                    {
                        if (appData.changFlag == true)
                        {
                            appData.state = APP_STATE_REFRESH;
                        }
                    }
                        break;
                }
        }
		refresh_count = 0;
    }
    
    
    appData.Key_Home.state = key_driver(&(appData.Key_Home), KEY_HOME_PORT_BIT);
    appData.Key_Left.state = key_driver(&(appData.Key_Left), KEY_LEFT_PORT_BIT);
    appData.Key_Right.state = key_driver(&(appData.Key_Right), KEY_RIGHT_PORT_BIT);
    switch(appData.Key_Home.state)
    {
        case KEY_SINGLE:
            send_button_signal(BT_M_SCLICK);
            DRV_USART0_WriteByte('h');
           
            break;
            
        case KEY_LONG:
            send_button_signal(BT_M_LONGCLICK);
            DRV_USART0_WriteByte('H');
#ifdef DEBUG
         SYS_PORTS_PinClear(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(0); 
#endif
            break;
    }
    
    
    switch(appData.Key_Left.state)
    {
        case KEY_SINGLE:
            send_button_signal(BT_L_SCLICK);
            DRV_USART0_WriteByte('l');
          
            break;
            
        case KEY_LONG:
            send_button_signal(BT_L_LONGCLICK);
            DRV_USART0_WriteByte('L');
            break;
    }
    
    switch(appData.Key_Right.state)
    {
        case KEY_SINGLE:
            send_button_signal(BT_R_SCLICK);
            DRV_USART0_WriteByte('r');
            
            break;
            
        case KEY_LONG:
            send_button_signal(BT_R_LONGCLICK);
            DRV_USART0_WriteByte('R');
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/******************************************************************************
  Function:
    static void USART_Task (void)
    
   Remarks:
    Feeds the USART transmitter by reading characters from a specified pipe.  The pipeRead function is a 
    standard interface that allows data to be exchanged between different automatically 
    generated application modules.  Typically, the pipe is connected to the application's
    USART receive function, but could be any other Harmony module which supports the pipe interface. 
*/
static void USART_Task (void)
{
    switch (appData.usart_state)
    {
        case USART_INIT:
            //some initial action
            appData.usart_state = USART_RUNNING;
            break;
        case USART_RUNNING:
        {
            /* echo test */
             if (appData.cflag)
            {
                /* process the rxbuffer, 
                * check the CRC
                * check protocol
                * unpackage vis MSG_TYPE_T, then load it to structure  */
                rxbuf_handle(rx_buff, rxlen);
                
                /* enable usart receive and  when complete */
                SYS_INT_SourceEnable(INT_SOURCE_USART_4_RECEIVE);
                appData.cflag = 0;
            }
            break;
        }
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

#define GetSystemClock()    (SYS_CLK_FREQ)
 static void APP_TMR_DelayMS( uint32_t TMR_DelayMS_in_ms )
 {
    uint32_t tWait = ( GetSystemClock() / 2000 ) * TMR_DelayMS_in_ms;
    uint32_t tStart = _CP0_GET_COUNT();
    while( ( _CP0_GET_COUNT() - tStart ) < tWait );
 }

/* load rx byte in RX interrupt to RX buffer one by one, 
 * then handle the event in USART_Tasks */
u8 load_rx_to_buffer(u8 data, u32* qlen, u8* buffer, u8* escape)
{
    
    switch(data)
    {
        case SOH:
            if (*escape)
            {
                buffer[(*qlen)++] = data;
                *escape = 0;
            }
            else
            {
                (*qlen) = 0;
                *escape = 0;
            }
           break;
        case EOT:
            if (*escape)
            {
                buffer[(*qlen)++] = data;
                *escape = 0;
            }
            else
            {
                //exit,set load complete flag
                appData.cflag = 1;
                //disable rx int
                SYS_INT_SourceDisable(INT_SOURCE_USART_4_RECEIVE);
            }
            break;
        case DLE:
            if (*escape)
            {
                buffer[(*qlen)++] = data;
                *escape = 0;
            }
            else
            {
                *escape = 1;
            }
            break;
       
        default:
            buffer[(*qlen)++] = data;
            *escape = 0;
            break;
    }
}

u8 rxbuf_handle(u8* packet, u32 plen)
{
    u16 crc;
    u8 msg_type;
    u8 ret = 0;
    u8* phead = packet + 2;
    //u8* ptail = packet + plen - 2;
    u32 real_len = plen - 4;
    u32 signal;
    u32 page_id;
    
    
    if(plen > 2)
    {
        /* CRC check */
        crc = (packet[plen-2]) & 0x00ff;
        crc = crc | ((packet[plen-1] << 8) & 0xFF00);
        if(CalculateCrc(packet, (plen-2)) == crc)
        {
            /* check msg_type */
            memcpy(&msg_type, packet + 1, 1);
            switch(msg_type)
            {
                //check version
                case MSG_MCU_VERSION_INFO:
                    //send mcu version info, DATA_MCU_VERSION_T
                    send_mcu_version();
                    appData.changFlag = true;
                    break;
                
                //get page_id and show corrective page
                case MSG_PAGE_ID:
                    memcpy(&page_id, packet + 2, 4);
                    switch(page_id)
                    {
                        case PAGE_SOFTUPDATE:
                            page_id = softupdate;
                            break;
                        case PAGE_SYSINFO:
                            page_id = sysinfo;
                            break;
                        case PAGE_HOME:
                            page_id = home;
                            break;
                        case PAGE_WEATHER:
                            page_id = weather;
                            break;
                        case PAGE_WIFI:
                            page_id = wifi;
                            break;
                        case PAGE_HOSTDEV:
                            page_id = devlist;
                            break;
                        default:
                            break;
                    }
                    if (!SYS_PORTS_PinRead(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN))
                    {
                        SYS_PORTS_PinSet(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
                        ILI9488Display(1); 
                    }
                    if(appData.cur_screen == (uint8_t)(page_id & 0xff))
                    {
                        appData.changFlag = true;
                    }else{
                        appData.cur_screen = (uint8_t)(page_id& 0xff);
                        appData.screen_state = SCREEN_INIT;
                        //GFX_HGC_ChangeScreen(appData.cur_screen);
                    }
                    break;

                //refresh the interface status of router, DATA_INTERFACE_STATUS_T   
                case MSG_INTERFACE_STATUS:
                    if (0 != memcmp(&(appData.ifstatus), phead, sizeof(appData.ifstatus)))
                    {
                        memcpy((u8*)&(appData.ifstatus), phead, sizeof(appData.ifstatus));
                        appData.changFlag = true;
                    }
                    else
                        appData.changFlag = false;
                    break;
                 
                //refresh home page info,DATA_HOME_T in appData    
                case MSG_HOME_INFO:
                    if (0 != memcmp(&(appData.homeinfo), phead, sizeof(appData.homeinfo)))
                    {
                        u8 mode = appData.homeinfo.mode;
                        memcpy((u8*)&(appData.homeinfo), phead, sizeof(appData.homeinfo));
                        appData.homeInfoChangFlag = true;
                        if(mode != appData.homeinfo.mode)
                        {
                            appData.screen_state= SCREEN_INIT;
                        }
                    }
                    else
                        appData.homeInfoChangFlag = false;
                    break;
                 
                //refresh wifi info, DATA_WIFI_T in appData   
                case MSG_WIFI_INFO:   
                {
                    if (0 != memcmp(&(appData.wificfg), phead, sizeof(appData.wificfg)))
                    {
                         memcpy((u8*)&(appData.wificfg), phead, sizeof(appData.wificfg));
                        /* if wifi connect way or ssid change, set change flag */
                        appData.changFlag = true;
                    }
                    else
                    {
                        appData.changFlag = false;
                    }
                    break;
                }
                //refresh hostdev list info, DATA_HOSTDEV_T in appData    
                case MSG_HOSTDEV_INFO:
                    if (0 != memcmp(&(appData.hostlist), phead, sizeof(appData.hostlist)))
                    {
                        memcpy((u8*)&(appData.hostlist), phead, sizeof(appData.hostlist));
                        appData.changFlag = true;
                    }
                    else
                        appData.changFlag = false;
                    break;
                 
                //refresh service info, DATA_SYSINFO_T in appData   
                case MSG_SYSTEM_INFO:
                    if (0 != memcmp(&(appData.systeminfo), phead, sizeof(appData.systeminfo)))
                    {
                        memcpy((u8*)&(appData.systeminfo), phead, sizeof(appData.systeminfo));
                        appData.changFlag = true;
                    }
                    else
                        appData.changFlag = false;
                    break;
                 case MSG_WEATHER_INFO:
                   if (0 != memcmp(&(appData.weatherinfo), phead, sizeof(appData.weatherinfo)))
                    {
                        memcpy((u8*)&(appData.weatherinfo), phead, sizeof(appData.weatherinfo));
                        appData.changFlag = true;
                    }
                   else
                       appData.changFlag = false;
                    break;
                
                //get event signal and handle correlative action   
                case MSG_EVENT:
                    memcpy(&signal, packet + 2, 4);
                    msg_event_handle( signal);
                    break; 
            }
        }
    }
    else
    {
        //erro handle
        ret = 1;
    }
    return ret;
    
}

/* build tx buffer, packet is only the struct string, not including  PROT_VER and msg_type
 * then send tx buffer in bytes one by one*/
u32 build_tx_buffer(u8* packet, u32 plen, u8* txbuf, MSG_TYPE_E msg_type)
{
    if (plen > (PACKET_SIZE - 2))
        return plen;
    
    u32 len = 0;
    u32 dlen = 0;
    u32 i = 0;
    u16 crc16 = 0x0000;
    u8 crc_L = 0x00;
    u8 crc_H = 0x00;
    u8 data[PACKET_SIZE] = {0};

    data[dlen++] = PROT_VER;
    data[dlen++] = msg_type;
//    memcpy(data + dlen, &msg_type, 4);
//    dlen += sizeof(msg_type);
    memcpy(data + dlen, packet, plen);
    dlen += plen;
    
    crc16 = CalculateCrc(data, dlen);
    crc_L = (u8)(crc16);
    crc_H = (u8)(crc16>>8);
 
    txbuf[len++] = SOH;
    
    for (;i < dlen;i++)
    {
        if ((EOT == data[i]) || (SOH == data[i]) || (DLE == data[i]))
        {
            txbuf[len++] = DLE;
        }
        txbuf[len++] = data[i];
    }
    
    
    if ((EOT == crc_L) || (SOH == crc_L) || (DLE == crc_L))
        txbuf[len++] == DLE;
    txbuf[len++] = crc_L;
    
    if ((EOT == crc_H) || (SOH == crc_H) || (DLE == crc_H))
        txbuf[len++] == DLE;
    txbuf[len++] = crc_H;
    
    txbuf[len++] = EOT;
    
    return len;
}

u32 send_tx_buffer(u8* packet, u32 plen)
{
    u8 ret = 0;
    u32 i = 0;
    if (NULL == packet || (plen > TX_BUF_SIZE - 2))
        return 1;
     //echo all buffer data
     for(i = 0;i < plen; i++)
     {
         DRV_USART0_WriteByte(packet[i]);
     } 
    //clear tx_buffer
    memset(packet, 0, plen);
    return ret;
}

/* some integrated command */
void send_mcu_version(void)
{
    DATA_MCU_VERSION_T mcu_ver;
    mcu_ver.swver = MCU_SW_VER;
    mcu_ver.major = MCU_MAJOR;
    mcu_ver.minor = MCU_MINOR;
    
    txlen = build_tx_buffer((u8*)&mcu_ver, sizeof(mcu_ver), tx_buff, MSG_MCU_VERSION_INFO);
    
    send_tx_buffer(tx_buff, txlen);
}

u8 send_event_signal(u32 event_signal)
{
    txlen = build_tx_buffer((u8*)&event_signal, sizeof(event_signal), tx_buff, MSG_EVENT);
    
    send_tx_buffer(tx_buff, txlen);
    
}

u8 send_button_signal(u32 button_signal)
{
    txlen = build_tx_buffer((u8*)&button_signal, sizeof(button_signal), tx_buff, MSG_BUTTON);
    
    send_tx_buffer(tx_buff, txlen);
}

u8 msg_event_handle(u32 signal)
{
    switch(signal)
    {
        
        case SCREEN_ON:
            SYS_PORTS_PinSet(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(1); 
            appData.ScreenOn = true;
            break;
        case SCREEN_OFF:
            SYS_PORTS_PinClear(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(0); 
            appData.ScreenOn = false;
            break;     
        case REBOOT:
            SYS_PORTS_PinSet(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(1); 
            appData.cur_screen = alert;
            appData.screen_state = SCREEN_INIT;
            appData.eventtype = REBOOT;
            break;     
        case FACTORY_RESET:
            SYS_PORTS_PinSet(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(1); 
            appData.cur_screen = alert;
            appData.screen_state = SCREEN_INIT;
            appData.eventtype = FACTORY_RESET;
            break;
        case ROUTER_UPGRADE:
            SYS_PORTS_PinSet(PORTS_ID_0, APP_SCREEN_DIMMING_CHNNEL, APP_SCREEN_DIMMING_PIN);
            ILI9488Display(1); 
            appData.cur_screen = alert;
            appData.screen_state = SCREEN_INIT;
            appData.eventtype = ROUTER_UPGRADE;
            break;
    }
    
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    memset(&appData, 0, sizeof(appData));
    
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    appData.home_tr.timer= 0;
    appData.home_tr.pointx = 160;
    appData.home_tr.pointy = 162;
    appData.home_tr.len = 21;
    appData.home_tr.num = 48;
    appData.home_tr.i_start = 13;
    appData.home_tr.radius = 120;
    appData.home_tr.turn_flag = 1;
    
    appData.alert_tr.timer=0;
    appData.alert_tr.pointx=160;
    appData.alert_tr.pointy=190;
    appData.alert_tr.len=20;
    appData.alert_tr.num=8;
    appData.alert_tr.i_start=0;
    appData.alert_tr.radius=50;
    appData.alert_tr.turn_flag=0;
    
    appData.cur_screen = welcome;
    appData.screen_state = SCREEN_INIT;
    appData.changFlag = true;
    appData.ScreenOn = true;
    appData.homeInfoChangFlag = true;
    
    appData.tmrServiceHandle = DRV_HANDLE_INVALID;
    appData.usart_state = USART_INIT;
    
    /* initial USART RX and TX buffer */
    memset(rx_buff, 0, RX_BUF_SIZE);
    rxlen = 0;
    memset(tx_buff, 0, RX_BUF_SIZE);
    txlen = 0;
    Escape = 0;     //no escape
    refresh_count = 0;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            /* initial timer handle */
            if (appData.tmrServiceHandle == DRV_HANDLE_INVALID)
            {
                appData.tmrServiceHandle = SYS_TMR_ObjectCreate(KEY_POLLING_PERIOD, 1, TimerCallBack, SYS_TMR_FLAG_PERIODIC);
                appInitialized &= ( DRV_HANDLE_INVALID != appData.tmrServiceHandle );
            }
            if (appInitialized)
                appData.state = APP_STATE_ANIMATION;
            break;
        }

        /* boot animation, when complete, change state to service */
        case APP_STATE_ANIMATION:
        {
            //boot animation task, when complete, goto next state
            appData.state = APP_STATE_SERVICE_TASKS;
            break;
        }
           
        case APP_STATE_SERVICE_TASKS:
        {
            /* loop usart_task, first get version check command, 
             * echo mcu version, then wait all data and ready signal to switch to main page */
            USART_Task();
            break;
        }

        /* TODO: implement your application state machine.*/
        case APP_STATE_REFRESH:
        {
            refresh_count = 0;
       //     GFX_HGC_DrawScreen_Primitives(appData.cur_screen);
            appData.state = APP_STATE_SERVICE_TASKS;
            appData.changFlag = false;
            appData.homeInfoChangFlag = false;
            break;
        }

        case APP_STATE_SPEED_REFRESH:
        {
            refresh_count = 0;
       //     GFX_HGC_DrawScreen_Primitives(appData.cur_screen);
            appData.state = APP_STATE_SERVICE_TASKS;
            appData.changFlag = false;
            appData.homeInfoChangFlag = false;
            break;
        }
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
/*******************************************************************************
 End of File
 */
