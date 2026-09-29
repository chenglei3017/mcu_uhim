#ifndef  __MCU_UI_H
#define  __MCU_UI_H


#if defined(__cplusplus)
	extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "mcu_mz.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define MCU_CONST   const

//#define pi (4.0*atan(1.0))
#define pi 3.1415926535898
#define Sin(n) sin(((n)*pi)*1.0/180)
#define Cos(n) cos(((n)*pi)*1.0/180)

enum MCU_FONT{
    MCU_FONT_WQY14=0,
    MCU_FONT_WQY16,
    MCU_FONT_WQY18,
    MCU_CITY_FONT_WQY18,
    MCU_WEATHER_WQY18,
    MCU_FONT_WQY20,
    MCU_FONT_WQY22,
    MCU_FONT_WQY24,
    MCU_FONT_WQY26,
    MCU_FONT_PF42,
    MCU_FONT_PF48
};




enum MCU_BMP{
    MCU_BMP_D_DEFAULT=0,
    MCU_BMP_D_1JIA,
    MCU_BMP_D_360,
    MCU_BMP_D_ASUNS,
    MCU_BMP_D_COOLPAD,
    MCU_BMP_D_DELL,
    MCU_BMP_D_HAIER,
    MCU_BMP_D_HASEE,
    MCU_BMP_D_HONOR,
    MCU_BMP_D_HP,
    MCU_BMP_D_HTC,
    MCU_BMP_D_HUAWEI,
    MCU_BMP_D_IPHONE,
    MCU_BMP_D_LENOVO,
    MCU_BMP_D_LETV,
    MCU_BMP_D_LG,
    MCU_BMP_D_MEITU,
    MCU_BMP_D_MEIZU,
    MCU_BMP_D_OPPO,
    MCU_BMP_D_PHICOMM,
    MCU_BMP_D_SAMSUNG,
    MCU_BMP_D_SMARTISAN,
    MCU_BMP_D_SONY,
    MCU_BMP_D_TCL,
    MCU_BMP_D_THINKPAD,
    MCU_BMP_D_TONGFANG,
    MCU_BMP_D_VIVO,
    MCU_BMP_D_WPHONE,
    MCU_BMP_D_MI,
    MCU_BMP_D_ZTE,
    MCU_BMP_W_LOCATIONFAILED,
    MCU_BMP_W_CLOUDY,
    MCU_BMP_W_DUST,
    MCU_BMP_W_FOGGY,
    MCU_BMP_W_HAZE,
    MCU_BMP_W_OVERCAST,
    MCU_BMP_W_RAIN,
    MCU_BMP_W_SNOW,
    MCU_BMP_W_SUN,
    MCU_BMP_W_WINDY,
    MCU_BMP_W_UNKNOWN,
    MCU_BMP_LOGO,
    MCU_BMP_SOFT_UPDATE,
    MCU_BMP_QR_CODE,
    MCU_BMP_HINT_DISCONNENT,
    MCU_BMP_UP_WHITE,
    MCU_BMP_DOWN_WHITE,
    MCU_BMP_WIFI,
    MCU_BMP_VISITOR,
    MCU_BMP_NUM_DOUBLE,
    MCU_BMP_NUM_SINGLE,
    MCU_BMP_USB_LINK,
    MCU_BMP_APMODE,
};

enum CONTEXT
{
    SOFTUPDATE_TITLE=0,
    SYSINFO_TITLE,
    APMODE_TITLE,
    WIFI_TITLE,
    WEATHER_TITLE,
    DEVLIST_TITLE,
    SOFTUPDATE_FIND_VER,
    SOFTUPDATE_CUR_VER,
    SOFTUPDATE_NEW_VER,
    SOFTUPDATE_TIP_MSG1,
    SOFTUPDATE_TIP_MSG2,
    SYSINFO_PD,
    SYSINFO_HW,
    SYSINFO_MAC,
    SYSINFO_VER,
    SYSINFO_SCAN,
    APMODE_CURIP,
    HOME_NET_DISC,
    HOME_DUAL_WIFI,
    HOME_WIFI_CLOSE,
    WIFI_WIFI_CLOSE,
    WIFI_SSID,
    WIFI_PWD,
    WIFI_GUEST_SSID,
    DEVLIST_DEVNAME,
    DEVLIST_UP,
    DEVLIST_DOWN,
    DEVLIST_DEV_NUM,
    DEVLIST_PAGE,
    ALERT_ROUTER_UPGRADE,
    ALERT_FACTORY_RESET,
    ALERT_REBOOT,
    ALERT_WAIT1_MSG,
    ALERT_WAIT2_MSG,
    WEATHER_NO_CITY_TIP_MSG1,
    WEATHER_NO_CITY_TIP_MSG2,
    WEATHER_TEMP_UNIT,
    SOFTUPDATE_CUR_VER_TEXT,
    SOFTUPDATE_NEW_VER_TEXT,
    SYSINFO_PD_TEXT,
    SYSINFO_HW_TEXT,
    SYSINFO_MAC_TEXT,
    SYSINFO_VER_TEXT,
    SYSINTERFACE_VALUE_TEXT,
    APMODE_CURIP_TEXT,
    HOME_UP_TEXT,
    HOME_DOWN_TEXT,
    HOME_UP_UNIT_TEXT,
    HOME_DOWN_UNIT_TEXT,
    HOME_DEV_NUM_TEXT,
    WIFI_SSID_TEXT,
    WIFI_PWD_TEXT,
    DEVLIST_DEVNAME_TEXT,
    DEVLIST_UP_TEXT,
    DEVLIST_DOWN_TEXT,
    DEVLIST_NUM_TEXT,
    DEVLIST_PAGE_TEXT,
    WEATHER_CITY_TEXT,
    WEATHER_STATE_TEXT,
    WEATHER_TEMP_TEXT,
    WEATHER_WEEK_TEXT,
    WEATHER_DATE_TEXT,
    WEATHER_TIME_TEXT,
};

enum LINE{
    WELCOME_PBAR_BK_LINE=0,
    WELCOME_PBAR_LINE,
    HOME_LINE,
    WIFI_LINE,
    DEVLIST_LINE,
};

enum ICON{
    WELCOME_ICON=0,
    SYSINFO_QRCODE_ICON,
    SYSINTERFACE_USB_LINK_ICON,
    HOME_NET_DISC_ICON,
    HOME_UP_ICON,
    HOME_DOWN_ICON,
    HOME_DEV_NUM_DOUBLE_ICON,
    HOME_DEV_NUM_SINGLE_ICON,
    WIFI_WIFI_CLOSE_ICON,
    WIFI_ICON,
    WIFI_GUEST_ICON,
    WEATHER_STATE_ICON,
    WEATHER_NO_CITY_ICON,
    DEVLIST_DEV_ICON,
    SOFTUPDATE_ICON,
    APMODE_ICON,
};

enum SCREEN_STATE{
    SCREEN_INIT=0,
    SCREEN_SHOW,
    SCREEN_END
};

enum WIFIMODE{
    wifi_close=0,
    wifi_2g=1,              //  0001
    wifi_5g=2,              //  0010
    wifi_2g_5g=3,           //  0011
    wifi_dual=4,            //  0100
    wifi_guest=8,           //  1000
    wifi_2g_guest=9,        //  1001
    wifi_5g_guest=10,       //  1010
    wifi_2g_5g_guest=11,    //  1011
    wifi_dual_guest=12,     //  1100
};

typedef struct {
  unsigned int index;
  unsigned char *pUtf8;
  unsigned int font_index;
  MCU_COLOR color;
  MCU_TEXTMODE textMode;
  MCU_WRAPMODE  wrapMode;
  MCU_TEXTALIGN textAlign;
  MCU_RECT  rect;   //y1 = y0 + font_height
} SCREEN_CONTEXT;

typedef struct{
    unsigned char penSize;
    MCU_COLOR   color;
    MCU_RECT    rect;
}SCREEN_LINE;

typedef struct{
    unsigned int bmp_index;
    int x0;
    int y0;
}SCREEN_ICON;

typedef struct{
    int pointx;
    int pointy;
    int radius;
    unsigned char  num;
    unsigned char  len;
    unsigned char i_start;
    int timer;
    unsigned char turn_flag;
}TURN_AROUND;

typedef struct{
    const int code;
    const char *text;
    unsigned int bmp_index;
}WEATHER;


static void DrawText(SCREEN_CONTEXT context,int xvalue,int yvalue);
static void DrawLine(SCREEN_LINE line,int yvalue);
static void DrawIcon(SCREEN_ICON icon,int xvalue,int yvalue);
static int CalTextWidth(unsigned char *text,int max_width);
static unsigned char *GetSpeedByU32(unsigned char connect_type,unsigned int data,int u_flag);
static int GetWifiMode(void);
static int BitCount2(unsigned int n);
static void DrawCir(void *turn_around);
static void DrawWelcome(void);
static void DrawSysinfo(void);
static void DrawSysInterface(void);
static void DrawHome(void);
static void DrawWifi(void);
static void DrawDevlist(void);
static void DrawInterface(int *x,int *y,int centerx,int centery,int enable,unsigned char *text);
static void DrawApMode(void);
static unsigned char * ChangeText(unsigned char *text,unsigned int text_width,unsigned int font_index);

extern void CountNum(unsigned char *count,int num);
extern void ChangeScreen(void);

#endif
