#ifndef __UHMI_COMMON_H__
#define __UHMI_COMMON_H__

typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned int        u32;

typedef char        INT8;
typedef short int   INT16;
typedef int         INT32;

#define PROT_VER 0x30

/**************
MCU_MAJOR
1: PIC32MZ1024EFE100
2: STM32F446VET100
***************/
typedef enum
{
  MCU_MAJOR_PIC32EFE=1,
  MCU_MAJOR_ST446=2,
}MCU_MAJOR_E;

/**************
MCU_MCU_MINOR
1: FRC
2: POSC
***************/
typedef enum
{
  MCU_MINOR_INC=1,
  MCU_MINOR_EXC=2,
}MCU_MINOR_E;



#define HOST_SHOW_MAX   5
#define HOST_ALL_MAX    50

#define EMAC_LEN    6   // ether_addr
#define SMAC_LEN    18  // mac string
#define SIP_LEN     16  // ipaddr string

#define DEV_NAME_LEN    36

#define SOH 01
#define EOT 04
#define DLE 0x10

/****************************************************
The frame starts with a control character, Start of
Header (SOH), and ends with another control character,
End of Transmission (EOT).
The Data Link
Control Characters
Some bytes in the Data field may imitate the control
characters, SOH and EOT. The Data Link
Escape (DLE) character is used to escape such bytes
that could be interpreted as control characters. The
bootloader always accepts the byte following a <DLE>
as data, and always sends a <DLE> before any of the
control characters.
****************************************************/

typedef enum {
    MSG_INVALID = 0,
    MSG_MCU_VERSION_INFO,   // DATA_MCU_VERSION_T
    MSG_REDAY,
    MSG_BUTTON,             // BT_TYPE_E
    MSG_PAGE_ID,            // PAGE_ID_E
    MSG_INTERFACE_STATUS,   // INTERNET_STATUS_T
    MSG_HOME_INFO,          // DATA_HOME_T
    MSG_WIFI_INFO,          // DATA_WIFI_T
    MSG_HOSTDEV_INFO,       // DATA_HOSTDEV_T
    MSG_SYSTEM_INFO,        // DATA_SYSTEM_T
    MSG_EVENT,              // EVENT_TYPE_E
    MSG_WEATHER_INFO,       // DATA_WEATHER_T
    MSG_MCU_RESUMED,
    MSG_MX = 0xf9
}MSG_TYPE_E;

typedef enum {
    PAGE_INVALID = 0,
    PAGE_SOFTUPDATE,
    PAGE_SYSINFO,
    PAGE_SYSIFACE,
    PAGE_HOME,
    PAGE_WEATHER,
    PAGE_WIFI,
    PAGE_HOSTDEV,
    PAGE_MAX,
}PAGE_ID_E;

typedef enum {
    BT_INVALID = 0,
    BT_L_SCLICK,
    BT_L_DBLCLICK,
    BT_L_LONGCLICK,
    BT_R_SCLICK,
    BT_R_DBLCLICK,
    BT_R_LONGCLICK,
    BT_M_SCLICK,
    BT_M_DBLCLICK,
    BT_M_LONGCLICK,
    BT_MAX
}BT_TYPE_E;

typedef enum {
    SSID_2G = 0,
    SSID_5G,
    SSID_GUEST,
    SSID_MAX
}SSID_TYPE_E;

typedef enum {
    CONNECT_TYPE_LAN = 0,
    CONNECT_TYPE_2G,
    CONNECT_TYPE_5G,
    CONNECT_TYPE_GUEST,
    CONNECT_TYPE_MAX
}CONNECT_TYPE_E;


typedef enum {
    WIFI_ON = 0,
    WIFI_OFF,
    WIFI_2G_ON,
    WIFI_2G_OFF,
    WIFI_5G_ON,
    WIFI_5G_OFF,
    REPEATER_ON,            //UHMI_WISP_ON
    REPEATER_OFF,           //UHMI_WISP_OFF
    REPEATER_FIAL,
    SCREEN_ON,
    SCREEN_OFF,
    BACK_RECOVERY,          //UHMI_BACK_RECOVERY
    ROUTER_UPGRADE,         //UHMI_ROUTER_UPGRADE
    REBOOT,                 //UHMI_ROUTER_REBOOT
    FACTORY_RESET,          //UHMI_ROUTER_RESET
    LAN_WAN_CONFLICT,       //UHMI_LAN_WAN_CONFLICT
    EVENT_TYPE_MAX
} EVENT_TYPE_E;

typedef enum {
    LOGO_DEFAULT = 0,   //2 &icon_default
    LOGO_1JIA,          //2 &icon_1jia
    LOGO_360,           //2 &icon_360
    LOGO_ASUS,          //2 &icon_asus
    LOGO_COOLPAD,       //2 &icon_coolpad
    LOG_DELL,           //2 &icon_dell
    LOG_HAIER,          //2 &icon_haier
    LOG_HASEE,          //2 &icon_hasee
    LOG_HONOR,          //2 &icon_honor
    LOG_HP,             //2 &icon_hp
    LOG_HTC,            //2 &icon_htc
    LOG_HUWEI,          //2 &icon_huawei
    LOG_IPHONE,         //2 &icon_iPhone
    LOG_LENOVO,         //2 &icon_lenovo
    LOG_LETV,           //2 &icon_letv
    LOG_LG,             //2 &icon_lg
    LOG_MEITU,          //2 &icon_meitu
    LOG_MEIZU,          //2 &icon_meizu
    LOG_OPPO,           //2 &icon_oppo
    LOG_PHICOMM,        //2 &icon_phicomm
    LOG_SAMSUNG,        //2 &icon_samsung
    LOG_SMARTISAN,      //2 &icon_smartisan
    LOG_SONY,           //2 &icon_sony
    LOG_TCL,            //2 &icon_tcl
    LOG_THINKPAD,       //2 &icon_thinkpad
    LOG_TONGFANG,       //2 &icon_tongfang
    LOG_VIVO,           //2 &icon_vivo
    LOG_WINDOWSPHONE,   //2 &icon_windowsphone
    LOG_XIAOMI,         //2 &icon_xiaomi
    LOG_ZTE,            //2 &icon_zte
}LOGO_ID_E;

typedef struct {
    u8  ssid_name[64];
    u8  ssid_pwd[64];
    u8  ssid_en;
    u8  client_nu;
    u8  pad0[2];
}SSID_INFO_T;

typedef struct {
    u16 swver;
    u8 major;      // MCU
    u8 minor;      // Peripherals
}DATA_MCU_VERSION_T;

typedef struct {
    u8 lan1_linked;
    u8 lan2_linked;
    u8 lan3_linked;
    u8 lan4_linked;
    u8 wan_linked;
    u8 usb_plugged;
    u8 pad0[2];
}DATA_INTERFACE_STATUS_T;

typedef struct {
    u8  connected;
    u8  flag;
    u8  mode;
    u8  pad0[1];
    u8  curip[SIP_LEN];
    u32 usrate;
    u32 dsrate;
}DATA_HOME_T;


typedef struct {
    u32 usrate;
    u32 dsrate;
    u8 hostname[DEV_NAME_LEN];
    u8 logo;
    u8 ct_type;
    u8 pad0[2];
}HOST_T;

typedef struct {
    u8 total;   // The total number of devices
    u8 index;   // current show index
    u8 num;     // The ubnmber of devices need display
    u8 pad0[1];
    HOST_T hostdev[HOST_SHOW_MAX];
}DATA_HOSTDEV_T;

typedef struct {
    u8 wifi2in1;
    u8 pad0[3];
    SSID_INFO_T ssid_info[SSID_MAX];
}DATA_WIFI_T;

typedef struct {
    u8  mn[8];  // Model number
    u8  hw[8];  // Hardware Version
    u8  sw[24]; // Software version
    u8  newsw[24]; // Software new version
    u8  mac[SMAC_LEN]; // Mac addr
}DATA_SYSINFO_T;

typedef struct {
    u8 city_ch[32];
    u8 temperature[4];
    u8 date[11];      //YYYY-MM-DD
    u8 time[6];       //hh:mm
    u8 weather_code;
    u8 week;
    u8 error;         //0 is ok, 99 or else
}DATA_WEATHER_T;

u16 CalculateCrc(char *data, unsigned int len);
#endif

