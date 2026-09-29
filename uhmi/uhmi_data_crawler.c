#include "uhmi_data_crawler.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <dhcpd.h>
#include <ip_ctl.h>
#include <wlcr_main.h>
#include <wlcr_shared.h>
#include <sys/sysinfo.h>
#include <bcmnvram.h>
#include <dirent.h>
#include <oui.h>

#include "uhmi_debug.h"
#include "hz2py.h"

pthread_mutex_t host_table_mtx = PTHREAD_MUTEX_INITIALIZER;


#define DEV_IP_LEN          16 // xxx.xxx.xxx.xxx?
#define DEV_MAC_LEN         18 // xx:xx:xx:xx:xx:xx

#define NVRAM_RENAME_LEN    128

//#define NVRAM_RATE_LIMIT  "dev_ratelimit"
#define NVRAM_RENAME        "dev_rename"
//#define NVRAM_IPCTL_RULES "ip_ctl_rules"
#define NVRAM_DEVDENY       "blockmac_rules"
#define NVRAM_STATIC_LIST   "dhcpreve_rules"

#define SCREEN_SSID_COVER   "********"

long get_uptime(void)
{
    struct sysinfo s_info;
    memset(&s_info, 0, sizeof(s_info));
    if(sysinfo(&s_info))
    {
        DPRINT(DBG_LEVEL_ERROR, "get uptime fail: %s\n", strerror(errno));
    }
    return s_info.uptime;

#if 0
    float   ltime =  0;
    char    buf[128] = {0};
    FILE    *fp = NULL;
    char    *p = NULL;
    fp = fopen("/proc/uptime", "r");
    if (fp)
    {
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%f%*f", &ltime);
        fclose(fp);
    }
    return ltime;
#endif
}

/************************************
return:
-1: error
 0: interface status no change
 1: interface status changed
************************************/
int get_interface_status(DATA_INTERFACE_STATUS_T *ifstatus)
{
    int ret = 0;

    FILE *fp = NULL;
    char temp[1024] = {0};
    char cmd[256] = {0};
    int i = 0, j = 0;
    BOOL usbflag = FALSE;

    DIR *dirfd = NULL;
    struct dirent *dir;

    DATA_INTERFACE_STATUS_T if_status;
    memset(&if_status, 0, sizeof(if_status));

    snprintf(cmd, sizeof(cmd) - 1, "et port_status all");

    if(!(fp = popen(cmd, "r")))
    {
        fprintf(stderr, "popen failed");
        goto err_exit;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        //Ignore the first three rows
        if(i <= 2)
        {
            i++;
            memset(temp, 0x0, sizeof(temp));
            continue;
        }
        //
        if(strstr(temp, "Up"))
        {
            switch(j)
            {
                case 0:
                    if_status.lan2_linked = 1;
                    break;

                case 1:
                    if_status.lan1_linked = 1;
                    break;

                case 2:
                    if_status.lan3_linked = 1;
                    break;

                case 3:
                    if_status.wan_linked = 1;
                    break;

                default:
                    break;
            }
        }
        j++;
        memset(temp, 0x0, sizeof(temp));
    }
    pclose(fp);




    dirfd = opendir("/tmp/share");
    if(dirfd != NULL)
    {
        while((dir = readdir(dirfd)) != NULL)
        {
            if(strncmp(dir->d_name, "sd", 2) == 0)
            {
                usbflag = TRUE;
                break;
            }
        }
        closedir(dirfd);
    }

    if (usbflag)
        if_status.usb_plugged = 1;
    else
        if_status.usb_plugged = 0;

    if (memcmp(&if_status, ifstatus, sizeof(if_status)))
    {
        ret = 1;
        memcpy(ifstatus, &if_status, sizeof(if_status));
    }


err_exit:

    return ret;

}

void send_interface_status(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_INTERFACE_STATUS;
    memcpy(&data[dlen], &g_uhmi_params.if_status, sizeof(g_uhmi_params.if_status));
    dlen += sizeof(g_uhmi_params.if_status);
    uhmi_send_data(data, dlen);
}


/*
 * Copy list info from shared-memory.
 *
 */
static int get_dev_list_from_shm(HOSTDEV_TAB_T *host_table)
{
    struct flock lk_info = {0};
    int lock_fd = -1;
    int sum = -1;
    int retry = 3;
    int index = 0;
    char buff[TERMNG_SHARED_SPACE] = {0};
    struct _wlcr *wlcr_p = NULL;
    int i = 0, j = 0;

    if (NULL == host_table)
    {
        DPRINT(DBG_LEVEL_ERROR, "host_table: %p\n", host_table);
        return sum;
    }
    if (nvram_match("mode","ap"))
        return sum;
    //system("killall -SIGUSR1 wl_cr"); /* daemon progress modify, needn't signal */

    retry ;
    while (--retry)
    {
        lock_fd = open(WLCR_LOCK_FILE, O_RDWR|O_CREAT, 0666);
        if (lock_fd < 0)
        {
            DPRINT(DBG_LEVEL_DEBUG, "lock_fd try open fail\n");
        }
        else
        {
            break;
        }
        usleep(20); //sleep 20 ms, and try again
    }
    if (!retry)
    {
        DPRINT(DBG_LEVEL_DEBUG, "lock_fd open fail\n");
        return sum;
    }

    retry = 3;
    while (--retry)
    {
        lk_info.l_type = F_WRLCK;
        lk_info.l_whence = SEEK_SET;
        lk_info.l_start = 0;
        lk_info.l_len = 0;
        if (!fcntl(lock_fd, F_SETLKW, &lk_info))
        {
            DPRINT(DBG_LEVEL_DEBUG, "lock_fd lock\n");
            break;
        }
    }
    if (!retry) {
        DPRINT(DBG_LEVEL_DEBUG, "Failed locking LOCK_FILE: %s\n", strerror(errno));
        close(lock_fd);
        unlink(WLCR_LOCK_FILE);
        return sum;
    }

    memset(buff, 0x00, sizeof(buff));
    devm_shared_read(buff, TERMNG_SHARED_SPACE);
    wlcr_p = (struct _wlcr *)buff;


    pthread_mutex_lock(&host_table_mtx);

    index = host_table->index;
    memset(host_table, 0, sizeof(HOSTDEV_TAB_T));

    sum = wlcr_p->list_2g.count + wlcr_p->list_5g.count + wlcr_p->list_guest.count + wlcr_p->list_wire.count;
    host_table->total = ((sum < HOST_ALL_MAX)? sum : HOST_ALL_MAX);

    host_table->index = ((index*5) < host_table->total)?index:(host_table->total/5);

    host_table->num_2g = wlcr_p->list_2g.count;
    host_table->num_5g = wlcr_p->list_5g.count;
    host_table->num_guest = wlcr_p->list_guest.count;
    host_table->num_wire = wlcr_p->list_wire.count;

    for (i = 0; i < wlcr_p->list_2g.count; i++)
    {
        strncpy(host_table->mac[j], wlcr_p->list_2g.info[i].str_mac, SMAC_LEN-1);
        host_table->hostdev[j].ct_type = CONNECT_TYPE_2G;
        host_table->hostdev[j].dsrate = wlcr_p->list_2g.info[i].rate_tx;
        host_table->hostdev[j].usrate = wlcr_p->list_2g.info[i].rate_rx;

        if ((++j) >= HOST_ALL_MAX)
            goto table_full;
    }

    for (i = 0; i < wlcr_p->list_5g.count; i++)
    {
        strncpy(host_table->mac[j], wlcr_p->list_5g.info[i].str_mac, SMAC_LEN-1);
        host_table->hostdev[j].ct_type = CONNECT_TYPE_5G;
        host_table->hostdev[j].dsrate = wlcr_p->list_5g.info[i].rate_tx;
        host_table->hostdev[j].usrate = wlcr_p->list_5g.info[i].rate_rx;

        if ((++j) >= HOST_ALL_MAX)
            goto table_full;
    }

    for (i = 0; i < wlcr_p->list_guest.count; i++)
    {
        strncpy(host_table->mac[j], wlcr_p->list_guest.info[i].str_mac, SMAC_LEN-1);
        host_table->hostdev[j].ct_type = CONNECT_TYPE_GUEST;
        host_table->hostdev[j].dsrate = wlcr_p->list_guest.info[i].rate_tx;
        host_table->hostdev[j].usrate = wlcr_p->list_guest.info[i].rate_rx;

        if ((++j) >= HOST_ALL_MAX)
            goto table_full;
    }

    for (i = 0; i < wlcr_p->list_wire.count; i++)
    {
        if (wlcr_p->list_wire.info[i].online == 0)
            continue;
        strncpy(host_table->mac[j], wlcr_p->list_wire.info[i].str_mac, SMAC_LEN-1);
        host_table->hostdev[j].ct_type = CONNECT_TYPE_LAN;
        host_table->hostdev[j].dsrate = wlcr_p->list_wire.info[i].rate_tx;
        host_table->hostdev[j].usrate = wlcr_p->list_wire.info[i].rate_rx;

        if ((++j) >= HOST_ALL_MAX)
            goto table_full;
    }

table_full:
    pthread_mutex_unlock(&host_table_mtx);
    //2 remove lock file
    DPRINT(DBG_LEVEL_INFO, "(%s %d)lock_fd remove\n", __FUNCTION__, __LINE__);
    close(lock_fd);
    unlink(WLCR_LOCK_FILE);

    return sum;
}

struct lease_t {
    unsigned char chaddr[16];
    u_int32_t yiaddr;
    u_int32_t expires;
    char hostname[64];
};
static int sync_hostname_to_link(HOSTDEV_TAB_T *host_table, struct lease_t *p_lease)
{
    char mac[32] = {0};
    int i = 0;

    if (NULL == host_table || NULL == p_lease)
        return -1;

    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
            p_lease->chaddr[0], p_lease->chaddr[1], p_lease->chaddr[2],
            p_lease->chaddr[3], p_lease->chaddr[4], p_lease->chaddr[5]);

    for (i = 0; i < host_table->total; i++)
    {
        if(strncasecmp(host_table->mac[i], mac, SMAC_LEN-1) == 0)
        {
            //hostname chinese encode by gb2312
            strncpy(host_table->hostdev[i].hostname, p_lease->hostname, sizeof(host_table->hostdev[i].hostname)-1);
            break;
        }
    }
    return 0;
}

/*
 * get client hostname from udhcpd lease file.
 * by the way, get online time(lan_lease - expires), because APP's CGI need it
 */
static int get_dev_status_hostname(HOSTDEV_TAB_T *host_table)
{
    FILE *fp_leases = NULL;
    struct lease_t lease;
    int index, num_interfaces=0;
    char path_lease[64];
    char sigline[] = "-XX";

    if (NULL == host_table)
        return -1;

    /* Write out leases file */
	// solve the problem that KB-712
//	sprintf(sigline, "-%d", SIGUSR1);
	sprintf(sigline, "-%d", SIGCHLD);
    eval("killall", sigline, "udhcpd");

    /* Count the number of lan and guest interfaces */
    if (nvram_get("lan_ifname"))
        num_interfaces++;
    if (nvram_get("lan1_ifname"))
        num_interfaces++;

    for (index = 0; index < num_interfaces; index++)
    {
        snprintf(path_lease, sizeof(path_lease), "/tmp/udhcpd%d.leases", index);

        if (!(fp_leases= fopen(path_lease, "r")))
        {
            DPRINT(DBG_LEVEL_DEBUG, "open file %s error\n", path_lease);
            continue;
        }

        while (fread(&lease, sizeof(lease), 1, fp_leases))
        {
            /* Do not display reserved leases */
            if (ETHER_ISNULLADDR(lease.chaddr))
            {
                DPRINT(DBG_LEVEL_DEBUG, "parse leases IP error\n");
                continue;
            }

            sync_hostname_to_link(host_table, &lease);
        }
        fclose(fp_leases);
    }

    return 0;
}


/*
 * rename nvram like: dev_rename="d8:42:ac:11:22:33,phicomm"
 */
static int get_dev_status_rename(HOSTDEV_TAB_T *host_table)
{
    char *rules = nvram_safe_get(NVRAM_RENAME);
    char rec[256];
    char mac[DEV_MAC_LEN];
    char rename[NVRAM_RENAME_LEN];
    int i = 0, j = 0;
    char s_base64[NVRAM_RENAME_LEN * 2];

    if(strlen(rules) == 0 || NULL == host_table)
        return -1;

    while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1)
    {
        //i++;
        if ((getNthValueSafe(0, rec, ',', mac, sizeof(mac)) == -1))
        {
            continue;
        }
        if ((getNthValueSafe(1, rec, ',', s_base64, sizeof(s_base64)) == -1))
        {
            continue;
        }
        websDecode64(rename, s_base64, sizeof(rename));

        for (j = 0; j < host_table->total; j++)
        {
            if(strncasecmp (host_table->mac[j], mac, SMAC_LEN) == 0)
            {
                strncpy(host_table->rename[j], rename, sizeof(host_table->rename[j])-1);
                break;
            }
        }
    }

    return 0;
}

// Updated host table every five seconds
void update_host_table(HOSTDEV_TAB_T *host_table)
{
    int ret = 0, i = 0;
    char buf[128] = {0};
    char hz[64] = {0};
    char py[64] = {0};

    if (NULL == host_table)
    {
        DPRINT(DBG_LEVEL_DEBUG, "host_table: %p\n", host_table);
        return;
    }
    else
    {
        DPRINT(DBG_LEVEL_DEBUG, "Enter\n");
    }


    ret = get_dev_list_from_shm(host_table);
    if (ret > 0)
    {
        get_dev_status_hostname(host_table);

        get_dev_status_rename(host_table);

        oui_handler_t oui = load_available_oui();
        for (i = 0; i < host_table->total; i++)
        {
            memset(hz, 0, sizeof(hz));
            memset(py, 0, sizeof(py));

            if(strlen(host_table->rename[i]) > 0)
            {
                strncpy(hz, host_table->rename[i], sizeof(hz)-1);
                hz2py(hz, py, strlen(hz), sizeof(py));
                strncpy(host_table->hostdev[i].hostname, py, DEV_NAME_LEN-1);
            }
            else if(strlen(host_table->hostdev[i].hostname) > 0) //translate chinese code, GB2312 to UTF-8
            {
                strncpy(hz, host_table->hostdev[i].hostname, sizeof(hz)-1);
                hz2py(hz, py, strlen(hz), sizeof(py));
                strncpy(host_table->hostdev[i].hostname, py, DEV_NAME_LEN-1);
            }
            else
            {
                strncpy(host_table->hostdev[i].hostname, "Unknow", DEV_NAME_LEN-1);
            }

            host_table->hostdev[i].logo = get_vendor_id(oui, host_table->mac[i]);
        }
        if (oui)
            release_oui(oui);
    }

}



int get_system_info(DATA_SYSINFO_T *sysinfo)
{
    u8 mac[18] = {0};
    char fw_version[32]={0};
    strncpy(fw_version,nvram_safe_get("fw_version"),sizeof(fw_version)-1);

    if (NULL == sysinfo)
    {
        DPRINT(DBG_LEVEL_ERROR, "sysinfo: %p invalid!\n", sysinfo);
        return -1;
    }

    strncpy(sysinfo->hw, nvram_safe_get("hd_version"), sizeof(sysinfo->hw) - 1);
    if (fw_version[0]=='V')
    {
        strncpy(sysinfo->sw, fw_version+1, sizeof(sysinfo->sw) - 1);
    }
    else
    {
        strncpy(sysinfo->sw, fw_version, sizeof(sysinfo->sw) - 1);
    }
    strncpy(sysinfo->mn, nvram_safe_get("product"), sizeof(sysinfo->mn) - 1);
    strncpy(sysinfo->newsw,nvram_safe_get("sw_version"), sizeof(sysinfo->newsw) - 1);
    strncpy(sysinfo->mac, nvram_safe_get("et0macaddr"), sizeof(sysinfo->mac) - 1);
    //memcpy(sysinfo->mac, (u8 *)ether_aton(mac), sizeof(sysinfo->mac));

    DPRINT(DBG_LEVEL_INFO, "hd_version[%s], fw_version[%s], product[%s],sw_version[%s]\n",
        sysinfo->hw, sysinfo->sw, sysinfo->mn,sysinfo->newsw);

    DPRINT(DBG_LEVEL_INFO, "mac[%s]\n", sysinfo->mac);

}

void send_system_info(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_SYSTEM_INFO;
    memcpy(&data[dlen], &g_uhmi_params.system_info, sizeof(g_uhmi_params.system_info));
    dlen += sizeof(g_uhmi_params.system_info);
    uhmi_send_data(data, dlen);
}

int get_wifi_info(DATA_WIFI_T *wifi)
{
    char hz[64] = {0};
    char py[64] = {0};

    int wispBand = 0;
    int wispSwitch = 0;

    if (NULL == wifi)
    {
        DPRINT(DBG_LEVEL_ERROR, "wifi: %p invalid!\n", wifi);
        return -1;
    }

    wifi->wifi2in1 = (strncmp(nvram_safe_get("bsd_role"), "0", 1)?1:0);
    wispBand=atoi(nvram_safe_get("frequency"));
    wispSwitch=atoi(nvram_safe_get("ure_disable"));
    if((wispSwitch==0)&&(wispBand==0))
    {
        wifi->ssid_info[SSID_2G].ssid_en = atoi(nvram_safe_get("wl0.1_bss_enabled"));

        strncpy(hz, nvram_safe_get("wl0.1_ssid"), sizeof(hz)-1);
        hz2py(hz, py, strlen(hz), sizeof(py));
        strncpy(wifi->ssid_info[SSID_2G].ssid_name, py, sizeof(wifi->ssid_info[SSID_2G].ssid_name)-1);

        strncpy(wifi->ssid_info[SSID_2G].ssid_pwd, nvram_safe_get("wl0.1_wpa_psk"), sizeof(wifi->ssid_info[SSID_2G].ssid_pwd)-1);
    }
    else
    {
        wifi->ssid_info[SSID_2G].ssid_en = atoi(nvram_safe_get("wl0_bss_enabled"));

        strncpy(hz, nvram_safe_get("wl0_ssid"), sizeof(hz)-1);
        hz2py(hz, py, strlen(hz), sizeof(py));
        strncpy(wifi->ssid_info[SSID_2G].ssid_name, py, sizeof(wifi->ssid_info[SSID_2G].ssid_name)-1);

        strncpy(wifi->ssid_info[SSID_2G].ssid_pwd, nvram_safe_get("wl0_wpa_psk"), sizeof(wifi->ssid_info[SSID_2G].ssid_pwd)-1);
    }

    DPRINT(DBG_LEVEL_INFO, "SSID_2G: %s pwd: %s\n", py, wifi->ssid_info[SSID_2G].ssid_pwd);

    memset(hz, 0, sizeof(hz));
    memset(py, 0, sizeof(py));
    if((wispSwitch==0)&&(wispBand==1))
    {
        wifi->ssid_info[SSID_5G].ssid_en = atoi(nvram_safe_get("wl1.1_bss_enabled"));

        strncpy(hz, nvram_safe_get("wl1.1_ssid"), sizeof(hz)-1);
        hz2py(hz, py, strlen(hz), sizeof(py));
        strncpy(wifi->ssid_info[SSID_5G].ssid_name, py, sizeof(wifi->ssid_info[SSID_5G].ssid_name)-1);

        strncpy(wifi->ssid_info[SSID_5G].ssid_pwd, nvram_safe_get("wl1.1_wpa_psk"), sizeof(wifi->ssid_info[SSID_5G].ssid_pwd)-1);
    }
    else
    {
        wifi->ssid_info[SSID_5G].ssid_en = atoi(nvram_safe_get("wl1_bss_enabled"));

        strncpy(hz, nvram_safe_get("wl1_ssid"), sizeof(hz)-1);
        hz2py(hz, py, strlen(hz), sizeof(py));
        strncpy(wifi->ssid_info[SSID_5G].ssid_name, py, sizeof(wifi->ssid_info[SSID_5G].ssid_name)-1);

        strncpy(wifi->ssid_info[SSID_5G].ssid_pwd, nvram_safe_get("wl1_wpa_psk"), sizeof(wifi->ssid_info[SSID_5G].ssid_pwd)-1);


    }
    DPRINT(DBG_LEVEL_INFO, "SSID_5G: %s pwd: %s\n", py, wifi->ssid_info[SSID_5G].ssid_pwd);


    wifi->ssid_info[SSID_GUEST].ssid_en = atoi(nvram_safe_get("vis_ssid_enable"));

    memset(hz, 0, sizeof(hz));
    memset(py, 0, sizeof(py));
    strncpy(hz, nvram_safe_get("vis_ssid"), sizeof(hz)-1);
        hz2py(hz, py, strlen(hz), sizeof(py));
        strncpy(wifi->ssid_info[SSID_GUEST].ssid_name, py, sizeof(wifi->ssid_info[SSID_GUEST].ssid_name)-1);
        DPRINT(DBG_LEVEL_INFO, "SSID_GUEST: %s\n", py);

    strncpy(wifi->ssid_info[SSID_GUEST].ssid_pwd, nvram_safe_get("vis_ssid_pwd"), sizeof(wifi->ssid_info[SSID_GUEST].ssid_pwd)-1);

    //added by lingfeng.fu, display wlreless pwd or cover with *, 3/13/2017
    if (atoi(nvram_safe_get("screen_2G5G_pwd_en")) == 0)
    {
        if (strlen(wifi->ssid_info[SSID_2G].ssid_pwd) != 0)
        {
           memset(wifi->ssid_info[SSID_2G].ssid_pwd, 0x0, sizeof(wifi->ssid_info[SSID_2G].ssid_pwd) - 1);
           strncpy(wifi->ssid_info[SSID_2G].ssid_pwd, SCREEN_SSID_COVER, sizeof(SCREEN_SSID_COVER) - 1);
        }
        if (strlen(wifi->ssid_info[SSID_5G].ssid_pwd) != 0)
        {
           memset(wifi->ssid_info[SSID_5G].ssid_pwd, 0x0, sizeof(wifi->ssid_info[SSID_5G].ssid_pwd) - 1);
           strncpy(wifi->ssid_info[SSID_5G].ssid_pwd, SCREEN_SSID_COVER, sizeof(SCREEN_SSID_COVER) - 1);
        }
    }
    if (atoi(nvram_safe_get("screen_guest_pwd_en")) == 0)
    {
        if (strlen(wifi->ssid_info[SSID_GUEST].ssid_pwd) != 0)
        {
            memset(wifi->ssid_info[SSID_GUEST].ssid_pwd, 0x0, sizeof(wifi->ssid_info[SSID_GUEST].ssid_pwd) - 1);
            strncpy(wifi->ssid_info[SSID_GUEST].ssid_pwd, SCREEN_SSID_COVER, sizeof(SCREEN_SSID_COVER) - 1);
        }
    }

    //get_dev_list_from_shm(NULL, &count);

    wifi->ssid_info[SSID_2G].client_nu = g_uhmi_params.host_table.num_2g;
    wifi->ssid_info[SSID_5G].client_nu = g_uhmi_params.host_table.num_5g;
    wifi->ssid_info[SSID_GUEST].client_nu = g_uhmi_params.host_table.num_guest;

    return 0;

}

void send_wifi_info(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_WIFI_INFO;
    memcpy(&data[dlen], &g_uhmi_params.wifi_info, sizeof(g_uhmi_params.wifi_info));
    dlen += sizeof(g_uhmi_params.wifi_info);
    uhmi_send_data(data, dlen);
}

int get_home_info(DATA_HOME_T *homeinfo)
{

    if (NULL == homeinfo)
    {
        DPRINT(DBG_LEVEL_ERROR, "homeinfo: %p invalid!\n", homeinfo);
        return -1;
    }

    if (strcmp(nvram_safe_get("pingcheck"), "1"))
    {
        homeinfo->connected = 0;
        homeinfo->usrate = 0;
        homeinfo->dsrate = 0;
    }
    else
    {
        homeinfo->connected = 1;
        homeinfo->flag = 1-atoi(nvram_safe_get("ure_disable"));
        homeinfo->usrate = atoi(nvram_safe_get("wan_txbytes"));
        homeinfo->dsrate = atoi(nvram_safe_get("wan_rxbytes"));
    }
    if( nvram_match("mode","router"))
    {
        homeinfo->mode = 0;
    }
    else if(nvram_match("mode","ap"))
    {
        homeinfo->mode = 1;
        memset(homeinfo->curip,0x0,SIP_LEN);
        if(strlen(nvram_safe_get("lan_ipaddr"))>0)
        {
            strncpy(homeinfo->curip,nvram_safe_get("lan_ipaddr"),SIP_LEN-1);
        }
        else
        {
            strncpy(homeinfo->curip,nvram_safe_get("ap_static_ipaddr"),SIP_LEN-1);
        }
    }
    DPRINT(DBG_LEVEL_INFO,"con:%d,flag:%d,mode:%d,ip:%s,usrate:%d,dsrate:%d\n",homeinfo->connected,homeinfo->flag,homeinfo->mode,
                    homeinfo->curip,homeinfo->usrate,homeinfo->dsrate);
    return 0;
}

void send_home_info(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_HOME_INFO;
    memcpy(&data[dlen], &g_uhmi_params.home_info, sizeof(g_uhmi_params.home_info));
    dlen += sizeof(g_uhmi_params.home_info);
    uhmi_send_data(data, dlen);
}

void send_host_table(void)
{
    DATA_HOSTDEV_T htable;
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

	memset(&htable, 0, sizeof(htable));

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_HOSTDEV_INFO;
#if 1
    pthread_mutex_lock(&host_table_mtx);
    htable.total = g_uhmi_params.host_table.total;
    htable.index = g_uhmi_params.host_table.index;

    memcpy(htable.hostdev, &g_uhmi_params.host_table.hostdev[htable.index * 5], sizeof(HOST_T) * HOST_SHOW_MAX);

    memcpy(&data[dlen], &htable, sizeof(htable));
    dlen += sizeof(htable);
    uhmi_send_data(data, dlen);

    DPRINT(DBG_LEVEL_DEBUG,"total:%d, index: %d\n", htable.total, htable.index);
    DPRINT(DBG_LEVEL_DEBUG,"hostname: %s, dsrate: %u, usrate: %u, log: %d\n",
        htable.hostdev[0].hostname, htable.hostdev[0].dsrate, htable.hostdev[0].usrate, htable.hostdev[0].logo);
    pthread_mutex_unlock(&host_table_mtx);
#endif
}

int get_weather_info(DATA_WEATHER_T *weather_info)
{
    if (NULL == weather_info)
    {
        DPRINT(DBG_LEVEL_DEBUG, "weather_info: %p invalid!\n", weather_info);
        return -1;
    }
    weather_screen *weather = NULL;
    weather_time * time_c = NULL;
    int error;
    int ret = 0;

    weather = (struct weather_screen_t *)malloc(sizeof(struct weather_screen_t));
    time_c = (struct weather_time_t *)malloc(sizeof(struct weather_time_t));
    memset(weather, 0, sizeof(struct weather_screen_t));
    memset(time_c, 0, sizeof(struct weather_time_t));
    if ((NULL == weather) || (NULL == time_c))
    {
        DPRINT(DBG_LEVEL_DEBUG, "weather %p or time %p invalid!\n", weather, time_c);
        ret = -1;
        goto err_exit;
    }

    //add chinese city name, in UTF-8
    if (0 != strlen(nvram_safe_get("county_ch")))
    {
        strncpy(weather_info->city_ch, nvram_safe_get("county_ch"), sizeof(weather_info->city_ch));
    }
    else
    {
		strncpy(weather_info->city_ch, nvram_safe_get("city_ch"), sizeof(weather_info->city_ch));
    }
	//add error, weather_code and temperature
	if (0 != strlen(nvram_safe_get("weather_code")))
	{
		weather_info->weather_code = (u8)atoi(nvram_safe_get("weather_code"));
	}
	if (0 != strlen(nvram_safe_get("weather_temp")))
	{
		strncpy(weather_info->temperature, nvram_safe_get("weather_temp"), sizeof(weather_info->temperature));
	}
	weather_info->error = (u8)atoi(nvram_safe_get("weather_error"));


    //add date(YYYY-MM-DD), time(hh:mm) and week(0-6 means Sun to Mon)
    if (0 != get_time_now(time_c))
    {
        DPRINT(DBG_LEVEL_DEBUG, "fail to get_time_now!\n");
    }
    snprintf(weather_info->date, sizeof(weather_info->date), "%04d-%02d-%02d", time_c->year, time_c->month, time_c->day);
    snprintf(weather_info->time, sizeof(weather_info->time), "%02d:%02d", time_c->hour, time_c->minute);
    weather_info->week = (u8)(time_c->week);

err_exit:
    if (weather)
    {
        free(weather);
        weather = NULL;
    }
    if (time_c)
    {
        free(time_c);
        time_c = NULL;
    }
#if 0
    DPRINT(DBG_LEVEL_DEBUG, "city_ch %s\n", weather_info->city_ch);
    DPRINT(DBG_LEVEL_DEBUG, "weather_code %d\n", weather_info->weather_code);
    DPRINT(DBG_LEVEL_DEBUG, "temperature %s\n", weather_info->temperature);
    DPRINT(DBG_LEVEL_DEBUG, "date %s\n", weather_info->date);
    DPRINT(DBG_LEVEL_DEBUG, "time %s\n", weather_info->time);
    DPRINT(DBG_LEVEL_DEBUG, "week %d\n", weather_info->week);
#endif
    return ret;
}

void send_weather_info(void)
{
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;

    data[dlen++] = PROT_VER;
    data[dlen++] = MSG_WEATHER_INFO;
    memcpy(&data[dlen], &g_uhmi_params.weather_info, sizeof(g_uhmi_params.weather_info));
    dlen += sizeof(g_uhmi_params.weather_info);

    uhmi_send_data(data, dlen);

}
