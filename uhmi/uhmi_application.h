#ifndef __UHMI_APPLICATION_H__
#define __UHMI_APPLICATION_H__

#include "uhmi_common.h"

typedef enum {
    APP_VERSION_CHECK = 1,
    APP_INIT_MCU_DATA,
    APP_SHOW_HOME_PAGE,
    APP_REFRESH_PAGE,
    APP_SCREEN_OFF,
    APP_WAITING,
    APP_HEART_BEAT,
    APP_MAX
} APP_STATUS_T;

void app_rx_handler(u8 *packet, u32 plen);
void app_task(void);
void app_screen_cfg(void);
void app_event_task(long int msgtype);

#endif
