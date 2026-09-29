#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

#define UHMI_MSG_KEY 1234
#define UHMI_DATA_SIZE 128

#define FTOK_KEY ftok("/usr/sbin/uhmi", 9000)

typedef enum {
    UHMI_MSG_INVALID = 0,
    UHMI_ROUTER_REBOOT,
    UHMI_ROUTER_RESET,
    UHMI_BACK_RECOVERY,
    UHMI_ROUTER_UPGRADE,
    UHMI_WIFI_ON,
    UHMI_WIFI_OFF,
    UHMI_2G_ON,
    UHMI_2G_OFF,
    UHMI_5G_ON,
    UHMI_5G_OFF,
    UHMI_WISP_ON,
    UHMI_WISP_OFF,    
    UHMI_USB_PLUGGED,
    UHMI_USB_UNPLUGGED,
    UHMI_LAN_WAN_CONFLICT,
    UHMI_SCREEN_SET,
    UHMI_UPDATE,
    UHMI_RUNAPP,
    UHMI_VER,
    UHMI_DEBUG,
    UHMI_DUMP,
	UHMI_SCREEN_ON,
	UHMI_SCREEN_OFF,
    UHMI_MSG_MAX
}UHMI_MSG_E;

typedef struct {
    long int        msg_type;
    unsigned char   data[UHMI_DATA_SIZE];
}UHMI_MSG_T;

