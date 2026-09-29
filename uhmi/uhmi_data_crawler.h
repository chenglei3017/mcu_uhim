#ifndef __UHMI_DATA_CRAWLER_H__
#define __UHMI_DATA_CRAWLER_H__

#include "uhmi_def.h"
#include "uhmi_debug.h"
#include "weather.h"

extern pthread_mutex_t host_table_mtx;

long get_uptime(void);
void update_host_table(HOSTDEV_TAB_T *host_table);

int get_system_info(DATA_SYSINFO_T *sysinfo);
int get_wifi_info(DATA_WIFI_T *wifi);
int get_home_info(DATA_HOME_T *homeinfo);
int get_interface_status(DATA_INTERFACE_STATUS_T *ifstatus);
int get_weather_info(DATA_WEATHER_T *weather_info);

void send_system_info(void);
void send_wifi_info(void);
void send_home_info(void);
void send_in_info(void);
void send_host_table(void);
void send_weather_info(void);



#endif
