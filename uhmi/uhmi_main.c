#include "uhmi_common.h"
#include "uhmi_uart.h"
#include "uhmi_bootloader.h"
#include "uhmi_application.h"
#include "uhmi_msg.h"
#include "uhmi_debug.h"
#include "uhmi_def.h"
#include "uhmi_bootloader.h"
#include <elog.h>
#include <signal.h>
#include <bcmgpio.h>
#include <sys/prctl.h>

int debug_level = DBG_LEVEL_ERROR;
int dump_mode = DUMP_PACKET_NONE;
BOOL noerror = TRUE;

UHMI_GLOBAL_PARAMS g_uhmi_params;

pthread_t tid_crawler = 0;
pthread_t tid_tx = 0;
pthread_t tid_rx = 0;
pthread_t tid_event = 0;

pthread_mutex_t send_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recive_mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t tx_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  tx_cond = PTHREAD_COND_INITIALIZER;


#if 0
void uhmi_pthread_data_crawler(void *arg)
{
    while(1)
    {


        sleep(4);
    }
}
#endif

/***************************************************************

Frame Format
The communication protocol follows the frame format,
as shown in Example 1. The frame format remains the
same in both directions, that is, from the host
application to the bootloader, and from the bootloader
to the host application.
EXAMPLE 1: FRAME FORMAT
<SOH><DATA><CRCL><CRCH><EOT>


The frame starts with a control character, Start of
Header (SOH), and ends with another control character,
End of Transmission (EOT). The integrity of the frame is
protected by two bytes of Cyclic Redundancy Check
(CRC)-16, represented by CRCL (low-byte) and CRCH
(high-byte).
Control Characters
Some bytes in the Data field may imitate the control
characters, SOH and EOT. The Data Link
Escape (DLE) character is used to escape such bytes
that could be interpreted as control characters. The
bootloader always accepts the byte following a <DLE>
as data, and always sends a <DLE> before any of the
control characters.
***************************************************************/
static BOOL uhmi_rx_to_packet(u8 *data, u32 dlen, u8 *packet, u32 *plen)
{
    static BOOL Escape = FALSE;
    BOOL RxPacketValid = FALSE;
    u16 crc;

    while((dlen > 0) && (RxPacketValid == FALSE))
    {
        dlen--;

        if((*plen) >= (PACKET_SIZE-2))
        {
            (*plen) = 0;
        }

        switch(*data)
        {
            case SOH: //2 Start of header
                if(Escape)
                {
                    //2 Received byte is not SOH, but data.
                    packet[(*plen)++] = *data;
                    //2 Reset Escape Flag.
                    Escape = FALSE;
                }
                else
                {
                    //2 Received byte is indeed a SOH which indicates start of new frame.
                    (*plen) = 0;
                }
                break;

            case EOT: //2 End of transmission
                if(Escape)
                {
                    //2 Received byte is not EOT, but data.
                    packet[(*plen)++] = *data;
                    //2 Reset Escape Flag.
                    Escape = FALSE;
                }
                else
                {
                    //2 Received byte is indeed a EOT which indicates end of frame.
                    //2 Calculate CRC to check the validity of the frame.
                    if((*plen) > 2)
                    {
                        crc = (packet[(*plen)-2]) & 0x00ff;
                        crc = crc | ((packet[(*plen)-1] << 8) & 0xFF00);
                        if(CalculateCrc(packet, ((*plen)-2)) == crc)
                        {
                            //2 CRC matches and frame received is valid.
                            RxPacketValid = TRUE;
                        }
                    }
                }
                break;


            case DLE: //2 Escape character received.
                if(Escape)
                {
                    //2 Received byte is not ESC but data.
                    packet[(*plen)++] = *data;
                    //2 Reset Escape Flag.
                    Escape = FALSE;
                }
                else
                {
                    //2 Received byte is an escape character.
                    //2 Set Escape flag to escape next byte.
                    Escape = TRUE;
                }
                break;

            default: //2 Data field.
                packet[(*plen)++] = *data;
                //2 Reset Escape Flag.
                Escape = FALSE;
                break;

        }
        //2 Increment the pointer.
        data++;

    }

    return RxPacketValid;
}

void *uhmi_pthread_rx(void *arg)
{
    u8  buf[RX_BUF_SIZE] = {0};
    u32 nread = 0;

    u8  packet[PACKET_SIZE] = {0};
    u32 plen = 0;

    int i = 0;

    while(1)
    {
        nread = g_uhmi_params.dev_rx(buf, RX_BUF_SIZE);

        if (uhmi_rx_to_packet(buf, nread, packet, &plen) == TRUE)
        {
            if (dump_mode & DUMP_PACKET_RX)
            {
                printf("uhmi_rx_to_packet:");
                for (i = 0; i < plen; i++)
                printf("%02x ", packet[i]);
                printf("\n");
            }

            switch(g_uhmi_params.mcu.rmode)
            {
                //2 MCU in app status, parse packet with application
                //2 level communication protocol.
                case MCU_APP_MODE:
                    app_rx_handler(packet, plen);
                    break;

                //2 MCU in boot status, parer packet with bootloader communication protoco.
                case MCU_BOOT_MODE:
                    boot_rx_handler(packet, plen);
                    break;

                default:
                    break;
            }


            memset(packet, 0, sizeof(packet));
            plen = 0;
        }
    }

}

/***************************************************************
On success,return zero.
***************************************************************/
static u32 uhmi_build_tx_buf(u8 *txbuf, u32 txbuf_size, u8 *packet, u32 plen)
{
    u32 len = 0;
    u32 i = 0;
    u16 crc16 = 0x0000;
    u8 crc_L = 0x00;
    u8 crc_H = 0x00;

    if (plen > (PACKET_SIZE - 2))
        return len;

    txbuf[len++] = SOH; //2 add streams head

    for (i=0; i<plen; i++)
    {
        //2 Insert a <DLE> before any of the control characters(SOH EOT DLE) in packet.
        if((EOT == packet[i]) || (SOH == packet[i]) || (DLE == packet[i]))
        {
            txbuf[len++] = DLE;
        }

        txbuf[len++] = packet[i];
    }

    //2 calculate checksum
    crc16 = CalculateCrc(packet, plen);

    crc_L = (u8)(crc16);
    crc_H = (u8)(crc16>>8);

    if((EOT == crc_L) || (SOH == crc_L) || (DLE == crc_L))
        txbuf[len++] = DLE;
    txbuf[len++] = crc_L;

    if((EOT == crc_H) || (SOH == crc_H) || (DLE == crc_H))
        txbuf[len++] = DLE;
    txbuf[len++] = crc_H;

    txbuf[len++] = EOT; //2 add streams tail

    return len;
}

void *uhmi_pthread_tx(void *arg)
{
    u8 txbuf[TX_BUF_SIZE] = {0};
    u32 buflen = 0;

    while(1)
    {
        pthread_mutex_lock(&tx_mtx);
        pthread_cond_wait(&tx_cond, &tx_mtx);

        buflen = uhmi_build_tx_buf(txbuf, sizeof(txbuf),
            g_uhmi_params.tx_packet, g_uhmi_params.tx_plen);

        if (buflen > 4)
            g_uhmi_params.dev_tx(txbuf, buflen);
        else
            DPRINT(DBG_LEVEL_DEBUG, "buflen[%d]", buflen);

        pthread_mutex_unlock(&tx_mtx);
    }
}

void uhmi_show_mcu_ver(void)
{
    printf("major: %d\n", g_uhmi_params.mcu.version.major);
    printf("minor: %d\n", g_uhmi_params.mcu.version.minor);
    printf("swver: %d\n", g_uhmi_params.mcu.version.swver);
    switch (g_uhmi_params.mcu.version.major)
    {
        case MCU_MAJOR_PIC32EFE:
            printf("mcu: PIC32EFE\n");
            break;

        case MCU_MAJOR_ST446:
            printf("mcu: ST446\n");
            break;
        default:
            printf("mcu: unknown\n");
            break;
    }

}
void *uhmi_pthread_event(void *arg)
{
    key_t   key;
    char    cmd[128] = {0};
    int     msgid = -1;
    UHMI_MSG_T msgbuf;

    memset(&msgbuf, 0, sizeof(msgbuf));

	//2 creat msg key
    key = FTOK_KEY;
    if (-1 == key)
    {
        DPRINT(DBG_LEVEL_ERROR, "FTOK_KEY error\n");
        pthread_exit((void *)0);
    }

    //2 Creat msgQ
	msgid = msgget(key, 0666 | IPC_CREAT);
    if (-1 == msgid)
    {
        DPRINT(DBG_LEVEL_ERROR, "msgget failed with error: %s\n", strerror(errno));
        pthread_exit((void *)0);
    }

    sprintf(cmd, "echo msgkey=0x%08x, msgid:%d > /tmp/uhmi_msg_info",
        key, msgid);
    system(cmd);

	while(1)
	{
		if( msgrcv(msgid, (void*)&msgbuf, UHMI_DATA_SIZE, 0, 0) >= 0)
		{
            switch(msgbuf.msg_type)
            {
                case UHMI_SCREEN_SET:
                    if (g_uhmi_params.mcu.rmode == MCU_APP_MODE)
                    {
                        app_screen_cfg();
                        DPRINT(DBG_LEVEL_DEBUG, "usb unplugged\n");
                    }
                    break;
                case UHMI_UPDATE:
                    uhmi_jump_to_boot();
                    break;

                case UHMI_RUNAPP:
                    uhmi_jump_to_app();
                    break;

                case UHMI_DEBUG:
                    printf("UHMI_DEBUG: 0x%08x\n", *(int *)msgbuf.data);
                    debug_level = *(int *)msgbuf.data;
                    break;

                case UHMI_DUMP:
                    printf("UHMI_DUMP: 0x%08x\n", *(int *)msgbuf.data);
                    dump_mode = *(int *)msgbuf.data;
                    break;

                case UHMI_VER:
                    uhmi_show_mcu_ver();
                    break;

                default:
                    if (MCU_APP_MODE == g_uhmi_params.mcu.rmode)
                        app_event_task(msgbuf.msg_type);
                    break;
            }
		}
        else
        {
            //2 if msgrcv fail, thread sleep 2s, avoid resource-intensive
            DPRINT(DBG_LEVEL_ERROR, "msgrcv error: %s\n", strerror(errno));
            //sleep(2);
        }
	}


    //2 delete msgQ
    if ((-1) != msgid)
    {
        DPRINT(DBG_LEVEL_DEBUG, "delete msgQ\n");
        if(msgctl(msgid, IPC_RMID, 0) == -1)
            DPRINT(DBG_LEVEL_ERROR, "msgctl(IPC_RMID) failed\n");
    }

}

/***************************************************************
On success,return zero.
***************************************************************/
int uhmi_thread_creat(void)
{
    int ret = 0;

#if 0
    if ((ret = pthread_create(&tid_crawler, NULL, uhmi_pthread_data_crawler, NULL)) != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_pthread_data_crawler: %s\n", strerror(errno));
        return ret;
    }
    usleep(50000);
#endif

    if ((ret = pthread_create(&tid_tx, NULL, uhmi_pthread_tx, NULL)) != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_pthread_tx: %s\n", strerror(errno));
        return ret;
    }
    usleep(50000);

    if ((ret = pthread_create(&tid_rx, NULL, uhmi_pthread_rx, NULL)) != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_pthread_rx: %s\n", strerror(errno));
        return ret;
    }
    usleep(50000);

    if ((ret = pthread_create(&tid_event, NULL, uhmi_pthread_event, NULL)) != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_pthread_event: %s\n", strerror(errno));
        return ret;
    }
    usleep(50000);

    return ret;
}


void uhmi_send_data(u8 *data, u32 len)
{
    pthread_mutex_lock(&tx_mtx);
    memcpy(g_uhmi_params.tx_packet, data, len);
    g_uhmi_params.tx_plen = len;
    pthread_cond_signal(&tx_cond);
    pthread_mutex_unlock(&tx_mtx);
}

void uhmi_usage(char* name)
{

printf("Usage: %s [OPTION]...\n", name);

    printf("\
    -h,     show help\n");
    printf("\
    -d,     Set debug level\n\
            0x00000001 DBG_LEVEL_NONE \n\
            0x00000002 DBG_LEVEL_INFO \n\
            0x00000004 DBG_LEVEL_NORMAL \n\
            0x00000008 DBG_LEVEL_ERROR \n\
            0x00000010 DBG_LEVEL_DEBUG \n\
            0xFFFFFFFF DBG_LEVEL_ALL  \n");
    printf("\
    -D,     Dump packet\n\
            none    not dump packet \n\
            rx      dump rx packet \n\
            tx      dump tx packet \n\
            both    dump both rx and tx packet \n");

    exit(0);
}


void uhmi_init_database(void)
{
    memset(&g_uhmi_params, 0, sizeof(g_uhmi_params));

    g_uhmi_params.mcu.rmode = MCU_APP_MODE;
    g_uhmi_params.mcu.screen_on = TRUE;

	g_uhmi_params.mcustatus.run_status = TRUE;
	g_uhmi_params.mcustatus.heartbeat_interval = HEARTBEAT_INTERVAL_1st;
	g_uhmi_params.mcustatus.heartbeat_ontime = get_uptime();

    g_uhmi_params.appstatus = APP_VERSION_CHECK;

    g_uhmi_params.weather.refresh_flag = FALSE;
    g_uhmi_params.weather.refresh_interval = WEATHER_REFRESH_INTERVAL;
    //update_host_table(&g_uhmi_params.host_table);

    //get_system_info(&g_uhmi_params.system_info);

    //get_wifi_info(&g_uhmi_params.wifi_info);

    //get_home_info(&g_uhmi_params.home_info);
    


//2 Communication Mode
#ifdef DEV_UART
    g_uhmi_params.dev_init = uart_init;
    g_uhmi_params.dev_rx = uart_rx;
    g_uhmi_params.dev_tx = uart_tx;
    g_uhmi_params.dev_close = uart_close;
#elif defined DEV_I2C
    g_uhmi_params.dev_init = i2c_init;
    g_uhmi_params.dev_rx = i2c_rx;
    g_uhmi_params.dev_tx = i2c_tx;
    g_uhmi_params.dev_close = i2c_close;
#elif defined DEV_SPI
    g_uhmi_params.dev_init = spi_init;
    g_uhmi_params.dev_rx = spi_rx;
    g_uhmi_params.dev_tx = spi_tx;
#elif defined DEV_USB
    g_uhmi_params.dev_init = usb_init;
    g_uhmi_params.dev_rx = usb_rx;
    g_uhmi_params.dev_tx = usb_tx;
#endif
}

void uhmi_gpio_set(int pin, GPIO_VALUE_E value)
{
    if( bcmgpio_connect(pin, BCMGPIO_DIRN_OUT))
    {
        printf("hotplug detected erro \n");
    }
    else
    {
        bcmgpio_out((1<<pin),  (value<<pin));
        bcmgpio_disconnect(pin);
    }
}

void uhmi_get_hex_ver(void)
{

    FILE *fp = NULL;
    char buf[256] = {0};
    int major = 0, minor = 0, ver = 0;

    fp = popen("ls /mcu/ 2>/dev/null | grep app", "r");
    if (fp == NULL)
    {
        DPRINT(DBG_LEVEL_ERROR, "%s\n", strerror(errno));
    }
    else
    {
        while(!feof(fp))
        {   
            major = 0; 
            minor = 0;
            ver = 0;
            if((NULL != fgets(buf, sizeof(buf), fp)) && (g_uhmi_params.mcu_hex_count < MCU_HEX_MAX))
            {
                sscanf(buf, "app.%d.%d.%d.%*s", &major, &minor, &ver);

                g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].major = major;
                g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].minor = minor;
                g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].swver = ver;                

                DPRINT(DBG_LEVEL_DEBUG, "hex file major:%d, minor:%d, ver:%d\n", 
                    g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].major, 
                    g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].minor, 
                    g_uhmi_params.hexver[g_uhmi_params.mcu_hex_count].swver);

                g_uhmi_params.mcu_hex_count++;
            }
        }
        pclose(fp);        

//        DPRINT(DBG_LEVEL_DEBUG, "hex file major:%d, minor:%d, ver:%d\n", 
//        g_uhmi_params.hexver.major, g_uhmi_params.hexver.minor, g_uhmi_params.hexver.swver);
        
    }
}

/***************************************************************
On success,return zero.
***************************************************************/
int uhmi_init(void)
{
    int ret = 0;

    //2 init g_uhmi_params
    uhmi_init_database();

    //2 get hex file ver
    uhmi_get_hex_ver();

    //2 init GPIO pin for MCU reset_mclr and boot_trigger
    uhmi_gpio_set(PIN_RESET_MCLR, PIN_SET_HIGH);
    uhmi_gpio_set(PIN_BOOT_TRIGGER, PIN_SET_LOW);

    //2 init Communication Interface
    if (g_uhmi_params.dev_init)
    {
        if ((ret = g_uhmi_params.dev_init()) != 0)
        {
            DPRINT(DBG_LEVEL_ERROR, "g_uhmi_params.dev_init fail\n");
            ret = -1;
        }
    }
    else
    {
        DPRINT(DBG_LEVEL_ERROR, "g_uhmi_params.dev_init is NULL\n");
        ret = -1;
    }

    if (NULL == g_uhmi_params.dev_rx)
    {
        DPRINT(DBG_LEVEL_ERROR, "g_uhmi_params.dev_rx is NULL\n");
        ret = -1;
    }

    if (NULL == g_uhmi_params.dev_tx)
    {
        DPRINT(DBG_LEVEL_ERROR, "g_uhmi_params.dev_tx is NULL\n");
        ret = -1;
    }

    if (NULL == g_uhmi_params.dev_close)
    {
        DPRINT(DBG_LEVEL_ERROR, "g_uhmi_params.dev_close is NULL\n");
        ret = -1;
    }

    return ret;

}


int checkCPUendian()
{
    union
    {
        unsigned int a;
        unsigned char b;
    }c;

    c.a = 1;
    
    return (c.b == 1);
}
/*return 1 : little-endian, return 0:big-endian*/

void sTerminate()
{
    //pthread_cancel(tid_crawler);

    pthread_cancel(tid_event);

    pthread_cancel(tid_rx);

    pthread_cancel(tid_tx);

    usleep(10000);
    
    exit(EXIT_SUCCESS);
}

void uhmi_jump_to_app(void)
{
    uhmi_gpio_set(PIN_RESET_MCLR, PIN_SET_LOW);
    uhmi_gpio_set(PIN_BOOT_TRIGGER, PIN_SET_LOW);
    usleep(50000);
    uhmi_gpio_set(PIN_RESET_MCLR, PIN_SET_HIGH);
    g_uhmi_params.mcu.rmode = MCU_APP_MODE;
    g_uhmi_params.appstatus = APP_VERSION_CHECK;
}

void uhmi_jump_to_boot(void)
{
    uhmi_gpio_set(PIN_RESET_MCLR, PIN_SET_LOW);
    uhmi_gpio_set(PIN_BOOT_TRIGGER, PIN_SET_HIGH);
    usleep(50000);
    uhmi_gpio_set(PIN_RESET_MCLR, PIN_SET_HIGH);
    g_uhmi_params.mcu.rmode = MCU_BOOT_MODE;
    memset(&g_boot_params, 0, sizeof(g_boot_params));
}

void main(int argc, char *argv[])
{
    INT32 oc = 0;

    elog_init(ELOGBACK_DISKFILE, "/tmp/uhmi.log");
    elog_printf(ELOG_INFO, "uhmi main init\n");
    signal(SIGTERM, sTerminate);
    while((oc = getopt(argc, argv, "d:D:")) != -1)
    {
        switch(oc)
        {
            case 'd':
                sscanf(optarg, "%x", &debug_level);
                break;
            case 'D':
                if (strcmp(optarg, "rx") == 0)
                    dump_mode = DUMP_PACKET_RX;
                else if (strcmp(optarg, "tx") == 0)
                    dump_mode = DUMP_PACKET_TX;
                else if (strcmp(optarg, "both") == 0)
                    dump_mode = DUMP_PACKET_BOTH;
                else if (strcmp(optarg, "none") == 0)
                    dump_mode = DUMP_PACKET_NONE;
                else
                    uhmi_usage(argv[0]);
                break;
            default:
                uhmi_usage(argv[0]);
                break;
        }
    }

    if (uhmi_init() != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_init fail!\n");
        goto err_exit;
    }
    printf("uhmi init .............ok!\n");

    if (uhmi_thread_creat() != 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "uhmi_thread_creat fail!\n");
        goto err_exit;
    }
    printf("uhmi thread creat......ok!\n");

    while(noerror)
    {
        switch(g_uhmi_params.mcu.rmode)
        {
            case MCU_APP_MODE:
                app_task();
                break;

            case MCU_BOOT_MODE:
                boot_task();
                break;

            default:
                DPRINT(DBG_LEVEL_ERROR, "unknown run mode!\n");
                break;
        }

        sleep(2);
    }

err_exit:

    g_uhmi_params.dev_close();
    exit(0);
}
