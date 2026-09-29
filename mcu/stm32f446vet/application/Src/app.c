#include "app.h"
#include "main.h"
#include "mcu_ui.h"

/***************************** global var************************************/
APP_DATA appData;
bool start_flag;
/* rx and tx buffer */
//orignal RX data, with DLE, SOH, EOT, PROT_VER and msg_type in income frame
//u8 rx_buff[RX_BUF_SIZE];
//u32 rxlen = 0;
//valid data, without DLE, SOH, EOT, PROT_VER and msg_type
u8 packet[PACKET_SIZE];
u32 pkt_len;
//cache data for send, used to caculate when make tx_buff
u8 tx_packet[PACKET_SIZE];
u32 txplen;
//TX data to be sent directly, with DLE, SOH, EOT, PROT_VER and msg_type for send
u8 tx_buff[TX_BUF_SIZE];
u32 txlen;

u8 refresh_count;
u8 Escape;
u8 key_value;
u8 Button_IC_Type;        //0 is MHS, 1 is Cypress

u8 IWDG_count;
u8 IWDG_resumed_state;
u8* IWDG_test;
u8 IWDG_c;
/******************* local functions declaration*******************************/
void Tim3_CallBack(void);

static u8 key_driver(Key* myKey, int port);
static u8 rxbuf_handle(u8* packet, u32 plen);
static u32 build_tx_buffer(u8* packet, u32 plen, MSG_TYPE_E msg_type);
static u32 send_tx_buffer(u8* packet, u32 plen);
static void send_mcu_version(void);
static void send_event_signal(u32 event_signal);
static void send_button_signal(u32 button_signal);
static void msg_event_handle(u32 signal);
static void USART_Task (void);
static void send_mcu_resumed_signal(void);
int init_flag = 0;

void load_rxbyte_to_packet(u8 data, u8* buffer, u32* qlen, u8* escape);
/********************functions for keys*****************************************/
static u8 key_driver(Key* myKey, int pin)
{
    unsigned char key_press, key_return;
    key_return = KEY_NONE;
    u8 tmp;

//    if (Button_IC_Type == BUTTON_CYPRESS)
//  {
//      tmp = CY8CMBR3108_ReadButton();
//      if (tmp)
//        UART_SendData(tmp);
//      if (SW_HOME_Pin == pin) key_press = (tmp&0x04)?0:1;
//      else if (SW_LEFT_Pin == pin) key_press = (tmp&0x08)?0:1;
//      else if (SW_RIGHT_Pin == pin) key_press = (tmp&0x01)?0:1;
//    }
//    else if (Button_IC_Type == BUTTON_MSH)
    key_press = HAL_GPIO_ReadPin(SW_HOME_GPIO_Port, pin);

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

/********************timer callback functions*********************************/
//callback run every 20ms(20.24ms actually)
void Tim3_CallBack(void)
{
  /*-------------------key scan--------------------------*/
  appData.Key_Home.state = key_driver(&(appData.Key_Home), SW_HOME_Pin);
  appData.Key_Left.state = key_driver(&(appData.Key_Left), SW_LEFT_Pin);
  appData.Key_Right.state = key_driver(&(appData.Key_Right), SW_RIGHT_Pin);

  switch(appData.Key_Home.state)
  {
    case KEY_SINGLE:
      send_button_signal(BT_M_SCLICK);
      break;

    case KEY_LONG:
      send_button_signal(BT_M_LONGCLICK);
	  break;
  }
  switch(appData.Key_Left.state)
  {
    case KEY_SINGLE:
      send_button_signal(BT_L_SCLICK);
      break;

    case KEY_LONG:
      send_button_signal(BT_L_LONGCLICK);
      break;
  }

  switch(appData.Key_Right.state)
  {
    case KEY_SINGLE:
      send_button_signal(BT_R_SCLICK);
      break;

    case KEY_LONG:
      send_button_signal(BT_R_LONGCLICK);
      break;
  }
}
/* used for screen refresh, called every 100ms */
void Tim4_CallBack(void)
{
//  if (--IWDG_count == 0)
//  {
//    IWDG->KR = IWDG_KEY_RELOAD;//reload IWDG
//    IWDG_count = IWDG_MAX_COUNT;
//  }
  ChangeScreen();
}
/****************** uart and event related functions *************************/

/* load rx byte in RX interrupt to RX buffer one by one,
 * then handle the event in USART_Tasks */

void load_rxbyte_to_packet(u8 data, u8* buffer, u32* qlen, u8* escape)
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
                start_flag = true;
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
              if (start_flag)
              {
                appData.cflag = 1;
                start_flag = false;
              }
              else
                (*qlen) = 0;
              //disable rx int
              //UART_ITConfig((uint32_t)(USART_CR1_RXNEIE), 0);
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

static u8 rxbuf_handle(u8* packet, u32 plen)
{
    u8 msg_type;
    u8 ret = 0;
    u8* phead = packet + 2;
    u32 signal;
    u32 page_id;
    u16 crc = 0;

    crc = (packet[plen-2]) & 0x00ff;
    crc = crc | ((packet[plen-1] << 8) & 0xFF00);
    if(CalculateCrc((char*)packet, (plen-2)) == crc)
    {
	/* check msg_type */
	memcpy(&msg_type, packet + 1, 1);
	switch(msg_type)
	{
          //check version
          case MSG_MCU_VERSION_INFO:
              send_mcu_version();
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
              case PAGE_SYSIFACE:
                page_id = sysinterface;
                break;
              case PAGE_HOME:
                page_id = home;
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
            if (!HAL_GPIO_ReadPin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN))
            {
              HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_SET);
              //ili9488_DisplayOn();
            }
            if(appData.cur_screen == (uint8_t)(page_id&0xff)){
               appData.changFlag = true;
            }else{
                appData.cur_screen = (uint8_t)(page_id&0xff);
                appData.screen_state = SCREEN_INIT;
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
                if(appData.homeinfo.mode != mode)
                {
                    appData.screen_state = SCREEN_INIT;
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
  else
  {
      //erro handle
      ret = 1;
  }
  return ret;

}

/* build tx buffer, packet is only the struct string, not including  PROT_VER and msg_type
 * then send tx buffer in bytes one by one*/
static u32 build_tx_buffer(u8* data, u32 dlen, MSG_TYPE_E msg_type)
{
    if (dlen > (PACKET_SIZE - 2))
        return dlen;

    u32 len = 0;
    u16 crc16 = 0x0000;
    u8 crc_L = 0x00;
    u8 crc_H = 0x00;
    u32 i = 0;

    //if tx_buff is not empty, bezero it
    if (txlen)
    {
      memset(tx_buff, 0x00, txlen);
      txlen = 0;
    }
    if (txplen)
    {
      memset(tx_packet, 0x00, txplen);
      txplen = 0;
    }
    //make packet
    tx_packet[txplen++] = PROT_VER;
    tx_packet[txplen++] = msg_type;
    memcpy(tx_packet + txplen, data, dlen);
    txplen += dlen;

    //add SOH
    tx_buff[len++] = SOH;

    //add packet frame
    for (;i < txplen;i++)
    {
        if ((EOT == tx_packet[i]) || (SOH == tx_packet[i]) || (DLE == tx_packet[i]))
        {
            tx_buff[len++] = DLE;
        }
        tx_buff[len++] = tx_packet[i];
    }
    //add crc_l and crc_H
    crc16 = CalculateCrc((char*)tx_packet, txplen);
    crc_L = (u8)(crc16);
    crc_H = (u8)(crc16>>8);

    if ((EOT == crc_L) || (SOH == crc_L) || (DLE == crc_L))
        tx_buff[len++] = DLE;
    tx_buff[len++] = crc_L;

    if ((EOT == crc_H) || (SOH == crc_H) || (DLE == crc_H))
        tx_buff[len++] = DLE;
    tx_buff[len++] = crc_H;
    //add EOT
    tx_buff[len++] = EOT;

    memset(tx_packet, 0x00, txplen);
    txplen = 0;

//    rxlen = len;
    return len;
}

static u32 send_tx_buffer(u8* packet, u32 plen)
{
    u8 ret = 0;
    u32 i = 0;
    if (NULL == packet || (plen > TX_BUF_SIZE - 2))
        return 1;
     //echo all buffer data
     for(i = 0;i < plen; i++)
     {
       UART_SendData(packet[i]);
     }
    //clear tx_buffer
    memset(packet, 0, plen);
    return ret;
}

/* some integrated command */
static void send_mcu_version(void)
{
    DATA_MCU_VERSION_T mcu_ver;
    mcu_ver.swver = (u16)(MCU_SW_VER & 0xffff);
    mcu_ver.major = (u8)(MCU_MAJOR & 0xff);
    mcu_ver.minor = (u8)(MCU_MINOR & 0xff);
    u8* data = (u8*)&mcu_ver;
    u32 len = (u32)sizeof(data);

    txlen = build_tx_buffer(data, len, MSG_MCU_VERSION_INFO);

    send_tx_buffer(tx_buff, txlen);
}

static void send_mcu_resumed_signal(void)
{
  txlen = build_tx_buffer("", 0, MSG_MCU_RESUMED);

  send_tx_buffer(tx_buff, txlen);
}

static void send_event_signal(u32 event_signal)
{

    txlen = build_tx_buffer((u8*)&event_signal, sizeof(event_signal), MSG_EVENT);

    send_tx_buffer(tx_buff, txlen);

}

static void send_button_signal(u32 button_signal)
{
    if (appData.key_en)
    {
        uint8_t pageid;
        pageid = appData.cur_screen;
        switch (button_signal)
        {
            case BT_R_SCLICK:
                if (pageid < devlist && pageid >= sysinfo)
                    pageid++;
                break;
            case BT_L_SCLICK:
                if (pageid <= devlist && pageid > sysinfo)
                    pageid--;
                break;
            case BT_M_SCLICK:
                pageid = home;
                break;
        }
    }
    txlen = build_tx_buffer((u8*)&button_signal, sizeof(button_signal), MSG_BUTTON);

    send_tx_buffer(tx_buff, txlen);
}

static void msg_event_handle(u32 signal)
{
    switch(signal)
    {

        case SCREEN_ON:
            HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_SET);
            //ili9488_DisplayOn();
            appData.ScreenOn = true;
            break;
        case SCREEN_OFF:
            HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_RESET);
            //ili9488_DisplayOff();
            appData.ScreenOn = false;
            break;
        case REBOOT:
              HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_SET);
              //ili9488_DisplayOn();
              appData.cur_screen = alert;
              appData.screen_state = SCREEN_INIT;
              appData.eventtype = REBOOT;
            break;
        case FACTORY_RESET:
              HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_SET);
              //ili9488_DisplayOn();
              appData.cur_screen = alert;
              appData.screen_state = SCREEN_INIT;
              appData.eventtype = FACTORY_RESET;
            break;
        case ROUTER_UPGRADE:
              HAL_GPIO_WritePin(APP_SCREEN_DIMMING_PORT, APP_SCREEN_DIMMING_PIN, GPIO_PIN_SET);
             // ili9488_DisplayOn();
              appData.cur_screen = alert;
              appData.screen_state = SCREEN_INIT;
              appData.eventtype = ROUTER_UPGRADE;
            break;
    }
}
/************************* draw screen functions ****************************/

/***************************** tasks in loop**********************************/
/* uart task, running in infinite loop */
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
             if (true == appData.cflag)
            {
              /* load and parse rx_buff to packet, record rxlen and reset rxlen */
               /* process the rxbuffer, check the CRC, check protocol */
                /* unpackage data according to MSG_TYPE_T, then load it to structure  */
                rxbuf_handle(packet, pkt_len);
                pkt_len = 0;
                //UART_ITConfig((uint32_t)(USART_CR1_RXNEIE), 1);
                appData.cflag = false;

            }
            break;
        }
    }
}

/* initialize, called in main.c */
void APP_Initialize (void)
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
  appData.alert_tr.len=16;
  appData.alert_tr.num=8;
  appData.alert_tr.i_start=0;
  appData.alert_tr.radius=45;
  appData.alert_tr.turn_flag=0;
  appData.count=0.0;

  appData.cur_screen = welcome;
  appData.screen_state = SCREEN_INIT;

  appData.changFlag = false;
  appData.ScreenOn = true;
  appData.homeInfoChangFlag = false;

  appData.usart_state = USART_INIT;
  appData.key_en = false;

  appData.cflag = false;
  /* initial USART RX and TX buffer */
//  memset(rx_buff, 0, RX_BUF_SIZE);
//  rxlen = 0;
  memset(tx_buff, 0, TX_BUF_SIZE);
  txlen = 0;
  memset(packet, 0, PACKET_SIZE);
  pkt_len = 0;
  memset(tx_packet, 0x00, PACKET_SIZE);
  txplen = 0;

  Escape = 0;
  refresh_count = 0;
  init_flag =1;
  start_flag = false;
}

/* main app task, running in infinite loop */
void APP_Tasks (void)
{
  /* Check the application's current state. */
  switch ( appData.state )
  {
    /* Application's initial state. */
    case APP_STATE_INIT:
    {
      //Start_Tim_IT();
      //if MCU resumed from IWDG, send msg MSG_MCU_RESUME to K3 to renew data
//      if (IWDG_resumed_state == 1)
//      {
//        send_mcu_resumed_signal();
//      }
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
      //ChangeScreen(appData.cur_screen,1);
      appData.state = APP_STATE_SERVICE_TASKS;
      appData.changFlag = false;
      appData.homeInfoChangFlag = false;
      break;
    }

    case APP_STATE_SPEED_REFRESH:
    {
      refresh_count = 0;
     // ChangeScreen(appData.cur_screen, 0);
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


