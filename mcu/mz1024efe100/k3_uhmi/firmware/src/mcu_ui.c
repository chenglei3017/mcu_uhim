#include "mcu_ui.h"
#include "app.h"

unsigned char _speed[32]={0};
unsigned char _tmpbuf[64]={0};
char _wisp_speed[16]="---";

unsigned char *_Week[10]={
    "周日",
    "周一",
    "周二",
    "周三",
    "周四",
    "周五",
    "周六",
};

WEATHER _Weather[]={
    {0,"晴",MCU_BMP_W_SUN},
    {1,"晴",MCU_BMP_W_SUN},
    {2,"晴",MCU_BMP_W_SUN},
    {3,"晴",MCU_BMP_W_SUN},
    {4,"多云",MCU_BMP_W_CLOUDY},
    {5,"晴间多云",MCU_BMP_W_CLOUDY},
    {6,"晴间多云",MCU_BMP_W_CLOUDY},
    {7,"大部多云",MCU_BMP_W_CLOUDY},
    {8,"大部多云",MCU_BMP_W_CLOUDY},
    {9,"阴",MCU_BMP_W_OVERCAST},
    {10,"阵雨",MCU_BMP_W_RAIN},
    {11,"雷阵雨",MCU_BMP_W_RAIN},
    {12,"雷阵雨伴有冰雹",MCU_BMP_W_RAIN},
    {13,"小雨",MCU_BMP_W_RAIN},
    {14,"中雨",MCU_BMP_W_RAIN},
    {15,"大雨",MCU_BMP_W_RAIN},
    {16,"暴雨",MCU_BMP_W_RAIN},
    {17,"大暴雨",MCU_BMP_W_RAIN},
    {18,"特大暴雨",MCU_BMP_W_RAIN},
    {19,"冻雨",MCU_BMP_W_RAIN},
    {20,"雨夹雪",MCU_BMP_W_SNOW},
    {21,"阵雪",MCU_BMP_W_SNOW},
    {22,"小雪",MCU_BMP_W_SNOW},
    {23,"中雪",MCU_BMP_W_SNOW},
    {24,"大雪",MCU_BMP_W_SNOW},
    {25,"暴雪",MCU_BMP_W_SNOW},
    {26,"浮尘",MCU_BMP_W_DUST},
    {27,"扬沙",MCU_BMP_W_DUST},
    {28,"沙尘暴",MCU_BMP_W_DUST},
    {29,"强沙尘暴",MCU_BMP_W_DUST},
    {30,"雾",MCU_BMP_W_FOGGY},
    {31,"霾",MCU_BMP_W_HAZE},
    {32,"风",MCU_BMP_W_WINDY},
    {33,"大风",MCU_BMP_W_WINDY},
    {34,"飓风",MCU_BMP_W_WINDY},
    {35,"热带风暴",MCU_BMP_W_WINDY},
    {36,"龙卷风",MCU_BMP_W_WINDY},
    {99,"未知",MCU_BMP_W_UNKNOWN},
};

SCREEN_CONTEXT _Contexts[]={
    { SOFTUPDATE_TITLE, "软件更新", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 80, 65, 240, 65+32}},
    { SYSINFO_TITLE, "AC3150 双频无线路由器", MCU_FONT_WQY22, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 30, 68, 290, 68+27}},
    { APMODE_TITLE, "AP模式", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 80, 65, 240, 65+32}},
    { WIFI_TITLE, "WiFi信息", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 100, 65, 220, 65+32}},
    { WEATHER_TITLE, "天气预报", MCU_FONT_WQY26 ,0x00FFFFFF ,MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 80, 65, 240, 65+32}},
    { DEVLIST_TITLE, "已接入终端", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 80, 65, 240, 65+32}},
    { SOFTUPDATE_FIND_VER, "发现新版本", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 80, 230, 240, 230+19}},
    { SOFTUPDATE_CUR_VER, "当前版本：", MCU_FONT_WQY14,0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 80, 257, 150, 257+18}},
    { SOFTUPDATE_NEW_VER, "最新版本：", MCU_FONT_WQY14,0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 80, 276, 150, 276+18}},
    { SOFTUPDATE_TIP_MSG1, "请前往WEB管理页面", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 70, 347, 250, 347+19} },
    { SOFTUPDATE_TIP_MSG2, "或使用斐讯路由APP升级", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 70, 368, 250, 368+19} }, 
    { SYSINFO_PD, "型号：", MCU_FONT_WQY16,0x00FFFFFF ,MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE ,MCU_TA_LEFT ,{ 37, 126, 83, 126+19}},
    { SYSINFO_HW, "H/W：", MCU_FONT_WQY16,0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 37, 152, 83, 152+19}},
    { SYSINFO_MAC, "MAC地址：", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT , { 37, 178, 123, 178+19} },
    { SYSINFO_VER, "软件版本：", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT , { 37, 203, 120, 203+19} },
    { SYSINFO_SCAN, "扫一扫安装APP 轻松管理路由器", MCU_FONT_WQY14, 0x00D9D9D9, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 50, 397, 270, 397+18}},
    { APMODE_CURIP, "当前IP地址：", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, {63, 369, 155, 369+19}},
    { HOME_NET_DISC, "不能连接网络", MCU_FONT_WQY20,0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{ 90, 163, 230, 163+24}},
    { HOME_DUAL_WIFI, "已启用双频合一", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{ 50, 348, 270, 348+32}},
    { HOME_WIFI_CLOSE, "无线网络已关闭", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{ 50, 347, 270, 347+32}},
    { WIFI_WIFI_CLOSE, "无线网络已关闭", MCU_FONT_WQY20, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{ 50, 278, 270, 278+24}},
    { WIFI_SSID, "无线", MCU_FONT_WQY18,0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 68, 131, 105, 131+22}},
    { WIFI_PWD, "密码", MCU_FONT_WQY18,0x00FFFFFF,MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 40, 181, 85, 181+22}},
    { WIFI_GUEST_SSID, "访客", MCU_FONT_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 68, 331, 105, 331+22}},
    { DEVLIST_DEVNAME, "名称", MCU_FONT_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 47, 134, 87, 134+22}},
    { DEVLIST_UP, "上行", MCU_FONT_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 167, 134, 207, 134+22}},
    { DEVLIST_DOWN, "下行", MCU_FONT_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 248, 134, 288, 134+22}},
    { DEVLIST_DEV_NUM, "接入终端数：", MCU_FONT_WQY14,0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 18, 417, 100, 417+18}},
    { DEVLIST_PAGE, "页", MCU_FONT_WQY14, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT, { 285, 417, 302, 417+18}},
    { ALERT_ROUTER_UPGRADE, "系统升级中", MCU_FONT_WQY24, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER , { 21, 275, 299, 275+29} },
    { ALERT_FACTORY_RESET, "恢复出厂设置中", MCU_FONT_WQY24, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER , {21, 275, 299, 275+29} },
    { ALERT_REBOOT, "路由器重启中", MCU_FONT_WQY24, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 21, 275, 299, 275+29}},
    { ALERT_WAIT1_MSG,"请耐心等待1~2分钟", MCU_FONT_WQY14, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 90, 311, 230, 311+18}},
    { ALERT_WAIT2_MSG,"请耐心等待2~3分钟", MCU_FONT_WQY14, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 90, 311, 230, 311+18}},
    { WEATHER_NO_CITY_TIP_MSG1, "获取城市失败", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 100, 272, 220, 272+19}},
    { WEATHER_NO_CITY_TIP_MSG2, "请前往WEB管理页面手动选择", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{50,293,270,293+19}}, 
    { WEATHER_TEMP_UNIT, "°C", MCU_FONT_PF48,0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE,MCU_TA_LEFT ,{ 190, 300, 240, 300+68}},            // (温度符号)
    { SOFTUPDATE_CUR_VER_TEXT, "", MCU_FONT_WQY14, 0x00A6A6A6, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT,{ 150, 257, 240, 257+18} },     // 21.5.37.244
    { SOFTUPDATE_NEW_VER_TEXT, "", MCU_FONT_WQY14, 0x00A6A6A6, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT,{ 150, 276, 240, 276+18} },     // 21.5.37.246
    { SYSINFO_PD_TEXT,"", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 84, 127, 180, 127+19} },               // K3
    { SYSINFO_HW_TEXT,"", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 84, 153, 180, 153+19} },               // A1
    { SYSINFO_MAC_TEXT,"", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 123, 179, 280, 179+19} },             // 11:22:33:44:55:66
    { SYSINFO_VER_TEXT,"", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 120, 204, 260, 204+19} },            // 21.4.1.1
    { SYSINTERFACE_VALUE_TEXT, "", MCU_FONT_WQY26, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER,{ 87, 180, 114, 180+32} },    // W
    { APMODE_CURIP_TEXT, "", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT,  {160, 369, 285, 369+19}},                // ap模式下的，当前ip地址
    { HOME_UP_TEXT, "", MCU_FONT_PF48, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT ,{ 99, 101, 198, 101+68}},                 // 100
    { HOME_DOWN_TEXT, "", MCU_FONT_PF48, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT ,{ 99, 155, 198, 155+68}},                // 12.3
    { HOME_UP_UNIT_TEXT, "", MCU_FONT_WQY20, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 200,132, 242, 132+24} },            // KB/s
    { HOME_DOWN_UNIT_TEXT, "", MCU_FONT_WQY20, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 200,185,242,185+24} },          // MB/s
    { HOME_DEV_NUM_TEXT, "", MCU_FONT_WQY16, 0x00000000, MCU_TEXTMODE_TRANS, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 184, 341, 200, 341+19} },          // 5
    { WIFI_SSID_TEXT, "", MCU_FONT_WQY16, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 140,133,293, 133+19}},              // @PHICOMM_AA
    { WIFI_PWD_TEXT, "", MCU_FONT_WQY16, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_CHAR, MCU_TA_LEFT, { 140, 174, 293, 174+(19*2)} },  // ******      | MCU_TA_VCENTER
    { DEVLIST_DEVNAME_TEXT, "", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 69, 182, 144, 182+19} },      // ipho...
    { DEVLIST_UP_TEXT, "", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 150, 182, 220, 182+19} },        // 100B/s
    { DEVLIST_DOWN_TEXT, "", MCU_FONT_WQY16, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT, { 225, 182, 295, 182+19} },        // 88.8kb/s
    { DEVLIST_NUM_TEXT, "", MCU_FONT_WQY14, 0x00808080, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT, { 105, 418, 130, 418+18} },        // 5
    { DEVLIST_PAGE_TEXT, "", MCU_FONT_WQY14, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT, { 240, 417, 283, 417+18}},       //  1/2
    { WEATHER_CITY_TEXT, "", MCU_CITY_FONT_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER, { 80, 115, 240, 115+22} },      // 上海
    { WEATHER_STATE_TEXT, "", MCU_WEATHER_WQY18, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 80, 264, 240, 264+22} },      // 晴
    { WEATHER_TEMP_TEXT, "", MCU_FONT_PF48, 0x00FFFFFF, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_CENTER ,{ 90, 300, 230, 300+68} },  // 23
    { WEATHER_WEEK_TEXT, "", MCU_FONT_WQY16, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 35, 378, 80, 378+19} },         // 周一
    { WEATHER_DATE_TEXT, "", MCU_FONT_WQY16, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_LEFT ,{ 35, 400, 140, 400+19} },        // 2017-07-11
    { WEATHER_TIME_TEXT, "", MCU_FONT_PF42, 0x00B3B3B3, MCU_TEXTMODE_NORMAL, MCU_WRAPMODE_NONE, MCU_TA_RIGHT ,{ 170, 367, 290, 367+60} },        // 08:00
 
};
SCREEN_LINE _Lines[]={
    { 2, 0x00333333, { 70, 425, 250, 425} },
    { 2, 0x00FFFFFF, { 70, 425, 70, 425} },
    { 2, 0x00FFFFFF, { 160, 356, 160, 374} },
    { 1, 0x00333333, { 28, 166, 292, 166} },
    { 1, 0x00333333, { 18, 166, 302, 166} }
};

SCREEN_ICON _Icons[]={
    { MCU_BMP_LOGO, 67, 182},
    { MCU_BMP_QR_CODE, 110, 261},
    { MCU_BMP_USB_LINK, 148, 90},
    { MCU_BMP_HINT_DISCONNENT, 145, 127},
    { MCU_BMP_UP_WHITE, 80, 130},
    { MCU_BMP_DOWN_WHITE, 80, 182},
    { MCU_BMP_NUM_DOUBLE, 177, 340},
    { MCU_BMP_NUM_SINGLE, 181, 340},
    { MCU_BMP_HINT_DISCONNENT, 145, 232},
    { MCU_BMP_WIFI, 40, 133},
    { MCU_BMP_VISITOR, 40, 333},
    { MCU_BMP_W_SUN, 106, 147},
    { MCU_BMP_W_LOCATIONFAILED, 120, 170},
    { MCU_BMP_D_DEFAULT, 13, 169},
    { MCU_BMP_SOFT_UPDATE, 124, 146},
    { MCU_BMP_APMODE, 120, 190},
};

static void DrawText(SCREEN_CONTEXT context,int xvalue,int yvalue)
{
    int tmp_x = 0;
    MCU_SetColor(context.color);
    MCU_SetFont(context.font_index);
    MCU_SetTextMode(context.textMode);
    MCU_SetTextAlign(context.textAlign);

    context.rect.x0 += xvalue;
    context.rect.y0 += yvalue;
    if (context.rect.x0 < 0)
    {
        tmp_x = context.rect.x0 - 10;
        context.rect.x0 = 10;
    }

    if (0 == context.rect.x1 && 0 == context.rect.y1)
    {
        MCU_DrawTextAtPos( context.pUtf8, context.rect.x0, context.rect.y0);
    }
    else
    {
        context.rect.x1 += xvalue;
        context.rect.y1 += yvalue;

        // change rect.x1 rect.x0
        if( tmp_x != 0)
        {
            context.rect.x1 += tmp_x;
        }
        else
        {
            if (context.rect.x1 > 320)
            {
                tmp_x = context.rect.x1 - 320 + 10;
                context.rect.x1 = 310;
                context.rect.x0 += tmp_x;
            }
        }

        if(context.textMode != MCU_TEXTMODE_TRANS)
        {
            MCU_SetColor(0x00000000);
            MCU_FillRect(context.rect);
        }
        MCU_SetColor(context.color);
        MCU_DrawTextInRect(context.pUtf8,context.rect,context.textAlign,context.textMode,context.wrapMode);
    }

}
static void DrawLine(SCREEN_LINE line,int yvalue)
{
    MCU_LINE l;
    l.x0 = line.rect.x0;
    l.y0 = line.rect.y0 + yvalue;
    l.x1 = line.rect.x1;
    l.y1 = line.rect.y1 + yvalue;

    MCU_SetPenSize(line.penSize);
    MCU_SetColor(line.color);
    MCU_DrawLine(l);
}

static void DrawIcon(SCREEN_ICON icon,int xvalue,int yvalue)
{
    MCU_DrawBitmapAtPos(icon.bmp_index,icon.x0+xvalue,icon.y0+yvalue);
}

static int CalTextWidth(unsigned char *text,int max_width)
{
    unsigned int i=0;
    int text_width=0;

    for ( i=0; i < strlen((const char *)text); ++i )
    {
        text_width += MCU_GetCharWidthByFontSize( MCU_FONT_WQY14, text[i] ); 

        if ( text_width > max_width)
            return i;   //font 14 dual
    }

    text_width = 0;

    for ( i=0; i < strlen((const char *)text); ++i )
    {
        text_width +=  MCU_GetCharWidthByFontSize( MCU_FONT_WQY16, text[i]);;

        if ( text_width > max_width )
            return -1;  //font 14
    }
    return -2;  //font 16
}

static int GetWifiMode(void)
{
    int wifiMode = 0;
    unsigned char  dual_en = appData.wificfg.wifi2in1;

    unsigned char wifi_2g_en = appData.wificfg.ssid_info[SSID_2G].ssid_en;
    unsigned char wifi_5g_en = appData.wificfg.ssid_info[SSID_5G].ssid_en;

    int guest_en = appData.wificfg.ssid_info[SSID_GUEST].ssid_en;

    if (1 == dual_en)
    {
       if (1 == wifi_2g_en && 1 == wifi_5g_en)
            wifiMode += (1<<2);	// +0100
    }
    else
    {
        if (1 == wifi_2g_en)
        {
            wifiMode += 1; // +0001
        }
        if (1 == wifi_5g_en)
        {
            wifiMode += (1<<1); // +0010
        }
    }
    if (1 == guest_en)
    {
        wifiMode += (1<<3); //  +1000
    }
    return wifiMode;
}
/*
 *  Count the number of 1 in the N binary
 */
static int BitCount2(unsigned int n)
{
    unsigned int c =0 ;
    for ( c=0; n; ++c)
    {
        n &= (n -1) ; // 清除最低位的1
    }
    return c ;
}

static void DrawCir(void *turn_around)
{
    TURN_AROUND *tr = (TURN_AROUND *)turn_around;
    int i=0,j=0;
    int colora = 0x14,colorb = 0xFF-colora;
    MCU_COLOR color = 0x00000000;
    double du = 360.0/tr->num;
    int a = tr->radius-tr->len;
    int b = tr->radius;

    MCU_LINE line={0,0,0,0};
    MCU_AA_SetFactor(6);

    if ( tr->i_start >= tr->num)
    {
        tr->i_start =0;
    }

    for ( i=tr->i_start; i<tr->num; ++i,++j)
    {
        line.x0 = (int)(tr->pointx+(a)*Sin((i)*du));
        line.y0 = (int)(tr->pointy-(a)*Cos((i)*du));
        line.x1 = (int)(tr->pointx+(b)*Sin((i)*du));
        line.y1 = (int)(tr->pointy-(b)*Cos((i)*du));
#ifdef  __MCU_ST_H
        MCU_SetPenSize(6);
        MCU_SetColor(0x00000000);
        MCU_DrawLine(line);
#endif
        if (tr->turn_flag)
        {
            color = colora+j*(colorb/tr->num);
            color = (color<<16)+(color<<8)+color;
        }
        else
        {
            color = 0x002e2e2e;
        }
        MCU_SetColor(color);
        MCU_SetPenSize(3);

        MCU_AA_DrawLine(line);
        if ( i == tr->num-1)
        {
            i = -1;
        }
        if (i == tr->i_start-1)
            break;
    }
}

static unsigned char *GetSpeedByU32(unsigned char connect_type,unsigned int data,int u_flag)
{
    float speed_data = data;
    int flag = 0;
    unsigned char buff[16] = {0};
    unsigned char speed_unit[5] = {0};
    memset( _speed, 0x0, sizeof(_speed));
    if ( 0 == data && CONNECT_TYPE_LAN == connect_type) // lan connect  0 == data && connect_type == 0
    {
        strcpy((char *)_speed,"----");
        return _speed;
    }
    else
    {
        while ( speed_data > 1000)
        {
            speed_data = speed_data / 1000.0;
            ++flag;
        }
        if ( flag != 0)
        {
            if ( 0 == u_flag || 1 == u_flag || 2 == u_flag)
            {
                if ( speed_data < 10)
                {
                    sprintf((char *)buff, "%.2f", speed_data);
                }
                else if ( speed_data < 100)
                {
                    sprintf((char *)buff,"%.1f", speed_data);
                }
                else
                {
                    sprintf((char *)buff, "%3.0f", speed_data);
                }
            }
        }
        else
        {
            sprintf((char *)buff, "%.0f", speed_data);    
        }
        switch(flag)
        {
            case 0:
                strncpy((char *)speed_unit, "B/s", 3);
                break;
            case 1:
                strncpy((char *)speed_unit, "KB/s", 4);
                break;
            case 2:
                strncpy((char *)speed_unit, "MB/s", 4);
                break;
            default:
                return "";

        }
    }

    if(0 == u_flag)
    {
        strncpy((char *)_speed,(const char *)buff,sizeof(_speed));
    }
    else if ( 1 == u_flag)
    {
        strncpy((char *)_speed,(const char *)speed_unit, sizeof(_speed));
    }
    else
    {
        strncpy((char *)_speed,(const char *)buff, sizeof(_speed));
        strcat((char *)_speed,(const char *)speed_unit);
    }
    return _speed;
}

void CountNum(unsigned char *count,int num)
{
    if ( *count-num >=0)
    {
        *count = -1;
    }
    ++(*count);
}
static void DrawBootBar(int cur_i,int num)
{
    int i=0;
    int plx=0;
    int ply = 443;
    int size = 6;
    int spacing = size+10;
    MCU_COLOR cur_color =  0x00FFFFFF;
    MCU_RECT rect = {0,442,320,455};

    static int tmp_num = 0;
    
    if ( SCREEN_INIT != appData.screen_state && false == appData.changFlag)
        return;

    if(tmp_num!=num)
    {
        MCU_SetColor(0x00000000);
        MCU_FillRect(rect);
        tmp_num = num;
    }

    if ( num%2 != 0)
    {
        plx = 160 -size/2 - (num/2)*(spacing);
    }
    else
    {
        plx = 160 - (spacing-size)/2 - size - (num/2-1)*(spacing);
    }
    for ( i=0; i<num; ++i)
    {
        if (i == cur_i-1)
        {
            cur_color = 0x00FFFFFF;
        }
        else
        {
            cur_color = 0x009A9A9A;
        }
        MCU_SetColor(cur_color);
        rect.x0 = plx + i*spacing;
        rect.y0 = ply;
        rect.x1 = plx + size + i*spacing;
        rect.y1 = ply + size;

        MCU_FillRect(rect);
    }
}
static void DrawWelcome(void)
{
    static int pbar_width = 0;
    CountNum( &appData.count, 15);
    if ( appData.count != 0)
      return;

    if ( SCREEN_INIT == appData.screen_state )
    {
        DrawIcon(_Icons[WELCOME_ICON],0,0);
        DrawLine(_Lines[WELCOME_PBAR_BK_LINE],0);
        _Lines[WELCOME_PBAR_LINE].rect.x1 = _Lines[WELCOME_PBAR_LINE].rect.x0;
        pbar_width = _Lines[WELCOME_PBAR_BK_LINE].rect.x0;
    }

    if ( pbar_width < _Lines[WELCOME_PBAR_BK_LINE].rect.x1 )
    {
        ++pbar_width;
    }
    _Lines[WELCOME_PBAR_LINE].rect.x1 = pbar_width;
    DrawLine( _Lines[WELCOME_PBAR_LINE], 0);
}

static void DrawSysinfo(void)
{
    if ( SCREEN_INIT == appData.screen_state)
    {
        DrawText( _Contexts[SYSINFO_TITLE], 0, 0);
        DrawText( _Contexts[SYSINFO_PD], 0, 0);
        DrawText( _Contexts[SYSINFO_HW], 0, 0);
        DrawText( _Contexts[SYSINFO_MAC], 0, 0);
        DrawText( _Contexts[SYSINFO_VER], 0, 0);
        DrawText( _Contexts[SYSINFO_SCAN], 0, 0);
        DrawIcon( _Icons[SYSINFO_QRCODE_ICON], 0, 0);
    }
    else
    {
        if ( false == appData.changFlag )
            return ;
    }
    _Contexts[SYSINFO_PD_TEXT].pUtf8 = appData.systeminfo.mn;
    _Contexts[SYSINFO_HW_TEXT].pUtf8 = appData.systeminfo.hw;
    _Contexts[SYSINFO_MAC_TEXT].pUtf8 = appData.systeminfo.mac;
    _Contexts[SYSINFO_VER_TEXT].pUtf8 = appData.systeminfo.sw;
    DrawText( _Contexts[SYSINFO_PD_TEXT], 0, 0);
    DrawText( _Contexts[SYSINFO_HW_TEXT], 0, 0);
    DrawText( _Contexts[SYSINFO_MAC_TEXT], 0, 0);
    DrawText( _Contexts[SYSINFO_VER_TEXT], 0, 0);
}

static void DrawSysInterface(void)
{
    unsigned char usb_en = appData.ifstatus.usb_plugged;
    unsigned char lan1_en = appData.ifstatus.lan1_linked;
    unsigned char lan2_en = appData.ifstatus.lan2_linked;
    unsigned char lan3_en = appData.ifstatus.lan3_linked;
    unsigned char wan_en = appData.ifstatus.wan_linked;
    int x[] = {77,86,140,121,133,87};
    int y[] = {156,173,219,236,166,197};
    MCU_RECT usb_rect = {160-24,67,160+24,79};
    MCU_RECT usb_rect1 = {160-32,79,160+32,122};
    MCU_RECT fill_rect = {147,89,173,115};

    if ( SCREEN_INIT == appData.screen_state || true == appData.changFlag)
    {
        MCU_SetPenSize(1);

        if ( 1 == usb_en )
        {
           DrawIcon(_Icons[SYSINTERFACE_USB_LINK_ICON],0,0);
           MCU_SetColor(0x00FFFFFF);
        }
        else
        {
           MCU_SetColor(0x00000000);
           MCU_FillRect(fill_rect); 
           MCU_SetColor(0x00808080);
        }

        MCU_DrawRect(usb_rect);
        MCU_DrawRect(usb_rect1);

        DrawInterface( x, y, 0, 0, lan1_en, "1");
        DrawInterface( x, y, 160, 0, lan2_en, "2");
        DrawInterface( x, y, 0,110, lan3_en, "3");
        DrawInterface( x, y, 160, 0, wan_en, "W");
    }
}

static void DrawHome(void)
{
    unsigned char netState = appData.homeinfo.connected;
    int wifiMode  = GetWifiMode();
    unsigned char wifi_2g_num = appData.wificfg.ssid_info[SSID_2G].client_nu;
    unsigned char wifi_5g_num = appData.wificfg.ssid_info[SSID_5G].client_nu;
    unsigned char temp_num[3] = {0};
    MCU_RECT fill_rect = {79, 106, 242, 214};
    MCU_RECT fill_rect2 = {40, 335, 280, 385};
    unsigned int usrate = appData.homeinfo.usrate;
    unsigned int dsrate = appData.homeinfo.dsrate;
    SCREEN_CONTEXT context = _Contexts[HOME_DUAL_WIFI];
#ifdef __MCU_ST_H
    GUI_RECT rect = {40,40,280,280};
#endif
    if (SCREEN_INIT == appData.screen_state || true == appData.changFlag || true == appData.homeInfoChangFlag)
    {
        MCU_SetColor(0x00000000);
        MCU_FillRect(fill_rect);
        switch (netState)
        {
            case 0:
                appData.home_tr.turn_flag = 0;
                DrawIcon( _Icons[HOME_NET_DISC_ICON], 0, 0);
                DrawText( _Contexts[HOME_NET_DISC], 0, 0);
                break;
            case 1:
                appData.home_tr.turn_flag = 1;
                if(appData.homeinfo.flag)//wisp
                {
                  _Contexts[HOME_UP_TEXT].pUtf8 = _wisp_speed;
                  DrawText( _Contexts[HOME_UP_TEXT], 5, 0);
                  _Contexts[HOME_DOWN_TEXT].pUtf8 = _wisp_speed;
                  DrawText( _Contexts[HOME_DOWN_TEXT], 5, 0);  
                  break;
                }
                DrawIcon( _Icons[HOME_UP_ICON], 0, 0);
                DrawIcon( _Icons[HOME_DOWN_ICON], 0, 0);
                _Contexts[HOME_UP_TEXT].pUtf8 = GetSpeedByU32( 1, usrate, 0);
                DrawText( _Contexts[HOME_UP_TEXT], 0, 0);
                _Contexts[HOME_DOWN_TEXT].pUtf8 = GetSpeedByU32( 1, dsrate, 0);
                DrawText( _Contexts[HOME_DOWN_TEXT], 0, 0);
                _Contexts[HOME_UP_UNIT_TEXT].pUtf8 = GetSpeedByU32( 1, usrate, 1);
                DrawText( _Contexts[HOME_UP_UNIT_TEXT], 0, 0);
                _Contexts[HOME_DOWN_UNIT_TEXT].pUtf8 = GetSpeedByU32( 1, dsrate, 1);
                DrawText( _Contexts[HOME_DOWN_UNIT_TEXT], 0, 0);
                break;
        }

    }
    if ( SCREEN_INIT == appData.screen_state || true == appData.changFlag )
    {
        MCU_SetColor(0x00000000);
        MCU_FillRect(fill_rect2);
        switch (wifiMode)
        {
            case 0://wifi close
            case 8:
                DrawText( _Contexts[HOME_WIFI_CLOSE], 0, 0);
                break;
            case 1: // 2.4g single  0001  1001
            case 9: 
                context.pUtf8 = "2.4G";
                DrawText( context, 0, 0);
                if ( wifi_2g_num > 10 )
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_DOUBLE_ICON], 0, 0);
                }
                else
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_SINGLE_ICON], 0, 0);
                }
                sprintf( (char *)temp_num, "%d", wifi_2g_num);
                _Contexts[HOME_DEV_NUM_TEXT].pUtf8 = temp_num;
                DrawText( _Contexts[HOME_DEV_NUM_TEXT], 0, 0);
                break;
            case 2: // 5g single   0010  1010
            case 10:
                context.pUtf8 = "5G";
                DrawText( context, 0, 0);
                if ( wifi_5g_num > 10)
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_DOUBLE_ICON], -10, 0);
                }
                else
                {
                     DrawIcon(_Icons[HOME_DEV_NUM_SINGLE_ICON],-10,0);
                }
                sprintf((char *)temp_num,"%d",wifi_5g_num);
                _Contexts[HOME_DEV_NUM_TEXT].pUtf8 = temp_num;
                DrawText( _Contexts[HOME_DEV_NUM_TEXT], -10, 0);
                break;
            case 3: // 2.4g and 5g   0011  1011
            case 11:
                context.pUtf8="2.4G";
                DrawText(context,-80,0);
                context.pUtf8="5G";
                DrawText(context,80,0);
                if ( wifi_2g_num > 10 )
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_DOUBLE_ICON], -80, 0);
                }
                else
                {
                     DrawIcon( _Icons[HOME_DEV_NUM_SINGLE_ICON], -80, 0);
                }
                if (wifi_5g_num>10)
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_DOUBLE_ICON], -10+80, 0);
                }
                else
                {
                   DrawIcon( _Icons[HOME_DEV_NUM_SINGLE_ICON], -10+80, 0);
                }
                sprintf((char *)temp_num,"%d",wifi_2g_num);
                _Contexts[HOME_DEV_NUM_TEXT].pUtf8 = temp_num;
                DrawText( _Contexts[HOME_DEV_NUM_TEXT], -80, 0);
                sprintf((char *)temp_num,"%d",wifi_5g_num);
                _Contexts[HOME_DEV_NUM_TEXT].pUtf8 = temp_num;
                DrawText( _Contexts[HOME_DEV_NUM_TEXT], -10+80, 0);
                DrawLine( _Lines[HOME_LINE], 0);
                break;
            case 4://dual wifi        0100 1100
            case 12:
                DrawText(_Contexts[HOME_DUAL_WIFI],0,0);
                if (wifi_2g_num+wifi_5g_num > 10)
                {
                   DrawIcon( _Icons[HOME_DEV_NUM_DOUBLE_ICON], 55, 0);
                }
                else
                {
                    DrawIcon( _Icons[HOME_DEV_NUM_SINGLE_ICON], 55, 0);
                }
                sprintf((char *)temp_num, "%d", wifi_2g_num + wifi_5g_num);
                _Contexts[HOME_DEV_NUM_TEXT].pUtf8 = temp_num;
                DrawText( _Contexts[HOME_DEV_NUM_TEXT], 55, 0);
                break;
        }
    }
    CountNum( &appData.count, 5);
    if (!appData.count)
    {
        ++(appData.home_tr.i_start);
#ifdef __MCU_ST_H
        GUI_MEMDEV_Draw( &rect, &DrawCir, &appData.home_tr, 0, 0);
#else
        DrawCir( &(appData.home_tr));
#endif
    }
}

static unsigned char * ChangeText(unsigned char *text,unsigned int text_width,unsigned int font_index)
{
    unsigned int i = 0;
    unsigned int width = 0;
    unsigned int point_width = 0;
    memset( _tmpbuf, 0, sizeof(_tmpbuf));
    strcpy( (char *)_tmpbuf, (char *)text);
    for ( i=0; i<strlen((char *)_tmpbuf); ++i)
    {
        width += MCU_GetCharWidthByFontSize(font_index,_tmpbuf[i]);
        point_width = MCU_GetCharWidthByFontSize(font_index,'.');

        if ( (width + 3*point_width) > text_width){  // + 3个'.'的宽度
            i = i-1;
            break;
        }
    }
    if (i >= strlen((char *)_tmpbuf))
        return _tmpbuf;

    _tmpbuf[i]='.';
    _tmpbuf[i+1]='.';
    _tmpbuf[i+2]='.';
    _tmpbuf[i+3]='\0';

    return _tmpbuf;
}

static void DrawWifi(void){
    int i=0;
    int text_flag=0;
    unsigned int index = SSID_2G;

    SCREEN_CONTEXT content;
    int wifiMode = GetWifiMode();
    int num = BitCount2(wifiMode);
    MCU_RECT rect = {5, 120, 300, 420};

    unsigned int ssid_context_rect_width = _Contexts[WIFI_SSID_TEXT].rect.x1 - _Contexts[WIFI_SSID_TEXT].rect.x0;
    unsigned int pwd_context_rect_width =  _Contexts[WIFI_PWD_TEXT].rect.x1 - _Contexts[WIFI_PWD_TEXT].rect.x0;

   // int guest_en = ((wifiMode&wifi_guest)==(1<<3)?1:0); // &1000
    unsigned char guest_en = appData.wificfg.ssid_info[SSID_GUEST].ssid_en;

    if (SCREEN_INIT == appData.screen_state)
    {
        DrawText( _Contexts[WIFI_TITLE], 0, 0);
    }
    else
    {
        if (appData.changFlag == false)
            return ;
    }

    MCU_SetColor(0x00000000);
    MCU_FillRect(rect);
    switch (num)
    {
        case 3:
            DrawIcon( _Icons[WIFI_GUEST_ICON], 0, 0);
            DrawText( _Contexts[WIFI_GUEST_SSID], 0, 0);
            DrawText( _Contexts[WIFI_PWD], 0, 200);

            _Contexts[WIFI_SSID_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[SSID_GUEST].ssid_name, ssid_context_rect_width, MCU_FONT_WQY16);
            DrawText( _Contexts[WIFI_SSID_TEXT], 0, 200);

            _Contexts[WIFI_PWD_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[SSID_GUEST].ssid_pwd, pwd_context_rect_width*2, MCU_FONT_WQY14);
            text_flag = CalTextWidth( _Contexts[WIFI_PWD_TEXT].pUtf8, pwd_context_rect_width);

            if (text_flag != -2)
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY14;
            }
            else
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY16;
            }
            DrawText( _Contexts[WIFI_PWD_TEXT], 0, 200);

        case 2:
            content = _Contexts[WIFI_SSID];
            if ( (1 == guest_en) && (2 == num) )
            {
                content.pUtf8 = _Contexts[WIFI_GUEST_SSID].pUtf8;
                DrawIcon( _Icons[WIFI_GUEST_ICON], 0, -100);
                index = SSID_GUEST;
            }
            else
            {
                content.pUtf8 = "5G";
                DrawIcon( _Icons[WIFI_ICON], 0, 100);
                index = SSID_5G;
            }
            DrawText(content,0,100);
            DrawText( _Contexts[WIFI_PWD], 0, 100);

            _Contexts[WIFI_SSID_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[index].ssid_name, ssid_context_rect_width, MCU_FONT_WQY16);
            DrawText( _Contexts[WIFI_SSID_TEXT], 0, 100);

            _Contexts[WIFI_PWD_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[index].ssid_pwd, pwd_context_rect_width*2, MCU_FONT_WQY14);
            text_flag = CalTextWidth( _Contexts[WIFI_PWD_TEXT].pUtf8, pwd_context_rect_width);
            if (text_flag!=-2)
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY14;
            }
            else
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY16;
            }
            DrawText(_Contexts[WIFI_PWD_TEXT],0,100);
        case 1:
            content = _Contexts[WIFI_SSID];
            if (guest_en==1 && num==1)
            {
                content.pUtf8=_Contexts[WIFI_GUEST_SSID].pUtf8;
                DrawIcon(_Icons[WIFI_GUEST_ICON],0,-200);
                index = SSID_GUEST;
            }
            else
            {
                switch (wifiMode)
                {
                    case 1:     //0001
                    case 3:     //0011
                    case 9:     //1001
                    case 11:    //1011
                        content.pUtf8 = "2.4G";
                        index = SSID_2G;
                        break;
                    case 2:     //0010
                    case 10:    //1010
                        content.pUtf8 = "5G";
                        index = SSID_5G;
                        break;
                    case 4:     //0100
                    case 12:    //1100
                        content.pUtf8 = _Contexts[WIFI_SSID].pUtf8;
                        index = SSID_2G;
                        break;
                    case 8:     //1000
                        content.pUtf8 = _Contexts[WIFI_GUEST_SSID].pUtf8;
                        index = SSID_GUEST;
                        break;
                }
                DrawIcon(_Icons[WIFI_ICON],0,0);
            }
            DrawText(content,0,0);
            DrawText(_Contexts[WIFI_PWD],0,0);

            _Contexts[WIFI_SSID_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[index].ssid_name, ssid_context_rect_width, MCU_FONT_WQY16);
            DrawText(_Contexts[WIFI_SSID_TEXT],0,0);
            _Contexts[WIFI_PWD_TEXT].pUtf8 = ChangeText( appData.wificfg.ssid_info[index].ssid_pwd, pwd_context_rect_width*2, MCU_FONT_WQY14);
            text_flag = CalTextWidth(_Contexts[WIFI_PWD_TEXT].pUtf8, pwd_context_rect_width);
            if (text_flag!=-2)
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY14;
            }
            else
            {
                _Contexts[WIFI_PWD_TEXT].font_index = MCU_FONT_WQY16;
            }
            DrawText(_Contexts[WIFI_PWD_TEXT],0,0);
            break;

        case 0:
            DrawIcon(_Icons[WIFI_WIFI_CLOSE_ICON],0,0);
            DrawText(_Contexts[WIFI_WIFI_CLOSE],0,0);
            break;
    }
    for(i=0;i<num*2;i++)
    {
        DrawLine(_Lines[WIFI_LINE],i*50);
    }
}

static void DrawDevlist(void)
{
    unsigned char  dev_num = appData.hostlist.total/5>appData.hostlist.index?5:appData.hostlist.total%5;
    unsigned int devname_context_rect_width = _Contexts[DEVLIST_DEVNAME_TEXT].rect.x1 - _Contexts[DEVLIST_DEVNAME_TEXT].rect.x0;
    int i=0;
    unsigned int devicon_index = 0;
    MCU_RECT rect = {10, 170, 310, 418};
    if (SCREEN_INIT == appData.screen_state)
    {
        DrawText( _Contexts[DEVLIST_TITLE], 0, 0);
        DrawText( _Contexts[DEVLIST_DEVNAME], 0, 0);
        DrawText( _Contexts[DEVLIST_UP], 0, 0);
        DrawText( _Contexts[DEVLIST_DOWN], 0, 0);
        DrawText( _Contexts[DEVLIST_DEV_NUM], 0, 0);
        DrawText( _Contexts[DEVLIST_PAGE], 0, 0);
    }
    else
    {
        if (false == appData.changFlag)
            return ;
    }
    DrawLine( _Lines[DEVLIST_LINE], 0);
    for (i=0; i < 5 ;i++)
    {
        if (i<dev_num)
        {
            devicon_index = appData.hostlist.hostdev[i].logo;
            if(devicon_index > MCU_BMP_W_LOCATIONFAILED) // MCU_BMP_D_ZTE = MCU_BMP_W_LOCATIONFAILED-1
            {
                devicon_index = MCU_BMP_D_DEFAULT;
            }
            _Icons[DEVLIST_DEV_ICON].bmp_index = devicon_index;
            DrawIcon( _Icons[DEVLIST_DEV_ICON], 0, i*48);

            _Contexts[DEVLIST_DEVNAME_TEXT].pUtf8 = ChangeText( appData.hostlist.hostdev[i].hostname, devname_context_rect_width, MCU_FONT_WQY16);
            DrawText( _Contexts[DEVLIST_DEVNAME_TEXT], 0, i*48);

            _Contexts[DEVLIST_UP_TEXT].pUtf8 = GetSpeedByU32( appData.hostlist.hostdev[i].ct_type, appData.hostlist.hostdev[i].usrate , 2);
            DrawText( _Contexts[DEVLIST_UP_TEXT], 0, i*48);
            _Contexts[DEVLIST_DOWN_TEXT].pUtf8 = GetSpeedByU32( appData.hostlist.hostdev[i].ct_type, appData.hostlist.hostdev[i].dsrate , 2);
            DrawText( _Contexts[DEVLIST_DOWN_TEXT], 0, i*48);
            DrawLine( _Lines[DEVLIST_LINE], (i+1)*48);
        }
        else
        {
            MCU_SetColor(0x00000000);
            rect.y0 = 170 + (i*48);
            MCU_FillRect(rect);
        }
    }
    memset(_tmpbuf,0x0,sizeof(_tmpbuf));
    sprintf( (char *)_tmpbuf, "%d", appData.hostlist.total);
    _Contexts[DEVLIST_NUM_TEXT].pUtf8 = _tmpbuf;
    DrawText( _Contexts[DEVLIST_NUM_TEXT], 0, 0);

    memset(_tmpbuf,0x0,sizeof(_tmpbuf));
    sprintf( (char *)_tmpbuf,"%d/%d", appData.hostlist.index+1, ((appData.hostlist.total-1)/5)+1 );
    _Contexts[DEVLIST_PAGE_TEXT].pUtf8 = _tmpbuf;
    DrawText( _Contexts[DEVLIST_PAGE_TEXT], 0, 0);

}
static void DrawAlert(void)
{
    unsigned int event_msg = appData.eventtype;
    unsigned int index =  0;//RECOVERY_ICON;
    MCU_RECT fill_rect = {0,0,320,480};
    SCREEN_CONTEXT context;

    if (SCREEN_INIT == appData.screen_state)
    {
        MCU_SetColor(0x00000000);
        MCU_FillRect(fill_rect);
        switch (event_msg)
        {
            case REBOOT:
                appData.alert_tr.turn_flag = 1;
                context = _Contexts[ALERT_REBOOT];
                break;
            case FACTORY_RESET:
                appData.alert_tr.turn_flag = 1;   // 0
                context = _Contexts[ALERT_FACTORY_RESET];
                index = 0;//RECOVERY_ICON;
                break;
            case WIFI_2G_ON:

            // break;
            case WIFI_2G_OFF:

            // break;
            case WIFI_5G_ON:

            // break;
            case WIFI_5G_OFF:

            //break;
            case WIFI_ON:

            //break;
            case WIFI_OFF:

            //break;
            case ROUTER_UPGRADE:
                appData.alert_tr.turn_flag = 1;   //0
                context = _Contexts[ALERT_ROUTER_UPGRADE];
                index= 0;//UPGRADE_ICON;
                break;
            default:
                return;
        }
        if (0 == appData.alert_tr.turn_flag )
        {
            DrawIcon( _Icons[index], 0, 0);
        }
        DrawText( context, 0, 0);
        if(event_msg!=ROUTER_UPGRADE)
        {
            DrawText( _Contexts[ALERT_WAIT1_MSG], 0, 0);
            
        }
        else
        {
            DrawText( _Contexts[ALERT_WAIT2_MSG], 0, 0);
        }
    }
    if (1 == appData.alert_tr.turn_flag)
    {
      CountNum( &appData.count, 5);
      if (appData.count != 0)
        return ;
      ++(appData.alert_tr.i_start);
      DrawCir(&appData.alert_tr);
    }
}

void DrawWeather(void)
{
    unsigned int code = 0;
    unsigned char str_unknown[]="--";
    int x_offset=0;
    int temp_len=0;
    MCU_RECT fill_rect = {10,110,310,370};
    unsigned char temp[8]={0};
    code = appData.weatherinfo.weather_code;
    if (code > 36 )
        code = 37;

    if ( SCREEN_INIT == appData.screen_state)
    {
        DrawText( _Contexts[WEATHER_TITLE],0,0);
        appData.alert_tr.turn_flag=1;
        DrawText(_Contexts[WEATHER_WEEK_TEXT],0,0);
        DrawText(_Contexts[WEATHER_DATE_TEXT],0,0);
        DrawText(_Contexts[WEATHER_TIME_TEXT],0,0);
    }
    else
    {
        if ( false == appData.changFlag )
            return ;
    }
    MCU_SetColor(0x00000000);
    MCU_FillRect(fill_rect);
    if ( strlen( (char *)appData.weatherinfo.city_ch) < 1)
    {
         DrawIcon( _Icons[WEATHER_NO_CITY_ICON], 0, 0);
         DrawText(_Contexts[WEATHER_NO_CITY_TIP_MSG1],0,0);
         DrawText(_Contexts[WEATHER_NO_CITY_TIP_MSG2],0,0);
    }
    else
    {
        _Contexts[WEATHER_CITY_TEXT].pUtf8 = appData.weatherinfo.city_ch; 
        DrawText(_Contexts[WEATHER_CITY_TEXT],0,0);

        if ( appData.weatherinfo.error != 0)
        {
           _Icons[WEATHER_STATE_ICON].bmp_index = _Weather[37].bmp_index;
           _Contexts[WEATHER_STATE_TEXT].pUtf8 = str_unknown;
           _Contexts[WEATHER_TEMP_TEXT].pUtf8 = str_unknown;
        }
        else 
        {
            _Icons[WEATHER_STATE_ICON].bmp_index = _Weather[code].bmp_index;
            _Contexts[WEATHER_STATE_TEXT].pUtf8 = (u8 *)_Weather[code].text;
           // _Contexts[WEATHER_TEMP_TEXT].pUtf8 = appData.weatherinfo.temperature;
  
            memset(temp,0x0,sizeof(temp_len));
            strcpy((char *)temp,(char *)appData.weatherinfo.temperature);
            strcat((char *)temp,(char *)(_Contexts[WEATHER_TEMP_UNIT].pUtf8));
            _Contexts[WEATHER_TEMP_TEXT].pUtf8 = temp; 
            /*switch(temp_len)
            {
            case 1:
                x_offset = -15;
                break;
            case 2:
                x_offset = 0;
                break;
            case 3:
                x_offset = 15;
                break;
            }*/
        }
        DrawIcon( _Icons[WEATHER_STATE_ICON], 0, 0);
        DrawText(_Contexts[WEATHER_STATE_TEXT],0,0);
        DrawText(_Contexts[WEATHER_TEMP_TEXT],0,0);
        //DrawText(_Contexts[WEATHER_TEMP_UNIT],x_offset,0);
    }
     _Contexts[WEATHER_WEEK_TEXT].pUtf8 = _Week[appData.weatherinfo.week];
     DrawText(_Contexts[WEATHER_WEEK_TEXT],0,0);
     _Contexts[WEATHER_DATE_TEXT].pUtf8  = appData.weatherinfo.date;
     DrawText(_Contexts[WEATHER_DATE_TEXT],0,0);
     _Contexts[WEATHER_TIME_TEXT].pUtf8  = appData.weatherinfo.time;
     DrawText(_Contexts[WEATHER_TIME_TEXT],0,0);

}

void DrawSoftUpdate(void)
{
    if ( SCREEN_INIT == appData.screen_state )
    {
        DrawText(_Contexts[SOFTUPDATE_TITLE],0,0);
        DrawIcon( _Icons[SOFTUPDATE_ICON], 0, 0);

        DrawText(_Contexts[SOFTUPDATE_FIND_VER],0,0);
        DrawText(_Contexts[SOFTUPDATE_CUR_VER],0,0);
        DrawText(_Contexts[SOFTUPDATE_NEW_VER],0,0);
        DrawText(_Contexts[SOFTUPDATE_TIP_MSG1],0,0);
        DrawText(_Contexts[SOFTUPDATE_TIP_MSG2],0,0);

    }
    else
    {
        if( false == appData.changFlag )
            return ;
    }
    _Contexts[SOFTUPDATE_CUR_VER_TEXT].pUtf8 = appData.systeminfo.sw;
    DrawText(_Contexts[SOFTUPDATE_CUR_VER_TEXT],0,0);
    _Contexts[SOFTUPDATE_NEW_VER_TEXT].pUtf8 = appData.systeminfo.newsw;
    DrawText(_Contexts[SOFTUPDATE_NEW_VER_TEXT],0,0);

}

void ChangeScreen(void)
{
    int cur_i = 0;
    int num = 0;
    switch (appData.screen_state){
        case SCREEN_INIT:
            MCU_Clear();
            appData.count = -1;
        case SCREEN_SHOW:
            switch(appData.cur_screen)
            {
                case welcome:
                    DrawWelcome();
                    break;
                case softupdate:
                    DrawSoftUpdate();
                    break;
                case sysinfo:
                    DrawSysinfo();
                    break;
                case sysinterface:
                    DrawSysInterface();
                    break;
                case home:
                    if (appData.homeinfo.mode == 0)
                    {
                        DrawHome();
                    }
                    else
                    {
                        DrawApMode();
                    }
                    break;
                case wifi:
                    DrawWifi();
                    break;
                case devlist:
                    DrawDevlist();
                    break;
                case weather:
                    DrawWeather();
                    break;
                case alert:
                    DrawAlert();
                break;
            }
            if (appData.cur_screen>welcome && appData.cur_screen < alert)
            {
                if( appData.homeinfo.mode == 0)
                {
                    if (strlen((char *)appData.systeminfo.newsw)>1)
                    {
                        cur_i = appData.cur_screen+appData.hostlist.index;
                        num = devlist+(appData.hostlist.total-1)/5;
                       
                    }
                    else
                    {
                        cur_i = appData.cur_screen-1+appData.hostlist.index;
                        num = devlist+(appData.hostlist.total-1)/5 -1;
                    }
                }
                else
                {
                    if (strlen((char *)appData.systeminfo.newsw)>1)
                    {
                        cur_i = appData.cur_screen;
                        num = 6;
                    }
                    else
                    {
                        cur_i = appData.cur_screen-1;
                        num = 5;
                    }
                }
                DrawBootBar(cur_i,num); 
            }
            appData.screen_state = SCREEN_SHOW;
            break;
        case SCREEN_END:
            break;
    }
    appData.changFlag = false;
    appData.homeInfoChangFlag = false;
}

static void DrawInterface(int *x,int *y,int centerx,int centery,int enable,unsigned char *text)
{
    int i = 0;
    int j = 0;
    MCU_COLOR color = 0x00FFFFFF;
    MCU_LINE line = {0,0,0,0};
    MCU_RECT rect = {0,0,0,0};
    if (enable)
        color = 0x00FFFFFF;
    else
        color = 0x00808080;

    for ( i=0; i<6; ++i)
    {
        x[i] = 2*centerx - x[i];
        x[i] = x[i]>0?x[i]:-x[i];

        y[i] +=centery;
    }
    x[5] -= (centerx/160)*( _Contexts[SYSINTERFACE_VALUE_TEXT].rect.x1 - _Contexts[SYSINTERFACE_VALUE_TEXT].rect.x0); //  centerx不为0时，即关于x轴160翻转，x[5]变为Text rect的右坐标，需修改为左坐标即需减去(Text rect width)

    if ( x[3] > x[4] )  // 交换x[3]与x[4]， DrawLine  line 起始x必须小于终止x
    {
        x[3] = x[3] + x[4];
        x[4] = x[3] - x[4];
        x[3] = x[3] - x[4];
    }

    for ( i=0; i<2; ++i)
    {
        MCU_SetColor(color);
        MCU_SetPenSize(1);
        line.x0 = x[0];
        line.y0 = y[1]+(y[2]-y[1])/2;
        line.x1 = x[0];
        line.y1 = y[1+i];
        MCU_DrawLine(line);

        line.y0 = y[1+i];
        line.x1 = x[1];
        MCU_DrawLine(line);

        line.x0 = x[1];
        line.y1 = y[i==0?0:3];
        MCU_DrawLine(line);

        line.y0 = y[i==0?0:3];
        line.x1 = x[2]; 
        MCU_DrawLine(line);

        line.x0 = x[2];
        line.y1 = y[0]+(y[3]-y[0])/2;
        MCU_DrawLine(line);
        if(!enable)
        {
            MCU_SetColor(0x00000000);
        }
        rect.x0 = x[3];
        rect.y0 = y[4]+i*31;
        rect.x1 = x[4];
        rect.y1 = y[5]+i*31;
        MCU_FillRect(rect);
        for ( j=i*3; j<(i+1)*4; ++j)
        {
            MCU_SetColor(0x00000000);
            MCU_SetPenSize(2);
            line.x0 = x[3];
            line.y0 = y[4]+6+j*8;
            line.x1 = x[4];
            line.y1 = y[4]+6+j*8;
            MCU_DrawLine(line);
        }

    }
    _Contexts[SYSINTERFACE_VALUE_TEXT].pUtf8 = text;
    _Contexts[SYSINTERFACE_VALUE_TEXT].color = color;

    DrawText( _Contexts[SYSINTERFACE_VALUE_TEXT], x[5]-_Contexts[SYSINTERFACE_VALUE_TEXT].rect.x0, y[0]-156); // y[0]-156为y轴增量
}

static void DrawApMode(void)
{
    if ( SCREEN_INIT == appData.screen_state )
    {
        DrawText(_Contexts[APMODE_TITLE],0,0);
        DrawIcon( _Icons[APMODE_ICON], 0, 0);
        DrawText( _Contexts[APMODE_CURIP], 0, 0);
    }
    else
    {
        if( false == appData.homeInfoChangFlag )
            return ;
    }
    _Contexts[APMODE_CURIP_TEXT].pUtf8 = appData.homeinfo.curip;
    DrawText( _Contexts[APMODE_CURIP_TEXT], 0, 0);
}
