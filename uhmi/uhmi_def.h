#ifndef __UHMI_DEF_H__
#define __UHMI_DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uhmi_application.h"
#include "uhmi_common.h"



/***********************
communication device
#define DEV_UART
#define DEV_SPI
#define DEV_I2C
#define DEV_USB
***********************/
#define DEV_UART

#define PIN_RESET_MCLR      8
#define PIN_BOOT_TRIGGER    7

#define PACKET_SIZE     1024

#define RX_BUF_SIZE 256
#define TX_BUF_SIZE (PACKET_SIZE * 2)

#define MCU_HEX_MAX 5

#define HEARTBEAT_INTERVAL_1st		120
#define HEARTBEAT_INTERVAL_2nd		60

#define WEATHER_REFRESH_INTERVAL  3600

typedef enum
{
    PIN_SET_LOW = 0,
    PIN_SET_HIGH
}GPIO_VALUE_E;

typedef enum {
    FALSE = 0,
    TRUE
} BOOL;

typedef enum {
    MCU_APP_MODE = 0,
    MCU_BOOT_MODE
} MCU_RUN_MODE_E;


typedef struct {
    BOOL                reday;
    BOOL                screen_on;
    long                screen_ontime;
    long                screen_interval;
    PAGE_ID_E           page_id;
    MCU_RUN_MODE_E      rmode;
    DATA_MCU_VERSION_T  version;
} MCU_T;

typedef struct{
	BOOL				run_status;
	long				heartbeat_interval;
	long				heartbeat_ontime;
}MCU_STATUS_T;

typedef struct {
    BOOL        refresh_flag;
    long        refresh_interval;
    long        refresh_ontime;
    long        lastupdate;
}WEATHER_T;

typedef struct {
    u8  total;
    u8  num_2g;
	u8  num_5g;
	u8  num_guest;
	u8  num_wire;
    u8  index;
    u8  mac[HOST_ALL_MAX][SMAC_LEN];
    u8  rename[HOST_ALL_MAX][DEV_NAME_LEN];
    HOST_T hostdev[HOST_ALL_MAX];
} HOSTDEV_TAB_T;


typedef struct {
    MCU_T mcu;
    WEATHER_T weather;

    DATA_MCU_VERSION_T hexver[MCU_HEX_MAX];
    int mcu_hex_count;
    
    APP_STATUS_T    appstatus;

    HOSTDEV_TAB_T   host_table;
    DATA_INTERFACE_STATUS_T if_status;
    DATA_SYSINFO_T  system_info;
    DATA_WIFI_T     wifi_info;
    DATA_HOME_T     home_info;
    DATA_WEATHER_T  weather_info;
	MCU_STATUS_T	mcustatus;

    u8  tx_packet[PACKET_SIZE];
    u32 tx_plen;
    u8  tx_retry;

    int (*dev_init)(void);
    int (*dev_rx)(char *buf, int buf_size);
    int (*dev_tx)(char *data, int data_len);
    void (*dev_close)(void);

} UHMI_GLOBAL_PARAMS;

extern UHMI_GLOBAL_PARAMS g_uhmi_params;

void uhmi_gpio_set(int pin, GPIO_VALUE_E value);
void uhmi_jump_to_app(void);
void uhmi_jump_to_boot(void);

#endif
