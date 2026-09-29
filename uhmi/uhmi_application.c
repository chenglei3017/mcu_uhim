#include "uhmi_application.h"
#include "uhmi_debug.h"
#include "uhmi_def.h"
#include "uhmi_msg.h"
#include "uhmi_data_crawler.h"
#include "uhmi_bootloader.h"

#include <sys/sysinfo.h>
#include <bcmnvram.h>

BOOL version_ack = FALSE;

long host_table_refresh_time = 0;

BOOL interface_popup_flag = FALSE;
long interface_popup_time = 0;
PAGE_ID_E interface_popup_prev;

void app_goto_prev_page(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    g_uhmi_params.mcu.page_id =  interface_popup_prev;
    interface_popup_flag = FALSE;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_PAGE_ID;
    *(PAGE_ID_E*)&data[dlen] = g_uhmi_params.mcu.page_id;
    dlen += sizeof(MSG_PAGE_ID);

    uhmi_send_data(data, dlen);
    DPRINT(DBG_LEVEL_DEBUG, "interface_popup_prev: %d\n", interface_popup_prev);
    usleep(20000);

}

// when recive screen cfg event, update screen intervl and screen ontime.
void app_screen_cfg(void)
{
    g_uhmi_params.mcu.screen_interval = atoi(nvram_safe_get("screen_time"));
    g_uhmi_params.mcu.screen_ontime = get_uptime();
}

void app_screen_off(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_EVENT;
    *(EVENT_TYPE_E*)&data[dlen] = SCREEN_OFF;
    dlen += sizeof(EVENT_TYPE_E);
    g_uhmi_params.mcu.screen_on = FALSE;

    uhmi_send_data(data, dlen);
}

void app_event_task(long int msgtype)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;
    BOOL errflag = FALSE;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_EVENT;

    switch(msgtype)
    {
        case UHMI_ROUTER_REBOOT:
            g_uhmi_params.appstatus = APP_WAITING;
            *(EVENT_TYPE_E*)&data[dlen] = REBOOT;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router reboot\n");
            break;

        case UHMI_ROUTER_RESET:
            g_uhmi_params.appstatus = APP_WAITING;
            *(EVENT_TYPE_E*)&data[dlen] = FACTORY_RESET;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router reset\n");
            break;

        case UHMI_BACK_RECOVERY:
            g_uhmi_params.appstatus = APP_WAITING;
            *(EVENT_TYPE_E*)&data[dlen] = BACK_RECOVERY;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router backup recovery\n");
            break;

        case UHMI_ROUTER_UPGRADE:
            g_uhmi_params.appstatus = APP_WAITING;
            *(EVENT_TYPE_E*)&data[dlen] = ROUTER_UPGRADE;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router upgrade\n");
            break;

        case UHMI_LAN_WAN_CONFLICT:
            *(EVENT_TYPE_E*)&data[dlen] = LAN_WAN_CONFLICT;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router wifi on\n");
            break;

        case UHMI_WIFI_ON:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_ON;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router wifi on\n");
            break;

        case UHMI_WIFI_OFF:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_OFF;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router wifi off\n");
            break;

        case UHMI_2G_ON:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_2G_ON;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router 2.4G on\n");
            break;

        case UHMI_2G_OFF:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_2G_OFF;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router 2.4G off\n");
            break;

        case UHMI_5G_ON:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_5G_ON;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router 5G on\n");
            break;

        case UHMI_5G_OFF:
            *(EVENT_TYPE_E*)&data[dlen] = WIFI_5G_OFF;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router 5G off\n");
            break;

        case UHMI_WISP_ON:
            *(EVENT_TYPE_E*)&data[dlen] = REPEATER_ON;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router wisp on\n");
            break;

        case UHMI_WISP_OFF:
            *(EVENT_TYPE_E*)&data[dlen] = REPEATER_OFF;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:router wisp off\n");
            break;

		 case UHMI_SCREEN_ON:
            *(EVENT_TYPE_E*)&data[dlen] = SCREEN_ON;
			g_uhmi_params.mcu.screen_on = TRUE;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:mcu screen on\n");
            break;

		case UHMI_SCREEN_OFF:
            *(EVENT_TYPE_E*)&data[dlen] = SCREEN_OFF;
			g_uhmi_params.mcu.screen_on = FALSE;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:mcu screen off\n");
            break;

        default:
            errflag = TRUE;
            DPRINT(DBG_LEVEL_DEBUG, "uhmi:Invalid msg type[%d]\n", msgtype);
            break;
    }

    if (!errflag)
    {
        dlen += sizeof(EVENT_TYPE_E);
        uhmi_send_data(data, dlen);
    }

}

void app_button_handler(BT_TYPE_E bt_type)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;

    g_uhmi_params.mcu.screen_ontime = get_uptime();

    interface_popup_flag = FALSE;
    if(APP_WAITING == g_uhmi_params.appstatus){
        return ;
    }
    if (TRUE != g_uhmi_params.mcu.screen_on)
    {
        data[dlen++] = MSG_EVENT;
        *(EVENT_TYPE_E*)&data[dlen] = SCREEN_ON;
        dlen += sizeof(EVENT_TYPE_E);
        g_uhmi_params.mcu.screen_on = TRUE;
    }
    else
    {
        pthread_mutex_lock(&host_table_mtx);
        switch(bt_type)
        {
            case BT_L_SCLICK:
                if ((PAGE_HOSTDEV == g_uhmi_params.mcu.page_id) && (g_uhmi_params.host_table.index > 0))
                {
                    g_uhmi_params.host_table.index--;
                }
                else if ((g_uhmi_params.mcu.page_id - 1) > PAGE_INVALID)
                {
                    if (g_uhmi_params.mcu.page_id == PAGE_SYSINFO)
                    {
                        if (strlen(g_uhmi_params.system_info.newsw) < 1)
                        {
                            break;
                        }
                    }

                    g_uhmi_params.mcu.page_id--;
                    g_uhmi_params.host_table.index = 0;
                }
                break;

            case BT_R_SCLICK:
                if( nvram_match("mode","router"))
                {
                    if ((PAGE_HOSTDEV == g_uhmi_params.mcu.page_id) 
                        && ((g_uhmi_params.host_table.index + 1)*5 < g_uhmi_params.host_table.total))
                    {
                        g_uhmi_params.host_table.index++;
                    }
                    else if (g_uhmi_params.mcu.page_id < (PAGE_MAX-1))
                    {
                        g_uhmi_params.mcu.page_id++;
                    }
                }
                else if(nvram_match("mode","ap"))
                {
                    if(g_uhmi_params.mcu.page_id < (PAGE_MAX-2))
                    {
                        g_uhmi_params.mcu.page_id++;
                    }
                }
                break;

            case BT_M_SCLICK:
                g_uhmi_params.mcu.page_id = PAGE_HOME;
                g_uhmi_params.host_table.index = 0;
                break;

            case BT_M_LONGCLICK:
                data[dlen++] = MSG_EVENT;
                *(EVENT_TYPE_E*)&data[dlen] = SCREEN_OFF;
                dlen += sizeof(EVENT_TYPE_E);
                g_uhmi_params.mcu.screen_on = FALSE;
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&host_table_mtx);
    }

    if (MSG_EVENT != data[1])
    {
        if (PAGE_HOSTDEV == g_uhmi_params.mcu.page_id)
        {
            send_host_table();
            usleep(5000);
        }

        data[dlen++] = MSG_PAGE_ID;
        *(PAGE_ID_E*)&data[dlen] = g_uhmi_params.mcu.page_id;
        dlen += sizeof(MSG_PAGE_ID);
    }
    uhmi_send_data(data, dlen);
	usleep(5000);

    if (g_uhmi_params.mcu.page_id == PAGE_WEATHER)
    {
        if (g_uhmi_params.weather.refresh_flag == FALSE)
        {
			if (check_process("weather") == 0)
			{
            g_uhmi_params.weather.refresh_flag = TRUE;
            g_uhmi_params.weather.refresh_ontime = get_uptime();
            //update, get and send weather
                //if (nvram_safe_get("get_wan_port_status") == '1')
                system("weather update_weather &");
            get_weather_info(&g_uhmi_params.weather_info);
            send_weather_info();
			}
        }
    }
    else
    {
		if (g_uhmi_params.weather.refresh_flag == TRUE)
		{
			g_uhmi_params.weather.refresh_flag = FALSE;
		}
    }
}

/***********************************************************************
* Packet format:
* 1Byte_Protocol + 1Byte_Type + nByte_Data + 1Byte_crcL + 1Byte_crcH
***********************************************************************/
void app_rx_handler(u8 *packet, u32 plen)
{
    u8 *pver = NULL, *ptype = NULL, *pdata = NULL;
    u32 dlen = 0;
    if ((NULL == packet) || (plen < 2))
    {
        DPRINT(DBG_LEVEL_DEBUG, "Invalid packet[%p] or plen[%d]\n", packet, plen);
        return;
    }

    pver = packet;
    if (PROT_VER != *pver)
    {
        DPRINT(DBG_LEVEL_DEBUG, "Protocol version error, PROT_VER[0x%02x] pver[0x%02x]", PROT_VER, *pver);
        return;
    }

    ptype = (packet+1);
    pdata = (packet+2);

    dlen = plen - 4;

    switch(*ptype)
    {
        case MSG_BUTTON:
            app_button_handler(*(BT_TYPE_E* )pdata);
            break;

        case MSG_MCU_VERSION_INFO:
            if (sizeof(DATA_MCU_VERSION_T) == dlen)
            {
                memcpy(&g_uhmi_params.mcu.version, (packet+2), sizeof(DATA_MCU_VERSION_T));
                version_ack = TRUE;
                g_uhmi_params.mcu.reday = TRUE;
				//if get right mcu version data, mcu status is OK
				g_uhmi_params.mcustatus.run_status = TRUE;
				DPRINT(DBG_LEVEL_DEBUG, "get heartbeat reply from mcu\n");
            }
            else
            {
                DPRINT(DBG_LEVEL_DEBUG, "data length error, size[%d], plen[%d]\n",
                    sizeof(DATA_MCU_VERSION_T),  plen);
            }
			break;

        default:
            DPRINT(DBG_LEVEL_DEBUG, "invalid msg type: %d\n", *ptype);
            break;
    }


}

/******************************
Input:
    retry: no response retry times
    timeout: timeout for ack
Output:
    null

return value:
0. MCU no response or version mismatch, need upgrade.
1. MCU fw is latest version.
******************************/
static int app_version_check(int retry, int timeout)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;
    int ret = 0;
    int i = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_MCU_VERSION_INFO;
    version_ack = FALSE;

    while(retry--)
    {
        uhmi_send_data(data, dlen);
        sleep(timeout++);

        if (TRUE == version_ack)
        {            
            printf("mcu version: %d.%d.%d\n",  
                    g_uhmi_params.mcu.version.major, 
                    g_uhmi_params.mcu.version.minor, 
                    g_uhmi_params.mcu.version.swver);


            for(i = 0; i < g_uhmi_params.mcu_hex_count; i++)
            {
                if ((g_uhmi_params.hexver[i].major == g_uhmi_params.mcu.version.major) &&
                    (g_uhmi_params.hexver[i].minor == g_uhmi_params.mcu.version.minor) &&
                    (g_uhmi_params.hexver[i].swver != g_uhmi_params.mcu.version.swver))
                {
                    printf("mcu version update to: %d.%d.%d\n",  
                        g_uhmi_params.hexver[i].major, 
                        g_uhmi_params.hexver[i].minor,
                        g_uhmi_params.hexver[i].swver);
                    return ret;
                }
            }

            DPRINT(DBG_LEVEL_DEBUG, "no new version\n");
            ret = 1;
            break;

        }
    }

    return ret;
}

void app_init_mcu_data(void)
{
    update_host_table(&g_uhmi_params.host_table);
    host_table_refresh_time = get_uptime();

    get_system_info(&g_uhmi_params.system_info);
    send_system_info();
    usleep(20000);

    get_wifi_info(&g_uhmi_params.wifi_info);
    send_wifi_info();
    usleep(20000);

    get_home_info(&g_uhmi_params.home_info);
    send_home_info();
    usleep(20000);

    send_host_table();
    usleep(20000);

    get_interface_status(&g_uhmi_params.if_status);
    send_interface_status();
    usleep(200000);

    get_weather_info(&g_uhmi_params.weather_info);
    send_weather_info();
    usleep(200000);

    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;
    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_PAGE_ID;
    g_uhmi_params.mcu.page_id = PAGE_HOME;
    memcpy(&data[dlen], &g_uhmi_params.mcu.page_id, sizeof(PAGE_ID_E));
    dlen += sizeof(PAGE_HOME);
    uhmi_send_data(data, dlen);
    usleep(20000);

    app_screen_cfg();

}

static void check_new_version(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;
    if (nvram_match("sw_vernum","1"))
    {
        if (nvram_match("check_version_flag","1") && g_uhmi_params.mcu.page_id != PAGE_SOFTUPDATE)
        {
            get_system_info(&g_uhmi_params.system_info);
            send_system_info();
            usleep(20000);
            g_uhmi_params.mcu.page_id = PAGE_SOFTUPDATE;
            data[dlen++] = PROT_VER;
            data[dlen++] = MSG_PAGE_ID;
            *(PAGE_ID_E*)&data[dlen] = g_uhmi_params.mcu.page_id;
            dlen += sizeof(MSG_PAGE_ID);
            uhmi_send_data(data,dlen);
            nvram_set("check_version_flag","0");
            nvram_commit();
        }
    }
    else // sw_vernum!=1
    {
        if (g_uhmi_params.mcu.page_id == PAGE_SOFTUPDATE)
        {
            g_uhmi_params.mcu.page_id = PAGE_SYSINFO;
            data[dlen++] = PROT_VER;
            data[dlen++] = MSG_PAGE_ID;
            *(PAGE_ID_E*)&data[dlen] = g_uhmi_params.mcu.page_id;
            dlen += sizeof(MSG_PAGE_ID);
            uhmi_send_data(data,dlen);
        }
    }
}

// Refresh current page
static void app_refresh_current_page(void)
{
    int ret = 0;
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    switch(g_uhmi_params.mcu.page_id)
    {
        case PAGE_SOFTUPDATE:
            get_system_info(&g_uhmi_params.system_info);
            send_system_info();
            break;

        case PAGE_SYSINFO:
            get_system_info(&g_uhmi_params.system_info);
            send_system_info();
            break;

        case PAGE_SYSIFACE:
            send_interface_status();
            break;

        case PAGE_HOME:
            get_home_info(&g_uhmi_params.home_info);
            send_home_info();
            usleep(20000);

        case PAGE_WIFI:
            get_wifi_info(&g_uhmi_params.wifi_info);
            send_wifi_info();
            break;

        case PAGE_WEATHER:
            get_weather_info(&g_uhmi_params.weather_info);
            send_weather_info();
            break;

        case PAGE_HOSTDEV:
            send_host_table();
            break;
    }

    check_new_version();

    ret = get_interface_status(&g_uhmi_params.if_status);
    if (1 == ret)
    {
        usleep(20000);
        g_uhmi_params.mcu.screen_ontime = get_uptime();
        interface_popup_time = get_uptime();
        if (FALSE == interface_popup_flag)
        {
            interface_popup_prev = g_uhmi_params.mcu.page_id;
            interface_popup_flag = TRUE;
        }

        g_uhmi_params.mcu.page_id =  PAGE_SYSIFACE;
        data[dlen++] = PROT_VER;
        data[dlen++] = MSG_PAGE_ID;
        *(PAGE_ID_E*)&data[dlen] = g_uhmi_params.mcu.page_id;
        dlen += sizeof(MSG_PAGE_ID);

        uhmi_send_data(data, dlen);

    }
}
//use MCU version request order as a heartbeat request command
static void app_send_heartbeat_packet(void)
{
	u8 data[PACKET_SIZE] = {0};
	u32 dlen = 0;

	data[dlen++] = PROT_VER;
	data[dlen++] = MSG_MCU_VERSION_INFO;

	usleep(20000);
	uhmi_send_data(data, dlen);
}

void app_task(void)
{
    long current_time = get_uptime();
    switch(g_uhmi_params.appstatus)
    {
        case APP_VERSION_CHECK:
            DPRINT(DBG_LEVEL_DEBUG, "APP_VERSION_CHECK\n");
            if ((0 == g_uhmi_params.mcu_hex_count) ||
                ( g_uhmi_params.hexver[0].major == 0 
                    && g_uhmi_params.hexver[0].minor == 0 
                    && g_uhmi_params.hexver[0].swver == 0))
            {
                // If hex file not exist, Don't need to check the version number.
                g_uhmi_params.appstatus = APP_INIT_MCU_DATA;
                break;
            }

            // check version, if not match, goto boot mode
            if (app_version_check(5, 1) == 0)
            {
                uhmi_jump_to_boot();
            }
            else
            {
                g_uhmi_params.appstatus = APP_INIT_MCU_DATA;
            }
            break;

        case APP_INIT_MCU_DATA:
            DPRINT(DBG_LEVEL_DEBUG, "APP_INIT_MCU_DATA\n");
            app_init_mcu_data();
            g_uhmi_params.appstatus = APP_REFRESH_PAGE;
            break;

        case APP_REFRESH_PAGE:
            if ((current_time - host_table_refresh_time) > 6)
            {
                host_table_refresh_time = current_time;
                //2 Every 6 seconds to refresh the host table
                update_host_table(&g_uhmi_params.host_table);
            }

            DPRINT(DBG_LEVEL_DEBUG, "APP_REFRESH_PAGE\n");
            if (TRUE == interface_popup_flag)
            {
                if ((current_time - interface_popup_time) > 4)
                {
                    app_goto_prev_page();
                }
            }

            app_refresh_current_page();

            if (g_uhmi_params.mcu.screen_interval > 0)
            {
                if ((current_time - g_uhmi_params.mcu.screen_ontime) > g_uhmi_params.mcu.screen_interval)
                {
                    app_screen_off();
                    g_uhmi_params.mcu.screen_ontime = current_time;
                }
            }
            if ((g_uhmi_params.weather.refresh_flag == TRUE) && (g_uhmi_params.mcu.screen_on == TRUE))
            {
                if ((current_time - g_uhmi_params.weather.refresh_ontime) > g_uhmi_params.weather.refresh_interval)
                {
                    if (0 == check_process("weather"))
                    {
                    // DPRINT(DBG_LEVEL_DEBUG, "update weather_now here every %ld seconds!\n",
                    //     current_time - g_uhmi_params.weather.refresh_ontime);
                    DPRINT(DBG_LEVEL_DEBUG, "update weather_now here every %ld seconds!\n", g_uhmi_params.weather.refresh_interval);
                    system("weather update_weather &");
                    get_weather_info(&g_uhmi_params.weather_info);
                    send_weather_info();
                    g_uhmi_params.weather.refresh_ontime = current_time;
                    }
                }
            }
			if ((current_time - g_uhmi_params.mcustatus.heartbeat_ontime) >= g_uhmi_params.mcustatus.heartbeat_interval)
			{
				g_uhmi_params.appstatus = APP_HEART_BEAT;
				g_uhmi_params.mcustatus.heartbeat_ontime = current_time;
			}
            break;

		case APP_HEART_BEAT:
			//normal status, send heartbeat packet every HEARTBEAT_INTERVAL_1st
			if (TRUE == g_uhmi_params.mcustatus.run_status)
			{
				app_send_heartbeat_packet();
				DPRINT(DBG_LEVEL_DEBUG, "send 1st heartbeat packet at %ld\n", current_time);
				g_uhmi_params.mcustatus.run_status = FALSE;
				g_uhmi_params.mcustatus.heartbeat_interval = HEARTBEAT_INTERVAL_1st;
				g_uhmi_params.appstatus = APP_REFRESH_PAGE;
			}
			//if didn't get reply
			else
			{
				//at the first time, set interval half and try again
				if (HEARTBEAT_INTERVAL_1st == g_uhmi_params.mcustatus.heartbeat_interval)
				{
					app_send_heartbeat_packet();
					DPRINT(DBG_LEVEL_DEBUG, "send 2nd heartbeat packet at %ld\n", current_time);
					g_uhmi_params.mcustatus.heartbeat_interval = HEARTBEAT_INTERVAL_2nd;
					g_uhmi_params.appstatus = APP_REFRESH_PAGE;
				}
				//at the second time, reset interval and status, then jump to app
				else if (HEARTBEAT_INTERVAL_2nd == g_uhmi_params.mcustatus.heartbeat_interval)
				{
					g_uhmi_params.mcustatus.heartbeat_interval = HEARTBEAT_INTERVAL_1st;
					g_uhmi_params.mcustatus.run_status = TRUE;
					DPRINT(DBG_LEVEL_ALL, "get no heartbeat reply from mcu, jump to app again at %ld\n", current_time);
					uhmi_jump_to_app();
				}
			}
            break;

        case APP_WAITING:
            break;

        default:
            DPRINT(DBG_LEVEL_DEBUG, "Invalid Status[%d]!", g_uhmi_params.appstatus);
            break;
    }

}
